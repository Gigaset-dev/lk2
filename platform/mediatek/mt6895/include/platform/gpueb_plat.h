/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once

#include <platform/addressmap.h>

#define GPUEB_SRAM_BASE                 (IO_BASE + 0x03C40000)
#define GPUEB_REG_BASE                  (IO_BASE + 0x03C60000)
#define GPUEB_RPC_BASE                  (IO_BASE + 0x03F90000)
#define GPUEB_PLL_BASE                  (IO_BASE + 0x03FA0000)
#define GPUEB_REG_ADDR(ofs)             (GPUEB_REG_BASE + ofs)
#define GPUEB_DBG_MODE                  0
#define GPUEB_AEE_LK_EXP                1
#define GPUEB_PERI_DUMP_SUPPORT         1
#define GPUEB_SYARAM_DATA_SUPPORT       1
#define GPU_EB_EXT_DATA_DUMP_SUPPORT    1
#if GPUEB_AEE_LK_EXP
#define GPUEB_XFILE_SUPPORT             1
#else
#define GPUEB_XFILE_SUPPORT             0
#endif
#define IGNORE_SRAM_VALID_CHECK         1
#define NEED_MFG1_PWR_CHECK             0
#define GPUEB_SRAM_RESET_BY_HWGRST      1

#define GPUEB_SRAM_SIZE                 0x20000    // 128 KB
#define GPUEB_GPR_SIZE                  0x00000064
#define SRAM_GPR_SIZE_4B                0x4        // 4 Bytes
#define IPI_MBOX_TOTAL                  4
#define SRAM_RESERVED_20B               0x00000014 // 20 Bytes
#define MBOX_SLOT_SIZE_4B               0x00000004
#define SRAM_SLOT_SIZE_80B              0x00000014 // 0x14 = 20 slots = 20*4Bytes = 80 Bytes
#define PIN_S_SIZE                      SRAM_SLOT_SIZE_80B
#define PIN_R_SIZE                      SRAM_SLOT_SIZE_80B
#define MBOX_TABLE_SIZE                 (PIN_S_SIZE + PIN_R_SIZE)
#define GPUEB_CFGREG_DUMP_SIZE          0x200
#define GPUEB_CKCTRL_DUMP_SIZE          0x10
#define GPUEB_DMA0_DUMP_SIZE            0x3C
#define GPUEB_DMA1_DUMP_SIZE            0x3C
#define GPUEB_INTC_DUMP_SIZE            0x60
#define GPUEB_SPM_INTF_DUMP_SIZE        0x18

/* GPU shared DRAM info */
#define GPUEB_DATA_DRAM_OFS             (0x100000)
#define GPUEB_XFILE_DRAM_OFS            (GPUEB_DATA_DRAM_OFS + GPUEB_SRAM_SIZE)

#define GPR_BASE_ADDR(x)                (GPUEB_SRAM_BASE + \
                                        GPUEB_SRAM_SIZE - \
                                        (IPI_MBOX_TOTAL * MBOX_TABLE_SIZE * MBOX_SLOT_SIZE_4B) - \
                                        GPUEB_GPR_SIZE + \
                                        (x * SRAM_GPR_SIZE_4B))

#define RESERVED_SRAM_SIZE              ((IPI_MBOX_TOTAL * MBOX_TABLE_SIZE * MBOX_SLOT_SIZE_4B) + \
                                        GPUEB_GPR_SIZE + SRAM_RESERVED_20B)

