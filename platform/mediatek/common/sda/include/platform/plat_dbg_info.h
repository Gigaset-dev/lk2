/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#define INFO_TYPE_MAX 3
#define BOOT_TAG_PLAT_DBG_INFO 0x88610013

struct dbg_info_in_bootargs {
    unsigned int key;
    unsigned int base;
    unsigned int size;
};

struct boot_tag_plat_dbg_info {
    u32 info_max;
    struct dbg_info_in_bootargs info[INFO_TYPE_MAX];
};

void *get_dbg_info_base(unsigned int key);
unsigned int get_dbg_info_size(unsigned int key);
