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
#define FRAME_HEIGHT            (2300)

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

/* i2c control start */
#define LCM_I2C_MODE    ST_MODE
#define LCM_I2C_SPEED   100

static int _lcm_i2c_write_bytes(u8 addr, u8 value, void *fdt)
{
    s32 ret;
    u8 write_data[2];
    u32 len;
    int i2c_id, i2c_addr;

    /* get i2c id and addr */
    if (lcm_util_get_i2c_lcd_bias(&i2c_id, &i2c_addr, fdt) < 0)
        return -1;

    i2c_id = i2c_id & 0xff;
    i2c_addr = i2c_addr & 0xff;

    /* pkt data */
    write_data[0] = addr;
    write_data[1] = value;
    len = 2;

    ret =  mtk_i2c_write((u8)i2c_id, (u8)i2c_addr, write_data, len, NULL);
    if (ret < 0)
        LCM_LOGI("[LCM][ERROR] %s: %d\n", __func__, ret);

    LCM_LOGI("[LCM][i2c write] %s: ch:%u,addr:0x%x,reg:0x%x,val:0x%x\n",
        __func__, i2c_id, i2c_addr, addr, value);

    return ret;
}

/* i2c control end */

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

static struct LCM_setting_table init_setting_vdo[] = {
    {0x00, 1, {0x00} },
    {0xFF, 3, {0x87, 0x56, 0x01} },

    {0x00, 1, {0x80} },
    {0xFF, 2, {0x87, 0x56} },


    {0x00, 1, {0xA1} },
    {0xB3, 6, {0x04, 0x38, 0x08, 0xFC, 0x00, 0xFC} },

    {0x00, 1, {0x80} },
    {0xC0, 6, {0x00, 0x92, 0x00, 0x08, 0x00, 0x24} },

    {0x00, 1, {0x90} },
    {0xC0, 6, {0x00, 0x92, 0x00, 0x08, 0x00, 0x24} },

    {0x00, 1, {0xA0} },
    {0xC0, 6, {0x01, 0x24, 0x00, 0x08, 0x00, 0x24} },

    {0x00, 1, {0xB0} },
    {0xC0, 5, {0x00, 0x92, 0x00, 0x08, 0x24} },

    {0x00, 1, {0xC1} },
    {0xC0, 8, {0x00, 0xD9, 0x00, 0xA5, 0x00, 0x91, 0x00, 0xF8} },

    {0x00, 1, {0xD7} },
    {0xC0, 6, {0x00, 0x91, 0x00, 0x08, 0x00, 0x24} },

    {0x00, 1, {0xA3} },
    {0xC1, 6, {0x00, 0x25, 0x00, 0x25, 0x00, 0x02} },

    {0x00, 1, {0x80} },
    {0xCE, 16, {0x01, 0x81, 0x09, 0x13, 0x00, 0xC8, 0x00, 0xE0, 0x00, 0x85,
    0x00, 0x95, 0x00, 0x64, 0x00, 0x70} },

    {0x00, 1, {0x90} },
    {0xCE, 15, {0x00, 0x8E, 0x0C, 0xDF, 0x00, 0x8E, 0x80, 0x09, 0x13, 0x00,
    0x04, 0x00, 0x22, 0x20, 0x20} },

    {0x00, 1, {0xA0} },
    {0xCE, 3, {0x00, 0x00, 0x00} },


    {0x00, 1, {0xB0} },
    {0xCE, 3, {0x22, 0x00, 0x00} },

    {0x00, 1, {0xD1} },
    {0xCE, 7, {0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00} },

    {0x00, 1, {0xE1} },
    {0xCE, 11, {0x08, 0x02, 0x4D, 0x02, 0x4D, 0x02, 0x4D, 0x00, 0x00, 0x00, 0x00} },

    {0x00, 1, {0xF1} },
    {0xCE, 9, {0x12, 0x09, 0x0C, 0x01, 0x1B, 0x01, 0x1C, 0x01, 0x37} },

