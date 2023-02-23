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

#include "lcm_drv.h"

#ifdef MTK_ROUND_CORNER_SUPPORT
#include "data_hw_roundedpattern.h"
#endif

#include "rt4831a_drv.h"

#define LCM_LOGI(string, args...)  dprintf(CRITICAL, "[LK/"LOG_TAG"]"string, ##args)
#define LCM_LOGD(string, args...)  dprintf(INFO, "[LK/"LOG_TAG"]"string, ##args)

static LCM_UTIL_FUNCS lcm_util;

#define SET_RESET_PIN(v, fdt)    (lcm_util.set_reset_pin(v, fdt))
#define MDELAY(n)        (lcm_util.mdelay(n))
#define UDELAY(n)        (lcm_util.udelay(n))

/* ------------------------------------------------------------------------ */
/* Local Functions */
/* ------------------------------------------------------------------------ */

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update) \
        lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update) \
        lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd) lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums) \
        lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd) \
        lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size) \
        lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)

/* ------------------------------------------------------------------------ */
/* Local Constants */
/* ------------------------------------------------------------------------ */
#define LCM_DSI_CMD_MODE        1
#define FRAME_WIDTH            (1080)
#define FRAME_HEIGHT            (2340)

#define REGFLAG_DELAY            0xFFFC
#define REGFLAG_UDELAY            0xFFFB
#define REGFLAG_END_OF_TABLE        0xFFFD
#define REGFLAG_RESET_LOW        0xFFFE
#define REGFLAG_RESET_HIGH        0xFFFF

struct LCM_setting_table {
    unsigned int cmd;
    unsigned char count;
    unsigned char para_list[130];
};

#if 0
static struct LCM_setting_table lcm_suspend_setting[] = {
    {0x28, 0, {} },
    {REGFLAG_DELAY, 20, {} },
    {0x10, 0, {} },
    {REGFLAG_DELAY, 120, {} }
};
#endif

static struct LCM_setting_table init_setting[] = {

    {REGFLAG_DELAY, 200, {} },

    /* set PLL to 190M */
    {0xB0, 1, {0x00} },
    {0xB6, 12, {0x51, 0x00, 0x06, 0x23, 0x8A, 0x13, 0x1A, 0x05,
              0x04, 0xFA, 0x05, 0x20} },

    {0x51, 2, {0x0F, 0xff} },
    {0x53, 1, {0x04} },
    {0x35, 1, {0x00} },
    {0x44, 0x02, {0x08, 0x66} }, /* set TE event @ line 0x866(2150) */
    {0x2a, 4, {0x00, 0x00, 0x04, 0x37} },
    {0x2b, 4, {0x00, 0x00, 0x09, 0x23} },

    {REGFLAG_DELAY, 200, {} },

    {0xB0, 1, {0x80} },
    {0xD4, 1, {0x93} },
    {0x50, 41, {0x42, 0x58, 0x81, 0x2D, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x6B, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0xFF, 0xD4,
            0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00,
            0x53, 0x18, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00} },

    /* set PPS to table and choose table 0 */
    {0xF7, 1, {0x01} }, /* key */
    {0xF8, 128, {0x11, 0x00, 0x00, 0x89, 0x30, 0x80, 0x09, 0x24, 0x04, 0x38,
        0x00, 0x14, 0x02, 0x1c, 0x02, 0x1c, 0x02, 0x00, 0x02, 0x0e,
        0x00, 0x20, 0x01, 0xe8, 0x00, 0x07, 0x00, 0x0c, 0x05, 0x0e,
        0x05, 0x16, 0x18, 0x00, 0x10, 0xf0, 0x03, 0x0c, 0x20, 0x00,
        0x06, 0x0b, 0x0b, 0x33, 0x0e, 0x1c, 0x2a, 0x38, 0x46, 0x54,
        0x62, 0x69, 0x70, 0x77, 0x79, 0x7b, 0x7d, 0x7e, 0x01, 0x02,
        0x01, 0x00, 0x09, 0x40, 0x09, 0xbe, 0x19, 0xfc, 0x19, 0xfa,
        0x19, 0xf8, 0x1a, 0x38, 0x1a, 0x78, 0x1a, 0xb6, 0x2a, 0xf6,
        0x2b, 0x34, 0x2b, 0x74, 0x3b, 0x74, 0x6b, 0xf4, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} },

