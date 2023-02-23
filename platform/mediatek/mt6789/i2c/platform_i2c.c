/*
 * Copyright (c) 2022 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <mt_i2c.h>
#include <platform/addressmap.h>
#include <platform_i2c.h>

enum gpio_list {
    SCL0_GPIO = 130,
    SDA0_GPIO = 131,
    SCL1_GPIO = 132,
    SDA1_GPIO = 133,
    SCL2_GPIO = 134,
    SDA2_GPIO = 135,
    SCL3_GPIO = 136,
    SDA3_GPIO = 137,
    SCL4_GPIO = 138,
    SDA4_GPIO = 139,
    SCL5_GPIO = 140,
    SDA5_GPIO = 141,
    SCL6_GPIO = 142,
    SDA6_GPIO = 143,
    SCL7_GPIO = 144,
    SDA7_GPIO = 145,
    SCL8_GPIO = 146,
    SDA8_GPIO = 147,
    SCL9_GPIO = 148,
    SDA9_GPIO = 149,
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
    //No need to initialize the settings separately
    return;
}

void i2c_config_init(void)
{
    mtk_i2c[I2C0].base = I2C0_BASE;
    mtk_i2c[I2C0].pdmabase = I2C0_APDMA_BASE;
    mtk_i2c[I2C1].base = I2C1_BASE;
    mtk_i2c[I2C1].pdmabase = I2C1_APDMA_BASE;
    mtk_i2c[I2C2].base = I2C2_BASE + I2C2_BASE_OFFSET;
    mtk_i2c[I2C2].pdmabase = I2C2_APDMA_BASE;
    mtk_i2c[I2C3].base = I2C3_BASE;
    mtk_i2c[I2C3].pdmabase = I2C3_APDMA_BASE;
    mtk_i2c[I2C4].base = I2C4_BASE + I2C4_BASE_OFFSET;
    mtk_i2c[I2C4].pdmabase = I2C4_APDMA_BASE;
    mtk_i2c[I2C5].base = I2C5_BASE;
    mtk_i2c[I2C5].pdmabase = I2C5_APDMA_BASE;
    mtk_i2c[I2C6].base = I2C6_BASE;
    mtk_i2c[I2C6].pdmabase = I2C6_APDMA_BASE;
    mtk_i2c[I2C7].base = I2C7_BASE + I2C7_BASE_OFFSET;
    mtk_i2c[I2C7].pdmabase = I2C7_APDMA_BASE;
    mtk_i2c[I2C8].base = I2C8_BASE + I2C8_BASE_OFFSET;
    mtk_i2c[I2C8].pdmabase = I2C8_APDMA_BASE;
    mtk_i2c[I2C9].base = I2C9_BASE + I2C9_BASE_OFFSET;
    mtk_i2c[I2C9].pdmabase = I2C9_APDMA_BASE;
}
