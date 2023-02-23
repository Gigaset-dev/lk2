/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2015. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#define LOG_TAG "LCM"

#include <debug.h>
#include <mt_i2c.h>
#include <platform.h>
#include "lcm_drv.h"


#include <gpio_api.h>
#include "lcm_util.h"

#include "../../../platform/mediatek/common/i2c/include/mt_i2c.h"


#define LCM_LOGI(string, args...)  dprintf(CRITICAL, "[LK/"LOG_TAG"]"string, ##args)
#define LCM_LOGD(string, args...)  dprintf(INFO, "[LK/"LOG_TAG"]"string, ##args)

static LCM_UTIL_FUNCS lcm_util;

#define SET_RESET_PIN(v, fdt)    (lcm_util.set_reset_pin(v, fdt))
#define MDELAY(n)        (lcm_util.mdelay(n))
#define UDELAY(n)        (lcm_util.udelay(n))

/* ------------------------------------------------------------------------ */
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


#define FRAME_WIDTH										(720)
#define FRAME_HEIGHT									(1600)

#define LCM_PHYSICAL_WIDTH									(64800)
#define LCM_PHYSICAL_HEIGHT									(129600)


#define REGFLAG_DELAY			0xFFFC
#define REGFLAG_UDELAY			0xFFFB
#define REGFLAG_END_OF_TABLE		0xFFFD
#define REGFLAG_RESET_LOW		0xFFFE
#define REGFLAG_RESET_HIGH		0xFFFF

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

//prize add by lipengpeng 20210420 start 
static void lcm_set_gpio_output(unsigned int GPIO, unsigned int output)
{
	mt_set_gpio_mode(GPIO, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO, (output>0)? GPIO_OUT_ONE: GPIO_OUT_ZERO);
}
//prize add by lipengpeng 20210420 start
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
	unsigned char para_list[120];
};

static struct LCM_setting_table lcm_suspend_setting[] = {
	{ 0xFF, 0x03, {0x98, 0x82, 0x00} },
	{0x28, 0, {} },
	{REGFLAG_DELAY, 20, {} },
	{0x10, 0, {} },
	{REGFLAG_DELAY, 120, {} }
};


