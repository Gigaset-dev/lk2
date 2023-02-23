/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

//#include <platform/mt_typedefs.h>
#include <stdlib.h>
//#include <platform/sync_write.h>
#include <reg.h>

#include <platform/addressmap.h>

#define SMI_COMMON_BASE         (IO_BASE + 0xffffffff)
#define DISP_DSI1_BASE          (IO_BASE + 0xffffffff)
#define MIPI_TX1_CONFIG_BASE    (IO_BASE + 0xffffffff)


//TDODO: get base reg addr from system header
#define DISP_INDEX_OFFSET (DISP_OVL0_2L_BASE - DISP_OVL0_BASE) /* used by ovl */
#define DISP_RDMA_INDEX_OFFSET (DISP_RDMA1_BASE - DISP_RDMA0_BASE) /* used by rdma */
#define DISP_CCORR_INDEX_OFFSET (DISP_CCORR1_BASE - DISP_CCORR0_BASE)

#define SMI_LARB0_SEC_CON               (IO_BASE_PHY + 0x04021F80)
#define SMI_LARB1_SEC_CON               (IO_BASE_PHY + 0x04022F80)

/* SEC_DISPLAY */
#define DISP_REG_MMSYS_SECURITY_DISABLE2        (IO_BASE_PHY + 0x04000A08)

#define DISPSYS_OVL0_BASE               DISP_OVL0_BASE
#define DISPSYS_OVL0_2L_BASE            DISP_OVL0_2L_BASE
#define DISPSYS_RDMA0_BASE              DISP_RDMA0_BASE
#define DISPSYS_TDSHP0_BASE             DISP_TDSHP0_BASE
#define DISPSYS_C3D0_BASE               DISP_C3D0_BASE
#define DISPSYS_COLOR0_BASE             DISP_COLOR0_BASE
#define DISPSYS_AAL0_BASE               DISP_AAL0_BASE
#define DISPSYS_GAMMA0_BASE             DISP_GAMMA0_BASE
#define DISPSYS_DSI0_BASE               DISP_DSI0_BASE
#define DISPSYS_DSI1_BASE               DISP_DSI1_BASE
#define DISPSYS_MIPITX0_BASE            MIPI_TX0_CONFIG_BASE
#define DISPSYS_MIPITX1_BASE            MIPI_TX1_CONFIG_BASE
#define DISPSYS_DITHER0_BASE            DISP_DITHER0_BASE
#define DISPSYS_PWM0_BASE               DISP_PWM0_BASE
#define DISPSYS_MUTEX_BASE              DISP_MUTEX_BASE
#define DISPSYS_SMI_LARB0_BASE          SMI_LARB0_BASE
#define DISPSYS_SMI_LARB1_BASE          SMI_LARB1_BASE
#define DISPSYS_SMI_COMMON_BASE         SMI_COMMON_BASE
#define DISPSYS_SMI_SUBCOM0_BASE        SMI_SUBCOM0_BASE
#define DISPSYS_SMI_SUBCOM1_BASE        SMI_SUBCOM1_BASE
#define DISPSYS_CCORR0_BASE             DISP_CCORR0_BASE
#define DISPSYS_CCORR1_BASE             DISP_CCORR1_BASE
#define DISPSYS_CONFIG_BASE             MMSYS_CONFIG_BASE
#define DISPSYS_POSTMASK0_BASE          DISP_POSTMASK0_BASE
#define DISPSYS_CM0_BASE                DISP_CM0_BASE
#define DISPSYS_SPR0_BASE               DISP_SPR0_BASE
#define DISPSYS_DSC_BASE                DISP_DSC_BASE

#define DISPSYS_APMIXED_BASE            APMIXED_BASE



// ---------------------------------------------------------------------------
//  Type Casting
// ---------------------------------------------------------------------------
#define AS_INT32(x)     (*(INT32 *)(x))
#define AS_INT16(x)     (*(INT16 *)(x))
#define AS_INT8(x)      (*(INT8  *)(x))

