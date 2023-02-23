/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <smc.h>
#include <sys/types.h>

void __hvc_conduit(uint32_t smc_id, ulong a0, ulong a1,
                ulong a2, ulong a3, ulong a4, ulong a5,
                ulong a6, struct __smccc_res *res);
