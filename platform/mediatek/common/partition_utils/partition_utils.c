/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <bootctrl.h>
#include <err.h>
#include <errno.h>
#include <lib/bio.h>
#include <lib/bio_ext.h>
#include <kernel/mutex.h>
#include <partition_utils.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "table.h"
#include <trace.h>
#include <ufs_interface.h>

#define EXT4_OFFSET                 (0x438) /* ext4 offset where the magic exists */
#define EXT4_MAGIC                  (0xEF53)
#define EXT4_MAGIC_SIZE             (2)

int partition_get_type(const char *name, int *p_type)
{
    if (!name || !p_type)
        return -EINVAL;

    bdev_t *dev;
    ssize_t len;
    unsigned int buf = 0;

    // FDE(Full-Disk-Encryption) would encrypt the userdata partition.
    // Can not get the magic in raw data.
    if (!strcmp(name, "userdata")) {
        if (!strcmp(MTK_USERIMAGES_FORMAT, "ext4"))
            *p_type = PART_EXT4;
        else
            *p_type = PART_F2FS;
        return NO_ERROR;
    }

    dev = bio_open_by_label(name);
    if (!dev) {
        dprintf(CRITICAL, "Partition [%s] is not exist.\n", name);
        return -ENODEV;
    }

    len = bio_read(dev, &buf, EXT4_OFFSET, EXT4_MAGIC_SIZE);

    if (len < 0 || len != (ssize_t)EXT4_MAGIC_SIZE) {
        bio_close(dev);
        return -EIO;
    }

    if (buf == EXT4_MAGIC)
        *p_type = PART_EXT4;
    else
        *p_type = PART_RAW;

    bio_close(dev);
    return NO_ERROR;
}

int partition_get_name_list(char **name_list, int *count)
{
    int i;
    struct part_meta_info *info = NULL;

    if (!name_list || !count) {
        dprintf(CRITICAL, "[PART_UTILS] Need buffer with %d entries\n", NUM_PARTITIONS);
        return -EINVAL;
    }

    // reset and get partition table
    if (partition_reset_tbl() < 0)
        return -EIO;

    info = get_info();
    //assign partition entries
    for (i = 0; i < get_part_count(); i++)
        name_list[i] = (char *)&info[i].name;

    *count = get_part_count();
    return NO_ERROR;
}

/*
 * Update gpt layout with re-size entry, named part_name. Using delta_lba
 * to modify the bondiary of entry which is named part_name and ahead entry.
 */
int partition_update_table(char *part_name, int delta_lba)
{
    int i;
    struct gpt_entry *entry_tbl = NULL;
    struct part_meta_info *info = NULL;

    if (!part_name)
        return -EINVAL;
    // reset and get partition table
    if (partition_reset_tbl() < 0)
        return -EIO;

    entry_tbl = get_entry();
    info = get_info();

    if (!entry_tbl || !info) {
        dprintf(CRITICAL, "Failed to get partition table!\n");
        return -EINVAL;
    }
    // Check first entry
    if (!strncmp((const char *)info[0].name,
                 (const char *)part_name, MAX_GPT_NAME_SIZE)) {
        dprintf(CRITICAL, "Do not re-size the first partition: %s\n", part_name);
        free(entry_tbl);
        return -EINVAL;
    }

    for (i = 1; i < get_part_count(); i++) {
        if (!strncmp((const char *)info[i].name,
                     (const char *)part_name, MAX_GPT_NAME_SIZE)) {
            entry_tbl[i-1].ending_lba -= delta_lba;
            entry_tbl[i].starting_lba -= delta_lba;
            part_entry_update(get_header(), entry_tbl);
            free(entry_tbl);
            return NO_ERROR;
        }
    }

    dprintf(CRITICAL, "find no partition named %s\n", part_name);
    free(entry_tbl);
    return -EINVAL;
}

/*
 * Update gpt layout with given gpt
 */