#define AS_UINT32(x)    (*(u32 *)(x))
#define AS_UINT16(x)    (*(u16 *)(x))
#define AS_UINT8(x)     (*(u8 *)(x))

// ---------------------------------------------------------------------------
//  Register Field Access
// ---------------------------------------------------------------------------
#define REG_FLD(width, shift) \
    ((unsigned int)((((width) & 0xFF) << 16) | ((shift) & 0xFF)))

#define REG_FLD_MSB_LSB(msb, lsb) REG_FLD((msb) - (lsb) + 1, (lsb))

#define REG_FLD_WIDTH(field) \
    ((unsigned int)(((field) >> 16) & 0xFF))

#define REG_FLD_SHIFT(field) \
    ((unsigned int)((field) & 0xFF))

#define REG_FLD_MASK(field) \
    ((unsigned int)((1ULL << REG_FLD_WIDTH(field)) - 1) << REG_FLD_SHIFT(field))

#define REG_FLD_VAL(field, val) \
    (((val) << REG_FLD_SHIFT(field)) & REG_FLD_MASK(field))

#define REG_FLD_VAL_GET(field, regval) \
    (((regval) & REG_FLD_MASK(field)) >> REG_FLD_SHIFT(field))


#define DISP_REG_GET(reg32) (readl(reg32))
#define DISP_REG_GET_FIELD(field, reg32) ((readl(reg32) & REG_FLD_MASK(field))  \
                                           >> REG_FLD_SHIFT(field))

#define DISP_CPU_REG_SET(reg32, val) writel(val, reg32)

#define DISP_CPU_REG_SET_FIELD(field, reg32, val)  \
           writel((readl(reg32) &  \
                   ~REG_FLD_MASK(field))|REG_FLD_VAL((field), (val)), reg32)

#define DISP_REG_MASK(handle, reg32, val, mask)     \
            writel((unsigned int)(readl(reg32)&~(mask))|(val), reg32)

#define DISP_REG_SET(handle, reg32, val) \
            writel(val, reg32)


#define DISP_REG_SET_FIELD(handle, field, reg32, val)  \
             writel((readl(reg32) &  \
                     ~REG_FLD_MASK(field))|REG_FLD_VAL((field), (val)), reg32)

#define REG_FLD_GET DISP_REG_GET_FIELD
#define REG_FLD_SET DISP_CPU_REG_SET_FIELD

/* RDMA */
#define DISP_REG_RDMA_INT_ENABLE                (DISPSYS_RDMA0_BASE+0x000)
#define DISP_REG_RDMA_INT_STATUS                (DISPSYS_RDMA0_BASE+0x004)
#define DISP_REG_RDMA_GLOBAL_CON                (DISPSYS_RDMA0_BASE+0x010)
#define DISP_REG_RDMA_SIZE_CON_0                (DISPSYS_RDMA0_BASE+0x014)
#define DISP_REG_RDMA_SIZE_CON_1                (DISPSYS_RDMA0_BASE+0x018)
#define DISP_REG_RDMA_TARGET_LINE               (DISPSYS_RDMA0_BASE+0x01C)
#define DISP_REG_RDMA_MEM_CON                   (DISPSYS_RDMA0_BASE+0x024)
#define DISP_REG_RDMA_MEM_SRC_PITCH             (DISPSYS_RDMA0_BASE+0x02C)
#define DISP_REG_RDMA_MEM_GMC_SETTING_0         (DISPSYS_RDMA0_BASE+0x030)
#define DISP_REG_RDMA_MEM_GMC_SETTING_1                (DISPSYS_RDMA0_BASE+0x034)
    #define MEM_GMC_SETTING_1_FLD_ULTRA_THRESHOLD_LOW \
                 REG_FLD_MSB_LSB(13, 0)
    #define MEM_GMC_SETTING_1_FLD_ULTRA_THRESHOLD_HIGH \
                 REG_FLD_MSB_LSB(29, 16)
    #define MEM_GMC_SETTING_1_FLD_RG_VALID_THRESHOLD_BLOCK_ULTRA \
                 REG_FLD_MSB_LSB(30, 30)
    #define MEM_GMC_SETTING_1_FLD_RG_VDE_BLOCK_ULTRA \
                 REG_FLD_MSB_LSB(31, 31)
