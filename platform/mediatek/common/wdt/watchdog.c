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
#include <err.h>
#include <lib/watchdog.h>
#include <platform/wdt.h>
#include <stdlib.h>

status_t platform_watchdog_init(lk_time_t  target_timeout,
                                lk_time_t *recommended_pet_period)
{
    platform_watchdog_set_enabled(false);

    target_timeout = ROUNDUP(target_timeout, 1000);
    if (target_timeout)
        mtk_wdt_set_timeout_value(target_timeout / 1000);

    *recommended_pet_period = target_timeout >> 1;

    return NO_ERROR;
}

void platform_watchdog_set_enabled(bool enabled)
{
    mtk_wdt_mark_stage(RGU_STAGE);
    mtk_wdt_config(enabled);
#if LK_AS_AEE
    if (enabled)
        mtk_wdt_isr_init();
#endif
}

void platform_watchdog_pet(void)
{
}
