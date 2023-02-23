/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <stdint.h>

/* ================================ */
/* platform specific implementation */
/* ================================ */
/* LKS means "Lock State" */
enum LKS {
    LKS_DEFAULT = 0x01,
    LKS_MP_DEFAULT,
    LKS_UNLOCK,
    LKS_LOCK
};

/* LKCS means "Lock Critical State" */
enum LKCS {
    LKCS_UNLOCK = 0x01,
    LKCS_LOCK
};

/* SBOOT_STATE */
enum SBOOT_STATE {
    SBOOT_DIS = 0,
    SBOOT_EN = 1
};

/* ========================================================= */
/* exported to APP layer, which serves as abstraction layer. */
/* should be moved to product line common folder later       */
/* ========================================================= */
#define STATUS_GET_LOCK_STATE_OK                  0x0
#define STATUS_GET_LOCK_STATE_FAIL_GENERAL        0x1
#define STATUS_GET_LOCK_STATE_FAIL_INVALID_BUF    0x2

#define STATUS_SET_LOCK_STATE_OK                  0x0
#define STATUS_SET_LOCK_STATE_FAIL_GENERAL        0x1
#define STATUS_SET_LOCK_STATE_FAIL_INVALID_STATE  0x2

#define STATUS_GET_WARRANTY_OK                    0x0
#define STATUS_GET_WARRANTY_GENERAL               0x1
#define STATUS_GET_WARRANTY_FAIL_INVALID_BUF      0x2

#define DEVICE_STATE_UNLOCKED                     0x0
#define DEVICE_STATE_LOCKED                       0x1

const char *get_warranty_str(void);
const char *get_lock_state_str(void);
int get_warranty(uint32_t *warranty);
int sec_cfg_get_lock_state(uint32_t *lock_state);
int sec_cfg_set_lock_state(uint32_t lock_state);

