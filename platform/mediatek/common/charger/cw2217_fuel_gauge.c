//#include <mt_typedefs.h>
#include <mt_i2c.h>  
#include "cw2217_fuel_gauge.h"
#include <platform/timer.h>
#include <libfdt.h>
#include <mt_logo.h>
#include <mtk_battery.h>
#include <mtk_charger.h>
#include <platform.h>
#include <platform/boot_mode.h>
#include <platform/leds.h>
#include <platform/wdt.h>
#include <platform_mtk.h>
#include <pmic_auxadc.h>
#include <pmic_keys.h>
#include <set_fdt.h>
#include <sysenv.h>
#include <debug.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <trace.h>

#include <debug.h>
#include <err.h>
#include <libfdt.h>
#include <lib/pl_boottags.h>
#include <mtk_battery.h>
#include <mt_gauge.h>
#include <pmic_auxadc.h>
#include <pmic_dlpt.h>
#include <set_fdt.h>
#include <trace.h>


#define CWFG_ENABLE_LOG 1 //CHANGE   Customer need to change this for enable/disable log
#define CWFG_I2C_BUSNUM 0 //CHANGE   Customer need to change this number according to the principle of hardware


#define REG_CHIP_ID             0x00
#define REG_VCELL_H             0x02
#define REG_VCELL_L             0x03
#define REG_SOC_INT             0x04
#define REG_SOC_DECIMAL         0x05
#define REG_TEMP                0x06
#define REG_MODE_CONFIG         0x08
#define REG_GPIO_CONFIG         0x0A
#define REG_SOC_ALERT           0x0B
#define REG_TEMP_MAX            0x0C
#define REG_TEMP_MIN            0x0D
#define REG_CURRENT_H           0x0E
#define REG_CURRENT_L           0x0F
#define REG_T_HOST_H            0xA0
#define REG_T_HOST_L            0xA1
#define REG_USER_CONF           0xA2
#define REG_CYCLE_H             0xA4
#define REG_CYCLE_L             0xA5
#define REG_SOH                 0xA6
#define REG_IC_STATE            0xA7
#define REG_STB_CUR_H           0xA8
#define REG_STB_CUR_L           0xA9
#define REG_FW_VERSION          0xAB
#define REG_BAT_PROFILE         0x10

#define CONFIG_MODE_RESTART     0x30
#define CONFIG_MODE_ACTIVE      0x00
#define CONFIG_MODE_SLEEP       0xF0
#define CONFIG_UPDATE_FLG       0x80
#define IC_VCHIP_ID             0xA0
#define IC_READY_MARK           0x0C
#define IC_TEMP_READY           0x08
#define IC_VOL_CUR_READY        0x04

#define SIZE_OF_PROFILE         80

#define RSENSE 10

#define INIT_TEST_TIME      50 /*must >= 50 , can not modify */

#define CW221X_ERROR_IIC      -1
#define CW221X_ERROR_CHIP_ID  -2
#define CW221X_ERROR_TIME_OUT -3
#define CW221X_NOT_ACTIVE          1
#define CW221X_PROFILE_NOT_READY   2
#define CW221X_PROFILE_NEED_UPDATE 3

#define GPIO_SOC_IRQ_VALUE      0x0    /* 0x7F */
#define CW_SLEEP_COUNTS         80

STRUCT_CW_BATTERY   cw_bat;

#define I2C_OK         0x0000

