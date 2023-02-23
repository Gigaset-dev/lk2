/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once
#include "nfi_base.h"

#define SPI_NAND_MAX_DELAY      6
#define SPI_NAND_MAX_OP         4

struct nfi_spi_delay {
    u8 tCLK_SAM_DLY;
    u8 tCLK_OUT_DLY;
    u8 tCS_DLY;
    u8 tWR_EN_DLY;
    u8 tIO_IN_DLY[4];
    u8 tIO_OUT_DLY[4];
    u8 tREAD_LATCH_LATENCY;
};

/* SPI Nand structure */
struct nfi_spi {
    struct nfi_base base;
    struct nfi_base *parent;

    u8 snfi_mode;
    u8 tx_count;

    u8 cmd[SPI_NAND_MAX_OP];
    u8 cur_cmd_idx;

    u32 row_addr[SPI_NAND_MAX_OP];
    u32 col_addr[SPI_NAND_MAX_OP];
    u8 cur_addr_idx;

    u8 read_cache_mode;
    u8 write_cache_mode;
};

