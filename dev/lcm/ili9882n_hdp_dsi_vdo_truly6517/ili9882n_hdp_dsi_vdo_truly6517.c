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
#if 1
{0xFF,03,{0x98,0x82,0x01}},  //3H 
{0x00,01,{0x47}},  //STVA    
{0x01,01,{0x32}},  //STVA Duty 4H
{0x02,01,{0x00}},  //45%   CLK duty 
{0x03,01,{0x00}},  //45%   CLK duty
{0x04,01,{0x04}},  //STVB
{0x05,01,{0x32}},  //STV Duty 4H
{0x06,01,{0x00}},  //45%   CLK duty
{0x07,01,{0x00}},  //45%   CLK duty
{0x08,01,{0x85}},  //CLK RISE    
{0x09,01,{0x04}},  //CLK FALL   
{0x0a,01,{0x72}}, //CLK Duty 4H 
{0x0b,01,{0x00}},
{0x0c,01,{0x00}},  //45%   CLK duty
{0x0d,01,{0x00}}, //45%   CLK duty   
{0x0e,01,{0x00}},
{0x0f,01,{0x00}},
{0x28,01,{0x48}},  //STCH1   
{0x29,01,{0x88}},
{0x2A,01,{0x48}},   //STCH2 
{0x2B,01,{0x88}},

//FW
{0x31,01,{0x0C}},     // RST_L
{0x32,01,{0x02}},     // VGL
{0x33,01,{0x02}},     // VGL
{0x34,01,{0x23}},     // GLV
{0x35,01,{0x02}},     // VGL_L
{0x36,01,{0x08}},     // STV1_L
{0x37,01,{0x0A}},     // STV2_L
{0x38,01,{0x06}},    // VDD
{0x39,01,{0x06}},     // VDD
{0x3A,01,{0x10}},     // CLK1_L
{0x3B,01,{0x10}},     // CLK1_L
{0x3C,01,{0x12}},     // CLK2_L
{0x3D,01,{0x12}},     // CLK2_L
{0x3E,01,{0x14}},     // CK1B_L
{0x3F,01,{0x14}},     // CK1B_L
{0x40,01,{0x16}},     // CK2B_L
{0x41,01,{0x16}},     // CK2B_L
{0x42,01,{0x07}},    // 
{0x43,01,{0x07}},     // 
{0x44,01,{0x07}},     // 
{0x45,01,{0x07}},     // 
{0x46,01,{0x07}},     // 
//FW
{0x47,01,{0x0D}},     // RST_R
{0x48,01,{0x02}},     // VGL
{0x49,01,{0x02}},     // VGL
{0x4A,01,{0x23}},     // GLV
{0x4B,01,{0x02}},     // VGL_R
{0x4C,01,{0x09}},     // STV1_R
{0x4D,01,{0x0B}},     // STV2_R
{0x4E,01,{0x06}},     // VDD
{0x4F,01,{0x06}},     // VDD
{0x50,01,{0x11}},     // CLK1_R
{0x51,01,{0x11}},     // CLK1_R
{0x52,01,{0x13}},    // CLK2_R
{0x53,01,{0x13}},     // CLK2_R
{0x54,01,{0x15}},     // CK1B_R
{0x55,01,{0x15}},     // CK1B_R
{0x56,01,{0x17}},     // CK2B_R
{0x57,01,{0x17}},     // CK2B_R
{0x58,01,{0x07}},     // 
{0x59,01,{0x07}},     // 
{0x5A,01,{0x07}},     // 
{0x5B,01,{0x07}},     // 
{0x5C,01,{0x07}},     // 
//BW
{0x61,01,{0x0C}},     // RST_L
{0x62,01,{0x02}},     // VGL
{0x63,01,{0x02}},     // VGL
{0x64,01,{0x23}},    // GLV
{0x65,01,{0x02}},     // VGL_L
{0x66,01,{0x08}},     // STV1_L
{0x67,01,{0x0A}},     // STV2_L
{0x68,01,{0x06}},     // VDD
{0x69,01,{0x06}},     // VDD
{0x6A,01,{0x10}},     // CLK1_L
{0x6B,01,{0x10}},     // CLK1_L
{0x6C,01,{0x12}},     // CLK2_L
{0x6D,01,{0x12}},     // CLK2_L
{0x6E,01,{0x14}},     // CK1B_L
{0x6F,01,{0x14}},     // CK1B_L
{0x70,01,{0x16}},     // CK2B_L
{0x71,01,{0x16}},     // CK2B_L
{0x72,01,{0x07}},     // 
{0x73,01,{0x07}},     // 
{0x74,01,{0x07}},     // 
{0x75,01,{0x07}},     // 
{0x76,01,{0x07}},     // 
//BW
{0x77,01,{0x0D}},     // RST_R
{0x78,01,{0x02}},     // VGL
{0x79,01,{0x02}},     // VGL
{0x7A,01,{0x23}},     // GLV
{0x7B,01,{0x02}},     // VGL_R
{0x7C,01,{0x09}},     // STV1_R
{0x7D,01,{0x0B}},     // STV2_R
{0x7E,01,{0x06}},     // VDD
{0x7F,01,{0x06}},     // VDD
{0x80,01,{0x11}},     // CLK1_R
{0x81,01,{0x11}},     // CLK1_R
{0x82,01,{0x13}},     // CLK2_R
{0x83,01,{0x13}},     // CLK2_R
{0x84,01,{0x15}},     // CK1B_R
{0x85,01,{0x15}},     // CK1B_R
{0x86,01,{0x17}},     // CK2B_R
{0x87,01,{0x17}},     // CK2B_R
{0x88,01,{0x07}},     // 
{0x89,01,{0x07}},     // 
{0x8A,01,{0x07}},     // 
{0x8B,01,{0x07}},     // 
{0x8C,01,{0x07}},     // 
{0xB0,01,{0x33}},
{0xB1,01,{0x33}},
{0xB2,01,{0x00}},
{0xD0,01,{0x01}},
{0xD1,01,{0x00}},
{0xE2,01,{0x00}},
{0xE6,01,{0x22}},
{0xE7,01,{0x54}},

