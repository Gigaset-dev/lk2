/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#define RETURN_STACK_BUF_LENGTH             0x2000
#define NUM_RETRUN_STACK_ENTRY_LITTLE_CORE  8
#define NUM_RETRUN_STACK_ENTRY_BIG_CORE     16

int return_stack_get(void **data, int *len);
void return_stack_put(void **data);
