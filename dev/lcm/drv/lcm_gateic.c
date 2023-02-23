/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include "lcm_gateic.h"

static struct mtk_gateic_funcs *mtk_gateic_dbi_ops;
static struct mtk_gateic_funcs *mtk_gateic_dpi_ops;
static struct mtk_gateic_funcs *mtk_gateic_dsi_ops;

bool mtk_gateic_match_lcm_list(const char *lcm_name,
    char **list, unsigned int count, const char *gateic_name)
{
    unsigned int i = 0;

    if (count == 1 &&
        strcmp(*list, "default") == 0) {
        LCM_MSG("%s, use default gateic:\"%s\"\n",
            __func__, gateic_name);
        return true;
    }

    for (i = 0; i < count; i++) {
        if (strcmp(*(list + i), lcm_name) == 0) {
            LCM_MSG("%s, lcm%d:\"%s\" of gateic:\"%s\" matched\n",
                __func__, i, *(list + i), gateic_name);
            return true;
        }
    }
    LCM_MSG("%s, gateic:\"%s\" doesn't support lcm:\"%s\", count:%u\n",
        __func__, gateic_name, lcm_name, count);
    return false;
}

static struct mtk_gateic_funcs *mtk_lcm_gateic_get_ops(char func)
{
    struct mtk_gateic_funcs *ops = NULL;

    switch (func) {
    case MTK_LCM_FUNC_DBI:
        ops = mtk_gateic_dbi_ops;
        break;
    case MTK_LCM_FUNC_DPI:
        ops = mtk_gateic_dpi_ops;
        break;
    case MTK_LCM_FUNC_DSI:
        ops = mtk_gateic_dsi_ops;
        break;
    default:
        LCM_DBG("%s: invalid func:%d\n", __func__, func);
        ops = NULL;
        break;
    }

    return ops;
}

int mtk_lcm_gateic_register(struct mtk_gateic_funcs *gateic_ops,
        char func)
{
    int ret = 0;

    if (gateic_ops == NULL)
        return -EFAULT;

    switch (func) {
    case MTK_LCM_FUNC_DBI:
        if (mtk_gateic_dbi_ops != NULL)
            LCM_MSG("%s, DBI gateic update ops\n", __func__);
        mtk_gateic_dbi_ops = gateic_ops;
        break;
    case MTK_LCM_FUNC_DPI:
        if (mtk_gateic_dpi_ops != NULL)
            LCM_MSG("%s, DPI gateic update ops\n", __func__);
        mtk_gateic_dpi_ops = gateic_ops;
        break;
    case MTK_LCM_FUNC_DSI:
        if (mtk_gateic_dsi_ops != NULL)
            LCM_MSG("%s, DSI gateic update ops\n", __func__);
        mtk_gateic_dsi_ops = gateic_ops;
        break;
    default:
        LCM_DBG("%s: invalid func:%d\n", __func__, func);
        ret = -EINVAL;
        break;
    }

    return ret;
}

struct mtk_gateic_funcs *mtk_lcm_gateic_get(char func)
{
    return mtk_lcm_gateic_get_ops(func);
}

int mtk_lcm_gateic_power_on(char func)
{
#ifndef PROJECT_TYPE_FPGA
    struct mtk_gateic_funcs *ops  = mtk_lcm_gateic_get_ops(func);

    if (ops == NULL || ops->power_on == NULL)
        return -EFAULT;

    return ops->power_on();
#else
    return 0;
#endif
}

int mtk_lcm_gateic_power_off(char func)
{
#ifndef PROJECT_TYPE_FPGA
    struct mtk_gateic_funcs *ops  = mtk_lcm_gateic_get_ops(func);

    if (ops == NULL || ops->power_off == NULL)
        return -EFAULT;

    return ops->power_off();
#else
    return 0;
#endif
}

int mtk_lcm_gateic_set_voltage(unsigned int level,
        char func)
{
#ifndef PROJECT_TYPE_FPGA
    struct mtk_gateic_funcs *ops  = mtk_lcm_gateic_get_ops(func);

    if (ops == NULL || ops->set_voltage == NULL)
        return -EFAULT;

    return ops->set_voltage(level);
#else
    return 0;
#endif
}

int mtk_lcm_gateic_reset(int on, char func)
{
#ifndef PROJECT_TYPE_FPGA
    struct mtk_gateic_funcs *ops  = mtk_lcm_gateic_get_ops(func);

    if (ops == NULL || ops->reset == NULL)
        return -EFAULT;

    return ops->reset(on);
#else
    return 0;
#endif
}

