/*
 * Copyright (c) 2019 MediaTek Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <compiler.h>
#include <debug.h>
#include <lk/init.h>
#include <platform.h>
#include <rtc.h>
#include <sys/types.h>
#include <mt6358_rtc_hw.h>
#include <pmic_wrap_common.h>


static struct reg_field mt6358_rtc_tc_reg_fields[TC_MAX] = {
    [TC_SECOND]           = REG_FIELD(MT6358_RTC_TC_SEC, PMIC_TC_SECOND_MASK, PMIC_TC_SECOND_SHIFT),
    [TC_MINUTE]           = REG_FIELD(MT6358_RTC_TC_MIN, PMIC_TC_MINUTE_MASK, PMIC_TC_MINUTE_SHIFT),
    [TC_HOUR]             = REG_FIELD(MT6358_RTC_TC_HOU, PMIC_TC_HOUR_MASK, PMIC_TC_HOUR_SHIFT),
    [TC_DOM]              = REG_FIELD(MT6358_RTC_TC_DOM, PMIC_TC_DOM_MASK, PMIC_TC_DOM_SHIFT),
    [TC_MONTH]            = REG_FIELD(MT6358_RTC_TC_MTH, PMIC_TC_MONTH_MASK, PMIC_TC_MONTH_SHIFT),
    [TC_YEAR]             = REG_FIELD(MT6358_RTC_TC_YEA, PMIC_TC_YEAR_MASK, PMIC_TC_YEAR_SHIFT),
};

static struct reg_field mt6358_rtc_alarm_reg_fields[AL_MAX] = {
    [AL_SECOND]           = REG_FIELD(MT6358_RTC_AL_SEC, PMIC_AL_SECOND_MASK, PMIC_AL_SECOND_SHIFT),
    [AL_MINUTE]           = REG_FIELD(MT6358_RTC_AL_MIN, PMIC_AL_MINUTE_MASK, PMIC_AL_MINUTE_SHIFT),
    [AL_HOUR]             = REG_FIELD(MT6358_RTC_AL_HOU, PMIC_AL_HOUR_MASK, PMIC_AL_HOUR_SHIFT),
    [AL_DOM]              = REG_FIELD(MT6358_RTC_AL_DOM, PMIC_AL_DOM_MASK, PMIC_AL_DOM_SHIFT),
    [AL_MONTH]            = REG_FIELD(MT6358_RTC_AL_MTH, PMIC_AL_MONTH_MASK, PMIC_AL_MONTH_SHIFT),
    [AL_YEAR]             = REG_FIELD(MT6358_RTC_AL_YEA, PMIC_AL_YEAR_MASK, PMIC_AL_YEAR_SHIFT),
};

static struct reg_field mt6358_rtc_spare_reg_fields[SPARE_MAX] = {
    [SPARE_PWRON_SECOND]  = REG_FIELD(MT6358_RTC_SPAR0, RTC_PWRON_SEC_MASK, RTC_PWRON_SEC_SHIFT),
    [SPARE_PWRON_MINUTE]  = REG_FIELD(MT6358_RTC_SPAR1, RTC_PWRON_MIN_MASK, RTC_PWRON_MIN_SHIFT),
    [SPARE_PWRON_HOUR]    = REG_FIELD(MT6358_RTC_SPAR1, RTC_PWRON_HOU_MASK, RTC_PWRON_HOU_SHIFT),
    [SPARE_PWRON_DOM]     = REG_FIELD(MT6358_RTC_SPAR1, RTC_PWRON_DOM_MASK, RTC_PWRON_DOM_SHIFT),
    [SPARE_PWRON_MONTH]   = REG_FIELD(MT6358_RTC_PDN2, RTC_PWRON_MTH_MASK, RTC_PWRON_MTH_SHIFT),
    [SPARE_PWRON_YEAR]    = REG_FIELD(MT6358_RTC_PDN2, RTC_PWRON_YEA_MASK, RTC_PWRON_YEA_SHIFT),
    [SPARE_BYPASS_PWR]    = REG_FIELD(MT6358_RTC_PDN1, 1, RTC_BYPASS_PWR_SHIFT),
    [SPARE_PWRON_TIME]    = REG_FIELD(MT6358_RTC_PDN1, 1, RTC_PWRON_TIME_SHIFT),
    [SPARE_KPOC]          = REG_FIELD(MT6358_RTC_PDN1, 1, RTC_KPOC_SHIFT),
    [SPARE_PWRON_AL]      = REG_FIELD(MT6358_RTC_PDN2, 1, RTC_PWRON_ALARM_SHIFT),
    [SPARE_PWRON_LOGO]    = REG_FIELD(MT6358_RTC_PDN2, 1, RTC_PWRON_LOGO_SHIFT),
    [SPARE_32K_LESS]      = REG_FIELD(MT6358_RTC_SPAR0, 1, RTC_32K_LESS_SHIFT),
    [SPARE_LP_DET]        = REG_FIELD(MT6358_RTC_SPAR0, 1, RTC_LP_DET_SHIFT),
};

static struct reg_field mt6358_rtc_bbpu_reg_fields[BBPU_MAX] = {
    [CBUSY]               = REG_FIELD(MT6358_RTC_BBPU, PMIC_CBUSY_MASK, PMIC_CBUSY_SHIFT),
};

static u16 mt6358_register[REG_MAX] = {
    MT6358_RTC_BBPU,
    MT6358_RTC_IRQ_STA,
    MT6358_RTC_IRQ_EN,
    MT6358_RTC_CII_EN,
    MT6358_RTC_AL_MASK,
    MT6358_RTC_TC_SEC,
    MT6358_RTC_TC_MIN,
    MT6358_RTC_TC_HOU,
    MT6358_RTC_TC_DOM,
    MT6358_RTC_TC_DOW,
    MT6358_RTC_TC_MTH,
    MT6358_RTC_TC_YEA,
    MT6358_RTC_AL_SEC,
    MT6358_RTC_AL_MIN,
    MT6358_RTC_AL_HOU,
    MT6358_RTC_AL_DOM,
    MT6358_RTC_AL_DOW,
    MT6358_RTC_AL_MTH,
    MT6358_RTC_AL_YEA,
    MT6358_RTC_OSC32CON,
    MT6358_RTC_POWERKEY1,
    MT6358_RTC_POWERKEY2,
    MT6358_RTC_PDN1,
    MT6358_RTC_PDN2,
    MT6358_RTC_SPAR0,
    MT6358_RTC_SPAR1,
    MT6358_RTC_PROT,
    MT6358_RTC_DIFF,
    MT6358_RTC_CALI,
    MT6358_RTC_WRTGR,
    MT6358_RTC_CON,
};

static const struct regmap_config mt6358_regmap_config = {
    .read    = pwrap_read,
    .write   = pwrap_write,
};

static void mt6358_rtc_init(uint level)
{
    rtc_set_regmap_field(FIELD_TC, mt6358_rtc_tc_reg_fields);
    rtc_set_regmap_field(FIELD_AL, mt6358_rtc_alarm_reg_fields);
    rtc_set_regmap_field(FIELD_SPARE, mt6358_rtc_spare_reg_fields);
    rtc_set_regmap_field(FIELD_BBPU, mt6358_rtc_bbpu_reg_fields);
    rtc_load_reg(mt6358_register, REG_MAX);
    rtc_regmap_init(&mt6358_regmap_config);
}

LK_INIT_HOOK(mt6358_rtc, mt6358_rtc_init, LK_INIT_LEVEL_PLATFORM_EARLY);