    /* Turn on DSC */
    {0xB0, 1, {0x00} },
    {0xEB, 2, {0x8B, 0x8B} },

    /* Flash QE setting */
    {0xDF, 2, {0x50, 0x40} },
    {0xF3, 5, {0x50, 0x00, 0x00, 0x00, 0x00} },
    {0xF2, 1, {0x11} },

    {REGFLAG_DELAY, 10, {} },

    {0xF3, 5, {0x01, 0x00, 0x00, 0x00, 0x01} },
    {0xF4, 2, {0x00, 0x02} },
    {0xF2, 1, {0x19} },

    {REGFLAG_DELAY, 20, {} },

    {0xDF, 2, {0x50, 0x42} },
    {0xB0, 1, {0x84} },
    {0xE6, 1, {0x01} },

    {0x11, 0, {} },

    {REGFLAG_DELAY, 120, {} },
    {0x29, 0, {} },
};

static struct LCM_setting_table bl_level[] = {
    {0x51, 2, {0xF, 0xFF} },
    {REGFLAG_END_OF_TABLE, 0x00, {} }
};

static void push_table(struct LCM_setting_table *table, unsigned int count,
               unsigned char force_update)
{
    unsigned int i;

    for (i = 0; i < count; i++) {
        unsigned int cmd;

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
            dsi_set_cmdq_V2(cmd, table[i].count,
                    table[i].para_list, force_update);
            break;
        }
    }
}

/* ----------------------------------------------------------------------- */
/* LCM Driver Implementations */
/* ----------------------------------------------------------------------- */

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

#if (LCM_DSI_CMD_MODE)
    params->dsi.mode = CMD_MODE;
    params->dsi.switch_mode = SYNC_PULSE_VDO_MODE;
#else
    params->dsi.mode = SYNC_PULSE_VDO_MODE;
    params->dsi.switch_mode = CMD_MODE;
#endif
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
#if (LCM_DSI_CMD_MODE)
    params->dsi.PLL_CLOCK = 215; /* ori clk = 570M, 1/3 compression */
#else
    params->dsi.PLL_CLOCK = 215;
#endif
#else
    params->dsi.pll_div1 = 0;
    params->dsi.pll_div2 = 0;
    params->dsi.fbk_div = 0x1;
#endif
    params->dsi.CLK_HS_POST = 36;
    params->dsi.clk_lp_per_line_enable = 0;
    params->dsi.esd_check_enable = 0;
    params->dsi.customization_esd_check_enable = 0;
    params->dsi.lcm_esd_check_table[0].cmd = 0x53;
    params->dsi.lcm_esd_check_table[0].count = 1;
    params->dsi.lcm_esd_check_table[0].para_list[0] = 0x24;

#ifdef MTK_ROUND_CORNER_SUPPORT
    params->round_corner_params.round_corner_en = 1;
    params->round_corner_params.full_content = 0;
    params->round_corner_params.h = ROUND_CORNER_H_TOP;
    params->round_corner_params.h_bot = ROUND_CORNER_H_BOT;
    params->round_corner_params.tp_size = sizeof(top_rc_pattern);
    params->round_corner_params.lt_addr = (void *)top_rc_pattern;