#define DISP_REG_RDMA_MEM_SLOW_CON                    (DISPSYS_RDMA0_BASE+0x038)
#define DISP_REG_RDMA_MEM_GMC_SETTING_2               (DISPSYS_RDMA0_BASE+0x03c)

#define DISP_REG_RDMA_FIFO_CON                        (DISPSYS_RDMA0_BASE+0x040)
    #define MEM_GMC_SETTING_0_FLD_RG_VDE_FORCE_PREULTRA \
                 REG_FLD_MSB_LSB(31, 31)

#define DISP_REG_RDMA_FIFO_LOG                  (DISPSYS_RDMA0_BASE+0x044)
#define DISP_REG_RDMA_C00                       (DISPSYS_RDMA0_BASE+0x054)
#define DISP_REG_RDMA_C01                       (DISPSYS_RDMA0_BASE+0x058)
#define DISP_REG_RDMA_C02                       (DISPSYS_RDMA0_BASE+0x05C)
#define DISP_REG_RDMA_C10                       (DISPSYS_RDMA0_BASE+0x060)
#define DISP_REG_RDMA_C11                       (DISPSYS_RDMA0_BASE+0x064)
#define DISP_REG_RDMA_C12                       (DISPSYS_RDMA0_BASE+0x068)
#define DISP_REG_RDMA_C20                       (DISPSYS_RDMA0_BASE+0x06C)
#define DISP_REG_RDMA_C21                       (DISPSYS_RDMA0_BASE+0x070)
#define DISP_REG_RDMA_C22                       (DISPSYS_RDMA0_BASE+0x074)
#define DISP_REG_RDMA_PRE_ADD_0                 (DISPSYS_RDMA0_BASE+0x078)
#define DISP_REG_RDMA_PRE_ADD_1                 (DISPSYS_RDMA0_BASE+0x07C)
#define DISP_REG_RDMA_PRE_ADD_2                 (DISPSYS_RDMA0_BASE+0x080)
#define DISP_REG_RDMA_POST_ADD_0                (DISPSYS_RDMA0_BASE+0x084)
#define DISP_REG_RDMA_POST_ADD_1                (DISPSYS_RDMA0_BASE+0x088)
#define DISP_REG_RDMA_POST_ADD_2                (DISPSYS_RDMA0_BASE+0x08C)
#define DISP_REG_RDMA_DUMMY                     (DISPSYS_RDMA0_BASE+0x090)
#define DISP_REG_RDMA_DEBUG_OUT_SEL             (DISPSYS_RDMA0_BASE+0x094)
#define DISP_REG_RDMA_MEM_START_ADDR            (DISPSYS_RDMA0_BASE+0xf00)
#define DISP_REG_RDMA_BG_CON_0                  (DISPSYS_RDMA0_BASE+0x0a0)
#define DISP_REG_RDMA_BG_CON_1                  (DISPSYS_RDMA0_BASE+0x0a4)
#define DISP_REG_RDMA_THRESHOLD_FOR_SODI        (DISPSYS_RDMA0_BASE+0x0a8)
    #define RDMA_THRESHOLD_FOR_SODI_FLD_LOW      REG_FLD_MSB_LSB(13, 0)
    #define RDMA_THRESHOLD_FOR_SODI_FLD_HIGH     REG_FLD_MSB_LSB(29, 16)

#define DISP_REG_RDMA_THRESHOLD_FOR_DVFS        (DISPSYS_RDMA0_BASE+0x0ac)
    #define RDMA_THRESHOLD_FOR_DVFS_FLD_LOW      REG_FLD_MSB_LSB(13, 0)
    #define RDMA_THRESHOLD_FOR_DVFS_FLD_HIGH     REG_FLD_MSB_LSB(29, 16)

