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
#include <assert.h>
#include <arch/arm64.h>
#include <dev/interrupt/arm_gic.h>
#include <dev/timer/arm_generic.h>
#include <dev/uart.h>
#include <err.h>
#include <kernel/vm.h>
#include <lib/watchdog.h>
#include <mt_gic.h>
#include <mt_i2c.h>
#include <mtk_key.h>
#include <platform.h>
#include <platform/addressmap.h>
#include <platform/memory_layout.h>
#include <platform/irq.h>
#include <platform/timer.h>
#include <platform/platform_ext.h>
#include <platform/vm.h>
#include <platform/wdt.h>

enum {
    MAP_ID_PROG = 0,
    MAP_ID_GIC,
    MAP_ID_IO,
    MAP_ID_DRAM,
    MAP_ID_MAX
};

struct mmu_initial_mapping mmu_initial_mappings[] = {
    [MAP_ID_PROG] = {
        .phys = MEMBASE,
        .virt = KERNEL_BASE,
        .size = MEMSIZE,
        .flags = 0,
        .name = "prog"
    },
    [MAP_ID_GIC] = {
        .phys = GIC_BASE_PHY,
        .virt = GIC_BASE,
        .size = GIC_SIZE,
        .flags = MMU_INITIAL_MAPPING_FLAG_DEVICE,
        .name = "gic"
    },
    [MAP_ID_IO] = {
        .phys = IO_BASE_PHY,
        .virt = IO_BASE,
        .size = IO_SIZE,
        .flags = MMU_INITIAL_MAPPING_FLAG_DEVICE,
        .name = "io"
    },
    [MAP_ID_DRAM] = { .size = 0 },
    [MAP_ID_MAX] = { .size = 0 },
};

static pmm_arena_t arena = {
    .name = "dram",
    .base = PMM_HEAP_BASE_PHY,
    .size = PMM_HEAP_SIZE,
    .flags = PMM_ARENA_FLAG_KMAP,
    .priority = 0,
};

__WEAK void check_dram_heap_boundary(void)
{
}

void platform_elX_init(void)
{
    unsigned int current_el = ARM64_READ_SYSREG(CURRENTEL) >> 2;

    if (current_el == 3) {
        ARM64_WRITE_SYSREG(cntfrq_el0, 13000000LL);
        gic_el3_setup();
    }
}

void platform_early_init(void)
{
    mtk_wdt_pwr_latch();
    uart_init_port(DEBUG_UART, 921600);
    arm_gic_init();
    clear_sec_pol_ctl_en();
    arm_generic_timer_init(ARM_GENERIC_TIMER_PHYSICAL_INT, 0);
    watchdog_hw_init(0);

    platform_early_init_ext();
}

static void platform_init_mmu_dram(vaddr_t va, paddr_t pa, size_t size)
{
    vmm_aspace_t *aspace;
    int err;

    /* Add to mmu_initial_mappings for pa to va lookup. */
    mmu_initial_mappings[MAP_ID_DRAM].phys = pa;
    mmu_initial_mappings[MAP_ID_DRAM].virt = va;
    mmu_initial_mappings[MAP_ID_DRAM].size = size;
    mmu_initial_mappings[MAP_ID_DRAM].flags = 0;
    mmu_initial_mappings[MAP_ID_DRAM].name = "dram";

    aspace = vaddr_to_aspace((void *)va);
    assert(aspace);

    err = arch_mmu_map(&aspace->arch_aspace, va, pa, size >> PAGE_SIZE_SHIFT,
                       ARCH_MMU_FLAG_CACHED);
    assert(!err);
}

void platform_init(void)
{
    check_dram_heap_boundary();
    platform_init_mmu_dram(DRAM_HEAP_BASE, DRAM_HEAP_BASE_PHY, DRAM_HEAP_SIZE);

    /*
     * pmm_add_arena() will call boot_alloc_mem() to allocate memory.
     * In the case of running from L2C shared SRAM, boot_alloc_mem() will
     * return addresses from L2C shared SRAM (which has only limited size).
     * Before we add a large dram arena into pmm, change boot memory to a DRAM
     * region.
     */
    boot_alloc_set_base(BOOT_ALLOC_BASE);
    pmm_add_arena(&arena);
    keypad_init(NULL);
    i2c_hw_init(NULL);
}

void platform_quiesce(void)
{
    platform_stop_timer();
}
