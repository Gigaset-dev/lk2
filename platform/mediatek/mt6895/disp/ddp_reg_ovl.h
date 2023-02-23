/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include "DISP_OVL_c_header.h"

#define DISP_REG_OVL_STA                          DISP_OVL0_OVL_STA
    #define STA_FLD_RUN                               OVL_STA_FLD_OVL_RUN
    #define STA_FLD_RDMA0_IDLE                        OVL_STA_FLD_RDMA0_IDLE
    #define STA_FLD_RDMA1_IDLE                        OVL_STA_FLD_RDMA1_IDLE
    #define STA_FLD_RDMA2_IDLE                        OVL_STA_FLD_RDMA2_IDLE
    #define STA_FLD_RDMA3_IDLE                        OVL_STA_FLD_RDMA3_IDLE

#define DISP_REG_OVL_INTEN                        DISP_OVL0_OVL_INTEN
    #define INTEN_FLD_REG_CMT_INTEN                   OVL_INTEN_FLD_OVL_REG_CMT_INTEN
    #define INTEN_FLD_FME_CPL_INTEN                   OVL_INTEN_FLD_OVL_FME_CPL_INTEN
    #define INTEN_FLD_FME_UND_INTEN                   OVL_INTEN_FLD_OVL_FME_UND_INTEN
    #define INTEN_FLD_FME_SWRST_DONE_INTEN            OVL_INTEN_FLD_OVL_FME_SWRST_DONE_INTEN
    #define INTEN_FLD_FME_HWRST_DONE_INTEN            OVL_INTEN_FLD_OVL_FME_HWRST_DONE_INTEN
    #define INTEN_FLD_RDMA0_EOF_ABNORMAL_INTEN        OVL_INTEN_FLD_RDMA0_EOF_ABNORMAL_INTEN
    #define INTEN_FLD_RDMA1_EOF_ABNORMAL_INTEN        OVL_INTEN_FLD_RDMA1_EOF_ABNORMAL_INTEN
    #define INTEN_FLD_RDMA2_EOF_ABNORMAL_INTEN        OVL_INTEN_FLD_RDMA2_EOF_ABNORMAL_INTEN
    #define INTEN_FLD_RDMA3_EOF_ABNORMAL_INTEN        OVL_INTEN_FLD_RDMA3_EOF_ABNORMAL_INTEN
    #define INTEN_FLD_RDMA0_SMI_UNDERFLOW_INTEN       OVL_INTEN_FLD_RDMA0_SMI_UNDERFLOW_INTEN
    #define INTEN_FLD_RDMA1_SMI_UNDERFLOW_INTEN       OVL_INTEN_FLD_RDMA1_SMI_UNDERFLOW_INTEN
    #define INTEN_FLD_RDMA2_SMI_UNDERFLOW_INTEN       OVL_INTEN_FLD_RDMA2_SMI_UNDERFLOW_INTEN
    #define INTEN_FLD_RDMA3_SMI_UNDERFLOW_INTEN       OVL_INTEN_FLD_RDMA3_SMI_UNDERFLOW_INTEN
    #define INTEN_FLD_ABNORMAL_SOF                    OVL_INTEN_FLD_ABNORMAL_SOF_INTEN
    #define INTEN_FLD_START_INTEN                     OVL_INTEN_FLD_OVL_START_INTEN


#define DISP_REG_OVL_INTSTA                       DISP_OVL0_OVL_INTSTA
    #define INTSTA_FLD_REG_CMT_INTSTA                 OVL_INTSTA_FLD_OVL_REG_CMT_INTSTA
    #define INTSTA_FLD_FME_CPL_INTSTA                 OVL_INTSTA_FLD_OVL_FME_CPL_INTSTA
    #define INTSTA_FLD_FME_UND_INTSTA                 OVL_INTSTA_FLD_OVL_FME_UND_INTSTA
    #define INTSTA_FLD_FME_SWRST_DONE_INTSTA          OVL_INTSTA_FLD_OVL_FME_SWRST_DONE_INTSTA
    #define INTSTA_FLD_FME_HWRST_DONE_INTSTA          OVL_INTSTA_FLD_OVL_FME_HWRST_DONE_INTSTA
    #define INTSTA_FLD_RDMA0_EOF_ABNORMAL_INTSTA      OVL_INTSTA_FLD_RDMA0_EOF_ABNORMAL_INTSTA
    #define INTSTA_FLD_RDMA1_EOF_ABNORMAL_INTSTA      OVL_INTSTA_FLD_RDMA1_EOF_ABNORMAL_INTSTA
    #define INTSTA_FLD_RDMA2_EOF_ABNORMAL_INTSTA      OVL_INTSTA_FLD_RDMA2_EOF_ABNORMAL_INTSTA
    #define INTSTA_FLD_RDMA3_EOF_ABNORMAL_INTSTA      OVL_INTSTA_FLD_RDMA3_EOF_ABNORMAL_INTSTA
    #define INTSTA_FLD_RDMA0_SMI_UNDERFLOW_INTSTA     OVL_INTSTA_FLD_RDMA0_SMI_UNDERFLOW_INTSTA
    #define INTSTA_FLD_RDMA1_SMI_UNDERFLOW_INTSTA     OVL_INTSTA_FLD_RDMA1_SMI_UNDERFLOW_INTSTA
    #define INTSTA_FLD_RDMA2_SMI_UNDERFLOW_INTSTA     OVL_INTSTA_FLD_RDMA2_SMI_UNDERFLOW_INTSTA
    #define INTSTA_FLD_RDMA3_SMI_UNDERFLOW_INTSTA     OVL_INTSTA_FLD_RDMA3_SMI_UNDERFLOW_INTSTA
    #define INTSTA_FLD_ABNORMAL_SOF                   OVL_INTSTA_FLD_ABNORMAL_SOF_INTSTA
    #define INTSTA_FLD_START_INTEN                    OVL_INTSTA_FLD_OVL_START_INTSTA

