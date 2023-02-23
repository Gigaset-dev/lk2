/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <nand_base.h>
#include <nand_chip.h>
#include <nand_device.h>
#include "nand_slc.h"
#include <nandx_core.h>
#include <nandx_util.h>
#include <nfi.h>

#include "device_slc.h"

static int nand_slc_read_status_enhanced(struct nand_base *nand,
                     int row)
{
    struct device_slc *dev_slc = device_to_slc(nand->dev);
    struct nand_device *dev = nand->dev;
    struct nfi *nfi = nand->nfi;
    u8 status = 0;

    nfi->reset(nfi);
    nfi->send_cmd(nfi, dev_slc->extend_cmds->read_status_enhanced);
    nfi->send_addr(nfi, 0, row, dev->col_cycle, dev->row_cycle);
    nfi->wait_ready(nfi, NAND_WAIT_POLLING, dev->array_timing->tWHR);
    nfi->read_bytes(nfi, &status, 1);

    return status;
}

static int nand_slc_cache_read_page(struct nand_chip *chip,
                    struct nand_ops *ops, int count)
{
    struct nand_base *nand = chip->nand;
    struct nand_device *dev = nand->dev;
    int i, ret = 0;
    int row = 0, col, sectors;
    u8 *data, *oob;

    for (i = 0; i <= count; i++) {
        if (i < count) {
            row = ops[i].row;
            col = ops[i].col;

            nand->addressing(nand, &row, &col);
        }

        if (i == 0) {
            ops[i].status = nand->read_page(nand, row);
            continue;
        }

        if (i == count - 1) {
            ops[i].status = nand->read_last(nand);
            continue;
        }

        ops[i].status = nand->read_cache(nand, row);
        if (ops[i - 1].status < 0) {
            ret = ops[i - 1].status;
            continue;
        }

        row = ops[i - 1].row;
        col = ops[i - 1].col;
        data = ops[i - 1].data;
        oob = ops[i - 1].oob;
        sectors = ops[i - 1].len / chip->sector_size;
        ops[i - 1].status = nand->read_data(nand, row, col, sectors,
                            data, oob);
        if (ops[i - 1].status > 0) {
            ops[i - 1].status =
                ops[i - 1].status >=
                dev->endurance->max_bitflips ? -ENANDFLIPS : 0;
        }

        ret = min_t(int, ret, ops[i - 1].status);
    }

    return ret;
}

static int nand_slc_read_multi(struct nand_base *nand, int row)
{
    struct device_slc *dev_slc = device_to_slc(nand->dev);
    struct nand_device *dev = nand->dev;
    struct nfi *nfi = nand->nfi;

    nfi->reset(nfi);
    nfi->send_cmd(nfi, dev_slc->extend_cmds->read_multi_1st);
    nfi->send_addr(nfi, 0, row, dev->col_cycle, dev->row_cycle);
    nfi->send_cmd(nfi, dev_slc->extend_cmds->read_multi_2nd);
    nfi->trigger(nfi);

    return nfi->wait_ready(nfi, NAND_WAIT_POLLING,
                   dev->array_timing->tDBSY);
}

static int nand_slc_multi_read_page(struct nand_chip *chip,
                    struct nand_ops *ops, int count)
{
    struct nand_base *nand = chip->nand;
    struct nand_slc *slc = base_to_slc(nand);
    struct nand_device *dev = nand->dev;
    int row[2], col[2], sectors;
    int i, j, ret = 0;
    u8 *data, *oob;

    for (i = 0; i < count; i += 2) {
        for (j = 0; j < 2; j++) {
            row[j] = ops[i + j].row;
            col[j] = ops[i + j].col;

            nand->addressing(nand, &row[j], &col[j]);
        }

        ops[i].status = slc->read_multi(nand, row[0]);
        if (ops[i].status < 0) {
            ret = ops[i].status;
            continue;
        }

        ops[i + 1].status = nand->read_page(nand, row[1]);
        if (ops[i + 1].status < 0) {
            ret = ops[i + 1].status;
            continue;
        }

        for (j = 0; j < 2; j++) {
            data = ops[i + j].data;
            oob = ops[i + j].oob;
            sectors = ops[i + j].len / chip->sector_size;
            ops[i + j].status = nand->read_data(nand, row[j],
                                col[j], sectors,
                                data, oob);
            if (ops[i + j].status > 0) {
                ops[i + j].status =
                    ops[i + j].status >=
                    dev->endurance->max_bitflips ?
                    -ENANDFLIPS : 0;
            }

            ret = min_t(int, ret, ops[i + j].status);
        }
    }

