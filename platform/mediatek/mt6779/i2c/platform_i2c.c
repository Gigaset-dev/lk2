/*
 * Copyright (c) 2019 MediaTek Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
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

#include <mt_i2c.h>
#include <platform/addressmap.h>
#include <platform_i2c.h>

enum gpio_list {
    SCL0_GPIO = 51,
    SDA0_GPIO = 52,
    SCL1_GPIO = 61,
    SDA1_GPIO = 62,
    SCL2_GPIO = 112,
    SDA2_GPIO = 113,
    SCL3_GPIO = 55,
    SDA3_GPIO = 56,
    SCL4_GPIO = 110,
    SDA4_GPIO = 111,
    SCL5_GPIO = 28,
    SDA5_GPIO = 29,
    SCL6_GPIO = 45,
    SDA6_GPIO = 46,
    SCL7_GPIO = 198,
    SDA7_GPIO = 199,
};

enum {
    I2C0 = 0,
    I2C1 = 1,
    I2C2 = 2,
    I2C3 = 3,
    I2C4 = 4,
    I2C5 = 5,
    I2C6 = 6,
    I2C7 = 7
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

    /********i2c6 gpio init***********/
    set_gpio_i2c_mode(SCL6_GPIO, I2C_MODE, INTERNAL_PULL_UP);
    set_gpio_i2c_mode(SDA6_GPIO, I2C_MODE, INTERNAL_PULL_UP);

    /********i2c7 gpio init***********/
    set_gpio_i2c_mode(SCL7_GPIO, I2C_MODE, INTERNAL_PULL_UP);
    set_gpio_i2c_mode(SDA7_GPIO, I2C_MODE, INTERNAL_PULL_UP);
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
