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

#define DISP_REG_CONFIG_MMSYS_CG_CON0            (DISPSYS_CONFIG_BASE + 0x100)
#define DISP_REG_CONFIG_MMSYS_CG_SET0            (DISPSYS_CONFIG_BASE + 0x104)
#define DISP_REG_CONFIG_MMSYS_CG_CLR0            (DISPSYS_CONFIG_BASE + 0x108)
    #define FLD_CG0_SMI_IOMMU                        REG_FLD(1, 24)
    #define FLD_CG0_SMI_GALS                        REG_FLD(1, 22)
    #define FLD_CG0_DISP_FAKE_ENG1                    REG_FLD(1, 21)
    #define FLD_CG0_DISP_FAKE_ENG0                    REG_FLD(1, 20)
    #define FLD_CG0_DSI0                            REG_FLD(1, 19)
    #define FLD_CG0_SMI_COMMON                         REG_FLD(1, 17)
    #define FLD_CG0_DITHER0                         REG_FLD(1, 16)
    #define FLD_CG0_POSTMASK0                        REG_FLD(1, 14)
    #define FLD_CG0_DISP_GAMMA0                        REG_FLD(1, 13)
    #define FLD_CG0_SMI_INFRA                        REG_FLD(1, 11)
    #define FLD_CG0_COLOR0                            REG_FLD(1, 10)
    #define FLD_CG0_CCORR0                            REG_FLD(1, 9)
    #define FLD_CG0_DISP_AAL0                        REG_FLD(1, 8)
    #define FLD_CG0_DISP_RSZ0                        REG_FLD(1, 7)
    #define FLD_CG0_DISP_WDMA0                        REG_FLD(1, 5)
    #define FLD_CG0_OVL0_2L                            REG_FLD(1, 4)
    #define FLD_CG0_DISP_RDMA0                        REG_FLD(1, 3)
    #define FLD_CG0_OVL0                            REG_FLD(1, 2)
    #define FLD_CG0_DISPSYS_CONFIG                    REG_FLD(1, 1)
    #define FLD_CG0_DISP_MUTEX0                     REG_FLD(1, 0)

#define DISP_REG_CONFIG_MMSYS_CG_CON1            (DISPSYS_CONFIG_BASE + 0x110)
#define DISP_REG_CONFIG_MMSYS_CG_SET1            (DISPSYS_CONFIG_BASE + 0x114)
#define DISP_REG_CONFIG_MMSYS_CG_CLR1            (DISPSYS_CONFIG_BASE + 0x118)


#define DISP_REG_CONFIG_MMSYS_CG_CON2            (DISPSYS_CONFIG_BASE + 0x1A0)
#define DISP_REG_CONFIG_MMSYS_CG_SET2            (DISPSYS_CONFIG_BASE + 0x1A4)
#define DISP_REG_CONFIG_MMSYS_CG_CLR2            (DISPSYS_CONFIG_BASE + 0x1A8)
    #define FLD_CG2_32MHZ                            REG_FLD(1, 25)
    #define FLD_CG2_26MHZ                            REG_FLD(1, 24)
    #define FLD_CG2_DPI                              REG_FLD(1, 8)
    #define FLD_CG2_DSI0                             REG_FLD(1, 0)

#define DISP_REG_CONFIG_MMSYS_LCM_RST_B         (DISPSYS_CONFIG_BASE + 0x180)

#define DISP_REG_CONFIG_SMI_LARB0_GREQ            (DISPSYS_CONFIG_BASE + 0x8DC)
#define DISP_REG_CONFIG_SMI_LARB1_GREQ        (DISPSYS_CONFIG_BASE + 0x8E0)

#define DISP_RDMA0_RSZ0_SOUT_SEL                (DISPSYS_CONFIG_BASE + 0xF0C)
#define DISP_SPR0_MOUT_EN                        (DISPSYS_CONFIG_BASE + 0xF10)
#define DISP_TOVL0_OUT0_MOUT_EN                    (DISPSYS_CONFIG_BASE + 0xF14)
#define DISP_TOVL0_OUT1_MOUT_EN                    (DISPSYS_CONFIG_BASE + 0xF18)
#define DISP_RSZ0_MOUT_EN                        (DISPSYS_CONFIG_BASE + 0xF1C)
#define DISP_DITHER0_MOUT_EN                (DISPSYS_CONFIG_BASE + 0xF20)
#define DISP_RSZ0_SEL_IN                    (DISPSYS_CONFIG_BASE + 0xF24)
#define DISP_RDMA0_SEL_IN                    (DISPSYS_CONFIG_BASE + 0xF28)
#define DISP_BYPASS_SPR0_SEL_IN             (DISPSYS_CONFIG_BASE + 0xF2C)
#define DSI0_SEL_IN                                (DISPSYS_CONFIG_BASE + 0xF30)
#define DISP_WDMA0_SEL_IN                        (DISPSYS_CONFIG_BASE + 0xF34)
#define DISPSYS_VERSION                            (DISPSYS_CONFIG_BASE + 0xFFC)

#define DISP_REG_CONFIG_MMSYS_MOUT_RST         (DISPSYS_CONFIG_BASE + 0xF00)
#define DISP_REG_CONFIG_MMSYS_MOUT_MASK0         (DISPSYS_CONFIG_BASE + 0xE90)
#define DISP_REG_CONFIG_MMSYS_MOUT_MASK1         (DISPSYS_CONFIG_BASE + 0xE94)
#define DISP_REG_CONFIG_MMSYS_MOUT_MASK2         (DISPSYS_CONFIG_BASE + 0xE98)

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

/* field definition */
/* ------------------------------------------------------------- */
