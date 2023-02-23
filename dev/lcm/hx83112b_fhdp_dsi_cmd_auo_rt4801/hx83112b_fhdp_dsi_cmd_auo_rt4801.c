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

#define LOG_TAG "LCM"

#include <debug.h>
#include <platform.h>
#include "lcm_drv.h"
#include "lcm_util.h"

#ifdef MTK_ROUND_CORNER_SUPPORT
#include "data_hw_roundedpattern.h"
#endif

#define LCM_LOGI(string, args...)  dprintf(0, "[LK/"LOG_TAG"]"string, ##args)
#define LCM_LOGD(string, args...)  dprintf(1, "[LK/"LOG_TAG"]"string, ##args)


static LCM_UTIL_FUNCS lcm_util;

#define SET_RESET_PIN(v, fdt)    (lcm_util.set_reset_pin(v, fdt))
#define MDELAY(n)       (lcm_util.mdelay(n))
#define UDELAY(n)       (lcm_util.udelay(n))

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update) \
        lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update) \
        lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd) lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums) \
        lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)   lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size) \
        lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)


#define FRAME_WIDTH         (1080)
#define FRAME_HEIGHT            (2160)

#define REGFLAG_DELAY           0xFFFC
#define REGFLAG_UDELAY          0xFFFB
#define REGFLAG_END_OF_TABLE        0xFFFD
#define REGFLAG_RESET_LOW       0xFFFE
#define REGFLAG_RESET_HIGH      0xFFFF

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

struct LCM_setting_table {
    unsigned int cmd;
    unsigned char count;
    unsigned char para_list[64];
};

