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

#include "ufs.h"
#include "ufs_core.h"
#include "ufs_error.h"
#include "ufs_hcd.h"
#include "ufs_rpmb.h"
#include "ufs_types.h"
#include "ufs_utils.h"

#include <err.h>
#include <lib/bio.h>
#include <lib/bio_ext.h>
#include <lib/partition.h>
#include <stdlib.h>
#include <string.h> //For memcpy, memset
#include <trace.h>
#include <ufs_cfg.h>
#include <ufs_interface.h>

#define LOCAL_TRACE 0

struct ufs_dev_t {
    bdev_t bdev;
    u32 part_id;
    struct ufs_hba *host;
};

struct wp_args {
    unsigned long blknr;
    u32 blkcnt;
    u8 type;
};

static ssize_t ufs_wrap_bread(struct bdev *dev, void *dst, bnum_t blknr,
                              uint blkcnt)
{
    int lun;
    int ret;
    struct ufs_dev_t *__dev = (struct ufs_dev_t *)dev;
    struct ufs_hba *hba = __dev->host;

    lun = ufs_switch_part(__dev->part_id);

    if (lun < 0)
        return ERR_INVALID_ARGS;

    mutex_acquire(&hba->lock);

    ret = ufs_block_read(UFS_DEFAULT_HOST_ID, lun, blknr, blkcnt, (unsigned long *)dst);

    LTRACEF("[UFS] r, %u, %u, %d, %d\n", (u32)blknr, blkcnt, lun, ret);

    mutex_release(&hba->lock);

    if (!ret)
        return (ssize_t)dev->block_size * blkcnt;
    else
        return ERR_IO;
}
static ssize_t ufs_wrap_bwrite(struct bdev *dev, const void *src, bnum_t blknr,
                               uint blkcnt)
{
    int lun;
    int ret;
    struct ufs_dev_t *__dev = (struct ufs_dev_t *)dev;
    struct ufs_hba *hba = __dev->host;

    lun = ufs_switch_part(__dev->part_id);

    if (lun < 0)
        return ERR_INVALID_ARGS;

    mutex_acquire(&hba->lock);

    ret = ufs_block_write(UFS_DEFAULT_HOST_ID, lun, blknr, blkcnt, (unsigned long *)src);

    LTRACEF("[UFS] w, %u, %u, %d, %d\n", (u32)blknr, blkcnt, lun, ret);

    mutex_release(&hba->lock);

    if (!ret)
        return (ssize_t)dev->block_size * blkcnt;
    else
        return ERR_IO;
}

static ssize_t ufs_erase(struct bdev *bdev, off_t start_addr, size_t len)
{
    u32 start_blk, end_blk;
    int lun;
    int ret;
    struct ufs_dev_t *__dev = (struct ufs_dev_t *)bdev;
    struct ufs_hba *hba = __dev->host;

    if (!len) {
        dprintf(CRITICAL, "[UFS] err: invalid erase size! len: 0x%zx\n", len);
        return ERR_INVALID_ARGS;
    }

    if ((start_addr % bdev->block_size) || (len % bdev->block_size)) {
        dprintf(CRITICAL,
            "[UFS] err: non-aligned erase address or length! start: 0x%llx, len: 0x%zx\n",
            start_addr, len);
        return ERR_INVALID_ARGS;
    }

    lun = ufs_switch_part(__dev->part_id);

    if (lun < UFS_ERR_NONE) {
        dprintf(CRITICAL,
            "[UFS] err: switch partition failed, part %d, ret %d\n", __dev->part_id, lun);
        return ERR_INVALID_ARGS;
    }

    end_blk = (u32)((start_addr + len) / (u64)bdev->block_size) - 1;

    if (end_blk >= hba->blk_cnt[lun]) {
        dprintf(CRITICAL, "[UFS] err: erase address out of range!\n");
        dprintf(CRITICAL,
            "[UFS] err: lun %d, blk_cnt %d, start <0x%llx>, len <0x%zx>, end_blk %d\n",
            lun, hba->blk_cnt[lun], start_addr, len, end_blk);
        return ERR_OUT_OF_RANGE;
    }

    start_blk = (u32)(start_addr / (u64)bdev->block_size);

    mutex_acquire(&hba->lock);

    ret = hba->blk_erase(hba, lun, start_blk, end_blk - start_blk + 1);

    mutex_release(&hba->lock);

    if (ret)
        dprintf(CRITICAL, "[UFS] err: erase fail <0x%llx - 0x%llx>, <%d - %d> ret %d\n",
            start_addr, start_addr + len, start_blk, end_blk, ret);

    return ret ? ERR_IO : (ssize_t)len;
}

