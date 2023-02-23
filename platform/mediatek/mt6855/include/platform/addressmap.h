/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once

#include <sys/types.h>

#define REGION(name, base, size)    name##_BASE_PHY = (uintptr_t)base, \
                                    name##_BASE     = (uintptr_t)KERNEL_ASPACE_BASE + base, \
                                    name##_SIZE     = (size_t)size

enum {
    REGION(SRAM,   0x00100000, 0x00020000),
    REGION(HWVER,  0x08000000, 0x00001000),
    REGION(GIC,    0x0C000000, 0x00400000),
    REGION(MCUCFG, 0x0C530000, 0x00200000),
    REGION(DBG,    0x0D000000, 0x00050000),
    REGION(IO,     0x10000000, 0x10000000),
    REGION(DRAM,   0x40000000, 0x40000000),
};

enum {
    MP_CPUSYS_TOP_BASE   = GIC_BASE + 0x0538000,
    CPCCFG_REG_BASE      = GIC_BASE + 0x053A800,
};

enum {
    TOPCLK_BASE          = IO_BASE + 0x00000000,
    INFRACFG_AO_BASE     = IO_BASE + 0x00001000,
    PERICFG_BASE         = IO_BASE + 0x00003000,
    GPIO_BASE            = IO_BASE + 0x00005000,
    APMIXED_BASE         = IO_BASE + 0x0000C000,
    PWRAP_BASE           = IO_BASE + 0x0000D000,
    INFRA_AO_BCRM_BASE   = IO_BASE + 0x00022000,
    BUS_DBG_BASE         = IO_BASE + 0x00208000,
    CCIF0_AP_BASE        = IO_BASE + 0x00209000,
    INFRACFG_BASE        = IO_BASE + 0x0020e000,
    TZCC_BASE            = IO_BASE + 0x00210000,
    EMI_APB_BASE         = IO_BASE + 0x00219000,
    DDRPHY_AO_CH0_BASE   = IO_BASE + 0x00230000,
    INFRACFG_AO_MEM_BASE = IO_BASE + 0x00270000,
    INFRA_TRACKER_BASE   = IO_BASE + 0x00314000,
    THERM_CTRL_BASE      = IO_BASE + 0x00315000,
    INFRA_BUS_HRE_BASE   = IO_BASE + 0x0032C000,
    EMI_SMPU_BASE        = IO_BASE + 0x00351000,
    EMI_SMPU2_BASE       = IO_BASE + 0x00352000,
    DPM_PM1_SRAM_BASE    = IO_BASE + 0x00900000,
    DPM_DM1_SRAM_BASE    = IO_BASE + 0x00920000,
    DPM_CFG_CH0_BASE     = IO_BASE + 0x00940000,
    UART0_BASE           = IO_BASE + 0x01001000,
    UART1_BASE           = IO_BASE + 0x01002000,
    DISP_PWM0_BASE       = IO_BASE + 0x0100E000,
    PERI_AO_BCRM_BASE    = IO_BASE + 0x01035000,
    USB_BASE             = IO_BASE + 0x01210000,
    MSDC0_BASE           = IO_BASE + 0x01230000,
    MSDC1_BASE           = IO_BASE + 0x01240000,
    I2C11_BASE           = IO_BASE + 0x01280000,
    UFS0_MPHY_BASE       = IO_BASE + 0x012A0000,
    UFS0_BASE            = IO_BASE + 0x012B0000,
    UFS0_AO_CONFIG_BASE  = IO_BASE + 0x012B8000,
    I2C0_APDMA_BASE      = IO_BASE + 0x01300080,
    I2C1_APDMA_BASE      = IO_BASE + 0x01300100,
    I2C2_APDMA_BASE      = IO_BASE + 0x01300180,
    I2C3_APDMA_BASE      = IO_BASE + 0x01300280,
    I2C4_APDMA_BASE      = IO_BASE + 0x01300300,
    I2C5_APDMA_BASE      = IO_BASE + 0x01300400,
    I2C6_APDMA_BASE      = IO_BASE + 0x01300480,
    I2C7_APDMA_BASE      = IO_BASE + 0x01300500,
    I2C8_APDMA_BASE      = IO_BASE + 0x01300600,
    I2C9_APDMA_BASE      = IO_BASE + 0x01300700,
    I2C10_APDMA_BASE     = IO_BASE + 0x01300800,
    I2C11_APDMA_BASE     = IO_BASE + 0x01300880,
    IOCFG_LB_BASE        = IO_BASE + 0x01B00000,
    IOCFG_LM_BASE        = IO_BASE + 0x01B10000,
    I2C5_BASE            = IO_BASE + 0x01B20000,
    MSDC1_TOP_BASE       = IO_BASE + 0x01c00000,
    EFUSE_BASE           = IO_BASE + 0x01C10000,
    IOCFG_RB_BASE        = IO_BASE + 0x01C40000,
    IOCFG_BL_BASE        = IO_BASE + 0x01D10000,
    IOCFG_BM_BASE        = IO_BASE + 0x01D30000,
    I2C3_BASE            = IO_BASE + 0x01DB0000,
    I2C6_BASE            = IO_BASE + 0x01DB1000,
    I2C7_BASE            = IO_BASE + 0x01DB2000,
    I2C8_BASE            = IO_BASE + 0x01DB3000,
    I2C10_BASE           = IO_BASE + 0x01DB4000,
    IOCFG_LT_BASE        = IO_BASE + 0x01E30000,
    MIPI_TX0_CONFIG_BASE = IO_BASE + 0x01E50000,
    MSDC0_TOP_BASE       = IO_BASE + 0x01e70000,
    USB_SIF_BASE         = IO_BASE + 0x01E40000,
    IOCFG_RM_BASE        = IO_BASE + 0x01EB0000,
    IOCFG_RT_BASE        = IO_BASE + 0x01EC0000,
    I2C0_BASE            = IO_BASE + 0x01ED0000,
    I2C1_BASE            = IO_BASE + 0x01ED1000,
    I2C2_BASE            = IO_BASE + 0x01ED2000,
    I2C4_BASE            = IO_BASE + 0x01ED3000,
    I2C9_BASE            = IO_BASE + 0x01ED4000,
    IOCFG_TL_BASE        = IO_BASE + 0x01F20000,
    MMSYS_CONFIG_BASE    = IO_BASE + 0x04000000,
    MM_MUTEX_BASE        = IO_BASE + 0x04001000,
    OVL0_BASE            = IO_BASE + 0x04002000,
    DISP_OVL1_2L_BASE    = IO_BASE + 0x04004000,
    DISP_RDMA0_BASE      = IO_BASE + 0x04006000,
    DISP_TDSHP0_BASE     = IO_BASE + 0x04007000,
    DISP_COLOR0_BASE     = IO_BASE + 0x04009000,
    DISP_CCORR0_BASE     = IO_BASE + 0x0400A000,
    DISP_AAL0_BASE       = IO_BASE + 0x0400D000,
    DISP_GAMMA0_BASE     = IO_BASE + 0x0400E000,
    DISP_POSTMASK0_BASE  = IO_BASE + 0x0400F000,
    DISP_DITHER0_BASE    = IO_BASE + 0x04010000,
    DISP_DSC_BASE        = IO_BASE + 0x04015000,
    DISP_DSI0_BASE       = IO_BASE + 0x04017000,
    SMI_LARB0_BASE       = IO_BASE + 0x04021000,
    SMI_LARB1_BASE       = IO_BASE + 0x04022000,
    SPM_BASE             = IO_BASE + 0x0C001000,
    SPM_SRAM_BASE        = IO_BASE + 0x0C002000,
    SRCLKEN_RC_BASE      = IO_BASE + 0X0C00D000,
    RGU_BASE             = IO_BASE + 0x0C007000,
    SEJ_BASE             = IO_BASE + 0x0C009000,
    SECURITY_AO_BASE     = IO_BASE + 0x0C00B000,
    VLPCFG_BASE          = IO_BASE + 0x0C00C000,
    KPD_BASE             = IO_BASE + 0x0C00E000,
    DVFSRC_BASE          = IO_BASE + 0x0C00F000,
    VLP_AO_BCRM_BASE     = IO_BASE + 0x0C017000,
    SCP_ADDR_BASE        = IO_BASE + 0x0C400000,
    SPMI_MST_BASE        = IO_BASE + 0x0C801000,
    PMIF_SPMI_BASE       = IO_BASE + 0x0C804000,
    MMINFRA_BUS_HRE_BASE = IO_BASE + 0x0E900000,
    SMI_SUBCOM0_BASE     = IO_BASE + 0x0E807000,
    SMI_SUBCOM1_BASE     = IO_BASE + 0x0E808000,
    VCP_ADDR_BASE        = IO_BASE + 0x0EA00000,
};