#define cw_printk(fmt, arg...)        \
	({                                    \
		if(CWFG_ENABLE_LOG){\
			dprintf(CRITICAL,"FG_CW2217 : %s : " fmt, __FUNCTION__ ,##arg);\
		}else{}                           \
	})     //need check by Chaman


#define CWFG_NAME "cw2217"
#define SIZE_BATINFO    80
#define CW2217_SLAVE_ADDR_WRITE   0xC8 //0x62  
#define CW2217_SLAVE_ADDR_READ    0xC9 //0x63
#define CW2217_SLAVE_ADDR    0x64 //0x63
#define CW2217_I2C_ID	0x00

//static struct mt_i2c_t cw2217_i2c;

//prize add by lipengpeng 20220707 start 
#ifndef mdelay
#define mdelay(x)       spin((x) * 1000)
#endif

//prize add by lipengpeng 20220707 end

static u32 cw2217_i2c_idinfo = 0x00;
//static unsigned char config_info[SIZE_BATINFO] = {0};

static unsigned char config_profile_info[SIZE_OF_PROFILE] = {
//prize add by lipengpeng 20220707 start
	0x50,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xA8,
	0xB7,0xBD,0xCF,0xC6,0xC7,0x85,0x4B,0x20,0xFF,
	0xFF,0xC8,0x88,0x71,0x59,0x4B,0x41,0x36,0x29,
	0x56,0x23,0xD9,0xBF,0xDD,0xD5,0xCD,0xCB,0xC9,
	0xC6,0xC2,0xBE,0xB8,0xB3,0xBA,0xBC,0xA0,0x8C,
	0x84,0x79,0x70,0x6C,0x6E,0x83,0x8F,0xA1,0x8D,
	0x60,0xD8,0x80,0x00,0xAB,0x10,0x00,0x82,0xE7,
	0x00,0x00,0x00,0x64,0x12,0x91,0xCA,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xDD
//prize add by lipengpeng 20220707 end
};

struct cw_battery {
    struct i2c_client *client;
    int charger_mode;
    int capacity;
    int voltage;
    int status;
    int time_to_empty;
	int change;
    //int alt;
};

//prize add by lipengpeng 20220707 start 

int cw_write(unsigned char addr, unsigned short* value)
{
	int ret = 0;
    unsigned char write_buf[2] = {0};
	//unsigned char write_buf[2] = {addr, * value};
	write_buf[0]= addr;
    write_buf[1] = (unsigned char)(*value & 0xff);
	//write_buf[2] = (*value) >> 8;
	
       ret = mtk_i2c_write(CW2217_I2C_ID, CW2217_SLAVE_ADDR, write_buf, 2, NULL);
    if (ret != 0)
        cw_printk("lpp---mtk_i2c_write failed ret: %d\n",ret);

    return ret;
}

int cw_read(unsigned char addr, unsigned short *dataBuffer) 
{
	int ret = 0;
    unsigned char ret_data[3] = {0};

	
	ret_data[0] = addr;

    ret = mtk_i2c_write_read(CW2217_I2C_ID, CW2217_SLAVE_ADDR,ret_data, 1, 1, NULL);
    if (ret != 0)
        dprintf("lpp--- I2CR[0x%02X] failed, code = %d\n",ret);
    else {
        dprintf("lpp---I2CR[0x%02X] = 0x%02X\n",ret_data);
        *dataBuffer = ret_data[0];
    }
    return ret;
}

int cw221x_read_word(unsigned char addr, unsigned short *dataBuffer) 
{
	int ret = 0;
    unsigned char ret_data[3] = {0};

	
	ret_data[0] = addr;
		
		ret = mtk_i2c_write_read(CW2217_I2C_ID,CW2217_SLAVE_ADDR, ret_data, 1, 2, NULL);
		//cw_printk("cw2217 read %2x = %2x ret=%d\n", addr,  ret);
    if (ret != 0)
        dprintf("lpp--- I2CR[0x%02X] failed, code = %d\n", ret);
    else {
        dprintf("lpp---I2CR[0x%02X] = 0x%02X\n", ret_data);
        *dataBuffer = (ret_data[1] << 8) + ret_data[0];
    }

	return ret;

}
//prize add by lipengpeng 20220707 end 

static int cw221x_get_state(void)
{
	int ret;
	unsigned short reg_val;
	int i;
	
	printf("lpp---cw221x cw221x_get_state start\n" );	
	
	ret = cw_read(REG_MODE_CONFIG, &reg_val);
	if(ret)
		return CW221X_ERROR_IIC;
	if (reg_val != CONFIG_MODE_ACTIVE)
		return CW221X_NOT_ACTIVE;
	
	printf("lpp---cw221x cw221x_get_state start111\n" );
	
	ret = cw_read(REG_SOC_ALERT, &reg_val);
	if (ret)
		return CW221X_ERROR_IIC;
	if (0x00 == (reg_val & CONFIG_UPDATE_FLG))
		return CW221X_PROFILE_NOT_READY;
	
		printf("lpp---cw221x cw221x_get_state start222\n" );
		
	for (i = 0; i < SIZE_OF_PROFILE; i++) {

		ret = cw_read((REG_BAT_PROFILE + i), &reg_val);
		if (ret)
			return CW221X_ERROR_IIC;

		printf("lpp---cw221x reg[%02X] = %02X\n", REG_BAT_PROFILE + i, reg_val);
		printf("lpp---cw221x config_profile_info[i] = %02X %d\n", config_profile_info[i],i);
		if (config_profile_info[i] != reg_val)
			break;
	}
	if ( i != SIZE_OF_PROFILE)
		return CW221X_PROFILE_NEED_UPDATE;
	
	return 0;
}

int cw221x_get_chip_id(int *chip_id)
{
	int ret;
	unsigned short reg_val;
	cw_printk("lpp---cw221x_get_chip_id start\n");
	ret = cw_read(REG_CHIP_ID, &reg_val);
	if(ret)
		return CW221X_ERROR_IIC;
	*chip_id = reg_val;
	return 0;
}

static int cw221x_write_profile(unsigned char buf[])
{
	int ret;
	int i;

	for (i = 0; i < SIZE_OF_PROFILE; i++) {
		ret = cw_write(REG_BAT_PROFILE + i, &buf[i]);
		//cw_printk("lpp---cw221x_write_profile ret=%d %d\n",ret,i);
		if (ret)
		{
			//cw_printk("lpp---cw221x_write_profile ret=%d\n",ret);
			return CW221X_ERROR_IIC;
		}
	}

	return 0;
}

int cw221x_sleep(void)
{
	int ret;
	unsigned short reg_val = CONFIG_MODE_RESTART;

	ret = cw_write(REG_MODE_CONFIG, &reg_val);
	if (ret)
		return CW221X_ERROR_IIC;
	
	//cw_delay10us(2 * 1000); /* Here delay must >= 20 ms */
	mdelay(30);

	reg_val = CONFIG_MODE_SLEEP;
	ret = cw_write(REG_MODE_CONFIG, &reg_val);
	if (ret)
		return CW221X_ERROR_IIC;
	//cw_delay10us(1 * 1000); 
	mdelay(1);

	/* printf("cw221x_sleep\n");*/
	return 0;
}

static int cw221x_active()
{
	int ret;
	unsigned short reg_val = CONFIG_MODE_RESTART;

	ret = cw_write(REG_MODE_CONFIG, &reg_val);
	if (ret)
		return CW221X_ERROR_IIC;
	//cw_delay10us(2 * 1000); /* Here delay must >= 20 ms */
	mdelay(30);

	reg_val = CONFIG_MODE_ACTIVE;
	ret = cw_write(REG_MODE_CONFIG, &reg_val);
	if (ret < 0)
		return CW221X_ERROR_IIC;
	//cw_delay10us(1 * 1000); 
	mdelay(1);

	/* printf("cw221x_active\n");*/
	return 0;
}

/*CW221X update profile function, Often called during initialization*/
static int cw221x_config_start_ic()
{
	int ret = 0;
	unsigned short reg_val = 0;
	unsigned int count = 0;

	ret = cw221x_sleep();
	if (ret < 0)
		return ret;
	printf("lpp---cw221x_config_start_ic start\n");
	/* update new battery info */
	ret = cw221x_write_profile(config_profile_info);
	if (ret < 0)
		return ret;
    printf("lpp---cw221x_write_profile sucess ret=%d\n",ret); 
	/* set UPDATE_FLAG AND SOC INTTERRUP VALUE*/
	reg_val = CONFIG_UPDATE_FLG | GPIO_SOC_IRQ_VALUE;   
	ret = cw_write(REG_SOC_ALERT, &reg_val);
	if (ret)
		return CW221X_ERROR_IIC;
   printf("lpp---REG_SOC_ALERT sucess ret=%d\n",ret);
	/*close all interruptes*/
	reg_val = 0; 
	ret = cw_write(REG_GPIO_CONFIG, &reg_val); 
	if (ret)
		return CW221X_ERROR_IIC;

	ret = cw221x_active();
	if (ret < 0) 
		return ret;
	 printf("lpp---cw221x_active sucess ret=%d\n",ret);
	while (1) {
		cw_read(REG_IC_STATE, &reg_val);
		if (IC_READY_MARK == (reg_val & IC_READY_MARK)) //0x0c 1100 & 100
		{
			printf("lpp---cw221x_config_start_ic 333 =%x %d\n",reg_val,count);
			break;
		}
		printf("lpp---cw221x_config_start_ic 444 =%x %d\n",reg_val,count);
		count++;
		if (count >= CW_SLEEP_COUNTS) {
			cw221x_sleep();
			return CW221X_ERROR_TIME_OUT;
		}
		
		mdelay(100); //prize add by lipengpeng must sleep  2000-3000
	}
	
	printf("lpp---cw221x_config_start_ic success\n");
	return 0;
}

int cw221x_init(void)
{
    int ret;
//    int i;
	int chip_id;

	//const void *data = NULL;
	//int len = 0;
	//int offset = 0;
	cw_printk("lpp---cw221x_init w start!");
#if 0
	offset = fdt_node_offset_by_compatible(get_lk_overlayed_dtb(),-1,"cellwise,cw221X");
	if (offset < 0) {
		cw_printk("[cw2217] Failed to find cellwise,cw2217 in dtb\n");
	}else{
		//batinfo
		data = fdt_getprop(get_lk_overlayed_dtb(), offset, "batinfo", &len);
		if (len > 0 && data){
			cw_printk("[cw2217] batinfo len(%d)\n",len);
			if (len != SIZE_BATINFO){
				cw_printk("[cw2217] get bat batinfo fail len(%d) != SIZE_BATINFO(%d)\n",len,SIZE_BATINFO);
			}else{
				memcpy(config_info,data,len);
				cw_printk("[cw2217]batinfo ");
				for (i=0;i<SIZE_BATINFO;i++){
					cw_printk("cw2217_init lk config_info[%d] =%x\n",i,config_info[i]);
				}
				cw_printk("\n");
			}
		}
		//i2c id
		offset = fdt_parent_offset(get_lk_overlayed_dtb(),offset);
		data = fdt_getprop(get_lk_overlayed_dtb(), offset, "id", &len);
		if (len > 0 && data){
			cw2217_i2c_idinfo = fdt32_to_cpu(*(unsigned int *)data);
			dprintf(INFO, "[cw2217] i2c_id(%d)\n",cw2217_i2c_idinfo);
		}
	}
#endif

	ret = cw221x_get_chip_id(&chip_id);
	
	cw_printk("lpp---cw221x_init chip_id=%x\n",chip_id);
	
	if(ret){
		return ret;
	}
	if(chip_id != IC_VCHIP_ID){
		return CW221X_ERROR_CHIP_ID;
	}
    cw_printk("lpp---cw221x_init IC_VCHIP_ID=%x\n",IC_VCHIP_ID);
	
	ret = cw221x_get_state();
	if (ret < 0)
		return ret;
    cw_printk("lpp---cw221x_init cw221x_get_state=%d %d\n",ret,CW221X_PROFILE_NEED_UPDATE);
	if (ret != 0) {
		ret = cw221x_config_start_ic();
		if (ret < 0)
			return ret;
	}

	cw_printk("cw221x_init w success!");
	return 0;
}

int cw221x_get_vol(void)
{
	int ret = 0;
	unsigned short temp_val_buff_H = 0;
	unsigned short temp_val_buff_L = 0;
	unsigned short temp_val_buff = 0;
	unsigned short ad_value = 0;
//prize add by lipengpeng 20220707 start 
	//ret = cw221x_read_word(REG_VCELL_H, &temp_val_buff);
	ret = cw_read(REG_VCELL_H, &temp_val_buff_H);
	cw_printk("lpp---temp_val_buff=%d\n",temp_val_buff_H);
	ret = cw_read(REG_VCELL_L, &temp_val_buff_L);
	cw_printk("lpp---temp_val_buff_L=%d\n",temp_val_buff_L);
	if(ret)
		return CW221X_ERROR_IIC;
//prize add by lipengpeng 20220707 end 	

	temp_val_buff= ((temp_val_buff_H << 8) | temp_val_buff_L);
	ad_value = temp_val_buff * 5 / 16;
	//*lp_vol = ad_value;
	
	return ad_value; 
}

#define UI_FULL     100
#define DECIMAL_MAX 80
#define DECIMAL_MIN 20 
int cw221x_get_capacity(void)
{
	int ret = 0;
	unsigned short temp_val_buff_H = 0;
	unsigned short temp_val_buff_L = 0;
	unsigned short temp_val_buff=0;
	unsigned short soc = 0;
	unsigned short soc_decimal = 0;
	unsigned short remainder = 0;
	unsigned short UI_SOC = 0;
//prize add by lipengpeng 20220707 start 
	ret = cw_read(REG_SOC_DECIMAL, &temp_val_buff_L);
	cw_printk("lpp---temp_val_buff_L=%d\n",temp_val_buff_L);
	ret = cw_read(REG_SOC_INT, &temp_val_buff_H);
	cw_printk("lpp---temp_val_buff_H=%d\n",temp_val_buff_H);
	if(ret)
		return CW221X_ERROR_IIC;
	
	soc = ((temp_val_buff_H << 8)| temp_val_buff_L);
	soc_decimal = temp_val_buff_L;
	
//prize add by lipengpeng 20220707 end 	

 	UI_SOC = (((unsigned long)soc * 256 + soc_decimal) * 100)/ (UI_FULL * 256);
	remainder = ((((unsigned long)soc * 256 + soc_decimal) * 100 * 100) / (UI_FULL * 256)) % 100;
	/* printf("soc = %d soc_decimal = %d ui_100 = %d UI_SOC = %d remainder = %d\n",
		soc, soc_decimal, UI_FULL, UI_SOC, remainder); */
	
	if(UI_SOC >= 100){
		UI_SOC = 100;
	}else if ((0 == UI_SOC) && (10 >= remainder)){
		UI_SOC = 0;
	}else{
		/* case 1 : aviod swing */
		if((UI_SOC >= (cw_bat.capacity - 1)) && (UI_SOC <= (cw_bat.capacity + 1)) 
			&& ((remainder > DECIMAL_MAX) || (remainder < DECIMAL_MIN)) && (UI_SOC != 100)){
			UI_SOC = cw_bat.capacity;
		}
	}
	
	//*lp_uisoc = UI_SOC;
	
	return UI_SOC;
}

int cw221x_get_temp(void)
{
	int ret = 0;
	int i;
	unsigned short reg_val = 0;
	int temp = 0;
	
	for(i=0;i<16;i++)
	{	
		ret = cw_read(REG_CHIP_ID+i, &reg_val);
		cw_printk("lpp---cw221x_get_temp=0x%x,%d\n",reg_val,i);
	}
	
	if(ret)
		return CW221X_ERROR_IIC;

	temp = (int)reg_val * 10 / 2 - 400;
	//*lp_temp = temp;
	
	return temp;
}

long get_complement_code(unsigned short raw_code)
{
	long complement_code = 0;
	int dir = 0;

	if (0 != (raw_code & 0x8000)){
		dir = -1;
		raw_code =  (~raw_code) + 1;
	}
	else{
		dir = 1;
	}

	complement_code = (long)raw_code * dir;

	return complement_code;
}

int cw221x_get_current(long *lp_current)
{
	int ret = 0;
	unsigned short temp_val_buff = 0;
	long current = 0;

	ret = cw221x_read_word(REG_CURRENT_H, &temp_val_buff);
	if(ret)
		return CW221X_ERROR_IIC;
	
	current = get_complement_code(temp_val_buff);
	current = current * 160 / RSENSE / 100;
	*lp_current = current;
	return 0; 
}

int cw221x_get_cycle_count(unsigned long *lp_count)
{
	int ret = 0;
	unsigned short temp_val_buff = 0;
	unsigned short cycle_buff = 0;

	ret = cw221x_read_word(REG_CYCLE_H, &temp_val_buff);
	if(ret)
		return CW221X_ERROR_IIC;

	cycle_buff = temp_val_buff/16;
	*lp_count = cycle_buff;
	return 0;	
}

int cw221x_get_soh(unsigned char *lp_soh)
{
	int ret = 0;
	unsigned short reg_val = 0;
	unsigned short SOH = 0;
	
	ret = cw_read(REG_SOH, &reg_val);
	if(ret)
		return CW221X_ERROR_IIC;

	SOH = reg_val;
	*lp_soh = SOH;
	
	return 0;
}

