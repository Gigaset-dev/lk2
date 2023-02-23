/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <sys/types.h>

#include "gpio_ext.h"
#include "mt6373_gpio_dev.h"

#define MT6373_DRV_MASK         0xf
#define MT6373_MODE_MASK        0xf
#define MT6373_MIN_GPIO_PIN     1
#define MT6373_MAX_GPIO_PIN     13

#ifdef USE_MT6373_GPIO
static u16 MODE_addr[] = {
    /* 1 */ 0xc4,
    /* 2 */ 0xc7,
    /* 3 */ 0xc7,
    /* 4 */ 0xca,
    /* 5 */ 0xca,
    /* 6 */ 0xcd,
    /* 7 */ 0xcd,
    /* 8 */ 0xd0,
    /* 9 */ 0xd0,
    /* 10 */ 0xd3,
    /* 11 */ 0xd3,
    /* 12 */ 0xd6,
    /* 13 */ 0xd6,
};

static s8 MODE_offset[] = {
    /* 1 */ 3,
    /* 2 */ 0,
    /* 3 */ 3,
    /* 4 */ 0,
    /* 5 */ 3,
    /* 6 */ 0,
    /* 7 */ 3,
    /* 8 */ 0,
    /* 9 */ 3,
    /* 10 */ 0,
    /* 11 */ 3,
    /* 12 */ 0,
    /* 13 */ 3,
};

static u16 DIR_addr[] = {
    /* 1 */ 0x88,
    /* 2 */ 0x8b,
    /* 3 */ 0x8b,
    /* 4 */ 0x8b,
    /* 5 */ 0x8b,
    /* 6 */ 0x8b,
    /* 7 */ 0x8b,
    /* 8 */ 0x8b,
    /* 9 */ 0x8b,
    /* 10 */ 0x8e,
    /* 11 */ 0x8e,
    /* 12 */ 0x8e,
    /* 13 */ 0x8e,
};

static s8 DIR_offset[] = {
    /* 1 */ 7,
    /* 2 */ 0,
    /* 3 */ 1,
    /* 4 */ 2,
    /* 5 */ 3,
    /* 6 */ 4,
    /* 7 */ 5,
    /* 8 */ 6,
    /* 9 */ 7,
    /* 10 */ 0,
    /* 11 */ 1,
    /* 12 */ 2,
    /* 13 */ 3,
};

static u16 DO_addr[] = {
    /* 1 */ 0xac,
    /* 2 */ 0xaf,
    /* 3 */ 0xaf,
    /* 4 */ 0xaf,
    /* 5 */ 0xaf,
    /* 6 */ 0xaf,
    /* 7 */ 0xaf,
    /* 8 */ 0xaf,
    /* 9 */ 0xaf,
    /* 10 */ 0xb2,
    /* 11 */ 0xb2,
    /* 12 */ 0xb2,
    /* 13 */ 0xb2,
};

static s8 DO_offset[] = {
    /* 1 */ 7,
    /* 2 */ 0,
    /* 3 */ 1,
    /* 4 */ 2,
    /* 5 */ 3,
    /* 6 */ 4,
    /* 7 */ 5,
    /* 8 */ 6,
    /* 9 */ 7,
    /* 10 */ 0,
    /* 11 */ 1,
    /* 12 */ 2,
    /* 13 */ 3,
};

static u16 DI_addr[] = {
    /* 1 */ 0xb5,
    /* 2 */ 0xb6,
    /* 3 */ 0xb6,
    /* 4 */ 0xb6,
    /* 5 */ 0xb6,
    /* 6 */ 0xb6,
    /* 7 */ 0xb6,
    /* 8 */ 0xb6,
    /* 9 */ 0xb6,
    /* 10 */ 0xb7,
    /* 11 */ 0xb7,
    /* 12 */ 0xb7,
    /* 13 */ 0xb7,
};

static s8 DI_offset[] = {
    /* 1 */ 7,
    /* 2 */ 0,
    /* 3 */ 1,
    /* 4 */ 2,
    /* 5 */ 3,
    /* 6 */ 4,
    /* 7 */ 5,
    /* 8 */ 6,
    /* 9 */ 7,
    /* 10 */ 0,
    /* 11 */ 1,
    /* 12 */ 2,
    /* 13 */ 3,
};

static u16 SMT_addr[] = {
    /* 1 */ 0x22,
    /* 2 */ 0x23,
    /* 3 */ 0x23,
    /* 4 */ 0x23,
    /* 5 */ 0x23,
    /* 6 */ 0x23,
    /* 7 */ 0x23,
    /* 8 */ 0x23,
    /* 9 */ 0x23,
    /* 10 */ 0x24,
    /* 11 */ 0x24,
    /* 12 */ 0x24,
    /* 13 */ 0x24,
};

static s8 SMT_offset[] = {
    /* 1 */ 7,
    /* 2 */ 0,
    /* 3 */ 1,
    /* 4 */ 2,
    /* 5 */ 3,
    /* 6 */ 4,
    /* 7 */ 5,
    /* 8 */ 6,
    /* 9 */ 7,
    /* 10 */ 0,
    /* 11 */ 1,
    /* 12 */ 2,
    /* 13 */ 3,
};

