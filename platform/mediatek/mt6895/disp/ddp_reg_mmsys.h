/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

/* field definition */
/* ------------------------------------------------------------- */
/* Config */
#define DISP_REG_CONFIG_MMSYS_INTEN                     (DISPSYS_CONFIG_BASE + 0x0)

/* work around 0_2L and 1_2L change*/
#define DISP_REG_CONFIG_MMSYS_MISC                      (DISPSYS_CONFIG_BASE + 0x0F0)
#define DISP1_REG_CONFIG_MMSYS_MISC                     (DISPSYS1_CONFIG_BASE + 0x0F0)
    #define FLD_OVL0_ULTRA_SEL                           REG_FLD_MSB_LSB(5, 2)
    #define FLD_OVL0_2L_ULTRA_SEL                        REG_FLD_MSB_LSB(9, 6)
    #define FLD_OVL1_2L_ULTRA_SEL                        REG_FLD_MSB_LSB(13, 10)

#define DISP_REG_CONFIG_MMSYS_EMI_REQ_CTRL              (DISPSYS_CONFIG_BASE + 0x0F8)
#define DISP1_REG_CONFIG_MMSYS_EMI_REQ_CTRL             (DISPSYS1_CONFIG_BASE + 0x0F8)

#define DISP_REG_CONFIG_MMSYS_CG_CON0                   (DISPSYS_CONFIG_BASE + 0x100)
#define DISP_REG_CONFIG_MMSYS_CG_SET0                   (DISPSYS_CONFIG_BASE + 0x104)
#define DISP_REG_CONFIG_MMSYS_CG_CLR0                   (DISPSYS_CONFIG_BASE + 0x108)
#define DISP1_REG_CONFIG_MMSYS_CG_CON0                  (DISPSYS1_CONFIG_BASE + 0x100)
#define DISP1_REG_CONFIG_MMSYS_CG_SET0                  (DISPSYS1_CONFIG_BASE + 0x104)
#define DISP1_REG_CONFIG_MMSYS_CG_CLR0                  (DISPSYS1_CONFIG_BASE + 0x108)

    #define FLD_CG0_DISP_WDMA1                      REG_FLD(1, 31)
    #define FLD_CG0_DISP_UFBC_WDMA0                 REG_FLD(1, 30)
    #define FLD_CG0_DSI0_MM                         REG_FLD(1, 29)
    #define FLD_CG0_DISP_DSC_WRAP0                  REG_FLD(1, 27)
    #define FLD_CG0_DISP_SPR0                       REG_FLD(1, 26)
    #define FLD_CG0_DISP_CM0                        REG_FLD(1, 25)
    #define FLD_CG0_DITHER0                         REG_FLD(1, 24)
    #define FLD_CG0_POSTMASK0                       REG_FLD(1, 23)
    #define FLD_CG0_DISP_GAMMA0                     REG_FLD(1, 22)
    #define FLD_CG0_DISP_AAL0                       REG_FLD(1, 21)
    #define FLD_CG0_DISP_CCORR1                     REG_FLD(1, 20)
    #define FLD_CG0_DISP_CCORR0                     REG_FLD(1, 19)
    #define FLD_CG0_DISP_COLOR0                     REG_FLD(1, 18)
    #define FLD_CG0_DISP_RSZ0                       REG_FLD(1, 17)
    #define FLD_CG0_DISP_DLO_ASYNC2                 REG_FLD(1, 16)
    #define FLD_CG0_DISP_DLO_ASYNC1                 REG_FLD(1, 15)
    #define FLD_CG0_DISP_DLO_ASYNC0                 REG_FLD(1, 14)
    #define FLD_CG0_DISP_DLI_ASYNC2                 REG_FLD(1, 13)
    #define FLD_CG0_DISP_DLI_ASYNC1                 REG_FLD(1, 12)
    #define FLD_CG0_DISP_DLI_ASYNC0                 REG_FLD(1, 11)
    #define FLD_CG0_DISP_RDMA1                      REG_FLD(1, 10)
    #define FLD_CG0_DISP_RDMA0                      REG_FLD(1, 9)
    #define FLD_CG0_OVL0_2L_NWCG                    REG_FLD(1, 8)
    #define FLD_CG0_DPI0                            REG_FLD(1, 7)
    #define FLD_CG0_FAKE_ENG1                       REG_FLD(1, 6)
    #define FLD_CG0_DISP_WDMA0                      REG_FLD(1, 5)
    #define FLD_CG0_INLINEROT                       REG_FLD(1, 4)
    #define FLD_CG0_FAKE_ENG0                       REG_FLD(1, 3)
    #define FLD_CG0_DISP_MERGE0                     REG_FLD(1, 2)
    #define FLD_CG0_DISP_OVL0                       REG_FLD(1, 1)
    #define FLD_CG0_DISP_MUTEX0                     REG_FLD(1, 0)