#endif
    params->dsi.dsc_enable = 1;
    params->dsi.dsc_params.ver = 17;
    params->dsi.dsc_params.slice_mode = 1;
    params->dsi.dsc_params.rgb_swap = 0;
    params->dsi.dsc_params.dsc_cfg = 34;
    params->dsi.dsc_params.rct_on = 1;
    params->dsi.dsc_params.bit_per_channel = 8;
    params->dsi.dsc_params.dsc_line_buf_depth = 9;
    params->dsi.dsc_params.bp_enable = 1;
    params->dsi.dsc_params.bit_per_pixel = 128;
    params->dsi.dsc_params.pic_height = 2340;
    params->dsi.dsc_params.pic_width = 1080;
    params->dsi.dsc_params.slice_height = 20;
    params->dsi.dsc_params.slice_width = 540;
    params->dsi.dsc_params.chunk_size = 540;
    params->dsi.dsc_params.xmit_delay = 512;
    params->dsi.dsc_params.dec_delay = 526;
    params->dsi.dsc_params.scale_value = 32;
    params->dsi.dsc_params.increment_interval = 488;
    params->dsi.dsc_params.decrement_interval = 7;
    params->dsi.dsc_params.line_bpg_offset = 12;
    params->dsi.dsc_params.nfl_bpg_offset = 1294;
    params->dsi.dsc_params.slice_bpg_offset = 1302;
    params->dsi.dsc_params.initial_offset = 6144;
    params->dsi.dsc_params.final_offset = 4336;
    params->dsi.dsc_params.flatness_minqp = 3;
    params->dsi.dsc_params.flatness_maxqp = 12;
    params->dsi.dsc_params.rc_model_size = 8192;
    params->dsi.dsc_params.rc_edge_factor = 6;
    params->dsi.dsc_params.rc_quant_incr_limit0 = 11;
    params->dsi.dsc_params.rc_quant_incr_limit1 = 11;
    params->dsi.dsc_params.rc_tgt_offset_hi = 3;
    params->dsi.dsc_params.rc_tgt_offset_lo = 3;
}

static void lcm_init_power(void *fdt)
{
#ifdef MTK_RT4831A_SUPPORT
    if (gate_ic_power_status_setting(fdt, 1)) {
        LCM_LOGI("set_gpio_lcm_led_en not defined\n");
        return;
    }
    _gate_ic_i2c_panel_bias_enable(fdt, 1);
#endif
}

static void lcm_suspend_power(void)
{
}

static void lcm_resume_power(void)
{
}

static void lcm_setbacklight(unsigned int level)
{
    unsigned int BacklightLevel;

    LCM_LOGI("%s,lk samsung backlight: level = %d\n", __func__, level);
    if (level > 255)
        level = 255;

    if (level > 0)
        BacklightLevel = level * 4095 / 255;
    else
        BacklightLevel = 0;

    bl_level[0].para_list[0] = ((BacklightLevel >> 8) & 0xF);
    bl_level[0].para_list[1] = (BacklightLevel & 0xFF);

    push_table(bl_level, countof(bl_level), 1);
}

static void lcm_init(void *fdt)
{
    int ret = 0;

    SET_RESET_PIN(0, fdt);
    MDELAY(10);

    SET_RESET_PIN(1, fdt);
    MDELAY(10);

    push_table(init_setting, sizeof(init_setting) /
           sizeof(init_setting[0]), 1);

    /* lcm_setbacklight(255); */
}

static void lcm_suspend(void)
{
#if 0
    int ret;

    push_table(lcm_suspend_setting, sizeof(lcm_suspend_setting) /
           sizeof(lcm_suspend_setting[0]), 1);

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

static void lcm_update(unsigned int x, unsigned int y, unsigned int width,
               unsigned int height)
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

    unsigned int data_array[3];

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
    unsigned int id = 0, version_id = 0;
    unsigned char buffer[1];
    unsigned int array[1];

    SET_RESET_PIN(1, fdt);
    SET_RESET_PIN(0, fdt);
    MDELAY(10);

    SET_RESET_PIN(1, fdt);
    MDELAY(20);

    array[0] = 0x00023700; /* read id return two byte,version and id */
    dsi_set_cmdq(array, 1, 1);

    read_reg_v2(0xda, buffer, 1);
    id = buffer[0]; /* we only need ID */

    LCM_LOGI("%s,samsung=0x%08x\n", __func__, id);

    if (id == 0x01) /* TODO */
        return 1;
    else
        return 0;
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

    LCM_LOGI("ATA check size = 0x%x,0x%x,0x%x,0x%x\n",
         x0_MSB, x0_LSB, x1_MSB, x1_LSB);
    data_array[0] = 0x0005390A; /* HS packet */
    data_array[1] = (x1_MSB << 24) | (x0_LSB << 16) | (x0_MSB << 8) | 0x2a;
    data_array[2] = (x1_LSB);
    dsi_set_cmdq(data_array, 3, 1);

    data_array[0] = 0x00043700; /* return two byte, version and id */
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

LCM_DRIVER r66451_fhdp_dsi_cmd_tianma_lcm_drv = {
    .name = "r66451_fhdp_dsi_cmd_tianma_drv",
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
