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
    REGION(SRAM,  0x00100000, 0x00030000),
    REGION(HWVER, 0x08000000, 0x00001000),
    REGION(GIC,   0x0C000000, 0x04000000),
    REGION(IO,    0x10000000, 0x10000000),
    REGION(DRAM,  0x40000000, 0x40000000),
};

enum {
    MCUCFG_BASE        = GIC_BASE + 0x00530000,
};

enum {
    INFRACFG_AO_BASE     = IO_BASE + 0x00001000,
    GPIO_BASE            = IO_BASE + 0x00005000,
    SPM_BASE             = IO_BASE + 0x00006000,
    RGU_BASE             = IO_BASE + 0x00007000,
    DVFSRC_BASE          = IO_BASE + 0x00012000,
    MHCCIF_EP_BASE       = IO_BASE + 0x00025000,
    PMIF_SPMI_BASE       = IO_BASE + 0x00026000,
    SPMI_MST_BASE        = IO_BASE + 0x00029000,
    CLDMA0_AO_BASE       = IO_BASE + 0x0004A000,
    I2C0_APDMA_BASE      = IO_BASE + 0x00217080,
    I2C1_APDMA_BASE      = IO_BASE + 0x00217100,
    I2C2_APDMA_BASE      = IO_BASE + 0x00217180,
    I2C3_APDMA_BASE      = IO_BASE + 0x00217200,
    I2C4_APDMA_BASE      = IO_BASE + 0x00217280,
    I2C5_APDMA_BASE      = IO_BASE + 0x00217300,
    CLDMA0_PD_BASE       = IO_BASE + 0x0021E000,
    DDRPHY_AO_CH0_BASE   = IO_BASE + 0x00230000,
    SSPM_SRAM_BASE       = IO_BASE + 0x00400000,
    SSPM_CFGREG_BASE     = IO_BASE + 0x00440000,
    DPM_PM1_SRAM_BASE    = IO_BASE + 0x00900000,
    DPM_DM1_SRAM_BASE    = IO_BASE + 0x00920000,
    DPM_CFG_CH0_BASE     = IO_BASE + 0x00940000,
    AUXADC_BASE          = IO_BASE + 0x01001000,
    UART0_BASE           = IO_BASE + 0x01002000,
    UART1_BASE           = IO_BASE + 0x01003000,
    NFI_BASE             = IO_BASE + 0x01005000,
    NFIECC_BASE          = IO_BASE + 0x01006000,
    USB3_BASE            = IO_BASE + 0x01200000,
    USB3_IPPC_BASE       = IO_BASE + 0x01203E00,
    PCIE0_MAC_BASE       = IO_BASE + 0x01280000,
    IOCFG_RM_BASE        = IO_BASE + 0x01C10000,
    IOCFG_RB_BASE        = IO_BASE + 0x01C20000,
    I2C0_BASE            = IO_BASE + 0x01C40000,
    I2C1_BASE            = IO_BASE + 0x01C41000,
    I2C2_BASE            = IO_BASE + 0x01C42000,
    I2C3_BASE            = IO_BASE + 0x01C43000,
    I2C4_BASE            = IO_BASE + 0x01C44000,
    I2C5_BASE            = IO_BASE + 0x01C45000,
    IOCFG_BL_BASE        = IO_BASE + 0x01D00000,
    IOCFG_BM_BASE        = IO_BASE + 0x01D10000,
    IOCFG_BR_BASE        = IO_BASE + 0x01D20000,
    IOCFG_LT_BASE        = IO_BASE + 0x01E00000,
    USB3_SIF2_BASE       = IO_BASE + 0x01E30000,
    EFUSE_BASE           = IO_BASE + 0x01EC0000,
    IOCFG_TL_BASE        = IO_BASE + 0x01F00000,
};
