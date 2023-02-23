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
#include <debug.h>
#include <endian.h>
#include <mt_disp_drv.h>
#include <platform/video.h>
#include <stdio.h>
#include <string.h>
#include <trace.h>
#include "video_font.h"

#define LOCAL_TRACE 0

#ifndef MTK_LCM_PHYSICAL_ROTATION
#define MTK_LCM_PHYSICAL_ROTATION "0"
#endif

#define CS_BG_COL           0x00
#define CS_FG_COL           0xa0

/* color format */
#define FC_5B (CS_FG_COL >> 3)
#define FC_6B (CS_FG_COL >> 2)
#define BC_5B (CS_BG_COL >> 3)
#define BC_6B (CS_BG_COL >> 2)

#define DR_TB_0 0x00000000
#define DR_TB_1 0x000000ff
#define DR_TB_3 0x0000ffff
#define DR_TB_7 0x00ffffff
#define DR_TB_8 0xff000000
#define DR_TB_C 0xffff0000
#define DR_TB_E 0xffffff00
#define DR_TB_F 0xffffffff

#define CFB_WIDTH       (pGD ? pGD->width : 0)
#define CFB_HEIGHT      (pGD ? pGD->height : 0)
#define DATA_FMT        (pGD ? pGD->format : 0)
#define PIXEL_SIZE      (pGD ? pGD->bpp : 0)
#define VDO_CFB_SIZE    (pGD ? pGD->memSize : 0)
#define VDO_CFB_ADDR    (pGD ? pGD->frameAddr : 0)

#define LINE_SIZE       (PIXEL_SIZE * CFB_WIDTH)

/* unit: pixel */
#define FONT_WIDTH      (MTK_VFW)
#define FONT_HEIGHT     (MTK_VFH)
/* unit: font */
#define CFB_ROW_LEN     g_cfb[0]
#define CFB_COL_LEN     g_cfb[1]
/* unit: byte */
#define CFB_ROW_SIZE    (FONT_HEIGHT * LINE_SIZE)
#define CFB_SIZE        (CFB_ROW_LEN * CFB_ROW_SIZE)
#define CFB_SCROLL_SIZE (CFB_SIZE - CFB_ROW_SIZE)

#ifdef CFB_LIT_END
#define SHTSWAP32(x)    ((((x) & 0x000000ff) <<  8) | (((x) & 0x0000ff00) >> 8)|\
                         (((x) & 0x00ff0000) <<  8) | (((x) & 0xff000000) >> 8))
#else
#define SWAP32_16(x)    ((((x) & 0x0000ffff) << 16) | (((x) & 0xffff0000) >> 16))
#define SHTSWAP32(x)    (x)
#endif

struct GraphicDevice {
    vaddr_t frameAddr;
    unsigned int memSize;
    unsigned int format;
    unsigned int bpp;
    unsigned int width;
    unsigned int height;
};

#define CFB_FMT_8BIT        0
#define CFB_555RGB_15BIT    1
#define CFB_565RGB_16BIT    2
#define CFB_X888RGB_32BIT   3
#define CFB_888RGB_24BIT    4
#define CFB_332RGB_8BIT     5

static struct GraphicDevice *pGD;  /* Pointer-to-Graphic-array */
static void *cfb_fb_addr;  /* frame-buffer-address */
static int cfb_rows;
static int cfb_cols;

static unsigned long g_cfb[2];

/* color pats */
static unsigned int cfb_eorx;
static unsigned int cfb_fgx;
static unsigned int cfb_bgx;
static unsigned int cfb_fg_alpha;
static unsigned int cfb_bg_alpha;
static unsigned int cfb_eorx_default;
static unsigned int cfb_fgx_default;
static unsigned int cfb_bgx_default;

/* pixel coordinate */
static unsigned int cfb_pixel_position_x;
static unsigned int cfb_pixel_position_y;

static const int lk_cfb_font_dtb24[16][3] = {
    {DR_TB_0, DR_TB_0, DR_TB_0},
    {DR_TB_0, DR_TB_0, DR_TB_7},
    {DR_TB_0, DR_TB_3, DR_TB_8},
    {DR_TB_0, DR_TB_3, DR_TB_F},
    {DR_TB_1, DR_TB_C, DR_TB_0},
    {DR_TB_1, DR_TB_C, DR_TB_7},
    {DR_TB_1, DR_TB_F, DR_TB_8},
    {DR_TB_1, DR_TB_F, DR_TB_F},
    {DR_TB_E, DR_TB_0, DR_TB_0},
    {DR_TB_E, DR_TB_0, DR_TB_7},
    {DR_TB_E, DR_TB_3, DR_TB_8},
    {DR_TB_E, DR_TB_3, DR_TB_F},
    {DR_TB_F, DR_TB_C, DR_TB_0},
    {DR_TB_F, DR_TB_C, DR_TB_7},
    {DR_TB_F, DR_TB_F, DR_TB_8},
    {DR_TB_F, DR_TB_F, DR_TB_F}
};

