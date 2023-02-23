/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include "ddp_reg.h"

enum disp_spr_id_t {
    DISP_SPR0,
    DISP_SPR1
};

#define DISP_REG_SPR_STA           0x0000

#define DISP_REG_SPR_INTEN         0x0004
    #define IF_END_INT_EN           BIT(0)
    #define OF_END_INT_EN           BIT(1)
#define DISP_REG_SPR_INTSTA        0x0008
    #define IF_END_INT              BIT(0)
    #define OF_END_INT              BIT(1)

#define DISP_REG_SPR_EN            0x000C
    #define SPR_EN                  BIT(0)
    #define CON_FLD_SPR_EN          REG_FLD_MSB_LSB(0, 0)
    #define SPR_LUT_EN              REG_FLD_MSB_LSB(1, 1)
    #define SPR_WRAP_MODE           BIT(2)
    #define SPR_RELAY_MODE          REG_FLD_MSB_LSB(4, 4)
    #define DISP_SPR_RELAY_MODE     BIT(5)
    #define IGNORE_ABNORMAL_SOF     BIT(16)
    #define READ_WRK_REG            BIT(20)
    #define SPR_FORCE_COMMIT        REG_FLD_MSB_LSB(21, 21)
    #define SPR_BYPASS_SHADOW       REG_FLD_MSB_LSB(22, 22)

#define DISP_REG_SPR_CFG           0x0010
    #define SPR_STALL_CG_ON         REG_FLD_MSB_LSB(0, 0)
    #define INDATA_RES_SEL          BIT(1)
    #define OUTDATA_RES_SEL         REG_FLD_MSB_LSB(3, 2)
    #define SPR_RGB_SWAP            REG_FLD_MSB_LSB(4, 4)
    #define RG_BYPASS_DITHER        REG_FLD_MSB_LSB(5, 5)
    #define POSTALIGN_EN            REG_FLD_MSB_LSB(16, 16)
    #define POSTALIGN_SEL           REG_FLD_MSB_LSB(25, 20)
    #define PADDING_REPEAT_EN       BIT(28)
    #define POSTALIGN_6TYPE_MODE    REG_FLD_MSB_LSB(31, 31)

#define DISP_REG_SPR_RST           0x0014
    #define SPR_RESET               BIT(0)
    #define SPR_TOP_RST             BIT(4)
    #define DISP_SPR_DBG_SEL        REG_FLD_MSB_LSB(19, 16)

#define DISP_REG_SPR_STATUS        0x0018
    #define IF_UNFINISH             BIT(0)
    #define OF_UNFINISH             BIT(1)
    #define FRAME_DONE              BIT(4)
    #define WRAP_STATE              REG_FLD_MSB_LSB(11, 8)
    #define HANDSHAKE               REG_FLD_MSB_LSB(31, 16)

#define DISP_REG_SPR_CHK_SUM0      0x001C
    #define CHECKSUM_EN             BIT(0)
    #define CHECKSUM_SEL            REG_FLD_MSB_LSB(2, 1)

#define DISP_REG_SPR_CHK_SUM1      0x0020
    #define CHECKSUM_LSB            REG_FLD_MSB_LSB(31, 0)

#define DISP_REG_SPR_CHK_SUM2      0x0024
    #define CHECKSUM_MSB            REG_FLD_MSB_LSB(31, 0)
#define DISP_REG_SPR_ROI_SIZE      0x002C
    #define REG_RESO_MAX_X          REG_FLD_MSB_LSB(12, 0)
    #define REG_RESO_MAX_Y          REG_FLD_MSB_LSB(28, 16)
#define DISP_REG_SPR_FRAME_DONE_DEL 0x0030
    #define DISP_FRAME_DONE_DEL     REG_FLD_MSB_LSB(7, 0)

#define DISP_REG_SPR_SW_SCRATCH    0x0038
    #define DISP_SPR_SW_SCRATCH     REG_FLD_MSB_LSB(31, 0)
#define DISP_REG_SPR_RDY_SEL       0x003C
    #define DISP_SPR_RDY_SEL        BIT(0)
    #define CROP_OUT_HSIZE          REG_FLD_MSB_LSB(28, 16)
#define DISP_REG_SPR_RDY_SEL_EN    0x0040
    #define DISP_SPR_RDY_SEL_EN     BIT(0)
    #define CROP_OUT_VSIZE          REG_FLD_MSB_LSB(28, 16)

