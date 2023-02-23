/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */


#define LOG_TAG "LCM"

#include <debug.h>
#include <mt_i2c.h>
#include <platform.h>

#include "lcm_drv.h"
#include "lcm_util.h"

#ifdef MTK_RT4831A_SUPPORT
#include "rt4831a_drv.h"
#endif

#define LCM_LOGI(string, args...)  dprintf(0, "[LK/"LOG_TAG"]"string, ##args)
#define LCM_LOGD(string, args...)  dprintf(1, "[LK/"LOG_TAG"]"string, ##args)

static LCM_UTIL_FUNCS lcm_util;

#define SET_RESET_PIN(v, fdt)    (lcm_util.set_reset_pin(v, fdt))
#define MDELAY(n)        (lcm_util.mdelay(n))
#define UDELAY(n)        (lcm_util.udelay(n))

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update) \
        lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update) \
        lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd) lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums) \
        lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)    lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size) \
        lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)

#define FRAME_WIDTH            (1080)
#define FRAME_HEIGHT            (2408)

#define REGFLAG_DELAY            0xFFFC
#define REGFLAG_UDELAY            0xFFFB
#define REGFLAG_END_OF_TABLE        0xFFFD
#define REGFLAG_RESET_LOW        0xFFFE
#define REGFLAG_RESET_HIGH        0xFFFF

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define LCM_ID_NT36672E (0x6e)

struct LCM_setting_table {
    unsigned int cmd;
    unsigned char count;
    unsigned char para_list[200];
};

#if 0
static struct LCM_setting_table lcm_suspend_setting[] = {
    {0x28, 0, {} },
    {REGFLAG_DELAY, 50, {} },
    {0x10, 0, {} },
    {REGFLAG_DELAY, 150, {} },
};
#endif

static struct LCM_setting_table init_setting_cmd[] = {
    {0xFF, 1, {0x10} },
    {0xFB, 1, {0x01} },
    {0x36, 1, {0x00} },
    {0x3B, 5, {0x03, 0x23, 0x36, 0x04, 0x04} },
    {0xB0, 1, {0x00} },
    {0xC1, 16, {0x89, 0x28,
        0x00, 0x08, 0x00, 0xAA, 0x02, 0x0E, 0x00,
        0x2B, 0x00, 0x07, 0x0D, 0xB7, 0x0C, 0xB7} },
    {0xC2, 1, {0x1B, 0xA0} },
    {0xE9, 1, {0x01} },

    {0xFF, 1, {0x20} },
    {0xFB, 1, {0x01} },
    {0x01, 1, {0x66} },
    {0x07, 1, {0x28} },
    {0x17, 1, {0x66} },
    {0x1B, 1, {0x01} },
    {0x2D, 1, {0x00} },
    {0x2F, 1, {0x83} },
    {0x30, 1, {0x1F} },
    {0x69, 1, {0xD0} },
    {0x95, 1, {0xA9} },
    {0x96, 1, {0xA9} },
    {0xF2, 1, {0x66} },
    {0xF3, 1, {0x54} },
    {0xF4, 1, {0x66} },
    {0xF5, 1, {0x54} },
    {0xF6, 1, {0x66} },
    {0xF7, 1, {0x54} },
    {0xF8, 1, {0x66} },
    {0xF9, 1, {0x54} },

    {0xFF, 1, {0x23} },
    {0xFB, 1, {0x01} },
    {0x07, 1, {0x20} },
    {0x08, 1, {0x07} },
    {0x09, 1, {0x04} },