static const int lk_cfb_font_dtable32[16][4] = {
    {DR_TB_0, DR_TB_0, DR_TB_0, DR_TB_0},
    {DR_TB_0, DR_TB_0, DR_TB_0, DR_TB_7},
    {DR_TB_0, DR_TB_0, DR_TB_7, DR_TB_0},
    {DR_TB_0, DR_TB_0, DR_TB_7, DR_TB_7},
    {DR_TB_0, DR_TB_7, DR_TB_0, DR_TB_0},
    {DR_TB_0, DR_TB_7, DR_TB_0, DR_TB_7},
    {DR_TB_0, DR_TB_7, DR_TB_7, DR_TB_0},
    {DR_TB_0, DR_TB_7, DR_TB_7, DR_TB_7},
    {DR_TB_7, DR_TB_0, DR_TB_0, DR_TB_0},
    {DR_TB_7, DR_TB_0, DR_TB_0, DR_TB_7},
    {DR_TB_7, DR_TB_0, DR_TB_7, DR_TB_0},
    {DR_TB_7, DR_TB_0, DR_TB_7, DR_TB_7},
    {DR_TB_7, DR_TB_7, DR_TB_0, DR_TB_0},
    {DR_TB_7, DR_TB_7, DR_TB_0, DR_TB_7},
    {DR_TB_7, DR_TB_7, DR_TB_7, DR_TB_0},
    {DR_TB_7, DR_TB_7, DR_TB_7, DR_TB_7}
};

static const int lk_cfb_font_dtable16[] = {
#if 0
    0x00000000, 0x0000ffff, 0xffff0000, 0xffffffff
#else    /* RGB565 with Little Endian Table */
    0x00000000, 0xffff0000, 0x0000ffff, 0xffffffff
#endif
};

static const int cfb_font_dtable15[] = {
    0x00000000, 0x00007fff, 0x7fff0000, 0x7fff7fff
};

static const int cfb_font_dtable8[] = {
    0x00000000, 0x000000ff, 0x0000ff00, 0x0000ffff,
    0x00ff0000, 0x00ff00ff, 0x00ffff00, 0x00ffffff,
    0xff000000, 0xff0000ff, 0xff00ff00, 0xff00ffff,
    0xffff0000, 0xffff00ff, 0xffffff00, 0xffffffff
};

static int get_cols(void)
{
    return cfb_cols;
}

static void set_cols(int value)
{
    cfb_cols = value;
}

static void inc_cols(int value)
{
    cfb_cols += value;
}

static int get_rows(void)
{
    return cfb_rows;
}

static void set_rows(int value)
{
    cfb_rows = value;
}

static void inc_rows(int value)
{
    cfb_rows += value;
}

