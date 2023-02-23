/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#define DISP_REG_POSTMASK_EN                      ((DISPSYS_POSTMASK0_BASE) + 0x000UL)
    #define EN_FLD_POSTMASK_EN                           REG_FLD_MSB_LSB(0, 0)

#define DISP_REG_POSTMASK_RESET                   ((DISPSYS_POSTMASK0_BASE) + 0x004UL)
    #define RESET_FLD_POSTMASK_RESET                     REG_FLD_MSB_LSB(0, 0)

#define DISP_REG_POSTMASK_INTEN                   ((DISPSYS_POSTMASK0_BASE) + 0x008UL)
    #define PM_INTEN_FLD_PM_IF_FME_END_INTEN             REG_FLD_MSB_LSB(0, 0)
    #define PM_INTEN_FLD_PM_FME_CPL_INTEN                REG_FLD_MSB_LSB(1, 1)
    #define PM_INTEN_FLD_PM_START_INTEN                  REG_FLD_MSB_LSB(2, 2)
    #define PM_INTEN_FLD_PM_ABNORMAL_SOF_INTEN           REG_FLD_MSB_LSB(4, 4)
    #define PM_INTEN_FLD_RDMA_FME_UND_INTEN              REG_FLD_MSB_LSB(8, 8)
    #define PM_INTEN_FLD_RDMA_FME_SWRST_DONE_INTEN       REG_FLD_MSB_LSB(9, 9)
    #define PM_INTEN_FLD_RDMA_FME_HWRST_DONE_INTEN       REG_FLD_MSB_LSB(10, 10)
    #define PM_INTEN_FLD_RDMA_EOF_ABNORMAL_INTEN         REG_FLD_MSB_LSB(11, 11)
    #define PM_INTEN_FLD_RDMA_SMI_UNDERFLOW_INTEN        REG_FLD_MSB_LSB(12, 12)

#define DISP_REG_POSTMASK_INTSTA                  ((DISPSYS_POSTMASK0_BASE) + 0x00CUL)
    #define PM_INTSTA_FLD_POSTMASK_IF_FME_END_INT        REG_FLD_MSB_LSB(0, 0)
    #define PM_INTSTA_FLD_POSTMASK_FME_CPL_INT           REG_FLD_MSB_LSB(1, 1)
    #define PM_INTSTA_FLD_POSTMASK_START_INT             REG_FLD_MSB_LSB(2, 2)
    #define PM_INTSTA_FLD_POSTMASK_ABNORMAL_SOF_INT      REG_FLD_MSB_LSB(4, 4)
    #define PM_INTSTA_FLD_RDMA_FME_UND_INT               REG_FLD_MSB_LSB(8, 8)
    #define PM_INTSTA_FLD_RDMA_FME_SWRST_DONE_INT        REG_FLD_MSB_LSB(9, 9)
    #define PM_INTSTA_FLD_RDMA_FME_HWRST_DONE_INT        REG_FLD_MSB_LSB(10, 10)
    #define PM_INTSTA_FLD_RDMA_EOF_ABNORMAL_INT          REG_FLD_MSB_LSB(11, 11)
    #define PM_INTSTA_FLD_RDMA_SMI_UNDERFLOW_INT         REG_FLD_MSB_LSB(12, 12)

#define DISP_REG_POSTMASK_CFG                     ((DISPSYS_POSTMASK0_BASE) + 0x020UL)
    #define PM_CFG_FLD_STALL_CG_ON                       REG_FLD_MSB_LSB(8, 8)
    #define PM_CFG_FLD_GCLAST_EN                         REG_FLD_MSB_LSB(6, 6)
    #define PM_CFG_FLD_OUTPUT_CLAMP                      REG_FLD_MSB_LSB(5, 5)
    #define PM_CFG_FLD_OUTPUT_NO_RND                     REG_FLD_MSB_LSB(4, 4)
    #define PM_CFG_FLD_BGCLR_IN_SEL                      REG_FLD_MSB_LSB(2, 2)
    #define PM_CFG_FLD_DRAM_MODE                         REG_FLD_MSB_LSB(1, 1)
    #define PM_CFG_FLD_RELAY_MODE                        REG_FLD_MSB_LSB(0, 0)

#define DISP_REG_POSTMASK_SIZE                    ((DISPSYS_POSTMASK0_BASE) + 0x030UL)
    #define PM_SIZE_FLD_HSIZE                            REG_FLD_MSB_LSB(28, 16)
    #define PM_SIZE_FLD_VSIZE                            REG_FLD_MSB_LSB(12, 0)

