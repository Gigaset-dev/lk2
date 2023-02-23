/*
 * Copyright (c) 2009, Google Inc.
 * Copyright (c) 2021, MediaTek Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <blockheader.h>
#include <debug.h>
#include <fastboot.h>
#include <fastboot_usb.h>
#include <lib/bio.h>
#include <lib/partition.h>
#include <lock_state.h>
#include <malloc.h>
#include <partition_utils.h>
#include <sboot.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "flash_commands.h"
#include "sparse_format.h"
#include "transfer.h"
#include <verified_boot_common.h>
#include <verified_boot_error.h>

void register_flash_commands(void)
{
    fastboot_register("download:", cmd_download, true, false);
    fastboot_register("flash:", cmd_flash_bio, true, true);
    fastboot_register("erase:", cmd_erase_bio, true, true);

#if (MTK_ULTRA_FLASH)
    fastboot_register("oem ultraflash:", cmd_oem_ultra_flash, true, true);
    fastboot_register("oem ultraflash_en", cmd_oem_ultra_flash_en, true, true);
    fastboot_register("ultraflash:", cmd_oem_ultra_flash, true, true);
#endif
}

#if (MTK_ULTRA_FLASH)

bool g_ultra_flash_enable = true;
char partition_name_history[MAX_PARTITION_NAME_LEN] = {0};
int partition_name_history_len;
static const char * const special_partition[] = {"system", "super"};

void switch_max_download_len(bool ultra_en)
{
    static const char *dl_max_str_history;
    static const char *dl_max_str = "0xC0000000";

    if (dl_max_str_history == NULL)
        dl_max_str_history = fastboot_get_var("max-download-size");

    fastboot_update_var("max-download-size",
                        ultra_en ? dl_max_str : dl_max_str_history);

    return;
}

bool is_special_partition(const char *partition)
{
    unsigned int idx = 0;

    for (idx = 0; idx < countof(special_partition); ++idx) {
        if (strnicmp(special_partition[idx], partition, 32) == 0)
            return true;
    }

    return false;
}

__WEAK bool get_dl_policy(const char *part_name)
{
    return true;
}

bool cmd_download_ultra(const char *arg, void *data, unsigned int sz)
{
    status_t ret = STATUS_OK;
    bdev_t *bdev = NULL;

    char response[MAX_RSP_SIZE];
    uint64_t len = strtoll(arg, 0, 16);
    struct partition_info_struct part_info;

    snprintf(response, MAX_RSP_SIZE, "DATA%08llx", len);

    if (usb_write(response, strlen(response)) < 0)
        return false;

    memset(&part_info, 0, sizeof(struct partition_info_struct));

    bdev = bio_open_by_label(partition_name_history);
    if (bdev == NULL)
        bdev = bio_open(partition_name_history);

    if (bdev == NULL) {
        fastboot_fail("partition does not exist");
        return false;
    }

    part_info.bdev = bdev;

    part_info.max_size = bdev->total_size;

    dprintf(ALWAYS, "download part_name[%s] part_max_sz[0x%llx] down_len [0x%llx]\n",
         partition_name_history, part_info.max_size, len);

    ret = download_data(len, &part_info);

    if (ret) {
        snprintf(response, MAX_RSP_SIZE, "Transmission FAIL: [%d]", ret);
        fastboot_fail(response);
        bio_close(bdev);
        return false;
    }

    fastboot_okay("");

    bio_close(bdev);
    return true;
}

char *trim_space(const char *str)
{
    char *p = NULL;

    if (str == NULL)
        return NULL;
    else
        p = (char *)str;

    while (*p && (*p == ' ' || *p == '\t'))
        ++p;

    return p;
}

void cmd_oem_ultra_flash(const char *arg, void *data, unsigned int sz)
{
    char response[MAX_RSP_SIZE];

    if (!g_ultra_flash_enable) {
        snprintf(response, MAX_RSP_SIZE, "Ultra-flash Disabled. [%s]",
                 fastboot_get_var("max-download-size"));
        fastboot_info(response);
        fastboot_okay("");
        return;
    }

    char *part = trim_space(arg);
    if (part == NULL)
        return;

    if (strnlen(part, MAX_PARTITION_NAME_LEN + 3) >= MAX_PARTITION_NAME_LEN) {
        fastboot_fail("name too long.");
        return;
    }

    memset(partition_name_history, 0, MAX_PARTITION_NAME_LEN);

    partition_name_history_len = strnlen(part, MAX_PARTITION_NAME_LEN - 1);

    strlcpy(partition_name_history, part, MAX_PARTITION_NAME_LEN);

    switch_max_download_len((partition_name_history_len != 0) &&
        (is_special_partition(partition_name_history)));

    snprintf(response, MAX_RSP_SIZE, "%s. [%s]", partition_name_history,
             fastboot_get_var("max-download-size"));

    fastboot_info(response);
    fastboot_okay("");

    return;
}

void cmd_oem_ultra_flash_en(const char *arg, void *data, unsigned int sz)
{
    char response[MAX_RSP_SIZE];

    if (!strncmp(arg, " 1", strlen(" 1"))) {
        // turn ultra flash on
        fastboot_info("Enable ultra-flash.");
        g_ultra_flash_enable = true;
    } else if (!strncmp(arg, " 0", strlen(" 0"))) {
        // turn ultra flash off
        fastboot_info("Disable ultra-flash.");
        g_ultra_flash_enable = false;

    } else {
        snprintf(response, MAX_RSP_SIZE, "\tCurrent ultra-flash setting:%s",
                 g_ultra_flash_enable ? "Enabled" : "Disabled");
        fastboot_info(response);
    }

    fastboot_okay("");

    return;
}

void *get_available_ram_base(void)
{
    return download_base;
}

#endif   //endif of MTK_ULTRA_FLASH


bool cmd_download_standard(const char *arg, void *data, unsigned int sz)
{
    char response[MAX_RSP_SIZE];
    unsigned int len = (unsigned int)strtoll(arg, 0, 16);
    int r;

    download_size = 0;

    if (len > download_max) {
        fastboot_fail("data is too large");
        return false;
    }

    snprintf(response, MAX_RSP_SIZE, "DATA%08x", len);

    if (usb_write(response, strlen(response)) < 0)
        return false;

    dprintf(ALWAYS, "Resp: %s\n", response);
    dprintf(ALWAYS, "download_base: 0x%x, length: 0x%x\n",
        (unsigned int)download_base, len);

    r = usb_read(download_base, len);

    if ((r < 0) || ((unsigned int)r != len)) {
        fastboot_fail("Read USB error");
        // fastboot_state = STATE_ERROR;
        return false;
    }

    download_size = len;

    fastboot_okay("");
    return true;
}

void cmd_download(const char *arg, void *data, unsigned int sz)
{
    bool result = false;

#if (MTK_ULTRA_FLASH)
    if (g_ultra_flash_enable && (partition_name_history_len != 0) &&
        (is_special_partition(partition_name_history)))
        result = cmd_download_ultra(arg, data, sz);
    else
        result = cmd_download_standard(arg, data, sz);

    if (!result)
        // restore the default max download size.
        switch_max_download_len(false);
#else
    cmd_download_standard(arg, data, sz);
#endif
    return;
}

void flash_auth_cert(void *data, unsigned int sz)
{
    int ret = 0;
    off_t size = 0;
    bdev_t *bdev = NULL;
    char partition[MAX_GPT_NAME_SIZE] = {0};

    ret = partition_get_active_preloader(partition, MAX_GPT_NAME_SIZE);

    if (ret) {
        fastboot_fail("get active preloader fail");
        return;
    }

    if (get_dl_policy(partition) == false) {
        fastboot_fail("partition is not writable");
        return;
    }

    bdev = bio_open_by_label(partition);

    if (bdev == NULL)
        bdev = bio_open(partition);

    if (bdev == NULL) {
        fastboot_fail("partition does not exist");
        return;
    }

    if (process_auth_cert(bdev, data, &sz) != 0) {
        fastboot_fail("process auth_cert fail.");
        bio_close(bdev);
        return;
    }

    size = bdev->total_size;

    if (ROUND_TO_PAGE(sz, 511) > size) {
        fastboot_fail("data size is larger than partition size.");
        bio_close(bdev);
        return;
    }

    if ((unsigned int)bio_write(bdev, data, 0, sz) != sz) {
        fastboot_fail("flash write failure");
        bio_close(bdev);
        return;
    }

    fastboot_okay("");
    bio_close(bdev);

    return;
}

void cmd_flash_bio_img(const char *arg, void *data, unsigned int sz)
{
    off_t size = 0;
    bdev_t *bdev = NULL;
    char cert[] = "auth_cert";

    if (strnicmp(arg, "gpt", 4) == 0) {
        if (partition_update_gpt_table(data, sz) != 0)
            fastboot_fail("write gpt failed.");
        else
            fastboot_okay("");
        return;
    }

    if (strnicmp(arg, cert, sizeof(cert)) == 0) {
        flash_auth_cert(data, sz);
        return;
    }

    if (get_dl_policy(arg) == false) {
        fastboot_fail("partition is not writable");
        return;
    }

    bdev = bio_open_by_label(arg);
    if (bdev == NULL)
        bdev = bio_open(arg);

    if (bdev == NULL) {
        fastboot_fail("partition does not exist");
        return;
    }

    if (strnicmp(arg, "preloader", sizeof("preloader") - 1) == 0) {
        if (process_preloader(data, &sz) != 0) {
            fastboot_fail("process preloader fail.");
            bio_close(bdev);
            return;
        }
    }

    size = bdev->total_size;

    if (ROUND_TO_PAGE(sz, 511) > size) {
        fastboot_fail("data size is larger than partition size.");
        bio_close(bdev);
        return;
    }

    if ((unsigned int)bio_write(bdev, data, 0, sz) != sz) {
        fastboot_fail("flash write failure");
        bio_close(bdev);
        return;
    }

    if (bdev->is_gpt == false) { // root device
        ssize_t ret;

        ret = partition_publish(bdev->name, 0x0);
        if (ret > 0)
            bio_dump_devices();
    }

    fastboot_okay("");
    bio_close(bdev);

    return;
}

void cmd_flash_bio_sparse_img(const char *arg, void *data, unsigned int sz)
{
    unsigned int chunk = 0;
    unsigned int chunk_data_sz = 0;
    static uint32_t *fill_buf;
    unsigned int int_cnt = 0;
    uint32_t fill_val = 0;
    unsigned int left = 0;
    unsigned int todo = 0;
    uint32_t chunk_blk_cnt = 0;
    uint32_t i = 0;
    struct sparse_header *sparse_header = NULL;
    struct chunk_header *chunk_header = NULL;
    uint64_t total_blocks = 0;
    off_t size = 0;
    bdev_t *bdev = NULL;

    bdev = bio_open_by_label(arg);
    if (bdev == NULL)
        bdev = bio_open(arg);

    if (bdev == NULL) {
        fastboot_fail("partition does not exist");
        return;
    }

    size = bdev->total_size;

    /*Read and skip over sparse image header */
    sparse_header = (struct sparse_header *)data;
    dprintf(ALWAYS, "Image size span 0x%llx, partition size 0x%llx\n",
        (unsigned long long)sparse_header->total_blks * sparse_header->blk_sz,
            size);

    if ((unsigned long long)sparse_header->total_blks * sparse_header->blk_sz >
        (unsigned long long)size) {
        fastboot_fail("sparse image size span overflow.");
        bio_close(bdev);
        return;
    }

    if (sparse_header->file_hdr_sz > sz) {
        fastboot_fail("sparse image header invalid.");
        bio_close(bdev);
        return;
    }

    data += sizeof(struct sparse_header);

    if (sparse_header->file_hdr_sz > sizeof(struct sparse_header))
        /*Skip the remaining bytes in a header that is longer than we expected.*/
        data += (sparse_header->file_hdr_sz - sizeof(struct sparse_header));

    dprintf(ALWAYS, "=== Sparse Image Header ===\n");
    dprintf(ALWAYS, "magic: 0x%x\n", sparse_header->magic);
    dprintf(ALWAYS, "major_version: 0x%x\n", sparse_header->major_version);
    dprintf(ALWAYS, "minor_version: 0x%x\n", sparse_header->minor_version);
    dprintf(ALWAYS, "file_hdr_sz: %d\n", sparse_header->file_hdr_sz);
    dprintf(ALWAYS, "chunk_hdr_sz: %d\n", sparse_header->chunk_hdr_sz);
    dprintf(ALWAYS, "blk_sz: %d\n", sparse_header->blk_sz);
    dprintf(ALWAYS, "total_blks: %d\n", sparse_header->total_blks);

    /*Start processing chunks */
    for (chunk = 0; chunk < sparse_header->total_chunks; chunk++) {
        /*Read and skip over chunk header */
        chunk_header = (struct chunk_header *)data;
        data += sizeof(struct chunk_header);

        if (sparse_header->chunk_hdr_sz > sizeof(struct chunk_header)) {
            /*Skip the remaining bytes in a header that is longer than
             *we expected.
             */
            data += (sparse_header->chunk_hdr_sz - sizeof(struct chunk_header));
        }

        if (size / sparse_header->blk_sz < chunk_header->chunk_sz) {
            fastboot_fail("sparse chunk size is too big.");
            bio_close(bdev);
            return;
        }

        chunk_data_sz = sparse_header->blk_sz * chunk_header->chunk_sz;

        // check chunk bounadary
        if (sparse_header->blk_sz *
            ((unsigned long long)chunk_header->chunk_sz + total_blocks) >
            (unsigned long long)size) {
            fastboot_fail("sparse chunk size overflow.");
            bio_close(bdev);
            return;
        }

        if ((sparse_header->total_blks - total_blocks) < chunk_header->chunk_sz) {
            fastboot_fail("sparse chunk blocks bigger than total blocks.");
            bio_close(bdev);
            return;
        }

        switch (chunk_header->chunk_type) {
        case CHUNK_TYPE_RAW:
        {
            if (chunk_header->total_sz !=
                (sparse_header->chunk_hdr_sz + chunk_data_sz)) {
                fastboot_fail("Bogus chunk size for chunk type Raw");
                bio_close(bdev);
                return;
            }

            dprintf(ALWAYS, "Raw: start block addr: 0x%llx\n", total_blocks);

            if (bio_write(bdev, data, total_blocks * sparse_header->blk_sz,
                          chunk_data_sz) != (long long)chunk_data_sz) {
                fastboot_fail("flash write failure");
                bio_close(bdev);
                return;
            }

            total_blocks += chunk_header->chunk_sz;
            data += chunk_data_sz;

            break;
        }
        case CHUNK_TYPE_DONT_CARE:
        {
            dprintf(ALWAYS, "!!Blank: start: 0x%llx  offset: 0x%x\n", total_blocks,
                    chunk_header->chunk_sz);
            total_blocks += chunk_header->chunk_sz;

            break;
        }
        case CHUNK_TYPE_FILL:
        {
            if (chunk_header->total_sz !=
                (sparse_header->chunk_hdr_sz + sizeof(uint32_t))) {
                fastboot_fail("Bogus chunk size for chunk type FILL");
                bio_close(bdev);
                return;
            }

        #define FILL_BUF_BLOCK_CNT ((4 * 1024 * 1024) / sparse_header->blk_sz)
        #define FILL_BUF_LEN (FILL_BUF_BLOCK_CNT * sparse_header->blk_sz)
            // fill_buf = (uint32_t *)memalign(CACHE_LINE, FILL_BUF_LEN);
            if (fill_buf == NULL)
                fill_buf = memalign(128, FILL_BUF_LEN);

            if (!fill_buf) {
                fastboot_fail("Malloc failed for: CHUNK_TYPE_FILL");
                bio_close(bdev);
                return;
            }

            fill_val = *(uint32_t *)data;
            data = (char *)data + sizeof(uint32_t);
            chunk_blk_cnt = chunk_data_sz / sparse_header->blk_sz;
            int_cnt = FILL_BUF_LEN / sizeof(fill_val);

            for (i = 0; i < int_cnt; i++)
                fill_buf[i] = fill_val;

            left = chunk_blk_cnt;

            while (left > 0) {

                todo = (left > FILL_BUF_BLOCK_CNT) ? FILL_BUF_BLOCK_CNT : left;

                if ((unsigned int)bio_write(
                    bdev, fill_buf, total_blocks * sparse_header->blk_sz,
                    todo * sparse_header->blk_sz) != todo * sparse_header->blk_sz) {
                    fastboot_fail("CHUNK_TYPE_FILL flash write failure");
                    bio_close(bdev);
                    return;
                }

                total_blocks += todo;
                left -= todo;
            }

            if (fill_buf != 0) {
                free(fill_buf);
                fill_buf = 0;
            }

            break;
        }
        case CHUNK_TYPE_CRC:
        {
            if (chunk_header->total_sz != sparse_header->chunk_hdr_sz) {
                fastboot_fail("Bogus chunk size for chunk type Dont Care");
                bio_close(bdev);
                return;
            }

            total_blocks += chunk_header->chunk_sz;
            data += chunk_data_sz;

            break;
        }
        default:
            fastboot_fail("Unknown chunk type");
            bio_close(bdev);
            return;
        }
    }

    dprintf(ALWAYS, "Wrote 0x%llx blocks, expected to write %d blocks\n",
            total_blocks, sparse_header->total_blks);

    if (total_blocks != sparse_header->total_blks)
        fastboot_fail("sparse image write failure");
    else
        fastboot_okay("");

    bio_close(bdev);
    return;
}