static void cfb_dchars(int x, int y, unsigned char *s, int count)
{
    unsigned char *pos = NULL;
    unsigned char *tdest = NULL;
    unsigned char *pdest = NULL;
    unsigned char *pfont = NULL;
    unsigned int data_fmt = 0;
    unsigned int row = 0;
    unsigned int offs = 0;
    unsigned char high_bits = 0;
    unsigned char low_bits = 0;

    pdest = cfb_fb_addr + y * LINE_SIZE + x * PIXEL_SIZE;
    data_fmt = DATA_FMT;

    pfont = mtk_vdo_fntdata;
    switch (data_fmt) {
    case CFB_555RGB_15BIT:
        while (count--) {
            offs = (*s++) * MTK_VFH;
            pos = pfont + offs;
            row = MTK_VFH;
            for (tdest = pdest; row--; tdest += LINE_SIZE) {
                unsigned char bits = *pos++;

                ((uint32_t *)tdest)[0] =
                    SHTSWAP32((cfb_font_dtable15[bits >> 6] & cfb_eorx) ^ cfb_bgx);
                ((uint32_t *)tdest)[1] =
                    SHTSWAP32((cfb_font_dtable15[bits >> 4 & 3] & cfb_eorx) ^ cfb_bgx);
                ((uint32_t *)tdest)[2] =
                    SHTSWAP32((cfb_font_dtable15[bits >> 2 & 3] & cfb_eorx) ^ cfb_bgx);
                ((uint32_t *)tdest)[3] =
                    SHTSWAP32((cfb_font_dtable15[bits & 3] & cfb_eorx) ^ cfb_bgx);
            }
            pdest = pdest + MTK_VFW * PIXEL_SIZE;
        }
        break;
    case CFB_565RGB_16BIT:
        while (count--) {
            offs = (*s++) * MTK_VFH;
            pos = pfont + offs;
            row = MTK_VFH;
            if (!strncmp(MTK_LCM_PHYSICAL_ROTATION, "180", 3)) {
                for (tdest = pdest + row * LINE_SIZE; row--; tdest -= LINE_SIZE) {
                    unsigned char bits = *pos++;

                    ((uint32_t *)tdest)[0] =
                        SWAP32_16((lk_cfb_font_dtable16[bits & 3] & cfb_eorx) ^ cfb_bgx);
                    ((uint32_t *)tdest)[1] =
                        SWAP32_16((lk_cfb_font_dtable16[bits >> 2 & 3] & cfb_eorx) ^ cfb_bgx);
                    ((uint32_t *)tdest)[2] =
                        SWAP32_16((lk_cfb_font_dtable16[bits >> 4 & 3] & cfb_eorx) ^ cfb_bgx);
                    ((uint32_t *)tdest)[3] =
                        SWAP32_16((lk_cfb_font_dtable16[bits >> 6] & cfb_eorx) ^ cfb_bgx);
                }
                pdest = pdest + MTK_VFW * PIXEL_SIZE;
            } else {
                for (tdest = pdest; row--; tdest += LINE_SIZE) {
                    unsigned char bits = *pos++;

                    ((uint32_t *)tdest)[0] =
                        SHTSWAP32((lk_cfb_font_dtable16[bits >> 6] & cfb_eorx) ^ cfb_bgx);
                    ((uint32_t *)tdest)[1] =
                        SHTSWAP32((lk_cfb_font_dtable16[bits >> 4 & 3] & cfb_eorx) ^ cfb_bgx);
                    ((uint32_t *)tdest)[2] =
                        SHTSWAP32((lk_cfb_font_dtable16[bits >> 2 & 3] & cfb_eorx) ^ cfb_bgx);
                    ((uint32_t *)tdest)[3] =
                        SHTSWAP32((lk_cfb_font_dtable16[bits & 3] & cfb_eorx) ^ cfb_bgx);
                }
                pdest = pdest + MTK_VFW * PIXEL_SIZE;
            }
        }
        break;
    case CFB_332RGB_8BIT:
    case CFB_FMT_8BIT:
        while (count--) {
            offs = (*s++) * MTK_VFH;
            pos = pfont + offs;
            row = MTK_VFH;
            for (tdest = pdest; row--; tdest += LINE_SIZE) {
                high_bits = *pos >> 4;
                low_bits = *pos & 15;
                ++pos;
                ((vaddr_t *)tdest)[0] =
                    (cfb_font_dtable8[high_bits] & cfb_eorx) ^ cfb_bgx;
                ((vaddr_t *)tdest)[1] =
                    (cfb_font_dtable8[low_bits] & cfb_eorx) ^ cfb_bgx;
            }
            pdest = pdest + MTK_VFW * PIXEL_SIZE;
        }
        break;
    case CFB_888RGB_24BIT:
        while (count--) {
            offs = (*s++) * MTK_VFH;
            pos = pfont + offs;
            row = MTK_VFH;
            for (tdest = pdest; row--; tdest += LINE_SIZE) {
                high_bits = *pos >> 4;
                low_bits = *pos & 15;
                ++pos;
                ((uint32_t *)tdest)[0] =
                    (lk_cfb_font_dtb24[high_bits][0] & cfb_eorx) ^ cfb_bgx;
                ((uint32_t *)tdest)[1] =
                    (lk_cfb_font_dtb24[high_bits][1] & cfb_eorx) ^ cfb_bgx;
                ((uint32_t *)tdest)[2] =
                    (lk_cfb_font_dtb24[high_bits][2] & cfb_eorx) ^ cfb_bgx;
                ((uint32_t *)tdest)[3] =
                    (lk_cfb_font_dtb24[low_bits][0] & cfb_eorx) ^ cfb_bgx;
                ((uint32_t *)tdest)[4] =
                    (lk_cfb_font_dtb24[low_bits][1] & cfb_eorx) ^ cfb_bgx;
                ((uint32_t *)tdest)[5] =
                    (lk_cfb_font_dtb24[low_bits][2] & cfb_eorx) ^ cfb_bgx;
            }
            pdest = pdest + MTK_VFW * PIXEL_SIZE;
        }
        break;
    case CFB_X888RGB_32BIT:
        while (count--) {
            offs = (*s++) * MTK_VFH;
            pos = pfont + offs;
            row = MTK_VFH;
            if (!strncmp(MTK_LCM_PHYSICAL_ROTATION, "90", 2)) {
                for (tdest = pdest + row * PIXEL_SIZE; row--; tdest -= PIXEL_SIZE) {
                    high_bits = *pos >> 4;
                    low_bits = *pos & 15;
                    ++pos;
                    ((uint32_t *)tdest)[0 * CFB_WIDTH + MTK_VFH * CFB_WIDTH] =
                        0xff000000 |
                        ((lk_cfb_font_dtable32[high_bits][0] & cfb_eorx) ^ cfb_bgx);
                    ((uint32_t *)tdest)[1 * CFB_WIDTH + MTK_VFH * CFB_WIDTH] =
                        0xff000000 |
                        ((lk_cfb_font_dtable32[high_bits][1] & cfb_eorx) ^ cfb_bgx);
                    ((uint32_t *)tdest)[2 * CFB_WIDTH + MTK_VFH * CFB_WIDTH] =
                        0xff000000 |
                        ((lk_cfb_font_dtable32[high_bits][2] & cfb_eorx) ^ cfb_bgx);
                    ((uint32_t *)tdest)[3 * CFB_WIDTH + MTK_VFH * CFB_WIDTH] =
                        0xff000000 |
                        ((lk_cfb_font_dtable32[high_bits][3] & cfb_eorx) ^ cfb_bgx);
                    ((uint32_t *)tdest)[4 * CFB_WIDTH + MTK_VFH * CFB_WIDTH] =
                        0xff000000 |
                        ((lk_cfb_font_dtable32[low_bits][0] & cfb_eorx) ^ cfb_bgx);
                    ((uint32_t *)tdest)[5 * CFB_WIDTH + MTK_VFH * CFB_WIDTH] =
                        0xff000000 |
                        ((lk_cfb_font_dtable32[low_bits][1] & cfb_eorx) ^ cfb_bgx);
                    ((uint32_t *)tdest)[6 * CFB_WIDTH + MTK_VFH * CFB_WIDTH] =
                        0xff000000 |
                        ((lk_cfb_font_dtable32[low_bits][2] & cfb_eorx) ^ cfb_bgx);
                    ((uint32_t *)tdest)[7 * CFB_WIDTH + MTK_VFH * CFB_WIDTH] =
                        0xff000000 |
                        ((lk_cfb_font_dtable32[low_bits][3] & cfb_eorx) ^ cfb_bgx);
                }
                pdest = pdest + MTK_VFH * PIXEL_SIZE;
            } else if (!strncmp(MTK_LCM_PHYSICAL_ROTATION, "180", 3)) {
                for (tdest = pdest + row * LINE_SIZE; row--; tdest -= LINE_SIZE) {
                    high_bits = *pos >> 4;
                    low_bits = *pos & 15;
                    ++pos;
                    ((uint32_t *)tdest)[7] =
                        0xff000000 |
                        ((lk_cfb_font_dtable32[high_bits][0] & cfb_eorx) ^ cfb_bgx);
                    ((uint32_t *)tdest)[6] =
                        0xff000000 |
                        ((lk_cfb_font_dtable32[high_bits][1] & cfb_eorx) ^ cfb_bgx);
                    ((uint32_t *)tdest)[5] =
                        0xff000000 |
                        ((lk_cfb_font_dtable32[high_bits][2] & cfb_eorx) ^ cfb_bgx);
                    ((uint32_t *)tdest)[4] =
                        0xff000000 |
                        ((lk_cfb_font_dtable32[high_bits][3] & cfb_eorx) ^ cfb_bgx);
                    ((uint32_t *)tdest)[3] =
                        0xff000000 |
                        ((lk_cfb_font_dtable32[low_bits][0] & cfb_eorx) ^ cfb_bgx);
                    ((uint32_t *)tdest)[2] =
                        0xff000000 |
                        ((lk_cfb_font_dtable32[low_bits][1] & cfb_eorx) ^ cfb_bgx);
                    ((uint32_t *)tdest)[1] =
                        0xff000000 |
                        ((lk_cfb_font_dtable32[low_bits][2] & cfb_eorx) ^ cfb_bgx);
                    ((uint32_t *)tdest)[0] =
                        0xff000000 |
                        ((lk_cfb_font_dtable32[low_bits][3] & cfb_eorx) ^ cfb_bgx);
                }
                pdest = pdest + MTK_VFW * PIXEL_SIZE;
            } else if (!strncmp(MTK_LCM_PHYSICAL_ROTATION, "270", 3)) {
                for (tdest = pdest; row--; tdest += PIXEL_SIZE) {
                    high_bits = *pos >> 4;
                    low_bits = *pos & 15;
                    ++pos;
                    ((uint32_t *)tdest)[7 * CFB_WIDTH + MTK_VFH * CFB_WIDTH] =
                        0xff000000 |
                        ((lk_cfb_font_dtable32[high_bits][0] & cfb_eorx) ^ cfb_bgx);
                    ((uint32_t *)tdest)[6 * CFB_WIDTH + MTK_VFH * CFB_WIDTH] =
                        0xff000000 |
                        ((lk_cfb_font_dtable32[high_bits][1] & cfb_eorx) ^ cfb_bgx);
                    ((uint32_t *)tdest)[5 * CFB_WIDTH + MTK_VFH * CFB_WIDTH] =
                        0xff000000 |
                        ((lk_cfb_font_dtable32[high_bits][2] & cfb_eorx) ^ cfb_bgx);
                    ((uint32_t *)tdest)[4 * CFB_WIDTH + MTK_VFH * CFB_WIDTH] =
                        0xff000000 |
                        ((lk_cfb_font_dtable32[high_bits][3] & cfb_eorx) ^ cfb_bgx);
                    ((uint32_t *)tdest)[3 * CFB_WIDTH + MTK_VFH * CFB_WIDTH] =
                        0xff000000 |
                        ((lk_cfb_font_dtable32[low_bits][0] & cfb_eorx) ^ cfb_bgx);
                    ((uint32_t *)tdest)[2 * CFB_WIDTH + MTK_VFH * CFB_WIDTH] =
                        0xff000000 |
                        ((lk_cfb_font_dtable32[low_bits][1] & cfb_eorx) ^ cfb_bgx);
                    ((uint32_t *)tdest)[1 * CFB_WIDTH + MTK_VFH * CFB_WIDTH] =
                        0xff000000 |
                        ((lk_cfb_font_dtable32[low_bits][2] & cfb_eorx) ^ cfb_bgx);
                    ((uint32_t *)tdest)[0 * CFB_WIDTH + MTK_VFH * CFB_WIDTH] =
                        0xff000000 |
                        ((lk_cfb_font_dtable32[low_bits][3] & cfb_eorx) ^ cfb_bgx);
                }
                pdest = pdest + MTK_VFH * PIXEL_SIZE;
            } else {
                for (tdest = pdest; row--; tdest += LINE_SIZE) {
                    high_bits = *pos >> 4;
                    low_bits = *pos & 15;
                    ++pos;
                    ((uint32_t *)tdest)[0] =
                        0xff000000 |
                        ((lk_cfb_font_dtable32[high_bits][0] & cfb_eorx) ^ cfb_bgx);
                    ((uint32_t *)tdest)[1] =
                        0xff000000 |
                        ((lk_cfb_font_dtable32[high_bits][1] & cfb_eorx) ^ cfb_bgx);
                    ((uint32_t *)tdest)[2] =
                        0xff000000 |
                        ((lk_cfb_font_dtable32[high_bits][2] & cfb_eorx) ^ cfb_bgx);
                    ((uint32_t *)tdest)[3] =
                        0xff000000 |
                        ((lk_cfb_font_dtable32[high_bits][3] & cfb_eorx) ^ cfb_bgx);
                    ((uint32_t *)tdest)[4] =
                        0xff000000 |
                        ((lk_cfb_font_dtable32[low_bits][0] & cfb_eorx) ^ cfb_bgx);
                    ((uint32_t *)tdest)[5] =
                        0xff000000 |
                        ((lk_cfb_font_dtable32[low_bits][1] & cfb_eorx) ^ cfb_bgx);
                    ((uint32_t *)tdest)[6] =
                        0xff000000 |
                        ((lk_cfb_font_dtable32[low_bits][2] & cfb_eorx) ^ cfb_bgx);
                    ((uint32_t *)tdest)[7] =
                        0xff000000 |
                        ((lk_cfb_font_dtable32[low_bits][3] & cfb_eorx) ^ cfb_bgx);
                }
                pdest = pdest + MTK_VFW * PIXEL_SIZE;
            }
        }
        break;
    default:
        break;
    }
}

