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
#include <libfdt.h>
#include <lib/pl_boottags.h>
#include <mtk_battery.h>
#include "mtk_battery_internal.h"
#include <mt_gauge.h>
#include <pmic_auxadc.h>
#include <pmic_dlpt.h>
#include <set_fdt.h>
#include <trace.h>

#define LOCAL_TRACE 0
#define BOOT_TAG_BAT_INFO      0x88610019
#define DEFAULT_BATTEMP 25
/*****************************************************************************
 *  global Variable
 ****************************************************************************/
unsigned int fg_swocv_v;
signed int fg_swocv_i;
bool g_isbat_init;
struct fuel_gauge_custom_data fg_cust_data;
struct FUELGAUGE_TEMPERATURE *pFg_table;
/*****************************************************************************
 *  External Variable
 ****************************************************************************/
static struct boot_tag_bat {
    u32 boot_voltage;
    u32 shutdown_time;
    u32 fg_swocv_v;
    u32 fg_swocv_i;
    u32 is_evb_board;
};
struct boot_tag_bat st_boot_bat_bat;
PL_BOOTTAGS_TO_BE_UPDATED(bat_info, BOOT_TAG_BAT_INFO, &st_boot_bat_bat);

void __attribute__ ((weak))
    charger_enable_charging(bool x)
{
}

bool __attribute__ ((weak))
    upmu_is_chr_det(void)
{
    return 0;
}

void __attribute__ ((weak))
    do_ptim_gauge(unsigned int *bat, unsigned int *cur)
{
}

static int pmic_get_auxadc_value(int channel, enum auxadc_val_type type)
{
    int ret, val = 0;

    ret = pmic_auxadc_read_value_ext(channel, &val, type);
    if (ret < 0)
        return ret;
    return val;
}

int get_bat_volt(void)
{
    return pmic_get_auxadc_value(AUXADC_CHAN_BATADC, AUXADC_VAL_PROCESSED);
}


void gauge_read_IM_current(void *data)
{
    if (g_isbat_init == 0)
        init_bat_cust_data();

    fgauge_read_IM_current(data);
}

int mtk_battery_init(void *fdt)
{
    if (g_isbat_init == 0) {
        init_bat_cust_data();

        if (fdt) {
            if (fdt_check_header(fdt) != 0)
                panic("Bad DTB header.\n");

            /* override default values by dts */
            init_bat_cust_data_dt(fdt);
        } else
            dprintf(CRITICAL, "[bat] fdt null, fall back to default\n");

        fbattery_init();
    }

    if (is_disable_bat())
        dis_gm3_src_sel(true);

    return 0;
}

void check_sw_ocv(void)
{
    unsigned int ptim_R_curr = 0;
    bool b_fg_is_charging;

    if (g_isbat_init == 0)
        init_bat_cust_data();

    if (!is_disable_bat()) {
        charger_enable_charging(false);
        LTRACEF("[%s]disable charge vbat:%d\n",
            __func__, get_bat_volt());

        if (upmu_is_chr_det() == true)
            mdelay(50);

        do_ptim_gauge(&fg_swocv_v, &ptim_R_curr);

        b_fg_is_charging = fg_charging_state();
        if (b_fg_is_charging == true)
            fg_swocv_i = ptim_R_curr;
        else
            fg_swocv_i = -ptim_R_curr;

        st_boot_bat_bat.fg_swocv_v = fg_swocv_v;
        st_boot_bat_bat.fg_swocv_i = fg_swocv_i;

        dprintf(INFO,
            "[%s]%d ptim[%d %d] fg_swocv_i:%d boot_vbat:%d shutdowntime:%d, update:%d %d tbat:%d\n",
            __func__, b_fg_is_charging, fg_swocv_v, ptim_R_curr, fg_swocv_i,
            st_boot_bat_bat.boot_voltage, st_boot_bat_bat.shutdown_time,
            st_boot_bat_bat.fg_swocv_v, st_boot_bat_bat.fg_swocv_i, force_get_tbat(true));

        charger_enable_charging(true);
    }
}



