/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

/* field definition */
/* ------------------------------------------------------------- */
/* Config */
#define DISP_REG_CONFIG_MMSYS_INTEN             (DISPSYS_CONFIG_BASE + 0x0)

#define DISP_REG_CONFIG_MMSYS_MISC        (DISPSYS_CONFIG_BASE + 0x0F0)
    #define FLD_OVL0_ULTRA_SEL        REG_FLD_MSB_LSB(3, 2)
    #define FLD_OVL0_2L_ULTRA_SEL        REG_FLD_MSB_LSB(7, 6)
    #define FLD_OVL2_2L_ULTRA_SEL        REG_FLD_MSB_LSB(19, 18)

#define DISP_REG_CONFIG_MMSYS_CG_CON0            (DISPSYS_CONFIG_BASE + 0x100)
#define DISP_REG_CONFIG_MMSYS_CG_SET0            (DISPSYS_CONFIG_BASE + 0x104)
#define DISP_REG_CONFIG_MMSYS_CG_CLR0            (DISPSYS_CONFIG_BASE + 0x108)
    #define FLD_CG0_DISP_DPI0                        REG_FLD(1, 30)
    #define FLD_CG0_DISP_RDMA4                        REG_FLD(1, 29)
    #define FLD_CG0_OVL2_2L                         REG_FLD(1, 28)
    #define FLD_CG0_SMI_GALS                        REG_FLD(1, 27)
    #define FLD_CG0_DISP_Y2R0                        REG_FLD(1, 26)
    #define FLD_CG0_DISP_MDP_COLOR4                    REG_FLD(1, 25)
    #define FLD_CG0_DISP_MDP_RDMA4                    REG_FLD(1, 24)
    #define FLD_CG0_DISP_MDP_HDR4                    REG_FLD(1, 23)
    #define FLD_CG0_DISP_MDP_AAL4                    REG_FLD(1, 22)
    #define FLD_CG0_DISP_MDP_RSZ4                    REG_FLD(1, 21)
    #define FLD_CG0_DISP_MDP_TDSHP4                    REG_FLD(1, 20)
    #define FLD_CG0_DISP_FAKE_ENG1                    REG_FLD(1, 19)
    #define FLD_CG0_DISP_FAKE_ENG0                    REG_FLD(1, 18)
    #define FLD_CG0_SMI_COMMON                         REG_FLD(1, 17)
    #define FLD_CG0_COLOR0                            REG_FLD(1, 16)
    #define FLD_CG0_DSI0_MM                         REG_FLD(1, 15)
    #define FLD_CG0_DISP_DSC                    REG_FLD(1, 14)
    #define FLD_CG0_POSTMASK0                        REG_FLD(1, 13)
    #define FLD_CG0_DISP_GAMMA0                        REG_FLD(1, 12)
    #define FLD_CG0_SMI_INFRA                        REG_FLD(1, 11)
    #define FLD_CG0_DITHER0                            REG_FLD(1, 10)
    #define FLD_CG0_CCORR0                            REG_FLD(1, 9)
    #define FLD_CG0_DISP_AAL0                        REG_FLD(1, 8)
    #define FLD_CG0_DISP_RSZ0                        REG_FLD(1, 7)
    #define FLD_CG0_DISP_UFBC_WDMA0                    REG_FLD(1, 6)
    #define FLD_CG0_DISP_WDMA0                        REG_FLD(1, 5)
    #define FLD_CG0_OVL0_2L                            REG_FLD(1, 4)
    #define FLD_CG0_DISP_RDMA0                        REG_FLD(1, 3)
    #define FLD_CG0_OVL0                            REG_FLD(1, 2)
    #define FLD_CG0_DISPSYS_CONFIG                    REG_FLD(1, 1)
    #define FLD_CG0_DISP_MUTEX0                     REG_FLD(1, 0)

#define DISP_REG_CONFIG_MMSYS_CG_CON1            (DISPSYS_CONFIG_BASE + 0x110)
#define DISP_REG_CONFIG_MMSYS_CG_SET1            (DISPSYS_CONFIG_BASE + 0x114)
#define DISP_REG_CONFIG_MMSYS_CG_CLR1            (DISPSYS_CONFIG_BASE + 0x118)
    #define FLD_CG1_SMI_IOMMU                        REG_FLD(1, 0)