static struct LCM_setting_table init_setting_vdo[] = {
{0xFF,0x03,{0x98,0x82,0x01}},  //3H 
{0x00,0x01,{0x47}},  //STVA    
{0x01,0x01,{0x32}},  //STVA Duty 4H
{0x02,0x01,{0x00}},  //45%   CLK duty 
{0x03,0x01,{0x00}},  //45%   CLK duty
{0x04,0x01,{0x04}},  //STVB
{0x05,0x01,{0x32}},  //STV Duty 4H
{0x06,0x01,{0x00}},  //45%   CLK duty
{0x07,0x01,{0x00}},  //45%   CLK duty
{0x08,0x01,{0x85}},  //CLK RISE    
{0x09,0x01,{0x04}},  //CLK FALL   
{0x0a,0x01,{0x72}}, //CLK Duty 4H 
{0x0b,0x01,{0x00}},
{0x0c,0x01,{0x00}},  //45%   CLK duty
{0x0d,0x01,{0x00}}, //45%   CLK duty   
{0x0e,0x01,{0x00}},
{0x0f,0x01,{0x00}},
{0x28,0x01,{0x48}},  //STCH1   
{0x29,0x01,{0x88}},
{0x2A,0x01,{0x48}},   //STCH2 
{0x2B,0x01,{0x88}},

//FW
{0x31,0x01,{0x0C}},     // RST_L
{0x32,0x01,{0x02}},     // VGL
{0x33,0x01,{0x02}},     // VGL
{0x34,0x01,{0x23}},     // GLV
{0x35,0x01,{0x02}},     // VGL_L
{0x36,0x01,{0x08}},     // STV1_L
{0x37,0x01,{0x0A}},     // STV2_L
{0x38,0x01,{0x06}},     // VDD
{0x39,0x01,{0x06}},     // VDD
{0x3A,0x01,{0x10}},     // CLK1_L
{0x3B,0x01,{0x10}},     // CLK1_L
{0x3C,0x01,{0x12}},     // CLK2_L
{0x3D,0x01,{0x12}},     // CLK2_L
{0x3E,0x01,{0x14}},     // CK1B_L
{0x3F,0x01,{0x14}},     // CK1B_L
{0x40,0x01,{0x16}},     // CK2B_L
{0x41,0x01,{0x16}},     // CK2B_L
{0x42,0x01,{0x07}},     // 
{0x43,0x01,{0x07}},     // 
{0x44,0x01,{0x07}},     // 
{0x45,0x01,{0x07}},     // 
{0x46,0x01,{0x07}},     // 
//FW
{0x47,0x01,{0x0D}},     // RST_R
{0x48,0x01,{0x02}},     // VGL
{0x49,0x01,{0x02}},     // VGL
{0x4A,0x01,{0x23}},     // GLV
{0x4B,0x01,{0x02}},     // VGL_R
{0x4C,0x01,{0x09}},     // STV1_R
{0x4D,0x01,{0x0B}},     // STV2_R
{0x4E,0x01,{0x06}},     // VDD
{0x4F,0x01,{0x06}},     // VDD
{0x50,0x01,{0x11}},     // CLK1_R
{0x51,0x01,{0x11}},     // CLK1_R
{0x52,0x01,{0x13}},     // CLK2_R
{0x53,0x01,{0x13}},     // CLK2_R
{0x54,0x01,{0x15}},     // CK1B_R
{0x55,0x01,{0x15}},     // CK1B_R
{0x56,0x01,{0x17}},     // CK2B_R
{0x57,0x01,{0x17}},     // CK2B_R
{0x58,0x01,{0x07}},     // 
{0x59,0x01,{0x07}},     // 
{0x5A,0x01,{0x07}},     // 
{0x5B,0x01,{0x07}},     // 
{0x5C,0x01,{0x07}},     // 
//BW
{0x61,0x01,{0x0C}},     // RST_L
{0x62,0x01,{0x02}},     // VGL
{0x63,0x01,{0x02}},    // VGL
{0x64,0x01,{0x23}},     // GLV
{0x65,0x01,{0x02}},     // VGL_L
{0x66,0x01,{0x08}},     // STV1_L
{0x67,0x01,{0x0A}},     // STV2_L
{0x68,0x01,{0x06}},     // VDD
{0x69,0x01,{0x06}},     // VDD
{0x6A,0x01,{0x10}},     // CLK1_L
{0x6B,0x01,{0x10}},     // CLK1_L
{0x6C,0x01,{0x12}},     // CLK2_L
{0x6D,0x01,{0x12}},     // CLK2_L
{0x6E,0x01,{0x14}},     // CK1B_L
{0x6F,0x01,{0x14}},     // CK1B_L
{0x70,0x01,{0x16}},     // CK2B_L
{0x71,0x01,{0x16}},     // CK2B_L
{0x72,0x01,{0x07}},     // 
{0x73,0x01,{0x07}},     // 
{0x74,0x01,{0x07}},     // 
{0x75,0x01,{0x07}},     // 
{0x76,0x01,{0x07}},     // 
//BW
{0x77,0x01,{0x0D}},     // RST_R
{0x78,0x01,{0x02}},     // VGL
{0x79,0x01,{0x02}},     // VGL
{0x7A,0x01,{0x23}},     // GLV
{0x7B,0x01,{0x02}},     // VGL_R
{0x7C,0x01,{0x09}},     // STV1_R
{0x7D,0x01,{0x0B}},     // STV2_R
{0x7E,0x01,{0x06}},     // VDD
{0x7F,0x01,{0x06}},     // VDD
{0x80,0x01,{0x11}},     // CLK1_R
{0x81,0x01,{0x11}},     // CLK1_R
{0x82,0x01,{0x13}},     // CLK2_R
{0x83,0x01,{0x13}},     // CLK2_R
{0x84,0x01,{0x15}},     // CK1B_R
{0x85,0x01,{0x15}},     // CK1B_R
{0x86,0x01,{0x17}},     // CK2B_R
{0x87,0x01,{0x17}},     // CK2B_R
{0x88,0x01,{0x07}},     // 
{0x89,0x01,{0x07}},     // 
{0x8A,0x01,{0x07}},     // 
{0x8B,0x01,{0x07}},     // 
{0x8C,0x01,{0x07}},     // 
{0xB0,0x01,{0x33}},
{0xB1,0x01,{0x33}},
{0xB2,0x01,{0x00}},
{0xD0,0x01,{0x01}},
{0xD1,0x01,{0x00}},
{0xE2,0x01,{0x00}},
{0xE6,0x01,{0x22}},
{0xE7,0x01,{0x54}},

// RTN. Internal VBP,0x Internal VFP
{0xFF,0x03,{0x98,0x82,0x02}},
{0xF1,0x01,{0x14}},    // Tcon ESD option  1C  20210107
{0x4B,0x01,{0x5A}},    // line_chopper
{0x50,0x01,{0xCA}},    // line_chopper
{0x51,0x01,{0x00}},     // line_chopper
{0x06,0x01,{0x8F}},     // Internal Line Time (RTN)
{0x0B,0x01,{0xA0}},     // Internal VFP[9]
{0x0C,0x01,{0x00}},     // Internal VFP[8]
{0x0D,0x01,{0x14}},     // Internal VBP
{0x0E,0x01,{0xE6}},     // Internal VFP
{0x4E,0x01,{0x11}},     // SRC BIAS
{0xF2,0x01,{0x4A}},    
// {0x4D,0x01,0xCE     // Power Saving Off

{0xFF,0x03,{0x98,0x82,0x03}},
{0x83,0x01,{0x20}},    // 
{0x84,0x01,{0x00}},

// Power Setting
{0xFF,0x03,{0x98,0x82,0x05}},
{0x03,0x01,{0x01}},    // Vcom
{0x04,0x01,{0x06}},   // Vcom
{0x58,0x01,{0x61}},    // VGL 2x
{0x63,0x01,{0x97}},     // GVDDN = -5.5V
{0x64,0x01,{0x97}},     // GVDDP = 5.5V
{0x68,0x01,{0xA1}},     // VGHO = 15V
{0x69,0x01,{0xA7}},     // VGH = 16V
{0x6A,0x01,{0x79}},     // VGLO = -10V
{0x6B,0x01,{0x6B}},     // VGL = -11V
{0x85,0x01,{0x37}},      // HW RESET option
{0x46,0x01,{0x00}},      // LVD HVREG option

// Resolution
{0xFF,0x03,{0x98,0x82,0x06}},
{0xD9,0x01,{0x1F}},     // 4Lane
// {0x08,0x01,0x00     // PLL
{0xC0,0x01,{0x40}},     // NL = 1600
{0xC1,0x01,{0x16}},     // NL = 1600
{0x80,0x01,{0x08}},      // mipi_cmd_toggle_en (9882Q=08 / 9882K=09) 
{0xB6,0x01,{0x30}},   

// Gamma Register
{0xFF,0x03,{0x98,0x82,0x08}},								
//{0xE0,0x1B,{0x40,0x24,0xD4,0xFF,0x32,0x55,0x5D,0x80,0xA7,0xC9,0xAA,0x00,0x2E,0x58,0x80,0xEA,0xA9,0xD8,0xF4,0x16,0xFF,0x32,0x55,0x7F,0xA1,0x03,0xEC}},								
//{0xE1,0x1B,{0x40,0x24,0xD4,0xFF,0x32,0x55,0x5D,0x80,0xA7,0xC9,0xAA,0x00,0x2E,0x58,0x80,0xEA,0xA9,0xD8,0xF4,0x16,0xFF,0x32,0x55,0x7F,0xA1,0x03,0xEC}},	

{0xE0,0x1B,{0x40,0xA0,0xB4,0xE5,0x22,0x55,0x54,0x7A,0xA6,0xCB,0xAA,0x03,0x31,0x5A,0x82,0xEA,0xAA,0xDA,0xF7,0x1B,0xFF,0x3B,0x62,0x91,0xBA,0x03,0xEC}},								
{0xE0,0x1B,{0x40,0xA0,0xB4,0xE5,0x22,0x55,0x54,0x7A,0xA6,0xCB,0xAA,0x03,0x31,0x5A,0x82,0xEA,0xAA,0xDA,0xF7,0x1B,0xFF,0x3B,0x62,0x91,0xBA,0x03,0xEC}},	
							
// OSC Auto Trim Setting
{0xFF,0x03,{0x98,0x82,0x0B}},
{0x9A,0x01,{0x89}},
{0x9B,0x01,{0x03}},
{0x9C,0x01,{0x06}},
{0x9D,0x01,{0x06}},
{0x9E,0x01,{0xE1}},
{0x9F,0x01,{0xE1}},
{0xAA,0x01,{0x22}},
{0xAB,0x01,{0xE0}},     // AutoTrimType
{0xAC,0x01,{0x7F}},     // trim_osc_max
{0xAD,0x01,{0x3F}},     // trim_osc_min

{0xFF,0x03,{0x98,0x82,0x0E}},
{0x11,0x01,{0x10}},     // TSVD Rise position
{0x13,0x01,{0x10}},     // LV mode TSHD Rise position
{0x00,0x01,{0xA0}},      // LV mode

{0xFF,0x03,{0x98,0x82,0x00}},
{0x35,0x1,{0x00}},
{0x11,0x00,{0x00}},
{REGFLAG_DELAY,120,{}},
{0x29,0x00,{0x00}},
{REGFLAG_DELAY,20,{}},
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
	params->physical_width = LCM_PHYSICAL_WIDTH/1000;
	params->physical_height = LCM_PHYSICAL_HEIGHT/1000;


