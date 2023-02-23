#define LOG_TAG "LCM"

#include <debug.h>
#include <mt_i2c.h>
#include <platform.h>
#include "lcm_drv.h"
#include "pmic_auxadc.h"


#include <gpio_api.h>
#include "lcm_util.h"

#include "../../../platform/mediatek/common/i2c/include/mt_i2c.h"


#define LCM_LOGI(string, args...)  dprintf(CRITICAL, "[LK/"LOG_TAG"]"string, ##args)
#define LCM_LOGD(string, args...)  dprintf(INFO, "[LK/"LOG_TAG"]"string, ##args)

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
#define wrtie_cmd(cmd) \
	lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums) \
	lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd) \
	lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size) \
	lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)

static const unsigned char LCD_MODULE_ID = 0x01;
/* --------------------------------------------------------------------------- */
/* Local Constants */
/* --------------------------------------------------------------------------- */
#define LCM_DSI_CMD_MODE	0
#define FRAME_WIDTH  										1080
#define FRAME_HEIGHT 										2400
//#define USE_LDO
#define GPIO_TP_RST (GPIO15 | 0x80000000)
#define LCM_PHYSICAL_WIDTH                  				(69500)
#define LCM_PHYSICAL_HEIGHT                  				(154440)

#define REGFLAG_DELAY             							 0xFFFA
#define REGFLAG_UDELAY             							 0xFFFB
#define REGFLAG_PORT_SWAP									 0xFFFC
#define REGFLAG_END_OF_TABLE      							 0xFFFD   // END OF REGISTERS MARKER

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
	unsigned char para_list[64];
};

