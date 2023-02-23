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

#define LCM_ID_NT36672C_TIANMA (0x03) // TO DO

static const unsigned int BL_MIN_LEVEL = 20;
static LCM_UTIL_FUNCS lcm_util;

#define SET_RESET_PIN(v, fdt)    (lcm_util.set_reset_pin(v, fdt))
#define MDELAY(n)        (lcm_util.mdelay(n))
#define UDELAY(n)        (lcm_util.udelay(n))


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

#define HFP_SUPPORT (0)

/* static unsigned char lcd_id_pins_value = 0xFF; */
static const unsigned char LCD_MODULE_ID = 0x01;
/* --------------------------------------------------------------------------- */
/* Local Constants */
/* --------------------------------------------------------------------------- */
#define LCM_DSI_CMD_MODE                                    0
#define FRAME_WIDTH                                     (1080)
#define FRAME_HEIGHT                                    (2400)

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
    unsigned char para_list[64];
};

static struct LCM_setting_table init_setting[] = {
#if HFP_SUPPORT
    {0xFF, 1, {0x25} },
    {0xFB, 1, {0x01} },
    {0x18, 1, {0x21} },
#endif
    {0xFF, 1, {0x10} },
    {REGFLAG_DELAY, 100, {} },
    {0xFB, 1, {0x01} },
    {0xBA, 1, {0x02} },
    {0x35, 1, {0x00} },
#if (LCM_DSI_CMD_MODE)
    {0xBB, 1, {0x10} },
#else
    {0xBB, 1, {0x03} },
#endif
    {0xC0, 1, {0x00} },

    {0x11, 0, {} },
    {REGFLAG_DELAY, 120, {} },
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
                       unsigned int count, unsigned char force_update)
{
    unsigned int i;

    for (i = 0; i < count; i++) {
        unsigned int cmd = table[i].cmd;

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
    params->dsi.IsCphy = 1;

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
    params->dsi.LANE_NUM = LCM_THREE_LANE;
    /* The following defined the fomat for data coming from LCD engine. */
    params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
    params->dsi.data_format.trans_seq = LCM_DSI_TRANS_SEQ_MSB_FIRST;
    params->dsi.data_format.padding = LCM_DSI_PADDING_ON_LSB;
    params->dsi.data_format.format = LCM_DSI_FORMAT_RGB888;

    /* Highly depends on LCD driver capability. */
    params->dsi.packet_size = 256;
    /* video mode timing */

    params->dsi.PS = LCM_PACKED_PS_24BIT_RGB888;

    params->dsi.vertical_sync_active = 10;
    params->dsi.vertical_backporch = 4;
    params->dsi.horizontal_sync_active = 20;
    params->dsi.horizontal_backporch = 22;
#if HFP_SUPPORT
    params->dsi.vertical_frontporch = 50;
    params->dsi.vertical_frontporch_for_low_power = 880;
    params->dsi.horizontal_frontporch = 944;
#else
    params->dsi.vertical_frontporch = 1280;
    params->dsi.vertical_frontporch_for_low_power = 2530;
    params->dsi.horizontal_frontporch = 256;
#endif
    params->dsi.vertical_active_line = FRAME_HEIGHT;
    params->dsi.horizontal_active_pixel = FRAME_WIDTH;
    params->dsi.ssc_disable = 1; /* ssc will decrease fps */

#if (LCM_DSI_CMD_MODE)
    params->dsi.PLL_CLOCK = 480; /* this value must be in MTK suggested table */
#else
    params->dsi.PLL_CLOCK = 538; /* this value must be in MTK suggested table */
#endif
    //params->dsi.data_rate = 1076;

    params->dsi.clk_lp_per_line_enable = 0;
    params->dsi.esd_check_enable = 1;
    params->dsi.customization_esd_check_enable = 0;
    params->dsi.lcm_esd_check_table[0].cmd = 0x0A;
    params->dsi.lcm_esd_check_table[0].count = 1;
    params->dsi.lcm_esd_check_table[0].para_list[0] = 0x9C;

    params->dsi.lane_swap_en = 0;
#ifdef MTK_RUNTIME_SWITCH_FPS_SUPPORT
    params->dsi.fps = 90;
#endif
#if !HFP_SUPPORT
    /* for VFP resolution, do golden settings of high frame rate*/
    params->dsi.vact_fps = 60;
#endif

#ifdef MTK_ROUND_CORNER_SUPPORT
    params->round_corner_params.round_corner_en = 1;
    params->round_corner_params.full_content = 0;
    params->round_corner_params.h = ROUND_CORNER_H_TOP;
    params->round_corner_params.h_bot = ROUND_CORNER_H_BOT;
    params->round_corner_params.tp_size = sizeof(top_rc_pattern);
    params->round_corner_params.lt_addr = (void *)top_rc_pattern;
#endif
    params->dsi.dsc_enable = 0;
}

/* turn on gate ic & control voltage to 5.8V */
static void lcm_init_power(void *fdt)
{
    if (lcm_util.set_gpio_lcd_enp_bias) {
        lcm_util.set_gpio_lcd_enp_bias(1, fdt);
        _lcm_i2c_write_bytes(0x0, 0xf, fdt);
        _lcm_i2c_write_bytes(0x1, 0xf, fdt);
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
    MDELAY(15);
    SET_RESET_PIN(0, fdt);
    MDELAY(15);

    SET_RESET_PIN(1, fdt);
    MDELAY(15);

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
            LCM_LOGI("nt35695B %s error: i=%u,lcm_init_time=%u,cur_time=%u\n", __func__, i,
                lcm_init_time, current_time());
        }
    }

    push_table(set_display_on, sizeof(set_display_on) / sizeof(struct LCM_setting_table), 1);

    is_display_on = 1;
    return 0;
}
#endif

