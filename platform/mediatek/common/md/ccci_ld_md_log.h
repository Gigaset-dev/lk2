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

/* Error trace log */
int ccci_lk_trace(const char *fmt, ...);
int ccci_lk_add_trace_to_kernel(void);

int ccci_lk_trace_init(void);
void ccci_lk_trace_de_init(void);

/* Log macro part */
#define ALWAYS_LOG(fmt, args...) LTRACEF_LEVEL(ALWAYS, "[CCCI]"fmt, ##args)
#define INFO_LOG(fmt, args...) LTRACEF_LEVEL(INFO, "[CCCI]"fmt, "[CCCI]"##args)
#define CCCI_TRACE_LOG(fmt, args...) ccci_lk_trace("[CCCI][%s:%d] "fmt, __func__, __LINE__, ##args)
