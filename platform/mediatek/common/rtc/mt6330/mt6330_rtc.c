/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <compiler.h>
#include <debug.h>
#include <lk/init.h>
#include <mt6330_rtc_hw.h>
#include <platform.h>
#include <rtc.h>
#include <spmi_common.h>
#include <sys/types.h>

static int mt6330_rtc_read(u16 reg, u16 *val);
static int mt6330_rtc_write(u16 reg, u16 val);

static struct spmi_device *mt6330_sdev;

static struct reg_field mt6330_rtc_tc_reg_fields[TC_MAX] = {
    [TC_SECOND] = REG_FIELD(MT6330_RTC_TC_SEC, PMIC_TC_SECOND_MASK, PMIC_TC_SECOND_SHIFT),
    [TC_MINUTE] = REG_FIELD(MT6330_RTC_TC_MIN, PMIC_TC_MINUTE_MASK, PMIC_TC_MINUTE_SHIFT),
    [TC_HOUR] = REG_FIELD(MT6330_RTC_TC_HOU, PMIC_TC_HOUR_MASK, PMIC_TC_HOUR_SHIFT),
    [TC_DOM] = REG_FIELD(MT6330_RTC_TC_DOM, PMIC_TC_DOM_MASK, PMIC_TC_DOM_SHIFT),
    [TC_MONTH] = REG_FIELD(MT6330_RTC_TC_MTH_L, PMIC_TC_MONTH_MASK, PMIC_TC_MONTH_SHIFT),
    [TC_YEAR] = REG_FIELD(MT6330_RTC_TC_YEA, PMIC_TC_YEAR_MASK, PMIC_TC_YEAR_SHIFT),
};

static struct reg_field mt6330_rtc_alarm_reg_fields[AL_MAX] = {
    [AL_SECOND] = REG_FIELD(MT6330_RTC_AL_SEC_L, PMIC_AL_SECOND_MASK, PMIC_AL_SECOND_SHIFT),
    [AL_MINUTE] = REG_FIELD(MT6330_RTC_AL_MIN, PMIC_AL_MINUTE_MASK, PMIC_AL_MINUTE_SHIFT),
    [AL_HOUR] = REG_FIELD(MT6330_RTC_AL_HOU_L, PMIC_AL_HOUR_MASK, PMIC_AL_HOUR_SHIFT),
    [AL_DOM] = REG_FIELD(MT6330_RTC_AL_DOM_L, PMIC_AL_DOM_MASK, PMIC_AL_DOM_SHIFT),
    [AL_MONTH] = REG_FIELD(MT6330_RTC_AL_MTH_L, PMIC_AL_MONTH_MASK, PMIC_AL_MONTH_SHIFT),
    [AL_YEAR] = REG_FIELD(MT6330_RTC_AL_YEA_L, PMIC_AL_YEAR_MASK, PMIC_AL_YEAR_SHIFT),
};

static struct reg_field mt6330_rtc_spare_reg_fields[SPARE_MAX] = {
    [SPARE_PWRON_SECOND] = REG_FIELD(MT6330_RTC_SPAR0_L, RTC_PWRON_SEC_MASK, RTC_PWRON_SEC_SHIFT),
    [SPARE_PWRON_MINUTE] = REG_FIELD(MT6330_RTC_SPAR1_L, RTC_PWRON_MIN_MASK, RTC_PWRON_MIN_SHIFT),
    [SPARE_PWRON_HOUR] = REG_FIELD(MT6330_RTC_SPAR1_L, RTC_PWRON_HOU_MASK, RTC_PWRON_HOU_SHIFT),
    [SPARE_PWRON_DOM] = REG_FIELD(MT6330_RTC_SPAR1_L, RTC_PWRON_DOM_MASK, RTC_PWRON_DOM_SHIFT),
    [SPARE_PWRON_MONTH] = REG_FIELD(MT6330_RTC_PDN2_L, RTC_PWRON_MTH_MASK, RTC_PWRON_MTH_SHIFT),
    [SPARE_PWRON_YEAR] = REG_FIELD(MT6330_RTC_PDN2_L, RTC_PWRON_YEA_MASK, RTC_PWRON_YEA_SHIFT),
    [SPARE_BYPASS_PWR] = REG_FIELD(MT6330_RTC_PDN1_L, 1, RTC_BYPASS_PWR_SHIFT),
    [SPARE_PWRON_TIME] = REG_FIELD(MT6330_RTC_PDN1_L, 1, RTC_PWRON_TIME_SHIFT),
    [SPARE_KPOC] = REG_FIELD(MT6330_RTC_PDN1_L, 1, RTC_KPOC_SHIFT),
    [SPARE_PWRON_AL] = REG_FIELD(MT6330_RTC_PDN2_L, 1, RTC_PWRON_ALARM_SHIFT),
    [SPARE_PWRON_LOGO] = REG_FIELD(MT6330_RTC_PDN2_L, 1, RTC_PWRON_LOGO_SHIFT),
    [SPARE_32K_LESS] = REG_FIELD(MT6330_RTC_SPAR0_L, 1, RTC_32K_LESS_SHIFT),
    [SPARE_LP_DET] = REG_FIELD(MT6330_RTC_SPAR0_L, 1, RTC_LP_DET_SHIFT),
};

