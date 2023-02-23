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

#include "rt4831a_drv.h"
#include <gpio_api.h>
#include "lcm_util.h"

#include "../../../platform/mediatek/common/i2c/include/mt_i2c.h"


#define LCM_LOGI(string, args...)  dprintf(CRITICAL, "[LK/"LOG_TAG"]"string, ##args)
#define LCM_LOGD(string, args...)  dprintf(INFO, "[LK/"LOG_TAG"]"string, ##args)

#define LCM_ID_NT36672EGMS_JDI (0x6E) // TO DO

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

//#define ONLY_60HZ   1
//#define HFP_SUPPORT   1

/* --------------------------------------------------------------------------- */
/* Local Constants */
/* --------------------------------------------------------------------------- */
#define LCM_DSI_CMD_MODE                                    0
#define FRAME_WIDTH                                     (1080)
#define FRAME_HEIGHT                                    (2408)

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


//prize add by lipengpeng 20210420 start 
//#define GPIO_LCD_BIAS_ENP (GPIO108 | 0x80000000)  //prize add 
//#define GPIO_LCD_BIAS_ENN (GPIO109  | 0x80000000)//prize add 
//#define GPIO_65132_ENP  GPIO_LCD_BIAS_ENP
//#define GPIO_65132_ENN  GPIO_LCD_BIAS_ENN
//prize add by lipengpeng 20210420 end
struct LCM_setting_table {
    unsigned int cmd;
    unsigned char count;
    unsigned char para_list[64];
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

//prize add by lipengpeng 20210420 start 
static void lcm_set_gpio_output(unsigned int GPIO, unsigned int output)
{
	mt_set_gpio_mode(GPIO, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO, (output>0)? GPIO_OUT_ONE: GPIO_OUT_ZERO);
}
//prize add by lipengpeng 20210420 start

/*static void lcm_set_bias(int enable)
{
	mt_set_gpio_mode(GPIO_65132_ENN, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO_65132_ENN, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_65132_ENN, GPIO_OUT_ONE);

	MDELAY(30);
	mt_set_gpio_mode(GPIO_65132_ENP, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO_65132_ENP, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_65132_ENP, GPIO_OUT_ONE);
	MDELAY(50);
	//if (enable){
	//	display_bias_vpos_set(6000);
	//	display_bias_vneg_set(6000);
	//	display_bias_enable();
	//}else{
	//	display_bias_disable();
		
	//}
}*/
//prize add by lipengpeng 20210420 end

static struct LCM_setting_table init_setting[] = {
		
//{0xB6,2,{0x07,0x20}},
{0x00,1,{0x00}},
{0xff,3,{0x87,0x22,0x01}},
{0x00,1,{0x80}},
{0xff,2,{0x87,0x22}},
{0x00,1,{0xa3}},                
{0xb3,2,{0x09,0x68}},
{0x00,1,{0x80}},
{0xC0,6,{0x00 ,0xB0 ,0x00 ,0x2C ,0x00 ,0x14}},
{0x00,1,{0x90}},
{0xC0,6,{0x00 ,0x4B ,0x00 ,0x2C ,0x00 ,0x14}},
{0x00,1,{0xA0}},
{0xC0,6,{0x00 ,0x4B ,0x00 ,0x2C ,0x00 ,0x14}},
{0x00,1,{0xB0}},
{0xC0,5,{0x00 ,0xD4 ,0x00 ,0x2C ,0x14}},
{0x00,1,{0xC1}},
{0xC0,8,{0x01 ,0x08 ,0x00 ,0xD1 ,0x00 ,0xB0 ,0x01 ,0x3A}},
{0x00,1,{0x70}},
{0xC0,6,{0x00 ,0x4B ,0x00 ,0x2C ,0x00 ,0x14}},
{0x00,1,{0xA3}},
{0xC1,6,{0x00, 0x54, 0x00 ,0x54 ,0x00 ,0x02}},
{0x00,1,{0xB7}},
{0xC1,2,{0x00 ,0x54}},
{0x00,1,{0x80}},
{0xCE,16,{0x01, 0x81 ,0xFF ,0xFF ,0x00 ,0x85 ,0x00 ,0x85 ,0x00 ,0xC8 ,0x00 ,0xC8 ,0x00 ,0xC8 ,0x00,0xC8}},
{0x00,1,{0x90}},
{0xCE,15,{0x00 ,0xBD ,0x12 ,0x75 ,0x00 ,0xBD ,0x80 ,0xFF ,0xFF ,0x00 ,0x06 ,0x40 ,0x0A ,0x0D ,0x0D}},
{0x00,1,{0xA0}},
{0xCE,3,{0x00 ,0x00 ,0x00}},
{0x00,1,{0xB0}},
{0xCE,3,{0x22 ,0x00 ,0x00}},
{0x00,1,{0xD1}},
{0xCE,7,{0x00 ,0x00 ,0x01 ,0x00 ,0x00 ,0x00 ,0x00}},
{0x00,1,{0xE1}},
{0xCE,11,{0x0A ,0x03 ,0x14 ,0x03 ,0x14 ,0x03 ,0x14 ,0x00 ,0x00 ,0x00 ,0x00}},
{0x00,1,{0xF1}},
{0xCE,9,{0x0E ,0x2A ,0x2A ,0x01 ,0x42 ,0x01 ,0x09 ,0x01 ,0x09}},
{0x00,1,{0xB0}},
{0xCF,4,{0x00 ,0x00 ,0xC2 ,0xC6}},
{0x00,1,{0xB5}},
{0xCF,4,{0x05 ,0x05 ,0x72 ,0x76}},
{0x00,1,{0xC0}},
{0xCF,4,{0x09 ,0x09 ,0x63 ,0x67}},
{0x00,1,{0xC5}},
{0xCF,4,{0x09 ,0x09 ,0x69 ,0x6D}},
{0x00,1,{0x60}},
{0xCF,8,{0x00 ,0x00 ,0xC2 ,0xC6 ,0x05 ,0x05 ,0x72 ,0x76}},
{0x00,1,{0x70}},
{0xCF,8,{0x00 ,0x00 ,0xC2 ,0xC6 ,0x05 ,0x05 ,0x72 ,0x76}},
{0x00,1,{0xD1}},
{0xC1,12,{0x0A ,0xEC ,0x0F ,0x19 ,0x19 ,0xD4 ,0x0A ,0xCE ,0x0F ,0x19 ,0x19 ,0xD4}},
{0x00,1,{0xE1}},
{0xC1,2,{0x0F ,0x19}},
{0x00,1,{0xE4}},
{0xCF,12,{0x09 ,0xF8 ,0x09 ,0xF7 ,0x09 ,0xF7 ,0x09 ,0xF7 ,0x09 ,0xF7 ,0x09 ,0xF7}},
{0x00,1,{0x80}},
{0xC1,2,{0x44 ,0x44}},
{0x00,1,{0x90}},
{0xC1,1,{0x03}},
{0x00,1,{0xF5}},
{0xCF,1,{0x00}},
{0x00,1,{0xF6}},
{0xCF,1,{0x78}},
{0x00,1,{0xF1}},
{0xCF,1,{0x78}},
{0x00,1,{0xF7}},
{0xCF,1,{0x11}},
{0x00,1,{0x8F}},
{0xC5,1,{0x20}},
{0x00,1,{0x80}},
{0xC2,8,{0x82,0x01,0x25,0x25,0x00,0x00,0x00,0x00}},
{0x00,1,{0x90}},
{0xC2,4,{0x00,0x00,0x00,0x00}},
{0x00,1,{0xA0}},
{0xC2,15,{0x00,0x80,0x00,0x17,0x8A,0x01,0x00,0x00,0x17,0x8A,0x02,0x01,0x00,0x17,0x8A}},
{0x00,1,{0xB0}},
{0xC2,10,{0x03,0x02,0x00,0x17,0x8A,0x80,0x08,0x03,0x00,0x00}},
{0x00,1,{0xCA}},
{0xC2,5,{0x84,0x08,0x03,0x00,0x00}},
{0x00,1,{0xE0}},
{0xC2,5,{0x33,0x33,0x70,0x00,0x70}},
{0x00,1,{0xE8}},		
{0xC2,8,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},			
{0x00,1,{0x80}},
{0xCB,16,{0x00,0x01,0x00,0x03,0xFD,0x01,0x01,0x00,0x00,0x00,0xFD,0x01,0x00,0x03,0x00,0x00}},
{0x00,1,{0x90}},
{0xCB,16,{0x00,0x00,0x00,0x0C,0xF0,0x00,0x00,0x00,0x00,0x00,0xFC,0x00,0x00,0x00,0x00,0x00}},
{0x00,1,{0xA0}},
{0xCB,8,{0x00,0x00,0x00,0x00,0x03,0x00,0x0C,0x00}},
{0x00,1,{0xB0}},
{0xCB,4,{0x13,0x54,0x05,0x30}},
{0x00,1,{0xC0}},
{0xCB,4,{0x13,0x54,0x05,0x30}},
{0x00,1,{0xD5}},
{0xCB,11,{0x01,0x00,0x01,0x01,0x00,0x01,0x01,0x00,0x01,0x01,0x00}},
{0x00,1,{0xE0}},
{0xCB,13,{0x01,0x01,0x00,0x01,0x01,0x00,0x01,0x01,0x00,0x01,0x01,0x00,0x01}},
{0x00,1,{0x80}},
{0xCC,16,{0x2C,0x12,0x2C,0x22,0x2C,0x0A,0x2C,0x2C,0x09,0x08,0x07,0x06,0x2C,0x2C,0x2C,0x2C}},
{0x00,1,{0x90}},
{0xCC,8,{0x2C,0x18,0x16,0x17,0x2C,0x1C,0x1D,0x1E}},
{0x00,1,{0xA0}},
{0xCC,16,{0x2C,0x12,0x2C,0x23,0x2C,0x0E,0x2C,0x2C,0x06,0x07,0x08,0x09,0x2C,0x2C,0x2C,0x2C}},
{0x00,1,{0xB0}},
{0xCC,8,{0x2C,0x18,0x16,0x17,0x2C,0x1C,0x1D,0x1E}},
{0x00,1,{0x80}},
{0xCD,16,{0x2C,0x2C,0x2C,0x02,0x2C,0x0A,0x2C,0x2C,0x09,0x08,0x07,0x06,0x2C,0x2C,0x2C,0x2C}},
{0x00,1,{0x90}},
{0xCD,8,{0x2C,0x18,0x16,0x17,0x2C,0x1C,0x1D,0x1E}},
{0x00,1,{0xA0}},
{0xCD,16,{0x2C,0x2C,0x2C,0x02,0x2C,0x0E,0x2C,0x2C,0x06,0x07,0x08,0x09,0x2C,0x2C,0x2C,0x2C}},
{0x00,1,{0xB0}},
{0xCD,8,{0x2C,0x18,0x16,0x17,0x2C,0x1C,0x1D,0x1E}},
{0x00,1,{0x86}},
{0xC0,8,{0x00,0x00,0x00,0x01,0x11,0x11,0x11,0x05}},
{0x00,1,{0x96}},
{0xC0,8,{0x00,0x00,0x00,0x01,0x11,0x11,0x11,0x05}},
{0x00,1,{0xA6}},
{0xC0,8,{0x00,0x00,0x00,0x01,0x11,0x11,0x11,0x05}},
{0x00,1,{0xA3}},
{0xCE,6,{0x00,0x00,0x00,0x01,0x11,0x05}},
{0x00,1,{0xB3}},
{0xCE,6,{0x00,0x00,0x00,0x01,0x11,0x05}},
{0x00,1,{0x76}},
{0xC0,8,{0x00,0x00,0x00,0x01,0x11,0x11,0x11,0x05}},	
{0x00,1,{0x82}},		
{0xa7,2,{0x10,0x00}},				
{0x00,1,{0x8d}},		
{0xa7,1,{0x01}},				
{0x00,1,{0x8f}},		
{0xa7,1,{0x01}},	
{0x00,1,{0xA0}},		
{0xC3,2,{0x35,0x21}},			
{0x00,1,{0xA4}},			
{0xC3,2,{0x01,0x20}},			
{0x00,1,{0xAA}},		
{0xC3,1,{0x21}},		
{0x00,1,{0xAd}},		
{0xC3,1,{0x01}},		
{0x00,1,{0xAe}},		
{0xC3,1,{0x20}},		
{0x00,1,{0xb3}},		
{0xC3,1,{0x21}},		
{0x00,1,{0xb6}},		
{0xC3,2,{0x01,0x20}},		
{0x00,1,{0xC3}},		
{0xC5,1,{0xFF}},		
{0x00,1,{0xA9}},		
{0xF5,1,{0x8E}},		
{0x00,1,{0x93}},
{0xC5,1,{0x25}},
{0x00,1,{0x97}},
{0xC5,1,{0x25}},
{0x00,1,{0x9A}},
{0xC5,1,{0x21}},
{0x00,1,{0x9C}},
{0xC5,1,{0x21}},		
{0x00,1,{0xB6}},		
{0xC5,8,{0x10,0x10,0x0E,0x0E,0x10,0x10,0x0E,0x0E}},										
{0x00,1,{0x90}},		
{0xc3,1,{0x08}},					
{0x00,1,{0xfa}},		
{0xc2,1,{0x23}},					
{0x00,1,{0xca}},		
{0xc0,1,{0x80}},					
{0x00,1,{0x82}},		
{0xF5,1,{0x01}},					
{0x00,1,{0x93}},		
{0xF5,1,{0x01}},					
{0x00,1,{0x9b}},		
{0xF5,1,{0x49}},					
{0x00,1,{0x9d}},		
{0xF5,1,{0x49}},					
{0x00,1,{0xbe}},		
{0xc5,2,{0xF0,0xF0}},				
{0x00,1,{0xdc}},		
{0xc3,1,{0x37}},					
{0x00,1,{0x8A}},		
{0xF5,1,{0xC7}},
{0x00,1,{0xB1}},		
{0xF5,1,{0x1F}},
{0x00,1,{0xB7}},		
{0xF5,1,{0x1F}},
{0x00,1,{0x99}},		
{0xCF,1,{0x50}},
{0x00,1,{0x9C}},		
{0xF5,1,{0x00}},
{0x00,1,{0x9E}},		
{0xF5,1,{0x00}},
{0x00,1,{0xB0}},		
{0xC5,6,{0xC0,0x4A,0x39,0xC0,0x4A,0x0E}},
{0x00,1,{0xC2}},		
{0xF5,1,{0x42}},
{0x00,1,{0x80}},
{0xC5,1,{0x77}},
{0x00,1,{0xE8}},
{0xC0,1,{0x40}},
{0x00,1,{0x00}},
{0xE1,40,{0x00,0x02,0x07,0x0F,0x36,0x1A,0x22,0x29,0x34,0xBA,0x3D,0x44,0x4A,0x4F,0x1B,0x54,0x5D,0x65,0x6C,0xD6,0x73,0x7B,0x83,0x8C,0xD4,0x96,0x9C,0xA3,0xAA,0x93,0xB3,0xBE,0xCD,0xD5,0xF3,0xE0,0xEF,0xF9,0xFF,0x84}},
{0x00,1,{0x30}},
{0xE1,40,{0x00,0x02,0x07,0x0F,0x36,0x1A,0x22,0x29,0x34,0xBA,0x3D,0x44,0x4A,0x4F,0x1B,0x54,0x5D,0x65,0x6C,0xD6,0x73,0x7B,0x83,0x8C,0xD4,0x96,0x9C,0xA3,0xAA,0x93,0xB3,0xBE,0xCD,0xD5,0xF3,0xE0,0xEF,0xF9,0xFF,0x84}},
{0x00,1,{0x60}},
{0xE1,40,{0x00,0x02,0x07,0x0F,0x36,0x1A,0x22,0x29,0x34,0xBA,0x3D,0x44,0x4A,0x4F,0x1B,0x54,0x5D,0x65,0x6C,0xD6,0x73,0x7B,0x83,0x8C,0xD4,0x96,0x9C,0xA3,0xAA,0x93,0xB3,0xBE,0xCD,0xD5,0xF3,0xE0,0xEF,0xF9,0xFF,0x84}},
{0x00,1,{0x90}},
{0xE1,40,{0x00,0x02,0x07,0x0F,0x36,0x1A,0x22,0x29,0x34,0xBA,0x3D,0x44,0x4A,0x4F,0x1B,0x54,0x5D,0x65,0x6C,0xD6,0x73,0x7B,0x83,0x8C,0xD4,0x96,0x9C,0xA3,0xAA,0x93,0xB3,0xBE,0xCD,0xD5,0xF3,0xE0,0xEF,0xF9,0xFF,0x84}},
{0x00,1,{0xC0}},
{0xE1,40,{0x00,0x02,0x07,0x0F,0x36,0x1A,0x22,0x29,0x34,0xBA,0x3D,0x44,0x4A,0x4F,0x1B,0x54,0x5D,0x65,0x6C,0xD6,0x73,0x7B,0x83,0x8C,0xD4,0x96,0x9C,0xA3,0xAA,0x93,0xB3,0xBE,0xCD,0xD5,0xF3,0xE0,0xEF,0xF9,0xFF,0x84}},
{0x00,1,{0xF0}},
{0xE1,16,{0x00,0x02,0x07,0x0F,0x36,0x1A,0x22,0x29,0x34,0xBA,0x3D,0x44,0x4A,0x4F,0x1B,0x54}},
{0x00,1,{0x00}},
{0xE2,24,{0x5D,0x65,0x6C,0xD6,0x73,0x7B,0x83,0x8C,0xD4,0x96,0x9C,0xA3,0xAA,0x93,0xB3,0xBE,0xCD,0xD5,0xF3,0xE0,0xEF,0xF9,0xFF,0x84}},
{0x00,1,{0xE0}},
{0xCF,1,{0x34}},
{0x00,1,{0x85}},
{0xA7,1,{0x41}},
{0x00,1,{0x80}},
{0xB3,1,{0x22}},
{0x00,1,{0xB0}},
{0xB3,1,{0x00}},
{0x00,1,{0x83}},
{0xB0,1,{0x63}},
{0x00,1,{0xA1}},
{0xB0,1,{0x02}},
{0x00,1,{0xA9}},
{0xB0,2,{0xAA,0x0A}},
{0x00,1,{0xB0}},
{0xCA,3,{0x01,0x01,0x0C}},
{0x00,1,{0xB5}},
{0xCA,1,{0x08}},
{0x1c,1,{0x02}},

{0x00,1,{0x00}}, 
{0xFF,3,{0xFF,0xFF,0xFF}},	

//prize
#if 0
{0x00,1,{0xA9}},
{0xF6,1,{0x00}},
{0x00,1,{0x88}},
{0xF6,1,{0x5A}},
{0x00,1,{0x90}},
{0xF6,8,{0x20,0x80,0x80,0x80,0x00,0x00,0x00,0x00}},

//prize
#endif

//prize
#if 0
{0x00,1,{0xA0}},
{0xF6,8,{0x0B,0x01,0x23,0x45,0x67,0x89,0xAB,0x00}},
{REGFLAG_DELAY, 10, {} },
{0x00,1,{0xA9}},
{0xF6,1,{0x00}},
{0x00,1,{0x88}},
{0xF6,1,{0x5A}},
//prize
#endif
	
//prize
#if 0
	{0x00, 1,{0xA0}},
	{0xF6, 9,{0x0F,0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF}},
	{0x00, 1,{0xA9}},
	{0xF6, 1,{0x00}},
	{0x00, 1,{0x88}},
	{0xF6, 1,{0x5A}},
//prize
#endif


