/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <err.h>
#include <errno.h>
#include "efi.h"
#include <lib/cksum.h>
#include <partition_utils.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <trace.h>

#define LOCAL_TRACE 0

static uint32_t sgpt_partition_lba_size;
static uint32_t entries_crc32; /* using to record the crc32 of entries */

/*
 * get the gpt header and get the required header fields
 * Return 0 on valid signature
 */
int get_gpt_header(bdev_t *dev, struct gpt_header *header)
{
    int err;

    err = bio_read(dev, header, dev->block_size, dev->block_size);
    if (err < 0) {
        LTRACEF_LEVEL(ALWAYS, "[PART_UTILS] Could not read backup gpt from block device\n");
        return err;
    }

    /* Check GPT Signature */
    if (((uint32_t *) header)[0] != GPT_SIGNATURE_2 ||
            ((uint32_t *) header)[1] != GPT_SIGNATURE_1) {
        /* Check the backup gpt */
        uint64_t backup_header_lba = dev->block_count - 1;

        err = bio_read(dev, header, (backup_header_lba * dev->block_size), dev->block_size);
        if (err < 0) {
            LTRACEF_LEVEL(ALWAYS, "[PART_UTILS] Could not read backup gpt from block device\n");
            return err;
        }

        if (((uint32_t *) header)[0] != GPT_SIGNATURE_2 ||
            ((uint32_t *) header)[1] != GPT_SIGNATURE_1) {
                LTRACEF_LEVEL(ALWAYS, "[PART_UTILS] gpt failed on both primary and secondary\n");
                return -EIO;
        }
    }

    return NO_ERROR;
}

/*
 * get the gpt entry and get the required entry fields
 * Return 0 on valid signature
 */
int get_gpt_entry(bdev_t *dev, struct gpt_header *header, struct gpt_entry *entry)
{
    int err = 0;

    err = bio_read(dev, entry, (header->partition_entry_lba * dev->block_size),
                    header->max_partition_count * header->partition_entry_size);
    if (err < 0) {
        LTRACEF_LEVEL(ALWAYS, "[PART_UTILS] read entry failed\n");
        return err;
    }
    return NO_ERROR;
}

/* change UTF-16 to UTF-8 */
static void w2s(uint8_t *dst, int dst_max, uint16_t *src, int src_max)
{
    int i = 0;
    int len = MIN(src_max, dst_max - 1);

    while (i < len) {
        if (!src[i])
            break;
        dst[i] = src[i] & 0xFF;
        i++;
    }

    dst[i] = 0;
}

/* change UTF-8 to UTF-16 */
static void s2w(uint16_t *dst, int dst_max, uint8_t *src, int src_max)
{
    int i = 0;
    int len = MAX(src_max, dst_max - 1);

    while (i < len) {
        if (!src[i])
            break;
        dst[i] = src[i] & 0x00FF;
        i++;
    }

    dst[i] = 0x0000;
}
const char hex_asc[] = "0123456789abcdef";
#define hex_asc_lo(x) hex_asc[((x)&0x0f)]
#define hex_asc_hi(x) hex_asc[((x)&0xf0)>>4]

static inline char *hex_byte_pack(char *buf, uint8_t byte)
{
    *buf++ = hex_asc_hi(byte);
    *buf++ = hex_asc_lo(byte);
    return buf;
}

static uint8_t *string(uint8_t *buf, uint8_t *end, const char *s)
{
    int len, i;

    len = strnlen(s, PART_META_INFO_UUIDLEN);
    for (i = 0; i < len; ++i) {
        if (buf < end)
            *buf = *s;
        ++buf;
        ++s;
    }
    if (buf < end)
        *buf = 0;
    return buf;
}

