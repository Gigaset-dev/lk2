/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once

#include <platform/addressmap.h>

#define AEE_MRDUMP_DDR_RSV_READY 0x9502

/* Platform defined */
#define MRDUMP_CB_SIZE    0x2000
#define MRDUMP_CB_OFFSET  0x0001E000
#define MRDUMP_CB_ADDR    (SRAM_BASE_PHY + MRDUMP_CB_OFFSET)
#define MRDUMP_DEFAULT_NONE
