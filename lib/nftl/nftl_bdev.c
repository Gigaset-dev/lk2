/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <err.h>
#include <errno.h>
#include <lib/bio.h>
#include <lib/nftl.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#define BBT_RESERVED_BLOCK  4
struct bdev *nftl_dev;

static int nftl_bio_block_mapping(struct bdev *dev, struct nftl_info *info,
                                  bnum_t *page)
{
    struct nftl_info *part;
    u64 offset, start = 0;
    u32 ppb = info->erase_size / info->write_size;
    u32 block_count = dev->block_count * (dev->block_size / info->write_size);
    int ret = 0;

    offset = (u64)info->write_size * (*page);
    part = nftl_search_by_address(offset, &start);
    if (part == NULL) {
        /* block not in partitions */
        ret = nftl_block_isbad(info, *page);
        if (ret)
            ret = ERR_NOT_FOUND;
    } else {
        offset -= start;
        *page = offset / info->write_size;
        ret = nftl_block_mapping(part, page);
        *page += start / info->write_size;
    }

    return ret;
}

static int nftl_bio_move_worn_bad_blk(struct bdev *dev, struct nftl_info *info,
                                  bnum_t *page)
{
    u32 dst_page, src_page;
    u32 pages_per_block;
    void *buf;
    int ret = 0;

    buf = malloc(info->write_size);
    if (!buf)
        return -ENOMEM;

    pages_per_block = info->erase_size / info->write_size;
    dst_page = *(bnum_t *)page;

findgood:
    src_page = (*(bnum_t *)page / pages_per_block) * pages_per_block;

    /* Find next good block */
    ret = nftl_bio_block_mapping(dev, info, &dst_page);
    if (ret) {
        dprintf(CRITICAL, "out of part range!\n");
        goto freebuf;
    }

    /* Erase good block */
    ret = nftl_erase(info, dst_page * info->write_size, info->erase_size);
#if MTK_NAND_WORN_BAD_TEST
        if (nand_check_erase_test_block((dst_page * info->write_size) / info->erase_size)) {
            dprintf(INFO, "Simulate erase fail case at offset %llu",
                (dst_page * info->write_size) / info->erase_size);
            ret = -EIO;
        }
#endif
    if (ret == -EIO) {
        dprintf(CRITICAL, "Erase fail at page %d\n", dst_page);
        nftl_block_markbad(info, dst_page);
        goto findgood;
    } else if ((u32)ret != info->erase_size) {
        dprintf(CRITICAL, "erase error %d\n", ret);
        goto freebuf;
    }

    /* Move programmed data to new good block */
    while (src_page < dst_page) {
        if (nftl_read(info, buf, src_page, info->write_size) < 0) {
            /*
             * Programmed data read error and data lost
             * Skip it and go to read next page
             */
            dprintf(INFO, "page %d data lost!\n", src_page);
            src_page++;
            dst_page++;
            continue;
        }

        ret = nftl_write(info, buf, dst_page, info->write_size);
#if MTK_NAND_WORN_BAD_TEST
        if (nand_check_program_test_page(dst_page)) {
            dprintf(INFO, "Simulate program fail case at page %u", dst_page);
            ret = -EIO;
        }
#endif
        if (ret == -EIO) {
            /* Program failed, mark bad and go to find good block */
            nftl_block_markbad(info, dst_page);
            goto findgood;
        } else if ((u32)ret != (info->write_size)) {
            /* return directly if other errors happened */
            dprintf(CRITICAL, "write error %d", ret);
            goto freebuf;
        }

        src_page++;
        dst_page++;
    }

    /* Mark worn block as bad */
    nftl_block_markbad(info, *(bnum_t *)page);

freebuf:
    free(buf);

    return ret < 0 ? ret : dst_page;
}


static ssize_t nftl_bio_read_block(struct bdev *dev, void *buf, bnum_t block,
                                   uint count)
{
    struct nftl_info *info = nftl_open(dev->name);
    uint32_t page_per_block;
    uint32_t read_count;
    ssize_t err = 0, bytes_read = 0;
    bnum_t phy_block;

    if (!info) {
        dprintf(CRITICAL, "%s: can't open nftl %s\n", __func__, dev->name);
        return -ENODEV;
    }

    page_per_block = info->erase_size / info->write_size;

    while (count) {
        read_count = page_per_block - (block % page_per_block);
        read_count = MIN(read_count, count);
        phy_block = block;
        err = nftl_bio_block_mapping(dev, info, &phy_block);
        if (err) {
            dprintf(CRITICAL, "out of part range!\n");
            break;
        }
        err = nftl_read(info, buf, phy_block * info->write_size,
                        read_count * info->write_size);
        if (err < 0)
            break;
        bytes_read += read_count * info->write_size;
        count -= read_count;
        block += read_count;
        buf += read_count * info->write_size;
    }

    return (err < 0) ? err : bytes_read;
}