#define DISP_REG_OVL_TRIG                         DISP_OVL0_OVL_TRIG
    #define TRIG_FLD_SW_TRIG                          OVL_TRIG_FLD_OVL_SW_TRIG

#define DISP_REG_OVL_RST                          DISP_OVL0_OVL_RST
#define DISP_REG_OVL_ROI_SIZE                     DISP_OVL0_OVL_ROI_SIZE
    #define ROI_SIZE_FLD_ROI_W                        OVL_ROI_SIZE_FLD_ROI_W
    #define ROI_SIZE_FLD_ROI_H                        OVL_ROI_SIZE_FLD_ROI_H

#define DISP_REG_OVL_ROI_BGCLR                    DISP_OVL0_OVL_ROI_BGCLR
    #define ROI_BGCLR_FLD_BLUE                        OVL_ROI_BGCLR_FLD_BLUE
    #define ROI_BGCLR_FLD_GREEN                       OVL_ROI_BGCLR_FLD_GREEN
    #define ROI_BGCLR_FLD_RED                         OVL_ROI_BGCLR_FLD_RED
    #define ROI_BGCLR_FLD_ALPHA                       OVL_ROI_BGCLR_FLD_ALPHA

#define DISP_REG_OVL_SRC_CON                      DISP_OVL0_OVL_SRC_CON
    #define SRC_CON_FLD_L0_EN                         OVL_SRC_CON_FLD_L0_EN
    #define SRC_CON_FLD_L1_EN                         OVL_SRC_CON_FLD_L1_EN
    #define SRC_CON_FLD_L2_EN                         OVL_SRC_CON_FLD_L2_EN
    #define SRC_CON_FLD_L3_EN                         OVL_SRC_CON_FLD_L3_EN
    #define SRC_CON_FLD_LC_EN                         OVL_SRC_CON_FLD_LC_EN

#define DISP_REG_OVL_L0_CON                       DISP_OVL0_OVL_L0_CON
    #define L_CON_FLD_APHA                            OVL_L0_CON_FLD_ALPHA
    #define L_CON_FLD_AEN                             OVL_L0_CON_FLD_ALPHA_EN
    #define L_CON_FLD_VIRTICAL_FLIP                   OVL_L0_CON_FLD_VERTICAL_FLIP_EN
    #define L_CON_FLD_HORI_FLIP                       OVL_L0_CON_FLD_HORIZONTAL_FLIP_EN
    #define L_CON_FLD_EXT_MTX_EN                      OVL_L0_CON_FLD_EXT_MTX_EN
    #define L_CON_FLD_CFMT                            OVL_L0_CON_FLD_CLRFMT
    #define L_CON_FLD_MTX                             OVL_L0_CON_FLD_INT_MTX_SEL
    #define L_CON_FLD_BTSW                            OVL_L0_CON_FLD_BYTE_SWAP
    #define L_CON_FLD_CLRFMT_MAN                      OVL_L0_CON_FLD_CLRFMT_MAN
    #define L_CON_FLD_RGB_SWAP                        OVL_L0_CON_FLD_RGB_SWAP
    #define L_CON_FLD_LARC                            OVL_L0_CON_FLD_LAYER_SRC
    #define L_CON_FLD_SKEN                            OVL_L0_CON_FLD_SRCKEY_EN

#define DISP_REG_OVL_L0_SRCKEY                    DISP_OVL0_OVL_L0_SRCKEY
#define DISP_REG_OVL_L0_SRC_SIZE                  DISP_OVL0_OVL_L0_SRC_SIZE
#define DISP_REG_OVL_L0_OFFSET                    DISP_OVL0_OVL_L0_OFFSET
#define DISP_REG_OVL_L0_ADDR                      DISP_OVL0_OVL_L0_ADDR
#define DISP_REG_OVL_L0_PITCH                     DISP_OVL0_OVL_L0_PITCH
    #define L_PITCH_FLD_LSP                           OVL_L0_PITCH_FLD_L0_SRC_PITCH
    #define L_PITCH_FLD_CONST_BLD                     OVL_L0_PITCH_FLD_L0_CONST_BLD
    #define L_PITCH_FLD_SRGB_SEL                      OVL_L0_PITCH_FLD_L0_SRGB_SEL
    #define L_PITCH_FLD_DRGB_SEL                      OVL_L0_PITCH_FLD_L0_DRGB_SEL
    #define L_PITCH_FLD_SURFL_EN                      OVL_L0_PITCH_FLD_SURFL_EN

#define DISP_REG_OVL_L0_TILE                      DISP_OVL0_OVL_L0_TILE
#define DISP_REG_OVL_L0_CLIP                      DISP_OVL0_OVL_L0_CLIP
    #define OVL_L_CLIP_FLD_LEFT                       OVL_L0_CLIP_FLD_L0_SRC_LEFT_CLIP
    #define OVL_L_CLIP_FLD_RIGHT                      OVL_L0_CLIP_FLD_L0_SRC_RIGHT_CLIP
    #define OVL_L_CLIP_FLD_TOP                        OVL_L0_CLIP_FLD_L0_SRC_TOP_CLIP
    #define OVL_L_CLIP_FLD_BOTTOM                     OVL_L0_CLIP_FLD_L0_SRC_BOTTOM_CLIP

#define DISP_REG_OVL_L1_CON                       DISP_OVL0_OVL_L1_CON
#define DISP_REG_OVL_L1_SRCKEY                    DISP_OVL0_OVL_L1_SRCKEY
#define DISP_REG_OVL_L1_SRC_SIZE                  DISP_OVL0_OVL_L1_SRC_SIZE
#define DISP_REG_OVL_L1_OFFSET                    DISP_OVL0_OVL_L1_OFFSET
#define DISP_REG_OVL_L1_ADDR                      DISP_OVL0_OVL_L1_ADDR
#define DISP_REG_OVL_L1_PITCH                     DISP_OVL0_OVL_L1_PITCH
#define DISP_REG_OVL_L1_TILE                      DISP_OVL0_OVL_L1_TILE
#define DISP_REG_OVL_L1_CLIP                      DISP_OVL0_OVL_L1_CLIP