	//TEON
//	{0x35, 1,{0x00}},
{0x11, 0, {} },
{REGFLAG_DELAY, 120, {} },
{0x29, 0, {} },
{REGFLAG_DELAY, 50, {} },

//{0xB7,2,{0x59,0x02}},
//{0xFF,2,{0xFF,0xFF}},

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

    params->dsi.vertical_sync_active = 4;
    params->dsi.vertical_backporch = 30;
#if HFP_SUPPORT
    params->dsi.vertical_frontporch = 30;
#else
    params->dsi.vertical_frontporch = 30;
#endif
    params->dsi.vertical_frontporch_for_low_power = 36;
    params->dsi.vertical_active_line = FRAME_HEIGHT;

    params->dsi.horizontal_sync_active = 8;
    params->dsi.horizontal_backporch = 32;
#if HFP_SUPPORT
    params->dsi.horizontal_frontporch = 40;
#else
    params->dsi.horizontal_frontporch = 40;
#endif
    params->dsi.horizontal_active_pixel = FRAME_WIDTH;
    params->dsi.ssc_disable = 1;

#if (LCM_DSI_CMD_MODE)
    params->dsi.PLL_CLOCK = 550;    /* this value must be in MTK suggested table */
#else
    params->dsi.PLL_CLOCK = 550;    /* this value must be in MTK suggested table */
#endif
    //params->dsi.data_rate = 1400;  //prize 

