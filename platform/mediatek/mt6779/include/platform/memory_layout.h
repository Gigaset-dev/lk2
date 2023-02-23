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
#include <platform/addressmap.h>

enum {
    REGION(LK_KERNEL_32, DRAM_BASE_PHY + 0x00008000, 0x07C00000),
    REGION(LK_KERNEL_64, DRAM_BASE_PHY + 0x00080000, 0x07C00000),
    REGION(LK_RAMDISK,   DRAM_BASE_PHY + 0x07C80000, 0x04000000),
    REGION(LK_DT,        DRAM_BASE_PHY + 0x0BC80000, 0x00400000),
    REGION(LK_HMAC,      DRAM_BASE_PHY + 0x0C180000, 0x00001000),
    REGION(PL_BOOTTAGS,  DRAM_BASE_PHY + 0x0C500000, 0x00100000),
    REGION(LK_MEM,       DRAM_BASE_PHY + 0x10000000, 0x0A000000),
};
