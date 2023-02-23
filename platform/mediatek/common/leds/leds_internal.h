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

#ifdef MTK_LCM_COMMON_DRIVER_SUPPORT
#include <lcm_common_drv.h>
#else
#ifdef MTK_RT4831A_SUPPORT
#include "rt4831a_drv.h"
#endif
#endif

enum mt_led_type {
    MT_LED_TYPE_RED = 0,
    MT_LED_TYPE_GREEN,
    MT_LED_TYPE_BLUE,
    MT_LED_TYPE_JOGBALL,
    MT_LED_TYPE_KEYBOARD,
    MT_LED_TYPE_BUTTON,
    MT_LED_TYPE_LCD,
    MT_LED_TYPE_TOTAL,
};

enum mt_led_mode {
    MT_LED_MODE_NONE,
    MT_LED_MODE_PWM,
    MT_LED_MODE_GPIO,
    MT_LED_MODE_PMIC,
    MT_LED_MODE_CUST_LCM,
    MT_LED_MODE_CUST_BLS_PWM,
    MT_LED_MODE_CUST_BLS_I2C
};

struct mt_led_data {
    const char         *name;
    enum mt_led_mode   mode;
    int                level;
    int                max_brightness;
    int                max_hw_brightness;
    int                index;
};

struct mt_leds {
    int nums;
    struct mt_led_data *leds_list;
    void         *fdt;
};

void leds_fdt_getprop_char_array(const void *fdt, int nodeoffset,
                                const char *name, unsigned char *out_value);
unsigned int leds_fdt_getprop_u32(const void *fdt, int nodeoffset,
                                const char *name, unsigned int default_val);
extern int primary_display_setbacklight(unsigned int level);
#ifdef MTK_RT4831A_SUPPORT
extern int _gate_ic_i2c_backlight_enable(void *fdt, unsigned int is_enable,
                               unsigned int pwm_enable);
extern int _gate_ic_i2c_backight_level_set(void *fdt, unsigned int brightness);
#endif


