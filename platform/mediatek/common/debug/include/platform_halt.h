/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <platform.h>
#include <sys/types.h>

status_t register_platform_halt_callback(
        const char *cb_name,
        status_t (*cb)(void *data,
                       platform_halt_action suggested_action,
                       platform_halt_reason reason),
        void *data);
