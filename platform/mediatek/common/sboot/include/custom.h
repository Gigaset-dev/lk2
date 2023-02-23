/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <verified_boot.h>

/* Replace with custom parameter struct
 * definition here
 */
struct mtk_param_t {
    char *sz;
    uint32_t size;
};

struct res_state_t {
    uint32_t err;
    uint32_t cust_state;
    uint32_t seccfg_state;
};

void set_param(void *cust_param);
void *get_param(void);
int custom_get_lock_state(uint32_t *lock_state);
int custom_set_lock_state(uint32_t lock_state);
int custom_error_process(struct res_state_t *res);
