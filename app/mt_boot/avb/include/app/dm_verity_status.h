/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once
#include <stdint.h>

/* dm-verity mode: how dm-verity reacts to error. */
enum verity_mode {
    VERITY_MODE_EIO = 0,
    VERITY_MODE_LOGGING = 1,
    VERITY_MODE_RESTART = 2,
    VERITY_MODE_LAST = VERITY_MODE_RESTART,
    VERITY_MODE_DEFAULT = VERITY_MODE_RESTART
};

/* dm-verity status: is error detected or everything is fine? */
enum verity_state {
    DM_VERITY_STATUS_OK = 0,
    DM_VERITY_GENERAL_ERROR = 1
};

int get_dm_verity_status(uint32_t *status);
int clear_dm_verity_status(void);
void dm_verity_handler(uint32_t hashtree_error_mode);
void get_hash_tree_error_mode(uint32_t *hashtree_error_mode);
