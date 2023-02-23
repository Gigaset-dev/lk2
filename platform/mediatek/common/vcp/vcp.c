/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#if ARCH_ARM
#include <arch/arm.h>
#endif
#if ARCH_ARM64
#include <arch/arm64.h>
#endif
#include <arch/ops.h>                   /* for arch_clean_invalidate_cache_range() */
#include <assert.h>
#include <err.h>
#include <errno.h>
#include <kernel/vm.h>
#include <libfdt.h>
#include <load_image.h>
#include <platform_mtk.h>
#include <reg.h>
#include <stdint.h>
#include <trace.h>
#include <vcp.h>

#define LOCAL_TRACE (0)

struct VCP_CONFIGS {
    int vcp_support;
    int sram_size;
    int status;     /* 1 for online, 2 for offline */
    int vcpctl;
    int reserved_dram_size;
    int core_nums;
} _vcp_configs;

/******************************************************************************
 * Get vcp configurations from device tree.
 ******************************************************************************/
static void _load_vcp_configs(void *fdt)
{
    int nodeoffset;
    unsigned int *idata = NULL;
    char *cdata = NULL;
    int len = 0;

    if (fdt == NULL)
        return;

    nodeoffset = fdt_node_offset_by_compatible(fdt, -1, "mediatek,vcp");
    if (nodeoffset < 0)
        assert(0);

    idata = (unsigned int *)fdt_getprop(fdt, nodeoffset, "vcp-support", &len);
    if (idata == NULL)
        _vcp_configs.vcp_support = 0;
    else
        _vcp_configs.vcp_support = fdt32_to_cpu(*(unsigned int *)idata);

    idata = (unsigned int *)fdt_getprop(fdt, nodeoffset, "vcp_sramSize", &len);
    if (idata == NULL)
        _vcp_configs.sram_size = 0;
    else
        _vcp_configs.sram_size = fdt32_to_cpu(*(unsigned int *)idata);

    idata = (unsigned int *)fdt_getprop(fdt, nodeoffset, "vcp_dramSize", &len);
    if (idata == NULL)
        _vcp_configs.reserved_dram_size = 0;
    else
        _vcp_configs.reserved_dram_size = fdt32_to_cpu(*(unsigned int *)idata);

    idata = (unsigned int *)fdt_getprop(fdt, nodeoffset, "core_nums", &len);
    if (idata == NULL)
        _vcp_configs.core_nums = 0;
    else
        _vcp_configs.core_nums = fdt32_to_cpu(*(unsigned int *)idata);

    cdata = (char *)fdt_getprop(fdt, nodeoffset, "status", &len);
    if (cdata == NULL)
        _vcp_configs.status = 0;
    else {
        if (strncmp(cdata, "okay", len) == 0)
            _vcp_configs.status = 1; /* 1 goes for "online" */
        else
            _vcp_configs.status = 0; /* 0 goes for "offline" */
    }

    cdata = (char *)fdt_getprop(fdt, nodeoffset, "vcpctl", &len);
    if (cdata == NULL)
        _vcp_configs.vcpctl = 0;
    else
        _vcp_configs.vcpctl = atoul(cdata);
}

/******************************************************************************
 * load vcp image from storage to dram and vcp's sram.
 ******************************************************************************/
int platform_load_vcp(void *fdt)
{
    int ret;
    int sram_size = 0;
    const char *part_name = "vcp";
    void *dram_addr_phy = 0;
    void *dram_addr = NULL;
    unsigned int dram_size = 0;
    const char *prompt = "VCP";

    /* load vcp settings from fdt */
    _load_vcp_configs(fdt);

    /* check if vcp is turned off manually*/
    if (vcp_get_exist() == 0) {
        LTRACEF("[VCP] get_vcp_status disabled\n");
        return ERR_OFFLINE;
    }

    if (!_vcp_configs.vcp_support) {
        LTRACEF("[VCP] vcp disabled\n");
        return NO_ERROR;
    }

    sram_size = vcp_get_sram_size();
    dram_size = vcp_get_resv_dram_size();
    if (sram_size <= 0 || dram_size <= 0)
        panic("[VCP] Invalid size from dts sram_size=%x, resv_dram_size=%x.\n",
            sram_size, dram_size);

    ret = (int)vcp_dram_alloc(&dram_addr_phy, dram_size);
    if (ret == 0)
        assert(0);

    ret = vmm_alloc_physical(vmm_get_kernel_aspace(),
                        "me_vcp_reserved",
                        dram_size,
                        &dram_addr,
                        PAGE_SIZE_SHIFT,
                        (paddr_t)dram_addr_phy,
                        0,
                        ARCH_MMU_FLAG_CACHED);
    if (ret != NO_ERROR)
        assert(0);

    LTRACEF("%s: dram_addr_phy=%p, dram_addr=%p\n",
        prompt, dram_addr_phy, dram_addr);
    LTRACEF("%s: part_name=%s, sram_size=%x, resv_dram_size=%x\n",
        prompt, part_name, sram_size, dram_size);

    vcp_power_on();

    ret = verify_load_vcp_image(part_name, dram_addr_phy, dram_addr);
    if (ret <= 0) {
        /* load image fail, power off vcp */
        vmm_free_region(vmm_get_kernel_aspace(), (vaddr_t)dram_addr);
        vcp_dram_free(dram_addr_phy);
        vcp_power_off();
    } else {
        /*clean dcache & icache before set up EMI MPU*/
        arch_clean_invalidate_cache_range((addr_t)dram_addr, dram_size);
        /* use emi mpu to protect the content of dram */
        vcp_emi_protect_enable((unsigned long long)dram_addr_phy, dram_size);
    }

    return NO_ERROR;
}

/******************************************************************************
 * APIs for VCP configurations
 ******************************************************************************/

/*
 * Load VCP sram size from dts
 */
int vcp_get_sram_size(void)
{
    return _vcp_configs.sram_size;
}
/*
 * Load VCP reserved dram size from dts
 * call mbloc alloc with this size
 */
int vcp_get_resv_dram_size(void)
{
    return _vcp_configs.reserved_dram_size;
}

int vcp_get_core_num(void)
{
    return _vcp_configs.core_nums;
}


/*
 * Get the final result of vcp setting from device tree or environment
 */
int vcp_get_exist(void)
{
    return _vcp_configs.status;
}

/*
 * Get the vcpctl setting from device tree
 */
int vcp_get_vcpctl(void)
{
    return _vcp_configs.vcpctl;
}
