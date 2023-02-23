/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <arch/arch_ops.h>
#include <assert.h>
#include <dconfig.h>
#include <debug.h>
#include <dev/interrupt/arm_gic.h>
#include <dev/timer/arm_generic.h>
#include <dev/uart.h>
#include <dynamic_log.h>
#include <err.h>
#include <kernel/vm.h>
#include <lib/pl_boottags.h>
#include <lib/watchdog.h>
#include <mcdi.h>
#include <mmc_core.h>
#include <mt_i2c.h>
#include <mt_disp_drv.h>
#include <platform.h>
#include <platform/addressmap.h>
#include <platform/dbgtop.h>
#include <platform/dfd_mcu.h>
#include <platform/gpio.h>
#include <platform/irq.h>
#include <platform/lastbus.h>
#include <platform/leds.h>
#include <platform/memory_layout.h>
#include <platform/timer.h>
#include <platform/video.h>
#include <platform/wdt.h>
#include <platform_mtk.h>
#include <reg.h>
#include <scp.h>
#include <string.h>
#include <sysenv.h>
#include <trace.h>
#include <ufs_interface.h>
#include <ufs_platform.h>
#include <verified_boot.h>

#define LOCAL_TRACE 1

struct mmu_initial_mapping mmu_initial_mappings[] = {
    {
        .phys = MEMBASE,
        .virt = KERNEL_BASE,
        .size = MEMSIZE,
        .flags = 0,
        .name = "ram"
    },
    {
        .phys = HWVER_BASE_PHY,
        .virt = HWVER_BASE,
        .size = HWVER_SIZE,
        .flags = MMU_INITIAL_MAPPING_FLAG_DEVICE,
        .name = "hwver"
    },
    {
        .phys = GIC_BASE_PHY,
        .virt = GIC_BASE,
        .size = GIC_SIZE,
        .flags = MMU_INITIAL_MAPPING_FLAG_DEVICE,
        .name = "gic"
    },
    {
        .phys = MCUCFG_BASE_PHY,
        .virt = MCUCFG_BASE,
        .size = MCUCFG_SIZE,
        .flags = MMU_INITIAL_MAPPING_FLAG_DEVICE,
        .name = "mcucfg"
    },
    {
        .phys = IO_BASE_PHY,
        .virt = IO_BASE,
        .size = IO_SIZE,
        .flags = MMU_INITIAL_MAPPING_FLAG_DEVICE,
        .name = "io"
    },
    {
        .phys = PL_BOOTTAGS_BASE_PHY,
        .virt = PL_BOOTTAGS_BASE,
        .size = PL_BOOTTAGS_SIZE,
        .flags = 0,
        .name = "pl_boottags"
    },
    {
        .phys = SRAM_BASE_PHY,
        .virt = SRAM_BASE,
        .size = SRAM_SIZE,
        .flags = 0,
        .name = "sram"
    },
    /* null entry to terminate the list */
    { .size = 0 }
};

static pmm_arena_t arena = {
    .name = "dram",
    .base = MEMBASE,
    .size = MEMSIZE,
    .flags = PMM_ARENA_FLAG_KMAP,
};

#if ARCH_ARM64
void platform_elX_init(void)
{
    unsigned int current_el = ARM64_READ_SYSREG(CURRENTEL) >> 2;
    uint64_t vec_base_phy = (uint64_t)&arm64_el2_or_el3_exception_base;

    if (current_el == 3) {
#if WITH_KERNEL_VM
        vec_base_phy &= ~(~0ULL << MMU_KERNEL_SIZE_SHIFT);
#endif
        ARM64_WRITE_SYSREG(VBAR_EL3, vec_base_phy);
        ARM64_WRITE_SYSREG(cntfrq_el0, 13000000LL);
        gic_el3_setup();
        disp_el3_setup();
        /* get DFD enable status */
        save_dfd_status();
        /* Treat all (E)SPIs/PPI/SGI as G1NS by default. */
        gic_config_irq_g1ns();
        dump_cpc_data();
    }
}

void platform_elX_to_el1(void)
{

    /*
     * Initialise CNTHCTL_EL2. All fields are
     * architecturally UNKNOWN on reset and are set to zero
     * except for field(s) listed below.
     *
     * CNTHCTL_EL2.EL1PCEN: Set to one to disable traps to
     * Hyp mode of Non-secure EL0 and EL1 accesses to the
     * physical timer registers.
     *
     * CNTHCTL_EL2.EL1PCTEN: Set to one to disable traps to
     * Hyp mode of  Non-secure EL0 and EL1 accesses to the
     * physical counter registers.
     */
    __asm__ volatile ("mrs x0, cnthctl_el2");
    __asm__ volatile ("orr x0, x0, #(1<<0)");
    __asm__ volatile ("orr x0, x0, #(1<<1)");
    __asm__ volatile ("msr cnthctl_el2, x0");

    /* switch to non-secure world */
    __asm__ volatile ("mrs x0, scr_el3");
    __asm__ volatile ("orr x0, x0, #1");
    __asm__ volatile ("msr scr_el3, x0");
}
#endif

void platform_init(void)
{
    void *fdt = NULL;

    /* do storage init above all */
#ifdef PROJECT_TYPE_FPGA
    int ret;

    ret = ufs_init();
    if (ret)
        emmc_init();
#else
    if ((readl(GPIO_BANK_BASE) & TRAP_UFS_FIRST) == 0)
        ufs_init();
    else
        emmc_init();
#endif

    /* do verify boot init */
    sec_lk_vb_init();

    /* do dconfig init */
    dconfig_initenv();
    env_init();
    /* do platform dtb init */
    fdt = platform_load_device_tree();

    /* do platform driver init */
    mt_gpio_set_default(fdt);
    i2c_hw_init(fdt);
    lastbus_init_aee(fdt);
    dfd_mcu_init_aee(fdt);
    disp_init(fdt);
    leds_init(fdt);
    leds_on();
    drv_video_init();
    dbgtop_drm_init(fdt);

    /* probe scp info from dts */
    scp_dts_probe(fdt);

    /* sspm boot up */
    platform_load_sspm();

    /* mcupm boot up */
    platform_load_mcupm();

    /* finished platform init, free dtb */
    if (fdt)
        free(fdt);
}

void platform_early_init(void)
{
    arch_enable_serror();
    pmm_add_arena(&arena);
    uart_init_port(log_port(), log_baudrate());
    arm_gic_init();
    arm_generic_timer_init(ARM_GENERIC_TIMER_PHYSICAL_INT, 0);
    platform_watchdog_set_enabled(true);
}

void platform_quiesce(void)
{
#ifndef PROJECT_TYPE_FPGA
    if ((readl(GPIO_BANK_BASE) & TRAP_UFS_FIRST) == 0)
        ufs_deinit();
#endif
    platform_stop_timer();
    /* store and clear restart reason in nonrst2 register */
    mtk_wdt_get_restart_reason();
    mtk_wdt_set_timeout_value(WDT_MAX_TIMEOUT);
    mtk_wdt_restart_timer();
}
