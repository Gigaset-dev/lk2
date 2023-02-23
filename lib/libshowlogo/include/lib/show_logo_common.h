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

// define the rectangle parameters
 struct RECT_REGION_T {
     int left, top, right, bottom;
 };

// dedfine the LCM SCREEM parameters
 struct LCM_SCREEN_T {
     int         width;
     int         height;
     int         bits_per_pixel;
     int         rotation;    // phical screen rotation:0 , 90, 180, 270
     int         needAllign;  // if need adjust the width or height with 32: no need  (0), need (1)
     int         allignWidth;
     int         need180Adjust;// if need adjust the drawing logo for 180 roration
     int         fb_size;
     int         fill_dst_bits;
     int         red_offset; // if red_offset is 0: logo use format BGR565 or ABGR8888
     int         blue_offset;// if blue_offset is 11/16: logo use format BGR565 or ABGR8888,
                              // blue_offset is 0: RGB565, ARGB8888
 };

/* public interface */

/*
 * Draw a rectangle with logo content
 *
 * @parameter
 *
 */
void fill_rect_with_content(void *fill_addr, struct RECT_REGION_T rect, void *src_addr,
                        struct LCM_SCREEN_T physical_screen, unsigned int bits);

/*
 * Draw a rectangle with spcial color
 *
 * @parameter
 *
 */
void fill_rect_with_color(void *fill_addr, struct RECT_REGION_T rect, unsigned int src_color,
                        struct LCM_SCREEN_T physical_screen);
