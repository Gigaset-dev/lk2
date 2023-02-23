/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once
#include <nand_base.h>

/*
 * spi nand handler
 * @base: spi nand base functions
 * @parent: common parent nand base functions
 * @tx_mode: spi bus width of transfer to device
 * @rx_mode: spi bus width of transfer from device
 * @op_mode: spi nand controller (NFI) operation mode
 * @ondie_ecc: spi nand on-die ecc flag
 */

struct nand_spi {
    struct nand_base base;
    struct nand_base *parent;
    u8 tx_mode;
    u8 rx_mode;
    u8 op_mode;
    bool ondie_ecc;
};

static inline struct nand_spi *base_to_spi(struct nand_base *base)
{
    return container_of(base, struct nand_spi, base);
}

