/*
 * Copyright (C) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

/* I2C base clock (KHz) */
#define I2C_SRC_CLK     (SRC_CLK_FRQ / 1000)
#define I2C_CLK_DIV     (5) /* frequency divisor */
#define I2C_CLK_RATE    (I2C_SRC_CLK / I2C_CLK_DIV)

#define I2C_FIFO_SIZE   (16)

#define I2C_NR      10 /* Number of I2C controllers */
#define DUTY_CYCLE  50

#define I2C_MODE    1

#define I2C2_MULTI_CH_EN 0
#define I2C4_MULTI_CH_EN 0
#define I2C7_MULTI_CH_EN 0
#define I2C8_MULTI_CH_EN 0
#define I2C9_MULTI_CH_EN 0

#if I2C2_MULTI_CH_EN
    #define I2C2_BASE_OFFSET  0x100
#else
    #define I2C2_BASE_OFFSET  0x0
#endif
#if I2C4_MULTI_CH_EN
    #define I2C4_BASE_OFFSET  0x100
#else
    #define I2C4_BASE_OFFSET  0x0
#endif
#if I2C7_MULTI_CH_EN
    #define I2C7_BASE_OFFSET  0x100
#else
    #define I2C7_BASE_OFFSET  0x0
#endif
#if I2C8_MULTI_CH_EN
    #define I2C8_BASE_OFFSET  0x100
#else
    #define I2C8_BASE_OFFSET  0x0
#endif
#if I2C9_MULTI_CH_EN
    #define I2C9_BASE_OFFSET  0x100
#else
    #define I2C9_BASE_OFFSET  0x0
#endif

void i2c_config_init(void);
void i2c_gpio_default_init(void);
extern struct mt_i2c mtk_i2c[I2C_NR];