	params->dsi.mode = SYNC_PULSE_VDO_MODE;
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
	params->dsi.vertical_sync_active                = 4;
	params->dsi.vertical_backporch                  = 16;
	params->dsi.vertical_frontporch                 = 230;
	params->dsi.vertical_active_line                = FRAME_HEIGHT;
	params->dsi.horizontal_sync_active              = 8;
	params->dsi.horizontal_backporch                = 18;
	params->dsi.horizontal_frontporch               = 20;
	params->dsi.horizontal_active_pixel             = FRAME_WIDTH;
	/*prize-zhaopengge modify lcd fps-20201012-start*/
    params->dsi.PLL_CLOCK = 268; //512 535 258
	params->dsi.cont_clock = 0;
	params->dsi.clk_lp_per_line_enable = 0;
	params->dsi.ssc_disable = 1;	
	
	params->dsi.esd_check_enable                    = 1;
	params->dsi.customization_esd_check_enable      = 1;
	params->dsi.lcm_esd_check_table[0].cmd = 0x0A;
	params->dsi.lcm_esd_check_table[0].count = 1;
	params->dsi.lcm_esd_check_table[0].para_list[0] = 0x9C;
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

	/* for VFP resolution, do golden settings of high frame rate*/
	//params->dsi.vact_fps = 90;
}

