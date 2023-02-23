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

#include <lib/show_logo_common.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <trace.h>

#include "show_logo_internal.h"

#define LOCAL_TRACE 0
#define CHECK_RECT_OK  0
#define CHECK_RECT_SIZE_ERROR  -1

#define RGB565_TO_ARGB8888(x)   \
    ((((x) &   0x1F) << 3) |    \
     (((x) &  0x7E0) << 5) |    \
     (((x) & 0xF800) << 8) |    \
     (0xFF << 24)) // opaque

#define RGB565_TO_ABGR8888(x)   \
    ((((x) &   0x1F) << 19) |   \
     (((x) &  0x7E0) <<  5) |   \
     (((x) & 0xF800) >>  8) |   \
     (0xFF << 24)) // opaque

#define ARGB8888_TO_ABGR8888(x) \
    ((((x) &   0xFF) << 16)  |  \
      ((x) & 0xFF00)         |  \
     (((x) & 0xFF0000) >> 16)|  \
      ((x) & 0xFF000000))

#define ARGB8888_TO_RGB565(x)      \
    (((((x) >> 19) & 0x1F) << 11)| \
     ((((x) >> 10) & 0x3F) <<  5)| \
      (((x) >>  3) & 0x1F))

#define ARGB8888_TO_BGR565(x)      \
     ((((x) >> 19) & 0x1F)       | \
     ((((x) >> 10) & 0x3F) <<  5)| \
      (((x) >>  3) & 0x1F) << 11)

#define ALIGN_TO(x, n)  \
    (((x) + ((n) - 1)) & ~((n) - 1))

/*
 * Fill one point address with  source color and color pixel bits
 *
 * @parameter
 *
 */
void fill_point_buffer(unsigned int *fill_addr, unsigned int src_color,
                       struct LCM_SCREEN_T physical_screen, unsigned int bits)
{
    if (physical_screen.bits_per_pixel == 32) {
        if (bits == 32) {
            if (physical_screen.blue_offset == 16)
                *fill_addr = ARGB8888_TO_ABGR8888(src_color);
            else
                *fill_addr = src_color;
        } else {
            if (physical_screen.blue_offset == 16)
                *fill_addr = RGB565_TO_ARGB8888(src_color);
            else
                *fill_addr = RGB565_TO_ABGR8888(src_color);
        }
    } else {
        LTRACEF("not support bits_per_pixel = %d\n", (int)physical_screen.bits_per_pixel);
    }
}

/*
 * Check the rectangle if valid
 */
int check_rect_valid(struct RECT_REGION_T rect)
{
    int draw_width = rect.right - rect.left;
    int draw_height = rect.bottom - rect.top;

    if (draw_width < 0 || draw_height < 0) {
        LTRACEF("drawing width or height is error\n");
        LTRACEF("rect:left= %d ,right= %d,top= %d,bottom= %d\n",
             rect.left, rect.right, rect.top, rect.bottom);

        return CHECK_RECT_SIZE_ERROR;
    }

    return CHECK_RECT_OK;
}

/*
 * Draw a rectangle region (32 bits perpixel) with logo content
 */
