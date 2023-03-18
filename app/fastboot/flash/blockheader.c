/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <blockheader.h>
#include <sboot.h>
#include <stdint.h>
#include <string.h>
#include <verified_boot_common.h>
#include <verified_boot_error.h>

int process_preloader(char *data, uint32_t *psz)
{
    int status = 0;
    char *cache = data + BUFFER_OFFSET_LENGTH;
    struct data_buffer_t hb, pl;

    pl.data = data;
    pl.length = *psz;
    memset(&hb, 0, sizeof(struct data_buffer_t));

    if (!sec_pl_bin_already_has_hb_adapter(&pl)) {
        status = sec_get_hb_size_adapter(FLASH_DEV_UFS, &pl, &hb.length);
        if (status != 0)
            return status;

        hb.data = cache;
        status = sec_create_loader_hb_adapter(FLASH_DEV_UFS, &pl, &hb);
        /* EMMC Use Case */
        uint32_t *p = (uint32_t *)(hb.data + 16);

        if (*p == 0x800)
            *p = 0x200;

        if (FAIL(status))
            return status;

        memcpy(cache + hb.length, data, *psz);
        *psz += hb.length;
        memcpy(data, cache, *psz);
    }

    return status;
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
    int status = 0;
    struct data_buffer_t cert;
    struct data_buffer_t hb;

    cert.data = cert_data;
    cert.length = cert_length;
    memset(&hb, 0, sizeof(struct data_buffer_t));
    status = sec_get_hb_size_adapter(FLASH_DEV_UFS, &cert, &hb.length);

    if (FAIL(status))
        return status;

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

