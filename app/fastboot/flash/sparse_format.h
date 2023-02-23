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
#include <stdint.h>

struct sparse_header {
    uint32_t magic;         /* 0xed26ff3a */
    uint16_t major_version; /* (0x1) - reject images with higher major versions */
    uint16_t minor_version; /* (0x0) - allow images with higer minor versions */
    uint16_t file_hdr_sz;   /* 28 bytes for first revision of the file format */
    uint16_t chunk_hdr_sz;  /* 12 bytes for first revision of the file format */
    uint32_t blk_sz;     /* block size in bytes, must be a multiple of 4 (4096) */
    uint32_t total_blks; /* total blocks in the non-sparse output image */
    uint32_t total_chunks;   /* total chunks in the sparse input image */
    uint32_t image_checksum; /* CRC32 checksum of the original data, counting*/
                               /* "don't care" */
    /* as 0. Standard 802.3 polynomial, use a Public Domain */
    /* table implementation */
};

#define SPARSE_HEADER_MAGIC  0xed26ff3a
#define CHUNK_TYPE_RAW       0xCAC1
#define CHUNK_TYPE_FILL      0xCAC2
#define CHUNK_TYPE_DONT_CARE 0xCAC3
#define CHUNK_TYPE_CRC       0xCAC4

struct chunk_header {
    uint16_t chunk_type; /* 0xCAC1 -> raw; 0xCAC2 -> fill; 0xCAC3 -> don't care */
    uint16_t reserved1;
    uint32_t chunk_sz; /* in blocks in output image */
    uint32_t total_sz; /* bytes of chunk input file including chunk header & data*/
};
