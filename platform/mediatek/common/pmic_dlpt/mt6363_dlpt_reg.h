/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

/* PMIC Registers for DLPT */
#define MT6363_AUXADC_ADC42_L                0x10dc
#define MT6363_AUXADC_CON36                  0x11b2
#define MT6363_AUXADC_IMP0                   0x1208
#define MT6363_AUXADC_IMP1                   0x1209
#define MT6363_AUXADC_IMP2                   0x120a
#define MT6363_AUXADC_IMP3                   0x120b
#define MT6363_AUXADC_IMP4                   0x120c
#define MT6363_ISINK_EN_CTRL0                0x220b
#define MT6363_ISINK_EN_CTRL1                0x220c

#define AUXADC_IMP_RDY_MASK             (0x1 << 7)
#define AUXADC_SOURCE_LBAT_SEL_MASK     0x1
#define ISINK_EN_CTRL0_MASK             0xFF
#define ISINK_EN_CTRL1_MASK             0xF0
