// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2022 Southchip Semiconductor Technology(Shanghai) Co., Ltd.
 */

#include <debug.h>
#include <errno.h>
#include <mt_i2c.h>
#include <platform/timer.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <trace.h>

#include "sc8989x_reg.h"

#define SC8989X_LK_DRV_VERSION "1.0.0_SC"

/* ================= */
/* Internal variable */
/* ================= */

enum sc8989x_part_no {
	SYV6970QCC = 0x01,
	SC89890H = 0x04,
	SC89890D = 0x05,
};

enum sc8989x_chg_mode {
    SC8989X_CHG_MODE_UNKNOWN,
    SC8989X_CHG_MODE_TRICHG,
    SC8989X_CHG_MODE_PRECHG,
    SC8989X_CHG_MODE_FASTCHG,
};

static const char * const sc8989x_chg_mode_name[] = {
    "not-charge",
    "pre-charge",
    "fast-charge",
	"term-charge",
};

struct i2c_info
{
    u8 i2c_chl;
    u8 i2c_addr;
};


struct sc8989x_chg_info {
    struct i2c_info i2c;
    u8 part_no;
    u8 rev;
    u32 iprec;
    u32 ichg;
    u32 aicr;
    u32 mivr;
    u32 cv;
};

static struct sc8989x_chg_info g_sc8989x = {
    .i2c = {
        .i2c_chl = 0x02,
        .i2c_addr = 0x6B,
    },
    .part_no = 0x00,
    .rev = 0x00,
    .iprec = 150,
    .ichg = 2000,
    .aicr = 1000,
    .mivr = 4500,
    .cv = 4400,
};

/* ========================= */
/* I2C operations */
/* ========================= */
static int sc8989x_i2c_write_byte(struct sc8989x_chg_info *info,
                                 u8 cmd, u8 data)
{
    int ret = 0;
    u8 write_buf[2] = {cmd, data};

    ret = mtk_i2c_write(info->i2c.i2c_chl, info->i2c.i2c_addr,
                        write_buf, 2, NULL);
    if (ret != 0)
        TRACEF("%s: I2CW[0x%02X] = 0x%02X failed, code = %d\n",
                __func__, cmd, data, ret);
    else
        TRACEF("%s: I2CW[0x%02X] = 0x%02X\n",
                __func__, cmd, data);
    return ret;
}

static int sc8989x_i2c_read_byte(struct sc8989x_chg_info *info,
                                u8 cmd, u8 *data)
{
    int ret = 0;
    u8 ret_data = cmd;

    ret = mtk_i2c_write_read(info->i2c.i2c_chl, info->i2c.i2c_addr,
                            &ret_data, 1, 1, NULL);
    if (ret != 0)
        TRACEF("%s: I2CR[0x%02X] failed, code = %d\n",
                __func__, cmd, ret);
    else {
        TRACEF("%s: I2CR[0x%02X] = 0x%02X\n",
                __func__, cmd, ret_data);
        *data = ret_data;
    }
    return ret;
}

static int sc8989x_i2c_update_bits(struct sc8989x_chg_info *info,
                                  u8 cmd, u8 mask, u8 data)
{
    int ret = 0;
    u8 reg_data = 0;

    ret = sc8989x_i2c_read_byte(info, cmd, &reg_data);
    if (ret != 0)
        return ret;
    reg_data = reg_data & 0xFF;
    reg_data &= ~mask;
    reg_data |= (data & mask);
    return sc8989x_i2c_write_byte(info, cmd, reg_data);
}

static bool sc8989x_is_hw_exist(struct sc8989x_chg_info *info)
{
	int ret;
	u8 data;
	u8 partno, rev;

	ret = sc8989x_i2c_read_byte(info, SC8989X_REG_14, &data);
	TRACEF("%s: [0x14] %x\n", __func__, data);
	if (ret < 0)
		return 0;
	
	partno = (data & SC8989X_PN_MASK) >> SC8989X_PN_SHIFT;
	rev = (data & SC8989X_DEV_REV_MASK) >> SC8989X_DEV_REV_SHIFT;
	if ((partno != SC89890H) && (partno != SC89890D)) {
		TRACEF("%s: incorrect part number, not sc8989x\n", __func__);
		return false;
	}

	TRACEF("%s: chip PN:%d, rev:%d\n", __func__, partno, rev);

	info->part_no = partno;
    info->rev = rev;
	return true;	
}

