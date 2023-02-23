/*
 * Copyright (c) 2022 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

/* PMIC FGADC */
#define MT6358_FGADC_CON1                   0xd0a

#define PMIC_FG_LATCHDATA_ST_ADDR           MT6358_FGADC_CON1
#define PMIC_FG_LATCHDATA_ST_MASK           0x1
#define PMIC_FG_LATCHDATA_ST_SHIFT          15

#define PMIC_FG_SW_READ_PRE_ADDR            MT6358_FGADC_CON1
#define PMIC_FG_SW_READ_PRE_MASK            0x1
#define PMIC_FG_SW_READ_PRE_SHIFT           0

#define PMIC_FG_SW_CLEAR_ADDR               MT6358_FGADC_CON1
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