void fill_rect_with_content_by_32bit_argb8888(unsigned int *fill_addr, struct RECT_REGION_T rect,
                     unsigned int *src_addr, struct LCM_SCREEN_T physical_screen, unsigned int bits)
{
    int virtual_width = physical_screen.needAllign == 0 ?
                        physical_screen.width:physical_screen.allignWidth;
    int virtual_height = physical_screen.height;
    int i = 0;
    int j = 0;
    unsigned int *dst_addr = fill_addr;
    unsigned int *color_addr = src_addr;

    LTRACE_ENTRY;

    for (i = rect.top; i < rect.bottom; i++) {
        for (j = rect.left; j < rect.right; j++) {
            color_addr = (unsigned int *)src_addr;
            src_addr++;
            switch (physical_screen.rotation) {
            case 90:
                dst_addr = fill_addr + (virtual_width * j  + virtual_width - i - 1);
                break;
            case 270:
                dst_addr = fill_addr + ((virtual_width * (virtual_height - j - 1) + i));
                break;
            case 180:
                // adjust fill in address
                dst_addr = fill_addr + virtual_width * (virtual_height - i) - j-1
                               -(virtual_width-physical_screen.width);
                break;
            default:
                dst_addr = fill_addr + virtual_width * i + j;
            }
            fill_point_buffer(dst_addr, *color_addr, physical_screen, bits);
            if ((i == rect.top && j == rect.left) || (i == rect.bottom - 1 && j == rect.left) ||
                        (i == rect.top && j == rect.right - 1) ||
                        (i == rect.bottom - 1 && j == rect.right - 1)) {
                LTRACEF("dst_addr= 0x%08x, color_addr= 0x%08x,i= %d, j=%d\n",
                        *dst_addr, *color_addr, i, j);
            }
        }
    }
}

/*
 * Draw a rectangle region (32 bits perpixel) with logo content
 */
void fill_rect_with_content_by_32bit_rgb565(unsigned int *fill_addr, struct RECT_REGION_T rect,
                   unsigned short *src_addr, struct LCM_SCREEN_T physical_screen, unsigned int bits)
{
    int draw_height = rect.bottom - rect.top;
    int virtual_width = physical_screen.needAllign == 0 ?
                        physical_screen.width:physical_screen.allignWidth;
    int virtual_height = physical_screen.height;
    int i = 0;
    int j = 0;
    unsigned int *dst_addr = fill_addr;
    unsigned short *color_addr = src_addr;

    LTRACE_ENTRY;

    for (i = rect.top; i < rect.bottom; i++) {
        for (j = rect.left; j < rect.right; j++) {
            switch (physical_screen.rotation) {
            case 90:
                color_addr = src_addr + draw_height*j + i;
                dst_addr = fill_addr + ((virtual_width * i + virtual_width - j));
                break;
            case 270:
                color_addr = src_addr + draw_height*j + i;
                dst_addr = fill_addr + ((virtual_width * (virtual_height - i - 1) + j));
                break;
            case 180:
                // adjust fill in address
                color_addr = src_addr++;
                dst_addr = fill_addr + virtual_width * (virtual_height - i) - j-1
                       -(virtual_width-physical_screen.width);
                break;
            default:
                color_addr = src_addr++;
                dst_addr = fill_addr + virtual_width * i + j;
            }
            fill_point_buffer(dst_addr, *color_addr, physical_screen, bits);
            if ((i == rect.top && j == rect.left) || (i == rect.bottom - 1 && j == rect.left) ||
                (i == rect.top && j == rect.right - 1) ||
                (i == rect.bottom - 1 && j == rect.right - 1)) {
                LTRACEF("dst_addr= 0x%08x, color_addr= 0x%08x,i= %d, j=%d\n",
                        *dst_addr, *color_addr, i, j);
            }
        }
    }
}

/*
 * Draw a rectangle region (16 bits per pixel) with logo content
 */
