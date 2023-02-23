/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <spmi_common.h>
#include <sys/types.h>

enum {
    GPIO_ATTR_MODE,
    GPIO_ATTR_DIR,
    GPIO_ATTR_DI,
    GPIO_ATTR_DO,
    GPIO_ATTR_SMT,
    GPIO_ATTR_DRV,
    GPIO_ATTR_PULLEN,
    GPIO_ATTR_PULLSEL,
    GPIO_ATTR_MAX,
};

struct gpio_attr_addr {
    u16 *addr;
    s8 *offset;
};

struct mt_gpio_ext_dev {
    const struct gpio_attr_addr *attr_addr;
    struct spmi_device *dev;
    u32 min_pin_no;
    u32 max_pin_no;
    u32 drv_mask;
    u32 mode_mask;
};
