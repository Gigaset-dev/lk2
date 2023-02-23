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

#define UFS_MMIO_PERICFG_BASE   (PERICFG_BASE)
#define UFS_HCI_BASE            (UFS0_BASE)
#define UFS_MPHY_BASE           (UFS0_MPHY_BASE)

enum {
    REG_UFS_PERICFG             = 0x448,
    REG_UFS_PERICFG_RST_N_BIT   = 3,
    REG_UFS_PERICFG_LDO_N_BIT   = 0xF,
    REG_UFS_PERICFG_LP_N_BIT    = 0xF
};

#define TRAP_UFS_FIRST  (1U << 11) /* bit 11 0: UFS, bit 11 1: eMMC */

#define GPIO_BANK_BASE (IO_BASE + 0x000006F0)

