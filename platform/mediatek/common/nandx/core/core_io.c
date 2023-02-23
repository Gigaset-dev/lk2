/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

/*NOTE: switch cache/multi*/
#include <nand_chip.h>
#include <nandx_core.h>
#include <nandx_util.h>

#include "core_io.h"

static struct nandx_desc *g_nandx;

static inline bool is_sector_align(u64 val)
{
    return reminder(val, g_nandx->chip->sector_size) ? false : true;
}

static inline bool is_page_align(u64 val)
{
    return reminder(val, g_nandx->chip->page_size) ? false : true;
}

static inline bool is_block_align(u64 val)
{
    return reminder(val, g_nandx->chip->block_size) ? false : true;
}

static inline u32 page_sectors(void)
{
    return div_down(g_nandx->chip->page_size, g_nandx->chip->sector_size);
}

static inline u32 sector_oob(void)
{
    return div_down(g_nandx->chip->oob_size, page_sectors());
}

static inline u32 sector_padded_size(void)
{
    return g_nandx->chip->sector_size + g_nandx->chip->sector_spare_size;
}

static inline u32 page_padded_size(void)
{
    return page_sectors() * sector_padded_size();
}

static inline u32 offset_to_padded_col(u64 offset)
{
    struct nandx_desc *nandx = g_nandx;
    u32 col, sectors;

    col = reminder(offset, nandx->chip->page_size);
    sectors = div_down(col, nandx->chip->sector_size);

    return col + sectors * nandx->chip->sector_spare_size;
}

static inline u32 offset_to_row(u64 offset)
{
    return div_down(offset, g_nandx->chip->page_size);
}

static inline u32 offset_to_col(u64 offset)
{
    return reminder(offset, g_nandx->chip->page_size);
}

static inline u32 oob_upper_size(void)
{
    return g_nandx->ecc_en ? g_nandx->chip->oob_size :
           g_nandx->chip->sector_spare_size * page_sectors();
}

static inline bool is_upper_oob_align(u64 val)
{
    return reminder(val, oob_upper_size()) ? false : true;
}

#define prepare_op(_op, _row, _col, _len, _data, _oob) \
    do { \
        (_op).row = (_row); \
        (_op).col = (_col); \
        (_op).len = (_len); \
        (_op).data = (_data); \
        (_op).oob = (_oob); \
    } while (0)

static int operation_multi(enum nandx_op_mode mode, u8 *data, u8 *oob,
               u64 offset, size_t len)
{
    struct nandx_desc *nandx = g_nandx;
    u32 row = offset_to_row(offset);
    u32 col = offset_to_padded_col(offset);

    if (nandx->mode == NANDX_IDLE) {
        nandx->mode = mode;
        nandx->ops_current = 0;
    } else if (nandx->mode != mode) {
        pr_err("forbid mixed operations.\n");
        return -EOPNOTSUPP;
    }

    prepare_op(nandx->ops[nandx->ops_current], row, col, len, data, oob);
    nandx->ops_current++;

    if (nandx->ops_current == nandx->ops_multi_len)
        return nandx_sync();

    return nandx->ops_multi_len - nandx->ops_current;
}

