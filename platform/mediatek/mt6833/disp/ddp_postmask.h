/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include "ddp_data_type.h"
#include "ddp_hal.h"


/* start postmask module */
int POSTMASKStart(enum DISP_MODULE_ENUM module, void *handle);

/* stop postmask module */
int POSTMASKStop(enum DISP_MODULE_ENUM module, void *handle);

/* reset postmask module */
int POSTMASKReset(enum DISP_MODULE_ENUM module, void *handle);

/* bypass postmask module */
int POSTMASKBypass(enum DISP_MODULE_ENUM module, int bypass);

int postmask_dump_analysis(enum DISP_MODULE_ENUM module);
int postmask_dump_reg(enum DISP_MODULE_ENUM module);
