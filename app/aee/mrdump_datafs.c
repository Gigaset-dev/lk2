/*
 * Copyright (c) 2019 MediaTek Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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

uint64_t lba_marker_time;
struct __PACKED marked_block_data {
    uint32_t lba;
    uint64_t zero_padding[510];
    uint64_t timestamp;
    uint32_t crc;
} bdata;

struct mrdump_lba_handle {
    struct mrdump_dev *dumpdev;
    uint64_t allocsize;
    uint64_t filesize;
    unsigned int wlba;
    unsigned int rlba;
    int bidx;
    unsigned int midx;
    unsigned int blknum;
    unsigned int block_lba[1024];
    uint8_t data[EXSPACE];
    bool file_check;
};

/*
 * 8-bits file check definition
 * bit 0     : full check
 * bit 1 - 7 : reserved
 */
enum {
    DATAFS_FULL_CHECK = 0x1,
} MRDUMP_DATAFS_FILE_FLAGS;

static uint64_t ext4_lba_to_block_offset(uint32_t lba)
{
    return (uint64_t)lba * (uint64_t)BLKSIZE;
}

static bool check_block_valid(const struct mrdump_lba_handle *handle, uint32_t lba)
{
    mtk_wdt_restart_timer();
    if (!handle->dumpdev->read(handle->dumpdev, ext4_lba_to_block_offset(lba),
            (uint8_t *)&bdata, BLKSIZE)) {
        LTRACEF_LEVEL(ALWAYS, " Read BlockData failed\n");
        return false;
    }

    if (bdata.lba != lba) {
        LTRACEF_LEVEL(ALWAYS, " Read BlockData LBA failed (c:%08x, v:%08x)\n", bdata.lba, lba);
        return false;
    }

    if (bdata.timestamp != lba_marker_time) {
        LTRACEF_LEVEL(ALWAYS, " Read BlockData timestamp failed (c:%016llx, v:%016llx)\n",
                bdata.timestamp, lba_marker_time);
        return false;
    }

    unsigned int crcval = crc32(0, Z_NULL, 0);

    crcval = crc32(crcval, (void *)&bdata, (BLKSIZE - 4));
    if (bdata.crc != crcval) {
        LTRACEF_LEVEL(ALWAYS, "%s: Get BlockData crc32 error (c:%08x, v:%08x)\n",
                __func__, crcval, bdata.crc);
        return false;
    }

    return true;
}

static int Get_Next_bidx(struct mrdump_lba_handle *handle, unsigned int moves)
{
    unsigned int mycrc;

    if (handle->bidx == 1022) {
        handle->rlba = handle->block_lba[handle->bidx];
        if (!handle->dumpdev->read(handle->dumpdev, ext4_lba_to_block_offset(handle->rlba),
                   (uint8_t *)handle->block_lba, BLKSIZE)) {
            LTRACEF_LEVEL(ALWAYS, " SDCard: Reading BlockLBA failed.\n");
            return -1;
        }
        // check crc32
        mycrc = crc32(0, Z_NULL, 0);
        mycrc = crc32(mycrc, (void *)handle->block_lba, (BLKSIZE-4));
        if (mycrc != handle->block_lba[1023]) {
            LTRACEF_LEVEL(ALWAYS, " Get next index crc32 error!\n");
            return -1;
        }
        handle->bidx = 0;
    } else {
        handle->bidx += moves;
    }
    return handle->bidx;
}

unsigned int Num_to_Join(const struct mrdump_lba_handle *handle, unsigned int idx)
{
    unsigned int i, j;
    uint32_t lba;

    for (i = 0, j = 0; i < MAX_CONTINUE; i++) {
        lba = handle->block_lba[idx+i];
        if ((lba - handle->block_lba[idx]) == i) {
            mtk_wdt_restart_timer();
            if (handle->file_check == DATAFS_FULL_CHECK) {
                if (!check_block_valid(handle, lba)) {
                    LTRACEF_LEVEL(ALWAYS, " BlockData Verification failed\n");
                    return 0;
                }
            }
            j++;
            continue;
        }
        break;
    }
    return j;
}

