/*
 * Fuelgauge battery driver
 *
 * Copyright (C) 2018 Siliconmitus
 *
 * This package is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */
#include <mt_i2c.h>
#include <platform/timer.h>
#include <libfdt.h>

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
#include <trace.h>

#include "sm5602_fg.h"
#include "sm5602_battery_table.h"

#define DATA_SIZE 2
#define I2C_DEFAULT_BUS_FREQ 100
#define I2C_OK			0
#define ENOMEM          12
#define EINVAL 			22
#define	ENODATA			61

#define ENABLE_INSPECTION_TABLE
#define ENABLE_IOCV_ADJ

#ifdef ENABLE_IOCV_ADJ
#define IOCV_MAX_ADJ_LEVEL 		0x1F33
#define IOCV_MIN_ADJ_LEVEL 		0x1D70
#define IOCI_MAX_ADJ_LEVEL 		0x1000
#define IOCI_MIN_ADJ_LEVEL 		0xCC
#define IOCV_I_SLOPE 	   		100
#define IOCV_I_OFFSET  	   		0
#endif



#define FG_VENDOR_ID	0x1 //SiliconMitus

#define sm_err(fmt, arg...)                                \
do {                                                    \
	 dprintf(CRITICAL,"sm5602: " fmt, ##arg);		\
} while (0)

//#define sm_printk(fmt, arg...)  printk("sm5602 : %s : " fmt, __FUNCTION__ ,##arg);

#define SMFG_NAME "sm5602"
#define SM_SLAVE_ADDR				0x71
#define SM_I2C_ID				0x00

static struct sm_fg_chip *sm = {0};

/******************************************************************
 *
 * fg charger i2c read/write
 *
 ****************************************************************/
int fg_write_word(unsigned char addr, unsigned short* value)
{
	int ret = 0;
	
    unsigned char write_buf[3] = {0};
	write_buf[0]= addr;
    write_buf[1] = (*value) & 0xff;
	write_buf[2] = (*value) >> 8;
	

    ret = mtk_i2c_write(sm->i2c.i2c_chl, sm->i2c.i2c_addr,write_buf, 3, NULL);
    if (ret != 0){
        dprintf(CRITICAL,"%s: I2CW[0x%02X] = 0x%hu failed, code = %d\n",__func__, addr, *value, ret);
	}
    else{
        //dprintf(CRITICAL,"%s: I2CW[0x%02X] = 0x%x%x\n",__func__, addr,write_buf[1],write_buf[2]);
	}
	
	return ret;
                
}

int fg_read_word(unsigned char addr, unsigned short *dataBuffer) 
{
	int ret = 0;
    unsigned char ret_data[3] = {0};

	
	ret_data[0] = addr;

    ret = mtk_i2c_write_read(sm->i2c.i2c_chl, sm->i2c.i2c_addr,ret_data, 1, 2, NULL);
    if (ret != 0){
        dprintf(CRITICAL,"%s: I2CR[0x%02X] failed, code = %d\n",__func__, addr, ret);
	}
    else {
        //dprintf(CRITICAL,"%s: I2CR[0x%02X] = %x %x\n",__func__, addr, ret_data[0],ret_data[1]);
        *dataBuffer = (ret_data[1] << 8) + ret_data[0];
    }
    return ret;
}

/* ************************************************************************* */
unsigned char fg_write_and_verify_word(unsigned char reg_addr, unsigned short *reg_data)
{
	unsigned short readback_data;
	int8_t retry;
	
	retry = 3;
	
	while(retry>0)
	{

		fg_write_word(reg_addr,reg_data);
		
		//gBS->Stall(480000);	// about 10ms
		
		fg_read_word(reg_addr, &readback_data);

		
		if(readback_data == (*reg_data))
		{
			return 0; 	// no error
		}
		else
		{
			retry--;
		}
	}
	
	return 1;	// error
}

static const u8 fg_dump_regs[] = {
	0x00, 0x01, 0x03, 0x04,
	0x05, 0x06, 0x07, 0x08,
	0x09, 0x0A, 0x0C, 0x0D,
	0x0E, 0x0F, 0x10, 0x11,
	0x12, 0x13, 0x14, 0x1A, 
	0x1B, 0x1C, 0x1E, 0x1f,
	0x62, 0x73, 0x74, 0x80,
	0x90, 0x91,	0x95, 0x96
};

static int fg_dump_debug(struct sm_fg_chip *sm)
{
	int i;
	int ret;
	unsigned short val = 0;

	for (i = 0; i < sizeof(fg_dump_regs); i++) {
		ret = fg_read_word(fg_dump_regs[i], &val);
		if (!ret)
			sm_err("Reg[0x%02X] = 0x%02X\n",
						fg_dump_regs[i], val);
	}
	return 0;
}

#define abs(x) (((x) < 0) ? -(x):(x))
#define MINVAL(a, b) ((a <= b) ? a : b)
#define MAXVAL(a, b) ((a > b) ? a : b)

static int fg_calculate_iocv(struct sm_fg_chip *sm)
{
	bool only_lb=false, sign_i_offset=0; //valid_cb=false, 
	int roop_start=0, roop_max=0, i=0, cb_last_index = 0, cb_pre_last_index =0;
	int lb_v_buffer[FG_INIT_B_LEN+1] = {0, 0, 0, 0, 0, 0, 0, 0};
	int lb_i_buffer[FG_INIT_B_LEN+1] = {0, 0, 0, 0, 0, 0, 0, 0};
	int cb_v_buffer[FG_INIT_B_LEN+1] = {0, 0, 0, 0, 0, 0, 0, 0};
	int cb_i_buffer[FG_INIT_B_LEN+1] = {0, 0, 0, 0, 0, 0, 0, 0};
	int i_offset_margin = 0x14, i_vset_margin = 0x67;
	int v_max=0, v_min=0, v_sum=0, lb_v_avg=0, cb_v_avg=0, lb_v_set=0, lb_i_set=0, i_offset=0;
	int i_max=0, i_min=0, i_sum=0, lb_i_avg=0, cb_i_avg=0, cb_v_set=0, cb_i_set=0;
	int lb_i_p_v_min=0, lb_i_n_v_max=0, cb_i_p_v_min=0, cb_i_n_v_max=0;

	unsigned short v_ret, i_ret = 0;
	int ret=0;

	unsigned short data = 0;

	ret = fg_read_word(FG_REG_END_V_IDX, &data);
	if (ret < 0) {
		sm_err("Failed to read FG_REG_END_V_IDX, ret = %d\n", ret);
		return ret;
	} else {
		sm_err("iocv_status_read = addr : 0x%x , data : 0x%x\n", FG_REG_END_V_IDX, data);
	}

	if((data & 0x0010) == 0x0000)
	{
		only_lb = true;
	}

    roop_max = (data & 0x000F);
    if(roop_max > FG_INIT_B_LEN)
    {
        roop_max = FG_INIT_B_LEN;
    }

	roop_start = FG_REG_START_LB_V;
	for (i = roop_start; i < roop_start + roop_max; i++)
	{
		ret = fg_write_word(0x8C, i);
		if (ret < 0) {
			sm_err("Failed to write 0x8C, %d ret = %d\n", i, ret);
			return ret;
		}
		mdelay(15); //15mS delay 		
		ret = fg_read_word(0x8D, &v_ret);
		if (v_ret & 0x8000) {
			ret = fg_read_word(0x8D, &v_ret);
			mdelay(15); //15mS delay
		}		
		if (ret < 0) {
			sm_err("Failed to read 0x%x, ret = %d\n",i, ret);
			return ret;
		}
		sm_err("iocv read = addr : 0x%x , data : 0x%x\n", i, v_ret);
		
		ret = fg_write_word(0x8C, i+0x10);
		if (ret < 0) {
			sm_err("Failed to write 0x8C, %d ret = %d\n", i+0x10, ret);
			return ret;
		}
		mdelay(15); //15mS delay 		
		ret = fg_read_word(0x8D, &i_ret);
		if (i_ret & 0x8000) {
			ret = fg_read_word(0x8D, &i_ret);
			mdelay(15); //15mS delay
		}		
		if (ret < 0) {
			sm_err("Failed to read 0x%x, ret = %d\n",i, ret);
			return ret;
		}
		sm_err("ioci read = addr : 0x%x , data : 0x%x\n", i+0x10, i_ret);

		if((i_ret&0x4000) == 0x4000)
		{
			i_ret = -(i_ret&0x3FFF);
		}

		lb_v_buffer[i-roop_start] = v_ret;
		lb_i_buffer[i-roop_start] = i_ret;

		if (i == roop_start)
		{
			v_max = v_ret;
			v_min = v_ret;
			v_sum = v_ret;
			i_max = i_ret;
			i_min = i_ret;
			i_sum = i_ret;
		}
		else
		{
			if(v_ret > v_max)
				v_max = v_ret;
			else if(v_ret < v_min)
				v_min = v_ret;
			v_sum = v_sum + v_ret;

			if(i_ret > i_max)
				i_max = i_ret;
			else if(i_ret < i_min)
				i_min = i_ret;
			i_sum = i_sum + i_ret;
		}

		if(abs(i_ret) > i_vset_margin)
		{
			if(i_ret > 0)
			{
				if(lb_i_p_v_min == 0)
				{
					lb_i_p_v_min = v_ret;
				}
				else
				{
					if(v_ret < lb_i_p_v_min)
						lb_i_p_v_min = v_ret;
				}
			}
			else
			{
				if(lb_i_n_v_max == 0)
				{
					lb_i_n_v_max = v_ret;
				}
				else
				{
					if(v_ret > lb_i_n_v_max)
						lb_i_n_v_max = v_ret;
				}
			}
		}
	}
	v_sum = v_sum - v_max - v_min;
	i_sum = i_sum - i_max - i_min;

	lb_v_avg = v_sum / (roop_max-2);
	lb_i_avg = i_sum / (roop_max-2);

	if(abs(lb_i_buffer[roop_max-1]) < i_vset_margin)
	{
		if(abs(lb_i_buffer[roop_max-2]) < i_vset_margin)
		{
			lb_v_set = MAXVAL(lb_v_buffer[roop_max-2], lb_v_buffer[roop_max-1]);
			if(abs(lb_i_buffer[roop_max-3]) < i_vset_margin)
			{
				lb_v_set = MAXVAL(lb_v_buffer[roop_max-3], lb_v_set);
			}
		}
		else
		{
			lb_v_set = lb_v_buffer[roop_max-1];
		}
	}
	else
	{
		lb_v_set = lb_v_avg;
	}

	if(lb_i_n_v_max > 0)
	{
		lb_v_set = MAXVAL(lb_i_n_v_max, lb_v_set);
	}

	if(roop_max > 3)
	{
		lb_i_set = (lb_i_buffer[2] + lb_i_buffer[3]) / 2;
	}

	if((abs(lb_i_buffer[roop_max-1]) < i_offset_margin) && (abs(lb_i_set) < i_offset_margin))
	{
		lb_i_set = MAXVAL(lb_i_buffer[roop_max-1], lb_i_set);
	}
	else if(abs(lb_i_buffer[roop_max-1]) < i_offset_margin)
	{
		lb_i_set = lb_i_buffer[roop_max-1];
	}
	else if(abs(lb_i_set) < i_offset_margin)
	{
		//lb_i_set = lb_i_set;
	}
	else
	{
		lb_i_set = 0;
	}

	i_offset = lb_i_set;

	i_offset = i_offset + 4;

	if(i_offset <= 0)
	{
		sign_i_offset = 1;
#ifdef IGNORE_N_I_OFFSET
		i_offset = 0;
#else
		i_offset = -i_offset;
#endif
	}

	i_offset = i_offset>>1;

	if(sign_i_offset == 0)
	{
		i_offset = i_offset|0x0080;
	}
    i_offset = i_offset | i_offset<<8;

	sm_err("iocv_l_max=0x%x, iocv_l_min=0x%x, iocv_l_avg=0x%x, lb_v_set=0x%x, roop_max=%d \n",
			v_max, v_min, lb_v_avg, lb_v_set, roop_max);
	sm_err("ioci_l_max=0x%x, ioci_l_min=0x%x, ioci_l_avg=0x%x, lb_i_set=0x%x, i_offset=0x%x, sign_i_offset=%d\n",
			i_max, i_min, lb_i_avg, lb_i_set, i_offset, sign_i_offset);

	if(!only_lb)
	{
		roop_start = FG_REG_START_CB_V;
		roop_max = 6;
		for (i = roop_start; i < roop_start + roop_max; i++)
		{
			ret = fg_write_word(0x8C, i);
			if (ret < 0) {
				sm_err("Failed to write 0x8C, %d ret = %d\n", i, ret);
				return ret;
			}
			mdelay(15); //15mS delay 		
			ret = fg_read_word(0x8D, &v_ret);
			if (v_ret & 0x8000) {
				ret = fg_read_word(0x8D, &v_ret);
				mdelay(15); //15mS delay
			}				
			if (ret < 0) {
				sm_err("Failed to read 0x%x, ret = %d\n",i, ret);
				return ret;
			}
			ret = fg_write_word(0x8C, i+0x10);
			if (ret < 0) {
				sm_err("Failed to write 0x8C, %d ret = %d\n", i+0x10, ret);
				return ret;
			}
			mdelay(15); //15mS delay 		
			ret = fg_read_word(0x8D, &i_ret);
			if (i_ret & 0x8000) {
				ret = fg_read_word(0x8D, &i_ret);
				mdelay(15); //15mS delay
			}				
			if (ret < 0) {
				sm_err("Failed to read 0x%x, ret = %d\n",i, ret);
				return ret;
			}

			if((i_ret&0x4000) == 0x4000)
			{
				i_ret = -(i_ret&0x3FFF);
			}

			cb_v_buffer[i-roop_start] = v_ret;
			cb_i_buffer[i-roop_start] = i_ret;

			if (i == roop_start)
			{
				v_max = v_ret;
				v_min = v_ret;
				v_sum = v_ret;
				i_max = i_ret;
				i_min = i_ret;
				i_sum = i_ret;
			}
			else
			{
				if(v_ret > v_max)
					v_max = v_ret;
				else if(v_ret < v_min)
					v_min = v_ret;
				v_sum = v_sum + v_ret;

				if(i_ret > i_max)
					i_max = i_ret;
				else if(i_ret < i_min)
					i_min = i_ret;
				i_sum = i_sum + i_ret;
			}

			if(abs(i_ret) > i_vset_margin)
			{
				if(i_ret > 0)
				{
					if(cb_i_p_v_min == 0)
					{
						cb_i_p_v_min = v_ret;
					}
					else
					{
						if(v_ret < cb_i_p_v_min)
							cb_i_p_v_min = v_ret;
					}
				}
				else
				{
					if(cb_i_n_v_max == 0)
					{
						cb_i_n_v_max = v_ret;
					}
					else
					{
						if(v_ret > cb_i_n_v_max)
							cb_i_n_v_max = v_ret;
					}
				}
			}
		}
		v_sum = v_sum - v_max - v_min;
		i_sum = i_sum - i_max - i_min;

		cb_v_avg = v_sum / (roop_max-2);
		cb_i_avg = i_sum / (roop_max-2);

		cb_last_index = (data & 0x000F)-7; //-6-1
		if(cb_last_index < 0)
		{
			cb_last_index = 5;
		}

        if(cb_last_index > FG_INIT_B_LEN)
        {
            cb_last_index = FG_INIT_B_LEN;
        }

		for (i = roop_max; i > 0; i--)
		{
			if(abs(cb_i_buffer[cb_last_index]) < i_vset_margin)
			{
				cb_v_set = cb_v_buffer[cb_last_index];
				if(abs(cb_i_buffer[cb_last_index]) < i_offset_margin)
				{
					cb_i_set = cb_i_buffer[cb_last_index];
				}

				cb_pre_last_index = cb_last_index - 1;
				if(cb_pre_last_index < 0)
				{
					cb_pre_last_index = 5;
				}

				if(abs(cb_i_buffer[cb_pre_last_index]) < i_vset_margin)
				{
					cb_v_set = MAXVAL(cb_v_buffer[cb_pre_last_index], cb_v_set);
					if(abs(cb_i_buffer[cb_pre_last_index]) < i_offset_margin)
					{
						cb_i_set = MAXVAL(cb_i_buffer[cb_pre_last_index], cb_i_set);
					}
				}
			}
			else
			{
				cb_last_index--;
				if(cb_last_index < 0)
				{
					cb_last_index = 5;
				}
			}
		}

		if(cb_v_set == 0)
		{
			cb_v_set = cb_v_avg;
			if(cb_i_set == 0)
			{
				cb_i_set = cb_i_avg;
			}
		}

		if(cb_i_n_v_max > 0)
		{
			cb_v_set = MAXVAL(cb_i_n_v_max, cb_v_set);
		}

		if(abs(cb_i_set) < i_offset_margin)
		{
			if(cb_i_set > lb_i_set)
			{
				i_offset = cb_i_set;
				i_offset = i_offset + 4;

				if(i_offset <= 0)
				{
					sign_i_offset = 1;
#ifdef IGNORE_N_I_OFFSET
					i_offset = 0;
#else
					i_offset = -i_offset;
#endif
				}

				i_offset = i_offset>>1;

				if(sign_i_offset == 0)
				{
					i_offset = i_offset|0x0080;
				}
                i_offset = i_offset | i_offset<<8;

			}
		}

		sm_err("iocv_c_max=0x%x, iocv_c_min=0x%x, iocv_c_avg=0x%x, cb_v_set=0x%x, cb_last_index=%d\n",
				v_max, v_min, cb_v_avg, cb_v_set, cb_last_index);
		sm_err("ioci_c_max=0x%x, ioci_c_min=0x%x, ioci_c_avg=0x%x, cb_i_set=0x%x, i_offset=0x%x, sign_i_offset=%d\n",
				i_max, i_min, cb_i_avg, cb_i_set, i_offset, sign_i_offset);

	}

    /* final set */
	sm_err("cb_i_set=%d, i_vset_margin=%d, only_lb=%d\n", cb_i_set, i_vset_margin, only_lb);
	if (only_lb) 
	{
		ret = MAXVAL(lb_v_set, cb_i_n_v_max);
		cb_i_set = lb_i_avg;
	}
	else 
	{
		ret = cb_v_set;
		cb_i_set = cb_i_avg;
	}

#ifdef ENABLE_IOCV_ADJ
	if (((ret < IOCV_MAX_ADJ_LEVEL) && (ret > IOCV_MIN_ADJ_LEVEL)) && ((abs(cb_i_set) < IOCI_MAX_ADJ_LEVEL) && (abs(cb_i_set) > IOCI_MIN_ADJ_LEVEL)))
	{
		cb_v_set = ret;
		ret = ret - (((cb_i_set * IOCV_I_SLOPE) + IOCV_I_OFFSET) / 1000);
		sm_err("first boot vbat-soc adjust 1st_v=0x%x, 2nd_v=0x%x, all_i=0x%x\n", cb_v_set, ret, cb_i_set);
	}
#endif

    sm_err("iocv ret=%d, table=%d \n",ret, sm->battery_table[BATTERY_TABLE0][FG_TABLE_LEN-1]);
    if(ret > sm->battery_table[BATTERY_TABLE0][FG_TABLE_LEN-1])
    {
        sm_err("iocv ret change hex 0x%x -> 0x%x ret is big than table data\n",ret, sm->battery_table[BATTERY_TABLE0][FG_TABLE_LEN-1]);
	    sm_err("iocv ret change dec %d -> %d ret is big than table data\n",ret, sm->battery_table[BATTERY_TABLE0][FG_TABLE_LEN-1]);
        ret = sm->battery_table[BATTERY_TABLE0][FG_TABLE_LEN-1];
    }
    else if(ret < sm->battery_table[BATTERY_TABLE0][0])
    {
        sm_err("iocv ret change 0x%x -> 0x%x \n", ret, (sm->battery_table[BATTERY_TABLE0][0] + 0x10));
        ret = sm->battery_table[BATTERY_TABLE0][0] + 0x10;
    }

	return ret;
}

static int fg_reg_init(void)
{
	 int cnt = 0, ret = 0;
	 unsigned short batt_rsns = 0 , val = 0; //0 : 5mohm
	 unsigned short data;
	 
	 sm_err("sm5602_fg_reg_init START!!\n");

	 ret = fg_write_word(sm5602_regs[SM_FG_REG_RSNS_SEL], &batt_rsns);
	 if (ret < 0) {
	  sm_err("Failed to write SM_FG_REG_RSNS_SEL, ret = %d\n", ret);
	  return ret;
	 } else {
	  sm_err("Write SM_FG_REG_RSNS_SEL = 0x%x\n", batt_rsns);
	 }

	 do 
	 {
		val = (FG_PARAM_UNLOCK_CODE | ((0x0 & 0x0003) << 6) | (FG_TABLE_LEN-1));
		ret = fg_write_word(sm5602_regs[SM_FG_REG_PARAM_CTRL], &val);//MANUAL_TABLE
		if (ret < 0){
			sm_err("Failed to write param_ctrl unlock, ret = %d\n", ret);
			return ret;
		}
		else
		{
			sm_err("Param Unlock\n");
		}
	  //mdelay(60);
	  ret = fg_read_word(sm5602_regs[SM_FG_REG_FG_OP_STATUS], &data);
	  if (ret < 0){
		sm_err("Failed to read FG_OP_STATUS, ret = %d\n", ret); 
	  } else {
		sm_err(" FG_OP_STATUS = 0x%x\n", data);
	  }
	  cnt++;
	 } while(((data & 0x03)!=0x03) && cnt <= 3);
	 val = 0x3256;
	 ret = fg_write_word(sm5602_regs[SM_FG_REG_VIT_PERIOD], &val);
	 if (ret < 0) {
	  sm_err("Failed to write vit period, ret = %d\n", ret);
	  return ret;
	 }
	 else{
		sm_err("vit period\n");
	 }
	 return 1;
}

#define FG_SOFT_RESET	0x1A6 
static int fg_reset(void)
{
    int ret;
	unsigned short val = FG_SOFT_RESET;

	if (!sm) {
		sm_err("%s : not allocated sm\n", __func__);
		return -ENOMEM;
	}
	
    ret = fg_write_word(sm->regs[SM_FG_REG_RESET], &val);
	if (ret < 0) {
		sm_err("could not reset, ret=%d\n", ret);
		return ret;
	}
	sm_err("=== %s ===\n", __func__);

	mdelay(800);

    return 0;
}

static int fg_check_reg_init_need(void)
{ 
	int ret = 0;
	unsigned short data = 0;
	unsigned short param_ver = 0;

	if (!sm) {
		sm_err("%s : not allocated sm\n", __func__);
		return -ENOMEM;
	}
	
	ret = fg_read_word(sm->regs[SM_FG_REG_FG_OP_STATUS], &data);
	if (ret < 0) {
			sm_err("Failed to read param_ctrl unlock, ret = %d\n", ret);
			return ret;
	} else {
		sm_err("FG_OP_STATUS = 0x%x\n", data);	

		ret = fg_read_word(FG_PARAM_VERION, &param_ver);
		if (ret < 0) {
				sm_err("Failed to read FG_PARAM_VERION, ret = %d\n", ret);
				return ret;
		} 

		sm_err("param_ver = 0x%x, common_param_version = 0x%x, battery_param_version = 0x%x\n", param_ver, sm->common_param_version, sm->battery_param_version);

		if ((((param_ver & COMMON_PARAM_MASK) >> COMMON_PARAM_SHIFT) >= sm->common_param_version) 
			&& ((param_ver & BATTERY_PARAM_MASK) >= sm->battery_param_version)) {

			if ((data & INIT_CHECK_MASK) == DISABLE_RE_INIT) {
				sm_err("SM_FG_REG_FG_OP_STATUS : 0x%x , return FALSE NO init need\n", data);
				return 0;
			}
			else
			{
				sm_err("SM_FG_REG_FG_OP_STATUS : 0x%x , return TRUE init need!!!!\n", data);
				return 1;
			}
		}
		else
		{
			if ((data & INIT_CHECK_MASK) == DISABLE_RE_INIT) {
				// Step1. Turn off charger
				// Step2. FG Reset
				ret = fg_reset();
				if (ret < 0) {
					sm_err("%s: fail to do reset(%d) retry\n", __func__, ret);
					ret = fg_reset();
					if (ret < 0) {
						sm_err("%s: fail to do reset(%d) retry_2\n", __func__, ret);
						ret = fg_reset();
						if (ret < 0) {
							sm_err("%s: fail to do reset(%d) reset fail 3 times!!!! return 0!!!!\n", __func__, ret);
							return 0;
						}
					}
				}
				// Step3. If Step1 was charging-on, turn on charger.
				
				sm_err("SM_FG_REG_FG_OP_STATUS : 0x%x , return TRUE init need because diff_ver SW reset!!!!\n", data);
				return 1;

			}
			else
			{
				sm_err("SM_FG_REG_FG_OP_STATUS : 0x%x , return TRUE init need!!!!\n", data);
				return 1;
			}
		}
	}
}

/* get battery cell type index */
static int get_battery_id(struct sm_fg_chip *sm)
{ 
	return 0;
}

#define PROPERTY_NAME_SIZE 128

static int fg_common_parse_dt(void)
{
    int battery_id = 0;

    battery_id = get_battery_id(sm);

	if (!sm) {
		sm_err("%s : not allocated sm\n", __func__);
		return -ENOMEM;
	}
	
    /* EN TEMP EX/IN */
	sm->en_temp_ex = sm5602_batt_type.type[battery_id].en_temp_ex;	
	sm_err("Temperature EX enabled = %d\n", sm5602_batt_type.type[battery_id].en_temp_ex);

    sm->en_temp_in = sm5602_batt_type.type[battery_id].en_temp_in;
    sm_err("Temperature IN enabled = %d\n", sm5602_batt_type.type[battery_id].en_temp_in);

	/* EN BATT DET  */	
	sm->en_batt_det = sm5602_batt_type.type[battery_id].en_batt_det;
	sm_err("Batt Det enabled = %d\n", sm5602_batt_type.type[battery_id].en_batt_det);

    /* MISC */
    sm->misc = sm5602_batt_type.type[battery_id].misc;	
	sm_err("misc = 0x%x\n", sm->misc);	
	
	/* IOCV MAN MODE */	
	sm->iocv_man_mode = sm5602_batt_type.type[battery_id].iocv_man_mode;
	sm_err("iocv_man_mode = %d\n", sm5602_batt_type.type[battery_id].iocv_man_mode);

    /* Aging */
	sm->aging_ctrl = sm5602_batt_type.type[battery_id].aging_ctrl;
	sm_err("aging_ctrl = 0x%x\n", sm5602_batt_type.type[battery_id].aging_ctrl);

    /* SOC Cycle cfg */
    sm->cycle_cfg = sm5602_batt_type.type[battery_id].cycle_cfg;
	sm_err("cycle_cfg = 0x%x\n", sm5602_batt_type.type[battery_id].cycle_cfg);

    /* RSNS */		
    sm->batt_rsns = sm5602_batt_type.type[battery_id].batt_rsns;
	sm_err("rsns = %d\n", sm5602_batt_type.type[battery_id].batt_rsns);

    /* IRQ Mask */
    sm->fg_irq_set = sm5602_batt_type.type[battery_id].fg_irq_set;
	sm_err("fg_irq_set = 0x%x\n", sm5602_batt_type.type[battery_id].fg_irq_set);

    /* LOW SOC1/2 */
    sm->low_soc1 = sm5602_batt_type.type[battery_id].low_soc1;
    sm_err("low_soc1 = 0x%x\n", sm5602_batt_type.type[battery_id].low_soc1);

    sm->low_soc2 = sm5602_batt_type.type[battery_id].low_soc2;	
    sm_err("low_soc2 = 0x%x\n", sm5602_batt_type.type[battery_id].low_soc2);

    /* V_L/H_ALARM */
    sm->v_l_alarm = sm5602_batt_type.type[battery_id].v_l_alarm;
    sm_err("v_l_alarm = 0x%x\n", sm5602_batt_type.type[battery_id].v_l_alarm);

    sm->v_h_alarm = sm5602_batt_type.type[battery_id].v_h_alarm;
    sm_err("v_h_alarm = 0x%x\n", sm5602_batt_type.type[battery_id].v_h_alarm);

    /* T_IN_H/L_ALARM */
    sm->t_l_alarm_in = sm5602_batt_type.type[battery_id].t_l_alarm_in;	
    sm_err("t_l_alarm_in = 0x%x\n", sm5602_batt_type.type[battery_id].t_l_alarm_in);

    sm->t_h_alarm_in = sm5602_batt_type.type[battery_id].t_h_alarm_in;
    sm_err("t_h_alarm_in = 0x%x\n", sm5602_batt_type.type[battery_id].t_h_alarm_in);

    /* T_EX_H/L_ALARM */
    sm->t_l_alarm_ex = sm5602_batt_type.type[battery_id].t_l_alarm_ex;
    sm_err("t_l_alarm_ex = 0x%x\n", sm5602_batt_type.type[battery_id].t_l_alarm_ex);

    sm->t_h_alarm_ex = sm5602_batt_type.type[battery_id].t_h_alarm_ex;
    sm_err("t_h_alarm_ex = 0x%x\n", sm5602_batt_type.type[battery_id].t_h_alarm_ex);

    /* Battery Table Number */
    sm->battery_table_num = sm5602_batt_type.type[battery_id].battery_table_num;
    sm_err("battery_table_num = %d\n", sm5602_batt_type.type[battery_id].battery_table_num);

    /* Paramater Number */
    sm->common_param_version = sm5602_batt_type.type[battery_id].common_param_version;
    sm_err("common_param_version = %d\n", sm5602_batt_type.type[battery_id].common_param_version);
	
	return 0;
}

static int fg_battery_parse_dt(void)
{
	//char prop_name[PROPERTY_NAME_SIZE];
	int battery_id = -1;
	//int battery_temp_table[FG_TEMP_TABLE_CNT_MAX];
//	int table[FG_TABLE_LEN];
//	int rs_value[4];
//	int topoff_soc[3];
	//int temp_offset[6];
//	int temp_cal[10];
//	int ext_temp_cal[10];
	//int battery_type[3];
//	int set_temp_poff[4];
	//int ret;
	int i, j;
	//int offset;

	if (!sm) {
		sm_err("%s : not allocated sm\n", __func__);
		return -ENOMEM;
	}

	/* battery_id*/
	battery_id = get_battery_id(sm);
	sm_err("battery id = %d\n", battery_id);

	/*  battery_table*/
	for (i = BATTERY_TABLE0; i < BATTERY_TABLE2; i++) {
		for (j = 0; j < FG_TABLE_LEN; j++) {
            if (i == BATTERY_TABLE0)
                sm->battery_table[i][j] = sm5602_batt_type.type[battery_id].battery_table[BATTERY_TABLE0][j];
            else if (i == BATTERY_TABLE1)
                sm->battery_table[i][j] = sm5602_batt_type.type[battery_id].battery_table[BATTERY_TABLE1][j];
            sm_err("<table[%d][%d] 0x%x>\n", i, j, sm->battery_table[i][j]);
		}
	}

	i = BATTERY_TABLE2;

	for(j=0; j < FG_ADD_TABLE_LEN; j++)
	{
		sm->battery_table[i][j] = sm5602_batt_type.type[battery_id].battery_table[BATTERY_TABLE2][j];
		sm_err("<table[%d][%d] 0x%x>\n", i, j, sm->battery_table[i][j]);
	}
	
    /* rs */
	sm->rs = sm5602_batt_type.type[battery_id].rs;
	sm_err("rs = <0x%x>\n", sm->rs);

    /* alpha */
	sm->alpha = sm5602_batt_type.type[battery_id].alpha;
	sm_err("alpha = <0x%x>\n", sm->alpha);

    /* beta */
	sm->beta = sm5602_batt_type.type[battery_id].beta;
	sm_err("beta = <0x%x>\n", sm->beta);

	/* rs_value*/
	for (i = 0; i < 4; i++) {
		sm->rs_value[i] = sm5602_batt_type.type[battery_id].rs_value[i];
	}
	sm_err("rs_value = <0x%x 0x%x 0x%x 0x%x>\n",
		sm->rs_value[0], sm->rs_value[1], sm->rs_value[2], sm->rs_value[3]);

	/* vit_period*/
	sm->vit_period = sm5602_batt_type.type[battery_id].vit_period;
	sm_err("vit_period = <0x%x>\n", sm->vit_period);

    /* battery_type*/
    sm->batt_v_max = sm5602_batt_type.type[battery_id].battery_type[0];
    sm->min_cap = sm5602_batt_type.type[battery_id].battery_type[1];
    sm->cap = sm5602_batt_type.type[battery_id].battery_type[2];
    sm_err("battery_type = <%d %d %d>\n",
        sm->batt_v_max, sm->min_cap, sm->cap);

    // TOPOFF SOC
    sm->topoff_soc = sm5602_batt_type.type[battery_id].topoff_soc[0];
    sm->top_off = sm5602_batt_type.type[battery_id].topoff_soc[1];
	sm->topoff_margin = sm5602_batt_type.type[battery_id].topoff_soc[2];
    sm_err("topoff = <%d %d %d>\n",
        sm->topoff_soc, sm->top_off, sm->topoff_margin);

    // Mix
	sm->mix_value = sm5602_batt_type.type[battery_id].mix_value;
    sm_err("mix_value = <%d>\n", sm->mix_value);

    /* VOLT CAL */
	sm->volt_cal = sm5602_batt_type.type[battery_id].volt_cal;
    sm_err("volt_cal = <0x%x>\n", sm->volt_cal);

	/* CURR OFFSET */
	sm->curr_offset = sm5602_batt_type.type[battery_id].curr_offset;
	sm_err("curr_offset = <0x%x>\n", sm->curr_offset);

	/* CURR SLOPE */
	sm->curr_slope = sm5602_batt_type.type[battery_id].curr_slope;
	sm_err("curr_slope = <0x%x>\n", sm->curr_slope);

	/* get battery_temp_table*/
	for (i = 0; i < FG_TEMP_TABLE_CNT_MAX; i++) {
		 sm->battery_temp_table[i] = sm5602_batt_type.type[battery_id].batery_thermal_table[i];
		 sm_err("battery_temp_table = <battery_temp_table[%d] 0x%x>\n",
			 i,	sm->battery_temp_table[i]);
	}

    /* Battery Paramter */
	sm->battery_param_version = sm5602_batt_type.type[battery_id].battery_param_version;
	sm_err("battery_param_version = <0x%x>\n", sm->battery_param_version);
	
	return 0;
}

static bool hal_fg_init(void)
{
	sm_err("sm5602 hal_fg_init...\n");

	/* Load common data from DTS*/
	//fg_common_parse_dt();
	/* Load battery data from DTS*/
	//fg_battery_parse_dt();

	if(!fg_reg_init())
        return false;

	sm_err("hal fg init OK\n");
	return true;
}

/*
 *	Initialazation FG
 */ 
int fg_init(void)
{
	int ret = 0;
	unsigned char *regs;

	sm_err("fg_init\n");

	if (!sm) {
		if (!(sm = malloc(sizeof(struct sm_fg_chip)))) {
			sm_err("SM Memory Allocation Fail\n");
			ret = -ENOMEM;
			goto err_0;
		} else
			memset(sm, 0, sizeof(struct sm_fg_chip));
		sm_err("SM Memory Allocation Done\n");
	} else {
		sm_err("SM Memory Aleady allocated\n");
	}
	
	sm->batt_soc	= -ENODATA;
	sm->batt_volt	= -ENODATA;
	sm->batt_curr	= -ENODATA;
	sm->i2c.i2c_chl = SM_I2C_ID;
	sm->i2c.i2c_addr = SM_SLAVE_ADDR;
	
	regs = sm5602_regs;
	memcpy(sm->regs, regs, NUM_REGS);

	if (hal_fg_init() == false) {
	    sm_err("Failed to Initialize Fuelgauge\n");
		ret = -ENODATA;
        goto err_1; 	
	}

	fg_dump_debug(sm);

	sm_err("sm fuel gauge probe successfully, SM5602\n");

	return 0;

err_1:
	free(sm);
err_0:
	return ret;
}

/* ************************************************************************* */
int fg_read_soc(void)
{
	int ret;
	unsigned int soc = 0;
	unsigned short data = 0;

	if (!sm) {
		sm_err("%s : not allocated sm\n", __func__);
		return -ENOMEM;
	}

	ret = fg_read_word(sm->regs[SM_FG_REG_SOC], &data);
	if (ret < 0) {
		sm_err("could not read SOC, ret = %d\n", ret);
		return ret;
	} else {
		/*integer bit;*/
		soc = ((data&0x7f00)>>8) * 10;
		/* integer + fractional bit*/
		soc = soc + (((data&0x00ff)*10)/256);
		
		if (data & 0x8000) {
			sm_err("fg_read_soc data=%d\n",data);
			soc *= -1;
		}
	}

	 sm_err("soc=%d(0x%x)\n",soc, data);
	return soc; // xx = x.x%
}

/* ************************************************************************* */
int fg_read_volt(void)
{
	int volt, ret = 0;
	unsigned short data = 0;

	if (!sm) {
		sm_err("%s : not allocated sm\n", __func__);
		return -ENOMEM;
	}

	ret = fg_read_word(sm->regs[SM_FG_REG_VOLTAGE], &data);
	if (ret < 0) {
		sm_err("could not read voltage, ret = %d\n", ret);
		return ret;
	}  else {
		volt = 1800 * (data & 0x7FFF) / 19622;
		if (data&0x8000)
			volt *= -1;

		volt += 2700;
	}
	return volt; //mV
}

/* ************************************************************************* */
int fg_read_current(void)
{
	int ret = 0, rsns = 0;
	unsigned short data = 0;
	int curr = 0;

	if (!sm) {
		sm_err("%s : not allocated sm\n", __func__);
		return -ENOMEM;
	}

	ret = fg_read_word(sm->regs[SM_FG_REG_CURRENT], &data);
	if (ret < 0) {
		sm_err("could not read current, ret = %d\n", ret);
		return ret;
	} else {
		if (sm->batt_rsns == -EINVAL) {
			sm_err("could not read sm->batt_rsns, rsns = 10mohm\n");
			rsns = 10;
		} else {
			sm->batt_rsns == 0 ? rsns = 5 : (rsns = sm->batt_rsns*10);
		}

		curr = ((data & 0x7FFF) * 1250 / 511 / rsns );

		if (data & 0x8000)
			curr *= -1;
	}

	sm_err("curr = %d,data=%d\n",curr,data);

	return curr; //mA
}

/* ************************************************************************* */
unsigned char sm5602_fg_is_present(void)
{
	unsigned char fuelgauge_is_present;
	unsigned short data = 0;

	if (!sm) {
		sm_err("%s : not allocated sm\n", __func__);
		return -ENOMEM;
	}

	fg_read_word(sm->regs[SM_FG_REG_DEVICE_ID], &data);
	
	if(((data & 0x00F0) >> 4) == FG_VENDOR_ID)
		fuelgauge_is_present=1;
	else
		fuelgauge_is_present=0;    

	return fuelgauge_is_present;
}
