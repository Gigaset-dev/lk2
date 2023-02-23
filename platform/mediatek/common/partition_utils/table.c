/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <err.h>
#include <errno.h>
#include <kernel/mutex.h>
#include <lib/bio.h>
#include <lib/partition.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "table.h"
#include <trace.h>

#define LOCAL_TRACE 0

struct partition_tbl_s {
    struct gpt_header *header;
    struct gpt_entry *entry_tbl;
    struct part_meta_info *info;
    int initial;
    int part_cnt;
    mutex_t lock;
} partition_table = {
    .header = NULL,
    .entry_tbl = NULL,
    .info = NULL,
    .initial = 0,
    .part_cnt = 0,
    .lock = MUTEX_INITIAL_VALUE(partition_table.lock),
};

struct gpt_entry *get_entry(void)
{
    return partition_table.entry_tbl;
}

struct gpt_header *get_header(void)
{
    return partition_table.header;
}

struct part_meta_info *get_info(void)
{
    return partition_table.info;
}

int get_part_count(void)
{
    return partition_table.part_cnt;
}

int get_entry_by_name(const char *name, struct gpt_entry **ret)
{
    int i;
    struct gpt_header *header = NULL;
    struct gpt_entry *entry = NULL;
    struct part_meta_info *info = NULL;

    if (!name) {
        LTRACEF("[PART_UTILS] Need buffer with %d entries\n", NUM_PARTITIONS);
        return -EINVAL;
    }

    // reset and get partition table
    if (partition_reset_tbl() < 0)
        return -EIO;

    header = partition_table.header;
    entry = partition_table.entry_tbl;
    info = partition_table.info;
    mutex_acquire(&partition_table.lock);
    //assign partition entries
    for (i = 0; i < (int)header->max_partition_count; i++) {
        if (entry[i].partition_type_guid.b[0] == 0
                && entry[i].partition_type_guid.b[1] == 0)
            break;
        if (!strncmp(name, (const char *)info[i].name, MAX_GPT_NAME_SIZE)) {
            *ret = entry + i;
            break;
        }
    }
    mutex_release(&partition_table.lock);
    return NO_ERROR;
}

int get_uuid_by_name(const char *name, char **uuid)
{
    struct gpt_header *header = NULL;
    struct gpt_entry *entry = NULL;
    struct part_meta_info *info = NULL;
    int i = 0;

    if (!name) {
        LTRACEF("[PART_UTILS] Partition name is NULL\n");
        return -EINVAL;
    }

    // reset and get partition table
    if (partition_reset_tbl() < 0)
        return -EIO;

    header = partition_table.header;
    entry = partition_table.entry_tbl;
    info = partition_table.info;

    for (i = 0; i < (int)header->max_partition_count; i++) {
        if (!entry[i].partition_type_guid.b[0]
                && !entry[i].partition_type_guid.b[1])
            break;
        if (!strncmp(name, (const char *)info[i].name, MAX_GPT_NAME_SIZE)) {
            *uuid = info[i].uuid;
            break;
        }
    }
    return NO_ERROR;
}

int part_entry_update(struct gpt_header *header, struct gpt_entry *entry_list)
{
    int ret = NO_ERROR;
    bdev_t *boot_dev;

    if (!header || !entry_list) {
        dprintf(CRITICAL, "can pass NULL header(0x%p) and entry(0x%p)\n",
            header, entry_list);
        return -EINVAL;
    }

    boot_dev = bio_open(PART_BOOTDEVICE_NAME);
    if (!boot_dev) {
        LTRACEF("Partition [%s] is not exist.\n", PART_BOOTDEVICE_NAME);
        return -ENODEV;
    }

    mutex_acquire(&partition_table.lock);
    //update primary gpt entry
    write_primary_gpt(boot_dev, header, entry_list);
    // update secondary gpt entry
    write_secondary_gpt(boot_dev, header, entry_list);
    // publish partition again
    partition_publish(PART_BOOTDEVICE_NAME, 0x0);
    mutex_release(&partition_table.lock);
    // reset and get partition table
    if (partition_reset_tbl() < 0)
        ret = -EIO;

    bio_close(boot_dev);
    return ret;
}

/* change UTF-16 to UTF-8 */
static void w2s(uint8_t *dst, int dst_max, uint16_t *src, int src_max)
{
    int i = 0;
    int len = MIN(src_max, dst_max - 1);

    while (i < len) {
        if (!src[i])
            break;
        dst[i] = src[i] & 0xFF;
        i++;
    }

    dst[i] = 0;
}

