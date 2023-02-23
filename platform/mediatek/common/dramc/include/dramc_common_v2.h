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

struct isu_info_t {
    unsigned int buf_size;
    unsigned long long buf_addr;
    unsigned long long ver_addr;
    unsigned long long con_addr;
};

struct emi_info {
    u8 dram_type;
    u8 support_ch_cnt;
    u8 ch_cnt;
    u8 rk_cnt;
    u16 mr_cnt;
    u16 freq_cnt;
    u64 rk_size[DRAMC_MAX_RK];
    u32 freq_step[DRAMC_FREQ_CNT];
    struct mr_info_t mr_info[DRAMC_MR_CNT];
    struct isu_info_t isu_info;
    u32 magic_tail;
};

void get_emi_info(struct emi_info *info);