#define DISP_REG_SPR_CK_ON         0x0044
    #define DISP_SPR_CK_ON          BIT(0)
#define DISP_REG_SPR_DBG0          0x0050
    #define INP_PIX_CNT             REG_FLD_MSB_LSB(12, 0)
    #define INP_LINE_CNT            REG_FLD_MSB_LSB(28, 16)

#define DISP_REG_SPR_DBG1          0x0054
    #define OUTP_PIX_CNT            REG_FLD_MSB_LSB(12, 0)
    #define OUTP_LINE_CNT           REG_FLD_MSB_LSB(28, 16)

#define DISP_REG_SPR_DBG2          0x0058
    #define HCNT_IN                 REG_FLD_MSB_LSB(13, 0)
    #define VCNT_IN                 REG_FLD_MSB_LSB(29, 16)

#define DISP_REG_SPR_DBG3          0x005C
    #define HCNT_OUT                REG_FLD_MSB_LSB(13, 0)
    #define VCNT_OUT                REG_FLD_MSB_LSB(29, 16)

#define DISP_REG_SPR_DBG4          0x0060
    #define PIX_CNT_POST            REG_FLD_MSB_LSB(12, 0)
    #define LINE_CNT_POST           REG_FLD_MSB_LSB(28, 16)

#define DISP_REG_SPR_DBG5          0x0064
    #define ROUT                    REG_FLD_MSB_LSB(11, 0)

#define DISP_REG_SPR_DBG6          0x0068
    #define GOUT                    REG_FLD_MSB_LSB(11, 0)

#define DISP_REG_SPR_DBG7          0x006C
    #define BOUT                    REG_FLD_MSB_LSB(11, 0)

#define DISP_REG_SPR_MANUAL_RST    0x0070
    #define RG_SPR_VSYNC_LOW        BIT(0)
    #define RG_SPR_VSYNC_PERIOD     REG_FLD_MSB_LSB(9, 4)

#define DISP_REG_SPR_DUMMY         0x0080
    #define RG_SPR_DUMMY            REG_FLD_MSB_LSB(31, 0)

#define DISP_REG_SPR_CTRL          0x0100
    #define REG_SYNC_SS             BIT(0)
    #define REG_SYNC_GS             BIT(4)
    #define SPR_POWERSAVING         REG_FLD_MSB_LSB(17, 16)
    #define SPR_SPECIALCASEEN       REG_FLD_MSB_LSB(28, 28)

#define DISP_REG_SPR_ARRANGE0      0x0104
    #define SPR_TH_H                REG_FLD_MSB_LSB(5, 0)
    #define SPR_TH_L                REG_FLD_MSB_LSB(21, 16)

#define DISP_REG_SPR_ARRANGE1      0x0108
    #define SPR_PIXELGROUP          REG_FLD_MSB_LSB(1, 0)
    #define SPR_ARRANGE_UL_P0       REG_FLD_MSB_LSB(6, 4)
    #define SPR_ARRANGE_UL_P1       REG_FLD_MSB_LSB(10, 8)
    #define SPR_ARRANGE_UL_P2       REG_FLD_MSB_LSB(14, 12)
    #define SPR_ARRANGE_DL_P0       REG_FLD_MSB_LSB(18, 16)
    #define SPR_ARRANGE_DL_P1       REG_FLD_MSB_LSB(22, 20)
    #define SPR_ARRANGE_DL_P2       REG_FLD_MSB_LSB(26, 24)

#define DISP_REG_SPR_ARRANGE2      0x010C
    #define SPR_VALIDDOTS_UL_P0 REG_FLD_MSB_LSB(1, 0)
    #define SPR_VALIDDOTS_UL_P1 REG_FLD_MSB_LSB(5, 4)
    #define SPR_VALIDDOTS_UL_P2 REG_FLD_MSB_LSB(9, 8)
    #define SPR_VALIDDOTS_DL_P0 REG_FLD_MSB_LSB(13, 12)
    #define SPR_VALIDDOTS_DL_P1 REG_FLD_MSB_LSB(17, 16)
    #define SPR_VALIDDOTS_DL_P2 REG_FLD_MSB_LSB(21, 20)
    #define SPR_VALIDDOTS_INC_UL REG_FLD_MSB_LSB(25, 24)
    #define SPR_VALIDDOTS_INC_DL REG_FLD_MSB_LSB(29, 28)
