/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <mmc_core.h>
#include <msdc_cfg.h>
#include <platform/addressmap.h>
#include <reg.h>

/*--------------------------------------------------------------------------*/
/* Common Macro                                                             */
/*--------------------------------------------------------------------------*/
#define REG_ADDR(x)             ((uint32_t *)(uintptr_t)(base + OFFSET_##x))

/*--------------------------------------------------------------------------*/
/* Common Definition                                                        */
/*--------------------------------------------------------------------------*/
#define MSDC_FIFO_SZ            (128)
#define MSDC_FIFO_THD           (128)
//#define MSDC_MAX_NUM            (2)

#define MSDC_MS                 (0)
#define MSDC_SDMMC              (1)

#define MSDC_MODE_UNKNOWN       (0)
#define MSDC_MODE_PIO           (1)
#define MSDC_MODE_DMA_BASIC     (2)
#define MSDC_MODE_DMA_DESC      (3)
#define MSDC_MODE_DMA_ENHANCED  (4)
#define MSDC_MODE_MMC_STREAM    (5)

#define MSDC_BUS_1BITS          (0)
#define MSDC_BUS_4BITS          (1)
#define MSDC_BUS_8BITS          (2)

#define MSDC_BURST_8B           (3)
#define MSDC_BURST_16B          (4)
#define MSDC_BURST_32B          (5)
#define MSDC_BURST_64B          (6)

#define MSDC_PIN_PULL_NONE      (0)
#define MSDC_PIN_PULL_DOWN      (1)
#define MSDC_PIN_PULL_UP        (2)
#define MSDC_PIN_KEEP           (3)

#define MSDC_350K_SCLK          (350000)
#define MSDC_400K_SCLK          (400000)
#define MSDC_25M_SCLK           (25000000)
#define MSDC_26M_SCLK           (26000000)
#define MSDC_50M_SCLK           (50000000)
#define MSDC_52M_SCLK           (52000000)
#define MSDC_100M_SCLK          (100000000)
#define MSDC_179M_SCLK          (179000000)
#define MSDC_200M_SCLK          (200000000)
#define MSDC_208M_SCLK          (208000000)
#define MSDC_400M_SCLK          (400000000)
#define MSDC_800M_SCLK          (800000000)

#define MSDC_AUTOCMD12          (0x0001)
#define MSDC_AUTOCMD23          (0x0002)
#define MSDC_AUTOCMD19          (0x0003)

#define TYPE_CMD_RESP_EDGE      (0)
#define TYPE_WRITE_CRC_EDGE     (1)
#define TYPE_READ_DATA_EDGE     (2)
#define TYPE_WRITE_DATA_EDGE    (3)

#define START_AT_RISING                 (0x0)
#define START_AT_FALLING                (0x1)
#define START_AT_RISING_AND_FALLING     (0x2)
#define START_AT_RISING_OR_FALLING      (0x3)

#define MSDC_DMA_BURST_8B       (3)
#define MSDC_DMA_BURST_16B      (4)
#define MSDC_DMA_BURST_32B      (5)
#define MSDC_DMA_BURST_64B      (6)

/*--------------------------------------------------------------------------*/
/* Register Offset                                                          */
/*--------------------------------------------------------------------------*/
#define OFFSET_MSDC_CFG                  (0x00)
#define OFFSET_MSDC_IOCON                (0x04)
#define OFFSET_MSDC_PS                   (0x08)
#define OFFSET_MSDC_INT                  (0x0c)
#define OFFSET_MSDC_INTEN                (0x10)
#define OFFSET_MSDC_FIFOCS               (0x14)
#define OFFSET_MSDC_TXDATA               (0x18)
#define OFFSET_MSDC_RXDATA               (0x1c)
#define OFFSET_SDC_CFG                   (0x30)
#define OFFSET_SDC_CMD                   (0x34)
#define OFFSET_SDC_ARG                   (0x38)
#define OFFSET_SDC_STS                   (0x3c)
#define OFFSET_SDC_RESP0                 (0x40)
#define OFFSET_SDC_RESP1                 (0x44)
#define OFFSET_SDC_RESP2                 (0x48)
#define OFFSET_SDC_RESP3                 (0x4c)
#define OFFSET_SDC_BLK_NUM               (0x50)
#define OFFSET_SDC_VOL_CHG               (0x54)
#define OFFSET_SDC_CSTS                  (0x58)
#define OFFSET_SDC_CSTS_EN               (0x5c)
#define OFFSET_SDC_DCRC_STS              (0x60)
#define OFFSET_SDC_ADV_CFG0              (0x64)
/* Only for EMMC Controller 4 registers below */
#define OFFSET_EMMC_CFG0                 (0x70)
#define OFFSET_EMMC_CFG1                 (0x74)
#define OFFSET_EMMC_STS                  (0x78)
#define OFFSET_EMMC_IOCON                (0x7c)
#define OFFSET_SDC_ACMD_RESP             (0x80)
#define OFFSET_SDC_ACMD19_TRG            (0x84)
#define OFFSET_SDC_ACMD19_STS            (0x88)
#define OFFSET_MSDC_DMA_SA_HIGH          (0x8C)
#define OFFSET_MSDC_DMA_SA               (0x90)
#define OFFSET_MSDC_DMA_CA               (0x94)
#define OFFSET_MSDC_DMA_CTRL             (0x98)
#define OFFSET_MSDC_DMA_CFG              (0x9c)
#define OFFSET_MSDC_DBG_SEL              (0xa0)
#define OFFSET_MSDC_DBG_OUT              (0xa4)
#define OFFSET_MSDC_DMA_LEN              (0xa8)
#define OFFSET_MSDC_PATCH_BIT0           (0xb0)
#define OFFSET_MSDC_PATCH_BIT1           (0xb4)
#define OFFSET_MSDC_PATCH_BIT2           (0xb8)
/* Only for SD/SDIO Controller 6 registers below */
#define OFFSET_DAT0_TUNE_CRC             (0xc0)
#define OFFSET_DAT1_TUNE_CRC             (0xc4)
#define OFFSET_DAT2_TUNE_CRC             (0xc8)
#define OFFSET_DAT3_TUNE_CRC             (0xcc)
#define OFFSET_CMD_TUNE_CRC              (0xd0)
#define OFFSET_SDIO_TUNE_WIND            (0xd4)
#define OFFSET_MSDC_PAD_TUNE0            (0xf0)
#define OFFSET_MSDC_PAD_TUNE1            (0xf4)
#define OFFSET_MSDC_DAT_RDDLY0           (0xf8)
#define OFFSET_MSDC_DAT_RDDLY1           (0xfc)
#define OFFSET_MSDC_DAT_RDDLY2           (0x100)
#define OFFSET_MSDC_DAT_RDDLY3           (0x104)
#define OFFSET_MSDC_HW_DBG               (0x110)
#define OFFSET_MSDC_VERSION              (0x114)
#define OFFSET_MSDC_ECO_VER              (0x118)
/* Only for EMMC 5.0 Controller 4 registers below */
#define OFFSET_EMMC50_PAD_CTL0           (0x180)
#define OFFSET_EMMC50_PAD_DS_CTL0        (0x184)
#define OFFSET_EMMC50_PAD_DS_TUNE        (0x188)
#define OFFSET_EMMC50_PAD_CMD_TUNE       (0x18c)
#define OFFSET_EMMC50_PAD_DAT01_TUNE     (0x190)
#define OFFSET_EMMC50_PAD_DAT23_TUNE     (0x194)
#define OFFSET_EMMC50_PAD_DAT45_TUNE     (0x198)
#define OFFSET_EMMC50_PAD_DAT67_TUNE     (0x19c)
#define OFFSET_EMMC51_CFG0               (0x204)
#define OFFSET_EMMC50_CFG0               (0x208)
#define OFFSET_EMMC50_CFG1               (0x20c)
#define OFFSET_EMMC50_CFG2               (0x21c)
#define OFFSET_EMMC50_CFG3               (0x220)
#define OFFSET_EMMC50_CFG4               (0x224)
#define OFFSET_SDC_FIFO_CFG              (0x228)
/*--------------------------------------------------------------------------*/
/* Register Address                                                         */
/*--------------------------------------------------------------------------*/
/* common register */
#define MSDC_CFG                         REG_ADDR(MSDC_CFG)
#define MSDC_IOCON                       REG_ADDR(MSDC_IOCON)
#define MSDC_PS                          REG_ADDR(MSDC_PS)
#define MSDC_INT                         REG_ADDR(MSDC_INT)
#define MSDC_INTEN                       REG_ADDR(MSDC_INTEN)
#define MSDC_FIFOCS                      REG_ADDR(MSDC_FIFOCS)
#define MSDC_TXDATA                      REG_ADDR(MSDC_TXDATA)
#define MSDC_RXDATA                      REG_ADDR(MSDC_RXDATA)