#define DISP_REG_OVL_L2_CON                       DISP_OVL0_OVL_L2_CON
#define DISP_REG_OVL_L2_SRCKEY                    DISP_OVL0_OVL_L2_SRCKEY
#define DISP_REG_OVL_L2_SRC_SIZE                  DISP_OVL0_OVL_L2_SRC_SIZE
#define DISP_REG_OVL_L2_OFFSET                    DISP_OVL0_OVL_L2_OFFSET
#define DISP_REG_OVL_L2_ADDR                      DISP_OVL0_OVL_L2_ADDR
#define DISP_REG_OVL_L2_PITCH                     DISP_OVL0_OVL_L2_PITCH
#define DISP_REG_OVL_L2_TILE                      DISP_OVL0_OVL_L2_TILE
#define DISP_REG_OVL_L2_CLIP                      DISP_OVL0_OVL_L2_CLIP

#define DISP_REG_OVL_L3_CON                       DISP_OVL0_OVL_L3_CON
#define DISP_REG_OVL_L3_SRCKEY                    DISP_OVL0_OVL_L3_SRCKEY
#define DISP_REG_OVL_L3_SRC_SIZE                  DISP_OVL0_OVL_L3_SRC_SIZE
#define DISP_REG_OVL_L3_OFFSET                    DISP_OVL0_OVL_L3_OFFSET
#define DISP_REG_OVL_L3_ADDR                      DISP_OVL0_OVL_L2_ADDR
#define DISP_REG_OVL_L3_PITCH                     DISP_OVL0_OVL_L3_PITCH
#define DISP_REG_OVL_L3_TILE                      DISP_OVL0_OVL_L3_TILE
#define DISP_REG_OVL_L3_CLIP                      DISP_OVL0_OVL_L3_CLIP

#define DISP_REG_OVL_RDMA0_CTRL                   DISP_OVL0_OVL_RDMA0_CTRL
    #define RDMA0_CTRL_FLD_RDMA_EN                    OVL_RDMA0_CTRL_FLD_RDMA0_EN
    #define RDMA0_CTRL_FLD_RMDA_FIFO_USED_SZ          OVL_RDMA0_CTRL_FLD_RDMA0_FIFO_USED_SIZE

#define DISP_REG_OVL_RDMA0_MEM_SLOW_CON           DISP_OVL0_OVL_RDMA0_MEM_SLOW_CON

#define DISP_REG_OVL_RDMA1_CTRL                   DISP_OVL0_OVL_RDMA1_CTRL
#define DISP_REG_OVL_RDMA1_MEM_GMC_SETTING        DISP_OVL0_OVL_RDMA1_MEM_GMC_SETTING1
#define DISP_REG_OVL_RDMA1_MEM_SLOW_CON           DISP_OVL0_OVL_RDMA1_MEM_SLOW_CON
#define DISP_REG_OVL_RDMA1_FIFO_CTRL              DISP_OVL0_OVL_RDMA1_FIFO_CTRL
#define DISP_REG_OVL_RDMA2_CTRL                   DISP_OVL0_OVL_RDMA2_CTRL
#define DISP_REG_OVL_RDMA2_MEM_GMC_SETTING        DISP_OVL0_OVL_RDMA2_MEM_GMC_SETTING1
#define DISP_REG_OVL_RDMA2_MEM_SLOW_CON           DISP_OVL0_OVL_RDMA2_MEM_SLOW_CON
#define DISP_REG_OVL_RDMA2_FIFO_CTRL              DISP_OVL0_OVL_RDMA2_FIFO_CTRL
#define DISP_REG_OVL_RDMA3_CTRL                   DISP_OVL0_OVL_RDMA3_CTRL
#define DISP_REG_OVL_RDMA3_MEM_GMC_SETTING        DISP_OVL0_OVL_RDMA3_MEM_GMC_SETTING1
#define DISP_REG_OVL_RDMA3_MEM_SLOW_CON           DISP_OVL0_OVL_RDMA3_MEM_SLOW_CON
#define DISP_REG_OVL_RDMA3_FIFO_CTRL              DISP_OVL0_OVL_RDMA3_FIFO_CTRL

