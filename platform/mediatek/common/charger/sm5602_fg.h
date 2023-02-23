/*
 * drivers/battery/sm5602_fg.h
 *
 * Copyright (C) 2018 SiliconMitus
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

/* Define to prevent redundant inclusion */
#ifndef __SM5602_FG_H_
#define __SM5602_FG_H_

#define FG_INIT_MARK				0xA000

#define FG_PARAM_UNLOCK_CODE	  	0x3700
#define FG_PARAM_LOCK_CODE	  		0x0000
#define FG_TABLE_LEN				0x18//real table length -1
#define FG_ADD_TABLE_LEN			0x8//real table length -1
#define FG_INIT_B_LEN		    	0x7//real table length -1

#define ENABLE_EN_TEMP_IN           0x0200
#define ENABLE_EN_TEMP_EX           0x0400
#define ENABLE_EN_BATT_DET          0x0800
#define ENABLE_IOCV_MAN_MODE        0x1000
#define ENABLE_FORCED_SLEEP         0x2000
#define ENABLE_SLEEPMODE_EN         0x4000
#define ENABLE_SHUTDOWN             0x8000

#define ENABLE_EN_SHUTDOWN          0x0001

/* REG */
#define FG_REG_SOC_CYCLE			0x0B
#define FG_REG_SOC_CYCLE_CFG		0x15
#define FG_REG_ALPHA             	0x20
#define FG_REG_BETA              	0x21
#define FG_REG_RS                	0x24
#define FG_REG_RS_1     			0x25
#define FG_REG_RS_2            		0x26
#define FG_REG_RS_3            		0x27
#define FG_REG_RS_0            		0x29
#define FG_REG_END_V_IDX			0x2F
#define FG_REG_START_LB_V			0x30
#define FG_REG_START_CB_V			0x38
#define FG_REG_START_LB_I			0x40
#define FG_REG_START_CB_I			0x48
#define FG_REG_VOLT_CAL				0x70
#define FG_REG_CURR_IN_OFFSET		0x75
#define FG_REG_CURR_IN_SLOPE		0x76
#define FG_REG_RMC					0x84

#define FG_REG_SRADDR				0x8C
#define FG_REG_SRDATA				0x8D
#define FG_REG_SWADDR				0x8E
#define FG_REG_SWDATA				0x8F

#define FG_REG_AGING_CTRL			0x9C

#define FG_TEMP_TABLE_CNT_MAX       0x65

#define I2C_ERROR_COUNT_MAX			0x5

#define FG_PARAM_VERION       		0x1E

#define INIT_CHECK_MASK         	0x0010
#define DISABLE_RE_INIT         	0x0010

#define COMMON_PARAM_MASK		0xFF00
#define COMMON_PARAM_SHIFT		8
#define BATTERY_PARAM_MASK		0x00FF

#define BATTERY_TYPE_NUM		1

enum sm_fg_reg_idx {
	SM_FG_REG_DEVICE_ID = 0,
	SM_FG_REG_CNTL,
	SM_FG_REG_INT,
	SM_FG_REG_INT_MASK,
	SM_FG_REG_STATUS,
	SM_FG_REG_SOC,
	SM_FG_REG_OCV,
	SM_FG_REG_VOLTAGE,
	SM_FG_REG_CURRENT,
	SM_FG_REG_TEMPERATURE_IN,
	SM_FG_REG_TEMPERATURE_EX,
	SM_FG_REG_V_L_ALARM,
	SM_FG_REG_V_H_ALARM,	
	SM_FG_REG_A_H_ALARM,
	SM_FG_REG_T_IN_H_ALARM,
	SM_FG_REG_SOC_L_ALARM,
	SM_FG_REG_FG_OP_STATUS,
	SM_FG_REG_TOPOFFSOC,
	SM_FG_REG_PARAM_CTRL,
	SM_FG_REG_SHUTDOWN,	
	SM_FG_REG_VIT_PERIOD,
	SM_FG_REG_CURRENT_RATE,
	SM_FG_REG_BAT_CAP,
	SM_FG_REG_CURR_OFFSET,
	SM_FG_REG_CURR_SLOPE,	
	SM_FG_REG_MISC,
	SM_FG_REG_RESET,
	SM_FG_REG_RSNS_SEL,
	SM_FG_REG_VOL_COMP,
	NUM_REGS,
};