#define DISP_REG_POSTMASK_SRAM_CFG                ((DISPSYS_POSTMASK0_BASE) + 0x040UL)
    #define PM_SRAM_CFG_FLD_MASK_NUM_SW_SET              REG_FLD_MSB_LSB(11, 4)
    #define PM_SRAM_CFG_FLD_MASK_L_TOP_EN                REG_FLD_MSB_LSB(3, 3)
    #define PM_SRAM_CFG_FLD_MASK_L_BOTTOM_EN             REG_FLD_MSB_LSB(2, 2)
    #define PM_SRAM_CFG_FLD_MASK_R_TOP_EN                REG_FLD_MSB_LSB(1, 1)
    #define PM_SRAM_CFG_FLD_MASK_R_BOTTOM_EN             REG_FLD_MSB_LSB(0, 0)

#define DISP_REG_POSTMASK_MASK_SHIFT              ((DISPSYS_POSTMASK0_BASE) + 0x044UL)
    #define PM_MASK_SHIFT_FLD_MASK_SHIFT_TOP_V           REG_FLD_MSB_LSB(31, 24)
    #define PM_MASK_SHIFT_FLD_MASK_SHIFT_BOTTOM_V        REG_FLD_MSB_LSB(23, 16)
    #define PM_MASK_SHIFT_FLD_MASK_SHIFT_L_H             REG_FLD_MSB_LSB(15, 8)
    #define PM_MASK_SHIFT_FLD_MASK_SHIFT_R_H             REG_FLD_MSB_LSB(7, 0)

#define DISP_REG_POSTMASK_GRAD_SHIFT              ((DISPSYS_POSTMASK0_BASE) + 0x048UL)
    #define PM_GRAD_SHIFT_FLD_GRAD_SHIFT_TOP             REG_FLD_MSB_LSB(19, 10)
    #define PM_GRAD_SHIFT_FLD_GRAD_SHIFT_BOTTOM          REG_FLD_MSB_LSB(9, 0)

#define DISP_REG_POSTMASK_BLEND_CFG               ((DISPSYS_POSTMASK0_BASE) + 0x050UL)
    #define PM_BLEND_CFG_FLD_CONST_A                     REG_FLD_MSB_LSB(23, 16)
    #define PM_BLEND_CFG_FLD_BLD_RND                     REG_FLD_MSB_LSB(4, 4)
    #define PM_BLEND_CFG_FLD_ALL_CONST_BLD               REG_FLD_MSB_LSB(3, 3)
    #define PM_BLEND_CFG_FLD_CONST_BLD                   REG_FLD_MSB_LSB(2, 2)
    #define PM_BLEND_CFG_FLD_PARGB_BLD                   REG_FLD_MSB_LSB(1, 1)
    #define PM_BLEND_CFG_FLD_A_EN                        REG_FLD_MSB_LSB(0, 0)

#define DISP_REG_POSTMASK_ROI_BGCLR               ((DISPSYS_POSTMASK0_BASE) + 0x054UL)
    #define PM_ROI_BGCLR_FLD_BGCLR_A                     REG_FLD_MSB_LSB(31, 24)
    #define PM_ROI_BGCLR_FLD_BGCLR_R                     REG_FLD_MSB_LSB(23, 16)
    #define PM_ROI_BGCLR_FLD_BGCLR_G                     REG_FLD_MSB_LSB(15, 8)
    #define PM_ROI_BGCLR_FLD_BGCLR_B                     REG_FLD_MSB_LSB(7, 0)

#define DISP_REG_POSTMASK_MASK_CLR                ((DISPSYS_POSTMASK0_BASE) + 0x058UL)
    #define PM_MASK_CLR_FLD_CNST_CLR_A                   REG_FLD_MSB_LSB(31, 24)
    #define PM_MASK_CLR_FLD_CNST_CLR_R                   REG_FLD_MSB_LSB(23, 16)
    #define PM_MASK_CLR_FLD_CNST_CLR_G                   REG_FLD_MSB_LSB(15, 8)
    #define PM_MASK_CLR_FLD_CNST_CLR_B                   REG_FLD_MSB_LSB(7, 0)