int BattThermistorConverTemp(int Res)
{
    int i = 0;
    int RES1 = 0, RES2 = 0;
    int TBatt_Value = -200, TMP1 = 0, TMP2 = 0;

    if (Res >= pFg_table[0].TemperatureR) {
        TBatt_Value = -40;
    } else if (Res <= pFg_table[20].TemperatureR) {
        TBatt_Value = 60;
    } else {
        RES1 = pFg_table[0].TemperatureR;
        TMP1 = pFg_table[0].BatteryTemp;

        for (i = 0; i <= 20; i++) {
            if (Res >= pFg_table[i].TemperatureR) {
                RES2 = pFg_table[i].TemperatureR;
                TMP2 = pFg_table[i].BatteryTemp;
                break;
            }
            {   /* hidden else */
                RES1 = pFg_table[i].TemperatureR;
                TMP1 = pFg_table[i].BatteryTemp;
            }
        }

        TBatt_Value = (((Res - RES2) * TMP1) + ((RES1 - Res) * TMP2)) / (RES1 - RES2);
    }

    LTRACEF("[%s] %d %d %d %d %d %d\n",
        __func__, RES1, RES2, Res, TMP1, TMP2, TBatt_Value);

    return TBatt_Value;
}


int BattVoltToTemp(int dwVolt, int volt_cali)
{
    long long TRes_temp;
    long long TRes;
    int sBaTTMP = -100;

    int vbif28 = RBAT_PULL_UP_VOLT; /* 2 side: BattVoltToTemp, TempToBattVolt */
    int delta_v;

    TRes_temp = (fg_cust_data.rbat_pull_up_r * (long long) dwVolt);
    if (fg_cust_data.pull_up_volt_by_bif != 0) {
        vbif28 = pmic_get_auxadc_value(AUXADC_CHAN_VBIF, AUXADC_VAL_PROCESSED) + volt_cali;
        delta_v = vbif28 - dwVolt;

        if (delta_v < 0)
            delta_v = delta_v * -1;

        /* avoid divide 0 case */
        if (delta_v == 0)
            delta_v = 1;

        TRes_temp = TRes_temp / delta_v;

        if (vbif28 > 3000 || vbif28 < 2500)
            LTRACEF("[RBAT_PULL_UP_VOLT_BY_BIF] vbif28:%d\n",
                pmic_get_auxadc_value(AUXADC_CHAN_VBIF, AUXADC_VAL_PROCESSED));
    } else {
        delta_v = RBAT_PULL_UP_VOLT - dwVolt;
        if (delta_v < 0)
            delta_v = delta_v * -1;

        if (delta_v == 0)
            delta_v = 1;

        TRes_temp = TRes_temp / delta_v;
    }

    if (fg_cust_data.rbat_pull_down_r != 0) {
        TRes = (TRes_temp * fg_cust_data.rbat_pull_down_r);

        if ((fg_cust_data.rbat_pull_down_r - TRes_temp) > 0)
            TRes = TRes / (fg_cust_data.rbat_pull_down_r - TRes_temp);
        else if ((fg_cust_data.rbat_pull_down_r - TRes_temp) < 0)
            TRes = TRes / (fg_cust_data.rbat_pull_down_r - TRes_temp) * -1;
    } else
        TRes = TRes_temp;

    /* convert register to temperature */
    if (!pmic_is_bif_exist())
        sBaTTMP = BattThermistorConverTemp((int)TRes);
    else
        sBaTTMP = BattThermistorConverTemp((int)TRes - BIF_NTC_R);

    LTRACEF("[%s] %d %d %d %d %d bif:%d\n",
        __func__, dwVolt, fg_cust_data.rbat_pull_up_r, vbif28, volt_cali,
        fg_cust_data.rbat_pull_down_r, fg_cust_data.pull_up_volt_by_bif);

    return sBaTTMP;
}


