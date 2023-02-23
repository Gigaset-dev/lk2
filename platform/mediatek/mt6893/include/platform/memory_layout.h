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
    REGION(LK_KERNEL_32, DRAM_BASE_PHY + 0x00008000, 0x07C00000),
    REGION(LK_KERNEL_64, DRAM_BASE_PHY + 0x00080000, 0x07C00000),
    REGION(LK_DT,        DRAM_BASE_PHY + 0x07C80000, 0x00400000),
    REGION(BL31,         DRAM_BASE_PHY + 0x08C00000, 0x00200000),
    REGION(LK_HMAC,      DRAM_BASE_PHY + 0x08E00000, 0x00001000),
    REGION(PL_DRAMBUF,   DRAM_BASE_PHY + 0x08F00000, 0x00100000),
    REGION(PL_BOOTTAGS,  DRAM_BASE_PHY + 0x09000000, 0x00100000),
    REGION(LK_RAMDISK,   DRAM_BASE_PHY + 0x11100000, 0x04000000),
    REGION(BL32,         DRAM_BASE_PHY + 0x30000000, 0x03200000),
    REGION(LK_MEM,       DRAM_BASE_PHY + 0x50000000, 0x12000000),
};

