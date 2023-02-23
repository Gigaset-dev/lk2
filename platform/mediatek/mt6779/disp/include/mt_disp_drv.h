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

/* #include <platform/mt_typedefs.h> */
#include <stdlib.h>
#include <stdbool.h>
#include "lcm_drv.h"
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

// ---------------------------------------------------------------------------
//  UBoot Display Utility Macros
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

#define NOT_REFERENCED(x) { \
    (x) = (x); \
    }
#define msleep(x)    spin(x * 1000)
#define printk  printf

//Project specific header file
#define CFG_DISPLAY_WIDTH       (DISP_GetScreenWidth())
#define CFG_DISPLAY_HEIGHT      (DISP_GetScreenHeight())
#define CFG_DISPLAY_BPP         (DISP_GetScreenBpp())

// ---------------------------------------------------------------------------
typedef enum {
    DISP_VERTICAL_PROG_BAR = 0,
    DISP_HORIZONTAL_PROG_BAR,
} DISP_PROG_BAR_DIRECT;





// ---------------------------------------------------------------------------
//  UBoot Display Export Functions
// ---------------------------------------------------------------------------

u32 mt_disp_get_vram_size(void);
void   disp_init(void *fdt);
void   mt_disp_power(bool on);
void   mt_disp_update(u32 x, u32 y, u32 width, u32 height);
u32 mt_disp_get_lcd_time(void);
const char *mt_disp_get_lcm_id(void);
u32 mt_disp_get_redoffset_32bit(void);

bool DISP_DetectDevice(void);

// -- Utility Functions for Customization --

void  *mt_get_logo_db_addr(void);
void  *mt_get_logo_db_addr_pa(void);
void   mt_free_logo_from_mblock(void)__attribute__((weak));
void  *mt_get_fb_addr(void);
u32 mt_get_fb_size(void);


void mt_disp_draw_prog_bar(DISP_PROG_BAR_DIRECT direct,
                           u32 left, u32 top,
                           u32 right, u32 bottom,
                           u32 fgColor, u32 bgColor,
                           u32 start_div, u32 total_div,
                           u32 occupied_div);

#ifdef __cplusplus
}
#endif