#define DISP_REG_CONFIG_MMSYS_CG_CON1                   (DISPSYS_CONFIG_BASE + 0x110)
#define DISP_REG_CONFIG_MMSYS_CG_SET1                   (DISPSYS_CONFIG_BASE + 0x114)
/*todo CG for PQ module*/
#define DISP_REG_CONFIG_MMSYS_CG_CLR1                   (DISPSYS_CONFIG_BASE + 0x118)
#define DISP1_REG_CONFIG_MMSYS_CG_CON1                  (DISPSYS1_CONFIG_BASE + 0x110)
#define DISP1_REG_CONFIG_MMSYS_CG_SET1                  (DISPSYS1_CONFIG_BASE + 0x114)
/*todo CG for PQ module*/
#define DISP1_REG_CONFIG_MMSYS_CG_CLR1                  (DISPSYS1_CONFIG_BASE + 0x118)

    #define FLD_CG1_SMI_SUB_COMM                    REG_FLD(1, 20)
    #define FLD_CG1_DISP_OVL1_2L_NWCG               REG_FLD(1, 16)
    #define FLD_CG1_DISP_OVL1_2L                    REG_FLD(1, 12)
    #define FLD_CG1_DISP_DLO_ASYNC3                 REG_FLD(1, 10)
    #define FLD_CG1_DISP_DLI_ASYNC3                 REG_FLD(1, 9)
    #define FLD_CG1_DISP_OVL0_2L                    REG_FLD(1, 8)
    #define FLD_CG1_DISP_CHIST1                     REG_FLD(1, 7)
    #define FLD_CG1_DISP_CHIST0                     REG_FLD(1, 6)
    #define FLD_CG1_DISP_MDP_AAL0                   REG_FLD(1, 5)
    #define FLD_CG1_DISP_Y2R0                       REG_FLD(1, 4)
    #define FLD_CG1_DISP_C3D0                       REG_FLD(1, 3)
    #define FLD_CG1_DISP_TDSHP0                     REG_FLD(1, 2)
    #define FLD_CG1_APB_BUS                         REG_FLD(1, 1)
    #define FLD_CG1_DP_INTF                         REG_FLD(1, 0)

#define DISP_REG_CONFIG_MMSYS_CG_CON2                   (DISPSYS_CONFIG_BASE + 0x1a0)
#define DISP_REG_CONFIG_MMSYS_CG_SET2                   (DISPSYS_CONFIG_BASE + 0x1a4)
#define DISP_REG_CONFIG_MMSYS_CG_CLR2                   (DISPSYS_CONFIG_BASE + 0x1a8)
#define DISP1_REG_CONFIG_MMSYS_CG_CON2                  (DISPSYS1_CONFIG_BASE + 0x1a0)
#define DISP1_REG_CONFIG_MMSYS_CG_SET2                  (DISPSYS1_CONFIG_BASE + 0x1a4)
#define DISP1_REG_CONFIG_MMSYS_CG_CLR2                  (DISPSYS1_CONFIG_BASE + 0x1a8)

    #define FLD_CG2_32MHZ                           REG_FLD(1, 25)
    #define FLD_CG2_26MHZ                           REG_FLD(1, 24)
    #define FLD_CG2_DPTX                            REG_FLD(1, 16)
    #define FLD_CG2_26MHZ_URGENT                    REG_FLD(1, 11)
    #define FLD_CG2_DSI1                            REG_FLD(1, 8)
    #define FLD_CG2_DSI0                            REG_FLD(1, 0)

#define DISP_REG_CONFIG_MMSYS_LCM_RST_B                (DISPSYS_CONFIG_BASE + 0x180)

