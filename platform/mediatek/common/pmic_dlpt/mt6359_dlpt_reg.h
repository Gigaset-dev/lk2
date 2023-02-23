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

/* PMIC Registers for DLPT */
#define MT6359_AUXADC_CON16                  0x11ac
#define MT6359_AUXADC_IMP0                   0x1208
#define MT6359_AUXADC_IMP1                   0x120a
#define MT6359_AUXADC_IMP2                   0x120c
#define MT6359_AUXADC_IMP3                   0x120e
#define MT6359_AUXADC_IMP4                   0x1210
#define MT6359_AUXADC_IMP5                   0x1212
#define MT6359_DUMMYLOAD_ANA_CON0            0x2108
#define MT6359_ISINK0_CON1                   0x210a
#define MT6359_ISINK1_CON1                   0x210c
#define MT6359_ISINK_EN_CTRL_SMPL            0x2110

#define ISINK_STEP_MASK     0x7
#define ISINK_STEP_SHIFT    12
#define ISINK_BIAS_EN_MASK  0x3
#define ISINK_BIAS_EN_SHIFT 8
#define ISINK_EN_MASK       0x3
#define ISINK_EN_SHIFT      0
