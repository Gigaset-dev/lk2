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

/* PMIC FGADC */
#define MT6359_FGADC_CON1                   0xd0a

#define PMIC_FG_LATCHDATA_ST_ADDR           MT6359_FGADC_CON1
#define PMIC_FG_LATCHDATA_ST_MASK           0x1
#define PMIC_FG_LATCHDATA_ST_SHIFT          15

#define PMIC_FG_SW_READ_PRE_ADDR            MT6359_FGADC_CON1
#define PMIC_FG_SW_READ_PRE_MASK            0x1
#define PMIC_FG_SW_READ_PRE_SHIFT           0x0

#define PMIC_FG_SW_CLEAR_ADDR               MT6359_FGADC_CON1
#define PMIC_FG_SW_CLEAR_MASK               0x1
#define PMIC_FG_SW_CLEAR_SHIFT              3

#define PMIC_FG_R_CURR_ADDR                 0xd88
#define PMIC_FG_R_CURR_MASK                 0xFFFF
#define PMIC_FG_R_CURR_SHIFT                0x0

#define PMIC_FG_CURRENT_OUT_ADDR            0xd8a
#define PMIC_FG_CURRENT_OUT_MASK            0xFFFF
#define PMIC_FG_CURRENT_OUT_SHIFT           0x0

#define PMIC_RG_FGADC_RST_SRC_SEL_ADDR      0xc2c
#define PMIC_RG_FGADC_RST_SRC_SEL_MASK      0x1
#define PMIC_RG_FGADC_RST_SRC_SEL_SHIFT     0x0

