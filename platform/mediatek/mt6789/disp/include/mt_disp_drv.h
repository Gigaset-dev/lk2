/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <assert.h>
#include <lcm_drv.h>
#include <stdbool.h>
#include <stdlib.h>

__BEGIN_CDECLS

// ---------------------------------------------------------------------------
//  UBoot Display Utility Macros
// ---------------------------------------------------------------------------

#define NOT_REFERENCED(x) {(x) = (x); }
#define msleep(x)    spin(x * 1000)
#define printk  printf

//Project specific header file
#define CFG_DISPLAY_WIDTH        (mt_disp_get_width())
#define CFG_DISPLAY_ALIGN_WIDTH  (mt_disp_get_align_width())
#define CFG_DISPLAY_HEIGHT       (mt_disp_get_height())
#define CFG_DISPLAY_ALIGN_HEIGHT (mt_disp_get_align_height())
#define CFG_DISPLAY_BPP          (mt_disp_get_bpp())

// ---------------------------------------------------------------------------
enum DISP_PROG_BAR_DIRECT {
    DISP_VERTICAL_PROG_BAR = 0,
    DISP_HORIZONTAL_PROG_BAR,
};





// ---------------------------------------------------------------------------
//  UBoot Display Export Functions
// ---------------------------------------------------------------------------
void mtkfb_draw_block(unsigned int x, unsigned int y, unsigned int w,
                                unsigned int h, unsigned int color);
u32 mt_disp_get_vram_size(void);
void disp_init(void *fdt);
void disp_sw_init(void *fdt);
void disp_el3_setup(void);
void mt_disp_power(bool on);
void mt_disp_update(u32 x, u32 y, u32 width, u32 height);
u32 mt_disp_get_lcd_time(void);
const char *mt_disp_get_lcm_id(void);
u32 mt_disp_get_width(void);
u32 mt_disp_get_align_width(void);
u32 mt_disp_get_height(void);
u32 mt_disp_get_align_height(void);
int mt_disp_get_bpp(void);
u32 mt_disp_get_redoffset_32bit(void);
bool mt_disp_get_fb_init_status(void);

bool DISP_DetectDevice(void);

// -- Utility Functions for Customization --

void *mt_get_fb_addr(void);
u32 mt_get_fb_size(void);
void *mt_get_tempfb_addr(void);


void mt_disp_draw_prog_bar(enum DISP_PROG_BAR_DIRECT direct,
                           u32 left, u32 top,
                           u32 right, u32 bottom,
                           u32 fgColor, u32 bgColor,
                           u32 start_div, u32 total_div,
                           u32 occupied_div);
extern u32 DISP_GetScreenWidth(void);
extern u32 DISP_GetScreenHeight(void);
extern void arch_clean_cache_range(addr_t start, size_t len);

__END_CDECLS

