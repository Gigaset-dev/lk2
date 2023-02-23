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


/* Local Constants */
/* --------------------------------------------------------------------------- */
#define LCM_DSI_CMD_MODE (1)
#define FRAME_WIDTH (1440)
#define FRAME_HEIGHT (3216)
#define PHYSICAL_WIDTH (67)
#define PHYSICAL_HEIGHT (149)

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
    unsigned char para_list[128];
};

#if 0
static struct LCM_setting_table lcm_suspend_setting[] = {
    {0x28, 0, {} },
    {0x10, 0, {} },
    {REGFLAG_DELAY, 120, {} },
    {0x4F, 1, {0x01} },
    {REGFLAG_DELAY, 120, {} }
};
#endif

static struct LCM_setting_table init_setting[] = {
    //DSC 10bit
    {0x9E, 128, {0x11, 0x00, 0x00, 0xAB, 0x30, 0x80, 0x0C, 0x90, 0x05,
    0xA0, 0x00, 0x18, 0x02, 0xD0, 0x02, 0xD0, 0x02, 0x00,
    0x02, 0x86, 0x00, 0x20, 0x02, 0x83, 0x00, 0x0A, 0x00,
    0x0D, 0x04, 0x86, 0x03, 0x2E, 0x18, 0x00, 0x10, 0xF0,
    0x07, 0x10, 0x20, 0x00, 0x06, 0x0F, 0x0F, 0x33, 0x0E,
    0x1C, 0x2A, 0x38, 0x46, 0x54, 0x62, 0x69, 0x70, 0x77,
    0x79, 0x7B, 0x7D, 0x7E, 0x02, 0x02, 0x22, 0x00, 0x2A,
    0x40, 0x2A, 0xBE, 0x3A, 0xFC, 0x3A, 0xFA, 0x3A, 0xF8,
    0x3B, 0x38, 0x3B, 0x78, 0x3B, 0xB6, 0x4B, 0xB6, 0x4B,
    0xF4, 0x4B, 0xF4, 0x6C, 0x34, 0x84, 0x74, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00} },
    {0x9D, 0x01, {0x01} },

    {0x11, 0x00, {} },
    {REGFLAG_DELAY, 120, {} },

    /* TSP_SYNC1 Fixed Setting */
    {0xF0, 0x02, {0x5A, 0x5A} },
    {0xB0, 0x03, {0x00, 0x22, 0xB9} },
    {0xB9, 0x02, {0xA1, 0xB1} },
    {0xB0, 0x03, {0x00, 0x3A, 0xB9} },
    /* TSP_VSYNC Fixed TE 02:120 03:90 05:60 */
    {0xB9, 0x01, {0x02} },
    {0xB0, 0x03, {0x00, 0x26, 0xB9} },
    {0xB9, 0x02, {0x00, 0x00} },
    {0xF7, 0x01, {0x0F} },
    {0xF0, 0x02, {0xA5, 0xA5} },

    /* TSP_SYNC3 Fixed Setting */
    {0xF0, 0x02, {0x5A, 0x5A} },
    {0xB0, 0x03, {0x00, 0x24, 0xB9} },
    {0xB9, 0x01, {0x21} },
    {0xB0, 0x03, {0x00, 0x38, 0xB9} },
    /* TSP_VSYNC Fixed TE 02:120 03:90 05:60 */
    {0xB9, 0x01, {0x02} },
    {0xB0, 0x03, {0x00, 0x2A, 0xB9} },
    {0xB9, 0x02, {0x00, 0x00} },
    {0xF7, 0x01, {0x0F} },
    {0xF0, 0x02, {0xA5, 0xA5} },

    /* 120hz Transition */
    {0xF0, 0x02, {0x5A, 0x5A} },
    {0xB0, 0x03, {0x00, 0x16, 0xF2} },
    {0xF2, 0x02, {0x1B, 0x50} },
    {0xBD, 0x02, {0x21, 0x02} },
    {0x60, 0x01, {0x00} },
    {0xB0, 0x03, {0x00, 0x10, 0xBD} },
    {0xBD, 0x01, {0x10} },
    {0xF7, 0x01, {0x0F} },
    {0xF0, 0x02, {0xA5, 0xA5} },

    /* Common Setting */
    {0xF0, 0x02, {0x5A, 0x5A} },
    /* FQ_CON Setting */
    {0xF2, 0x02, {0x00, 0x01} },
    {0xF0, 0x02, {0xA5, 0xA5} },

    /* TE(Vsync) ON */
    {0xF0, 0x02, {0x5A, 0x5A} },
    {0x35, 0x01, {0x00} },
    //{0xB9, 0x01, {0x02} },
    {0xF0, 0x02, {0xA5, 0xA5} },

