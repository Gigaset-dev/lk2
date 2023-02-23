/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once
#include <nand_chip.h>
#include <nandx_util.h>

typedef int (*func_chip_ops)(struct nand_chip *, struct nand_ops *,
                 int);

enum nandx_op_mode {
    NANDX_IDLE,
    NANDX_WRITE,
    NANDX_READ,
    NANDX_ERASE
};

struct nandx_desc {
    struct nand_chip *chip;
    struct nandx_info info;
    enum nandx_op_mode mode;

    bool multi_en;
    bool ecc_en;

    struct nand_ops *ops;
    int ops_len;
    int ops_multi_len;
    int ops_current;
    int min_write_pages;

    u8 *head_buf;
    u8 *tail_buf;
};

