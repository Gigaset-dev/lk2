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

/* Platform defined */
#define MRDUMP_CB_SIZE    0x2000
#define MRDUMP_CB_OFFSET  0x0001E000
#define MRDUMP_CB_ADDR    (SRAM_BASE_PHY + MRDUMP_CB_OFFSET)

/* rgu mapping table for mrdump_key */
#define MRDUMP_KEY_RGU_REMAP_ENABLE 1

struct gpio_rgu_remap_table_t {
    unsigned int gpio_num;
    unsigned int rgu_num;
};

static struct gpio_rgu_remap_table_t gpio_rgu_remap_table[] = {
    {13, 4},
    {14, 5},
    {15, 6},
    {16, 7},
    {17, 8},
    {18, 9},
    {19, 10},
    {20, 11},
    {21, 12},
    {22, 13},
    {23, 14},
    {24, 15},
};