/* sdmmc register */
#define SDC_CFG                          REG_ADDR(SDC_CFG)
#define SDC_CMD                          REG_ADDR(SDC_CMD)
#define SDC_ARG                          REG_ADDR(SDC_ARG)
#define SDC_STS                          REG_ADDR(SDC_STS)
#define SDC_RESP0                        REG_ADDR(SDC_RESP0)
#define SDC_RESP1                        REG_ADDR(SDC_RESP1)
#define SDC_RESP2                        REG_ADDR(SDC_RESP2)
#define SDC_RESP3                        REG_ADDR(SDC_RESP3)
#define SDC_BLK_NUM                      REG_ADDR(SDC_BLK_NUM)
#define SDC_VOL_CHG                      REG_ADDR(SDC_VOL_CHG)
#define SDC_CSTS                         REG_ADDR(SDC_CSTS)
#define SDC_CSTS_EN                      REG_ADDR(SDC_CSTS_EN)
#define SDC_DCRC_STS                     REG_ADDR(SDC_DCRC_STS)
#define SDC_ADV_CFG0                     REG_ADDR(SDC_ADV_CFG0)

/* emmc register*/
#define EMMC_CFG0                        REG_ADDR(EMMC_CFG0)
#define EMMC_CFG1                        REG_ADDR(EMMC_CFG1)
#define EMMC_STS                         REG_ADDR(EMMC_STS)
#define EMMC_IOCON                       REG_ADDR(EMMC_IOCON)

/* auto command register */
#define SDC_ACMD_RESP                    REG_ADDR(SDC_ACMD_RESP)
#define SDC_ACMD19_TRG                   REG_ADDR(SDC_ACMD19_TRG)
#define SDC_ACMD19_STS                   REG_ADDR(SDC_ACMD19_STS)

/* dma register */
#define MSDC_DMA_SA_HIGH                 REG_ADDR(MSDC_DMA_SA_HIGH)
#define MSDC_DMA_SA                      REG_ADDR(MSDC_DMA_SA)
#define MSDC_DMA_CA                      REG_ADDR(MSDC_DMA_CA)
#define MSDC_DMA_CTRL                    REG_ADDR(MSDC_DMA_CTRL)
#define MSDC_DMA_CFG                     REG_ADDR(MSDC_DMA_CFG)
#define MSDC_DMA_LEN                     REG_ADDR(MSDC_DMA_LEN)

/* data read delay */
#define MSDC_DAT_RDDLY0                  REG_ADDR(MSDC_DAT_RDDLY0)
#define MSDC_DAT_RDDLY1                  REG_ADDR(MSDC_DAT_RDDLY1)
#define MSDC_DAT_RDDLY2                  REG_ADDR(MSDC_DAT_RDDLY2)
#define MSDC_DAT_RDDLY3                  REG_ADDR(MSDC_DAT_RDDLY3)

/* debug register */
#define MSDC_DBG_SEL                     REG_ADDR(MSDC_DBG_SEL)
#define MSDC_DBG_OUT                     REG_ADDR(MSDC_DBG_OUT)

/* misc register */
#define MSDC_PATCH_BIT0                  REG_ADDR(MSDC_PATCH_BIT0)
#define MSDC_PATCH_BIT1                  REG_ADDR(MSDC_PATCH_BIT1)
#define MSDC_PATCH_BIT2                  REG_ADDR(MSDC_PATCH_BIT2)
#define DAT0_TUNE_CRC                    REG_ADDR(DAT0_TUNE_CRC)
#define DAT1_TUNE_CRC                    REG_ADDR(DAT1_TUNE_CRC)
#define DAT2_TUNE_CRC                    REG_ADDR(DAT2_TUNE_CRC)
#define DAT3_TUNE_CRC                    REG_ADDR(DAT3_TUNE_CRC)
#define CMD_TUNE_CRC                     REG_ADDR(CMD_TUNE_CRC)
#define SDIO_TUNE_WIND                   REG_ADDR(SDIO_TUNE_WIND)
#define MSDC_PAD_TUNE0                   REG_ADDR(MSDC_PAD_TUNE0)
#define MSDC_PAD_TUNE1                   REG_ADDR(MSDC_PAD_TUNE1)
#define MSDC_HW_DBG                      REG_ADDR(MSDC_HW_DBG)
#define MSDC_VERSION                     REG_ADDR(MSDC_VERSION)
#define MSDC_ECO_VER                     REG_ADDR(MSDC_ECO_VER)
/* eMMC 5.0 register */
#define EMMC50_PAD_CTL0                  REG_ADDR(EMMC50_PAD_CTL0)
#define EMMC50_PAD_DS_CTL0               REG_ADDR(EMMC50_PAD_DS_CTL0)
#define EMMC50_PAD_DS_TUNE               REG_ADDR(EMMC50_PAD_DS_TUNE)
#define EMMC50_PAD_CMD_TUNE              REG_ADDR(EMMC50_PAD_CMD_TUNE)
#define EMMC50_PAD_DAT01_TUNE            REG_ADDR(EMMC50_PAD_DAT01_TUNE)
#define EMMC50_PAD_DAT23_TUNE            REG_ADDR(EMMC50_PAD_DAT23_TUNE)
#define EMMC50_PAD_DAT45_TUNE            REG_ADDR(EMMC50_PAD_DAT45_TUNE)
#define EMMC50_PAD_DAT67_TUNE            REG_ADDR(EMMC50_PAD_DAT67_TUNE)
#define EMMC51_CFG0                      REG_ADDR(EMMC51_CFG0)
#define EMMC50_CFG0                      REG_ADDR(EMMC50_CFG0)
#define EMMC50_CFG1                      REG_ADDR(EMMC50_CFG1)
#define EMMC50_CFG2                      REG_ADDR(EMMC50_CFG2)
#define EMMC50_CFG3                      REG_ADDR(EMMC50_CFG3)
#define EMMC50_CFG4                      REG_ADDR(EMMC50_CFG4)
#define SDC_FIFO_CFG                     REG_ADDR(SDC_FIFO_CFG)
#define MSDC_AES_SEL                     REG_ADDR(MSDC_AES_SEL)

