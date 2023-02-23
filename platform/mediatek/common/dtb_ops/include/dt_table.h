/*
 * Copyright (c) 2019 MediaTek Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#pragma once

#include <stdint.h>

#define DT_TABLE_MAGIC 0xd7b7ab1e
#define DT_TABLE_DEFAULT_PAGE_SIZE 2048
#define DT_TABLE_DEFAULT_VERSION 0

struct dt_table_header {
    uint32_t magic;             /* DT_TABLE_MAGIC */
    uint32_t total_size;        /* includes dt_table_header + all dt_table_entry
                                 * and all dtb/dtbo
                                 */
    uint32_t header_size;       /* sizeof(dt_table_header) */

    uint32_t dt_entry_size;     /* sizeof(dt_table_entry) */
    uint32_t dt_entry_count;    /* number of dt_table_entry */
    uint32_t dt_entries_offset; /* offset to the first dt_table_entry
                                 * from head of dt_table_header.
                                 * The value will be equal to header_size if
                                 * no padding is appended
                                 */

    uint32_t page_size;         /* flash page size we assume */
    uint32_t version;           /* DTBO image version, the current version is 0.
                                 * The version will be incremented when the dt_table_header
                                 * struct is updated.
                                 */
};

enum dt_compression_info {
    NO_COMPRESSION,
    ZLIB_COMPRESSION,
    GZIP_COMPRESSION
};

struct dt_table_entry {
    uint32_t dt_size;
    uint32_t dt_offset;         /* offset from head of dt_table_header */

    uint32_t id;                /* optional, must be zero if unused */
    uint32_t rev;               /* optional, must be zero if unused */
    uint32_t custom[4];         /* optional, must be zero if unused */
};

struct dt_table_entry_v1 {
    uint32_t dt_size;
    uint32_t dt_offset;         /* offset from head of dt_table_header */

    uint32_t id;                /* optional, must be zero if unused */
    uint32_t rev;               /* optional, must be zero if unused */
    uint32_t flags;             /* For version 1 of dt_table_header, the 4 least significant bits
                                 * of 'flags' will be used indicate the compression
                                 * format of the DT entry as per the enum 'dt_compression_info'
                                 */
    uint32_t custom[3];         /* optional, must be zero if unused */
};