    /* CASET/PASET Setting */
    {0x2A, 0x04, {0x00, 0x00, 0x05, 0x9F} }, //1439
    {0x2B, 0x04, {0x00, 0x00, 0x0C, 0x8F} }, //3215

    /* Scaler Setting */
    {0xF0, 0x02, {0x5A, 0x5A} },
    {0xC3, 0x01, {0x00} },
    {0xF0, 0x02, {0xA5, 0xA5} },

    /* Pre-charge time setting */
    {0xF0, 0x02, {0x5A, 0x5A} },
    {0xB0, 0x03, {0x00, 0x2B, 0xF6} },
    {0xF6, 0x03, {0x60, 0x63, 0x69} },
    {0xF7, 0x01, {0x0F} },
    {0xF0, 0x02, {0xA5, 0xA5} },

    /* HLPM Power Saving */
    {0xF0, 0x02, {0x5A, 0x5A} },
    {0xB0, 0x03, {0x00, 0x46, 0xF4} },
    {0xF4, 0x01, {0x08} },
    {0xF0, 0x02, {0xA5, 0xA5} },

    /* DCDC setting of AOD Sequence */
    {0xF0, 0x02, {0x5A, 0x5A} },
    {0xB0, 0x03, {0x00, 0x18, 0xB1} },
    {0xB1, 0x01, {0x05} },
    {0xF0, 0x02, {0xA5, 0xA5} },

    /* Brightness Control */
    {0xF0, 0x02, {0x5A, 0x5A} },
    {0xB0, 0x03, {0x00, 0x0D, 0x63} },
    /* Dimming Setting */
    {0x63, 0x01, {0x01} },
    {0xB0, 0x03, {0x00, 0x0C, 0x63} },
    {0x63, 0x01, {0x00} },
    {0xF0, 0x02, {0xA5, 0xA5} },
    {0x53, 0x01, {0x28} },
    {0xF7, 0x01, {0x0F} },

    /* ACL Mode */
    {0xF0, 0x02, {0x5A, 0x5A} },
    {0x55, 0x01, {0x00} },
    {0xF0, 0x02, {0xA5, 0xA5} },

    /* OPEC Setting */
    {0xF0, 0x02, {0x5A, 0x5A} },
    {0xB0, 0x03, {0x00, 0x56, 0x1F} },
    {0x1F, 0x10, {0x01, 0x17, 0x01, 0x18, 0x06, 0x88,
                0x06, 0x89, 0x0A, 0xE2, 0x0A, 0xE3,
                0x11, 0xF5, 0x1B, 0xC0} },
    {0xB0, 0x03, {0x00, 0x6A, 0x1F} },
    {0x1F, 0x2C, {0x01, 0x17, 0x03, 0x44, 0x04, 0x5A,
                0x05, 0x71, 0x06, 0x88, 0x07, 0x9E,
                0x08, 0xB5, 0x09, 0xCB, 0x0A, 0xE2,
                0x0B, 0xF9, 0x0D, 0x0F, 0x0E, 0x26,
                0x0F, 0x3C, 0x10, 0x53} },
    {0xB0, 0x03, {0x00, 0x52, 0x1F} },
    {0x1F, 0x01, {0x02} },
    {0xB0, 0x03, {0x00, 0x54, 0x1F} },
    {0x1F, 0x01, {0x00} },
    {0xF0, 0x02, {0xA5, 0xA5} },

    /* Backlight on */
    {0x51, 2, {0x07, 0xFF} },

    /* Display On Setting */
    {0x29, 0x00, {} },
    {REGFLAG_END_OF_TABLE, 0x00, {} }
};

#ifdef LCM_SET_DISPLAY_ON_DELAY
/* to reduce init time, we move 120ms delay to lcm_set_display_on() !! */
static struct LCM_setting_table set_display_on[] = {
    {0x29, 0, {} },
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
            dsi_set_cmdq_V2(cmd, table[i].count,
                table[i].para_list, force_update);
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
    params->dsi.switch_mode = BURST_VDO_MODE;
#else
    params->dsi.mode = BURST_VDO_MODE;
    params->dsi.switch_mode = CMD_MODE;
#endif
    params->dsi.switch_mode_enable = 0;

    /* DSI */
    /* Command mode setting */
    params->dsi.LANE_NUM                = LCM_FOUR_LANE;
    //The following defined the fomat for data coming from LCD engine.
    params->dsi.data_format.color_order     = LCM_COLOR_ORDER_RGB;
    params->dsi.data_format.trans_seq       = LCM_DSI_TRANS_SEQ_MSB_FIRST;
    params->dsi.data_format.padding         = LCM_DSI_PADDING_ON_LSB;
    params->dsi.data_format.format              = LCM_DSI_FORMAT_RGB888;
    /* Highly depends on LCD driver capability. */
    params->dsi.packet_size = 256;

