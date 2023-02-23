/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <cust_msdc.h>
#include <err.h>
#include <errno.h>
#include <lib/kcmdline.h>
#include <lk/init.h>
#include <msdc.h>
#include "msdc_io.h"

struct msdc_cust msdc_cap[MSDC_MAX_NUM] = {
    {
        MSDC50_CLKSRC_DEFAULT, /* host clock source          */
        MSDC_SMPL_RISING,   /* command latch edge            */
        MSDC_SMPL_RISING,   /* data latch edge               */
        MSDC_DRVN_GEAR4,    /* clock pad driving             */
        MSDC_DRVN_GEAR4,    /* command pad driving           */
        MSDC_DRVN_GEAR4,    /* data pad driving              */
        MSDC_DRVN_GEAR4,    /* rst pad driving               */
        MSDC_DRVN_GEAR4,    /* ds pad driving                */
        8,                  /* data pins                     */
        0,                  /* data address offset           */
        MSDC_HIGHSPEED //| MSDC_HS200 | MSDC_HS400
    },
#ifndef MSDC1_EMMC
    {
        MSDC30_CLKSRC_DEFAULT, /* host clock source          */
        MSDC_SMPL_RISING,   /* command latch edge            */
        MSDC_SMPL_RISING,   /* data latch edge               */
        MSDC_DRVN_GEAR3,    /* clock pad driving             */
        MSDC_DRVN_GEAR3,    /* command pad driving           */
        MSDC_DRVN_GEAR3,    /* data pad driving              */
        MSDC_DRVN_DONT_CARE,/* rst pad driving               */
        MSDC_DRVN_DONT_CARE,/* ds pad driving                */
        4,                  /* data pins                     */
        0,                  /* data address offset           */
        MSDC_HIGHSPEED | MSDC_UHS1 | MSDC_DDR
    },
#else
    {
        MSDC50_CLKSRC_DEFAULT, /* host clock source          */
        MSDC_SMPL_RISING,   /* command latch edge            */
        MSDC_SMPL_RISING,   /* data latch edge               */
        MSDC_DRVN_GEAR4,    /* clock pad driving             */
        MSDC_DRVN_GEAR4,    /* command pad driving           */
        MSDC_DRVN_GEAR4,    /* data pad driving              */
        MSDC_DRVN_GEAR4,    /* rst pad driving               */
        MSDC_DRVN_GEAR4,    /* ds pad driving                */
        4,                  /* data pins                     */
        0,                  /* data address offset           */
        MSDC_HIGHSPEED
    },
#endif
};

struct msdc_cust get_msdc_capability(unsigned int id)
{
    return msdc_cap[id];
}

