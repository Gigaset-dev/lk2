/*
 * Copyright (c) 2019 MediaTek Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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
#include <scp.h>
#include <scp_plat.h>
#include <stdint.h>
#include <trace.h>

#define LOCAL_TRACE (0)

struct SCP_CONFIGS {
    int sram_size;
    int status;     /* 1 for online, 2 for offline */
    int scpctl;
} _scp_configs;


void __attribute__ ((weak)) scp_sharedram_alloc(void)
{
    /* normal dump, will allocate at kernel */
    return;
}

/******************************************************************************
 * Get scp configurations from device tree.
 ******************************************************************************/
static void _load_scp_configs(void *fdt)
{
    int nodeoffset;
    unsigned int *idata = NULL;
    char *cdata = NULL;
    int len = 0;

    if (fdt == NULL)
        return;

    nodeoffset = fdt_node_offset_by_compatible(fdt, -1, "mediatek,scp");
    if (nodeoffset < 0)
        assert(0);

    idata = (unsigned int *)fdt_getprop(fdt, nodeoffset, "scp_sramSize", &len);
    if (idata == NULL)
        _scp_configs.sram_size = 0;
    else
        _scp_configs.sram_size = fdt32_to_cpu(*(unsigned int *)idata);

    cdata = (char *)fdt_getprop(fdt, nodeoffset, "status", &len);
    if (cdata == NULL)
        _scp_configs.status = 0;
    else {
        if (strncmp(cdata, "okay", len) == 0)
            _scp_configs.status = 1; /* 1 goes for "online" */
        else
            _scp_configs.status = 0; /* 0 goes for "offline" */
    }

    cdata = (char *)fdt_getprop(fdt, nodeoffset, "scpctl", &len);
    if (cdata == NULL)
        _scp_configs.scpctl = 0;
    else
        _scp_configs.scpctl = atoul(cdata);
}

/******************************************************************************
 * load scp image from storage to dram and scp's sram.
 ******************************************************************************/
int platform_load_scp(void *fdt)
{
    int ret;
    int sram_size = 0;
    const char *part_name = "scp";
    void *dram_addr_phy = 0;
    void *dram_addr = NULL;
    unsigned int dram_size = 0;
    const char *prompt = "SCP";

    /* load scp settings from fdt */
    _load_scp_configs(fdt);

    /* check if scp is turned off manually*/
    if (scp_get_exist() == 0) {
        LTRACEF("[SCP] get_scp_status disabled\n");
        disable_scp_hw();
        return ERR_OFFLINE;
    }

    sram_size = scp_get_sram_size();
    if (sram_size <= 0)
        assert(0);

    ret = (int)scp_dram_alloc(&dram_addr_phy, &dram_size);
    if (ret == 0)
        assert(0);

    ret = vmm_alloc_physical(vmm_get_kernel_aspace(),
                        "SCP-reserved",
                        dram_size,
                        &dram_addr,
                        PAGE_SIZE_SHIFT,
                        (paddr_t)dram_addr_phy,
                        0,
                        ARCH_MMU_FLAG_CACHED);
    if (ret != NO_ERROR)
        assert(0);

    LTRACEF("%s: dram_addr_phy=%p, dram_addr=%p, size=%lu\n",
        prompt, dram_addr_phy, dram_addr, ret*PAGE_SIZE);
    LTRACEF("%s: part_name=%s, sram_size=%d\n", prompt, part_name, sram_size);

    ret = verify_load_scp_image(part_name, dram_addr_phy, dram_addr);
    if (ret <= 0)
        assert(0);

    /*clean dcache & icache before set up EMI MPU*/
    arch_clean_invalidate_cache_range((addr_t)dram_addr, dram_size);

    /* use emi mpu to protect the content of dram */
    scp_emi_protect_enable(dram_addr_phy);

    /* allocate and set protection of share dram for scp */
    scp_sharedram_alloc();

    return NO_ERROR;
}

/******************************************************************************
 * APIs for SCP configurations
 ******************************************************************************/

/*
 * Load SCP sram size from dts
 */
int scp_get_sram_size(void)
{
    return _scp_configs.sram_size;
}

/*
 * Get the final result of scp setting from device tree or environment
 */
int scp_get_exist(void)
{
    return _scp_configs.status;
}

/*
 * Get the scpctl setting from device tree
 */
int scp_get_scpctl(void)
{
    return _scp_configs.scpctl;
}

