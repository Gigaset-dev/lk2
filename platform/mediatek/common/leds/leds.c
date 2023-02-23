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
#include <debug.h>
#include <libfdt.h>
#include <platform/leds.h>
#include <string.h>
#include <trace.h>

#include "leds_internal.h"
#include "leds_disp_pwm.h"

#define LOCAL_TRACE 0

const char *leds_name[MT_LED_TYPE_TOTAL] = {
    "red",
    "green",
    "blue",
    "jogball-backlight",
    "keyboard-backlight",
    "button-backlight",
    "lcd-backlight",
};

const char *leds_node[MT_LED_TYPE_TOTAL] = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    "/mtk_leds/backlight",
};

enum led_brightness backlight_default_level = LED_FULL * 40 / 100;
struct mt_led_data backlight_info = {"lcd-backlight", MT_LED_MODE_CUST_BLS_PWM, 127, 255, 1023, 0};
struct mt_leds leds_default = {
    .nums = 1,
    .leds_list = &backlight_info
};

struct mt_leds mt_leds = {0, NULL, NULL};
struct mt_led_data leds_dts[MT_LED_TYPE_TOTAL];

/**
 * If the property is not found or the value is not set,
 * return default value 0.
 */
unsigned int leds_fdt_getprop_u32(const void *fdt, int nodeoffset,
                                 const char *name, unsigned int default_val)
{
    const void *data = NULL;
    int len = 0;

    data = fdt_getprop(fdt, nodeoffset, name, &len);
    if (len > 0 && data)
        return fdt32_to_cpu(*(unsigned int *)data);
    else
        return default_val;
}

/**
 * If the property is not found or the value is not set,
 * use default value 0.
 */
void leds_fdt_getprop_char_array(const void *fdt, int nodeoffset,
                                const char *name, unsigned char *out_value)
{
    int i;
    unsigned int *data = NULL;
    int len = 0;

    data = (unsigned int *)fdt_getprop(fdt, nodeoffset, name, &len);
    if (len > 0 && data) {
        len = len / sizeof(unsigned int);
        for (i = 0; i < len; i++)
            *(out_value+i) = (unsigned char)((fdt32_to_cpu(*(data+i))) & 0xFF);
    } else
        *out_value = 0;
}

unsigned int leds_brightness_mapping(int max, int hw_max, int level)
{
    unsigned int mapped_level;

    if (level < 0)
        mapped_level =  0;
    else
        mapped_level = (hw_max * level + max / 2) / max;

    return mapped_level;
}

int leds_brightness_hw_set(struct mt_led_data *led_data, enum led_brightness level)
{
    unsigned int set_level;

    LTRACEF("set %s level %d(%d, %d)\n", led_data->name, level,
        led_data->max_brightness, led_data->max_hw_brightness);

    level = MIN((int)LED_FULL, MIN(led_data->max_brightness, (int)level));
    if (led_data->level == (int)level) {
        dprintf(CRITICAL, "set %s level %d no change, return\n",
            led_data->name, level);
        return 0;
    }

    set_level = leds_brightness_mapping(led_data->max_brightness,
        led_data->max_hw_brightness, level);
    LTRACEF("set %s level %d:%d\n", led_data->name, level, set_level);

    return set_level;
}

/****************************************************************************
 * internal functions
 ***************************************************************************/
static void get_mt_leds_dtsi(void *fdt)
{
    bool isDTinited = 0;
    int i, offset;
    char led_node[64] = "/soc";

    if (fdt) {
        if (fdt_check_header(fdt) != 0)
            panic("Bad DTB header.\n");
    } else {
        dprintf(CRITICAL, "[leds] fdt null, fall back to default\n");
        goto apply_default;
    }

    if (mt_leds.leds_list == NULL) {
        mt_leds.leds_list = leds_dts;
        mt_leds.nums = MT_LED_TYPE_TOTAL;
    }
    mt_leds.fdt = fdt;

    LTRACEF("get mt_leds info from dts start ---\n");
    for (i = 0; i < MT_LED_TYPE_TOTAL; i++) {
        mt_leds.leds_list[i].name = leds_name[i];
        mt_leds.leds_list[i].index = i;
        if (leds_node[i] == NULL)
            offset = -1;
        else {
            offset = fdt_path_offset(fdt, leds_node[i]);
            if (offset < 0 && strlcat(led_node, leds_node[i], 64) < 64)
                offset = fdt_path_offset(fdt, led_node);
        }
        LTRACEF("get %s %d info from dts %d!\n", leds_name[i], i, offset);
        if (offset >= 0) {
            isDTinited = 1;
            LTRACEF("find LED node offset > 0!\n");
            mt_leds.leds_list[i].mode = leds_fdt_getprop_u32(fdt, offset, "led_mode", 0);
            mt_leds.leds_list[i].max_brightness = leds_fdt_getprop_u32(fdt,
                offset, "max-brightness", 255);
            mt_leds.leds_list[i].max_hw_brightness = leds_fdt_getprop_u32(fdt, offset,
                "max-hw-brightness", 1023);
            mt_leds.leds_list[i].level = 0;
            switch (mt_leds.leds_list[i].mode) {
            case MT_LED_MODE_CUST_BLS_PWM:
                init_pwm_config(fdt, offset, &(mt_leds.leds_list[i]));
                LTRACEF("The backlight hw mode is BLS.\n");
                break;
            case MT_LED_MODE_CUST_LCM:
            case MT_LED_MODE_CUST_BLS_I2C:
                LTRACEF("The backlight hw mode is LCM.\n");
                break;
            default:
                break;
            }
            LTRACEF("led[%d] offset is %d,mode is %d,level is %d(%d, %d).\n",
                  i, offset, mt_leds.leds_list[i].mode, mt_leds.leds_list[i].level,
                  mt_leds.leds_list[i].max_brightness,
                  mt_leds.leds_list[i].max_hw_brightness);
        }
    }

    LTRACEF("get mt_leds info from dts end ---\n");

apply_default:
    if (isDTinited == 0)
        mt_leds = leds_default;
}

