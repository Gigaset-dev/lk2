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
#include <lk/init.h>
#include <pmic_auxadc_legacy.h>

#include "registers.h"

#define DEF_R_RATIO      1
#define DEF_AVG_NUM      8

static const struct auxadc_regs auxadc_regs_tbl[] = {
    PMIC_AUXADC_REG(BATADC, MT6359, AUXADC_RQST0, 0, AUXADC_ADC0),
    PMIC_AUXADC_REG(BAT_TEMP, MT6359, AUXADC_RQST0, 3, AUXADC_ADC3),
    PMIC_AUXADC_REG(CHIP_TEMP, MT6359, AUXADC_RQST0, 4, AUXADC_ADC4),
    PMIC_AUXADC_REG(VCORE_TEMP, MT6359, AUXADC_RQST1, 8, AUXADC_ADC38),
    PMIC_AUXADC_REG(VPROC_TEMP, MT6359, AUXADC_RQST1, 9, AUXADC_ADC39),
    PMIC_AUXADC_REG(VGPU_TEMP, MT6359, AUXADC_RQST1, 10, AUXADC_ADC40),
    PMIC_AUXADC_REG(ACCDET, MT6359, AUXADC_RQST0, 5, AUXADC_ADC5),
    PMIC_AUXADC_REG(VDCXO, MT6359, AUXADC_RQST0, 6, AUXADC_ADC6),
    PMIC_AUXADC_REG(TSX_TEMP, MT6359, AUXADC_RQST0, 7, AUXADC_ADC7),
    PMIC_AUXADC_REG(HPOFS_CAL, MT6359, AUXADC_RQST0, 9, AUXADC_ADC9),
    PMIC_AUXADC_REG(DCXO_TEMP, MT6359, AUXADC_RQST0, 10, AUXADC_ADC10),
    PMIC_AUXADC_REG(VBIF, MT6359, AUXADC_RQST0, 11, AUXADC_ADC11),
};

static const struct auxadc_device_chan_spec mt6359_chan_spec[] = {
    PMIC_AUXADC_CHAN_SPEC(BATADC, 7, 2, 128),
    PMIC_AUXADC_CHAN_SPEC(BAT_TEMP, 5, 2, DEF_AVG_NUM),
    PMIC_AUXADC_CHAN_SPEC(CHIP_TEMP, DEF_R_RATIO, DEF_R_RATIO, DEF_AVG_NUM),
    PMIC_AUXADC_CHAN_SPEC(VCORE_TEMP, DEF_R_RATIO, DEF_R_RATIO, DEF_AVG_NUM),
    PMIC_AUXADC_CHAN_SPEC(VPROC_TEMP, DEF_R_RATIO, DEF_R_RATIO, DEF_AVG_NUM),
    PMIC_AUXADC_CHAN_SPEC(VGPU_TEMP, DEF_R_RATIO, DEF_R_RATIO, DEF_AVG_NUM),
    PMIC_AUXADC_CHAN_SPEC(ACCDET, DEF_R_RATIO, DEF_R_RATIO, DEF_AVG_NUM),
    PMIC_AUXADC_CHAN_SPEC(VDCXO, 3, 2, DEF_AVG_NUM),
    PMIC_AUXADC_CHAN_SPEC(TSX_TEMP, DEF_R_RATIO, DEF_R_RATIO, 128),
    PMIC_AUXADC_CHAN_SPEC(HPOFS_CAL, DEF_R_RATIO, DEF_R_RATIO, 256),
    PMIC_AUXADC_CHAN_SPEC(DCXO_TEMP, DEF_R_RATIO, DEF_R_RATIO, 16),
    PMIC_AUXADC_CHAN_SPEC(VBIF, 5, 2, DEF_AVG_NUM),
};

static void mt6359_auxadc_init(uint level)
{
    pmic_auxadc_device_register_legacy(mt6359_chan_spec,
                                           countof(mt6359_chan_spec));
}

LK_INIT_HOOK(mt6359_auxadc, mt6359_auxadc_init, LK_INIT_LEVEL_PLATFORM_EARLY);
