/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <bootctrl.h>
#include <compiler.h>
#include <debug.h>
#include <errno.h>
#include <lib/bio_ext.h>
#include <lib/cksum.h>
#include <stdint.h>
#include <string.h>
#include <trace.h>

#define LOCAL_TRACE 0

#define BOOTCTRL_PARTITION      "misc"
#define OFFSETOF_SLOT_SUFFIX    2048
#define BOOTCTRL_MAGIC          0x42414342
#define BOOTCTRL_VERSION        1
#define BOOT_CONTROL_MAX_RETRY  7
#define BOOT_CONTROL_MAX_PRI    7
#define READ_PARTITION          0
#define WRITE_PARTITION         1
#define PART_BOOT1              1
#define PART_BOOT2              2

#ifndef PART_BOOTDEVICE_NAME
#define PART_BOOTDEVICE_NAME "bootdevice"
#endif

struct slot_metadata {
    // Slot priority with 15 meaning highest priority, 1 lowest
    // priority and 0 the slot is unbootable.
    uint8_t priority : 4;
    // Number of times left attempting to boot this slot.
    uint8_t tries_remaining : 3;
    // 1 if this slot has booted successfully, 0 otherwise.
    uint8_t successful_boot : 1;
    // 1 if this slot is corrupted from a dm-verity corruption, 0
    // otherwise.
    uint8_t verity_corrupted : 1;
    // Reserved for further use.
    uint8_t reserved : 7;
} __PACKED;

/* Bootloader Control AB
 *
 * This struct can be used to manage A/B metadata. It is designed to
 * be put in the 'slot_suffix' field of the 'bootloader_message'
 * structure described above. It is encouraged to use the
 * 'bootloader_control' structure to store the A/B metadata, but not
 * mandatory.
 */
struct bootloader_control {
    // NUL terminated active slot suffix.
    char slot_suffix[4];
    // Bootloader Control AB magic number (see BOOT_CTRL_MAGIC).
    uint32_t magic;
    // Version of struct being used (see BOOT_CTRL_VERSION).
    uint8_t version;
    // Number of slots being managed.
    uint8_t nb_slot : 3;
    // Number of times left attempting to boot recovery.
    uint8_t recovery_tries_remaining : 3;
    // Status of any pending snapshot merge of dynamic partitions.
    uint8_t merge_status : 3;
    // Ensure 4-bytes alignment for slot_info field.
    uint8_t reserved0[1];
    // Per-slot information.  Up to 4 slots.
    struct slot_metadata slot_info[4];
    // Reserved for further use.
    uint8_t reserved1[8];
    // CRC32 of all 28 bytes preceding this field (little endian
    // format).
    uint32_t crc32_le;
} __PACKED;

static const char *suffix[2] = {BOOTCTRL_SUFFIX_A, BOOTCTRL_SUFFIX_B};

int check_suffix_with_slot(const char *suffix)
{
    int slot = -1;

    if (!suffix) {
        LTRACEF_LEVEL(CRITICAL, "[LK_AB] %s check args failed", __func__);
        return slot;
    }

    if (!strcmp(suffix, BOOTCTRL_SUFFIX_A))
        slot = 0;
    else if (!strcmp(suffix, BOOTCTRL_SUFFIX_B))
        slot = 1;
    else
        LTRACEF_LEVEL(CRITICAL, "[LK_AB] unknown slot suffix\n");

    return slot;
}

static void initDefaultBootControl(struct bootloader_control *bctrl)
{
    int slot = 0;
    struct slot_metadata *slotp;

    if (!bctrl) {
        LTRACEF_LEVEL(CRITICAL, "[LK_AB] %s check args failed", __func__);
        return;
    }

    bctrl->magic = BOOTCTRL_MAGIC;
    bctrl->version = BOOTCTRL_VERSION;
    bctrl->nb_slot = BOOTCTRL_NUM_SLOTS;

    /* Set highest priority and reset retry count */
    for (slot = 0; slot < BOOTCTRL_NUM_SLOTS; slot++) {
        slotp = &bctrl->slot_info[slot];
        slotp->successful_boot = 0;
        slotp->priority = BOOT_CONTROL_MAX_PRI;
        slotp->tries_remaining = BOOT_CONTROL_MAX_RETRY;
    }

    bctrl->crc32_le = crc32(0, (const unsigned char *)bctrl,
            sizeof(struct bootloader_control) - sizeof(uint32_t));
}