    {0xFF, 1, {0x24} },
    {0xFB, 1, {0x01} },
    {0x00, 1, {0x20} },
    {0x01, 1, {0x20} },
    {0x02, 1, {0x20} },
    {0x03, 1, {0x20} },
    {0x04, 1, {0x20} },
    {0x05, 1, {0x20} },
    {0x06, 1, {0x13} },
    {0x07, 1, {0x15} },
    {0x08, 1, {0x17} },
    {0x09, 1, {0x13} },
    {0x0A, 1, {0x15} },
    {0x0B, 1, {0x17} },
    {0x0C, 1, {0x24} },
    {0x0D, 1, {0x01} },
    {0x0E, 1, {0x2F} },
    {0x0F, 1, {0x2D} },
    {0x10, 1, {0x2E} },
    {0x11, 1, {0x2C} },
    {0x12, 1, {0x8B} },
    {0x13, 1, {0x8C} },
    {0x14, 1, {0x20} },
    {0x15, 1, {0x20} },
    {0x16, 1, {0x0F} },
    {0x17, 1, {0x22} },
    {0x18, 1, {0x20} },
    {0x19, 1, {0x20} },
    {0x1A, 1, {0x20} },
    {0x1B, 1, {0x20} },
    {0x1C, 1, {0x20} },
    {0x1D, 1, {0x20} },
    {0x1E, 1, {0x13} },
    {0x1F, 1, {0x15} },
    {0x20, 1, {0x17} },
    {0x21, 1, {0x13} },
    {0x22, 1, {0x15} },
    {0x23, 1, {0x17} },
    {0x24, 1, {0x24} },
    {0x25, 1, {0x01} },
    {0x26, 1, {0x2F} },
    {0x27, 1, {0x2D} },
    {0x28, 1, {0x2E} },
    {0x29, 1, {0x2C} },
    {0x2A, 1, {0x8B} },
    {0x2B, 1, {0x8C} },
    {0x2D, 1, {0x20} },
    {0x2F, 1, {0x20} },
    {0x30, 1, {0x0F} },
    {0x31, 1, {0x22} },
    {0x32, 1, {0x00} },
    {0x33, 1, {0x03} },
    {0x35, 1, {0x3C} },
    {0x36, 1, {0x01} },
    {0x4D, 1, {0x04} },
    {0x4E, 1, {0x38} },
    {0x4F, 1, {0x38} },
    {0x53, 1, {0x38} },
    {0x79, 1, {0x40} },
    {0x7A, 1, {0x82} },
    {0x7B, 1, {0x8F} },
    {0x82, 1, {0x13} },
    {0x84, 1, {0x31} },
    {0x85, 1, {0x00} },
    {0x86, 1, {0x00} },
    {0x87, 1, {0x00} },
    {0x90, 1, {0x13} },
    {0x92, 1, {0x31} },
    {0x93, 1, {0x00} },
    {0x94, 1, {0x00} },
    {0x95, 1, {0x00} },
    {0x9C, 1, {0xF4} },
    {0x9D, 1, {0x01} },
    {0xA0, 1, {0x0F} },
    {0xA2, 1, {0x0F} },
    {0xA3, 1, {0x02} },
    {0xC4, 1, {0x80} },
    {0xC6, 1, {0xC0} },
    {0xC9, 1, {0x00} },
    {0xD9, 1, {0x80} },
    {0xE9, 1, {0x02} },

    {0xFF, 1, {0x25} },
    {0xFB, 1, {0x01} },
    {0x19, 1, {0xE4} },
    {0x21, 1, {0xC0} },
    {0x66, 1, {0x40} },
    {0x67, 1, {0x29} },
    {0x68, 1, {0x50} },
    {0x69, 1, {0x60} },
    {0x6B, 1, {0x00} },
    {0x71, 1, {0x6D} },
    {0x77, 1, {0x60} },
    {0x79, 1, {0x7E} },
    {0x7D, 1, {0x40} },
    {0x7E, 1, {0x2D} },
    {0xC0, 1, {0x4D} },
    {0xC1, 1, {0xA9} },
    {0xC2, 1, {0xD2} },
    {0xC4, 1, {0x11} },
    {0xD6, 1, {0x80} },
    {0xD7, 1, {0x02} },
    {0xDA, 1, {0x02} },
    {0xDD, 1, {0x02} },
    {0xE0, 1, {0x02} },
    {0xF0, 1, {0x00} },
    {0xF1, 1, {0x04} },

