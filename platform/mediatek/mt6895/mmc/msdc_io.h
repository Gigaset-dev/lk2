/*
 *
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <msdc.h>

/*******************************************************************************
 *PINMUX and GPIO definition
 ******************************************************************************/

#define MSDC_PIN_PULL_NONE      (0)
#define MSDC_PIN_PULL_DOWN      (1)
#define MSDC_PIN_PULL_UP        (2)
#define MSDC_PIN_KEEP           (3)

/* add pull down/up mode define */
#define MSDC_GPIO_PULL_UP       (0)
#define MSDC_GPIO_PULL_DOWN     (1)

#define MSDC_TDRDSEL_SLEEP      (0)
#define MSDC_TDRDSEL_3V         (1)
#define MSDC_TDRDSEL_1V8        (2)
#define MSDC_TDRDSEL_CUST       (3)

/*
 * MSDC Driving Strength Definition: specify as gear instead of driving
 */
#define MSDC_DRVN_GEAR0         0x0
#define MSDC_DRVN_GEAR1         0x1
#define MSDC_DRVN_GEAR2         0x2
#define MSDC_DRVN_GEAR3         0x3
#define MSDC_DRVN_GEAR4         0x4
#define MSDC_DRVN_GEAR5         0x5
#define MSDC_DRVN_GEAR6         0x6
#define MSDC_DRVN_GEAR7         0x7
#define MSDC_DRVN_DONT_CARE     0x0

#define msdc_set_driving(host, msdc_cap) \
    msdc_set_driving_by_id(host->id, msdc_cap)

#define msdc_set_ies(host, set_ies) \
    msdc_set_ies_by_id(host->id, set_ies)

#define msdc_set_sr(host, clk, cmd, dat, rst, ds) \
    msdc_set_sr_by_id(host->id, clk, cmd, dat, rst, ds)

#define msdc_set_smt(host, set_smt) \
    msdc_set_smt_by_id(host->id, set_smt)

#define msdc_set_tdsel(host, flag, value) \
    msdc_set_tdsel_by_id(host->id, flag, value)

#define msdc_set_rdsel(host, flag, value) \
    msdc_set_rdsel_by_id(host->id, flag, value)

#define msdc_get_tdsel(host, value) \
    msdc_get_tdsel_by_id(host->id, value)

#define msdc_get_rdsel(host, value) \
    msdc_get_rdsel_by_id(host->id, value)

#define msdc_dump_padctl(host) \
    msdc_dump_padctl_by_id(host->id)

#define msdc_pin_config(host, mode) \
    msdc_pin_config_by_id(host->id, mode)


/*******************************************************************************
 * CLOCK definition
 ******************************************************************************/

/* each PLL have different gears for select
 * software can used mux interface from clock management module to select
 */
enum {
    MSDC1_CLKSRC_26MHZ   = 0,
    MSDC1_CLKSRC_208MHZ,
    MSDC1_CLKSRC_182MHZ,
    MSDC1_CLKSRC_156MHZ,
    MSDC1_CLKSRC_200MHZ,
    MSDC1_CLKSRC_MAX
};

#define MSDC1_CLKSRC_DEFAULT    MSDC1_CLKSRC_200MHZ

#define MSDCPLL_FREQ            384000000

#define MSDC1_SRC_0             26000000
#define MSDC1_SRC_1             208000000
#define MSDC1_SRC_2             182000000
#define MSDC1_SRC_3             156000000
#define MSDC1_SRC_4             (MSDCPLL_FREQ / 2)