    /* video mode timing */

    params->dsi.PS = LCM_PACKED_PS_24BIT_RGB888;
    params->dsi.vertical_sync_active                = 8;
    params->dsi.vertical_backporch                  = 8;
    params->dsi.vertical_frontporch                 = 8;
    params->dsi.vertical_active_line                    = FRAME_HEIGHT;

    params->dsi.horizontal_sync_active              = 10;
    params->dsi.horizontal_backporch                = 20;
    params->dsi.horizontal_frontporch               = 40;
    params->dsi.horizontal_active_pixel             = FRAME_WIDTH;

    params->dsi.PLL_CLOCK = 718;    /* this value must be in MTK suggested table */
    params->dsi.data_rate  = 1436;  /* this value must be in MTK suggested table */

    params->dsi.ssc_disable = 1;
    //params->dsi.CLK_TRAIL = 9;
    /* clk continuous video mode */
    params->dsi.cont_clock = 0;
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
    /* DSC 10bit */
    params->dsi.dsc_enable = 1;
    params->dsi.dsc_params.ver = 17;
    params->dsi.dsc_params.slice_mode = 1;
    params->dsi.dsc_params.rgb_swap = 0;//PPS4[4]0x30 convert_rgb bit 4
    params->dsi.dsc_params.dsc_cfg = 2088;
    params->dsi.dsc_params.rct_on = 1;
    params->dsi.dsc_params.bit_per_channel = 10;//pps3[7:4]bits_per_component 0xAB
    params->dsi.dsc_params.dsc_line_buf_depth = 11;//PPS3[3:0] linebuf_depth 0xAB
    params->dsi.dsc_params.bp_enable = 1;//PPS4[5] 0x30 block_pred_enable bit 4
    params->dsi.dsc_params.bit_per_pixel = 128;//PPS4[1:0],PPS5[7:0]0x080
    params->dsi.dsc_params.pic_height = 3216;//PPS6[7:0],PPS7[7:0]0xc90
    params->dsi.dsc_params.pic_width = 1440;//PPS8[7:0],PPS9[7:0]0x5a0
    params->dsi.dsc_params.slice_height = 24;//PPS10[7:0],PPS11[7:0]0x018
    params->dsi.dsc_params.slice_width = 720;//PPS12[7:0],PPS13[7:0]0x2d0
    params->dsi.dsc_params.chunk_size = 720;//PPS14[7:0],PPS15[7:0]0x2d0
    params->dsi.dsc_params.xmit_delay = 512;//PPS16[1:0],PPS17[7:0]0x200
    params->dsi.dsc_params.dec_delay = 646;//PPS18[7:0],PPS19[7:0]0x286
    params->dsi.dsc_params.scale_value = 32;//PPS21[5:0]0x20
    params->dsi.dsc_params.increment_interval = 643;//PPS22[7:0],PPS23[7:0]0x283
    params->dsi.dsc_params.decrement_interval = 10;//PPS24[3:0],PPS25[7:0]0x0a
    params->dsi.dsc_params.line_bpg_offset = 13;//PPS27[4:0]0x0D
    params->dsi.dsc_params.nfl_bpg_offset = 1158;//PPS28[7:0],PPS29[7:0]0x486
    params->dsi.dsc_params.slice_bpg_offset = 814;//PPS30[7:0],PPS31[7:0]0x32E
    params->dsi.dsc_params.initial_offset = 6144;//PPS32[7:0],PPS33[7:0]0x1800
    params->dsi.dsc_params.final_offset = 4336;//PPS34[7:0],PPS35[7:0]0x10f0
    params->dsi.dsc_params.flatness_minqp = 7;//PPS36[4:0]0x07
    params->dsi.dsc_params.flatness_maxqp = 16;//PPS37[4:0]0x10
    params->dsi.dsc_params.rc_model_size = 8192;//PPS38[7:0],PPS39[7:0]0x2000
    params->dsi.dsc_params.rc_edge_factor = 6;//PPS40[3:0]0x06
    params->dsi.dsc_params.rc_quant_incr_limit0 = 15;//PPS41[4:0]0x0F
    params->dsi.dsc_params.rc_quant_incr_limit1 = 15;//PPS42[4:0]0x0F
    params->dsi.dsc_params.rc_tgt_offset_hi = 3;//PPS43[7:4] 0x03
    params->dsi.dsc_params.rc_tgt_offset_lo = 3;//PPS43[3:0] 0x03
}