    return ret;
}

static int nand_chip_slc_read_page(struct nand_chip *chip,
                   struct nand_ops *ops,
                   int count)
{
    struct nand_base *nand = chip->nand;
    struct nand_slc *slc = base_to_slc(nand);

    if (slc->cache)
        return slc->cache_read_page(chip, ops, count);

    if (slc->multi)
        return slc->multi_read_page(chip, ops, count);

    return slc->read_page(chip, ops, count);
}

static int nand_slc_cache_write_page(struct nand_chip *chip,
                     struct nand_ops *ops, int count)
{
    struct nand_base *nand = chip->nand;
    struct nand_device *dev = nand->dev;
    int i, ret = 0;
    int row, col;
    u8 *data, *oob;

    for (i = 0; i < count; i++) {
        row = ops[i].row;
        col = ops[i].col;

        nand->addressing(nand, &row, &col);

        ops[i].status = nand->write_enable(nand);
        if (ops[i].status) {
            pr_err("Write Protect at %x!\n", row);
            ops[i].status = -ENANDWP;
            return -ENANDWP;
        }

        data = ops[i].data;
        oob = ops[i].oob;
        ops[i].status = nand->program_data(nand, row, col, data, oob);
        if (ops[i].status < 0) {
            ret = ops[i].status;
            continue;
        }

        if (i == count - 1)
            ops[i].status = nand->program_page(nand, -1);
        else
            ops[i].status = nand->program_cache(nand);
        if (ops[i].status < 0) {
            ret = ops[i].status;
            continue;
        }

        ops[i].status = nand->read_status(nand);
        if (ops[i].status & dev->status->program_fail)
            ops[i].status = -ENANDWRITE;

        ret = min_t(int, ret, ops[i].status);
    }

    return ret;
}

static int nand_slc_program_multi(struct nand_base *nand)
{
    struct device_slc *dev_slc = device_to_slc(nand->dev);
    struct nand_device *dev = nand->dev;
    struct nfi *nfi = nand->nfi;

    nfi->reset(nfi);
    nfi->send_cmd(nfi, dev_slc->extend_cmds->program_multi_2nd);
    nfi->trigger(nfi);

    return nfi->wait_ready(nfi, NAND_WAIT_POLLING,
                   dev->array_timing->tDBSY);
}

static int nand_slc_multi_write_page(struct nand_chip *chip,
                     struct nand_ops *ops, int count)
{
    struct nand_base *nand = chip->nand;
    struct nand_slc *slc = base_to_slc(nand);
    struct nand_device *dev = nand->dev;
    int i, j, idx, ret = 0;
    int row[2], col[2];
    u8 *data, *oob;

    for (i = 0; i < count; i++) {
        idx = i & 1;
        row[idx] = ops[i].row;
        col[idx] = ops[i].col;

        nand->addressing(nand, &row[idx], &col[idx]);

        ops[i].status = nand->write_enable(nand);
        if (ops[i].status) {
            pr_err("Write Protect at %x!\n", row[idx]);
            ops[i].status = -ENANDWP;
            return -ENANDWP;
        }

        data = ops[i].data;
        oob = ops[i].oob;
        ops[i].status = nand->program_data(nand, row[idx], col[idx],
                           data, oob);
        if (ops[i].status < 0) {
            ret = ops[i].status;
            continue;
        }

        if (idx)
            ops[i].status = slc->program_multi(nand);
        else
            ops[i].status = nand->program_page(nand, -1);

        if (ops[i].status < 0) {
            ret = ops[i].status;
            continue;
        }

        for (j = 0; j < 2 && idx == 1; j++) {
            ops[i + j - 1].status =
                slc->read_status_enhanced(nand, row[j]);
            if (ops[i + j - 1].status & dev->status->program_fail)
                ops[i + j - 1].status = -ENANDWRITE;

            ret = min_t(int, ret, ops[i + j - 1].status);
        }
    }

    return ret;
}

static int nand_chip_slc_write_page(struct nand_chip *chip,
                    struct nand_ops *ops, int count)
{
    struct nand_base *nand = chip->nand;
    struct nand_slc *slc = base_to_slc(nand);

