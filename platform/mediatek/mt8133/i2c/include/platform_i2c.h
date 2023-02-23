/*
 * Copyright (C) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

/* I2C base clock (KHz) */
#define I2C_SRC_CLK     (SRC_CLK_FRQ / 1000)
#define I2C_CLK_DIV     (5) /* frequency divisor */
#define I2C_CLK_RATE    (I2C_SRC_CLK / I2C_CLK_DIV)

#define I2C_FIFO_SIZE   (8)

#define I2C_NR      4 /* Number of I2C controllers */
#define DUTY_CYCLE  50

#define I2C_MODE    1

void i2c_config_init(void);
void i2c_gpio_default_init(void);
extern struct mt_i2c mtk_i2c[I2C_NR];
