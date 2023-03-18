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
#if MTK_WITH_COMMON_BOOTCTRL
#include <bootctrl.h>
#endif
#include <debug.h>
#include <fastboot.h>
#include <fastboot_oem_cmd.h>
#include <lib/bio.h>
#include <lib/log_store.h>
#ifdef WITH_PLATFORM_MEDIATEK_COMMON_CHARGER
#include <mtk_charger.h>
#endif
#include <platform.h>
#include <platform/aee_common.h>
#include <platform/log_store_lk.h>
#include <platform/mrdump_expdb.h>
#include <platform/mrdump_params.h>
#include <platform/usb2jtag.h>
#include <platform/wdt.h>
#include <sboot.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <trace.h>
#include <verified_boot_common.h>

#define LOCAL_TRACE 0

void cmd_getvar(const char *arg, void *data, unsigned int sz)
{
    struct fastboot_var *var;
    char response[MAX_RSP_SIZE];
    int n;

    if (!strcmp(arg, "all")) {
        for (var = varlist; var; var = var->next) {
            n = snprintf(response, MAX_RSP_SIZE, "\t%s: %s", var->name, var->value);
            if (n < 0)
                LTRACEF("snprintf error.\n");
            else
                fastboot_info(response);
        }
        fastboot_okay("Done!!");
        return;
    }
    for (var = varlist; var; var = var->next) {
        if (!strcmp(var->name, arg)) {
            fastboot_okay(var->value);
            return;
        }
    }
    fastboot_okay("");
}

void cmd_reboot(const char *arg, void *data, unsigned int sz)
{
    dprintf(ALWAYS, "rebooting the device\n");
    fastboot_okay("");
    /* PMIC cold reset */
    platform_halt(HALT_ACTION_REBOOT, HALT_REASON_POR);
}

void cmd_reboot_bootloader(const char *arg, void *data, unsigned int sz)
{
    dprintf(ALWAYS, "rebooting the device to bootloader\n");
    fastboot_okay("");
    mtk_wdt_set_restart_reason(MTK_WDT_NONRST2_BOOT_BOOTLOADER);
    platform_halt(HALT_ACTION_REBOOT, HALT_REASON_SW_RESET);
}

static void cmd_reboot_recovery_common(const char *reason)
{
    bdev_t *dev_misc = bio_open_by_label("misc");
    const unsigned int size = 2048; // size of bootloader_message
    char *pmisc_msg = NULL;

    if (!dev_misc) {
        fastboot_fail("open misc fail");
        return;
    }

    pmisc_msg = malloc(sizeof(uint8_t) * size);
    if (!pmisc_msg) {
        fastboot_fail("allocate misc msg fail");
        bio_close(dev_misc);
        return;
    }

    memset((uint8_t *)pmisc_msg, 0x0, size);

    strncpy(pmisc_msg, reason, strlen(reason));

    ssize_t written = bio_write(dev_misc, pmisc_msg, 0, size);

    bio_close(dev_misc);
    free(pmisc_msg);

    if (written <= 0)
        fastboot_fail("bio_write failed");
    else {
        fastboot_okay("");
        mtk_wdt_set_restart_reason(MTK_WDT_NONRST2_BOOT_RECOVERY);
        platform_halt(HALT_ACTION_REBOOT, HALT_REASON_SW_RESET);
    }
}

void cmd_reboot_fastboot(const char *arg, void *data, unsigned int sz)
{
    cmd_reboot_recovery_common("boot-fastboot");
}

void cmd_reboot_recovery(const char *arg, void *data, unsigned int sz)
{
    cmd_reboot_recovery_common("boot-recovery");
}

void cmd_set_active(const char *arg, void *data, unsigned int sz)
{
#if MTK_WITH_COMMON_BOOTCTRL >= 3
    if (!strcmp(arg, "a")) {
        if (!set_active_slot(BOOTCTRL_SUFFIX_A))
            fastboot_okay("");
        else
            fastboot_fail("set slot a failed!");
    } else if (!strcmp(arg, "b")) {
        if (!set_active_slot(BOOTCTRL_SUFFIX_B))
            fastboot_okay("");
        else
            fastboot_fail("set slot b failed!");
    } else
        fastboot_fail("command parameter is not allowed!");
#else
    fastboot_fail("set active slot: not supported!");
#endif
}

/* implement in target product's app */
__WEAK void notify_boot_linux(void)
{
}

void cmd_continue(const char *arg, void *data, unsigned int sz)
{
    fastboot_okay("");
    /* to exit fastboot_handler loop */
    fastboot_exit();
    /* notify app to boot linux */
    notify_boot_linux();
}