/* HW CMDQ registger */
#define MTKCQ_CFG0                       REG_ADDR(MTKCQ_CFG0)
#define MTKCQ_CFG1                       REG_ADDR(MTKCQ_CFG1)
#define MTKCQ_CFG2                       REG_ADDR(MTKCQ_CFG2)
#define MTKCQ_ERR_ST                     REG_ADDR(MTKCQ_ERR_ST)
#define MTKCQ_CMD45_READY                REG_ADDR(MTKCQ_CMD45_READY)
#define MTKCQ_TASK_READY_ST              REG_ADDR(MTKCQ_TASK_READY_ST)
#define MTKCQ_TASK_DONE_ST               REG_ADDR(MTKCQ_TASK_DONE_ST)
#define MTKCQ_ERR_ST_CLR                 REG_ADDR(MTKCQ_ERR_ST_CLR)
#define MTKCQ_CMD_DONE_CLR               REG_ADDR(MTKCQ_CMD_DONE_CLR)
#define MTKCQ_SW_CTL_CQ                  REG_ADDR(MTKCQ_SW_CTL_CQ)
#define MTKCQ_CMD44_RESP                 REG_ADDR(MTKCQ_CMD44_RESP)
#define MTKCQ_CMD45_RESP                 REG_ADDR(MTKCQ_CMD45_RESP)
#define MTKCQ_CMD13_RCA                  REG_ADDR(MTKCQ_CMD13_RCA)
#define EMMC51_CQCB_CFG3                 REG_ADDR(EMMC51_CQCB_CFG3)
#define EMMC51_CQCB_CMD44                REG_ADDR(EMMC51_CQCB_CMD44)
#define EMMC51_CQCB_CMD45                REG_ADDR(EMMC51_CQCB_CMD45)
#define EMMC51_CQCB_TIDMAP               REG_ADDR(EMMC51_CQCB_TIDMAP)
#define EMMC51_CQCB_TIDMAPCLR            REG_ADDR(EMMC51_CQCB_TIDMAPCLR)
#define EMMC51_CQCB_CURCMD               REG_ADDR(EMMC51_CQCB_CURCMD)

/*--------------------------------------------------------------------------*/
/* Register Mask                                                            */
/*--------------------------------------------------------------------------*/

/* MSDC_CFG mask */
#define MSDC_CFG_MODE                   (0x1  << 0)     /* RW */
#define MSDC_CFG_CKPDN                  (0x1  << 1)     /* RW */
#define MSDC_CFG_RST                    (0x1  << 2)     /* A0 */
#define MSDC_CFG_PIO                    (0x1  << 3)     /* RW */
#define MSDC_CFG_CKDRVEN                (0x1  << 4)     /* RW */
#define MSDC_CFG_BV18SDT                (0x1  << 5)     /* RW */
#define MSDC_CFG_BV18PSS                (0x1  << 6)     /* R  */
#define MSDC_CFG_CKSTB                  (0x1  << 7)     /* R  */
#define MSDC_CFG_CKDIV                  (0xfff << 8)    /* RW */
#define MSDC_CFG_CKDIV_BITS             (12)
#define MSDC_CFG_CKMOD                  (0x3  << 20)    /* W1C */
#define MSDC_CFG_CKMOD_BITS             (2)
#define MSDC_CFG_CKMOD_HS400            (0x1  << 22)    /* RW */
#define MSDC_CFG_START_BIT              (0x3  << 23)    /* RW */
#define MSDC_CFG_SCLK_STOP_DDR          (0x1  << 25)    /* RW */
#define MSDC_CFG_DVFS_EN                (0x1  << 30)    /* RW */

/* MSDC_IOCON mask */
#define MSDC_IOCON_SDR104CKS            (0x1  << 0)     /* RW */
#define MSDC_IOCON_RSPL                 (0x1  << 1)     /* RW */
#define MSDC_IOCON_R_D_SMPL             (0x1  << 2)     /* RW */
#define MSDC_IOCON_DDLSEL               (0x1  << 3)     /* RW */
#define MSDC_IOCON_DDR50CKD             (0x1  << 4)     /* RW */
#define MSDC_IOCON_R_D_SMPL_SEL         (0x1  << 5)     /* RW */
#define MSDC_IOCON_W_D_SMPL             (0x1  << 8)     /* RW */
#define MSDC_IOCON_W_D_SMPL_SEL         (0x1  << 9)     /* RW */
#define MSDC_IOCON_W_D0SPL              (0x1  << 10)    /* RW */
#define MSDC_IOCON_W_D1SPL              (0x1  << 11)    /* RW */
#define MSDC_IOCON_W_D2SPL              (0x1  << 12)    /* RW */
#define MSDC_IOCON_W_D3SPL              (0x1  << 13)    /* RW */
#define MSDC_IOCON_R_D0SPL              (0x1  << 16)    /* RW */
#define MSDC_IOCON_R_D1SPL              (0x1  << 17)    /* RW */
#define MSDC_IOCON_R_D2SPL              (0x1  << 18)    /* RW */
#define MSDC_IOCON_R_D3SPL              (0x1  << 19)    /* RW */
#define MSDC_IOCON_R_D4SPL              (0x1  << 20)    /* RW */
#define MSDC_IOCON_R_D5SPL              (0x1  << 21)    /* RW */
#define MSDC_IOCON_R_D6SPL              (0x1  << 22)    /* RW */
#define MSDC_IOCON_R_D7SPL              (0x1  << 23)    /* RW */

/* MSDC_PS mask */
#define MSDC_PS_CDEN                    (0x1  << 0)     /* RW */
#define MSDC_PS_CDSTS                   (0x1  << 1)     /* R  */
#define MSDC_PS_CDDEBOUNCE              (0xf  << 12)    /* RW */
#define MSDC_PS_DAT                     (0xff << 16)    /* R  */
#define MSDC_PS_DAT8PIN                 (0xFF << 16)    /* RU */
#define MSDC_PS_DAT4PIN                 (0xF  << 16)    /* RU */
#define MSDC_PS_DAT0                    (0x1  << 16)    /* RU */

#define MSDC_PS_CMD                     (0x1  << 24)    /* RU */

#define MSDC_PS_WP                      (0x1  << 31)    /* RU  */

/* MSDC_INT mask */
#define MSDC_INT_MMCIRQ                 (0x1  <<  0)    /* W1C */
#define MSDC_INT_CDSC                   (0x1  <<  1)    /* W1C */

#define MSDC_INT_ACMDRDY        (0x1   <<  3)     /* W1C */
#define MSDC_INT_ACMDTMO        (0x1   <<  4)     /* W1C */
#define MSDC_INT_ACMDCRCERR     (0x1   <<  5)     /* W1C */
#define MSDC_INT_DMAQ_EMPTY     (0x1   <<  6)     /* W1C */
#define MSDC_INT_SDIOIRQ        (0x1   <<  7)     /* W1C Only for SD/SDIO */
#define MSDC_INT_CMDRDY         (0x1   <<  8)     /* W1C */
#define MSDC_INT_CMDTMO         (0x1   <<  9)     /* W1C */
#define MSDC_INT_RSPCRCERR      (0x1   << 10)     /* W1C */
#define MSDC_INT_CSTA           (0x1   << 11)     /* R */
#define MSDC_INT_XFER_COMPL     (0x1   << 12)     /* W1C */
#define MSDC_INT_DXFER_DONE     (0x1   << 13)     /* W1C */
#define MSDC_INT_DATTMO         (0x1   << 14)     /* W1C */
#define MSDC_INT_DATCRCERR      (0x1   << 15)     /* W1C */
#define MSDC_INT_ACMD19_DONE    (0x1   << 16)     /* W1C */
#define MSDC_INT_BDCSERR        (0x1  << 17)      /* W1C */
#define MSDC_INT_GPDCSERR       (0x1  << 18)      /* W1C */
#define MSDC_INT_DMAPRO         (0x1  << 19)      /* W1C */
#define MSDC_INT_GEAR_OUT_BOUND (0x1  << 20)      /* W1C */
#define MSDC_INT_ACMD53_DONE    (0x1  << 21)      /* W1C */
#define MSDC_INT_ACMD53_FAIL    (0x1  << 22)      /* W1C */
#define MSDC_INT_AXI_RESP_ERR   (0x1  << 23)      /* W1C */

/* MSDC_INTEN mask */
#define MSDC_INTEN_MMCIRQ       (0x1   <<  0)     /* RW */
#define MSDC_INTEN_CDSC         (0x1   <<  1)     /* RW */

