/*
 * Copyright (c) 2021 MediaTek Inc.
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
#include "rt4831a_drv.h"

#define LCM_LOGI(string, args...)  dprintf(CRITICAL, "[LK/"LOG_TAG"]"string, ##args)
#define LCM_LOGD(string, args...)  dprintf(INFO, "[LK/"LOG_TAG"]"string, ##args)

#define LCM_ID_S6E3HAE_SAMSUNG (0x01)

static LCM_UTIL_FUNCS lcm_util;

#define SET_RESET_PIN(v, fdt)    (lcm_util.set_reset_pin(v, fdt))
#define MDELAY(n)       (lcm_util.mdelay(n))
#define UDELAY(n)       (lcm_util.udelay(n))


/* --------------------------------------------------------------------------- */
/* Local Functions */
/* --------------------------------------------------------------------------- */

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

/* --------------------------------------------------------------------------- */
/* Local Constants */
/* --------------------------------------------------------------------------- */
#define LCM_DSI_CMD_MODE 1
#define PANEL_2K 1

#if (PANEL_2K)
#define FRAME_WIDTH                                     (1440)
#define FRAME_HEIGHT                                    (3200)
#else
#define FRAME_WIDTH                                     (1080)
#define FRAME_HEIGHT                                    (2400)
#endif
#define TRUE 1
#define FALSE 0
#define PHYSICAL_WIDTH (71)
#define PHYSICAL_HEIGHT (153)

#ifndef MACH_FPGA
#define GPIO_65132_EN GPIO_LCD_BIAS_ENP_PIN
#endif

#define REGFLAG_DELAY       0xFFFC
#define REGFLAG_UDELAY  0xFFFB
#define REGFLAG_END_OF_TABLE    0xFFFD
#define REGFLAG_RESET_LOW   0xFFFE
#define REGFLAG_RESET_HIGH  0xFFFF

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

struct LCM_setting_table {
    unsigned int cmd;
    unsigned char count;
    unsigned char para_list[128];
};


static struct LCM_setting_table lcm_suspend_setting[] = {
    {0x28, 0, {} },
    {REGFLAG_DELAY, 10, {} },
    {0x10, 0, {} },
    {REGFLAG_DELAY, 120, {} },
    {REGFLAG_END_OF_TABLE, 0x00, {} }
};

#if (PANEL_2K)
static struct LCM_setting_table init_setting[] = {
    /* DSC setting */
    {0x9D, 01, {0x01} },
    {0x9E, 120, {0x11, 0x00, 0x00, 0x89, 0x30, 0x80, 0x0C, 0x80,
            0x05, 0xA0, 0x00, 0x19, 0x02, 0xD0, 0x02, 0xD0,
            0x02, 0x00, 0x02, 0x68, 0x00, 0x20, 0x02, 0xBE,
            0x00, 0x0A, 0x00, 0x0C, 0x04, 0x00, 0x03, 0x0D,
            0x18, 0x00, 0x10, 0xF0, 0x03, 0x0C, 0x20, 0x00,
            0x06, 0x0B, 0x0B, 0x33, 0x0E, 0x1C, 0x2A, 0x38,
            0x46, 0x54, 0x62, 0x69, 0x70, 0x77, 0x79, 0x7B,
            0x7D, 0x7E, 0x01, 0x02, 0x01, 0x00, 0x09, 0x40,
            0x09, 0xBE, 0x19, 0xFC, 0x19, 0xFA, 0x19, 0xF8,
            0x1A, 0x38, 0x1A, 0x78, 0x1A, 0xB6, 0x2A, 0xF6,
            0x2B, 0x34, 0x2B, 0x74, 0x3B, 0x74, 0x6B, 0xF4,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} },
    {0x11, 01, {0x00} },
    {REGFLAG_DELAY, 120, {} },
    /* common setting */
    {0x35, 01, {0x00} },
    /* page address set */
    {0x2A, 04, {0x00, 0x00, 0x05, 0x9F} },
    {0x2B, 04, {0x00, 0x00, 0x0c, 0x7F} },
    /*frequency select 120hz */
    {0xF0, 02, {0x5A, 0x5A} },
    {0x60, 01, {0x00} },
    {0xF7, 01, {0x0F} },
    {0xF0, 02, {0xA5, 0xA5} },

    /* dimming setting */
    {0xF0, 02, {0x5A, 0x5A} },
    /* dimming speed setting */
    {0xB0, 01, {0x0E} },
    {0x94, 01, {0x28} },
    /* elvss dim setting */
    {0xB0, 01, {0x0D} },
    {0x94, 01, {0x60} },
    {0x53, 01, {0x28} },
    //{0x51, 02, {0x03, 0xFF} },
    {0xF7, 01, {0x0F} },
    {0xF0, 02, {0xA5, 0xA5} },

