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

#define SSPM_TBUF_SUPPORT   1
#define SSPM_COREDUP_RETRY  10

#define SSPM_BACKUP         (SSPM_CFGREG_BASE + 0x24)
#define SSPM_TBUF_WPTR      (SSPM_CFGREG_BASE + 0x4C)
#define SSPM_PC             (SSPM_CFGREG_BASE + 0x64)
#define SSPM_AHB_STATUS     (SSPM_CFGREG_BASE + 0x6C)
#define SSPM_AHB_M0_ADDR    (SSPM_CFGREG_BASE + 0x70)
#define SSPM_AHB_M1_ADDR    (SSPM_CFGREG_BASE + 0x74)
#define SSPM_AHB_M2_ADDR    (SSPM_CFGREG_BASE + 0xBC)
#define SSPM_LASTK_SZ       (SSPM_CFGREG_BASE + 0x114)
#define SSPM_LASTK_ADDR     (SSPM_CFGREG_BASE + 0x118)
#define SSPM_RM_ADDR        (SSPM_CFGREG_BASE + 0x128)
#define SSPM_RM_SZ          (SSPM_CFGREG_BASE + 0x12C)
#define SSPM_DM_ADDR        (SSPM_CFGREG_BASE + 0x130)
#define SSPM_DM_SZ          (SSPM_CFGREG_BASE + 0x134)
#define SSPM_SP             (SSPM_CFGREG_BASE + 0x14C)
#define SSPM_LR             (SSPM_CFGREG_BASE + 0x150)
#define SSPM_TBUFL          (SSPM_CFGREG_BASE + 0x154)
#define SSPM_TBUFH          (SSPM_CFGREG_BASE + 0x158)
#define SSPM_DBG_SEL        (SSPM_CFGREG_BASE + 0x15C)
#define SSPM_AXI_STATUS     (SSPM_CFGREG_BASE + 0x1D8)

#define SSPM_SYSREG_START   (SRAM_BASE_PHY + 0x40000000)
#define SSPM_SYSREG_END     (SSPM_SYSREG_START + SRAM_SIZE)
#define SSPM_DRAM_REGION    0x80000000

#define AEE_LKDUMP_SSPM_COREDUMP_SZ 0x40080 /* 256KB + 128Byte */
#define AEE_LKDUMP_SSPM_DATA_SZ     0x400   /* 1KB */
#define AEE_LKDUMP_SSPM_LAST_LOG_SZ 0x400   /* 1KB */
