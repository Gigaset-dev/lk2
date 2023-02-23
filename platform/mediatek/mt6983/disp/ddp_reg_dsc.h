/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include "ddp_reg.h"

#define DISP_REG_DSC_CON            0x0000
    #define DSC_EN                 BIT(0)
    #define DSC_DUAL_INOUT            BIT(2)
    #define DSC_IN_SRC_SEL            BIT(3)
    #define DSC_BYPASS            BIT(4)
    #define DSC_UFOE_SEL            BIT(16)
    #define CON_FLD_DSC_EN            REG_FLD_MSB_LSB(0, 0)
    #define CON_FLD_DISP_DSC_BYPASS        REG_FLD_MSB_LSB(4, 4)
    #define CON_FLD_DISP_DSC_RELAY        REG_FLD_MSB_LSB(5, 5)
    #define DSC_EMPTY_FLAG_SEL REG_FLD_MSB_LSB(15, 14)

#define DISP_REG_DSC_SPR            0x0014
    #define CFG_FLD_DSC_SPR_EN    BIT(26)
    #define CFG_FLD_DSC_SPR_FORMAT_SEL    BIT(24)

#define DISP_REG_DSC_PIC_W            0x0018
    #define CFG_FLD_PIC_WIDTH        REG_FLD_MSB_LSB(15, 0)
    #define CFG_FLD_PIC_HEIGHT_M1        REG_FLD_MSB_LSB(31, 16)

#define DISP_REG_DSC_PIC_H            0x001C

#define DISP_REG_DSC_SLICE_W            0x0020
    #define CFG_FLD_SLICE_WIDTH        REG_FLD_MSB_LSB(15, 0)

#define DISP_REG_DSC_SLICE_H            0x0024

#define DISP_REG_DSC_CHUNK_SIZE            0x0028

#define DISP_REG_DSC_BUF_SIZE            0x002C

#define DISP_REG_DSC_MODE            0x0030
    #define DSC_SLICE_MODE            BIT(0)
    #define DSC_RGB_SWAP            BIT(2)

#define DISP_REG_DSC_CFG            0x0034

#define DISP_REG_DSC_PAD            0x0038
#define DISP_REG_DSC_ENC_WIDTH        0x003C

#define DISP_REG_DSC_DBG_CON        0x0060
    #define DSC_CKSM_CAL_EN            BIT(9)
#define DISP_REG_DSC_OBUF            0x0070
#define DISP_REG_DSC_PPS0            0x0080
#define DISP_REG_DSC_PPS1            0x0084
#define DISP_REG_DSC_PPS2            0x0088
#define DISP_REG_DSC_PPS3            0x008C
#define DISP_REG_DSC_PPS4            0x0090
#define DISP_REG_DSC_PPS5            0x0094
#define DISP_REG_DSC_PPS6            0x0098
#define DISP_REG_DSC_PPS7            0x009C
#define DISP_REG_DSC_PPS8            0x00A0
#define DISP_REG_DSC_PPS9            0x00A4
#define DISP_REG_DSC_PPS10            0x00A8
#define DISP_REG_DSC_PPS11            0x00AC
#define DISP_REG_DSC_PPS12            0x00B0
#define DISP_REG_DSC_PPS13            0x00B4
#define DISP_REG_DSC_PPS14            0x00B8
#define DISP_REG_DSC_PPS15            0x00BC
#define DISP_REG_DSC_PPS16            0x00C0
#define DISP_REG_DSC_PPS17            0x00C4
#define DISP_REG_DSC_PPS18            0x00C8
#define DISP_REG_DSC_PPS19            0x00CC

#define DISP_REG_DSC_SHADOW            0x0200
    #define DSC_FORCE_COMMIT        BIT(1)
    #define DSC_FLD_VER            REG_FLD_MSB_LSB(6, 5)

