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

int drv_video_init(void);

void video_putc(const char c);
void video_puts(const char *s);
void video_printf(const char *fmt, ...);
void video_clear_screen(void);

void video_set_cursor(int row, int col);
int video_get_rows(void);
int video_get_cols(void);

void video_set_fg_color(unsigned char r, unsigned char g, unsigned char b);
void video_set_bg_color(unsigned char r, unsigned char g, unsigned char b);
void video_set_default_color(void);

void video_set_pixel_position(unsigned int x, unsigned int y);
void video_get_pixel_position(unsigned int *x, unsigned int *y);
void video_disp_string_by_scale(int font_scale, const char *s);
void video_display_image(const unsigned int data[],
                         unsigned int x, unsigned int y,
                         unsigned int width, unsigned int height);
void video_clear_image(const unsigned int data,
                       unsigned int x, unsigned int y,
                       unsigned int width, unsigned int height);
void video_set_color(unsigned int bg_color, unsigned int fg_color);
void video_clear(void);