/* only user load use fastboot to control uart log */
#if !LK_DEBUGLEVEL
void cmd_uart_set(const char *arg, void *data, unsigned int sz)
{
    if (strncmp(arg, " on", 3) == 0) {
        dprintf(ALWAYS, "[FASTBOOT] oem p2u on!\n");
        set_pllk_config(UART_LOG, 1);
        fastboot_okay("");
    } else if (strncmp(arg, " off", 4) == 0) {
        dprintf(ALWAYS, "[FASTBOOT] oem p2u off!\n");
        set_pllk_config(UART_LOG, 0);
        fastboot_okay("");
    }
}

FASTBOOT_OEM_CMD_START(oem_p2u)
    .cmd_str = "oem p2u",
    .cmd_handler = cmd_uart_set,
    .allowed_when_security_on = true,
    .forbidden_when_lock_on = false,
FASTBOOT_OEM_CMD_END

#endif

#define LK_LOG_DUMP_SIZE (21)
void dump_pllk_log(const char *arg, void *data, unsigned int sz)
{
    size_t size = 0;
    u32 i;
    void *addr = NULL;
    char buf[LK_LOG_DUMP_SIZE];
    struct sram_log_header *pheader = NULL;

    pheader = get_logstore_header();
    addr = (void *)get_logstore_buffer();
    size = pheader->pl_lk_log.off_pl + pheader->pl_lk_log.sz_pl + pheader->pl_lk_log.sz_lk;

    memset(buf, 0, LK_LOG_DUMP_SIZE);
    for (i = 0; i < size / (LK_LOG_DUMP_SIZE-1); i++) {
        memcpy(buf, (void *)(addr + i*(LK_LOG_DUMP_SIZE-1)), (LK_LOG_DUMP_SIZE-1));
        fastboot_info(buf);
    }
    fastboot_okay("");
}

FASTBOOT_OEM_CMD_START(dump_pllk_log)
    .cmd_str = "oem dump_pllk_log",
    .cmd_handler = dump_pllk_log,
    .allowed_when_security_on = true,
    .forbidden_when_lock_on = false,
FASTBOOT_OEM_CMD_END


void cmd_printk_ratelimit(const char *arg, void *data, unsigned int sz)
{
     if (strncmp(arg, " on", 3) == 0) {
        dprintf(ALWAYS, "[FASTBOOT] oem printk-ratelimit on!\n");
        set_pllk_config(PRINTK_RATELIMIT, 1);
        fastboot_okay("");
    } else if (strncmp(arg, " off", 4) == 0) {
        dprintf(ALWAYS, "[FASTBOOT] oem printk-ratelimit off!\n");
        set_pllk_config(PRINTK_RATELIMIT, 0);
        fastboot_okay("");
    } else {
        dprintf(INFO, "UNKNOWN ARGUMENT\n");
        fastboot_info("UNKNOWN ARGUMENT\n");
    }
}

FASTBOOT_OEM_CMD_START(printk_ratelimit)
    .cmd_str = "oem printk-ratelimit",
    .cmd_handler = cmd_printk_ratelimit,
    .allowed_when_security_on = true,
    .forbidden_when_lock_on = true,
FASTBOOT_OEM_CMD_END

static void cmd_oem_usb2jtag(const char *arg, void *data, unsigned int sz)
{
    if (!strncmp(arg, " 1", strlen(" 1"))) {
        /* turn usb2jtag on */
        dprintf(ALWAYS, "[FASTBOOT] Enable USB2JTAG");
        fastboot_info("Enable USB2JTAG");
        set_usb2jtag(1);
    } else if (!strncmp(arg, " 0", strlen(" 0"))) {
        /* turn usb2jtag off */
        dprintf(ALWAYS, "[FASTBOOT] Disable USB2JTAG");
        fastboot_info("Disable USB2JTAG");
        set_usb2jtag(0);
    } else {
        dprintf(ALWAYS, "[FASTBOOT] Current USB2JAG setting: %s\n",
                        get_usb2jtag() ? "Enable" : "Disable");
    }

    dprintf(ALWAYS, "[FASTBOOT] USB2JTAG Done!\n");

    fastboot_okay("");
    /* normal reboot */
    mtk_wdt_set_restart_reason(0x0);
    platform_halt(HALT_ACTION_REBOOT, HALT_REASON_SW_RESET);
}

FASTBOOT_OEM_CMD_START(cmd_oem_usb2jtag)
    .cmd_str = "oem usb2jtag",
    .cmd_handler = cmd_oem_usb2jtag,
FASTBOOT_OEM_CMD_END

