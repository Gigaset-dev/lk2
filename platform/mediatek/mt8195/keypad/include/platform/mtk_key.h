/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <stdbool.h>

enum key_index {
    DOWNLOAD_KEY,
    MAX_KEY_NUM,
};

void keypad_init(void *fdt);
bool detect_key(enum key_index index);
