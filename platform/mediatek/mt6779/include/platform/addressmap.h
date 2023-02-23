/*
 * Copyright (c) 2019 MediaTek Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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
    REGION(MCUCFG, 0x0C530000, 0x00010000),
    REGION(IO,     0x10000000, 0x10000000),
    REGION(DRAM,   0x40000000, 0x40000000),
};

enum {
    INFRACFG_AO_BASE   = IO_BASE + 0x00001000,
    PERICFG_BASE       = IO_BASE + 0x00003000,
    GPIO_BASE          = IO_BASE + 0x00005000,
    SPM_BASE           = IO_BASE + 0x00006000,
    RGU_BASE           = IO_BASE + 0x00007000,
    HACC_BASE          = IO_BASE + 0x0000A000,
    APMIXED_BASE       = IO_BASE + 0x0000C000,
    PWRAP_BASE         = IO_BASE + 0x0000D000,
    KPD_BASE           = IO_BASE + 0x00010000,
    DVFSRC_BASE        = IO_BASE + 0x00012000,
    AES_TOP_BASE       = IO_BASE + 0x00016000,
    SSPM_CFGREG_BASE   = IO_BASE + 0x00440000,
    SCP_ADDR_BASE      = IO_BASE + 0x00500000,
    I2C0_APDMA_BASE    = IO_BASE + 0x01000080,
    I2C1_APDMA_BASE    = IO_BASE + 0x01000100,
    I2C2_APDMA_BASE    = IO_BASE + 0x01000180,
    I2C3_APDMA_BASE    = IO_BASE + 0x01000300,
    I2C4_APDMA_BASE    = IO_BASE + 0x01000380,
    I2C5_APDMA_BASE    = IO_BASE + 0x01000500,
    I2C6_APDMA_BASE    = IO_BASE + 0x01000580,
    I2C7_APDMA_BASE    = IO_BASE + 0x01000680,
    I2C8_APDMA_BASE    = IO_BASE + 0x01000780,
    I2C9_APDMA_BASE    = IO_BASE + 0x01000800,
    UART0_BASE         = IO_BASE + 0x01002000,
    UART1_BASE         = IO_BASE + 0x01003000,
    DISP_PWM0_BASE     = IO_BASE + 0x0100E000,
    I2C6_BASE          = IO_BASE + 0x01005000,
    I2C0_BASE          = IO_BASE + 0x01007000,
    I2C1_BASE          = IO_BASE + 0x01008000,
    I2C2_BASE          = IO_BASE + 0x01009000,
    I2C3_BASE          = IO_BASE + 0x0100F000,
    I2C4_BASE          = IO_BASE + 0x01011000,
    I2C9_BASE          = IO_BASE + 0x01015000,
    I2C5_BASE          = IO_BASE + 0x01016000,
    I2C7_BASE          = IO_BASE + 0x0101A000,
    I2C8_BASE          = IO_BASE + 0x0101B000,
    USB3_BASE          = IO_BASE + 0x01200000,
    USB3_IPPC_BASE     = IO_BASE + 0x01203E00,
    UFS0_BASE          = IO_BASE + 0x01270000,
    EFUSE_BASE         = IO_BASE + 0x01C10000,
    IOCFG_RM_BASE      = IO_BASE + 0x01C20000,
    IOCFG_BR_BASE      = IO_BASE + 0x01D10000,
    IOCFG_LM_BASE      = IO_BASE + 0x01E20000,
    USB3_SIF2_BASE     = IO_BASE + 0x01E40000,
    MIPI_TX0_BASE      = IO_BASE + 0x01E50000,
    MIPI_TX1_BASE      = IO_BASE + 0x01E60000,
    IOCFG_LB_BASE      = IO_BASE + 0x01E70000,
    IOCFG_RT_BASE      = IO_BASE + 0x01EA0000,
    IOCFG_LT_BASE      = IO_BASE + 0x01F20000,
    IOCFG_TL_BASE      = IO_BASE + 0x01F30000,
    UFS0_MPHY_BASE     = IO_BASE + 0x01FA0000,
    MMSYS_CONFIG_BASE  = IO_BASE + 0x04000000,
    DISP_OVL0_BASE     = IO_BASE + 0x04008000,
    DISP_OVL0_2L_BASE  = IO_BASE + 0x04009000,
    DISP_OVL1_2L_BASE  = IO_BASE + 0x0400A000,
    DISP_RDMA0_BASE    = IO_BASE + 0x0400B000,
    DISP_RDMA1_BASE    = IO_BASE + 0x0400C000,
    DISP_WDMA0_BASE    = IO_BASE + 0x0400D000,
    DISP_COLOR0_BASE   = IO_BASE + 0x0400E000,
    DISP_CCORR0_BASE   = IO_BASE + 0x0400F000,
    DISP_AAL0_BASE     = IO_BASE + 0x04010000,
    DISP_GAMMA0_BASE   = IO_BASE + 0x04011000,
    DISP_DITHER0_BASE  = IO_BASE + 0x04012000,
    DSI0_BASE          = IO_BASE + 0x04014000,
    DPI_BASE           = IO_BASE + 0x04015000,
    MM_MUTEX_BASE      = IO_BASE + 0x04016000,
    SMI_LARB0_BASE     = IO_BASE + 0x04017000,
    SMI_LARB1_BASE     = IO_BASE + 0x04018000,
    SMI_COMMON_BASE    = IO_BASE + 0x04019000,
    DISP_POSTMASK_BASE = IO_BASE + 0x04021000,
};
