/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include "ddp_reg.h"

enum disp_cm_id_t {
    DISP_CM0,
    DISP_CM1
};

#define DISP_REG_CM_EN               0x0000
    #define CM_EN                     BIT(0)
    #define CON_FLD_CM_EN             REG_FLD_MSB_LSB(0, 0)

#define DISP_REG_CM_RESET            0x0004
    #define CM_RESET                  BIT(0)

#define DISP_REG_CM_INTEN            0x0008
    #define IF_END_INT_EN             BIT(0)
    #define OF_END_INT_EN             BIT(1)

#define DISP_REG_CM_INTSTA           0x000C
    #define IF_END_INT                BIT(0)
    #define OF_END_INT                BIT(1)

#define DISP_REG_CM_STATUS           0x0010
    #define IF_UNFINISH               BIT(0)
    #define OF_UNFINISH               BIT(1)
    #define HANDSHAKE                 REG_FLD_MSB_LSB(27, 4)

#define DISP_REG_CM_CFG              0x0020
    #define RELAY_MODE                BIT(0)
    #define CON_FLD_RELAY_MODE        REG_FLD_MSB_LSB(0, 0)
    #define CM_ENGINE_EN              REG_FLD_MSB_LSB(1, 1)
    #define CM_GAMMA_OFF              BIT(2)
    #define CM_GAMMA_ROUND_EN         BIT(4)
    #define CON_FLD_CM_GAMMA_ROUND_EN REG_FLD_MSB_LSB(4, 4)
    #define HG_FCM_CK_ON              BIT(8)
    #define HF_FCM_CK_ON              BIT(9)
    #define CM_8B_SWITCH              REG_FLD_MSB_LSB(10, 10)
    #define REPEAT_DCM_OFF            BIT(12)
    #define CHKSUM_EN                 BIT(28)
    #define CHKSUM_SEL                REG_FLD_MSB_LSB(30, 29)

#define DISP_REG_CM_INPUT_COUNT      0x0024
    #define INP_PIX_CNT               REG_FLD_MSB_LSB(12, 0)
    #define INP_LINE_CNT              REG_FLD_MSB_LSB(28, 16)

#define DISP_REG_CM_OUTPUT_COUNT     0x0028
    #define OUTP_PIX_CNT              REG_FLD_MSB_LSB(12, 0)
    #define OUTP_LINE_CNT             REG_FLD_MSB_LSB(28, 16)

#define DISP_REG_CM_CHKSUM           0x002C
    #define CHKSUM                    REG_FLD_MSB_LSB(31, 0)

#define DISP_REG_CM_SIZE             0x0030
    #define VSIZE                     REG_FLD_MSB_LSB(12, 0)
    #define HSIZE                     REG_FLD_MSB_LSB(28, 16)
#define DISP_REG_CM_COEF_0           0x0080
    #define CM_C01                    REG_FLD_MSB_LSB(12, 0)
    #define CM_C00                    REG_FLD_MSB_LSB(28, 16)
#define DISP_REG_CM_COEF_1           0x0084
    #define CM_C10                    REG_FLD_MSB_LSB(12, 0)
    #define CM_C02                    REG_FLD_MSB_LSB(28, 16)
#define DISP_REG_CM_COEF_2           0x0088
    #define CM_C12                    REG_FLD_MSB_LSB(12, 0)
    #define CM_C11                    REG_FLD_MSB_LSB(28, 16)
#define DISP_REG_CM_COEF_3           0x008C
    #define CM_C21                    REG_FLD_MSB_LSB(12, 0)
    #define CM_C20                    REG_FLD_MSB_LSB(28, 16)
#define DISP_REG_CM_COEF_4           0x0090
    #define CM_COEFF_PURE_GRAY_EN     BIT(0)
    #define CM_COEFF_ROUND_EN         BIT(1)
    #define CM_C22                    REG_FLD_MSB_LSB(28, 16)

#define DISP_REG_CM_PRECISION        0x0094
    #define CM_PRECISION_MASK         REG_FLD_MSB_LSB(8, 0)

#define DISP_REG_CM_SHADOW           0x00A0
    #define READ_WRK_REG              BIT(0)
    #define CM_FORCE_COMMIT           BIT(1)
    #define CM_BYPASS_SHADOW          BIT(2)
#define DISP_REG_CM_DUMMY_REG        0x00C0
    #define DUMMY_REG                 REG_FLD_MSB_LSB(31, 0)

#define DISP_REG_CM_FUNC_DCM         0x00CC
    #define CM_FUNC_DCM_DIS           REG_FLD_MSB_LSB(31, 0)

#define DISP_REG_CM_COLOR_OFFSET_0   0x0100
    #define CM_COLOR_OFFSET_PURE_GRAY_EN     BIT(31)
    #define CM_COLOR_OFFSET_COEF_0           REG_FLD_MSB_LSB(29, 0)

#define DISP_REG_DISP_CM_COLOR_OFFSET_1  0x0104
    #define CM_COLOR_OFFSET_COEF_1           REG_FLD_MSB_LSB(29, 0)
#define DISP_REG_DISP_CM_COLOR_OFFSET_2  0x0108
    #define CM_COLOR_OFFSET_COEF_2           REG_FLD_MSB_LSB(29, 0)