/* currently no use
static struct LCM_setting_table lcm_suspend_setting[] = {
    {0x28, 0, {} },
    {REGFLAG_DELAY, 50, {} },
    {0x10, 0, {} },
    {REGFLAG_DELAY, 150, {} },
};
*/
static struct LCM_setting_table init_setting_vdo[] = {
    {0xB9, 0x03, {0x83, 0x11, 0x2B} },
    {
        0xB1, 0x0A, {
            0xF8, 0x29, 0x29, 0x00, 0x00, 0x0F, 0x14, 0x0F,
            0x14, 0x33
        }
    },
    {0xD2, 0x02, {0x2C, 0x2C} },
    {
        0xB2, 0x0B, {
            0x80, 0x02, 0x00, 0x80, 0x70, 0x00, 0x08, 0x1C,
            0x05, 0x01, 0x04
        }
    },
    {0xE9, 0x01, {0xD1} },
    {0xB2, 0x02, {0x00, 0x08} },
    {0xE9, 0x01, {0x00} },
    {0xE9, 0x01, {0xCE} },
    {0xB2, 0x01, {0xA3} },
    {0xE9, 0x01, {0x00} },
    {0xBD, 0x01, {0x02} },
    {0xB2, 0x02, {0xB5, 0x0A} },
    {0xBD, 0x01, {0x00} },
    {0xDD, 0x08, {0x00, 0x00, 0x08, 0x1C, 0x09, 0x34, 0x34, 0x8B} },
    {
        0xB4, 0x18, {
            0x01, 0xD3, 0x00, 0x00, 0x00, 0x00, 0x03, 0xD0,
            0x00, 0x00, 0x0F, 0xCB, 0x01, 0x00, 0x00, 0x13,
            0x00, 0x2E, 0x08, 0x01, 0x12, 0x00, 0x00, 0x2E
        }
    },
    {0xBD, 0x01, {0x02} },
    {0xB4, 0x01, {0x92} },
    {0xBD, 0x01, {0x00} },
    {0xB6, 0x03, {0x81, 0x81, 0xE3} },
    {0xC0, 0x01, {0x44} },
    {0xCC, 0x01, {0x08} },
    {0xBD, 0x01, {0x03} },
    {
        0xC1, 0x39, {
            0xFF, 0xFA, 0xF6, 0xF3, 0xEF, 0xEB, 0xE7, 0xE0,
            0xDC, 0xD9, 0xD6, 0xD2, 0xCF, 0xCB, 0xC7, 0xC3,
            0xBF, 0xBB, 0xB7, 0xB0, 0xA8, 0xA1, 0x9A, 0x92,
            0x89, 0x81, 0x7A, 0x73, 0x6B, 0x63, 0x5A, 0x51,
            0x48, 0x40, 0x38, 0x31, 0x29, 0x20, 0x16, 0x0D,
            0x09, 0x07, 0x05, 0x02, 0x00, 0x08, 0x2E, 0xF6,
            0x20, 0x18, 0x94, 0xF8, 0x6F, 0x59, 0x18, 0xFC,
            0x00
        }
    },
    {0xBD, 0x01, {0x02} },
    {
        0xC1, 0x39, {
            0xFF, 0xFA, 0xF6, 0xF3, 0xEF, 0xEB, 0xE7, 0xE0,
            0xDC, 0xD9, 0xD6, 0xD2, 0xCF, 0xCB, 0xC7, 0xC3,
            0xBF, 0xBB, 0xB7, 0xB0, 0xA8, 0xA1, 0x9A, 0x92,
            0x89, 0x81, 0x7A, 0x73, 0x6B, 0x63, 0x5A, 0x51,
            0x48, 0x40, 0x38, 0x31, 0x29, 0x20, 0x16, 0x0D,
            0x09, 0x07, 0x05, 0x02, 0x00, 0x08, 0x2E, 0xF6,
            0x20, 0x18, 0x94, 0xF8, 0x6F, 0x59, 0x18, 0xFC,
            0x00
        }
    },
    {0xBD, 0x01, {0x01} },
    {
        0xC1, 0x39, {
            0xFF, 0xFA, 0xF6, 0xF3, 0xEF, 0xEB, 0xE7, 0xE0,
            0xDC, 0xD9, 0xD6, 0xD2, 0xCF, 0xCB, 0xC7, 0xC3,
            0xBF, 0xBB, 0xB7, 0xB0, 0xA8, 0xA1, 0x9A, 0x92,
            0x89, 0x81, 0x7A, 0x73, 0x6B, 0x63, 0x5A, 0x51,
            0x48, 0x40, 0x38, 0x31, 0x29, 0x20, 0x16, 0x0D,
            0x09, 0x07, 0x05, 0x02, 0x00, 0x08, 0x2E, 0xF6,
            0x20, 0x18, 0x94, 0xF8, 0x6F, 0x59, 0x18, 0xFC,
            0x00
        }
    },
    {0xBD, 0x01, {0x00} },
    {0xC1, 0x01, {0x01} },
    {
        0xD3, 0x16, {
            0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x0A,
            0x0A, 0x07, 0x00, 0x00, 0x08, 0x09, 0x09, 0x09,
            0x09, 0x32, 0x10, 0x09, 0x00, 0x09
        }
    },
    {0xE9, 0x01, {0xE3} },
    {0xD3, 0x03, {0x05, 0x08, 0x86} },
    {0xE9, 0x01, {0x00} },
    {0xBD, 0x01, {0x01} },
    {0xE9, 0x01, {0xC8} },
    {0xD3, 0x01, {0x81} },
    {0xE9, 0x01, {0x00} },
    {0xBD, 0x01, {0x00} },
    {
        0xD5, 0x30, {
            0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
            0x31, 0x31, 0x30, 0x30, 0x2F, 0x2F, 0x31, 0x31,
            0x30, 0x30, 0x2F, 0x2F, 0xC0, 0x18, 0x40, 0x40,
            0x01, 0x00, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02,
            0x21, 0x20, 0x18, 0x18, 0x19, 0x19, 0x18, 0x18,
            0x03, 0x03, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18
        }
    },
    {
        0xD6, 0x30, {
            0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
            0x31, 0x31, 0x30, 0x30, 0x2F, 0x2F, 0x31, 0x31,
            0x30, 0x30, 0x2F, 0x2F, 0xC0, 0x18, 0x40, 0x40,
            0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x00, 0x01,
            0x20, 0x21, 0x18, 0x18, 0x18, 0x18, 0x19, 0x19,
            0x20, 0x20, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18
        }
    },
    {
        0xD8, 0x18, {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        }
    },
    {0xBD, 0x01, {0x01} },
    {
        0xD8, 0x18, {
            0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
            0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
            0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA
        }
    },
    {0xBD, 0x01, {0x02} },
    {
        0xD8, 0x0C, {
            0xAF, 0xFF, 0xFA, 0xAA, 0xBA, 0xAA, 0xAA, 0xFF,
            0xFA, 0xAA, 0xBA, 0xAA
        }
    },
    {0xBD, 0x01, {0x03} },
    {
        0xD8, 0x18, {
            0xAA, 0xAA, 0xAB, 0xAA, 0xAE, 0xAA, 0xAA, 0xAA,
            0xAB, 0xAA, 0xAE, 0xAA, 0xAA, 0xFF, 0xFA, 0xAA,
            0xBA, 0xAA, 0xAA, 0xFF, 0xFA, 0xAA, 0xBA, 0xAA
        }
    },
    {0xBD, 0x01, {0x00} },
    {
        0xE7, 0x19, {
            0x09, 0x09, 0x00, 0x07, 0xE6, 0x00, 0x27, 0x00,
            0x07, 0x00, 0x00, 0xE6, 0x2A, 0x00, 0xE6, 0x00,
            0x0A, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x12,
            0x04
        }
    },
    {0xE9, 0x01, {0xE4} },
    {0xE7, 0x02, {0x17, 0x69} },
    {0xE9, 0x01, {0x00} },
    {0xBD, 0x01, {0x01} },
    {
        0xE7, 0x09, {
            0x02, 0x00, 0x01, 0x20, 0x01, 0x0E, 0x08, 0xEE,
            0x09
        }
    },
    {0xBD, 0x01, {0x02} },
    {0xE7, 0x03, {0x20, 0x20, 0x00} },
    {0xBD, 0x01, {0x03} },
    {0xE7, 0x06, {0x00, 0x08, 0x01, 0x00, 0x00, 0x20} },
    {0xE9, 0x01, {0xC9} },
    {0xE7, 0x02, {0x2E, 0xCB} },
    {0xE9, 0x01, {0x00} },
    {0xBD, 0x01, {0x00} },
    {0xD1, 0x01, {0x27} },
    {0xBC, 0x01, {0x07} },
    {0xBD, 0x01, {0x01} },
    {0xE9, 0x01, {0xC2} },
    {0xCB, 0x01, {0x27} },
    {0xE9, 0x01, {0x00} },
    {0xBD, 0x01, {0x00} },
    {0x51, 0x02, {0x0F, 0xFF} },
    {0x53, 0x01, {0x24} },
    {0x55, 0x01, {0x00} },
    {0x35, 0x01, {0x00} },
    {0x44, 0x02, {0x08, 0x66} }, /* set TE event @ line 0x866(2150) */