#define DISP_REG_OVL_L0_Y2R_PARA_R0               DISP_OVL0_OVL_L0_Y2R_PARA_R0
#define DISP_REG_OVL_L0_Y2R_PARA_R1               DISP_OVL0_OVL_L0_Y2R_PARA_R1
#define DISP_REG_OVL_L0_Y2R_PARA_G0               DISP_OVL0_OVL_L0_Y2R_PARA_G0
#define DISP_REG_OVL_L0_Y2R_PARA_G1               DISP_OVL0_OVL_L0_Y2R_PARA_G1
#define DISP_REG_OVL_L0_Y2R_PARA_B0               DISP_OVL0_OVL_L0_Y2R_PARA_B0
#define DISP_REG_OVL_L0_Y2R_PARA_B1               DISP_OVL0_OVL_L0_Y2R_PARA_B1
#define DISP_REG_OVL_L0_Y2R_PARA_YUV_A_0          DISP_OVL0_OVL_L0_Y2R_PARA_YUV_A_0
#define DISP_REG_OVL_L0_Y2R_PARA_YUV_A_1          DISP_OVL0_OVL_L0_Y2R_PARA_YUV_A_1
#define DISP_REG_OVL_L0_Y2R_PARA_RGB_A_0          DISP_OVL0_OVL_L0_Y2R_PARA_RGB_A_0
#define DISP_REG_OVL_L0_Y2R_PARA_RGB_A_1          DISP_OVL0_OVL_L0_Y2R_PARA_RGB_A_1
#define DISP_REG_OVL_L1_Y2R_PARA_R0               DISP_OVL0_OVL_L1_Y2R_PARA_R0
#define DISP_REG_OVL_L1_Y2R_PARA_R1               DISP_OVL0_OVL_L1_Y2R_PARA_R1
#define DISP_REG_OVL_L1_Y2R_PARA_G0               DISP_OVL0_OVL_L1_Y2R_PARA_G0
#define DISP_REG_OVL_L1_Y2R_PARA_G1               DISP_OVL0_OVL_L1_Y2R_PARA_G1
#define DISP_REG_OVL_L1_Y2R_PARA_B0               DISP_OVL0_OVL_L1_Y2R_PARA_B0
#define DISP_REG_OVL_L1_Y2R_PARA_B1               DISP_OVL0_OVL_L1_Y2R_PARA_B1
#define DISP_REG_OVL_L1_Y2R_PARA_YUV_A_0          DISP_OVL0_OVL_L1_Y2R_PARA_YUV_A_0
#define DISP_REG_OVL_L1_Y2R_PARA_YUV_A_1          DISP_OVL0_OVL_L1_Y2R_PARA_YUV_A_1
#define DISP_REG_OVL_L1_Y2R_PARA_RGB_A_0          DISP_OVL0_OVL_L1_Y2R_PARA_RGB_A_0
#define DISP_REG_OVL_L1_Y2R_PARA_RGB_A_1          DISP_OVL0_OVL_L1_Y2R_PARA_RGB_A_1
#define DISP_REG_OVL_L2_Y2R_PARA_R0               DISP_OVL0_OVL_L2_Y2R_PARA_R0
#define DISP_REG_OVL_L2_Y2R_PARA_R1               DISP_OVL0_OVL_L2_Y2R_PARA_R1
#define DISP_REG_OVL_L2_Y2R_PARA_G0               DISP_OVL0_OVL_L2_Y2R_PARA_G0
#define DISP_REG_OVL_L2_Y2R_PARA_G1               DISP_OVL0_OVL_L2_Y2R_PARA_G1
#define DISP_REG_OVL_L2_Y2R_PARA_B0               DISP_OVL0_OVL_L2_Y2R_PARA_B0
#define DISP_REG_OVL_L2_Y2R_PARA_B1               DISP_OVL0_OVL_L2_Y2R_PARA_B1
#define DISP_REG_OVL_L2_Y2R_PARA_YUV_A_0          DISP_OVL0_OVL_L2_Y2R_PARA_YUV_A_0
#define DISP_REG_OVL_L2_Y2R_PARA_YUV_A_1          DISP_OVL0_OVL_L2_Y2R_PARA_YUV_A_1
#define DISP_REG_OVL_L2_Y2R_PARA_RGB_A_0          DISP_OVL0_OVL_L2_Y2R_PARA_RGB_A_0
#define DISP_REG_OVL_L2_Y2R_PARA_RGB_A_1          DISP_OVL0_OVL_L2_Y2R_PARA_RGB_A_1
#define DISP_REG_OVL_L3_Y2R_PARA_R0               DISP_OVL0_OVL_L3_Y2R_PARA_R0
#define DISP_REG_OVL_L3_Y2R_PARA_R1               DISP_OVL0_OVL_L3_Y2R_PARA_R1
#define DISP_REG_OVL_L3_Y2R_PARA_G0               DISP_OVL0_OVL_L3_Y2R_PARA_G0
#define DISP_REG_OVL_L3_Y2R_PARA_G1               DISP_OVL0_OVL_L3_Y2R_PARA_G1
#define DISP_REG_OVL_L3_Y2R_PARA_B0               DISP_OVL0_OVL_L3_Y2R_PARA_B0
#define DISP_REG_OVL_L3_Y2R_PARA_B1               DISP_OVL0_OVL_L3_Y2R_PARA_B1
#define DISP_REG_OVL_L3_Y2R_PARA_YUV_A_0          DISP_OVL0_OVL_L3_Y2R_PARA_YUV_A_0
#define DISP_REG_OVL_L3_Y2R_PARA_YUV_A_1          DISP_OVL0_OVL_L3_Y2R_PARA_YUV_A_1
#define DISP_REG_OVL_L3_Y2R_PARA_RGB_A_0          DISP_OVL0_OVL_L3_Y2R_PARA_RGB_A_0
#define DISP_REG_OVL_L3_Y2R_PARA_RGB_A_1          DISP_OVL0_OVL_L3_Y2R_PARA_RGB_A_1
#define DISP_REG_OVL_DEBUG_MON_SEL                DISP_OVL0_OVL_DEBUG_MON_SEL
#define DISP_REG_OVL_RDMA_BURST_CON0              DISP_OVL0_OVL_RDMA_BURST_CON0

#define DISP_REG_OVL_DUMMY_REG                    DISP_OVL0_OVL_DUMMY_REG
#define DISP_REG_OVL_GDRDY_PRD                    DISP_OVL0_OVL_GDRDY_PRD