static int sc8989x_enable_term(struct sc8989x_chg_info *info, bool enable)
{
	u8 val;
	int ret;

	if (enable)
		val = SC8989X_TERM_ENABLE;
	else
		val = SC8989X_TERM_DISABLE;

	val <<= SC8989X_EN_TERM_SHIFT;

	ret = sc8989x_i2c_update_bits(info, SC8989X_REG_07,
				SC8989X_EN_TERM_MASK, val);

	return ret;
}

static int sc8989x_enable_charger(struct sc8989x_chg_info *info)
{
	int ret;
	u8 val = SC8989X_CHG_ENABLE << SC8989X_CHG_CONFIG_SHIFT;

	ret = sc8989x_i2c_update_bits(info, SC8989X_REG_03,
				SC8989X_CHG_CONFIG_MASK, val);
	return ret;
}

static int sc8989x_disable_charger(struct sc8989x_chg_info *info)
{
	int ret;

	u8 val = SC8989X_CHG_DISABLE << SC8989X_CHG_CONFIG_SHIFT;

	ret = sc8989x_i2c_update_bits(info, SC8989X_REG_03,
				SC8989X_CHG_CONFIG_MASK, val);

	return ret;
}

/* =========================================================== */
/* The following is implementation for interface of sc8989x */
/* =========================================================== */

static int sc8989x_dump_register(void)
{
	struct sc8989x_chg_info *info = &g_sc8989x;
	int ret;
	u8 addr;
	u8 val;

	for (addr = 0x00; addr <= 0x14; addr++) {
		ret = sc8989x_i2c_read_byte(info, addr, &val);
		if (!ret)
			TRACEF("%s:Reg[%02X] = 0x%02X\n", __func__, addr, val);
	}
	
	return ret;
}

static int sc8989x_enable_charging(bool enable)
{
	int ret = 0;
	struct sc8989x_chg_info *info = &g_sc8989x;

	TRACEF("%s: enable = %d\n", __func__, enable);	

	if (enable)
		ret = sc8989x_enable_charger(info);
	else
		ret = sc8989x_disable_charger(info);
	
	return ret;
}

static int sc8989x_get_vbus(unsigned int *vbus)
{
	struct sc8989x_chg_info *info = &g_sc8989x;
	int ret;
	u8 val;
	
	sc8989x_i2c_update_bits(info, SC8989X_REG_02, SC8989X_CONV_START_MASK, 
			SC8989X_CONV_START << SC8989X_CONV_START_SHIFT);
	spin(30000);
	
	ret = sc8989x_i2c_read_byte(info, SC8989X_REG_11, &val);
	if (ret)
		TRACEF("%s:read vbus fail : ret %d\n", __func__, ret);
	
	if ((val & 0x7F) <= 1)
		*vbus = 0;
	else
		*vbus = (unsigned int)((2600 + (val & 0x7F) * 100) * 1000);

	return ret;
}

static int sc8989x_get_ichg(unsigned int *ichg)
{
	int ret = 0;
	u8 val = 0;
	int curr;
	struct sc8989x_chg_info *info = &g_sc8989x;

		
	ret = sc8989x_i2c_read_byte(info, SC8989X_REG_04, &val);
	if (!ret) {
		curr = ((u32)(val & SC8989X_ICHG_MASK ) >> SC8989X_ICHG_SHIFT) 
                * SC8989X_ICHG_LSB;
		curr +=  SC8989X_ICHG_BASE;
		
		*ichg = (unsigned int)(curr * 1000);
	}
	
	return ret;
}

static int sc8989x_set_ichg(unsigned int ichg)
{
	struct sc8989x_chg_info *info = &g_sc8989x;
	
	u8 reg_ichg;
	
	ichg /= 1000; /*to mA */

	reg_ichg = (ichg - SC8989X_ICHG_BASE) / SC8989X_ICHG_LSB;

	reg_ichg <<= SC8989X_ICHG_SHIFT;
	
	return sc8989x_i2c_update_bits(info, SC8989X_REG_04,
				SC8989X_ICHG_MASK, reg_ichg);
	
}