int force_get_tbat(bool update)
{
    int bat_temper_volt = 1;
    int bat_temper_value = 0;
    static int pre_bat_temper_val = -1;
    int fg_r_value = 0;
    int fg_meter_res_value = 0;
    int fg_current_temp = 0;
    bool fg_current_state = 0;
    int bat_temper_volt_temp = 0;
    int vol_cali = 0;

    static int pre_bat_temper_volt_temp, pre_bat_temper_volt;
    static int pre_fg_current_temp;
    static int pre_fg_current_state;
    static int pre_fg_r_value;
    static int pre_bat_temper_val2;

    if (g_isbat_init == 0) {
        LTRACEF("[%s] ERROR! bat not init!\n", __func__);
        return DEFAULT_BATTEMP;
    }

    if (is_disable_bat())
        return DEFAULT_BATTEMP;

    if (update == true || pre_bat_temper_val == -1) {
        bat_temper_volt = pmic_get_v_bat_temp();

        if (bat_temper_volt != 0) {
            fg_r_value = fg_cust_data.r_fg_value;
            fg_meter_res_value = fg_cust_data.fg_meter_resistance;

            gauge_get_current(&fg_current_state, &fg_current_temp);
            fg_current_temp = fg_current_temp / 10;

            if (fg_current_state == 1) {
                bat_temper_volt_temp = bat_temper_volt;
                bat_temper_volt =
                bat_temper_volt - ((fg_current_temp * (fg_meter_res_value + fg_r_value)) / 10000);
                vol_cali = -((fg_current_temp * (fg_meter_res_value + fg_r_value)) / 10000);
            } else {
                bat_temper_volt_temp = bat_temper_volt;
                bat_temper_volt =
                bat_temper_volt + ((fg_current_temp * (fg_meter_res_value + fg_r_value)) / 10000);
                vol_cali = ((fg_current_temp * (fg_meter_res_value + fg_r_value)) / 10000);
            }

            bat_temper_value = BattVoltToTemp(bat_temper_volt, vol_cali);
        }

        LTRACEF("[%s] %d,%d,%d,%d,%d,temp:%d r:%d %d\n",
        __func__,
        bat_temper_volt_temp, bat_temper_volt,
        fg_current_state, fg_current_temp,
        fg_r_value, bat_temper_value,
        fg_meter_res_value, fg_r_value);

        if (pre_bat_temper_val2 == 0) {
            pre_bat_temper_volt_temp = bat_temper_volt_temp;
            pre_bat_temper_volt = bat_temper_volt;
            pre_fg_current_temp = fg_current_temp;
            pre_fg_current_state = fg_current_state;
            pre_fg_r_value = fg_r_value;
            pre_bat_temper_val2 = bat_temper_value;
        } else {
            pre_bat_temper_volt_temp = bat_temper_volt_temp;
            pre_bat_temper_volt = bat_temper_volt;
            pre_fg_current_temp = fg_current_temp;
            pre_fg_current_state = fg_current_state;
            pre_fg_r_value = fg_r_value;
            pre_bat_temper_val2 = bat_temper_value;
            LTRACEF("[%s] current:%d,%d,%d,%d,%d,%d pre:%d,%d,%d,%d,%d,%d\n",
            __func__,
            bat_temper_volt_temp, bat_temper_volt, fg_current_state, fg_current_temp,
            fg_r_value, bat_temper_value, pre_bat_temper_volt_temp, pre_bat_temper_volt,
            pre_fg_current_state, pre_fg_current_temp, pre_fg_r_value,
            pre_bat_temper_val2);
        }
    } else {
        bat_temper_value = pre_bat_temper_val;
    }

    return bat_temper_value;
}

