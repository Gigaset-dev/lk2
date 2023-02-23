/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <app.h>
#include <arch.h>
#include <arch/mmu.h>
#include <arch/ops.h>
#include <assert.h>
#include <err.h>
#include <kernel/vm.h>
#include <lib/pl_boottags.h>
#include <load_image.h>
#include <mblock.h>
#include <platform.h>
#include <platform/memory_layout.h>
#include <platform_mtk.h>
#include <profiling.h>

#ifdef LEGACY_BL31
#define ATF_HEADER_SIG_SIZE (0x240)
static size_t bl31_load_offset = (PAGE_SIZE - ATF_HEADER_SIG_SIZE);
static size_t bl31_jump_offset = PAGE_SIZE;
#else
static size_t bl31_load_offset;
static size_t bl31_jump_offset;
#endif

typedef void (*jump_func_type)(ulong arg0, ulong arg1, ulong arg2, ulong arg3) __NO_RETURN;

void mtk_sip(uint64_t bl31_addr, paddr_t bl31_info, uint32_t bl31_reserve)
{
    platform_quiesce_el3();

    jump_func_type jump_func = (jump_func_type)bl31_addr;
    (*jump_func)(bl31_info, bl31_reserve, 0, 0);
}

void jump_to_bl31(uint64_t bl31_addr, paddr_t bl31_info, uint32_t bl31_reserve)
{
    __asm__ volatile("smc #0\n\t");
}

static status_t app_load_picachu(void)
{
    status_t ret = NO_ERROR;

    PROFILING_START("load picachu");
    ret = platform_load_picachu();
    PROFILING_END();

    return ret;
}

static status_t app_load_sspm(void)
{
    status_t ret = NO_ERROR;

    PROFILING_START("load sspm");
    ret = platform_load_sspm();
    PROFILING_END();

    return ret;
}

static status_t app_load_mcupm(void)
{
    status_t ret = NO_ERROR;

    PROFILING_START("load mcupm");
    ret = platform_load_mcupm();
    PROFILING_END();

    return ret;
}

__WEAK paddr_t bl31_mblock_alloc(const char *reserved_name)
{
    return mblock_alloc(BL31_SIZE, PAGE_SIZE, MBLOCK_NO_LIMIT,
        (paddr_t)BL31_BASE_PHY, 0, reserved_name);
}

static status_t app_load_bl31(void)
{
    paddr_t bl31_addr_pa;
    void *bl31_addr_va = (void *)BL31_BASE;
    char part_name[16] = "tee";
    status_t ret = NO_ERROR;

    PROFILING_START("load bl31");
    /* mblock reserved for BL31-reserved */
    bl31_addr_pa = bl31_mblock_alloc("BL31-reserved");

    if (bl31_addr_pa != BL31_BASE_PHY) {
        dprintf(CRITICAL, "[BL31] mblock allocation failed\n");
        return ERR_NO_MEMORY;
    }

    /* mmap the allocated memory */
    ret = vmm_alloc_physical(vmm_get_kernel_aspace(),
                        "vm-BL31-reserved",
                        BL31_SIZE,
                        &bl31_addr_va,
                        PAGE_SIZE_SHIFT,
                        (paddr_t)bl31_addr_pa,
                        VMM_FLAG_VALLOC_SPECIFIC,
                        ARCH_MMU_FLAG_CACHED);

    if (ret != NO_ERROR) {
        dprintf(CRITICAL, "[BL31] vmm_alloc_physical failed, err: %d\n", ret);
        goto err_vmm_alloc;
    }

    /* load image */
    ret = load_partition(part_name, "atf", bl31_addr_va + bl31_load_offset, BL31_SIZE);
    if (ret < 0) {
        dprintf(CRITICAL, "[BL31] load failed, ret: %d\n", ret);
        goto err_load_partition;
    }

    ret = vmm_free_region(vmm_get_kernel_aspace(), (vaddr_t)bl31_addr_va);

    PROFILING_END();
    return NO_ERROR;


err_load_partition:
    vmm_free_region(vmm_get_kernel_aspace(), (vaddr_t)bl31_addr_va);

err_vmm_alloc:
    mblock_free(bl31_addr_pa);

    return ret;
}

static status_t app_load_bl33(void)
{
    paddr_t bl33_addr_pa;
    void *bl33_addr_va = (void *)LK_MEM_BASE;
    char part_name[8] = "lk";
    status_t ret = NO_ERROR;

    PROFILING_START("load bl33");
    bl33_addr_pa = mblock_alloc(LK_MEM_SIZE, PAGE_SIZE, MBLOCK_NO_LIMIT,
        (paddr_t)LK_MEM_BASE_PHY, 0, "BL33-reserved");

    if (bl33_addr_pa != LK_MEM_BASE_PHY) {
        dprintf(CRITICAL, "BL33 mblock allocation failed\n");
        return ERR_NO_MEMORY;
    }

    ret = vmm_alloc_physical(vmm_get_kernel_aspace(),
                        "vm-BL33-reserved",
                        LK_MEM_SIZE,
                        &bl33_addr_va,
                        PAGE_SIZE_SHIFT,
                        (paddr_t)bl33_addr_pa,
                        VMM_FLAG_VALLOC_SPECIFIC,
                        ARCH_MMU_FLAG_CACHED);

    if (ret != NO_ERROR) {
        dprintf(CRITICAL, "BL33 vmm_alloc_physical failed, err: %d\n", ret);
        goto err_vmm_alloc;
    }

    ret = load_partition(part_name, "lk", bl33_addr_va, LK_MEM_SIZE);
    if (ret < 0) {
        dprintf(CRITICAL, "BL33 load failed, ret: %d\n", ret);
        goto err_load_partition;
    }

    PROFILING_END();
    return NO_ERROR;


err_load_partition:
    vmm_free_region(vmm_get_kernel_aspace(), (vaddr_t)bl33_addr_va);

err_vmm_alloc:
    mblock_free(bl33_addr_pa);

    return ret;
}

static status_t app_load_bl32(void)
{
    status_t ret = NO_ERROR;

    PROFILING_START("load bl32");
    ret = platform_set_tz_tags();
    ret |= platform_load_tee();
    PROFILING_END();

    return ret;
}

void bl2_ext_boot_init(const struct app_descriptor *app)
{
}

void bl2_ext_boot_entry(const struct app_descriptor *app, void *args)
{
    paddr_t bl31_info = lk_boot_args[0];
    ulong reserved1 = lk_boot_args[1];
    ulong reserved2 = lk_boot_args[2];
    ulong reserved3 = lk_boot_args[3];

    ASSERT(app_load_bl33() == NO_ERROR);
    ASSERT(app_load_bl31() == NO_ERROR);
    ASSERT(app_load_sspm() == NO_ERROR);
    ASSERT(app_load_mcupm() == NO_ERROR);
    ASSERT(app_load_bl32() == NO_ERROR);
    ASSERT(app_load_picachu() == NO_ERROR);

    arch_chain_load(jump_to_bl31, BL31_BASE_PHY + bl31_jump_offset, bl31_info,
                    reserved1, reserved2);
}

APP_START(bl2_ext_boot)
.init = bl2_ext_boot_init,
.entry = bl2_ext_boot_entry,
 APP_END
