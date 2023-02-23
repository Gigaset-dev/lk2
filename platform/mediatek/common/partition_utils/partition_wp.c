/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <bootctrl.h>
#include <err.h>
#include <errno.h>
#include <lib/bio.h>
#include <lib/bio_ext.h>
#include <partition_utils.h>
#include <partition_wp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <trace.h>
#include <ufs_interface.h>

#include "table.h"

struct wp_args {
    unsigned long blknr;
    u32 blkcnt;
    u8 type;
};

static int perloader_set_wp(int type)
{
    int ret = NO_ERROR;
    struct wp_args *args = NULL;
    bdev_t *bdev = NULL;
    char preloader_name[MAX_GPT_NAME_SIZE] = {0};

    args = (struct wp_args *)memalign(CACHE_LINE, sizeof(struct wp_args));
    if (!args) {
        dprintf(CRITICAL, "[Part_WP]Lack of memory!!!\n");
        ret = -ENOSPC;
        goto out;
    }

    ret = partition_get_active_preloader(preloader_name, MAX_GPT_NAME_SIZE);
    if (ret)
        goto out;

    bdev = bio_open(preloader_name);
    if (!bdev) {
        dprintf(CRITICAL, "Partition [%s] is not exist.\n", preloader_name);
        ret = -ENODEV;
        goto out;
    }

    args->blknr = 0;
    args->blkcnt = bdev->block_count;
    args->type = type;
    dprintf(ALWAYS, "[Part_WP](%s) %lu %u %u\n",
            bdev->name, args->blknr, args->blkcnt, (unsigned int)args->type);

    ret = bio_ioctl(bdev, BIO_IOCTL_SET_WRITE_PROTECT, (void *)args);
    if (ret != 0) {
        dprintf(CRITICAL, "[Part_WP] bio set fialed\n");
        ret = -ENOSPC;
        goto out;
    }
out:
    if (bdev != NULL)
        bio_close(bdev);
    if (args != NULL)
        free(args);
    return ret;
}

int partition_write_prot_set(const char *start_part, const char *end_part, int type)
{
    int ret = NO_ERROR;
    unsigned long nr_wp_sects, start_sect;
    struct gpt_entry *start_entry = NULL, *end_entry = NULL;
    struct wp_args *args = NULL;
    bdev_t *bdev = NULL;

    if (!start_part || !end_part) {
        dprintf(CRITICAL, "[Part_WP]Input name is NULL\n");
        return -EINVAL;
    }

    if (!strncmp(start_part, "preloader", sizeof("preloader"))) {
        // preloader is placed in different area of flash
        ret = perloader_set_wp(type);
        goto out;
    }

    args = (struct wp_args *)memalign(CACHE_LINE, sizeof(struct wp_args));
    if (!args) {
        dprintf(CRITICAL, "[Part_WP]Lack of memory!!!\n");
        ret = -ENOSPC;
        goto out;
    }

    // the size info not stores in bio, get info from part_table
    ret |= get_entry_by_name(start_part, &start_entry);
    ret |= get_entry_by_name(end_part, &end_entry);
    if (ret || !start_entry || !end_entry) {
        dprintf(CRITICAL, "[Part_WP]no find start %s end %s\n", start_part, end_part);
        ret = -EINVAL;
        goto out;
    }

    start_sect = start_entry->starting_lba;
    nr_wp_sects =  end_entry->ending_lba - start_sect + 1;
    args->blknr = start_sect;
    args->blkcnt = nr_wp_sects;
    args->type = type;
    bdev = bio_open(PART_BOOTDEVICE_NAME);
    if (!bdev) {
        dprintf(CRITICAL, "Partition [%s] is not exist.\n",
            PART_BOOTDEVICE_NAME);
        ret = -ENODEV;
        goto out;
    }

    dprintf(CRITICAL,  "[Part_WP](%s -> %s) %lu %u %u\n", start_part,
        end_part, args->blknr, args->blkcnt, (unsigned int)args->type);

    ret = bio_ioctl(bdev, BIO_IOCTL_SET_WRITE_PROTECT, (void *)args);
    if (ret != 0) {
        dprintf(CRITICAL, "[Part_WP] bio set fialed\n");
        goto out;
    }
out:
    if (bdev != NULL)
        bio_close(bdev);
    if (args != NULL)
        free(args);
    return ret;
}
