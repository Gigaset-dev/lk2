/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <assert.h>
#include <dconfig.h>
#include <debug.h>
#include <dev/interrupt/arm_gic.h>
#include <dev/timer/arm_generic.h>
#include <dev/uart.h>
#include <devapc.h>
#include <dtb.h>
#include <dynamic_log.h>
#include <err.h>
#include <kernel/vm.h>
#include <lib/pl_boottags.h>
#include <lib/watchdog.h>
#include <libfdt.h>
#include <load_image.h>
#include <mt_disp_drv.h>
#include <mt_i2c.h>
#include <mt_logo.h>
#include <mt6315.h>
#include <platform.h>
#include <platform/addressmap.h>
#include <platform/dbgtop.h>
#include <platform/gpio.h>
#include <platform/irq.h>
#include <platform/leds.h>
#include <platform/memory_layout.h>
#include <platform/timer.h>
#include <platform/video.h>
#include <platform/wdt.h>
#include <platform_mtk.h>
#include <reg.h>
#include <scp.h>
#include <spmi_common.h>
#include <string.h>
#include <subpmic.h>
#include <trace.h>
#include <ufs_interface.h>
#include <ufs_platform.h>

#define LOCAL_TRACE 1

#define BOOT_TAG_BOOT_REASON    0x88610001
static uint32_t    boot_reason;

struct mmu_initial_mapping mmu_initial_mappings[] = {
    {
        .phys = MEMBASE,
        .virt = KERNEL_BASE,
        .size = MEMSIZE,
        .flags = 0,
        .name = "ram"
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

uint32_t platform_get_boot_reason(void)
{
    return boot_reason;
}

static void *load_device_tree(void)
{
    int32_t ret = NO_ERROR;
    void *dtb_addr;
    void *dtbo_ptr = NULL;
    uint32_t dtbo_size = 0;
    void *overlayed_dtb = NULL;

    dtb_addr = malloc(DTB_MAX_SIZE);
    ASSERT(dtb_addr);

    ret = load_partition("lk", "lk_main_dtb", dtb_addr, DTB_MAX_SIZE);
    if (ret < 0) {
        dprintf(CRITICAL, "Error load main dtb (%d)\n", ret);
        free(dtb_addr);
        return NULL;
    }

    ret = load_dtbo("dtbo", "dtbo", NULL, &dtbo_ptr, &dtbo_size);
    LTRACEF("dtbo_size=%u\n", dtbo_size);
    if (ret < 0) {
        dprintf(CRITICAL, "load_dtbo fail, fdt size(%d)\n", fdt_totalsize(dtb_addr));
        return dtb_addr;
    }

    ret = dtb_overlay((void *)dtb_addr, fdt_totalsize(dtb_addr),
                    dtbo_ptr, dtbo_size, &overlayed_dtb);
    if (ret != NO_ERROR)
        dprintf(CRITICAL, "DT overlay fail (%d)\n", ret);

    /*
     * compact the merged dt and reveal it's real size, the
     * intention behind is for ro operation in platform level
     * so any write operation later will fail to hint user
     */
    ret = fdt_pack(overlayed_dtb);
    ASSERT(ret >= 0);

    free(dtb_addr);
    free(dtbo_ptr);
    return overlayed_dtb;
}

void platform_init(void)
{
    void *fdt = NULL;

    if ((readl(GPIO_BANK_BASE) & TRAP_UFS_FIRST) == 0)
        ufs_init();

    /* do dconfig init */
    dconfig_initenv();

    /* do platform dtb init */
    fdt = load_device_tree();

    dbgtop_drm_init(fdt);

    mt_gpio_set_default(fdt);

    i2c_hw_init(fdt);
    subpmic_init(fdt);
    /* do display related init */
    leds_init(fdt);
    disp_init(fdt);
    /* probe fdt for coredump */
    scp_dts_probe(fdt);
    leds_on();
    mt_disp_show_boot_logo();
    drv_video_init();

    mt6315_device_register(SPMI_MASTER_0, SPMI_SLAVE_3);
    mt6315_device_register(SPMI_MASTER_0, SPMI_SLAVE_6);
    mt6315_device_register(SPMI_MASTER_0, SPMI_SLAVE_7);

    /* do devapc domain init */
    device_APC_dom_setup();

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
    platform_watchdog_set_enabled(true);
}

void platform_quiesce(void)
{
    platform_stop_timer();
    mtk_wdt_set_timeout_value(WDT_MAX_TIMEOUT);
    mtk_wdt_restart_timer();
}

static void pl_boottags_boot_reason_hook(struct boot_tag *tag)
{
    memcpy(&boot_reason, &tag->data, sizeof(boot_reason));
}
PL_BOOTTAGS_INIT_HOOK(pl_boottags_boot_reason_hook,
                      BOOT_TAG_BOOT_REASON,
                      pl_boottags_boot_reason_hook);
