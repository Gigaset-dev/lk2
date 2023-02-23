/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once
#include <platform/addressmap.h>

enum {
    REGION(LK_KERNEL_32,   DRAM_BASE_PHY + 0x00008000, 0x02800000),
    REGION(LK_KERNEL_64,   DRAM_BASE_PHY + 0x00080000, 0x02800000),
    REGION(PL_BOOTTAGS,    DRAM_BASE_PHY + 0x02B00000, 0x00100000),
    REGION(LK_DT,          DRAM_BASE_PHY + 0x02F00000, 0x00100000),
};
