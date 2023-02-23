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

/* TOP REGISTER */
#define OFFSET_EMMC_TOP_CONTROL         (0x00)
#define OFFSET_EMMC_TOP_CMD             (0x04)
#define OFFSET_TOP_EMMC50_PAD_CTL0      (0x08)
#define OFFSET_TOP_EMMC50_PAD_DS_TUNE   (0x0c)
#define OFFSET_TOP_EMMC50_PAD_DAT0_TUNE (0x10)
#define OFFSET_TOP_EMMC50_PAD_DAT1_TUNE (0x14)
#define OFFSET_TOP_EMMC50_PAD_DAT2_TUNE (0x18)
#define OFFSET_TOP_EMMC50_PAD_DAT3_TUNE (0x1c)
#define OFFSET_TOP_EMMC50_PAD_DAT4_TUNE (0x20)
#define OFFSET_TOP_EMMC50_PAD_DAT5_TUNE (0x24)
#define OFFSET_TOP_EMMC50_PAD_DAT6_TUNE (0x28)
#define OFFSET_TOP_EMMC50_PAD_DAT7_TUNE (0x2c)

#define EMMC_TOP_CONTROL                REG_OP(EMMC_TOP_CONTROL)
#define EMMC_TOP_CMD                    REG_OP(EMMC_TOP_CMD)
#define TOP_EMMC50_PAD_CTL0             REG_OP(TOP_EMMC50_PAD_CTL0)
#define TOP_EMMC50_PAD_DS_TUNE          REG_OP(TOP_EMMC50_PAD_DS_TUNE)
#define TOP_EMMC50_PAD_DAT0_TUNE        REG_OP(TOP_EMMC50_PAD_DAT0_TUNE)
#define TOP_EMMC50_PAD_DAT1_TUNE        REG_OP(TOP_EMMC50_PAD_DAT1_TUNE)
#define TOP_EMMC50_PAD_DAT2_TUNE        REG_OP(TOP_EMMC50_PAD_DAT2_TUNE)
#define TOP_EMMC50_PAD_DAT3_TUNE        REG_OP(TOP_EMMC50_PAD_DAT3_TUNE)
#define TOP_EMMC50_PAD_DAT4_TUNE        REG_OP(TOP_EMMC50_PAD_DAT4_TUNE)
#define TOP_EMMC50_PAD_DAT5_TUNE        REG_OP(TOP_EMMC50_PAD_DAT5_TUNE)
#define TOP_EMMC50_PAD_DAT6_TUNE        REG_OP(TOP_EMMC50_PAD_DAT6_TUNE)
#define TOP_EMMC50_PAD_DAT7_TUNE        REG_OP(TOP_EMMC50_PAD_DAT7_TUNE)


/* EMMC_TOP_CONTROL mask */
#define PAD_RXDLY_SEL           (0x1 << 0)      /* RW */
#define DELAY_EN                (0x1 << 1)      /* RW */
#define PAD_DAT_RD_RXDLY2       (0x1F << 2)     /* RW */
#define PAD_DAT_RD_RXDLY        (0x1F << 7)     /* RW */
#define PAD_DAT_RD_RXDLY2_SEL   (0x1 << 12)     /* RW */
#define PAD_DAT_RD_RXDLY_SEL    (0x1 << 13)     /* RW */
#define DATA_K_VALUE_SEL        (0x1 << 14)     /* RW */
#define SDC_RX_ENH_EN           (0x1 << 15)     /* TW */

/* EMMC_TOP_CMD mask */
#define PAD_CMD_RXDLY2          (0x1F << 0)     /* RW */
#define PAD_CMD_RXDLY           (0x1F << 5)     /* RW */
#define PAD_CMD_RD_RXDLY2_SEL   (0x1 << 10)     /* RW */
#define PAD_CMD_RD_RXDLY_SEL    (0x1 << 11)     /* RW */
#define PAD_CMD_TX_DLY          (0x1F << 12)    /* RW */

/* TOP_EMMC50_PAD_CTL0 mask */
#define HL_SEL                  (0x1 << 0)      /* RW */
#define DCC_SEL                 (0x1 << 1)      /* RW */
#define DLN1                    (0x3 << 2)      /* RW */
#define DLN0                    (0x3 << 4)      /* RW */
#define DLP1                    (0x3 << 6)      /* RW */
#define DLP0                    (0x3 << 8)      /* RW */
#define PAD_CLK_TXDLY           (0x1F << 10)    /* RW */

/* TOP_EMMC50_PAD_DS_TUNE mask */
#define PAD_DS_DLY3             (0x1F << 0)     /* RW */
#define PAD_DS_DLY2             (0x1F << 5)     /* RW */
#define PAD_DS_DLY1             (0x1F << 10)    /* RW */
#define PAD_DS_DLY2_SEL         (0x1 << 15)     /* RW */
#define PAD_DS_DLY_SEL          (0x1 << 16)     /* RW */

/* TOP_EMMC50_PAD_DAT0_TUNE mask */
#define DAT0_RD_DLY2            (0x1F << 0)     /* RW */
#define DAT0_RD_DLY1            (0x1F << 5)     /* RW */
#define PAD_DAT0_TX_DLY         (0x1F << 10)    /* RW */