    {0x00, 1, {0xB0} },
    {0xCF, 4, {0x00, 0x00, 0xB0, 0xB4} },

    {0x00, 1, {0xB5} },
    {0xCF, 4, {0x04, 0x04, 0xB8, 0xBC} },

    {0x00, 1, {0xC0} },
    {0xCF, 4, {0x08, 0x08, 0xD2, 0xD6} },

    {0x00, 1, {0xC5} },
    {0xCF, 4, {0x00, 0x00, 0x08, 0x0C} },

    {0x00, 1, {0xE8} },
    {0xC0, 1, {0x40} },

    {0x00, 1, {0x80} },
    {0xc2, 4, {0x84, 0x01, 0x3A, 0x3A} },

    {0x00, 1, {0x90} },
    {0xC2, 4, {0x02, 0x01, 0x03, 0x03} },

    {0x00, 1, {0xA0} },
    {0xC2, 15, {0x84, 0x04, 0x00, 0x03, 0x8E, 0x83, 0x04, 0x00, 0x03, 0x8E,
    0x82, 0x04, 0x00, 0x03, 0x8E} },

    {0x00, 1, {0xB0} },
    {0xC2, 5, {0x81, 0x04, 0x00, 0x03, 0x8E} },

    {0x00, 1, {0xE0} },
    {0xC2, 14, {0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x00, 0x00, 0x12, 0x00,
    0x05, 0x02, 0x03, 0x03} },

    {0x00, 1, {0xC0} },
    {0xC3, 4, {0x99, 0x99, 0x99, 0x99} },

    {0x00, 1, {0xD0} },
    {0xC3, 8, {0x45, 0x00, 0x00, 0x05, 0x45, 0x00, 0x00, 0x05} },

    {0x00, 1, {0x80} },
    {0xCB, 16, {0xC1, 0xC1, 0x00, 0xC1, 0xC1, 0x00, 0x00, 0xC1, 0xFE, 0x00,
    0xC1, 0x00, 0xFD, 0xC1, 0x00, 0xC0} },

