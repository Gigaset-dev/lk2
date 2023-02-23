/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include "adsp_plat.h"
#include <arch/ops.h>
#include <err.h>
#include <kernel/vm.h>
#include <libfdt.h>
#include <load_image.h>
#include <mblock.h>
#include <platform/adsp_reg.h>
#include <string.h>
#include <trace.h>


#define LOCAL_TRACE (0)

typedef struct {
    unsigned int id;
    int status;
    struct adsp_imgs_t img[3];   /* iram, dram, sram */
} ADSP_CORE_T;

struct ADSP_CONFIGS {
    int status;
    unsigned int core_num;
    ADSP_CORE_T core[2];
} _adsp_configs;

/******************************************************************************
 * Get adsp configurations from device tree.
 ******************************************************************************/
static void _load_adsp_configs(void *fdt)
{
    int nodeoffset, len;
    int n = 0;
    unsigned int *idata = NULL;
    unsigned int coreid;
    char *cdata = NULL, *img_name;
    char compat_name[32];

    /* get adsp_common status */
    if (fdt == NULL)
        return;

    nodeoffset = fdt_node_offset_by_compatible(fdt, -1, ADSP_DT_NODE_COMMON);
    if (nodeoffset < 0) {
        LTRACEF("%s failed to find %s in dtb\n", LOG_TAG, ADSP_DT_NODE_COMMON);
        assert(0);
    }

    cdata = (char *)fdt_getprop(fdt, nodeoffset, "status", &len);
    if (cdata == NULL)
        _adsp_configs.status = 0;
    else {
        if (strncmp(cdata, "okay", len) == 0)
            _adsp_configs.status = 1; /* 1 goes for "online" */
        else
            _adsp_configs.status = 0; /* 0 goes for "offline" */
    }

    /* get core number */
    idata = (unsigned int *)fdt_getprop(fdt, nodeoffset, "core_num", &len);
    if (idata == NULL) {
        _adsp_configs.core_num = 0;
        _adsp_configs.status = 0;
    } else
        _adsp_configs.core_num = fdt32_to_cpu(*(unsigned int *)idata);

    for (coreid = 0; coreid < _adsp_configs.core_num; coreid ++) {
        _adsp_configs.core[coreid].id = coreid;
        memset(compat_name, 0, sizeof(compat_name));
        n = snprintf(compat_name, sizeof(compat_name), "%s_%d",
            ADSP_DT_NODE_CORE, coreid);
        if (n < 0 || n > (int)sizeof(compat_name))
            assert(0);
        nodeoffset = fdt_node_offset_by_compatible(fdt, -1, compat_name);
        if (nodeoffset < 0) {
            LTRACEF("%s failed to find %s in dtb\n", LOG_TAG, compat_name);
            assert(0);
        }

        /* get adsp coreX status */
        cdata = (char *)fdt_getprop(fdt, nodeoffset, "status", &len);
        if (cdata == NULL)
            _adsp_configs.core[coreid].status = 0;
        else {
            if (strncmp(cdata, "okay", len) == 0)
                _adsp_configs.core[coreid].status = 1; /* true goes for "online" */
            else
                _adsp_configs.core[coreid].status = 0; /* false goes for "offline" */
        }

        /* get adsp coreX memory configurations */
        idata = (unsigned int *)fdt_getprop(fdt, nodeoffset, "reg", &len);
        if (len > 0 && idata) {
            len = len / sizeof(unsigned int);
            img_name = _adsp_configs.core[coreid].img[0].name;
            n = snprintf(img_name, sizeof(char)*20, "hifi3_%s_%s",
                (coreid==0)? "a": "b", "iram");
            img_name = _adsp_configs.core[coreid].img[1].name;
            n = snprintf(img_name, sizeof(char)*20, "hifi3_%s_%s",
                (coreid==0)? "a": "b", "dram");
            _adsp_configs.core[coreid].img[0].base = (void *)fdt32_to_cpu(*(idata + 1));
            _adsp_configs.core[coreid].img[0].size = fdt32_to_cpu(*(idata + 3));
            _adsp_configs.core[coreid].img[1].base = (void *)fdt32_to_cpu(*(idata + 4*1 + 1));
            _adsp_configs.core[coreid].img[1].size = fdt32_to_cpu(*(idata + 4*1 + 3));
        }
        idata = (unsigned int *)fdt_getprop(fdt, nodeoffset, "system", &len);
        if (idata == NULL)
            assert(0);
        img_name = _adsp_configs.core[coreid].img[2].name;
        n = snprintf(img_name, sizeof(char)*20, "hifi3_%s_%s",
            (coreid==0)? "a": "b", "sram");
        _adsp_configs.core[coreid].img[2].base = (void *)fdt32_to_cpu(*(idata + 1));
        _adsp_configs.core[coreid].img[2].size = fdt32_to_cpu(*(idata + 3));

    }
}
/******************************************************************************
 * load adsp image from storage to dram and adsp's sram.
 ******************************************************************************/
