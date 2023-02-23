/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <platform/addressmap.h>
#include <platform/pll_common.h>
#include <pmif.h>

/* timeout setting */
enum {
    TIMEOUT_READ_US        = 255,
    TIMEOUT_WAIT_IDLE_US   = 255
};

enum {
    RDATA_WACS_RDATA_SHIFT = 0,
    RDATA_WACS_FSM_SHIFT   = 16,
    RDATA_WACS_REQ_SHIFT   = 19,
    RDATA_SYNC_IDLE_SHIFT  = 20,
    RDATA_INIT_DONE_SHIFT  = 22,
    RDATA_SYS_IDLE_SHIFT   = 23,
};

struct mtk_pmicspi_mst_regs {
    u32 reserved1[4];
    u32 other_busy_sta_0;
    u32 wrap_en;
    u32 reserved2[2];
    u32 man_en;
    u32 man_acc;
    u32 reserved3[3];
    u32 mux_sel;
    u32 reserved4[3];
    u32 dio_en;
    u32 rddmy;
    u32 cslext_write;
    u32 cslext_read;
    u32 cshext_write;
    u32 cshext_read;
    u32 ext_ck_write;
    u32 ext_ck_read;
    u32 si_sampling_ctrl;
};

STATIC_ASSERT(__offsetof(struct mtk_pmicspi_mst_regs, other_busy_sta_0) == 0x10);
STATIC_ASSERT(__offsetof(struct mtk_pmicspi_mst_regs, man_en) == 0x20);
STATIC_ASSERT(__offsetof(struct mtk_pmicspi_mst_regs, mux_sel) == 0x34);
STATIC_ASSERT(__offsetof(struct mtk_pmicspi_mst_regs, dio_en) == 0x44);
STATIC_ASSERT(__offsetof(struct mtk_pmicspi_mst_regs, si_sampling_ctrl) == 0x64);

static struct mtk_pmicspi_mst_regs * const mtk_pmicspi_mst = (void *)PMICSPI_MST_BASE;

/* PMIC registers */
enum {
    PMIC_BASE             = 0x0000,
    PMIC_SMT_CON1         = PMIC_BASE + 0x0032,
    PMIC_DRV_CON1         = PMIC_BASE + 0x003a,
    PMIC_FILTER_CON0      = PMIC_BASE + 0x0042,
    PMIC_GPIO_PULLEN0_CLR = PMIC_BASE + 0x0098,
    PMIC_RG_SPI_CON0      = PMIC_BASE + 0x0408,
    PMIC_DEW_DIO_EN       = PMIC_BASE + 0x040c,
    PMIC_DEW_READ_TEST    = PMIC_BASE + 0x040e,
    PMIC_DEW_WRITE_TEST   = PMIC_BASE + 0x0410,
    PMIC_DEW_CRC_EN       = PMIC_BASE + 0x0414,
    PMIC_DEW_CRC_VAL      = PMIC_BASE + 0x0416,
    PMIC_DEW_RDDMY_NO     = PMIC_BASE + 0x0424,
    PMIC_RG_SPI_CON2      = PMIC_BASE + 0x0426,
    PMIC_SPISLV_KEY       = PMIC_BASE + 0x044a,
    PMIC_INT_STA          = PMIC_BASE + 0x0452,
    PMIC_AUXADC_ADC7      = PMIC_BASE + 0x1096,
    PMIC_AUXADC_ADC10     = PMIC_BASE + 0x109c,
    PMIC_AUXADC_RQST0     = PMIC_BASE + 0x1108,
};

#define BIT(nr) (1 << (nr))

#define PMIF_SPI_HW_INF     0x307F
#define PMIF_SPI_MD         BIT(8)
#define PMIF_SPI_AP_SECURE  BIT(9)
#define PMIF_SPI_AP         BIT(10)
#define PMIF_SPI_STAUPD     BIT(14)
#define PMIF_SPI_TSX_HW     BIT(19)
#define PMIF_SPI_DCXO_HW    BIT(20)

#define DEFAULT_SLVID   0

#define PMIF_CMD_STA    BIT(2)
#define SPIMST_STA      BIT(9)

enum {
    SPI_CLK = 0x1,
    SPI_CSN = 0x1 << 1,
    SPI_MOSI = 0x1 << 2,
    SPI_MISO = 0x1 << 3,
    SPI_FILTER = (SPI_CLK | SPI_CSN | SPI_MOSI | SPI_MISO) << 4,
    SPI_SMT = SPI_CLK | SPI_CSN | SPI_MOSI | SPI_MISO,
    SPI_PULL_DISABLE = (SPI_CLK | SPI_CSN | SPI_MOSI | SPI_MISO) << 4,
};

enum {
    IO_4_MA = 0x9,
    SLV_IO_4_MA = 0x8,
};

enum {
    SPI_CLK_SHIFT = 0,
    SPI_CSN_SHIFT = 4,
    SPI_MOSI_SHIFT = 8,
    SPI_MISO_SHIFT = 12,
    SPI_DRIVING = SLV_IO_4_MA << SPI_CLK_SHIFT | SLV_IO_4_MA << SPI_CSN_SHIFT |
                  SLV_IO_4_MA << SPI_MOSI_SHIFT | SLV_IO_4_MA << SPI_MISO_SHIFT,
};

enum {
    DUMMY_READ_CYCLES = 0x8,
};

enum {
    E_CLK_EDGE = 1,
    E_CLK_LAST_SETTING,
};

void platform_pmif_spi_init(void);