#define DISP_REG_CONFIG_MMSYS_CG_CON2            (DISPSYS_CONFIG_BASE + 0x1A0)
#define DISP_REG_CONFIG_MMSYS_CG_SET2            (DISPSYS_CONFIG_BASE + 0x1A4)
#define DISP_REG_CONFIG_MMSYS_CG_CLR2            (DISPSYS_CONFIG_BASE + 0x1A8)
    #define FLD_CG2_32MHZ                            REG_FLD(1, 25)
    #define FLD_CG2_26MHZ                            REG_FLD(1, 24)
    #define FLD_CG2_DPI                                REG_FLD(1, 8)
    #define FLD_CG2_DSI0                             REG_FLD(1, 0)

#define DISP_REG_CONFIG_MMSYS_LCM_RST_B         (DISPSYS_CONFIG_BASE + 0x180)

#define DISP_REG_CONFIG_SMI_LARB0_GREQ            (DISPSYS_CONFIG_BASE + 0x8DC)
#define DISP_REG_CONFIG_SMI_LARB1_GREQ        (DISPSYS_CONFIG_BASE + 0x8E0)

#define DISP_REG_CONFIG_DISP_RDMA1_RSZ1_SEL_IN         (DISPSYS_CONFIG_BASE + 0xF0C)
#define DISP_REG_CONFIG_DISP_RSZ1_MOUT_EN         (DISPSYS_CONFIG_BASE + 0xF90)
#define DISP_REG_CONFIG_DISP_RDMA1_RSZ1_SOUT_SEL         (DISPSYS_CONFIG_BASE + 0xFDC)
#define DISP_REG_CONFIG_DISP_COLOR1_OUT_SEL_IN         (DISPSYS_CONFIG_BASE + 0x65C)
#define DISP_REG_CONFIG_DISP_TOVL1_PQOUT_MDP_RDMA5_SEL_IN         (DISPSYS_CONFIG_BASE + 0xFFC)
#define DISP_REG_CONFIG_MDP_AAL5_SOUT_SEL         (DISPSYS_CONFIG_BASE + 0xFD0)
#define DISP_REG_CONFIG_DISP_CCORR1_SOUT_SEL         (DISPSYS_CONFIG_BASE + 0xFD4)
#define DISP_REG_CONFIG_MDP_AAL1_SEL_IN         (DISPSYS_CONFIG_BASE + 0xFD8)
#define DISP_REG_CONFIG_DISP_MDP_AAL5_MDP_HDR5_SEL_IN         (DISPSYS_CONFIG_BASE + 0xFE0)
#define DISP_REG_CONFIG_DISP_DITHER1_MOUT_EN         (DISPSYS_CONFIG_BASE + 0xFE4)
#define DISP_REG_CONFIG_DISP_PQ1_SOUT_SEL         (DISPSYS_CONFIG_BASE + 0xFE8)
#define DISP_REG_CONFIG_DSI1_SEL_IN         (DISPSYS_CONFIG_BASE + 0xFF0)
#define DISP_REG_CONFIG_DISP_WDMA1_SEL_IN         (DISPSYS_CONFIG_BASE + 0xFF4)
#define DISP_REG_CONFIG_DISP_PQ0_SOUT_SEL         (DISPSYS_CONFIG_BASE + 0xFF8)
#define DISP_REG_CONFIG_DISP_DSC_WRAP_SOUT_SEL         (DISPSYS_CONFIG_BASE + 0xF48)
#define DISP_REG_CONFIG_DISP_DSI0_DSC_WRAP_SOUT_SEL         (DISPSYS_CONFIG_BASE + 0xF80)
#define DISP_REG_CONFIG_DISP_DP_WRAP_SEL_IN         (DISPSYS_CONFIG_BASE + 0xF84)
#define DISP_REG_CONFIG_DISP_RDMA4_PQ0_MERGE0_SEL_IN         (DISPSYS_CONFIG_BASE + 0xF88)
#define DISP_REG_CONFIG_DISP_RDMA5_PQ1_SEL_IN         (DISPSYS_CONFIG_BASE + 0xF8C)
#define DISP_REG_CONFIG_DISP_RDMA4_SOUT_SEL         (DISPSYS_CONFIG_BASE + 0xF24)
#define DISP_REG_CONFIG_DISP_RDMA5_SOUT_SEL         (DISPSYS_CONFIG_BASE + 0xF28)