#define MSDC_INTEN_ACMDRDY      (0x1   <<  3)     /* RW */
#define MSDC_INTEN_ACMDTMO      (0x1   <<  4)     /* RW */
#define MSDC_INTEN_ACMDCRCERR   (0x1   <<  5)     /* RW */
#define MSDC_INTEN_DMAQ_EMPTY   (0x1   <<  6)     /* RW */
#define MSDC_INTEN_SDIOIRQ      (0x1   <<  7)     /* RW Only for SDIO*/
#define MSDC_INTEN_CMDRDY       (0x1   <<  8)     /* RW */
#define MSDC_INTEN_CMDTMO       (0x1   <<  9)     /* RW */
#define MSDC_INTEN_RSPCRCERR    (0x1   << 10)     /* RW */
#define MSDC_INTEN_CSTA         (0x1   << 11)     /* RW */
#define MSDC_INTEN_XFER_COMPL   (0x1   << 12)     /* RW */
#define MSDC_INTEN_DXFER_DONE   (0x1   << 13)     /* RW */
#define MSDC_INTEN_DATTMO       (0x1   << 14)     /* RW */
#define MSDC_INTEN_DATCRCERR    (0x1   << 15)     /* RW */
#define MSDC_INTEN_ACMD19_DONE  (0x1   << 16)     /* RW */
#define MSDC_INTEN_BDCSERR      (0x1   << 17)     /* RW */
#define MSDC_INTEN_GPDCSERR     (0x1   << 18)     /* RW */
#define MSDC_INTEN_DMAPRO       (0x1   << 19)     /* RW */
#define MSDC_INTEN_GOBOUND      (0x1   << 20)     /* RW  Only for SD/SDIO ACMD 53*/
#define MSDC_INTEN_ACMD53_DONE  (0x1   << 21)     /* RW  Only for SD/SDIO ACMD 53*/
#define MSDC_INTEN_ACMD53_FAIL  (0x1   << 22)     /* RW  Only for SD/SDIO ACMD 53*/
#define MSDC_INTEN_AXI_RESP_ERR (0x1   << 23)     /* RW  Only for eMMC 5.0*/

#define MSDC_INTEN_DFT (MSDC_INTEN_MMCIRQ | MSDC_INTEN_CDSC | MSDC_INTEN_ACMDRDY\
                       | MSDC_INTEN_ACMDTMO | MSDC_INTEN_ACMDCRCERR | MSDC_INTEN_DMAQ_EMPTY \
                       | MSDC_INTEN_CMDRDY | MSDC_INTEN_CMDTMO | MSDC_INTEN_RSPCRCERR \
                       | MSDC_INTEN_CSTA | MSDC_INTEN_XFER_COMPL | MSDC_INTEN_DXFER_DONE \
                       | MSDC_INTEN_DATTMO | MSDC_INTEN_DATCRCERR\
                       | MSDC_INTEN_BDCSERR | MSDC_INTEN_ACMD19_DONE | MSDC_INTEN_GPDCSERR)


/* MSDC_FIFOCS mask */
#define MSDC_FIFOCS_RXCNT       (0xFF  <<  0)     /* R */
#define MSDC_FIFOCS_TXCNT       (0xFF  << 16)     /* R */
#define MSDC_FIFOCS_CLR         (0x1   << 31)     /* RW */

/* SDC_CFG mask */
#define SDC_CFG_SDIOINTWKUP     (0x1   <<  0)     /* RW */
#define SDC_CFG_INSWKUP         (0x1   <<  1)     /* RW */
#define SDC_CFG_WRDTOC          (0x1fff  << 2)  /* RW */
#define SDC_CFG_BUSWIDTH        (0x3   << 16)     /* RW */
#define SDC_CFG_SDIO            (0x1   << 19)     /* RW */
#define SDC_CFG_SDIOIDE         (0x1   << 20)     /* RW */
#define SDC_CFG_INTATGAP        (0x1   << 21)     /* RW */
#define SDC_CFG_DTOC            (0xFF  << 24)     /* RW */

/* SDC_CMD mask */
#define SDC_CMD_OPC             (0x3F  <<  0)     /* RW */
#define SDC_CMD_BRK             (0x1   <<  6)     /* RW */
#define SDC_CMD_RSPTYP          (0x7   <<  7)     /* RW */
#define SDC_CMD_DTYP            (0x3   << 11)     /* RW */
#define SDC_CMD_RW              (0x1   << 13)     /* RW */
#define SDC_CMD_STOP            (0x1   << 14)     /* RW */
#define SDC_CMD_GOIRQ           (0x1   << 15)     /* RW */
#define SDC_CMD_BLKLEN          (0xFFF << 16)     /* RW */
#define SDC_CMD_AUTOCMD         (0x3   << 28)     /* RW */
#define SDC_CMD_VOLSWTH         (0x1   << 30)     /* RW */
#define SDC_CMD_ACMD53          (0x1   << 31)     /* RW Only for SD/SDIO ACMD 53*/

/* SDC_STS mask */
#define SDC_STS_SDCBUSY            (0x1   <<  0)     /* RW */
#define SDC_STS_CMDBUSY            (0x1   <<  1)     /* RW */
#define SDC_STS_START_BIT_CRCERR   (0x1  << 2)
#define SDC_STS_CMD_WR_BUSY        (0x1   << 16)     /* RW !!! MT2701  Add*/
#define SDC_STS_DATA_TIMEOUT_TYPE  (0x3  << 17)      /* RU */
#define SDC_STS_CMD_TIMEOUT_TYPE   (0x3  << 19)      /* RU */
#define SDC_STS_SWR_COMPL          (0x1   << 31)     /* RW */

/* SDC_VOL_CHG mask */
#define SDC_VOL_CHG_CNT         (0xffff << 0)     /* RW  */
/* SDC_DCRC_STS mask */
#define SDC_DCRC_STS_POS        (0xFF  <<  0)     /* RO */
#define SDC_DCRC_STS_NEG        (0xFF  <<  8)     /* RO */

/* SDC_ADV_CFG0 mask */
#define SDC_ADV_CFG0_RESP_CRC             (0x7f  << 0) /* RU */
#define SDC_ADV_CFG0_RESP_INDEX           (0x3f  << 7) /* RU */
#define SDC_ADV_CFG0_RESP_ENDBIT          (0x1  << 13) /* RU */
#define SDC_ADV_CFG0_ENDBIT_CHECK         (0x1  << 14) /* RW */
#define SDC_ADV_CFG0_INDEX_CHECK          (0x1  << 15) /* RW */
#define SDC_ADV_CFG0_DAT_BUF_CLK_DIV      (0x3  << 16) /* RW */
#define SDC_ADV_CFG0_DAT_BUF_FREQ_CTL_EN  (0x1  << 18) /* RW */
#define SDC_ADV_CFG0_SDIO_IRQ_ENHANCE_EN  (0x1  << 19) /* RW */
#define SDC_ADV_CFG0_SDC_RX_ENH_EN        (0x1  << 20) /* RW */

/* EMMC_CFG0 mask */
#define EMMC_CFG0_BOOTSTART     (0x1   <<  0)     /* WO Only for eMMC */
#define EMMC_CFG0_BOOTSTOP      (0x1   <<  1)     /* WO Only for eMMC */
#define EMMC_CFG0_BOOTMODE      (0x1   <<  2)     /* RW Only for eMMC */
#define EMMC_CFG0_BOOTACKDIS    (0x1   <<  3)     /* RW Only for eMMC */

#define EMMC_CFG0_BOOTWDLY      (0x7   << 12)     /* RW Only for eMMC */
#define EMMC_CFG0_BOOTSUPP      (0x1   << 15)     /* RW Only for eMMC */

/* EMMC_CFG1 mask */
#define EMMC_CFG1_BOOTDATTMC    (0xFFFFF <<  0)     /* RW Only for eMMC */
#define EMMC_CFG1_BOOTACKTMC    (0xFFF   << 20)     /* RW Only for eMMC */

