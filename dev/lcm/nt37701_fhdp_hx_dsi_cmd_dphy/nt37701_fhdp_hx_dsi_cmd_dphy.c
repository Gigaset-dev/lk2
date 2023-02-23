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
#ifdef MTK_RT4831A_SUPPORT
#include "rt4831a_drv.h"
#endif
#define LCM_LOGI(string, args...)  dprintf(CRITICAL, "[LK/"LOG_TAG"]"string, ##args)
#define LCM_LOGD(string, args...)  dprintf(INFO, "[LK/"LOG_TAG"]"string, ##args)

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
#define read_reg(cmd) \
      lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size) \
        lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)

/* Local Constants */
/* --------------------------------------------------------------------------- */
#define LCM_DSI_CMD_MODE                1
#define FRAME_WIDTH                    1080
#define FRAME_HEIGHT                    2400

#define REGFLAG_DELAY                0xFFFC
#define REGFLAG_UDELAY                0xFFFB
#define REGFLAG_END_OF_TABLE                0xFFFD
#define REGFLAG_RESET_LOW                0xFFFE
#define REGFLAG_RESET_HIGH                0xFFFF

#define LCM_ID_NT37701 0x0
#define ENABLE_DSC 1

struct LCM_setting_table {
    unsigned int cmd;
    unsigned char count;
    unsigned char para_list[200];
};

static struct LCM_setting_table init_setting[] = {
    /* Switch PAGE0 */
    {0xF0, 5, {0x55, 0xAA, 0x52, 0x08, 0x00} },
    /* Voltage */
    {0x6F, 1, {0x06} },
    {0xB5, 3, {0x2B, 0x06, 0x33} },
    /* IDLE */
    {0x6F, 1, {0x0B} },
    {0xB5, 3, {0x2B, 0x23, 0x33} },
    /* ELVSS_SWIRE 1~5 for 144 */
    {0x6F, 1, {0x10} },
    {0xB5, 5, {0x06, 0x06, 0x06, 0x06, 0x06} },
    /* cmd Mode */
    {0xC0, 3, {0x76, 0xF3, 0xC1} },//cmd mode
    {0xB9, 2, {0x00, 0x96} },
    {0xBD, 2, {0x04, 0xB0} },
    {0xC9, 1, {0x84} },
    /* Switch Page4 */
    {0xF0, 5, {0x55, 0xAA, 0x52, 0x08, 0x04} },
    {0x6F, 1, {0x01} },
    {0xCB, 5, {0x05, 0x0F, 0x1F, 0x3E, 0x7C} },
    {0x6F, 1, {0x06} },
    {0xCB, 10, {0x00, 0x08, 0x00, 0x3C, 0x01, 0x48, 0x07, 0xFF, 0x0F, 0xFF} },
    /* F_R_EN_ON */
    {0xC2, 1, {0x14} },
    /* SPI Speed */
    {0xB1, 1, {0x02} },
    /* FLASH DMR SEL */
    {0xB2, 1, {0x40} },
    /* DMR addr && size for reload */
    {0x6F, 1, {0x01} },
    {0xB2, 3, {0x00, 0x00, 0x00} },
    {0x6F, 1, {0x04} },
    {0xB2, 3, {0x09, 0xE3, 0x40} },
    {0x6F, 1, {0x07} },
    {0xB2, 3, {0x09, 0xE4, 0x00} },
    {0x6F, 1, {0x0A} },
    {0xB2, 3, {0x09, 0xE3, 0x40} },
    /* DMR Setting */
    {0xCB, 1, {0x86} },
    {0xD0, 4, {0x00, 0x00, 0x00, 0x10} },
    /* Switch Page8 */
    {0xF0, 5, {0x55, 0xAA, 0x52, 0x08, 0x08} },
    {0xC0, 2, {0x8E, 0xFF} },
    //Vporth Setting
    {0x3B, 4, {0x00, 0x14, 0x00, 0x1C} },
    //DSC setting
    {0x03, 1, {0x01} },
    {0x90, 1, {0x91} },
    {0x91, 18, {0x89, 0x28, 0x00, 0x08,
        0xC2, 0x00, 0x03, 0x1C, 0x00,
        0xF6, 0x00, 0x0F, 0x0D, 0xB7,
        0x06, 0x5C, 0x10, 0xF0} },
    {0x2C, 0, {} },
    //AC/DC Dimming
    {0x51, 4, {0x07, 0xFF, 0x0F, 0xFF} },
    {0x53, 1, {0x28} },
    //TE on
    {0x35, 0, {} },
    //Gram Address
    {0x2A, 4, {0x00, 0x00, 0x04, 0x37} },
    {0x2B, 4, {0x00, 0x00, 0x09, 0x5F} },
    //FrameRate 60Hz:0x01  120HZ:0x02
    {0x2F, 1, {0x01} },
    /* Sleep Out */
    {0x11, 0, {} },
    {REGFLAG_DELAY, 120, {} },
    /* Display On */
    {0x29, 0, {} },
};

#ifdef LCM_SET_DISPLAY_ON_DELAY
/* to reduce init time, we move 120ms delay to lcm_set_display_on() !! */
static struct LCM_setting_table set_display_on[] = {
    {0x29, 0, {} },
};
#endif

static struct LCM_setting_table bl_level[] = {
    {0x51, 1, {0xFF} },
    {REGFLAG_END_OF_TABLE, 0x00, {} }
};

