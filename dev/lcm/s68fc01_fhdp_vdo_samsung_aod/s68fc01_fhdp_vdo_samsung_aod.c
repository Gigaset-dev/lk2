/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#define BUILD_LK
#define LOG_TAG "LCM"

#include <debug.h>
#include <mt_i2c.h>
#include <platform.h>

#include "lcm_drv.h"

#ifdef MTK_ROUND_CORNER_SUPPORT
#include "data_rgba4444_roundedpattern.h"
#endif

#include "rt4831a_drv.h"

#ifdef BUILD_LK
#define LCM_LOGI(string, args...)  dprintf(0, "[LK/"LOG_TAG"]"string, ##args)
#define LCM_LOGD(string, args...)  dprintf(1, "[LK/"LOG_TAG"]"string, ##args)
#else
#define LCM_LOGI(fmt, args...)  pr_notice("[KERNEL/"LOG_TAG"]"fmt, ##args)
#define LCM_LOGD(fmt, args...)  pr_debug("[KERNEL/"LOG_TAG"]"fmt, ##args)
#endif

#define LCM_ID_s68fc01 (0xB3)

static const unsigned int BL_MIN_LEVEL = 20;
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

/* static unsigned char lcd_id_pins_value = 0xFF; */
static const unsigned char LCD_MODULE_ID = 0x01;
/* --------------------------------------------------------------------------- */
/* Local Constants */
/* --------------------------------------------------------------------------- */
#define LCM_DSI_CMD_MODE                                    0
#define FRAME_WIDTH                                     (1080)
#define FRAME_HEIGHT                                    (2400)

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
    unsigned char para_list[64];
};

static struct LCM_setting_table lcm_suspend_setting[] = {
    {0x28, 0, {} },
    {REGFLAG_DELAY, 10, {} },
    {0x10, 0, {} },
    {REGFLAG_DELAY, 150, {} },
    {REGFLAG_END_OF_TABLE, 0x00, {} }
};

#if 0
static struct LCM_setting_table init_setting[] = {
    {0x11, 0, {} },
    {REGFLAG_DELAY, 5, {} },

    /* TE vsync ON */
    {0x35, 1, {0x00} },
    {0x34, 0, {} },
    /* FAIL Safe Setting */
    {0xFC, 2, {0x5A, 0x5A} },
    {0xED, 12, {0x00, 0x01, 0x00, 0x40, 0x04, 0x08, 0xA8, 0x84,
    0x4A, 0x73, 0x02, 0x0A} },
    {0xFC, 2, {0xA5, 0xA5} },
    /* Backlight Dimming Setting */
    {0xF0, 2, {0x5A, 0x5A} },
    {0xB0, 1, {0x05} },
    {0xB3, 1, {0x07} },//0x87 dim on,0x07 dim off
    {0xF0, 2, {0xA5, 0xA5} },
    {0x53, 1, {0x20} },//dim speed 0x20 1frame,0x28 32 frame
    {0x51, 2, {0x03, 0xFD} },//BRIGHTNESS control,min (0x00,0x0c)
    /* HBM MODE ON setting */
    {0x53, 1, {0xE0} },//
    {0x51, 2, {0x07, 0xFF} },//
    /* ACL off */
    {0x55, 1, {0x00} },
    /* Display On*/
    {0x29, 0, {} },
    {REGFLAG_END_OF_TABLE, 0x00, {} }
};
#endif
static struct LCM_setting_table init_setting[] = {
    {0x9F, 2, {0xA5, 0xA5} },
    {0x11, 0, {} },
    {REGFLAG_DELAY, 20, {} },
    {0x9F, 2, {0x5A, 0x5A} },
    /* TE vsync ON */
    {0x9F, 2, {0xA5, 0xA5} },
    {0x35, 1, {0x00} },
    {0x9F, 2, {0x5A, 0x5A} },
    /* FAIL SAFE Setting */
    {0xFC, 2, {0x5A, 0x5A} },
    {0xED, 12, {0x00, 0x01, 0x00, 0x40, 0x04, 0x08, 0xA8, 0x84, 0x4A, 0x73, 0x02, 0x0A} },
    {0xFC, 2, {0xA5, 0xA5} },
    /* ELVSS Dim Setting */
    {0xF0, 2, {0x5A, 0x5A} },
    {0xB0, 1, {0x05} },
    {0xB3, 1, {0x87} },
    {0xF0, 2, {0xA5, 0xA5} },
    /* Backlight Dimming Setting */
    {0x53, 1, {0x20} },
    /* ACL off */
    {0x55, 1, {0x00} },
    {REGFLAG_DELAY, 100, {} },
    /* Display On*/
    {0x9F, 2, {0xA5, 0xA5} },
    {0x29, 0, {} },
    {0x51, 2, {0x03, 0xff} },
    {0x9F, 2, {0x5A, 0x5A} },
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

