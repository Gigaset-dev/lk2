/*
 * Copyright (c) 2021 MediaTek Inc. All Rights Reserved.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <stdint.h>

#define BOOTCONFIG_LEN             1024

uint32_t bootconfig_finalized(void *fdt, void *ramdisk_end_addr);
