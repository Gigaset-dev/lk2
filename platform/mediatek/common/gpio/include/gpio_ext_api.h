/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <sys/types.h>

enum {
    MT_GPIO_EXT_MT6373,
    MT_GPIO_EXT_MAX,
};

/*direction*/
s32 mt_gpio_ext_set_dir(int dev_id, u32 pin, u32 dir);
s32 mt_gpio_ext_get_dir(int dev_id, u32 pin);

/*driving*/
s32 mt_gpio_ext_set_drv(int dev_id, u32 pin, u32 drv);
s32 mt_gpio_ext_get_drv(int dev_id, u32 pin);

/*pull enable*/
s32 mt_gpio_ext_set_pull_enable(int dev_id, u32 pin, u32 en);
s32 mt_gpio_ext_get_pull_enable(int dev_id, u32 pin);

/*pull select*/
s32 mt_gpio_ext_set_pull_select(int dev_id, u32 pin, u32 sel);
s32 mt_gpio_ext_get_pull_select(int dev_id, u32 pin);

/*schmitt trigger*/
s32 mt_gpio_ext_set_smt(int dev_id, u32 pin, u32 enable);
s32 mt_gpio_ext_get_smt(int dev_id, u32 pin);

/*input/output*/
s32 mt_gpio_ext_set_out(int dev_id, u32 pin, u32 output);
s32 mt_gpio_ext_get_out(int dev_id, u32 pin);
s32 mt_gpio_ext_get_in(int dev_id, u32 pin);

/*mode control*/
s32 mt_gpio_ext_set_mode(int dev_id, u32 pin, u32 mode);
s32 mt_gpio_ext_get_mode(int dev_id, u32 pin);
