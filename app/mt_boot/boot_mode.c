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
#include <app/boot_ui.h>
#ifdef WITH_PLATFORM_MEDIATEK_COMMON_CHARGER
#include <mtk_charger.h>
#endif
#include <lib/kcmdline.h>
#ifdef WITH_PLATFORM_MEDIATEK_COMMON_KEYPAD
#include <mtk_key.h>
#endif
#include <platform/boot_mode.h>
#include <platform/mboot_params.h>
#include <platform/wdt.h>
#include <platform_mtk.h>
#include <string.h>
#include <trace.h>

#define LOCAL_TRACE 2

static u8 g_boot_mode = NORMAL_BOOT;

/* It's NOT KPOC boot if the boot_reason is in the list. */
uint32_t kpoc_exclude_list[] = {
    BR_POWER_KEY,
    BR_WDT_BY_PASS_PWK,
    BR_2SEC_REBOOT,
    BR_KERNEL_PANIC,
    BR_WDT_SW,
    BR_WDT_HW,
    BR_POWER_EXC,
    BR_LONG_POWKEY,
    BR_POWER_LOSS,
    BR_REBOOT_EXCEPTION
};

u8 get_boot_mode(void)
{
    return g_boot_mode;
}

static u8 pl_mode_check(void)
{
    u8 boot_mode = platform_get_boot_mode();

    if (boot_mode == META_BOOT)
        return META_BOOT;
    else if (boot_mode == FACTORY_BOOT)
        return FACTORY_BOOT;
    else if (boot_mode == ALARM_BOOT)
        return ALARM_BOOT;
    return 0;
}

static bool recovery_check(void)
{
    if (mtk_wdt_get_restart_reason() == MTK_WDT_NONRST2_BOOT_RECOVERY)
        return true;
    return false;
}

static bool fastboot_check(void)
{
    if (mtk_wdt_get_restart_reason() == MTK_WDT_NONRST2_BOOT_BOOTLOADER)
        return true;
    return false;
}

static bool factory_key_check(void)
{
#ifdef WITH_PLATFORM_MEDIATEK_COMMON_KEYPAD
    if (detect_key(FACTORY_KEY))
        return true;
#endif
    return false;
}

static bool bootmenu_check(void)
{
#ifdef WITH_PLATFORM_MEDIATEK_COMMON_KEYPAD
    if (detect_key(BOOT_MENU_KEY))
        return true;
#endif
    return false;
}

static bool kpoc_check(void)
{
    uint32_t boot_reason = platform_get_boot_reason();
    uint32_t i;
#ifdef WITH_PLATFORM_MEDIATEK_COMMON_CHARGER
    int off_mode_status = 1;

    off_mode_status = get_off_mode_charge_status();
    if (!off_mode_status)
        return false;
#endif

    for (i = 0; i < countof(kpoc_exclude_list); i++) {
        if (boot_reason == kpoc_exclude_list[i])
            return false;
    }

    LTRACEF("boot_readon = %d\n", boot_reason);
    /* KPOC boot */
    return true;
}

void boot_mode_check(void)
{
    u8 pl_mode = pl_mode_check();

    if (pl_mode)
        g_boot_mode = pl_mode;
    else if (recovery_check())
        g_boot_mode = RECOVERY_BOOT;
    else if (fastboot_check())
        g_boot_mode = FASTBOOT_BOOT;
    else if (factory_key_check())
        g_boot_mode = FACTORY_BOOT;
    else if (bootmenu_check())
        g_boot_mode = boot_menu_select();
    else if (kpoc_check())
        g_boot_mode = KERNEL_POWER_OFF_CHARGING_BOOT;
}

struct type_map {
    uint32_t type;
    char name[40];
};

static struct type_map exp_type[] = {
    {AEE_EXP_TYPE_HWT,          "Watchdog"},
    {AEE_EXP_TYPE_KE,           "kernel_panic"},
    {AEE_EXP_TYPE_NESTED_PANIC, "kernel_panic"},
    {AEE_EXP_TYPE_HANG_DETECT,  "hang_detect"},
    {AEE_EXP_TYPE_SMART_RESET,  "mrdump"},
    {AEE_EXP_TYPE_BL33_CRASH,   "lk_crash"},
};

static struct type_map boot_type[] = {
    {BR_2SEC_REBOOT,      "2sec_reboot"},
    {BR_USB,              "usb"},
    {BR_POWER_EXC,        "ocp"},
    {BR_LONG_POWKEY,      "reboot_longkey"},
    {BR_RTC,              "rtc"},
    {BR_POWER_LOSS,       "power_loss"},
    {BR_WDT,              "wdt"},
    {BR_TOOL_BY_PASS_PWK, "tool_by_pass_pwk"},
    {BR_WDT_SW,           "wdt_sw"},
    {BR_WDT_HW,           "wdt_hw"},
    {BR_UNKNOWN,          "unknown_reboot"},
    {BR_POWER_KEY,        "PowerKey"},
    {BR_REBOOT_EXCEPTION, "RebootException"},
};

static struct type_map wdt_type[] = {
    {0x01,  "HW_reboot"},
    {0x08,  "SPM_Thermal_reboot"},
    {0x10,  "SPM_reboot"},
    {0x20,  "Thermal_reboot"},
    {0x80,  "security_reboot"},
    {0x400, "SSPM_reboot"},
    {0x800, "PMIC_cold_reboot"},
};

static char *get_bootreason_str(struct type_map *table,
    u8 table_size, uint32_t type, bool mask)
{
    uint32_t i;
    struct type_map *entry = NULL;

    for (i = 0; i < table_size; i++) {
        entry = table + i;
        if (mask == true) {
            if ((type & entry->type) == entry->type)
                return entry->name;
        } else if (type == entry->type)
            return entry->name;
    }

    return NULL;
}

#define CMDLINE_LEN 100
void bootreason_cmdline(void)
{
    unsigned int rc_wdt_status;
    uint32_t pl_boot_reason = platform_get_boot_reason();
    unsigned int rc_exp_type;
    char bootreason[CMDLINE_LEN] = "androidboot.bootreason=";
    char *tmp = NULL;

    if (pl_boot_reason == BR_WDT_BY_PASS_PWK) {
    /*by pass mode have more detail reboot reason*/
        if (mboot_params_get_wdt_status(&rc_wdt_status)
            && mboot_params_get_exp_type(&rc_exp_type)) {
            LTRACEF("rc_wdt_status = %d, rc_exp_type = %d\n", rc_wdt_status, rc_exp_type);
            tmp = get_bootreason_str(exp_type, countof(exp_type), rc_exp_type, false);
            if (tmp != NULL) {
                kcmdline_append(strncat(bootreason, tmp, CMDLINE_LEN - strlen(tmp) - 1));
                return;
            }

            tmp = get_bootreason_str(wdt_type, countof(wdt_type), rc_wdt_status, true);
            if (tmp != NULL) {
                kcmdline_append(strncat(bootreason, tmp, CMDLINE_LEN - strlen(tmp) - 1));
                return;
            }
        }
    } else {
        tmp = get_bootreason_str(boot_type, countof(boot_type), pl_boot_reason, false);
        if (tmp != NULL) {
            kcmdline_append(strncat(bootreason, tmp, CMDLINE_LEN - strlen(tmp) - 1));
            return;
        }
    }

    /* default is soft reboot*/
    kcmdline_append("androidboot.bootreason=reboot");
 }

