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
#include <gpio_api.h>
#include <spmi_common.h>

#include "lcm_drv.h"
#include "lcm_util.h"

#define LCM_LOGI(string, args...)  dprintf(CRITICAL, "[LK/"LOG_TAG"]"string, ##args)
#define LCM_LOGD(string, args...)  dprintf(INFO, "[LK/"LOG_TAG"]"string, ##args)

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
#define PANEL_2K 0

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
#define GPIO_LCM_RESET_PIN  GPIO86

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



#if 1

static int mt6368_ldo_3p0_enable(u8 en)
{
    int ret = 0, val = 0;
    struct spmi_device *mt6368_devs; //MT 6368 ID: 0x5

    mt6368_devs = get_spmi_device(SPMI_MASTER_1, SPMI_SLAVE_5);//MT 6368 ID: 0x5
    if (!mt6368_devs) {
        LCM_LOGI("[LCM][ERROR]get mt6368_devs err %s\n", __func__);
        return ERR_INVALID_ARGS;
    }
    if (en) {
        ret = spmi_ext_register_writel_field(mt6368_devs, 0x1D99, 0xC, 0xF, 0);
        ret = spmi_ext_register_writel_field(mt6368_devs, 0x1C31, 0x1, 0x1, 0);
        LCM_LOGI("[LCM]%s ret=%d en=%d\n", __func__, ret, en);

        ret = spmi_ext_register_readl(mt6368_devs, 0x1D99, &val, 1);
        LCM_LOGI("[LCM]%s ret=%d val=%d\n", __func__, ret, val);
        ret = spmi_ext_register_readl(mt6368_devs, 0x1C31, &val, 1);
        LCM_LOGI("[LCM]%s ret=%d val=%d\n", __func__, ret, val);
    } else {
        ret = spmi_ext_register_writel_field(mt6368_devs, 0x1C31, 0x0, 0x1, 0);
        LCM_LOGI("[LCM]%s ret=%d en=%d\n", __func__, ret, en);
        ret = spmi_ext_register_readl(mt6368_devs, 0x1C31, &val, 1);
        LCM_LOGI("[LCM]%s ret=%d val=%d\n", __func__, ret, val);
    }

    return ret;
}

static int mt6368_ldo_1p8_enable(u8 en)
{
    int ret = 0, val = 0;
    struct spmi_device *mt6368_devs; //MT 6368 ID: 0x5

    mt6368_devs = get_spmi_device(SPMI_MASTER_1, SPMI_SLAVE_5);//MT 6368 ID: 0x5
    if (!mt6368_devs) {
        LCM_LOGI("[LCM][ERROR]get mt6368_devs err %s\n", __func__);
        return ERR_INVALID_ARGS;
    }

    if (en) {
        ret = spmi_ext_register_writel_field(mt6368_devs, 0x1C15, 0x1, 0x1, 0);
        LCM_LOGI("[LCM]%s ret=%d en=%d\n", __func__, ret, en);
        ret = spmi_ext_register_readl(mt6368_devs, 0x1C15, &val, 1);
        LCM_LOGI("[LCM]%s ret=%d val=%d\n", __func__, ret, val);
    } else {
        ret = spmi_ext_register_writel_field(mt6368_devs, 0x1C15, 0x0, 0x1, 0);
        LCM_LOGI("[LCM]%s ret=%d en=%d\n", __func__, ret, en);
        ret = spmi_ext_register_readl(mt6368_devs, 0x1C15, &val, 1);
        LCM_LOGI("[LCM]%s ret=%d val=%d\n", __func__, ret, val);
    }

    return ret;
}
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
#else

static struct LCM_setting_table init_setting[] = {
    /* ESD */
    {0xF0, 05, {0x55, 0xAA, 0x52, 0x08, 0x00} },
    {0x6F, 01, {0x05} },
    {0xBE, 01, {0x08} },
    {0x6F, 01, {0x01} },
    {0xBE, 01, {0x45} },
    /* DC init Code */
    /* 3.1 DVDD Strong */
    {0xF0, 05, {0x55, 0xAA, 0x52, 0x08, 0x01} },
    {0xC5, 04, {0x00, 0x0B, 0x0B, 0x0B} },
    /* DSC setting */
    {0x90, 02, {0x13, 0x03} },
    {0x91, 18, {0xAB, 0x28, 0x00, 0x0C, 0xC2, 0x00, 0x03,
                0x1C, 0x01, 0x7E, 0x00, 0x0F, 0x08,
                0xBB, 0x04, 0x3D, 0x10, 0xF0} },
    {0x1F, 01, {0x70} },
    /* 3.2 TE ON */
    {0x35, 01, {0x00} },
    {0x5A, 01, {0x01} },
    /* 3.3 CASET/RASET Setting*/
    {0x2A, 04, {0x00, 0x00, 0x04, 0x37} },
    {0x2B, 04, {0x00, 0x00, 0x09, 0x5F} },
    /* 3.4 Dimming ON Setting */
    {0x53, 01, {0x28} },
    /* 3.4.1 Adjusting dimming speed */
    {0xF0, 05, {0x55, 0xAA, 0x52, 0x08, 0x00} },
    {0x6F, 01, {0x05} },
    {0xB2, 02, {0x08, 0x08} },
    {0x6F, 01, {0x09} },
    {0xB2, 02, {0x5F, 0x5F} },

    /*RTE*/
    {0x6F, 01, {0x13} },
    {0xC0, 02, {0x02, 0x64} },