    {0x11, 0, {} },

#ifndef LCM_SET_DISPLAY_ON_DELAY
    {REGFLAG_DELAY, 150, {} },

    {0xE9, 0x01, {0xC2} },
    {0xB0, 0x01, {0x01} },
    {0xE9, 0x01, {0x00} },

    {0x29, 0, {} },
    {REGFLAG_DELAY, 50, {} },
#endif
};

#ifdef LCM_SET_DISPLAY_ON_DELAY
/* to reduce init time, we move 120ms delay to lcm_set_display_on() !! */
static struct LCM_setting_table set_display_on[] = {
    {0xE9, 0x01, {0xC2} },
    {0xB0, 0x01, {0x01} },
    {0xE9, 0x01, {0x00} },

    {0x29, 0, {} },
    {REGFLAG_DELAY, 50, {} },
};
#endif

static struct LCM_setting_table bl_level[] = {
    {0x51, 1, {0xFF} },
    {REGFLAG_END_OF_TABLE, 0x00, {} }
};

static void push_table(struct LCM_setting_table *table, unsigned int count,
                       unsigned char force_update)
{
    unsigned int i;
    unsigned int cmd;

    for (i = 0; i < count; i++) {
        cmd = table[i].cmd;
        switch (cmd) {
        case REGFLAG_DELAY:
            if (table[i].count <= 10)
                MDELAY(table[i].count);
            else
                MDELAY(table[i].count);
            break;
        case REGFLAG_UDELAY:
            UDELAY(table[i].count);
            break;
        case REGFLAG_END_OF_TABLE:
            break;
        default:
            dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list,
                            force_update);
            break;
        }
    }
}

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}

