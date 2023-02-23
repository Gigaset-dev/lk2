/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <app/dm_verity_status.h>
#include <avb_slot_verify.h>
#include <debug.h>
#include <platform.h>
#include <pmic_keys.h>
#if (WITH_PLATFORM_MEDIATEK_COMMON_VIDEO)
#include <platform/video.h>
#endif
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <trace.h>

#define DM_VERITY_ERROR_WAIT_TIME   5000

#define udelay(x)   spin(x)
#define mdelay(x)   udelay((x) * 1000)

unsigned int g_verity_mode = VERITY_MODE_RESTART;

static void show_dm_verity_error(void)
{
#if (MTK_SECURITY_SW_SUPPORT)
    unsigned int wait_time = 0;

#if (WITH_PLATFORM_MEDIATEK_COMMON_VIDEO)
    video_clear_screen();
    video_set_cursor(video_get_rows() / 2, 0);
    video_printf("dm-verity corruption\n\n");
    video_printf("Your device is corrupt.\n");
    video_printf("It can't be trusted and may not work properly.\n");
    video_printf("Press power button to continue.\n");
    video_printf("Or, device will power off in %ds\n", DM_VERITY_ERROR_WAIT_TIME / 1000);
#else
    dprintf(CRITICAL, "dm-verity corruption\n\n");
    dprintf(CRITICAL, "Your device is corrupt.\n");
    dprintf(CRITICAL, "It can't be trusted and may not work properly.\n");
    dprintf(CRITICAL, "Press power button to continue.\n");
    dprintf(CRITICAL, "Or, device will power off in %ds\n", DM_VERITY_ERROR_WAIT_TIME / 1000);
#endif

    while (wait_time < DM_VERITY_ERROR_WAIT_TIME) {
        if (is_pmic_key_pressed(POWER_KEY))
            return;
        mdelay(100);
        wait_time += 100;
    }
    platform_halt(HALT_ACTION_SHUTDOWN, HALT_REASON_SW_WATCHDOG);
#endif
}

void dm_verity_handler(AvbHashtreeErrorMode hashtree_error_mode)
{
#if (MTK_SECURITY_SW_SUPPORT)
    uint32_t status = 0;

    get_dm_verity_status(&status);

    if ((status == DM_VERITY_GENERAL_ERROR) ||
        (hashtree_error_mode == AVB_HASHTREE_ERROR_MODE_EIO))
        show_dm_verity_error();
#endif
}

void get_hash_tree_error_mode(uint32_t *hashtree_error_mode)
{
#if (MTK_SECURITY_SW_SUPPORT)
    *hashtree_error_mode = AVB_HASHTREE_ERROR_MODE_MANAGED_RESTART_AND_EIO;
#else
    *hashtree_error_mode = AVB_HASHTREE_ERROR_MODE_RESTART_AND_INVALIDATE;
#endif
}