static struct LCM_setting_table lcm_initialization_setting[] = {
//----------------------LCD initial code start----------------------//
//CMD2 ENABLE
{0x00,1,{0x00}},
{0xFF,3,{0x87,0x56,0x01}},

{0x00,1,{0x80}},
{0xFF,2,{0x87,0x56}},

//Panel resulotion
{0x00,1,{0xA1}},
{0xB3,6,{0x04,0x38,0x09,0x60,0x00,0xFC}},   
//----------------------------------------------//
//TCON Setting
{0x00,1,{0x80}},
{0xC0,6,{0x00,0x86,0x00,0x24,0x00,0x46}},

{0x00,1,{0x90}},
{0xC0,6,{0x00,0x86,0x00,0x24,0x00,0x46}},

{0x00,1,{0xA0}},
{0xC0,6,{0x01,0x07,0x00,0x24,0x00,0x46}},

{0x00,1,{0xB0}},
{0xC0,5,{0x00,0x86,0x00,0x24,0x46}},

{0x00,1,{0xC1}},
{0xC0,8,{0x00,0xC4,0x00,0x99,0x00,0x83,0x00,0xE5}},

{0x00,1,{0xD7}},
{0xC0,6,{0x00,0x83,0x00,0x24,0x00,0x46}},

{0x00,1,{0xA3}},
{0xC1,6,{0x00,0x34,0x00,0x34,0x00,0x02}},

{0x00,1,{0x80}},
{0xCE,16,{0x01,0x81,0x09,0x13,0x00,0xEC,0x00,0xEC,0x00,0x85,0x00,0xA0,0x00,0x74,0x00,0x74}},

{0x00,1,{0x90}},
{0xCE,15,{0x00,0x8E,0x0E,0xB6,0x00,0x8E,0x80,0x09,0x13,0x00,0x04,0x00,0x3A,0x39,0x2B}},

{0x00,1,{0xA0}},
{0xCE,3,{0x20,0x00,0x00}},

{0x00,1,{0xB0}},
{0xCE,3,{0x22,0x00,0x00}},

{0x00,1,{0xD1}},
{0xCE,7,{0x00,0x00,0x01,0x00,0x00,0x00,0x00}},

{0x00,1,{0xE1}},
{0xCE,11,{0x08,0x02,0x4D,0x02,0x4D,0x02,0x4D,0x00,0x00,0x00,0x00}},

{0x00,1,{0xF1}},
{0xCE,9,{0x16,0x0B,0x0F,0x01,0x43,0x01,0x43,0x01,0x3D}},

{0x00,1,{0xB0}},
{0xCF,4,{0x00,0x00,0xDA,0xDE}},

{0x00,1,{0xB5}},
{0xCF,4,{0x05,0x05,0x12,0x16}},

{0x00,1,{0xC0}},
{0xCF,4,{0x09,0x09,0x4C,0x50}},

{0x00,1,{0xC5}},
{0xCF,4,{0x00,0x00,0x08,0x0C}},
//---------------------------------------------------------------------------------//
//Panel scan mode
{0x00,1,{0xE8}},
{0xC0,1,{0x40}},

//BOE GIP setting
{0x00,1,{0xCA}},
{0xC0,1,{0x80}},

//VST
{0x00,1,{0x80}},
{0xc2,8,{0x83,0x01,0x01,0x81,0x82,0x01,0x01,0x81}},

//CKV1-3
{0x00,1,{0xa0}},
{0xc2,15,{0x8a,0x0a,0x00,0x00,0x86,0x89,0x0a,0x00,0x00,0x86,0x88,0x0a,0x00,0x00,0x86}},
//CKV4
{0x00,1,{0xb0}},
{0xc2,5,{0x87,0x0a,0x00,0x00,0x86}},

{0x00,1,{0xe0}},
{0xc2,14,{0x33,0x33,0x33,0x33,0x33,0x33,0x00,0x00,0x03,0x09,0x10,0x6a,0x01,0x81}},

{0x00,1,{0x80}},
{0xcb,16,{0xfd,0xfd,0x3c,0x30,0xfd,0x0c,0x1c,0xfd,0x02,0xfe,0xfd,0xfd,0xfd,0xfd,0x00,0xfc}},

{0x00,1,{0x90}},
{0xcb,16,{0xff,0x00,0x00,0x00,0xff,0x00,0x00,0x00,0x00,0xff,0x00,0x00,0x00,0x00,0x00,0x00}},

{0x00,1,{0xa0}},
{0xcb,4,{0x00,0x00,0x00,0x00}},

{0x00,1,{0xb0}},
{0xcb,4,{0x50,0x41,0xA4,0x50}},

{0x00,1,{0xc0}},
{0xcb,4,{0x50,0x41,0xA4,0x50}},
//============================================================================//
//BOE CGOUT Select
{0x00,1,{0x80}},//U2D Left CGOUT Select
{0xcc,16,{0x25,0x25,0x00,0x24,0x29,0x29,0x1b,0x18,0x1a,0x17,0x19,0x16,0x00,0x00,0x00,0x00}},
	
{0x00,1,{0x90}},//U2D Left CGOUT Select
{0xcc,8,{0x07,0x09,0x01,0x26,0x26,0x22,0x24,0x03}},

{0x00,1,{0x80}},//U2D Right CGOUT Select
{0xcd,16,{0x25,0x25,0x00,0x24,0x29,0x29,0x1b,0x18,0x1a,0x17,0x19,0x16,0x00,0x00,0x00,0x00}},

{0x00,1,{0x90}},//U2D Right CGOUT Select
{0xcd,8,{0x06,0x08,0x01,0x26,0x26,0x22,0x24,0x02}},

{0x00,1,{0xa0}},//D2U Left CGOUT Select
{0xcc,16,{0x25,0x25,0x00,0x24,0x00,0x00,0x1b,0x18,0x1a,0x17,0x19,0x16,0x00,0x00,0x00,0x00}},

{0x00,1,{0xb0}},//D2U Left CGOUT Select
{0xcc,8,{0x08,0x06,0x01,0x26,0x26,0x24,0x23,0x02}},

{0x00,1,{0xa0}},//D2U Right CGOUT Select
{0xcd,16,{0x25,0x25,0x00,0x24,0x00,0x00,0x1b,0x18,0x1a,0x17,0x19,0x16,0x00,0x00,0x00,0x00}},

{0x00,1,{0xb0}},//D2U Right CGOUT Select
{0xcd,8,{0x09,0x07,0x01,0x26,0x26,0x24,0x23,0x03}},
//============================================================================//
//Source mapping
{0x00,1,{0xA0}},
{0xC4,3,{0x8d,0xd8,0x8d}},

//pump x2
{0x00,1,{0x93}}, 
{0xC5,1,{0x5A}},

//Gamma 32
{0x00,1,{0xA4}},  
{0xD7,1,{0x00}},

//EN_VGHO1 follow EN_VGH
{0x00,1,{0x9B}},  
{0xF5,1,{0x4B}},

//VGHO1 precharge off
{0x00,1,{0x93}},  
{0xF5,2,{0x00,0x00}},

//EN_VGLO1 follow EN_VGL
{0x00,1,{0x9D}},  
{0xF5,1,{0x49}},

//VGLO1 precharge off
{0x00,1,{0x82}},  
{0xF5,2,{0x00,0x00}},

//ahung:hvreg_en_vglo1s,1,{hvreg_en_vglo2s
{0x00,1,{0x80}},
{0xF5,2,{0x59,0x59}},

//hvreg_en_vgho1s,1,{hvreg_en_vgho2s,1,{pump_en_vgh_s
{0x00,1,{0x84}},
{0xF5,3,{0x59,0x59,0x59}},

//pump_en_vgl_s
{0x00,1,{0x96}},
{0xF5,1,{0x59}},

//vclreg_en_s
{0x00,1,{0xA6}},
{0xF5,1,{0x59}},

//pwron 3->5
{0x00,1,{0xCA}},
{0xC0,1,{0x80}},

//ahung:vcom_en_vcom
{0x00,1,{0xB1}},
{0xF5,1,{0x1f}},

//GVDD=4.6V
//NGVDD=-4.6V
{0x00,1,{0x00}},
{0xD8,2,{0x2B,0x2B}},

{0x00,1,{0x00}},
{0xD9,2,{0x22,0x22}},

{0x00,1,{0x00}},
{0xE1,40,{0x00,0x06,0x08,0x0F,0x2F,0x19,0x20,0x26,0x30,0x69,0x38,0x3E,0x44,0x49,0x20,0x4D,0x55,0x5C,0x63,0x99,0x6A,0x71,0x78,0x81,0x1C,0x8A,0x90,0x96,0x9D,0xDB,0xA6,0xB1,0xBF,0xC8,0x9A,0xD5,0xE7,0xF5,0xFF,0x07}},
{0x00,1,{0x00}},
{0xE2,40,{0x00,0x06,0x08,0x0F,0x2F,0x1B,0x22,0x28,0x32,0x69,0x3A,0x40,0x46,0x4B,0x20,0x4F,0x57,0x5E,0x65,0x99,0x6C,0x71,0x78,0x81,0x1C,0x8A,0x90,0x96,0x9D,0xDB,0xA6,0xB1,0xBF,0xC8,0x9A,0xD5,0xE7,0xF5,0xFF,0x07}},
{0x00,1,{0x00}},
{0xE3,40,{0x00,0x06,0x08,0x0F,0x2F,0x19,0x20,0x26,0x30,0x69,0x38,0x3E,0x44,0x49,0x20,0x4D,0x55,0x5C,0x63,0x99,0x6A,0x71,0x78,0x81,0x1C,0x8A,0x90,0x96,0x9D,0xDB,0xA6,0xB1,0xBF,0xC8,0x9A,0xD5,0xE7,0xF5,0xFF,0x07}},
{0x00,1,{0x00}},
{0xE4,40,{0x00,0x06,0x08,0x0F,0x2F,0x1B,0x22,0x28,0x32,0x69,0x3A,0x40,0x46,0x4B,0x20,0x4F,0x57,0x5E,0x65,0x99,0x6C,0x71,0x78,0x81,0x1C,0x8A,0x90,0x96,0x9D,0xDB,0xA6,0xB1,0xBF,0xC8,0x9A,0xD5,0xE7,0xF5,0xFF,0x07}},
{0x00,1,{0x00}},
{0xE5,40,{0x00,0x06,0x08,0x0F,0x2F,0x19,0x20,0x26,0x30,0x69,0x38,0x3E,0x44,0x49,0x20,0x4D,0x55,0x5C,0x63,0x99,0x6A,0x71,0x78,0x81,0x1C,0x8A,0x90,0x96,0x9D,0xDB,0xA6,0xB1,0xBF,0xC8,0x9A,0xD5,0xE7,0xF5,0xFF,0x07}},
{0x00,1,{0x00}},
{0xE6,40,{0x00,0x06,0x08,0x0F,0x2F,0x1B,0x22,0x28,0x32,0x69,0x3A,0x40,0x46,0x4B,0x20,0x4F,0x57,0x5E,0x65,0x99,0x6C,0x71,0x78,0x81,0x1C,0x8A,0x90,0x96,0x9D,0xDB,0xA6,0xB1,0xBF,0xC8,0x9A,0xD5,0xE7,0xF5,0xFF,0x07}},

//VGHO1=8V & VGLO1=-8V
{0x00,1,{0xB6}},
{0xC5,4,{0x19,0x19,0x19,0x19}},

//Reduce Source ring for COB
{0x00,1,{0x85}},
{0xC4,1,{0x1C}},

{0x00,1,{0x80}},
{0xA4,1,{0x6C}},

{0x00,1,{0x8D}},
{0xA5,1,{0x06}},

{0x00,1,{0xc6}},
{0xF5,1,{0x02}},

{0x00,1,{0xa9}},
{0xF5,1,{0xc1}},

{0x00,1,{0xb1}},
{0xc5,1,{0x8a}},

{0x00,1,{0xb4}},
{0xc5,1,{0x8a}},

{0x00,1,{0xA0}},
{0xC3,16,{0x15,0x01,0x23,0x45,0x54,0x32,0x10,0x00,0x00,0x00,0x34,0x50,0x12,0x21,0x05,0x43}},

{0x00,1,{0x8C}}, 
{0xC3,3,{0x00,0x00,0x00}},
//whs-chk-gap
//{0x00,1,{0x86}},
//{0xc0,1,{0x00,1,{0x00,1,{0x00}},
//{0x00,1,{0x89}},
//{0xc0,1,{0x02,1,{0x12,1,{0x01}},
{0x00,1,{0x89}},
{0xc0,3,{0x02,0x0c,0x07}},

{0x00,1,{0xCB}},
{0xC0,1,{0x19}},//power off²åÊ®Ö¡ºÚ»­Ãæ

{0x00,1,{0x84}},
{0xc5,2,{0x26,0x26}},

{0x00,1,{0x80}},
{0xA4,1,{0x6C}},

{0x00,1,{0x97}},
{0xC4,1,{0x01}},

////Rotate[4:0]
//{0x00,1,{0x80}},
//{0xa7,1,{0x10}},

//GOFF width
{0x00,1,{0xD0}},
{0xC3,12,{0x46,0x00,0x00,0x00,0x46,0x00,0x00,0x30,0x46,0x00,0x00,0x30}},

//----------------------LCD initial code End----------------------//
//{0x1C,1,{0x10}},//720*1600
{0x11,0,{}},
{REGFLAG_DELAY,120, {}},
{0x29,0,{}},
{REGFLAG_DELAY,30, {}},

{0x35,1,{0x00}},
{0xB7,2,{0x59,0x02}},
{0xFF,3,{0xff,0xff,0xff}},//send RED

{REGFLAG_END_OF_TABLE, 0x00, {}}
};

