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
#include <app.h>
#include <dconfig.h>
#include <debug.h>
#include <lib/kcmdline.h>
#include <lib/watchdog.h>
#ifdef WITH_PLATFORM_MEDIATEK_COMMON_KEYPAD
#include <mtk_key.h>
#endif
#ifdef WITH_PLATFORM_MEDIATEK_COMMON_LOGO
#include <mt_logo.h>
#endif
#include <platform/boot_mode.h>
#ifdef WITH_PLATFORM_MEDIATEK_COMMON_VIDEO
#include <platform/video.h>
#endif
#include <string.h>
#include <trace.h>

#define LOCAL_TRACE 2

typedef void (*entry_cb)(void);

struct boot_mode_menu {
    u8 boot_mode;
    const char *item_text;
    entry_cb callback;
    bool default_en;
};

void ftrace_cb(void)
{
    kcmdline_append("androidboot.boot_trace=1");
    kcmdline_append("trace_event=sched_switch,cpu_frequency,sched_wakeup,workqueue,module_load");
    kcmdline_append("log_buf_len=4M printk.devkmsg=on");
}

void initcall_cb(void)
{
    kcmdline_append("initcall_debug=1 log_buf_len=4M printk.devkmsg=on");
}

struct boot_mode_menu ui_entry[] = {
    {RECOVERY_BOOT, "[Recovery    Mode]", NULL, true},
    {FASTBOOT_BOOT, "[Fastboot    Mode]", NULL, true},
    {NORMAL_BOOT,   "[Normal      Mode]", NULL, true},
    {NORMAL_BOOT,   "[Normal      Boot +ftrace]", ftrace_cb, false},
    {NORMAL_BOOT,   "[Normal      Boot +initcall]", initcall_cb, false},
};

static bool activate_menu(void)
{
    const char *params = dconfig_getenv("enhance_menu");
    bool enable_ext_menu = false;
    bool ret = true;
    unsigned int i;

#if (LK_DEBUGLEVEL >= 1)
    enable_ext_menu = true;
#endif

    if (params != NULL) {
        LTRACEF("[dconfig]enhance_menu: %s\n", params);
        if (strcmp(params, "enable") == 0)
            enable_ext_menu = true;
        else if (strcmp(params, "disable") == 0)
            enable_ext_menu = false;
    }

    for (i = 0; i < countof(ui_entry); i++) {
        if (enable_ext_menu) {
            if (ui_entry[i].default_en == false)
                ui_entry[i].default_en = true;
        } else {
            if (ui_entry[i].default_en)
                break;
            if (i == (countof(ui_entry)-1)) {
                LTRACEF("[boot menu]warn: Items are all off.\n");
                ret = false;
            }
        }
    }
    return ret;
}

static void update_menu(unsigned int index)
{
#ifdef WITH_PLATFORM_MEDIATEK_COMMON_VIDEO
    #define LEN 100
    const char *title_msg = "Select Boot Mode:\n[VOLUME_UP to select.  VOLUME_DOWN is OK.]\n\n";
    char str_buf[LEN];
    unsigned int i, length;

    video_clear_screen();
    video_set_cursor(video_get_rows()/2, 0);
    video_printf(title_msg);

    for (i = 0; i < countof(ui_entry); i++) {
        if (!ui_entry[i].default_en)
            continue;
        memset(str_buf, 0, LEN);
        length = strlen(ui_entry[i].item_text);
        snprintf(str_buf, length+1, "%s", ui_entry[i].item_text);
        if (i == index) {
            LTRACEF("Switch to %s mode.\n", str_buf);
            sprintf(str_buf+length, "     <<==\n");
        } else
            sprintf(str_buf+length, "         \n");
        video_printf(str_buf);
    }
#endif
}

/* loop in key detection, if boot mode is selected, return the boot mode */
u8 boot_menu_select(void)
{
#ifdef WITH_PLATFORM_MEDIATEK_COMMON_KEYPAD
    unsigned int select_idx = 0;
    unsigned int i;

    if (!activate_menu())
        return NORMAL_BOOT;

#ifdef WITH_PLATFORM_MEDIATEK_COMMON_LOGO
    mt_disp_clear_screen(0);
#endif
    update_menu(0);
    platform_watchdog_set_enabled(false);

    while (1) {
        if (detect_key(PMIC_RST_KEY)) {
            select_idx += 1;
            for (i = 0; i < countof(ui_entry); i++) {
                if (select_idx >= countof(ui_entry))
                    select_idx = 0;

                if (ui_entry[select_idx].default_en)
                    break;
                else
                    select_idx += 1;
            }
            update_menu(select_idx);
            spin(300*1000); //delay 300ms
        } else if (detect_key(MENU_OK_KEY))
            break;
    }

    if (ui_entry[select_idx].callback)
        ui_entry[select_idx].callback();

    platform_watchdog_set_enabled(true);
    LTRACEF("Boot mode:%s is selected!\n", ui_entry[select_idx].item_text);
    return ui_entry[select_idx].boot_mode;
#else
    return NORMAL_BOOT;
#endif
}
