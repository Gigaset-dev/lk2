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

#include <stdbool.h>

enum {
    CHARGER_UNKNOWN = 0,
    STANDARD_HOST,
    CHARGING_HOST,
    NONSTANDARD_CHARGER,
    STANDARD_CHARGER,
};

struct charger_custom_data {
    bool disable_charger;
    bool power_path_support;
    bool enable_pe_plus;
    int boot_battery_voltage;
    int max_charger_voltage;
    int min_charger_voltage;
    int fast_charge_voltage;

    int usb_charger_current;
    int ac_charger_current;
    int ac_charger_input_current;
    int non_std_ac_charger_current;
    int charging_host_charger_current;
    int ta_ac_charger_current;
    int pd_charger_current;

    int temp_t4_threshold;
    int temp_t3_threshold;
    int temp_t1_threshold;

    bool enable_anime;
    int led_brightness;
    int blinking_times;
    int blinking_period;

    int r_charger_1;
    int r_charger_2;

    int min_vsys_voltage;
    bool enable_check_vsys;
    bool enable_power_measure;
    bool battery_volt_is_low;
};

void charger_init(void *fdt);
void mtk_charger_start(void);
void charger_enable_charging(bool enable);
void charger_enable_power_path(bool enable);
int charger_check_battery_plugout_once(bool *plugout_once);
bool mtk_charger_kpoc_check(void);
bool is_battery_voltage_low(void);
void set_off_mode_charge_status(int enable_charge);
int get_off_mode_charge_status(void);