static inline void cfb_drawstr(int x, int y, unsigned char *s)
{
    cfb_dchars(x, y, s, strlen((char *)s));
}

static void cfb_putchar(int x, int y, unsigned char c)
{
    cfb_dchars(x, y, &c, 1);
}

static void cfb_scrollup(void)
{
    /* copy up rows ignoring the first one */
    memcpy(cfb_fb_addr, (char *)cfb_fb_addr + CFB_ROW_SIZE, CFB_SCROLL_SIZE);
    memset((char *)cfb_fb_addr - CFB_ROW_SIZE + CFB_SIZE, CS_BG_COL, CFB_ROW_SIZE);
}

static void cfb_backsp(void)
{
    inc_cols(-1);

    if (get_cols() < 0) {
        set_cols(CFB_COL_LEN - 1);
        inc_rows(-1);

        if (get_rows() < 0)
            set_rows(0);
    }

    cfb_putchar(get_cols() * MTK_VFW, get_rows() * MTK_VFH, ' ');
}

static void cfb_newline(void)
{
    /*
     * Check if last character in the line was just drawn. If so, cursor was
     * overwritten and need not to be cleared. Cursor clearing without this
     * check causes overwriting the 1st character of the line if line length
     * is >= CFB_COL_LEN
     */
    inc_rows(1);
    set_cols(0);

    /* Check if we need to scroll the terminal */
    if (get_rows() >= (int)(CFB_ROW_LEN)) {
        cfb_scrollup();
        inc_rows(-1);
    }
}

