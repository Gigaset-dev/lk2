/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <debug.h>
#include <platform/addressmap.h>
#include <platform/mtk_key.h>
#include <platform/reg.h>
#include <trace.h>

#define LOCAL_TRACE 0

enum {
    KP_MEM1 = KPD_BASE + 0x0004,
};

enum {
    KPD_NUM_KEYS = 72
};

static int key_mapping[MAX_KEY_NUM] = { 9 };

void keypad_init(void *fdt)
{
}

bool detect_key(enum key_index index)
{
    int idx, bit, din, key;

    if (index >= MAX_KEY_NUM) {
        LTRACEF("Invalid key index: %d (>= %d)\n", index, MAX_KEY_NUM);
        return false;
    }

    key = key_mapping[index];

    if (key > KPD_NUM_KEYS)
        return false;

     idx = key / 16;
     bit = key % 16;

     din = readw(KP_MEM1 + (idx << 2)) & (1U << bit);
     if (!din) {
         LTRACEF("key %d is pressed\n", key);
         return true;
     }

     return false;
}
