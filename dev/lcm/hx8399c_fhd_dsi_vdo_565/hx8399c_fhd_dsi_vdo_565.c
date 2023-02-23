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


#define FRAME_WIDTH										(1080)
#define FRAME_HEIGHT									(2160)

#define LCM_PHYSICAL_WIDTH									(64000)
#define LCM_PHYSICAL_HEIGHT									(130000)


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
	{REGFLAG_DELAY, 20, {}},
	{0x28, 0, {0x00}},
	{REGFLAG_DELAY, 50, {}},
    	// Sleep Mode On
	{0x10, 0, {0x00}},
	{REGFLAG_DELAY, 150, {}},
	{0xB0,1,{0x04}},
	{0xB1,1,{0x01}},
	{REGFLAG_DELAY, 20, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

/*
29 04 B9 FF 83 99
29 03 BA 63 23
29 13 B1 02 04 74 94 01 32 33 11 11 5C 4D 06
29 13 B2 00 80 80 CC 05 07 5A 11 00 00 10 0E
29 45 B4 00 FF 02 A7 02 A7 02 A7 02 00 03 05 00 2D 03 0E 0A 21 03 02 00 0B A5 87 02 A7 02 A7 02 A7 02 00 03 05 00 2D 03 0E 0A 02 00 0B A5 01
29 34 D3 00 00 03 03 00 00 06 00 32 10 04 00 04 00 00 00 00 00 00 00 00 00 00 01 00 05 05 07 00 00 00 05 40
29 33 D5 18 18 19 19 18 18 21 20 03 02 05 04 07 06 01 00 18 18 18 18 18 18 2F 2F 30 30 31 31 18 18 18 18
29 33 D6 18 18 19 19 58 58 20 21 00 01 06 07 04 05 02 03 58 58 58 58 58 58 2F 2F 30 30 31 31 58 58 58 58
29 09 D8 A2 AA 02 A0 A2 AA 02 A0
29 02 BD 01
29 09 D8 E2 AA 03 F0 E2 AA 03 F0
29 02 BD 02
29 09 D8 E2 AA 03 F0 E2 AA 03 F0
29 02 BD 00
29 55 E0 01 20 2B 26 53 5D 6D 6A 72 7C 84 8A 8F 99 A1 A5 AA B5 BB C1 B6 C4 C7 66 61 6B 73 01 11 21 20 53 5D 6D 6A 72 7C 84 8A 8F 99 A1 A5 AA B3 B2 C1 B6 C4 C7 66 61 6B 73
29 03 B6 87 87
29 02 D2 77
29 02 CC 08
29 01 11 
29 01 29
*/
static struct LCM_setting_table init_setting_vdo[] = {
#if 1
{0XB9,3,{0XFF,0X83,0X99}},
{0XBA,2,{0X63,0X23}},
{0XB1,12,{0X00,0X04,0X71,0X91,0X01,0X32,0X33,0X11,0X11,0X4D,0X57,0X06}},
{0XB2,11,{0X00,0X80,0X80,0XCC,0X05,0X07,0X5A,0X11,0X00,0X00,0X10}},
{0XB4,44,{0X00,0XFF,0X02,0XA7,0X02,0XA7,0X02,0XA7,0X02,0X00,0X03,0X05,0X00,0X2D,0X03,0X0E,0X0A,0X21,0X03,0X02,0X00,0X0B,0XA5,0X87,0X02,0XA7,0X02,0XA7,0X02,0XA7,0X02,0X00,0X03,0X05,0X00,0X2D,0X03,0X0E,0X0A,0X02,0X00,0X0B,0XA5,0X01}},
{0XD3,44,{0X00,0X0C,0X03,0X03,0X00,0X00,0X14,0X04,0X32,0X10,0X09,0X00,0X09,0X32,0X10,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X11,0X00,0X02,0X02,0X03,0X00,0X00,0X00,0X0A,0X40}},
{0XD5,33,{0X18,0X18,0X18,0X18,0X03,0X02,0X01,0X00,0X18,0X18,0X18,0X18,0X18,0X18,0X19,0X19,0X21,0X20,0X18,0X18,0X18,0X18,0X18,0X18,0X18,0X18,0X2F,0X2F,0X30,0X30,0X31,0X31}},
{0XD6,32,{0X18,0X18,0X18,0X18,0X00,0X01,0X02,0X03,0X18,0X18,0X40,0X40,0X19,0X19,0X18,0X18,0X20,0X21,0X40,0X40,0X18,0X18,0X18,0X18,0X18,0X18,0X2F,0X2F,0X30,0X30,0X31,0X31}},
{0XD8,32,{0XAF,0XAA,0XEA,0XAA,0XAF,0XAA,0XEA,0XAA,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00}},
{0XBD,1,{0X01}},
{0XD8,16,{0XFF,0XEF,0XEA,0XBF,0XFF,0XEF,0XEA,0XBF,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00}},
{0XBD,1,{0X02}},
{0XD8,8,{0XFF,0XEF,0XEA,0XBF,0XFF,0XEF,0XEA,0XBF}},
{0XBD,1,{0X00}},
{0XE0,54,{0X01,0X20,0X2B,0X26,0X53,0X5D,0X6D,0X6A,0X72,0X7C,0X84,0X8A,0X8F,0X99,0XA1,0XA5,0XAA,0XB5,0XBB,0XC1,0XB6,0XC4,0XC7,0X66,0X61,0X6B,0X73,0X01,0X11,0X21,0X20,0X53,0X5D,0X6D,0X6A,0X72,0X7C,0X84,0X8A,0X8F,0X99,0XA1,0XA5,0XAA,0XB3,0XB2,0XC1,0XB6,0XC4,0XC7,0X66,0X61,0X6B,0X73}},
{0XB6,2,{0X81,0X81}},
{0XD2,1,{0X66}},
{0XCC,1,{0X08}},
#else
{0xB9,0x3,{0xFF,0x83,0x99} },
{0xBA,0x2,{0x63,0x23} },
{0xB1,0xC,{0x02,0x04,0x74,0x94,0x01,0x32,0x33,0x11,0x11,0x5C,0x4D,0x06} },
{0xB2,0xC,{0x00,0x80,0x80,0xCC,0x05,0x07,0x5A,0x11,0x00,0x00,0x10,0x0E} },
{0xB4,0x2C,{0x00,0xFF,0x02,0xA7,0x02,0xA7,0x02,0xA7,0x02,0x00,0x03,0x05,0x00,0x2D,0x03,0x0E,0x0A,0x21,0x03,0x02,0x00,0x0B,0xA5,0x87,0x02,0xA7,0x02,0xA7,0x02,0xA7,0x02,0x00,0x03,0x05,0x00,0x2D,0x03,0x0E,0x0A,0x02,0x00,0x0B,0xA5,0x01} },
{0xD3,0x21,{0x00,0x00,0x03,0x03,0x00,0x00,0x06,0x00,0x32,0x10,0x04,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x05,0x05,0x07,0x00,0x00,0x00,0x05,0x40} },
{0xD5,0x20,{0x18,0x18,0x19,0x19,0x18,0x18,0x21,0x20,0x03,0x02,0x05,0x04,0x07,0x06,0x01,0x00,0x18,0x18,0x18,0x18,0x18,0x18,0x2F,0x2F,0x30,0x30,0x31,0x31,0x18,0x18,0x18,0x18} },
{0xD6,0x20,{0x18,0x18,0x19,0x19,0x58,0x58,0x20,0x21,0x00,0x01,0x06,0x07,0x04,0x05,0x02,0x03,0x58,0x58,0x58,0x58,0x58,0x58,0x2F,0x2F,0x30,0x30,0x31,0x31,0x58,0x58,0x58,0x58} },
{0xD8,0x8,{0xA2,0xAA,0x02,0xA0,0xA2,0xAA,0x02,0xA0} },
{0xBD,0x1,{0x01} },
{0xD8,0x8,{0xE2,0xAA,0x03,0xF0,0xE2,0xAA,0x03,0xF0} },
{0xBD,0x1,{0x02} },
{0xD8,0x8,{0xE2,0xAA,0x03,0xF0,0xE2,0xAA,0x03,0xF0} },
{0xBD,0x1,{0x00} },
{0xE0,0x36,{0x01,0x20,0x2B,0x26,0x53,0x5D,0x6D,0x6A,0x72,0x7C,0x84,0x8A,0x8F,0x99,0xA1,0xA5,0xAA,0xB5,0xBB,0xC1,0xB6,0xC4,0xC7,0x66,0x61,0x6B,0x73,0x01,0x11,0x21,0x20,0x53,0x5D,0x6D,0x6A,0x72,0x7C,0x84,0x8A,0x8F,0x99,0xA1,0xA5,0xAA,0xB3,0xB2,0xC1,0xB6,0xC4,0xC7,0x66,0x61,0x6B,0x73} },
{0xB6,0x2,{0x87,0x87} },
{0xD2,0x1,{0x77} },
{0xCC,0x1,{0x08} },
#endif
{0x11, 1,{0x00}},
{REGFLAG_DELAY,120,{}},
{0x29,1,{0x00}},
{REGFLAG_DELAY,20,{}},
{REGFLAG_END_OF_TABLE,0x00,{}}
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
	params->dbi.io_driving_current      = LCM_DRIVING_CURRENT_4MA;
	/* Highly depends on LCD driver capability. */
	params->dsi.packet_size = 256;
	/* video mode timing */

	params->dsi.PS = LCM_PACKED_PS_24BIT_RGB888;
	params->dsi.vertical_sync_active = 2;
	params->dsi.vertical_backporch = 5;
	params->dsi.vertical_frontporch = 9;
	params->dsi.vertical_active_line                = FRAME_HEIGHT;
	params->dsi.horizontal_sync_active = 20; //50--40
	params->dsi.horizontal_backporch = 20;
	params->dsi.horizontal_frontporch = 80;//  60-->75
	params->dsi.horizontal_active_pixel             = FRAME_WIDTH;
	/*prize-zhaopengge modify lcd fps-20201012-start*/
	params->dsi.PLL_CLOCK                           = 469;

	params->dsi.ssc_disable = 1;
	params->dsi.ssc_range                           = 4;
	#if  0
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

	//SET_RESET_PIN(0, fdt);

	SET_RESET_PIN(1, fdt);
	MDELAY(20);
	SET_RESET_PIN(0, fdt);
	MDELAY(20);

	SET_RESET_PIN(1, fdt);
	MDELAY(150);//ili9882n at least 10ms

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

	MDELAY(10);
    SET_RESET_PIN(1, fdt);
	MDELAY(10);
    SET_RESET_PIN(0, fdt);
    MDELAY(20);
    SET_RESET_PIN(1, fdt);
    MDELAY(120);

	MDELAY(5);
	array[0]=0x00043700;
	dsi_set_cmdq(array, 1, 1);
	
    read_reg_v2(0xDA,&buffer[0], 1);
	read_reg_v2(0xDB,&buffer[1], 1);
	read_reg_v2(0xDC,&buffer[2], 1);
    id = ( (buffer[0] << 16)|(buffer[1] << 8)| (buffer[2]) ); //
	
#ifdef BUILD_LK
	dprintf(CRITICAL, "%s, LK debug: hx8399 id = 0x%08x\n", __func__, id);
#else
	LCM_LOGI("%s: hx8399 id = 0x%d \n", __func__, id);
#endif
	
    if(0x83990c == id)
	{
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

LCM_DRIVER hx8399c_fhd_dsi_vdo_565_lcm_drv = {
	.name = "hx8399c_fhd_dsi_vdo_565_drv",
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

