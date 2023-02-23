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
#include <lib/show_animation_common.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <trace.h>

#include "decompress_common.h"

struct LOGO_PARA_T {
    unsigned int logonum;
    unsigned int logolen;
    vaddr_t inaddr;
 };

static int charging_low_index;
static int charging_animation_index;
static int version0_charging_index;
static int bits;
static int logo_offset = -1;

#define CHECK_LOGO_BIN_OK  0
#define CHECK_LOGO_BIN_ERROR  -1
#define LOCAL_TRACE 0
// version_0 animation
#define ANIM_V0_REGIONS  4
#define ANIM_V0_SPACE_AFTER_REGION  4

struct RECT_REGION_T bar_rect = {BAR_LEFT, BAR_TOP, BAR_RIGHT, BAR_BOTTOM};
struct RECT_REGION_T percent_location_rect = {PERCENT_LEFT, PERCENT_TOP,
                                                    PERCENT_RIGHT, PERCENT_BOTTOM};
static unsigned short  number_pic_addr[(NUMBER_RIGHT - NUMBER_LEFT)*(NUMBER_BOTTOM - NUMBER_TOP)*4]
                                    = {0x0};
static unsigned short  line_pic_addr[(TOP_ANIMATION_RIGHT - TOP_ANIMATION_LEFT)*4] = {0x0};
static unsigned short  percent_pic_addr[(PERCENT_RIGHT - PERCENT_LEFT)*
                                        (PERCENT_BOTTOM - PERCENT_TOP)*4] = {0x0};
static unsigned short  top_animation_addr[(TOP_ANIMATION_RIGHT - TOP_ANIMATION_LEFT)*
                                           (TOP_ANIMATION_BOTTOM - TOP_ANIMATION_TOP)*4] = {0x0};

// dynamic animation logo parameter
//const int top_animation_height = TOP_ANIMATION_BOTTOM - TOP_ANIMATION_TOP;
// num parameter
static const int number_pic_size = (NUMBER_RIGHT - NUMBER_LEFT)*(NUMBER_BOTTOM - NUMBER_TOP)*4;
// line parameter
static const int line_pic_size = (TOP_ANIMATION_RIGHT - TOP_ANIMATION_LEFT)*4;

/*
 * Check logo.bin address if valid, and get logo related info
 */
int check_logo_index_valid(unsigned int index, void *logo_addr, struct LOGO_PARA_T *logo_info)
{
    unsigned int *pinfo = (unsigned int *)logo_addr;

    logo_info->logonum = pinfo[0];
    LTRACEF("logonum =%d, index =%d\n", logo_info->logonum, index);

    if (index >= logo_info->logonum) {
        LTRACEF("unsupported logo, index =%d\n", index);
        return CHECK_LOGO_BIN_ERROR;
    }

    if (index < logo_info->logonum - 1)
        logo_info->logolen = pinfo[3+index] - pinfo[2+index];
    else
        logo_info->logolen = pinfo[1] - pinfo[2+index];

    logo_info->inaddr = (vaddr_t)logo_addr + (vaddr_t)pinfo[2+index];
    LTRACEF("in_addr=0x%08lx,  logolen=%d\n", logo_info->inaddr,  logo_info->logolen);

    return CHECK_LOGO_BIN_OK;
}

int get_total_logo_images_entries(int total_logos)
{
    if (total_logos % LOGOS_COUNT_NORMAL == 0)  // Normal logo
        return LOGOS_COUNT_NORMAL;
    else if ((total_logos % (LOGOS_COUNT_NORMAL + LOGOS_COUNT_FAST_CHARGING)) == 0)// Fast charging
        return (LOGOS_COUNT_NORMAL + LOGOS_COUNT_FAST_CHARGING);
    else if ((total_logos % (LOGOS_COUNT_NORMAL + LOGOS_COUNT_FAST_CHARGING +
                LOGOS_COUNT_WIRELESS)) == 0)
        return (LOGOS_COUNT_NORMAL + LOGOS_COUNT_FAST_CHARGING + LOGOS_COUNT_WIRELESS);
    else
        return CHECK_LOGO_BIN_ERROR;
}

