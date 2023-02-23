/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once
#if LK_AS_BL2_EXT
#include <platform/picachu_def.h>
#else
#include "picachu_def.h"
#endif


/*****************************************************************************
 *
 *****************************************************************************/
int check_lut(void);

#ifdef SUPPORT_DVCS_LUT
int check_dvcs_lut(void);
unsigned int *get_dvcs_mincode_tbl(void);
#endif

void lut_update_vf_table(
    const unsigned int cluster_id,
    const unsigned int point_id,
    const unsigned int freq,
    const unsigned int volt);
