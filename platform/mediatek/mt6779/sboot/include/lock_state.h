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

#include <stdint.h>

/* ========================================================= */
/* exported to APP layer, which serves as abstraction layer. */
/* should be moved to product line common folder later       */
/* ========================================================= */
#define STATUS_GET_LOCK_STATE_OK                0x0
#define STATUS_GET_LOCK_STATE_FAIL_GENERAL      0x1
#define STATUS_GET_LOCK_STATE_FAIL_INVALID_BUF  0x2

#define STATUS_SET_LOCK_STATE_OK                  0x0
#define STATUS_SET_LOCK_STATE_FAIL_GENERAL        0x1
#define STATUS_SET_LOCK_STATE_FAIL_INVALID_STATE  0x2

#define STATUS_GET_WARRANTY_OK                0x0
#define STATUS_GET_WARRANTY_GENERAL           0x1
#define STATUS_GET_WARRANTY_FAIL_INVALID_BUF  0x2

#define DEVICE_STATE_UNLOCKED 0x0
#define DEVICE_STATE_LOCKED   0x1

int get_lock_state(uint32_t *lock_state);
int set_lock_state(uint32_t lock_state);
int get_warranty(uint32_t *warranty);

const char *get_lock_state_str(void);
const char *get_warranty_str(void);