#define GPUEB_GPR0                      GPR_BASE_ADDR(0)
#define GPUEB_GPR1                      GPR_BASE_ADDR(1)
#define GPUEB_GPR2                      GPR_BASE_ADDR(2)
#define GPUEB_GPR3                      GPR_BASE_ADDR(3)
#define GPUEB_GPR6                      GPR_BASE_ADDR(6)
#define GPUEB_GPR7                      GPR_BASE_ADDR(7)
#define GPUEB_GPR8                      GPR_BASE_ADDR(8)
#define GPUEB_GPR9                      GPR_BASE_ADDR(9)
#define GPUEB_GPR10                     GPR_BASE_ADDR(10)
#define GPUEB_GPR12                     GPR_BASE_ADDR(12)
#define GPUEB_GPR13                     GPR_BASE_ADDR(13)
#define GPUEB_GPR14                     GPR_BASE_ADDR(14)
#define GPUEB_GPR15                     GPR_BASE_ADDR(15)
#define GPUEB_GPR17                     GPR_BASE_ADDR(17)
#define GPUEB_GPR19                     GPR_BASE_ADDR(19)
#define GPUEB_GPR20                     GPR_BASE_ADDR(20)
#define GPUEB_GPR21                     GPR_BASE_ADDR(21)
#define GPUEB_GPR22                     GPR_BASE_ADDR(22)
#define GPUEB_GPR23                     GPR_BASE_ADDR(23)

#define GPUEB_INFO                      GPUEB_GPR0
#define GPUEB_ABNORMAL_BOOT             GPUEB_GPR1
#define GPUEB_INIT_FOOTPRINT            GPUEB_GPR2
#define PLT_RD_MAGIC                    GPUEB_GPR3
#define GPUMPU_RSV_ADDR                 GPUEB_GPR6
#define GPUEB_LASTK_SZ                  GPUEB_GPR7
#define GPUEB_LASTK_ADDR                GPUEB_GPR8
#define GPUEB_DATA_SZ                   GPUEB_GPR9
#define GPUEB_DATA_ADDR                 GPUEB_GPR10
#define GPUEB_RM_ADDR                   GPUEB_GPR12
#define GPUEB_RM_SZ                     GPUEB_GPR13
#define GPUEB_DM_ADDR                   GPUEB_GPR14
#define GPUEB_DM_SZ                     GPUEB_GPR15
#define GPUFREQ_FOOTPRINT_GPR           GPUEB_GPR17
#define GPUEB_LASTK_W_POS               GPUEB_GPR19
#define GPUEB_LASTK_R_POS               GPUEB_GPR20
#define GPUEB_LAST_PCLRSP               GPUEB_GPR21
#define GPUEB_LAST_TBUF                 GPUEB_GPR22
#define GPUEB_WARM_BOOT                 GPUEB_GPR23

#define GPUEB_CKCTRL_BASE               GPUEB_REG_ADDR(0x0200)
#define GPUEB_INTC_BASE                 GPUEB_REG_ADDR(0x0300)
#define GPUEB_DMA_BASE                  GPUEB_REG_ADDR(0x0400)
#define GPUEB_CFGREG_BASE               GPUEB_REG_ADDR(0x0600)
#define GPUEB_CFGREG_DBG_AXI_STATUS     GPUEB_REG_ADDR(0x0644)
#define GPUEB_CFGREG_DBG_AXI_VIO_INFO   GPUEB_REG_ADDR(0x0648)
#define GPUEB_CFGREG_DBG_AXI_VIO_ADDR   GPUEB_REG_ADDR(0x064C)
#define GPUEB_CFGREG_TBUF_WPTR          GPUEB_REG_ADDR(0x06D0)
#define GPUEB_CFGREG_TBUF_ADDR          GPUEB_REG_ADDR(0x06D4)
#define GPUEB_CFGREG_TBUF_RDATA0        GPUEB_REG_ADDR(0x06E0)
#define GPUEB_CFGREG_TBUF_RDATA1        GPUEB_REG_ADDR(0x06E4)
#define GPUEB_CFGREG_TBUF_RDATA2        GPUEB_REG_ADDR(0x06E8)
#define GPUEB_CFGREG_TBUF_RDATA3        GPUEB_REG_ADDR(0x06EC)
#define GPUEB_CFGREG_DBG_MON_PC         GPUEB_REG_ADDR(0x0784)
#define GPUEB_PMIC_INTF                 GPUEB_REG_ADDR(0x0800)
#define GPUEB_SPM_INTF                  GPUEB_REG_ADDR(0x0800)
#define GPUEB_MBOX_IPI_GPUEB            GPUEB_REG_ADDR(0x2000)

