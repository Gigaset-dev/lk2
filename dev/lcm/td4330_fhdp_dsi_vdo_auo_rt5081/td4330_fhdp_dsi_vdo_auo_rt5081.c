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

#include <gpio_api.h>

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


#define FRAME_WIDTH            (720)
#define FRAME_HEIGHT            (1560)
#define HFP (48)
#define HSA (4)
#define HBP (48)
#define VFP (150)
#define VSA (4)
#define VBP (12)

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

#define GPIO_LCD_BIAS_ENP (GPIO108)  //prize add 
//#endif
//#ifndef GPIO_LCD_BIAS_ENN_PIN
#define GPIO_LCD_BIAS_ENN (GPIO109)//prize add 


#define GPIO_65132_ENP  GPIO_LCD_BIAS_ENP
#define GPIO_65132_ENN  GPIO_LCD_BIAS_ENN
/* i2c control start */

//#define LCM_I2C_MODE    ST_MODE
//#define LCM_I2C_SPEED   100
//#define LCM_ID_TD4330  0x02

#define LCM_I2C_ID   	6
#define LCM_I2C_ADDR    0x3e

static int _lcm_i2c_write_bytes(u8 addr, u8 value)
{
    s32 ret = 0;
    u8 write_data[2];
    u8 len;
    int i2c_id, i2c_addr;

    i2c_id = LCM_I2C_ID;
    i2c_addr = LCM_I2C_ADDR;

 
    write_data[0] = addr;
    write_data[1] = value;
	
    len = 2;

    ret = mtk_i2c_write((u8)i2c_id, (u8)i2c_addr, write_data, len, NULL);
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

{0x0A,1,{0x08}},
{0xF0,2,{0x5A,0x59}},
{0xF1,2,{0xA5,0xA6}},
{0xB0,30,{0x82,0x81,0x05,0x04,0x87,0x86,0x84,0x85,0x66,0x66,0x33,0x33,0x20,0x01,0x01,0x78,0x01,0x01,0x0F,0x05,0x04,0x03,0x02,0x01,0x02,0x03,0x04,0x00,0x00,0x00}},
{0xB1,29,{0x11,0x44,0x86,0x00,0x01,0x00,0x01,0x7C,0x01,0x01,0x04,0x08,0x54,0x00,0x00,0x00,0x44,0x40,0x02,0x01,0x40,0x02,0x01,0x40,0x02,0x01,0x40,0x02,0x01}},
{0xB2,17,{0x54,0xC4,0x82,0x05,0x40,0x02,0x01,0x40,0x02,0x01,0x05,0x05,0x54,0x0C,0x0C,0x0D,0x0B}},
{0xB3,31,{0x02,0x00,0x00,0x00,0x00,0x26,0x26,0x91,0xA2,0x33,0x44,0x00,0x26,0x00,0x18,0x01,0x02,0x08,0x20,0x30,0x08,0x09,0x44,0x20,0x40,0x20,0x40,0x08,0x09,0x22,0x33}},
{0xB4,28,{0x0A,0x02,0xDC,0x1D,0x00,0x02,0x02,0x02,0x02,0x12,0x10,0x02,0x02,0x0E,0x0C,0x04,0x03,0x03,0x03,0x03,0x03,0x03,0xFF,0xFF,0xFC,0x00,0x00,0x00}},
{0xB5,18,{0x0B,0x02,0xDC,0x1D,0x00,0x02,0x02,0x02,0x02,0x13,0x11,0x02,0x02,0x0F,0x0D,0x05,0x03,0x03,0x03,0x03,0x03,0x03,0xFF,0xFF,0xFC,0x00,0x00,0x00}},
{0xB8,24,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
{0xBB,13,{0x01,0x05,0x09,0x11,0x0D,0x19,0x1D,0x55,0x25,0x69,0x00,0x21,0x25}},
{0xBC,14,{0x00,0x00,0x00,0x00,0x02,0x20,0xFF,0x00,0x03,0x33,0x01,0x73,0x33,0x00}},
{0xBD,10,{0xE9,0x02,0x4F,0xCF,0x72,0xA4,0x08,0x44,0xAE,0x15}},
{0xBE,10,{0x65,0x65,0x50,0x46,0x0C,0x77,0x43,0x07,0x0E,0x0E}},
{0xBF,8,{0x07,0x25,0x07,0x25,0x7F,0x00,0x11,0x04}},
{0xC0,9,{0x10,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0xFF,0x00}},
{0xC1,19,{0xC0,0x0C,0x20,0x96,0x04,0x30,0x30,0x04,0x2A,0x18,0x36,0x00,0x07,0xCF,0xFF,0xFF,0xC0,0x00,0xC0}},
{0xC2,1,{0x00}},
{0xC3,9,{0x06,0x00,0xFF,0x00,0xFF,0x00,0x00,0x81,0x01}},
{0xC4,10,{0x84,0x01,0x2B,0x41,0x00,0x3C,0x00,0x03,0x03,0x2E}},
{0xC5,11,{0x03,0x1C,0xC0,0xB8,0x50,0x10,0x64,0x44,0x08,0x09,0x26}},
{0xC6,10,{0x87,0x9A,0x2A,0x29,0x29,0x33,0x7F,0x04,0x08,0x00}},
{0xC7,22,{0xF7,0xA8,0x81,0x65,0x37,0x18,0xEA,0x43,0x14,0xF0,0xCA,0x9F,0xF9,0xCF,0xB2,0x88,0x71,0x4E,0x1A,0x7E,0xC0,0x00}},
{0xC8,22,{0xF7,0xA8,0x81,0x65,0x37,0x18,0xEA,0x43,0x14,0xF0,0xCA,0x9F,0xF9,0xCF,0xB2,0x88,0x71,0x4E,0x1A,0x7E,0xC0,0x00}},
{0xCB,1,{0x00}},
{0xD0,5,{0x80,0x0D,0xFF,0x0F,0x61}},
{0xD2,1,{0x42}},
{0xFE,4,{0xFF,0xFF,0xFF,0x40}},
{0xF1,2,{0x5A,0x59}},
{0xF0,2,{0xA5,0xA6}},
{0x35,1,{0x00}},
{0x11,0,{}},
{REGFLAG_DELAY,120,{}},
{0x29,0,{}},
{REGFLAG_DELAY,20,{}},
{0x26,1,{0x01}},

};


static void lcm_set_bias(void)
{
	dprintf(0,"lcm_set_bias===========================\n");
	
	mt_set_gpio_mode(GPIO_65132_ENP, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO_65132_ENP, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_65132_ENP, GPIO_OUT_ONE);
	
	MDELAY(30);
	
	mt_set_gpio_mode(GPIO_65132_ENN, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO_65132_ENN, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_65132_ENN, GPIO_OUT_ONE);

	MDELAY(50);
	
}


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
  //  params->dsi.switch_mode = CMD_MODE;
 //   params->dsi.switch_mode_enable = 0;

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

    params->dsi.vertical_sync_active = VSA;
    params->dsi.vertical_backporch = VBP;
    params->dsi.vertical_frontporch = VFP;
  //  params->dsi.vertical_frontporch_for_low_power = 750;
    params->dsi.vertical_active_line = FRAME_HEIGHT;

    params->dsi.horizontal_sync_active = HSA;
    params->dsi.horizontal_backporch = HBP;
    params->dsi.horizontal_frontporch = HFP;
    params->dsi.horizontal_active_pixel = FRAME_WIDTH;
    params->dsi.ssc_disable = 1;
#ifndef MACH_FPGA
    /* this value must be in MTK suggested table */
    params->dsi.PLL_CLOCK = 278;
#else
    params->dsi.pll_div1 = 0;
    params->dsi.pll_div2 = 0;
    params->dsi.fbk_div = 0x1;
#endif
    params->dsi.clk_lp_per_line_enable = 0;
    params->dsi.esd_check_enable = 0;
    params->dsi.customization_esd_check_enable = 0;
    params->dsi.lcm_esd_check_table[0].cmd = 0x0a;
    params->dsi.lcm_esd_check_table[0].count = 1;
    params->dsi.lcm_esd_check_table[0].para_list[0] = 0x9c;
}