#define DISP_REG_RDMA_SRAM_SEL                  (DISPSYS_RDMA0_BASE+0x0b0)
#define DISP_REG_RDMA_STALL_CG_CON              (DISPSYS_RDMA0_BASE+0x0b4)
#define DISP_REG_RDMA_SHADOW_UPDATE             (DISPSYS_RDMA0_BASE+0x0bc)
    #define RG_SHADOW_BYPASS_SHADOW              REG_FLD_MSB_LSB(1, 1)
    #define RG_SHADOW_READ_WORK_REG              REG_FLD_MSB_LSB(2, 2)

#define DISP_REG_RDMA_DRAM_CON                  (DISPSYS_RDMA0_BASE+0x0c0)
#define DISP_REG_RDMA_DVFS_SETTING_PRE          (DISPSYS_RDMA0_BASE+0x0d0)
#define DISP_REG_RDMA_DVFS_SETTING_ULTRA        (DISPSYS_RDMA0_BASE+0x0d4)

#define DISP_REG_RDMA_MEM_GMC_SETTING_3         (DISPSYS_RDMA0_BASE+0x0e8)

#define DISP_REG_RDMA_DRAM_CON                  (DISPSYS_RDMA0_BASE+0x0c0)
#define DISP_REG_RDMA_BG_CON_2                  (DISPSYS_RDMA0_BASE+0x0c4)
#define DISP_REG_RDMA_DVFS_SETTING_PRE          (DISPSYS_RDMA0_BASE+0x0d0)
    #define RG_DVFS_PRE_ULTRA_THRESHOLD_LOW      REG_FLD_MSB_LSB(13, 0)
    #define RG_DVFS_PRE_ULTRA_THRESHOLD_HIGH     REG_FLD_MSB_LSB(29, 16)
#define DISP_REG_RDMA_DVFS_SETTING_ULTRA        (DISPSYS_RDMA0_BASE+0x0d4)
    #define RG_DVFS_ULTRA_THRESHOLD_LOW          REG_FLD_MSB_LSB(13, 0)
    #define RG_DVFS_ULTRA_THRESHOLD_HIGH         REG_FLD_MSB_LSB(29, 16)
#define DISP_REG_RDMA_LEAVE_DRS_SETTING         (DISPSYS_RDMA0_BASE+0x0d8)
    #define RG_IS_DRS_STATUS_THRESHOLD_LOW       REG_FLD_MSB_LSB(13, 0)
    #define RG_IS_DRS_STATUS_THRESHOLD_HIGH      REG_FLD_MSB_LSB(29, 16)
#define DISP_REG_RDMA_ENTER_DRS_SETTING         (DISPSYS_RDMA0_BASE+0x0dc)
    #define RG_NOT_DRS_STATUS_THRESHOLD_LOW      REG_FLD_MSB_LSB(13, 0)
    #define RG_NOT_DRS_STATUS_THRESHOLD_HIGH     REG_FLD_MSB_LSB(29, 16)
#define DISP_REG_RDMA_MEM_GMC_SETTING_3         (DISPSYS_RDMA0_BASE+0x0e8)
    #define FLD_LOW_FOR_URGENT \
                 REG_FLD_MSB_LSB(13, 0)
    #define FLD_HIGH_FOR_URGENT \
                 REG_FLD_MSB_LSB(29, 16)
    #define FLD_RG_VALID_THRESHOLD_BLOCK_URGENT \
                 REG_FLD_MSB_LSB(30, 30)
    #define FLD_RG_VDE_BLOCK_URGENT \
                 REG_FLD_MSB_LSB(31, 31)

#define DISP_REG_RDMA_MEM_GMC_SETTING_4         (DISPSYS_RDMA0_BASE+0x0ec)
    #define FLD_RDMA_DSI_BUFF_ULTRA_SEL         REG_FLD_MSB_LSB(0, 0)

