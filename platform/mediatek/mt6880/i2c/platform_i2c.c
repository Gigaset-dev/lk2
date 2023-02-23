/*
 * Copyright (C) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <mt_i2c.h>
#include <platform/addressmap.h>
#include <platform_i2c.h>

enum gpio_list {
    SCL0_GPIO = 106,
    SDA0_GPIO = 105,
    SCL1_GPIO = 108,
    SDA1_GPIO = 107,
    SCL2_GPIO = 110,
    SDA2_GPIO = 109,
    SCL3_GPIO = 198,
    SDA3_GPIO = 197,
    SCL4_GPIO = 80,
    SDA4_GPIO = 79,
    SCL5_GPIO = 82,
    SDA5_GPIO = 81
};

enum {
    I2C0 = 0,
    I2C1 = 1,
    I2C2 = 2,
    I2C3 = 3,
    I2C4 = 4,
    I2C5 = 5
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

    /********i2c4 gpio init***********/
    set_gpio_i2c_mode(SCL4_GPIO, I2C_MODE, INTERNAL_PULL_UP);
    set_gpio_i2c_mode(SDA4_GPIO, I2C_MODE, INTERNAL_PULL_UP);

    /********i2c5 gpio init***********/
    set_gpio_i2c_mode(SCL5_GPIO, I2C_MODE, INTERNAL_PULL_UP);
    set_gpio_i2c_mode(SDA5_GPIO, I2C_MODE, INTERNAL_PULL_UP);
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
    mtk_i2c[I2C4].base = I2C4_BASE;
    mtk_i2c[I2C4].pdmabase = I2C4_APDMA_BASE;
    mtk_i2c[I2C5].base = I2C5_BASE;
    mtk_i2c[I2C5].pdmabase = I2C5_APDMA_BASE;
}