// RTN. Internal VBP, Internal VFP
{0xFF,03,{0x98,0x82,0x02}},
{0xF1,01,{0x1C}},    // Tcon ESD option
{0x4B,01,{0x5A}},    // line_chopper
{0x50,01,{0xCA}},    // line_chopper
{0x51,01,{0x00}},     // line_chopper
{0x06,01,{0x8F}},     // Internal Line Time (RTN)
{0x0B,01,{0xA0}},     // Internal VFP[9]
{0x0C,01,{0x00}},     // Internal VFP[8]
{0x0D,01,{0x14}},     // Internal VBP
{0x0E,01,{0xE6}},     // Internal VFP
{0x4E,01,{0x11}},     // SRC BIAS
// REGISTER,4D,01,CE     // Power Saving Off

// Power Setting
{0xFF,03,{0x98,0x82,0x05}},
{0x03,01,{0x01}},    // Vcom
{0x04,01,{0x2C}},    // Vcom
{0x19,01,{0xB9}},    // Vcom
{0x58,01,{0x61}},    // VGL 2x
{0x63,01,{0x8D}},     // GVDDN = -5.3V
{0x64,01,{0x8D}},     // GVDDP = 5.3V
{0x68,01,{0xA1}},     // VGHO = 15V
{0x69,01,{0xA7}},     // VGH = 16V
{0x6A,01,{0x79}},     // VGLO = -10V
{0x6B,01,{0x6B}},     // VGL = -11V
{0x85,01,{0x37}},      // HW RESET option
{0x46,01,{0x00}},      // LVD HVREG option

// Resolution
{0xFF,03,{0x98,0x82,0x06}},
{0xD9,01,{0x1F}},     // 4Lane
// REGISTER,08,01,00     // PLL
{0xC0,01,{0x40}},     // NL = 1600
{0xC1,01,{0x16}},     // NL = 1600

// Gamma Register
{0xFF,03,{0x98,0x82,0x08}},								
{0xE0,27,{0x00,0xA0,0x78,0xAD,0xEF,0x55,0x23,0x4B,0x7B,0xA1,0xA9,0xDE,0x10,0x3D,0x67,0xEA,0x93,0xC6,0xE5,0x0B,0xFF,0x2C,0x55,0x86,0xB3,0x03,0xEC}},							
{0xE1,27,{0x00,0x24,0x78,0xAD,0xEF,0x55,0x23,0x4B,0x7B,0xA1,0xA9,0xDE,0x10,0x3D,0x67,0xEA,0x93,0xC6,0xE5,0x0B,0xFF,0x2C,0x55,0x86,0xB3,0x03,0xEC}},						

// OSC Auto Trim Setting
{0xFF,03,{0x98,0x82,0x0B}},
{0x9A,01,{0x44}},
{0x9B,01,{0x81}},
{0x9C,01,{0x03}},
{0x9D,01,{0x03}},
{0x9E,01,{0x70}},
{0x9F,01,{0x70}},
{0xAB,01,{0xE0}},     // AutoTrimType

{0xFF,03,{0x98,0x82,0x0E}},
{0x11,01,{0x10}},     // TSVD Rise position
{0x13,01,{0x10}},     // LV mode TSHD Rise position
{0x00,01,{0xA0}},      // LV mode

