/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <bits.h>
#include <reg.h>
#include <stdbool.h>
#include <sys/types.h>

#define nandx_bit(bit) BIT(ULONG_MAX, bit)

struct nandx_split64 {
    u64 head;
    size_t head_len;
    u64 body;
    size_t body_len;
    u64 tail;
    size_t tail_len;
};

struct nandx_split32 {
    u32 head;
    u32 head_len;
    u32 body;
    u32 body_len;
    u32 tail;
    u32 tail_len;
};

#ifndef do_div
#define do_div(n, base) \
    ({ \
        u32 __base = (base); \
        u32 __rem; \
        __rem = ((u64)(n)) % __base; \
        (n) = ((u64)(n)) / __base; \
        __rem; \
    })
#endif

#define div_up(x, y) \
    ({ \
        u64 __temp = ((x) + (y) - 1); \
        do_div(__temp, (y)); \
        __temp; \
    })

#define div_down(x, y) \
    ({ \
        u64 __temp = (x); \
        do_div(__temp, (y)); \
        __temp; \
    })
#define div_round_up(x, y)      (div_up(x, y) * (y))
#define div_round_down(x, y)    (div_down(x, y) * (y))

#define nandx_split(split, offset, len, val, align) \
    do { \
        (split)->head = (offset); \
        (val) = div_round_down((offset), (align)); \
        (val) = (align) - ((offset) - (val)); \
        if ((val) == (align)) \
            (split)->head_len = 0; \
        else if ((val) > (len)) \
            (split)->head_len = len; \
        else \
            (split)->head_len = val; \
        (split)->body = (offset) + (split)->head_len; \
        (split)->body_len = div_round_down((len) - \
                           (split)->head_len,\
                           (align)); \
        (split)->tail = (split)->body + (split)->body_len; \
        (split)->tail_len = (len) - (split)->head_len - \
                    (split)->body_len; \
    } while (0)

static inline void nandx_set_bits32(unsigned long addr, u32 mask,
                    u32 val)
{
    u32 temp = readl((void *)addr);

    temp &= ~(mask);
    temp |= val;
    writel(temp, (void *)addr);
}

enum nand_type {
    NAND_SPI,
    NAND_SLC,
    NAND_MLC,
    NAND_TLC
};

/**
 * mtk_ic_version - indicates specifical IC, IP need this to load some info
 */
enum mtk_ic_version {
    NANDX_MT6880,
};

/**
 * nandx_ioctl_cmd - operations supported by nandx
 *
 * @NFI_CTRL_DMA dma enable or not
 * @NFI_CTRL_NFI_MODE customer/read/program/erase...
 * @NFI_CTRL_ECC ecc enable or not
 * @NFI_CTRL_ECC_MODE nfi/dma/pio
 * @CHIP_CTRL_DRIVE_STRENGTH enum chip_ctrl_drive_strength
 */
enum nandx_ctrl_cmd {
    CORE_CTRL_NAND_INFO,

    NFI_CTRL_DMA,
    NFI_CTRL_NFI_MODE,
    NFI_CTRL_AUTOFORMAT,
    NFI_CTRL_NFI_IRQ,
    NFI_CTRL_PAGE_IRQ,
    NFI_CTRL_RANDOMIZE,
    NFI_CTRL_BAD_MARK_SWAP,
    NFI_CTRL_IOCON,

    NFI_CTRL_ECC,
    NFI_CTRL_ECC_MODE,
    NFI_CTRL_ECC_CLOCK,
    NFI_CTRL_ECC_IRQ,
    NFI_CTRL_ECC_PAGE_IRQ,
    NFI_CTRL_ECC_DECODE_MODE,

    SNFI_CTRL_OP_MODE,
    SNFI_CTRL_RX_MODE,
    SNFI_CTRL_TX_MODE,
    SNFI_CTRL_DELAY_MODE,

    CHIP_CTRL_OPS_CACHE,
    CHIP_CTRL_OPS_MULTI,
    CHIP_CTRL_PSLC_MODE,
    CHIP_CTRL_DRIVE_STRENGTH,
    CHIP_CTRL_DDR_MODE,
    CHIP_CTRL_ONDIE_ECC,
    CHIP_CTRL_TIMING_MODE
};

/**
 * nandx_info - basic information
 */
