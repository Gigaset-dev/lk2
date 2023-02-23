/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <blockheader.h>
#include <debug.h>
#include <err.h>
#include <lib/bio.h>
#include <sboot.h>
#include <stdint.h>
#include <string.h>
#include <verified_boot_common.h>
#include <verified_boot_error.h>
#include "flash_commands.h"

int process_preloader(char *data, uint32_t *psz)
{
    int ret = 0;
    char *cache = data + BUFFER_OFFSET_LENGTH;
    struct data_buffer_t hb, pl;

    pl.data = data;
    pl.length = *psz;
    memset(&hb, 0, sizeof(struct data_buffer_t));

    if (!sec_pl_bin_already_has_hb_adapter(&pl)) {
        ret = sec_get_hb_size_adapter(FLASH_DEV_UFS, &pl, &hb.length);
        if (ret)
            return ret;

        hb.data = cache;
        ret = sec_create_loader_hb_adapter(FLASH_DEV_UFS, &pl, &hb);

        if (ret)
            return ret;

        memcpy(cache + hb.length, data, *psz);
        *psz += hb.length;
        memcpy(data, cache, *psz);
    }

    return ret;
}

int get_hb_size(char *data, uint32_t length, uint32_t *sz)
{
    struct data_buffer_t pl;

    pl.data = data;
    pl.length = length;

    return sec_get_hb_size_adapter(FLASH_DEV_UFS, &pl, sz);
}

int process_header_block_for_cert(char *block_cache, char *cert_data,
    uint32_t cert_length, uint32_t *cert_offset)
{
    int ret = 0;
    struct data_buffer_t cert;
    struct data_buffer_t hb;

    cert.data = cert_data;
    cert.length = cert_length;
    memset(&hb, 0, sizeof(struct data_buffer_t));
    ret = sec_get_hb_size_adapter(FLASH_DEV_UFS, &cert, &hb.length);

    if (ret)
        return ret;

    hb.data = block_cache;

    return sec_get_updated_cert_hb_adapter(FLASH_DEV_UFS, &hb, &cert, cert_offset);
}

int erase_header_block_for_cert(char *block_cache)
{
    struct data_buffer_t hb;

    hb.data = block_cache;
    hb.length = HEADER_BLOCK_SIZE_COMBO;

    return sec_erase_header_block_for_cert_adapter(&hb);
}

int get_cert_offset(char *block_cache)
{
    struct data_buffer_t hb;

    hb.data = block_cache;
    hb.length = HEADER_BLOCK_SIZE_COMBO;

    return sec_get_cert_offset_adapter(&hb);
}

int process_auth_cert(struct bdev *bdev, char *data, uint32_t *psz)
{
    uint32_t length = *psz;
    uint32_t cert_offset = 0;
    off_t size = bdev->total_size;
    int ret = 0;
    char *block_cache = data + BUFFER_OFFSET_LENGTH;

    /*To Avoid Overlap, Preloader Length Should Be Less Than Or Equal To BUFFER_OFFSET_LENGTH */
    if (size > BUFFER_OFFSET_LENGTH)
        size = BUFFER_OFFSET_LENGTH;

    if ((unsigned int)bio_read(bdev, block_cache, 0, size) != size)
        return ERR_IO;

    ret = process_header_block_for_cert(block_cache, data, length, &cert_offset);

    if (ret)
        return ERR_FAULT;

    memcpy(block_cache + cert_offset, data, length);
    *psz = cert_offset + length;
    memcpy(data, block_cache, *psz);

    return ret;
}

int process_erase_auth_cert(struct bdev *bdev, off_t *poffset, uint32_t *psz)
{
    uint32_t length = *psz;
    int ret = 0;
    off_t size = HEADER_BLOCK_SIZE_COMBO;

    char *block_cache = (char *)((char *)get_available_ram_base() + BUFFER_OFFSET_LENGTH);

    if ((unsigned int)bio_read(bdev, block_cache, 0, size) != size)
        return ERR_IO;

    *poffset = get_cert_offset(block_cache);
    *psz = MAX_CERT_LEN;
    ret = erase_header_block_for_cert(block_cache);

    if (ret)
        return ERR_FAULT;

    if ((unsigned int)bio_write(bdev, block_cache, 0, size) != size)
        return ERR_IO;

    return ret;
}

