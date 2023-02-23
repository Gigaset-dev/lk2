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
#include <sys/types.h>
#include "transfer.h"

#define STORAGE_BLOCK_SIZE 4096

enum unsparse_wait_phase {
    UNSPARSE_WAIT_SPARSE_HEADER,
    UNSPARSE_WAIT_CHUNK_HEADER,
    UNSPARSE_WAIT_CHUNK_DATA,
    UNSPARSE_WAIT_CHUNK_NOT_CARE,
    UNSPARSE_WAIT_CHUNK_FILL,
    UNSPARSE_WAIT_CHUNK_CRC,
};

struct cache_ctx {
    uint8_t *c_base;   // cache buffer base
    uint32_t c_offset; // cache buffer offset
    uint32_t c_size;   // cache buffer size
};

struct unsparse_status {
    status_t handle_status;
    enum unsparse_wait_phase wait_phase;
    uint32_t byte_to_process; // byte to be processed in one packet.
    uint8_t *buf;             // byte buffer of one packet.
    struct cache_ctx ctx;        // buffer download context.
    struct partition_info_struct *partition;
};

struct unsparse_data {
    struct sparse_header sparse_hdr;
    struct chunk_header chunk_hdr;
    uint32_t chunks_processed;
    uint64_t chunk_remain_data_size; // data remained in current packet.
    uint64_t image_address_offset;   // offset of current processed data in packet
    uint32_t unhandle_buf_size;
    uint8_t unhandle_buf[sizeof(struct chunk_header)];
};

bool is_sparse_image(uint8_t *data, uint32_t length);
bool support_sparse_image(void);
uint64_t unspared_size(uint8_t *data);
void init_unsparse_status(struct unsparse_status *status,
                          struct partition_info_struct *partition);
void write_sparse_data(struct unsparse_status *status, uint8_t *data, uint32_t length);
void end_write_sparse_data(struct unsparse_status *status);