#define DISP_REG_SPR_WEIGHT0       0x0110
    #define SPR_PARA_UL_P0_D0_X REG_FLD_MSB_LSB(7, 0)
    #define SPR_PARA_UL_P0_D0_Y REG_FLD_MSB_LSB(15, 8)
    #define SPR_PARA_UL_P0_D1_X REG_FLD_MSB_LSB(23, 16)
    #define SPR_PARA_UL_P0_D1_Y REG_FLD_MSB_LSB(31, 24)

#define DISP_REG_SPR_WEIGHT1       0x0114
    #define SPR_PARA_UL_P0_D2_X REG_FLD_MSB_LSB(7, 0)
    #define SPR_PARA_UL_P0_D2_Y REG_FLD_MSB_LSB(15, 8)
    #define SPR_PARA_UL_P1_D0_X REG_FLD_MSB_LSB(23, 16)
    #define SPR_PARA_UL_P1_D0_Y REG_FLD_MSB_LSB(31, 24)
#define DISP_REG_SPR_WEIGHT2       0x0118
    #define SPR_PARA_UL_P1_D1_X REG_FLD_MSB_LSB(7, 0)
    #define SPR_PARA_UL_P1_D1_Y REG_FLD_MSB_LSB(15, 8)
    #define SPR_PARA_UL_P1_D2_X REG_FLD_MSB_LSB(23, 16)
    #define SPR_PARA_UL_P1_D2_Y REG_FLD_MSB_LSB(31, 24)
#define DISP_REG_SPR_WEIGHT3       0x011C
    #define SPR_PARA_UL_P2_D0_X REG_FLD_MSB_LSB(7, 0)
    #define SPR_PARA_UL_P2_D0_Y REG_FLD_MSB_LSB(15, 8)
    #define SPR_PARA_UL_P2_D1_X REG_FLD_MSB_LSB(23, 16)
    #define SPR_PARA_UL_P2_D1_Y REG_FLD_MSB_LSB(31, 24)
#define DISP_REG_SPR_WEIGHT4       0x0120
    #define SPR_PARA_UL_P2_D2_X REG_FLD_MSB_LSB(7, 0)
    #define SPR_PARA_UL_P2_D2_Y REG_FLD_MSB_LSB(15, 8)
    #define SPR_PARA_DL_P0_D0_X REG_FLD_MSB_LSB(23, 16)
    #define SPR_PARA_DL_P0_D0_Y REG_FLD_MSB_LSB(31, 24)

#define DISP_REG_SPR_WEIGHT5       0x0124
    #define SPR_PARA_DL_P0_D1_X REG_FLD_MSB_LSB(7, 0)
    #define SPR_PARA_DL_P0_D1_Y REG_FLD_MSB_LSB(15, 8)
    #define SPR_PARA_DL_P0_D2_X REG_FLD_MSB_LSB(23, 16)
    #define SPR_PARA_DL_P0_D2_Y REG_FLD_MSB_LSB(31, 24)

#define DISP_REG_SPR_WEIGHT6       0x0128
    #define SPR_PARA_DL_P1_D0_X REG_FLD_MSB_LSB(7, 0)
    #define SPR_PARA_DL_P1_D0_Y REG_FLD_MSB_LSB(15, 8)
    #define SPR_PARA_DL_P1_D1_X REG_FLD_MSB_LSB(23, 16)
    #define SPR_PARA_DL_P1_D1_Y REG_FLD_MSB_LSB(31, 24)

#define DISP_REG_SPR_WEIGHT7      0x012C
    #define SPR_PARA_DL_P1_D2_X REG_FLD_MSB_LSB(7, 0)
    #define SPR_PARA_DL_P1_D2_Y REG_FLD_MSB_LSB(15, 8)
    #define SPR_PARA_DL_P2_D0_X REG_FLD_MSB_LSB(23, 16)
    #define SPR_PARA_DL_P2_D0_Y REG_FLD_MSB_LSB(31, 24)