static int operation_sequent(enum nandx_op_mode mode, u8 *data, u8 *oob,
                 u64 offset, size_t len)
{
    struct nandx_desc *nandx = g_nandx;
    struct nand_chip *chip = nandx->chip;
    u32 row = offset_to_row(offset);
    func_chip_ops chip_ops;
    u8 *ref_data = data, *ref_oob = oob;
    int align, ops, row_step;
    int i, rem;

    align = data ? chip->page_size : oob_upper_size();
    ops = data ? div_down(len, align) : div_down(len, oob_upper_size());
    row_step = 1;

    switch (mode) {
    case NANDX_ERASE:
        chip_ops = chip->erase_block;
        align = chip->block_size;
        ops = div_down(len, align);
        row_step = chip->block_pages;
        break;

    case NANDX_READ:
        chip_ops = chip->read_page;
        break;

    case NANDX_WRITE:
        chip_ops = chip->write_page;
        break;

    default:
        return -EINVAL;
    }

    if (!data) {
        ref_data = nandx->head_buf;
        memset(ref_data, 0xff, chip->page_size);
    }

    if (!oob) {
        ref_oob = nandx->head_buf + chip->page_size;
        memset(ref_oob, 0xff, oob_upper_size());
    }

    for (i = 0; i < ops; i++) {
        prepare_op(nandx->ops[nandx->ops_current],
               row + i * row_step, 0, align, ref_data, ref_oob);
        nandx->ops_current++;
        /* if data or oob is null, nandx->head_buf or
         * nandx->head_buf + chip->page_size should not been used
         * so, here it is safe to use the buf.
         */
        ref_data = data ? ref_data + chip->page_size : nandx->head_buf;
        ref_oob = oob ? ref_oob + oob_upper_size() :
              nandx->head_buf + chip->page_size;
    }

    if (nandx->mode == NANDX_WRITE) {
        rem = reminder(nandx->ops_current, nandx->min_write_pages);
        if (rem)
            return nandx->min_write_pages - rem;
    }

    nandx->ops_current = 0;
    return chip_ops(chip, nandx->ops, ops);
}

static int read_pages(u8 *data, u8 *oob, u64 offset, size_t len)
{
    struct nandx_desc *nandx = g_nandx;
    struct nand_chip *chip = nandx->chip;
    struct nandx_split64 split = {0};
    u8 *ref_data = data, *ref_oob;
    u32 row, col;
    int ret = 0, i, ops;
    u32 head_offset = 0;
    u64 val;

    if (!data)
        return operation_sequent(NANDX_READ, NULL, oob, offset, len);

    ref_oob = oob ? oob : nandx->head_buf + chip->page_size;

    nandx_split(&split, offset, len, val, chip->page_size);

    if (split.head_len) {
        row = offset_to_row(split.head);
        col = offset_to_col(split.head);
        prepare_op(nandx->ops[nandx->ops_current], row, 0,
               chip->page_size,
               nandx->head_buf, ref_oob);
        nandx->ops_current++;

        head_offset = col;

        ref_data += split.head_len;
        ref_oob = oob ? ref_oob + oob_upper_size() :
              nandx->head_buf + chip->page_size;
    }

    if (split.body_len) {
        ops = div_down(split.body_len, chip->page_size);
        row = offset_to_row(split.body);
        for (i = 0; i < ops; i++) {
            prepare_op(nandx->ops[nandx->ops_current],
                   row + i, 0, chip->page_size,
                   ref_data, ref_oob);
            nandx->ops_current++;
            ref_data += chip->page_size;
            ref_oob = oob ? ref_oob + oob_upper_size() :
                  nandx->head_buf + chip->page_size;
        }
    }

    if (split.tail_len) {
        row = offset_to_row(split.tail);
        prepare_op(nandx->ops[nandx->ops_current], row, 0,
               chip->page_size, nandx->tail_buf, ref_oob);
        nandx->ops_current++;
    }

    ret = chip->read_page(chip, nandx->ops, nandx->ops_current);

    if (split.head_len)
        memcpy(data, nandx->head_buf + head_offset, split.head_len);
    if (split.tail_len)
        memcpy(ref_data, nandx->tail_buf, split.tail_len);

    nandx->ops_current = 0;
    return ret;
}

int nandx_read(u8 *data, u8 *oob, u64 offset, size_t len)
{
    struct nandx_desc *nandx = g_nandx;

    if (!len || len > nandx->info.total_size)
        return -EINVAL;
    if (div_up(len, nandx->chip->page_size) > (u64)nandx->ops_len)
        return -EINVAL;
    if (!data && !oob)
        return -EINVAL;
    /**
     * as design, oob not support partial read
     * and, the length of oob buf should be oob size aligned
     */
    if (!data && !is_upper_oob_align(len))
        return -EINVAL;

    if (g_nandx->multi_en) {
        /* as design, there only 2 buf for partial read,
         * if partial read allowed for multi read,
         * there are not enough buf
         */
        if (!is_sector_align(offset))
            return -EINVAL;
        if (data && !is_sector_align(len))
            return -EINVAL;
        return operation_multi(NANDX_READ, data, oob, offset, len);
    }

    nandx->ops_current = 0;
    nandx->mode = NANDX_IDLE;
    return read_pages(data, oob, offset, len);
}

