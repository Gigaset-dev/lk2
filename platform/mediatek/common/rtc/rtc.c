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
#include <errno.h>
#include <debug.h>
#include <platform.h>
#include <rtc.h>
#include <string.h>
#include <sys/types.h>

struct mtk_rtc mt_rtc;


int rtc_read(u16 reg, u16 *val)
{
    return mt_rtc.regmap->read(reg, val);
}

int rtc_write(u16 reg, u16 val)
{
    return mt_rtc.regmap->write(reg, val);
}

int rtc_field_read(struct reg_field *field, u16 *val)
{
    int ret = 0;
    u16 reg_val = 0;

    ret = rtc_read(field->reg, &reg_val);
    if (ret != 0)
        return ret;

    reg_val &= (field->mask << field->shift);
    reg_val >>= field->shift;
    *val = reg_val;

    return ret;
}

int rtc_field_write(struct reg_field *field, u16 val)
{
    u16 mask, tmp;
    u16 orig = 0;
    int ret;

    mask = (field->mask << field->shift);
    val <<=  field->shift;

    ret = rtc_read(field->reg, &orig);
    if (ret != 0)
        return ret;

    tmp = orig & ~mask;
    tmp |= val & mask;

    return rtc_write(field->reg, tmp);
}

static bool rtc_busy_wait(void)
{
    lk_time_t begin = current_time();
    u16 cbusy = 0;

    do {
        rtc_field_read(mt_rtc.bbpu[CBUSY], &cbusy);
        /* Time > 1sec,  time out and set recovery mode enable. */
        if (current_time() - begin > 1000000) {
            RTC_ERR("BBPU CBUSY time out\n");
            return false;
        }
    } while (cbusy);

    return true;
}

bool rtc_write_trigger(void)
{
    rtc_write(mt_rtc.reg[RTC_WRTGR], 1);
    return rtc_busy_wait();
}

void rtc_writeif_unlock(void)
{
    rtc_write(mt_rtc.reg[RTC_PROT], RTC_PROT_UNLOCK1);
    rtc_write_trigger();
    rtc_write(mt_rtc.reg[RTC_PROT], RTC_PROT_UNLOCK2);
    rtc_write_trigger();
}

static void rtc_get_tick(struct rtc_time *tm)
{
    u16 data[TC_MAX] = {0};
    int i;

    for (i = TC_SECOND; i < TC_MAX; i++)
        rtc_field_read(mt_rtc.tc[i], &data[i]);

    tm->tm_sec = data[TC_SECOND];
    tm->tm_min = data[TC_MINUTE];
    tm->tm_hour = data[TC_HOUR];
    tm->tm_mday = data[TC_DOM];
    tm->tm_mon = data[TC_MONTH];
    tm->tm_year = data[TC_YEAR];
}

void rtc_get_time(struct rtc_time *tm)
{
    u16 sec = 0;

    rtc_get_tick(tm);

    rtc_field_read(mt_rtc.tc[TC_SECOND], &sec);
    if (sec < tm->tm_sec) {    /* SEC has carried */
        rtc_get_tick(tm);
    }
    tm->tm_year += RTC_MIN_YEAR;
}

void rtc_set_alarm(struct rtc_time *tm)
{
    rtc_field_write(mt_rtc.alarm[AL_SECOND], tm->tm_sec);
    rtc_field_write(mt_rtc.alarm[AL_MINUTE], tm->tm_min);
    rtc_field_write(mt_rtc.alarm[AL_HOUR], tm->tm_hour);
    rtc_field_write(mt_rtc.alarm[AL_DOM], tm->tm_mday);
    rtc_field_write(mt_rtc.alarm[AL_MONTH], tm->tm_mon);
    rtc_field_write(mt_rtc.alarm[AL_YEAR], tm->tm_year - RTC_MIN_YEAR);
}

int rtc_get_spare_register(enum rtc_spare cmd, u16 *val)
{
    if (cmd < SPARE_MAX)
        return rtc_field_read(mt_rtc.spare[cmd], val);
    else
        return -EINVAL;
}

int rtc_set_spare_register(enum rtc_spare cmd, u16 val)
{
    if (cmd < SPARE_MAX)
        return rtc_field_write(mt_rtc.spare[cmd], val);
    else
        return -EINVAL;
}

void rtc_set_regmap_field(int type, struct reg_field *field)
{
    int i;

    switch (type) {
    case FIELD_TC:
        for (i = 0; i < TC_MAX; i++)
            mt_rtc.tc[i] = &field[i];
        break;

    case FIELD_AL:
        for (i = 0; i < AL_MAX; i++)
            mt_rtc.alarm[i] = &field[i];
        break;

    case FIELD_SPARE:
        for (i = 0; i < SPARE_MAX; i++)
            mt_rtc.spare[i] = &field[i];
        break;

    case FIELD_BBPU:
        for (i = 0; i < BBPU_MAX; i++)
            mt_rtc.bbpu[i] = &field[i];
        break;

    default:
        RTC_ERR("unknown field type[%d]\n", type);
        break;
    }
}

void rtc_load_reg(const u16 *reg, int count)
{
    memcpy(mt_rtc.reg, reg, count*sizeof(u16));
}

void rtc_regmap_init(const struct regmap_config *config)
{
    mt_rtc.regmap = config;
}