#define DISP_REG_POSTMASK_STATUS                  ((DISPSYS_POSTMASK0_BASE) + 0x0A0UL)
    #define PM_STATUS_FLD_POSTMASK_HANDSHAKE             REG_FLD_MSB_LSB(19, 6)
    #define PM_STATUS_FLD_RDMA2DPM_IDLE                  REG_FLD_MSB_LSB(5, 5)
    #define PM_STATUS_FLD_DPM_RUNNING                    REG_FLD_MSB_LSB(4, 4)
    #define PM_STATUS_FLD_OUT_RELAY_IDLE                 REG_FLD_MSB_LSB(1, 1)
    #define PM_STATUS_FLD_IN_RELAY_IDLE                  REG_FLD_MSB_LSB(0, 0)

#define DISP_REG_POSTMASK_INPUT_COUNT             ((DISPSYS_POSTMASK0_BASE) + 0x0A4UL)
    #define PM_INPUT_COUNT_FLD_INP_LINE_CNT              REG_FLD_MSB_LSB(28, 16)
    #define PM_INPUT_COUNT_FLD_INP_PIX_CNT               REG_FLD_MSB_LSB(12, 0)

#define DISP_REG_POSTMASK_DEBUG_0                 ((DISPSYS_POSTMASK0_BASE) + 0x0B0UL)
#define DISP_REG_POSTMASK_DEBUG_1                 ((DISPSYS_POSTMASK0_BASE) + 0x0B4UL)
#define DISP_REG_POSTMASK_DEBUG_2                 ((DISPSYS_POSTMASK0_BASE) + 0x0B8UL)
#define DISP_REG_POSTMASK_MEM_ADDR                ((DISPSYS_POSTMASK0_BASE) + 0x100UL)
#define DISP_REG_POSTMASK_MEM_LENGTH              ((DISPSYS_POSTMASK0_BASE) + 0x104UL)
#define DISP_REG_POSTMASK_RDMA_FIFO_CTRL          ((DISPSYS_POSTMASK0_BASE) + 0x108UL)

#define DISP_REG_POSTMASK_MEM_GMC_SETTING2        ((DISPSYS_POSTMASK0_BASE) + 0x10CUL)
    #define PM_MEM_GMC_FLD_FORCE_REQ_TH                  REG_FLD_MSB_LSB(30, 30)
    #define PM_MEM_GMC_FLD_REQ_TH_ULTRA                  REG_FLD_MSB_LSB(29, 29)
    #define PM_MEM_GMC_FLD_REQ_TH_PREULTRA               REG_FLD_MSB_LSB(28, 28)
    #define PM_MEM_GMC_FLD_ISSUE_REQ_TH_URG              REG_FLD_MSB_LSB(27, 16)
    #define PM_MEM_GMC_FLD_ISSUE_REQ_TH                  REG_FLD_MSB_LSB(11, 0)

#define DISP_REG_POSTMASK_PAUSE_REGION            ((DISPSYS_POSTMASK0_BASE) + 0x110UL)
    #define PM_PAUSE_REGION_FLD_RDMA_PAUSE_END           REG_FLD_MSB_LSB(27, 16)
    #define PM_PAUSE_REGION_FLD_RDMA_PAUSE_START         REG_FLD_MSB_LSB(11, 0)

#define DISP_REG_POSTMASK_RDMA_GREQ_NUM           ((DISPSYS_POSTMASK0_BASE) + 0x130UL)
    #define PM_GREQ_FLD_IOBUF_FLUSH_ULTRA                REG_FLD_MSB_LSB(31, 31)
    #define PM_GREQ_FLD_IOBUF_FLUSH_PREULTRA             REG_FLD_MSB_LSB(30, 30)
    #define PM_GREQ_FLD_GRP_BRK_STOP                     REG_FLD_MSB_LSB(29, 29)
    #define PM_GREQ_FLD_GRP_END_STOP                     REG_FLD_MSB_LSB(28, 28)
    #define PM_GREQ_FLD_GREQ_STOP_EN                     REG_FLD_MSB_LSB(27, 27)
    #define PM_GREQ_FLD_GREQ_DIS_CNT                     REG_FLD_MSB_LSB(26, 24)
    #define PM_GREQ_FLD_OSTD_GREQ_NUM                    REG_FLD_MSB_LSB(23, 16)
    #define PM_GREQ_FLD_GREQ_NUM_SHT                     REG_FLD_MSB_LSB(14, 13)
    #define PM_GREQ_FLD_GREQ_NUM_SHT_VAL                 REG_FLD_MSB_LSB(12, 12)
    #define PM_GREQ_FLD_GREQ_URG_NUM                     REG_FLD_MSB_LSB(7, 4)
    #define PM_GREQ_FLD_GREQ_NUM                         REG_FLD_MSB_LSB(3, 0)