    {0x00, 1, {0x90} },
    {0xCB, 16, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00} },

    {0x00, 1, {0xA0} },
    {0xCB, 4, {0x00, 0x00, 0x00, 0x00} },

    {0x00, 1, {0xB0} },
    {0xCB, 4, {0x55, 0x55, 0x95, 0x55} },

    {0x00, 1, {0xC0} },
    {0xCB, 4, {0x10, 0x51, 0x84, 0x50} },

    {0x00, 1, {0x80} },
    {0xCC, 16, {0x00, 0x00, 0x00, 0x25, 0x25, 0x29, 0x16, 0x17, 0x18, 0x19,
    0x1A, 0x1B, 0x22, 0x24, 0x06, 0x06} },

    {0x00, 1, {0x90} },
    {0xCC, 8, {0x08, 0x08, 0x24, 0x02, 0x12, 0x01, 0x29, 0x29} },

    {0x00, 1, {0x80} },
    {0xCD, 16, {0x00, 0x00, 0x00, 0x25, 0x25, 0x29, 0x16, 0x17, 0x18, 0x19,
    0x1A, 0x1B, 0x22, 0x24, 0x07, 0x07} },

    {0x00, 1, {0x90} },
    {0xCD, 8, {0x09, 0x09, 0x24, 0x02, 0x12, 0x01, 0x29, 0x29} },

    {0x00, 1, {0xA0} },
    {0xCC, 16, {0x00, 0x00, 0x00, 0x25, 0x25, 0x29, 0x16, 0x17, 0x18, 0x19,
    0x1A, 0x1B, 0x24, 0x23, 0x09, 0x09} },

    {0x00, 1, {0xB0} },
    {0xCC, 8, {0x07, 0x07, 0x24, 0x12, 0x02, 0x01, 0x29, 0x29} },

    {0x00, 1, {0xA0} },
    {0xCD, 16, {0x00, 0x00, 0x00, 0x25, 0x25, 0x29, 0x16, 0x17, 0x18, 0x19,
    0x1A, 0x1B, 0x24, 0x23, 0x08, 0x08} },

    {0x00, 1, {0xB0} },
    {0xCD, 8, {0x06, 0x06, 0x24, 0x12, 0x02, 0x01, 0x29, 0x29} },

    {0x00, 1, {0x80} },
    {0xA7, 1, {0x10} },

    {0x00, 1, {0x82} },
    {0xA7, 2, {0x33, 0x02} },

    {0x00, 1, {0x85} },
    {0xA7, 1, {0x10} },
    {0x00, 1, {0xA0} },
    {0xC3, 16, {0x35, 0x02, 0x41, 0x35, 0x53, 0x14, 0x20, 0x00, 0x00, 0x00,
    0x13, 0x50, 0x24, 0x42, 0x05, 0x31} },

    {0x00, 1, {0x85} },
    {0xC4, 1, {0x1C} },
    {0x00, 1, {0x97} },
    {0xC4, 1, {0x01} },

    {0x00, 1, {0xA0} },
    {0xC4, 3, {0x2D, 0xD2, 0x2D} },

    {0x00, 1, {0x93} },
    {0xC5, 1, {0x23} },

    {0x00, 1, {0x97} },
    {0xC5, 1, {0x23} },

    {0x00, 1, {0x9A} },
    {0xC5, 1, {0x23} },

    {0x00, 1, {0x9C} },
    {0xC5, 1, {0x23} },


    {0x00, 1, {0xB6} },
    {0xC5, 2, {0x1E, 0x1E} },


    {0x00, 1, {0xB8} },
    {0xC5, 2, {0x19, 0x19} },

    {0x00, 1, {0x9B} },
    {0xF5, 1, {0x4B} },

    {0x00, 1, {0x93} },
    {0xF5, 2, {0x00, 0x00} },

    {0x00, 1, {0x9D} },
    {0xF5, 1, {0x49} },

    {0x00, 1, {0x82} },
    {0xF5, 2, {0x00, 0x00} },

    {0x00, 1, {0x8C} },
    {0xC3, 3, {0x00, 0x00, 0x00} },

    {0x00, 1, {0x84} },
    {0xC5, 2, {0x28, 0x28} },

    {0x00, 1, {0xA4} },
    {0xD7, 1, {0x00} },

    {0x00, 1, {0x80} },
    {0xF5, 2, {0x59, 0x59} },

    {0x00, 1, {0x84} },
    {0xF5, 3, {0x59, 0x59, 0x59} },

    {0x00, 1, {0x96} },
    {0xF5, 1, {0x59} },

    {0x00, 1, {0xA6} },
    {0xF5, 1, {0x59} },

    {0x00, 1, {0xCA} },
    {0xC0, 1, {0x80} },

    {0x00, 1, {0xB1} },
    {0xF5, 1, {0x1F} },


    {0x00, 1, {0x00} },
    {0xD8, 2, {0x2F, 0x2F} },


    {0x00, 1, {0x00} },
    {0xD9, 2, {0x23, 0x23} },

    {0x00, 1, {0x86} },
    {0xC0, 6, {0x01, 0x01, 0x01, 0x01, 0x10, 0x05} },
    {0x00, 1, {0x96} },
    {0xC0, 6, {0x01, 0x01, 0x01, 0x01, 0x10, 0x05} },
    {0x00, 1, {0xA6} },
    {0xC0, 6, {0x01, 0x01, 0x01, 0x01, 0x1D, 0x05} },
    {0x00, 1, {0xE9} },
    {0xC0, 6, {0x01, 0x01, 0x01, 0x01, 0x10, 0x05} },
    {0x00, 1, {0xA3} },
    {0xCE, 6, {0x01, 0x01, 0x01, 0x01, 0x10, 0x05} },
    {0x00, 1, {0xB3} },
    {0xCE, 6, {0x01, 0x01, 0x01, 0x01, 0x10, 0x05} },


    {0x00, 1, {0x00} },
    {0xE1, 40, {0x06, 0x0A, 0x0A, 0x0F, 0x6C, 0x1A, 0x21, 0x28, 0x32, 0x61,
    0x3A, 0x41, 0x47, 0x4D, 0xAC, 0x51, 0x5A, 0x62, 0x69, 0xA6, 0x70, 0x78,
    0x7F, 0x88, 0xCD, 0x92, 0x98, 0x9E, 0xA6, 0x48, 0xAE, 0xB9, 0xC6, 0xCE,
    0x97, 0xD9, 0xE7, 0xF0, 0xF5, 0xAB} },

    {0x00, 1, {0x00} },
    {0xE2, 40, {0x0D, 0x0A, 0x0A, 0x0F, 0x6C, 0x1A, 0x21, 0x28, 0x32, 0x61,
    0x3A, 0x41, 0x47, 0x4D, 0xAC, 0x51, 0x5A, 0x62, 0x69, 0xA6, 0x70, 0x78,
    0x7F, 0x88, 0xCD, 0x92, 0x98, 0x9E, 0xA6, 0x48, 0xAE, 0xB9, 0xC6, 0xCE,
    0x97, 0xD9, 0xE7, 0xF0, 0xF5, 0xAB} },

    {0x00, 1, {0x00} },
    {0xE3, 40, {0x06, 0x0A, 0x0A, 0x0F, 0x6C, 0x1A, 0x21, 0x28, 0x32, 0x61,
    0x3A, 0x41, 0x47, 0x4D, 0xAC, 0x51, 0x5A, 0x62, 0x69, 0xA6, 0x70, 0x78,
    0x7F, 0x88, 0xCD, 0x92, 0x98, 0x9E, 0xA6, 0x48, 0xAE, 0xB9, 0xC6, 0xCE,
    0x97, 0xD9, 0xE7, 0xF0, 0xF5, 0xAB} },

    {0x00, 1, {0x00} },
    {0xE4, 40, {0x0D, 0x0A, 0x0A, 0x0F, 0x6C, 0x1A, 0x21, 0x28, 0x32, 0x61,
    0x3A, 0x41, 0x47, 0x4D, 0xAC, 0x51, 0x5A, 0x62, 0x69, 0xA6, 0x70, 0x78,
    0x7F, 0x88, 0xCD, 0x92, 0x98, 0x9E, 0xA6, 0x48, 0xAE, 0xB9, 0xC6, 0xCE,
    0x97, 0xD9, 0xE7, 0xF0, 0xF5, 0xAB} },

    {0x00, 1, {0x00} },
    {0xE5, 40, {0x06, 0x0A, 0x0A, 0x0F, 0x6C, 0x1A, 0x21, 0x28, 0x32, 0x61,
    0x3A, 0x41, 0x47, 0x4D, 0xAC, 0x51, 0x5A, 0x62, 0x69, 0xA6, 0x70, 0x78,
    0x7F, 0x88, 0xCD, 0x92, 0x98, 0x9E, 0xA6, 0x48, 0xAE, 0xB9, 0xC6, 0xCE,
    0x97, 0xD9, 0xE7, 0xF0, 0xF5, 0xAB} },

    {0x00, 1, {0x00} },
    {0xE6, 40, {0x0D, 0x0A, 0x0A, 0x0F, 0x6C, 0x1A, 0x21, 0x28, 0x32, 0x61,
    0x3A, 0x41, 0x47, 0x4D, 0xAC, 0x51, 0x5A, 0x62, 0x69, 0xA6, 0x70, 0x78,
    0x7F, 0x88, 0xCD, 0x92, 0x98, 0x9E, 0xA6, 0x48, 0xAE, 0xB9, 0xC6, 0xCE,
    0x97, 0xD9, 0xE7, 0xF0, 0xF5, 0xAB} },

    {0x00, 1, {0xCC} },
    {0xC0, 1, {0x10} },


    {0x00, 1, {0xB3} },
    {0xC5, 1, {0xD1} },


    {0x00, 1, {0x80} },
    {0xCA, 12, {0xCE, 0xBB, 0xAB, 0x9F, 0x96, 0x8E, 0x87, 0x82, 0x80, 0x80, 0x80, 0x80} },
    {0x00, 1, {0x90} },
    {0xCA, 9, {0xFD, 0xFF, 0xEA, 0xFC, 0xFF, 0xCC, 0xFA, 0xFF, 0x66} },


    {0x00, 1, {0x00} },
    {0xFF, 3, {0xFF, 0xFF, 0xFF} },


    {0x51, 1, {0xff, 0x0f} },
    {0x53, 1, {0x24} },
    {0x55, 1, {0x01} },

    {0x35, 0x01, {0x00} },

    {0x11, 0, {} },
