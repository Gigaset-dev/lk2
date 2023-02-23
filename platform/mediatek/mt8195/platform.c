/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <assert.h>
#include <dev/interrupt/arm_gic.h>
#include <dev/timer/arm_generic.h>
#include <dev/uart.h>
#include <err.h>
#include <kernel/vm.h>
#include <memory.h>
#include <mt_gic.h>
#include <platform.h>
#include <platform/mtk_key.h>
#include <platform/irq.h>
#include <platform/timer.h>
#include <platform/wdt.h>
#include <platform/vm.h>
#include <trace.h>

#define LOCAL_TRACE 0

__WEAK void platform_early_init_ext(void)
{
}

__WEAK void platform_init_ext(void)
{
}

void platform_early_init(void)
{
    uart_init_port(DEBUG_UART, 921600);
    arm_gic_init();
    clear_sec_pol_ctl_en();
    arm_generic_timer_init(ARM_GENERIC_TIMER_PHYSICAL_INT, 0);
    keypad_init(NULL);

    platform_early_init_ext();
}

void platform_init(void)
{
    dprintf(ALWAYS, "platform init\n");

    platform_init_ext();
}

void platform_quiesce(void)
{
    platform_stop_timer();

    mtk_wdt_set_timeout_value(WDT_MAX_TIMEOUT);
    mtk_wdt_restart_timer();
}
