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
#include "mt6685_rtc_hw.h"
#include <platform.h>
#include <rtc.h>
#include <spmi_common.h>
#include <sys/types.h>
#include <trace.h>

#define LOCAL_TRACE 0

static int mt6685_rtc_read(u16 reg, u16 *val);
static int mt6685_rtc_write(u16 reg, u16 val);

static struct spmi_device *mt6685_sdev;

static struct reg_field mt6685_rtc_tc_reg_fields[TC_MAX] = {
    [TC_SECOND] = REG_FIELD(MT6685_RTC_TC_SEC, RTC_TC_SECOND_MASK, RTC_TC_SECOND_SHIFT),
    [TC_MINUTE] = REG_FIELD(MT6685_RTC_TC_MIN, RTC_TC_MINUTE_MASK, RTC_TC_MINUTE_SHIFT),
    [TC_HOUR] = REG_FIELD(MT6685_RTC_TC_HOU, RTC_TC_HOUR_MASK, RTC_TC_HOUR_SHIFT),
    [TC_DOM] = REG_FIELD(MT6685_RTC_TC_DOM, RTC_TC_DOM_MASK, RTC_TC_DOM_SHIFT),
    [TC_MONTH] = REG_FIELD(MT6685_RTC_TC_MTH_L, RTC_TC_MONTH_MASK, RTC_TC_MONTH_SHIFT),
    [TC_YEAR] = REG_FIELD(MT6685_RTC_TC_YEA, RTC_TC_YEAR_MASK, RTC_TC_YEAR_SHIFT),
};

static struct reg_field mt6685_rtc_alarm_reg_fields[AL_MAX] = {
    [AL_SECOND] = REG_FIELD(MT6685_RTC_AL_SEC_L, RTC_AL_SECOND_MASK, RTC_AL_SECOND_SHIFT),
    [AL_MINUTE] = REG_FIELD(MT6685_RTC_AL_MIN, RTC_AL_MINUTE_MASK, RTC_AL_MINUTE_SHIFT),
    [AL_HOUR] = REG_FIELD(MT6685_RTC_AL_HOU_L, RTC_AL_HOUR_MASK, RTC_AL_HOUR_SHIFT),
    [AL_DOM] = REG_FIELD(MT6685_RTC_AL_DOM_L, RTC_AL_DOM_MASK, RTC_AL_DOM_SHIFT),
    [AL_MONTH] = REG_FIELD(MT6685_RTC_AL_MTH_L, RTC_AL_MONTH_MASK, RTC_AL_MONTH_SHIFT),
    [AL_YEAR] = REG_FIELD(MT6685_RTC_AL_YEA_L, RTC_AL_YEAR_MASK, RTC_AL_YEAR_SHIFT),
};

static struct reg_field mt6685_rtc_spare_reg_fields[SPARE_MAX] = {
    [SPARE_PWRON_SECOND] = REG_FIELD(MT6685_RTC_SPAR0_L, RTC_PWRON_SEC_MASK, RTC_PWRON_SEC_SHIFT),
    [SPARE_PWRON_MINUTE] = REG_FIELD(MT6685_RTC_SPAR1_L, RTC_PWRON_MIN_MASK, RTC_PWRON_MIN_SHIFT),
    [SPARE_PWRON_HOUR] = REG_FIELD(MT6685_RTC_SPAR1_L, RTC_PWRON_HOU_MASK, RTC_PWRON_HOU_SHIFT),
    [SPARE_PWRON_DOM] = REG_FIELD(MT6685_RTC_SPAR1_L, RTC_PWRON_DOM_MASK, RTC_PWRON_DOM_SHIFT),
    [SPARE_PWRON_MONTH] = REG_FIELD(MT6685_RTC_PDN2_L, RTC_PWRON_MTH_MASK, RTC_PWRON_MTH_SHIFT),
    [SPARE_PWRON_YEAR] = REG_FIELD(MT6685_RTC_PDN2_L, RTC_PWRON_YEA_MASK, RTC_PWRON_YEA_SHIFT),
    [SPARE_BYPASS_PWR] = REG_FIELD(MT6685_RTC_PDN1_L, 1, RTC_BYPASS_PWR_SHIFT),
    [SPARE_PWRON_TIME] = REG_FIELD(MT6685_RTC_PDN1_L, 1, RTC_PWRON_TIME_SHIFT),
    [SPARE_KPOC] = REG_FIELD(MT6685_RTC_PDN1_L, 1, RTC_KPOC_SHIFT),
    [SPARE_PWRON_AL] = REG_FIELD(MT6685_RTC_PDN2_L, 1, RTC_PWRON_ALARM_SHIFT),
    [SPARE_PWRON_LOGO] = REG_FIELD(MT6685_RTC_PDN2_L, 1, RTC_PWRON_LOGO_SHIFT),
    [SPARE_32K_LESS] = REG_FIELD(MT6685_RTC_SPAR0_L, 1, RTC_32K_LESS_SHIFT),
    [SPARE_LP_DET] = REG_FIELD(MT6685_RTC_SPAR0_L, 1, RTC_LP_DET_SHIFT),
};

