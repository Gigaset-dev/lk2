/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include "lcm_gateic.h"

#define ADDR_BACKLIGHT_CONFIG1            (0x02)
#define ADDR_BACKLIGHT_CONFIG2            (0x03)
#define ADDR_BACKLIGHT_BRIGHTNESS_LSB    (0x04)
#define ADDR_BACKLIGHT_BRIGHTNESS_MSB    (0x05)
#define ADDR_BACKLIGHT_AUTO_FREQ_LOW    (0x06)
#define ADDR_BACKLIGHT_AUTO_FREQ_HIGH    (0x07)
#define ADDR_BACKLIGHT_ENABLE            (0x08)
#define ADDR_BACKLIGHT_OPTION1            (0x10)
#define ADDR_BACKLIGHT_OPTION2            (0x11)
#define ADDR_BACKLIGHT_SMOOTH            (0x14)
#define ADDR_BIAS_CONFIG1                (0x09)
#define ADDR_BIAS_CONFIG2                (0x0A)
#define ADDR_BIAS_CONFIG3                (0x0B)
#define ADDR_BIAS_LCM                    (0x0C)
#define ADDR_BIAS_VPOS                    (0x0D)
#define ADDR_BIAS_VNEG                    (0x0E)

#define BRIGHTNESS_LOW_OFFSET            (0)
#define BRIGHTNESS_HIGH_OFFSET            (3)
#define BRIGHTNESS_LOW_MASK                (0x07)
#define BRIGHTNESS_HIGH_MASK            (0x7F8)

#define RT4831A_VOL_UNIT (50) //50mV
#define RT4831A_VOL_MIN_LEVEL (4000) //4000mV
#define RT4831A_VOL_MAX_LEVEL (6500) //6500mV
#define RT4831A_VOL_REG_VALUE(level) ((level - RT4831A_VOL_MIN_LEVEL) / RT4831A_VOL_UNIT)
#define RT4831A_LCM_I2C_MODE (0)
#define RT4831A_LCM_I2C_SPEED (400)

struct mtk_gateic_data ctx_rt4831a;
#define LCM_FUNC_RT4831A (MTK_LCM_FUNC_DSI)

#ifndef PROJECT_TYPE_FPGA

static int rt4831a_push_i2c_data(unsigned char *table, unsigned int size,
        unsigned int unit)
{
    int ret = 0;
    unsigned int i = 0;

    if (table == NULL ||
        size == 0 || unit < 2) {
        LCM_MSG("%s, invalid table, size:%u, unit:%u\n",
            __func__, size, unit);
        return -EINVAL;
    }

    for (i = 0; i < size; i++) {
        if (unit > 2)
            ret = mtk_panel_i2c_write_multiple_bytes(&ctx_rt4831a.client,
                    *(table + i * unit),
                    table + i * unit + 1, unit - 1);
        else if (unit == 2)
            ret = mtk_panel_i2c_write_bytes(&ctx_rt4831a.client,
                    *(table + i * 2),
                    *(table + i * 2 + 1));

        if (ret < 0) {
            LCM_MSG("%s, failed of i2c write, i:%u addr:0x%x, unit:0x%x\n",
                __func__, i, *(table + i * unit), unit);
            break;
        }
    }

    return 0;
}

static int rt4831a_update_backlight_table(unsigned int level, unsigned char *table,
        unsigned int size, unsigned int unit)
{
    unsigned int i = 0;

    if (table == NULL ||
        size == 0 || unit < 2) {
        LCM_MSG("%s, invalid table, size:%u, unit:%u\n",
            __func__, size, unit);
        return -EINVAL;
    }

    for (i = 0; i < size; i++) {
        if (*(table + i * unit) == ADDR_BACKLIGHT_BRIGHTNESS_LSB)
            *(table + i * unit + 1) = (level & BRIGHTNESS_LOW_MASK) >>
                        BRIGHTNESS_LOW_OFFSET;
        else if (*(table + i * unit) == ADDR_BACKLIGHT_BRIGHTNESS_MSB)
            *(table + i * unit + 1) = (level & BRIGHTNESS_HIGH_MASK) >>
                        BRIGHTNESS_HIGH_OFFSET;
    }

    return 0;
}

