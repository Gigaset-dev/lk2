/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <arch.h>
#include <assert.h>
#include <compiler.h>
#include <debug.h>
#include <err.h>
#include <lib/bio.h>
#include <lib/nftl.h>
#include <lib/partition.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "pmt.h"

int partition_nand_publish(const char *device, off_t offset)
{
    int err, i;
    u_char *buf;
    u32 pmt_size = sizeof(struct pt_resident) * MAX_PARTITION_COUNT;
    u8 pmt[PT_SIG_SIZE];
    struct pt_resident *pt;
    bdev_t *dev, *partdev;
    unsigned int block_size = 0;
    off_t offset_start = offset;

    // clear any partitions that may have already existed
    partition_unpublish(device);

    dev = bio_open(device);
    if (!dev) {
        printf("partition_publish: unable to open device\n");
        return -1;
    }

    buf = malloc(pmt_size);
    if (buf == NULL)
        return ERR_NO_MEMORY;
    pt = (struct pt_resident *)buf;

    bio_ioctl(dev, 0x100, &block_size);
    dprintf(INFO, "block size %d\n", block_size);

    for (i = 0; i < 2; i++) {
        offset = offset_start + i * (off_t)block_size;
        err = bio_read(dev, pmt, offset, sizeof(pmt));
        if (err < 0) {
            dprintf(INFO, "PMT: read PMT block 0x%llX fail\n", offset);
            continue;
        }

        if (is_valid_mpt(pmt) == 0 && is_valid_pt(pmt) == 0) {
            err = ERR_FAULT;
            dprintf(INFO, "PMT: not find sig in PMT block 0x%llX\n", offset);
            continue;
        }

        err = bio_read(dev, buf, offset + PT_SIG_SIZE, pmt_size);
        if (err < 0) {
            dprintf(INFO, "PMT: read PMT block 0x%llX fail\n", offset);
            continue;
        }

        break;
    }

    if (err < 0)
        goto err;

    for (i = 0 ; i < MAX_PARTITION_COUNT ; i++) {
        if (strlen((const char *)pt[i].name) == 0)
            break;

        dprintf(INFO, "pmt name:%s, start addr 0x%llX, size: 0x%llX\n",
            pt[i].name, pt[i].offset, pt[i].size);
        char subdevice[128];

        (void)snprintf(subdevice, sizeof(subdevice) - 1, "%sp%d", device, i);

        err = bio_publish_subdevice(device, subdevice,
                                    pt[i].offset/dev->block_size, pt[i].size/dev->block_size);
        if (err < 0) {
            dprintf(INFO, "error publishing subdevice '%s'\n", pt[i].name);
            continue;
        }

        partdev = bio_open(subdevice);

        if (!partdev) {
            dprintf(INFO, "error open subdevice '%s'\n", subdevice);
            continue;
        }

        partdev->label = strdup((char *)pt[i].name);
        partdev->is_gpt = false;
        bio_close(partdev);

        nftl_add_part(device, subdevice, partdev->label, (u64)pt[i].offset, (u64)pt[i].size);
    }

    bio_close(dev);

err:
    free(buf);
    return (err < 0) ? err : i;
}


