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

#include <arch/ops.h>
#include <assert.h>
#include <debug.h>
#include <err.h>
#include <kernel/vm.h>
#include <libfdt.h>
#include <lib/kcmdline.h>
#include <lk/init.h>
#include <malloc.h>
#include <mblock.h>
#include <platform/aee_common.h>
#include <platform/mboot_params.h>
#include <platform/mrdump.h>
#include <platform/mrdump_params.h>
#include <printf.h>
#include <set_fdt.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <trace.h>

#define LOCAL_TRACE 0

void mrdump_free_reserved(void)
{
    static const char * const free_str[] = { "pl-boottag", "pl-drambuf" };
    const struct reserved_t *mblock_reserved;
    int i = 0;

    if (aee_check_enable() > AEE_ENABLE_NO) {
        for (i = 0; i < countof(free_str); i++) {
            mblock_reserved = mblock_query_reserved_by_name(free_str[i], 0);
            if (mblock_reserved) {
                if (mblock_free(mblock_reserved->start)) {
                    LTRACEF_LEVEL(ALWAYS, "free %s failed\n", free_str[i]);
                    ASSERT(0);
                }
            }
        }
    }
}

void mrdump_save_reserved_info(void)
{
    const struct reserved_t *mblock_reserved;
    const struct reserved_t *aee_lk_reserved;
    struct mrdump_reserved_info *mrdump_reserved_info;
    void *mblock_va = NULL;
    char name[MBLOCK_RESERVED_NAME_SIZE];
    int i;
    status_t err;

    aee_lk_reserved = mblock_query_reserved_by_name(AEE_LK_NAME, 0);
    if (!aee_lk_reserved) {
        LTRACEF_LEVEL(ALWAYS, "%s reserved memroy query failed\n", AEE_LK_NAME);
        return;
    }

    err = vmm_alloc_physical(vmm_get_kernel_aspace(),
            "vm_mblock_reserved_info", ROUNDUP(aee_lk_reserved->size, PAGE_SIZE),
            &mblock_va, PAGE_SIZE_SHIFT,
            (paddr_t)aee_lk_reserved->start, 0,
            ARCH_MMU_FLAG_CACHED);
    if (err != NO_ERROR) {
        dprintf(CRITICAL, "vm_mblock_reserved_info  vmm_alloc_physical failed\n");
        return;
    }

    /*
     * mblock reserved info offset = aee-lk base + aee-lk size - size of mblock
     */
    mrdump_reserved_info =
        (struct mrdump_reserved_info *)(mblock_va +
        aee_lk_reserved->size - MBLOCK_RESERVED_INFO_SIZE);

    for (i = 0; i < MBLOCK_RESERVED_NUM_MAX; i++) {
        mblock_reserved = mblock_query_reserved_by_idx(i);

        if (!mblock_reserved) {
                LTRACEF_LEVEL(ALWAYS, "mblock query finished, total reserved count %d\n", i);
                mrdump_reserved_info->mblock_reserved_num = i;
                break;
        }

        mrdump_reserved_info->reserved[i].start = mblock_reserved->start;
        mrdump_reserved_info->reserved[i].size = mblock_reserved->size;
        mrdump_reserved_info->reserved[i].mapping = mblock_reserved->mapping;
        strncpy(mrdump_reserved_info->reserved[i].name, mblock_reserved->name, sizeof(name) - 1);

        LTRACEF_LEVEL(INFO,
            "mrdump show :mblock-R[%d].start: 0x%llx, size: 0x%llx mapping: %d, name:%s\n", i,
            mrdump_reserved_info->reserved[i].start, mrdump_reserved_info->reserved[i].size,
            mrdump_reserved_info->reserved[i].mapping, mrdump_reserved_info->reserved[i].name);
    }

    /* calculate crc */
    mrdump_reserved_info->crc32 = 0xffffffff;
    mrdump_reserved_info->crc32 = calculate_crc32((void *)mrdump_reserved_info,
                                               sizeof(struct mrdump_reserved_info));

    /* set aee-lk range as invalid for flush to DRAM */
    arch_clean_invalidate_cache_range((addr_t)mblock_va,
            (size_t)ROUNDUP(aee_lk_reserved->size, PAGE_SIZE));

    vmm_free_region(vmm_get_kernel_aspace(), (vaddr_t)mblock_va);
}

/* Setup debug-kinfo device tree setting */
static void mrdump_debug_kinfo_fdt_setup(void *fdt)
{
    const struct reserved_t *reserved;
    int offset = 0;
    int nodeoffset = 0;
    int ret = 0;
    uint32_t phandle = 0;
    char name[128];
    char compatible[128];

    if (!fdt) {
        LTRACEF_LEVEL(CRITICAL, "fdt is NULL\n");
        return;
    }

    offset = fdt_path_offset(fdt, "/");
    if (offset < 0) {
        LTRACEF_LEVEL(CRITICAL, "fdt offset is invalid\n");
        return;
    }

    nodeoffset = fdt_add_subnode(fdt, offset, DEBUG_KINFO_DTS_NAME);
    if (nodeoffset < 0) {
        LTRACEF_LEVEL(CRITICAL, "Error: fdt_add_subnode failed offset=0x%x\n", nodeoffset);
        return;
    }

    snprintf(compatible, sizeof(compatible), "%s", DEBUG_KINFO_COMPATIBLE);
    ret = fdt_setprop_string(fdt, nodeoffset, "compatible", compatible);
    if (ret) {
        LTRACEF_LEVEL(CRITICAL, "Error: fdt_setprop_string failed ret=0x%x\n", ret);
        return;
    }

    ret = mblock_pre_alloc_phandle(DEBUG_KINFO_DTS_PATH, DEBUG_KINFO_RESERVED);
    if (ret) {
        LTRACEF_LEVEL(ALWAYS, "pre alloc phandle failed\n");
    }
}
SET_FDT_INIT_HOOK(mrdump_debug_kinfo_fdt_setup, mrdump_debug_kinfo_fdt_setup)


