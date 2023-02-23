/*
 * Copyright (c) 2009, Google Inc.
 * Copyright (c) 2019, MediaTek Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#include <app.h>
#if MTK_WITH_COMMON_BOOTCTRL
#include <bootctrl.h>
#endif
#include <compiler.h>
#if WITH_APP_FASTBOOT_DEVICE_LOCK
#include <device_lock.h>
#endif
#include <dev/udc.h>
#include <errno.h>
#include <fastboot.h>
#include <fastboot_oem_cmd.h>
#ifdef WITH_APP_FASTBOOT_FLASH
#include <flash_cmd.h>
#endif
#include <kernel/event.h>
#include <kernel/timer.h>
#include <lib/bio.h>
#if WITH_APP_FASTBOOT_DEVICE_LOCK
#include <lock_state.h>
#endif
#include <malloc.h>
#ifdef WITH_PLATFORM_MEDIATEK_COMMON_CHARGER
#include <mtk_charger.h>
#endif
#include <platform/boot_mode.h>
#include <platform_mtk.h>
#include <platform/wdt.h>
#include <string.h>
#include <sys_commands.h>
#include <trace.h>
#include <variables.h>

#define LOCAL_TRACE 2

/* would be replaced. ex: in cust_usb.h */
#ifndef USB_VENDORID
#define USB_VENDORID            (0x0E8D)
#define USB_PRODUCTID           (0x201C)
#define USB_VERSIONID           (0x0100)
#define USB_MANUFACTURER        "MediaTek"
#define USB_PRODUCT_NAME        "Android"
#endif

#ifndef CUSTOM_FASTBOOT_BUF_SIZE
#define BUF_SIZE                (0x4000000)
#else
#define BUF_SIZE CUSTOM_FASTBOOT_BUF_SIZE
#endif

static event_t enter_fastboot_event = EVENT_INITIAL_VALUE(enter_fastboot_event, false, 0);
static timer_t wdt_timer;

#if (WITH_PLATFORM_MEDIATEK_COMMON_MD && LK_AS_BL33)
#define MD_VER_LEN              (64)
char radio_version[MD_VER_LEN + 1] = {0};

void get_md_version(void)
{
    int len = 0;
    int ret = 0;

    dprintf(INFO, "Load modem image in fastboot mode to get md version\n");
    ret = platform_load_modem(NULL, FASTBOOT_BOOT);
    if (ret) {
        dprintf(CRITICAL, "Load modem for md version failed, ret: %d\n", ret);
        return;
    }

    len = ccci_get_md_version(radio_version, MD_VER_LEN);
    if (len <= 0) {
        if (snprintf(radio_version, MD_VER_LEN, "N/A") < 0)
            dprintf(CRITICAL, "snprintf error in %s: %d\n", __FILE__, __LINE__);
    }
}
#endif

/* for target product to implement its own serial number */
__WEAK char *get_serialno(void)
{
    /* fastboot use 13 bytes as default, max is 19 */
    static const char *sn_buf = "0123456789ABCDEF";

    return (char *)sn_buf;
}

void notify_enter_fastboot(void)
{
    LTRACEF("Notify enter fastboot.\n");
    event_signal(&enter_fastboot_event, false);
}

static void wait_for_enter_fastboot(void)
{
    LTRACEF("Wait for enter fastboot event...\n");
    event_wait(&enter_fastboot_event);
    event_unsignal(&enter_fastboot_event);
    LTRACEF("Ready to enter fastboot...\n");
}

static void register_oem_commands(void)
{
    extern const struct fastboot_oem_cmd_descriptor __fb_oem_cmd_start[];
    extern const struct fastboot_oem_cmd_descriptor __fb_oem_cmd_end[];

    const struct fastboot_oem_cmd_descriptor *cmd;

    for (cmd = __fb_oem_cmd_start; cmd != __fb_oem_cmd_end; cmd++) {
        if (cmd->cmd_handler)
            fastboot_register(cmd->cmd_str, cmd->cmd_handler,
                    cmd->allowed_when_security_on, cmd->forbidden_when_lock_on);
   }
}