static void lcm_suspend(void)
{
}

static void lcm_resume(void)
{
    //lcm_init();
}

#if LCM_DSI_CMD_MODE == 1
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
    unsigned int id = 0;
    unsigned char buffer[2];
    unsigned int array[16];

    LCM_LOGI("%S: enter\n", __func__);

    SET_RESET_PIN(1, fdt);
    MDELAY(10);
    SET_RESET_PIN(0, fdt);
    MDELAY(10);
    SET_RESET_PIN(1, fdt);
    MDELAY(10);

    array[0] = 0x00013700; /* read id return two byte,version and id */
    dsi_set_cmdq(array, 1, 1);

    read_reg_v2(0xDA, buffer, 1);
    id = buffer[0];     /* we only need ID */

    LCM_LOGI("%s,nt3672C_id_tianma=0x%08x, expect:0x%08x\n", __func__, id, LCM_ID_NT36672C_TIANMA);

    if (id == LCM_ID_NT36672C_TIANMA)
        return 1;
    else
        return 0;
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

static unsigned int lcm_ata_check(unsigned char *buffer)
{
    return 0;
}

static void lcm_setbacklight_cmdq(void *handle, unsigned int level)
{

    LCM_LOGI("%s,nt36672c backlight: level = %d\n", __func__, level);

    bl_level[0].para_list[0] = level;

    push_table(bl_level, sizeof(bl_level) / sizeof(struct LCM_setting_table), 1);
}

static void lcm_setbacklight(unsigned int level)
{
    LCM_LOGI("%s,nt36672c backlight: level = %d\n", __func__, level);

    bl_level[0].para_list[0] = level;

    push_table(bl_level, sizeof(bl_level) / sizeof(struct LCM_setting_table), 1);
}

static void *lcm_switch_mode(int mode)
{
    return NULL;
}


LCM_DRIVER nt36672c_fhdp_dsi_vdo_cphy_90hz_tianma_rt4801_lcm_drv = {
    .name = "nt36672c_fhdp_dsi_vdo_cphy_90hz_tianma_rt4801_lcm_drv",
    .set_util_funcs = lcm_set_util_funcs,
    .get_params = lcm_get_params,
    .init = lcm_init,
    .suspend = lcm_suspend,
    .resume = lcm_resume,
    .compare_id = lcm_compare_id,
    .init_power = lcm_init_power,
    .resume_power = lcm_resume_power,
    .suspend_power = lcm_suspend_power,
    .esd_check = lcm_esd_check,
    .set_backlight = lcm_setbacklight,
    .ata_check = lcm_ata_check,
    .update = lcm_update,
    .switch_mode = lcm_switch_mode,
};