/* TOP_EMMC50_PAD_DAT1_TUNE mask */
#define DAT1_RD_DLY2            (0x1F << 0)     /* RW */
#define DAT1_RD_DLY1            (0x1F << 5)     /* RW */
#define PAD_DAT1_TX_DLY         (0x1F << 10)    /* RW */

/* TOP_EMMC50_PAD_DAT2_TUNE mask */
#define DAT2_RD_DLY2            (0x1F << 0)     /* RW */
#define DAT2_RD_DLY1            (0x1F << 5)     /* RW */
#define PAD_DAT2_TX_DLY         (0x1F << 10)    /* RW */

/* TOP_EMMC50_PAD_DAT3_TUNE mask */
#define DAT3_RD_DLY2            (0x1F << 0)     /* RW */
#define DAT3_RD_DLY1            (0x1F << 5)     /* RW */
#define PAD_DAT3_TX_DLY         (0x1F << 10)    /* RW */

/* TOP_EMMC50_PAD_DAT4_TUNE mask */
#define DAT4_RD_DLY2            (0x1F << 0)     /* RW */
#define DAT4_RD_DLY1            (0x1F << 5)     /* RW */
#define PAD_DAT4_TX_DLY         (0x1F << 10)    /* RW */

/* TOP_EMMC50_PAD_DAT5_TUNE mask */
#define DAT5_RD_DLY2            (0x1F << 0)     /* RW */
#define DAT5_RD_DLY1            (0x1F << 5)     /* RW */
#define PAD_DAT5_TX_DLY         (0x1F << 10)    /* RW */

/* TOP_EMMC50_PAD_DAT6_TUNE mask */
#define DAT6_RD_DLY2            (0x1F << 0)     /* RW */
#define DAT6_RD_DLY1            (0x1F << 5)     /* RW */
#define PAD_DAT6_TX_DLY         (0x1F << 10)    /* RW */

/* TOP_EMMC50_PAD_DAT7_TUNE mask */
#define DAT7_RD_DLY2            (0x1F << 0)     /* RW */
#define DAT7_RD_DLY1            (0x1F << 5)     /* RW */
#define PAD_DAT7_TX_DLY         (0x1F << 10)    /* RW */

/*******************************************************************************
 * Power Definition
 ******************************************************************************/
#define CMD_RSP_TA_CNTR_DEFAULT         0
#define WRDAT_CRCS_TA_CNTR_DEFAULT      0
#define BUSY_MA_DEFAULT                 1

#define CRCSTSENSEL_HS400_DEFAULT       3
#define RESPSTENSEL_HS400_DEFAULT       3
#define CRCSTSENSEL_HS_DEFAULT          1
#define RESPSTENSEL_HS_DEFAULT          1
#define CRCSTSENSEL_FPGA_DEFAULT        0

enum {
    MSDC_HIGHZ = 0,
    MSDC_10KOHM,
    MSDC_50KOHM,
    MSDC_8KOHM,
    MSDC_PST_MAX
};


/*
 * each PLL have different gears for select
 * software can used mux interface from clock management module to select
 */
enum {
    MSDC50_CLKSRC4HCLK_26MHZ  = 0,
    MSDC50_CLKSRC4HCLK_273MHZ,
    MSDC50_CLKSRC4HCLK_156MHZ,
    MSDC50_CLKSRC4HCLK_182MHZ,
    MSDC_DONOTCARE_HCLK,
    MSDC50_CLKSRC4HCLK_MAX
};

enum {
    MSDC50_CLKSRC_26MHZ  = 0,
    MSDC50_CLKSRC_400MHZ,  /* MSDCPLL_CK */
    MSDC50_CLKSRC_182MHZ,  /*MSDCPLL_D2 */
    MSDC50_CLKSRC_78MHZ,
    MSDC50_CLKSRC_312MHZ,
    MSDC50_CLKSRC_273MHZ,  /*MSDCPLL_D4 */
    MSDC50_CLKSRC_249MHZ,
    MSDC50_CLKSRC_156MHZ,
    MSDC50_CLKSRC_MAX
};

/*
 * MSDC0/1/2
 * PLL MUX SEL List
 */
enum {
    MSDC30_CLKSRC_26MHZ   = 0,
    MSDC30_CLKSRC_200MHZ,
    MSDC30_CLKSRC_182MHZ,
    MSDC30_CLKSRC_91MHZ,
    MSDC30_CLKSRC_156MHZ,
    MSDC30_CLKSRC_104MHZ,
    MSDC30_CLKSRC_MAX
};

#define MSDC50_CLKSRC_DEFAULT     MSDC50_CLKSRC_400MHZ
#define MSDC30_CLKSRC_DEFAULT     MSDC30_CLKSRC_200MHZ


/*--------------------------------------------------------------------------*/
/* Descriptor Structure                                                     */
/*--------------------------------------------------------------------------*/
#define DMA_FLAG_NONE       (0x00000000)
#define DMA_FLAG_EN_CHKSUM  (0x00000001)
#define DMA_FLAG_PAD_BLOCK  (0x00000002)
#define DMA_FLAG_PAD_DWORD  (0x00000004)

