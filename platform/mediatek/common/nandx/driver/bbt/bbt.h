/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#define BBT_BLOCK_GOOD          0x03
#define BBT_BLOCK_WORN          0x02
#define BBT_BLOCK_RESERVED      0x01
#define BBT_BLOCK_FACTORY_BAD   0x00

#define BBT_INVALID_ADDR        0
/* The maximum number of blocks to scan for a bbt */
#define NAND_BBT_SCAN_MAXBLOCKS 4
#define NAND_BBT_USE_FLASH      0x00020000
#define NAND_BBT_NO_OOB         0x00040000

/* Search good / bad pattern on the first and the second page */
#define NAND_BBT_SCAN2NDPAGE    0x00008000
/* Search good / bad pattern on the last page of the eraseblock */
#define NAND_BBT_SCANLASTPAGE   0x00010000

#define NAND_DRAM_BUF_DATABUF_ADDR  (NAND_BUF_ADDR)

struct bbt_pattern {
    u8 *data;
    int len;
};

struct bbt_desc {
    struct bbt_pattern pattern;
    u8 version;
    u64 bbt_addr;/*0: invalid value; otherwise, valid value*/
};

struct bbt_manager {
    /* main bbt descriptor and mirror descriptor */
    struct bbt_desc desc[2];/* 0: main bbt; 1: mirror bbt */
    int max_blocks;
    u8 *bbt;
};

#define BBT_ENTRY_MASK          0x03
#define BBT_ENTRY_SHIFT         2

#define GET_BBT_LENGTH(blocks) (blocks >> 2)
#define GET_ENTRY(block) ((block) >> BBT_ENTRY_SHIFT)
#define GET_POSITION(block) (((block) & BBT_ENTRY_MASK) * 2)
#define GET_MARK_VALUE(block, mark) \
    (((mark) & BBT_ENTRY_MASK) << GET_POSITION(block))

int scan_bbt(struct nandx_info *nand);

int bbt_mark_bad(struct nandx_info *nand, off_t offset);

int bbt_is_bad(struct nandx_info *nand, off_t offset);