static int write_pages(u8 *data, u8 *oob, u64 offset, size_t len)
{
    struct nandx_desc *nandx = g_nandx;
    struct nand_chip *chip = nandx->chip;
    struct nandx_split64 split = {0};
    int ret, rem, i, ops;
    u32 row, col;
    u8 *ref_oob = oob;
    u64 val;

    nandx->mode = NANDX_WRITE;

    if (!data)
        return operation_sequent(NANDX_WRITE, NULL, oob, offset, len);

    if (!oob) {
        ref_oob = nandx->head_buf + chip->page_size;
        memset(ref_oob, 0xff, oob_upper_size());
    }

    nandx_split(&split, offset, len, val, chip->page_size);

    /*NOTE: slc can support sector write, here copy too many data.*/
    if (split.head_len) {
        row = offset_to_row(split.head);
        col = offset_to_col(split.head);
        memset(nandx->head_buf, 0xff, page_padded_size());
        memcpy(nandx->head_buf + col, data, split.head_len);
        prepare_op(nandx->ops[nandx->ops_current], row, 0,
               chip->page_size, nandx->head_buf, ref_oob);
        nandx->ops_current++;

        data += split.head_len;
        ref_oob = oob ? ref_oob + oob_upper_size() :
              nandx->head_buf + chip->page_size;
    }

    if (split.body_len) {
        row = offset_to_row(split.body);
        ops = div_down(split.body_len, chip->page_size);
        for (i = 0; i < ops; i++) {
            prepare_op(nandx->ops[nandx->ops_current],
                   row + i, 0, chip->page_size, data, ref_oob);
            nandx->ops_current++;
            data += chip->page_size;
            ref_oob = oob ? ref_oob + oob_upper_size() :
                  nandx->head_buf + chip->page_size;
        }
    }

    if (split.tail_len) {
        row = offset_to_row(split.tail);
        memset(nandx->tail_buf, 0xff, page_padded_size());
        memcpy(nandx->tail_buf, data, split.tail_len);
        prepare_op(nandx->ops[nandx->ops_current], row, 0,
               chip->page_size, nandx->tail_buf, ref_oob);
        nandx->ops_current++;
    }

    rem = reminder(nandx->ops_current, nandx->min_write_pages);
    if (rem)
        return nandx->min_write_pages - rem;

    ret = chip->write_page(chip, nandx->ops, nandx->ops_current);

    nandx->ops_current = 0;
    nandx->mode = NANDX_IDLE;
    return ret;
}

int nandx_write(u8 *data, u8 *oob, u64 offset, size_t len)
{
    struct nandx_desc *nandx = g_nandx;

    if (!len || len > nandx->info.total_size)
        return -EINVAL;
    if (div_up(len, nandx->chip->page_size) > (u64)nandx->ops_len)
        return -EINVAL;
    if (!data && !oob)
        return -EINVAL;
    if (!data && !is_upper_oob_align(len))
        return -EINVAL;

    if (nandx->multi_en) {
        if (!is_page_align(offset))
            return -EINVAL;
        if (data && !is_page_align(len))
            return -EINVAL;

        return operation_multi(NANDX_WRITE, data, oob, offset, len);
    }

    return write_pages(data, oob, offset, len);
}

int nandx_erase(u64 offset, size_t len)
{
    struct nandx_desc *nandx = g_nandx;

    if (!len || len > nandx->info.total_size)
        return -EINVAL;
    if (div_down(len, nandx->chip->block_size) > (u64)nandx->ops_len)
        return -EINVAL;
    if (!is_block_align(offset) || !is_block_align(len))
        return -EINVAL;

    if (g_nandx->multi_en)
        return operation_multi(NANDX_ERASE, NULL, NULL, offset, len);

    nandx->ops_current = 0;
    nandx->mode = NANDX_IDLE;
    return operation_sequent(NANDX_ERASE, NULL, NULL, offset, len);
}