static int rt4831a_enable_backlight(unsigned int enable, unsigned int pwm_enable)
{
    int ret = 0;
    unsigned int backlight_mode = ((pwm_enable == 1) ? BL_PWM_MODE : BL_I2C_MODE);
    unsigned char table[8][2] = {
        {ADDR_BACKLIGHT_CONFIG2, 0x9D},
        {ADDR_BACKLIGHT_BRIGHTNESS_LSB, 0x7},
        {ADDR_BACKLIGHT_BRIGHTNESS_MSB, 0xFF},
        {ADDR_BACKLIGHT_AUTO_FREQ_LOW, 0x0},
        {ADDR_BACKLIGHT_AUTO_FREQ_HIGH, 0x0},
        {ADDR_BACKLIGHT_ENABLE, 0x15},
        {ADDR_BACKLIGHT_OPTION1, 0x06},
        {ADDR_BACKLIGHT_OPTION2, 0xB7},
    };
    unsigned int unit = countof(table[0]);
    unsigned int size = sizeof(table) / unit;

    LCM_MSG("%s+ enable:%u, pwm:%u\n", __func__, enable, pwm_enable);
    if (enable == 0) {
        ret = mtk_panel_i2c_write_bytes(&ctx_rt4831a.client,
                ADDR_BACKLIGHT_ENABLE, 0x0);
        if (ret < 0)
            LCM_MSG("%s, failed to disbale backlight, mode:%u",
                __func__, backlight_mode);
        return ret;
    }

    switch (backlight_mode) {
    case BL_PWM_MODE:
        ret = mtk_panel_i2c_write_bytes(&ctx_rt4831a.client,
                ADDR_BACKLIGHT_CONFIG1, 0x6B);
        break;
    case BL_I2C_MODE:
        ret = mtk_panel_i2c_write_bytes(&ctx_rt4831a.client,
                ADDR_BACKLIGHT_CONFIG1, 0x68);
        break;
    default:
        ret = -EINVAL;
        break;
    }

    if (ret < 0) {
        LCM_MSG("%s, failed to enbale backlight, mode:%u",
            __func__, backlight_mode);
        return ret;
    }

    rt4831a_update_backlight_table(ctx_rt4831a.backlight_level, &table[0][0], 8, 2);
    ret = rt4831a_push_i2c_data(&table[0][0], size, unit);
    if (ret < 0)
        LCM_MSG("%s, failed to push backlight table, mode:%u, ret:%d",
            __func__, backlight_mode, ret);

    return 0; //for evb case, i2c ops are always failed
}

static int rt4831a_set_backlight(unsigned int level)
{
    int ret = 0;
    unsigned char table[2][2] = {
        {ADDR_BACKLIGHT_BRIGHTNESS_LSB, 0},
        {ADDR_BACKLIGHT_BRIGHTNESS_MSB, 0}
    };
    unsigned int unit = countof(table[0]);
    unsigned int size = sizeof(table) / unit;

    LCM_MSG("%s+  last level:0x%x, curr level:0x%x\n",
        __func__, ctx_rt4831a.backlight_level, level);

    if (ctx_rt4831a.backlight_level == 0 && level != 0) {
        ctx_rt4831a.backlight_level = level;
        return rt4831a_enable_backlight(1, 0);
    }
    rt4831a_update_backlight_table(level, &table[0][0], size, unit);
    ret = rt4831a_push_i2c_data(&table[0][0], size, unit);
    if (ret < 0)
        LCM_MSG("ERROR %d!! i2c write data fail 0x%0x, 0x%0x !!\n",
                ret, table[0][1], table[1][1]);

    ctx_rt4831a.backlight_level = level;
    return 0; //for evb case, i2c ops are always failed
}

static int rt4831a_reset(int on)
{
    u32 gpio = 0;

    if (ctx_rt4831a.init != 1) {
        LCM_MSG("%s gate ic is not initialized\n", __func__);
        return -1;
    }

    if (ctx_rt4831a.reset_gpio < 0) {
        LCM_MSG("%s: cannot get reset_gpio %d\n",
            __func__, ctx_rt4831a.reset_gpio);
        return -EFAULT;
    }

    gpio = ctx_rt4831a.reset_gpio;
    mt_set_gpio_mode(gpio, GPIO_MODE_00);
    mt_set_gpio_dir(gpio, GPIO_DIR_OUT);
    if (on != 0)
        mt_set_gpio_out(gpio, GPIO_OUT_ONE);
    else
        mt_set_gpio_out(gpio, GPIO_OUT_ZERO);

    LCM_DBG("%s, gpio:0x%x, on:%d, mode:%d, dir:%d\n",
        __func__, gpio, on, GPIO_MODE_00, GPIO_DIR_OUT);
    return 0;
}