int calculate_logo_offset(unsigned int index, void *dec_logo_addr,
                          void *logo_addr, struct LCM_SCREEN_T physical_screen)
{
    struct LOGO_PARA_T logo_info;
    int logo_width = physical_screen.width;
    int logo_height = physical_screen.height;
    int raw_data_size;
    unsigned int *pinfo;
    unsigned int logo_index = index;
    int total_logo_entries = 0;

    if (logo_offset != -1)
        return logo_offset;

    pinfo = (unsigned int *)logo_addr;
    logo_offset = 0;
    dprintf(INFO, "pinfo[0] = %d\n", pinfo[0]);
    total_logo_entries = get_total_logo_images_entries(pinfo[0]);
    dprintf(INFO, "total_logo_entries = %d\n", total_logo_entries);

    if (total_logo_entries == CHECK_LOGO_BIN_ERROR) {
        dprintf(CRITICAL, "[%s][Error]Logo entries not correct", __func__);
        logo_offset = -1;
        return -1;
    }
    if (total_logo_entries == pinfo[0])
        return logo_offset;

    while (logo_index <= pinfo[0]) {
        if (check_logo_index_valid(logo_index, logo_addr, &logo_info) != CHECK_LOGO_BIN_OK) {
            dprintf(CRITICAL, "[%s][Error]Logo resolution not correct", __func__);
            logo_offset = -1;
            return -1;
        }
        raw_data_size = decompress_logo((void *)logo_info.inaddr, dec_logo_addr, logo_info.logolen,
                                        physical_screen.fb_size);
        dprintf(INFO, "Width = %d\n  Height = %d\n  raw_data_size = %d\n",
                logo_width, logo_height, raw_data_size);

        if (raw_data_size == logo_width*logo_height*4) {
            bits = 32;
            break;
        } else if (raw_data_size == logo_width*logo_height*2) {
            bits = 16;
            break;
        } else {
            logo_offset += total_logo_entries;// Add number of logos entries
            logo_index += total_logo_entries;
        }
        dprintf(INFO, "bits = %d\n", bits);
    }
    dprintf(INFO, "[%s]logo_offset = %d\n", __func__, logo_offset);
    return logo_offset;

}


/*
 * Fill a screen size buffer with logo content
 */
void fill_animation_logo(unsigned int index, void *fill_addr, void *dec_logo_addr, void *logo_addr,
                        struct LCM_SCREEN_T physical_screen)
{
    struct LOGO_PARA_T logo_info;
    int logo_width;
    int logo_height;
    int raw_data_size;
    int logo_index = index;

    int logo_offset = -1;


    logo_offset = calculate_logo_offset(index, dec_logo_addr, logo_addr, physical_screen);
    if (logo_offset == -1)
        return;
    logo_index = logo_index + logo_offset;
    if (check_logo_index_valid(logo_index, logo_addr, &logo_info) != CHECK_LOGO_BIN_OK)
        return;

    logo_width = physical_screen.width;
    logo_height = physical_screen.height;
    struct RECT_REGION_T rect = {0, 0, logo_width, logo_height};

    raw_data_size = decompress_logo((void *)logo_info.inaddr, dec_logo_addr, logo_info.logolen,
                                    physical_screen.fb_size);
    //RECT_REGION_T rect = {0, 0, physical_screen.width, physical_screen.height};

    if (physical_screen.rotation == 270 || physical_screen.rotation == 90) {
        logo_width = physical_screen.height;
        logo_height = physical_screen.width;
    }
    if (bits == 0) {
        if (raw_data_size == logo_width*logo_height*4) {
            bits = 32;
        } else if (raw_data_size == logo_width*logo_height*2) {
            bits = 16;
        } else {
            LTRACEF("Logo data error\n");
            return;
        }
        LTRACEF("bits = %d\n", bits);
    }