    {REGFLAG_DELAY, 120, {} },
    {0x29, 01, {0x00} },
    {REGFLAG_END_OF_TABLE, 0x00, {} }
};
#else
static struct LCM_setting_table init_setting[] = {
    /* FHDP DSC setting */
    {0x9D, 01, {0x01} },
    {0x9E, 120, {0x11, 0x00, 0x00, 0x89, 0x30, 0x80, 0x09, 0x60,
            0x04, 0x38, 0x00, 0x20, 0x02, 0x1C, 0x02, 0x1C,
            0x02, 0x00, 0x02, 0x0E, 0x00, 0x20, 0x03, 0x15,
            0x00, 0x07, 0x00, 0x0C, 0x03, 0x19, 0x03, 0x2E,
            0x18, 0x00, 0x10, 0xF0, 0x03, 0x0C, 0x20, 0x00,
            0x06, 0x0B, 0x0B, 0x33, 0x0E, 0x1C, 0x2A, 0x38,
            0x46, 0x54, 0x62, 0x69, 0x70, 0x77, 0x79, 0x7B,
            0x7D, 0x7E, 0x01, 0x02, 0x01, 0x00, 0x09, 0x40,
            0x09, 0xBE, 0x19, 0xFC, 0x19, 0xFA, 0x19, 0xF8,
            0x1A, 0x38, 0x1A, 0x78, 0x1A, 0xB6, 0x2A, 0xF6,
            0x2B, 0x34, 0x2B, 0x74, 0x3B, 0x74, 0x6B, 0xF4,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} },
    {0x11, 01, {0x00} },
    {REGFLAG_DELAY, 120, {} },
    /* common setting */
    {0x35, 01, {0x00} },
    /* page address set */
    {0x2A, 04, {0x00, 0x00, 0x05, 0x9F} },
    {0x2B, 04, {0x00, 0x00, 0x0c, 0x7F} },
    /* frequency select 60hz */
    {0xF0, 02, {0x5A, 0x5A} },
    {0x60, 01, {0x08} },
    {0xF7, 01, {0x0F} },
    {0xF0, 02, {0xA5, 0xA5} },
    /* scaler setting FHDP */
    {0xF0, 02, {0x5A, 0x5A} },
    {0xC3, 01, {0x4D} },
    {0xF0, 02, {0xA5, 0xA5} },

    /* dimming setting */
    {0xF0, 02, {0x5A, 0x5A} },
    /* dimming speed setting */
    {0xB0, 01, {0x0E} },
    {0x94, 01, {0x28} },
    /* elvss dim setting */
    {0xB0, 01, {0x0D} },
    {0x94, 01, {0x60} },
    {0x53, 01, {0x28} },
    //{0x51, 02, {0x03, 0xFF} },
    {0xF7, 01, {0x0F} },
    {0xF0, 02, {0xA5, 0xA5} },

    {0x29, 01, {0x00} },
    {REGFLAG_END_OF_TABLE, 0x00, {} }
};
#endif

static struct LCM_setting_table bl_level[] = {
    {0x51, 2, {0x03, 0xFF} },
    {REGFLAG_END_OF_TABLE, 0x00, {} }
};


static void push_table(struct LCM_setting_table *table,
                unsigned int count, unsigned char force_update)
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
            dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
            break;
        }
    }
}

/* --------------------------------------------------------------------------- */
/* LCM Driver Implementations */
/* --------------------------------------------------------------------------- */

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
    params->physical_width = PHYSICAL_WIDTH;
    params->physical_height = PHYSICAL_HEIGHT;

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
#if (PANEL_2K)
    params->dsi.vertical_sync_active = 10;
    params->dsi.vertical_backporch = 10;
    params->dsi.vertical_frontporch = 54;
    params->dsi.vertical_frontporch_for_low_power = 620;
    params->dsi.vertical_active_line = FRAME_HEIGHT;

    params->dsi.horizontal_sync_active = 2;
    params->dsi.horizontal_backporch = 16;
    params->dsi.horizontal_frontporch = 20;
    params->dsi.horizontal_active_pixel = FRAME_WIDTH;
#else
    params->dsi.vertical_sync_active = 2;
    params->dsi.vertical_backporch = 8;
    params->dsi.vertical_frontporch = 30;
    params->dsi.vertical_active_line = FRAME_HEIGHT;

    params->dsi.horizontal_sync_active = 10;
    params->dsi.horizontal_backporch = 20;
    params->dsi.horizontal_frontporch = 20;
    params->dsi.horizontal_active_pixel = FRAME_WIDTH;
#endif
    params->dsi.ssc_disable = 1;
#ifndef MACH_FPGA
#if (LCM_DSI_CMD_MODE)
#if (PANEL_2K)
    params->dsi.PLL_CLOCK = 680;    /* this value must be in MTK suggested table */
    params->dsi.data_rate  = 1360;  /* this value must be in MTK suggested table */
#else
    params->dsi.PLL_CLOCK = 680;    /* this value must be in MTK suggested table */
    params->dsi.data_rate  = 1360;  /* this value must be in MTK suggested table */