#define DISP_REG_RDMA_IN_P_CNT                  (DISPSYS_RDMA0_BASE+0x120)
#define DISP_REG_RDMA_IN_LINE_CNT               (DISPSYS_RDMA0_BASE+0x124)
#define DISP_REG_RDMA_OUT_P_CNT                 (DISPSYS_RDMA0_BASE+0x128)
#define DISP_REG_RDMA_OUT_LINE_CNT              (DISPSYS_RDMA0_BASE+0x12c)

#define DISP_REG_RDMA_DBG_OUT                   (DISPSYS_RDMA0_BASE+0x100)
#define DISP_REG_RDMA_ULTRA_SRC_SEL             (DISPSYS_RDMA0_BASE+0x1a0)
    #define FLD_RG_PREULTRA_RDMA_SRC            REG_FLD_MSB_LSB(7, 6)
    #define FLD_RG_ULTRA_RDMA_SRC               REG_FLD_MSB_LSB(15, 14)

#define DISP_REG_RDMA_GREQ_NUM_SEL              (DISPSYS_RDMA0_BASE+0x1a4)
#define DISP_REG_RDMA_GREQ_URG_NUM_SEL          (DISPSYS_RDMA0_BASE+0x1a8)
    #define FLD_RG_LAYER_SMI_ID_EN              REG_FLD_MSB_LSB(29, 29)