int nandx_sync(void)
{
    struct nandx_desc *nandx = g_nandx;
    struct nand_chip *chip = nandx->chip;
    func_chip_ops chip_ops;
    int ret, i, rem;

    if (!nandx->ops_current)
        return 0;

    rem = reminder(nandx->ops_current, nandx->ops_multi_len);
    if (nandx->multi_en && rem) {
        ret = -EIO;
        goto error;
    }

    switch (nandx->mode) {
    case NANDX_IDLE:
        return 0;
    case NANDX_ERASE:
        chip_ops = chip->erase_block;
        break;
    case NANDX_READ:
        chip_ops = chip->read_page;
        break;
    case NANDX_WRITE:
        chip_ops = chip->write_page;
        break;
    default:
        return -EINVAL;
    }

    rem = reminder(nandx->ops_current, nandx->min_write_pages);
    if (!nandx->multi_en && nandx->mode == NANDX_WRITE && rem) {
        /* in one process of program, only allow 2 pages to do partial
         * write, here we supposed 1st buf would be used, and 2nd
         * buf should be not used.
         */
        memset(nandx->tail_buf, 0xff,
               chip->page_size + oob_upper_size());
        for (i = 0; i < rem; i++) {
            prepare_op(nandx->ops[nandx->ops_current],
                   nandx->ops[nandx->ops_current - 1].row + 1,
                   0, chip->page_size, nandx->tail_buf,
                   nandx->tail_buf + chip->page_size);
            nandx->ops_current++;
        }
    }

    ret = chip_ops(nandx->chip, nandx->ops, nandx->ops_current);

error:
    nandx->mode = NANDX_IDLE;
    nandx->ops_current = 0;

    return ret;
}

int nandx_ioctl(enum nandx_ctrl_cmd cmd, void *arg)
{
    struct nandx_desc *nandx = g_nandx;
    struct nand_chip *chip = nandx->chip;
    int ret = 0;

    switch (cmd) {
    case CORE_CTRL_NAND_INFO:
        *(struct nandx_info *)arg = nandx->info;
        break;

    case CHIP_CTRL_OPS_MULTI:
        ret = chip->chip_ctrl(chip, cmd, arg);
        if (!ret)
            nandx->multi_en = *(bool *)arg;
        break;

    case NFI_CTRL_ECC:
        ret = chip->chip_ctrl(chip, cmd, arg);
        if (!ret)
            nandx->ecc_en = *(bool *)arg;
        break;

    default:
        ret = chip->chip_ctrl(chip, cmd, arg);
        break;
    }

    return ret;
}

bool nandx_is_bad_block(u64 offset)
{
    struct nandx_desc *nandx = g_nandx;

    prepare_op(nandx->ops[0], offset_to_row(offset), 0,
           nandx->chip->page_size, nandx->head_buf,
           nandx->head_buf + nandx->chip->page_size);

    return nandx->chip->is_bad_block(nandx->chip, nandx->ops, 1);
}

int nandx_suspend(void)
{
    return g_nandx->chip->suspend(g_nandx->chip);
}

int nandx_resume(void)
{
    return g_nandx->chip->resume(g_nandx->chip);
}