static int sc8989x_get_aicr(unsigned int *curr)
{
	struct sc8989x_chg_info *info = &g_sc8989x;
	int ret = 0;
	u8 val;
	int ilim;
	
	ret = sc8989x_i2c_read_byte(info, SC8989X_REG_00, &val);
	if (!ret) {
		val = val & SC8989X_IINLIM_MASK;
		val = val >> SC8989X_IINLIM_SHIFT;
		ilim = val * SC8989X_IINLIM_LSB + SC8989X_IINLIM_BASE;	
		*curr = (unsigned int)(ilim * 1000); /*to uA*/
	}
	
	return ret;		
}

static int sc8989x_set_aicr(unsigned int curr)
{
	struct sc8989x_chg_info *info = &g_sc8989x;

	u8 val;
	
	curr /= 1000;/*to mA*/
	
	if (curr < SC8989X_IINLIM_BASE)
		curr = SC8989X_IINLIM_BASE;

	val = (curr - SC8989X_IINLIM_BASE) /  SC8989X_IINLIM_LSB;
	val <<= SC8989X_IINLIM_SHIFT;
		
	return sc8989x_i2c_update_bits(info, SC8989X_REG_00,
				SC8989X_IINLIM_MASK, val);	
}

static int sc8989x_set_mivr(u32 mivr)
{
	u8 reg_mivr = 0;
	struct sc8989x_chg_info *info = &g_sc8989x;


	mivr /= 1000; /*to mV*/

	if (mivr < SC8989X_VINDPM_BASE)
		mivr = SC8989X_VINDPM_BASE;
		
	reg_mivr = (mivr - SC8989X_VINDPM_BASE) / SC8989X_VINDPM_LSB;
	reg_mivr <<= SC8989X_VINDPM_SHIFT;

	TRACEF("%s: mivr = %d(0x%02X)\n", __func__, mivr, reg_mivr);
	
	return sc8989x_i2c_update_bits(info, SC8989X_REG_0D,
				SC8989X_VINDPM_MASK, reg_mivr);
}

static int sc8989x_set_vchg(u32 vchg)
{
	struct sc8989x_chg_info *info = &g_sc8989x;
	u8 reg_vchg;

	vchg /= 1000; /* to mV */

	if (vchg < SC8989X_VREG_BASE)
		vchg = SC8989X_VREG_BASE;
	
	reg_vchg = (vchg - SC8989X_VREG_BASE) / SC8989X_VREG_LSB;
	reg_vchg <<= SC8989X_VREG_SHIFT;

	return sc8989x_i2c_update_bits(info, SC8989X_REG_06,
				SC8989X_VREG_MASK, reg_vchg);
}

static int sc8989x_reset_wdt(void)
{
	struct sc8989x_chg_info *info = &g_sc8989x;
	
	return sc8989x_i2c_update_bits(info, SC8989X_REG_02,
				SC8989X_WDT_RESET_MASK, SC8989X_WDT_RESET << SC8989X_WDT_RESET);
}

static int sc8989x_enable_wdt(bool en)
{
	struct sc8989x_chg_info *info = &g_sc8989x;
	u8 val;
	
	if (en)
		val = SC8989X_WDT_40S << SC8989X_WDT_SHIFT;
	else
		val = SC8989X_WDT_DISABLE << SC8989X_WDT_SHIFT;
	
	return sc8989x_i2c_update_bits(info, SC8989X_REG_07,
				SC8989X_WDT_MASK, val);
}

static int sc8989x_check_charging_mode(void)
{
	struct sc8989x_chg_info *info = &g_sc8989x;
	u8 val;
	int ret;
	
	TRACEF("%s\n", __func__);
	ret = sc8989x_i2c_read_byte(info, SC8989X_REG_0B, &val);
	if (ret < 0)
		return ret;
	
	TRACEF("%s: in %s mode\n", __func__, 
		sc8989x_chg_mode_name[(val & 
			SC8989X_CHRG_STAT_MASK) >> SC8989X_CHRG_STAT_SHIFT]);
	
	return ret;
}

