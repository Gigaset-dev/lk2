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
    REGION(DRAM_HEAP,       DRAM_BASE_PHY + 0x20000000, 0x0c000000),
};

enum {
    REGION(PMM_HEAP,        DRAM_HEAP_BASE_PHY + 0x00000000, 0x0b000000),
    REGION(BOOT_ALLOC,      DRAM_HEAP_BASE_PHY + 0x0b000000, 0x01000000),
};