void init_bat_cust_data(void)
{
    fg_cust_data.disable_mtkbattery = 0;
    fg_cust_data.car_tune_value = CAR_TUNE_VALUE;
    fg_cust_data.fg_meter_resistance = FG_METER_RESISTANCE;
    fg_cust_data.r_fg_value = R_FG_VALUE;
    fg_cust_data.pmic_min_vol = PMIC_MIN_VOL;
    fg_cust_data.poweron_system_iboot = POWERON_SYSTEM_IBOOT;
    fg_cust_data.bat_ntc_10 = BAT_NTC_10;
    fg_cust_data.bat_ntc_47 = BAT_NTC_47;
    fg_cust_data.rbat_pull_up_r = 24000;
    fg_cust_data.rbat_pull_down_r = 0;
    fg_cust_data.pull_up_volt_by_bif = 0;
    fg_cust_data.unit_fgcurrent = UNIT_FGCURRENT;
    pFg_table = (struct FUELGAUGE_TEMPERATURE *)&Fg_Temperature_Table_NTC10;

    if (fg_cust_data.bat_ntc_10 == 1) {
        fg_cust_data.rbat_pull_up_r = 24000;
        pFg_table = (struct FUELGAUGE_TEMPERATURE *)&Fg_Temperature_Table_NTC10;
    }
    if (fg_cust_data.bat_ntc_47 == 1) {
        fg_cust_data.rbat_pull_up_r = 61900;
        pFg_table = (struct FUELGAUGE_TEMPERATURE *)&Fg_Temperature_Table_NTC47;
    }

    g_isbat_init = 1;

    LTRACEF("[%s] init done, ntc10:%d, ntc47:%d\n", __func__,
        fg_cust_data.bat_ntc_10, fg_cust_data.bat_ntc_47);
}

unsigned int chr_fdt_getprop_u32(const void *fdt, int nodeoffset, const char *name)
{
    const void *data = NULL;
    int len = 0;

    data = fdt_getprop(fdt, nodeoffset, name, &len);
    if (len > 0 && data)
        return fdt32_to_cpu(*(unsigned int *)data);
    else
        return 0;
}

int init_bat_cust_data_dt(void *fdt)
{
    int offset = 0, val = 0;

    if (fdt != NULL) {
        offset = fdt_node_offset_by_compatible(
            fdt, -1, "mediatek,bat_gm30");

        if (offset < 0)
            offset = fgauge_get_dtsi_offset(fdt);


        if (offset >= 0) {
            val = chr_fdt_getprop_u32(fdt, offset, "DISABLE_MTKBATTERY");
            if (val)
                fg_cust_data.disable_mtkbattery = val;

            val = chr_fdt_getprop_u32(fdt, offset, "CAR_TUNE_VALUE");
            if (val)
                fg_cust_data.car_tune_value = val * UNIT_TRANS_10;

            val = chr_fdt_getprop_u32(fdt, offset, "FG_METER_RESISTANCE");
            if (val)
                fg_cust_data.fg_meter_resistance = val;

            val = chr_fdt_getprop_u32(fdt, offset, "R_FG_VALUE");
            if (val)
                fg_cust_data.r_fg_value = val * UNIT_TRANS_10;

            val = chr_fdt_getprop_u32(fdt, offset, "CURR_MEASURE_20A");
            if (val)
                fg_cust_data.curr_measure_20a = val;

            val = chr_fdt_getprop_u32(fdt, offset, "UNIT_MULTIPLE");
            if (val)
                fg_cust_data.unit_fgcurrent = val * UNIT_FGCURRENT;

            val = chr_fdt_getprop_u32(fdt, offset, "PMIC_MIN_VOL");
            if (val)
                fg_cust_data.pmic_min_vol = val;

            val = chr_fdt_getprop_u32(fdt, offset, "POWERON_SYSTEM_IBOOT");
            if (val)
                fg_cust_data.poweron_system_iboot = val;

            val = chr_fdt_getprop_u32(fdt, offset, "RBAT_PULL_DOWN_R");
            if (val)
                fg_cust_data.rbat_pull_down_r = val;

            val = chr_fdt_getprop_u32(fdt, offset, "RBAT_PULL_UP_VOLT");
            if (val)
                fg_cust_data.pull_up_volt_by_bif = val;

            LTRACEF("[%s]disable:%d, car_tune:%d,fg_meter_r %d,rfg:%d, gauge0[%d %d],%d %d\n",
                __func__, fg_cust_data.disable_mtkbattery,
                fg_cust_data.car_tune_value,
                fg_cust_data.fg_meter_resistance, fg_cust_data.r_fg_value,
                fg_cust_data.pmic_min_vol, fg_cust_data.poweron_system_iboot,
                fg_cust_data.rbat_pull_down_r, fg_cust_data.pull_up_volt_by_bif);

            g_isbat_init = 1;
            return 0;
        } else {
            dprintf(CRITICAL, "[%s]battery:no device tree!,use default value\n",
                __func__);
            return 1;
        }
    }
    return 0;
}

