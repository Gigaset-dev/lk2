/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once

#define SRAM_FLAG_MAGIC       0x5566520
#define PLAT_SRAM_FLAG_KEY    0xDB45

struct PLAT_SRAM_FLAG {
    unsigned int plat_magic;
    unsigned int plat_sram_flag0;
    unsigned int plat_sram_flag1;
    unsigned int plat_sram_flag2;
};

