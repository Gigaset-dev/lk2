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

#include <ddp_pwm.h>
#include <debug.h>
#include <libfdt.h>
#include <list.h>
#include <stdlib.h>
#include <trace.h>

#include "leds_disp_pwm.h"
#include "leds_internal.h"

#define LOCAL_TRACE 0

struct bls_pwm_led_data backlight_data =  {
    .name = "lcd-backlight",
    .pwm_data = {0, 1, 0, 0, 0},
    .gate_enable = 0
};

struct bls_pwm_leds pwm_leds_default = {
    .nums = 1,
    .leds_list = &backlight_data
};

struct bls_pwm_leds pwm_leds = {0, NULL, NULL};

int init_pwm_config(void *fdt, int offset, struct mt_led_data *led_data)
{
    unsigned char pwm_config[5] = {0};

    pwm_leds.leds_list = realloc(pwm_leds.leds_list,
            sizeof(struct bls_pwm_led_data) * (pwm_leds.nums + 1));
    if (!pwm_leds.leds_list)
        return -1;

    pwm_leds.leds_list[pwm_leds.nums].name = led_data->name;
    pwm_leds.fdt = fdt;

    LTRACEF("get pwm leds %s, %d, %d\n", led_data->name, pwm_leds.nums, offset);

    leds_fdt_getprop_char_array(fdt, offset, "pwm_config", pwm_config);
    pwm_leds.leds_list[pwm_leds.nums].pwm_data.clock_source = pwm_config[0];
    pwm_leds.leds_list[pwm_leds.nums].pwm_data.div = pwm_config[1];
    pwm_leds.leds_list[pwm_leds.nums].pwm_data.low_duration = pwm_config[2];
    pwm_leds.leds_list[pwm_leds.nums].pwm_data.High_duration = pwm_config[3];
    pwm_leds.leds_list[pwm_leds.nums].pwm_data.pmic_pad = pwm_config[4];

    pwm_leds.leds_list[pwm_leds.nums].gate_enable =
        leds_fdt_getprop_u32(fdt, offset, "gate_enable", 0);

    LTRACEF("%s, PWM_config [%d %d %d %d %d], gate_enable %d\n",
            led_data->name,
            pwm_config[0], pwm_config[1], pwm_config[2],
            pwm_config[3], pwm_config[4],
            pwm_leds.leds_list[pwm_leds.nums].gate_enable);
    pwm_leds.nums += 1;

    return 0;
}

static int get_pwm_led_index_by_name(struct bls_pwm_leds leds, const char *name)
{
    int i = 0;

    while (i < MT_LED_TYPE_TOTAL) {
        if (!strcmp(leds.leds_list[i].name, name))
            return i;
        i++;
    }
    return -1;
}

struct PWM_config *get_pwm_config_by_name(const char *name)
{
    int index = 0;

    if (pwm_leds.leds_list == NULL)
        pwm_leds = pwm_leds_default;
    index = get_pwm_led_index_by_name(pwm_leds, name);
    LTRACEF("get leds %d:%s\n", index, name);

    if (index >= pwm_leds.nums || index < 0) {
        dprintf(CRITICAL, "get the LED index by name failed!\n");
        return NULL;
    }
    LTRACEF("get PWM_config [%d %d %d %d %d]\n",
            pwm_leds.leds_list[index].pwm_data.clock_source,
            pwm_leds.leds_list[index].pwm_data.div,
            pwm_leds.leds_list[index].pwm_data.low_duration,
            pwm_leds.leds_list[index].pwm_data.High_duration,
            pwm_leds.leds_list[index].pwm_data.pmic_pad);
    return &(pwm_leds.leds_list[index].pwm_data);
}

int disp_pwm_brightness_set(struct mt_led_data *led_data, unsigned int set_level)
{
    int index;

    if (pwm_leds.leds_list == NULL)
        pwm_leds = pwm_leds_default;

    index = get_pwm_led_index_by_name(pwm_leds, led_data->name);

    if (index >= pwm_leds.nums || index < 0) {
        dprintf(CRITICAL, "get the LED index by name failed!\n");
        return -1;
    }

    if (pwm_leds.leds_list[index].gate_enable == 1 &&
        led_data->level == 0 &&
        set_level != 0){
        dprintf(CRITICAL, "get the LED index by name failed!\n");
#if defined(MTK_RT4831A_SUPPORT) || defined(MTK_LCM_COMMON_DRIVER_SUPPORT)
        _gate_ic_i2c_backlight_enable(pwm_leds.fdt, 1, 1);
#endif
    }
    dprintf(CRITICAL, "brightness set by disp_pwm: %d\n", set_level);

    return disp_bls_set_backlight(set_level);
}


