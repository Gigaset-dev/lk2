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
#include <stdint.h>
#include <string.h>
#include "sparse_format.h"
#include "sparse_state_machine.h"

// only engine data structure.
static struct unsparse_data m_unsparse_data;

static void set_unsparse_status(struct unsparse_status *status,
                                status_t handle_status,
                                enum unsparse_wait_phase wait_phase, uint32_t size,
                                uint8_t *buf)
{
    status->handle_status = handle_status;
    status->wait_phase = wait_phase;
    status->byte_to_process = size;
    status->buf = buf;
}

void init_unsparse_status(struct unsparse_status *status,
                          struct partition_info_struct *partition)
{
    memset(status, 0, sizeof(struct unsparse_status));
    set_unsparse_status(status, STATUS_OK, UNSPARSE_WAIT_SPARSE_HEADER, 0, 0);

    status->partition = partition;
}

void end_write_sparse_data(struct unsparse_status *status)
{
    // for the last package
}

static void memsetint(void *p, int i, int len)
{
    int idx = 0;
    int count = len >> 2;
    int *buf = (int *)p;

    while (idx < count)
        buf[idx++] = i;

    return;
}

inline uint64_t bio_write_wp(bdev_t *dev, uint64_t offset, void *data,
                             uint64_t size)
{
    size_t wrote = bio_write(dev, data, offset, (size_t)size);

    return wrote;
}

