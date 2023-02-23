/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <platform/dramc.h>

#define BOOT_TAG_EMI_INFO    0x88610020

struct emi_info {
    unsigned int dram_type;
    unsigned int ch_num;
    unsigned int rk_num;
    unsigned long long rank_size[DRAMC_MAX_RK];
};

void get_emi_info(struct emi_info *info);

