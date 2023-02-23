/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include "lcm_gateic.h"

#define RT4801H_VOL_UNIT (100)
#define RT4801H_VOL_MIN_LEVEL (4000)
#define RT4801H_VOL_MAX_LEVEL (6500)
#define RT4801H_VOL_REG_VALUE(level) ((level - RT4801H_VOL_MIN_LEVEL) / RT4801H_VOL_UNIT)
#define RT4801H_LCM_I2C_MODE (0)
#define RT4801H_LCM_I2C_SPEED (100)

struct mtk_gateic_data ctx_rt4801h;
#define LCM_FUNC_RT4801H (MTK_LCM_FUNC_DSI)

#ifndef PROJECT_TYPE_FPGA
static int rt4801h_reset(int on)
{
    u32 gpio = 0;

    if (ctx_rt4801h.init != 1) {
        LCM_MSG("%s gate ic is not initialized\n", __func__);
        return -1;
    }

    if (ctx_rt4801h.reset_gpio < 0) {
        LCM_MSG("%s: cannot get reset_gpio %d\n",
            __func__, ctx_rt4801h.reset_gpio);
        return -EFAULT;
    }

    gpio = ctx_rt4801h.reset_gpio;
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

static int rt4801h_power_on(void)
{
    int ret = 0;
    u32 gpio = 0;

    if (ctx_rt4801h.init != 1) {
        LCM_MSG("%s gate ic is not initialized\n", __func__);
        return -1;
    }

    if (ctx_rt4801h.ref++ > 0) {
        LCM_MSG("%s gate ic (%u) already power on\n", __func__, ctx_rt4801h.ref);
        return 0;
    }

    LCM_DBG("%s++\n", __func__);
    if (ctx_rt4801h.bias_pos_gpio < 0 ||
        ctx_rt4801h.bias_neg_gpio < 0) {
        LCM_MSG("%s: cannot get bias %d, %d\n",
            __func__, ctx_rt4801h.bias_pos_gpio,
            ctx_rt4801h.bias_neg_gpio);
        return -EFAULT;
    }
    gpio = ctx_rt4801h.bias_pos_gpio;
    mt_set_gpio_mode(gpio, GPIO_MODE_00);
    mt_set_gpio_dir(gpio, GPIO_DIR_OUT);
    mt_set_gpio_out(gpio, GPIO_OUT_ONE);

    mtk_panel_delay(LCM_FUNC_RT4801H, MTK_LCM_UTIL_TYPE_MDELAY, 2);

    gpio = ctx_rt4801h.bias_neg_gpio;
    mt_set_gpio_mode(gpio, GPIO_MODE_00);
    mt_set_gpio_dir(gpio, GPIO_DIR_OUT);
    mt_set_gpio_out(gpio, GPIO_OUT_ONE);

    LCM_DBG("%s--\n", __func__);
    return ret;
}

static int rt4801h_power_off(void)
{
    u32 gpio = 0;

    if (ctx_rt4801h.init != 1) {
        LCM_MSG("%s gate ic is not initialized\n", __func__);
        return -1;
    }

    if (ctx_rt4801h.ref == 0) {
        LCM_MSG("%s gate ic (%u) already power off\n",
            __func__, ctx_rt4801h.ref);
        return 0;
    }

    LCM_DBG("%s++\n", __func__);
    ctx_rt4801h.ref--;
    if (ctx_rt4801h.ref > 0) {
        LCM_MSG("%s, do nothing, there are other users, %u\n",
            __func__, ctx_rt4801h.ref);
        return 0;
    }

    if (ctx_rt4801h.bias_pos_gpio < 0 ||
        ctx_rt4801h.bias_neg_gpio < 0) {
        LCM_MSG("%s: cannot get bias %d, %d\n",
            __func__, ctx_rt4801h.bias_pos_gpio,
            ctx_rt4801h.bias_neg_gpio);
        return -EFAULT;
    }

    gpio = ctx_rt4801h.bias_neg_gpio;
    mt_set_gpio_mode(gpio, GPIO_MODE_00);
    mt_set_gpio_dir(gpio, GPIO_DIR_OUT);
    mt_set_gpio_out(gpio, GPIO_OUT_ZERO);

    mtk_panel_delay(LCM_FUNC_RT4801H, MTK_LCM_UTIL_TYPE_MDELAY, 1);

    gpio = ctx_rt4801h.bias_pos_gpio;
    mt_set_gpio_mode(gpio, GPIO_MODE_00);
    mt_set_gpio_dir(gpio, GPIO_DIR_OUT);
    mt_set_gpio_out(gpio, GPIO_OUT_ZERO);

    LCM_DBG("%s--\n", __func__);
    return 0;
}

static int rt4801h_set_voltage(unsigned int level)
{
    int ret = 0;

    if (ctx_rt4801h.init != 1) {
        LCM_MSG("%s gate ic is not initialized\n", __func__);
        return -1;
    }

    if (ctx_rt4801h.ref == 0) {
        LCM_MSG("%s gate ic (%u) is power off\n", __func__, ctx_rt4801h.ref);
        return -2;
    }

    if (level < RT4801H_VOL_MIN_LEVEL || level > RT4801H_VOL_MAX_LEVEL) {
        LCM_MSG("%s invalid voltage level:%d\n", __func__, level);
        return -3;
    }

    LCM_MSG("%s++, level:%u, id:0x%x\n",
        __func__, level, RT4801H_VOL_REG_VALUE(level));
    ret = mtk_panel_i2c_write_bytes(&ctx_rt4801h.client, 0, RT4801H_VOL_REG_VALUE(level));
    if (ret)
        return ret;
    ret = mtk_panel_i2c_write_bytes(&ctx_rt4801h.client, 1, RT4801H_VOL_REG_VALUE(level));

    LCM_DBG("%s--\n", __func__);
    return ret;
}

static int rt4801h_gateic_parse_pin(const void *fdt)
{
    int ret = 0;

    ctx_rt4801h.reset_gpio = mtk_panel_get_pin(fdt, "lcm_rst_out0_gpio");
    if (ctx_rt4801h.reset_gpio < 0) {
        LCM_MSG("%s: failed to parse reset pin\n", __func__);
        ret = -EFAULT;
    }

    ctx_rt4801h.bias_pos_gpio = mtk_panel_get_pin(fdt, "lcd_bias_enp0_gpio");
    if (ctx_rt4801h.bias_pos_gpio < 0) {
        LCM_MSG("%s: failed to parse enpos pin\n", __func__);
        ret = -EFAULT;
    }

    ctx_rt4801h.bias_neg_gpio = mtk_panel_get_pin(fdt, "lcd_bias_enn0_gpio");
    if (ctx_rt4801h.bias_neg_gpio < 0) {
        LCM_MSG("%s: failed to parse reset pin\n", __func__);
        ret = -EFAULT;
    }

    LCM_DBG("%s, reset gpio:0x%x, pos gpio:0x%x, neg gpio:0x%x\n",
        __func__, ctx_rt4801h.reset_gpio, ctx_rt4801h.bias_pos_gpio, ctx_rt4801h.bias_neg_gpio);
    return ret;
}

struct mt_i2c *rt4801h_get_i2c_client(void)
{
    if (ctx_rt4801h.init == 0) {
        LCM_MSG("%s, %d, error: has not been initialized\n",
            __func__, __LINE__);
        return NULL;
    }

    return &ctx_rt4801h.client;
}

static struct mtk_gateic_funcs rt4801h_ops = {
    .reset = rt4801h_reset,
    .power_on = rt4801h_power_on,
    .power_off = rt4801h_power_off,
    .set_voltage = rt4801h_set_voltage,
    .get_i2c_client = rt4801h_get_i2c_client,
};

int rt4801h_gateic_init(const void *fdt, char func, const char *lcm_name)
{
    int ret = 0, i = 0, len = 0, count = 0;
    int id = 0, addr = 0, offset = 0;
    const char *lcm_tmp;

    if (func != LCM_FUNC_RT4801H || lcm_name == NULL)
        return -EFAULT;

    if (ctx_rt4801h.init == 1) {
        if (mtk_gateic_match_lcm_list(lcm_name, ctx_rt4801h.lcm_list,
            ctx_rt4801h.lcm_count, "rt4801h") == true)
            return 0;
        else
            return -EFAULT;
    }

    LCM_DBG("%s++, lcm:%s\n", __func__, lcm_name);

    offset = fdt_node_offset_by_compatible(fdt, -1,
        "mediatek,mtk-drm-gateic-drv-rt4801h");
    if (offset < 0) {
        LCM_MSG("%s, failed to get rt4801h dts, ret:%d\n",
            __func__, ret);
        return offset;
    }
    count = fdt_stringlist_count(fdt, offset, "panel-list");
    ctx_rt4801h.lcm_list = malloc(ROUNDUP(count * sizeof(char *), 4));
    if (count > 0) {
        for (i = 0; i < count; i++) {
            LCM_MALLOC(ctx_rt4801h.lcm_list[i], MTK_LCM_NAME_LENGTH);
            if (ctx_rt4801h.lcm_list[i] == NULL) {
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
            ret = snprintf(ctx_rt4801h.lcm_list[i],
                MTK_LCM_NAME_LENGTH - 1, "%s", lcm_tmp);
            if (ret < 0 ||
                ret >= MTK_LCM_NAME_LENGTH) {
                LCM_MSG("%s, %d, failed to get panel_list:%d, len:%d, ret:%d\n",
                    __func__, __LINE__, i, len, ret);
                ret = -EFAULT;
                goto end;
            }
        }
        ctx_rt4801h.lcm_count = (unsigned int)i;
    } else {
        LCM_MSG("%s, %d, failed to get panel-list, %d\n",
            __func__, __LINE__, count);
        return -EFAULT;
    }

    if (ctx_rt4801h.lcm_count > 0 &&
        mtk_gateic_match_lcm_list(lcm_name, ctx_rt4801h.lcm_list,
            ctx_rt4801h.lcm_count, "rt4801h") == false) {
        ret = -EFAULT;
        goto end;
    }

    if (mtk_panel_get_i2c_lcd_bias(fdt, &id, &addr)) {
        LCM_MSG("%s: failed to get i2c data:%d, %d\n",
            __func__, id, addr);
        return -EFAULT;
    }

    ctx_rt4801h.client.id = (unsigned short)(id & 0xFFFF);
    ctx_rt4801h.client.addr = (unsigned short)addr;
    ctx_rt4801h.client.mode = RT4801H_LCM_I2C_MODE;
    ctx_rt4801h.client.speed = RT4801H_LCM_I2C_SPEED;
    ctx_rt4801h.init = 1;
    ctx_rt4801h.ref = 0;
    LCM_DBG("%s, i2c id:%u, addr:0x%x, mode:%u, speed:%u\n",
        __func__, ctx_rt4801h.client.id, ctx_rt4801h.client.addr,
        ctx_rt4801h.client.mode, ctx_rt4801h.client.speed);

    ret = rt4801h_gateic_parse_pin(fdt);
    if (ret != 0) {
        LCM_MSG("%s: failed to get gateic pins of rt4801h:%d\n",
            __func__, ret);
        return ret;
    }

    ret = mtk_lcm_gateic_register(&rt4801h_ops, func);
    LCM_DBG("%s: set gateic ops:0x%lx, %d\n",
        __func__, (unsigned long)&rt4801h_ops, ret);
    if (ret != 0) {
        LCM_MSG("%s: failed to set gateic ops of rt4801h:%d\n",
            __func__, ret);
        return ret;
    }

    LCM_DBG("%s--, %d\n", __func__, ret);

    return ret;

end:
    for (i = 0; i < count; i++) {
        if (ctx_rt4801h.lcm_list[i] == NULL)
            break;
        LCM_FREE(ctx_rt4801h.lcm_list[i],
            MTK_LCM_NAME_LENGTH);
    }
    ctx_rt4801h.lcm_count = 0;
    free(ctx_rt4801h.lcm_list);
    return ret;

}
#else

int rt4801h_gateic_init(const void *fdt, char func, const char *lcm_name)
{
    LCM_MSG("%s, bypass power control\n", __func__);
    return 0;
}

#endif
