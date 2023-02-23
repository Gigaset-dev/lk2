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
#include <debug.h>
#include <lk/init.h>
#include <malloc.h>
#include <platform.h>
#include <platform/addressmap.h>
#include <platform/aee_common.h>
#include <platform/mboot_params.h>
#include <platform/reg.h>
#include <platform_mtk.h>
#ifdef WITH_PLATFORM_MEDIATEK_COMMON_VIDEO
#include <platform/video.h>
#endif
#include <platform/wdt.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <trace.h>

#include "aee.h"
#include "KEHeader.h"
#include "mrdump_private.h"

#define LOCAL_TRACE 0

static void exit_aee_init(void)
{
    /* unmap aee_debug_kinfo */
    aee_debug_kinfo_unmap();
    /* PMIC cold reset*/
    platform_halt(HALT_ACTION_REBOOT, HALT_REASON_POR);
}

static void aee_init(const struct app_descriptor *app)
{
    LTRACEF_LEVEL(ALWAYS, "Trigger exception flow\n");
    /* boot args info */
    LTRACEF_LEVEL(ALWAYS, "boot_reason(%d) boot_mode(%d)\n",
                  platform_get_boot_reason(),
                  platform_get_boot_mode());
    LTRACEF_LEVEL(ALWAYS, "ddr_reserve_ready(%d) ddr_reserve_success(%d)\n",
                  aee_ddr_reserve_enable(), aee_ddr_reserve_success());

    /* map aee_debug_kinfo */
    if (aee_debug_kinfo_map()) {
        LTRACEF_LEVEL(ALWAYS, "aee_debug_kinfo map fail\n");
        exit_aee_init();
    }

    switch (aee_check_enable()) {
    case AEE_ENABLE_NO:
        LTRACEF_LEVEL(ALWAYS, "aee disabled but jump into aee lk?\n");
        exit_aee_init();
        break;
    case AEE_ENABLE_MINI:
        kedump_mini();
        exit_aee_init();
        break;
    case AEE_ENABLE_FULL:
        kedump_mini();
        switch (mrdump_detection()) {
        case -1:
            exit_aee_init();
        case 0:
            /* Do nothing for fastboot loop */
            break;
        case 1:
            mrdump_run();
            exit_aee_init();
            break;
        default:
            LTRACEF_LEVEL(1, "Unsupport mrdump detection return\n");
            exit_aee_init();
            break;
        }
        break;
    default:
        LTRACEF_LEVEL(1, "Unsupport aee enable status\n");
        exit_aee_init();
        break;
    }
}

APP_START(aee)
.init = aee_init,
APP_END

