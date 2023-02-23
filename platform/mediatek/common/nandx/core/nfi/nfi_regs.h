/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#define NFI_CNFG                0x000
#define         CNFG_AHB                nandx_bit(0)
#define         CNFG_READ_EN            nandx_bit(1)
#define         CNFG_DMA_BURST_EN       nandx_bit(2)
#define         CNFG_RESEED_SEC_EN      nandx_bit(4)
#define         CNFG_RAND_SEL           nandx_bit(5)
#define         CNFG_BYTE_RW            nandx_bit(6)
#define         CNFG_HW_ECC_EN          nandx_bit(8)
#define         CNFG_AUTO_FMT_EN        nandx_bit(9)
#define         CNFG_RAND_MASK          GENMASK(5, 4)
#define         CNFG_OP_MODE_MASK       GENMASK(14, 12)
#define         CNFG_IDLE_MOD           0
#define         CNFG_READ_MODE          (1 << 12)
#define         CNFG_SINGLE_READ_MODE   (2 << 12)
#define         CNFG_PROGRAM_MODE       (3 << 12)
#define         CNFG_ERASE_MODE         (4 << 12)
#define         CNFG_RESET_MODE         (5 << 12)
#define         CNFG_CUSTOM_MODE        (6 << 12)
#define NFI_PAGEFMT             0x004
#define         PAGEFMT_SPARE_SHIFT     16
#define         PAGEFMT_FDM_ECC_SHIFT   12
#define         PAGEFMT_FDM_SHIFT       8
#define         PAGEFMT_SEC_SEL_512     nandx_bit(2)
#define         PAGEFMT_512_2K          0
#define         PAGEFMT_2K_4K           1
#define         PAGEFMT_4K_8K           2
#define         PAGEFMT_8K_16K          3
#define NFI_CON                 0x008
#define         CON_FIFO_FLUSH          nandx_bit(0)
#define         CON_NFI_RST             nandx_bit(1)
#define         CON_BRD                 nandx_bit(8)
#define         CON_BWR                 nandx_bit(9)
#define         CON_SEC_SHIFT           12
#define NFI_ACCCON              0x00c
#define NFI_INTR_EN             0x010
#define         INTR_BUSY_RETURN_EN     nandx_bit(4)
#define         INTR_AHB_DONE_EN        nandx_bit(6)
#define         INTR_IRQ_EN             nandx_bit(31)
#define NFI_INTR_STA            0x014
#define NFI_CMD                 0x020
#define NFI_ADDRNOB             0x030
#define         ROW_SHIFT               4
#define NFI_COLADDR             0x034
#define NFI_ROWADDR             0x038
#define NFI_STRDATA             0x040
#define         STAR_EN                 1
#define         STAR_DE                 0
#define NFI_CNRNB               0x044
#define NFI_DATAW               0x050
#define NFI_DATAR               0x054
#define NFI_PIO_DIRDY           0x058
#define         PIO_DI_RDY              1
#define NFI_STA                 0x060
#define         STA_CMD                 nandx_bit(0)
#define         STA_ADDR                nandx_bit(1)
#define         FLASH_MACRO_IDLE        nandx_bit(5)
#define         STA_BUSY                nandx_bit(8)
#define         STA_BUSY2READY          nandx_bit(9)
#define         STA_EMP_PAGE            nandx_bit(12)
#define         NFI_FSM_CUSTDATA        (0xe << 16)
#define         NFI_FSM_MASK            GENMASK(19, 16)
#define         NAND_FSM_MASK           GENMASK(29, 23)
#define NFI_ADDRCNTR            0x070
#define         CNTR_VALID_MASK         GENMASK(16, 0)
#define         CNTR_MASK               GENMASK(16, 12)
#define         ADDRCNTR_SEC_SHIFT      12
#define         ADDRCNTR_SEC(val) \
    (((val) & CNTR_MASK) >> ADDRCNTR_SEC_SHIFT)
#define NFI_STRADDR             0x080
#define NFI_BYTELEN             0x084
#define NFI_CSEL                0x090
#define NFI_IOCON               0x094
#define         BRSTN_MASK              GENMASK(7, 4)
#define         BRSTN_SHIFT             4
#define NFI_FDML(x)             (0x0a0 + (x) * 8)
#define NFI_FDMM(x)             (0x0a4 + (x) * 8)
#define NFI_DEBUG_CON1          0x220
#define         STROBE_MASK             GENMASK(4, 3)
#define         STROBE_SHIFT            3
#define         ECC_CLK_EN              nandx_bit(11)
#define         AUTOC_SRAM_MODE         nandx_bit(12)
#define         BYPASS_MASTER_EN        nandx_bit(15)
#define NFI_MASTER_STA          0x224
#define         MASTER_BUS_BUSY         0x3
#define NFI_SECCUS_SIZE         0x22c
#define         SECCUS_SIZE_EN          nandx_bit(17)
#define         SECCUS_SIZE_MASK        GENMASK(12, 0)
#define         SECCUS_SIZE_SHIFT       0

#define NFI_RANDOM_CNFG         0x238
#define         RAN_ENCODE_EN           nandx_bit(0)
#define         ENCODE_SEED_SHIFT       1
#define         RAN_DECODE_EN           nandx_bit(16)
#define         DECODE_SEED_SHIFT       17
#define         RAN_SEED_MASK           0x7fff
#define NFI_EMPTY_THRESH        0x23c
#define NFI_NAND_TYPE_CNFG      0x240
#define         NAND_TYPE_ASYNC         0
#define         NAND_TYPE_TOGGLE        1
#define         NAND_TYPE_SYNC          2
#define NFI_ACCCON1             0x244
#define NFI_DELAY_CTRL          0x248
#define NFI_TLC_RD_WHR2         0x300
#define         TLC_RD_WHR2_EN          nandx_bit(12)
#define         TLC_RD_WHR2_MASK        GENMASK(11, 0)
#define SNF_SNF_CNFG            0x55c
#define         SPI_MODE_EN             1
#define         SPI_MODE_DIS            0


