/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#define LASTPC_BUF_LENGTH          (2000)

enum {
    LASTPC_V1 = 0,
    LASTPC_V2,
    LASTPC_V3,
};

/* platform-dependent configs for lastpc */
struct plt_cfg_pc_latch {
    unsigned int nr_max_core;
    unsigned int nr_max_big_core;
    unsigned long *core_dbg_sel;
    unsigned long *core_dbg_mon;
    int (*dump)(const struct plt_cfg_pc_latch *self, char *buf, int *wp);
    unsigned int dump_pc_only;
    unsigned int version;
};

int lastpc_get(void **data, int *len);
void lastpc_put(void **data);
