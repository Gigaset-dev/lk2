/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once

#include <platform/addressmap.h>

#define INIT_HRE_BK_CONFIG        1
#define INIT_HRE_RS_CONFIG        2
#define SAVE_HRE_BK_CONFIG        3
#define SAVE_HRE_RS_CONFIG        4

#define MMINFRA_HRE_ADDR_BASE (IO_BASE + 0x0E900000)
#define MMINFRA_HRE_ADDR_MIN  0x0

void mminfra_hre_bkrs(unsigned int select);
