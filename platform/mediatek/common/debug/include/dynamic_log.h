/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <sys/types.h>

/* get log dynamic switch status */
unsigned int log_dynamic_switch(void);
unsigned int log_enable(void);
unsigned int log_port(void);
unsigned int log_baudrate(void);

