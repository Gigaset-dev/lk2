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

/* Current related */
#define UNIT_FGCURRENT     (610352)
#define CAR_TUNE_VALUE      1000 //1.00
#define R_FG_VALUE          50

#define DEFAULT_R_FG (50)
#define UNIT_CHARGE     (85)

#define FG_METER_RESISTANCE     75

/* Gauge 0% related */
#define PMIC_MIN_VOL 34000
#define POWERON_SYSTEM_IBOOT 500        /* mA */

/* BAT TEMP related  */
#define BAT_NTC_10 1
#define BAT_NTC_47 0

int fbattery_init(void);
int fgauge_get_dtsi_offset(void *fdt);
int fgauge_get_current(bool *fg_is_charging, int *data);
void fgauge_read_IM_current(void *data);
int dis_gm3_src_sel(bool is_dis);
bool fg_charging_state(void);

