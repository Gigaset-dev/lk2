/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once

#include <platform/addressmap.h>

// Use MCUPM's setting for trial run

#define GPUEB_SRAM_BASE                 (GIC_BASE + 0x00540000)
#define GPUEB_REG_BASE                  (GIC_BASE + 0x00560000)
#define GPUEB_REG_ADDR(ofs)             (GPUEB_REG_BASE + ofs)
#define GPUEB_DBG_MODE                  0

#define GPUEB_SRAM_SIZE                 0x20000    // 128 KB
#define GPUEB_GPR_SIZE                  0x00000064
#define SRAM_GPR_SIZE_4B                0x4        // 4 Bytes
#define IPI_MBOX_TOTAL                  8
#define SRAM_RESERVED_20B               0x00000014 // 20 Bytes
#define MBOX_SLOT_SIZE_4B               0x00000004
#define SRAM_SLOT_SIZE_80B              0x00000014 // 0x14 = 20 slots = 20*4Bytes = 80 Bytes
#define PIN_S_SIZE                      SRAM_SLOT_SIZE_80B
#define PIN_R_SIZE                      SRAM_SLOT_SIZE_80B
#define MBOX_TABLE_SIZE                 (PIN_S_SIZE + PIN_R_SIZE)

#define GPR_BASE_ADDR(x)                (GPUEB_SRAM_BASE + \
                                        GPUEB_SRAM_SIZE - \
                                        (IPI_MBOX_TOTAL * MBOX_TABLE_SIZE * MBOX_SLOT_SIZE_4B) - \
                                        GPUEB_GPR_SIZE + \
                                        (x * SRAM_GPR_SIZE_4B))

#define RESERVED_SRAM_SIZE              ((IPI_MBOX_TOTAL * MBOX_TABLE_SIZE * MBOX_SLOT_SIZE_4B) + \
                                        GPUEB_GPR_SIZE + SRAM_RESERVED_20B)

#define GPUEB_INFO                      GPR_BASE_ADDR(0)
#define GPUEB_ABNORMAL_BOOT             GPR_BASE_ADDR(1)
#define PLT_RD_MAGIC                    GPR_BASE_ADDR(3)
#define GPUEB_LASTK_SZ                  GPR_BASE_ADDR(7)
#define GPUEB_LASTK_ADDR                GPR_BASE_ADDR(8)
#define GPUEB_DATA_SZ                   GPR_BASE_ADDR(9)
#define GPUEB_DATA_ADDR                 GPR_BASE_ADDR(10)
#define GPUEB_RM_ADDR                   GPR_BASE_ADDR(12)
#define GPUEB_RM_SZ                     GPR_BASE_ADDR(13)
#define GPUEB_DM_ADDR                   GPR_BASE_ADDR(14)
#define GPUEB_DM_SZ                     GPR_BASE_ADDR(15)
#define GPUEB_LASTK_W_POS               GPR_BASE_ADDR(19)
#define GPUEB_LASTK_R_POS               GPR_BASE_ADDR(20)
#define GPUEB_LAST_PCLRSP               GPR_BASE_ADDR(21)
#define GPUEB_LAST_TBUF                 GPR_BASE_ADDR(22)
#define GPUEB_WARM_BOOT                 GPR_BASE_ADDR(23)

#define GPUEB_CFGREG_SW_RSTN            GPUEB_REG_ADDR(0x0000)
#define GPUEB_CFGREG_DBG_AXI_STATUS     GPUEB_REG_ADDR(0x0044)
#define GPUEB_CFGREG_DBG_AXI_VIO_INFO   GPUEB_REG_ADDR(0x0048)
#define GPUEB_CFGREG_DBG_AXI_VIO_ADDR   GPUEB_REG_ADDR(0x004C)
#define GPUEB_CFGREG_TBUF_WPTR          GPUEB_REG_ADDR(0x00D0)
#define GPUEB_CFGREG_TBUF_ADDR          GPUEB_REG_ADDR(0x00D4)
#define GPUEB_CFGREG_TBUF_RDATA0        GPUEB_REG_ADDR(0x00E0)
#define GPUEB_CFGREG_TBUF_RDATA1        GPUEB_REG_ADDR(0x00E4)
#define GPUEB_CFGREG_TBUF_RDATA2        GPUEB_REG_ADDR(0x00E8)
#define GPUEB_CFGREG_TBUF_RDATA3        GPUEB_REG_ADDR(0x00EC)
#define GPUEB_CFGREG_DBG_MON_PC         GPUEB_REG_ADDR(0x0184)
#define GPUEB_MBOX_IPI_GPUEB            GPUEB_REG_ADDR(0x2000)

#define AEE_LKDUMP_GPUEB_COREDUMP_SZ    0x40080 // 256 KB + 128 Bytes
#define AEE_LKDUMP_GPUEB_DATA_SZ        0x400   // 1 KB
#define AEE_LKDUMP_GPUEB_LAST_LOG_SZ    0x400   // 1 KB

#define MDUMP_L2TCM_SIZE                GPUEB_SRAM_SIZE // L2_TCM
#define MDUMP_REGDUMP_SIZE              0x003f00 // Register backup (max size)
#define MDUMP_TBUF_SIZE                 0x000100
#define MDUMP_DRAM_SIZE                 0x100000 // EE size limit, max to 1MB
#define GPUEB_TBUF_SIZE                 0x80     // 4 bytes PC * 32 PCs

#define INITIAL_MAGIC                   0xdeadbeef

#define GPUEB_MEM_SIZE                  0x00100000  // 1MB
#define GPUEB_MEM_ALIGN                 0x00010000  // 64KB (minimal size for EMI MPU)
#define GPUEB_MEM_LIMIT                 0x80000000

#define PT_MAGIC                        0x58901690
#define PT_ID_GPUEB_DM                  0
#define PT_ID_GPUEB_PM                  1
#define PT_ID_GPUEB_XFILE               99
#define MOD                             "[GPUEBLDR]"


/*
 * Loader size could be found in tinysys link.ld.c
 * "
 *  .loader : {
 *      *boot.o(.text* .rodata*)
 *  } > sram
 *  _loader_end = ABSOLUTE(.);
 *
 *  .sram_region 0x2000:  => This line has specified the sram_region must start from 0x2000
 * "
 */

#define GPUEB_LOADER_SIZE               0x2000
#define GPUEB_TCM_SZ                    0x00020000  // 128KB
#define GPUEB_IMG_OFS                   0x2000
#define GPUEB_RDMP_OFS                  0x20000