static int sc8989x_enable_ilimt(bool enable)
{
    u8 val;
    struct sc8989x_chg_info *info = &g_sc8989x;

    if (enable) {
        val = SC8989X_ENILIM_ENABLE << SC8989X_ENILIM_SHIFT;
    } else {
        val = SC8989X_ENILIM_DISABLE << SC8989X_ENILIM_SHIFT;
    }

    return sc8989x_i2c_update_bits(info, SC8989X_REG_00,
				SC8989X_ENILIM_MASK, val);
}

static int sc8989x_init_setting(struct sc8989x_chg_info *info)
{
	int ret = 0;

	TRACEF("%s\n", __func__);
	
	ret = sc8989x_enable_wdt(false);
	if (ret < 0)
		TRACEF("%s: disable wdt failed\n", __func__);

    ret = sc8989x_enable_ilimt(false);
    if (ret < 0)
        TRACEF("%s: set ilimit failed\n", __func__);

	ret = sc8989x_set_vchg(info->cv * 1000);
	if (ret < 0)
		TRACEF("%s: set chargevolt failed\n", __func__);
	
	ret = sc8989x_set_ichg(info->ichg * 1000);
	if (ret < 0)
		TRACEF("%s: set ichg failed\n", __func__);

	ret = sc8989x_set_aicr(info->aicr * 1000);
	if (ret < 0)
		TRACEF("%s: set aicr failed\n", __func__);

	ret = sc8989x_set_mivr(info->mivr * 1000);
	if (ret < 0)
		TRACEF("%s: set mivr failed\n", __func__);
	
	ret = sc8989x_enable_term(info, true);
	if (ret < 0)
		TRACEF("%s: failed to enable termination\n", __func__);

    ret = sc8989x_enable_charging(true);
    if (ret < 0)
		TRACEF("%s: failed to enable charging\n", __func__);

    ret = sc8989x_dump_register();

	return ret;
}

static int sc8989x_charger_init(void)
{
	int ret = 0;
	
	/* Check primary charger */
	if (!sc8989x_is_hw_exist(&g_sc8989x)) {
		return -ENODEV;
	}
	TRACEF("%s: %s\n", __func__, SC8989X_LK_DRV_VERSION);
	
	ret = sc8989x_init_setting(&g_sc8989x);
	if (ret < 0) {
        TRACEF("%s: init setting fail\n", __func__);
        return ret;
    }

	return ret;
}

//**********externl api************
int mtk_charger_init(void)
{
	return sc8989x_charger_init();
}

int mtk_charger_dump_register(void)
{
	return sc8989x_dump_register();
}

int mtk_charger_enable_charging(bool enable)
{
	return sc8989x_enable_charging(enable);
}

int mtk_charger_get_vbus(unsigned int *vbus)
{
	return sc8989x_get_vbus(vbus);
}

int mtk_charger_set_ichg(unsigned int ichg)
{
	return sc8989x_set_ichg(ichg * 1000);
}

int mtk_charger_get_aicr(unsigned int *aicr)
{
	return sc8989x_get_aicr(aicr);
}

int mtk_charger_set_aicr(unsigned int aicr)
{
	return sc8989x_set_aicr(aicr * 1000);
}

int mtk_charger_set_mivr(unsigned int mivr)
{
	return sc8989x_set_mivr(mivr * 1000);
}

int mtk_charger_enable_power_path(bool enable)
{
	return 0;
}

int mtk_charger_reset_pumpx(bool reset)
{
	struct sc8989x_chg_info *info = &g_sc8989x;
	
	if (reset)
		sc8989x_get_aicr(&info->aicr);
	
	return sc8989x_set_aicr(reset ? 100000 : info->aicr * 1000);
}

int mtk_charger_reset_wdt(void)
{
	return sc8989x_reset_wdt();
}

int mtk_charger_sw_reset(void)
{
	return 0;
}

int mtk_charger_enable_wdt(bool en)
{
	return sc8989x_enable_wdt(en);
}
