/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#define TOTAL_LATCH_COUNT       3
#define MAX_RESULT_COUNT_CEN    43
#define MAX_RESULT_COUNT_CHN    9
#define RESULT_OFFSET           0x88C
#define CTRL_OFFSET             0x888
#define LATCH_LASTEMI_LENGTH    4096

struct emi_lastemi_header {
    unsigned int lastemi_magic;
    unsigned int lastemi_version;
    unsigned int total_latch_count;
    unsigned int nr_cen_emi;
    unsigned int max_result_count_cen_emi;
    unsigned int max_result_count_chn_emi;
    unsigned int lastemi_dram_type;
    unsigned int lastemi_magic_l;
};

struct base_addesss {
    unsigned long cen_base;
    unsigned long chn_base[2];
};

struct emi_lastemi_data {
    struct emi_lastemi_header header;
    struct base_addesss *base;
    unsigned int chn_num;
};

int emi_lastemi_init(void *fdt);
int emi_lastemi_get(void **data, int *len);
void emi_lastemi_put(void **data);