static int ufs_ioctl(struct bdev *bdev, int request, void *argp)
{
    LTRACEF("[UFS] dev %p, request %d, argp %p\n", bdev, request, argp);

    int ret = ERR_NOT_SUPPORTED;
    struct ufs_dev_t *__dev = (struct ufs_dev_t *)bdev;
    struct wp_args *wp_config;

    switch (request) {
    case BIO_IOCTL_SET_BOOT_REGION:
        if (argp) {
            struct ufs_hba *hba = ufs_get_host(0);
            u32 lu = *(u32 *)argp;

            if (lu > 0 && lu <= MAX_LUN)
                ret = ufs_set_boot_lu(hba, lu);
        }
        break;
    case BIO_IOCTL_SET_WRITE_PROTECT:
        wp_config = (struct wp_args *)argp;
        dprintf(CRITICAL, "[UFS]%d %lu %u %u\n",
                __dev->part_id, wp_config->blknr, wp_config->blkcnt, (unsigned int)wp_config->type);
        ret = ufs_set_write_protect(0, __dev->part_id,
                                    wp_config->blknr, wp_config->blkcnt, wp_config->type);
        ret = NO_ERROR;
        break;
    case BIO_IOCTL_GET_BOOTDEV_TYPE:
        *(int *) argp = UFS_BOOT;
        ret = NO_ERROR;
        break;
    case BIO_IOCTL_GET_ACTIVE_BOOT:
        ret = ufs_get_active_boot_part((int *) argp);
        break;
    }

    return ret;
}

int ufs_get_active_boot_part(u32 *active_boot_part)
{
    struct ufs_hba *hba = ufs_get_host(UFS_DEFAULT_HOST_ID);
    u32 b_boot_lun_en;
    int ret;

    if (!active_boot_part)
        return -1;
    mutex_acquire(&hba->lock);

    ret = hba->query_attr(hba, UPIU_QUERY_OPCODE_READ_ATTR, ATTR_B_BOOT_LUN_EN, 0, 0,
        &b_boot_lun_en);

    mutex_release(&hba->lock);

    if (ret != UFS_ERR_NONE) {
        dprintf(CRITICAL, "[UFS] err: read ATTR_B_BOOT_LUN_EN error: %d\n", ret);
        return ret;
    }

    if (b_boot_lun_en == ATTR_B_BOOT_LUN_EN_BOOT_LU_A)      // Enabled boot from Boot LU A
        *active_boot_part = UFS_LU_BOOT1;
    else if (b_boot_lun_en == ATTR_B_BOOT_LUN_EN_BOOT_LU_B) // Enabled boot from Boot LU B
        *active_boot_part = UFS_LU_BOOT2;
    else {
        dprintf(CRITICAL, "[UFS] err: invalid ATTR_B_BOOT_LUN_EN %d\n", *active_boot_part);
        return -1;
    }

    return ret;
}

unsigned long long ufs_get_device_size(void)
{
    struct ufs_hba *hba = ufs_get_host(UFS_DEFAULT_HOST_ID);
    unsigned long long size;
    u32 i;

    for (i = 0, size = 0; i < UFS_LU_INTERNAL_CNT; i++)
        size += (unsigned long long)hba->blk_cnt[i] * (unsigned long long)UFS_BLOCK_SIZE;

    return size;
}

int ufs_get_unique_id(struct ufs_unique_id *id)
{
    struct ufs_hba *hba = ufs_get_host(UFS_DEFAULT_HOST_ID);

    memset(id, 0, sizeof(struct ufs_unique_id));

    if (hba->dev_info.wmanufacturerid == 0)
        return -1;

    /* UFS vendor id */
    id->vid = hba->dev_info.wmanufacturerid;

    /* UFS product ID */
    if (hba->dev_info.product_id[0] != 0)
        strlcpy((char *)id->pid, hba->dev_info.product_id,
            MAX_PRODUCT_ID_LEN + 1);

    /* UFS serial number */
    if (hba->dev_info.serial_number_len != 0)
        strlcpy((char *)id->sn, hba->dev_info.serial_number,
            hba->dev_info.serial_number_len * 2 + 1);

    return 0;
}

static void ufs_bio_ops(const void *name, const int part_id, const int nblks,
                        struct ufs_hba *host)
{
    struct ufs_dev_t *dev;