    if (slc->cache)
        return slc->cache_write_page(chip, ops, count);

    if (slc->multi)
        return slc->multi_write_page(chip, ops, count);

    return slc->write_page(chip, ops, count);
}

static int nand_slc_erase_multi(struct nand_base *nand, int row)
{
    struct device_slc *dev_slc = device_to_slc(nand->dev);
    struct nand_device *dev = nand->dev;
    struct nfi *nfi = nand->nfi;

    nfi->reset(nfi);
    nfi->send_cmd(nfi, dev_slc->extend_cmds->erase_multi_1st);
    nfi->send_addr(nfi, 0, row, dev->col_cycle, dev->row_cycle);
    nfi->send_cmd(nfi, dev_slc->extend_cmds->erase_multi_2nd);
    nfi->trigger(nfi);

    return nfi->wait_ready(nfi, NAND_WAIT_POLLING,
                   dev->array_timing->tDBSY);
}

static int nand_slc_multi_erase_block(struct nand_chip *chip,
                      struct nand_ops *ops, int count)
{
    struct nand_base *nand = chip->nand;
    struct nand_slc *slc = base_to_slc(nand);
    struct nand_device *dev = nand->dev;
    int i, j, idx, ret = 0;
    int row[2], col[2];

    for (i = 0; i < count; i++) {
        idx = i & 1;
        row[idx] = ops[i].row;
        col[idx] = ops[i].col;

        nand->addressing(nand, &row[idx], &col[idx]);

        ops[i].status = nand->write_enable(nand);
        if (ops[i].status) {
            pr_err("Write Protect at %x!\n", row[idx]);
            ops[i].status = -ENANDWP;
            return -ENANDWP;
        }

        if (idx == 1)
            ops[i].status = nand->erase_block(nand, row[idx]);
        else
            ops[i].status = slc->erase_multi(nand, row[idx]);

        if (ops[i].status < 0) {
            ret = min_t(int, ret, ops[i].status);
            continue;
        }

        for (j = 0; j < 2 && idx == 1; j++) {
            ops[i + j - 1].status =
                slc->read_status_enhanced(nand, row[j]);
            if (ops[i + j - 1].status & dev->status->program_fail)
                ops[i + j - 1].status = -ENANDWRITE;

            ret = min_t(int, ret, ops[i + j - 1].status);
        }
    }

    return ret;
}

static int nand_chip_slc_erase_block(struct nand_chip *chip,
                     struct nand_ops *ops, int count)
{
    struct nand_base *nand = chip->nand;
    struct nand_slc *slc = base_to_slc(nand);

    if (slc->multi)
        return slc->multi_erase_block(chip, ops, count);

    return slc->erase_block(chip, ops, count);
}

static int nand_slc_set_format(struct nand_base *nand)
{
    struct nfi_format format = {
        nand->dev->page_size,
        nand->dev->spare_size,
        nand->dev->endurance->ecc_req
    };

    return nand->nfi->set_format(nand->nfi, &format);
}

static int nand_slc_set_timing(struct nand_base *nand, int timing_mode)
{
    struct device_slc *dev_slc = device_to_slc(nand->dev);
    struct nand_slc *slc = base_to_slc(nand);
    struct nand_sdr_timing *timing;
    struct slc_timing_mode *mode;
    struct nfi *nfi = nand->nfi;
    u32 val, back;
    int ret;

    if (timing_mode < 0 || timing_mode >= SLC_TIMING_NUM)
        return -EINVAL;

    slc->timing_mode = timing_mode;
    timing = &dev_slc->timing[timing_mode];

    if (dev_slc->timing_mode) {
        mode = dev_slc->timing_mode;
        val = (u32)mode->mode[timing_mode];
        ret = nand->set_feature(nand, mode->addr, (u8 *)&val, 4);
        if (ret)
            return ret;

        ret = nand->get_feature(nand, mode->addr,
                    (u8 *)&back, 4);
        if (ret)
            return ret;

        if (mode->mode[timing_mode] != back)
            return -EFAULT;
    }

    return nfi->set_timing(nfi, timing, NAND_TIMING_SDR);
}

