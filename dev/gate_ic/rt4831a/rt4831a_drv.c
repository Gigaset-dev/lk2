/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#ifdef MTK_RT4831A_SUPPORT

#include "rt4831a_drv.h"

//#include <platform/mt_gpio.h>
#include <gpio_api.h>

//#include <platform/mt_i2c.h>
#include <mt_i2c.h>

//#include <platform/mt_gpt.h>
//#include <platform/mt_typedefs.h>

#include <libfdt.h>
//#include <lk_builtin_dtb.h>

#include <debug.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

/* i2c control start */
extern void *g_fdt;
#define RT4831A_I2C_MODE   0
#define RT4831A_I2C_SPEED  400
#define MAX_REG_SIZE       64
#define I2C_OK             0x0000

static struct mt_i2c rt4831a_i2c;
static unsigned int is_inited;
static unsigned int last_brightness;

enum BACKLIGHT_REG {
    BACKLIGHT_CONFIG_1 = 0x2,
    BACKLIGHT_CONFIG_2 = 0x3,
    BACKLIGHT_BRIGHTNESS_LSB = 0x4,
    BACKLIGHT_BRIGHTNESS_MSB = 0x5,
    BACKLIGHT_AUTO_FREQ_LOW = 0x6,
    BACKLIGHT_AUTO_FREQ_HIGH = 0x7,
    BACKLIGHT_ENABLE = 0x8,
    BACKLIGHT_OPTION_1 = 0x10,
    BACKLIGHT_OPTION_2 = 0x11,
    BACKLIGHT_SMOOTH = 0x14,
    BACKLIGHT_MAX_NUM = 0x15
};

static int util_get_i2c_lcd_bias(void *fdt, int *id, int *addr)
{
    int offset;
    unsigned int *data = NULL;
    int len = 0;

    dprintf(0, "%s+\n", __func__);

    offset = fdt_node_offset_by_compatible(fdt, -1, "mediatek,gate-ic-i2c");
    if (offset < 0) {
        dprintf(0, "get fdt i2c_lcd_bias node fail\n");
        return -1;
    }
    data = (unsigned int *)fdt_getprop(fdt, offset, "reg", &len);
    if (!data || !len) {
        dprintf(0, "get i2c_lcd_bias reg fail\n");
        return -1;
    }
    *addr = fdt32_to_cpu(*(unsigned int *)data);
    //offset = fdt_parent_offset(fdt, offset);
    data = (unsigned int *)fdt_getprop(fdt, offset, "id", &len);
    if (!data || !len) {
        dprintf(0, "get i2c_lcd_bias id fail\n");
        return -1;
    }
    *id = fdt32_to_cpu(*(unsigned int *)data);

    dprintf(0, "%s-: addr = %d, id = %d\n", __func__, *addr, *id);

    return 0;
}

int util_get_drv_pin(void *fdt, char *pin_name)
{
    int offset, idx, phandle;
    unsigned int *data = NULL;
    int len = 0;
    char pinctrl_name[128] = {'\0'};

    dprintf(0, "%s+\n", __func__);

    offset = fdt_node_offset_by_compatible(fdt, -1, "mediatek,mtkfb");
    if (offset < 0) {
        dprintf(INFO, "get fdt mtkfb node fail\n");
        return -1;
    }

    idx = fdt_stringlist_search(fdt, offset, "pinctrl-names", pin_name);
    if (idx < 0) {
        dprintf(INFO, "get idx fail. pinctrl_name:%s\n", pinctrl_name);
        return -1;
    }

    snprintf(pinctrl_name, (sizeof(pinctrl_name) - 1), "pinctrl-%d", idx);

    data = (unsigned int *)fdt_getprop(fdt, offset, pinctrl_name, &len);
    if (!data || !len) {
        dprintf(INFO, "get lcm pinctrl fail. pinctrl_name:%s\n", pinctrl_name);
        return -1;
    }

    phandle = fdt32_to_cpu(*(unsigned int *)data);

    offset = fdt_node_offset_by_phandle(fdt, phandle);
    if (offset < 0) {
        dprintf(INFO, "get lcm pinctrl fail. phandle:0x%x\n", phandle);
        return -1;
    }

    offset = fdt_subnode_offset(fdt, offset, "pins_cmd_dat");
    if (offset < 0) {
        dprintf(INFO, "get pins_cmd_dat node fail\n");
        return -1;
    }

    data = (unsigned int *)fdt_getprop(fdt, offset, "pinmux", &len);
    if (!data || !len) {
        dprintf(INFO, "get pinmux fail\n");
        return -1;
    }

    /* Pin define format: 0~7->pin status, 8~15->pin id */
    return fdt32_to_cpu(*(unsigned int *)data) >> 8;
}