#ifndef LCM_SET_DISPLAY_ON_DELAY
    {REGFLAG_DELAY, 180, {} },

    {0x29, 0, {} },
    {REGFLAG_DELAY, 100, {} },
#endif
    {REGFLAG_END_OF_TABLE, 0x00, {} }
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

    params->dsi.mode = SYNC_EVENT_VDO_MODE;
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

    params->dsi.vertical_sync_active = 16;
    params->dsi.vertical_backporch = 16;
    params->dsi.vertical_frontporch = 70;
    params->dsi.vertical_frontporch_for_low_power = 840;
    params->dsi.vertical_active_line = FRAME_HEIGHT;

    params->dsi.horizontal_sync_active = 16;
    params->dsi.horizontal_backporch = 14;
    params->dsi.horizontal_frontporch = 30;
    params->dsi.horizontal_active_pixel = FRAME_WIDTH;
    params->dsi.ssc_disable = 1; /* ssc will decrease fps */
#ifndef MACH_FPGA
    /* this value must be in MTK suggested table */
    params->dsi.PLL_CLOCK = 530;
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
}

/* turn on gate ic & control voltage to 5.8V */
static void lcm_init_power(void *fdt)
{
    if (lcm_util.set_gpio_lcd_enp_bias) {
        lcm_util.set_gpio_lcd_enp_bias(1, fdt);
        _lcm_i2c_write_bytes(0x0, 0x12, fdt);
        _lcm_i2c_write_bytes(0x1, 0x12, fdt);
    } else
        LCM_LOGI("set_gpio_lcd_enp_bias not defined\n");
}