#define DISP_REG_SPR_WEIGHT8      0x0130
    #define SPR_PARA_DL_P2_D1_X REG_FLD_MSB_LSB(7, 0)
    #define SPR_PARA_DL_P2_D1_Y REG_FLD_MSB_LSB(15, 8)
    #define SPR_PARA_DL_P2_D2_X REG_FLD_MSB_LSB(23, 16)
    #define SPR_PARA_DL_P2_D2_Y REG_FLD_MSB_LSB(31, 24)

#define DISP_REG_SPR_BORDER0      0x0134
    #define SPR_BORDER_UP_UL_P0_D0 REG_FLD_MSB_LSB(1, 0)
    #define SPR_BORDER_UP_UL_P0_D1 REG_FLD_MSB_LSB(5, 4)
    #define SPR_BORDER_UP_UL_P0_D2 REG_FLD_MSB_LSB(9, 8)
    #define SPR_BORDER_UP_UL_P1_D0 REG_FLD_MSB_LSB(13, 12)
    #define SPR_BORDER_UP_UL_P1_D1 REG_FLD_MSB_LSB(17, 16)
    #define SPR_BORDER_UP_UL_P1_D2 REG_FLD_MSB_LSB(21, 20)
    #define SPR_BORDER_UP_UL_P2_D0 REG_FLD_MSB_LSB(25, 24)
    #define SPR_BORDER_UP_UL_P2_D1 REG_FLD_MSB_LSB(29, 28)

#define DISP_REG_SPR_BORDER1      0x0138
    #define SPR_BORDER_UP_UL_P2_D2 REG_FLD_MSB_LSB(1, 0)
    #define SPR_BORDER_UP_DL_P0_D0 REG_FLD_MSB_LSB(5, 4)
    #define SPR_BORDER_UP_DL_P0_D1 REG_FLD_MSB_LSB(9, 8)
    #define SPR_BORDER_UP_DL_P0_D2 REG_FLD_MSB_LSB(13, 12)
    #define SPR_BORDER_UP_DL_P1_D0 REG_FLD_MSB_LSB(17, 16)
    #define SPR_BORDER_UP_DL_P1_D1 REG_FLD_MSB_LSB(21, 20)
    #define SPR_BORDER_UP_DL_P1_D2 REG_FLD_MSB_LSB(25, 24)
    #define SPR_BORDER_UP_DL_P2_D0 REG_FLD_MSB_LSB(29, 28)

#define DISP_REG_SPR_BORDER2      0x013C
    #define SPR_BORDER_UP_DL_P2_D1 REG_FLD_MSB_LSB(1, 0)
    #define SPR_BORDER_UP_DL_P2_D2 REG_FLD_MSB_LSB(5, 4)
    #define SPR_BORDER_DN_UL_P0_D0 REG_FLD_MSB_LSB(9, 8)
    #define SPR_BORDER_DN_UL_P0_D1 REG_FLD_MSB_LSB(13, 12)
    #define SPR_BORDER_DN_UL_P0_D2 REG_FLD_MSB_LSB(17, 16)
    #define SPR_BORDER_DN_UL_P1_D0 REG_FLD_MSB_LSB(21, 20)
    #define SPR_BORDER_DN_UL_P1_D1 REG_FLD_MSB_LSB(25, 24)
    #define SPR_BORDER_DN_UL_P1_D2 REG_FLD_MSB_LSB(29, 28)

#define DISP_REG_SPR_BORDER3      0x0140
    #define SPR_BORDER_DN_UL_P2_D0 REG_FLD_MSB_LSB(1, 0)
    #define SPR_BORDER_DN_UL_P2_D1 REG_FLD_MSB_LSB(5, 4)
    #define SPR_BORDER_DN_UL_P2_D2 REG_FLD_MSB_LSB(9, 8)
    #define SPR_BORDER_DN_DL_P0_D0 REG_FLD_MSB_LSB(13, 12)
    #define SPR_BORDER_DN_DL_P0_D1 REG_FLD_MSB_LSB(17, 16)
    #define SPR_BORDER_DN_DL_P0_D2 REG_FLD_MSB_LSB(21, 20)
    #define SPR_BORDER_DN_DL_P1_D0 REG_FLD_MSB_LSB(25, 24)
    #define SPR_BORDER_DN_DL_P1_D1 REG_FLD_MSB_LSB(29, 28)