{0xFF,03,{0x98,0x82,0x00}},
{0x35,1,{0x00}},
#else
{0xFF,3,{0x98,0x82,0x01}},
{0x00,1,{0x40}},
{0x01,1,{0x00}},
{0x02,1,{0x10}},
{0x03,1,{0x00}},
{0x08,1,{0x01}},
{0x09,1,{0x05}},
{0x0a,1,{0x40}},
{0x0b,1,{0x00}},
{0x0c,1,{0x10}},
{0x0d,1,{0x10}},
{0x0e,1,{0x00}},
{0x0f,1,{0x00}},
{0x10,1,{0x00}},
{0x11,1,{0x00}},
{0x12,1,{0x00}},
{0x31,1,{0x07}},
{0x32,1,{0x07}},
{0x33,1,{0x07}},
{0x34,1,{0x02}},
{0x35,1,{0x02}},
{0x36,1,{0x02}},
{0x37,1,{0x02}},
{0x38,1,{0x2B}},
{0x39,1,{0x2A}},
{0x3A,1,{0x14}},
{0x3B,1,{0x13}},
{0x3C,1,{0x12}},
{0x3D,1,{0x11}},
{0x3E,1,{0x10}},
{0x3F,1,{0x08}},
{0x40,1,{0x07}},
{0x41,1,{0x07}},
{0x42,1,{0x07}},
{0x43,1,{0x02}},
{0x44,1,{0x02}},
{0x45,1,{0x06}},
{0x46,1,{0x06}},
{0x47,1,{0x07}},
{0x48,1,{0x07}},
{0x49,1,{0x07}},
{0x4A,1,{0x02}},
{0x4B,1,{0x02}},
{0x4C,1,{0x02}},
{0x4D,1,{0x02}},
{0x4E,1,{0x2B}},
{0x4F,1,{0x2A}},
{0x50,1,{0x14}},
{0x51,1,{0x13}},
{0x52,1,{0x12}},
{0x53,1,{0x11}},
{0x54,1,{0x10}},
{0x55,1,{0x08}},
{0x56,1,{0x07}},
{0x57,1,{0x07}},
{0x58,1,{0x07}},
{0x59,1,{0x02}},
{0x5A,1,{0x02}},
{0x5B,1,{0x06}},
{0x5C,1,{0x06}},
{0x61,1,{0x07}},
{0x62,1,{0x07}},
{0x63,1,{0x07}},
{0x64,1,{0x02}},
{0x65,1,{0x02}},
{0x66,1,{0x02}},
{0x67,1,{0x02}},
{0x68,1,{0x2B}},
{0x69,1,{0x2A}},
{0x6A,1,{0x10}},
{0x6B,1,{0x11}},
{0x6C,1,{0x12}},
{0x6D,1,{0x13}},
{0x6E,1,{0x14}},
{0x6F,1,{0x08}},
{0x70,1,{0x07}},
{0x71,1,{0x07}},
{0x72,1,{0x07}},
{0x73,1,{0x02}},
{0x74,1,{0x02}},
{0x75,1,{0x06}},
{0x76,1,{0x06}},
{0x77,1,{0x07}},
{0x78,1,{0x07}},
{0x79,1,{0x07}},
{0x7A,1,{0x02}},
{0x7B,1,{0x02}},
{0x7C,1,{0x02}},
{0x7D,1,{0x02}},
{0x7E,1,{0x2B}},
{0x7F,1,{0x2A}},
{0x80,1,{0x10}},
{0x81,1,{0x11}},
{0x82,1,{0x12}},
{0x83,1,{0x13}},
{0x84,1,{0x14}},
{0x85,1,{0x08}},
{0x86,1,{0x07}},
{0x87,1,{0x07}},
{0x88,1,{0x07}},
{0x89,1,{0x02}},
{0x8A,1,{0x02}},
{0x8B,1,{0x06}},
{0x8C,1,{0x06}},
{0xb0,1,{0x33}},
{0xb1,1,{0x33}},
{0xb2,1,{0x00}},
{0xc3,1,{0xff}},
{0xca,1,{0x44}},
{0xd0,1,{0x01}},
{0xd1,1,{0x12}},
{0xd2,1,{0x10}},
{0xd5,1,{0x55}},
{0xd8,1,{0x01}},
{0xd9,1,{0x02}},
{0xda,1,{0x80}},
{0xdc,1,{0x10}},
{0xdd,1,{0x40}},
{0xdf,1,{0xb6}},
{0xe2,1,{0x75}},
{0xe6,1,{0x22}},
{0xe7,1,{0x54}},
{0xFF,3,{0x98,0x82,0x02}},
{0xF1,1,{0x1C}},
{0x4B,1,{0x5A}},
{0x50,1,{0xCA}},
{0x51,1,{0x00}},
{0x06,1,{0x8F}},
{0x0B,1,{0xA0}},
{0x0C,1,{0x00}},
{0x0D,1,{0x0E}},
{0x0E,1,{0xE6}},
{0x4E,1,{0x11}},
{0xFF,3,{0x98,0x82,0x05}},
{0x03,1,{0x01}},
{0x04,1,{0x17}},
{0x50,1,{0x0F}},
{0x58,1,{0x61}},
{0x63,1,{0x9C}},
{0x64,1,{0x9C}},
{0x68,1,{0x29}},
{0x69,1,{0x2F}},
{0x6A,1,{0x3D}},
{0x6B,1,{0x2F}},
{0x85,1,{0x37}},
{0x46,1,{0x00}},
{0xFF,3,{0x98,0x82,0x06}},
{0xD9,1,{0x1F}},
{0xC0,1,{0x40}},
{0xC1,1,{0x16}},
{0x48,1,{0x0F}},
{0x4D,1,{0x80}},
{0x4E,1,{0x40}},
{0x83,1,{0x00}},
{0x84,1,{0x00}},
{0xC7,1,{0x05}},
{0xFF,3,{0x98,0x82,0x08}},
{0xE0,27,{0x40,0x24,0xAE,0xE9,0x2C,0x55,0x5E,0x85,0xB2,0xD5,0xAA,0x0B,0x35,0x5B,0x80,0xEA,0xA6,0xD4,0xF0,0x14,0xFF,0x31,0x57,0x85,0xAB,0x03,0xEC}},
{0xE1,27,{0x40,0x24,0xAE,0xE9,0x2C,0x55,0x5E,0x85,0xB2,0xD5,0xAA,0x0B,0x35,0x5B,0x80,0xEA,0xA6,0xD4,0xF0,0x14,0xFF,0x31,0x57,0x85,0xAB,0x03,0xEC}},
{0xFF,3,{0x98,0x82,0x0B}},
{0x9A,1,{0x44}},
{0x9B,1,{0x84}},
{0x9C,1,{0x03}},
{0x9D,1,{0x03}},
{0x9E,1,{0x71}},
{0x9F,1,{0x71}},
{0xAB,1,{0xE0}},
{0xFF,3,{0x98,0x82,0x0E}},
{0x11,1,{0x10}},
{0x13,1,{0x10}},
{0x00,1,{0xA0}},
{0xFF,3,{0x98,0x82,0x00}},
{0x35,1,{0x00}},
#endif
{0x11,0,{0x00}},
{REGFLAG_DELAY,120,{}},
{0x29,0,{0x00}},
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
	params->dsi.vertical_backporch                  = 10;
	params->dsi.vertical_frontporch                 = 230;
	params->dsi.vertical_active_line                = FRAME_HEIGHT;
	params->dsi.horizontal_sync_active              = 8;
	params->dsi.horizontal_backporch                = 20;
	params->dsi.horizontal_frontporch               = 20;
	params->dsi.horizontal_active_pixel             = FRAME_WIDTH;
	/*prize-zhaopengge modify lcd fps-20201012-start*/
	params->dsi.PLL_CLOCK                           = 260;
	#if  1
	params->dsi.ssc_disable = 1;
	params->dsi.ssc_range                           = 4;

	params->dsi.HS_TRAIL                            = 15;
	params->dsi.noncont_clock                       = 1;
	params->dsi.noncont_clock_period                = 1;

	/* ESD check function */
	params->dsi.esd_check_enable                    = 0;
	params->dsi.customization_esd_check_enable      = 1;
	params->dsi.lcm_esd_check_table[0].cmd = 0x0A;
	params->dsi.lcm_esd_check_table[0].count = 1;
	params->dsi.lcm_esd_check_table[0].para_list[0] = 0x9C;
	#endif
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

	SET_RESET_PIN(0, fdt);

	SET_RESET_PIN(1, fdt);
	MDELAY(1);
	SET_RESET_PIN(0, fdt);
	MDELAY(10);

	SET_RESET_PIN(1, fdt);
	MDELAY(20);//ili9882n at least 10ms

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

	//return 1;
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
	dprintf(CRITICAL, "%s, LK debug: ili9882 id = 0x%08x\n", __func__, id);
#else
	LCM_LOGI("%s: ili9882 id = 0x%d \n", __func__, id);
#endif
	//lcm_compare_id_tt();
	
    if(0x98820d == id)
	{
		//prize
		//prize
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

LCM_DRIVER ili9882n_hdp_dsi_vdo_truly6517_lcm_drv = {
	.name = "ili9882n_hdp_dsi_vdo_truly6517_drv",
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