int nandx_init(struct nfi_resource *res)
{
    struct nand_chip *chip;
    struct nandx_desc *nandx;
    int ret = 0;

    if (!res)
        return -EINVAL;

    chip = nand_chip_init(res);
    if (!chip) {
        pr_err("nand chip init fail.\n");
        return -EFAULT;
    }

    nandx = (struct nandx_desc *)mem_alloc(1, sizeof(struct nandx_desc));
    if (!nandx) {
        ret = -ENOMEM;
        goto nandx_error;
    }

    g_nandx = nandx;

    nandx->chip = chip;
    nandx->min_write_pages = chip->min_program_pages;
    nandx->ops_multi_len = nandx->min_write_pages * chip->plane_num;
    nandx->ops_len = chip->block_pages * chip->plane_num;
    nandx->ops = mem_alloc(1, sizeof(struct nand_ops) * nandx->ops_len);
    if (!nandx->ops) {
        ret = -ENOMEM;
        goto ops_error;
    }

    nandx->head_buf = mem_alloc(2, page_padded_size());
    if (!nandx->head_buf) {
        ret = -ENOMEM;
        goto buf_error;
    }
    nandx->tail_buf = nandx->head_buf + page_padded_size();
    memset(nandx->head_buf, 0xff, 2 * page_padded_size());
    nandx->multi_en = false;
    nandx->ecc_en = false;
    nandx->ops_current = 0;
    nandx->mode = NANDX_IDLE;

    nandx->info.max_io_count = nandx->ops_len;
    nandx->info.min_write_pages = nandx->min_write_pages;
    nandx->info.plane_num = chip->plane_num;
    nandx->info.oob_size = chip->oob_size;
    nandx->info.page_parity_size = chip->sector_spare_size * page_sectors();
    nandx->info.page_size = chip->page_size;
    nandx->info.block_size = chip->block_size;
    nandx->info.total_size = chip->block_size * chip->block_num;
    nandx->info.fdm_ecc_size = chip->fdm_ecc_size;
    nandx->info.fdm_reg_size = chip->fdm_reg_size;
    nandx->info.ecc_strength = chip->ecc_strength;
    nandx->info.sector_size = chip->sector_size;

    return 0;

buf_error:
    mem_free(nandx->ops);
ops_error:
    mem_free(nandx);
nandx_error:
    nand_chip_exit(chip);

    return ret;
}

void nandx_exit(void)
{
    nand_chip_exit(g_nandx->chip);
    mem_free(g_nandx->head_buf);
    mem_free(g_nandx->ops);
    mem_free(g_nandx);
}

int nandx_unit_test(u64 offset, size_t len)
{
    u8 *src_buf, *dst_buf;
    size_t i;
    int ret;

    if (!len || len > g_nandx->chip->block_size)
        return -EINVAL;

    src_buf = mem_alloc(1, len);
    if (!src_buf)
        return -ENOMEM;

    dst_buf = mem_alloc(1, len);
    if (!dst_buf) {
        mem_free(src_buf);
        return -ENOMEM;
    }

    /*fill random data in source buffer, em... it's not real random data.*/
    for (i = 0; i < len; i++)
        src_buf[i] = (u8)reminder(get_current_time_us(), 255);

    ret = nandx_erase(offset, g_nandx->chip->block_size);
    if (ret < 0) {
        pr_err("erase fail with ret %d\n", ret);
        goto error;
    }

    ret = nandx_write(src_buf, NULL, offset, len);
    if (ret < 0) {
        pr_err("write fail with ret %d\n", ret);
        goto error;
    }

    ret = nandx_read(dst_buf, NULL, offset, len);
    if (ret < 0) {
        pr_err("read fail with ret %d\n", ret);
        goto error;
    }

    for (i = 0; i < len; i++) {
        if (dst_buf[i] != src_buf[i]) {
            pr_err("read after write, check fail\n");
            pr_err("dst_buf should be same as src_buf\n");
            ret = -EIO;
            goto error;
        }
    }

    ret = nandx_erase(offset, g_nandx->chip->block_size);
    if (ret < 0) {
        pr_err("erase fail with ret %d\n", ret);
        goto error;
    }

    ret = nandx_read(dst_buf, NULL, offset, len);
    if (ret < 0) {
        pr_err("read fail with ret %d\n", ret);
        goto error;
    }

    for (i = 0; i < len; i++) {
        if (dst_buf[i] != 0xff) {
            pr_err("read after erase, check fail\n");
            pr_err("all data should be 0xff\n");
            ret = -ENANDERASE;
            goto error;
        }
    }

error:
    mem_free(src_buf);
    mem_free(dst_buf);

    return ret;
}
