/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <emi_lastemi_v1.h>

#define EMI_DBG(x...) dprintf(INFO, x)
#define LACTH_LOG_ENABLE 0

static const char * const emicen_compatible[] = {
    "/soc/emicen",
    "/emicen",
    NULL
};

static const char * const emichn_compatible[] = {
    "/soc/emichn",
    "/emichn",
    NULL
};

struct emi_addr2dram_data {
    unsigned long  emi_cona;
    unsigned long  emi_conf;
    unsigned long  emi_conh;
    unsigned long  emi_conh_2nd;
    unsigned long  emi_conk;
    unsigned long  emi_chn_cona;
    unsigned long  emi_chn_conc;
    unsigned long  emi_chn_conc_2nd;
    unsigned long  disph;
    unsigned long  hash;
};


void *emi_get_fdt(void);
int emi_get_addr2dram_version(void);
int emi_get_addr2dram_data_without_dts(struct emi_addr2dram_data *data);
int emi_get_lastemi_data_without_dts(struct emi_lastemi_data *data);