static void lcm_suspend_power(void)
{
#if 0
    SET_RESET_PIN(0);
    if (lcm_util.set_gpio_lcd_enp_bias)
        lcm_util.set_gpio_lcd_enp_bias(0);
    else
        LCM_LOGI("set_gpio_lcd_enp_bias not defined\n");
#endif
}

static void lcm_resume_power(void)
{
#if 0
    SET_RESET_PIN(0);
    lcm_init_power();
#endif
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
    MDELAY(5);

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

    /* we need to wait 200ms after lcm init to set display on */
    timeout = 120;

    while ((current_time() - lcm_init_time) <= timeout) {
        i++;
        MDELAY(1);
        if (i % 1000 == 0) {
            LCM_LOGI("ft8756 %s error: i=%u,lcm_init_time=%u,cur_time=%u\n", __func__, i,
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
#if 0
    push_table(lcm_suspend_setting, sizeof(lcm_suspend_setting) /
           sizeof(struct LCM_setting_table), 1);

    SET_RESET_PIN(0, fdt);
    MDELAY(10);
#endif
}

static void lcm_resume(void)
{
    //lcm_init();
}

static void lcm_update(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
#ifdef LCM_SET_DISPLAY_ON_DELAY
    lcm_set_display_on();
#endif
}

static unsigned int lcm_ata_check(unsigned char *buffer)
{
#if 0
    unsigned int ret = 0;
    unsigned int x0 = FRAME_WIDTH / 4;
    unsigned int x1 = FRAME_WIDTH * 3 / 4;

    unsigned char x0_MSB = ((x0 >> 8) & 0xFF);
    unsigned char x0_LSB = (x0 & 0xFF);
    unsigned char x1_MSB = ((x1 >> 8) & 0xFF);
    unsigned char x1_LSB = (x1 & 0xFF);

    unsigned int data_array[3];
    unsigned char read_buf[4];

    LCM_LOGI("ATA check size = 0x%x, 0x%x, 0x%x, 0x%x\n",
         x0_MSB, x0_LSB, x1_MSB, x1_LSB);
    data_array[0] = 0x0005390A; /* HS packet */
    data_array[1] = (x1_MSB << 24) | (x0_LSB << 16) | (x0_MSB << 8) | 0x2a;
    data_array[2] = (x1_LSB);
    dsi_set_cmdq(data_array, 3, 1);

    data_array[0] = 0x00043700; /* read id return two byte,version and id */
    dsi_set_cmdq(data_array, 1, 1);

    read_reg_v2(0x2A, read_buf, 4);

    if ((read_buf[0] == x0_MSB) && (read_buf[1] == x0_LSB) &&
        (read_buf[2] == x1_MSB) && (read_buf[3] == x1_LSB))
        ret = 1;
    else
        ret = 0;

    x0 = 0;
    x1 = FRAME_WIDTH - 1;

    x0_MSB = ((x0 >> 8) & 0xFF);
    x0_LSB = (x0 & 0xFF);
    x1_MSB = ((x1 >> 8) & 0xFF);
    x1_LSB = (x1 & 0xFF);

    data_array[0] = 0x0005390A; /* HS packet */
    data_array[1] = (x1_MSB << 24) | (x0_LSB << 16) | (x0_MSB << 8) | 0x2a;
    data_array[2] = (x1_LSB);
    dsi_set_cmdq(data_array, 3, 1);
    return ret;
#else
    return 0;
#endif
}

static void lcm_setbacklight_cmdq(void *handle, unsigned int level)
{
    LCM_LOGI("%s,ft8756 backlight: level = %d\n", __func__, level);

    bl_level[0].para_list[0] = level;

    push_table(bl_level, sizeof(bl_level) /
           sizeof(struct LCM_setting_table), 1);
}

static void lcm_setbacklight(unsigned int level)
{
    LCM_LOGI("%s,ft8756 backlight: level = %d\n", __func__, level);

    bl_level[0].para_list[0] = level;

    push_table(bl_level, sizeof(bl_level) /
           sizeof(struct LCM_setting_table), 1);
}

#define LCM_ID_FT8756 (0x40)

static unsigned int lcm_compare_id(void *fdt)
{
    unsigned int id0 = 0, id1 = 0, id2 = 0;
    unsigned char buffer[4];
    unsigned int array[16];

    SET_RESET_PIN(1, fdt);
    SET_RESET_PIN(0, fdt);
    MDELAY(1);

    SET_RESET_PIN(1, fdt);
    MDELAY(20);

    array[0] = 0x00013700;
    dsi_set_cmdq(array, 1, 1);

    read_reg_v2(0xDA, buffer, 3);
    id0 = buffer[0];
    id1 = buffer[1];
    id2 = buffer[2];

    LCM_LOGI("%s, ft8756 debug: id:0x%x-%x-%x, expected:0x%08x\n",
        __func__, id0, id1, id2, LCM_ID_FT8756);

    if (id0 == LCM_ID_FT8756 && id1 == LCM_ID_FT8756 && id2 == LCM_ID_FT8756)
        return 1;
    else
        return 0;

}

LCM_DRIVER ft8756_fhdp_dsi_vdo_auo_rt4801_lcm_drv = {
    .name = "ft8756_fhdp_dsi_vdo_auo_rt4801_drv",
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
    .ata_check = lcm_ata_check,
    .update = lcm_update,
};