static int init_partition_buffer(bdev_t *bdev)
{
    int err = NO_ERROR;
    size_t entry_tbl_size, part_info_size;

    //firstly get header to identify entries count
    partition_table.header = (struct gpt_header *)memalign(CACHE_LINE, bdev->block_size);
    if (!partition_table.header) {
        dprintf(CRITICAL, "[PART_UTILS] lack of memory for partition header\n");
        return -ENOSPC;
    }

    err = get_gpt_header(bdev, partition_table.header);
    if (err) {
        dprintf(CRITICAL, "[PART_UTILS] gpt header get failed\n");
        return err;
    }

    entry_tbl_size = partition_table.header->max_partition_count * sizeof(struct gpt_entry);
    part_info_size = partition_table.header->max_partition_count * sizeof(struct part_meta_info);
    partition_table.entry_tbl = (struct gpt_entry *)memalign(CACHE_LINE, entry_tbl_size);
    partition_table.info = (struct part_meta_info *)memalign(CACHE_LINE, part_info_size);
    if (!partition_table.entry_tbl || !partition_table.info) {
        dprintf(CRITICAL, "[PART_UTILS] lack of memory for partition entry_tbl and info\n");
        return -ENOSPC;
    }
    return NO_ERROR;
}

int partition_reset_tbl(void)
{
    int err = 0;
    uint32_t i = 0;
    bdev_t *bdev;
    size_t header_size, entry_tbl_size, part_info_size;
    struct gpt_header *header = NULL;
    struct gpt_entry *entry_tbl = NULL;
    struct part_meta_info *info = NULL;

    bdev = bio_open(PART_BOOTDEVICE_NAME);
    if (!bdev) {
        LTRACEF("Partition [%s] is not exist.\n", PART_BOOTDEVICE_NAME);
        return -ENODEV;
    }

    mutex_acquire(&partition_table.lock);
    if (!partition_table.initial) {
        err = init_partition_buffer(bdev);
        if (err)
            goto out;
        partition_table.initial = 1;
    }

    if (!partition_table.header || !partition_table.entry_tbl || !partition_table.info) {
        dprintf(CRITICAL, "[PART_UTILS] lack of memory for partition table\n");
        err = -ENOSPC;
        goto out;
    }
    // header parse
    header_size = bdev->block_size;
    memset(partition_table.header, 0x0, header_size);
    err = get_gpt_header(bdev, partition_table.header);
    if (err) {
        dprintf(CRITICAL, "[PART_UTILS] gpt header get failed\n");
        err = -ENOSPC;
        goto out;
    }

    // entry parse
    entry_tbl_size = partition_table.header->max_partition_count * sizeof(struct gpt_entry);
    part_info_size = partition_table.header->max_partition_count * sizeof(struct part_meta_info);
    memset(partition_table.entry_tbl, 0x0, entry_tbl_size);
    memset(partition_table.info, 0x0, part_info_size);

    err = get_gpt_entry(bdev, partition_table.header, partition_table.entry_tbl);
    if (err || !partition_table.header || !partition_table.entry_tbl) {
        dprintf(CRITICAL, "[PART_UTILS] get GPT failed\n");
        err = -ENOSPC;
        goto out;
    }
    // update info
    header = partition_table.header;
    entry_tbl = partition_table.entry_tbl;
    info = partition_table.info;
    partition_table.part_cnt = 0;
    for (i = 0; i < header->max_partition_count; i++) {
         if (!entry_tbl[i].partition_type_guid.b[0]
            && !entry_tbl[i].partition_type_guid.b[1])
            break;
        w2s(info[i].name, MAX_GPT_NAME_SIZE,
            (uint16_t *)entry_tbl[i].partition_name, MAX_GPT_NAME_SIZE);
        info[i].name[MAX_GPT_NAME_SIZE - 1] = (uint8_t)'\0';
        if (efi_guid_unparse(&entry_tbl[i].unique_partition_guid,
            info[i].uuid) == NULL) {
            dprintf(CRITICAL, "[PART_UTILS] %s guid parse failed\n", info[i].name);
            err = -EINVAL;
            goto out;
        }
        partition_table.part_cnt++;
    }

out:
    mutex_release(&partition_table.lock);
    bio_close(bdev);
    return err;
}

void table_dump(void)
{
    struct gpt_header *header = NULL;
    struct gpt_entry *entry = NULL;
    struct part_meta_info *info = NULL;
    int i;
    // reset and get partition table
    if (partition_reset_tbl() < 0)
        return;

    header = partition_table.header;
    entry = partition_table.entry_tbl;
    info = partition_table.info;
    dprintf(CRITICAL, "[PART_UTILS] dump partition table with ent(%d):\n",
        partition_table.part_cnt);
    for (i = 0; i < (int)header->max_partition_count; i++) {
        if (!entry[i].partition_type_guid.b[0]
                && !entry[i].partition_type_guid.b[1])
            break;
            dprintf(CRITICAL, "[GPT] %s (%llu, %llu) %s\n",
                info[i].name,
                entry[i].starting_lba,
                entry[i].ending_lba,
                info[i].uuid
                );
    }
}
