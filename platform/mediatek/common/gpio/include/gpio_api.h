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
#pragma once

#include <platform/reg.h>
#include <platform/gpio.h>

/*----------------------------------------------------------------------------*/
//  Error Code No.
#define RSUCCESS        0
#define ERACCESS        1
#define ERINVAL         2
#define ERWRAPPER       3
/*----------------------------------------------------------------------------*/

/******************************************************************************
 * Enumeration for GPIO pin
 ******************************************************************************/
/* GPIO MODE CONTROL VALUE*/
enum GPIO_MODE {
    GPIO_MODE_UNSUPPORTED = -1,
    GPIO_MODE_GPIO  = 0,
    GPIO_MODE_00    = 0,
    GPIO_MODE_01    = 1,
    GPIO_MODE_02    = 2,
    GPIO_MODE_03    = 3,
    GPIO_MODE_04    = 4,
    GPIO_MODE_05    = 5,
    GPIO_MODE_06    = 6,
    GPIO_MODE_07    = 7,

    GPIO_MODE_MAX,
    GPIO_MODE_DEFAULT = GPIO_MODE_01,
    GPIO_MODE_INIT_NO_COVER = 0xFF,
};
/*----------------------------------------------------------------------------*/
/* GPIO DIRECTION */
enum GPIO_DIR {
    GPIO_DIR_UNSUPPORTED = -1,
    GPIO_DIR_IN     = 0,
    GPIO_DIR_OUT    = 1,

    GPIO_DIR_MAX,
    GPIO_DIR_DEFAULT = GPIO_DIR_IN,
};
/*----------------------------------------------------------------------------*/
/* GPIO PULL ENABLE*/
enum GPIO_PULL_EN {
    GPIO_PULL_EN_UNSUPPORTED = -1,
    GPIO_PULL_DISABLE   = 0,
    GPIO_PULL_ENABLE    = 1,
    GPIO_PULL_ENABLE_R0 = 2,
    GPIO_PULL_ENABLE_R1 = 3,
    GPIO_PULL_ENABLE_R0R1 = 4,

    GPIO_PULL_EN_MAX,
    GPIO_PULL_EN_DEFAULT = GPIO_PULL_ENABLE,
};
/*----------------------------------------------------------------------------*/
/* GPIO SMT*/
enum GPIO_SMT {
    GPIO_SMT_UNSUPPORTED = -1,
    GPIO_SMT_DISABLE = 0,
    GPIO_SMT_ENABLE  = 1,

    GPIO_SMT_MAX,
    GPIO_SMT_DEFAULT = GPIO_SMT_ENABLE,
};
/*----------------------------------------------------------------------------*/
/* GPIO IES*/
enum GPIO_IES {
    GPIO_IES_UNSUPPORTED = -1,
    GPIO_IES_DISABLE = 0,
    GPIO_IES_ENABLE  = 1,

    GPIO_IES_MAX,
    GPIO_IES_DEFAULT = GPIO_IES_ENABLE,
};
/*----------------------------------------------------------------------------*/
/* GPIO DRIVING*/
enum GPIO_DRV {
    GPIO_DRV_UNSUPPORTED = -1,
    GPIO_DRV0   = 0,
    GPIO_DRV1   = 1,
    GPIO_DRV2   = 2,
    GPIO_DRV3   = 3,
    GPIO_DRV4   = 4,
    GPIO_DRV5   = 5,
    GPIO_DRV6   = 6,
    GPIO_DRV7   = 7,
};
/*----------------------------------------------------------------------------*/
/* GPIO PULL-UP/PULL-DOWN*/
enum GPIO_PULL {
    GPIO_PULL_UNSUPPORTED = -1,
    GPIO_PULL_DOWN  = 0,
    GPIO_PULL_UP    = 1,
    GPIO_NO_PULL    = 2,

    GPIO_PULL_MAX,
    GPIO_PULL_DEFAULT = GPIO_PULL_DOWN
};
/*----------------------------------------------------------------------------*/
/* GPIO OUTPUT */
enum GPIO_OUT {
    GPIO_OUT_UNSUPPORTED = -1,
    GPIO_OUT_ZERO = 0,
    GPIO_OUT_ONE  = 1,

    GPIO_OUT_MAX,
    GPIO_OUT_DEFAULT = GPIO_OUT_ZERO,
    GPIO_DATA_OUT_DEFAULT = GPIO_OUT_ZERO,
};
/*----------------------------------------------------------------------------*/
/* GPIO INPUT */
enum GPIO_IN {
    GPIO_IN_UNSUPPORTED = -1,
    GPIO_IN_ZERO = 0,
    GPIO_IN_ONE  = 1,

    GPIO_IN_MAX,
};
/*----------------------------------------------------------------------------*/

/******************************************************************************
 * Definition
 ******************************************************************************/
/* GPIO register */
struct VAL_REGS {
    unsigned int val;
    unsigned int set;
    unsigned int rst;
    unsigned int _align1;
};

/* DT related */
struct gpio_pin {
    unsigned char mode;
    unsigned char dir;
    unsigned char pullen;
    unsigned char pull;
    unsigned char dataout;
    unsigned char smt;
};

/******************************************************************************
 * MACRO Definition
 ******************************************************************************/
#define GPIO_WR32(addr, data)           write32(addr, data)
#define GPIO_RD32(addr)                 read32(addr)
#define GPIO_SET_BITS(REG, BIT)         GPIO_WR32(REG + REGSET, BIT)
#define GPIO_CLR_BITS(REG, BIT)         GPIO_WR32(REG + REGCLR, BIT)

/******************************************************************************
 * GPIO Driver interface
 ******************************************************************************/
/*direction*/
s32 mt_set_gpio_dir(enum GPIO_PIN pin, enum GPIO_DIR dir);
s32 mt_get_gpio_dir(enum GPIO_PIN pin);

/*driving*/
s32 mt_set_gpio_drv(enum GPIO_PIN pin, enum GPIO_DRV drv);
s32 mt_get_gpio_drv(enum GPIO_PIN pin);

/*pull enable*/
s32 mt_set_gpio_pull_enable(enum GPIO_PIN pin, enum GPIO_PULL_EN en);
s32 mt_get_gpio_pull_enable(enum GPIO_PIN pin);

/*pull select*/
s32 mt_set_gpio_pull_select(enum GPIO_PIN pin, enum GPIO_PULL sel);
s32 mt_get_gpio_pull_select(enum GPIO_PIN pin);

/*schmitt trigger*/
s32 mt_set_gpio_smt(enum GPIO_PIN pin, enum GPIO_SMT enable);
s32 mt_get_gpio_smt(enum GPIO_PIN pin);

/*IES*/
s32 mt_set_gpio_ies(enum GPIO_PIN pin, enum GPIO_IES enable);
s32 mt_get_gpio_ies(enum GPIO_PIN pin);

/*input/output*/
s32 mt_set_gpio_out(enum GPIO_PIN pin, enum GPIO_OUT output);
s32 mt_get_gpio_out(enum GPIO_PIN pin);
s32 mt_get_gpio_in(enum GPIO_PIN pin);

/*mode control*/
s32 mt_set_gpio_mode(enum GPIO_PIN pin, enum GPIO_MODE mode);
s32 mt_get_gpio_mode(enum GPIO_PIN pin);