static void push_table(struct LCM_setting_table *table,
            unsigned int count,
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
            dsi_set_cmdq_V2(cmd,
                table[i].count,
                table[i].para_list,
                force_update);
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

    params->dsi.mode = CMD_MODE;
    /* DSI */
    /* Command mode setting */
    params->dsi.LANE_NUM                    = LCM_FOUR_LANE;
    //The following defined the fomat for data coming from LCD engine.
    params->dsi.data_format.color_order    = LCM_COLOR_ORDER_RGB;
    params->dsi.data_format.trans_seq    = LCM_DSI_TRANS_SEQ_MSB_FIRST;
    params->dsi.data_format.padding    = LCM_DSI_PADDING_ON_LSB;
    params->dsi.data_format.format        = LCM_DSI_FORMAT_RGB888;
    /* Highly depends on LCD driver capability. */
    params->dsi.packet_size = 256;
    params->dsi.PS = LCM_PACKED_PS_24BIT_RGB888;

    /* video mode timing */
    //VSA
    params->dsi.vertical_sync_active                  = 2;
    //VBP
    params->dsi.vertical_backporch                    = 8;
    //VFP
    params->dsi.vertical_frontporch                   = 20;
    params->dsi.vertical_active_line                  = FRAME_HEIGHT;
    //HSA
    params->dsi.horizontal_sync_active                = 10;
    //HBP
    params->dsi.horizontal_backporch                  = 20;
    //HFP
    params->dsi.horizontal_frontporch                 = 40;
    //HACT
    params->dsi.horizontal_active_pixel               = FRAME_WIDTH;
    params->dsi.ssc_disable                           = 1;

    params->dsi.PLL_CLOCK                = 480;
    params->dsi.data_rate                = 960;

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
    params->dsi.dsc_params.slice_mode = 0;
    params->dsi.dsc_params.rgb_swap = 0;
    params->dsi.dsc_params.dsc_cfg = 34;
    params->dsi.dsc_params.rct_on = 1;
    params->dsi.dsc_params.bit_per_channel = 8;
    params->dsi.dsc_params.dsc_line_buf_depth = 9;
    params->dsi.dsc_params.bp_enable = 1;
    params->dsi.dsc_params.bit_per_pixel = 128;
    params->dsi.dsc_params.pic_height = 2400;
    params->dsi.dsc_params.pic_width = 1080;
    params->dsi.dsc_params.slice_height = 8;
    params->dsi.dsc_params.slice_width = 1080;
    params->dsi.dsc_params.chunk_size = 1080;
    params->dsi.dsc_params.xmit_delay = 512;
    params->dsi.dsc_params.dec_delay = 796;
    params->dsi.dsc_params.scale_value = 32;
    params->dsi.dsc_params.increment_interval = 246;
    params->dsi.dsc_params.decrement_interval = 15;
    params->dsi.dsc_params.line_bpg_offset = 12;
    params->dsi.dsc_params.nfl_bpg_offset = 3511;
    params->dsi.dsc_params.slice_bpg_offset = 1628;
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

/* turn on gate ic & control voltage to 5.5V */
static void lcm_init_power(void *fdt)
{
    SET_RESET_PIN(0, fdt);
    MDELAY(100);


#ifdef MTK_RT4831A_SUPPORT
    if (gate_ic_power_status_setting(fdt, 1)) {
        LCM_LOGI("set_gpio_lcm_led_en not defined\n");
        return;
    }
    _gate_ic_i2c_panel_bias_enable(fdt, 1);
#endif
    LCM_LOGI("%s---\n", __func__);
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
    MDELAY(15);

    SET_RESET_PIN(1, fdt);
    MDELAY(1);
    SET_RESET_PIN(0, fdt);
    MDELAY(10);

    SET_RESET_PIN(1, fdt);
    MDELAY(10);

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
            LCM_LOGI("nt37701 %s error: i=%u,lcm_init_time=%u,cur_time=%u\n",
                __func__, i, lcm_init_time, current_time());
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

}

static void lcm_resume(void)
{

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

static void lcm_update(unsigned int x,
        unsigned int y,
        unsigned int width,
        unsigned int height)
{

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

    array[0] = 0x00013700;
    dsi_set_cmdq(array, 1, 1);

    read_reg_v2(0xDA, buffer, 1);
    id = buffer[0];     /* we only need ID */

    LCM_LOGI("%s,nt37701_id_jdi=0x%08x\n", __func__, id);
    if (id == LCM_ID_NT37701)
        return 1;
    else
        return 0;
}

/* return TRUE: need recovery */
/* return FALSE: No need recovery */
static unsigned int lcm_esd_check(void)
{
    return 0;
}

static void lcm_setbacklight(unsigned int level)
{
    LCM_LOGI("%s,nt37701 backlight: level = %d\n", __func__, level);

    bl_level[0].para_list[0] = level;

    push_table(bl_level,
        sizeof(bl_level) / sizeof(struct LCM_setting_table),
        1);
}

LCM_DRIVER nt37701_fhdp_hx_dsi_cmd_dphy_lcm_drv = {
    .name              = "nt37701_fhdp_hx_dsi_cmd_dphy_lcm_drv",
    .set_util_funcs    = lcm_set_util_funcs,
    .get_params        = lcm_get_params,
    .init              = lcm_init,/*tianma init fun.*/
    .suspend           = lcm_suspend,
    .resume            = lcm_resume,
    .compare_id        = lcm_compare_id,
    .init_power        = lcm_init_power,
    .resume_power      = lcm_resume_power,
    .suspend_power     = lcm_suspend_power,
    .set_backlight     = lcm_setbacklight,
    .update            = lcm_update,
};