#define DISP_REG_SPR_BORDER4      0x0144
    #define SPR_BORDER_DN_DL_P1_D2 REG_FLD_MSB_LSB(1, 0)
    #define SPR_BORDER_DN_DL_P2_D0 REG_FLD_MSB_LSB(5, 4)
    #define SPR_BORDER_DN_DL_P2_D1 REG_FLD_MSB_LSB(9, 8)
    #define SPR_BORDER_DN_DL_P2_D2 REG_FLD_MSB_LSB(13, 12)
    #define SPR_BORDER_LT_UL_P0_D0 REG_FLD_MSB_LSB(17, 16)
    #define SPR_BORDER_LT_UL_P0_D1 REG_FLD_MSB_LSB(21, 20)
    #define SPR_BORDER_LT_UL_P0_D2 REG_FLD_MSB_LSB(25, 24)
    #define SPR_BORDER_LT_UL_P1_D0 REG_FLD_MSB_LSB(29, 28)

#define DISP_REG_SPR_BORDER5      0x0148
    #define SPR_BORDER_LT_UL_P1_D1 REG_FLD_MSB_LSB(1, 0)
    #define SPR_BORDER_LT_UL_P1_D2 REG_FLD_MSB_LSB(5, 4)
    #define SPR_BORDER_LT_UL_P2_D0 REG_FLD_MSB_LSB(9, 8)
    #define SPR_BORDER_LT_UL_P2_D1 REG_FLD_MSB_LSB(13, 12)
    #define SPR_BORDER_LT_UL_P2_D2 REG_FLD_MSB_LSB(17, 16)
    #define SPR_BORDER_LT_DL_P0_D0 REG_FLD_MSB_LSB(21, 20)
    #define SPR_BORDER_LT_DL_P0_D1 REG_FLD_MSB_LSB(25, 24)
    #define SPR_BORDER_LT_DL_P0_D2 REG_FLD_MSB_LSB(29, 28)

#define DISP_REG_SPR_BORDER6      0x014C
    #define SPR_BORDER_LT_DL_P1_D0 REG_FLD_MSB_LSB(1, 0)
    #define SPR_BORDER_LT_DL_P1_D1 REG_FLD_MSB_LSB(5, 4)
    #define SPR_BORDER_LT_DL_P1_D2 REG_FLD_MSB_LSB(9, 8)
    #define SPR_BORDER_LT_DL_P2_D0 REG_FLD_MSB_LSB(13, 12)
    #define SPR_BORDER_LT_DL_P2_D1 REG_FLD_MSB_LSB(17, 16)
    #define SPR_BORDER_LT_DL_P2_D2 REG_FLD_MSB_LSB(21, 20)
    #define SPR_BORDER_RT_UL_P0_D0 REG_FLD_MSB_LSB(25, 24)
    #define SPR_BORDER_RT_UL_P0_D1 REG_FLD_MSB_LSB(29, 28)

#define DISP_REG_SPR_BORDER7      0x0150
    #define SPR_BORDER_RT_UL_P0_D2 REG_FLD_MSB_LSB(1, 0)
    #define SPR_BORDER_RT_UL_P1_D0 REG_FLD_MSB_LSB(3, 2)
    #define SPR_BORDER_RT_UL_P1_D1 REG_FLD_MSB_LSB(5, 4)
    #define SPR_BORDER_RT_UL_P1_D2 REG_FLD_MSB_LSB(7, 6)
    #define SPR_BORDER_RT_UL_P2_D0 REG_FLD_MSB_LSB(9, 8)
    #define SPR_BORDER_RT_UL_P2_D1 REG_FLD_MSB_LSB(11, 10)
    #define SPR_BORDER_RT_UL_P2_D2 REG_FLD_MSB_LSB(13, 12)
    #define SPR_BORDER_RT_DL_P0_D0 REG_FLD_MSB_LSB(15, 14)

#define DISP_REG_SPR_BORDER8     0x0154
    #define SPR_BORDER_RT_DL_P0_D1 REG_FLD_MSB_LSB(1, 0)
    #define SPR_BORDER_RT_DL_P0_D2 REG_FLD_MSB_LSB(3, 2)
    #define SPR_BORDER_RT_DL_P1_D0 REG_FLD_MSB_LSB(5, 4)
    #define SPR_BORDER_RT_DL_P1_D1 REG_FLD_MSB_LSB(7, 6)
    #define SPR_BORDER_RT_DL_P1_D2 REG_FLD_MSB_LSB(9, 8)
    #define SPR_BORDER_RT_DL_P2_D0 REG_FLD_MSB_LSB(11, 10)
    #define SPR_BORDER_RT_DL_P2_D1 REG_FLD_MSB_LSB(13, 12)
    #define SPR_BORDER_RT_DL_P2_D2 REG_FLD_MSB_LSB(15, 14)