    params->dsi.vertical_sync_active = 2;
    params->dsi.vertical_backporch = 9;
    params->dsi.vertical_frontporch = 21;
    params->dsi.vertical_active_line = FRAME_HEIGHT;

    params->dsi.horizontal_sync_active = 14;
    params->dsi.horizontal_backporch = 22;
    params->dsi.horizontal_frontporch = 48;
    params->dsi.horizontal_active_pixel = FRAME_WIDTH;
    params->dsi.ssc_disable = 1;
#ifndef MACH_FPGA
#if (LCM_DSI_CMD_MODE)
    params->dsi.PLL_CLOCK = 553;    /* this value must be in MTK suggested table */
#else
    params->dsi.PLL_CLOCK = 549;    /* this value must be in MTK suggested table */
    params->dsi.data_rate  = 1098;  /* this value must be in MTK suggested table */
#endif
#else
    params->dsi.pll_div1 = 0;
    params->dsi.pll_div2 = 0;
    params->dsi.fbk_div = 0x1;
#endif

    params->dsi.ssc_disable = 1;
    params->dsi.clk_lp_per_line_enable = 0;
    params->dsi.esd_check_enable = 0;
    params->dsi.customization_esd_check_enable = 0;
    params->dsi.lcm_esd_check_table[0].cmd = 0x53;
    params->dsi.lcm_esd_check_table[0].count = 1;
    params->dsi.lcm_esd_check_table[0].para_list[0] = 0x24;

#ifdef MTK_ROUND_CORNER_SUPPORT
    params->round_corner_params.round_corner_en = 1;
    params->round_corner_params.full_content = 0;
    params->round_corner_params.w = ROUND_CORNER_W;
    params->round_corner_params.h = ROUND_CORNER_H;
    params->round_corner_params.lt_addr = left_top;
    params->round_corner_params.lb_addr = left_bottom;
    params->round_corner_params.rt_addr = right_top;
    params->round_corner_params.rb_addr = right_bottom;
#endif
}

static void lcm_init_power(void)
{
    // #ifndef MACH_FPGA
    // #ifdef BUILD_LK
    //     mt6325_upmu_set_rg_vgp1_en(1);
    // #else
    //     LCM_LOGI("%s, begin\n", __func__);
    //     hwPowerOn(MT6325_POWER_LDO_VGP1, VOL_DEFAULT, "LCM_DRV");
    //     LCM_LOGI("%s, end\n", __func__);
    // #endif
    // #endif
}

static void lcm_suspend_power(void)
{
    // #ifndef MACH_FPGA
    // #ifdef BUILD_LK
    //     mt6325_upmu_set_rg_vgp1_en(0);
    // #else
    //     LCM_LOGI("%s, begin\n", __func__);
    //     hwPowerDown(MT6325_POWER_LDO_VGP1, "LCM_DRV");
    //     LCM_LOGI("%s, end\n", __func__);
    // #endif
    // #endif
}

