/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once

#include <sys/types.h>

/******************************************************************************
 * Definition
 ******************************************************************************/
struct GPIO_REGS {
    struct VAL_REGS din[5];   /*0x0000 ~ 0x004F: 80 bytes */
    u8 rsv00[80];            /*0x0050 ~ 0x009F: 80 bytes */
    struct VAL_REGS dout[5];  /*0x00A0 ~ 0x00EF: 80 bytes */
    u8 rsv01[80];            /*0x00F0 ~ 0x013F: 80 bytes */
    struct VAL_REGS dir[5];   /*0x0140 ~ 0x018F: 80 bytes */
    u8 rsv02[80];            /*0x0190 ~ 0x01DF: 80 bytes */
    struct VAL_REGS mode[15]; /*0x01E0 ~ 0x02CF: 240 bytes */
};

#define GPIO_MODE_BITS         3
#define MAX_GPIO_MODE_PER_REG  10
#define MAX_GPIO_REG_BITS      32

#define REGSET                 4
#define REGCLR                 8
#define REGMWR                 0xC

/*
 * define HAS_PULLSEL_PULLEN
 * PULLSEL + PULLEN, PUPD discrete
 */
#define HAS_PULLSEL_PULLEN
#define ELEMENTS_PER_GPIO       7
#define GPIO_DT_NODE_NAME_SOC   "/soc/gpio@10005000"
#define GPIO_DT_NODE_PROP_NAME  "gpio_init_default"

