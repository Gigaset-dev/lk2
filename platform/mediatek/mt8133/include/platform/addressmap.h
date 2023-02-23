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

#define REGION(name, base, size) name##_BASE_PHY = base,                      \
                                 name##_BASE     = KERNEL_ASPACE_BASE + base, \
                                 name##_SIZE     = size

enum {
    REGION(SRAM, 0x00100000, 0x00012000),
    REGION(GIC,  0x0c000000, 0x04000000),
    REGION(IO,   0x10000000, 0x10000000),
    REGION(DRAM, 0x40000000, 0x40000000),
};

enum {
    CKSYS_BASE       = IO_BASE + 0x0,
    INFRACFG_AO_BASE = IO_BASE + 0x00001000,
    KPD_BASE         = IO_BASE + 0x00002000,
    PERICFG_BASE     = IO_BASE + 0x00003000,
    GPIO_BASE        = IO_BASE + 0x00005000,
    RGU_BASE         = IO_BASE + 0x00007000,
    APMIXED_BASE     = IO_BASE + 0x0000c000,
    PWRAP_BASE       = IO_BASE + 0x0000d000,
    MCUCFG_BASE      = IO_BASE + 0x00200000,
    I2C0_APDMA_BASE  = IO_BASE + 0x01000080,
    I2C1_APDMA_BASE  = IO_BASE + 0x01000100,
    I2C2_APDMA_BASE  = IO_BASE + 0x01000180,
    I2C3_APDMA_BASE  = IO_BASE + 0x01000200,
    AUXADC_BASE      = IO_BASE + 0x01001000,
    UART0_BASE       = IO_BASE + 0x01002000,
    UART1_BASE       = IO_BASE + 0x01003000,
    USB3_BASE        = IO_BASE + 0x01200000,
    USB3_IPPC_BASE   = IO_BASE + 0x01203E00,
    USB3_SIF2_BASE   = IO_BASE + 0x01CC0000,
    I2C0_BASE        = IO_BASE + 0x01007000,
    I2C1_BASE        = IO_BASE + 0x01008000,
    I2C2_BASE        = IO_BASE + 0x01009000,
    I2C3_BASE        = IO_BASE + 0x0100F000,
};