static void lcm_resume_power(void)
{
    // #ifndef MACH_FPGA
    // #ifdef BUILD_LK
    //     mt6325_upmu_set_rg_vgp1_en(1);
    // #else
    //     LCM_LOGI("%s, begin\n", __func__);
    //     hwPowerOn(MT6325_POWER_LDO_VGP1, VOL_DEFAULT, "LCM_DRV");
    //     LCM_LOGI("%s, end\n", __func__);
    // #endif
    // #endif
}

#ifdef LCM_SET_DISPLAY_ON_DELAY
static u32 lcm_init_time;
static int is_display_on;
#endif

static void lcm_init(void *fdt)
{
    int ret = 0;

    SET_RESET_PIN(0, fdt);

#if 0
    /*config rt5081 register 0xB2[7:6]=0x3, that is set db_delay=4ms.*/
    ret = PMU_REG_MASK(0xB2, (0x3 << 6), (0x3 << 6));

    /* set AVDD 5.4v, (4v+28*0.05v) */
    /*ret = RT5081_write_byte(0xB3, (1 << 6) | 28);*/
    ret = PMU_REG_MASK(0xB3, 28, (0x3F << 0));
    if (ret < 0)
        LCM_LOGI("s68fc01----tps6132----cmd=%0x--i2c write error----\n", 0xB3);
    else
        LCM_LOGI("s68fc01----tps6132----cmd=%0x--i2c write success----\n", 0xB3);

    /* set AVEE */
    /*ret = RT5081_write_byte(0xB4, (1 << 6) | 28);*/
    ret = PMU_REG_MASK(0xB4, 28, (0x3F << 0));
    if (ret < 0)
        LCM_LOGI("s68fc01----tps6132----cmd=%0x--i2c write error----\n", 0xB4);
    else
        LCM_LOGI("s68fc01----tps6132----cmd=%0x--i2c write success----\n", 0xB4);

    /* enable AVDD & AVEE */
    /* 0x12--default value; bit3--Vneg; bit6--Vpos; */
    /*ret = RT5081_write_byte(0xB1, 0x12 | (1<<3) | (1<<6));*/
    ret = PMU_REG_MASK(0xB1, (1<<3) | (1<<6), (1<<3) | (1<<6));
    if (ret < 0)
        LCM_LOGI("s68fc01----tps6132----cmd=%0x--i2c write error----\n", 0xB1);
    else
        LCM_LOGI("s68fc01----tps6132----cmd=%0x--i2c write success----\n", 0xB1);

    MDELAY(15);

#endif
    SET_RESET_PIN(1, fdt);
    MDELAY(1);
    SET_RESET_PIN(0, fdt);
    MDELAY(10);

    SET_RESET_PIN(1, fdt);
    MDELAY(10);

    push_table(init_setting,
            sizeof(init_setting) / sizeof(struct LCM_setting_table), 1);
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
            LCM_LOGI("s68fc01B %s error: i=%u,lcm_init_time=%u,cur_time=%u\n", __func__, i,
                lcm_init_time, current_time());
        }
    }

    push_table(set_display_on,
            sizeof(set_display_on) / sizeof(struct LCM_setting_table), 1);

    is_display_on = 1;
    return 0;
}
#endif