#ifdef WITH_PLATFORM_MEDIATEK_COMMON_CHARGER
void cmd_oem_off_mode_charge(const char *arg, void *data, unsigned int sz)
{
    if (!strncmp(arg, " 1", strlen(" 1"))) {
        //CHARGE MODE
        set_off_mode_charge_status(1);
        fastboot_publish("off-mode-charge", "1");
    } else if (!strncmp(arg, " 0", strlen(" 0"))) {
        //NORMAL MODE
        set_off_mode_charge_status(0);
        fastboot_publish("off-mode-charge", "0");
    } else
        fastboot_info("unknown argument");
    fastboot_okay("");
}

FASTBOOT_OEM_CMD_START(cmd_oem_off_mode_charge)
    .cmd_str = "oem off-mode-charge",
    .cmd_handler = cmd_oem_off_mode_charge,
    .allowed_when_security_on = true,
    .forbidden_when_lock_on = false,
FASTBOOT_OEM_CMD_END
#endif

#ifdef MTK_MRDUMP_ENABLE
/* register fastboot oem function */
static void cmd_oem_mrdump_out(const char *arg, void *data, unsigned int sz)
{
    const char *ptr = arg + 1;

    if (aee_check_enable() == AEE_ENABLE_FULL) {
        if (mrdump_set_output_device(ptr) != MRDUMP_DEV_UNKNOWN)
            fastboot_okay("");
        else
            fastboot_fail("MRDUMP output device setting failed.");
    } else
        fastboot_fail("MRDUMP is not support now.");

}

FASTBOOT_OEM_CMD_START(cmd_oem_mrdump_out_set)
    .cmd_str = "oem mrdump_out_set",
    .cmd_handler = cmd_oem_mrdump_out,
    .allowed_when_security_on = true,
    .forbidden_when_lock_on = false,
FASTBOOT_OEM_CMD_END

static void cmd_oem_mrdump_file_allocate(const char *arg, void *data, unsigned int sz)
{
    char response[MAX_RSP_SIZE];
    const char *ptr = arg + 1;
    bool ret = 1;
    int size_m;

    if (aee_check_enable() == AEE_ENABLE_FULL) {
        size_m = atoi(ptr);
        if (size_m < 0)
            size_m = 0;

        switch (size_m) {
        case 0:
            ret = mrdump_set_env("mrdump_allocate_size", "0");
            fastboot_info("mrdump_allocate_size -> 0");
            break;
        case 1:
            ret = mrdump_set_env("mrdump_allocate_size", "halfmem");
            fastboot_info("mrdump_allocate_size -> halfmem");
            break;
        case 2:
            ret = mrdump_set_env("mrdump_allocate_size", "fullmem");
            fastboot_info("mrdump_allocate_size -> fullmem");
            break;
        default:
            if (size_m >= 256)
                ret = mrdump_set_env("mrdump_allocate_size", (char *)ptr);
            int n = snprintf(response, MAX_RSP_SIZE,
                    "mrdump_allocate_size -> :%s", ptr);
            if (n < 0) {
                strncpy(response, "unknown error", sizeof(response) - 1);
                response[sizeof(response) - 1] = '\0';
            }
            fastboot_info(response);
            break;
        }

        if (ret == 0)
            fastboot_okay("");
        else
            fastboot_fail("mrdump_allocate_size not set.");
    } else
        fastboot_fail("MRDUMP is not support now.");
}

FASTBOOT_OEM_CMD_START(cmd_oem_mrdump_file_allocate)
    .cmd_str = "oem mrdump_fallocate",
    .cmd_handler = cmd_oem_mrdump_file_allocate,
    .allowed_when_security_on = true,
    .forbidden_when_lock_on = false,
FASTBOOT_OEM_CMD_END

static void cmd_oem_mrdump_chkimg(const char *arg, void *data, unsigned int sz)
{
    int status = mrdump_check_enable();

    switch (status) {
    case MRDUMP_RUNTIME_ENABLE:
        fastboot_info("mrdump runtime enabled");
        break;
    case MRDUMP_RUNTIME_DISABLE:
        fastboot_info("mrdump runtime disabled");
        break;
    case MRDUMP_SUCCESS_ENABLE:
        fastboot_info("mrdump successfully enabled");
        break;
    case MRDUMP_ALWAYS_ENABLE:
        fastboot_info("mrdump always enabled");
        break;
    case MRDUMP_ALWAYS_DISABLE:
        fastboot_info("mrdump always disabled");
        break;
    case MRDUMP_NO_SOCID_FROM_CHIP:
        fastboot_info("unable to get socid from chip");
        break;
    case MRDUMP_SEC_IMG_AUTH_INIT_ERROR:
        fastboot_info("mrdump dconfig image auth init failed");
        break;
    case MRDUMP_DCONFIG_MALLOC_ERROR:
        fastboot_info("mrdump dconfig not enough memory");
        break;
    case MRDUMP_MBOOT_LOAD_PART_ERROR:
        fastboot_info("mrdump dconfig partition_read failed");
        break;
    case MRDUMP_DCONFIG_IMG_VERIFY_ERROR:
        fastboot_info("mrdump dconfig image verify failed");
        break;
    case MRDUMP_DCONFIG_SOCID_CERT_ERROR:
        fastboot_info("unable to get socid from certed image");
        break;
    case MRDUMP_DCNFIG_SOCID_MISMATCH:
        fastboot_info("socid mismatched");
        break;
    case MRDUMP_NOT_VALID_IMG:
        fastboot_info("not a valid mrdump enabled image");
        break;
    default:
        fastboot_info("unknown error");
        break;
    }
    fastboot_okay("");
}