    dev = malloc(sizeof(struct ufs_dev_t));
    /* malloc fail */
    ASSERT(dev);
    /* construct the block device */
    memset(dev, 0, sizeof(struct ufs_dev_t));

    /* setup partition id*/
    dev->part_id = part_id;
    /* setup host */
    dev->host = host;
    /* bio block device register */
    bio_initialize_bdev(&dev->bdev, name,
                        UFS_BLOCK_SIZE, nblks,
                        0, NULL, BIO_FLAGS_NONE);
    /* override our block device hooks */
    if (part_id == UFS_LU_RPMB) {
        /* RPMB does not use bio */
    } else {
        dev->bdev.read_block = ufs_wrap_bread;
        dev->bdev.write_block = ufs_wrap_bwrite;
        dev->bdev.erase = ufs_erase;
        dev->bdev.ioctl = ufs_ioctl;
    }
    bio_register_device(&dev->bdev);
    partition_publish(dev->bdev.name, 0x0);
}

int ufs_init(void)
{
    struct ufs_hba *hba = ufs_get_host(UFS_DEFAULT_HOST_ID);
    int ret;
    int i;

    ufs_cfg_mode(UFS_DEFAULT_HOST_ID, UFS_MODE_DMA);

    mutex_init(&hba->lock);
    ret = ufshcd_init();

    if (ret != UFS_ERR_NONE) {
        dprintf(CRITICAL, "[UFS] err: ufshcd_init failed\n");
        return ret;
    }

    /* get LU size */
    for (i = 0; i < UFS_LU_INTERNAL_CNT; i++) {
        ret = ufs_get_lu_size(hba, i, NULL, &(hba->blk_cnt[i]));

        if (ret) {
            dprintf(CRITICAL, "[UFS] err: ufs_get_lu_size(%d) fail, ret %d\n", i, ret);
            return ret;
        }
    }

    /* fill in device description */
    ufs_bio_ops("bootdevice", UFS_LU_USER, hba->blk_cnt[UFS_LU_2], hba);

    ufs_bio_ops("preloader_a", UFS_LU_BOOT1, hba->blk_cnt[UFS_LU_0], hba);

    ufs_bio_ops("preloader_b", UFS_LU_BOOT2, hba->blk_cnt[UFS_LU_1], hba);

    init_ufs_rpmb_sharemem();

    dprintf(ALWAYS, "[UFS] info: boot device found\n");

    dprintf(ALWAYS, "[UFS] info: ufs init OK\n");

    return ret;
}

int ufs_deinit(void)
{
    int ret = 0;
    struct ufs_hba *hba = ufs_get_host(UFS_DEFAULT_HOST_ID);

    deinit_ufs_rpmb_sharemem();

    /* disable auto-hibern8 and send power off notify */
    ufshcd_writel(hba, 0, REG_AHIT);
    ufs_poweroff_notify(hba);

    ufs_reset_device_low(hba);
    return ret;
}

int ufs_otp_lock_req(char *otp_part_name)
{
    int ret = 0;
    bdev_t *dev = NULL;
    u8 *buf = NULL;

    dev = bio_open_by_label(otp_part_name);
    if (!dev) {
        dprintf(CRITICAL, "[UFS] %s: OTP partition %s is not found\n", __func__, otp_part_name);
        ret = -ENODEV;
        goto out;
    }

    /* Should be align with partition table */
    buf = (u8 *)memalign(CACHE_LINE, UFS_BLOCK_SIZE);
    if (!buf) {
        dprintf(CRITICAL, "[UFS] %s: alloc buf failed\n", __func__);
        ret = -ENOMEM;
        goto out;
    }

    ret = bio_read(dev, (unsigned long *)buf, 0, dev->block_size);
    if (ret < 0 || ret != (ssize_t)dev->block_size) {
        dprintf(CRITICAL, "[UFS] %s: read failed\n", __func__);
        ret = -ENODEV;
        goto out;
    }
    LTRACEF("[UFS] %s: otp lock blk size = %lu\n", __func__, dev->block_size);

    if (!strncmp((const char *)buf, "LOCK", 4))
        ret = 1;
    else
        ret = 0;

out:
    if (buf)
        free(buf);
    if (dev)
        bio_close(dev);
    if (ret < 0)
        dprintf(CRITICAL, "[UFS] %s: ret = %d\n", __func__, ret);

    return ret;
}
