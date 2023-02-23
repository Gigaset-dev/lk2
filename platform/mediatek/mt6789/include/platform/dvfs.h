/*
 * Copyright (c) 2022 MediaTek Inc.
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