static void lcm_init_power(void *fdt)
{
    SET_RESET_PIN(0, fdt);
    MDELAY(100);
	//lcm_set_gpio_output(118, GPIO_OUT_ONE);
	//MDELAY(10);
	//lcm_set_gpio_output(119, GPIO_OUT_ONE);
    if (lcm_util.set_gpio_lcd_enp_bias) {
        lcm_util.set_gpio_lcd_enp_bias(1, fdt);
        _lcm_i2c_write_bytes(0x00, 0x12,fdt); //5.8V
		MDELAY(1);
        _lcm_i2c_write_bytes(0x01, 0x12,fdt); //5.8V
		MDELAY(1);
        LCM_LOGI("lcm_init_power %s\n", __func__);
    } else
        LCM_LOGI("set_gpio_lcd_enp_bias not defined\n");
	MDELAY(5);
    lcm_set_gpio_output(150, GPIO_OUT_ONE);
}

static void lcm_suspend_power(void)
{
}

static void lcm_resume_power(void)
{
}

static void lcm_init(void *fdt)
{
	//prize
	//mt_set_gpio_mode(SPI_MI, GPIO_MODE_01);
	//mt_set_gpio_mode(SPI_MO, GPIO_MODE_01);
    //prize
	//SET_RESET_PIN(0);

	MDELAY(5);
	SET_RESET_PIN(1, fdt);
	MDELAY(10);
	SET_RESET_PIN(0, fdt);
	MDELAY(10);

	SET_RESET_PIN(1, fdt);
	MDELAY(20);//ili9882q at least 10ms

	push_table(init_setting_vdo, sizeof(init_setting_vdo) /
		   sizeof(struct LCM_setting_table), 1);
}