#define INT_ENABLE_FLD_TARGET_LINE_INT_EN                            REG_FLD(1, 5)
#define INT_ENABLE_FLD_FIFO_UNDERFLOW_INT_EN                         REG_FLD(1, 4)
#define INT_ENABLE_FLD_EOF_ABNORMAL_INT_EN                           REG_FLD(1, 3)
#define INT_ENABLE_FLD_FRAME_END_INT_EN                              REG_FLD(1, 2)
#define INT_ENABLE_FLD_FRAME_START_INT_EN                            REG_FLD(1, 1)
#define INT_ENABLE_FLD_REG_UPDATE_INT_EN                             REG_FLD(1, 0)
#define INT_STATUS_FLD_FIFO_EMPTY_INT_FLAG                           REG_FLD(1, 6)
#define INT_STATUS_FLD_TARGET_LINE_INT_FLAG                          REG_FLD(1, 5)
#define INT_STATUS_FLD_FIFO_UNDERFLOW_INT_FLAG                       REG_FLD(1, 4)
#define INT_STATUS_FLD_EOF_ABNORMAL_INT_FLAG                         REG_FLD(1, 3)
#define INT_STATUS_FLD_FRAME_END_INT_FLAG                            REG_FLD(1, 2)
#define INT_STATUS_FLD_FRAME_START_INT_FLAG                          REG_FLD(1, 1)
#define INT_STATUS_FLD_REG_UPDATE_INT_FLAG                           REG_FLD(1, 0)
#define GLOBAL_CON_FLD_SMI_BUSY                                      REG_FLD(1, 12)
#define GLOBAL_CON_FLD_RESET_STATE                                   REG_FLD(3, 8)
#define GLOBAL_CON_FLD_SOFT_RESET                                    REG_FLD(1, 4)
#define GLOBAL_CON_FLD_RG_PIXEL_10_BIT                               REG_FLD(1, 3)
#define GLOBAL_CON_FLD_MODE_SEL                                      REG_FLD(1, 1)
#define GLOBAL_CON_FLD_ENGINE_EN                                     REG_FLD(1, 0)
#define SIZE_CON_0_FLD_MATRIX_INT_MTX_SEL                            REG_FLD(4, 20)
#define SIZE_CON_0_FLD_MATRIX_WIDE_GAMUT_EN                          REG_FLD(1, 18)
#define SIZE_CON_0_FLD_MATRIX_ENABLE                                 REG_FLD(1, 17)
#define SIZE_CON_0_FLD_MATRIX_EXT_MTX_EN                             REG_FLD(1, 16)
#define SIZE_CON_0_FLD_OUTPUT_FRAME_WIDTH                            REG_FLD(13, 0)
#define SIZE_CON_1_FLD_OUTPUT_FRAME_HEIGHT                           REG_FLD(20, 0)
#define TARGET_LINE_FLD_TARGET_LINE                                  REG_FLD(20, 0)
#define MEM_CON_FLD_MEM_MODE_HORI_BLOCK_NUM                          REG_FLD(8, 24)
#define MEM_CON_FLD_MEM_MODE_INPUT_COSITE                            REG_FLD(1, 13)
#define MEM_CON_FLD_MEM_MODE_INPUT_UPSAMPLE                          REG_FLD(1, 12)
#define MEM_CON_FLD_MEM_MODE_INPUT_SWAP                              REG_FLD(1, 8)
#define MEM_CON_FLD_MEM_MODE_INPUT_FORMAT                            REG_FLD(4, 4)
#define MEM_CON_FLD_MEM_MODE_TILE_INTERLACE                          REG_FLD(1, 1)
#define MEM_CON_FLD_MEM_MODE_TILE_EN                                 REG_FLD(1, 0)
#define MEM_SRC_PITCH_FLD_MEM_MODE_SRC_PITCH                         REG_FLD(16, 0)
#define MEM_GMC_SETTING_0_FLD_PRE_ULTRA_THRESHOLD_LOW                REG_FLD(13, 0)
#define MEM_GMC_SETTING_0_FLD_PRE_ULTRA_THRESHOLD_HIGH               REG_FLD(12, 16)
#define MEM_GMC_SETTING_0_FLD_RG_VALID_THRESHOLD_FORCE_PREULTRA      REG_FLD(1, 30)
#define MEM_GMC_SETTING_0_FLD_SETTING_0__RG_VDE_FORCE_PREULTRA       REG_FLD(1, 31)
#define MEM_SLOW_CON_FLD_MEM_MODE_SLOW_COUNT                         REG_FLD(16, 16)
#define MEM_SLOW_CON_FLD_MEM_MODE_SLOW_EN                            REG_FLD(1, 0)
#define MEM_GMC_SETTING_2_FLD_ISSUE_REQ_THRESHOLD                    REG_FLD(13, 0)
#define FIFO_CON_FLD_FIFO_UNDERFLOW_EN                               REG_FLD(1, 31)
#define FIFO_CON_FLD_FIFO_PSEUDO_SIZE                                REG_FLD(12, 16)
#define FIFO_CON_FLD_OUTPUT_VALID_THRESHOLD_PER_LINE                 REG_FLD(1, 15)
#define FIFO_CON_FLD_OUTPUT_VALID_FIFO_THRESHOLD                     REG_FLD(12, 0)
#define FIFO_LOG_FLD_RDMA_FIFO_LOG                                   REG_FLD(12, 0)
#define C00_FLD_DISP_RDMA_C00                                        REG_FLD(13, 0)
#define C01_FLD_DISP_RDMA_C01                                        REG_FLD(13, 0)
#define C02_FLD_DISP_RDMA_C02                                        REG_FLD(13, 0)
#define C10_FLD_DISP_RDMA_C10                                        REG_FLD(13, 0)
#define C11_FLD_DISP_RDMA_C11                                        REG_FLD(13, 0)
#define C12_FLD_DISP_RDMA_C12                                        REG_FLD(13, 0)
#define C20_FLD_DISP_RDMA_C20                                        REG_FLD(13, 0)
#define C21_FLD_DISP_RDMA_C21                                        REG_FLD(13, 0)
#define C22_FLD_DISP_RDMA_C22                                        REG_FLD(13, 0)
#define PRE_ADD_0_FLD_DISP_RDMA_PRE_ADD_0                            REG_FLD(9, 0)
#define PRE_ADD_1_FLD_DISP_RDMA_PRE_ADD_1                            REG_FLD(9, 0)
#define PRE_ADD_2_FLD_DISP_RDMA_PRE_ADD_2                            REG_FLD(9, 0)
#define POST_ADD_0_FLD_DISP_RDMA_POST_ADD_0                          REG_FLD(9, 0)
#define POST_ADD_1_FLD_DISP_RDMA_POST_ADD_1                          REG_FLD(9, 0)
#define POST_ADD_2_FLD_DISP_RDMA_POST_ADD_2                          REG_FLD(9, 0)
#define DUMMY_FLD_DISP_RDMA_DUMMY                                    REG_FLD(32, 0)
#define DEBUG_OUT_SEL_FLD_DISP_RDMA_DEBUG_OUT_SEL                    REG_FLD(4, 0)
#define MEM_START_ADDR_FLD_MEM_MODE_START_ADDR                       REG_FLD(32, 0)
#define RDMA_BG_CON_0_LEFT                                           REG_FLD(13, 0)
#define RDMA_BG_CON_0_RIGHT                                          REG_FLD(13, 16)
#define RDMA_BG_CON_1_TOP                                            REG_FLD(13, 0)
#define RDMA_BG_CON_1_BOTTOM                                         REG_FLD(13, 16)
#define MEM_GMC_SETTING_3_FLD_URGENT_THRESHOLD_LOW                   REG_FLD(12, 0)
#define MEM_GMC_SETTING_3_FLD_URGENT_THRESHOLD_HIGH                  REG_FLD(12, 16)

