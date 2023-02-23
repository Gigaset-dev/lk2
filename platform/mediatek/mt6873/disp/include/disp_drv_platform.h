/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

//#include <linux/dma-mapping.h>
//#include "mt_typedefs.h"
#include <stdlib.h>
//#include "mt_gpio.h"
//#include "sync_write.h"
#include <reg.h>
//#include "disp_drv_log.h"

///LCD HW feature options for MT6575
#define MTK_LCD_HW_SIF_VERSION      2  ///for MT6575, we naming it is V2 because MT6516/73 is V1...
#define MTKFB_NO_M4U
#define MT65XX_NEW_DISP
//#define MTK_LCD_HW_3D_SUPPORT
#define ALIGN_TO(x, n)  \
    (((x) + ((n) - 1)) & ~((n) - 1))

#define MTK_FB_ALIGNMENT 32  //Hardware 3D
//#define MTK_FB_ALIGNMENT 1 //SW 3D
#define MTK_FB_START_DSI_ISR
#define DISP_HW_RC

/* #define MBLOCK_SUPPORT */
#define GPIO_SUPPORT
/* #define LED_SUPPORT */

#define DFO_USE_NEW_API
#define MTKFB_FPGA_ONLY

#define LK_BYPASS_SHADOW_REG

//#define LK_FILL_MIPI_IMPEDANCE

#define DISP_HELPER_STAGE_NORMAL_LK
