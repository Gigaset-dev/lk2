/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <debug.h>
#include <trace.h>

#define LOCAL_TRACE 0

/* DEBUG MARCO */
#define SPMITAG                 "[SPMI] "
#define SPMI_ERR(fmt, arg...)   dprintf(CRITICAL, SPMITAG fmt, ##arg)
#define SPMI_ERRL(fmt, arg...)  dprintf(CRITICAL, fmt, ##arg)
#define SPMI_INFO(fmt, arg...)  LTRACEF_LEVEL(ALWAYS, SPMITAG fmt, ##arg)