static struct reg_field mt6685_rtc_bbpu_reg_fields[BBPU_MAX] = {
    [CBUSY] = REG_FIELD(MT6685_RTC_BBPU_L, RTC_CBUSY_MASK, RTC_CBUSY_SHIFT),
};

static u16 mt6685_register[REG_MAX] = {
    MT6685_RTC_BBPU_L,
    MT6685_RTC_IRQ_STA,
    MT6685_RTC_IRQ_EN,
    MT6685_RTC_CII_EN_L,
    MT6685_RTC_AL_MASK,
    MT6685_RTC_TC_SEC,
    MT6685_RTC_TC_MIN,
    MT6685_RTC_TC_HOU,
    MT6685_RTC_TC_DOM,
    MT6685_RTC_TC_DOW,
    MT6685_RTC_TC_MTH_L,
    MT6685_RTC_TC_YEA,
    MT6685_RTC_AL_SEC_L,
    MT6685_RTC_AL_MIN,
    MT6685_RTC_AL_HOU_L,
    MT6685_RTC_AL_DOM_L,
    MT6685_RTC_AL_DOW_L,
    MT6685_RTC_AL_MTH_L,
    MT6685_RTC_AL_YEA_L,
    MT6685_RTC_OSC32CON_L,
    MT6685_RTC_POWERKEY1_L,
    MT6685_RTC_POWERKEY2_L,
    MT6685_RTC_PDN1_L,
    MT6685_RTC_PDN2_L,
    MT6685_RTC_SPAR0_L,
    MT6685_RTC_SPAR1_L,
    MT6685_RTC_PROT_L,
    MT6685_RTC_DIFF_L,
    MT6685_RTC_CALI_L,
    MT6685_RTC_WRTGR,
    MT6685_RTC_CON_L,
};

static const struct regmap_config mt6685_regmap_config = {
    .read    = mt6685_rtc_read,
    .write   = mt6685_rtc_write,
};

static void rtc_bus_init(void)
{
    mt6685_sdev = get_spmi_device(SPMI_MASTER_1, SPMI_SLAVE_9);
    if (!mt6685_sdev)
        RTC_ERR("%s: get spmi device fail\n", __func__);
}

static u16 rtc_config_interface(u16 RegNum, u8 val, u8 MASK, u8 SHIFT)
{
    int ret = 0;
    u16 org = 0;

    ret = spmi_ext_register_readl(mt6685_sdev, RegNum, &org, 1);
    if (ret < 0) {
        RTC_ERR("%s: fail, addr = 0x%x, ret = %d\n",
              __func__, RegNum, ret);
        return ret;
    }
    org &= ~(MASK << SHIFT);
    org |= (val << SHIFT);

    ret = spmi_ext_register_writel(mt6685_sdev, RegNum, &org, 1);
    if (ret < 0) {
        RTC_ERR("%s: fail, addr = 0x%x, ret = %d\n",
              __func__, RegNum, ret);
        return ret;
    }
    return 0;
}

static u16 rtc_config_interface_read(u16 addr)
{
    int ret = 0;
    u8 rdata = 0;

    ret = spmi_ext_register_readl(mt6685_sdev, addr, &rdata, 1);
    if (ret < 0) {
        RTC_ERR("%s: fail, addr = 0x%x, ret = %d\n",
               __func__, addr, ret);
        return ret;
    }

    return rdata;
}

static int mt6685_rtc_read(u16 reg, u16 *val)
{
    int ret = 0;

    ret = spmi_ext_register_readl(mt6685_sdev, reg, (unsigned char *)val, 2);
    if (ret < 0) {
        RTC_ERR("%s: fail, addr = 0x%x, ret = %d\n",
               __func__, reg, ret);
        return ret;
    }

    return 0;
}

static int mt6685_rtc_write(u16 addr, u16 data)
{
    int ret = 0;
    struct rtc_time tm;

    ret = spmi_ext_register_writel(mt6685_sdev, addr, &data, 2);
    if (ret < 0) {
        RTC_ERR("%s: fail, addr = 0x%x, ret = %d\n",
              __func__, addr, ret);
        return ret;
    }

        if (addr == MT6685_RTC_WRTGR || addr == MT6685_RTC_SEC_WRTGR) {
        /*need 1 ms delay to make sure write completely*/
        spin(1000);
    }

    return 0;
}

static void mt6685_rtc_init(uint level)
{
    mt6685_sdev = get_spmi_device(SPMI_MASTER_1, SPMI_SLAVE_9);
    if (!mt6685_sdev)
        RTC_ERR("%s: get spmi device fail\n", __func__);

    rtc_set_regmap_field(FIELD_TC, mt6685_rtc_tc_reg_fields);
    rtc_set_regmap_field(FIELD_AL, mt6685_rtc_alarm_reg_fields);
    rtc_set_regmap_field(FIELD_SPARE, mt6685_rtc_spare_reg_fields);
    rtc_set_regmap_field(FIELD_BBPU, mt6685_rtc_bbpu_reg_fields);
    rtc_load_reg(mt6685_register, REG_MAX);
    rtc_regmap_init(&mt6685_regmap_config);
}
LK_INIT_HOOK(mt6685_rtc, mt6685_rtc_init, LK_INIT_LEVEL_PLATFORM_EARLY);