FASTBOOT_OEM_CMD_START(cmd_oem_mrdump_chkimg)
    .cmd_str = "oem mrdump_chkimg",
    .cmd_handler = cmd_oem_mrdump_chkimg,
    .allowed_when_security_on = true,
    .forbidden_when_lock_on = false,
FASTBOOT_OEM_CMD_END
#endif

static char int_to_str(char c)
{
    if (c <= 9)
        return (c + 48);

    return (c - 9) + 64;
}

void cmd_oem_get_socid(const char *arg, void *data, unsigned int sz)
{
#define MAX_SOCID_MSG_SZ (128)
    uint32_t ret = 0;
    uint32_t i, j;
    char msg[MAX_SOCID_MSG_SZ] = {0};
    char socid_arr[SOC_ID_LEN] = {0};
    char socid_first_half[SOC_ID_LEN + 1] = {0};
    char socid_second_half[SOC_ID_LEN + 1] = {0};

    ret = sec_get_socid_from_pl_boottags((uint8_t *)socid_arr, SOC_ID_LEN);
    if (ret)
        goto end;

    /* separate 64 characters into two lines in order to fit fastboot
     * message length.
     */
    for (i = 0, j = 0; i < SOC_ID_LEN && j < (SOC_ID_LEN / 2); i += 2, j++) {
        socid_first_half[i] = int_to_str((socid_arr[j] & 0xF0) >> 4);
        socid_first_half[i + 1] = int_to_str(socid_arr[j] & 0x0F);
    }
    socid_first_half[SOC_ID_LEN] = '\0';

    for (i = 0, j = (SOC_ID_LEN / 2); i < SOC_ID_LEN && j < SOC_ID_LEN; i += 2, j++) {
        socid_second_half[i] = int_to_str((socid_arr[j] & 0xF0) >> 4);
        socid_second_half[i + 1] = int_to_str(socid_arr[j] & 0x0F);
    }
    socid_second_half[SOC_ID_LEN] = '\0';

    fastboot_info("dump socid...");
    fastboot_info(socid_first_half);
    fastboot_info(socid_second_half);
    fastboot_info("finish dump");

end:
    if (ret) {
        if (snprintf(msg, MAX_SOCID_MSG_SZ, "\nget_socid failed - Err:0x%x\n", ret) >= 0) {
            msg[MAX_SOCID_MSG_SZ - 1] = '\0';
            fastboot_fail(msg);
        } else {
            fastboot_fail("unknown error\n");
        }
    } else
        fastboot_okay("");
    return;
}

FASTBOOT_OEM_CMD_START(cmd_oem_get_socid)
    .cmd_str = "oem get_socid",
    .cmd_handler = cmd_oem_get_socid,
    .allowed_when_security_on = true,
    .forbidden_when_lock_on = true,
FASTBOOT_OEM_CMD_END

void cmd_oem_sd_init(const char *arg, void *data, unsigned int sz)
{
    sdcard_init();
    fastboot_okay("");
}

FASTBOOT_OEM_CMD_START(cmd_oem_sd_init)
    .cmd_str = "oem sd_init",
    .cmd_handler = cmd_oem_sd_init,
    .allowed_when_security_on = true,
    .forbidden_when_lock_on = false,
FASTBOOT_OEM_CMD_END

void cmd_oem_bio_dump(const char *arg, void *data, unsigned int sz)
{
    bio_dump_devices();
    fastboot_okay("");
}

FASTBOOT_OEM_CMD_START(cmd_oem_bio_dump)
    .cmd_str = "oem bio_dump",
    .cmd_handler = cmd_oem_bio_dump,
    .allowed_when_security_on = true,
    .forbidden_when_lock_on = false,
FASTBOOT_OEM_CMD_END