    fill_rect_with_content(fill_addr, rect, dec_logo_addr, physical_screen, bits);
}

/*
 * Fill a rectangle size address with special color
 */
void fill_animation_prog_bar(struct RECT_REGION_T rect_bar,
                        unsigned int fgColor,
                        unsigned int start_div, unsigned int occupied_div,
                        void *fill_addr, struct LCM_SCREEN_T physical_screen)
{
    unsigned int div_size  = (rect_bar.bottom - rect_bar.top) / (ANIM_V0_REGIONS);
    unsigned int draw_size = div_size - (ANIM_V0_SPACE_AFTER_REGION);
    unsigned int i;

    for (i = start_div; i < start_div + occupied_div; ++i) {
        unsigned int draw_bottom = rect_bar.bottom - div_size * i - (ANIM_V0_SPACE_AFTER_REGION);
        unsigned int draw_top    = draw_bottom - draw_size;
        struct RECT_REGION_T rect = {rect_bar.left, draw_top, rect_bar.right, draw_bottom};

        fill_rect_with_color(fill_addr, rect, fgColor, physical_screen);
    }
}

/*
 * Fill a rectangle with logo content
 */
void fill_animation_dynamic(unsigned int index, struct RECT_REGION_T rect, void *fill_addr,
                        void *dec_logo_addr, void *logo_addr, struct LCM_SCREEN_T physical_screen)
{
    struct LOGO_PARA_T logo_info;
    int raw_data_size;
    int logo_index = index;

    int logo_offset = 0;


    logo_offset = calculate_logo_offset(index, dec_logo_addr, logo_addr, physical_screen);
    if (logo_offset == -1)
        return;
    logo_index = logo_index + logo_offset;
    if (check_logo_index_valid(logo_index, logo_addr, &logo_info) != CHECK_LOGO_BIN_OK)
        return;

    raw_data_size = decompress_logo((void *)logo_info.inaddr, (void *)dec_logo_addr,
                                logo_info.logolen, (rect.right-rect.left)*(rect.bottom-rect.top)*4);

    if (bits == 0) {
        if (raw_data_size == (rect.right-rect.left)*(rect.bottom-rect.top)*4) {
            bits = 32;
        } else if (raw_data_size == (rect.right-rect.left)*(rect.bottom-rect.top)*2) {
            bits = 16;
        } else {
            LTRACEF("[show_animation_common]Logo data error\n");
            return;
        }
        LTRACEF("[show_animation_common]bits = %d\n", bits);
    }
    fill_rect_with_content(fill_addr, rect, dec_logo_addr, physical_screen, bits);
}


/*
 * Fill a rectangle  with number logo content
 *
 * number_position: 0~1st number, 1~2nd number
 */
void fill_animation_number(unsigned int index, unsigned int number_position, void *fill_addr,
                        void *dec_logo_addr, void *logo_addr, struct LCM_SCREEN_T physical_screen)
{
    LTRACEF("[show_animation_common]index= %d, number_position = %d\n",
                index, number_position);

    struct LOGO_PARA_T logo_info;
    int raw_data_size;
    int logo_index = index;
    int logo_offset = 0;
    struct RECT_REGION_T battery_number_rect = {NUMBER_LEFT +
                            (NUMBER_RIGHT - NUMBER_LEFT)*number_position,
                            NUMBER_TOP,
                            NUMBER_RIGHT + (NUMBER_RIGHT - NUMBER_LEFT)*number_position,
                            NUMBER_BOTTOM};

    logo_offset = calculate_logo_offset(index, dec_logo_addr, logo_addr, physical_screen);
    if (logo_offset == -1)
        return;
    logo_index = logo_index + logo_offset;
    if (check_logo_index_valid(logo_index, logo_addr, &logo_info) != CHECK_LOGO_BIN_OK)
        return;

    // draw default number rect,
    raw_data_size = decompress_logo((void *)logo_info.inaddr, (void *)number_pic_addr,
                                    logo_info.logolen, number_pic_size);

    if (bits == 0) {
        if (raw_data_size == (NUMBER_RIGHT - NUMBER_LEFT)*(NUMBER_BOTTOM - NUMBER_TOP)*4) {
            bits = 32;
        } else if (raw_data_size == (NUMBER_RIGHT - NUMBER_LEFT)*(NUMBER_BOTTOM - NUMBER_TOP)*2) {
            bits = 16;
        } else {
            LTRACEF("[show_animation_common: ]Logo data error\n");
            return;
        }
        LTRACEF("[show_animation_common: ]bits = %d\n", bits);
    }
    fill_rect_with_content(fill_addr, battery_number_rect, number_pic_addr, physical_screen, bits);
}