static struct reg_field mt6330_rtc_bbpu_reg_fields[BBPU_MAX] = {
    [CBUSY] = REG_FIELD(MT6330_RTC_BBPU_L, PMIC_CBUSY_MASK, PMIC_CBUSY_SHIFT),
};

static u16 mt6330_register[REG_MAX] = {
    MT6330_RTC_BBPU_L,
    MT6330_RTC_IRQ_STA,
    MT6330_RTC_IRQ_EN,
    MT6330_RTC_CII_EN_L,
    MT6330_RTC_AL_MASK,
    MT6330_RTC_TC_SEC,
    MT6330_RTC_TC_MIN,
    MT6330_RTC_TC_HOU,
    MT6330_RTC_TC_DOM,
    MT6330_RTC_TC_DOW,
    MT6330_RTC_TC_MTH_L,
    MT6330_RTC_TC_YEA,
    MT6330_RTC_AL_SEC_L,
    MT6330_RTC_AL_MIN,
    MT6330_RTC_AL_HOU_L,
    MT6330_RTC_AL_DOM_L,
    MT6330_RTC_AL_DOW_L,
    MT6330_RTC_AL_MTH_L,
    MT6330_RTC_AL_YEA_L,
    MT6330_RTC_OSC32CON_L,
    MT6330_RTC_POWERKEY1_L,
    MT6330_RTC_POWERKEY2_L,
    MT6330_RTC_PDN1_L,
    MT6330_RTC_PDN2_L,
    MT6330_RTC_SPAR0_L,
    MT6330_RTC_SPAR1_L,
    MT6330_RTC_PROT_L,
    MT6330_RTC_DIFF_L,
    MT6330_RTC_CALI_L,
    MT6330_RTC_WRTGR,
    MT6330_RTC_CON_L,
};

enum {
    RTC_STATE_INIT,
    RTC_STATE_R,
    RTC_STATE_W,
    RTC_STATE_WT,
};

enum {
    RTC_TYPE_NORMAL,
    RTC_TYPE_SECURITY,
    RTC_TYPE_NUM,
};

u8 rtc_state[RTC_TYPE_NUM] = {RTC_STATE_INIT, RTC_STATE_INIT};

static const struct regmap_config mt6330_regmap_config = {
    .read    = mt6330_rtc_read,
    .write   = mt6330_rtc_write,
};

static int pmic_config_interface(u32 RegNum, u8 val, u8 MASK, u8 SHIFT)
{
    u8 org = 0;
    int ret = 0;

    ret = spmi_ext_register_readl(mt6330_sdev, RegNum, &org, 1);
    if (ret < 0) {
        RTC_ERR("%s: spmi read fail, addr: %d, ret: %d\n",
                 __func__, RegNum, ret);
        return ret;
    }
    org &= ~(MASK << SHIFT);
    org |= (val << SHIFT);

    ret = spmi_ext_register_writel(mt6330_sdev, RegNum, &org, 1);
    if (ret < 0) {
        RTC_ERR("%s: spmi write fail, addr: %d, ret: %d\n",
                 __func__, RegNum, ret);
        return ret;
    }
    return 0;
}

static int RTC_get_type_and_lock_shift(u16 reg, u8 *rtc_type, u8 *lock_shift)
{
    if (reg >= MT6330_RTC_ANA_ID && reg <= MT6330_RTC_CON_H) {
        *rtc_type = RTC_TYPE_NORMAL;
        *lock_shift = 0;
    } else if (reg >= MT6330_RTC_SEC_ANA_ID && reg <= MT6330_RTC_SEC_WRTGR) {
        *rtc_type = RTC_TYPE_SECURITY;
        *lock_shift = 1;
    } else
        return 0;

    return 1;
}