static void mrdump_ddr_rsv_fdt(void *fdt, int offset, const void *enable)
{
    if (fdt_setprop_string(fdt, offset, "mrdump,ddr_rsv", enable) == 0) {
        LTRACEF_LEVEL(ALWAYS, "%s: set ddr_rsv = %s\n",
        __func__, (char *)enable);
    } else {
        LTRACEF_LEVEL(ALWAYS, "%s: failed to set ddr_rsv %s\n",
        __func__, (char *)enable);
    }
}

/* +++ mrdump control block params +++ */
static int mrdump_cb_fdt(void *fdt, int offset)
{
    const struct reserved_t *cblock_reserved;

    if (!fdt) {
        LTRACEF_LEVEL(ALWAYS, "%s: fdt is NULL\n", __func__);
        return -1;
    }

    cblock_reserved = mblock_query_reserved_by_name("aee_debug_kinfo", 0);
    if (cblock_reserved) {
        if (fdt_setprop_u32(fdt, offset, "mrdump,cblock",
                    cblock_reserved->start + MRDUMP_CB_DRAM_OFF))
            return -1;

        if (fdt_appendprop_u32(fdt, offset, "mrdump,cblock", MRDUMP_CB_SIZE))
            return -1;
    }

    return 1;
}

static void mrdump_lk_version_fdt(void *fdt, int offset, const void *lk_version)
{
    if (!fdt) {
        LTRACEF_LEVEL(ALWAYS, "%s: fdt is NULL\n", __func__);
        return;
    }

    if (fdt_setprop_string(fdt, offset, "mrdump,lk", lk_version) == 0) {
        LTRACEF_LEVEL(ALWAYS, "%s: set lk_version = %s\n",
                __func__, (char *)lk_version);
    } else {
        LTRACEF_LEVEL(ALWAYS, "%s: failed to set lk_version %s\n",
                __func__, (char *)lk_version);
    }
}

static void mrdump_params_fdt_init(void *fdt)
{
    int chosen_offset;

    if (!fdt) {
        LTRACEF_LEVEL(ALWAYS, "fdt is NULL\n");
        return;
    }

    chosen_offset = fdt_path_offset(fdt, "/chosen");
    if (chosen_offset < 0) {
        LTRACEF_LEVEL(ALWAYS, "chosen not found\n");
        return;
    }

    switch (aee_check_enable()) {
    case AEE_ENABLE_NO:
        if (fdt_setprop_string(fdt, chosen_offset, "aee,enable", "no") != 0)
            LTRACEF_LEVEL(ALWAYS, "failed to set aee,enable node\n");
        return;
    case AEE_ENABLE_MINI:
        if (fdt_setprop_string(fdt, chosen_offset, "aee,enable", "mini") != 0)
            LTRACEF_LEVEL(ALWAYS, "failed to set aee,enable node\n");
        break;
    case AEE_ENABLE_FULL:
        if (fdt_setprop_string(fdt, chosen_offset, "aee,enable", "full") != 0)
            LTRACEF_LEVEL(ALWAYS, "failed to set aee,enable node\n");
        break;
    default:
        ASSERT(0);
        return;
    }

    /* ddr rsv */
    LTRACEF_LEVEL(ALWAYS, "ddr reserve status(%x).\n", aee_ddr_reserve_ready());

    if (aee_ddr_reserve_ready() != AEE_MRDUMP_DDR_RSV_READY)
        mrdump_ddr_rsv_fdt(fdt, chosen_offset, "no");
    else {
        if (aee_check_enable() == AEE_ENABLE_NO)
            mrdump_ddr_rsv_fdt(fdt, chosen_offset, "no");
        else
            mrdump_ddr_rsv_fdt(fdt, chosen_offset, "yes");
    }

    /* mrdump control block params */
    if (mrdump_cb_fdt(fdt, chosen_offset) <= 0)
        LTRACEF_LEVEL(ALWAYS, "failed to set mrdump,cblock entry\n");

    /* mrdump lk_version */
    mrdump_lk_version_fdt(fdt, chosen_offset, MRDUMP_GO_DUMP);
}
SET_FDT_INIT_HOOK(mrdump_params_fdt_init, mrdump_params_fdt_init);

#ifdef MTK_MRDUMP_ENABLE
static void mrdump_params_init(uint level)
{
    if (aee_check_enable() != AEE_ENABLE_FULL)
        return;

    /* default setting */
    if (!mrdump_get_default_output_device())
        LTRACEF_LEVEL(ALWAYS, "MT-RAMDUMP: unknown output device.\n");
}
LK_INIT_HOOK(init_mrdump_params, mrdump_params_init, LK_INIT_LEVEL_APPS - 1);
#endif
