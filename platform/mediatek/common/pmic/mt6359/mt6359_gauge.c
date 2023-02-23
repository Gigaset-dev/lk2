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

#include <debug.h>
#include <err.h>
#include "fg_registers.h"
#include <libfdt.h>
#include <mt_gauge.h>
#include <mtk_battery.h>
#include <pmic_wrap_common.h>
#include <stdbool.h>
#include <sys/types.h>
#include <trace.h>

#define LOCAL_TRACE 0

bool b_is_charging;

bool fg_charging_state(void)
{
    return b_is_charging;
}

int fbattery_init(void)
{
        return 0;
}

int fgauge_get_dtsi_offset(void *fdt)
{
        int offset = 0;

        offset = fdt_node_offset_by_compatible(
            fdt, -1, "mediatek,mt6359p-gauge");

        return offset;
}

unsigned int fg_get_data_ready_status(void)
{
    return pwrap_read_field(PMIC_FG_LATCHDATA_ST_ADDR,
        PMIC_FG_LATCHDATA_ST_MASK, PMIC_FG_LATCHDATA_ST_SHIFT);
}

void fgauge_read_IM_current(void *data)
{
    u16 uvalue16 = 0;
    signed int dvalue = 0;
    signed int ori_curr = 0;
    signed int curr_rfg = 0;
    long long Temp_Value = 0;
    struct fuel_gauge_custom_data *fg_cust_data;

    fg_cust_data = get_battery_cust_data();

    uvalue16 = pwrap_read_field(PMIC_FG_R_CURR_ADDR, PMIC_FG_R_CURR_MASK, PMIC_FG_R_CURR_SHIFT);

    /*calculate the real world data    */
    dvalue = (unsigned int) uvalue16;
    if (dvalue == 0) {
        Temp_Value = (long long) dvalue;
        b_is_charging = false;
    } else if (dvalue > 32767) {
        /* > 0x8000 */
        Temp_Value = (long long) (dvalue - 65535);
        Temp_Value = Temp_Value - (Temp_Value * 2);
        b_is_charging = false;
    } else {
        Temp_Value = (long long) dvalue;
        b_is_charging = true;
    }

    Temp_Value = Temp_Value * UNIT_FGCURRENT;
    Temp_Value = Temp_Value/100000;
    dvalue = (unsigned int) Temp_Value;

    ori_curr = dvalue;
    curr_rfg = dvalue;

    /* Auto adjust value */
    if (fg_cust_data->r_fg_value != DEFAULT_R_FG) {
        dvalue = (dvalue * DEFAULT_R_FG) / fg_cust_data->r_fg_value;
        curr_rfg = dvalue;
    }

    dvalue = ((dvalue * fg_cust_data->car_tune_value) / 1000);

    if (b_is_charging == true) {
        LTRACEF("[%s](charging)FG_CURRENT:0x%x,curr:[%d,%d,%d] mA, Rfg:%d ratio:%d\n",
            __func__, uvalue16, ori_curr, curr_rfg, dvalue, fg_cust_data->r_fg_value,
            fg_cust_data->car_tune_value);

    } else {
        LTRACEF("[%s](discharg)FG_CURRENT:0x%x,curr:[%d,%d,%d] mA, Rfg:%d ratio:%d\n",
            __func__, uvalue16, ori_curr, curr_rfg, dvalue, fg_cust_data->r_fg_value,
            fg_cust_data->car_tune_value);
    }

    *(signed int *) (data) = dvalue;
}