/* ------------------------------------------------------------- */

/* golden setting */
enum GS_RDMA_FLD {
    GS_RDMA_PRE_ULTRA_TH_LOW = 0,
    GS_RDMA_PRE_ULTRA_TH_HIGH,
    GS_RDMA_VALID_TH_FORCE_PRE_ULTRA,
    GS_RDMA_VDE_FORCE_PRE_ULTRA,
    GS_RDMA_ULTRA_TH_LOW,
    GS_RDMA_ULTRA_TH_HIGH,
    GS_RDMA_VALID_TH_BLOCK_ULTRA,
    GS_RDMA_VDE_BLOCK_ULTRA,
    GS_RDMA_ISSUE_REQ_TH,
    GS_RDMA_OUTPUT_VALID_FIFO_TH,
    GS_RDMA_FIFO_SIZE,
    GS_RDMA_FIFO_UNDERFLOW_EN,
    GS_RDMA_TH_LOW_FOR_SODI,
    GS_RDMA_TH_HIGH_FOR_SODI,
    GS_RDMA_TH_LOW_FOR_DVFS,
    GS_RDMA_TH_HIGH_FOR_DVFS,
    GS_RDMA_SRAM_SEL,
    GS_RDMA_DVFS_PRE_ULTRA_TH_LOW,
    GS_RDMA_DVFS_PRE_ULTRA_TH_HIGH,
    GS_RDMA_DVFS_ULTRA_TH_LOW,
    GS_RDMA_DVFS_ULTRA_TH_HIGH,
    GS_RDMA_IS_DRS_STATUS_TH_LOW,
    GS_RDMA_IS_DRS_STATUS_TH_HIGH,
    GS_RDMA_NOT_DRS_STATUS_TH_LOW,
    GS_RDMA_NOT_DRS_STATUS_TH_HIGH,
    GS_RDMA_URGENT_TH_LOW,
    GS_RDMA_URGENT_TH_HIGH,
    GS_RDMA_LAYER_SMI_ID_EN,
    GS_RDMA_FLD_NUM,
};

#include "ddp_reg_mmsys.h"
#include "ddp_reg_dsi.h"
#include "ddp_reg_mutex.h"
#include "ddp_reg_ovl.h"
#include "ddp_reg_mipi.h"
#include "ddp_reg_postmask.h"
#include "ddp_reg_pq.h"
