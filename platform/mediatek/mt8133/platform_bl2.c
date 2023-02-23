/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <kernel/vm.h>
#include <memory.h>
#include <platform/addressmap.h>
#include <platform/platform_ext.h>
#include <platform/pll.h>

static pmm_arena_t arena_early = {
    .name = "sram",
    .base = SRAM_BASE_PHY,
    .size = SRAM_SIZE,
    .flags = PMM_ARENA_FLAG_KMAP,
    .priority = 1,
};

void platform_early_init_ext(void)
{
    mt_pll_init();
    mt_mem_init();
    pmm_add_arena(&arena_early);
}

void check_dram_heap_boundary(void)
{
    assert(DRAM_BASE_PHY + mt_mem_size() >=
           DRAM_HEAP_BASE_PHY + DRAM_HEAP_SIZE);
}