static struct LCM_setting_table lcm_suspend_setting[] = {
	{0x28, 0, {}},
	{REGFLAG_DELAY, 50, {} },
	{0x10, 0, {}},
	{REGFLAG_DELAY, 150, {} },

	{REGFLAG_END_OF_TABLE, 0x00, {}}  
};

static void push_table(struct LCM_setting_table *table, unsigned int count,
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

	params->dsi.mode = SYNC_EVENT_VDO_MODE;
	params->dsi.switch_mode = CMD_MODE;
	params->dsi.switch_mode_enable = 0;
		
	// DSI
	/* Command mode setting */
	//1 Three lane or Four lane
	params->dsi.LANE_NUM				= LCM_FOUR_LANE;
	//The following defined the fomat for data coming from LCD engine.
	params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
	params->dsi.data_format.trans_seq	= LCM_DSI_TRANS_SEQ_MSB_FIRST;
	params->dsi.data_format.padding 	= LCM_DSI_PADDING_ON_LSB;
	params->dsi.data_format.format		= LCM_DSI_FORMAT_RGB888;
		
	/* Highly depends on LCD driver capability. */
	params->dsi.packet_size = 256;
	/* video mode timing */
		
	params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;

	params->dsi.vertical_sync_active = 30;//
	params->dsi.vertical_backporch = 40;//
	params->dsi.vertical_frontporch = 10;// 8  prize --»Æ¼ÌÎä--for shanping
	params->dsi.vertical_frontporch_for_low_power = 10;
	params->dsi.vertical_active_line = FRAME_HEIGHT;

	params->dsi.horizontal_sync_active = 20;
	params->dsi.horizontal_backporch =80;
	params->dsi.horizontal_frontporch = 10;
	params->dsi.horizontal_active_pixel = FRAME_WIDTH;

    params->dsi.PLL_CLOCK= 538;        //4LANE   232
	params->dsi.ssc_disable = 1;
	params->dsi.ssc_range = 1;
	//params->dsi.cont_clock = 0;
	params->dsi.clk_lp_per_line_enable = 0;

	params->physical_width = LCM_PHYSICAL_WIDTH/1000;
	params->physical_height = LCM_PHYSICAL_HEIGHT/1000;

	#if 1
	params->dsi.ssc_disable = 1;
		
	params->dsi.esd_check_enable = 1;
	params->dsi.customization_esd_check_enable = 1;
	params->dsi.lcm_esd_check_table[0].cmd			= 0x0a;
	params->dsi.lcm_esd_check_table[0].count		= 1;
	params->dsi.lcm_esd_check_table[0].para_list[0] = 0x9c;
	#endif
	/*prize-add-for round corner-houjian-20180918-start*/
	#ifdef MTK_ROUND_CORNER_SUPPORT
	params->round_corner_params.w = ROUND_CORNER_W;
	params->round_corner_params.h = ROUND_CORNER_H;
	params->round_corner_params.lt_addr = left_top;
	params->round_corner_params.rt_addr = right_top;
	params->round_corner_params.lb_addr = left_bottom;
	params->round_corner_params.rb_addr = right_bottom;
	#endif
	/*prize-add-for round corner-houjian-20180918-end*/
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
#define LCM_CHANNEL (AUXADC_CHAN_VIN3)
#define MIN_VAL 800
#define MAX_VAL 1100
//extern int auxadc_get_voltage(unsigned int channel);
extern int pmic_auxadc_read_value_ext(int channel, int *val,enum auxadc_val_type type);
static unsigned int lcm_compare_id_tt(void)
{
    int rawdata=0;
	int ret=0;
	//ret = auxadc_get_voltage(LCM_CHANNEL);
	pmic_auxadc_read_value_ext(LCM_CHANNEL,&rawdata,AUXADC_VAL_RAW);
	if(rawdata >=MIN_VAL && rawdata <=MAX_VAL)
	{
		dprintf(CRITICAL, "%s, hjw ft8756 111 id = %d\n", __func__, rawdata);
		return 1;
	}else{
		dprintf(CRITICAL, "%s, hjw ft8756 222 id = %d\n", __func__, rawdata);
		return 0;
	}
}

static unsigned int lcm_compare_id(void *fdt)
{
	#define LCM_ID 0x400000
    int array[4];
    char buffer[4]={0,0,0,0};
    int id = 0;

/*	
	display_ldo18_enable(1);
	//TP_rest prize-huangjiwu-for fae  begin
	mt_set_gpio_mode(GPIO_TP_RST, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO_TP_RST, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_TP_RST, GPIO_OUT_ONE);
	MDELAY(5);

	display_bias_enable();
*/
	SET_RESET_PIN(1, fdt);

	MDELAY(10);
	SET_RESET_PIN(0, fdt);

	MDELAY(10);  

    SET_RESET_PIN(1, fdt);
	MDELAY(60);//250

    array[0] = 0x00013700;
    dsi_set_cmdq(array, 1, 1);

    MDELAY(1);
    read_reg_v2(0xda, &buffer[0], 1);//    NC 0x00  0x98 0x16
    MDELAY(1);
    read_reg_v2(0xdb, &buffer[1], 1);//    NC 0x00  0x98 0x16
    MDELAY(1);
    read_reg_v2(0xdc, &buffer[2], 1);//    NC 0x00  0x98 0x16

	id = (buffer[0]<<16) | (buffer[1]<<8) | buffer[2];

#ifdef BUILD_LK
	dprintf(0,"%s: ft8756 id = 0x%08x\n", __func__, id);
#else
	LCM_LOGI("%s: ft8756 id = 0x%08x \n", __func__, id);
#endif
	if(LCM_ID == id)
	{
		if(lcm_compare_id_tt())
		{
			return 1;
		}
	}
    return 0;
}

static void lcm_init(void *fdt)
{
//	int ret = 0;
#if defined (USE_LDO)
	display_ldo18_enable(1);
	
	display_ldo28_enable(1);
	MDELAY(10);
	SET_RESET_PIN(0);
	//TP_rest prize-huangjiwu-for fae  begin
	mt_set_gpio_mode(GPIO_TP_RST, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO_TP_RST, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_TP_RST, GPIO_OUT_ONE);
	MDELAY(5);
	//TP_rest prize-huangjiwu-for fae  end
	//AVDD  AVEE
	display_bias_enable();
	//lcm_ret
#endif
	SET_RESET_PIN(1, fdt);

	MDELAY(10);
	SET_RESET_PIN(0, fdt);

	MDELAY(10);  

	SET_RESET_PIN(1, fdt);

	MDELAY(60);//250
	
	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting)/sizeof(struct LCM_setting_table), 1);
}

static void lcm_suspend(void)
{
	int ret = 0;

	push_table(lcm_suspend_setting, sizeof(lcm_suspend_setting)/sizeof(struct LCM_setting_table), 1);
	MDELAY(10);//100
	
}

static void lcm_resume(void)
{
	//lcm_init();
}
static void lcm_update(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{

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
LCM_DRIVER ft8756_hdp_dsi_vdo_csot67_lcm_drv = 
{
    .name			= "ft8756_hdp_dsi_vdo_csot67",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.compare_id     = lcm_compare_id,
	.init_power = lcm_init_power,
	.resume_power = lcm_resume_power,
	.suspend_power = lcm_suspend_power,
	.esd_check = lcm_esd_check,
	//.set_backlight = lcm_setbacklight_cmdq,
	.ata_check = lcm_ata_check,
	.update = lcm_update,
};