// Store data and write when buffer(handle's data) full
// return left length to avoid recursive call. --> turn to for loop
static int Do_Store_or_Write(struct mrdump_lba_handle *handle, uint8_t *buf, uint32_t Length)
{
    int total;
    unsigned int leftspace, mylen, reval;

    total = BLKSIZE * handle->blknum;
    leftspace = total - handle->midx;

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
        handle->data[handle->midx] = *buf;
        handle->midx++;
        buf++;
        mylen--;
    }

    // Write
    if (handle->midx == total) {
        if (!handle->dumpdev->write(handle->dumpdev, ext4_lba_to_block_offset(handle->wlba),
                handle->data, total)) {
            LTRACEF_LEVEL(ALWAYS, " SDCard: Write dump data failed.\n");
            return -1;
        }
        handle->bidx = Get_Next_bidx(handle, handle->blknum);
        if (handle->bidx < 0) {
            LTRACEF_LEVEL(ALWAYS, " SDCard: Reading bidx failed.\n");
            return -1;
        }
        if (handle->bidx == 1022) {
            handle->bidx = Get_Next_bidx(handle, handle->blknum);
            if (handle->bidx < 0) {
                LTRACEF_LEVEL(ALWAYS, " SDCard: Reading 1022 bidx failed.\n");
                return -1;
            }
        }
        handle->blknum = Num_to_Join(handle, handle->bidx);
        if (handle->blknum == 0)
            return -1;
        handle->wlba = handle->block_lba[handle->bidx];
        handle->midx = 0;
    }
    return reval;
}

