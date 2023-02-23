/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <arch/defines.h>
#include <kernel/vm.h>
#include <platform/addressmap.h>
#include <platform/memory_layout.h>
#include <platform/platform_ext.h>

/*
 * In early init (like heap init), we need to allocate memory from pmm.
 * We add an early arena here with size 1MB for it.
 * In platform_init, we will add another arena with bigger size (DRAM HEAP)
 * We do this trick because we want to make the platform_init() common for both BL2 and BL33.
 *
 * Notice that the BL33_EARLY_HEAP is defined as the 1MB area before BL33 image.
 * We don't need to explicitly define it in mmu_initial_mappings.
 * The area itself will be covered and mapped by [MAP_ID_PROG].
 * There will be problems if we define the BL33_EARLY_HEAP outside the [MAP_ID_PROG].
 */
static pmm_arena_t arena_early = {
    .name = "heap_early_init",
    .base = MEMBASE + KERNEL_LOAD_OFFSET - (1 << 20), /* BL33_EARLY_HEAP */
    .size = (1 << 20), /* BL33_EARLY_HEAP_SIZE == 0x100000 == 1MB */
    .flags = PMM_ARENA_FLAG_KMAP,
    .priority = 1,
};

void platform_early_init_ext(void)
{
    /* Check if the BL33_EARLY_HEAP address is valid */
    assert(BL33_EARLY_HEAP_SIZE == (1 << 20));
    assert((MEMBASE + KERNEL_LOAD_OFFSET - BL33_EARLY_HEAP_SIZE) == BL33_EARLY_HEAP_BASE_PHY);
    assert(BL33_EARLY_HEAP_BASE_PHY >= MEMBASE);

    /* Add early arena */
    pmm_add_arena(&arena_early);
}