bool cmd_flash_bio_standard(const char *arg, void *data, unsigned int sz)
{
    struct sparse_header *sparse_header = (struct sparse_header *)data;

    if (sparse_header->magic != SPARSE_HEADER_MAGIC)
        cmd_flash_bio_img(arg, data, sz);
    else
        cmd_flash_bio_sparse_img(arg, data, sz);

    return true;
}

void cmd_flash_bio(const char *arg, void *data, unsigned int sz)
{
    if (get_dl_policy(arg) == false) {
        fastboot_fail("partition is not writable");
        return;
    }

#if (MTK_ULTRA_FLASH)
    bool result = false;

    if (g_ultra_flash_enable && (partition_name_history_len != 0) &&
        (is_special_partition(partition_name_history))) {
        fastboot_info(partition_name_history);
        fastboot_okay("");
        result = true;
    } else {
        result = cmd_flash_bio_standard(arg, data, sz);
    }

    if (!result)
        // restore the default max download size.
        switch_max_download_len(false);
#else
    cmd_flash_bio_standard(arg, data, sz);
#endif
    return;
}

void erase_auth_cert(void *data, unsigned int sz)
{
    int ret = 0;
    uint32_t size = 0;
    off_t offset = 0;
    bdev_t *bdev = NULL;
    char partition[MAX_GPT_NAME_SIZE] = {0};

    ret = partition_get_active_preloader(partition, MAX_GPT_NAME_SIZE);

    if (ret) {
        fastboot_fail("get active preloader fail");
        return;
    }

    if (get_dl_policy(partition) == false) {
        fastboot_fail("partition is not writable");
        return;
    }

    bdev = bio_open_by_label(partition);

    if (bdev == NULL)
        bdev = bio_open(partition);

    if (bdev == NULL) {
        fastboot_fail("partition does not exist");
        return;
    }

    size = (uint32_t)bdev->total_size;

    if (process_erase_auth_cert(bdev, &offset, &size) != 0) {
        fastboot_fail("process auth_cert fail.");
        bio_close(bdev);
        return;
    }

    ret = bio_erase(bdev, offset, size);

    if (ret < 0) {
        fastboot_fail("erase failed");
        goto closebdev;
    }

    fastboot_okay("");

closebdev:
    bio_close(bdev);
    return;

}

void cmd_erase_bio(const char *arg, void *data, unsigned int sz)
{
    ssize_t ret;
    bdev_t *bdev = NULL;
    char cert[] = "auth_cert";

    if (strnicmp(arg, cert, sizeof(cert)) == 0) {
        erase_auth_cert(data, sz);
        return;
    }

    if (get_dl_policy(arg) == false) {
        fastboot_fail("partition is not writable");
        return;
    }

    bdev = bio_open_by_label(arg);
    if (bdev == NULL)
        bdev = bio_open(arg);

    if (bdev == NULL) {
        fastboot_fail("partition does not exist");
        return;
    }

    off_t size = bdev->total_size;

    ret = bio_erase(bdev, 0, size);
    if (ret < 0) {
        fastboot_fail("erase failed");
        goto closebdev;
    }

    if (bdev->is_gpt == false) { // root device
        ret = partition_unpublish(bdev->name);
        if (ret > 0)
            bio_dump_devices();
    }

    fastboot_okay("");

closebdev:
        bio_close(bdev);
        return;
}

