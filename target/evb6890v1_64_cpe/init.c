/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <gpio_api.h>
#include <platform/gpio.h>
#include <target.h>

void target_init(void)
{
    /* Pull low to reset wifi chip */
    mt_set_gpio_out(GPIO18, 0);
}


void target_quiesce(void)
{
    /* Pull high to finish wifi reset */
    mt_set_gpio_out(GPIO18, 1);
}