    {0xFF, 1, {0x26} },
    {0xFB, 1, {0x01} },
    {0x00, 1, {0x10} },
    {0x01, 1, {0xFB} },
    {0x03, 1, {0x00} },
    {0x04, 1, {0xFB} },
    {0x05, 1, {0x08} },
    {0x06, 1, {0x10} },
    {0x08, 1, {0x10} },
    {0x14, 1, {0x06} },
    {0x15, 1, {0x01} },
    {0x74, 1, {0xAF} },
    {0x81, 1, {0x0E} },
    {0x83, 1, {0x02} },
    {0x85, 1, {0x01} },
    {0x87, 1, {0x01} },
    {0x88, 1, {0x04} },
    {0x8A, 1, {0x1A} },
    {0x8B, 1, {0x11} },
    {0x8C, 1, {0x24} },
    {0x8E, 1, {0x42} },
    {0x8F, 1, {0x11} },
    {0x90, 1, {0x11} },
    {0x91, 1, {0x11} },
    {0x9A, 1, {0x80} },
    {0x9B, 1, {0x04} },
    {0x9C, 1, {0x00} },
    {0x9D, 1, {0x00} },
    {0x9E, 1, {0x00} },

    {0xFF, 1, {0x27} },
    {0xFB, 1, {0x01} },
    {0x01, 1, {0x68} },
    {0x20, 1, {0x81} },
    {0x21, 1, {0x6A} },
    {0x25, 1, {0x81} },
    {0x26, 1, {0x94} },
    {0x6E, 1, {0x23} },
    {0x6F, 1, {0x01} },
    {0x70, 1, {0x00} },
    {0x71, 1, {0x00} },
    {0x72, 1, {0x00} },
    {0x73, 1, {0x21} },
    {0x74, 1, {0x03} },
    {0x75, 1, {0x00} },
    {0x76, 1, {0x00} },
    {0x77, 1, {0x00} },
    {0x7D, 1, {0x09} },
    {0x7E, 1, {0x6B} },
    {0x80, 1, {0x23} },
    {0x82, 1, {0x09} },
    {0x83, 1, {0x6B} },
    {0x88, 1, {0x03} },
    {0x89, 1, {0x01} },
    {0xE3, 1, {0x01} },
    {0xE4, 1, {0xE2} },
    {0xE5, 1, {0x02} },
    {0xE6, 1, {0xD3} },
    {0xE9, 1, {0x02} },
    {0xEA, 1, {0x1A} },
    {0xEB, 1, {0x03} },
    {0xEC, 1, {0x28} },

    {0xFF, 1, {0x2A} },
    {0xFB, 1, {0x01} },
    {0x00, 1, {0x91} },
    {0x03, 1, {0x20} },
    {0x04, 1, {0x4C} },
    {0x07, 1, {0x4D} },
    {0x0A, 1, {0x70} },
    {0x0C, 1, {0x04} },
    {0x0D, 1, {0x40} },
    {0x0F, 1, {0x01} },
    {0x11, 1, {0xE1} },
    {0x15, 1, {0x0F} },
    {0x16, 1, {0xAE} },
    {0x19, 1, {0x0F} },
    {0x1A, 1, {0x82} },
    {0x1B, 1, {0x23} },
    {0x1D, 1, {0x36} },
    {0x1E, 1, {0x3E} },
    {0x1F, 1, {0x3E} },
    {0x20, 1, {0x3E} },
    {0x28, 1, {0xFD} },
    {0x29, 1, {0x0B} },
    {0x2A, 1, {0x6D} },
    {0x2D, 1, {0x0B} },
    {0x2F, 1, {0x02} },
    {0x30, 1, {0x85} },
    {0x33, 1, {0x9A} },
    {0x34, 1, {0xFF} },
    {0x35, 1, {0x40} },
    {0x36, 1, {0x12} },
    {0x37, 1, {0xF9} },
    {0x38, 1, {0x45} },
    {0x39, 1, {0x0D} },
    {0x3A, 1, {0x85} },
    {0x45, 1, {0x06} },
    {0x46, 1, {0x40} },
    {0x47, 1, {0x02} },
    {0x48, 1, {0x01} },
    {0x4A, 1, {0x58} },
    {0x4E, 1, {0x0F} },
    {0x4F, 1, {0xAE} },
    {0x52, 1, {0x0F} },
    {0x53, 1, {0x82} },
    {0x54, 1, {0x23} },
    {0x56, 1, {0x36} },
    {0x57, 1, {0x52} },
    {0x58, 1, {0x52} },
    {0x59, 1, {0x52} },
    {0x7A, 1, {0x09} },
    {0x7B, 1, {0x40} },
    {0x7F, 1, {0xF0} },
    {0x83, 1, {0x0F} },
    {0x84, 1, {0xAE} },
    {0x87, 1, {0x0F} },
    {0x88, 1, {0x82} },
    {0x89, 1, {0x23} },
    {0x8B, 1, {0x36} },
    {0x8C, 1, {0x7D} },
    {0x8D, 1, {0x7D} },
    {0x8E, 1, {0x7D} },

