/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <stdint.h>

#define MAX_VALUE_SZ    (128)

uint32_t avb_extract_from_cmdline(char *in, uint32_t in_sz, const char *key);
uint32_t avb_get_cmdline(const char *key, char *output, uint32_t output_sz);