struct nandx_info {
    u32 max_io_count;
    u32 min_write_pages;
    u32 plane_num;
    u32 oob_size;
    u32 page_parity_size;
    u32 page_size;
    u32 block_size;
    u64 total_size;
    u32 fdm_reg_size;
    u32 fdm_ecc_size;
    u32 ecc_strength;
    u32 sector_size;
};

/**
 * nfi_resource - the resource needed by nfi & ecc to do initialization
 */
struct nfi_resource {
    enum mtk_ic_version ic_ver;
    void *dev;

    void *ecc_regs;
    int ecc_irq_id;

    void *nfi_regs;
    int nfi_irq_id;

    u32 clock_1x;
    u32 *clock_2x;
    int clock_2x_num;

    int min_oob_req;

    u8 nand_type;
};

/**
 * nandx_init - init all related modules below
 *
 * @res: basic resource of the project
 *
 * return 0 if init success, otherwise return negative error code
 */
int nandx_init(struct nfi_resource *res);

/**
 * nandx_exit - release resource those that obtained in init flow
 */
void nandx_exit(void);

/**
 * nandx_read - read data from nand this function can read data and related
 *   oob from specifical address
 *   if do multi_ops, set one operation per time, and call nandx_sync at last
 *   in multi mode, not support page partial read
 *   oob not support partial read
 *
 * @data: buf to receive data from nand
 * @oob: buf to receive oob data from nand which related to data page
 *   length of @oob should oob size aligned, oob not support partial read
 * @offset: offset address on the whole flash
 * @len: the length of @data that need to read
 *
 * if read success return 0, otherwise return negative error code
 */
int nandx_read(u8 *data, u8 *oob, u64 offset, size_t len);

/**
 * nandx_write -  write data to nand
 *   this function can write data and related oob to specifical address
 *   if do multi_ops, set one operation per time, and call nandx_sync at last
 *
 * @data: source data to be written to nand,
 *   for multi operation, the length of @data should be page size aliged
 * @oob: source oob which related to data page to be written to nand,
 *   length of @oob should oob size aligned
 * @offset: offset address on the whole flash, the value should be start address
 *   of a page
 * @len: the length of @data that need to write,
 *   for multi operation, the len should be page size aliged
 *
 * if write success return 0, otherwise return negative error code
 * if return value > 0, it indicates that how many pages still need to write,
 * and data has not been written to nand
 * please call nandx_sync after pages aligned $nandx_info.min_write_pages
 */
int nandx_write(u8 *data, u8 *oob, u64 offset, size_t len);

/**
 * nandx_erase - erase an area of nand
 *   if do multi_ops, set one operation per time, and call nandx_sync at last
 *
 * @offset: offset address on the flash
 * @len: erase length which should be block size aligned
 *
 * if erase success return 0, otherwise return negative error code
 */
int nandx_erase(u64 offset, size_t len);

/**
 * nandx_sync - sync all operations to nand
 *   when do multi_ops, this function will be called at last operation
 *   when write data, if number of pages not aligned
 *   by $nandx_info.min_write_pages, this interface could be called to do
 *   force write, 0xff will be padded to blanked pages.
 */
int nandx_sync(void);

/**
 * nandx_is_bad_block - check if the block is bad
 *   only check the flag that marked by the flash vendor
 *
 * @offset: offset address on the whole flash
 *
 * return true if the block is bad, otherwise return false
 */
bool nandx_is_bad_block(u64 offset);

/**
 * nandx_ioctl - set/get property of nand chip
 *
 * @cmd: parameter that defined in enum nandx_ioctl_cmd
 * @arg: operate parameter
 *
 * return 0 if operate success, otherwise return negative error code
 */
int nandx_ioctl(enum nandx_ctrl_cmd cmd, void *arg);

/**
 * nandx_suspend - suspend nand, and store some data
 *
 * return 0 if suspend success, otherwise return negative error code
 */
int nandx_suspend(void);

/**
 * nandx_resume - resume nand, and replay some data
 *
 * return 0 if resume success, otherwise return negative error code
 */
int nandx_resume(void);

/**
 * nandx_unit_test - unit test
 *
 * @offset: offset address on the whole flash
 * @len: should be not larger than a block size, we only test a block per time
 *
 * return 0 if test success, otherwise return negative error code
 */
int nandx_unit_test(u64 offset, size_t len);

int scan_bbt(struct nandx_info *nand);

int bbt_mark_bad(struct nandx_info *nand, off_t offset);
int bbt_is_bad(struct nandx_info *nand, off_t offset);
