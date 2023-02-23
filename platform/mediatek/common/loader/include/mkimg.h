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

#define MKIMG_MAGIC               (0x58881688)
#define MKIMG_EXT_MAGIC           (0x58891689)

#define MKIMG_HDR_SZ              (0x200)
#define MKIMG_NAME_SZ             (32)

#define LEGACY_MKIMG_ALIGN_SZ     (4096)

/* image types */
#define IMG_TYPE_ID_OFFSET        (0)
#define IMG_TYPE_RESERVED0_OFFSET (8)
#define IMG_TYPE_RESERVED1_OFFSET (16)
#define IMG_TYPE_GROUP_OFFSET     (24)

#define IMG_TYPE_ID_MASK          (0xffU << IMG_TYPE_ID_OFFSET)
#define IMG_TYPE_RESERVED0_MASK   (0xffU << IMG_TYPE_RESERVED0_OFFSET)
#define IMG_TYPE_RESERVED1_MASK   (0xffU << IMG_TYPE_RESERVED1_OFFSET)
#define IMG_TYPE_GROUP_MASK       (0xffU << IMG_TYPE_GROUP_OFFSET)

#define IMG_TYPE_GROUP_AP         (0x00U << IMG_TYPE_GROUP_OFFSET)
#define IMG_TYPE_GROUP_MD         (0x01U << IMG_TYPE_GROUP_OFFSET)
#define IMG_TYPE_GROUP_CERT       (0x02U << IMG_TYPE_GROUP_OFFSET)

/* AP group */
#define IMG_TYPE_IMG_AP_BIN       (0x00 | IMG_TYPE_GROUP_AP)
#define IMG_TYPE_AND_VFY_BOOT_SIG (0x01 | IMG_TYPE_GROUP_AP)

/* MD group */
#define IMG_TYPE_IMG_MD_LTE       (0x00 | IMG_TYPE_GROUP_MD)
#define IMG_TYPE_IMG_MD_C2K       (0x01 | IMG_TYPE_GROUP_MD)

/* CERT group */
#define IMG_TYPE_CERT1            (0x00 | IMG_TYPE_GROUP_CERT)
#define IMG_TYPE_CERT1_MD         (0x01 | IMG_TYPE_GROUP_CERT)
#define IMG_TYPE_CERT2            (0x02 | IMG_TYPE_GROUP_CERT)

union mkimg_hdr {
    struct {
        uint32_t magic;
        uint32_t dsz;
        char name[MKIMG_NAME_SZ];
        uint32_t maddr;
        uint32_t mode;
        /* extension */
        uint32_t ext_magic;
        uint32_t hdr_sz;
        uint32_t hdr_ver;
        uint32_t img_type;
        uint32_t img_list_end;
        uint32_t align_sz;
        uint32_t dsz_extend;
        uint32_t maddr_extend;
        uint32_t scrambled;
    } info;
    uint8_t data[MKIMG_HDR_SZ];
};
