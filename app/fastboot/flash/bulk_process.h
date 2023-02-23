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

#include "transfer.h"

struct bulk_cache_ctx {
    uint8_t *c_base;   // cache buffer base
    uint32_t c_offset; // cache buffer offset
    uint32_t c_size;   // cache buffer size
};

struct bulk_status {
    status_t handle_status;
    uint32_t byte_to_process; // byte to be processed in one packet.
    uint64_t image_address_offset;
    uint8_t *buf;           // byte buffer of one packet.
    struct bulk_cache_ctx ctx; // buffer download context
    struct partition_info_struct *partition;
};

void init_bulk_process_status(struct bulk_status *status,
                              struct partition_info_struct *partition);
void write_bulk_data(struct bulk_status *status, uint8_t *data, uint32_t length);

// ex functions
void end_write_bulk(struct bulk_status *status);
