/*
 * Copyright (c) 2021 MediaTek Inc.
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
    struct VAL_REGS dir[16];   /*0x0000 ~ 0x00FF: 256 bytes */
    struct VAL_REGS dout[16];  /*0x0100 ~ 0x01FF: 256 bytes */
    struct VAL_REGS din[16];   /*0x0200 ~ 0x02FF: 256 bytes */
    struct VAL_REGS mode[32];  /*0x0300 ~ 0x04FF: 512 bytes */
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
#define GPIO_DT_NODE_NAME_SOC   "/gpio@10005000"
#define GPIO_DT_NODE_PROP_NAME  "gpio_init_default"