    {0xFF, 1, {0x2C} },
    {0xFB, 1, {0x01} },
    {0x00, 1, {0x02} },
    {0x01, 1, {0x02} },
    {0x02, 1, {0x02} },
    {0x03, 1, {0x14} },
    {0x04, 1, {0x14} },
    {0x05, 1, {0x14} },
    {0x0D, 1, {0x50} },
    {0x0E, 1, {0x01} },
    {0x17, 1, {0x48} },
    {0x18, 1, {0x48} },
    {0x19, 1, {0x48} },
    {0x2D, 1, {0xAF} },
    {0x2F, 1, {0x10} },
    {0x30, 1, {0xFB} },
    {0x32, 1, {0x00} },
    {0x33, 1, {0xFB} },
    {0x35, 1, {0x15} },
    {0x37, 1, {0x15} },
    {0x4D, 1, {0x14} },
    {0x4E, 1, {0x02} },
    {0x4F, 1, {0x06} },
    {0x53, 1, {0x02} },
    {0x54, 1, {0x02} },
    {0x55, 1, {0x02} },
    {0x61, 1, {0x78} },
    {0x62, 1, {0x01} },
    {0x80, 1, {0xAF} },
    {0x81, 1, {0x10} },
    {0x82, 1, {0xFB} },
    {0x84, 1, {0x00} },
    {0x85, 1, {0xFB} },
    {0x87, 1, {0x20} },
    {0x89, 1, {0x20} },
    {0x9D, 1, {0x1E} },
    {0x9E, 1, {0x02} },
    {0x9F, 1, {0x13} },

    {0xFF, 1, {0xF0} },
    {0xFB, 1, {0x01} },
    {0x5A, 1, {0x00} },
    {0x9F, 1, {0x19} },

    {0xFF, 1, {0xE0} },
    {0xFB, 1, {0x01} },
    {0x25, 1, {0x00} },
    {0x35, 1, {0x82} },
    {0x4E, 1, {0x00} },
    {0x55, 1, {0x00} },
    {0x85, 1, {0x32} },

    {0xFF, 1, {0xC0} },
    {0xFB, 1, {0x01} },
    {0x9C, 1, {0x11} },
    {0x9D, 1, {0x11} },

    {0xFF, 1, {0xD0} },
    {0xFB, 1, {0x01} },
    {0x53, 1, {0x22} },
    {0x54, 1, {0x02} },

    {0XFF, 1, {0X25} },
    {0XFB, 1, {0X01} },
    {0X18, 1, {0X22} },
    {0X79, 1, {0X7F} },

    {0XFF, 1, {0X2A} },
    {0XFB, 1, {0X01} },
    {0X7A, 1, {0X09} },
    {0X7C, 1, {0X02} },
    {0X7F, 1, {0XF0} },
    {0X8C, 1, {0X7E} },
    {0X8D, 1, {0X7E} },
    {0X8E, 1, {0X7E} },