static int rt4831a_power_on(void)
{
    int ret = 0;
    u32 gpio = 0;
    unsigned char table[8][2] = {
        {ADDR_BIAS_CONFIG2, 0x11},
        {ADDR_BIAS_CONFIG3, 0x00},
        /*default voltage of 5.4v*/
        {ADDR_BIAS_LCM, 0x24},
        {ADDR_BIAS_VPOS, 0x1c},
        {ADDR_BIAS_VNEG, 0x1c},
        /*set dsv FPWM mode*/
        {0xF0, 0x69},
        {0xB1, 0x6c},
        /* bias enable*/
        {ADDR_BIAS_CONFIG1, 0x9e},
    };
    unsigned int unit = countof(table[0]);
    unsigned int size = sizeof(table) / unit;

    if (ctx_rt4831a.init != 1) {
        LCM_MSG("%s gate ic is not initialized\n", __func__);
        return -1;
    }

    if (ctx_rt4831a.ref++ > 0) {
        LCM_MSG("%s gate ic (%u) already power on\n", __func__, ctx_rt4831a.ref);
        return 0;
    }

    LCM_DBG("%s++\n", __func__);
    if (ctx_rt4831a.bias_pos_gpio < 0 ||
        ctx_rt4831a.bias_neg_gpio < 0) {
        LCM_MSG("%s: cannot get bias %d, %d\n",
            __func__, ctx_rt4831a.bias_pos_gpio,
            ctx_rt4831a.bias_neg_gpio);
        return -EFAULT;
    }
    gpio = ctx_rt4831a.bias_pos_gpio;
    mt_set_gpio_mode(gpio, GPIO_MODE_00);
    mt_set_gpio_dir(gpio, GPIO_DIR_OUT);
    mt_set_gpio_out(gpio, GPIO_OUT_ONE);

    ret = rt4831a_push_i2c_data(&table[0][0], size, unit);
    if (ret < 0)
        LCM_MSG("%s, failed to push power on table, ret:%d",
            __func__, ret);

    LCM_DBG("%s--\n", __func__);
    return 0; //for evb case, i2c ops are always failed
}

static int rt4831a_power_off(void)
{
    u32 gpio = 0;
    int ret = 0;

    if (ctx_rt4831a.init != 1) {
        LCM_MSG("%s gate ic is not initialized\n", __func__);
        return -1;
    }

    if (ctx_rt4831a.ref == 0) {
        LCM_MSG("%s gate ic (%u) already power off\n",
            __func__, ctx_rt4831a.ref);
        return 0;
    }

    LCM_DBG("%s++\n", __func__);
    ctx_rt4831a.ref--;
    if (ctx_rt4831a.ref > 0) {
        LCM_MSG("%s, do nothing, there are other users, %u\n",
            __func__, ctx_rt4831a.ref);
        return 0;
    }

    if (ctx_rt4831a.backlight_status == 0) {
        ret = mtk_panel_i2c_write_bytes(&ctx_rt4831a.client,
                ADDR_BIAS_CONFIG1, 0x18);
        if (ret < 0)
            LCM_MSG("%s, %d, failed of i2c write\n", __func__, __LINE__);

        if (ctx_rt4831a.bias_pos_gpio < 0) {
            LCM_MSG("%s: cannot get bias %d\n",
                __func__, ctx_rt4831a.bias_pos_gpio);
            return -EFAULT;
        }

        gpio = ctx_rt4831a.bias_pos_gpio;
        mt_set_gpio_mode(gpio, GPIO_MODE_00);
        mt_set_gpio_dir(gpio, GPIO_DIR_OUT);
        mt_set_gpio_out(gpio, GPIO_OUT_ZERO);
    }

    LCM_DBG("%s--\n", __func__);
    return 0; //for evb case, i2c ops are always failed
}

