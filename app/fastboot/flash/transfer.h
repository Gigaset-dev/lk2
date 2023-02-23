/*
 * Copyright (c) 2009, Google Inc.
 * Copyright (c) 2019, MediaTek Inc.
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

#pragma once
#include <lib/bio.h>
#include <sys/types.h>

#define ROUND_TO_PAGE(x, y) (((x) + (y)) & (~(y)))
#define PARTITION_NAME_SIZE MAX_PARTITION_NAME_LEN

#define MTK_ULTRA_FLASH                  (1)
#define MAX_EXT_SIG_LEN                  (2 * 1024 * 1024)
#define STATUS_OK                        (0)
#define STATUS_ERR                       (1)
#define STATUS_INVALID_PARAMETERS        (-100)
#define STATUS_TOO_LARGE                 (-101)
#define STATUS_USB_ERR                   (-102)
#define STATUS_EMMC_ERR                  (-103)
#define STATUS_NAND_ERR                  (-104)
#define STATUS_THREAD                    (-105)
#define STATUS_UNKNOWN_SPARSE_CHUNK_TYPE (-106)

#define STATUS_SPARSE_INCOMPLETE (100)

struct partition_info_struct {
    bdev_t *bdev;
    uint64_t max_size;
};

struct download_data_context {
    bool first_run;
    uint64_t bytes_written;
    uint64_t length_to_write;
    struct partition_info_struct *part_info;
};

static inline void
init_download_data_context(struct download_data_context *data_ctx,
                           uint64_t data_length,
                           struct partition_info_struct *part_info)
{
    data_ctx->first_run = 1;
    data_ctx->bytes_written = 0;
    data_ctx->length_to_write = data_length;
    data_ctx->part_info = part_info;
}

status_t download_data(uint64_t data_length, struct partition_info_struct *part_info);
unsigned char *get_global_cache1(void);