    {0XFF, 1, {0X2C} },
    {0XFB, 1, {0X01} },
    {0X87, 1, {0X20} },
    {0X89, 1, {0X20} },
    {0X9F, 1, {0X0D} },

    {0xFF, 1, {0x10} },
    {0x51, 1, {0xFF} },
    {0x53, 1, {0x24} },
    {0x55, 1, {0x00} },
    {0x35, 1, {0x00} },

    {0x29, 1, {0x00} },
    {0x11, 1, {0x00} },

    {REGFLAG_DELAY, 100, {} },
};

#ifdef LCM_SET_DISPLAY_ON_DELAY
/* to reduce init time, we move 120ms delay to lcm_set_display_on() !! */
static struct LCM_setting_table set_display_on[] = {
    {0x29, 0, {} },
    {REGFLAG_DELAY, 100, {} }, //Delay 100ms
};
#endif

static struct LCM_setting_table bl_level[] = {
    {0x51, 1, {0xFF} },
    {REGFLAG_END_OF_TABLE, 0x00, {} }
};

static void push_table(struct LCM_setting_table *table,
        unsigned int count, unsigned char force_update)
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

    params->dsi.mode = BURST_VDO_MODE;
//    params->dsi.mode = CMD_MODE;            //mtk code
    params->dsi.switch_mode = CMD_MODE;
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

    /* params->dsi.ssc_disable = 1; */
    params->dsi.vertical_sync_active = 8;
    params->dsi.vertical_backporch = 20;
    params->dsi.vertical_frontporch = 54;
    params->dsi.vertical_frontporch_for_low_power = 888;//45hz
    params->dsi.vertical_active_line = FRAME_HEIGHT;

    params->dsi.horizontal_sync_active = 18;
    params->dsi.horizontal_backporch = 22;
    params->dsi.horizontal_frontporch = 814;
    params->dsi.horizontal_active_pixel = FRAME_WIDTH;
    params->dsi.HS_PRPR = 10;
    params->dsi.ssc_disable = 1;
#ifndef MACH_FPGA
    /* this value must be in MTK suggested table */
    params->dsi.PLL_CLOCK = 587;
#else
    params->dsi.pll_div1 = 0;
    params->dsi.pll_div2 = 0;
    params->dsi.fbk_div = 0x1;
#endif
    params->dsi.clk_lp_per_line_enable = 0;
    params->dsi.esd_check_enable = 1;
    params->dsi.customization_esd_check_enable = 0;
    params->dsi.lcm_esd_check_table[0].cmd = 0x0a;
    params->dsi.lcm_esd_check_table[0].count = 1;
    params->dsi.lcm_esd_check_table[0].para_list[0] = 0x9c;

    params->dsi.dsc_enable = 1;
    params->dsi.dsc_params.ver = 17;
    params->dsi.dsc_params.slice_mode = 1;
    params->dsi.dsc_params.rgb_swap = 0;
    params->dsi.dsc_params.dsc_cfg = 34;
    params->dsi.dsc_params.rct_on = 1;
    params->dsi.dsc_params.bit_per_channel = 8;
    params->dsi.dsc_params.dsc_line_buf_depth = 9;
    params->dsi.dsc_params.bp_enable = 1;
    params->dsi.dsc_params.bit_per_pixel = 128;  //128
    params->dsi.dsc_params.pic_height = 2408;
    params->dsi.dsc_params.pic_width = 1080;
    params->dsi.dsc_params.slice_height = 8;
    params->dsi.dsc_params.slice_width = 540;
    params->dsi.dsc_params.chunk_size = 540;
    params->dsi.dsc_params.xmit_delay = 170;
    params->dsi.dsc_params.dec_delay = 526;
    params->dsi.dsc_params.scale_value = 32;
    params->dsi.dsc_params.increment_interval = 43;
    params->dsi.dsc_params.decrement_interval = 7;
    params->dsi.dsc_params.line_bpg_offset = 12;
    params->dsi.dsc_params.nfl_bpg_offset = 3511;
    params->dsi.dsc_params.slice_bpg_offset = 3255;
    params->dsi.dsc_params.initial_offset = 6144;
    params->dsi.dsc_params.final_offset = 7072;
    params->dsi.dsc_params.flatness_minqp = 3;
    params->dsi.dsc_params.flatness_maxqp = 12;
    params->dsi.dsc_params.rc_model_size = 8192;
    params->dsi.dsc_params.rc_edge_factor = 6;
    params->dsi.dsc_params.rc_quant_incr_limit0 = 11;
    params->dsi.dsc_params.rc_quant_incr_limit1 = 11;
    params->dsi.dsc_params.rc_tgt_offset_hi = 3;
    params->dsi.dsc_params.rc_tgt_offset_lo = 3;
}