static int nand_slc_set_drive(struct nand_base *nand, int drive_level)
{
    struct device_slc *dev_slc = device_to_slc(nand->dev);
    struct slc_drive_strength *drive = &dev_slc->drive_strength;
    u8 back[4] = { 0xff, 0xff, 0xff, 0xff };
    u8 value[4] = { 0 };

    switch (drive_level) {
    case CHIP_DRIVE_NORMAL:
        value[0] = drive->normal;
        break;

    case CHIP_DRIVE_HIGH:
        value[0] = drive->high;
        break;

    case CHIP_DRIVE_MIDDLE:
        value[0] = drive->middle;
        break;

    case CHIP_DRIVE_LOW:
        value[0] = drive->low;
        break;

    default:
        return -EOPNOTSUPP;
    }

    nand->set_feature(nand, drive->addr, value, 4);
    nand->get_feature(nand, drive->addr, back, 4);

    if (value[0] != back[0])
        return -EFAULT;

    return 0;
}

static int nand_chip_slc_ctrl(struct nand_chip *chip, int cmd,
                  void *args)
{
    struct nand_base *nand = chip->nand;
    struct nand_slc *slc = base_to_slc(nand);
    struct nfi *nfi = nand->nfi;
    int ret = 0, value = *(int *)args;

    switch (cmd) {
    case CHIP_CTRL_DRIVE_STRENGTH:
        ret = nand_slc_set_drive(nand, value);
        break;

    case CHIP_CTRL_TIMING_MODE:
        ret = nand_slc_set_timing(nand, value);
        break;

    case CHIP_CTRL_OPS_CACHE:
        slc->cache = value ? true : false;
        break;

    case CHIP_CTRL_OPS_MULTI:
        if (chip->plane_num > 1)
            slc->multi = value ? true : false;
        else
            ret = -EOPNOTSUPP;
        break;

    case CHIP_CTRL_PSLC_MODE:
    case CHIP_CTRL_DDR_MODE:
    case CHIP_CTRL_ONDIE_ECC:
        ret = -EOPNOTSUPP;
        break;

    default:
        ret = nfi->nfi_ctrl(nfi, cmd, args);
        break;
    }

    return ret;
}

static int nand_chip_slc_resume(struct nand_chip *chip)
{
    struct nand_base *nand = chip->nand;
    struct nand_slc *slc = base_to_slc(nand);

    nand->reset(nand);

    nand_slc_set_format(nand);

    return nand_slc_set_timing(nand, slc->timing_mode);
}

struct nand_base *nand_init(struct nand_chip *chip)
{
    struct nand_base *nand;
    struct nand_slc *slc;
    struct device_slc *dev_slc;
    int ret;

    slc = mem_alloc(1, sizeof(struct nand_slc));
    if (!slc) {
        pr_err("alloc nand_slc fail\n");
        return NULL;
    }

    slc->parent = chip->nand;
    nand = &slc->base;
    memcpy(nand, slc->parent, sizeof(struct nand_base));

    slc->cache = slc->multi = false;

    slc->read_page = chip->read_page;
    slc->write_page = chip->write_page;
    slc->erase_block = chip->erase_block;

    slc->read_multi = nand_slc_read_multi;
    slc->program_multi = nand_slc_program_multi;
    slc->erase_multi = nand_slc_erase_multi;
    slc->read_status_enhanced = nand_slc_read_status_enhanced;

    slc->cache_read_page = nand_slc_cache_read_page;
    slc->multi_read_page = nand_slc_multi_read_page;
    slc->cache_write_page = nand_slc_cache_write_page;
    slc->multi_write_page = nand_slc_multi_write_page;
    slc->multi_erase_block = nand_slc_multi_erase_block;

    chip->read_page = nand_chip_slc_read_page;
    chip->write_page = nand_chip_slc_write_page;
    chip->erase_block = nand_chip_slc_erase_block;
    chip->chip_ctrl = nand_chip_slc_ctrl;
    chip->resume = nand_chip_slc_resume;
    chip->nand_type = NAND_SLC;

    ret = nand_detect_device(nand);
    if (ret)
        goto error;

    ret = nand_slc_set_format(nand);
    if (ret)
        goto error;

    dev_slc = device_to_slc(nand->dev);
    ret = nand_slc_set_timing(nand, dev_slc->default_mode);
    if (ret)
        goto error;

    return nand;

error:
    mem_free(slc);
    return NULL;
}

void nand_exit(struct nand_base *nand)
{
    struct nand_slc *slc = base_to_slc(nand);

    nand_base_exit(slc->parent);
    mem_free(nand);
}