static uint8_t *uuid_string(uint8_t *buf, uint8_t *args)
{
    /*
     * uuid format: xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxx,
     * which contains 16*2 + 4('-') + 1 ('\0') = 37 bytes
     */
    char uuid[PART_META_INFO_UUIDLEN];
    char *p = uuid;
    int i;
    static const uint8_t index[16] = {3, 2, 1, 0, 5, 4, 7, 6, 8, 9, 10, 11, 12, 13, 14, 15};
    uint8_t *end;

    end = buf + PART_META_INFO_UUIDLEN;
    for (i = 0; i < 16; i++) {
        p = hex_byte_pack(p, args[index[i]]);
        switch (i) {
        case 3:
        case 5:
        case 7:
        case 9:
            *p++ = '-';
            break;
        default:
            break;
        }
    }

    *p = 0;
    return string(buf, end, uuid);
}

uint8_t *efi_guid_unparse(struct efi_guid_t *guid, uint8_t *out)
{
    if (uuid_string(out, guid->b) == NULL) {
        LTRACEF_LEVEL(ALWAYS, "[PART_UTILS] parse uuid string format fail!\n");
        return NULL;
    }
    return out;
}

static uint64_t last_lba(void)
{
    uint64_t ret = -1;
    bdev_t *dev;

    dev = bio_open(PART_BOOTDEVICE_NAME);
    if (!dev) {
        LTRACEF("Partition [%s] is not exist.\n", PART_BOOTDEVICE_NAME);
        return -ENODEV;
    }

    ret = dev->block_count - 1;
    bio_close(dev);
    return ret;
}

static void set_spgt_partition_lba_size(u32 size)
{
    sgpt_partition_lba_size = size;
}

static uint32_t get_spgt_partition_lba_size(void)
{
    return sgpt_partition_lba_size;
}

static void set_entries_crc32(uint32_t crc32)
{
    entries_crc32 = crc32;
}

static uint32_t get_entries_crc32(void)
{
    return entries_crc32;
}

static uint64_t get_gpt_header_last_usable_lba(void)
{
    return (last_lba() - get_spgt_partition_lba_size());
}

static void pack_pheader_data(struct gpt_header *header)
{
    header->reserved = 0;
    header->alternate_lba = last_lba();
    header->last_usable_lba = get_gpt_header_last_usable_lba();
    header->partition_entry_array_crc32 = get_entries_crc32();
    header->header_crc32 = 0;
    header->header_crc32 = crc32(0, (unsigned char *)header, sizeof(struct gpt_header));
}

static void pack_sheader_data(struct gpt_header *header)
{
    header->header_crc32 = 0;
    header->my_lba = header->alternate_lba;
    header->alternate_lba = 1;
    header->partition_entry_lba = get_gpt_header_last_usable_lba() + 1;
    header->partition_entry_array_crc32 = get_entries_crc32();
    header->header_crc32 = crc32(0, (unsigned char *)header, sizeof(struct gpt_header));
}

static void pack_entries_data(struct gpt_header *header, struct gpt_entry *entries)
{
    int i;
    int nr_parts = 0;

    //calculate how many partitions
    for (i = 0; i < (int)header->max_partition_count; i++) {
        if (!entries[i].partition_name[0]) {
            nr_parts = i;
            break;
        }
    }

    /* Update gpt entries */
    for (i = nr_parts-1; i >= 0; i--) {
        /* it's last partition (do not have reserved partition ex: no flashinfo, otp),
         * only need to update last partition
         * may not go to here unless customer remove flashinfo partition
         */
        if (i == nr_parts-1 && !entries[i].starting_lba && !entries[i].ending_lba) {
            entries[i].ending_lba = last_lba() - get_spgt_partition_lba_size();
            entries[i].starting_lba = entries[i-1].ending_lba + 1;
            break;
        }

        /* Process reserved partitions(flashinfo, otp) and last partition before
         * reserved partition(ex: userdata or intsd)
         * Reserved partition size is stored in ending_lba, sgpt partition size can be
         * retrived from  get_spgt_partition_size()(stored in header->reserved)
         */
        if (!entries[i].starting_lba) {
                /* it's a reserved partition and it's last partition,
                 * entries[i].ending_lba not empty(partition size is here)
                 */
            if (i == nr_parts-1 && entries[i].ending_lba) {
                entries[i].starting_lba =
                    last_lba() - get_spgt_partition_lba_size() - entries[i].ending_lba + 1;
                entries[i].ending_lba = last_lba() - get_spgt_partition_lba_size();
            } else if (entries[i].ending_lba) {
                /* reserved parttiion but not last one
                 * may not go to here uless there exists more than one reserved partitions
                 * (ex: otp + flashinfo)
                 */
                entries[i].starting_lba = entries[i+1].starting_lba - entries[i].ending_lba;
                entries[i].ending_lba = entries[i+1].starting_lba - 1;
            } else {
                /* userdata or intsed */
                entries[i].starting_lba = entries[i-1].ending_lba + 1;
                entries[i].ending_lba = entries[i+1].starting_lba - 1;
            }
        }
    }

    set_entries_crc32(crc32(0, (uint8_t *)entries,
                            sizeof(struct gpt_entry) * header->max_partition_count));
}

