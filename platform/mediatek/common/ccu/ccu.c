/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <arch/ops.h>      /* for arch_xxxx_cache_range() */
#include <assert.h>
#include <err.h>
#include <errno.h>
#include <kernel/vm.h>
#include <libfdt.h>
#include <load_image.h>
#include <mblock.h>
#include <platform_mtk.h>
#include <reg.h>
#include <smc.h>
#include <smc_id_table.h>
#include <stdint.h>
#include <trace.h>

#include "ccu.h"

#define LOCAL_TRACE (0)

#ifdef WITH_PLATFORM_MEDIATEK_COMMON_EMI
#include <emi_mpu.h>
#endif

struct CCU_CONFIGS {
    uint64_t dram_pa;
    unsigned int dram_size;
    unsigned int sram_size;
    unsigned int sram_offset;
    unsigned int emi_region;
    int status;     /* 1 for online, 0 for offline */
    int emi_status; /* 1 for ready, 0 for otherwise */
} _ccu_configs = {0};

/******************************************************************************
 * APIs for CCU configurations
 ******************************************************************************/
static int ccu_get_exist(void)
{
    return _ccu_configs.status;
}

static int ccu_emi_status(void)
{
    return _ccu_configs.emi_status;
}

static unsigned int ccu_get_sram_size(void)
{
    return _ccu_configs.sram_size;
}

static uint64_t ccu_get_sram_offset(void)
{
    return _ccu_configs.sram_offset;
}

static unsigned int ccu_get_dram_size(void)
{
    return _ccu_configs.dram_size;
}

static uint64_t ccu_get_dram_paddr(void)
{
    return _ccu_configs.dram_pa;
}

static uint32_t ccu_get_emi_region(void)
{
    return _ccu_configs.emi_region;
}

/******************************************************************************
 * Set EMI MPU to protect DRAM buffer.
 ******************************************************************************/
static void ccu_emi_protect_enable(uint64_t pa, unsigned int size)
{
    /*
     * setup EMI MPU
     * domain 0: AP
     * domain 12: CCU
     * CCU SR# : 20
     */
#ifdef WITH_PLATFORM_MEDIATEK_COMMON_EMI
    struct emi_region_info_t region_info;

    region_info.start = pa;
    region_info.end = pa + size;
    region_info.region = ccu_get_emi_region();
    emi_mpu_set_protection(&region_info);
#endif
}

/******************************************************************************
 * Get CCU configurations from device tree.
 ******************************************************************************/