void fill_rect_with_content_by_16bit_argb8888(unsigned short *fill_addr, struct RECT_REGION_T rect,
                                        unsigned int *src_addr, struct LCM_SCREEN_T physical_screen)
{
    int virtual_width = physical_screen.needAllign == 0 ?
                        physical_screen.width:physical_screen.allignWidth;
    int virtual_height = physical_screen.height;
    int i = 0;
    int j = 0;
    unsigned short *dst_addr = fill_addr;
    unsigned int *color_addr = src_addr;

    LTRACE_ENTRY;

    for (i = rect.top; i < rect.bottom; i++) {
        for (j = rect.left; j < rect.right; j++) {
            switch (physical_screen.rotation) {
            case 90:
                color_addr = src_addr++;
                dst_addr = fill_addr + (virtual_width * j  + virtual_width - i - 1);
                break;
            case 270:
                color_addr = src_addr++;
                dst_addr = fill_addr + ((virtual_width * (virtual_height - j - 1) + i));
                break;
            case 180:
                // adjust fill in address
                color_addr = src_addr++;
                dst_addr = fill_addr + virtual_width * (virtual_height - i) - j-1
                       -(virtual_width-physical_screen.width);
                break;
            default:
                color_addr = src_addr++;
                dst_addr = fill_addr + virtual_width * i + j;

            }
            if (physical_screen.blue_offset == 11)
                *dst_addr = ARGB8888_TO_BGR565(*color_addr);
            else
                *dst_addr = ARGB8888_TO_RGB565(*color_addr);

            if ((i == rect.top && j == rect.left) || (i == rect.bottom - 1 && j == rect.left) ||
                (i == rect.top && j == rect.right - 1) || (i == rect.bottom - 1 &&
                j == rect.right - 1)) {
                LTRACEF("dst_addr= 0x%08x, color_addr= 0x%08x,i= %d, j=%d\n",
                        *dst_addr, *color_addr, i, j);
            }
        }
    }
}

/*
 * Draw a rectangle region (16 bits per pixel) with logo content
 */
void fill_rect_with_content_by_16bit_rgb565(unsigned short *fill_addr, struct RECT_REGION_T rect,
                                    unsigned short *src_addr, struct LCM_SCREEN_T physical_screen)
{
    int draw_height = rect.bottom - rect.top;
    int virtual_width = physical_screen.needAllign == 0 ?
                        physical_screen.width:physical_screen.allignWidth;
    int virtual_height = physical_screen.height;
    int i = 0;
    int j = 0;
    unsigned short *dst_addr = fill_addr;
    unsigned short *color_addr = src_addr;

    LTRACE_ENTRY;

    for (i = rect.top; i < rect.bottom; i++) {
        for (j = rect.left; j < rect.right; j++) {
            switch (physical_screen.rotation) {
            case 90:
                color_addr = src_addr + draw_height*j + i;
                dst_addr = fill_addr + ((virtual_width * i + virtual_width - j));
                break;
            case 270:
                color_addr = src_addr + draw_height*j + i;
                dst_addr = fill_addr + ((virtual_width * (virtual_height - i - 1) + j));
                break;
            case 180:
                // adjust fill in address
                color_addr = src_addr++;
                dst_addr = fill_addr + virtual_width * (virtual_height - i) - j-1-
                              (virtual_width-physical_screen.width);
                break;
            default:
                color_addr = src_addr++;
                dst_addr = fill_addr + virtual_width * i + j;
            }
            *dst_addr = *color_addr;
            if ((i == rect.top && j == rect.left) || (i == rect.bottom - 1 && j == rect.left) ||
               (i == rect.top && j == rect.right - 1) || (i == rect.bottom - 1 &&
               j == rect.right - 1)) {
                LTRACEF("[show_logo_common]dst_addr= 0x%08x, color_addr= 0x%08x,i= %d, j=%d\n",
                       *dst_addr, *color_addr, i, j);
            }
        }
    }
}

/*
 * Draw aa rectangle region (32 bits per pixel)with spcial color
 */
void fill_rect_with_color_by_32bit(unsigned int *fill_addr, struct RECT_REGION_T rect,
                                   unsigned int src_color, struct LCM_SCREEN_T physical_screen)
{
    //int virtual_width = physical_screen.width;
    int virtual_width = physical_screen.needAllign == 0 ?
                        physical_screen.width:physical_screen.allignWidth;
    int virtual_height = physical_screen.height;
    int i = 0;
    int j = 0;
    unsigned int *dst_addr = fill_addr;

    LTRACE_ENTRY;

    for (i = rect.top; i < rect.bottom; i++) {
        for (j = rect.left; j < rect.right; j++) {
            switch (physical_screen.rotation) {
            case 90:
                dst_addr = fill_addr + ((virtual_width * i + virtual_width - j));
                break;
            case 270:
                dst_addr = fill_addr + ((virtual_width * (virtual_height - i - 1) + j));
                break;
            case 180:
                dst_addr = fill_addr + virtual_width * (virtual_height - i) - j-1;
                break;
            default:
                dst_addr = fill_addr + virtual_width * i + j;
            }
            fill_point_buffer(dst_addr, src_color, physical_screen, 32);
        }
    }
}

