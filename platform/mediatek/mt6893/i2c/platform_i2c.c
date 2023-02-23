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
    SCL0_GPIO = 204,
    SDA0_GPIO = 205,
    SCL1_GPIO = 118,
    SDA1_GPIO = 119,
    SCL2_GPIO = 141,
    SDA2_GPIO = 142,
    SCL3_GPIO = 160,
    SDA3_GPIO = 161,
    SCL4_GPIO = 139,
    SDA4_GPIO = 140,
    SCL5_GPIO = 202,
    SDA5_GPIO = 203,
    SCL6_GPIO = 200,
    SDA6_GPIO = 201,
    SCL7_GPIO = 124,
    SDA7_GPIO = 125,
    SCL8_GPIO = 122,
    SDA8_GPIO = 123,
    SCL9_GPIO = 120,
    SDA9_GPIO = 121,
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
    I2C8 = 8,
    I2C9 = 9,
};

/* i2c default setting */
struct mt_i2c mtk_i2c[I2C_NR] = {
    {
        .speed = 400,
        .st_rs = I2C_TRANS_STOP,
        .dma_en = false,
        .poll_en = true,
        .filter_msg = false,
        .pushpull = false,
        .clk = I2C_CLK_RATE,
    },
    {
        .speed = 400,
        .st_rs = I2C_TRANS_STOP,
        .dma_en = false,
        .poll_en = true,
        .filter_msg = false,
        .pushpull = false,
        .clk = I2C_CLK_RATE,
    },
    {
        .speed = 400,
        .st_rs = I2C_TRANS_STOP,
        .dma_en = false,
        .poll_en = true,
        .filter_msg = false,
        .pushpull = false,
        .clk = I2C_CLK_RATE,
    },
    {
        .speed = 400,
        .st_rs = I2C_TRANS_STOP,
        .dma_en = false,
        .poll_en = true,
        .filter_msg = false,
        .pushpull = false,
        .clk = I2C_CLK_RATE,
    },
    {
        .speed = 400,
        .st_rs = I2C_TRANS_STOP,
        .dma_en = false,
        .poll_en = true,
        .filter_msg = false,
        .pushpull = false,
        .clk = I2C_CLK_RATE,
    },
    {
        .speed = 400,
        .st_rs = I2C_TRANS_STOP,
        .dma_en = false,
        .poll_en = true,
        .filter_msg = false,
        .pushpull = false,
        .clk = I2C_CLK_RATE,
    },
    {
        .speed = 400,
        .st_rs = I2C_TRANS_STOP,
        .dma_en = false,
        .poll_en = true,
        .filter_msg = false,
        .pushpull = false,
        .clk = I2C_CLK_RATE,
    },
    {
        .speed = 400,
        .st_rs = I2C_TRANS_STOP,
        .dma_en = false,
        .poll_en = true,
        .filter_msg = false,
        .pushpull = false,
        .clk = I2C_CLK_RATE,
    },
    {
        .speed = 400,
        .st_rs = I2C_TRANS_STOP,
        .dma_en = false,
        .poll_en = true,
        .filter_msg = false,
        .pushpull = false,
        .clk = I2C_CLK_RATE,
    },
    {
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
    set_gpio_i2c_mode(SCL0_GPIO, I2C_MODE, INTERNAL_PULL_UP);
    set_gpio_i2c_mode(SDA0_GPIO, I2C_MODE, INTERNAL_PULL_UP);

    /********i2c1 gpio init***********/
    //set_gpio_i2c_mode(SCL1_GPIO, I2C_MODE, INTERNAL_PULL_UP);
    //set_gpio_i2c_mode(SDA1_GPIO, I2C_MODE, INTERNAL_PULL_UP);

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

    /********i2c6 gpio init***********/
    set_gpio_i2c_mode(SCL6_GPIO, I2C_MODE, INTERNAL_PULL_UP);
    set_gpio_i2c_mode(SDA6_GPIO, I2C_MODE, INTERNAL_PULL_UP);

    /********i2c7 gpio init***********/
    set_gpio_i2c_mode(SCL7_GPIO, I2C_MODE, INTERNAL_PULL_UP);
    set_gpio_i2c_mode(SDA7_GPIO, I2C_MODE, INTERNAL_PULL_UP);

    /********i2c8 gpio init***********/
    set_gpio_i2c_mode(SCL8_GPIO, I2C_MODE, INTERNAL_PULL_UP);
    set_gpio_i2c_mode(SDA8_GPIO, I2C_MODE, INTERNAL_PULL_UP);

    /********i2c9 gpio init***********/
    set_gpio_i2c_mode(SCL9_GPIO, I2C_MODE, INTERNAL_PULL_UP);
    set_gpio_i2c_mode(SDA9_GPIO, I2C_MODE, INTERNAL_PULL_UP);

}

void i2c_config_init(void)
{
    mtk_i2c[I2C0].base = I2C0_BASE;
    mtk_i2c[I2C0].pdmabase = I2C0_APDMA_BASE;
    mtk_i2c[I2C1].base = I2C1_BASE;
    mtk_i2c[I2C1].pdmabase = I2C1_APDMA_BASE;
    mtk_i2c[I2C2].base = I2C2_BASE + I2C2_BASE_OFFSET;
    mtk_i2c[I2C2].pdmabase = I2C2_APDMA_BASE + I2C2_APDMA_OFFSET;
    mtk_i2c[I2C3].base = I2C3_BASE;
    mtk_i2c[I2C3].pdmabase = I2C3_APDMA_BASE;
    mtk_i2c[I2C4].base = I2C4_BASE + I2C4_BASE_OFFSET;
    mtk_i2c[I2C4].pdmabase = I2C4_APDMA_BASE + I2C4_APDMA_OFFSET;
    mtk_i2c[I2C5].base = I2C5_BASE;
    mtk_i2c[I2C5].pdmabase = I2C5_APDMA_BASE;
    mtk_i2c[I2C6].base = I2C6_BASE;
    mtk_i2c[I2C6].pdmabase = I2C6_APDMA_BASE;
    mtk_i2c[I2C7].base = I2C7_BASE + I2C7_BASE_OFFSET;
    mtk_i2c[I2C7].pdmabase = I2C7_APDMA_BASE + I2C7_APDMA_OFFSET;
    mtk_i2c[I2C8].base = I2C8_BASE + I2C8_BASE_OFFSET;
    mtk_i2c[I2C8].pdmabase = I2C8_APDMA_BASE + I2C8_APDMA_OFFSET;
    mtk_i2c[I2C9].base = I2C9_BASE + I2C9_BASE_OFFSET;
    mtk_i2c[I2C9].pdmabase = I2C9_APDMA_BASE + I2C9_APDMA_OFFSET;
}
