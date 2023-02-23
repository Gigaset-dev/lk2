/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once

#include <platform/addressmap.h>

#define AEE_MRDUMP_DDR_RSV_READY 0x9502

/* rgu mapping table for mrdump_key */
#define MRDUMP_KEY_RGU_REMAP_ENABLE 1

struct gpio_rgu_remap_table_t {
    unsigned int gpio_num;
    unsigned int rgu_num;
};

static struct gpio_rgu_remap_table_t gpio_rgu_remap_table[13] = {
    {186, 0},
    {187, 1},
    {188, 2},
    {189, 3},
    {190, 4},
    {191, 5},
    {12, 6},
    {13, 7},
    {43, 8},
    {88, 9},
    {89, 10},
    {90, 11},
    {91, 12},
};

