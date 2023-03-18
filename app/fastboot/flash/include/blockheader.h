/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <stdint.h>

#define HEADER_BLOCK_SIZE_COMBO               (4096)
#define HEADER_BLOCK_SIZE_UFS                 (4096)
#define HEADER_BLOCK_SIZE_EMMC                (2048)
#define HEADER_BLOCK_SIZE_NAND                (2048)
#define BUFFER_OFFSET_LENGTH                  (0x300000)

int erase_header_block_for_cert(char *block_cache);
int process_preloader(char *data, uint32_t *psz);
int process_header_block_for_cert(char *block_cache, char *cert_data,
    uint32_t cert_length,
    uint32_t *cert_offset);
