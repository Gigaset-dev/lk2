/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once

#define DRAMC_MAX_RK      2
#define DRAMC_FREQ_CNT    7
#define DRAMC_MR_CNT      4

#define DRAMC_BUF_LENGTH 0x800

struct mr_info_t {
    unsigned short mr_index;
    unsigned short mr_value;
};
