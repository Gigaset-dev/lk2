/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <bootctrl.h>
#include <debug.h>
#include <errno.h>
#include <init_mtk.h>
#include <lib/bio.h>
#include <lib/kcmdline.h>
#include <lk/init.h>
#include <platform/boot_trap.h>
#include <platform/boot_part.h>
#include <reg.h>
#include <stdint.h>
#include <string.h>

#define BOOTCTRL_PARTITION      "misc"
#define OFFSETOF_SLOT_SUFFIX    2048
#define BOOTCTRL_MAGIC          0x30424100
#define BOOTCTRL_MAJOR_VERSION  1
#define BOOTCTRL_MINOR_VERSION  0
#define BOOT_CONTROL_MAX_RETRY  3
#define BOOT_CONTROL_MAX_PRI    15
#define BOOTCTRL_NUM_SLOTS      2
#define READ_PARTITION          0
#define WRITE_PARTITION         1

static const char *suffix[2] = {BOOTCTRL_SUFFIX_A, BOOTCTRL_SUFFIX_B};
static struct bootloader_control boot_control;
static int bootctl_exists;

static void initDefaultBootControl(struct bootloader_control *bctrl)
{
    int slot = 0;
    struct slot_metadata *slotp;

    bctrl->magic = BOOTCTRL_MAGIC;
    bctrl->version_major = BOOTCTRL_MAJOR_VERSION;
    bctrl->version_minor = BOOTCTRL_MINOR_VERSION;

    /* Set highest priority and reset retry count */
    for (slot = 0; slot < BOOTCTRL_NUM_SLOTS; slot++) {
        slotp = &bctrl->slot_info[slot];
        slotp->successful_boot = 0;
        slotp->priority = BOOT_CONTROL_MAX_PRI;
        slotp->retry_count = BOOT_CONTROL_MAX_RETRY;
    }
}

static int read_write_partition_info(struct bootloader_control *bctrl, int mode)
{
    if (bctrl == NULL) {
        dprintf(CRITICAL, "[LK_AB] %s failed, bctrl is NULL\n", __func__);
        return -EINVAL;
    }
    if (mode != READ_PARTITION && mode != WRITE_PARTITION) {
        dprintf(CRITICAL, "[LK_AB] %s failed, mode is %d\n", __func__, mode);
        return -EINVAL;
    }
    dprintf(INFO, "[LK_AB] %s\n",
            mode == READ_PARTITION ? "READ_PARTITION" : "WRITE_PARTITION");
    if (mode == READ_PARTITION) {
        if (boot_control.magic != BOOTCTRL_MAGIC) {
            bdev_t *dev_misc = bio_open_by_label(BOOTCTRL_PARTITION);

            if (!dev_misc) {
                dprintf(CRITICAL, "[LK_AB] open misc fail\n");
                return -ENODEV;
            }

            int read_size = bio_read(dev_misc, (uint8_t *) &boot_control,
                    (off_t) OFFSETOF_SLOT_SUFFIX,  (size_t) sizeof(struct bootloader_control));
            if (read_size <= 0) {
                dprintf(CRITICAL, "[LK_AB] read boot_control fail\n");
                bio_close(dev_misc);
                return -EIO;
            }
            dprintf(INFO, "[LK_AB] magic=0x%x, [0]=0x%02x%02x%02x, [1]=0x%02x%02x%02x\n",
                    boot_control.magic,
                    boot_control.slot_info[0].priority,
                    boot_control.slot_info[0].retry_count,
                    boot_control.slot_info[0].successful_boot,
                    boot_control.slot_info[1].priority,
                    boot_control.slot_info[1].retry_count,
                    boot_control.slot_info[1].successful_boot);
            if (boot_control.magic != BOOTCTRL_MAGIC) {
                initDefaultBootControl(&boot_control);
                bootctl_exists = 0;
            } else {
                bootctl_exists = 1;
            }
            bio_close(dev_misc);
        }
        memcpy(bctrl, &boot_control, sizeof(struct bootloader_control));
    } else if (mode == WRITE_PARTITION) {
        if (!bootctl_exists) {
            dprintf(CRITICAL, "[LK_AB] skip bootctrl write because it's not inited\n");
            return -ENODATA;
        }
        bdev_t *dev_misc = bio_open_by_label(BOOTCTRL_PARTITION);

        if (!dev_misc) {
            dprintf(CRITICAL, "[LK_AB] open misc fail\n");
            return -ENODEV;
        }
        if (bio_write(dev_misc, (uint8_t *) bctrl,
                (off_t) OFFSETOF_SLOT_SUFFIX,
                (size_t) sizeof(struct bootloader_control)) <= 0) {
            dprintf(CRITICAL, "[LK_AB] bio_write failed\n");
            bio_close(dev_misc);
            return -EIO;
        }
        bio_close(dev_misc);
        memcpy(&boot_control, bctrl, sizeof(struct bootloader_control));
    }

    return 0;
}

const char *get_suffix(void)
{
    int slot = 0, ret = -1;

    struct bootloader_control metadata;

    ret = read_write_partition_info(&metadata, READ_PARTITION);
    if (ret < 0) {
        dprintf(CRITICAL, "[LK_AB] read_partition_info failed, ret: 0x%x\n", ret);
        return "";
    }

    if (metadata.magic != BOOTCTRL_MAGIC) {
        dprintf(CRITICAL, "[LK_AB] magic is not match 0x%x != 0x%x\n",
                metadata.magic, BOOTCTRL_MAGIC);
        return "";
    }

    if (metadata.slot_info[0].priority >= metadata.slot_info[1].priority)
        slot = 0;
    else if (metadata.slot_info[0].priority < metadata.slot_info[1].priority)
        slot = 1;

    return suffix[slot];
}

bool is_ab_enable(void)
{
    return true;
}

static void bootctrl_init(uint level)
{
    const char *ab_suffix = get_suffix();

    if (!strcmp(ab_suffix, BOOTCTRL_SUFFIX_A)) {
        kcmdline_append("bootslot=a");
        switch (readl(TRAP_TEST_RD) & BOOT_TRAP_MASK) {
        case BOOT_FROM_EMMC:
            kcmdline_subst(EMMC_KCMDLINE_ROOT, EMMC_KCMDLINE_ROOT_A);
            break;
        case BOOT_FROM_SNAND:
        case BOOT_FROM_PNAND:
            kcmdline_subst(NAND_KCMDLINE_ROOT, NAND_KCMDLINE_ROOT_A);
            break;
        }
    } else if (!strcmp(ab_suffix, BOOTCTRL_SUFFIX_B)) {
        kcmdline_append("bootslot=b");
        switch (readl(TRAP_TEST_RD) & BOOT_TRAP_MASK) {
        case BOOT_FROM_EMMC:
            kcmdline_subst(EMMC_KCMDLINE_ROOT, EMMC_KCMDLINE_ROOT_B);
            break;
        case BOOT_FROM_SNAND:
        case BOOT_FROM_PNAND:
            kcmdline_subst(NAND_KCMDLINE_ROOT, NAND_KCMDLINE_ROOT_B);
            break;
        }
    }
}

MT_LK_INIT_HOOK(BL33, bootctrl, bootctrl_init, LK_INIT_LEVEL_APPS - 1);
