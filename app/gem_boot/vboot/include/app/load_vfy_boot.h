/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once
#include <stdint.h>

/* list of boot image types */
#define BOOTIMG_TYPE_BOOT        (0)

int load_vfy_boot(uint32_t bootimg_type);
