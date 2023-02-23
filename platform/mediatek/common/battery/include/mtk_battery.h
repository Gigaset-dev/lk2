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

struct fuel_gauge_custom_data {
    int disable_mtkbattery;
    int car_tune_value;
    int fg_meter_resistance;
    int r_fg_value;

    /* boot related */
    int pmic_min_vol;
    int poweron_system_iboot;

    /* bat temp related */
    int bat_ntc_10;
    int bat_ntc_47;
    int rbat_pull_up_r;
    int rbat_pull_down_r;
    /* BIF support related */
    int pull_up_volt_by_bif;
    int unit_fgcurrent;
    int curr_measure_20a;
};

struct fuel_gauge_custom_data *get_battery_cust_data(void);

/* common API */
int mtk_battery_init(void *fdt);
void check_sw_ocv(void);
int force_get_tbat(bool update);
bool is_disable_bat(void);
bool pmic_is_bif_exist(void);
int pmic_get_v_bat_temp(void);
void gauge_read_IM_current(void *data);
int gauge_get_current(bool *is_charging, int *battery_current);