static void publish_attributes(void)
{
    char buffer_size[20];
    int tmp;

    if (sprintf(buffer_size, "0x%x", BUF_SIZE) < 0)
        assert(0);

    fastboot_publish("version", "0.5");
    fastboot_publish("version-bootloader", get_lk_version());
    fastboot_publish("max-download-size", buffer_size);
    fastboot_publish("product", PROJECT);
    fastboot_publish("serialno", get_serialno());
    fastboot_publish("is-userspace", "no");
#if (WITH_PLATFORM_MEDIATEK_COMMON_MD && LK_AS_BL33)
    fastboot_publish("version-baseband", (const char *)radio_version);
#endif
    publish_partition_info();

#if WITH_APP_FASTBOOT_DEVICE_LOCK
    update_secure_status_var();
#endif

#if MTK_WITH_COMMON_BOOTCTRL >= 3
    if (is_ab_enable()) {
        int result = 0;

        fastboot_publish("current-slot",
            !strncmp(get_suffix(), BOOTCTRL_SUFFIX_A, strlen(BOOTCTRL_SUFFIX_A)) ? "a" : "b");

        result = sprintf(buffer_size, "%d", BOOTCTRL_NUM_SLOTS);
        if (result < 0 && errno != 0)
            LTRACEF("Get BOOTCTRL_NUM_SLOTS fail\n");
        else
            fastboot_publish("slot-count", buffer_size);

        tmp = get_bctrl_mdata(0, BOOTCTRL_UNBOOT);
        if (tmp >= 0)
            fastboot_publish("slot-unbootable:a", !tmp ? "no" : "yes");

        tmp = get_bctrl_mdata(1, BOOTCTRL_UNBOOT);
        if (tmp >= 0)
            fastboot_publish("slot-unbootable:b", !tmp ? "no" : "yes");

        tmp = get_bctrl_mdata(0, BOOTCTRL_SUCCESS);
        if (tmp >= 0)
            fastboot_publish("slot-successful:a", !tmp ? "no" : "yes");

        tmp = get_bctrl_mdata(1, BOOTCTRL_SUCCESS);
        if (tmp >= 0)
            fastboot_publish("slot-successful:b", !tmp ? "no" : "yes");

        tmp = get_bctrl_mdata(0, BOOTCTRL_RETRY);
        if (tmp >= 0) {
            result = sprintf(buffer_size, "%d", tmp);
            if (result < 0 && errno != 0)
                LTRACEF("Get slot-retry-count:a fail\n");
            else
                fastboot_publish("slot-retry-count:a", buffer_size);
        }

        tmp = get_bctrl_mdata(1, BOOTCTRL_RETRY);
        if (tmp >= 0) {
            result = sprintf(buffer_size, "%d", tmp);
            if (result < 0 && errno != 0)
                LTRACEF("Get slot-retry-count:b fail\n");
            else
                fastboot_publish("slot-retry-count:b", buffer_size);
        }
    } else
        fastboot_publish("slot-count", "0");
#endif

#ifdef WITH_PLATFORM_MEDIATEK_COMMON_CHARGER
    int off_mode_status = 1;

    off_mode_status = get_off_mode_charge_status();
    if ((off_mode_status == 0) || (off_mode_status == 1))
        fastboot_publish("off-mode-charge", off_mode_status ? "1" : "0");
    else
        LTRACEF("off mode charge status is out of boundary\n");
#endif
}

static void unpublish_attributes(void)
{
    fastboot_unpublish_all();
}

static void register_commands(void)
{
    fastboot_register("getvar:", cmd_getvar, true, false);
    fastboot_register("reboot", cmd_reboot, true, false);
    fastboot_register("reboot-bootloader", cmd_reboot_bootloader, true, false);
    fastboot_register("reboot-fastboot", cmd_reboot_fastboot, true, false);
    fastboot_register("reboot-recovery", cmd_reboot_recovery, true, false);
    fastboot_register("continue", cmd_continue, false, false);
    fastboot_register("set_active:", cmd_set_active, true, false);
#ifdef WITH_APP_FASTBOOT_FLASH
    register_flash_commands();
#endif
    register_oem_commands();
}

static void unregister_commands(void)
{
    fastboot_unregister_all();
}

static void fastboot_entry(const struct app_descriptor *app, void *args)
{
    void *buf;

    wait_for_enter_fastboot();

    timer_initialize(&wdt_timer);
    timer_set_periodic(&wdt_timer, 5000, (timer_callback)mtk_wdt_restart_timer, NULL);

    bio_dump_devices();
    register_commands();
#if (WITH_PLATFORM_MEDIATEK_COMMON_MD && LK_AS_BL33)
    get_md_version();
#endif
    publish_attributes();

    buf = malloc(BUF_SIZE);
    if (buf) {
        fastboot_init(buf, BUF_SIZE);
        free(buf);
    } else {
        LTRACEF("malloc 0x%x bytes failed.\n", BUF_SIZE);
    }

    unregister_commands();
    unpublish_attributes();
}

APP_START(fastboot)
    .entry = fastboot_entry,
APP_END
