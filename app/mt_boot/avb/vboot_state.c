/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <app/vboot_state.h>
#include <debug.h>
#include <lib/kcmdline.h>
#if (WITH_PLATFORM_MEDIATEK_COMMON_VIDEO)
#include <platform/video.h>
#endif
#include <platform/wdt.h>
#include <stdint.h>
#include <string.h>
#include <trace.h>

#define udelay(x) spin(x)
#define mdelay(x) udelay((x) * 1000)

static uint32_t g_boot_state = BOOT_STATE_ORANGE;

void print_boot_state(void)
{
    switch (g_boot_state) {
    case BOOT_STATE_ORANGE:
        dprintf(INFO, "boot state: orange\n");
        break;
    case BOOT_STATE_YELLOW:
        dprintf(INFO, "boot state: yellow\n");
        break;
    case BOOT_STATE_RED:
        dprintf(INFO, "boot state: red\n");
        break;
    case BOOT_STATE_GREEN:
        dprintf(INFO, "boot state: green\n");
        break;
    default:
        dprintf(INFO, "boot state: unknown\n");
        break;
    }
}

void set_boot_state_to_cmdline(void)
{
    switch (g_boot_state) {
    case BOOT_STATE_ORANGE:
        kcmdline_append("androidboot.verifiedbootstate=orange");
        break;
    case BOOT_STATE_YELLOW:
        kcmdline_append("androidboot.verifiedbootstate=yellow");
        break;
    case BOOT_STATE_RED:
        kcmdline_append("androidboot.verifiedbootstate=red");
        break;
    case BOOT_STATE_GREEN:
        kcmdline_append("androidboot.verifiedbootstate=green");
        break;
    default:
        break;
    }
}

uint32_t get_boot_state(void)
{
    return g_boot_state;
}

void set_boot_state(uint32_t boot_state)
{
    g_boot_state = boot_state;
}

static int orange_state_warning(void)
{
    int ret = 0;

#if (WITH_PLATFORM_MEDIATEK_COMMON_VIDEO)
    video_clear_screen();
    video_set_cursor(video_get_rows() / 2, 0);
    video_printf("Orange State\n\n");
    video_printf("OS Not Being Verified Or Custom OS. Dismiss After 5 Seconds.\n");
#endif
    mtk_wdt_restart_timer();
    mdelay(5000);
    mtk_wdt_restart_timer();

    return ret;
}

static int red_state_warning(const char *img_name)
{
    int ret = 0;
    char show_img_name[MAX_IMG_NAME_LEN] = {'\0'};
    int copy_len = 0;

    if (img_name != NULL) {
        copy_len = strlen(img_name);
        if (copy_len > 0) {
            if (copy_len >= MAX_IMG_NAME_LEN)
                copy_len = MAX_IMG_NAME_LEN - 1;
            if (img_name[0] == '/') {
                memcpy(show_img_name, (img_name + 1), copy_len - 1);
                show_img_name[copy_len - 1] = '\0';
            } else {
                memcpy(show_img_name, img_name, copy_len);
                show_img_name[copy_len] = '\0';
            }
        }
    }

#if (WITH_PLATFORM_MEDIATEK_COMMON_VIDEO)
    video_clear_screen();
    video_set_cursor(video_get_rows() / 2, 0);
    video_printf("Red State\n\n");
    video_printf("Device Cannot Boot. Enter Repair Mode.");
    video_printf("Dismiss After 5 Seconds.\n");
#endif
    mtk_wdt_restart_timer();
    mdelay(5000);
    mtk_wdt_restart_timer();

    return ret;
}

int show_warning(const char *img_name)
{
    int ret = 0;

    switch (g_boot_state) {
    case BOOT_STATE_ORANGE:
        ret = orange_state_warning();
        break;
    case BOOT_STATE_YELLOW:
        /* Fall into red state since yellow state is not supported */
    case BOOT_STATE_RED:
        if (img_name == NULL)
            ret = red_state_warning("UNKNOWN");
        else
            ret = red_state_warning(img_name);

        break;
    case BOOT_STATE_GREEN:
    default:
        break;
    }

    return ret;
}