/* turn on gate ic & control voltage to 5.5V */
static void lcm_init_power(void *fdt)
{
    SET_RESET_PIN(0, fdt);

#ifdef MTK_RT4831A_SUPPORT
    if (gate_ic_power_status_setting(fdt, 1)) {
        LCM_LOGI("set_gpio_lcm_led_en not defined\n");
        return;
    }
    _gate_ic_i2c_panel_bias_enable(fdt, 1);
#endif
    LCM_LOGI("-----%s------\n", __func__);
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
    MDELAY(5);
    SET_RESET_PIN(1, fdt);
    MDELAY(5);
    SET_RESET_PIN(0, fdt);
    MDELAY(5);

    SET_RESET_PIN(1, fdt);
    MDELAY(15);

    push_table(init_setting_cmd, sizeof(init_setting_cmd) /
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

    /* we need to wait 200ms after lcm init to set display on */
    timeout = 200;

    while ((current_time() - lcm_init_time) <= timeout) {
        i++;
        MDELAY(1);
        if (i % 1000 == 0) {
            LCM_LOGI("td4320 %s error: i=%u,lcm_init_time=%u,cur_time=%u\n",
                 __func__, i, lcm_init_time, current_time());
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
#if 0
    push_table(lcm_suspend_setting, sizeof(lcm_suspend_setting) /
           sizeof(struct LCM_setting_table), 1);

    SET_RESET_PIN(0);
    MDELAY(10);
#endif
}

static void lcm_resume(void)
{
#if 0
    lcm_init();
#endif
}

static void lcm_setbacklight(unsigned int level)
{
    printf("%s,nt36672e,tianma backlight: level = %d\n", __func__, level);

    bl_level[0].para_list[0] = level;

    push_table(bl_level, sizeof(bl_level) /
           sizeof(struct LCM_setting_table), 1);
}

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

static unsigned int lcm_compare_id(void *fdt)
{
    unsigned int id0 = 0;
    unsigned char buffer[2];
    unsigned int array[16];

    SET_RESET_PIN(1, fdt);
    SET_RESET_PIN(0, fdt);
    MDELAY(1);

    SET_RESET_PIN(1, fdt);
    MDELAY(20);

    array[0] = 0x00013700;
    dsi_set_cmdq(array, 1, 1);

    read_reg_v2(0xDA, buffer, 1);
    id0 = buffer[0];

    dprintf(0, "%s, nt36672e debug: id:0x%x, expected:0x%08x\n", __func__, id0, LCM_ID_NT36672E);

    if (id0 == LCM_ID_NT36672E)
        return 1;
    else
        return 0;

}

LCM_DRIVER nt36672e_fhdp_dsi_vdo_120hz_tianma_hfp_lcm_drv = {
    .name = "nt36672e_fhdp_dsi_vdo_120hz_tianma_hfp_lcm_drv",
    .set_util_funcs = lcm_set_util_funcs,
    .get_params = lcm_get_params,
    .init = lcm_init,
    .suspend = lcm_suspend,
    .resume = lcm_resume,
    .compare_id = lcm_compare_id,
    .init_power = lcm_init_power,
    .resume_power = lcm_resume_power,
    .suspend_power = lcm_suspend_power,
    .set_backlight = lcm_setbacklight,
    //.ata_check = lcm_ata_check,
    .update = lcm_update,
};
