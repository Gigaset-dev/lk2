/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <debug.h>
#include <lk/init.h>
#include <pmic_auxadc_legacy.h>
#include <trace.h>
#include "registers.h"

#define LOCAL_TRACE 0

#define DEF_R_RATIO      1
#define DEF_AVG_NUM      8

static const struct auxadc_regs auxadc_regs_tbl[] = {
    PMIC_AUXADC_REG(BATADC, MT6357, AUXADC_RQST0, 0, AUXADC_ADC0),
    PMIC_AUXADC_REG(ISENSE, MT6357, AUXADC_RQST0, 1, AUXADC_ADC1),
    PMIC_AUXADC_REG(VCDT, MT6357, AUXADC_RQST0, 2, AUXADC_ADC2),
    PMIC_AUXADC_REG(BAT_TEMP, MT6357, AUXADC_RQST0, 3, AUXADC_ADC3),
    PMIC_AUXADC_REG(CHIP_TEMP, MT6357, AUXADC_RQST0, 4, AUXADC_ADC4),
    PMIC_AUXADC_REG(ACCDET, MT6357, AUXADC_RQST0, 5, AUXADC_ADC5),
    PMIC_AUXADC_REG(VBIF, MT6357, AUXADC_RQST0, 11, AUXADC_ADC11),
};

static const struct auxadc_device_chan_spec mt6357_chan_spec[] = {
    PMIC_AUXADC_CHAN_SPEC(BATADC, 3, 1, 128),
    PMIC_AUXADC_CHAN_SPEC(ISENSE, 3, 1, 128),
    PMIC_AUXADC_CHAN_SPEC(VCDT, DEF_R_RATIO, DEF_R_RATIO, DEF_AVG_NUM),
    PMIC_AUXADC_CHAN_SPEC(BAT_TEMP, DEF_R_RATIO, DEF_R_RATIO, DEF_AVG_NUM),
    PMIC_AUXADC_CHAN_SPEC(CHIP_TEMP, DEF_R_RATIO, DEF_R_RATIO, DEF_AVG_NUM),
    PMIC_AUXADC_CHAN_SPEC(ACCDET, DEF_R_RATIO, DEF_R_RATIO, DEF_AVG_NUM),
    PMIC_AUXADC_CHAN_SPEC(VBIF, DEF_R_RATIO, DEF_R_RATIO, DEF_AVG_NUM),
};

static void mt6357_auxadc_init(uint level)
{
    int ret;

    ret = pmic_auxadc_device_register_legacy(mt6357_chan_spec,
                                                 countof(mt6357_chan_spec));
    if (ret)
        LTRACEF_LEVEL(CRITICAL, "%s fail\n", __func__);
}

LK_INIT_HOOK(mt6357_auxadc, mt6357_auxadc_init, LK_INIT_LEVEL_PLATFORM_EARLY);