static int read_write_partition_info(struct bootloader_control *bctrl, int mode)
{
    if (bctrl == NULL) {
        LTRACEF_LEVEL(CRITICAL, "[LK_AB] %s failed, bctrl is NULL\n", __func__);
        return -EINVAL;
    }

    if (mode != READ_PARTITION && mode != WRITE_PARTITION) {
        LTRACEF_LEVEL(CRITICAL, "[LK_AB] %s failed, mode is %d\n", __func__, mode);
        return -EINVAL;
    }

    LTRACEF_LEVEL(INFO, "[LK_AB] %s\n",
            mode == READ_PARTITION ? "READ_PARTITION" : "WRITE_PARTITION");

    bdev_t *dev_misc = bio_open_by_label(BOOTCTRL_PARTITION);

    if (!dev_misc) {
        LTRACEF_LEVEL(CRITICAL, "[LK_AB] open misc fail\n");
        return -ENODEV;
    }

    if (mode == READ_PARTITION) {
        struct bootloader_control boot_control;

        int read_size = bio_read(dev_misc, (uint8_t *) &boot_control,
                    (off_t) OFFSETOF_SLOT_SUFFIX,  (size_t) sizeof(struct bootloader_control));
        if (read_size <= 0) {
            LTRACEF_LEVEL(CRITICAL, "[LK_AB] read boot_control fail\n");
            bio_close(dev_misc);
            return -EIO;
        }

        LTRACEF_LEVEL(INFO, "[LK_AB] magic=0x%x, priority: [0]=0x%x, [1]=0x%x\n",
                boot_control.magic,
                boot_control.slot_info[0].priority,
                boot_control.slot_info[1].priority);

        if (!boot_control.magic) {
            LTRACEF_LEVEL(CRITICAL,
                "[LK_AB] boot_ctrl magic number is zero for nonAB project without initialzation\n");
            return 0;
        } else if (boot_control.magic != BOOTCTRL_MAGIC) {
            initDefaultBootControl(&boot_control);
            read_write_partition_info(&boot_control, WRITE_PARTITION);
        }

        memcpy(bctrl, &boot_control, sizeof(struct bootloader_control));
    } else if (mode == WRITE_PARTITION) {
        if (bio_write(dev_misc, (uint8_t *) bctrl,
                (off_t) OFFSETOF_SLOT_SUFFIX,
                (size_t) sizeof(struct bootloader_control)) <= 0) {
            LTRACEF_LEVEL(CRITICAL, "[LK_AB] bio_write failed\n");
            bio_close(dev_misc);
            return -EIO;
        }
    }
    bio_close(dev_misc);

    return 0;
}

const char *get_suffix(void)
{
    int slot = 0, ret = -1;

    struct bootloader_control metadata;

    ret = read_write_partition_info(&metadata, READ_PARTITION);
    if (ret < 0) {
        LTRACEF_LEVEL(CRITICAL, "[LK_AB] read_partition_info failed, ret: 0x%x\n", ret);
        return "";
    }

    if (metadata.magic != BOOTCTRL_MAGIC) {
        LTRACEF_LEVEL(CRITICAL,
            "[LK_AB] boot_ctrl magic number is not match %x , BOOTCTRL_MAGIC = %x\n",
            metadata.magic, BOOTCTRL_MAGIC);
        return "";
    } else {
        LTRACEF_LEVEL(CRITICAL,
                "[LK_AB] boot_ctrl magic number is match, compare priority %d, %d\n",
                metadata.slot_info[0].priority, metadata.slot_info[1].priority);

        if (metadata.slot_info[0].priority >= metadata.slot_info[1].priority)
            slot = 0;
        else if (metadata.slot_info[0].priority < metadata.slot_info[1].priority)
            slot = 1;
    }

    return suffix[slot];
}

/*
 * get_bctrl_mdata - Get boot control metadata in MISC.
 *
 * @slot: The slot number e.g. 0 for _a and 1 for _b
 * @field: An id which is a one to one mapping for each element in the Metadata
 */
