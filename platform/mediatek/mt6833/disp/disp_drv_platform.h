/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once


#include <stdlib.h>
#include <reg.h>


#define MTKFB_NO_M4U
#define MT65XX_NEW_DISP
#define ALIGN_TO(x, n)  \
    (((x) + ((n) - 1)) & ~((n) - 1))

#define MTK_FB_ALIGNMENT 32
#define MTK_BITS_PER_BYTE 8
#define DISP_HW_RC

/* #define LED_SUPPORT */

#define LK_BYPASS_SHADOW_REG

//#define LK_FILL_MIPI_IMPEDANCE

#define DISP_HELPER_STAGE_NORMAL_LK

//#define DISP_PRIM_DUAL_PIPE