#define DISP_REG_CONFIG_GCE_EVENT_SEL                  (DISPSYS_CONFIG_BASE + 0x308)
#define DISP1_REG_CONFIG_GCE_EVENT_SEL                 (DISPSYS1_CONFIG_BASE + 0x308)

#define DISP_REG_CONFIG_SMI_LARB0_GREQ                 (DISPSYS_CONFIG_BASE + 0x8DC)
#define DISP1_REG_CONFIG_SMI_LARB0_GREQ                (DISPSYS1_CONFIG_BASE + 0x8DC)

#define DISP_REG_CONFIG_SMI_LARB1_GREQ                 (DISPSYS_CONFIG_BASE + 0x8E0)
#define DISP1_REG_CONFIG_SMI_LARB1_GREQ                (DISPSYS1_CONFIG_BASE + 0x8E0)

#define DISP_REG_CONFIG_BYPASS_MUX_SHADOW              (DISPSYS_CONFIG_BASE + 0xF00)
#define DISP1_REG_CONFIG_BYPASS_MUX_SHADOW             (DISPSYS1_CONFIG_BASE + 0xF00)

#define DISP_REG_CONFIG_DISP_MAIN_OVL_DISP_PQ0_SEL_IN             (DISPSYS_CONFIG_BASE + 0xF34)
#define DISP_REG_CONFIG_DISP_PQ0_SEL_IN                           (DISPSYS_CONFIG_BASE + 0xF38)
#define DISP_REG_CONFIG_DISP_RDMA0_SEL_IN                         (DISPSYS_CONFIG_BASE + 0xF3C)
#define DISP_REG_CONFIG_DISP_DSI0_SEL_IN                          (DISPSYS_CONFIG_BASE + 0xF84)
#define DISP_REG_CONFIG_DISP_RDMA0_POS_SEL_IN                     (DISPSYS_CONFIG_BASE + 0xF40)
#define DISP_REG_CONFIG_DISP_COLOR0_SEL_IN                        (DISPSYS_CONFIG_BASE + 0xF48)
#define DISP_REG_CONFIG_DISP_C3D0_SEL_IN                          (DISPSYS_CONFIG_BASE + 0xF44)
#define DISP_REG_CONFIG_DISP_MDP_AAL0_SEL_IN                      (DISPSYS_CONFIG_BASE + 0xF4C)
#define DISP_REG_CONFIG_DISP_MAIN0_SEL_IN                         (DISPSYS_CONFIG_BASE + 0xF78)
#define DISP_REG_CONFIG_DISP_DSC_WRAP0_L_SEL_IN                   (DISPSYS_CONFIG_BASE + 0xF68)
#define DISP_REG_CONFIG_DISP_DSC_WRAP0_R_SEL_IN                   (DISPSYS_CONFIG_BASE + 0xF6C)
#define DISP_REG_CONFIG_DISP_RDMA0_SOUT_SEL                       (DISPSYS_CONFIG_BASE + 0xF98)
#define DISP_REG_CONFIG_DISP_TDSHP0_SOUT_SEL                      (DISPSYS_CONFIG_BASE + 0xF9C)
#define DISP_REG_CONFIG_DISP_CCORR1_SOUT_SEL                      (DISPSYS_CONFIG_BASE + 0xFA4)
#define DISP_REG_CONFIG_DISP_C3D0_SOUT_SEL                        (DISPSYS_CONFIG_BASE + 0xFA0)
#define DISP_REG_CONFIG_DISP_PQ0_SOUT_SEL                         (DISPSYS_CONFIG_BASE + 0xFAC)
#define DISP_REG_CONFIG_DISP_DLI0_SOUT_SEL                        (DISPSYS_CONFIG_BASE + 0xFB4)
#define DISP_REG_CONFIG_DISP_MAIN0_SOUT_SEL                       (DISPSYS_CONFIG_BASE + 0xFBC)
#define DISP_REG_CONFIG_DISP_OVL0_BLEND_MOUT_EN                   (DISPSYS_CONFIG_BASE + 0xFCC)
#define DISP_REG_CONFIG_DISP_RDMA0_POS_MOUT_EN                    (DISPSYS_CONFIG_BASE + 0xFDC)
#define DISP_REG_CONFIG_DISP_POSTMASK0_MOUT_EN                    (DISPSYS_CONFIG_BASE + 0xFE0)
#define DISP_REG_CONFIG_DISP_DITHER0_MOUT_EN                      (DISPSYS_CONFIG_BASE + 0xFE4)
#define DISP_REG_CONFIG_DISP_SPR0_MOUT_EN                         (DISPSYS_CONFIG_BASE + 0xFE8)
#define DISP_REG_CONFIG_DISP_DSC_WRAP0_MOUT_EN                    (DISPSYS_CONFIG_BASE + 0xFF4)