static int mt6330_rtc_read(u16 reg, u16 *val)
{
    int ret = 0;
    u8 rtc_type;
    u8 lock_shift;

    if (RTC_get_type_and_lock_shift(reg, &rtc_type, &lock_shift)) {
        if (rtc_state[rtc_type] == RTC_STATE_INIT || rtc_state[rtc_type] == RTC_STATE_WT) {
            // lock writing path
            pmic_config_interface(MT6330_SCK_TOP_CON0_H, 1, 0x1, lock_shift);
        } else if (rtc_state[rtc_type] == RTC_STATE_W) {
            // Trigger pending write before read
            if (rtc_type == RTC_TYPE_SECURITY)
                pmic_config_interface(MT6330_RTC_SEC_WRTGR, 1, 0xFF, 0);
            else
                pmic_config_interface(MT6330_RTC_WRTGR, 1, 0xFF, 0);

            // delay 1 ms for write completely
            spin(1000);
            // lock writing path
            pmic_config_interface(MT6330_SCK_TOP_CON0_H, 1, 0x1, lock_shift);
        }
        // else, should be already locked, don't need to lock again.
        rtc_state[rtc_type] = RTC_STATE_R;
    }
    // else, not access RTC register

    ret = spmi_ext_register_readl(mt6330_sdev, reg, (unsigned char *)val, 2);
    if (ret < 0) {
        RTC_ERR("%s: fail, reg = 0x%x, ret = %d\n",
               __func__, reg, ret);
        return ret;
    }

    return 0;
}

static int mt6330_rtc_write(u16 reg, u16 val)
{
    int ret = 0;
    u8 rtc_type;
    u8 lock_shift;

    if (RTC_get_type_and_lock_shift(reg, &rtc_type, &lock_shift)) {
        if (reg == MT6330_RTC_WRTGR || reg == MT6330_RTC_SEC_WRTGR) {
            if (rtc_state[rtc_type] == RTC_STATE_W) {
                // must be already unlocked, don't need to unlock again, just update state.
                rtc_state[rtc_type] = RTC_STATE_WT;
            } else
                return 0; // nothing to write
        } else {
            if (rtc_state[rtc_type] == RTC_STATE_INIT) {
                // lock writing path first to clear any unexpected writing data
                pmic_config_interface(MT6330_SCK_TOP_CON0_H, 1, 0x1, lock_shift);
                // unlock writing path
                pmic_config_interface(MT6330_SCK_TOP_CON0_H, 0, 0x1, lock_shift);
            } else if (rtc_state[rtc_type] == RTC_STATE_R) {
                // unlock writing path
                pmic_config_interface(MT6330_SCK_TOP_CON0_H, 0, 0x1, lock_shift);
            }
            // else, should be already unlocked, don't need to unlock again.
            rtc_state[rtc_type] = RTC_STATE_W;
        }
    }
    // else, not access RTC register

    ret = spmi_ext_register_writel(mt6330_sdev, reg, (unsigned char *)&val, 2);
    if (ret < 0) {
        RTC_ERR("%s: fail, reg = 0x%x, ret = %d\n",
          __func__, reg, ret);
        return ret;
    }

    if (reg == MT6330_RTC_WRTGR || reg == MT6330_RTC_SEC_WRTGR) {
        // need 1 ms delay to make sure write completely
        spin(1000);
    }

    return 0;
}

static void mt6330_rtc_init(uint level)
{
    mt6330_sdev = get_spmi_device(SPMI_MASTER_1, SPMI_SLAVE_4);
    if (!mt6330_sdev)
        RTC_ERR("%s: get spmi device fail\n", __func__);
    rtc_set_regmap_field(FIELD_TC, mt6330_rtc_tc_reg_fields);
    rtc_set_regmap_field(FIELD_AL, mt6330_rtc_alarm_reg_fields);
    rtc_set_regmap_field(FIELD_SPARE, mt6330_rtc_spare_reg_fields);
    rtc_set_regmap_field(FIELD_BBPU, mt6330_rtc_bbpu_reg_fields);
    rtc_load_reg(mt6330_register, REG_MAX);
    rtc_regmap_init(&mt6330_regmap_config);
}

LK_INIT_HOOK(mt6330_rtc, mt6330_rtc_init, LK_INIT_LEVEL_PLATFORM_EARLY);

