/*
 * Copyright (C) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <mt_i2c.h>
#include <platform/addressmap.h>
#include <platform_i2c.h>

enum gpio_list {
    SDA0_GPIO = 57,
    SCL0_GPIO = 58,
    SDA1_GPIO = 59,
    SCL1_GPIO = 60,
    SDA2_GPIO = 61,
    SCL2_GPIO = 62,
    SDA3_GPIO = 63,
    SCL3_GPIO = 64
};

enum {
    I2C0 = 0,
    I2C1 = 1,
    I2C2 = 2,
    I2C3 = 3
};

/* i2c default setting */
struct mt_i2c mtk_i2c[I2C_NR] = {
    {
        .speed = 100,
        .st_rs = I2C_TRANS_STOP,
        .dma_en = false,
        .poll_en = true,
        .filter_msg = false,
        .pushpull = false,
        .clk = I2C_CLK_RATE,
    },
    {
        .speed = 100,
        .st_rs = I2C_TRANS_STOP,
        .dma_en = false,
        .poll_en = true,
        .filter_msg = false,
        .pushpull = false,
        .clk = I2C_CLK_RATE,
    },
    {
        .speed = 100,
        .st_rs = I2C_TRANS_STOP,
        .dma_en = false,
        .poll_en = true,
        .filter_msg = false,
        .pushpull = false,
        .clk = I2C_CLK_RATE,
    },
    {
        .speed = 100,
        .st_rs = I2C_TRANS_STOP,
        .dma_en = false,
        .poll_en = true,
        .filter_msg = false,
        .pushpull = false,
        .clk = I2C_CLK_RATE,
    },
};

void i2c_gpio_default_init(void)
{
    /********i2c0 gpio init***********/
    set_gpio_i2c_mode(SCL0_GPIO, I2C_MODE, INTERNAL_PULL_UP);
    set_gpio_i2c_mode(SDA0_GPIO, I2C_MODE, INTERNAL_PULL_UP);

    /********i2c1 gpio init***********/
    set_gpio_i2c_mode(SCL1_GPIO, I2C_MODE, INTERNAL_PULL_UP);
    set_gpio_i2c_mode(SDA1_GPIO, I2C_MODE, INTERNAL_PULL_UP);

    /********i2c2 gpio init***********/
    set_gpio_i2c_mode(SCL2_GPIO, I2C_MODE, INTERNAL_PULL_UP);
    set_gpio_i2c_mode(SDA2_GPIO, I2C_MODE, INTERNAL_PULL_UP);

    /********i2c3 gpio init***********/
    set_gpio_i2c_mode(SCL3_GPIO, I2C_MODE, INTERNAL_PULL_UP);
    set_gpio_i2c_mode(SDA3_GPIO, I2C_MODE, INTERNAL_PULL_UP);
}

void i2c_config_init(void)
{
    mtk_i2c[I2C0].base = I2C0_BASE;
    mtk_i2c[I2C0].pdmabase = I2C0_APDMA_BASE;
    mtk_i2c[I2C1].base = I2C1_BASE;
    mtk_i2c[I2C1].pdmabase = I2C1_APDMA_BASE;
    mtk_i2c[I2C2].base = I2C2_BASE;
    mtk_i2c[I2C2].pdmabase = I2C2_APDMA_BASE;
    mtk_i2c[I2C3].base = I2C3_BASE;
    mtk_i2c[I2C3].pdmabase = I2C3_APDMA_BASE;
}
