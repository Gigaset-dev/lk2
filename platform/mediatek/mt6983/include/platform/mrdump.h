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
    {4, 0},
    {5, 1},
    {6, 2},
    {7, 3},
    {8, 4},
    {9, 5},
    {10, 6},
    {11, 7},
    {25, 8},
    {26, 9},
    {27, 10},
    {28, 11},
    {29, 12},
};