int partition_update_gpt_table(uint8_t *gpt_raw, unsigned int size)
{
    int ret = NO_ERROR;
    unsigned int gpt_offset = 0, hdr_offset = 0;
    bdev_t *bdev = NULL;
    struct gpt_header *header = NULL;
    struct gpt_entry *entries = NULL;

    if (!gpt_raw)
        return -EINVAL;

    bdev = bio_open(PART_BOOTDEVICE_NAME);
    if (!bdev) {
        dprintf(CRITICAL, "Partition [%s] is not exist.\n", PART_BOOTDEVICE_NAME);
        return -ENODEV;
    }

    /* fastboot PGPT image check */
    if (((uint32_t *) gpt_raw)[0] == GPT_SIGNATURE_2 ||
            ((uint32_t *) gpt_raw)[1] == GPT_SIGNATURE_1) {
        gpt_offset = bdev->block_size;
        hdr_offset = 0;
    } else {
        //get header addr with mbr offset
        header = (struct gpt_header *)(gpt_raw + bdev->block_size);
        gpt_offset = bdev->block_size * 2;
        hdr_offset = bdev->block_size;
    }

    bio_close(bdev);
    header = (struct gpt_header *)(gpt_raw + hdr_offset);
    if (size != (header->max_partition_count * sizeof(struct gpt_entry) + gpt_offset)) {
        dprintf(CRITICAL, "[GPT_Update] Wrong Content Binary ent_count %u size %u need to be %lu\n",
            header->max_partition_count, size,
            (header->max_partition_count * sizeof(struct gpt_entry) + gpt_offset));
        return -EINVAL;
    }

    entries = (struct gpt_entry *)(gpt_raw + gpt_offset);
    dprintf(CRITICAL, "[GPT_Update] update pgt with gpt_offset(%u)\n", gpt_offset);
    ret = part_entry_update(header, entries);
    table_dump();
    return ret;
}

int partition_get_uuid_by_name(const char *name, char **out)
{
    return get_uuid_by_name(name, out);
}

int partition_get_bootdev_type(void)
{
    int bootdev_type = BOOTDEV_SDMMC;
    bdev_t *bdev = NULL;
    int ret = NO_ERROR;

    bdev = bio_open("preloader_a");
    if (!bdev) {
        dprintf(CRITICAL, "Partition [preloader_a] is not exist.\n");
        ret = -ENODEV;
        goto out;
    }

    ret = bio_ioctl(bdev, BIO_IOCTL_GET_BOOTDEV_TYPE, (void *) &bootdev_type);
    if (ret != 0) {
        dprintf(CRITICAL, "bio get bootdev type fialed\n");
        ret = -ENOSPC;
        goto out;
    }

    dprintf(INFO, "bootdev_type %d.\n", bootdev_type);

out:
    if (bdev != NULL)
        bio_close(bdev);

    return bootdev_type;
}

void partition_table_dump(void)
{
    table_dump();
}
/*
 * return active 0 or 1, or negative error code if any.
 */
static int partition_get_active_bit_by_name(const char *part_name)
{
    int ret = 0;
    struct gpt_entry *entry = NULL;

    ret = get_entry_by_name(part_name, &entry);
    if (ret != NO_ERROR)
        return -EINVAL;

    if (!entry)
        return -ENODATA;

    return (entry->attributes & PART_ATTR_LEGACY_BIOS_BOOTABLE);
}
/*
 * return 1 means exist; 0 is not.
 */
static int get_exist_part(const char *part_name)
{
    int part_cnt = 0, i = 0;
    char *name_list[NUM_PARTITIONS] = {NULL};

    partition_get_name_list((char **)&name_list, &part_cnt);
    for (i = 0; i < part_cnt; i++) {
        if (!strncmp(part_name, name_list[i], MAX_GPT_NAME_SIZE))
            return 1;
    }

    return 0;
}
/*
 * while ab load, return part_name + get_suffix().
 * in non-ab, return active partition if has backup.
 */