/*
 * Draw a rectangle region (16 bits per pixel)  with spcial color
 */
void fill_rect_with_color_by_16bit(unsigned short *fill_addr, struct RECT_REGION_T rect,
                                   unsigned int src_color, struct LCM_SCREEN_T physical_screen)
{
    //int virtual_width = physical_screen.width;
    int virtual_width = physical_screen.needAllign == 0 ?
                                physical_screen.width:physical_screen.allignWidth;
    int virtual_height = physical_screen.height;
    int i = 0;
    int j = 0;
    unsigned short *dst_addr = fill_addr;

    LTRACE_ENTRY;

    for (i = rect.top; i < rect.bottom; i++) {
        for (j = rect.left; j < rect.right; j++) {
            switch (physical_screen.rotation) {
            case 90:
                dst_addr = fill_addr + ((virtual_width * i + virtual_width - j));
                break;
            case 270:
                dst_addr = fill_addr + ((virtual_width * (virtual_height - i - 1) + j));
                break;
            case 180:
                dst_addr = fill_addr + virtual_width * (virtual_height - i) - j - 1;
                break;
            default:
                dst_addr = fill_addr + virtual_width * i + j;
            }
            *dst_addr = (unsigned short)src_color;
        }
    }
}

/*
 * Draw a rectangle region  with logo content
 */
void fill_rect_with_content(void *fill_addr, struct RECT_REGION_T rect, void *src_addr,
                            struct LCM_SCREEN_T physical_screen, unsigned int bits)
{
    LTRACE_ENTRY;

    if (check_rect_valid(rect) != CHECK_RECT_OK)
        return;
    if (bits == 32) {
        if (physical_screen.fill_dst_bits == 16) {
            fill_rect_with_content_by_16bit_argb8888((unsigned short *)fill_addr, rect,
                    (unsigned int *)src_addr, physical_screen);
        } else if (physical_screen.fill_dst_bits == 32) {
            fill_rect_with_content_by_32bit_argb8888((unsigned int *)fill_addr, rect,
                    (unsigned int *)src_addr, physical_screen, bits);
        } else {
            LTRACEF("unsupported physical_screen.fill_dst_bits =%d\n",
                    physical_screen.fill_dst_bits);
        }
    } else {
        if (physical_screen.fill_dst_bits == 16) {
            fill_rect_with_content_by_16bit_rgb565((unsigned short *)fill_addr, rect,
                    (unsigned short *)src_addr, physical_screen);
        } else if (physical_screen.fill_dst_bits == 32) {
            fill_rect_with_content_by_32bit_rgb565((unsigned int *)fill_addr, rect,
                    (unsigned short *)src_addr, physical_screen, bits);
        } else {
            LTRACEF("unsupported physical_screen.fill_dst_bits =%d\n",
                    physical_screen.fill_dst_bits);
        }
    }
}

/*
 * Draw a rectangle region  with spcial color
 */
void fill_rect_with_color(void *fill_addr, struct RECT_REGION_T rect, unsigned int src_color,
                          struct LCM_SCREEN_T physical_screen)
{
    LTRACE_ENTRY;

    if (check_rect_valid(rect) != CHECK_RECT_OK)
        return;
    if (physical_screen.bits_per_pixel == 16) {
        fill_rect_with_color_by_16bit((unsigned short *)fill_addr, rect,
                                src_color, physical_screen);
    } else if (physical_screen.bits_per_pixel == 32) {
        fill_rect_with_color_by_32bit((unsigned int *)fill_addr, rect,
                                src_color, physical_screen);
    } else {
        LTRACEF("unsupported physical_screen.fill_dst_bits =%d\n",
                                physical_screen.fill_dst_bits);
    }
}
