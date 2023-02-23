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

#include <lib/cust_display.h>
#include <lib/show_logo_common.h>
#include <stdbool.h>

#define VERSION_OLD_ANIMATION 0
#define VERSION_NEW_ANIMATION 1
#define VERSION_WIRELESS_CHARGING_ANIMATION  2

void fill_animation_logo(unsigned int index, void *fill_addr, void *dec_logo_addr,
                        void *logo_addr, struct LCM_SCREEN_T physical_screen);
void fill_animation_prog_bar(struct RECT_REGION_T rect_bar,
                        unsigned int fgColor,
                        unsigned int start_div, unsigned int occupied_div,
                        void *fill_addr, struct LCM_SCREEN_T physical_screen);
void fill_animation_dynamic(unsigned int index, struct RECT_REGION_T rect, void *fill_addr,
                        void *dec_logo_addr, void *logo_addr, struct LCM_SCREEN_T physical_screen);
void fill_animation_number(unsigned int index, unsigned int number_position, void *fill_addr,
                        void *dec_logo_addr, void *logo_addr, struct LCM_SCREEN_T physical_screen);
void fill_animation_line(unsigned int index, unsigned int capacity_grids, void *fill_addr,
                        void *dec_logo_addr, void *logo_addr, struct LCM_SCREEN_T physical_screen);
void fill_animation_battery_old(unsigned int capacity, void *fill_addr, void *dec_logo_addr,
                        void *logo_addr, struct LCM_SCREEN_T physical_screen);
void fill_animation_battery_new(unsigned int capacity, void *fill_addr, void *dec_logo_addr,
                        void *logo_addr, struct LCM_SCREEN_T physical_screen);
void fill_animation_battery_by_ver(unsigned int capacity, void *fill_addr, void *dec_logo_addr,
                        void *logo_addr, struct LCM_SCREEN_T physical_screen, int version);