void revise_part_name(const char *part_name, char *revised_part_name,
       int revised_part_name_size)
{
    int err = 0;
    char name_with_suffix[MAX_GPT_NAME_SIZE] = {0};

    if (!part_name || !revised_part_name || revised_part_name_size != MAX_GPT_NAME_SIZE) {
        dprintf(CRITICAL, "[Part] illegal ptr %p size %d\n", part_name, revised_part_name_size);
        return;
    }
    /*
     * the following code is to check the real name in gpt entry:
     * firstly, check is ab load or not. if ab load check name + suffix.
     * if non-ab format would check is the part with backup or not then
     * check active bits. if still can't find above, try to access original name.
     */
    if (is_ab_enable()) { // try ab
        memset(name_with_suffix, 0x0, MAX_GPT_NAME_SIZE);
        err = snprintf(name_with_suffix, MAX_GPT_NAME_SIZE - 1, "%s%s", part_name, get_suffix());
        if (err < 0) {
            dprintf(CRITICAL, "Fail to revise part name (%d)\n", err);
            return;
        }

        if (get_exist_part(name_with_suffix))
            goto update_real_name;
    } else { // try nonab with backup
        memset(name_with_suffix, 0x0, MAX_GPT_NAME_SIZE);
        err = snprintf(name_with_suffix, MAX_GPT_NAME_SIZE - 1, "%s%s", part_name, "2");
        if (err < 0) {
            dprintf(CRITICAL, "Fail to revise part name (%d)\n", err);
            return;
        }

        if (!get_exist_part(name_with_suffix))
            goto try_original;

        if (partition_get_active_bit_by_name(name_with_suffix) != 1) {
            memset(name_with_suffix, 0x0, MAX_GPT_NAME_SIZE);
            err = snprintf(name_with_suffix, MAX_GPT_NAME_SIZE - 1, "%s%s", part_name, "1");
            if (err < 0) {
                dprintf(CRITICAL, "Fail to revise part name (%d)\n", err);
                return;
            }

            if (!get_exist_part(name_with_suffix))
                goto try_original;
        }
        // get partition name with backup
        goto update_real_name;
    }

try_original:
    memset(name_with_suffix, 0x0, MAX_GPT_NAME_SIZE);
    err = snprintf(name_with_suffix, MAX_GPT_NAME_SIZE - 1, "%s", part_name);
    if (err < 0) {
        dprintf(CRITICAL, "Fail to revise part name (%d)\n", err);
        return;
    }

update_real_name:
    memset(revised_part_name, 0x0, MAX_GPT_NAME_SIZE);
    err = snprintf(revised_part_name, MAX_GPT_NAME_SIZE - 1, "%s", name_with_suffix);
    if (err < 0) {
        dprintf(CRITICAL, "Fail to revise part name (%d)\n", err);
        return;
    }

    dprintf(INFO, "[Part] get %s by %s\n", revised_part_name, part_name);
}
/*
 * get active preloader name
 */
int partition_get_active_preloader(char *part_name, const int name_size)
{
    int ret = NO_ERROR, active_boot_part = 1; // default select LU0
    bdev_t *bdev = NULL;

    if (!part_name || name_size != MAX_GPT_NAME_SIZE) {
        dprintf(CRITICAL, "[Part] illegal ptr %p size %d\n", part_name, name_size);
        return -EINVAL;
    }

    bdev = bio_open("preloader_a");
    if (!bdev) {
        dprintf(CRITICAL, "Partition [preloader_a] is not exist.\n");
        return -ENODEV;
    }

    ret = bio_ioctl(bdev, BIO_IOCTL_GET_ACTIVE_BOOT, (void *)&active_boot_part);
    if (ret != 0) {
        dprintf(CRITICAL, "[Part_WP] bio get fialed\n");
        ret = -ENOSPC;
        goto out;
    }

    if (active_boot_part == 1)
        strncpy(part_name, "preloader_a", sizeof("preloader_a"));
    else
        strncpy(part_name, "preloader_b", sizeof("preloader_b"));
out:
    bio_close(bdev);
    return ret;
}