static int rt4831a_set_voltage(unsigned int level)
{
    int ret = 0;
    unsigned int i = 0;
    unsigned char level_id = 0;
    unsigned char table[3][2] = {
        {ADDR_BIAS_LCM, 0},
        {ADDR_BIAS_VPOS, 0},
        {ADDR_BIAS_VNEG, 0}
    };
    unsigned int unit = countof(table[0]);
    unsigned int size = sizeof(table) / unit;

    if (ctx_rt4831a.init != 1) {
        LCM_MSG("%s gate ic is not initialized\n", __func__);
        return -1;
    }

    if (ctx_rt4831a.ref == 0) {
        LCM_MSG("%s gate ic (%u) is power off\n",
            __func__, ctx_rt4831a.ref);
        return -2;
    }

    if (level < RT4831A_VOL_MIN_LEVEL || level > RT4831A_VOL_MAX_LEVEL) {
        LCM_MSG("%s invalid voltage level:%d\n", __func__, level);
        return -3;
    }

    LCM_DBG("%s++\n", __func__);
    level_id = (unsigned char)RT4831A_VOL_REG_VALUE(level);
    LCM_MSG("%s++ level:%d, id:%u\n", __func__, level, level_id);
    for (i = 0; i < size; i++)
        table[i][1] = level_id;

    ret = rt4831a_push_i2c_data(&table[0][0], size, unit);
    if (ret < 0)
        LCM_MSG("%s, failed to push voltage table,level:%u-%u, ret:%d",
            __func__, level, RT4831A_VOL_REG_VALUE(level), ret);
    LCM_DBG("%s--\n", __func__);
    return 0; //for evb case, i2c ops are always failed
}

static int rt4831a_gateic_parse_pin(const void *fdt)
{
    int ret = 0;

    ctx_rt4831a.reset_gpio = mtk_panel_get_pin(fdt, "lcm_rst_out0_gpio");
    if (ctx_rt4831a.reset_gpio < 0) {
        LCM_MSG("%s: failed to parse reset pin\n", __func__);
        ret = -EFAULT;
    }

    ctx_rt4831a.bias_pos_gpio = mtk_panel_get_pin(fdt, "lcm_led_en0_gpio");
    if (ctx_rt4831a.bias_pos_gpio < 0) {
        LCM_MSG("%s: failed to parse enpos pin\n", __func__);
        ret = -EFAULT;
    }

    LCM_DBG("%s, reset gpio:0x%x, pos gpio:0x%x\n",
        __func__, ctx_rt4831a.reset_gpio, ctx_rt4831a.bias_pos_gpio);
    return ret;
}

struct mt_i2c *rt4831a_get_i2c_client(void)
{
    if (ctx_rt4831a.init == 0) {
        LCM_MSG("%s, %d, error: has not been initialized\n",
            __func__, __LINE__);
        return NULL;
    }

    return &ctx_rt4831a.client;
}

static struct mtk_gateic_funcs rt4831a_ops = {
    .reset = rt4831a_reset,
    .power_on = rt4831a_power_on,
    .power_off = rt4831a_power_off,
    .set_voltage = rt4831a_set_voltage,
    .get_i2c_client = rt4831a_get_i2c_client,
    .enable_backlight = rt4831a_enable_backlight,
    .set_backlight = rt4831a_set_backlight,
};

