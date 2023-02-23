/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once

#include <platform/addressmap.h>

#define MFGSYS_SYSRAM_HISTORY_SUPPORT      1
#define MFGSYS_HISTORY_OFS                (0xF000)
#define MFGSYS_HISTORY_SYSRAM_BASE        (SRAM_BASE + MFGSYS_HISTORY_OFS)
#define AEE_LKDUMP_MFGSYS_HISTORY_DATA_SZ (0x1000)// 4 KB

#define EXPMOD                            "[MFGSYSEXP]"