static unsigned char sm5602_regs[NUM_REGS] = {
	0x00, /* DEVICE_ID */
	0x01, /* CNTL */
	0x02, /* INT */
	0x03, /* INT_MASK */
	0x04, /* STATUS */
	0x05, /* SOC */
	0x06, /* OCV */
	0x07, /* VOLTAGE */
	0x08, /* CURRENT */
	0x09, /* TEMPERATURE_IN */
	0x0A, /* TEMPERATURE_EX */
	0x0C, /* V_L_ALARM */
	0x0D, /* V_H_ALARM */	
	0x0E, /* A_H_ALARM */
	0x0F, /* T_IN_H_ALARM */
	0x10, /* SOC_L_ALARM */
	0x11, /* FG_OP_STATUS */
	0x12, /* TOPOFFSOC */
	0x13, /* PARAM_CTRL */
	0x14, /* SHUTDOWN */
	0x1A, /* VIT_PERIOD */
	0x1B, /* CURRENT_RATE */
	0x62, /* BAT_CAP */	
	0x73, /* CURR_OFFSET */	
	0x74, /* CURR_SLOPE */
	0x90, /* MISC */
	0x91, /* RESET */
	0x95, /* RSNS_SEL */
	0x96, /* VOL_COMP */
};

enum battery_table_type {
	BATTERY_TABLE0 = 0,
	BATTERY_TABLE1,
	BATTERY_TABLE2,
	BATTERY_TABLE_MAX,
};

struct i2c_info
{
    u8 i2c_chl;
    u8 i2c_addr;
};

struct sm_fg_chip {
	struct i2c_info i2c;
	unsigned char regs[NUM_REGS];
	int	batt_id;
	
	/* Status Tracking */
	int	batt_soc;
	int batt_ocv;
	int	batt_volt;
	int	batt_curr;
    int topoff_soc;
	int topoff_margin;
    int top_off;
	
	/* DT */
	bool en_temp_ex;
	bool en_temp_in;
	bool en_batt_det;
	bool iocv_man_mode;
	int aging_ctrl;
	int batt_rsns;	/* Sensing resistor value */
	int cycle_cfg;
	int fg_irq_set;
	int low_soc1;
	int low_soc2;
	int v_l_alarm;
	int v_h_alarm;
	int battery_table_num;
	int misc;
    int batt_v_max;
	int min_cap;
	unsigned int common_param_version;
	int t_l_alarm_in; 
	int t_h_alarm_in;
	unsigned int t_l_alarm_ex;
	unsigned int t_h_alarm_ex;
	
	/* Battery Data */
	int battery_table[BATTERY_TABLE_MAX][FG_TABLE_LEN];
	signed short battery_temp_table[FG_TEMP_TABLE_CNT_MAX]; /* Degree */
	int alpha;
	int beta;
	int rs;
	int rs_value[4];
	int vit_period;
	int mix_value;
	const char		*battery_type;
	int volt_cal;
	int curr_offset;
	int curr_slope;
	int cap;

	unsigned int battery_param_version;	
};

struct sm5602_fg_batt_data {
	int	batt_id;	
	
	/* DT */
	bool en_temp_ex;
	bool en_temp_in;
	bool en_batt_det;
	bool iocv_man_mode;
	int aging_ctrl;
	int batt_rsns;	/* Sensing resistor value */
	int cycle_cfg;
	int fg_irq_set;
	int low_soc1;
	int low_soc2;
	int v_l_alarm;
	int v_h_alarm;
	int battery_table_num;
	int misc;
  //  int batt_v_max;
	//int min_cap;
	unsigned int common_param_version;
	int t_l_alarm_in; 
	int t_h_alarm_in;
	unsigned int t_l_alarm_ex;
	unsigned int t_h_alarm_ex;
	
	/* Battery Data */
	int battery_table[BATTERY_TABLE_MAX][FG_TABLE_LEN];
//	signed short battery_temp_table[FG_TEMP_TABLE_CNT_MAX]; /* Degree */
	unsigned int battery_param_version;
    int battery_type[3];
    //int battery_table0[FG_TABLE_LEN];
    //int battery_table1[FG_TABLE_LEN];
    //int battery_table2[FG_ADD_TABLE_LEN];
	int alpha;
	int beta;
	int rs;
	int rs_value[4];
	int vit_period;
    int volt_cal;
	int mix_value;
	//const char		*battery_type;
	int curr_offset;
	int curr_slope;
	//int cap;
	//int	batt_curr;
    int topoff_soc[3];
	//int topoff_margin;
    signed short batery_thermal_table[FG_TEMP_TABLE_CNT_MAX];



};

struct sm5602_fg_batt_type_data {
    struct sm5602_fg_batt_data type[BATTERY_TYPE_NUM];
};

int fg_init(void);
int fg_read_soc(void);
int fg_read_volt(void);
int fg_read_current(void);
unsigned char sm5602_fg_is_present(void);

#ifndef mdelay
#define mdelay(x)       spin((x) * 1000)
#endif

#endif
