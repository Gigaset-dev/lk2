/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once
#include <platform/addressmap.h>

#define SRCLKEN_RC_REG(ofs)         (SRCLKEN_RC_BASE + (ofs))
#define RC_WDT_LATCH_0              SRCLKEN_RC_REG(0x1D8)
#define RC_WDT_LATCH_1              SRCLKEN_RC_REG(0x1DC)
#define RC_WDT_LATCH_2              SRCLKEN_RC_REG(0x1E0)
#define RC_WDT_LATCH_3              SRCLKEN_RC_REG(0x1E4)
#define RC_WDT_LATCH_4              SRCLKEN_RC_REG(0x1E8)
#define RC_WDT_LATCH_5              SRCLKEN_RC_REG(0x1EC)
#define RC_WDT_LATCH_6              SRCLKEN_RC_REG(0x0C0)
#define RC_WDT_LATCH_7              SRCLKEN_RC_REG(0x0C4)
#define RC_WDT_LATCH_8              SRCLKEN_RC_REG(0x0C8)
#define RC_WDT_LATCH_9              SRCLKEN_RC_REG(0x0CC)
#define RC_WDT_LATCH_10             SRCLKEN_RC_REG(0x0D0)
#define RC_WDT_LATCH_11             SRCLKEN_RC_REG(0x0D4)
#define RC_WDT_LATCH_12             SRCLKEN_RC_REG(0x0D8)
#define RC_WDT_LATCH_13             SRCLKEN_RC_REG(0x0DC)
#define RC_WDT_LATCH_14             SRCLKEN_RC_REG(0x0E0)
#define RC_WDT_LATCH_15             SRCLKEN_RC_REG(0x0E4)
#define RC_WDT_LATCH_16             SRCLKEN_RC_REG(0x0EC)
#define RC_WDT_LATCH_17             SRCLKEN_RC_REG(0x0F0)
#define RC_WDT_LATCH_18             SRCLKEN_RC_REG(0x0F4)
#define RC_WDT_LATCH_19             SRCLKEN_RC_REG(0x0F8)
#define RC_WDT_LATCH_20             SRCLKEN_RC_REG(0x0FC)

#define SRCLKEN_RC_DUMP_SIZE        (0x1000)