#define DISP_REG_POSTMASK_RDMA_GREQ_URG_NUM       ((DISPSYS_POSTMASK0_BASE) + 0x134UL)
    #define PM_GREQ_URG_FLD_ARB_URG_BIAS                 REG_FLD_MSB_LSB(12, 12)
    #define PM_GREQ_URG_FLD_ARB_GREQ_URG_TH              REG_FLD_MSB_LSB(11, 0)

#define DISP_REG_POSTMASK_RDMA_ULTRA_SRC          ((DISPSYS_POSTMASK0_BASE) + 0x140UL)
    #define PM_ULTRA_FLD_ULTRA_RDMA_SRC                  REG_FLD_MSB_LSB(15, 14)
    #define PM_ULTRA_FLD_ULTRA_ROI_END_SRC               REG_FLD_MSB_LSB(13, 12)
    #define PM_ULTRA_FLD_ULTRA_SMI_SRC                   REG_FLD_MSB_LSB(11, 10)
    #define PM_ULTRA_FLD_ULTRA_BUF_SRC                   REG_FLD_MSB_LSB(9, 8)
    #define PM_ULTRA_FLD_PREULTRA_RDMA_SRC               REG_FLD_MSB_LSB(7, 6)
    #define PM_ULTRA_FLD_PREULTRA_ROI_END_SRC            REG_FLD_MSB_LSB(5, 4)
    #define PM_ULTRA_FLD_PREULTRA_SMI_SRC                REG_FLD_MSB_LSB(3, 2)
    #define PM_ULTRA_FLD_PREULTRA_BUF_SRC                REG_FLD_MSB_LSB(1, 0)

#define DISP_REG_POSTMASK_RDMA_BUF_LOW_TH         ((DISPSYS_POSTMASK0_BASE) + 0x144UL)
    #define PM_TH_FLD_RDMA_PREULTRA_LOW_TH               REG_FLD_MSB_LSB(23, 12)
    #define PM_TH_FLD_RDMA_ULTRA_LOW_TH                  REG_FLD_MSB_LSB(11, 0)

#define DISP_REG_POSTMASK_RDMA_BUF_HIGH_TH        ((DISPSYS_POSTMASK0_BASE) + 0x148UL)
    #define PM_TH_FLD_RDMA_PREULTRA_HIGH_DIS             REG_FLD_MSB_LSB(31, 31)
    #define PM_TH_FLD_RDMA_PREULTRA_HIGH_TH              REG_FLD_MSB_LSB(23, 12)

#define DISP_REG_POSTMASK_NUM_0                   ((DISPSYS_POSTMASK0_BASE) + 0x800UL)
    #define PM_NUM_FLD_MASK_NUM_1                        REG_FLD_MSB_LSB(31, 24)
    #define PM_NUM_FLD_GRAD_NUM_1                        REG_FLD_MSB_LSB(20, 16)
    #define PM_NUM_FLD_MASK_NUM_2                        REG_FLD_MSB_LSB(15, 8)
    #define PM_NUM_FLD_GRAD_NUM_2                        REG_FLD_MSB_LSB(4, 0)

#define DISP_REG_POSTMASK_NUM(n)                  (DISP_REG_POSTMASK_NUM_0 + (0x4 * (n)))

#define DISP_REG_POSTMASK_GRAD_VAL_0              ((DISPSYS_POSTMASK0_BASE) + 0xA00UL)
    #define PM_GRAD_VAL_FLD_GRAD_VAL_1                   REG_FLD_MSB_LSB(31, 24)
    #define PM_GRAD_VAL_FLD_GRAD_VAL_2                   REG_FLD_MSB_LSB(23, 16)
    #define PM_GRAD_VAL_FLD_GRAD_VAL_3                   REG_FLD_MSB_LSB(15, 8)
    #define PM_GRAD_VAL_FLD_GRAD_VAL_4                   REG_FLD_MSB_LSB(7, 0)

#define DISP_REG_POSTMASK_GRAD_VAL(n)             (DISP_REG_POSTMASK_GRAD_VAL_0 + (0x4 * (n)))