int fgauge_get_current(bool *fg_is_charging, int *data)
{
    unsigned short uvalue16 = 0;
    signed int dvalue = 0;
    signed int curr_rfg = 0;
    signed int ori_curr = 0;
    int m = 0;
    long long Temp_Value = 0;
    unsigned int ret = 0;
    struct fuel_gauge_custom_data *fg_cust_data;

    fg_cust_data = get_battery_cust_data();

    LTRACEF("[fgauge_read_current],cartune:%d,%d,%d\n",
        fg_cust_data->car_tune_value, fg_cust_data->r_fg_value, UNIT_FGCURRENT);

    pwrap_write_field(PMIC_FG_SW_READ_PRE_ADDR, 1,
        PMIC_FG_SW_READ_PRE_MASK, PMIC_FG_SW_READ_PRE_SHIFT);

    m = 0;

    while (fg_get_data_ready_status() == 0) {
        m++;
        if (m > 1000) {
            LTRACEF("[%s]fg_get_data_ready_status timeout 1 !%d\n", __func__, ret);
            break;
        }
    }

    uvalue16 = pwrap_read_field(PMIC_FG_CURRENT_OUT_ADDR,
        PMIC_FG_CURRENT_OUT_MASK, PMIC_FG_CURRENT_OUT_SHIFT);

    pwrap_write_field(PMIC_FG_SW_CLEAR_ADDR, 1,
        PMIC_FG_SW_CLEAR_MASK, PMIC_FG_SW_CLEAR_SHIFT);

    pwrap_write_field(PMIC_FG_SW_READ_PRE_ADDR, 0,
        PMIC_FG_SW_READ_PRE_MASK, PMIC_FG_SW_READ_PRE_SHIFT);

    m = 0;
    while (fg_get_data_ready_status() != 0) {
        m++;
        if (m > 1000) {
            LTRACEF("[%s] fg_get_data_ready_status 2! ret:%d\n", __func__, ret);
            break;
        }
    }

    pwrap_write_field(PMIC_FG_SW_CLEAR_ADDR, 0,
        PMIC_FG_SW_CLEAR_MASK, PMIC_FG_SW_CLEAR_SHIFT);

    /*calculate the real world data    */
    dvalue = (unsigned int) uvalue16;
    if (dvalue == 0) {
        Temp_Value = (long long) dvalue;
        *fg_is_charging = false;
    } else if (dvalue > 32767) {
        /* > 0x8000 */
        Temp_Value = (long long) (dvalue - 65535);
        Temp_Value = Temp_Value - (Temp_Value * 2);
        *fg_is_charging = false;
    } else {
        Temp_Value = (long long) dvalue;
        *fg_is_charging = true;
    }

    Temp_Value = Temp_Value * UNIT_FGCURRENT;
    Temp_Value = Temp_Value / 100000;
    dvalue = (unsigned int) Temp_Value;

    ori_curr = dvalue;
    curr_rfg = dvalue;

    /* Auto adjust value */
    if (fg_cust_data->r_fg_value != DEFAULT_R_FG) {
        dvalue = (dvalue * DEFAULT_R_FG) / fg_cust_data->r_fg_value;
        curr_rfg = dvalue;
    }

    dvalue = ((dvalue * fg_cust_data->car_tune_value) / 1000);

    *data = dvalue;
    LTRACEF("[%s]is_charge:%d Ori_curr:0x%x,curr:[%d,%d,Final:%d] Rfg:%d ratio:%d\n",
        __func__,
        *fg_is_charging, uvalue16, ori_curr, curr_rfg, dvalue,
        fg_cust_data->r_fg_value, fg_cust_data->car_tune_value);

    return 0;
}

int dis_gm3_src_sel(bool is_dis)
{
    u16 reset_sel, new_reset_sel;

    reset_sel = pwrap_read_field(PMIC_RG_FGADC_RST_SRC_SEL_ADDR,
        PMIC_RG_FGADC_RST_SRC_SEL_MASK, PMIC_RG_FGADC_RST_SRC_SEL_SHIFT);

    if (is_dis == true) {
        if (reset_sel == 1)
            pwrap_write_field(PMIC_RG_FGADC_RST_SRC_SEL_ADDR, 0,
                PMIC_RG_FGADC_RST_SRC_SEL_MASK, PMIC_RG_FGADC_RST_SRC_SEL_SHIFT);
    }
    new_reset_sel = pwrap_read_field(PMIC_RG_FGADC_RST_SRC_SEL_ADDR,
        PMIC_RG_FGADC_RST_SRC_SEL_MASK, PMIC_RG_FGADC_RST_SRC_SEL_SHIFT);

    LTRACEF("[%s]: is_dis:%d reset_sel=%d new=%d\n",
        __func__, is_dis, reset_sel, new_reset_sel);

    return 0;
}

