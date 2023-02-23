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
    struct VAL_REGS dir[5];   /*0x0000 ~ 0x004F:  80 ( 5*16) bytes */
    u8 rsv00[176];            /*0x0050 ~ 0x00FF: 176 (11*16) bytes */
    struct VAL_REGS dout[5];  /*0x0100 ~ 0x014F:  80 ( 5*16) bytes */
    u8 rsv01[176];            /*0x0150 ~ 0x01FF: 176 (11*16) bytes */
    struct VAL_REGS din[5];   /*0x0200 ~ 0x024F:  80 ( 5*16) bytes */
    u8 rsv02[176];            /*0x0250 ~ 0x02FF: 176 (11*16) bytes */
    struct VAL_REGS mode[18]; /*0x0300 ~ 0x041F: 288 (18*16) bytes */
};

#define GPIO_MODE_BITS         4
#define MAX_GPIO_MODE_PER_REG  8
#define MAX_GPIO_REG_BITS      32

#define SUPPORT_MODE_MWR
#define REGSET                 4
#define REGCLR                 8
#define REGMWR                 0xc

/*
 * define HAS_PUPD
 * PU + PD register, PUPD continue
 */
#define HAS_PUPD