#define DISP_REG_SPR_BORDER9     0x0158
    #define SPR_BORDER_UP_PARA0 REG_FLD_MSB_LSB(7, 0)
    #define SPR_BORDER_UP_PARA1 REG_FLD_MSB_LSB(15, 8)
    #define SPR_BORDER_DN_PARA0 REG_FLD_MSB_LSB(23, 16)
    #define SPR_BORDER_DN_PARA1 REG_FLD_MSB_LSB(31, 24)

#define DISP_REG_SPR_BORDER10    0x015C
    #define SPR_BORDER_RT_PARA0 REG_FLD_MSB_LSB(7, 0)
    #define SPR_BORDER_RT_PARA1 REG_FLD_MSB_LSB(15, 8)
    #define SPR_BORDER_LT_PARA0 REG_FLD_MSB_LSB(23, 16)
    #define SPR_BORDER_LT_PARA1 REG_FLD_MSB_LSB(31, 24)

#define DISP_REG_SPR_SPE0        0x0160
    #define SPR_SET4_CASE0_SET REG_FLD_MSB_LSB(8, 0)
    #define SPR_SET4_CASE0_EN  BIT(12)
    #define SPR_SET4_CASE1_SET REG_FLD_MSB_LSB(24, 16)
    #define SPR_SET4_CASE1_EN  BIT(28)

#define DISP_REG_SPR_SPE1        0x0164
    #define SPR_SET4_CASE2_SET REG_FLD_MSB_LSB(8, 0)
    #define SPR_SET4_CASE2_EN  BIT(12)
    #define SPR_SET4_CASE3_SET REG_FLD_MSB_LSB(24, 16)
    #define SPR_SET4_CASE3_EN  BIT(28)

#define DISP_REG_SPR_SPE2        0x0168
    #define SPR_SET4_CASE4_SET REG_FLD_MSB_LSB(8, 0)
    #define SPR_SET4_CASE4_EN  BIT(12)
    #define SPR_SET4_CASE5_SET REG_FLD_MSB_LSB(24, 16)
    #define SPR_SET4_CASE5_EN  BIT(28)

#define DISP_REG_SPR_SPE3        0x016C
    #define SPR_SET3_CASE0_SET REG_FLD_MSB_LSB(8, 0)
    #define SPR_SET3_CASE0_EN  BIT(12)
    #define SPR_SET3_CASE1_SET REG_FLD_MSB_LSB(24, 16)
    #define SPR_SET3_CASE1_EN  BIT(28)

#define DISP_REG_SPR_SPE4        0x0170
    #define SPR_SET3_CASE2_SET REG_FLD_MSB_LSB(8, 0)
    #define SPR_SET3_CASE2_EN  BIT(12)
    #define SPR_SET3_CASE3_SET REG_FLD_MSB_LSB(24, 16)
    #define SPR_SET3_CASE3_EN  BIT(28)

#define DISP_REG_SPR_SPE5        0x0174
    #define SPR_SET2_CASE0_SET REG_FLD_MSB_LSB(8, 0)
    #define SPR_SET2_CASE0_EN  BIT(12)
    #define SPR_SET2_CASE1_SET REG_FLD_MSB_LSB(24, 16)
    #define SPR_SET2_CASE1_EN  BIT(28)

#define DISP_REG_SPR_SPE6        0x0178
    #define SPR_SET2_CASE2_SET REG_FLD_MSB_LSB(8, 0)
    #define SPR_SET2_CASE2_EN  BIT(12)
    #define SPR_SET2_CASE3_SET REG_FLD_MSB_LSB(24, 16)
    #define SPR_SET2_CASE3_EN  BIT(28)

#define DISP_REG_SPR_SPE7        0x017C
    #define SPR_SET2_CASE4_SET REG_FLD_MSB_LSB(8, 0)
    #define SPR_SET2_CASE4_EN  BIT(12)
    #define SPR_SET2_CASE5_SET REG_FLD_MSB_LSB(24, 16)
    #define SPR_SET2_CASE5_EN  BIT(28)