#define GPUEB_CFGREG_SW_RSTN                GPUEB_CFGREG_BASE
#define GPUEB_CFGREG_SW_RSTN_SW_RSTN        (0x1 << 0)
#define GPUEB_CFGREG_SW_RSTN_DMA_BUSY_MASK  (0x1 << 1)

#define GPUEB_DMA0_BASE                 GPUEB_DMA_BASE
#define GPUEB_DMA1_BASE                 (GPUEB_DMA_BASE + 0x0100)

#define GPUEB_PMIC_SW_INTF              (GPUEB_PMIC_INTF + 0x100)
#define GPUEB_PMIC_CMD_DAT              (GPUEB_PMIC_INTF + 0x104)
#define GPUEB_PMIC_CMD_STATE            (GPUEB_PMIC_INTF + 0x108)
#define GPUEB_PMIC_READ_CMD_DAT         (GPUEB_PMIC_INTF + 0x110)
#define GPUEB_PMIC_READ_CMD_DAT_BACK    (GPUEB_PMIC_INTF + 0x114)

#define GPUEB_MBOX_IPI_GPUEB_STA        (GPUEB_MBOX_IPI_GPUEB + 0x0000)
#define GPUEB_MBOX_IPI_GPUEB_SET        (GPUEB_MBOX_IPI_GPUEB + 0x0004)
#define GPUEB_MBOX_IPI_GPUEB_CLR        (GPUEB_MBOX_IPI_GPUEB + 0x0008)
#define GPUEB_MBOX_SW_INT_STA           (GPUEB_MBOX_IPI_GPUEB + 0x0078)
#define GPUEB_MBOX_SW_INT_GPUEB_SET     (GPUEB_MBOX_IPI_GPUEB + 0x0070)
#define GPUEB_MBOX_SW_INT_GPUEB_CLR     (GPUEB_MBOX_IPI_GPUEB + 0x0074)

#define MFG_RPC_GPUEB_TURBO                 (GPUEB_RPC_BASE + 0x102C)
#define MFG_RPC_GPUEB_CFG                   (GPUEB_RPC_BASE + 0x1030)
#define MFG_RPC_AO_CLK_CFG                  (GPUEB_RPC_BASE + 0x1034)
#define MFG_RPC_INTF_PWR_RDY_REG            (GPUEB_RPC_BASE + 0x0040)
#define MFG_GPU_EB_MBIST_TCM_DELSEL         (GPUEB_RPC_BASE + 0x1060)
#define MFG_GPU_EB_MBIST_BTB_DELSEL         (GPUEB_RPC_BASE + 0x1064)
#define MFG_GPU_EB_SPM_RPC_SLP_PROT_EN_SET  (GPUEB_RPC_BASE + 0x1040)
#define MFG_GPU_EB_SPM_RPC_SLP_PROT_EN_CLR  (GPUEB_RPC_BASE + 0x1044)
#define MFG_GPU_EB_SPM_RPC_SLP_PROT_EN_STA  (GPUEB_RPC_BASE + 0x1048)
#define MFG_GPU_EB_CLONED_SC_MFG1_PWR_CON   (GPUEB_RPC_BASE + 0x1070)
#define MFG_PTP_BRISKET_PWR_CON             (GPUEB_RPC_BASE + 0x1080)

#define GPUEBPLL_PLL1U_PLL1_CON1            (GPUEB_PLL_BASE + 0x080C)

#define GPUEB_PLL                       GPUEBPLL_PLL1U_PLL1_CON1
#define GPUEB_MUX                       MFG_RPC_AO_CLK_CFG