#define DISP_REG_OVL_RDMA0_BUF_LOW                DISP_OVL0_OVL_RDMA0_BUF_LOW
#define DISP_REG_OVL_RDMA1_BUF_LOW                DISP_OVL0_OVL_RDMA1_BUF_LOW
#define DISP_REG_OVL_RDMA2_BUF_LOW                DISP_OVL0_OVL_RDMA2_BUF_LOW
#define DISP_REG_OVL_RDMA3_BUF_LOW                DISP_OVL0_OVL_RDMA3_BUF_LOW
#define DISP_REG_OVL_SMI_DBG                      DISP_OVL0_OVL_SMI_DBG
#define DISP_REG_OVL_GREQ_LAYER_CNT               DISP_OVL0_OVL_GREQ_LAYER_CNT
#define DISP_REG_OVL_GDRDY_PRD_NUM                DISP_OVL0_OVL_GDRDY_PRD_NUM
#define DISP_REG_OVL_FLOW_CTRL_DBG                DISP_OVL0_OVL_FLOW_CTRL_DBG
#define DISP_REG_OVL_ADDCON_DBG                   DISP_OVL0_OVL_ADDCON_DBG
    #define ADDCON_DBG_FLD_ROI_X                      OVL_ADDCON_DBG_FLD_ROI_X
    #define ADDCON_DBG_FLD_L0_WIN_HIT                 OVL_ADDCON_DBG_FLD_L0_WIN_HIT
    #define ADDCON_DBG_FLD_L1_WIN_HIT                 OVL_ADDCON_DBG_FLD_L1_WIN_HIT
    #define ADDCON_DBG_FLD_ROI_Y                      OVL_ADDCON_DBG_FLD_ROI_Y
    #define ADDCON_DBG_FLD_L3_WIN_HIT                 OVL_ADDCON_DBG_FLD_L3_WIN_HIT
    #define ADDCON_DBG_FLD_L2_WIN_HIT                 OVL_ADDCON_DBG_FLD_L2_WIN_HIT
#define DISP_REG_OVL_RDMA0_DBG                    DISP_OVL0_OVL_RDMA0_DBG
    #define RDMA0_DBG_FLD_RDMA0_WRAM_RST_CS           OVL_RDMA0_DBG_FLD_RDMA0_WRAM_RST_CS
    #define RDMA0_DBG_FLD_RDMA0_LAYER_GREQ            OVL_RDMA0_DBG_FLD_RDMA0_LAYER_GREQ
    #define RDMA0_DBG_FLD_RDMA0_OUT_DATA              OVL_RDMA0_DBG_FLD_RDMA0_OUT_DATA
    #define RDMA0_DBG_FLD_RDMA0_OUT_READY             OVL_RDMA0_DBG_FLD_RDMA0_OUT_READY
    #define RDMA0_DBG_FLD_RDMA0_OUT_VALID             OVL_RDMA0_DBG_FLD_RDMA0_OUT_VALID
    #define RDMA0_DBG_FLD_RDMA0_SMI_BUSY              OVL_RDMA0_DBG_FLD_RDMA0_SMI_BUSY
    #define RDMA0_DBG_FLD_RDMA0_SMI_GREQ              OVL_RDMA0_DBG_FLD_SMI_GREQ

#define DISP_REG_OVL_RDMA1_DBG                    DISP_OVL0_OVL_RDMA1_DBG
#define DISP_REG_OVL_RDMA2_DBG                    DISP_OVL0_OVL_RDMA2_DBG
#define DISP_REG_OVL_RDMA3_DBG                    DISP_OVL0_OVL_RDMA3_DBG
#define DISP_REG_OVL_L0_CLR                       DISP_OVL0_OVL_L0_CLR
#define DISP_REG_OVL_L1_CLR                       DISP_OVL0_OVL_L1_CLR
#define DISP_REG_OVL_L2_CLR                       DISP_OVL0_OVL_L2_CLR
#define DISP_REG_OVL_L3_CLR                       DISP_OVL0_OVL_L3_CLR
#define DISP_REG_OVL_LC_CLR                       DISP_OVL0_OVL_LC_CLR
#define DISP_REG_OVL_CRC                          DISP_OVL0_OVL_CRC
#define DISP_REG_OVL_LC_CON                       DISP_OVL0_OVL_LC_CON
#define DISP_REG_OVL_LC_SRCKEY                    DISP_OVL0_OVL_LC_SRCKEY
#define DISP_REG_OVL_LC_SRC_SIZE                  DISP_OVL0_OVL_LC_SRC_SIZE
#define DISP_REG_OVL_LC_OFFSET                    DISP_OVL0_OVL_LC_OFFSET
#define DISP_REG_OVL_LC_SRC_SEL                   DISP_OVL0_OVL_LC_SRC_SEL
#define DISP_REG_OVL_BANK_CON                     DISP_OVL0_OVL_BANK_CON
#define DISP_REG_OVL_FUNC_DCM0                    DISP_OVL0_OVL_FUNC_DCM0
#define DISP_REG_OVL_FUNC_DCM1                    DISP_OVL0_OVL_FUNC_DCM1
#define DISP_REG_OVL_DVFS_L0_ROI                  DISP_OVL0_OVL_DVFS_L0_ROI
#define DISP_REG_OVL_DVFS_L1_ROI                  DISP_OVL0_OVL_DVFS_L1_ROI
#define DISP_REG_OVL_DVFS_L2_ROI                  DISP_OVL0_OVL_DVFS_L2_ROI
#define DISP_REG_OVL_DVFS_L3_ROI                  DISP_OVL0_OVL_DVFS_L3_ROI
#define DISP_REG_OVL_DVFS_EL0_ROI                 DISP_OVL0_OVL_DVFS_EL0_ROI
#define DISP_REG_OVL_DVFS_EL1_ROI                 DISP_OVL0_OVL_DVFS_EL1_ROI
#define DISP_REG_OVL_DVFS_EL2_ROI                 DISP_OVL0_OVL_DVFS_EL2_ROI
#define DISP_REG_OVL_DATAPATH_EXT_CON             DISP_OVL0_OVL_DATAPATH_EXT_CON
#define DISP_REG_OVL_EL0_CON                      DISP_OVL0_OVL_EL0_CON
#define DISP_REG_OVL_EL0_SRCKEY                   DISP_OVL0_OVL_EL0_SRCKEY
#define DISP_REG_OVL_EL0_SRC_SIZE                 DISP_OVL0_OVL_EL0_SRC_SIZE
#define DISP_REG_OVL_EL0_OFFSET                   DISP_OVL0_OVL_EL0_OFFSET
#define DISP_REG_OVL_EL0_ADDR                     DISP_OVL0_OVL_EL0_ADDR
#define DISP_REG_OVL_EL0_PITCH                    DISP_OVL0_OVL_EL0_PITCH
#define DISP_REG_OVL_EL0_TILE                     DISP_OVL0_OVL_EL0_TILE
#define DISP_REG_OVL_EL0_CLIP                     DISP_OVL0_OVL_EL0_CLIP
#define DISP_REG_OVL_EL1_CON                      DISP_OVL0_OVL_EL1_CON
#define DISP_REG_OVL_EL1_SRCKEY                   DISP_OVL0_OVL_EL1_SRCKEY
#define DISP_REG_OVL_EL1_SRC_SIZE                 DISP_OVL0_OVL_EL1_SRC_SIZE
#define DISP_REG_OVL_EL1_OFFSET                   DISP_OVL0_OVL_EL1_OFFSET
#define DISP_REG_OVL_EL1_ADDR                     DISP_OVL0_OVL_EL1_ADDR
#define DISP_REG_OVL_EL1_PITCH                    DISP_OVL0_OVL_EL1_PITCH
#define DISP_REG_OVL_EL1_TILE                     DISP_OVL0_OVL_EL1_TILE
#define DISP_REG_OVL_EL1_CLIP                     DISP_OVL0_OVL_EL1_CLIP
#define DISP_REG_OVL_EL2_CON                      DISP_OVL0_OVL_EL2_CON
#define DISP_REG_OVL_EL2_SRCKEY                   DISP_OVL0_OVL_EL2_SRCKEY
#define DISP_REG_OVL_EL2_SRC_SIZE                 DISP_OVL0_OVL_EL2_SRC_SIZE
#define DISP_REG_OVL_EL2_OFFSET                   DISP_OVL0_OVL_EL2_OFFSET
#define DISP_REG_OVL_EL2_ADDR                     DISP_OVL0_OVL_EL2_ADDR
#define DISP_REG_OVL_EL2_PITCH                    DISP_OVL0_OVL_EL2_PITCH
#define DISP_REG_OVL_EL2_TILE                     DISP_OVL0_OVL_EL2_TILE
#define DISP_REG_OVL_EL2_CLIP                     DISP_OVL0_OVL_EL2_CLIP
#define DISP_REG_OVL_EL0_CLEAR                    DISP_OVL0_OVL_EL0_CLR
#define DISP_REG_OVL_EL1_CLEAR                    DISP_OVL0_OVL_EL1_CLR
#define DISP_REG_OVL_EL2_CLEAR                    DISP_OVL0_OVL_EL2_CLR
#define DISP_REG_OVL_SECURE                       DISP_OVL0_OVL_SECURE