#define MSDC_WRITE32(addr, data)    writel(data, addr)
#define MSDC_READ32(addr)           readl(addr)
#define MSDC_WRITE8(addr, data)     writeb(data, addr)
#define MSDC_READ8(addr)            readb(addr)

#define udelay(x) spin(x)
#define mdelay(x) spin((x) * 1000)

#define MSDC_SET_BIT32(addr, mask) \
    do {    \
        unsigned int tv = MSDC_READ32(addr); \
        tv |= ((u32)(mask)); \
        MSDC_WRITE32(addr, tv); \
    } while (0)
#define MSDC_CLR_BIT32(addr, mask) \
    do {    \
        unsigned int tv = MSDC_READ32(addr); \
        tv &= ~((u32)(mask)); \
        MSDC_WRITE32(addr, tv); \
    } while (0)

#define MSDC_SET_FIELD(reg, field, val) \
    do { \
        u32 tv = MSDC_READ32(reg); \
        tv &= ~((u32)(field)); \
        tv |= ((val) << (__builtin_ffs((u32)(field)) - 1)); \
        MSDC_WRITE32(reg, tv); \
    } while (0)

#define MSDC_GET_FIELD(reg, field, val) \
    do { \
        u32 tv = MSDC_READ32(reg); \
        val = ((tv & (field)) >> (__builtin_ffs((u32)(field)) - 1)); \
    } while (0)

#define MSDC_RETRY(expr, retry, cnt) \
    do { \
        uint32_t t = cnt; \
        uint32_t r = retry; \
        uint32_t c = cnt; \
        while (r) { \
            if (!(expr)) \
                break; \
            if (c-- == 0) { \
                r--; udelay(200); c = t; \
            } \
        } \
        if (r == 0) \
            dprintf(CRITICAL, "%s->%d: retry %d times failed!\n", __func__, \
                    __LINE__, retry); \
    } while (0)

#define MSDC_RESET() \
    do { \
        MSDC_SET_BIT32(MSDC_CFG, MSDC_CFG_RST); \
        MSDC_RETRY(MSDC_READ32(MSDC_CFG) & MSDC_CFG_RST, 5, 1000); \
    } while (0)

#define MSDC_CLR_INT() \
    do { \
        uint32_t val = MSDC_READ32(MSDC_INT); \
        MSDC_WRITE32(MSDC_INT, val); \
    } while (0)

#define MSDC_CLR_FIFO() \
    do { \
        MSDC_SET_BIT32(MSDC_FIFOCS, MSDC_FIFOCS_CLR); \
        MSDC_RETRY(MSDC_READ32(MSDC_FIFOCS) & MSDC_FIFOCS_CLR, 5, 1000); \
    } while (0)

#define WAIT_COND(cond, tmo, left) \
    do { \
        u32 t = tmo; \
        while (1) { \
            if ((cond) || (t == 0)) \
                break; \
            if (t > 0) { \
                udelay(1); t--; \
            } \
        } \
        left = t; \
    } while (0)

#define MSDC_FIFO_WRITE32(val)  MSDC_WRITE32(MSDC_TXDATA, val)
#define MSDC_FIFO_READ32()      MSDC_READ32(MSDC_RXDATA)
#define MSDC_FIFO_WRITE8(val)   MSDC_WRITE8(MSDC_TXDATA, val)
#define MSDC_FIFO_READ8()       MSDC_READ8(MSDC_RXDATA)

#define MSDC_TXFIFOCNT() \
    ((MSDC_READ32(MSDC_FIFOCS) & MSDC_FIFOCS_TXCNT) >> 16)
#define MSDC_RXFIFOCNT() \
    ((MSDC_READ32(MSDC_FIFOCS) & MSDC_FIFOCS_RXCNT) >> 0)

#define SDC_IS_BUSY()       (MSDC_READ32(SDC_STS) & SDC_STS_SDCBUSY)
#define SDC_IS_CMD_BUSY()   (MSDC_READ32(SDC_STS) & SDC_STS_CMDBUSY)

#define SDC_SEND_CMD(cmd, arg) \
    do { \
        MSDC_WRITE32(SDC_ARG, (arg)); \
        MSDC_WRITE32(SDC_CMD, (cmd)); \
    } while (0)

#define MSDC_DMA_ON     MSDC_CLR_BIT32(MSDC_CFG, MSDC_CFG_PIO)
#define MSDC_DMA_OFF    MSDC_SET_BIT32(MSDC_CFG, MSDC_CFG_PIO)

#define MSDC_RELIABLE_WRITE     (0x1 << 0)
#define MSDC_PACKED             (0x1 << 1)
#define MSDC_TAG_REQUEST        (0x1 << 2)
#define MSDC_CONTEXT_ID         (0x1 << 3)
#define MSDC_FORCED_PROG        (0x1 << 4)

/*
 *MSDC TOP REG
 */
#define REG_OP(x)                 ((uint32_t *)((uintptr_t)host->base_top + OFFSET_##x))