#define PIN_ENCRYPT 0x80000000

long gate_ic_power_status_setting(void *fdt, unsigned int value)
{
    long ret = 0;
    int lcd_en_pin;

#ifdef MACH_FPGA
    DDPMSG("In FPGA stage, no need to control gate power ic by gpio\n");
#else

    dprintf(0, "%s+\n", __func__);

    if (fdt == NULL) {
        dprintf(CRITICAL, "fdt is NULL!\n");
        return -1;
    }

    lcd_en_pin = util_get_drv_pin(fdt, "lcm_led_en0_gpio");
    if (lcd_en_pin < 0)
        return -1;

    //lcd_en_pin |= PIN_ENCRYPT;
    if (value) { /* power on gate power ic */
        mt_set_gpio_mode(lcd_en_pin, GPIO_MODE_00);
        mt_set_gpio_dir(lcd_en_pin, GPIO_DIR_OUT);
        mt_set_gpio_out(lcd_en_pin, GPIO_OUT_ONE);
    } else { /* power off gate power ic */
        mt_set_gpio_mode(lcd_en_pin, GPIO_MODE_00);
        mt_set_gpio_dir(lcd_en_pin, GPIO_DIR_OUT);
        mt_set_gpio_out(lcd_en_pin, GPIO_OUT_ZERO);
    }
#endif


    return ret;
}

int _gate_ic_i2c_write_bytes(void *fdt, unsigned char addr, unsigned char value)
{
    signed int ret_code = I2C_OK;
    unsigned char write_data[2];
    unsigned char read_data[2];
    int id, i2c_addr;

    dprintf(0, "%s+\n", __func__);

    write_data[0] = addr;
    write_data[1] = value;
    read_data[0] = write_data[0];
    read_data[1] = write_data[1];

    if (util_get_i2c_lcd_bias(fdt, &id, &i2c_addr) < 0)
        return -1;

    if (id < 0 || id > 0xFFFF)
        return -1;

    rt4831a_i2c.id = (unsigned short)(id & 0XFFFF);
    rt4831a_i2c.addr = (unsigned short)i2c_addr;
    rt4831a_i2c.mode = RT4831A_I2C_MODE;
    rt4831a_i2c.speed = RT4831A_I2C_SPEED;

    ret_code = mtk_i2c_write(rt4831a_i2c.id, rt4831a_i2c.addr, &write_data[0], 2, NULL);
    if (ret_code < 0)
        dprintf(0, "[LCM][ERROR] %s: %d\n", __func__, ret_code);
    mtk_i2c_write_read(rt4831a_i2c.id, rt4831a_i2c.addr, &read_data[0], 1, 1, NULL);
    return ret_code;
}

int _gate_ic_i2c_write_regs(void *fdt, unsigned char SubAddr,
                            unsigned char *Regs, unsigned short nRegs)
{
    u8 data[MAX_REG_SIZE];
    unsigned short len = nRegs + 1;
    signed int ret_code = I2C_OK;
    int id, i2c_addr;

    dprintf(0, "%s+\n", __func__);

    if (len > sizeof(data)) {
        dprintf(0, "[LCM][ERROR] %s: i2c wr: len=%d is too big!\n", __func__, nRegs);
        return -1;
    }

    data[0] = SubAddr;
    memcpy(&data[1], Regs, nRegs);

    if (util_get_i2c_lcd_bias(fdt, &id, &i2c_addr) < 0)
        return -1;

    if (id < 0 || id > 0xFFFF)
        return -1;

    rt4831a_i2c.id = (unsigned short)(id & 0XFFFF);
    rt4831a_i2c.addr = (unsigned short)i2c_addr;
    rt4831a_i2c.mode = RT4831A_I2C_MODE;
    rt4831a_i2c.speed = RT4831A_I2C_SPEED;

    ret_code = mtk_i2c_write(rt4831a_i2c.id, rt4831a_i2c.addr, data, len, NULL);
    if (ret_code < 0)
        dprintf(0, "[LCM][ERROR] %s: %d\n", __func__, ret_code);

    dprintf(0, "%s-\n", __func__);
    return ret_code;
}