#define GPUEB_RSV_SYSRAM_BASE           (SRAM_BASE + 0x11000) // 0x00111000
#define GPUEB_RSV_SYSRAM_GPUEB_OFS      (0xC00)
#define GPUEB_RSV_SYSRAM_EB_DATA_BASE   (GPUEB_RSV_SYSRAM_BASE + GPUEB_RSV_SYSRAM_GPUEB_OFS)

#define PWR_STATUS_OFS                  (0xF3C)
#define SPM_XPU_PWR_STATUS              (SPM_BASE + PWR_STATUS_OFS)
#define SPM_XPU_PWR_STATUS_MFG0         (0x1 << 1)

#define MFG1_PWR_CON_OFS                (0xEBC)
#define SPM_MFG1_PWR_CON                (SPM_BASE + MFG1_PWR_CON_OFS)
#define MFG1_POWERED_ON                 (0xC000000D)

#define GPUEB_BOOT_UP_COMPLETE          0x55667788

#define AEE_LKDUMP_GPUEB_COREDUMP_SZ    (0x40080)  // 256 KB + 128 Bytes
#define AEE_LKDUMP_GPUEB_LAST_LOG_SZ    (0x1000)   // 4 KB
#define AEE_LKDUMP_GPUEB_SYSRAM_DATA_SZ (0x1000)   // 4 KB
#define AEE_LKDUMP_GPUFREQ_DATA_SZ      (0x400)    // 1KB
#define AEE_LKDUMP_GPU_EB_EXT_DATA_SZ   (AEE_LKDUMP_GPUEB_SYSRAM_DATA_SZ + \
                                        AEE_LKDUMP_GPUFREQ_DATA_SZ)
#define AEE_LKDUMP_GPUEB_XFILE_SZ       0x100000   // 1MB

#define MDUMP_L2TCM_SIZE                GPUEB_SRAM_SIZE // L2_TCM
#define MDUMP_REGDUMP_SIZE              0x000400 // Register backup (max size)
#define MDUMP_TBUF_SIZE                 0x000100
#define MDUMP_DRAM_SIZE                 0x100000 // EE size limit, max to 1MB
#define GPUEB_TBUF_SIZE                 0x80     // 4 bytes PC * 32 PCs
#define GPUEB_GPUFREQ_RERV_SIZE         0x400

#define INITIAL_MAGIC                   0xdeadbeef

#define REGION_ID_GPU_MPU_TAB           12
#define REGION_ID_GPU_EB_RESERVE        31

#define GPUEB_MEM_ALIGN                 0x00010000  // 64KB (minimal size for EMI MPU)
#define GPUEB_MEM_LIMIT                 0x80000000
#define GPUEB_MEM_LOWER_BOUND           0x240000000 // 9GB
#define GPUEB_MEM_UPPER_BOUND           0x340000000 // 13GB
#define GPUEB_FW_MEM_SIZE               0x00100000  // 1MB
#define GPUEB_SHARED_MEM_SIZE           0x00200000  // 2MB
#define GPUMPU_MEM_SIZE                 0x0         // 4MB -> 0MB, need to align DTS/GPUEB setting
#define GPU_FW_PRV_HEAP                 0x00400000  // 16MB -> 4MB, need to align DTS/GPUEB setting

#define PT_MAGIC                        0x58901690
#define PT_ID_GPUEB_FW                  1
#define PT_ID_GPUEB_XFILE               99          // 0x63
#define MOD                             "[GPUEB][LDR]"
#define EXPMOD                          "[GPUEB][EXPDB]"

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

#define GPUEB_COPY_SIZE                 (GPUEB_SRAM_SIZE - RESERVED_SRAM_SIZE)
#define GPUEB_LOADER_SIZE               0x2000
#define GPUEB_IMG_OFS                   0x2000
#define GPUEB_RDMP_OFS                  0x20000
#define GPUEB_SYSRAM_DATA_SZ            AEE_LKDUMP_GPUEB_SYSRAM_DATA_SZ
