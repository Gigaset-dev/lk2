/* Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <debug.h>
#include <lib/cksum.h>
#include <lib/zlib.h>
#include <malloc.h>
#include <platform/wdt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include <trace.h>

#include "aee.h"
#include "mrdump_private.h"

#define LOCAL_TRACE 0

#define BLKSIZE         4096
#define MAX_CONTINUE    16
#define EXSPACE         (BLKSIZE*MAX_CONTINUE)  // Expect continue space

struct mrdump_part_handle {
    struct mrdump_dev *dumpdev;
    uint64_t filesize;
    uint64_t woffset;
    int midx;
    uint8_t data[EXSPACE];
};

// Store data and write when buffer(handle's data) full
// return left length to avoid recursive call. --> turn to for loop
static int Do_Store_or_Write(struct mrdump_part_handle *handle, uint8_t *buf, uint32_t Length)
{
    unsigned int leftspace, mylen, reval;

    leftspace = EXSPACE - handle->midx;

    // Check Length
    if (Length > leftspace) {
        mylen = leftspace;
        reval = Length - leftspace;
    } else {
        mylen = Length;
        reval = 0;
    }

    // Store
    while (mylen > 0) {
        if (handle->midx < 0) {
            voprintf_error("Error: handle->midx < 0");
            return -1;
        }
        handle->data[handle->midx] = *buf;
        handle->midx++;
        buf++;
        mylen--;
    }

    // Write
    if (handle->midx == EXSPACE) {
        if (!handle->dumpdev->write(handle->dumpdev, handle->woffset, handle->data, EXSPACE)) {
            voprintf_error(" SDCard: Write dump data failed.\n");
            return -1;
        }

        handle->woffset = handle->woffset + EXSPACE;
        handle->midx = 0;
    }

    return reval;
}

static int lba_write_cb(void *opaque_handle, void *buf, int size)
{
    unsigned int len;
    int ret;
    uint8_t *ptr;

    struct mrdump_part_handle *handle = opaque_handle;

    handle->filesize += size;

    // End of File, write the left Data in handle data buffer...
    if ((buf == NULL) && (size == 0)) {
        if (!handle->dumpdev->write(handle->dumpdev, handle->woffset, handle->data, handle->midx)) {
            LTRACEF_LEVEL(ALWAYS, " SDCard: Write dump data failed.\n");
            return -1;
        }
        return 0;
    }

    // buf should not be NULL if not EOF
    if (buf == NULL)
        return -1;

    // process of Store and write
    len = size;
    ptr = (uint8_t *)buf;
    while (1) {
        ret = Do_Store_or_Write(handle, ptr, len);
        if (ret < 0) {
            LTRACEF_LEVEL(ALWAYS, " SDCard: Store and Write failed.\n");
            return -1;
        } else if (ret == 0) {
            break;
        } else {
            ptr  += (len - ret);
            len   = ret;
        }
    }
    LTRACEF_LEVEL(1, " dump handle->filesize %lld\n", handle->filesize);
    return size;
}


int mrdump_partition_output(const struct mrdump_control_block *mrdump_cb,
                       const struct kzip_addlist *memlist,
                       const struct kzip_addlist *memlist_cmm,
                       struct mrdump_dev *mrdump_dev)
{
    uint8_t InfoHeader[MRDUMP_PAF_TOTAL_SIZE];
    unsigned int mycrc;
    off_t dram_size = mblock_get_memory_size();

    if (mrdump_dev == NULL)
        return -BDATA_STATE_PARTITION_ACCESS_ERROR;

    voprintf_info("Output to MRDUMP Partition %s\n", mrdump_dev->name);

    if (dram_size > mrdump_dev->bdev->total_size) {
        voprintf_error(" mrdump partition(%llx) is not enough for dram size(%llx)\n",
                                                mrdump_dev->bdev->total_size, dram_size);
        return -BDATA_STATE_PARTITION_ACCESS_ERROR;
    }

    // pre-work for dynamic partition header
    bzero(InfoHeader, sizeof(InfoHeader));

    if (!mrdump_dev->read(mrdump_dev, 0ULL, (uint8_t *)InfoHeader, sizeof(InfoHeader))) {
        voprintf_error(" SDCard: Reading InfoHeader failed.\n");
        return -BDATA_STATE_PARTITION_ACCESS_ERROR;
    }

    uint64_t paf_coredump_size = *(uint64_t *)(InfoHeader + MRDUMP_PAF_COREDUMPSIZE);

    if (paf_coredump_size != 0) {
        voprintf_error(" CORE DUMP SIZE is not Zero! Abort!(coresize=%llu)\n",
            paf_coredump_size);
        return -BDATA_STATE_BLOCK_HEADER_ERROR;
    }

    struct mrdump_part_handle *handle = memalign(16, sizeof(struct mrdump_part_handle));

    if (handle == NULL) {
        LTRACEF_LEVEL(ALWAYS, "No enough memory.\n");
        voprintf_error("No enough memory or alloc memory not align 16B\n");
        return -BDATA_STATE_NO_MEM_ERROR;
    }
    memset(handle, 0, sizeof(struct mrdump_part_handle));
    handle->dumpdev = mrdump_dev;

    // Mark coredump as ULLONG_MAX to verify the unknown interrupt case during dumping data
    *(uint64_t *)(InfoHeader + MRDUMP_PAF_COREDUMPSIZE) = ULLONG_MAX;

    //calculate the CRC
    mycrc = crc32(0L, NULL, 0);
    *(uint32_t *)(InfoHeader + MRDUMP_PAF_CRC32) = crc32(mycrc, (const unsigned char *)InfoHeader,
                                                                             MRDUMP_LBA_DATAONLY);

    //update the coredump size and CRC
    if (!handle->dumpdev->write(handle->dumpdev, 0ULL, (uint8_t *)InfoHeader, sizeof(InfoHeader))) {
        voprintf_error(" SDCard: Write InfoHeader error.\n");
        free(handle);
        return -BDATA_STATE_PARTITION_ACCESS_ERROR;
    }

    // Starting Dumping Data
    handle->woffset = BLKSIZE;

    bool ok = true;

    mtk_wdt_restart_timer();
    struct kzip_file *zf = kzip_open(handle, lba_write_cb);

    if (zf != NULL) {
        if (!kzip_add_file(zf, memlist, "SYS_COREDUMP"))
            ok = false;
        if (ok && !kzip_add_file(zf, memlist_cmm, "kernel.cmm"))
            ok = false;
        mtk_wdt_restart_timer();
        kzip_close(zf);
        lba_write_cb(handle, NULL, 0); /* really write onto emmc of the last part */
    } else
        ok = false;

    if (!ok) {
        free(handle);
        return -BDATA_STATE_PARTITION_ACCESS_ERROR;
    }

    voprintf_info(" Zip COREDUMP size is: %lld\n", handle->filesize);

    // Record File Size...
    *(uint64_t *)(InfoHeader + MRDUMP_PAF_COREDUMPSIZE) = handle->filesize;

    mycrc = crc32(0L, NULL, 0);
    *(uint32_t *)(InfoHeader + MRDUMP_PAF_CRC32) = crc32(mycrc, (const unsigned char *)InfoHeader,
                                                                             MRDUMP_LBA_DATAONLY);

    if (!handle->dumpdev->write(handle->dumpdev, 0ULL, (uint8_t *)InfoHeader, sizeof(InfoHeader))) {
        voprintf_error(" SDCard: Write InfoHeader error.\n");
        free(handle);
        return -BDATA_STATE_PARTITION_ACCESS_ERROR;
    }
    free(handle);

    mtk_wdt_restart_timer();
    voprintf_info("OUTPUT:%s\nMODE:%s\n", "PARTITION_DATA",
                  mrdump_mode2string(mrdump_cb->crash_record.reboot_mode));

    return 0;
}