int rt4831a_gateic_init(const void *fdt, char func, const char *lcm_name)
{
    int ret = 0, i = 0, len = 0, count = 0;
    int id = 0, addr = 0, offset = 0;
    const char *lcm_tmp;

    if (func != LCM_FUNC_RT4831A || lcm_name == NULL)
        return -EFAULT;

    if (ctx_rt4831a.init == 1) {
        if (mtk_gateic_match_lcm_list(lcm_name, ctx_rt4831a.lcm_list,
            ctx_rt4831a.lcm_count, "rt4831a") == true)
            return 0;
        else
            return -EFAULT;
    }
    LCM_DBG("%s++\n", __func__);

    offset = fdt_node_offset_by_compatible(fdt, -1,
        "mediatek,mtk-drm-gateic-drv-rt4831a");
    if (offset < 0) {
        LCM_MSG("%s, failed to get rt4831a dts, ret:%d\n",
            __func__, ret);
        return offset;
    }
    count = fdt_stringlist_count(fdt, offset, "panel-list");
    ctx_rt4831a.lcm_list = malloc(ROUNDUP(count * sizeof(char *), 4));
    if (count > 0) {
        for (i = 0; i < count; i++) {
            LCM_MALLOC(ctx_rt4831a.lcm_list[i], MTK_LCM_NAME_LENGTH);
            if (ctx_rt4831a.lcm_list[i] == NULL) {
                LCM_MSG("%s, %d, failed to allocate lcm_list, i:%d\n",
                    __func__, __LINE__, i);
                return -ENOMEM;
            }

            lcm_tmp = fdt_stringlist_get(fdt, offset, "panel-list", i, &len);
            if (lcm_tmp == NULL) {
                ret = -EFAULT;
                LCM_MSG("%s, %d, failed to get lcm_list, i:%d, len:%d\n",
                    __func__, __LINE__, i, len);
                goto end;
            }
            LCM_DBG("%s, %d, lcm:%d, \"%s\"\n", __func__, __LINE__, i, lcm_tmp);
            ret = snprintf(ctx_rt4831a.lcm_list[i],
                MTK_LCM_NAME_LENGTH - 1, "%s", lcm_tmp);
            if (ret < 0 ||
                ret >= MTK_LCM_NAME_LENGTH) {
                LCM_MSG("%s, %d, failed to get panel_list:%d, len:%d, ret:%d\n",
                    __func__, __LINE__, i, len, ret);
                ret = -EFAULT;
                goto end;
            }
        }
        ctx_rt4831a.lcm_count = (unsigned int)i;
    } else {
        LCM_MSG("%s, %d, failed to get panel-list, %d\n",
            __func__, __LINE__, count);
        return -EFAULT;
    }

    if (ctx_rt4831a.lcm_count > 0 &&
        mtk_gateic_match_lcm_list(lcm_name, ctx_rt4831a.lcm_list,
            ctx_rt4831a.lcm_count, "rt4831a") == false) {
        ret = -EFAULT;
        goto end;
    }

    if (mtk_panel_get_i2c_lcd_bias(fdt, &id, &addr)) {
        LCM_MSG("%s: failed to get i2c data:%d, %d\n",
            __func__, id, addr);
        return -EFAULT;
    }

    ctx_rt4831a.client.id = (unsigned short)(id & 0xFFFF);
    ctx_rt4831a.client.addr = (unsigned short)addr;
    ctx_rt4831a.client.mode = RT4831A_LCM_I2C_MODE;
    ctx_rt4831a.client.speed = RT4831A_LCM_I2C_SPEED;
    ctx_rt4831a.backlight_level = 0;
    ctx_rt4831a.init = 1;
    ctx_rt4831a.ref = 0;
    LCM_MSG("%s, i2c id:%u, addr:0x%x, mode:%u, speed:%u\n",
        __func__, ctx_rt4831a.client.id, ctx_rt4831a.client.addr,
        ctx_rt4831a.client.mode, ctx_rt4831a.client.speed);

    ret = rt4831a_gateic_parse_pin(fdt);
    if (ret != 0) {
        LCM_MSG("%s: failed to get gateic pins of rt4831a:%d\n",
            __func__, ret);
        return ret;
    }

    ret = mtk_lcm_gateic_register(&rt4831a_ops, func);
    LCM_DBG("%s: set gateic ops:0x%lx, %d\n",
        __func__, (unsigned long)&rt4831a_ops, ret);
    if (ret != 0) {
        LCM_MSG("%s: failed to set gateic ops of rt4831a:%d\n",
            __func__, ret);
        return ret;
    }

    LCM_DBG("%s--, %d\n", __func__, ret);

    return ret;

end:
    for (i = 0; i < count; i++) {
        if (ctx_rt4831a.lcm_list[i] == NULL)
            break;
        LCM_FREE(ctx_rt4831a.lcm_list[i],
            MTK_LCM_NAME_LENGTH);
    }
    ctx_rt4831a.lcm_count = 0;
    free(ctx_rt4831a.lcm_list);
    return ret;

}

#else

int rt4831a_gateic_init(const void *fdt, char func, const char *lcm_name)
{
    LCM_MSG("%s bypass fpga power control\n", __func__);
    return 0
}

#endif