#define DISP_REG_CONFIG_MMSYS_MOUT_RST         (DISPSYS_CONFIG_BASE + 0xF00)
#define DISP_REG_CONFIG_AFBC_WDMA0_SEL_IN         (DISPSYS_CONFIG_BASE + 0xF04)
#define DISP_REG_CONFIG_AFBC_WDMA1_SEL_IN         (DISPSYS_CONFIG_BASE + 0xF08)
#define DISP_REG_CONFIG_DISP_RDMA2_RSZ0_RSZ1_SEL_IN         (DISPSYS_CONFIG_BASE + 0xF18)
#define DISP_REG_CONFIG_DISP_OVL0_2L_BLENDOUT_SOUT_SEL         (DISPSYS_CONFIG_BASE + 0xF2C)
#define DISP_REG_CONFIG_DISP_TOVL0_OUT0_SEL_IN         (DISPSYS_CONFIG_BASE + 0xF30)
#define DISP_REG_CONFIG_DISP_RDMA0_SOUT_SEL         (DISPSYS_CONFIG_BASE + 0xF38)
#define DISP_REG_CONFIG_DISP_RDMA0_RSZ0_SEL_IN         (DISPSYS_CONFIG_BASE + 0xF3C)
#define DISP_REG_CONFIG_DISP_COLOR0_OUT_SEL_IN         (DISPSYS_CONFIG_BASE + 0x658)
#define DISP_REG_CONFIG_DISP_OVL0_OVL1_OVL1_2L_BGOUT_SEL_IN         (DISPSYS_CONFIG_BASE + 0xF5C)
#define DISP_REG_CONFIG_DISP_OVL1_OVL1_2L_BGOUT_SOUT_SEL         (DISPSYS_CONFIG_BASE + 0xF60)
#define DISP_REG_CONFIG_DISP_OVL0_2L_OVL1_OVL1_2L_BGOUT_SEL_IN         (DISPSYS_CONFIG_BASE + 0xF68)
#define DISP_REG_CONFIG_DISP_OVL0_BLENDOUT_SOUT_SEL         (DISPSYS_CONFIG_BASE + 0xF6C)
#define DISP_REG_CONFIG_DISP_TOVL0_OUT1_SEL_IN         (DISPSYS_CONFIG_BASE + 0xF70)
#define DISP_REG_CONFIG_DISP_MDP_AAL5_SEL_IN         (DISPSYS_CONFIG_BASE + 0xEBC)
#define DISP_REG_CONFIG_DISP_MDP_AAL5_SOUT_SEL         (DISPSYS_CONFIG_BASE + 0xEC4)