static void lcm_get_params(LCM_PARAMS *params)
{
    memset(params, 0, sizeof(LCM_PARAMS));

    params->type = LCM_TYPE_DSI;

    params->width = FRAME_WIDTH;
    params->height = FRAME_HEIGHT;

    params->dsi.mode = CMD_MODE;
    params->dsi.switch_mode = SYNC_PULSE_VDO_MODE;
    params->dsi.switch_mode_enable = 0;

    /* DSI */
    /* Command mode setting */
    params->dsi.LANE_NUM = LCM_FOUR_LANE;
    /* The following defined the fomat for data coming from LCD engine. */
    params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
    params->dsi.data_format.trans_seq = LCM_DSI_TRANS_SEQ_MSB_FIRST;
    params->dsi.data_format.padding = LCM_DSI_PADDING_ON_LSB;
    params->dsi.data_format.format = LCM_DSI_FORMAT_RGB888;

    /* Highly depends on LCD driver capability. */
    params->dsi.packet_size = 256;
    /* video mode timing */

    params->dsi.PS = LCM_PACKED_PS_24BIT_RGB888;

    params->dsi.vertical_sync_active = 2;
    params->dsi.vertical_backporch = 8;
    params->dsi.vertical_frontporch = 20;
    params->dsi.vertical_frontporch_for_low_power = 620;
    params->dsi.vertical_active_line = FRAME_HEIGHT;

    params->dsi.horizontal_sync_active = 10;
    params->dsi.horizontal_backporch = 20;
    params->dsi.horizontal_frontporch = 40;
    params->dsi.horizontal_active_pixel = FRAME_WIDTH;
    /* params->dsi.ssc_disable = 1; */
#ifndef MACH_FPGA
    /* this value must be in MTK suggested table */
    params->dsi.PLL_CLOCK = 480;
#else
    params->dsi.pll_div1 = 0;
    params->dsi.pll_div2 = 0;
    params->dsi.fbk_div = 0x1;
#endif
    params->dsi.CLK_HS_POST = 36;
    params->dsi.clk_lp_per_line_enable = 0;
    params->dsi.esd_check_enable = 1;
    params->dsi.customization_esd_check_enable = 0;
    params->dsi.lcm_esd_check_table[0].cmd = 0x0a;
    params->dsi.lcm_esd_check_table[0].count = 1;
    params->dsi.lcm_esd_check_table[0].para_list[0] = 0x9c;

#ifdef MTK_ROUND_CORNER_SUPPORT
    params->round_corner_params.round_corner_en = 1;
    params->round_corner_params.full_content = 0;
    params->round_corner_params.h = ROUND_CORNER_H_TOP;
    params->round_corner_params.h_bot = ROUND_CORNER_H_BOT;
    params->round_corner_params.tp_size = sizeof(top_rc_pattern);
    params->round_corner_params.lt_addr = (void *)top_rc_pattern;
#endif
}

/* turn on gate ic & control voltage to 5.5V */
static void lcm_init_power(void *fdt)
{
    if (lcm_util.set_gpio_lcd_enp_bias) {
        lcm_util.set_gpio_lcd_enp_bias(1, fdt);
        //_lcm_i2c_write_bytes(0x0, 0xf);
        //_lcm_i2c_write_bytes(0x1, 0xf);
    } else
        LCM_LOGI("set_gpio_lcd_enp_bias not defined\n");
}

static void lcm_suspend_power(void)
{
}

static void lcm_resume_power(void)
{
}

#ifdef LCM_SET_DISPLAY_ON_DELAY
static u32 lcm_init_time;
static int is_display_on;
#endif

static void lcm_init(void *fdt)
{
    SET_RESET_PIN(0, fdt);

    SET_RESET_PIN(1, fdt);
    MDELAY(1);
    SET_RESET_PIN(0, fdt);
    MDELAY(10);

    SET_RESET_PIN(1, fdt);
    MDELAY(10);

    push_table(init_setting_vdo, sizeof(init_setting_vdo) /
               sizeof(struct LCM_setting_table), 1);

#ifdef LCM_SET_DISPLAY_ON_DELAY
    lcm_init_time = current_time();
    is_display_on = 0;
#endif
}

