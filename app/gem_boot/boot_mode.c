/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <platform/boot_mode.h>
#include <platform/wdt.h>
#include <platform_mtk.h>
#include <string.h>
#include <trace.h>

#define LOCAL_TRACE 2

static u8 g_boot_mode = NORMAL_BOOT;

u8 get_boot_mode(void)
{
    return g_boot_mode;
}

static u8 pl_mode_check(void)
{
    u8 boot_mode = platform_get_boot_mode();

    if (boot_mode == META_BOOT)
        return META_BOOT;
    return 0;
}

#ifdef WITH_PLATFORM_MEDIATEK_COMMON_CHARGER
static bool kpoc_check(void)
{
    uint32_t boot_reason = platform_get_boot_reason();

    if (boot_reason == BR_WDT_BY_PASS_PWK || boot_reason == BR_KERNEL_PANIC ||
           boot_reason == BR_WDT_SW || boot_reason == BR_WDT_HW ||
           boot_reason == BR_POWER_KEY || boot_reason == BR_LONG_POWKEY)
        return false;
    LTRACEF("boot_reason = %d\n", boot_reason);
    return true;
}
#endif

void boot_mode_check(void)
{
    u8 pl_mode = pl_mode_check();

    if (pl_mode)
        g_boot_mode = pl_mode;
#ifdef WITH_PLATFORM_MEDIATEK_COMMON_CHARGER
    else if (kpoc_check())
        g_boot_mode = KERNEL_POWER_OFF_CHARGING_BOOT;
#endif
}