static void cfb_colback(void)
{
    set_cols(0);
}

void video_putc(const char c)
{
    static int next_line = 1;

    /*
     * check newline here in order to
     * scroll the screen immediately for the first time video_printf()
     */
    if (cfb_cols >= (int)(CFB_COL_LEN))
        cfb_newline();

    switch (c) {
    case 13:
        /* back to first column */
        cfb_colback();
        break;
    case '\n':
        /* next line */
        if (cfb_cols || (!cfb_cols && next_line))
            cfb_newline();
        next_line = 1;
        break;
    case 8:
        /* back space */
        cfb_backsp();
        break;
    case 9:
        /* tab-8 */
        cfb_cols |= 0x0008;
        cfb_cols &= ~0x0007;
        if (get_cols() >= (int)(CFB_COL_LEN))
            cfb_newline();
        break;
    default:
        /* draw the char */
        if (!strncmp(MTK_LCM_PHYSICAL_ROTATION, "90", 2))
            cfb_putchar((CFB_ROW_LEN - get_rows()) * MTK_VFH, get_cols() * MTK_VFW, c);
        else if (!strncmp(MTK_LCM_PHYSICAL_ROTATION, "180", 3))
            cfb_putchar((CFB_COL_LEN - get_cols() - 1) * MTK_VFW,
                           (CFB_ROW_LEN - get_rows()) * MTK_VFH, c);
        else if (!strncmp(MTK_LCM_PHYSICAL_ROTATION, "270", 3))
            cfb_putchar((CFB_COL_LEN - CFB_ROW_LEN + get_rows()) * MTK_VFH,
                           (CFB_ROW_LEN - get_cols() - 10) * MTK_VFW, c);
        else
            cfb_putchar(get_cols() * MTK_VFW, get_rows() * MTK_VFH, c);

        inc_cols(1);

        /* check for newline */
        if (get_cols() >= (int)(CFB_COL_LEN)) {
            cfb_newline();
            next_line = 0;
        }
        break;
    }
}

void video_puts(const char *s)
{
    unsigned int cfb_count = strlen(s);

    while (cfb_count--) {
        video_putc(*s);
        s++;
    }

    mt_disp_update(0, 0, CFB_WIDTH, CFB_HEIGHT);
}

void video_printf(const char *fmt, ...)
{
    va_list args;
    char printbuffer[256];

    if (!pGD)
        return;

    va_start(args, fmt);

    /*
     * For this to work, printbuffer must be larger than
     * anything we ever want to print.
     */
    vsprintf(printbuffer, fmt, args);
    va_end(args);

    /* Print the string */
    video_puts(printbuffer);
}

void video_set_cursor(int row, int col)
{
    if (row >= 0 && row < (int)(CFB_ROW_LEN) && col >= 0 && col <= (int)(CFB_COL_LEN)) {
        cfb_rows = row;
        cfb_cols = col;
    }
}

void video_set_fg_color(unsigned char r, unsigned char g, unsigned char b)
{
    unsigned char color8bit = 0;
    unsigned int data_fmt = 0;
    unsigned char r_5B = r >> 3;
    unsigned char g_5B = g >> 3;
    unsigned char g_6B = g >> 2;
    unsigned char b_5B = b >> 3;

    data_fmt = DATA_FMT;
    LTRACEF("r-%u,g-%u,b-%u\n", r, g, b);

    switch (data_fmt) {
    case CFB_555RGB_15BIT:
        cfb_fgx = ((r_5B << 26) | (g_5B << 21) | (b_5B << 16) | /* high 16 0555 */
                   (r_5B << 10) | (r_5B << 5) | b_5B);          /* low 16 0555 */
        break;
    case CFB_565RGB_16BIT:
        cfb_fgx = ((r_5B << 27) | (g_6B << 21) | (b_5B << 16) | /* high 16 565 */
                   (r_5B << 11) | (g_6B << 5) | b_5B);          /* low 16 565 */
        break;
    case CFB_X888RGB_32BIT:
        cfb_fgx = (r << 16) | (g << 8) | b;
        break;
    case CFB_332RGB_8BIT:
        color8bit = ((r & 0xe0) | ((g >> 3) & 0x1c) | b >> 6);
        cfb_fgx = (color8bit << 24) | (color8bit << 16) | (color8bit << 8) | color8bit;
        break;
    default:
        LTRACEF("not supported data_fmt:%u\n", data_fmt);
        break;
    }
    cfb_eorx = cfb_fgx ^ cfb_bgx;
}

