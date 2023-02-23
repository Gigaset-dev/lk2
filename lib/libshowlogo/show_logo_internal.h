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

/* internal use function */

int  check_rect_valid(struct RECT_REGION_T rect);
void fill_point_buffer(unsigned int *fill_addr, unsigned int src_color,
                struct LCM_SCREEN_T physical_screen, unsigned int bits);
void fill_rect_with_content_by_32bit_argb8888(unsigned int *fill_addr, struct RECT_REGION_T rect,
                unsigned int *src_addr, struct LCM_SCREEN_T physical_screen, unsigned int bits);
void fill_rect_with_content_by_32bit_rgb565(unsigned int *fill_addr, struct RECT_REGION_T rect,
                unsigned short *src_addr, struct LCM_SCREEN_T physical_screen, unsigned int bits);
void fill_rect_with_color_by_32bit(unsigned int *fill_addr, struct RECT_REGION_T rect,
                unsigned int src_color, struct LCM_SCREEN_T physical_screen);
void fill_rect_with_content_by_16bit_argb8888(unsigned short *fill_addr, struct RECT_REGION_T rect,
                unsigned int *src_addr, struct LCM_SCREEN_T physical_screen);
void fill_rect_with_content_by_16bit_rgb565(unsigned short *fill_addr, struct RECT_REGION_T rect,
                unsigned short *src_addr, struct LCM_SCREEN_T physical_screen);
void fill_rect_with_color_by_16bit(unsigned short *fill_addr, struct RECT_REGION_T rect,
                unsigned int src_color, struct LCM_SCREEN_T physical_screen);