    params->dsi.clk_lp_per_line_enable = 0;
    params->dsi.esd_check_enable = 0;
    params->dsi.customization_esd_check_enable = 0;
    params->dsi.lcm_esd_check_table[0].cmd = 0x0A;
    params->dsi.lcm_esd_check_table[0].count = 1;
    params->dsi.lcm_esd_check_table[0].para_list[0] = 0x9C;

    params->dsi.lane_swap_en = 0;
#ifdef MTK_RUNTIME_SWITCH_FPS_SUPPORT   //prize 
    params->dsi.fps = 120;  //prize 
#endif   //prize 
    params->dsi.vact_fps = 60; //prize add 
	
#ifdef MTK_ROUND_CORNER_SUPPORT
    params->round_corner_params.round_corner_en = 1;
    params->round_corner_params.full_content = 0;
    params->round_corner_params.h = ROUND_CORNER_H_TOP;
    params->round_corner_params.h_bot = ROUND_CORNER_H_BOT;
    params->round_corner_params.tp_size = sizeof(top_rc_pattern);
    params->round_corner_params.lt_addr = (void *)top_rc_pattern;
#endif
   /* params->dsi.dsc_enable = 1;  //prize 
    params->dsi.dsc_params.ver = 17;
    params->dsi.dsc_params.slice_mode = 1;
    params->dsi.dsc_params.rgb_swap = 0;
    params->dsi.dsc_params.dsc_cfg = 34;
    params->dsi.dsc_params.rct_on = 1;
    params->dsi.dsc_params.bit_per_channel = 8;
    params->dsi.dsc_params.dsc_line_buf_depth = 9;
    params->dsi.dsc_params.bp_enable = 1;
    params->dsi.dsc_params.bit_per_pixel = 128;
    params->dsi.dsc_params.pic_height = 2408;
    params->dsi.dsc_params.pic_width = 1080;
    params->dsi.dsc_params.slice_height = 8;
    params->dsi.dsc_params.slice_width = 540;
    params->dsi.dsc_params.chunk_size = 540;
    params->dsi.dsc_params.xmit_delay = 170;
    params->dsi.dsc_params.dec_delay = 526;
    params->dsi.dsc_params.scale_value = 32;
    params->dsi.dsc_params.increment_interval = 43;
    params->dsi.dsc_params.decrement_interval = 7;
    params->dsi.dsc_params.line_bpg_offset = 12;
    params->dsi.dsc_params.nfl_bpg_offset = 3511;
    params->dsi.dsc_params.slice_bpg_offset = 3255;
    params->dsi.dsc_params.initial_offset = 6144;
    params->dsi.dsc_params.final_offset = 7072;
    params->dsi.dsc_params.flatness_minqp = 3;
    params->dsi.dsc_params.flatness_maxqp = 12;
    params->dsi.dsc_params.rc_model_size = 8192;
    params->dsi.dsc_params.rc_edge_factor = 6;
    params->dsi.dsc_params.rc_quant_incr_limit0 = 11;
    params->dsi.dsc_params.rc_quant_incr_limit1 = 11;
    params->dsi.dsc_params.rc_tgt_offset_hi = 3;
    params->dsi.dsc_params.rc_tgt_offset_lo = 3;
*/	
}

//prize add by lipengpeng 20210420 start 
//#ifndef MTK_RT4831A_SUPPORT
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

//#endif
//prize add by lipengpeng 20210420 end 
static void lcm_init_power(void *fdt)
{
        SET_RESET_PIN(0, fdt);
        MDELAY(100);
//prize add by lipengpeng 20210420 start 
	//lcm_set_gpio_output(108, GPIO_OUT_ONE);
	//MDELAY(5);
    //lcm_set_gpio_output(109, GPIO_OUT_ONE);	
	//MDELAY(5);
    if (lcm_util.set_gpio_lcd_enp_bias) {
        lcm_util.set_gpio_lcd_enp_bias(1, fdt);  //eanble 5.8V
        _lcm_i2c_write_bytes(0x00, 0x12,fdt); //5.8V
		MDELAY(1);
        _lcm_i2c_write_bytes(0x01, 0x12,fdt); //5.8V
		MDELAY(1);
        LCM_LOGI("lcm_init_power %s\n", __func__);

   } else {
        LCM_LOGI("set_gpio_lcd_enp_bias not defined\n");
    }
	MDELAY(5);
    lcm_set_gpio_output(107, GPIO_OUT_ONE);
	
//prize add by lipengpeng 20210420 end 
}

static void lcm_suspend_power(void)
{

}

static void lcm_resume_power()
{
//	SET_RESET_PIN(0, fdt);
//	lcm_init_power(fdt);
}

#ifdef LCM_SET_DISPLAY_ON_DELAY
static u32 lcm_init_time;
static int is_display_on;

#endif

static void lcm_init(void *fdt)
{
    int ret = 0;
	unsigned int id = 0,id1=0;
	unsigned char buffer[2];
    unsigned int array[16];

    SET_RESET_PIN(0, fdt);


#ifndef MACH_FPGA
#if 0
    /*config rt5081 register 0xB2[7:6]=0x3, that is set db_delay=4ms.*/
    ret = PMU_REG_MASK(0xB2, (0x3 << 6), (0x3 << 6));

    /* set AVDD 5.4v, (4v+28*0.05v) */
    /*ret = RT5081_write_byte(0xB3, (1 << 6) | 28);*/
    ret = PMU_REG_MASK(0xB3, 28, (0x3F << 0));
    if (ret < 0)
        LCM_LOGI("nt35695----tps6132----cmd=%0x--i2c write error----\n", 0xB3);
    else
        LCM_LOGI("nt35695----tps6132----cmd=%0x--i2c write success----\n", 0xB3);

    /* set AVEE */
    /*ret = RT5081_write_byte(0xB4, (1 << 6) | 28);*/
    ret = PMU_REG_MASK(0xB4, 28, (0x3F << 0));
    if (ret < 0)
        LCM_LOGI("nt35695----tps6132----cmd=%0x--i2c write error----\n", 0xB4);
    else
        LCM_LOGI("nt35695----tps6132----cmd=%0x--i2c write success----\n", 0xB4);

    /* enable AVDD & AVEE */
    /* 0x12--default value; bit3--Vneg; bit6--Vpos; */
    /*ret = RT5081_write_byte(0xB1, 0x12 | (1<<3) | (1<<6));*/
    ret = PMU_REG_MASK(0xB1, (1<<3) | (1<<6), (1<<3) | (1<<6));
    if (ret < 0)
        LCM_LOGI("nt35695----tps6132----cmd=%0x--i2c write error----\n", 0xB1);
    else
        LCM_LOGI("nt35695----tps6132----cmd=%0x--i2c write success----\n", 0xB1);

    MDELAY(15);
#endif
#endif

    SET_RESET_PIN(1, fdt);
    MDELAY(10);
    SET_RESET_PIN(0, fdt);
    MDELAY(10);

    SET_RESET_PIN(1, fdt);
    MDELAY(10);

//prize add by lipengpeng 20220520 start  test
	array[0] = 0x00013700;
	dsi_set_cmdq(array, 1, 1);

	read_reg_v2(0xDA, buffer, 1);
	id = buffer[0];

	array[0] = 0x00013700;
	dsi_set_cmdq(array, 1, 1);

	read_reg_v2(0xA1, buffer, 1);
	id1 = buffer[0];
	
	dprintf(0, "%s,FT8720 id = 0x%08x,0x%08x\n", __func__, id,id1);
	
	MDELAY(10);
//prize add by lipengpeng 20220520 end 	test

    dprintf(0, "[dong] go table\n");
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
            LCM_LOGI("nt35695B %s error: i=%u,lcm_init_time=%u,cur_time=%u\n", __func__,
                i, lcm_init_time, current_time());
        }
    }

    push_table(set_display_on, sizeof(set_display_on) / sizeof(struct LCM_setting_table), 1);

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
#ifndef MACH_FPGA
    /* enable AVDD & AVEE */
    /* 0x12--default value; bit3--Vneg; bit6--Vpos; */
    /*ret = RT5081_write_byte(0xB1, 0x12);*/
    ret = PMU_REG_MASK(0xB1, (0<<3) | (0<<6), (1<<3) | (1<<6));
    if (ret < 0)
        LCM_LOGI("nt35695----tps6132----cmd=%0x--i2c write error----\n", 0xB1);
    else
        LCM_LOGI("nt35695----tps6132----cmd=%0x--i2c write success----\n", 0xB1);

    MDELAY(5);

