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

#include <debug.h>
#include <lib/bio.h>
#include <stdlib.h>
#include <string.h>

#include "bulk_process.h"
#include "transfer.h"

void init_bulk_process_status(struct bulk_status *status,
                              struct partition_info_struct *partition)
{
    memset(status, 0, sizeof(struct bulk_status));
    status->partition = partition;
    status->image_address_offset = 0;
}

void end_write_bulk(struct bulk_status *status)
{
    return;
}

static void write_bulk_internal(struct bulk_status *status)
{
    size_t wrote = bio_write(status->partition->bdev, status->buf,
                  status->image_address_offset, (size_t)status->byte_to_process);

    if (wrote != status->byte_to_process)
        status->handle_status = STATUS_EMMC_ERR;

    if (status->handle_status != STATUS_OK)
        return;

    status->image_address_offset += status->byte_to_process;

    status->byte_to_process = 0;
    return;
}

void write_bulk_data(struct bulk_status *status, uint8_t *data, uint32_t length)
{
    status->buf = data;
    status->byte_to_process = length;

    do {
        write_bulk_internal(status);
        if (status->handle_status != STATUS_OK)
            return;

    } while (status->byte_to_process > 0);

    return;
}