#define DISP_REG_CONFIG_DISP_MDP_TDSHP5_SOUT_SEL         (DISPSYS_CONFIG_BASE + 0xF94)
#define DISP_REG_CONFIG_DISP_RDMA3_SOUT_SEL         (DISPSYS_CONFIG_BASE + 0xF98)
#define DISP_REG_CONFIG_DISP_OVL1_OVL1_2L_PQOUT_SEL_IN         (DISPSYS_CONFIG_BASE + 0xF9C)
#define DISP_REG_CONFIG_DISP_OVL1_2L_BLENDOUT_SOUT_SEL         (DISPSYS_CONFIG_BASE + 0xFA0)
#define DISP_REG_CONFIG_DISP_OVL1_2L_BGOUT_SOUT_SEL         (DISPSYS_CONFIG_BASE + 0xFA4)
#define DISP_REG_CONFIG_DISP_OVL1_BLENDOUT_SOUT_SEL         (DISPSYS_CONFIG_BASE + 0xFA8)
#define DISP_REG_CONFIG_DISP_OVL1_BGOUT_SOUT_SEL         (DISPSYS_CONFIG_BASE + 0xFAC)
#define DISP_REG_CONFIG_DISP_TOVL1_OUT0_SEL_IN         (DISPSYS_CONFIG_BASE + 0xFB0)
#define DISP_REG_CONFIG_DISP_TOVL1_OUT1_SEL_IN         (DISPSYS_CONFIG_BASE + 0xFB4)
#define DISP_REG_CONFIG_DISP_OVL1_OVL1_2L_BGOUT_SEL_IN         (DISPSYS_CONFIG_BASE + 0xFB8)
#define DISP_REG_CONFIG_DISP_TOVL1_OUT1_MOUT_EN         (DISPSYS_CONFIG_BASE + 0xFBC)
#define DISP_REG_CONFIG_DISP_TOVL1_OUT0_MOUT_EN         (DISPSYS_CONFIG_BASE + 0xFC0)
#define DISP_REG_CONFIG_DISP_RDMA1_SEL_IN         (DISPSYS_CONFIG_BASE + 0xFC4)
#define DISP_REG_CONFIG_DISP_RSZ1_SEL_IN         (DISPSYS_CONFIG_BASE + 0xFC8)
#define DISP_REG_CONFIG_DISP_RDMA1_SOUT_SEL         (DISPSYS_CONFIG_BASE + 0xFCC)
#define DISP_REG_CONFIG_MMSYS_MOUT_MASK0         (DISPSYS_CONFIG_BASE + 0xE90)
#define DISP_REG_CONFIG_MMSYS_MOUT_MASK1         (DISPSYS_CONFIG_BASE + 0xE94)
#define DISP_REG_CONFIG_MMSYS_MOUT_MASK2         (DISPSYS_CONFIG_BASE + 0xE98)

#define DISP_REG_CONFIG_DISP_RDMA2_RSZ0_RSZ1_SOUT_SEL \
    (DISPSYS_CONFIG_BASE + 0xF08)
#define DISP_REG_CONFIG_DISP_MDP_TDSHP4_SOUT_SEL \
    (DISPSYS_CONFIG_BASE + 0xF0C)
#define DISP_REG_CONFIG_DISP_OVL0_2L_UFOD_SEL_IN \
    (DISPSYS_CONFIG_BASE + 0xF10)
#define DISP_REG_CONFIG_DISP_OVL0_UFOD_SEL_IN \
    (DISPSYS_CONFIG_BASE + 0xF14)
#define DISP_REG_CONFIG_DISP_TOVL0_OUT0_MOUT_EN \
    (DISPSYS_CONFIG_BASE + 0xF18)
#define DISP_REG_CONFIG_DISP_TOVL0_OUT1_MOUT_EN \
    (DISPSYS_CONFIG_BASE + 0xF1C)
#define DISP_REG_CONFIG_DISP_OVL0_OVL0_2L_PQOUT_SEL_IN \
    (DISPSYS_CONFIG_BASE + 0xF20)
#define DISP_REG_CONFIG_DISP_RSZ0_SEL_IN \
    (DISPSYS_CONFIG_BASE + 0xF24)
#define DISP_REG_CONFIG_DISP_RSZ0_MOUT_EN \
    (DISPSYS_CONFIG_BASE + 0xF28)
#define DISP_REG_CONFIG_DISP_RDMA0_SEL_IN \
    (DISPSYS_CONFIG_BASE + 0xF2C)
#define DISP_REG_CONFIG_DISP_RDMA0_RSZ0_SOUT_SEL \
    (DISPSYS_CONFIG_BASE + 0xF30)
#define DISP_REG_CONFIG_DISP_CCORR0_SOUT_SEL \
    (DISPSYS_CONFIG_BASE + 0xF34)
#define DISP_REG_CONFIG_DISP_AAL0_SEL_IN \
    (DISPSYS_CONFIG_BASE + 0xF38)
#define DISP_REG_CONFIG_DISP_DITHER0_MOUT_EN \
    (DISPSYS_CONFIG_BASE + 0xF3C)
#define DISP_REG_CONFIG_DSI0_SEL_IN \
    (DISPSYS_CONFIG_BASE + 0xF40)
#define DISP_REG_CONFIG_DISP_WDMA0_SEL_IN \
    (DISPSYS_CONFIG_BASE + 0xF44)
#define DISP_REG_CONFIG_UFBC_WDMA0_SEL_IN \
    (DISPSYS_CONFIG_BASE + 0xF48)