/* for ovl golden setting */
#define DISP_REG_OVL_RDMA0_MEM_GMC_SETTING        ((DISPSYS_OVL0_BASE) + 0x0C8UL)
    #define FLD_OVL_RDMA_MEM_GMC_ULTRA_THRESHOLD         REG_FLD_MSB_LSB(9, 0)
    #define FLD_OVL_RDMA_MEM_GMC_PRE_ULTRA_THRESHOLD     REG_FLD_MSB_LSB(25, 16)
    #define FLD_OVL_RDMA_MEM_GMC_ULTRA_THRESHOLD_HIGH_OFS \
                            REG_FLD_MSB_LSB(28, 28)
    #define FLD_OVL_RDMA_MEM_GMC_PRE_ULTRA_THRESHOLD_HIGH_OFS \
                            REG_FLD_MSB_LSB(31, 31)

#define DISP_REG_OVL_RDMA0_FIFO_CTRL              ((DISPSYS_OVL0_BASE) + 0x0D0UL)
    #define FLD_OVL_RDMA_FIFO_THRD                       REG_FLD_MSB_LSB(9, 0)
    #define FLD_OVL_RDMA_FIFO_SIZE                       REG_FLD_MSB_LSB(27, 16)
    #define FLD_OVL_RDMA_FIFO_UND_EN                     REG_FLD_MSB_LSB(31, 31)

#define DISP_REG_OVL_RDMA0_MEM_GMC_S2             ((DISPSYS_OVL0_BASE) + 0x1E0UL)
    #define FLD_OVL_RDMA_MEM_GMC2_ISSUE_REQ_THRES        REG_FLD_MSB_LSB(11, 0)
    #define FLD_OVL_RDMA_MEM_GMC2_ISSUE_REQ_THRES_URG    REG_FLD_MSB_LSB(27, 16)
    #define FLD_OVL_RDMA_MEM_GMC2_REQ_THRES_PREULTRA     REG_FLD_MSB_LSB(28, 28)
    #define FLD_OVL_RDMA_MEM_GMC2_REQ_THRES_ULTRA        REG_FLD_MSB_LSB(29, 29)
    #define FLD_OVL_RDMA_MEM_GMC2_FORCE_REQ_THRES        REG_FLD_MSB_LSB(30, 30)

#define DISP_REG_OVL_RDMA_BURST_CON1              ((DISPSYS_OVL0_BASE) + 0x1F4UL)
    #define FLD_RDMA_BURST_CON1_BURST16_EN               REG_FLD_MSB_LSB(28, 28)