void video_set_bg_color(unsigned char r, unsigned char g, unsigned char b)
{
    unsigned char color8bit = 0;
    unsigned int data_fmt = 0;
    unsigned char r_5B = r >> 3;
    unsigned char g_5B = g >> 3;
    unsigned char g_6B = g >> 2;
    unsigned char b_5B = b >> 3;

    data_fmt = DATA_FMT;
    LTRACEF("r-%u,g-%u,b-%u\n", r, g, b);

    switch (data_fmt) {
    case CFB_555RGB_15BIT:
        cfb_bgx = ((r_5B << 26) | (g_5B << 21) | (b_5B << 16) | /* high 16 0555 */
                   (r_5B << 10) | (r_5B << 5) | b_5B);          /* low 16 0555 */
        break;
    case CFB_565RGB_16BIT:
        cfb_bgx = ((r_5B << 27) | (g_6B << 21) | (b_5B << 16) | /* high 16 565 */
                   (r_5B << 11) | (g_6B << 5) | b_5B);          /* low 16 565 */
        break;
    case CFB_X888RGB_32BIT:
        cfb_bgx = (r << 16) | (g << 8) | b;
        break;
    case CFB_332RGB_8BIT:
        color8bit = ((r & 0xe0) | ((g >> 3) & 0x1c) | b >> 6);
        cfb_bgx = (color8bit << 24) | (color8bit << 16) | (color8bit << 8) | color8bit;
        break;
    default:
        LTRACEF("not supported data_fmt:%u\n", data_fmt);
        break;
    }
    cfb_eorx = cfb_fgx ^ cfb_bgx;
}

void video_set_default_color(void)
{
    LTRACE_ENTRY;

    cfb_eorx = cfb_eorx_default;
    cfb_fgx = cfb_fgx_default;
    cfb_bgx = cfb_bgx_default;

    LTRACE_EXIT;
}

int video_get_rows(void)
{
    return CFB_ROW_LEN;
}

int video_get_cols(void)
{
    return CFB_COL_LEN;
}

void video_clear_screen(void)
{
    if (pGD) {
        memset((void *)pGD->frameAddr, 0, pGD->memSize);
        mt_disp_update(0, 0, CFB_WIDTH, CFB_HEIGHT);
    }
}

#define MAKE_TWO_RGB565_COLOR(high, low)  (((low) << 16) | (high))

static void cfb_dchar_scale(unsigned int x, unsigned int y, int font_scale, char c)
{
    unsigned char ch = *((unsigned char *)&c);
    const unsigned char *cdat;
    unsigned char *dest;
    int rows, cols;
    int cols_mul, rows_mul;
    unsigned char *pfont = NULL;

    int font_draw_table16[4];

    if (x > (CFB_WIDTH - MTK_VFW * font_scale)) {
        LTRACEF("draw width too large,x=%d\n", x);
        return;
    }
    if (y > (CFB_HEIGHT - MTK_VFH * font_scale)) {
        LTRACEF("draw height too large,y=%d\n", y);
        return;
    }

    dest = cfb_fb_addr + y * LINE_SIZE + x * PIXEL_SIZE;

    pfont = mtk_vdo_fntdata;

    switch (PIXEL_SIZE) {
    case 2:
        font_draw_table16[0] = MAKE_TWO_RGB565_COLOR(cfb_bgx, cfb_bgx);
        font_draw_table16[1] = MAKE_TWO_RGB565_COLOR(cfb_bgx, cfb_fgx);
        font_draw_table16[2] = MAKE_TWO_RGB565_COLOR(cfb_fgx, cfb_bgx);
        font_draw_table16[3] = MAKE_TWO_RGB565_COLOR(cfb_fgx, cfb_fgx);

        cdat = (const unsigned char *)pfont + ch * MTK_VFH;

        for (rows = MTK_VFH; rows--; dest += LINE_SIZE) {
            unsigned char bits = *cdat++;

            ((uint32_t *)dest)[0] = font_draw_table16[bits >> 6];
            ((uint32_t *)dest)[1] = font_draw_table16[bits >> 4 & 3];
            ((uint32_t *)dest)[2] = font_draw_table16[bits >> 2 & 3];
            ((uint32_t *)dest)[3] = font_draw_table16[bits & 3];
        }
        break;
    case 3:
        cdat = (const unsigned char *)pfont + ch * MTK_VFH;
        for (rows = MTK_VFH; rows--; dest += LINE_SIZE*font_scale) {
            unsigned char bits = *cdat++;
            unsigned char *temp_row = dest;

            for (rows_mul = 0; rows_mul < font_scale; rows_mul++) {
                unsigned char *tmp = temp_row;

                for (cols = 0; cols < 8; ++cols) {
                    uint32_t color = ((bits >> (7 - cols)) & 0x1) ? cfb_fgx : cfb_bgx;

                    for (cols_mul = 0; cols_mul < font_scale; cols_mul++) {
                        ((unsigned char *)tmp)[0] = color & 0xff;
                        ((unsigned char *)tmp)[1] = (color >> 8) & 0xff;
                        ((unsigned char *)tmp)[2] = (color >> 16) & 0xff;

                        tmp += 3;
                    }
                }
                temp_row += LINE_SIZE;
            }
        }
        break;
    case 4:
        cdat = (const unsigned char *)pfont + ch * MTK_VFH;
        for (rows = MTK_VFH; rows--; dest += LINE_SIZE*font_scale) {
            unsigned char bits = *cdat++;
            unsigned char *temp_row = dest;

            for (rows_mul = 0; rows_mul < font_scale; rows_mul++) {
                unsigned char *tmp = temp_row;

                for (cols = 0; cols < 8; ++cols) {
                    unsigned int color = ((bits >> (7 - cols)) & 0x1) ? cfb_fgx : cfb_bgx;

                    for (cols_mul = 0; cols_mul < font_scale; cols_mul++) {
                        ((unsigned char *)tmp)[0] = color & 0xff;
                        ((unsigned char *)tmp)[1] = (color >> 8) & 0xff;
                        ((unsigned char *)tmp)[2] = (color >> 16) & 0xff;
                        ((unsigned char *)tmp)[3] = 0xff;

                        tmp += 4;
                    }
                }
                temp_row += LINE_SIZE;
            }
        }
        break;
    default:
        LTRACEF("draw char fail,PIXEL_SIZE=%d\n", PIXEL_SIZE);
        break;
    }
}

