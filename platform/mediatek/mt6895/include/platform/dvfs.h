/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once

#include <platform/addressmap.h>

#define CSRAM_OFFSET 0x0001bc00
#define CSRAM_BASE_PHY (SRAM_BASE_PHY + CSRAM_OFFSET) /* base address */
#define CSRAM_BASE (SRAM_BASE + CSRAM_OFFSET) /* base address */
#define CSRAM_SIZE 0x1400 /* total range (5K bytes) */

#ifndef DEFAULT_OPP_INDEX_1
#define DEFAULT_OPP_INDEX_1 0xF
#endif
#ifndef DEFAULT_OPP_INDEX_2
#define DEFAULT_OPP_INDEX_2 0xF
#endif
#ifndef DEFAULT_OPP_INDEX_3
#define DEFAULT_OPP_INDEX_3 0xF
#endif

#define OFFS_ENABLE               (CSRAM_BASE + 0x0094)   /* 37 */
#define OFFS_TARGET_1             (CSRAM_BASE + 0x0098)   /* 38 */
#define OFFS_TARGET_2             (CSRAM_BASE + 0x01B8)   /* 110 */
#define OFFS_TARGET_3             (CSRAM_BASE + 0x02D8)   /* 182 */