#define DISP_REG_OVL_RDMA_GREQ_NUM                ((DISPSYS_OVL0_BASE) + 0x1F8UL)
    #define FLD_OVL_RDMA_GREQ_LAYER0_GREQ_NUM            REG_FLD_MSB_LSB(3, 0)
    #define FLD_OVL_RDMA_GREQ_LAYER1_GREQ_NUM            REG_FLD_MSB_LSB(7, 4)
    #define FLD_OVL_RDMA_GREQ_LAYER2_GREQ_NUM            REG_FLD_MSB_LSB(11, 8)
    #define FLD_OVL_RDMA_GREQ_LAYER3_GREQ_NUM            REG_FLD_MSB_LSB(15, 12)
    #define FLD_OVL_RDMA_GREQ_OSTD_GREQ_NUM              REG_FLD_MSB_LSB(23, 16)
    #define FLD_OVL_RDMA_GREQ_GREQ_DIS_CNT               REG_FLD_MSB_LSB(26, 24)
    #define FLD_OVL_RDMA_GREQ_STOP_EN                    REG_FLD_MSB_LSB(27, 27)
    #define FLD_OVL_RDMA_GREQ_GRP_END_STOP               REG_FLD_MSB_LSB(28, 28)
    #define FLD_OVL_RDMA_GREQ_GRP_BRK_STOP               REG_FLD_MSB_LSB(29, 29)
    #define FLD_OVL_RDMA_GREQ_IOBUF_FLUSH_PREULTRA       REG_FLD_MSB_LSB(30, 30)
    #define FLD_OVL_RDMA_GREQ_IOBUF_FLUSH_ULTRA          REG_FLD_MSB_LSB(31, 31)

#define DISP_REG_OVL_RDMA_GREQ_URG_NUM            ((DISPSYS_OVL0_BASE) + 0x1FCUL)
    #define FLD_OVL_RDMA_GREQ_LAYER0_GREQ_URG_NUM        REG_FLD_MSB_LSB(3, 0)
    #define FLD_OVL_RDMA_GREQ_LAYER1_GREQ_URG_NUM        REG_FLD_MSB_LSB(7, 4)
    #define FLD_OVL_RDMA_GREQ_LAYER2_GREQ_URG_NUM        REG_FLD_MSB_LSB(11, 8)
    #define FLD_OVL_RDMA_GREQ_LAYER3_GREQ_URG_NUM        REG_FLD_MSB_LSB(15, 12)
    #define FLD_OVL_RDMA_GREQ_ARG_GREQ_URG_TH            REG_FLD_MSB_LSB(25, 16)
    #define FLD_OVL_RDMA_GREQ_ARG_URG_BIAS               REG_FLD_MSB_LSB(28, 28)
    #define FLD_OVL_RDMA_GREQ_NUM_SHT_VAL                REG_FLD_MSB_LSB(29, 29)

#define DISP_REG_OVL_RDMA_ULTRA_SRC               ((DISPSYS_OVL0_BASE) + 0x20CUL)
    #define FLD_OVL_RDMA_PREULTRA_BUF_SRC                REG_FLD_MSB_LSB(1, 0)
    #define FLD_OVL_RDMA_PREULTRA_SMI_SRC                REG_FLD_MSB_LSB(3, 2)
    #define FLD_OVL_RDMA_PREULTRA_ROI_END_SRC            REG_FLD_MSB_LSB(5, 4)
    #define FLD_OVL_RDMA_PREULTRA_RDMA_SRC               REG_FLD_MSB_LSB(7, 6)
    #define FLD_OVL_RDMA_ULTRA_BUF_SRC                   REG_FLD_MSB_LSB(9, 8)
    #define FLD_OVL_RDMA_ULTRA_SMI_SRC                   REG_FLD_MSB_LSB(11, 10)
    #define FLD_OVL_RDMA_ULTRA_ROI_END_SRC               REG_FLD_MSB_LSB(13, 12)
    #define FLD_OVL_RDMA_ULTRA_RDMA_SRC                  REG_FLD_MSB_LSB(15, 14)

#define DISP_REG_OVL_RDMAn_BUF_LOW(layer)         ((DISPSYS_OVL0_BASE) + 0x210UL + ((layer)<<2))
    #define FLD_OVL_RDMA_BUF_LOW_ULTRA_TH                REG_FLD_MSB_LSB(11, 0)
    #define FLD_OVL_RDMA_BUF_LOW_PREULTRA_TH             REG_FLD_MSB_LSB(23, 12)

#define DISP_REG_OVL_RDMAn_BUF_HIGH(layer)        ((DISPSYS_OVL0_BASE) + 0x220UL + ((layer)<<2))
    #define FLD_OVL_RDMA_BUF_HIGH_PREULTRA_TH            REG_FLD_MSB_LSB(23, 12)
    #define FLD_OVL_RDMA_BUF_HIGH_PREULTRA_DIS           REG_FLD_MSB_LSB(31, 31)

#define DISP_REG_OVL_EN                           ((DISPSYS_OVL0_BASE) + 0x00CUL)
    #define EN_FLD_OVL_EN                                REG_FLD_MSB_LSB(0, 0)
    #define EN_FLD_OP_8BIT_MODE                          REG_FLD_MSB_LSB(4, 4)
    #define EN_FLD_OP_10BIT_MODE                         REG_FLD_MSB_LSB(5, 5)
    #define EN_FLD_ARB_16F_NO_RND                        REG_FLD_MSB_LSB(7, 7)
    #define EN_FLD_HG_FOVL_CK_ON                         REG_FLD_MSB_LSB(8, 8)
    #define EN_FLD_HG_FSMI_CK_ON                         REG_FLD_MSB_LSB(9, 9)
    #define EN_FLD_HF_FOVL_CK_ON                         REG_FLD_MSB_LSB(10, 10)
    #define EN_FLD_IGNORE_ABN_SOF                        REG_FLD_MSB_LSB(16, 16)
    #define EN_FLD_BLOCK_EXT_ULTRA                       REG_FLD_MSB_LSB(18, 18)
    #define EN_FLD_BLOCK_EXT_PREULTRA                    REG_FLD_MSB_LSB(19, 19)
    #define EN_FLD_REG_BYPASS_SHADOW                     REG_FLD_MSB_LSB(22, 22)
    #define EN_FLD_NO_FORM_HDR_ULTRA                     REG_FLD_MSB_LSB(28, 28)
    #define EN_FLD_NO_FORM_HDR_PREULTRA                  REG_FLD_MSB_LSB(29, 29)


