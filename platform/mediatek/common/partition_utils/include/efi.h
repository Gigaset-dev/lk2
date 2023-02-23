/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once
#include <lib/bio.h>

/* GPT Signature should be 0x5452415020494645 */
#define GPT_SIGNATURE_1 0x54524150
#define GPT_SIGNATURE_2 0x20494645

#define MAX_GPT_NAME_SIZE          (72)
#define PARTITION_TYPE_GUID_SIZE   (16)
#define PART_META_INFO_NAMELEN     (64)
#define PART_META_INFO_UUIDLEN     (37)
#define UNIQUE_PARTITION_GUID_SIZE (16)
#define NUM_PARTITIONS             (192)
#define PART_ATTR_LEGACY_BIOS_BOOTABLE  (0x00000004UL) /* bit2 = active bit for OTA */

struct efi_guid_t {
    uint8_t b[16];
} __PACKED;

struct part_meta_info {
    uint8_t name[MAX_GPT_NAME_SIZE];
    uint8_t uuid[PART_META_INFO_UUIDLEN];
};

struct gpt_entry {
    struct efi_guid_t partition_type_guid;
    struct efi_guid_t unique_partition_guid;
    uint64_t starting_lba;
    uint64_t ending_lba;
    uint64_t attributes;
    uint8_t partition_name[MAX_GPT_NAME_SIZE];
} __PACKED;

struct gpt_header {
    uint64_t signature;
    uint32_t revision;
    uint32_t header_size;
    uint32_t header_crc32;
    uint32_t reserved;
    uint64_t my_lba;
    uint64_t alternate_lba;
    uint64_t first_usable_lba;
    uint64_t last_usable_lba;
    struct efi_guid_t disk_guid;
    uint64_t partition_entry_lba;
    uint32_t max_partition_count;
    uint32_t partition_entry_size;
    uint32_t partition_entry_array_crc32;
} __PACKED;

int write_primary_gpt(bdev_t *dev, struct gpt_header *hdr, struct gpt_entry *entry);
int write_secondary_gpt(bdev_t *dev, struct gpt_header *hdr, struct gpt_entry *entry);
uint8_t *efi_guid_unparse(struct efi_guid_t *guid, uint8_t *out);
int get_gpt_header(bdev_t *dev, struct gpt_header *header);
int get_gpt_entry(bdev_t *dev, struct gpt_header *header, struct gpt_entry *entry);