static void _load_ccu_configs(void *fdt)
{
    int nodeoffset;
    unsigned int *idata = NULL;
    char *cdata = NULL;
    int len = 0;

    if (fdt == NULL)
        return;

    nodeoffset = fdt_node_offset_by_compatible(fdt, -1, "mediatek,ccu_rproc");
    if (nodeoffset < 0) {
        // assert(0);
        dprintf(CRITICAL, "[CCU] CCU DT node not found, disable CCU.\n");
        _ccu_configs.status = 0;

    }

    cdata = (char *)fdt_getprop(fdt, nodeoffset, "secured", &len);
    if (cdata == NULL)
        _ccu_configs.status = 0;
    else {
        if (strncmp(cdata, "yes", len) == 0)
            _ccu_configs.status = 1; /* 1 goes for "secured flow" */
        else
            _ccu_configs.status = 0; /* 0 goes for "non-secured flow" */
    }

    cdata = (char *)fdt_getprop(fdt, nodeoffset, "ccu_emiStatus", &len);
    if (cdata == NULL)
        _ccu_configs.emi_status = 1;
    else {
        if (strncmp(cdata, "no", len) == 0)
            _ccu_configs.emi_status = 0; /* 0, not set emi protection */
        else
            _ccu_configs.emi_status = 1; /* 1, set emi protection */
    }

    idata = (unsigned int *)fdt_getprop(fdt, nodeoffset, "ccu_sramSize", &len);
    if (idata == NULL)
        _ccu_configs.sram_size = CCU_SRAM_SIZE;
    else
        _ccu_configs.sram_size = fdt32_to_cpu(*(unsigned int *)idata);

    if (_ccu_configs.sram_size == 0)
        _ccu_configs.sram_size = CCU_SRAM_SIZE;

    idata = (unsigned int *)fdt_getprop(fdt, nodeoffset, "ccu_sramOffset", &len);
    if (idata == NULL)
        _ccu_configs.sram_offset = CCU_SRAM_SIZE;
    else
        _ccu_configs.sram_offset = fdt32_to_cpu(*(unsigned int *)idata);

    if (_ccu_configs.sram_offset == 0)
        _ccu_configs.sram_offset = CCU_SRAM_SIZE;
    if (_ccu_configs.sram_offset < _ccu_configs.sram_size)
        _ccu_configs.sram_offset = _ccu_configs.sram_size;

    idata = (unsigned int *)fdt_getprop(fdt, nodeoffset, "ccu_dramSize", &len);
    if (idata == NULL)
        _ccu_configs.dram_size = CCU_DRAM_SIZE;
    else
        _ccu_configs.dram_size = fdt32_to_cpu(*(unsigned int *)idata);

    if (_ccu_configs.dram_size == 0)
        _ccu_configs.dram_size = CCU_DRAM_SIZE;

    idata = (unsigned int *)fdt_getprop(fdt, nodeoffset, "ccu_dramAddr", &len);
    if (idata == NULL)
        _ccu_configs.dram_pa = DRAM_BASE;
    else
        _ccu_configs.dram_pa = (uint64_t)fdt32_to_cpu(*(unsigned int *)idata);

    if (_ccu_configs.dram_pa == 0)
        _ccu_configs.dram_pa = DRAM_BASE;

    idata = (unsigned int *)fdt_getprop(fdt, nodeoffset, "ccu_emiRegion", &len);
    if (idata == NULL)
        _ccu_configs.emi_region = CCU_EMI_REGION;
    else
        _ccu_configs.emi_region = (uint64_t)fdt32_to_cpu(*(unsigned int *)idata);

    if (_ccu_configs.emi_region == 0)
        _ccu_configs.emi_region = CCU_EMI_REGION;
}

/******************************************************************************
 * load ccu image from storage to dram.
 ******************************************************************************/