    {0x6F, 01, {0x1F} },
    {0xC0, 01, {0x38} },
    /* AFP */
    {0x6F, 01, {0x22} },
    {0xC0, 02, {0x0A, 0x00} },

    {0x6F, 01, {0x20} },
    {0xC0, 02, {0x02, 0x00} },

    {0x40, 01, {0x07} },
    {0x6F, 01, {0x01} },
    {0x40, 02, {0x09, 0x2C} },
    {0x6F, 01, {0x03} },
    {0x40, 02, {0x13, 0x20} },

    /* 3.5 Frame Rate 120Hz GIR OFF */
    {0x2F, 01, {0x00} },
    {0x26, 01, {0x00} },
    /* 3.6 GIR OFF */
    {0x5F, 01, {0x01} },
    /* 3.7 DBV */
    {0x51, 02, {0x07, 0xFF} },
    /* 4. User Command Set */
    /* Sleep Out */
    {0x11, 00, {} },
    {REGFLAG_DELAY, 120, {} },
    /* Display On */
    {0x29, 01, {0x00} },
    {REGFLAG_END_OF_TABLE, 0x00, {} }
};

#endif

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
    params->dsi.PLL_CLOCK = 550;    /* this value must be in MTK suggested table */
    params->dsi.data_rate  = 1100;  /* this value must be in MTK suggested table */
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
   params->dsi.dsc_params.slice_mode = 0;
   params->dsi.dsc_params.rgb_swap = 0;
   params->dsi.dsc_params.dsc_cfg = 40;
   params->dsi.dsc_params.rct_on = 1;
   params->dsi.dsc_params.bit_per_channel = 10;
   params->dsi.dsc_params.dsc_line_buf_depth = 11;
   params->dsi.dsc_params.bp_enable = 1;
   params->dsi.dsc_params.bit_per_pixel = 128;
   params->dsi.dsc_params.pic_height = 2400;
   params->dsi.dsc_params.pic_width = 1080;
   params->dsi.dsc_params.slice_height = 12;
   params->dsi.dsc_params.slice_width = 1080;
   params->dsi.dsc_params.chunk_size = 1080;
   params->dsi.dsc_params.xmit_delay = 512;
   params->dsi.dsc_params.dec_delay = 796;
   params->dsi.dsc_params.scale_value = 32;
   params->dsi.dsc_params.increment_interval = 382;
   params->dsi.dsc_params.decrement_interval = 15;
   params->dsi.dsc_params.line_bpg_offset = 12;
   params->dsi.dsc_params.nfl_bpg_offset = 2235;
   params->dsi.dsc_params.slice_bpg_offset = 1085;
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

static void lcm_init_power(void *fdt)
{
#if 0
    mt_set_gpio_mode(GPIO41, GPIO_MODE_00); //LCM_RESET
    mt_set_gpio_dir(GPIO41, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO41, GPIO_OUT_ZERO);

    MDELAY(5);

    mt6368_ldo_1p8_enable(1);               //1.8V,VAUD18
    //MDELAY(10);
    mt_set_gpio_mode(GPIO147, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO147, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO147, 1);             //1.6V,DSIP0_DVD_1P6
    MDELAY(5);
    mt6368_ldo_3p0_enable(1);               //3.0,VIBR30
    //MDELAY(10);
    mt_set_gpio_mode(GPIO41, GPIO_MODE_00); //LCM_RESET
    mt_set_gpio_dir(GPIO41, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO41, GPIO_OUT_ONE);
    MDELAY(5);
    //SET_RESET_PIN(0);
    mt_set_gpio_out(GPIO41, GPIO_OUT_ZERO);
    MDELAY(5);
    //SET_RESET_PIN(1);
    mt_set_gpio_out(GPIO41, GPIO_OUT_ONE);
    MDELAY(10);
#endif
    LCM_LOGI("%s[lh]: -\n", __func__);
}

static void lcm_suspend_power(void)
{
#if 0
    LCM_LOGD("%s +\n", __func__);
    mt6368_ldo_1p8_enable(0);               //1.8V,VAUD18
    MDELAY(10);
    mt_set_gpio_mode(GPIO147, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO147, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO147, 0);             //1.6V,DSIP0_DVD_1P6
    MDELAY(10);
    mt6368_ldo_3p0_enable(0);               //3.0,VIBR30
    MDELAY(10);
    LCM_LOGD("%s -\n", __func__);
#endif
}

static void lcm_resume_power(void)
{
#if 0
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
    SET_RESET_PIN(0, fdt);
    MDELAY(15);
    SET_RESET_PIN(1, fdt);
    MDELAY(1);
    SET_RESET_PIN(0, fdt);
    MDELAY(10);
    SET_RESET_PIN(1, fdt);
    MDELAY(10);

    push_table(init_setting, sizeof(init_setting) / sizeof(struct LCM_setting_table), 1);

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


static unsigned int lcm_compare_id(void *fdt)
{
   return 1;
}

static void lcm_setbacklight(unsigned int level)
{
    LCM_LOGI("%s, backlight: level = %d\n", __func__, level);

    bl_level[0].para_list[0] = (level >> 8) & 0x7;
    bl_level[0].para_list[1] = (level & 0xFF);

    push_table(bl_level, sizeof(bl_level) / sizeof(struct LCM_setting_table), 1);
}

LCM_DRIVER nt37801_fhdplus_cmd_120hz_r4831a_lcm_drv = {
    .name = "nt37801_fhdplus_cmd_120hz_r4831a_lcm_drv",
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