void _gate_ic_i2c_panel_bias_enable(void *fdt, unsigned int power_status)
{
    dprintf(0, "%s+\n", __func__);
    if (fdt == NULL) {
        dprintf(CRITICAL, "fdt is NULL!\n");
        return;
    }

    if (power_status) {
        if (is_inited) {
            dprintf(INFO, "panel bias is already enabled, so skip this\n");
            return;
        }
        _gate_ic_i2c_write_bytes(fdt, 0x0a, 0x11);
        _gate_ic_i2c_write_bytes(fdt, 0x0b, 0x00);

        /*set bias to 5.4v*/
        _gate_ic_i2c_write_bytes(fdt, 0x0c, 0x24);
        _gate_ic_i2c_write_bytes(fdt, 0x0d, 0x1c);
        _gate_ic_i2c_write_bytes(fdt, 0x0e, 0x1c);

        /* set dsv FPWM mode */
        _gate_ic_i2c_write_bytes(fdt, 0xF0, 0x69);
        _gate_ic_i2c_write_bytes(fdt, 0xB1, 0x6c);
        /*bias enable*/
        _gate_ic_i2c_write_bytes(fdt, 0x09, 0x9e);
        is_inited = 1;
    } else {
        _gate_ic_i2c_write_bytes(fdt, 0x09, 0x18);
        is_inited = 0;
    }

}

int _gate_ic_i2c_backlight_enable(void *fdt, unsigned int is_enable, unsigned int pwm_enable)
{
    unsigned char config_Regs[BACKLIGHT_MAX_NUM];
    int ret = 0;

    dprintf(0, "%s+\n", __func__);
    if (fdt == NULL) {
        dprintf(CRITICAL, "fdt is NULL!\n");
        return -1;
    }

    if (!is_enable) {
        _gate_ic_i2c_write_bytes(fdt, BACKLIGHT_ENABLE, 0x00);
        return ret;
    }
    if (pwm_enable) {
        config_Regs[BACKLIGHT_CONFIG_1] = 0x6B;
        last_brightness = 2047;
    } else {
        config_Regs[BACKLIGHT_CONFIG_1] = 0x68;
    }

    config_Regs[BACKLIGHT_CONFIG_2] = 0x9D;
    config_Regs[BACKLIGHT_BRIGHTNESS_LSB] = last_brightness & 0x7;
    config_Regs[BACKLIGHT_BRIGHTNESS_MSB] = (last_brightness & 0x7F8) >> 3;
    config_Regs[BACKLIGHT_AUTO_FREQ_LOW] = 0;
    config_Regs[BACKLIGHT_AUTO_FREQ_HIGH] = 0;
    config_Regs[BACKLIGHT_ENABLE] = 0x15;

    config_Regs[BACKLIGHT_OPTION_1] = 0x06;
    config_Regs[BACKLIGHT_OPTION_2] = 0xB7;
    ret |= _gate_ic_i2c_write_regs(fdt, BACKLIGHT_CONFIG_1, &config_Regs[BACKLIGHT_CONFIG_1], 7);
    ret |= _gate_ic_i2c_write_regs(fdt, BACKLIGHT_OPTION_1, &config_Regs[BACKLIGHT_OPTION_1], 2);
    return ret;

}

int _gate_ic_i2c_backight_level_set(void *fdt, unsigned int brightness)
{

    unsigned char m_Regs[2];

    dprintf(0, "%s+\n", __func__);

    if (fdt == NULL) {
        dprintf(CRITICAL, "fdt is NULL!\n");
        return -1;
    }

    if (last_brightness == 0 && brightness != 0) {
        last_brightness = brightness;
        return _gate_ic_i2c_backlight_enable(fdt, 1, 0);
    }
    m_Regs[0] = brightness & 0x7;
    m_Regs[1] = (brightness & 0x7F8) >> 3;

    last_brightness = brightness;

    return _gate_ic_i2c_write_regs(fdt, BACKLIGHT_BRIGHTNESS_LSB, m_Regs, 2);

}
/* i2c control end */
#endif

