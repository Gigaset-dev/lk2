/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <arch/arm.h>
#include <arch/ops.h>                   /* for arch_sync_cache_range() */
#include <assert.h>
#include <err.h>
#include <errno.h>
#include <kernel/vm.h>
#include <libfdt.h>
#include <load_image.h>
#include <medmcu.h>
#include <medmcu_plat.h>
#include <platform_mtk.h>
#include <reg.h>
#include <stdint.h>
#include <trace.h>

#define LOCAL_TRACE (0)

struct MEDMCU_CONFIGS {
    int sram_size;
    int status;     /* 1 for online, 2 for offline */
    int medmcuctl;
} _medmcu_configs;


/******************************************************************************
 * Get medmcu configurations from device tree.
 ******************************************************************************/
static void _load_medmcu_configs(void *fdt)
{
    int nodeoffset;
    unsigned int *idata = NULL;
    char *cdata = NULL;
    int len = 0;

    if (fdt == NULL)
        return;

    nodeoffset = fdt_node_offset_by_compatible(fdt, -1, "mediatek,medmcu");
    if (nodeoffset < 0)
        assert(0);

    idata = (unsigned int *)fdt_getprop(fdt, nodeoffset, "scp_sramSize", &len);
    if (idata == NULL)
        _medmcu_configs.sram_size = 0;
    else
        _medmcu_configs.sram_size = fdt32_to_cpu(*(unsigned int *)idata);

    cdata = (char *)fdt_getprop(fdt, nodeoffset, "status", &len);
    if (cdata == NULL)
        _medmcu_configs.status = 0;
    else {
        if (strncmp(cdata, "okay", len) == 0)
            _medmcu_configs.status = 1; /* 1 goes for "online" */
        else
            _medmcu_configs.status = 0; /* 0 goes for "offline" */
    }

    cdata = (char *)fdt_getprop(fdt, nodeoffset, "medmcuctl", &len);
    if (cdata == NULL)
        _medmcu_configs.medmcuctl = 0;
    else
        _medmcu_configs.medmcuctl = atoul(cdata);
}

/******************************************************************************
 * load medmcu image from storage to dram and medmcu's sram.
 ******************************************************************************/
int platform_load_medmcu(void *fdt)
{
    int ret;
    int sram_size = 0;
    const char *part_name = "medmcu_1";
    void *dram_addr_phy = 0;
    void *dram_addr = NULL;
    unsigned int dram_size = 0;
    const char *prompt = "MEDMCU";

    /* load medmcu settings from fdt */
    _load_medmcu_configs(fdt);

    /* check if medmcu is turned off manually*/
    if (medmcu_get_exist() == 0) {
        LTRACEF("[MEDMCU] get_medmcu_status disabled\n");
        disable_medmcu_hw();
        return ERR_OFFLINE;
    }

    sram_size = medmcu_get_sram_size();
    if (sram_size <= 0)
        assert(0);

    ret = (int)medmcu_dram_alloc(&dram_addr_phy, &dram_size);
    if (ret == 0)
        assert(0);

    ret = vmm_alloc_physical(vmm_get_kernel_aspace(),
                        "MEDMCU-reserved",
                        dram_size,
                        &dram_addr,
                        PAGE_SIZE_SHIFT,
                        (paddr_t)dram_addr_phy,
                        0,
                        ARCH_MMU_FLAG_CACHED);
    if (ret != NO_ERROR)
        assert(0);

    LTRACEF("%s: dram_addr_phy=%p, dram_addr=%p, size=%d\n",
        prompt, dram_addr_phy, dram_addr, ret*PAGE_SIZE);
    LTRACEF("%s: part_name=%s, sram_size=%d\n", prompt, part_name, sram_size);

    ret = verify_load_medmcu_image(part_name, dram_addr_phy, dram_addr);
    if (ret <= 0)
        assert(0);

    /*clean dcache & icache before set up EMI MPU*/
    arch_sync_cache_range((addr_t)dram_addr, dram_size);

    /* use emi mpu to protect the content of dram */
    medmcu_emi_protect_enable(dram_addr_phy);

    return NO_ERROR;
}

/******************************************************************************
 * APIs for MEDMCU configurations
 ******************************************************************************/

/*
 * Load MEDMCU sram size from dts
 */
int medmcu_get_sram_size(void)
{
    return _medmcu_configs.sram_size;
}

/*
 * Get the final result of medmcu setting from device tree or environment
 */
int medmcu_get_exist(void)
{
    return _medmcu_configs.status;
}

/*
 * Get the medmcuctl setting from device tree
 */
int medmcu_get_medmcuctl(void)
{
    return _medmcu_configs.medmcuctl;
}

