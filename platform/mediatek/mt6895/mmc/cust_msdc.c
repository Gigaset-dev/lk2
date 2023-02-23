/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <cust_msdc.h>
#include <msdc.h>
#include "msdc_io.h"

struct msdc_cust msdc_cap[MSDC_MAX_NUM] = {
    /* msdc_cap[0] is for compatibility only */
    {
        0,                     /* host clock source */
        0,                     /* command latch edge */
        0,                     /* data latch edge */
        0,                     /* clock pad driving */
        0,                     /* command pad driving */
        0,                     /* data pad driving */
        0,                     /* rst pad driving */
        0,                     /* ds pad driving */
        0,                     /* data pins */
        0,                     /* data address offset */
        0
    },
#ifndef MSDC1_EMMC
    {
        MSDC30_CLKSRC_DEFAULT, /* host clock source */
        MSDC_SMPL_RISING,      /* command latch edge */
        MSDC_SMPL_RISING,      /* data latch edge */
        MSDC_DRVN_GEAR3,       /* clock pad driving */
        MSDC_DRVN_GEAR3,       /* command pad driving */
        MSDC_DRVN_GEAR3,       /* data pad driving */
        MSDC_DRVN_DONT_CARE,   /* rst pad driving */
        MSDC_DRVN_DONT_CARE,   /* ds pad driving */
        4,                     /* data pins */
        0,                     /* data address offset */
        MSDC_HIGHSPEED | MSDC_UHS1 | MSDC_DDR
    },
#else
    {
        MSDC50_CLKSRC_DEFAULT, /* host clock source */
        MSDC_SMPL_RISING,      /* command latch edge */
        MSDC_SMPL_RISING,      /* data latch edge */
        MSDC_DRVN_GEAR4,       /* clock pad driving */
        MSDC_DRVN_GEAR4,       /* command pad driving */
        MSDC_DRVN_GEAR4,       /* data pad driving */
        MSDC_DRVN_GEAR4,       /* rst pad driving */
        MSDC_DRVN_GEAR4,       /* ds pad driving */
        4,                     /* data pins */
        0,                     /* data address offset */
        MSDC_HIGHSPEED
    },
#endif
};

struct msdc_cust get_msdc_capability(unsigned int id)
{
    return msdc_cap[id];
}

