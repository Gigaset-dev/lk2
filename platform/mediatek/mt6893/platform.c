/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <assert.h>
#include <bits.h>
#include <dconfig.h>
#include <dev/interrupt/arm_gic.h>
#include <dev/timer/arm_generic.h>
#include <dev/uart.h>
#include <devapc.h>
#include <dynamic_log.h>
#include <err.h>
#include <gpu_dfd.h>
#include <kernel/vm.h>
#include <lib/pl_boottags.h>
#include <lib/watchdog.h>
#include <mmc_core.h>
#include <mt_disp_drv.h>
#include <mt_i2c.h>
#include <mtk_key.h>
#include <platform.h>
#include <platform/addressmap.h>
#include <platform/dbgtop.h>
#include <platform/irq.h>
#include <platform/memory_layout.h>
#include <platform/mrdump_params.h>
#include <platform/sec_devinfo.h>
#include <platform/timer.h>
#include <platform/video.h>
#include <platform/wdt.h>
#include <platform_mtk.h>
#include <profiling.h>
#include <reg.h>
#include <RoT.h>
#include <string.h>
#include <sysenv.h>
#include <trace.h>
#include <ufs_platform.h>
#include <ufs_interface.h>

#define LOCAL_TRACE 1

#define BOOT_TAG_BOOT_REASON    0x88610001
static uint32_t    boot_reason;

#define BOOT_TAG_BOOT_MODE      0x88610002
static uint32_t    boot_mode;

#define BOOT_TAG_BOOT_TIME       0x88610007
static struct boot_tag_boot_time {
    lk_time_t bl2_boot_time;
    lk_time_t bl2_ext_boot_time;
    lk_time_t logo_time;
} time_profile;

static lk_time_t lk_start_time;

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

lk_time_t get_pl_boot_time(void)
{
    return time_profile.bl2_boot_time;
}

lk_time_t get_bl2_ext_boot_time(void)
{
    return time_profile.bl2_ext_boot_time;
}

lk_time_t get_lk_start_time(void)
{
    return lk_start_time;
}

lk_time_t get_logo_time(void)
{
    return time_profile.logo_time;
}

uint32_t platform_get_boot_reason(void)
{
    return boot_reason;
}

uint32_t platform_get_boot_mode(void)
{
    return boot_mode;
}

void platform_init(void)
{
    void *fdt = NULL;

    /* For those modules need calling devinfo API for segment,
     * devinfo needs to be initialized as early as possible.
     */
    PROFILING(devinfo_init_mode((char *)MTK_MT22_MODE));


    /* do storage init above all */
    if ((readl(GPIO_BANK_BASE) & TRAP_UFS_FIRST) == 0)
        PROFILING(ufs_init());
    else
        PROFILING(emmc_init());

    /* do dconfig init */
    PROFILING(dconfig_initenv());

    /* do platform dtb init */
    PROFILING(fdt = platform_load_device_tree());

    /* do platform driver init */
    PROFILING(i2c_hw_init(fdt));
    PROFILING(dbgtop_drm_init(fdt));
    PROFILING(env_init());
    PROFILING(disp_sw_init(fdt));
    PROFILING(drv_video_init());
    PROFILING(keypad_init(fdt));
    PROFILING(gpu_dfd_init());

    /* finished platform init, free dtb */
    if (fdt)
        free(fdt);
}

void platform_early_init(void)
{
    pmm_add_arena(&arena);
    uart_init_port(log_port(), log_baudrate());
    arm_gic_init();
    arm_generic_timer_init(ARM_GENERIC_TIMER_PHYSICAL_INT, 0);
    /* start time profiling after timer_init to get current_time correctly */
    lk_start_time = current_time();
    platform_watchdog_set_enabled(true);
}

void platform_quiesce(void)
{
    uint64_t isr = ARM64_READ_SYSREG(isr_el1);

    /* check if ISR EL1 A bit[8] was toggled before exiting */
    if (BIT(isr, 8))
        panic("Serror! isr_el1 = 0x%llx\n", isr);

    ufs_deinit();

    platform_stop_timer();
    mtk_wdt_set_timeout_value(WDT_MAX_TIMEOUT);
    mtk_wdt_restart_timer();
    /* save mblock reserved info into AEE-LK reserved memory
     * 1. provide tinysys's reserved info for their coredump
     * 2. parse mblock reserved info for offline debug tool
     */
    mrdump_save_reserved_info();
    platform_sec_post_init();

}

void platform_sec_post_init(void)
{
    devapc_protection_enable(DEVAPC_INFRA);
}

static void pl_boottags_boot_mode_hook(struct boot_tag *tag)
{
    memcpy(&boot_mode, &tag->data, sizeof(boot_mode));
}
PL_BOOTTAGS_INIT_HOOK(pl_boottags_boot_mode_hook,
                      BOOT_TAG_BOOT_MODE,
                      pl_boottags_boot_mode_hook);

static void pl_boottags_boot_reason_hook(struct boot_tag *tag)
{
    memcpy(&boot_reason, &tag->data, sizeof(boot_reason));
}
PL_BOOTTAGS_INIT_HOOK(pl_boottags_boot_reason_hook,
                      BOOT_TAG_BOOT_REASON,
                      pl_boottags_boot_reason_hook);

static void pl_boottags_boot_time_hook(struct boot_tag *tag)
{
    memcpy(&time_profile, &tag->data, sizeof(struct boot_tag_boot_time));
}
PL_BOOTTAGS_INIT_HOOK(time_profile, BOOT_TAG_BOOT_TIME, pl_boottags_boot_time_hook);