static int lba_write_cb(void *opaque_handle, void *buf, int size)
{
    unsigned int    len, moves;
    int             ret;
    uint8_t         *Ptr;

    struct mrdump_lba_handle *handle = opaque_handle;

    if ((handle->filesize + size) > handle->allocsize) {
        LTRACEF_LEVEL(ALWAYS, " dump size > allocated size. file-size %lld allocate-size %lld\n",
                handle->filesize + size, handle->allocsize);
        return -1;
    }
    handle->filesize += size;

    // End of File, write the left Data in handle data buffer...
    if ((buf == NULL) && (size == 0)) {
        if (!handle->dumpdev->write(handle->dumpdev, ext4_lba_to_block_offset(handle->wlba),
                handle->data, handle->midx)) {
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
    ret = len;
    Ptr = (uint8_t *)buf;
    while (1) {
        ret = Do_Store_or_Write(handle, Ptr, len);
        if (ret < 0) {
            LTRACEF_LEVEL(ALWAYS, " SDCard: Store and Write failed.\n");
            return -1;
        } else if (ret == 0) {
            break;
        } else {
            moves = len - ret;
            Ptr  += moves;
            len   = ret;
        }
    }
    LTRACEF_LEVEL(1, " dump handle->filesize %lld\n", handle->filesize);
    return size;
}

static inline uint16_t paf_version_extract(uint8_t pafinfo[MRDUMP_PAF_TOTAL_SIZE])
{
    return pafinfo[0] + (pafinfo[1] << 8);
}

static void dump_paf_info(uint8_t InfoLBA[MRDUMP_PAF_TOTAL_SIZE], const char *msg, ...)
{
    va_list ap;

    va_start(ap, msg);
    voprintf('E', msg, ap);
    va_end(ap);

    uint32_t paf_version = paf_version_extract(InfoLBA);
    uint32_t paf_info_lba = *(uint32_t *)(InfoLBA + MRDUMP_PAF_INFO_LBA);
    uint32_t paf_addr_lba = *(uint32_t *)(InfoLBA + MRDUMP_PAF_ADDR_LBA);
    uint64_t paf_alloc_size = *(uint64_t *)(InfoLBA + MRDUMP_PAF_ALLOCSIZE);
    uint64_t paf_coredump_size = *(uint64_t *)(InfoLBA + MRDUMP_PAF_COREDUMPSIZE);
    uint64_t paf_timestamp = *(uint64_t *)(InfoLBA + MRDUMP_PAF_TIMESTAMP);
    uint32_t paf_crc32 = *(uint32_t *)(InfoLBA + MRDUMP_PAF_CRC32);

    LTRACEF_LEVEL(1, " pafile ver %u info-lba %u addr-lba %u alloc-size %llu",
              paf_version, paf_info_lba, paf_addr_lba, paf_alloc_size);
    LTRACEF_LEVEL(1, " coredump-size %llu timestamp %llx crc %x\n",
              paf_coredump_size, paf_timestamp, paf_crc32);
}

int mrdump_ext4_output(const struct mrdump_control_block *mrdump_cb,
                       const struct kzip_addlist *memlist,
                       const struct kzip_addlist *memlist_cmm,
                       struct mrdump_dev *mrdump_dev)
{
    uint8_t InfoLBA[MRDUMP_PAF_TOTAL_SIZE];
    unsigned int mycrc;

    if (mrdump_dev == NULL)
        return -BDATA_STATE_FILE_ACCESS_ERROR;

    voprintf_info("Output to EXT4 Partition %s\n", mrdump_dev->name);

    // pre-work for ext4 LBA
    bzero(InfoLBA, sizeof(InfoLBA));

    dump_paf_info(InfoLBA, " first check\n");
    // Error 1. InfoLBA starting address not available
    if (mrdump_cb->output_fs_lbaooo == 0) {
        LTRACEF_LEVEL(ALWAYS, " Pre-Allocate has no LBA markers(lbaooo=%u). RAM-Dump stop!\n",
                mrdump_cb->output_fs_lbaooo);
        return -BDATA_STATE_BLOCK_HEADER_ERROR;
    }
    if (!mrdump_dev->read(mrdump_dev, ext4_lba_to_block_offset(mrdump_cb->output_fs_lbaooo),
            (uint8_t *)InfoLBA, sizeof(InfoLBA))) {
        LTRACEF_LEVEL(ALWAYS, " SDCard: Reading InfoLBA failed.\n");
        return -BDATA_STATE_FILE_ACCESS_ERROR;
    }

    uint32_t paf_version = paf_version_extract(InfoLBA);
    uint32_t paf_info_lba = *(uint32_t *)(InfoLBA + MRDUMP_PAF_INFO_LBA);
    uint32_t paf_addr_lba = *(uint32_t *)(InfoLBA + MRDUMP_PAF_ADDR_LBA);
    uint64_t paf_alloc_size = *(uint64_t *)(InfoLBA + MRDUMP_PAF_ALLOCSIZE);
    uint64_t paf_coredump_size = *(uint64_t *)(InfoLBA + MRDUMP_PAF_COREDUMPSIZE);
    uint64_t paf_timestamp = *(uint64_t *)(InfoLBA + MRDUMP_PAF_TIMESTAMP);
    uint32_t paf_info_crc32 = *(uint32_t *)(InfoLBA + MRDUMP_PAF_CRC32);

    if (paf_version != MRDUMP_PAF_VERSION) {
        dump_paf_info(InfoLBA, " Unsupport PAF version %d (expected: %d)\n",
            paf_version, MRDUMP_PAF_VERSION);
        return -BDATA_STATE_BLOCK_HEADER_ERROR;
    }
    // Error 3. InfoLBA[EXT4_1ST_LBA] should be mrdump_cb->output_fs_lbaooo
    if (mrdump_cb->output_fs_lbaooo != paf_info_lba) {
        dump_paf_info(InfoLBA, " LBA Starting Address Error(LBA0=%u)! Abort!\n", paf_info_lba);
        return -BDATA_STATE_BLOCK_HEADER_ERROR;
    }

    // Error 4. EXT4_CORE_DUMP_SIZE is not zero --> want to hold 1st dump
    if (paf_coredump_size != 0) {
        dump_paf_info(InfoLBA, " CORE DUMP SIZE is not Zero! Abort!(coresize=%llu)\n",
            paf_coredump_size);
        return -BDATA_STATE_BLOCK_HEADER_ERROR;
    }

    // Error 5. EXT4_USER_FILESIZE is zero
    if (paf_alloc_size == 0) {
        dump_paf_info(InfoLBA, " Allocate file with zero size. Abort!(filesize=%llu)\n",
            paf_alloc_size);
        return -BDATA_STATE_BLOCK_HEADER_ERROR;
    }

    // get lba_marker_time as timestamp
    lba_marker_time = paf_timestamp;

    // Error 2. CRC not matched
    mycrc = crc32(0L, Z_NULL, 0);
    mycrc = crc32(mycrc, (const unsigned char *)InfoLBA, sizeof(InfoLBA) - 4);
    if (mycrc != paf_info_crc32) {
        dump_paf_info(InfoLBA, " InfoLBA CRC32 Error! Abort! (CRC1=0x%08x, CRC2=0x%08x)\n",
            mycrc, paf_info_crc32);
        return -BDATA_STATE_BLOCK_HEADER_ERROR;
    }

    struct mrdump_lba_handle *handle = memalign(16, sizeof(struct mrdump_lba_handle));

    if (handle == NULL) {
        LTRACEF_LEVEL(ALWAYS, "No enough memory.\n");
        voprintf_error("No enough memory or alloc memory not align 16B\n");
        return -BDATA_STATE_NO_MEM_ERROR;
    }
    memset(handle, 0, sizeof(struct mrdump_lba_handle));
    handle->dumpdev = mrdump_dev;
    handle->rlba = mrdump_cb->output_fs_lbaooo;
    handle->allocsize = paf_alloc_size;
    handle->file_check = (*(uint8_t *)(InfoLBA + MRDUMP_PAF_FILE_CHECK)) & DATAFS_FULL_CHECK;

    if (handle->file_check == DATAFS_FULL_CHECK)
        voprintf_debug("Do full check\n");

    // Mark coredump as ULLONG_MAX to verify the unknown interrupt case during dumping data
    *(uint64_t *)(InfoLBA + MRDUMP_PAF_COREDUMPSIZE) = ULLONG_MAX;

    //calculate the CRC
    mycrc = crc32(0L, Z_NULL, 0);
    *(uint32_t *)(InfoLBA + MRDUMP_PAF_CRC32) = crc32(mycrc, (const unsigned char *)InfoLBA,
        MRDUMP_LBA_DATAONLY);

    //update the coredump sizeand CRC
    if (!handle->dumpdev->write(handle->dumpdev, ext4_lba_to_block_offset(paf_info_lba),
            (uint8_t *)InfoLBA, sizeof(InfoLBA))) {
        LTRACEF_LEVEL(ALWAYS, " SDCard: Write InfoLBA error.\n");
        free(handle);
        return -BDATA_STATE_FILE_ACCESS_ERROR;
    }

    // Starting Dumping Data
    handle->rlba = paf_addr_lba;
    if (!handle->dumpdev->read(handle->dumpdev, ext4_lba_to_block_offset(handle->rlba),
            (uint8_t *)handle->block_lba, BLKSIZE)) {
        dump_paf_info(InfoLBA, " SDCard: Reading BlockLBA error.\n");
        free(handle);
        return -BDATA_STATE_FILE_ACCESS_ERROR;
    }
    handle->wlba = handle->block_lba[handle->midx];
    handle->blknum = Num_to_Join(handle, handle->bidx);
    if (handle->blknum == 0) {
        LTRACEF_LEVEL(ALWAYS, "No continuous space.\n");
        free(handle);
        return -BDATA_STATE_NO_MEM_ERROR;
    }

    voprintf_info(" Pre-Allocate starts at LBA: %u\n", paf_info_lba);
    voprintf_info(" SYS_COREDUMP starts at LBA: %u\n", handle->wlba);

    mtk_wdt_restart_timer();

    struct kzip_file *zf = kzip_open(handle, lba_write_cb);
    bool ok = true;

    if (zf != NULL) {
        if (!kzip_add_file(zf, memlist, "SYS_COREDUMP"))
            ok = false;
        if (ok && !kzip_add_file(zf, memlist_cmm, "kernel.cmm"))
            ok = false;
        mtk_wdt_restart_timer();
        kzip_close(zf);
        lba_write_cb(handle, NULL, 0); /* really write onto emmc of the last part */
        zf = NULL;
    } else {
        ok = false;
    }

    if (!ok) {
        free(handle);
        return -BDATA_STATE_NO_MEM_ERROR;
    }

    voprintf_info(" SYS_COREDUMP ends at LBA: %u\n", handle->wlba);
    voprintf_info(" Zip COREDUMP size is: %lld\n", handle->filesize);

    // Record File Size...
    *(uint64_t *)(InfoLBA + MRDUMP_PAF_COREDUMPSIZE) = handle->filesize;

    mycrc = crc32(0L, Z_NULL, 0);
    *(uint32_t *)(InfoLBA + MRDUMP_PAF_CRC32) = crc32(mycrc, (const unsigned char *)InfoLBA,
        MRDUMP_LBA_DATAONLY);

    if (!handle->dumpdev->write(handle->dumpdev, ext4_lba_to_block_offset(paf_info_lba),
            (uint8_t *)InfoLBA, sizeof(InfoLBA))) {
        LTRACEF_LEVEL(ALWAYS, " SDCard: Write InfoLBA error.\n");
        free(handle);
        return -BDATA_STATE_FILE_ACCESS_ERROR;
    }
    free(handle);

    mtk_wdt_restart_timer();
    voprintf_info(" OUTPUT:%s\nMODE:%s\n", "DATA_FS",
        mrdump_mode2string(mrdump_cb->crash_record.reboot_mode));
    return BDATA_STATE_CHECK_PASS;
}

