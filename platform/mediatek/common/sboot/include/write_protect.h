/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <stdint.h>
#include <sys/types.h>

int write_protect_flow(u8 boot_mode, uint32_t boot_state);
