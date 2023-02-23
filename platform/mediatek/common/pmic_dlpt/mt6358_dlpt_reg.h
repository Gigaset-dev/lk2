/*
 * Copyright (c) 2021 MediaTek Inc.
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
#define MT6358_AUXADC_ADC28                  0x10c0
#define MT6358_AUXADC_CON16                  0x11ac
#define MT6358_AUXADC_IMP0                   0x1208
#define MT6358_AUXADC_IMP1                   0x120a
#define MT6358_AUXADC_IMP2                   0x120c
#define MT6358_AUXADC_DCM_CON                0x1260
#define MT6358_ISINK0_CON1                   0x1f0a
#define MT6358_ISINK1_CON1                   0x1f0c
#define MT6358_ISINK_EN_CTRL_SMPL            0x1f10

#define ISINK_STEP_MASK     0x7
#define ISINK_STEP_SHIFT    12
#define ISINK_BIAS_EN_MASK  0x3
#define ISINK_BIAS_EN_SHIFT 8
#define ISINK_EN_MASK       0x3
#define ISINK_EN_SHIFT      0