void video_drawstr_scale(int font_scale, const char *s)
{
    unsigned int char_count = strlen(s);
    int i = 0;

    while (char_count--) {
        cfb_dchar_scale((get_cols() + i * font_scale) * MTK_VFW,
                        get_rows() * MTK_VFH, font_scale, *s);
        s++;
        i++;
    }

    mt_disp_update(0, 0, CFB_WIDTH, CFB_HEIGHT);
}

void video_drawstr_scale_by_pixel(unsigned int x, unsigned int y, int font_scale, const char *s)
{
    unsigned int char_count = strlen(s);
    int i = 0;

    while (char_count--) {
        cfb_dchar_scale(x + i * font_scale * MTK_VFW, y, font_scale, *s);
        s++;
        i++;
    }

    mt_disp_update(0, 0, CFB_WIDTH, CFB_HEIGHT);
}

static void video_draw_image(void *addr, unsigned int x, unsigned int y,
                             unsigned int w, unsigned int h, const unsigned int *color)
{
    unsigned int i = 0;
    unsigned int j = 0;
    void *start_addr = addr + y * LINE_SIZE + x * PIXEL_SIZE;
    unsigned int *line_addr = start_addr;

    for (j = 0; j < h; j++) {
        line_addr = start_addr;
        for (i = 0; i < w; i++)
            line_addr[i] = (color[j * w + i] & 0xFFFFFF) | 0xFF000000;

        start_addr += LINE_SIZE;
    }

    mt_disp_update(0, 0, 0, 0);
}

static void video_clear_roi(void *addr, unsigned int x, unsigned int y,
                            unsigned int w, unsigned int h, const unsigned int color)
{
    unsigned int i = 0;
    unsigned int j = 0;
    void *start_addr = addr + y * LINE_SIZE + x * PIXEL_SIZE;
    unsigned int *line_addr = start_addr;

    for (j = 0; j < h; j++) {
        line_addr = start_addr;
        for (i = 0; i < w; i++)
            line_addr[i] = color | 0xFF000000;

        start_addr += LINE_SIZE;
    }

    mt_disp_update(0, 0, 0, 0);
}

/* set fg color, now only support color format ARGB */
static void video_set_fg_color_by_pixel(unsigned int fg_color)
{
    unsigned char r, g, b;

    r = fg_color >> 16 & 0xFF;
    g = fg_color >> 8 & 0xFF;
    b = fg_color & 0xFF;
    cfb_fg_alpha = fg_color & 0xFF000000;

    video_set_fg_color(r, g, b);
}

/* set bg color, now only support color format ARGB */
static void video_set_bg_color_by_pixel(unsigned int bg_color)
{
    unsigned char r, g, b;

    r = bg_color >> 16 & 0xFF;
    g = bg_color >> 8 & 0xFF;
    b = bg_color & 0xFF;
    cfb_bg_alpha = bg_color & 0xFF000000;

    video_set_bg_color(r, g, b);
}

static void video_clear_screen_by_bg(void)
{
    video_clear_roi(cfb_fb_addr, 0, 0, CFB_WIDTH, CFB_HEIGHT,
                    cfb_bgx | cfb_bg_alpha);
}

static void video_clear_image_by_color(const unsigned int color, unsigned int x, unsigned int y,
                                       unsigned int width, unsigned int height)
{
    video_clear_roi(cfb_fb_addr, x, y, width, height, color);
}

void video_display_image(const unsigned int data[], unsigned int x, unsigned int y,
                         unsigned int width, unsigned int height)
{
    video_draw_image(cfb_fb_addr, x, y, width, height, data);
}

void video_clear_image(const unsigned int color, unsigned int x, unsigned int y,
                       unsigned int width, unsigned int height)
{
    video_clear_image_by_color(color, x, y, width, height);
}

/* set bg & fg colors, now only support color format ARGB */
void video_set_color(unsigned int bg_color, unsigned int fg_color)
{
    video_set_bg_color_by_pixel(bg_color);
    video_set_fg_color_by_pixel(fg_color);
}

void video_clear(void)
{
    if (pGD)
        video_clear_screen_by_bg();
}

void video_set_pixel_position(unsigned int x, unsigned int y)
{
    if ((x > CFB_WIDTH) || (y > CFB_HEIGHT))
        LTRACEF("coordinate set error\n");

    cfb_pixel_position_x = x;
    cfb_pixel_position_y = y;
}

void video_get_pixel_position(unsigned int *x, unsigned int *y)
{
    if ((x == NULL) || (y == NULL)) {
        LTRACEF("ERROR, pointer is NULL\n");
        return;
    }

    *x = cfb_pixel_position_x;
    *y = cfb_pixel_position_y;
}