/*
 * Fill a line with special color
 */
void fill_animation_line(unsigned int index, unsigned int capacity_grids, void *fill_addr,
                         void *dec_logo_addr, void *logo_addr, struct LCM_SCREEN_T physical_screen)
{
    struct LOGO_PARA_T logo_info;
    int raw_data_size;
    int logo_index = index;
    int logo_offset = 0;
    struct RECT_REGION_T rect = {CAPACITY_LEFT, CAPACITY_TOP, CAPACITY_RIGHT, CAPACITY_BOTTOM};

    logo_offset = calculate_logo_offset(index, dec_logo_addr, logo_addr, physical_screen);
    if (logo_offset == -1)
        return;
    logo_index = logo_index + logo_offset;
    if (check_logo_index_valid(logo_index, logo_addr, &logo_info) != CHECK_LOGO_BIN_OK)
        return;

    raw_data_size = decompress_logo((void *)logo_info.inaddr, (void *)line_pic_addr,
                                    logo_info.logolen, line_pic_size);

    if (bits == 0) {
        if (raw_data_size == (TOP_ANIMATION_RIGHT - TOP_ANIMATION_LEFT)*4) {
            bits = 32;
        } else if (raw_data_size == (TOP_ANIMATION_RIGHT - TOP_ANIMATION_LEFT)*2) {
            bits = 16;
        } else {
            LTRACEF("Logo data error\n");
            return;
        }
        LTRACEF("bits = %d\n", bits);
    }
    int i = capacity_grids;

    for (; i < CAPACITY_BOTTOM; i++) {
        rect.top = i;
        rect.bottom = i+1;
        fill_rect_with_content(fill_addr, rect, line_pic_addr, physical_screen, bits);

    }
}

/*
 * Show old charging animation
 */
void fill_animation_battery_old(unsigned int capacity,  void *fill_addr, void *dec_logo_addr,
                                void *logo_addr, struct LCM_SCREEN_T physical_screen)
{
    int capacity_grids = 0;
    struct RECT_REGION_T rect_bar = {bar_rect.left + 1, bar_rect.top + 1, bar_rect.right,
                                     bar_rect.bottom};

    if (capacity > 100)
        capacity = 100;

    capacity_grids = (capacity * (ANIM_V0_REGIONS)) / 100;

    if (version0_charging_index < capacity_grids * 2)
        version0_charging_index = capacity_grids * 2;

    if (capacity < 100) {
        version0_charging_index > 7 ? version0_charging_index = capacity_grids * 2 :
                                      version0_charging_index++;
    } else {
        version0_charging_index = ANIM_V0_REGIONS * 2;
    }

    fill_animation_logo(ANIM_V0_BACKGROUND_INDEX, fill_addr, dec_logo_addr, logo_addr,
                        physical_screen);

    fill_animation_prog_bar(rect_bar,
                        (unsigned int)(BAR_OCCUPIED_COLOR),
                        0,  version0_charging_index/2,
                        fill_addr, physical_screen);

    fill_animation_prog_bar(rect_bar,
                       (unsigned int)(BAR_EMPTY_COLOR),
                       version0_charging_index/2, ANIM_V0_REGIONS - version0_charging_index/2,
                       fill_addr, physical_screen);
}

