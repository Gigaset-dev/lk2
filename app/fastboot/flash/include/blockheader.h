/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <lib/bio.h>
#include <stdint.h>
#include <sys/types.h>

#define HEADER_BLOCK_SIZE_COMBO               (4096)
#define HEADER_BLOCK_SIZE_UFS                 (4096)
#define HEADER_BLOCK_SIZE_EMMC                (2048)
#define HEADER_BLOCK_SIZE_NAND                (2048)
#define BUFFER_OFFSET_LENGTH                  (0x300000)
#define MAX_CERT_LEN                          (32*1024)

int process_preloader(char *data, uint32_t *psz);
int process_auth_cert(struct bdev *bdev, char *data, uint32_t *psz);
int process_erase_auth_cert(struct bdev *bdev, off_t *poffset, uint32_t *psz);