#ifdef LCM_SET_DISPLAY_ON_DELAY
static int lcm_set_display_on(void)
{
    u32 timeout, i = 0;

    if (is_display_on)
        return 0;

    /* we need to wait 150ms after lcm init to set display on */
    timeout = 120;

    while ((current_time() - lcm_init_time) <= timeout) {
        i++;
        MDELAY(1);
        if (i % 1000 == 0) {
            LCM_LOGI("hx83112b %s error: i=%u,lcm_init_time=%u,cur_time=%u\n", __func__, i,
                lcm_init_time, current_time());
        }
    }

    push_table(set_display_on, sizeof(set_display_on) /
               sizeof(struct LCM_setting_table), 1);

    is_display_on = 1;
    return 0;
}
#endif

static void lcm_suspend(void)
{
}

static void lcm_resume(void)
{
}

#if 1
static void lcm_update(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
    unsigned int x0 = x;
    unsigned int y0 = y;
    unsigned int x1 = x0 + width - 1;
    unsigned int y1 = y0 + height - 1;

    unsigned char x0_MSB = ((x0 >> 8) & 0xFF);
    unsigned char x0_LSB = (x0 & 0xFF);
    unsigned char x1_MSB = ((x1 >> 8) & 0xFF);
    unsigned char x1_LSB = (x1 & 0xFF);
    unsigned char y0_MSB = ((y0 >> 8) & 0xFF);
    unsigned char y0_LSB = (y0 & 0xFF);
    unsigned char y1_MSB = ((y1 >> 8) & 0xFF);
    unsigned char y1_LSB = (y1 & 0xFF);

    unsigned int data_array[16];

#ifdef LCM_SET_DISPLAY_ON_DELAY
    lcm_set_display_on();
#endif

    data_array[0] = 0x00053902;
    data_array[1] = (x1_MSB << 24) | (x0_LSB << 16) | (x0_MSB << 8) | 0x2a;
    data_array[2] = (x1_LSB);
    dsi_set_cmdq(data_array, 3, 1);

    data_array[0] = 0x00053902;
    data_array[1] = (y1_MSB << 24) | (y0_LSB << 16) | (y0_MSB << 8) | 0x2b;
    data_array[2] = (y1_LSB);
    dsi_set_cmdq(data_array, 3, 1);

    data_array[0] = 0x002c3909;
    dsi_set_cmdq(data_array, 1, 0);
}
#else
static void lcm_update(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
#ifdef LCM_SET_DISPLAY_ON_DELAY
    lcm_set_display_on();
#endif
}
#endif

static unsigned int lcm_ata_check(unsigned char *buffer)
{
    return 0;
}

static void lcm_setbacklight_cmdq(void *handle, unsigned int level)
{
    LCM_LOGI("%s,hx83112b backlight: level = %d\n", __func__, level);

    bl_level[0].para_list[0] = level;

    push_table(bl_level, sizeof(bl_level) /
               sizeof(struct LCM_setting_table), 1);
}

static void lcm_setbacklight(unsigned int level)
{
    LCM_LOGI("%s,hx83112b backlight: level = %d\n", __func__, level);

    bl_level[0].para_list[0] = level;

    push_table(bl_level, sizeof(bl_level) /
               sizeof(struct LCM_setting_table), 1);
}

LCM_DRIVER hx83112b_fhdp_dsi_cmd_auo_rt4801_lcm_drv = {
    .name = "hx83112b_fhdp_dsi_cmd_auo_rt4801_drv",
    .set_util_funcs = lcm_set_util_funcs,
    .get_params = lcm_get_params,
    .init = lcm_init,
    .suspend = lcm_suspend,
    .resume = lcm_resume,
    .init_power = lcm_init_power,
    .resume_power = lcm_resume_power,
    .suspend_power = lcm_suspend_power,
    .set_backlight = lcm_setbacklight,
    .ata_check = lcm_ata_check,
    .update = lcm_update,
};

