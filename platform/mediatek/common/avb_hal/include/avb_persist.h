/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once
#include <avb_ops.h>

#define PERSIST_MAGIC                   0x90790922
#define PERSIST_MAJOR_VER               0x1
#define PERSIST_MINOR_VER               0x0

#define PERSIST_UNSPECIFIED_OFFSET      0xFFFFFFFFFFFFFFFF
#define PERSIST_ALIGN_SZ                16
#define PERSIST_HDR_SZ                  64
#define DEFAULT_PERSIST_PREALLOC_BUF_SZ 512
#define PERSIST_TAG_MAGIC               0x90790804
#define MAX_PERSIST_TAG_SZ              8

#define PERSISTHDR_SIZE                 sizeof(union PersistHdr)
#define PERSISTDESCHDR_SIZE             sizeof(struct PersistDescriptorHdr)
#define PERSISTDESC_SIZE                sizeof(struct PersistDescriptor)

union PersistHdr {
    struct {
        uint32_t magic;
        uint32_t major_ver;
        uint32_t minor_ver;
    } info;

    uint8_t data[PERSIST_HDR_SZ];
};

struct PersistDescriptorHdr {
    uint32_t tag;
    uint32_t key_sz;    /* in bytes */
    uint32_t value_sz;  /* in bytes */
};

struct PersistDescriptor {
    struct PersistDescriptorHdr hdr;
    uint8_t *key;
    uint8_t *value;
};

struct PersistDescriptor *get_persist_desc(void);
bool get_init_persist_status(void);
void set_update_persist_status(void);
uint32_t init_persist_value(AvbOps *ops, const char *part_name, uint64_t offset);
uint32_t write_persist_value(AvbOps *ops, const char *part_name);