/* EMMC_STS mask */
#define EMMC_STS_BOOTCRCERR     (0x1   <<  0)     /* W1C Only for eMMC */
#define EMMC_STS_BOOTACKERR     (0x1   <<  1)     /* W1C Only for eMMC */
#define EMMC_STS_BOOTDATTMO     (0x1   <<  2)     /* W1C Only for eMMC */
#define EMMC_STS_BOOTACKTMO     (0x1   <<  3)     /* W1C Only for eMMC */
#define EMMC_STS_BOOTUPSTATE    (0x1   <<  4)     /* RU Only for eMMC */
#define EMMC_STS_BOOTACKRCV     (0x1   <<  5)     /* W1C Only for eMMC */
#define EMMC_STS_BOOTDATRCV     (0x1   <<  6)     /* RU Only for eMMC */

/* EMMC_IOCON mask */
#define EMMC_IOCON_BOOTRST      (0x1   <<  0)     /* RW Only for eMMC */

/* SDC_ACMD19_TRG mask */
#define SDC_ACMD19_TRG_TUNESEL  (0xF   <<  0)     /* RW */

/* MSDC_DMA_SA_HIGH */
#define MSDC_DMA_SURR_ADDR_HIGH4BIT     (0xf  << 0)     /* RW */

/* MSDC_DMA_CTRL mask */
#define MSDC_DMA_CTRL_START             (0x1  <<  0)     /* WO */
#define MSDC_DMA_CTRL_STOP              (0x1  << 1)     /* W  */
#define MSDC_DMA_CTRL_RESUME            (0x1  << 2)     /* W  */
#define MSDC_DMA_CTRL_REDAYM            (0x1  << 3)     /* RO */
#define MSDC_DMA_CTRL_MODE              (0x1  << 8)     /* RW */
#define MSDC_DMA_CTRL_ALIGN             (0x1  << 9)     /* RW */
#define MSDC_DMA_CTRL_LASTBUF           (0x1  << 10)    /* RW */
#define MSDC_DMA_CTRL_SPLIT1K           (0x1  << 11)    /* RW */
#define MSDC_DMA_CTRL_BURSTSZ           (0x7  << 12)    /* RW */

/* MSDC_DMA_CFG mask */
#define MSDC_DMA_CFG_STS                (0x1  << 0)     /* R */
#define MSDC_DMA_CFG_DECSEN             (0x1  << 1)     /* RW */
#define MSDC_DMA_CFG_LOCKDISABLE        (0x1  << 2)     /* RW */
#define MSDC_DMA_CFG_AHBEN              (0x3  << 8)     /* RW */
#define MSDC_DMA_CFG_ACTEN              (0x3  << 12)    /* RW */
#define MSDC_DMA_CFG_CS12B              (0x1  << 16)    /* RW */

/* MSDC_PATCH_BIT0 mask */
#define MSDC_PB0_EN_START_BIT_CHK_SUP   (0x1 << 0)
#define MSDC_PB0_EN_8BITSUP             (0x1 << 1)
#define MSDC_PB0_DIS_RECMDWR            (0x1 << 2)
#define MSDC_PB0_RD_DAT_SEL             (0x1 << 3)
#define MSDC_PB0_RESV2                  (0x3 << 4)
#define MSDC_PB0_DESCUP                 (0x1 << 6)
#define MSDC_PB0_INT_DAT_LATCH_CK_SEL   (0x7 << 7)
#define MSDC_PB0_CKGEN_MSDC_DLY_SEL     (0x1F<<10)
#define MSDC_PB0_FIFORD_DIS             (0x1 << 15)
#define MSDC_PB0_BLKNUM_SEL             (0x1 << 16)
#define MSDC_PB0_SDIO_INTCSEL           (0x1 << 17)
#define MSDC_PB0_SDC_BSYDLY             (0xf << 18)
#define MSDC_PB0_SDC_WDOD               (0xf << 22)
#define MSDC_PB0_CMDIDRTSEL             (0x1 << 26)
#define MSDC_PB0_CMDFAILSEL             (0x1 << 27)
#define MSDC_PB0_SDIO_INTDLYSEL         (0x1 << 28)
#define MSDC_PB0_SPCPUSH                (0x1 << 29)
#define MSDC_PB0_DETWR_CRCTMO           (0x1 << 30)
#define MSDC_PB0_EN_DRVRSP              (0x1 << 31)    /* RW */

/* MSDC_PATCH_BIT1 mask */
#define MSDC_PB1_WRDAT_CRCS_TA_CNTR     (0x7 << 0)
#define MSDC_PB1_CMD_RSP_TA_CNTR        (0x7 << 3)
#define MSDC_PB1_GET_BUSY_MA            (0x1 << 6)
#define MSDC_PB1_CHECK_BUSY_SEL         (0x1 << 7)
#define MSDC_PB1_STOP_DLY_SEL           (0xf << 8)
#define MSDC_PB1_BIAS_EN18IO_28NM       (0x1 << 12)
#define MSDC_PB1_BIAS_EXT_28NM          (0x1 << 13)
#define MSDC_PB1_DDR_CMD_FIX_SEL        (0x1 << 14)
#define MSDC_PB1_RESET_GDMA             (0x1 << 15)
#define MSDC_PB1_SINGLE_BURST           (0x1 << 16)
#define MSDC_PB1_FROCE_STOP             (0x1 << 17)
#define MSDC_PB1_STATE_CLR              (0x1 << 19)
#define MSDC_PB1_POP_MARK_WATER         (0x1 << 20)
#define MSDC_PB1_DCM_EN                 (0x1 << 21)
#define MSDC_PB1_AXI_WRAP_CKEN          (0x1 << 22)
#define MSDC_PB1_CKCLK_GDMA_EN          (0x1 << 23)
#define MSDC_PB1_CKSPCEN                (0x1 << 24)
#define MSDC_PB1_CKPSCEN                (0x1 << 25)
#define MSDC_PB1_CKVOLDETEN             (0x1 << 26)
#define MSDC_PB1_CKACMDEN               (0x1 << 27)
#define MSDC_PB1_CKSDEN                 (0x1 << 28)    /* RW */
#define MSDC_PB1_CKWCTLEN               (0x1 << 29)    /* RW */
#define MSDC_PB1_CKRCTLEN               (0x1 << 30)    /* RW */
#define MSDC_PB1_CKSHBFFEN              (0x1 << 31)    /* RW */

/* MSDC_PATCH_BIT2 mask */
#define MSDC_PB2_ENHANCEGPD             (0x1 <<  0)    /* RW !!! MT2701  Add */
#define MSDC_PB2_SUPPORT64G             (0x1 <<  1)    /* RW !!! MT2701  Add */
#define MSDC_PB2_RESPWAITCNT            (0x3 << 2)
#define MSDC_PB2_CFGRDATCNT             (0x1f << 4)
#define MSDC_PB2_CFGRDAT                (0x1 << 9)
#define MSDC_PB2_INTCRESPSEL            (0x1 << 11)
#define MSDC_PB2_CFGRESPCNT             (0x7 << 12)
#define MSDC_PB2_CFGRESP                (0x1 << 15)
#define MSDC_PB2_RESPSTENSEL            (0x7 << 16)
#define MSDC_PB2_DDR50_SEL              (0x1 << 19)
#define MSDC_PB2_POPENCNT               (0xf << 20)
#define MSDC_PB2_CFG_CRCSTS_SEL         (0x1 << 24)
#define MSDC_PB2_CFGCRCSTSEDGE          (0x1 << 25)
#define MSDC_PB2_CFGCRCSTSCNT           (0x3 << 26)
#define MSDC_PB2_CFGCRCSTS              (0x1 << 28)
#define MSDC_PB2_CRCSTSENSEL            (0x7 << 29)    /* RW !!! MT2701  Add */

#define MSDC_MASK_ACMD53_CRC_ERR_INTR   (0x1<<4)
#define MSDC_ACMD53_FAIL_ONE_SHOT       (0X1<<5)