static void lcm_suspend(void)
{
#if 0
    int ret;

    push_table(lcm_suspend_setting,
            sizeof(lcm_suspend_setting) / sizeof(struct LCM_setting_table), 1);
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

static unsigned int lcm_compare_id(void)
{
#if 0
    unsigned int id = 0;
    unsigned int version_id;
    unsigned char buffer[2];
    unsigned int array[16];

    SET_RESET_PIN(1);
    SET_RESET_PIN(0);
    MDELAY(1);

    SET_RESET_PIN(1);
    MDELAY(20);

    array[0] = 0x00023700;  /* read id return two byte,version and id */
    dsi_set_cmdq(array, 1, 1);

    read_reg_v2(0xDA, buffer, 2);
    id = buffer[0];     /* we only need ID */

    read_reg_v2(0xDB, buffer, 1);
    version_id = buffer[0];

    LCM_LOGI("%s,s68fc01_id=0x%08x,version_id=0x%x\n", __func__, id, version_id);

    if (id == LCM_ID_s68fc01)
        return 1;
    else
        return 0;
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

    read_reg_v2(0x53, buffer, 1);

    if (buffer[0] != 0x24) {
        LCM_LOGI("[LCM ERROR] [0x53]=0x%02x\n", buffer[0]);
        return TRUE;
    }
    LCM_LOGI("[LCM NORMAL] [0x53]=0x%02x\n", buffer[0]);
    return FALSE;
#else
    return FALSE;
#endif

}

static unsigned int lcm_ata_check(unsigned char *buffer)
{
#ifndef BUILD_LK
    unsigned int ret = 0;
    unsigned int x0 = FRAME_WIDTH / 4;
    unsigned int x1 = FRAME_WIDTH * 3 / 4;

    unsigned char x0_MSB = ((x0 >> 8) & 0xFF);
    unsigned char x0_LSB = (x0 & 0xFF);
    unsigned char x1_MSB = ((x1 >> 8) & 0xFF);
    unsigned char x1_LSB = (x1 & 0xFF);

    unsigned int data_array[3];
    unsigned char read_buf[4];

    LCM_LOGI("ATA check size = 0x%x, 0x%x, 0x%x, 0x%x\n", x0_MSB, x0_LSB, x1_MSB, x1_LSB);
    data_array[0] = 0x0005390A; /* HS packet */
    data_array[1] = (x1_MSB << 24) | (x0_LSB << 16) | (x0_MSB << 8) | 0x2a;
    data_array[2] = (x1_LSB);
    dsi_set_cmdq(data_array, 3, 1);

    data_array[0] = 0x00043700; /* read id return two byte,version and id */
    dsi_set_cmdq(data_array, 1, 1);

    read_reg_v2(0x2A, read_buf, 4);

    if ((read_buf[0] == x0_MSB) && (read_buf[1] == x0_LSB)
            && (read_buf[2] == x1_MSB) && (read_buf[3] == x1_LSB))
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

    LCM_LOGI("%s,s68fc01 backlight: level = %d\n", __func__, level);

    bl_level[0].para_list[1] = level;

    push_table(bl_level, sizeof(bl_level) / sizeof(struct LCM_setting_table), 1);
}

static void lcm_setbacklight(unsigned int level)
{
    LCM_LOGI("%s,s68fc01 backlight: level = %d\n", __func__, level);

    bl_level[0].para_list[1] = level;

    push_table(bl_level, sizeof(bl_level) / sizeof(struct LCM_setting_table), 1);
}

static void *lcm_switch_mode(int mode)
{
#ifndef BUILD_LK
    /* customization:
     * 1. V2C config 2 values, C2V config 1 value;
     * 2. config mode control register
     */
    if (mode == 0) {    /* V2C */
        lcm_switch_mode_cmd.mode = CMD_MODE;
        /* mode control addr */
        lcm_switch_mode_cmd.addr = 0xBB;
        /* enabel GRAM firstly, ensure writing one frame to GRAM */
        lcm_switch_mode_cmd.val[0] = 0x13;
        /* disable video mode secondly */
        lcm_switch_mode_cmd.val[1] = 0x10;
    } else {        /* C2V */
        lcm_switch_mode_cmd.mode = SYNC_PULSE_VDO_MODE;
        lcm_switch_mode_cmd.addr = 0xBB;
        /* disable GRAM and enable video mode */
        lcm_switch_mode_cmd.val[0] = 0x03;
    }
    return (void *)(&lcm_switch_mode_cmd);
#else
    return NULL;
#endif
}


LCM_DRIVER s68fc01_fhdp_vdo_samsung_aod_lcm_drv = {
    .name = "s68fc01_fhdp_vdo_samsung_aod_lcm_drv",
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
