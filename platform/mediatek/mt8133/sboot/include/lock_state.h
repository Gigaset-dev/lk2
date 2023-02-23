/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
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
