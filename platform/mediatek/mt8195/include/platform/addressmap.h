/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once

#define REGION(name, base, size) name##_BASE_PHY = base,                      \
                                 name##_BASE     = KERNEL_ASPACE_BASE + base, \
                                 name##_SIZE     = size

enum {
    REGION(SRAM,    0x00100000, 0x00030000),
    REGION(GIC,     0x0c000000, 0x00400000),
    REGION(MCUCFG,  0x0c530000, 0x00010000),
    REGION(IO,      0x10000000, 0x10000000),
    REGION(DRAM,    0x40000000, 0x40000000),
};

enum {
    CKSYS_BASE       = IO_BASE + 0x0,
    INFRACFG_AO_BASE = IO_BASE + 0x00001000,
    PERICFG_BASE     = IO_BASE + 0x00003000,
    GPIO_BASE        = IO_BASE + 0x00005000,
    SPM_BASE         = IO_BASE + 0x00006000,
    RGU_BASE         = IO_BASE + 0x00007000,
    APMIXED_BASE     = IO_BASE + 0x0000c000,
    KPD_BASE         = IO_BASE + 0x00010000,
    PWRAP_BASE       = IO_BASE + 0x00024000,
    PMICSPI_MST_BASE = IO_BASE + 0x00025000,
    PMIF_SPMI_BASE   = IO_BASE + 0x00027000,
    SPMI_MST_BASE    = IO_BASE + 0x00029000,
    I2C0_APDMA_BASE  = IO_BASE + 0x00220080,
    I2C1_APDMA_BASE  = IO_BASE + 0x00220200,
    I2C2_APDMA_BASE  = IO_BASE + 0x00220380,
    I2C3_APDMA_BASE  = IO_BASE + 0x00220480,
    I2C4_APDMA_BASE  = IO_BASE + 0x00220500,
    I2C5_APDMA_BASE  = IO_BASE + 0x00220580,
    I2C6_APDMA_BASE  = IO_BASE + 0x00220600,
    I2C7_APDMA_BASE  = IO_BASE + 0x00220680,
    DDRPHY_BASE      = IO_BASE + 0x00330000,
    SCPCFG_BASE      = IO_BASE + 0x00700000,
    UART0_BASE       = IO_BASE + 0x01001100,
    UART1_BASE       = IO_BASE + 0x01001200,
    PERICFG_AO_BASE  = IO_BASE + 0x01003000,
    USB3_BASE        = IO_BASE + 0x01200000,
    USB3_IPPC_BASE   = IO_BASE + 0x01203E00,
    MSDC0_BASE       = IO_BASE + 0x01230000,
    MSDC1_BASE       = IO_BASE + 0x01240000,
    MSDC1_TOP_BASE   = IO_BASE + 0x01C70000,
    I2C5_BASE        = IO_BASE + 0x01D00000,
    I2C6_BASE        = IO_BASE + 0x01D01000,
    I2C7_BASE        = IO_BASE + 0x01D02000,
    IOCFG_BM_BASE    = IO_BASE + 0x01D10000,
    IOCFG_BL_BASE    = IO_BASE + 0x01D30000,
    IOCFG_BR_BASE    = IO_BASE + 0x01D40000,
    I2C0_BASE        = IO_BASE + 0x01E00000,
    I2C1_BASE        = IO_BASE + 0x01E01000,
    I2C2_BASE        = IO_BASE + 0x01E02000,
    I2C3_BASE        = IO_BASE + 0x01E03000,
    I2C4_BASE        = IO_BASE + 0x01E04000,
    IOCFG_LM_BASE    = IO_BASE + 0x01E20000,
    USB3_SIF2_BASE   = IO_BASE + 0x01E40000,
    IOCFG_RB_BASE    = IO_BASE + 0x01EB0000,
    IOCFG_TL_BASE    = IO_BASE + 0x01F40000,
    MSDC0_TOP_BASE   = IO_BASE + 0x01F50000,
};