static void set_bat_data(void *fdt)
{
    int offset = 0;

    if (fdt != NULL) {
        offset = fdt_node_offset_by_compatible(
            fdt, -1, "mediatek,bat_gm30");

        if (offset < 0)
            offset = fgauge_get_dtsi_offset(fdt);

        if (offset >= 0) {
            fdt_setprop_u32(fdt, offset, "shutdown_time",
                st_boot_bat_bat.shutdown_time);
            fdt_setprop_u32(fdt, offset, "fg_swocv_v",
                st_boot_bat_bat.fg_swocv_v);
            fdt_setprop_u32(fdt, offset, "fg_swocv_i",
                st_boot_bat_bat.fg_swocv_i);

            dprintf(CRITICAL, "[%s]: battery:set shutdown_time:%d swocv_v:%d swocv_i:%d to dtsi!\n",
                __func__, st_boot_bat_bat.shutdown_time,
                st_boot_bat_bat.fg_swocv_v,
                st_boot_bat_bat.fg_swocv_i);
        } else {
            dprintf(CRITICAL, "[%s]battery:no device tree!\n", __func__);
        }
    }
}

SET_FDT_INIT_HOOK(set_bat_data, set_bat_data);

struct fuel_gauge_custom_data *get_battery_cust_data(void)
{
    if (g_isbat_init == 0)
        init_bat_cust_data();

    return &fg_cust_data;
}

bool pmic_is_bif_exist(void)
{
    return false;
}

int pmic_get_v_bat_temp(void)
{
    int adc = 0;
    bool is_disable = is_disable_bat();

    if (is_disable)
        return adc;

    adc = pmic_get_auxadc_value(AUXADC_CHAN_BAT_TEMP, AUXADC_VAL_PROCESSED);
    return adc;
}

bool is_disable_bat(void)
{
    return fg_cust_data.disable_mtkbattery;
}


int gauge_get_current(bool *is_charging, int *battery_current)
{
    fgauge_get_current(is_charging, battery_current);
    return 0;
}

static void pl_boottags_bat_hook(struct boot_tag *tag)
{
    struct boot_tag_bat *p = (struct boot_tag_bat *)&tag->data;

    if (p != NULL) {
        st_boot_bat_bat.boot_voltage = p->boot_voltage;
        st_boot_bat_bat.shutdown_time = p->shutdown_time;
        st_boot_bat_bat.fg_swocv_v = p->fg_swocv_v;
        st_boot_bat_bat.fg_swocv_i = p->fg_swocv_i;
        st_boot_bat_bat.is_evb_board = p->is_evb_board;
    } else {
        dprintf(CRITICAL, "[%s] null pointer in hook function\n", __func__);
    }
}

PL_BOOTTAGS_INIT_HOOK(bat_info, BOOT_TAG_BAT_INFO, pl_boottags_bat_hook);