/*
 * Show new charging animation
 */
void fill_animation_battery_new(unsigned int capacity, void *fill_addr, void *dec_logo_addr,
                                void *logo_addr, struct LCM_SCREEN_T physical_screen)
{
    LTRACEF("capacity : %d\n", capacity);

    if (capacity >= 100) {
        //show_logo(37); // battery 100
        fill_animation_logo(FULL_BATTERY_INDEX, fill_addr, dec_logo_addr, logo_addr,
                            physical_screen);
    } else if (capacity < 10) {
        LTRACEF("charging_low_index = %d\n", charging_low_index);
        charging_low_index++;

        fill_animation_logo(LOW_BAT_ANIM_START_0 + charging_low_index, fill_addr, dec_logo_addr,
                            logo_addr, physical_screen);
        fill_animation_number(NUMBER_PIC_START_0 + capacity, 1, fill_addr, dec_logo_addr, logo_addr,
                              physical_screen);
        fill_animation_dynamic(NUMBER_PIC_PERCENT, percent_location_rect, fill_addr,
                               percent_pic_addr, logo_addr, physical_screen);

        if (charging_low_index >= 9)
          charging_low_index = 0;
    } else {
        unsigned int capacity_grids = 0;

        capacity_grids = CAPACITY_BOTTOM - (CAPACITY_BOTTOM - CAPACITY_TOP) * (capacity - 10) / 90;
        LTRACEF("capacity_grids : %d,charging_animation_index = %d\n",
                capacity_grids, charging_animation_index);

        //background
        fill_animation_logo(ANIM_V1_BACKGROUND_INDEX, fill_addr, dec_logo_addr, logo_addr,
                            physical_screen);
        fill_animation_line(ANIM_LINE_INDEX, capacity_grids, fill_addr, dec_logo_addr, logo_addr,
                            physical_screen);
        fill_animation_number(NUMBER_PIC_START_0 + (capacity/10), 0, fill_addr, dec_logo_addr,
                            logo_addr, physical_screen);
        fill_animation_number(NUMBER_PIC_START_0 + (capacity%10), 1, fill_addr, dec_logo_addr,
                            logo_addr, physical_screen);
        fill_animation_dynamic(NUMBER_PIC_PERCENT, percent_location_rect, fill_addr,
                            percent_pic_addr, logo_addr, physical_screen);

         if (capacity <= 90) {
            struct RECT_REGION_T top_animation_rect = {TOP_ANIMATION_LEFT, capacity_grids -
                   (TOP_ANIMATION_BOTTOM - TOP_ANIMATION_TOP), TOP_ANIMATION_RIGHT, capacity_grids};
            charging_animation_index++;
            fill_animation_dynamic(BAT_ANIM_START_0 + charging_animation_index, top_animation_rect,
                                   fill_addr, top_animation_addr, logo_addr, physical_screen);

            if (charging_animation_index >= 9)
                charging_animation_index = 0;
         }
    }
}

/*
 * Show charging animation by version
 */
void fill_animation_battery_by_ver(unsigned int capacity, void *fill_addr, void *dec_logo_addr,
                                  void *logo_addr, struct LCM_SCREEN_T physical_screen, int version)
{
    LTRACEF("version : %d\n", version);
    switch (version) {
    case VERSION_OLD_ANIMATION:
        fill_animation_battery_old(capacity, fill_addr, dec_logo_addr, logo_addr, physical_screen);
        break;
    case VERSION_NEW_ANIMATION:
        fill_animation_battery_new(capacity, fill_addr, dec_logo_addr, logo_addr, physical_screen);
        break;
    case VERSION_WIRELESS_CHARGING_ANIMATION:
        break;
    default:
        fill_animation_battery_old(capacity, fill_addr, dec_logo_addr, logo_addr, physical_screen);
        break;
    }
}
