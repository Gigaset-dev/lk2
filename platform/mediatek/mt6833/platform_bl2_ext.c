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
#include <mmc_core.h>
#include <mt_disp_drv.h>
#include <mt_i2c.h>
#include <mt_logo.h>
#include <mtk_battery.h>
#include <mtk_charger.h>
#include <mt6315.h>
#include <platform.h>
#include <platform_mtk.h>
#include <platform/addressmap.h>
#include <platform/boot_mode.h>
#include <platform/gpio.h>
#include <platform/irq.h>
#include <platform/leds.h>
#include <platform/memory_layout.h>
#include <platform/timer.h>
#include <platform_mtk.h>
#include <pmic_dlpt.h>
#include <profiling.h>
#include <reg.h>
#include <spmi_common.h>
#include <string.h>
#include <sysenv.h>
#include <trace.h>
#include <ufs_interface.h>
#include <ufs_platform.h>

#define LOCAL_TRACE 1

#define BOOT_TAG_BOOT_TIME       0x88610007
static struct boot_tag_boot_time {
    lk_time_t bl2_boot_time;
    lk_time_t bl2_ext_boot_time;
    lk_time_t logo_time;
    lk_time_t tfa_boot_time;
    lk_time_t sec_os_boot_time;
    lk_time_t gz_boot_time;
} time_profile;

PL_BOOTTAGS_TO_BE_UPDATED(time_profile, BOOT_TAG_BOOT_TIME, &time_profile);

static lk_time_t start_time;

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
    }
}
#endif

void platform_init(void)
{
    void *fdt = NULL;

    if ((readl(GPIO_BANK_BASE) & TRAP_UFS_FIRST) == 0)
        PROFILING(ufs_init());
    else
        PROFILING(emmc_init());
    /* do dconfig init */
    PROFILING(dconfig_initenv());
    PROFILING(env_init());
    /* do platform dtb init */
    PROFILING(fdt = platform_load_device_tree());

    /* do platform driver init */
    PROFILING(mt_gpio_set_default(fdt));

    /* do platform driver init */
    PROFILING(i2c_hw_init(fdt));
    PROFILING(disp_init(fdt));
    PROFILING(leds_init(fdt));
    PROFILING(charger_init(fdt));
    PROFILING(mtk_battery_init(fdt));
    PROFILING(check_sw_ocv());
    PROFILING(mtk_charger_start());
    PROFILING(calc_dlpt_imix_r());
    PROFILING(leds_on());

    if (mtk_charger_kpoc_check()) {
        PROFILING(mt_disp_show_low_battery());
    } else if (platform_get_boot_mode() == NORMAL_BOOT ||
        platform_get_boot_mode() == ALARM_BOOT) {
        PROFILING(mt_disp_show_boot_logo());
        time_profile.logo_time = current_time() - start_time;
    }

    PROFILING(mt6315_device_register(SPMI_MASTER_1, SPMI_SLAVE_3));
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
    /* start time profiling after timer_init to get current_time correctly */
    start_time = current_time();
    platform_watchdog_set_enabled(true);
}

void platform_quiesce(void)
{
    if ((readl(GPIO_BANK_BASE) & TRAP_UFS_FIRST) == 0)
        ufs_deinit();
    /* Finalize the boot time of BL2 extension before stopping timer
     * and updating pl_boottags.
     */
    time_profile.bl2_ext_boot_time = current_time() - start_time;
    platform_stop_timer();

    /* Update pl_boottags registered with PL_BOOTTAGS_TO_BE_UPDATED. */
    update_pl_boottags((struct boot_tag *)PL_BOOTTAGS_BASE);
}

void platform_quiesce_el3(void)
{
    unsigned int current_el = ARM64_READ_SYSREG(CURRENTEL) >> 2;

    /* de-init gic, clean up for later TFA stage */
    if (current_el == 3)
        gic_el3_deinit();
}

static void pl_boottags_boot_time_hook(struct boot_tag *tag)
{
    struct boot_tag_boot_time *p = (struct boot_tag_boot_time *)&tag->data;

    time_profile.bl2_boot_time     = p->bl2_boot_time;
    time_profile.bl2_ext_boot_time = p->bl2_ext_boot_time;
    time_profile.logo_time         = p->logo_time;
    time_profile.tfa_boot_time     = p->tfa_boot_time;
    time_profile.sec_os_boot_time  = p->sec_os_boot_time;
    time_profile.gz_boot_time      = p->gz_boot_time;
}
PL_BOOTTAGS_INIT_HOOK(time_profile, BOOT_TAG_BOOT_TIME, pl_boottags_boot_time_hook);
