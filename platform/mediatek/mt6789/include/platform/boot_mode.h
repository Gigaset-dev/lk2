/*
 * Copyright (c) 2022 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once
#include <sys/types.h>

enum boot_reason {
    BR_POWER_KEY = 0,
    BR_USB,
    BR_RTC,
    BR_WDT,
    BR_WDT_BY_PASS_PWK,
    BR_TOOL_BY_PASS_PWK,
    BR_2SEC_REBOOT,
    BR_UNKNOWN,
    BR_KERNEL_PANIC,
    BR_WDT_SW,
    BR_WDT_HW,
    BR_POWER_EXC = 30,
    BR_LONG_POWKEY,
    BR_POWER_LOSS,
    BR_REBOOT_EXCEPTION
};

enum boot_mode {
    NORMAL_BOOT                    = 0,
    META_BOOT                      = 1,
    RECOVERY_BOOT                  = 2,
    FACTORY_BOOT                   = 4,
    ALARM_BOOT                     = 7,
    KERNEL_POWER_OFF_CHARGING_BOOT = 8,
    FASTBOOT_BOOT = 99
};

