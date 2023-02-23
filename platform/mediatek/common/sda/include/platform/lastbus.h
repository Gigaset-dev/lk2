/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <platform/plat_debug.h>

#define LASTBUS_BUF_LENGTH        0x4000
#define NR_MAX_LASTBUS_MONITOR    16
#define MAX_MONITOR_NAME_LEN      32
#define NR_MAX_LASTBUS_IDLE_MASK  8

#define TIMEOUT_THRES_SHIFT       16
#define TIMEOUT_TYPE_SHIFT        1

#define LASTBUS_TIMEOUT_CLR       0x0200
#define LASTBUS_DEBUG_CKEN        0x0008
#define LASTBUS_DEBUG_EN          0x0004
#define LASTBUS_TIMEOUT           0x0001

enum LASTBUS_SW_VERSION {
    LASTBUS_SW_V1 = 1,
    LASTBUS_SW_V2 = 2,
};

enum LASTBUS_TIMEOUT_TYPE {
    LASTBUS_TIMEOUT_FIRST = 0,
    LASTBUS_TIMEOUT_LAST = 1
};

struct lastbus_idle_mask {
    unsigned int reg_offset;
    unsigned int reg_value;
};

struct lastbus_monitor {
    char name[MAX_MONITOR_NAME_LEN];
    vaddr_t base;
    unsigned int num_ports;
    int bus_freq_mhz;
    unsigned int idle_mask_en;
    unsigned int num_idle_mask;
    struct lastbus_idle_mask idle_masks[NR_MAX_LASTBUS_IDLE_MASK];
};

struct cfg_lastbus {
    unsigned int sw_version;
    unsigned int enabled;
    unsigned int timeout_ms;
    unsigned int timeout_type;
    unsigned int num_used_monitors;
    struct lastbus_monitor monitors[NR_MAX_LASTBUS_MONITOR];
};

void lastbus_init(void *fdt);
void lastbus_init_aee(void *fdt);
int lastbus_get(void **data, int *len);
void lastbus_put(void **data);