/* MSDC_PAD_TUNE mask */
#define MSDC_PAD_TUNE0_DATWRDLY         (0x1F <<  0)     /* RW */
#define MSDC_PAD_TUNE0_DELAYEN          (0x1  <<  7)     /* RW */
#define MSDC_PAD_TUNE0_DATRRDLY         (0x1F <<  8)     /* RW */
#define MSDC_PAD_TUNE0_DATRRDLYSEL      (0x1  << 13)     /* RW */
#define MSDC_PAD_TUNE0_RXDLYSEL         (0x1  << 15)     /* RW */
#define MSDC_PAD_TUNE0_CMDRDLY          (0x1F << 16)     /* RW */
#define MSDC_PAD_TUNE0_CMDRRDLYSEL      (0x1  << 21)     /* RW */
#define MSDC_PAD_TUNE0_CMDRRDLY         (0x1F << 22)   /* RW */
#define MSDC_PAD_TUNE0_CLKTXDLY         (0x1F << 27)   /* RW */

/* MSDC_PAD_TUNE1 mask */
#define MSDC_PAD_TUNE1_DATRRDLY2        (0x1F <<  8)     /* RW */
#define MSDC_PAD_TUNE1_DATRRDLY2SEL     (0x1  << 13)     /* RW */
#define MSDC_PAD_TUNE1_CMDRDLY2         (0x1F << 16)     /* RW */
#define MSDC_PAD_TUNE1_CMDRRDLY2SEL     (0x1  << 21)     /* RW */

/* MSDC_DAT_RDDLY0 mask */
#define MSDC_DAT_RDDLY0_D3            (0x1F  <<  0)     /* RW */
#define MSDC_DAT_RDDLY0_D2            (0x1F  <<  8)     /* RW */
#define MSDC_DAT_RDDLY0_D1            (0x1F  << 16)     /* RW */
#define MSDC_DAT_RDDLY0_D0            (0x1F  << 24)     /* RW */

/* MSDC_DAT_RDDLY1 mask */
#define MSDC_DAT_RDDLY1_D7            (0x1F  <<  0)     /* RW */

#define MSDC_DAT_RDDLY1_D6            (0x1F  <<  8)     /* RW */

#define MSDC_DAT_RDDLY1_D5            (0x1F  << 16)     /* RW */

#define MSDC_DAT_RDDLY1_D4            (0x1F  << 24)     /* RW */

/* MSDC_DAT_RDDLY2 mask */
#define MSDC_DAT_RDDLY2_D3            (0x1F  <<  0)     /* RW !!! MT2701  Add*/

#define MSDC_DAT_RDDLY2_D2            (0x1F  <<  8)     /* RW !!! MT2701  Add*/

#define MSDC_DAT_RDDLY2_D1            (0x1F  << 16)     /* RW !!! MT2701  Add*/

#define MSDC_DAT_RDDLY2_D0            (0x1F  << 24)     /* RW !!! MT2701  Add*/

/* MSDC_DAT_RDDLY3 mask */
#define MSDC_DAT_RDDLY3_D7            (0x1F  <<  0)     /* RW !!! MT2701  Add*/

#define MSDC_DAT_RDDLY3_D6            (0x1F  <<  8)     /* RW !!! MT2701  Add*/

#define MSDC_DAT_RDDLY3_D5            (0x1F  << 16)     /* RW !!! MT2701  Add*/

#define MSDC_DAT_RDDLY3_D4            (0x1F  << 24)     /* RW !!! MT2701  Add*/

/* MSDC_HW_DBG_SEL mask */
#define MSDC_HW_DBG0_SEL              (0xFF  <<  0)     /* RW DBG3->DBG0 !!! MT2701  Change*/
#define MSDC_HW_DBG1_SEL              (0x3F  <<  8)     /* RW DBG2->DBG1 !!! MT2701  Add*/

#define MSDC_HW_DBG2_SEL              (0xFF  << 16)     /* RW DBG1->DBG2 !!! MT2701  Add*/
#define MSDC_HW_DBG3_SEL              (0x3F  << 24)     /* RW DBG0->DBG3 !!! MT2701  Add*/
#define MSDC_HW_DBG_WRAPTYPE_SEL      (0x1   << 30)

/* MSDC_EMMC50_PAD_CTL0 mask*/
#define MSDC_EMMC50_PAD_CTL0_DCCSEL     (0x1  << 0)
#define MSDC_EMMC50_PAD_CTL0_HLSEL      (0x1  <<  1)     /* RW */
#define MSDC_EMMC50_PAD_CTL0_DLP0       (0x3  <<  2)     /* RW */
#define MSDC_EMMC50_PAD_CTL0_DLN0       (0x3  <<  4)     /* RW */
#define MSDC_EMMC50_PAD_CTL0_DLP1       (0x3  <<  6)     /* RW */
#define MSDC_EMMC50_PAD_CTL0_DLN1       (0x3  <<  8)     /* RW */

/* MSDC_EMMC50_PAD_DS_CTL0 mask */
#define MSDC_EMMC50_PAD_DS_CTL0_SR    (0x1  <<  0)     /* RW */
#define MSDC_EMMC50_PAD_DS_CTL0_R0    (0x1  <<  1)     /* RW */
#define MSDC_EMMC50_PAD_DS_CTL0_R1    (0x1  <<  2)     /* RW */
#define MSDC_EMMC50_PAD_DS_CTL0_PUPD  (0x1  <<  3)     /* RW */
#define MSDC_EMMC50_PAD_DS_CTL0_IES   (0x1  <<  4)     /* RW */
#define MSDC_EMMC50_PAD_DS_CTL0_SMT   (0x1  <<  5)     /* RW */
#define MSDC_EMMC50_PAD_DS_CTL0_RDSEL (0x3F <<  6)     /* RW */
#define MSDC_EMMC50_PAD_DS_CTL0_TDSEL (0xF  << 12)     /* RW */
#define MSDC_EMMC50_PAD_DS_CTL0_DRV   (0x7  << 16)     /* RW */


/* EMMC50_PAD_DS_TUNE mask */
#define MSDC_EMMC50_PAD_DS_TUNE_DLYSEL  (0x1  <<  0)  /* RW */
#define MSDC_EMMC50_PAD_DS_TUNE_DLY2SEL (0x1  <<  1)  /* RW */
#define MSDC_EMMC50_PAD_DS_TUNE_DLY1    (0x1F <<  2)  /* RW */
#define MSDC_EMMC50_PAD_DS_TUNE_DLY2    (0x1F <<  7)  /* RW */
#define MSDC_EMMC50_PAD_DS_TUNE_DLY3    (0x1F << 12)  /* RW */

/* EMMC50_PAD_CMD_TUNE mask */
#define MSDC_EMMC50_PAD_CMD_TUNE_DLY3SEL (0x1  <<  0)  /* RW */
#define MSDC_EMMC50_PAD_CMD_TUNE_RXDLY3  (0x1F <<  1)  /* RW */
#define MSDC_EMMC50_PAD_CMD_TUNE_TXDLY   (0x1F <<  6)  /* RW */

/* EMMC50_PAD_DAT01_TUNE mask */
#define MSDC_EMMC50_PAD_DAT0_RXDLY3SEL   (0x1  <<  0)  /* RW */
#define MSDC_EMMC50_PAD_DAT0_RXDLY3      (0x1F <<  1)  /* RW */
#define MSDC_EMMC50_PAD_DAT0_TXDLY       (0x1F <<  6)  /* RW */
#define MSDC_EMMC50_PAD_DAT1_RXDLY3SEL   (0x1  << 16)  /* RW */
#define MSDC_EMMC50_PAD_DAT1_RXDLY3      (0x1F << 17)  /* RW */
#define MSDC_EMMC50_PAD_DAT1_TXDLY       (0x1F << 22)  /* RW */