/* turn on gate ic & control voltage to 5.8V */
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
    LCM_LOGI("%s\n", __func__);
    MDELAY(15);
    SET_RESET_PIN(1, fdt);
    MDELAY(5);
    SET_RESET_PIN(0, fdt);
    MDELAY(2);
    SET_RESET_PIN(1, fdt);
    MDELAY(5);

    push_table(init_setting,
        sizeof(init_setting) / sizeof(struct LCM_setting_table),
        1);

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

    /* we need to wait 120ms after lcm init to set display on */
    timeout = 120;

    while ((current_time() - lcm_init_time) <= timeout) {
        i++;
        MDELAY(1);
        if (i % 1000 == 0) {
            LCM_LOGI("ana6705 %s error: i=%u,lcm_init_time=%u,cur_time=%u\n", __func__,
                i, lcm_init_time, current_time());
        }
    }

    push_table(set_display_on,
        sizeof(set_display_on) / sizeof(struct LCM_setting_table),
        1);

    is_display_on = 1;
    return 0;
}
#endif

static void lcm_suspend(void)
{
#if 0
    int ret;

    push_table(lcm_suspend_setting,
        sizeof(lcm_suspend_setting) / sizeof(struct LCM_setting_table),
        1);
#ifndef MACH_FPGA
    /* enable AVDD & AVEE */
    /* 0x12--default value; bit3--Vneg; bit6--Vpos; */
    /*ret = RT5081_write_byte(0xB1, 0x12);*/
    ret = PMU_REG_MASK(0xB1, (0<<3) | (0<<6), (1<<3) | (1<<6));
    if (ret < 0)
        LCM_LOGI("ana6705--cmd=%0x--i2c write error----\n", 0xB1);
    else
        LCM_LOGI("ana6705--cmd=%0x--i2c write success----\n", 0xB1);

    MDELAY(5);

#endif
    SET_RESET_PIN(0);
    MDELAY(10);
#endif
}

static void lcm_resume(void)
{
    //lcm_init();
}

#if (LCM_DSI_CMD_MODE)
static void lcm_update(unsigned int x,
                        unsigned int y,
                        unsigned int width,
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
#else /* not LCM_DSI_CMD_MODE */

static void lcm_update(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
#ifdef LCM_SET_DISPLAY_ON_DELAY
    lcm_set_display_on();
#endif
}
#endif

static unsigned int lcm_compare_id(void)
{
#if 0
    unsigned int id = 0;
    unsigned char buffer[2];
    unsigned int array[16];

    LCM_LOGI("%S: enter\n", __func__);

    SET_RESET_PIN(1);
    MDELAY(10);
    SET_RESET_PIN(0);
    MDELAY(10);
    SET_RESET_PIN(1);
    MDELAY(10);

    array[0] = 0x00013700;  /* read id return two byte,version and id */
    dsi_set_cmdq(array, 1, 1);

    read_reg_v2(0xDA, buffer, 1);
    id = buffer[0];     /* we only need ID */

    LCM_LOGI("%s,ana6705_id_jdi=0x%08x\n", __func__, id);

#endif
    return 1;
}

/* return TRUE: need recovery */
/* return FALSE: No need recovery */
static unsigned int lcm_esd_check(void)
{
#ifndef BUILD_LK
    char buffer[3];
    int array[4];

    array[0] = 0x00013700;
    dsi_set_cmdq(array, 1, 1);

    read_reg_v2(0x0A, buffer, 1);

    if (buffer[0] != 0x9C) {
        LCM_LOGI("[LCM ERROR] [0x0A]=0x%02x\n", buffer[0]);
        return TRUE;
    }
    LCM_LOGI("[LCM NORMAL] [0x0A]=0x%02x\n", buffer[0]);
return FALSE;
#else
    return FALSE;
#endif
}

static void lcm_setbacklight(unsigned int level)
{
    LCM_LOGI("%s,ana6705 backlight: level = %d\n",
        __func__, level);

    bl_level[0].para_list[0] = level;

    push_table(bl_level,
        sizeof(bl_level) / sizeof(struct LCM_setting_table), 1);
}


LCM_DRIVER ana6705_fhdp_samsung_dsi_cmd_dphy_lcm_drv = {
    .name = "ana6705_fhdp_samsung_dsi_cmd_dphy_lcm_drv",
    .set_util_funcs = lcm_set_util_funcs,
    .get_params = lcm_get_params,
    .init = lcm_init,
    .suspend = lcm_suspend,
    .resume = lcm_resume,
    .compare_id = lcm_compare_id,
    .esd_check = lcm_esd_check,
    .init_power = lcm_init_power,
    .resume_power = lcm_resume_power,
    .suspend_power = lcm_suspend_power,
    .set_backlight = lcm_setbacklight,
    .update = lcm_update,
};
