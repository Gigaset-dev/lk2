/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <mt_i2c.h>
#include <platform/addressmap.h>
#include <platform_i2c.h>

enum gpio_list {
    SDA0_GPIO = 8,
    SCL0_GPIO = 9,
    SDA1_GPIO = 10,
    SCL1_GPIO = 11,
    SDA2_GPIO = 12,
    SCL2_GPIO = 13,
    SDA3_GPIO = 14,
    SCL3_GPIO = 15,
    SDA4_GPIO = 16,
    SCL4_GPIO = 17,
    SDA5_GPIO = 29,
    SCL5_GPIO = 30,
    SDA6_GPIO = 25,
    SCL6_GPIO = 26,
    SDA7_GPIO = 27,
    SCL7_GPIO = 28,
};

enum {
    I2C0 = 0,
    I2C1 = 1,
    I2C2 = 2,
    I2C3 = 3,
    I2C4 = 4,
    I2C5 = 5,
    I2C6 = 6,
    I2C7 = 7,
};

/* i2c default setting */
struct mt_i2c mtk_i2c[I2C_NR] = {
    {   /* I2C0 */
        .speed = 400,
        .st_rs = I2C_TRANS_STOP,
        .dma_en = false,
        .poll_en = true,
        .filter_msg = false,
        .pushpull = false,
        .clk = I2C_CLK_RATE,
    },
    {   /* I2C1 */
        .speed = 400,
        .st_rs = I2C_TRANS_STOP,
        .dma_en = false,
        .poll_en = true,
        .filter_msg = false,
        .pushpull = false,
        .clk = I2C_CLK_RATE,
    },
    {   /* I2C2 */
        .speed = 400,
        .st_rs = I2C_TRANS_STOP,
        .dma_en = false,
        .poll_en = true,
        .filter_msg = false,
        .pushpull = false,
        .clk = I2C_CLK_RATE,
    },
    {   /* I2C3 */
        .speed = 400,
        .st_rs = I2C_TRANS_STOP,
        .dma_en = false,
        .poll_en = true,
        .filter_msg = false,
        .pushpull = false,
        .clk = I2C_CLK_RATE,
    },
    {   /* I2C4 */
        .speed = 400,
        .st_rs = I2C_TRANS_STOP,
        .dma_en = false,
        .poll_en = true,
        .filter_msg = false,
        .pushpull = false,
        .clk = I2C_CLK_RATE,
    },
    {   /* I2C5 */
        .speed = 400,
        .st_rs = I2C_TRANS_STOP,
        .dma_en = false,
        .poll_en = true,
        .filter_msg = false,
        .pushpull = false,
        .clk = I2C_CLK_RATE,
    },
    {   /* I2C6 */
        .speed = 400,
        .st_rs = I2C_TRANS_STOP,
        .dma_en = false,
        .poll_en = true,
        .filter_msg = false,
        .pushpull = false,
        .clk = I2C_CLK_RATE,
    },
    {   /* I2C7 */
        .speed = 400,
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
    set_gpio_i2c_mode(SCL0_GPIO, PINMUX_MODE1_I2C, INTERNAL_PULL_UP);
    set_gpio_i2c_mode(SDA0_GPIO, PINMUX_MODE1_I2C, INTERNAL_PULL_UP);

    /********i2c1 gpio init***********/
    set_gpio_i2c_mode(SCL1_GPIO, PINMUX_MODE1_I2C, INTERNAL_PULL_UP);
    set_gpio_i2c_mode(SDA1_GPIO, PINMUX_MODE1_I2C, INTERNAL_PULL_UP);

    /********i2c2 gpio init***********/
    set_gpio_i2c_mode(SCL2_GPIO, PINMUX_MODE1_I2C, INTERNAL_PULL_UP);
    set_gpio_i2c_mode(SDA2_GPIO, PINMUX_MODE1_I2C, INTERNAL_PULL_UP);

    /********i2c3 gpio init***********/
    set_gpio_i2c_mode(SCL3_GPIO, PINMUX_MODE1_I2C, INTERNAL_PULL_UP);
    set_gpio_i2c_mode(SDA3_GPIO, PINMUX_MODE1_I2C, INTERNAL_PULL_UP);

    /********i2c4 gpio init***********/
    set_gpio_i2c_mode(SCL4_GPIO, PINMUX_MODE1_I2C, INTERNAL_PULL_UP);
    set_gpio_i2c_mode(SDA4_GPIO, PINMUX_MODE1_I2C, INTERNAL_PULL_UP);

    /********i2c5 gpio init***********/
    set_gpio_i2c_mode(SCL5_GPIO, PINMUX_MODE3_I2C, INTERNAL_PULL_UP);
    set_gpio_i2c_mode(SDA5_GPIO, PINMUX_MODE3_I2C, INTERNAL_PULL_UP);

    /********i2c6 gpio config internal pull up for m1v1***********/
    set_gpio_i2c_mode(SCL6_GPIO, PINMUX_MODE4_I2C, INTERNAL_PULL_UP);
    set_gpio_i2c_mode(SDA6_GPIO, PINMUX_MODE4_I2C, INTERNAL_PULL_UP);

    /********i2c7 gpio init***********/
    set_gpio_i2c_mode(SCL7_GPIO, PINMUX_MODE5_I2C, EXTERNAL_PULL_UP);
    set_gpio_i2c_mode(SDA7_GPIO, PINMUX_MODE5_I2C, EXTERNAL_PULL_UP);
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
    mtk_i2c[I2C6].base = I2C6_BASE;
    mtk_i2c[I2C6].pdmabase = I2C6_APDMA_BASE;
    mtk_i2c[I2C7].base = I2C7_BASE;
    mtk_i2c[I2C7].pdmabase = I2C7_APDMA_BASE;
}