/* EMMC50_PAD_DAT23_TUNE mask */
#define MSDC_EMMC50_PAD_DAT2_RXDLY3SEL   (0x1  <<  0)  /* RW */
#define MSDC_EMMC50_PAD_DAT2_RXDLY3      (0x1F <<  1)  /* RW */
#define MSDC_EMMC50_PAD_DAT2_TXDLY       (0x1F <<  6)  /* RW */
#define MSDC_EMMC50_PAD_DAT3_RXDLY3SEL   (0x1  << 16)  /* RW */
#define MSDC_EMMC50_PAD_DAT3_RXDLY3      (0x1F << 17)  /* RW */
#define MSDC_EMMC50_PAD_DAT3_TXDLY       (0x1F << 22)  /* RW */

/* EMMC50_PAD_DAT45_TUNE mask */
#define MSDC_EMMC50_PAD_DAT4_RXDLY3SEL   (0x1  <<  0)  /* RW */
#define MSDC_EMMC50_PAD_DAT4_RXDLY3      (0x1F <<  1)  /* RW */
#define MSDC_EMMC50_PAD_DAT4_TXDLY       (0x1F <<  6)  /* RW */
#define MSDC_EMMC50_PAD_DAT5_RXDLY3SEL   (0x1  << 16)  /* RW */
#define MSDC_EMMC50_PAD_DAT5_RXDLY3      (0x1F << 17)  /* RW */
#define MSDC_EMMC50_PAD_DAT5_TXDLY       (0x1F << 22)  /* RW */

/* EMMC50_PAD_DAT67_TUNE mask */
#define MSDC_EMMC50_PAD_DAT6_RXDLY3SEL   (0x1  <<  0)  /* RW */
#define MSDC_EMMC50_PAD_DAT6_RXDLY3      (0x1F <<  1)  /* RW */
#define MSDC_EMMC50_PAD_DAT6_TXDLY       (0x1F <<  6)  /* RW */
#define MSDC_EMMC50_PAD_DAT7_RXDLY3SEL   (0x1  << 16)  /* RW */
#define MSDC_EMMC50_PAD_DAT7_RXDLY3      (0x1F << 17)  /* RW */
#define MSDC_EMMC50_PAD_DAT7_TXDLY       (0x1F << 22)  /* RW */

/* EMMC51_CFG0 mask */
#define MSDC_EMMC51_CFG_CMDQEN          (0x1    <<  0)
#define MSDC_EMMC51_CFG_NUM             (0x3F   <<  1)
#define MSDC_EMMC51_CFG_RSPTYPE         (0x7    <<  7)
#define MSDC_EMMC51_CFG_DTYPE           (0x3    << 10)
#define MSDC_EMMC51_CFG_RDATCNT         (0x3FF  << 12)
#define MSDC_EMMC51_CFG_WDATCNT         (0x3FF  << 22)

/* EMMC50_CFG0 mask */
#define MSDC_EMMC50_CFG_PADCMD_LATCHCK  (0x1 << 0)
#define MSDC_EMMC50_CFG_CRC_STS_CNT     (0x3 << 1)
#define MSDC_EMMC50_CFG_CRC_STS_EDGE    (0x1 << 3)
#define MSDC_EMMC50_CFG_CRC_STS_SEL     (0x1 << 4)
#define MSDC_EMMC50_CFG_END_BIT_CHK_CNT (0xf << 5)
#define MSDC_EMMC50_CFG_CMD_RESP_SEL    (0x1 << 9)
#define MSDC_EMMC50_CFG_CMD_EDGE_SEL    (0x1 << 10)
#define MSDC_EMMC50_CFG_ENDBIT_CNT      (0x3FF << 11)
#define MSDC_EMMC50_CFG_READ_DAT_CNT    (0x7 << 21)
#define MSDC_EMMC50_CFG_EMMC50_MON_SEL  (0x1 << 24)
#define MSDC_EMMC50_CFG_TXSKEW_SEL      (0x1 << 29)

/* EMMC50_CFG1 mask */
#define MSDC_EMMC50_CFG1_CKSWITCH_CNT   (0x7  << 8)
#define MSDC_EMMC50_CFG1_RDDAT_STOP     (0x1  << 11)
#define MSDC_EMMC50_CFG1_WAITCLK_CNT    (0xF  << 12)
#define MSDC_EMMC50_CFG1_DBG_SEL        (0xFF << 16)
#define MSDC_EMMC50_CFG1_PSHCNT         (0x7  << 24)
#define MSDC_EMMC50_CFG1_PSHPSSEL       (0x1  << 27)
#define MSDC_EMMC50_CFG1_DSCFG          (0x1  << 28)
#define MSDC_EMMC50_CFG1_SPARE1         (0x7UL << 29)

/* EMMC50_CFG2_mask */
#define MSDC_EMMC50_CFG2_AXI_IOMMU_WR_EMI       (0x1 << 1)
#define MSDC_EMMC50_CFG2_AXI_SHARE_EN_WR_EMI    (0x1 << 2)
#define MSDC_EMMC50_CFG2_AXI_IOMMU_RD_EMI       (0x1 << 7)
#define MSDC_EMMC50_CFG2_AXI_SHARE_EN_RD_EMI    (0x1 << 8)
#define MSDC_EMMC50_CFG2_AXI_BOUND_128B         (0x1 << 13)
#define MSDC_EMMC50_CFG2_AXI_BOUND_256B         (0x1 << 14)
#define MSDC_EMMC50_CFG2_AXI_BOUND_512B         (0x1 << 15)
#define MSDC_EMMC50_CFG2_AXI_BOUND_1K           (0x1 << 16)
#define MSDC_EMMC50_CFG2_AXI_BOUND_2K           (0x1 << 17)
#define MSDC_EMMC50_CFG2_AXI_BOUND_4K           (0x1 << 18)
#define MSDC_EMMC50_CFG2_AXI_RD_OUTS_NUM        (0x1F << 19)
#define MSDC_EMMC50_CFG2_AXI_SET_LEN            (0xf << 24)
#define MSDC_EMMC50_CFG2_AXI_RESP_ERR_TYPE      (0x3 << 28)
#define MSDC_EMMC50_CFG2_AXI_BUSY               (0x1 << 30)

/* EMMC50_CFG3_mask */
#define MSDC_EMMC50_CFG3_OUTS_WR                (0x1F << 0)
#define MSDC_EMMC50_CFG3_ULTRA_SET_WR           (0x3F << 5)
#define MSDC_EMMC50_CFG3_PREULTRA_SET_WR        (0x3F << 11)
#define MSDC_EMMC50_CFG3_ULTRA_SET_RD           (0x3F << 17)
#define MSDC_EMMC50_CFG3_PREULTRA_SET_RD        (0x3F << 23)

/* EMMC50_CFG4_mask */
#define MSDC_EMMC50_CFG4_IMPR_ULTRA_SET_WR      (0xFF << 0)
#define MSDC_EMMC50_CFG4_IMPR_ULTRA_SET_RD      (0xFF << 8)
#define MSDC_EMMC50_CFG4_ULTRA_EN               (0x3  << 16)
#define MSDC_EMMC50_CFG4_AXI_WRAP_DBG_SEL       (0x1F << 18)

/* SDC_FIFO_CFG mask */
#define SDC_FIFO_CFG_EMMC50_BLOCK_LENGTH        (0x1FF << 0)
#define SDC_FIFO_CFG_WR_PTR_MARGIN              (0xFF << 16)
#define SDC_FIFO_CFG_WR_VALID_SEL               (0x1  << 24)
#define SDC_FIFO_CFG_RD_VALID_SEL               (0x1  << 25)
#define SDC_FIFO_CFG_WR_VALID                   (0x1  << 26)
#define SDC_FIFO_CFG_RD_VALID                   (0x1  << 27)

/* SDIO_TUNE_WIND mask*/
#define MSDC_SDIO_TUNE_WIND             (0x1F << 0)

/* MSDC_AES_SEL mask*/
#define MSDC_AES_SEL_SEL                (0x3F << 0)
#define MSDC_AES_SEL_EN                 (0xF  << 8)

/* MTKCQ_CFG0 mask*/
#define MTKCQ_CFG0_ACMD13_BLK_CNT       (0xFFFF << 0)