static void write_sparse_data_internal(struct unsparse_status *status)
{
    uint32_t size = status->byte_to_process;
    uint8_t *buf = status->buf;
    uint32_t sizeOfX = 0;

    int filled_seed = 0;
    uint8_t *filled_data_cache = 0;
    uint32_t filled_data_len = 0;

    if (size == 0)
        return;

    switch (status->wait_phase) {
    case UNSPARSE_WAIT_SPARSE_HEADER:
    {
        if (size >= sizeof(struct sparse_header)) {
            memset((void *)&m_unsparse_data, 0x00, sizeof(struct unsparse_data));
            memcpy((void *)&m_unsparse_data.sparse_hdr, buf, sizeof(struct sparse_header));

            // for real sparse header larger than sparse_header_t struct
            // for titan ext4_sparse_header_8byte_align:  sparse_header_t 8byte align,
            // chunk_header_t 8byte align

            size -= m_unsparse_data.sparse_hdr.file_hdr_sz;
            buf += m_unsparse_data.sparse_hdr.file_hdr_sz;

            m_unsparse_data.unhandle_buf_size = 0;
            m_unsparse_data.chunks_processed = 0;

            set_unsparse_status(status, STATUS_OK, UNSPARSE_WAIT_CHUNK_HEADER, size,
                                buf);
        }
        break;
    }

    case UNSPARSE_WAIT_CHUNK_HEADER:
    {
        if (m_unsparse_data.chunks_processed >=
            m_unsparse_data.sparse_hdr.total_chunks) {
            set_unsparse_status(status, STATUS_OK, UNSPARSE_WAIT_CHUNK_HEADER, 0,
                                buf);
            return;
        }

        if (m_unsparse_data.unhandle_buf_size + size >=
            (m_unsparse_data.sparse_hdr.chunk_hdr_sz)) {

            if (m_unsparse_data.unhandle_buf_size > 0) {
                uint32_t sizeOfUsedBuf = m_unsparse_data.sparse_hdr.chunk_hdr_sz -
                    m_unsparse_data.unhandle_buf_size;

                memcpy(&m_unsparse_data.chunk_hdr, m_unsparse_data.unhandle_buf,
                       m_unsparse_data.unhandle_buf_size);

                // for real sparse header larger than sparse_header_t struct
                if (m_unsparse_data.sparse_hdr.chunk_hdr_sz > sizeof(struct chunk_header))
                    memcpy(((uint8_t *)&m_unsparse_data.chunk_hdr) +
                           m_unsparse_data.unhandle_buf_size,
                           buf,
                           sizeOfUsedBuf - (m_unsparse_data.sparse_hdr.chunk_hdr_sz -
                                            sizeof(struct chunk_header)));
                else
                    memcpy(((uint8_t *)&m_unsparse_data.chunk_hdr) +
                           m_unsparse_data.unhandle_buf_size,
                           buf, sizeOfUsedBuf);

                size -= sizeOfUsedBuf;
                buf += sizeOfUsedBuf;
                m_unsparse_data.unhandle_buf_size = 0;
            } else {
                // for real sparse header larger than sparse_header_t struct
                memcpy(&m_unsparse_data.chunk_hdr, buf, sizeof(struct chunk_header));

                size -= m_unsparse_data.sparse_hdr.chunk_hdr_sz;
                buf += m_unsparse_data.sparse_hdr.chunk_hdr_sz;
            }

            m_unsparse_data.chunk_remain_data_size =
                (uint64_t)m_unsparse_data.chunk_hdr.chunk_sz *
                m_unsparse_data.sparse_hdr.blk_sz;

            switch (m_unsparse_data.chunk_hdr.chunk_type) {
            case CHUNK_TYPE_RAW:
            {
                set_unsparse_status(status, STATUS_OK, UNSPARSE_WAIT_CHUNK_DATA, size,
                                    buf);
                break;
            }
            case CHUNK_TYPE_DONT_CARE:
            {
                m_unsparse_data.image_address_offset +=
                    m_unsparse_data.chunk_remain_data_size;
                m_unsparse_data.chunk_remain_data_size = 0;

                set_unsparse_status(status, STATUS_OK, UNSPARSE_WAIT_CHUNK_HEADER, size,
                                    buf);
                break;
            }
            case CHUNK_TYPE_FILL:
            {
                set_unsparse_status(status, STATUS_OK, UNSPARSE_WAIT_CHUNK_FILL, size,
                                    buf);
                break;
            }
            case CHUNK_TYPE_CRC:
            {
                set_unsparse_status(status, STATUS_OK, UNSPARSE_WAIT_CHUNK_CRC, size,
                                    buf);
                break;
            }
            default:
                status->handle_status = STATUS_UNKNOWN_SPARSE_CHUNK_TYPE;
                return;
            }

            ++m_unsparse_data.chunks_processed;
        } else {
            m_unsparse_data.unhandle_buf_size = size;
            memcpy(m_unsparse_data.unhandle_buf, buf, size);
            size = 0; // force to jump out while loop
            set_unsparse_status(status, STATUS_OK, UNSPARSE_WAIT_CHUNK_HEADER, size,
                                buf);
        }
        break;
    }
    case UNSPARSE_WAIT_CHUNK_FILL:
    {
        filled_seed = *(int *)buf;
        buf += sizeof(uint32_t);
        size -= sizeof(uint32_t);

    #define FILLED_DATA_CACHE_SIZE (2 * 1024 * 1024)
        filled_data_cache = (uint8_t *)get_global_cache1();

        memsetint(filled_data_cache, filled_seed, FILLED_DATA_CACHE_SIZE);

        while (true) {
            filled_data_len =
                (m_unsparse_data.chunk_remain_data_size > FILLED_DATA_CACHE_SIZE)
                ? FILLED_DATA_CACHE_SIZE
                : m_unsparse_data.chunk_remain_data_size;

            if (filled_data_len == 0)
                break;

            if (bio_write_wp(status->partition->bdev,
                             m_unsparse_data.image_address_offset, filled_data_cache,
                             filled_data_len) != filled_data_len) {
                status->handle_status = STATUS_EMMC_ERR;
            }

            if (status->handle_status != STATUS_OK)
                return;

            m_unsparse_data.image_address_offset += filled_data_len;
            m_unsparse_data.chunk_remain_data_size -= filled_data_len;
        }

        set_unsparse_status(status, STATUS_OK, UNSPARSE_WAIT_CHUNK_HEADER, size,
                            buf);
        break;
    }
    case UNSPARSE_WAIT_CHUNK_CRC:
    {
        filled_seed = *(uint32_t *)buf;
        buf += sizeof(uint32_t);
        size -= sizeof(uint32_t);
        m_unsparse_data.chunk_remain_data_size = 0;

        set_unsparse_status(status, STATUS_OK, UNSPARSE_WAIT_CHUNK_HEADER, size,
                            buf);
        break;
    }
    case UNSPARSE_WAIT_CHUNK_DATA:
    {
        if (size >= m_unsparse_data.chunk_remain_data_size) {
            if (bio_write_wp(status->partition->bdev,
                             m_unsparse_data.image_address_offset, buf,
                             m_unsparse_data.chunk_remain_data_size) !=
                m_unsparse_data.chunk_remain_data_size)
                status->handle_status = STATUS_EMMC_ERR;

            if (status->handle_status != STATUS_OK)
                return;

            buf += m_unsparse_data.chunk_remain_data_size;
            size -= m_unsparse_data.chunk_remain_data_size;
            m_unsparse_data.image_address_offset +=
                m_unsparse_data.chunk_remain_data_size;
            m_unsparse_data.chunk_remain_data_size = 0;

            set_unsparse_status(status, STATUS_OK, UNSPARSE_WAIT_CHUNK_HEADER, size,
                                buf);
        } else { // big trunk, so it need more than 1 package.
            sizeOfX = size;
            // not 512 aligned.

            if (size & (STORAGE_BLOCK_SIZE - 1)) {
                // last fragment in a package.
                // this package must have successive package that contain the identical
                // trunk.
                if (size < STORAGE_BLOCK_SIZE) {
                    set_unsparse_status(status, STATUS_SPARSE_INCOMPLETE,
                                        UNSPARSE_WAIT_CHUNK_DATA, size, buf);
                    break;
                } else {
                    size &= ~(STORAGE_BLOCK_SIZE - 1);
                }
            }

            if (bio_write_wp(status->partition->bdev,
                             m_unsparse_data.image_address_offset, buf, size) != size)
                status->handle_status = STATUS_EMMC_ERR;

            if (status->handle_status != STATUS_OK)
                return;

            m_unsparse_data.image_address_offset += size;
            m_unsparse_data.chunk_remain_data_size -= size;
            buf += size;
            size = sizeOfX & (STORAGE_BLOCK_SIZE - 1); // size = 0 org

            set_unsparse_status(status, STATUS_OK, UNSPARSE_WAIT_CHUNK_DATA, size,
                                buf);
        }
        break;
    }
    default:
        break;
    }

    return;
}

void write_sparse_data(struct unsparse_status *status, uint8_t *data, uint32_t length)
{
    status->buf = data;
    status->byte_to_process = length;

    do {
        write_sparse_data_internal(status);

        if (status->handle_status != STATUS_OK)
            return;

    } while (status->byte_to_process > 0);

    return;
}

inline bool is_sparse_image(uint8_t *data, uint32_t length)
{
    struct sparse_header *sparse_header = (struct sparse_header *)data;

    return (sparse_header->magic == SPARSE_HEADER_MAGIC);
}

inline uint64_t unspared_size(uint8_t *data)
{
    uint64_t size = 0;
    struct sparse_header *sparse_header = (struct sparse_header *)data;

    size = (uint64_t)sparse_header->blk_sz * sparse_header->total_blks;

    return size;
}

inline bool support_sparse_image(void)
{
    return true;
}
