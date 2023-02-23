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

#define R63417_ID			(0x00003417)
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

/* ------------------------------------------------------------------------ */
/* Local Constants */
/* --------------------------------------------------------------------------- */
#define LCM_DSI_CMD_MODE                                    0
#define FRAME_WIDTH                                     (1080)
#define FRAME_HEIGHT                                    (2160)
//#define USE_LDO
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
static struct LCM_setting_table lcm_suspend_setting[] = {
	{0x28, 0, {} },
	{0x10, 0, {} },
	{REGFLAG_DELAY, 120, {} },
	{0x4F, 1, {0x01} },
	{REGFLAG_DELAY, 120, {} }
};
static struct LCM_setting_table init_setting[] = {
 	{0x11,01,{0x00}},                   
	{REGFLAG_DELAY, 120, {}},         
	{0x29,01,{0x00}},                  
	{REGFLAG_DELAY, 20, {}}, 
     
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};
static struct LCM_setting_table bl_level[] = {
	{0x51, 1, {0xFF} },
	{REGFLAG_END_OF_TABLE, 0x00, {} }
};
static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
	unsigned int i;
	for (i = 0; i < count; i++) {
		unsigned cmd;
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
	params->physical_width = 69;
	params->physical_height = 136;
	
	/* enable tearing-free */
	params->dbi.te_mode = LCM_DBI_TE_MODE_VSYNC_ONLY;
	params->dbi.te_edge_polarity = LCM_POLARITY_RISING;
	
#if (LCM_DSI_CMD_MODE)
	params->dsi.mode = CMD_MODE;
	params->dsi.switch_mode = SYNC_PULSE_VDO_MODE;
#else
	params->dsi.mode = BURST_VDO_MODE;
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
	params->dbi.io_driving_current      = LCM_DRIVING_CURRENT_4MA;
	params->dsi.word_count=FRAME_WIDTH*3;
	/* Highly depends on LCD driver capability. */
	params->dsi.packet_size = 256;
	/* video mode timing */

	params->dsi.PS = LCM_PACKED_PS_24BIT_RGB888;

    params->dsi.vertical_sync_active                = 2; //2
    params->dsi.vertical_backporch                  = 14; //14
    params->dsi.vertical_frontporch                 = 16;  //16
    params->dsi.vertical_active_line                = FRAME_HEIGHT;

	params->dsi.horizontal_sync_active				= 8;//10 8
	params->dsi.horizontal_backporch				= 16;//34 32; 
	params->dsi.horizontal_frontporch				= 16;//24 43;
	params->dsi.horizontal_active_pixel				= FRAME_WIDTH;
	
	params->dsi.PLL_CLOCK = 460; //465,450;	/* this value must be in MTK suggested table */
	
	params->dsi.esd_check_enable=1;
	params->dsi.customization_esd_check_enable=1;

	params->dsi.lcm_esd_check_table[0].cmd=0x0A;  
	params->dsi.lcm_esd_check_table[0].count=1;  
	params->dsi.lcm_esd_check_table[0].para_list[0]=0x1C;
}

static void lcm_init_power(void *fdt)
{
        SET_RESET_PIN(0, fdt);
        MDELAY(100);
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
	//SET_RESET_PIN(0);
	//SET_RESET_PIN(0);
#if defined (USE_LDO)
	display_ldo18_enable(1);
//	MDELAY(2);
	//SET_RESET_PIN(0);
	display_ldo28_enable(1);
	MDELAY(10);
#endif
    SET_RESET_PIN(1, fdt);
	
	//display_bias_enable();
	
	MDELAY(2);
    SET_RESET_PIN(0, fdt);
	MDELAY(20);
    SET_RESET_PIN(1, fdt);
	MDELAY(150);
	push_table(init_setting, sizeof(init_setting) / sizeof(struct LCM_setting_table), 1);
}
static void lcm_suspend(void)
{
//	push_table(lcm_suspend_setting, sizeof(lcm_suspend_setting) / sizeof(struct LCM_setting_table), 1);
//	MDELAY(10);
#if defined (USE_LDO)
	//mt_set_gpio_out(VLCM_LDO18_EN, GPIO_OUT_ZERO);
	//MDELAY(2);
	//mt_set_gpio_out(VLCM_LDO28_EN, GPIO_OUT_ZERO);
	//MDELAY(2);
#endif
}
static void lcm_resume(void)
{
//	lcm_init();
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

static unsigned int lcm_compare_id(void *fdt)
{
	unsigned int id = 0;
	unsigned char buffer[5];
	unsigned int array[16];
	//display_ldo28_enable(1);
//	MDELAY(2);
//	display_bias_enable();
//	MDELAY(3);
    SET_RESET_PIN(1, fdt);
	MDELAY(10);
    SET_RESET_PIN(0, fdt);
	MDELAY(50);
    SET_RESET_PIN(1, fdt);
	MDELAY(50); 
	array[0] = 0x00022902;                          
	array[1] = 0x000000b0; 
	dsi_set_cmdq(array, 2, 1);
	array[0] = 0x00053700;// read id return two byte,version and id
	dsi_set_cmdq(array, 1, 1);
	read_reg_v2(0xbf, buffer, 4);
	id = buffer[2]<<8 | buffer[3]; //we only need ID
	#ifdef BUILD_LK
		printf("%s, myf LK r63417 debug: id= x%08x,id0 = 0x%08x,id1 = 0x%08x,id2 = 0x%08x,id3 = 0x%08x\n", __func__, id,buffer[0],buffer[1],buffer[2],buffer[3]);
    #endif
	//display_bias_disable();
	if (id == R63417_ID)
		return 1;
	else
		return 0;
}
static unsigned int lcm_esd_check(void)
{
    return FALSE;
}
static unsigned int lcm_ata_check(unsigned char *buffer)
{
	return 0;
}

static void lcm_setbacklight_cmdq(void *handle, unsigned int level)
{
	LCM_LOGI("%s,nt35695 backlight: level = %d\n", __func__, level);
	bl_level[0].para_list[0] = level;
	push_table(bl_level, sizeof(bl_level) / sizeof(struct LCM_setting_table), 1);
}
static void lcm_setbacklight(unsigned int level)
{
	LCM_LOGI("%s,nt35695 backlight: level = %d\n", __func__, level);
	bl_level[0].para_list[0] = level;
	push_table(bl_level, sizeof(bl_level) / sizeof(struct LCM_setting_table), 1);
}
static void *lcm_switch_mode(int mode)
{
    return NULL;
}
LCM_DRIVER td4310_fhd2160_dsi_vdo_dzx_lcm_drv = 
{
    .name			= "td4310_fhd2160_dsi_vdo_dzx",
  	//prize-lixuefeng-20150512-start
	#if defined(CONFIG_PRIZE_HARDWARE_INFO) && !defined (BUILD_LK)
	.lcm_info = {
		.chip	= "td4310",
		.vendor	= "dzx",
		.id		= "0x8009",
		.more	= "1920*1080",
	},
	#endif
	//prize-lixuefeng-20150512-end		
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
	// .set_backlight = lcm_setbacklight,
	.ata_check = lcm_ata_check,
	.update = lcm_update,
    .switch_mode = lcm_switch_mode,
};