#endif
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

static struct LCM_setting_table page_switch_on[] = {
        {0xFF, 1, {0x20} },
};

static struct LCM_setting_table page_switch_off[] = {
        {0xFF, 1, {0x10} },
};

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

    push_table(page_switch_on, sizeof(page_switch_on) / sizeof(struct LCM_setting_table), 1);

    array[0] = 0x00013700;    /* read id return two byte,version and id */
    dsi_set_cmdq(array, 1, 1);

    read_reg_v2(0x1A, buffer, 1);
    id = buffer[0];     /* we only need ID */

    push_table(page_switch_off, sizeof(page_switch_off) / sizeof(struct LCM_setting_table), 1);

    LCM_LOGI("%s,nt3672C_id_jdi=0x%08x\n", __func__, id);

    if (id == LCM_ID_NT36672EGMS_JDI)
        return 1;
    else
        return 0;

}


/* return TRUE: need recovery */
/* return FALSE: No need recovery */
static unsigned int lcm_esd_check(void)
{
#if 0
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

    LCM_LOGI("%s,nt36672egms backlight: level = %d\n", __func__, level);

    bl_level[0].para_list[0] = level;

    push_table(bl_level, sizeof(bl_level) / sizeof(struct LCM_setting_table), 1);
}

static void lcm_setbacklight(unsigned int level)
{
    LCM_LOGI("%s,nt36672egms backlight: level = %d\n", __func__, level);

    bl_level[0].para_list[0] = level;

    push_table(bl_level, sizeof(bl_level) / sizeof(struct LCM_setting_table), 1);
}

static void *lcm_switch_mode(int mode)
{
    return NULL;
}


LCM_DRIVER nt36672egms_fhdp_dsi_vdo_120hz_jdi_cphy_lcm_drv = {
    .name = "nt36672egms_fhdp_dsi_vdo_120hz_jdi_cphy_lcm_drv",
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
