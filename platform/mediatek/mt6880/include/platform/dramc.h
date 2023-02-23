/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once

#include <sys/types.h>

#define DRAMC_MAX_RK         2
#define DRAMC_FREQ_CNT       7
#define DRAMC_MR_CNT         4

#define DRAMC_BUF_LENGTH     0x800

struct mr_info_t {
    u16 mr_index;
    u16 mr_value;
};