#define DISP_REG_SPR_SPE8        0x0180
    #define SPR_SET2_CASE6_SET REG_FLD_MSB_LSB(8, 0)
    #define SPR_SET2_CASE6_EN  BIT(12)
    #define SPR_SET2_CASE7_SET REG_FLD_MSB_LSB(24, 16)
    #define SPR_SET2_CASE7_EN  BIT(28)

#define DISP_REG_SPR_SPE9        0x0184
    #define SPR_SET2_CASE8_SET REG_FLD_MSB_LSB(8, 0)
    #define SPR_SET2_CASE8_EN  BIT(12)
    #define SPR_SET2_CASE9_SET REG_FLD_MSB_LSB(24, 16)
    #define SPR_SET2_CASE9_EN  BIT(28)

#define DISP_REG_SPR_SPE10       0x0188
    #define SPR_SET2_CASE10_SET REG_FLD_MSB_LSB(8, 0)
    #define SPR_SET2_CASE10_EN  BIT(12)
    #define SPR_SET2_CASE11_SET REG_FLD_MSB_LSB(24, 16)
    #define SPR_SET2_CASE11_EN  BIT(28)

#define DISP_REG_SPR_SPE11       0x018C
    #define SPR_SET1_CASE0_SET REG_FLD_MSB_LSB(8, 0)
    #define SPR_SET1_CASE0_EN  BIT(12)
    #define SPR_SET1_CASE1_SET REG_FLD_MSB_LSB(24, 16)
    #define SPR_SET1_CASE1_EN  BIT(28)

#define DISP_REG_SPR_SPE12       0x0190
    #define SPR_SET1_CASE2_SET REG_FLD_MSB_LSB(8, 0)
    #define SPR_SET1_CASE2_EN  BIT(12)
    #define SPR_SET1_CASE3_SET REG_FLD_MSB_LSB(24, 16)
    #define SPR_SET1_CASE3_EN  BIT(28)

#define DISP_REG_SPR_SPE13       0x0194
    #define SPR_SET1_CASE4_SET REG_FLD_MSB_LSB(8, 0)
    #define SPR_SET1_CASE4_EN  BIT(12)
    #define SPR_SET1_CASE5_SET REG_FLD_MSB_LSB(24, 16)
    #define SPR_SET1_CASE5_EN  BIT(28)

#define DISP_REG_SPR_SPE14       0x0198
    #define SPR_SET1_CASE6_SET REG_FLD_MSB_LSB(8, 0)
    #define SPR_SET1_CASE6_EN  BIT(12)
    #define SPR_SET1_CASE7_SET REG_FLD_MSB_LSB(24, 16)
    #define SPR_SET1_CASE7_EN  BIT(28)

#define DISP_REG_SPR_GAMMA0      0x019C
    #define GV0 REG_FLD_MSB_LSB(12, 0)
    #define GV1 REG_FLD_MSB_LSB(28, 16)

#define DISP_REG_SPR_GAMMA16     0x01DC
    #define GV32 REG_FLD_MSB_LSB(12, 0)
    #define TF_IN_EN BIT(16)

#define DISP_REG_SPR_DEGAMMA0    0x01E0
    #define DGV0 REG_FLD_MSB_LSB(11, 0)
    #define DGV1 REG_FLD_MSB_LSB(27, 16)

#define DISP_REG_SPR_DEGAMMA16   0x0224
    #define DGV32 REG_FLD_MSB_LSB(11, 0)
    #define TF_OUT_EN BIT(16)
    #define DEGAMMA_CLAMP_OPT BIT(17)

#define DISP_REG_SPR_DITHER_0    0x0300
    #define START BIT(0)
    #define OUT_SEL BIT(4)
    #define FRAME_DONE_DEL REG_FLD_MSB_LSB(15, 8)
    #define CRC_CEN BIT(16)
    #define CRC_START BIT(20)
    #define CRC_CLR BIT(24)

#define DISP_REG_SPR_DITHER_1    0x0304
    #define DITHER_SOFT_RESET BIT(0)

#define DISP_REG_SPR_DITHER_2    0x0308
    #define AUX0_I BIT(0)
    #define AUX1_I BIT(1)

#define DISP_REG_SPR_DITHER_5    0x0314
    #define W_DEMO REG_FLD_MSB_LSB(15, 0)

