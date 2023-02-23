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

#if defined(CUST_BATTERY_LOWVOL_THRESHOLD)
#define BATTERY_LOWVOL_THRESHOLD CUST_BATTERY_LOWVOL_THRESHOLD
#else
#define BATTERY_LOWVOL_THRESHOLD             3450
#endif

/**
 * for sleep and delay (mdelay and udelay)
 */
#define udelay(x)   spin(x)
#define mdelay(x)   udelay((x) * 1000)


#define UNIT_TRANS_10 10
/* battery meter parameter */

#define RBAT_PULL_UP_VOLT   2800
#define BIF_NTC_R 16000

struct FUELGAUGE_TEMPERATURE {
        signed int BatteryTemp;
        signed int TemperatureR;
};

struct FUELGAUGE_TEMPERATURE Fg_Temperature_Table_NTC10[21] = {
        {-40, 195652},
        {-35, 148171},
        {-30, 113347},
        {-25, 87559},
        {-20, 68237},
        {-15, 53650},
        {-10, 42506},
        {-5, 33892},
        {0, 27219},
        {5, 22021},
        {10, 17926},
        {15, 14674},
        {20, 12081},
        {25, 10000},
        {30, 8315},
        {35, 6948},
        {40, 5834},
        {45, 4917},
        {50, 4161},
        {55, 3535},
        {60, 3014}
};

static struct FUELGAUGE_TEMPERATURE Fg_Temperature_Table_NTC47[21] = {
        {-40, 1747920},
        {-35, 1245428},
        {-30, 898485},
        {-25, 655802},
        {-20, 483954},
        {-15, 360850},
        {-10, 271697},
        {-5, 206463},
        {0, 158214},
        {5, 122259},
        {10, 95227},
        {15, 74730},
        {20, 59065},
        {25, 47000},
        {30, 37643},
        {35, 30334},
        {40, 24591},
        {45, 20048},
        {50, 16433},
        {55, 13539},
        {60, 11210}
};

/* internal API */
void init_bat_cust_data(void);
int init_bat_cust_data_dt(void *fdt);
