/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <platform/addressmap.h>

#define NON_SECURE_SRAM_ADDR        (SRAM_BASE + 0x0000DC00)

void sram_sec_init(vaddr_t start_addr);