#endif
#else
    params->dsi.PLL_CLOCK = 680;    /* this value must be in MTK suggested table */
    params->dsi.data_rate  = 1360;  /* this value must be in MTK suggested table */
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
    params->round_corner_params.round_corner_en = 0;
    params->round_corner_params.full_content = 0;
    params->round_corner_params.h = ROUND_CORNER_H_TOP;
    params->round_corner_params.h_bot = ROUND_CORNER_H_BOT;
    params->round_corner_params.tp_size = sizeof(top_rc_pattern);
    params->round_corner_params.lt_addr = (void *)top_rc_pattern;
#endif
#if (PANEL_2K)
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
    params->dsi.dsc_params.pic_height = 3200;
    params->dsi.dsc_params.pic_width = 1440;
    params->dsi.dsc_params.slice_height = 25;
    params->dsi.dsc_params.slice_width = 720;
    params->dsi.dsc_params.chunk_size = 720;
    params->dsi.dsc_params.xmit_delay = 512;
    params->dsi.dsc_params.dec_delay = 616;
    params->dsi.dsc_params.scale_value = 32;
    params->dsi.dsc_params.increment_interval = 702;
    params->dsi.dsc_params.decrement_interval = 10;
    params->dsi.dsc_params.line_bpg_offset = 12;
    params->dsi.dsc_params.nfl_bpg_offset = 1024;
    params->dsi.dsc_params.slice_bpg_offset = 781;
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
#else
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
    params->dsi.dsc_params.pic_height = 2400;
    params->dsi.dsc_params.pic_width = 1080;
    params->dsi.dsc_params.slice_height = 32;
    params->dsi.dsc_params.slice_width = 540;
    params->dsi.dsc_params.chunk_size = 540;
    params->dsi.dsc_params.xmit_delay = 512;
    params->dsi.dsc_params.dec_delay = 526;
    params->dsi.dsc_params.scale_value = 32;
    params->dsi.dsc_params.increment_interval = 789;
    params->dsi.dsc_params.decrement_interval = 7;
    params->dsi.dsc_params.line_bpg_offset = 12;
    params->dsi.dsc_params.nfl_bpg_offset = 793;
    params->dsi.dsc_params.slice_bpg_offset = 814;
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
#endif
}

#ifndef MTK_RT4831A_SUPPORT
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
#endif

static void lcm_init_power(void *fdt)
{
    SET_RESET_PIN(0, fdt);
#ifdef MTK_RT4831A_SUPPORT
    if (gate_ic_power_status_setting(fdt, 1)) {
        LCM_LOGI("set_gpio_lcm_led_en not defined\n");
        return;
    }
    _gate_ic_i2c_panel_bias_enable(fdt, 1);
#else
    if (lcm_util.set_gpio_lcd_enp_bias) {
        lcm_util.set_gpio_lcd_enp_bias(1, fdt);
        _lcm_i2c_write_bytes(0x00, 0x0f, fdt);
        _lcm_i2c_write_bytes(0x01, 0x0f, fdt);
        LCM_LOGI("%s\n", __func__);

    } else {
        LCM_LOGI("set_gpio_lcd_enp_bias not defined\n");
    }
#endif
}

static void lcm_suspend_power(void)
{

}

static void lcm_resume_power(void)
{

}


static void lcm_init(void *fdt)
{
    int ret = 0;

    SET_RESET_PIN(0, fdt);


    MDELAY(15);
    SET_RESET_PIN(1, fdt);
    MDELAY(10);
    SET_RESET_PIN(0, fdt);
    MDELAY(10);

    SET_RESET_PIN(1, fdt);
    MDELAY(10);

    push_table(init_setting, sizeof(init_setting) / sizeof(struct LCM_setting_table), 1);

}


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

#if 1

static void lcm_update(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
#if 0
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
#endif
}
#endif


static unsigned int lcm_compare_id(void *fdt)
{
    unsigned int id = 0;
    unsigned char buffer[2];
    unsigned int array[16];

    LCM_LOGI("%s: enter\n", __func__);

    SET_RESET_PIN(1, fdt);
    MDELAY(10);
    SET_RESET_PIN(0, fdt);
    MDELAY(10);
    SET_RESET_PIN(1, fdt);
    MDELAY(10);

    array[0] = 0x00013700;    /* read id return two byte,version and id */
    dsi_set_cmdq(array, 1, 1);

    read_reg_v2(0xDA, buffer, 1);
    id = buffer[0];     /* we only need ID */

    LCM_LOGI("%s, S6E3HAE_SAMSUNG_LCM=0x%08x\n", __func__, id);

    if (id == LCM_ID_S6E3HAE_SAMSUNG)
        return 1;
    else
        return 0;

}

static void lcm_setbacklight(unsigned int level)
{
    LCM_LOGI("%s, s6e3hae Samsung backlight: level = %d\n", __func__, level);

    bl_level[0].para_list[0] = (level >> 8) & 0x7;
    bl_level[0].para_list[1] = (level & 0xFF);

    push_table(bl_level, sizeof(bl_level) / sizeof(struct LCM_setting_table), 1);
}

LCM_DRIVER s6e3hae_wqhd_cmd_samsung_120hz_lcm_drv = {
    .name = "s6e3hae_wqhd_cmd_samsung_120hz_lcm_drv",
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
    .update = lcm_update,
};