void video_disp_string_by_scale(int font_scale, const char *s)
{
    unsigned int x = 0, y = 0;

    if (font_scale < 1) {
        LTRACEF("font_scale error\n");
        return;
    }
    if (s == NULL) {
        LTRACEF("error, string is null\n");
        return;
    }
    video_get_pixel_position(&x, &y);
    video_drawstr_scale_by_pixel(x, y, font_scale, s);
}

static struct GraphicDevice *_get_disp_info(void)
{
    static struct GraphicDevice *gd;
    vaddr_t fb_vaddr = 0;
    u32 fb_size = 0;

    if (gd)
        goto done;

    gd = malloc(sizeof(*gd));
    if (!gd)
        goto done;
    memset(gd, 0, sizeof(*gd));

    fb_vaddr = (vaddr_t)mt_get_fb_addr();
    fb_size = mt_get_fb_size();

    gd->frameAddr   = fb_vaddr + fb_size;
    gd->width       = CFG_DISPLAY_WIDTH;
    gd->height      = CFG_DISPLAY_HEIGHT;
    gd->format      = CFB_X888RGB_32BIT;
    gd->bpp         = CFG_DISPLAY_BPP / 8;
    gd->memSize     = gd->width * gd->height * gd->bpp;

    /* params validation */
    if (!fb_vaddr || !gd->width || !gd->height || !gd->bpp) {
        dprintf(CRITICAL, "gd:fb_addr=0x%08lx, (wxh)=(%dx%d), bpp=%d\n",
               gd->frameAddr, gd->width, gd->height, gd->bpp);
        free(gd);
        gd = NULL;
    }

done:
    return gd;
}

static int video_init(void)
{
    unsigned char color8bit = 0;
    unsigned int data_fmt = 0;

    pGD = _get_disp_info();
    if (!pGD) {
        dprintf(CRITICAL, "video hw init failed, check display driver\n");
        return -1;
    }

    if (!strncmp(MTK_LCM_PHYSICAL_ROTATION, "90", 2) ||
        !strncmp(MTK_LCM_PHYSICAL_ROTATION, "270", 3)) {
        g_cfb[0] = (FONT_WIDTH ? (CFB_HEIGHT / FONT_WIDTH) : 0);
        g_cfb[1] = (FONT_HEIGHT ? (CFB_WIDTH / FONT_HEIGHT) : 0);
    } else {
        g_cfb[0] = (FONT_HEIGHT ? (CFB_HEIGHT / FONT_HEIGHT) : 0);
        g_cfb[1] = (FONT_WIDTH ? (CFB_WIDTH / FONT_WIDTH) : 0);
    }

    cfb_fb_addr = (void *)VDO_CFB_ADDR;
    data_fmt = DATA_FMT;
    /* Init drawing pats */
    switch (data_fmt) {
    case CFB_555RGB_15BIT:
        cfb_fgx = ((FC_5B << 26) | (FC_5B << 21) | (FC_5B << 16) |  /* high 16 0555 */
                   (FC_5B << 10) | (FC_5B << 5) | FC_5B);           /* low 16 0555 */
        cfb_bgx = ((BC_5B << 26) | (BC_5B << 21) | (BC_5B << 16) |  /* high 16 0555 */
                   (BC_5B << 10) | (BC_5B << 5) | BC_5B);           /* low 16 0555 */
        break;
    case CFB_565RGB_16BIT:
        cfb_fgx = ((FC_5B << 27) | (FC_6B << 21) | (FC_5B << 16) |  /* high 16 565 */
                   (FC_5B << 11) | (FC_6B << 5) | FC_5B);           /* low 16 565 */
        cfb_bgx = ((BC_5B << 27) | (BC_6B << 21) | (BC_5B << 16) |  /* high 16 565 */
                   (BC_5B << 11) | (BC_6B << 5) | BC_5B);           /* low 16 565 */
        break;
    case CFB_FMT_8BIT:
        cfb_fgx = 0x01010101;
        cfb_bgx = 0x00000000;
        break;
    case CFB_X888RGB_32BIT:
        cfb_fgx = (CS_FG_COL << 16) | (CS_FG_COL << 8) | CS_FG_COL;
        cfb_bgx = (CS_BG_COL << 16) | (CS_BG_COL << 8) | CS_BG_COL;
        break;
    case CFB_332RGB_8BIT:
        color8bit = ((CS_FG_COL & 0xe0) | ((CS_FG_COL >> 3) & 0x1c) | CS_FG_COL >> 6);
        cfb_fgx = (color8bit << 24) | (color8bit << 16) | (color8bit << 8) | color8bit;
        color8bit = ((CS_BG_COL & 0xe0) | ((CS_BG_COL >> 3) & 0x1c) | CS_BG_COL >> 6);
        cfb_bgx = (color8bit << 24) | (color8bit << 16) | (color8bit << 8) | color8bit;
        break;
    case CFB_888RGB_24BIT:
        cfb_fgx = (CS_FG_COL << 24) | (CS_FG_COL << 16) | (CS_FG_COL << 8) | CS_FG_COL;
        cfb_bgx = (CS_BG_COL << 24) | (CS_BG_COL << 16) | (CS_BG_COL << 8) | CS_BG_COL;
        break;
    default:
        break;
    }
    cfb_eorx = cfb_fgx ^ cfb_bgx;
    cfb_eorx_default = cfb_eorx;
    cfb_fgx_default = cfb_fgx;
    cfb_bgx_default = cfb_bgx;

    /* Initialize cfb char position */
    set_cols(0);
    set_rows(0);

    return 0;
}

int drv_video_init(void)
{
    if (video_init() == -1)
        return -1;

    /*
     * set cursor to the bottom-right corner
     * scroll screen immediately for the first time video_printf()
     */
    video_set_cursor(CFB_ROW_LEN - 1, CFB_COL_LEN);
    return 0;
}