#define DISP_REG_OVL_DATAPATH_CON                 ((DISPSYS_OVL0_BASE) + 0x024UL)
    #define DATAPATH_CON_FLD_LAYER_SMI_ID_EN             REG_FLD_MSB_LSB(0, 0)
    #define DATAPATH_CON_FLD_RANDOM_BGCLR_EN             REG_FLD_MSB_LSB(1, 1)
    #define DATAPATH_CON_FLD_BGCLR_IN_SEL                REG_FLD_MSB_LSB(2, 2)
    #define DATAPATH_CON_FLD_OUTPUT_NO_RND               REG_FLD_MSB_LSB(3, 3)
    #define DATAPATH_CON_FLD_L0_FBDC_EN                  REG_FLD_MSB_LSB(4, 4)
    #define DATAPATH_CON_FLD_L1_FBDC_EN                  REG_FLD_MSB_LSB(5, 5)
    #define DATAPATH_CON_FLD_L2_FBDC_EN                  REG_FLD_MSB_LSB(6, 6)
    #define DATAPATH_CON_FLD_L3_FBDC_EN                  REG_FLD_MSB_LSB(7, 7)
    #define DATAPATH_CON_FLD_L0_GPU_MODE                 REG_FLD_MSB_LSB(8, 8)
    #define DATAPATH_CON_FLD_L1_GPU_MODE                 REG_FLD_MSB_LSB(9, 9)
    #define DATAPATH_CON_FLD_L2_GPU_MODE                 REG_FLD_MSB_LSB(10, 10)
    #define DATAPATH_CON_FLD_L3_GPU_MODE                 REG_FLD_MSB_LSB(11, 11)
    #define DATAPATH_CON_FLD_ADOBE_MODE                  REG_FLD_MSB_LSB(12, 12)
    #define DATAPATH_CON_FLD_ADOBE_LAYER                 REG_FLD_MSB_LSB(14, 13)
    #define DATAPATH_CON_FLD_OVL_GAMMA_OUT               REG_FLD_MSB_LSB(15, 15)
    #define DATAPATH_CON_FLD_PQ_OUT_SEL                  REG_FLD_MSB_LSB(18, 16)
    #define DATAPATH_CON_FLD_PQ_OUT_EN                   REG_FLD_MSB_LSB(19, 19)
    #define DATAPATH_CON_FLD_RDMA0_OUT_SEL               REG_FLD_MSB_LSB(20, 20)
    #define DATAPATH_CON_FLD_RDMA1_OUT_SEL               REG_FLD_MSB_LSB(21, 21)
    #define DATAPATH_CON_FLD_RDMA2_OUT_SEL               REG_FLD_MSB_LSB(22, 22)
    #define DATAPATH_CON_FLD_PQ_OUT_OPT                  REG_FLD_MSB_LSB(23, 23)
    #define DATAPATH_CON_FLD_GCLAST_EN                   REG_FLD_MSB_LSB(24, 24)
    #define DATAPATH_CON_FLD_HDR_GCLAST_EN               REG_FLD_MSB_LSB(25, 25)
    #define DATAPATH_CON_FLD_OUTPUT_CLAMP                REG_FLD_MSB_LSB(26, 26)
    #define DATAPATH_CON_FLD_OUTPUT_INTERLACE            REG_FLD_MSB_LSB(27, 27)

#define DISP_REG_OVL_FBDC_CFG1                    ((DISPSYS_OVL0_BASE) + 0x804UL)
    #define FLD_EL0_FBDC_PXLFMT                          REG_FLD_MSB_LSB(6, 0)
    #define FLD_EL0_FBDC_PXLFMT_EN                       REG_FLD_MSB_LSB(7, 7)
    #define FLD_EL1_FBDC_PXLFMT                          REG_FLD_MSB_LSB(14, 8)
    #define FLD_EL1_FBDC_PXLFMT_EN                       REG_FLD_MSB_LSB(15, 15)
    #define FLD_EL2_FBDC_PXLFMT                          REG_FLD_MSB_LSB(22, 16)
    #define FLD_EL2_FBDC_PXLFMT_EN                       REG_FLD_MSB_LSB(23, 23)
    #define FLD_FBDC_8XE_MODE                            REG_FLD_MSB_LSB(24, 24)
    #define FLD_FBDC_MODE_V3_1_EN                        REG_FLD_MSB_LSB(25, 25)
    #define FLD_FBDC_FILTER_EN                           REG_FLD_MSB_LSB(28, 28)
    #define FLD_FBDC_FILTER_CLEAR                        REG_FLD_MSB_LSB(31, 29)


enum GS_OVL_FLD {
    GS_OVL_RDMA_ULTRA_TH = 0,
    GS_OVL_RDMA_PRE_ULTRA_TH,
    GS_OVL_RDMA_FIFO_THRD,
    GS_OVL_RDMA_FIFO_SIZE,
    GS_OVL_RDMA_ISSUE_REQ_TH,
    GS_OVL_RDMA_ISSUE_REQ_TH_URG,
    GS_OVL_RDMA_REQ_TH_PRE_ULTRA,
    GS_OVL_RDMA_REQ_TH_ULTRA,
    GS_OVL_RDMA_FORCE_REQ_TH,
    GS_OVL_RDMA_GREQ_NUM, /* whole reg */
    GS_OVL_RDMA_GREQ_URG_NUM, /* whole reg */
    GS_OVL_RDMA_ULTRA_SRC, /* whole reg */
    GS_OVL_RDMA_ULTRA_LOW_TH,
    GS_OVL_RDMA_PRE_ULTRA_LOW_TH,
    GS_OVL_RDMA_PRE_ULTRA_HIGH_TH,
    GS_OVL_RDMA_PRE_ULTRA_HIGH_DIS,
    GS_OVL_BLOCK_EXT_ULTRA,
    GS_OVL_BLOCK_EXT_PRE_ULTRA,
    GS_OVL_FLD_NUM,
};
