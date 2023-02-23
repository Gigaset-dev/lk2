/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once

#include <platform/addressmap.h>
#include <stdlib.h>
#include <sys/types.h>


#define SSPM_SRAM_BASE       (IO_BASE + 0x00400000) // IO_BASE define in addressmap.h
#define SSPM_CFG_BASE        (IO_BASE + 0x00440000)

/*SSPM_MEM_SIZE Should be large than SSPM_IMG_SZ + SSPM_TCM_SZ + SSPM_CPM_SZ */
#define SSPM_MEM_SIZE       0x00100000  // 1M
#define SSPM_DRAM_ADDR_MAX  0x9FFFFFFF  // max address can SCP remap
#define SSPM_IMG_SZ         0x000C0000  // 768K
#define SSPM_TCM_SZ         0x00028000  // 160K
#define SSPM_CPM_SZ         0x00000000  // 0K
#define SSPM_DDR_SIZE       0x00002000  // 8K
#define SSPM_DDR_ALIGN      0x00040000  // 256K
#define SSPM_MEM_ALIGN      0x00010000  // 64K (minimal size for EMI MPU)

#define SSPM_SW_RSTN        SSPM_CFG_BASE
#define BIT_SW_RSTN         (1 << 0)
#define BIT_POSTWRITE_DIS   (1 << 29)
#define VALUE_SW_RSTN       (BIT_SW_RSTN)

#define SSPM_CFGREG_GPR0    (SSPM_SW_RSTN + 0x20)
#define SSPM_CFGREG_GPR1    (SSPM_SW_RSTN + 0x24)
#define SSPM_DDR_RMAP0      (SSPM_SW_RSTN + 0x94)

#define PT_MAGIC            0x58901690
#define PT_ID_SSPM_DM                0
#define PT_ID_SSPM_PM                1
#define PT_ID_SSPM_CDM               2  // un-used
#define PT_ID_SSPM_CPM               3
#define PT_ID_SSPM_XFILE            99


/* sspm expdb */
#define SSPM_TBUF_SUPPORT   1
#define SSPM_COREDUP_RETRY  10

#define SSPM_BACKUP         (SSPM_CFG_BASE + 0x24)
#define SSPM_TBUF_WPTR      (SSPM_CFG_BASE + 0x4C)
#define SSPM_PC             (SSPM_CFG_BASE + 0x64)
#define SSPM_AHB_STATUS     (SSPM_CFG_BASE + 0x6C)
#define SSPM_AHB_M0_ADDR    (SSPM_CFG_BASE + 0x70)
#define SSPM_AHB_M1_ADDR    (SSPM_CFG_BASE + 0x74)
#define SSPM_AHB_M2_ADDR    (SSPM_CFG_BASE + 0xBC)
#define SSPM_LASTK_SZ       (SSPM_CFG_BASE + 0x114)
#define SSPM_LASTK_ADDR     (SSPM_CFG_BASE + 0x118)
#define SSPM_RM_ADDR        (SSPM_CFG_BASE + 0x128)
#define SSPM_RM_SZ          (SSPM_CFG_BASE + 0x12C)
#define SSPM_DM_ADDR        (SSPM_CFG_BASE + 0x130)
#define SSPM_DM_SZ          (SSPM_CFG_BASE + 0x134)
#define SSPM_SP             (SSPM_CFG_BASE + 0x14C)
#define SSPM_LR             (SSPM_CFG_BASE + 0x150)
#define SSPM_TBUFL          (SSPM_CFG_BASE + 0x154)
#define SSPM_TBUFH          (SSPM_CFG_BASE + 0x158)
#define SSPM_DBG_SEL        (SSPM_CFG_BASE + 0x15C)
#define SSPM_AXI_STATUS     (SSPM_CFG_BASE + 0x1D8)

#define SSPM_SYSREG_START   (SRAM_BASE_PHY + 0x40000000)
#define SSPM_SYSREG_END     (SSPM_SYSREG_START + SRAM_SIZE)
#define SSPM_DRAM_REGION    0x80000000

#define AEE_LKDUMP_SSPM_COREDUMP_SZ 0x40080 /* 256KB + 128Byte */
#define AEE_LKDUMP_SSPM_DATA_SZ     0x400   /* 1KB */
#define AEE_LKDUMP_SSPM_LAST_LOG_SZ 0x400   /* 1KB */


void sspm_emi_protect_enable(void *addr, int first_boot);
void sspm_wdt_set(void);
void sspm_pinmux_set(void);
