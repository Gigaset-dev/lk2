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

#pragma once


#include <debug.h>


#define RTCTAG                "[RTC]"
#define RTC_ERR(fmt, arg ...) dprintf(CRITICAL, RTCTAG "%s(%d):" fmt, \
                        __func__, __LINE__, ## arg)
#define RTC_INFO(fmt, arg ...) dprintf(SPEW, RTCTAG "%s(%d):" fmt, \
                        __func__, __LINE__, ## arg)

#define RTC_MIN_YEAR                    1968

#define RTC_PROT_UNLOCK1                0x586a
#define RTC_PROT_UNLOCK2                0x9136

#define RTC_POWERKEY1_KEY               0xa357
#define RTC_POWERKEY2_KEY               0x67d2

#define PMIC_SWCID_ADDR                 0xa
#define MT6359_SWCID                    0x5900


#define REG_FIELD(_reg, _mask, _shift) {        \
                .reg = _reg,    \
                .mask = _mask,  \
                .shift = _shift,    \
                }

struct reg_field {
    u16 reg;
    u16 mask;
    u16 shift;
};

enum rtc_tc {
    TC_SECOND,
    TC_MINUTE,
    TC_HOUR,
    TC_DOM,
    TC_MONTH,
    TC_YEAR,
    TC_MAX
};

enum rtc_alarm {
    AL_SECOND,
    AL_MINUTE,
    AL_HOUR,
    AL_DOM,
    AL_MONTH,
    AL_YEAR,
    AL_MAX
};

enum rtc_spare {
    SPARE_PWRON_SECOND,
    SPARE_PWRON_MINUTE,
    SPARE_PWRON_HOUR,
    SPARE_PWRON_DOM,
    SPARE_PWRON_MONTH,
    SPARE_PWRON_YEAR,
    SPARE_BYPASS_PWR,
    SPARE_PWRON_TIME,
    SPARE_KPOC,
    SPARE_PWRON_AL,
    SPARE_PWRON_LOGO,
    SPARE_32K_LESS,
    SPARE_LP_DET,
    SPARE_MAX,
};

enum rtc_bbpu {
    CBUSY,
    BBPU_MAX
};

enum rtc_register {
    RTC_BBPU,
    RTC_IRQ_STA,
    RTC_IRQ_EN,
    RTC_CII_EN,
    RTC_AL_MASK,
    RTC_TC_SEC,
    RTC_TC_MIN,
    RTC_TC_HOU,
    RTC_TC_DOM,
    RTC_TC_DOW,
    RTC_TC_MTH,
    RTC_TC_YEA,
    RTC_AL_SEC,
    RTC_AL_MIN,
    RTC_AL_HOU,
    RTC_AL_DOM,
    RTC_AL_DOW,
    RTC_AL_MTH,
    RTC_AL_YEA,
    RTC_OSC32CON,
    RTC_POWERKEY1,
    RTC_POWERKEY2,
    RTC_PDN1,
    RTC_PDN2,
    RTC_SPAR0,
    RTC_SPAR1,
    RTC_PROT,
    RTC_DIFF,
    RTC_CALI,
    RTC_WRTGR,
    RTC_CON,
    REG_MAX
};

enum rtc_field_type {
    FIELD_TC,
    FIELD_AL,
    FIELD_SPARE,
    FIELD_BBPU,
    TYPE_MAX
};

struct regmap_config {
    int (*read)(u16 reg, u16 *val);
    int (*write)(u16 reg, u16 val);
};

struct mtk_rtc {
    u16     reg[REG_MAX];
    struct reg_field    *tc[TC_MAX];
    struct reg_field    *alarm[AL_MAX];
    struct reg_field    *spare[SPARE_MAX];
    struct reg_field    *bbpu[BBPU_MAX];
    const struct regmap_config *regmap;
};

struct rtc_time {
    u16 tm_sec;
    u16 tm_min;
    u16 tm_hour;
    u16 tm_mday;
    u16 tm_mon;
    u16 tm_year;
};



int rtc_read(u16 reg, u16 *val);
int rtc_write(u16 reg, u16 val);
int rtc_field_read(struct reg_field *field, u16 *val);
int rtc_field_write(struct reg_field *field, u16 val);
bool rtc_write_trigger(void);
void rtc_writeif_unlock(void);
void rtc_get_time(struct rtc_time *tm);
void rtc_set_alarm(struct rtc_time *tm);
int rtc_get_spare_register(enum rtc_spare cmd, u16 *val);
int rtc_set_spare_register(enum rtc_spare cmd, u16 val);
void rtc_set_regmap_field(int type, struct reg_field *field);
void rtc_regmap_init(const struct regmap_config *config);
void rtc_load_reg(const u16 *reg, int count);
