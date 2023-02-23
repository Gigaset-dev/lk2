/*
* Copyright (c) 2019 MediaTek Inc.
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files
* (the "Software"), to deal in the Software without restriction,
* including without limitation the rights to use, copy, modify, merge,
* publish, distribute, sublicense, and/or sell copies of the Software,
* and to permit persons to whom the Software is furnished to do so,
* subject to the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

//#include <linux/dma-mapping.h>
/* #include "mt_typedefs.h" */
#include <stdlib.h>
/* #include "mt_gpio.h" */
/* #include "sync_write.h" */
#include <reg.h>
/* #include <platform/mt_gpt.h> */

//#include "disp_drv_log.h"

#ifndef OUTREGBIT
#define OUTREGBIT(TYPE, REG, bit, value)  \
                    do {    \
                        TYPE r = *((TYPE *)&readl(&REG));    \
                        r.bit = value;    \
                        writel(&REG, AS_UINT32(&r));    \
                    } while (0)
#endif

///LCD HW feature options for MT6575
#define MTK_LCD_HW_SIF_VERSION      2       ///for MT6575, we naming it is V2 because MT6516/73 is V1...
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

//#define LK_BYPASS_SHADOW_REG