static u16 DRV_addr[] = {
    /* 1 */ 0x29,
    /* 2 */ 0x2a,
    /* 3 */ 0x2a,
    /* 4 */ 0x2b,
    /* 5 */ 0x2b,
    /* 6 */ 0x2c,
    /* 7 */ 0x2c,
    /* 8 */ 0x2d,
    /* 9 */ 0x2d,
    /* 10 */ 0x2e,
    /* 11 */ 0x2e,
    /* 12 */ 0x2f,
    /* 13 */ 0x2f,
};

static s8 DRV_offset[] = {
    /* 1 */ 4,
    /* 2 */ 0,
    /* 3 */ 4,
    /* 4 */ 0,
    /* 5 */ 4,
    /* 6 */ 0,
    /* 7 */ 4,
    /* 8 */ 0,
    /* 9 */ 4,
    /* 10 */ 0,
    /* 11 */ 4,
    /* 12 */ 0,
    /* 13 */ 4,
};

static u16 PULLEN_addr[] = {
    /* 1 */ 0x91,
    /* 2 */ 0x94,
    /* 3 */ 0x94,
    /* 4 */ 0x94,
    /* 5 */ 0x94,
    /* 6 */ 0x94,
    /* 7 */ 0x94,
    /* 8 */ 0x94,
    /* 9 */ 0x94,
    /* 10 */ 0x97,
    /* 11 */ 0x97,
    /* 12 */ 0x97,
    /* 13 */ 0x97,
};

static s8 PULLEN_offset[] = {
    /* 1 */ 7,
    /* 2 */ 0,
    /* 3 */ 1,
    /* 4 */ 2,
    /* 5 */ 3,
    /* 6 */ 4,
    /* 7 */ 5,
    /* 8 */ 6,
    /* 9 */ 7,
    /* 10 */ 0,
    /* 11 */ 1,
    /* 12 */ 2,
    /* 13 */ 3,
};

static u16 PULLSEL_addr[] = {
    /* 1 */ 0x9a,
    /* 2 */ 0x9d,
    /* 3 */ 0x9d,
    /* 4 */ 0x9d,
    /* 5 */ 0x9d,
    /* 6 */ 0x9d,
    /* 7 */ 0x9d,
    /* 8 */ 0x9d,
    /* 9 */ 0x9d,
    /* 10 */ 0xa0,
    /* 11 */ 0xa0,
    /* 12 */ 0xa0,
    /* 13 */ 0xa0,
};

static s8 PULLSEL_offset[] = {
    /* 1 */ 7,
    /* 2 */ 0,
    /* 3 */ 1,
    /* 4 */ 2,
    /* 5 */ 3,
    /* 6 */ 4,
    /* 7 */ 5,
    /* 8 */ 6,
    /* 9 */ 7,
    /* 10 */ 0,
    /* 11 */ 1,
    /* 12 */ 2,
    /* 13 */ 3,
};

const struct gpio_attr_addr mt6373_attr_addr[GPIO_ATTR_MAX] = {
    [GPIO_ATTR_MODE] = { .addr = MODE_addr, .offset = MODE_offset, },
    [GPIO_ATTR_DIR] = { .addr = DIR_addr, .offset = DIR_offset, },
    [GPIO_ATTR_DI] = { .addr = DI_addr, .offset = DI_offset, },
    [GPIO_ATTR_DO] = { .addr = DO_addr, .offset = DO_offset, },
    [GPIO_ATTR_SMT] = { .addr = SMT_addr, .offset = SMT_offset, },
    [GPIO_ATTR_DRV] = { .addr = DRV_addr, .offset = DRV_offset, },
    [GPIO_ATTR_PULLSEL] = { .addr = PULLSEL_addr, .offset = PULLSEL_offset, },
    [GPIO_ATTR_PULLEN] = { .addr = PULLEN_addr, .offset = PULLEN_offset, },
};

struct mt_gpio_ext_dev mt6373_dev = {
    .attr_addr = mt6373_attr_addr,
    .dev = NULL,
    .min_pin_no = MT6373_MIN_GPIO_PIN,
    .max_pin_no = MT6373_MAX_GPIO_PIN,
    .drv_mask = MT6373_DRV_MASK,
    .mode_mask = MT6373_MODE_MASK,
};

struct mt_gpio_ext_dev *mt_gpio_get_dev_mt6373(void)
{
    struct spmi_device *dev;

    if (!mt6373_dev.dev) {
        dev = get_spmi_device(MT6373_SPMI_MASTER_ID, MT6373_SPMI_SLAVE_ID);
        if (!dev)
            return NULL;
        mt6373_dev.dev = dev;
    }

    return &mt6373_dev;
}

#else
struct mt_gpio_ext_dev *mt_gpio_get_dev_mt6373(void)
{
    return NULL;
}

#endif
