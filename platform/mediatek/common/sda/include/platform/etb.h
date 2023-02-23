/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#define MAX_NR_ETB 20
#define ETB_USER_TYPE 8

struct cfg_etb {
    unsigned long base;
    unsigned int support_multi_user;
};

struct plt_cfg_etb {
    unsigned int nr_etb;
    unsigned long total_etb_sz; /* in byte */
    unsigned long dbgao_base;
    unsigned long dem_base;
    struct cfg_etb etb[];
};

extern const struct plt_cfg_etb cfg_etb;

#define DEM_ATB_CLK 0x70
/* ETB registers, "CoreSight Components TRM", 9.3 */
#define ETB_DEPTH        0x04
#define ETB_STATUS       0x0c
#define ETB_READMEM      0x10
#define ETB_READADDR     0x14
#define ETB_WRITEADDR    0x18
#define ETB_TRIGGERCOUNT 0x1c
#define ETB_CTRL    0x20
#define ETB_FFSR    0x300
#define ETB_LAR     0xfb0
#define ETB_LSR     0xfb4

#define ETB_UNLOCK_MAGIC    0xc5acce55

#define CIRC_CNT(head, tail, size) (((head) - (tail)) & ((size)-1))

int etb_get(void **data, int *len);
void etb_put(void **data);
