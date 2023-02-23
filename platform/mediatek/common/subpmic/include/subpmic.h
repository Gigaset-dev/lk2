/*
 * Copyright (c) 2019 MediaTek Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include <sys/types.h>

enum subpmic_chan {
    SUBPMIC_CHAN_PMU,
    SUBPMIC_CHAN_PMIC,
    SUBPMIC_CHAN_LDO,
    SUBPMIC_CHAN_TYPEC,
    SUBPMIC_CHAN_MAX,
};

int subpmic_init(void *fdt);
int subpmic_read_interface(enum subpmic_chan chan,
                           u8 addr, u8 *data, u8 mask, u8 shift);
int subpmic_config_interface(enum subpmic_chan chan,
                             u8 addr, u8 data, u8 mask, u8 shift);
int subpmic_block_read(enum subpmic_chan chan, u8 addr, u8 len, u8 *dst);
int subpmic_block_write(enum subpmic_chan chan, u8 addr, u8 len, u8 *src);