int platform_load_ccu(void *fdt)
{
    uint32_t i;
    int ret;
    struct ccu_img_hdr *img_hdr;
    const char *part_name = "ccu";
    void *dram_va = NULL;
    unsigned int base_addr = 0;
    const char *prompt = "CCU";
    struct __smccc_res res;
    uint64_t dram_pa;
    unsigned int dram_size, sram_size, sram_offset;

    /* load ccu settings from fdt */
    _load_ccu_configs(fdt);

    /* check if ccu is turned off manually*/
    if (ccu_get_exist() == 0) {
        dprintf(CRITICAL, "[%s]: get_ccu_status disabled\n", prompt);
        return NO_ERROR;
    }

    dram_pa = ccu_get_dram_paddr();
    dram_size = ccu_get_dram_size();
    sram_size = ccu_get_sram_size();
    sram_offset = ccu_get_sram_offset();

    if (need_relocate())
        dram_pa = mblock_alloc_range((u64)dram_size, CCU_DRAM_ALIGN,
            DRAM_BOUND_LOWER, DRAM_BOUND_UPPER, 0, 0, DRAM_NAME_CCU);
    else
        dram_pa = mblock_alloc((u64)dram_size, CCU_DRAM_ALIGN,
            (u64)(dram_pa+dram_size), 0, 0, DRAM_NAME_CCU);

    if (!dram_pa) {
        dprintf(CRITICAL, "[%s] mblock allocation failed\n", prompt);
        ret = ERR_NO_MEMORY;
        goto ccu_err_mblock_alloc;
    }
    LTRACEF("[%s] pa=0x%llx size=0x%x\n", prompt, dram_pa, dram_size);

    ret = vmm_alloc_physical(vmm_get_kernel_aspace(),
                        DRAM_NAME_CCU,
                        dram_size,
                        &dram_va,
                        PAGE_SIZE_SHIFT,
                        (paddr_t)dram_pa,
                        0,
                        ARCH_MMU_FLAG_CACHED);
    if (ret != NO_ERROR) {
        dprintf(CRITICAL, "[%s] vmm_alloc_physical failed\n", prompt);
        ret = ERR_NO_MEMORY;
        goto ccu_err_vmm_alloc;
    }
    LTRACEF("[%s] va=0x%llx size=0x%x\n", prompt, (uint64_t)dram_va, dram_size);

    ret = load_partition(part_name, IMG_NAME_CCU, dram_va, dram_size);
    if (ret < 0) {
        dprintf(CRITICAL, "[%s] load ccu fail, ret = %d\n", prompt, ret);
        ret = ERR_IO;
        goto ccu_err_load_partition;
    }

    /* Sync cache to make sure all data flush to DRAM */
    /* arch_sync_cache_range((addr_t)dram_va, dram_size); */ /* NOT work on LK2/mt6983. */
    arch_clean_cache_range((addr_t)dram_va, dram_size);

    LTRACEF("[%s] load ccu OK, part_name=%s size=0x%x\n", prompt, part_name, ret);

    img_hdr = (struct ccu_img_hdr *)dram_va;
    if (img_hdr->magicno != CCU_MAGIC_NO) {
        dprintf(CRITICAL, "[%s] CCU binary does NOT have CCU mark.\n", prompt);
        ret = CCU_ERR_MAGIC_NO;
        goto ccu_err_load_partition;
    }

    for (i = 0; i < img_hdr->sect_no; ++i) {
        LTRACEF("[%s] sect[%d] offset 0x%x daddr 0x%x size 0x%x\n", prompt, i,
                img_hdr->sectors[i].offset, img_hdr->sectors[i].daddr, img_hdr->sectors[i].size);

        if (img_hdr->sectors[i].daddr < CCU_DRAM_BASE) {
            if (base_addr == 0)
                base_addr = img_hdr->sectors[i].daddr & BASE_MASK;
            if (sram_offset <= (img_hdr->sectors[i].daddr-base_addr)) {
                if (sram_size < (img_hdr->sectors[i].daddr-base_addr-sram_offset+
                                     img_hdr->sectors[i].size)) {
                    dprintf(CRITICAL, "[%s] CCU binary dtcm addr/size errors.\n", prompt);
                    ret = CCU_ERR_TCM_SIZE;
                    goto ccu_err_load_partition;
                }
            } else  {
                if (sram_size < (img_hdr->sectors[i].daddr-base_addr+
                                img_hdr->sectors[i].size)) {
                    dprintf(CRITICAL, "[%s] CCU binary itcm addr/size errors.\n", prompt);
                    ret = CCU_ERR_TCM_SIZE;
                    goto ccu_err_load_partition;
                }
            }
        }
    }

    __smc_conduit(MTK_SIP_BL_CCU_CONTROL, CCU_SMC_REQ_INIT, (ulong)dram_pa, dram_size,
                  0, 0, 0, 0, &res);
    if (res.a0) {
        dprintf(CRITICAL, "[%s] CCU REQ_INIT errors (%d).\n", prompt, (int)res.a0);
        ret = CCU_ERR_REQ_INIT;
        goto ccu_err_load_partition;
    } else {
        LTRACEF("[%s] CCU(INIT, 0x%llx, 0x%x, 0x%x), OK\n", prompt, dram_pa, dram_size, base_addr);
    }

    /* use emi mpu to protect the content of dram */
    if (ccu_emi_status())
        ccu_emi_protect_enable(dram_pa, dram_size);

    dprintf(INFO, "[%s] done\n", prompt);

    return NO_ERROR;

ccu_err_load_partition:
    vmm_free_region(vmm_get_kernel_aspace(), (vaddr_t)dram_va);
ccu_err_vmm_alloc:
    mblock_free(dram_pa);
ccu_err_mblock_alloc:
    return ret;
}