#define DISP_REG_CONFIG_DISP_OVL2_2L_OUT0_MOUT_EN \
    (DISPSYS_CONFIG_BASE + 0xF4C)
#define DISP_REG_CONFIG_DISP_TOVL0_PQOUT_MDP_RDMA4_SEL_IN \
    (DISPSYS_CONFIG_BASE + 0xF50)
#define DISP_REG_CONFIG_MDP_AAL4_SOUT_SEL \
    (DISPSYS_CONFIG_BASE + 0xF54)
#define DISP_REG_CONFIG_DISP_MDP_AAL4_MDP_HDR4_SEL_IN \
    (DISPSYS_CONFIG_BASE + 0xF58)
#define DISP_REG_CONFIG_DISP_MDP_AAL4_SEL_IN \
    (DISPSYS_CONFIG_BASE + 0xF5C)
#define DISP_REG_CONFIG_DISP_MDP_AAL4_SOUT_SEL \
    (DISPSYS_CONFIG_BASE + 0xF60)
#define DISP_REG_CONFIG_DISP_Y2R0_SOUT_SEL \
    (DISPSYS_CONFIG_BASE + 0xF64)

#define DISP_REG_CONFIG_MMSYS_DL_VALID0         (DISPSYS_CONFIG_BASE + 0xE9C)
#define DISP_REG_CONFIG_MMSYS_DL_VALID1         (DISPSYS_CONFIG_BASE + 0xEA0)
#define DISP_REG_CONFIG_MMSYS_DL_VALID2         (DISPSYS_CONFIG_BASE + 0xEA4)
#define DISP_REG_CONFIG_MMSYS_DL_VALID3         (DISPSYS_CONFIG_BASE + 0xE80)
#define DISP_REG_CONFIG_MMSYS_DL_VALID4         (DISPSYS_CONFIG_BASE + 0xE84)
#define DISP_REG_CONFIG_MMSYS_DL_VALID5         (DISPSYS_CONFIG_BASE + 0xE88)
#define DISP_REG_CONFIG_MMSYS_DL_READY0         (DISPSYS_CONFIG_BASE + 0xEA8)
#define DISP_REG_CONFIG_MMSYS_DL_READY1         (DISPSYS_CONFIG_BASE + 0xEAC)
#define DISP_REG_CONFIG_MMSYS_DL_READY2         (DISPSYS_CONFIG_BASE + 0xEB0)
#define DISP_REG_CONFIG_MMSYS_DL_READY3         (DISPSYS_CONFIG_BASE + 0xE70)
#define DISP_REG_CONFIG_MMSYS_DL_READY4         (DISPSYS_CONFIG_BASE + 0xE74)
#define DISP_REG_CONFIG_MMSYS_DL_READY5         (DISPSYS_CONFIG_BASE + 0xE78)

#define DISP_REG_CONFIG_MMSYS_OVL_CON             (DISPSYS_CONFIG_BASE + 0xF04)
    #define FLD_CON_OVL3_2L                         REG_FLD(2, 8)
    #define FLD_CON_OVL1_2L                         REG_FLD(2, 6)
    #define FLD_CON_OVL1                            REG_FLD(2, 4)
    #define FLD_CON_OVL0_2L                         REG_FLD(2, 2)
    #define FLD_CON_OVL0                            REG_FLD(2, 0)

/* SMI_LARB0 */
#define DISP_REG_SMI_LARB0_MMU_EN                 (DISPSYS_SMI_LARB0_BASE+0x380)
#define REG_FLD_MMU_EN                                REG_FLD(1, 0)

/* SMI_SUBCOM0 */
#define DISP_REG_SMI_SUBCOM0_L1LEN                 (DISPSYS_SMI_SUBCOM0_BASE+0x100)

/* SMI_SUBCOM1 */
#define DISP_REG_SMI_SUBCOM1_L1LEN                 (DISPSYS_SMI_SUBCOM1_BASE+0x100)

/* APMIXED */
#define DISP_REG_APMIXED_PLL_CON0   (DISPSYS_APMIXED_BASE+0x14)
#define FLD_PLL_MIPI_DSI0_26M_CK_EN    REG_FLD_MSB_LSB(17, 16)

/* field definition */
/* ------------------------------------------------------------- */