/* MTKCQ_CFG1 mask*/
#define MTKCQ_CFG1_ACMD13_IDLE_TIME     (0xFFFF << 0)

/* MTKCQ_CFG2 mask*/
#define MTKCQ_CFG2_CQ_EN                (0x1 << 0)
#define MTKCQ_CFG2_CQ_BUSY              (0x1 << 1)
#define MTKCQ_CFG2_CMD13_FLAG           (0x1 << 4)

/* MTKCQ_ERR_ST mask*/
#define MTKCQ_ERR_ST_CMD44_RESP_CRCERR          (0x1 << 0)      /* RO */
#define MTKCQ_ERR_ST_CMD44_RESP_TO              (0x1 << 1)      /* RO */
#define MTKCQ_ERR_ST_CMD45_RESP_CRCERR          (0x1 << 4)      /* RO */
#define MTKCQ_ERR_ST_CMD45_RESP_TO              (0x1 << 5)      /* RO */
#define MTKCQ_ERR_ST_CMD13_RESP_CRCERR          (0x1 << 8)      /* RO */
#define MTKCQ_ERR_ST_CMD13_RESP_TO              (0x1 << 9)      /* RO */
#define MTKCQ_ERR_ST_OTHER_CMD_RESP_CRCERR      (0x1 << 12)     /* RO */
#define MTKCQ_ERR_ST_OTHER_CMD_RESP_TO          (0x1 << 13)     /* RO */
#define MTKCQ_ERR_ST_RESP_ERR_ST                (0x1 << 16)     /* RO */
#define MTKCQ_ERR_ST_CQ_R1_RESP_ERR             (0x1 << 17)     /* RO */

/* MTKCQ_CMD45_READY mask*/
#define MTKCQ_CMD45_READY_ST            (0xFFFFFFFF << 0)       /* RW */

/* MTKCQ_TASK_READY_ST mask*/
#define MTKCQ_TASK_READY_ST_MA          (0xFFFFFFFF << 0)       /* RO */

/* MTKCQ_TASK_DONE_ST mask*/
#define MTKCQ_TASK_DONE_ST_CMD13_UPDATE_ST      (0x1 << 0)      /* RO */
#define MTKCQ_TASK_DONE_ST_CMD45_DONE_ST        (0x1 << 1)      /* RO */
#define MTKCQ_TASK_DONE_ST_OTHER_CMD_DONE_ST    (0x1 << 2)      /* RO */
#define MTKCQ_TASK_DONE_ST_CMD_DONE_ST          (0x1 << 3)      /* RO */
#define MTKCQ_TASK_DONE_ST_CMD13_DONE_ST        (0x1 << 4)      /* RO */
#define MTKCQ_TASK_DONE_ST_CQ_SWCMD_MISS        (0x1 << 5)      /* RO */
#define MTKCQ_TASK_DONE_ST_CMD45_DONE_ST_MASK   (0x1 << 6)      /* RW */

/* MTKCQ_ERR_ST_CLR mask*/
#define MTKCQ_ERR_ST_CLR_CMD44_RESP_CRCERR_CLR          (0x1 << 0)      /* W1 */
#define MTKCQ_ERR_ST_CLR_CMD44_RESP_TO_CLR              (0x1 << 1)      /* W1 */
#define MTKCQ_ERR_ST_CLR_CMD45_RESP_CRCERR_CLR          (0x1 << 4)      /* W1 */
#define MTKCQ_ERR_ST_CLR_CMD45_RESP_TO_CLR              (0x1 << 5)      /* W1 */
#define MTKCQ_ERR_ST_CLR_CMD13_RESP_CRCERR_CLR          (0x1 << 8)      /* W1 */
#define MTKCQ_ERR_ST_CLR_CMD13_RESP_TO_CLR              (0x1 << 9)      /* W1 */
#define MTKCQ_ERR_ST_CLR_OTHER_CMD_RESP_CRCERR_CLR      (0x1 << 12)     /* W1 */
#define MTKCQ_ERR_ST_CLR_CQ_R1_RESP_ERR_CLR             (0x1 << 17)     /* W1 */

/* MTKCQ_CMD_DONE_CLR mask*/
#define MTKCQ_CMD_DONE_CLR_CMD13_UPDATE_ST_CLR          (0x1 << 0)      /* W1 */
#define MTKCQ_CMD_DONE_CLR_CMD45_DONE_ST_CLR            (0x1 << 1)      /* W1 */
#define MTKCQ_CMD_DONE_CLR_OTHER_CMD_DONE_ST_CLR        (0x1 << 2)      /* W1 */
#define MTKCQ_CMD_DONE_CLR_CMD13_DONE_ST_CLR            (0x1 << 4)      /* W1 */

/* MTKCQ_SW_CTL_CQ mask*/
#define MTKCQ_SW_CTL_CQ_SW_RESTART_CQ   (0x1 << 0)      /* RW */
#define MTKCQ_SW_CTL_CQ_GO_IDLE         (0x1 << 4)      /* RW */
#define MTKCQ_SW_CTL_CQ_GO_ACTIVE       (0x1 << 5)      /* A0 */

/* MTKCQ_CMD44_RESP mask*/
#define MTKCQ_CMD44_RESP_MASK           (0xFFFFFFFF << 0)       /* RO */

/* MTKCQ_CMD45_RESP mask*/
#define MTKCQ_CMD45_RESP_MASK           (0xFFFFFFFF << 0)       /* RO */

/* MTKCQ_CMD13_RCA mask*/
#define MTKCQ_CMD13_RCA_MASK            (0xFFFF << 0)   /* RW */

/* EMMC51_CQCB_CFG3 mask*/
#define EMMC51_CQCB_CFG3_MSDC_CQCB_CLR  (0x1 << 0)      /* A0 */
#define EMMC51_CQCB_CFG3_CQCB_FIFO_FULL (0x1 << 4)      /* RU */

/* EMMC51_CQCB_CMD44 mask*/
#define EMMC51_CQCB_CMD44_MASK          (0xFFFFFFFF << 0)       /* RW */

/* EMMC51_CQCB_CMD45 mask*/
#define EMMC51_CQCB_CMD45_MASK          (0xFFFFFFFF << 0)       /* RW */

/* EMMC51_CQCB_TIDMAP mask*/
#define EMMC51_CQCB_TIDMAP_TASKIDMAP    (0xFFFFFFFF << 0)       /* RW */

/* EMMC51_CQCB_TIDMAPCLR mask*/
#define EMMC51_CQCB_TIDMAPCLR_MASK      (0xFFFFFFFF << 0)       /* WO */

/* EMMC51_CQCB_CURCMD mask*/
#define EMMC51_CQCB_CURCMD_ID           (0x3F << 0)     /* RW */

#if defined(FEATURE_MULTI_HOST)
#include "msdc_reg_mh.h"
#endif

int msdc_init(struct mmc_host *host);
void msdc_config_bus(struct mmc_host *host, u32 width);
int msdc_dma_transfer(struct mmc_host *host, struct mmc_data *data);
int msdc_tune_bwrite(struct mmc_host *host, u32 dst, u8 *src, u32 nblks);
int msdc_tune_bread(struct mmc_host *host, u8 *dst, u32 src, u32 nblks);
void msdc_reset_tune_counter(struct mmc_host *host);
int msdc_abort_handler(struct mmc_host *host, int abort_card);
int msdc_tune_read(struct mmc_host *host);
void msdc_config_clock(struct mmc_host *host, int state, u32 hz);
int msdc_cmd(struct mmc_host *host, struct mmc_command *cmd);
void msdc_set_timeout(struct mmc_host *host, u32 ns, u32 clks);
void msdc_set_autocmd(struct mmc_host *host, int cmd);
int msdc_get_autocmd(struct mmc_host *host);
void msdc_set_reliable_write(struct mmc_host *host, int on);
void msdc_set_max_phys_segs(struct mmc_host *host);
int msdc_pio_read(struct mmc_host *host, u32 *ptr, u32 size);