int get_bctrl_mdata(unsigned int slot, unsigned int field)
{
    int ret = 0;

    struct bootloader_control metadata;

    if (slot >= BOOTCTRL_NUM_SLOTS) {
        LTRACEF_LEVEL(CRITICAL, "[LK_AB] wrong slot number %d\n", slot);
        return -EINVAL;
    }

    ret = read_write_partition_info(&metadata, READ_PARTITION);
    if (ret < 0) {
        LTRACEF_LEVEL(CRITICAL, "[LK_AB] read_partition_info failed, ret: 0x%x\n", ret);
        return ret;
    }

    if (metadata.magic != BOOTCTRL_MAGIC) {
        LTRACEF_LEVEL(CRITICAL,
            "[LK_AB] boot_ctrl magic number is not match %x , BOOTCTRL_MAGIC = %x\n",
            metadata.magic, BOOTCTRL_MAGIC);
        return -ENXIO;
    }

    switch (field) {
    case BOOTCTRL_UNBOOT:
        return metadata.slot_info[slot].priority > 0 ? 0 : 1;
    case BOOTCTRL_SUCCESS:
        return metadata.slot_info[slot].successful_boot;
    case BOOTCTRL_RETRY:
        return metadata.slot_info[slot].tries_remaining;
    default:
        LTRACEF_LEVEL(CRITICAL, "[LK_AB] Invalid field %d\n", field);
        return -EINVAL;
    }
}

bool is_ab_enable(void)
{
    if (!strncmp(get_suffix(), BOOTCTRL_SUFFIX_A, strlen(BOOTCTRL_SUFFIX_A)) ||
        !strncmp(get_suffix(), BOOTCTRL_SUFFIX_B, strlen(BOOTCTRL_SUFFIX_B)))
        return true;
    else
        return false;
}

int set_active_slot(const char *suffix)
{
    int slot = 0, slot1 = 0;
    int ret = -1;
    struct slot_metadata *slotp;
    struct bootloader_control metadata;
    u32 boot_part;
    bdev_t *bdev;

    if (!suffix) {
        LTRACEF_LEVEL(CRITICAL, "[LK_AB] %s check args failed", __func__);
        return ret;
    }

    slot = check_suffix_with_slot(suffix);
    if (slot < 0 || slot >= BOOTCTRL_NUM_SLOTS) {
        LTRACEF_LEVEL(CRITICAL, "[LK_AB] %s failed, slot: 0x%x\n", __func__, slot);
        return ret;
    }

    if (suffix == NULL) {
        LTRACEF_LEVEL(CRITICAL, "[LK_AB] input suffix is NULL\n");
        return ret;
    }

    ret = read_write_partition_info(&metadata, READ_PARTITION);
    if (ret < 0) {
        LTRACEF_LEVEL(CRITICAL, "[LK_AB] partition_read failed, ret: 0x%x\n", ret);
        return ret;
    }

    metadata.magic = BOOTCTRL_MAGIC;

    /* Set highest priority and reset retry count */
    slotp = &metadata.slot_info[slot];
    slotp->successful_boot = 0;
    slotp->priority = BOOT_CONTROL_MAX_PRI;
    slotp->tries_remaining = BOOT_CONTROL_MAX_RETRY;

    /* Ensure other slot doesn't have as high a priority. */
    slot1 = (slot == 0) ? 1 : 0;
    slotp = &metadata.slot_info[slot1];
    if (slotp->priority >= BOOT_CONTROL_MAX_PRI)
        slotp->priority = BOOT_CONTROL_MAX_PRI - 1;

    metadata.crc32_le = crc32(0, (const unsigned char *)&metadata,
            sizeof(struct bootloader_control) - sizeof(uint32_t));

    /* Switch boot part in boot region */
    boot_part = slot ? PART_BOOT2 : PART_BOOT1;
    bdev = bio_open(PART_BOOTDEVICE_NAME);
    if (!bdev) {
        LTRACEF_LEVEL(CRITICAL, "[LK_AB] open bdev fail\n");
        return -ENODEV;
    }

    ret = bio_ioctl(bdev, BIO_IOCTL_SET_BOOT_REGION, (void *) &boot_part);
    if (ret < 0) {
        LTRACEF_LEVEL(CRITICAL, "[LK_AB] set to slot %d fail, ret 0x%x\n",
                      slot, ret);
        bio_close(bdev);
        return ret;
    }
    bio_close(bdev);

    ret = read_write_partition_info(&metadata, WRITE_PARTITION);
    if (ret < 0) {
        LTRACEF_LEVEL(CRITICAL, "[LK_AB] partition_write failed, ret 0x%x\n",
                      ret);
        return ret;
    }

    return 0;
}
