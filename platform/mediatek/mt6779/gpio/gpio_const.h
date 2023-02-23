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

#include <sys/types.h>

/******************************************************************************
 * Definition
 ******************************************************************************/
struct GPIO_REGS {
    struct VAL_REGS dir[7];   /*0x0000 ~ 0x006F: 112 bytes */
    u8 rsv00[144];            /*0x0070 ~ 0x00FF: 144 bytes */
    struct VAL_REGS dout[7];  /*0x0100 ~ 0x016F: 112 bytes */
    u8 rsv01[144];            /*0x0170 ~ 0x01FF: 144 bytes */
    struct VAL_REGS din[7];   /*0x0200 ~ 0x026F: 112 bytes */
    u8 rsv02[144];            /*0x0270 ~ 0x02FF: 144 bytes */
    struct VAL_REGS mode[26]; /*0x0300 ~ 0x0490: 416 bytes */
};

#define GPIO_MODE_BITS         4
#define MAX_GPIO_MODE_PER_REG  8
#define MAX_GPIO_REG_BITS      32

#define SUPPORT_MODE_MWR
#define REGSET                 4
#define REGCLR                 8
#define REGMWR                 0xC

/*
 * define HAS_PUPD
 * PU + PD register, PUPD continue
 */
#define HAS_PUPD

/*
 * define HAS_PULLSEL_PULLEN
 * PULLSEL + PULLEN, PUPD discrete
 */

#define ELEMENTS_PER_GPIO       7
#define GPIO_DT_NODE_NAME_SOC   "/soc/gpio@10005000"
#define GPIO_DT_NODE_PROP_NAME  "gpio_init_default"
