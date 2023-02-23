/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <arch/arch_ops.h>
#include <assert.h>
#include <bits.h>
#include <dconfig.h>
#include <debug.h>
#include <dev/interrupt/arm_gic.h>
#include <dev/timer/arm_generic.h>
#include <dev/uart.h>
#include <devapc.h>
#include <dynamic_log.h>
#include <err.h>
#include <kernel/vm.h>
#include <lib/pl_boottags.h>
#include <lib/watchdog.h>
#include <mblock.h>
#include <mmc_core.h>
#include <mt_disp_drv.h>
#include <mt_i2c.h>
#include <mt6315.h>
#include <mt6368.h>
#include <mtk_dcm.h>
#include <mtk_key.h>
#include <platform.h>
#include <platform/addressmap.h>
#include <platform/dbgtop.h>
#include <platform/dfd_mcu.h>
#include <platform/gz_api.h>
#include <platform/irq.h>
#include <platform/lastbus.h>
#include <platform/memory_layout.h>
#include <platform/mrdump_params.h>
#include <platform/timer.h>
#include <platform/video.h>
#include <platform/wdt.h>
#include <platform_mtk.h>
#include <profiling.h>
#include <reg.h>
#include <spmi_common.h>
#include <sboot.h>
#include <string.h>
#include <sysenv.h>
#include <trace.h>
#include <ufs_platform.h>
#include <ufs_interface.h>
#include <verified_boot.h>

#define LOCAL_TRACE 1

#define BOOT_TAG_BOOT_TIME      0x88610007
static struct boot_tag_boot_time {
    lk_time_t bl2_boot_time;
    lk_time_t bl2_ext_boot_time;
    lk_time_t logo_time;
    lk_time_t tfa_boot_time;
    lk_time_t sec_os_boot_time;
    lk_time_t gz_boot_time;
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
        .phys = MCUCFG_BASE_PHY,
        .virt = MCUCFG_BASE,
        .size = MCUCFG_SIZE,
        .flags = MMU_INITIAL_MAPPING_FLAG_DEVICE,
        .name = "mcucfg"
    },
    {
        .phys = DBG_BASE_PHY,
        .virt = DBG_BASE,
        .size = DBG_SIZE,
        .flags = MMU_INITIAL_MAPPING_FLAG_DEVICE,
        .name = "dbg"
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

lk_time_t get_tfa_boot_time(void)
{
    return time_profile.tfa_boot_time;
}

lk_time_t get_sec_os_boot_time(void)
{
    return time_profile.sec_os_boot_time;
}

lk_time_t get_gz_boot_time(void)
{
    return time_profile.gz_boot_time;
}

lk_time_t get_lk_start_time(void)
{
    return lk_start_time;
}

lk_time_t get_logo_time(void)
{
    return time_profile.logo_time;
}

void platform_init(void)
{
    void *fdt = NULL;

    /* do storage init above all */
#ifdef PROJECT_TYPE_FPGA
    int ret;

    PROFILING(ret = ufs_init());
    if (ret)
       PROFILING(emmc_init());
#else
    PROFILING(ufs_init());
#endif

    /* do verify boot init */
    PROFILING(sec_lk_vb_init());

    /* do dconfig init */
    PROFILING(dconfig_initenv());

    /* do platform dtb init */
    PROFILING(fdt = platform_load_device_tree());

    /* do platform driver init */
    PROFILING(i2c_hw_init(fdt));
    PROFILING(lastbus_init(fdt));
    PROFILING(disp_sw_init(fdt));
    PROFILING(drv_video_init());
    PROFILING(env_init());
    PROFILING(keypad_init(fdt));
    PROFILING(mt6368_device_register());
    PROFILING(mt6315_device_register(SPMI_MASTER_1, SPMI_SLAVE_6));
    PROFILING(mt6315_device_register(SPMI_MASTER_1, SPMI_SLAVE_7));
    PROFILING(mt_dcm_init(fdt));
    PROFILING(dbgtop_drm_init(fdt));
    PROFILING(dfd_mcu_init(fdt));

    /* finished platform init, free dtb */
    if (fdt)
        free(fdt);
}

uint32_t need_relocate(void)
{
    if (mblock_get_memory_size() > 0x400000000ULL)
        return 1;
    else
        return 0;
}

void platform_early_init(void)
{
    arch_enable_serror();
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
    ufs_deinit();
    platform_stop_timer();
    /* store and clear restart reason in nonrst2 register */
    mtk_wdt_get_restart_reason();
    mtk_wdt_set_timeout_value(WDT_MAX_TIMEOUT);
    mtk_wdt_restart_timer();
    /* save mblock reserved info into AEE-LK reserved memory
     * 1. provide tinysys's reserved info for their coredump
     * 2. parse mblock reserved info for offline debug tool
     */
    mrdump_save_reserved_info();

    /* unmap2 deals with el1s2 and el2s1 unmap tasks */
    if (is_el2_enabled())
        PROFILING(gz_unmap2());

    platform_sec_post_init();
}

void platform_sec_post_init(void)
{
    sec_otp_set_da_ver_adpter();
    devapc_protection_enable(DEVAPC_INFRA);
    devapc_protection_enable(DEVAPC_VLP);
    devapc_protection_enable(DEVAPC_ADSP);
    devapc_protection_enable(DEVAPC_MMINFRA);
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
