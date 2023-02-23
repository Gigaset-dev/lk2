/*
 * Copyright (c) 2019 MediaTek Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 *publish, distribute, sublicense, and/or sell copies of the Software,
 *and to permit persons to whom the Software is furnished to do so,
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
#pragma once

#include <platform/leds.h>
#include <platform/leds_config.h>
#include <stdbool.h>

#include "leds_internal.h"


struct bls_pwm_led_data {
    const char         *name;
    struct PWM_config pwm_data;
    unsigned int gate_enable;
};

struct bls_pwm_leds {
    int    nums;
    struct bls_pwm_led_data *leds_list;
    void   *fdt;
};

int disp_pwm_brightness_set(struct mt_led_data *led_data, unsigned int level);
int init_pwm_config(void *fdt, int offset, struct mt_led_data *led_data);

//extern int _gate_ic_i2c_backlight_enable(unsigned int is_enable, unsigned int pwm_enable);

