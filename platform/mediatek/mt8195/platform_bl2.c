/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <arch/arch_ops.h>
#include <arch/mmu.h>
#include <dev/interrupt/arm_gic.h>
#include <init_mtk.h>
#include <kernel/vm.h>
#include <lib/watchdog.h>
#include <lk/init.h>
#include <memory.h>
#include <mt_i2c.h>
#include <mt6315.h>
#include <platform/addressmap.h>
#include <platform/memory_layout.h>
#include <platform/pll.h>
#include <platform/pmic.h>
#include <platform/wdt.h>
#include <platform/vm.h>
#include <pmic_wrap.h>
#include <pmif.h>
#include <spmi_common.h>
#include <string.h>
#include <subpmic.h>
#include <trace.h>

#define LOCAL_TRACE 0

enum {
    MAP_ID_PROG = 0,
    MAP_ID_MCUCFG,
    MAP_ID_GIC,
    MAP_ID_IO,
    MAP_ID_DRAM_HEAP,
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
    [MAP_ID_MCUCFG] = {
        .phys = MCUCFG_BASE_PHY,
        .virt = MCUCFG_BASE,
        .size = MCUCFG_SIZE,
        .flags = MMU_INITIAL_MAPPING_FLAG_DEVICE,
        .name = "mcucfg"
    },
    [MAP_ID_IO] = {
        .phys = IO_BASE_PHY,
        .virt = IO_BASE,
        .size = IO_SIZE,
        .flags = MMU_INITIAL_MAPPING_FLAG_DEVICE,
        .name = "io"
    },
    [MAP_ID_DRAM_HEAP] = { .size = 0 },
    [MAP_ID_MAX] = { .size = 0 },
};

static pmm_arena_t arena = {
    .name = "dram",
    .base = PMM_HEAP_BASE_PHY,
    .size = PMM_HEAP_SIZE,
    .flags = PMM_ARENA_FLAG_KMAP,
    .priority = 0,
};

static pmm_arena_t arena_early = {
    .name = "sram",
    .base = SRAM_BASE_PHY,
    .size = SRAM_SIZE,
    .flags = PMM_ARENA_FLAG_KMAP,
    .priority = 1,
};

#if ARCH_ARM64
void platform_elX_init(void)
{
    unsigned int current_el = ARM64_READ_SYSREG(CURRENTEL) >> 2;
    uint64_t vec_base_phy;

    if (current_el == 3) {
        /* setup el3 exception base (pa) to vbar_el3 */
        vec_base_phy = (uint64_t)&arm64_el2_or_el3_exception_base;
#if WITH_KERNEL_VM
        vec_base_phy &= ~(~0ULL << MMU_KERNEL_SIZE_SHIFT);
#endif
        ARM64_WRITE_SYSREG(vbar_el3, vec_base_phy);

        /* setup arch timer frequency */
        ARM64_WRITE_SYSREG(cntfrq_el0, 13000000LL);

        /* initialize gic at el3 */
        gic_el3_setup();
    }
}
#endif

static void mem_init_wrap(uint level)
{
    LTRACEF("dram calibration @ level=%u\n", level);
    mt_mem_init();
}
MT_LK_INIT_HOOK(BL2, mem_init_wrap, &mem_init_wrap, LK_INIT_LEVEL_PLATFORM - 1);

static void check_dram_heap_boundary(void)
{
    assert(DRAM_BASE_PHY + mt_mem_size() >=
           DRAM_HEAP_BASE_PHY + DRAM_HEAP_SIZE);
}

static void platform_init_mmu_dram(vaddr_t va, paddr_t pa, size_t size,
                                   uint32_t index, const char *name)
{
    vmm_aspace_t *aspace;
    int err;

    aspace = vaddr_to_aspace((void *)va);
    assert(aspace);

    err = arch_mmu_map(&aspace->arch_aspace, va, pa, size >> PAGE_SIZE_SHIFT,
                       ARCH_MMU_FLAG_CACHED);
    assert(!err);

    /* Add to mmu_initial_mappings for pa to va lookup. */
    mmu_initial_mappings[index].phys = pa;
    mmu_initial_mappings[index].virt = va;
    mmu_initial_mappings[index].size = size;
    mmu_initial_mappings[index].flags = 0;
    mmu_initial_mappings[index].name = name;

    vmm_reserve_space(aspace, name, size, va);
}

void platform_early_init_ext(void)
{
    mt_pll_init();

    /* init pmic bus */
    platform_pmif_spmi_init();
    platform_pmif_spi_init();

    /* init watchdog */
    mtk_wdt_pre_init();
    platform_watchdog_set_enabled(true);

    pmm_add_arena(&arena_early);
}

void platform_init_ext(void)
{
    check_dram_heap_boundary();
    platform_init_mmu_dram(DRAM_HEAP_BASE, DRAM_HEAP_BASE_PHY,
                           DRAM_HEAP_SIZE, MAP_ID_DRAM_HEAP, "dram_heap");

    /*
     * pmm_add_arena() will call boot_alloc_mem() to allocate memory.
     * In the case of running from L2C shared SRAM, boot_alloc_mem() will
     * return addresses from L2C shared SRAM (which has only limited size).
     * Before we add a large dram arena into pmm, change boot memory to a DRAM
     * region.
     */
    boot_alloc_set_base(BOOT_ALLOC_BASE);
    pmm_add_arena(&arena);

    /* init pmic, subpmic and bus */
    i2c_hw_init(NULL);
    pmic_init();
    subpmic_init(NULL);
    mt6315_device_register(SPMI_MASTER_0, SPMI_SLAVE_6);
    mt6315_device_register(SPMI_MASTER_0, SPMI_SLAVE_7);
}