int write_primary_gpt(bdev_t *boot_dev, struct gpt_header *header, struct gpt_entry *entries)
{
    int err;
    uint32_t part_entries_cnt_per_block;
    uint64_t pentries_write_size;

    if (header->alternate_lba != last_lba())
        set_spgt_partition_lba_size(header->alternate_lba); //sgpt partition size is here
    else
        set_spgt_partition_lba_size(last_lba() - header->last_usable_lba);

    pack_entries_data(header, entries); //move to gpt entries

    pack_pheader_data(header); //update pheader

    /* Write pheader */
    err = bio_write(boot_dev, (uint8_t *)header, boot_dev->block_size, boot_dev->block_size);
    if (err < 0 || (uint32_t)err != boot_dev->block_size) {
        LTRACEF_LEVEL(ALWAYS, "[GPT_Update]write pheader, err(%d)\n", err);
        return err;
    }

    /* Write gpt entries */
    part_entries_cnt_per_block = boot_dev->block_size / header->partition_entry_size;
    pentries_write_size = (uint64_t)((header->max_partition_count + part_entries_cnt_per_block - 1)
        / part_entries_cnt_per_block) * boot_dev->block_size;
    err = bio_write(boot_dev, (uint8_t *)entries, 2 * boot_dev->block_size, pentries_write_size);
    if (err < 0 || (uint64_t)err != pentries_write_size) {
        LTRACEF_LEVEL(ALWAYS, "[GPT_Update]write pentries, err(%d)\n", err);
        return err;
    }
    return 0;
}

int write_secondary_gpt(bdev_t *boot_dev, struct gpt_header *header, struct gpt_entry *entries)
{
    int err;
    uint32_t part_entries_cnt_per_block;
    uint64_t pentries_write_size;
    uint64_t sentries_start_lba;

    /* fastboot PGPT image check */
    pack_sheader_data(header); //update sheader

    /* Write secondary header */
    err = bio_write(boot_dev,
        (uint8_t *)header, last_lba() * boot_dev->block_size, boot_dev->block_size);
    if (err < 0 || (uint32_t)err != boot_dev->block_size) {
        LTRACEF_LEVEL(ALWAYS, "[GPT_Update]write sheader, err(%d)\n", err);
        return err;
    }

    /* Write secondary entries */
    sentries_start_lba = get_gpt_header_last_usable_lba() + 1;
    part_entries_cnt_per_block = boot_dev->block_size / header->partition_entry_size;
    pentries_write_size = (u64)((header->max_partition_count + part_entries_cnt_per_block - 1)
        / part_entries_cnt_per_block) * boot_dev->block_size;
    err = bio_write(boot_dev, (uint8_t *)entries, sentries_start_lba, pentries_write_size);
    if (err < 0 || (uint64_t)err != pentries_write_size) {
        LTRACEF_LEVEL(ALWAYS, "[GPT_Update]write pentries, err(%d)\n", err);
        return err;
    }

    return 0;
}