static ssize_t nftl_bio_write_block(struct bdev *dev, const void *buf,
                                    bnum_t block, uint count)
{
    struct nftl_info *info = nftl_open(dev->name);
    uint32_t page_per_block;
    uint32_t write_count;
    ssize_t err = 0, bytes_write = 0;
    bnum_t phy_block;

    if (!info) {
        dprintf(CRITICAL, "%s: can't open nftl %s\n", __func__, dev->name);
        return -ENODEV;
    }

    page_per_block = info->erase_size / info->write_size;

    while (count) {
        write_count = page_per_block - (block % page_per_block);
        write_count = MIN(write_count, count);
        phy_block = block;
        err = nftl_bio_block_mapping(dev, info, &phy_block);
        if (err)
                dprintf(CRITICAL, "out of part range!\n");

        if (err < 0)
            break;
REWRITE:
        write_count = page_per_block - (block % page_per_block);
        write_count = MIN(write_count, count);
        err = nftl_write(info, buf, phy_block * info->write_size,
                         write_count * info->write_size);
        if (err == -EIO) {
            /* move worn bad block */
            dprintf(CRITICAL, "Write fail at blk %d\n", phy_block);
            err = nftl_bio_move_worn_bad_blk(dev, info, &phy_block);
            if (err < 0) {
                dprintf(CRITICAL, "Fail to recover worn bad block, err:%d!\n", (int)err);
                return err;
            } else {
                dprintf(CRITICAL, "Recover worn bad block successfully!\n");
                block = err;
                phy_block = block;
            }
            goto REWRITE;
        } else if (err != (ssize_t)(write_count * info->write_size)) {
            dprintf(CRITICAL, "Write error %d\n", (int)err);
            return err;
        }
        bytes_write += write_count * info->write_size;
        count -= write_count;
        block += write_count;
        buf += write_count * info->write_size;
    }

    return (err < 0) ? err : bytes_write;
}

static ssize_t nftl_bio_erase(struct bdev *dev, off_t offset, size_t len)
{
    u32 blocks;
    u32 good_count = 0, bad_count = 0, worn_count = 0;
    ssize_t erase_len = 0, ret = 0;
    struct nftl_info *info = nftl_open(dev->name);
    bnum_t erase_page;

    if (!info) {
        dprintf(CRITICAL, "%s: can't open nftl %s\n", __func__, dev->name);
        return -ENODEV;
    }


    if (offset % info->erase_size)
        return ERR_INVALID_ARGS;
    if (len % info->erase_size)
        return ERR_INVALID_ARGS;
    if (len == 0)
        return 0;

    blocks = len / info->erase_size;

    while (blocks) {
        erase_page = offset / info->write_size;
        if (!nftl_block_isbad(info, erase_page)) {
            ret = nftl_erase(info, erase_page * info->write_size, info->erase_size);
            if (ret == -EIO) {
                dprintf(CRITICAL, "Erase fail at page %d\n", erase_page);
                nftl_block_markbad(info, erase_page);
                worn_count++;
            } else if (ret != (ssize_t)info->erase_size) {
                dprintf(CRITICAL, "erase error %d\n", (int)ret);
                return ret;
            } else {
                good_count++;
            }
        } else {
            dprintf(CRITICAL, "block%d is bad skip it.\n", erase_page / info->erase_size);
            bad_count++;
        }
        erase_len += info->erase_size;
        offset += info->erase_size;
        blocks--;
    }

    dprintf(CRITICAL, "bad block count %u, erased good block count %u, worn bad count %u",
            bad_count, good_count, worn_count);

    return (ret < 0) ? ret : erase_len;
}

static int nftl_bio_ioctl(struct bdev *dev, int request, void *argp)
{
    struct nftl_info *info = nftl_open(dev->name);

    if (!info) {
        dprintf(CRITICAL, "%s: can't open nftl %s\n", __func__, dev->name);
        return -ENODEV;
    }

    return nftl_ioctl(info, request, argp);
}

int nftl_mount_bdev(struct nftl_info *info)
{
    struct bdev *dev;
    u32 lba_count, lba_size;

    dev = (struct bdev *)malloc(sizeof(struct bdev));
    if (!dev) {
        dprintf(CRITICAL, "%s: no enough memory\n", __func__);
        return -ENOMEM;
    }

    memset(dev, 0, sizeof(struct bdev));
    lba_size = info->write_size;
    lba_count = info->total_size / lba_size;
    /* reserve 4blocks for bbt and 4blocks for SGPT */
    lba_count -= (BBT_RESERVED_BLOCK * info->erase_size / lba_size);

    bio_initialize_bdev(dev, info->name, lba_size, lba_count, 0, NULL, BIO_FLAGS_NONE);
    dev->read_block = nftl_bio_read_block;
    dev->write_block = nftl_bio_write_block;
    dev->ioctl = nftl_bio_ioctl;
    dev->erase = nftl_bio_erase;
    dev->erase_byte = 0xff;
    bio_register_device(dev);

    nftl_dev = dev;

    return 0;
}