/* turn on gate ic & control voltage to 5.5V */
static void lcm_init_power(void *fdt)
{
	LCM_LOGI("gezi %s,td4320 \n", __func__);
	
	lcm_set_bias();
	
	_lcm_i2c_write_bytes(0x0, 0x14);
    _lcm_i2c_write_bytes(0x1, 0x14);
	
    if (lcm_util.set_gpio_lcd_enp_bias) {
		LCM_LOGI(" 111 gezi %s,td4320 \n", __func__);
        lcm_util.set_gpio_lcd_enp_bias(1, fdt);
       // _lcm_i2c_write_bytes(0x0, 0xf, fdt);
       // _lcm_i2c_write_bytes(0x1, 0xf, fdt);
    } else
        LCM_LOGI("gezi set_gpio_lcd_enp_bias not defined\n");
	
	
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
	LCM_LOGI("gezi %s,td4320 \n", __func__);
	//lcm_set_bias(1);
   // SET_RESET_PIN(0, fdt);
    SET_RESET_PIN(1, fdt);
    MDELAY(10);
    SET_RESET_PIN(0, fdt);
    MDELAY(10);
    SET_RESET_PIN(1, fdt);
    MDELAY(5);

    push_table(init_setting_vdo, sizeof(init_setting_vdo) /
           sizeof(struct LCM_setting_table), 1);

}
/*
#ifdef LCM_SET_DISPLAY_ON_DELAY
static int lcm_set_display_on(void)
{
    u32 timeout, i = 0;

    if (is_display_on)
        return 0;

    
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
*/
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
    //lcm_init();
}
/*
#if 1
static void lcm_update(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
#ifdef LCM_SET_DISPLAY_ON_DELAY
    lcm_set_display_on();
#endif
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
    data_array[0] = 0x0005390A; 
    data_array[1] = (x1_MSB << 24) | (x0_LSB << 16) | (x0_MSB << 8) | 0x2a;
    data_array[2] = (x1_LSB);
    dsi_set_cmdq(data_array, 3, 1);

    data_array[0] = 0x00043700; 
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

    data_array[0] = 0x0005390A; 
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
    LCM_LOGI("%s,td4320 backlight: level = %d\n", __func__, level);

    bl_level[0].para_list[0] = level;

    push_table(bl_level, sizeof(bl_level) /
           sizeof(struct LCM_setting_table), 1);
}

static void lcm_setbacklight(unsigned int level)
{
    LCM_LOGI("%s,td4320 backlight: level = %d\n", __func__, level);

    bl_level[0].para_list[0] = level;

    push_table(bl_level, sizeof(bl_level) /
           sizeof(struct LCM_setting_table), 1);
}

static unsigned int lcm_compare_id(void *fdt)
{
    unsigned int id = 0;
    unsigned char buffer[1];
    unsigned int array[16];

    SET_RESET_PIN(1, fdt);
    SET_RESET_PIN(0, fdt);
    MDELAY(1);

    SET_RESET_PIN(1, fdt);
    MDELAY(20);

    array[0] = 0x00013700;  
    dsi_set_cmdq(array, 1, 1);

    read_reg_v2(0xBF, buffer, 1);
    id = buffer[0];

    LCM_LOGI("%s,td4330 id = 0x%08x\n", __func__, id);

    if (id == LCM_ID_TD4330)
        return 1;
    else
        return 0;

}
*/

LCM_DRIVER td4330_fhdp_dsi_vdo_auo_rt5081_lcm_drv = {
    .name = "td4330_fhdp_dsi_vdo_auo_rt5081_drv",
    .set_util_funcs = lcm_set_util_funcs,
    .get_params = lcm_get_params,
    .init = lcm_init,
    .suspend = lcm_suspend,
    .resume = lcm_resume,
    .init_power = lcm_init_power,
    //.compare_id = lcm_compare_id,
    .resume_power = lcm_resume_power,
    .suspend_power = lcm_suspend_power,
    //.set_backlight = lcm_setbacklight,
    //.ata_check = lcm_ata_check,
    //.update = lcm_update,
};