#define DISP_REG_SPR_DITHER_6    0x0318
    #define EDITHER_EN   BIT(0)
    #define LFSR_EN      BIT(1)
    #define RDITHER_EN   BIT(2)
    #define ROUND_EN     BIT(3)
    #define FPHASE       REG_FLD_MSB_LSB(9, 4)
    #define FPHASE_EN    BIT(12)
    #define FPHASE_R     BIT(13)
    #define LEFT_EN      REG_FLD_MSB_LSB(15, 14)
    #define WRAP_MODE    BIT(16)

#define DISP_REG_SPR_DITHER_7    0x031C
    #define DRMODE_R REG_FLD_MSB_LSB(1, 0)
    #define DRMODE_G REG_FLD_MSB_LSB(5, 4)
    #define DRMODE_B REG_FLD_MSB_LSB(9, 8)

#define DISP_REG_SPR_DITHER_8    0x0320
    #define INK BIT(0)
    #define INK_DATA_R REG_FLD_MSB_LSB(27, 16)

#define DISP_REG_SPR_DITHER_9    0x0324
    #define INK_DATA_G REG_FLD_MSB_LSB(11, 0)
    #define INK_DATA_B REG_FLD_MSB_LSB(27, 16)

#define DISP_REG_SPR_DITHER_10   0x0328
    #define FPHASE_CTRL REG_FLD_MSB_LSB(1, 0)
    #define FPHASE_SEL REG_FLD_MSB_LSB(5, 4)
    #define FPHASE_BIT REG_FLD_MSB_LSB(10, 8)

#define DISP_REG_SPR_DITHER_11   0x032C
    #define SUBPIX_EN BIT(0)
    #define SUB_R REG_FLD_MSB_LSB(5, 4)
    #define SUB_G REG_FLD_MSB_LSB(9, 8)
    #define SUB_B REG_FLD_MSB_LSB(13, 12)

#define DISP_REG_SPR_DITHER_12   0x0330
    #define LSB_OFF BIT(0)
    #define TABLE_EN REG_FLD_MSB_LSB(5, 4)
    #define H_ACTIVE REG_FLD_MSB_LSB(31, 16)

#define DISP_REG_SPR_DITHER_13   0x0334
    #define RSHIFT_R REG_FLD_MSB_LSB(2, 0)
    #define RSHIFT_G REG_FLD_MSB_LSB(6, 4)
    #define RSHIFT_B REG_FLD_MSB_LSB(10, 8)

#define DISP_REG_SPR_DITHER_14   0x0338
    #define TESTPIN_EN BIT(0)
    #define DIFF_SHIFT REG_FLD_MSB_LSB(6, 4)
    #define DEBUG_MODE REG_FLD_MSB_LSB(9, 8)

#define DISP_REG_SPR_DITHER_15   0x033C
    #define NEW_BIT_MODE BIT(0)
    #define INPUT_RSHIFT_R REG_FLD_MSB_LSB(18, 16)
    #define ADD_LSHIFT_R REG_FLD_MSB_LSB(22, 20)
    #define OVFLW_BIT_R REG_FLD_MSB_LSB(26, 24)
    #define LSB_ERR_SHIFT_R REG_FLD_MSB_LSB(30, 28)

#define DISP_REG_SPR_DITHER_16   0x0340
    #define INPUT_RSHIFT_G REG_FLD_MSB_LSB(18, 16)
    #define ADD_LSHIFT_G REG_FLD_MSB_LSB(22, 20)
    #define OVFLW_BIT_G REG_FLD_MSB_LSB(26, 24)
    #define LSB_ERR_SHIFT_G REG_FLD_MSB_LSB(30, 28)
    #define INPUT_RSHIFT_B REG_FLD_MSB_LSB(18, 16)
    #define ADD_LSHIFT_B REG_FLD_MSB_LSB(22, 20)
    #define OVFLW_BIT_B REG_FLD_MSB_LSB(26, 24)
    #define LSB_ERR_SHIFT_B REG_FLD_MSB_LSB(30, 28)

#define DISP_REG_SPR_DITHER_17   0x0344
    #define CRC_OUT REG_FLD_MSB_LSB(15, 0)
    #define CRC_RDY BIT(16)

#define DISP_REG_SPR_DITHER_18   0x0348
    #define SPR_FUNC_DCM_DIS BIT(0)