int mtk_lcm_gateic_set_backlight(unsigned int level,
        char func)
{
#ifndef PROJECT_TYPE_FPGA
    struct mtk_gateic_funcs *ops  = mtk_lcm_gateic_get_ops(func);

    if (ops == NULL || ops->set_backlight == NULL)
        return -EFAULT;

    return ops->set_backlight(level);
#else
    return 0;
#endif
}

int mtk_lcm_gateic_enable_backlight(unsigned int enable,
        unsigned int pwm_enable, char func)
{
#ifndef PROJECT_TYPE_FPGA
    struct mtk_gateic_funcs *ops  = mtk_lcm_gateic_get_ops(func);

    if (ops == NULL || ops->enable_backlight == NULL)
        return -EFAULT;

    return ops->enable_backlight(enable, pwm_enable);
#else
    return 0;
#endif
}

int mtk_lcm_gateic_i2c_write_bytes(unsigned char addr, unsigned char value, char func)
{
#ifndef PROJECT_TYPE_FPGA
    struct mtk_gateic_funcs *ops  = mtk_lcm_gateic_get_ops(func);
    struct mt_i2c *client = NULL;

    if (ops == NULL || ops->get_i2c_client == NULL)
        return -EFAULT;

    client = ops->get_i2c_client();
    if (client == NULL)
        return -EFAULT;

    return mtk_panel_i2c_write_bytes(client, addr, value);
#else
    return 0;
#endif
}

int mtk_lcm_gateic_i2c_write_multiple_bytes(unsigned char addr,
         unsigned char *value, unsigned int size, char func)
{
#ifndef PROJECT_TYPE_FPGA
    struct mtk_gateic_funcs *ops  = mtk_lcm_gateic_get_ops(func);
    struct mt_i2c *client = NULL;

    if (ops == NULL || ops->get_i2c_client == NULL)
        return -EFAULT;

    client = ops->get_i2c_client();
    if (client == NULL)
        return -EFAULT;

    return mtk_panel_i2c_write_multiple_bytes(client, addr, value, size);
#else
    return 0;
#endif
}

int mtk_lcm_power_status_setting(unsigned int value, char func)
{
    if (value == 0)
        return mtk_lcm_gateic_power_off(func);
    return mtk_lcm_gateic_power_on(func);
}

/*********** led interfaces define **********/
int _gate_ic_i2c_write_bytes(void *fdt, unsigned char addr, unsigned char value)
{
    LCM_DBG("%s++, addr:0x%x, value:0x%x\n", __func__, addr, value);
    return mtk_lcm_gateic_write_bytes(addr, value, MTK_LCM_FUNC_DSI);
}

int _gate_ic_i2c_write_regs(void *fdt, unsigned char addr,
         unsigned char *value, unsigned short size)
{
    LCM_DBG("%s++, addr:0x%x, size:%u\n", __func__, addr, size);
    return mtk_lcm_gateic_write_multiple_bytes(addr, value, size,
                MTK_LCM_FUNC_DSI);
}

void _gate_ic_i2c_panel_bias_enable(void *fdt, unsigned int power_status)
{
}

long gate_ic_power_status_setting(void *fdt, unsigned int value)
{
    LCM_DBG("%s++, value:%u\n", __func__, value);
    return mtk_lcm_power_status_setting(value, MTK_LCM_FUNC_DSI);
}

int _gate_ic_i2c_backlight_enable(void *fdt, unsigned int enable, unsigned int pwm_enable)
{
    LCM_DBG("%s ++, enable:%u, pwm:%u\n",
        __func__, enable, pwm_enable);
    return mtk_lcm_gateic_enable_backlight(enable, pwm_enable,
                MTK_LCM_FUNC_DSI);
}

int _gate_ic_i2c_backight_level_set(void *fdt, unsigned int level)
{
    LCM_DBG("%s++, level:%u\n", __func__, level);
    return mtk_lcm_gateic_set_backlight(level, MTK_LCM_FUNC_DSI);
}
/************************************/

int mtk_lcm_gateic_drv_init(const void *fdt, char func, const char *lcm_name)
{
    int ret = -1;

#ifdef MTK_LCM_GATEIC_RT4801H_SUPPORT
    ret = rt4801h_gateic_init(fdt, func, lcm_name);
    if (ret == 0)
        goto end;
#endif
#ifdef MTK_LCM_GATEIC_RT4831A_SUPPORT
    ret = rt4831a_gateic_init(fdt, func, lcm_name);
    if (ret == 0)
        goto end;
#endif

end:
    return ret;
}

