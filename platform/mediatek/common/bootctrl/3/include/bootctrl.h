/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

/* THE HAL BOOTCTRL HEADER MUST BE IN SYNC WITH THE UBOOT BOOTCTRL HEADER */

#pragma once

#include <stdbool.h>

#define BOOTCTRL_SUFFIX_A       "_a"
#define BOOTCTRL_SUFFIX_B       "_b"
#define BOOTCTRL_NUM_SLOTS      2

enum {
  BOOTCTRL_UNBOOT = 0,
  BOOTCTRL_SUCCESS,
  BOOTCTRL_RETRY
};

/* bootctrl API */
int check_suffix_with_slot(const char *suffix);
int get_bctrl_mdata(unsigned int slot, unsigned int field);
const char *get_suffix(void);
bool is_ab_enable(void);
int set_active_slot(const char *suffix);