static int brightness_set_by_mode(struct mt_led_data *led_data, int level)
{
    LTRACEF("get mode: %d\n", led_data->mode);
    int hw_level = leds_brightness_hw_set(led_data, level);

    switch (led_data->mode) {
    case MT_LED_MODE_PWM:
//            return brightness_set_pwm(led_data, level);
        break;
    case MT_LED_MODE_GPIO:
//            return brightness_set_gpio(led_data, level);
        break;
    case MT_LED_MODE_PMIC:
//            return brightness_set_pmic(led_data, level);
        break;
    case MT_LED_MODE_CUST_LCM:
        LTRACEF("set level by display: %d\n", hw_level);
        return primary_display_setbacklight(hw_level);
        break;
    case MT_LED_MODE_CUST_BLS_PWM:
        LTRACEF("set level by pwm: %d\n", hw_level);
        return disp_pwm_brightness_set(led_data, hw_level);
        break;
    case MT_LED_MODE_CUST_BLS_I2C:
        LTRACEF("set level by i2c: %d\n", hw_level);
#if defined(MTK_RT4831A_SUPPORT) || defined(MTK_LCM_COMMON_DRIVER_SUPPORT)
        _gate_ic_i2c_backight_level_set(mt_leds.fdt, hw_level);
#endif
        break;
    case MT_LED_MODE_NONE:
    default:
        break;
    }
    return -1;
}

static int get_led_index_by_name(const char *name, struct mt_leds leds)
{
    int i = 0;

    while (i < MT_LED_TYPE_TOTAL) {
        if (!strcmp(leds.leds_list[i].name, name))
            return i;
        i++;
    }
    return -1;
}
static int mt_leds_brightness_set(const char *name, enum led_brightness level)
{
    int index = get_led_index_by_name(name, mt_leds);
    int ret = 0;

    LTRACEF("get leds index: %d:%s set %d\n", index, name, level);

    if (index >= MT_LED_TYPE_TOTAL || index < 0) {
        dprintf(CRITICAL, "get the LED index by name failed!\n");
        ret = -1;
    } else if ((int)level != mt_leds.leds_list[index].level) {
        LTRACEF("%s level is %d\n", mt_leds.leds_list[index].name, level);
        ret = brightness_set_by_mode(&(mt_leds.leds_list[index]), level);
        mt_leds.leds_list[index].level = level;
    }
    return ret;
}

/****************************************************************************
 * external functions
 ***************************************************************************/
void leds_battery_full_charging(void)
{
    mt_leds_brightness_set(leds_name[MT_LED_TYPE_RED], LED_OFF);
    mt_leds_brightness_set(leds_name[MT_LED_TYPE_GREEN], LED_FULL);
    mt_leds_brightness_set(leds_name[MT_LED_TYPE_BLUE], LED_OFF);
}

void leds_battery_low_charging(void)
{
    mt_leds_brightness_set(leds_name[MT_LED_TYPE_RED], LED_FULL);
    mt_leds_brightness_set(leds_name[MT_LED_TYPE_GREEN], LED_OFF);
    mt_leds_brightness_set(leds_name[MT_LED_TYPE_BLUE], LED_OFF);
}

void leds_battery_medium_charging(void)
{
    mt_leds_brightness_set(leds_name[MT_LED_TYPE_RED], LED_FULL);
    mt_leds_brightness_set(leds_name[MT_LED_TYPE_GREEN], LED_OFF);
    mt_leds_brightness_set(leds_name[MT_LED_TYPE_BLUE], LED_OFF);
}

void leds_set(enum led_brightness level)
{
    dprintf(CRITICAL, "leds set on level =  %d\n", level);
    mt_leds_brightness_set(leds_name[MT_LED_TYPE_LCD], level);
}

void leds_on(void)
{
    dprintf(CRITICAL, "leds set on level =  %d\n", backlight_default_level);
    mt_leds_brightness_set(leds_name[MT_LED_TYPE_LCD], backlight_default_level);
}

void leds_off(void)
{
    dprintf(CRITICAL, "leds set off.\n");
    mt_leds_brightness_set(leds_name[MT_LED_TYPE_LCD], LED_OFF);
}

void leds_init(void *fdt)
{
    dprintf(CRITICAL, "leds init.\n");
    get_mt_leds_dtsi(fdt);
}

void leds_deinit(void)
{
    dprintf(CRITICAL, "leds off.\n");
    mt_leds_brightness_set(leds_name[MT_LED_TYPE_RED], LED_OFF);
    mt_leds_brightness_set(leds_name[MT_LED_TYPE_GREEN], LED_OFF);
    mt_leds_brightness_set(leds_name[MT_LED_TYPE_BLUE], LED_OFF);
}