static void lcm_suspend(void)
{
}

static void lcm_resume(void)
{
//	lcm_init();
}

static void lcm_update(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{

}
#if 0
/*prize-Solve the LCD td4321 compatible-anhengxuan-20201112-start*/
extern int IMM_GetOneChannelValue(int dwChannel, int data[4], int *rawdata);
static unsigned int lcm_compare_id_tt(void)
{
    unsigned int rawdata=0;
    int data[4] = {0, 0, 0, 0};    
    int adc_value=0;
	IMM_GetOneChannelValue(2,data,&rawdata);
	adc_value=data[0]*1000+data[1]*10;
#ifdef BUILD_LK
    LCM_LOGI("LCD_ID_td4321 adc_value = %d\n",adc_value);
#else
    LCM_LOGI("LCM_ID_td4321 adc_value = %d\n",adc_value);
#endif
	if ((adc_value>800)&&(adc_value<1200) )
	{
		return 1;
	}
	else
    {
		return 0;
    }
}
/*prize-Solve the LCD td4321 compatible-anhengxuan-20201112-end*/
#endif
static unsigned int lcm_compare_id(void *fdt)
{
	int array[4];
    char buffer[4]={0,0,0,0};
	char buffer1[4]={0,0,0,0};
	char buffer2[4]={0,0,0,0};
    unsigned int id=0;
	//int rawdata = 0;
	int ret = 0;
	
	dprintf(CRITICAL, "%s--------------------\n", __func__);

	struct LCM_setting_table lcm_id[] = {
		{ 0xFF, 0x03, {0x98, 0x82, 0x06} }
	};
//	struct LCM_setting_table switch_table_page0[] = {
//		{ 0xFF, 0x03, {0x98, 0x81, 0x00} }
//	};
/*	
    display_ldo18_enable(1);
    display_bias_vpos_enable(1);
    display_bias_vneg_enable(1);
    MDELAY(10);
    display_bias_vpos_set(5800);
	display_bias_vneg_set(5800);
*/
	//display_ldo18_enable(1);
	MDELAY(10);
    SET_RESET_PIN(1, fdt);
	MDELAY(10);
    SET_RESET_PIN(0, fdt);
    MDELAY(20);
    SET_RESET_PIN(1, fdt);
    MDELAY(120);

	push_table(lcm_id, sizeof(lcm_id) / sizeof(struct LCM_setting_table), 1);
	 MDELAY(5);
	    array[0]=0x00043902;
        array[1]=0x068298ff;
        dsi_set_cmdq(array, 2, 1);
	MDELAY(5);
	array[0]=0x00023700;
	dsi_set_cmdq(array, 1, 1);
    read_reg_v2(0xF0,&buffer[0], 1);
    MDELAY(5);
    read_reg_v2(0xF1,&buffer[1], 1);
    MDELAY(5);
    read_reg_v2(0xF2,&buffer[2], 1);

    id = ( (buffer[0] << 16)|(buffer[1] << 8)| (buffer[2]) ); //
	
#ifdef BUILD_LK
	dprintf(CRITICAL, "%s, LK debug: ili9882Q id = 0x%08x\n", __func__, id);
#else
	LCM_LOGI("%s: ili9882 id = 0x%d \n", __func__, id);
#endif
	//lcm_compare_id_tt();
	//return 1;
	
    if(0x988210 == id)
	{
		//is_9882N_detect=1;
	    return 1;
	}
    else{
		return 0;
	}

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

	LCM_LOGI("ATA check size = 0x%x,0x%x,0x%x,0x%x\n",
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

LCM_DRIVER ili9882q_hdp_dsi_vdo_truly6517_lcm_drv = {
	.name = "ili9882q_hdp_dsi_vdo_truly6517_drv",
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
	//.set_backlight = lcm_setbacklight_cmdq,
	.ata_check = lcm_ata_check,
	.update = lcm_update,
};

