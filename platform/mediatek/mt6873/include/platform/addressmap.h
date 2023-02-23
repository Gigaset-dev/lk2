/*
 * Copyright (c) 2020 MediaTek Inc.
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
    REGION(GIC,    0x0C000000, 0x04000000),
    REGION(IO,     0x10000000, 0x10000000),
    REGION(DRAM,   0x40000000, 0x40000000),
};

enum {
    MCUCFG_BASE = GIC_BASE + 0x00530000,
};

enum {
    INFRACFG_AO_BASE     = IO_BASE + 0x00001000,
    PERICFG_BASE         = IO_BASE + 0x00003000,
    GPIO_BASE            = IO_BASE + 0x00005000,
    SPM_BASE             = IO_BASE + 0x00006000,
    RGU_BASE             = IO_BASE + 0x00007000,
    APMIXED_BASE         = IO_BASE + 0x0000C000,
    KPD_BASE             = IO_BASE + 0x00010000,
    DVFSRC_BASE          = IO_BASE + 0x00012000,
    PWRAP_BASE           = IO_BASE + 0x00026000,
    PMIF_SPMI_BASE       = IO_BASE + 0x00027000,
    SPMI_MST_BASE        = IO_BASE + 0x00029000,
    BUS_DBG_BASE         = IO_BASE + 0x00208000,
    I2C0_APDMA_BASE      = IO_BASE + 0x00217080,
    I2C1_APDMA_BASE      = IO_BASE + 0x00217100,
    I2C2_APDMA_BASE      = IO_BASE + 0x00217180,
    I2C3_APDMA_BASE      = IO_BASE + 0x00217300,
    I2C4_APDMA_BASE      = IO_BASE + 0x00217380,
    I2C5_APDMA_BASE      = IO_BASE + 0x00217500,
    I2C6_APDMA_BASE      = IO_BASE + 0x00217580,
    I2C7_APDMA_BASE      = IO_BASE + 0x00217600,
    I2C8_APDMA_BASE      = IO_BASE + 0x00217780,
    I2C9_APDMA_BASE      = IO_BASE + 0x00217900,
    PERI_TRACKER_BASE    = IO_BASE + 0x00218000,
    DDRPHY_AO_CH0_BASE   = IO_BASE + 0x00230000,
    INFRA_TRACKER_BASE   = IO_BASE + 0x00314000,
    SCP_ADDR_BASE        = IO_BASE + 0x00500000,
    DPM_PM1_SRAM_BASE    = IO_BASE + 0x00900000,
    DPM_DM1_SRAM_BASE    = IO_BASE + 0x00920000,
    DPM_CFG_CH0_BASE     = IO_BASE + 0x00940000,
    UART0_BASE           = IO_BASE + 0x01002000,
    UART1_BASE           = IO_BASE + 0x01003000,
    DISP_PWM0_BASE       = IO_BASE + 0x0100E000,
    USB3_BASE            = IO_BASE + 0x01200000,
    USB3_IPPC_BASE       = IO_BASE + 0x01203E00,
    UFS0_BASE            = IO_BASE + 0x01270000,
    EFUSE_BASE           = IO_BASE + 0x01C10000,
    IOCFG_RM_BASE        = IO_BASE + 0x01C20000,
    I2C3_BASE            = IO_BASE + 0x01CB0000,
    I2C7_BASE            = IO_BASE + 0x01D00000,
    I2C8_BASE            = IO_BASE + 0x01D01000,
    I2C9_BASE            = IO_BASE + 0x01D02000,
    IOCFG_BM_BASE        = IO_BASE + 0x01D10000,
    I2C1_BASE            = IO_BASE + 0x01D20000,
    I2C2_BASE            = IO_BASE + 0x01D21000,
    I2C4_BASE            = IO_BASE + 0x01D22000,
    IOCFG_BL_BASE        = IO_BASE + 0x01D30000,
    IOCFG_BR_BASE        = IO_BASE + 0x01D40000,
    I2C5_BASE            = IO_BASE + 0x01E00000,
    IOCFG_LM_BASE        = IO_BASE + 0x01E20000,
    USB3_SIF2_BASE       = IO_BASE + 0x01E40000,
    MIPI_TX_CONFIG_BASE  = IO_BASE + 0x01E50000,
    IOCFG_LB_BASE        = IO_BASE + 0x01E70000,
    IOCFG_RT_BASE        = IO_BASE + 0x01EA0000,
    I2C0_BASE            = IO_BASE + 0x01F00000,
    I2C6_BASE            = IO_BASE + 0x01F01000,
    IOCFG_LT_BASE        = IO_BASE + 0x01F20000,
    IOCFG_TL_BASE        = IO_BASE + 0x01F30000,
    UFS0_MPHY_BASE       = IO_BASE + 0x01FA0000,
    MMSYS_CONFIG_BASE    = IO_BASE + 0x04000000,
    MM_MUTEX_BASE        = IO_BASE + 0x04001000,
    SMI_COMMON_BASE      = IO_BASE + 0x04002000,
    SMI_LARB0_BASE       = IO_BASE + 0x04003000,
    SMI_LARB1_BASE       = IO_BASE + 0x04004000,
    OVL0_BASE            = IO_BASE + 0x04005000,
    DISP_OVL0_2L_BASE    = IO_BASE + 0x04006000,
    DISP_RDMA0_BASE      = IO_BASE + 0x04007000,
    DISP_RSZ0_BASE       = IO_BASE + 0x04008000,
    DISP_COLOR0_BASE     = IO_BASE + 0x04009000,
    DISP_CCORR0_BASE     = IO_BASE + 0x0400a000,
    DISP_AAL0_BASE       = IO_BASE + 0x0400b000,
    DISP_GAMMA0_BASE     = IO_BASE + 0x0400c000,
    DISP_POSTMASK0_BASE  = IO_BASE + 0x0400d000,
    DISP_DITHER0_BASE    = IO_BASE + 0x0400e000,
    DISP_DSC_BASE        = IO_BASE + 0x0400f000,
    DISP_DSI0_BASE       = IO_BASE + 0x04010000,
    MDP_AAL4_BASE        = IO_BASE + 0x0401a000,
    SMI_SUBCOM0_BASE     = IO_BASE + 0x04022000,
    SMI_SUBCOM1_BASE     = IO_BASE + 0x04023000,
    DISP_RDMA1_BASE      = IO_BASE + 0x04103000,
    DISP_DSI1_BASE       = IO_BASE + 0x0410e000,
};
