/*
 * Copyright (c) 2022 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once

#include <stdint.h>
#include "kraken.h"

void load_kraken_image(uint8_t *dest, uint32_t *len,
               uint8_t *img, int img_size);
int check_kraken_image_magic_cookie(uint8_t **dest, uint32_t *len,
                    uint8_t *img, int img_size);