status_t platform_load_adsp(void *fdt)
{
    int ret = 0;
    void *dram_addr_phy = NULL;
    void *dram_addr_vir = NULL;
    unsigned int dram_size = 0;
    const char *part_name = "audio_dsp";

    /* load adsp settings from fdt */
    _load_adsp_configs(fdt);

    /* check if adsp is turned off by kerenl */
    if (adsp_get_status() == 0) {
        LTRACEF("%s get status disabled\n", LOG_TAG);
        ret = ERR_OFFLINE;
        goto err_load_configs;
    }

    ret = (int)adsp_dram_alloc(&dram_addr_phy, &dram_size);
    if (!ret) {
        ret = ERR_NO_MEMORY;
        goto err_mblock_alloc;
    }
    /* dram remap value is not used after vmm mapping */
    adsp_update_sram_imgs_base(dram_addr_phy);
    print_adsp_img_info();

    ret = vmm_alloc_physical(vmm_get_kernel_aspace(),
                        ADSP_IMG_RSV_NAME,
                        dram_size,
                        &dram_addr_vir,
                        PAGE_SIZE_SHIFT,
                        (paddr_t)dram_addr_phy,
                        0,
                        ARCH_MMU_FLAG_CACHED);
    if (ret != NO_ERROR) {
        ret = ERR_NO_MEMORY;
        goto err_vmm_alloc;
    }
    LTRACEF("%s dram_addr_phy=%p, dram_addr_virt=%p, dram_size=0x%x\n",
           LOG_TAG, dram_addr_phy, dram_addr_vir, dram_size);

    ret = verify_load_adsp_img(part_name, dram_addr_phy, dram_addr_vir);
    if (ret < 0)
        goto err_load_partition;

    /* clean dcache & icache before setup EMI MPU */
    arch_clean_invalidate_cache_range((addr_t)dram_addr_vir, (size_t)dram_size);
    /* set emi mpu to protect adsp dram */
    adsp_emi_protect_enable(MPU_REGION_ID_ADSP_RO_MEM, dram_addr_phy, dram_size);
    return NO_ERROR;

err_load_partition:
    vmm_free_region(vmm_get_kernel_aspace(), (vaddr_t)dram_addr_vir);
err_vmm_alloc:
    mblock_free(dram_addr_phy);
err_mblock_alloc:
err_load_configs:
    adsp_set_status(0);
    return ret;
}

/******************************************************************************
 * APIs for ADSP configurations
 ******************************************************************************/
int adsp_get_status(void)
{
    return _adsp_configs.status;
}

void adsp_set_status(int status)
{
    unsigned int cid;

    _adsp_configs.status = status;
    if (status == 0 && _adsp_configs.core_num <= 2) {
        for (cid = 0; cid < _adsp_configs.core_num; cid ++) {
            _adsp_configs.core[cid].status = 0;
        }
    }
}

int adsp_core_exist(unsigned int cid)
{
    return _adsp_configs.core[cid].status;
}

unsigned int adsp_get_core_num(void)
{
    return _adsp_configs.core_num;
}

struct adsp_imgs_t *adsp_get_img_info(unsigned int cid, unsigned int idx)
{
    return &_adsp_configs.core[cid].img[idx];
}

int adsp_set_img_info(unsigned int cid, unsigned int idx, struct adsp_imgs_t *img)
{
    if (strncmp(_adsp_configs.core[cid].img[idx].name, img->name,
         sizeof(char)*20) != 0) {
        return -1;
    }
    _adsp_configs.core[cid].img[idx].base = img->base;
    _adsp_configs.core[cid].img[idx].size = img->size;
    return 0;
}

/******************************************************************************
 * APIs for debug
 ******************************************************************************/
void print_adsp_img_info(void)
{
    unsigned int cid, img_id;
    struct adsp_imgs_t *img;

    for (cid = 0; cid < adsp_get_core_num(); cid ++) {
        for (img_id = 0; img_id < 3; img_id ++) {
            img = adsp_get_img_info(cid, img_id);
            LTRACEF("%s cid=%d, %s (base, size) = (%p, 0x%x)\n",
                __func__, cid, img->name, img->base, img->size);
        }
    }
}