#define DISP1_REG_CONFIG_DISP_MAIN_OVL_DISP_PQ0_SEL_IN             (DISPSYS1_CONFIG_BASE + 0xF34)
#define DISP1_REG_CONFIG_DISP_PQ0_SEL_IN                           (DISPSYS1_CONFIG_BASE + 0xF38)
#define DISP1_REG_CONFIG_DISP_RDMA0_SEL_IN                         (DISPSYS1_CONFIG_BASE + 0xF3C)
#define DISP1_REG_CONFIG_DISP_DSI0_SEL_IN                          (DISPSYS1_CONFIG_BASE + 0xF84)
#define DISP1_REG_CONFIG_DISP_RDMA0_POS_SEL_IN                     (DISPSYS1_CONFIG_BASE + 0xF40)
#define DISP1_REG_CONFIG_DISP_COLOR0_SEL_IN                        (DISPSYS1_CONFIG_BASE + 0xF48)
#define DISP1_REG_CONFIG_DISP_C3D0_SEL_IN                          (DISPSYS1_CONFIG_BASE + 0xF44)
#define DISP1_REG_CONFIG_DISP_MDP_AAL0_SEL_IN                      (DISPSYS1_CONFIG_BASE + 0xF4C)
#define DISP1_REG_CONFIG_DISP_MAIN0_SEL_IN                         (DISPSYS1_CONFIG_BASE + 0xF78)
#define DISP1_REG_CONFIG_DISP_DSC_WRAP0_L_SEL_IN                   (DISPSYS1_CONFIG_BASE + 0xF68)
#define DISP1_REG_CONFIG_DISP_DSC_WRAP0_R_SEL_IN                   (DISPSYS1_CONFIG_BASE + 0xF6C)
#define DISP1_REG_CONFIG_DISP_RDMA0_SOUT_SEL                       (DISPSYS1_CONFIG_BASE + 0xF98)
#define DISP1_REG_CONFIG_DISP_TDSHP0_SOUT_SEL                      (DISPSYS1_CONFIG_BASE + 0xF9C)
#define DISP1_REG_CONFIG_DISP_CCORR1_SOUT_SEL                      (DISPSYS1_CONFIG_BASE + 0xFA4)
#define DISP1_REG_CONFIG_DISP_C3D0_SOUT_SEL                        (DISPSYS1_CONFIG_BASE + 0xFA0)
#define DISP1_REG_CONFIG_DISP_PQ0_SOUT_SEL                         (DISPSYS1_CONFIG_BASE + 0xFAC)
#define DISP1_REG_CONFIG_DISP_DLI0_SOUT_SEL                        (DISPSYS1_CONFIG_BASE + 0xFB4)
#define DISP1_REG_CONFIG_DISP_MAIN0_SOUT_SEL                       (DISPSYS1_CONFIG_BASE + 0xFBC)
#define DISP1_REG_CONFIG_DISP_OVL0_BLEND_MOUT_EN                   (DISPSYS1_CONFIG_BASE + 0xFCC)
#define DISP1_REG_CONFIG_DISP_RDMA0_POS_MOUT_EN                    (DISPSYS1_CONFIG_BASE + 0xFDC)
#define DISP1_REG_CONFIG_DISP_POSTMASK0_MOUT_EN                    (DISPSYS1_CONFIG_BASE + 0xFE0)
#define DISP1_REG_CONFIG_DISP_DITHER0_MOUT_EN                      (DISPSYS1_CONFIG_BASE + 0xFE4)
#define DISP1_REG_CONFIG_DISP_SPR0_MOUT_EN                         (DISPSYS1_CONFIG_BASE + 0xFE8)
#define DISP1_REG_CONFIG_DISP_DSC_WRAP0_MOUT_EN                    (DISPSYS1_CONFIG_BASE + 0xFF4)

#define DISP_REG_CONFIG_MMSYS_MOUT_MASK0        (DISPSYS_CONFIG_BASE + 0xE90)
#define DISP_REG_CONFIG_MMSYS_MOUT_MASK1        (DISPSYS_CONFIG_BASE + 0xE94)
#define DISP_REG_CONFIG_MMSYS_MOUT_MASK2        (DISPSYS_CONFIG_BASE + 0xE98)

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

#define DISP1_REG_CONFIG_MMSYS_MOUT_MASK0       (DISPSYS1_CONFIG_BASE + 0xE90)
#define DISP1_REG_CONFIG_MMSYS_MOUT_MASK1       (DISPSYS1_CONFIG_BASE + 0xE94)
#define DISP1_REG_CONFIG_MMSYS_MOUT_MASK2       (DISPSYS1_CONFIG_BASE + 0xE98)

#define DISP1_REG_CONFIG_MMSYS_DL_VALID0        (DISPSYS1_CONFIG_BASE + 0xE9C)
#define DISP1_REG_CONFIG_MMSYS_DL_VALID1        (DISPSYS1_CONFIG_BASE + 0xEA0)
#define DISP1_REG_CONFIG_MMSYS_DL_VALID2        (DISPSYS1_CONFIG_BASE + 0xEA4)
#define DISP1_REG_CONFIG_MMSYS_DL_VALID3        (DISPSYS1_CONFIG_BASE + 0xE80)
#define DISP1_REG_CONFIG_MMSYS_DL_VALID4        (DISPSYS1_CONFIG_BASE + 0xE84)
#define DISP1_REG_CONFIG_MMSYS_DL_VALID5        (DISPSYS1_CONFIG_BASE + 0xE88)
#define DISP1_REG_CONFIG_MMSYS_DL_READY0        (DISPSYS1_CONFIG_BASE + 0xEA8)
#define DISP1_REG_CONFIG_MMSYS_DL_READY1        (DISPSYS1_CONFIG_BASE + 0xEAC)
#define DISP1_REG_CONFIG_MMSYS_DL_READY2        (DISPSYS1_CONFIG_BASE + 0xEB0)
#define DISP1_REG_CONFIG_MMSYS_DL_READY3        (DISPSYS1_CONFIG_BASE + 0xE70)
#define DISP1_REG_CONFIG_MMSYS_DL_READY4        (DISPSYS1_CONFIG_BASE + 0xE74)
#define DISP1_REG_CONFIG_MMSYS_DL_READY5        (DISPSYS1_CONFIG_BASE + 0xE78)

/* workaround 0_2L and 1_2L change*/
#define DISP_REG_CONFIG_MMSYS_OVL_CON             (DISPSYS_CONFIG_BASE + 0xF08)
#define DISP1_REG_CONFIG_MMSYS_OVL_CON            (DISPSYS1_CONFIG_BASE + 0xF08)
    #define FLD_CON_OVL1_2L_NWCG                    REG_FLD(2, 8)
    #define FLD_CON_OVL0_2L_NWCG                    REG_FLD(2, 6)
    #define FLD_CON_OVL1_2L                         REG_FLD(2, 4)
    #define FLD_CON_OVL0_2L                         REG_FLD(2, 2)
    #define FLD_CON_OVL0                            REG_FLD(2, 0)

/* SMI_LARB0 */
#define DISP_REG_SMI_LARB0_MMU_EN                 (DISPSYS_SMI_LARB0_BASE+0x380)
#define DISP_REG_SMI_LARB1_MMU_EN                 (DISPSYS_SMI_LARB1_BASE+0x380)
#define REG_FLD_MMU_EN                              REG_FLD(1, 0)

/* SMI_SUBCOM0 */
#define DISP_REG_SMI_SUBCOM0_L1LEN                (DISPSYS_SMI_SUBCOM0_BASE+0x100)

/* SMI_SUBCOM1 */
#define DISP_REG_SMI_SUBCOM1_L1LEN                (DISPSYS_SMI_SUBCOM1_BASE+0x100)

/* field definition */
/* ------------------------------------------------------------- */
