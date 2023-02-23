/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <bits.h>
#include <debug.h>
#include <err.h>
#include <errno.h>
#include <kernel/mutex.h>
#include <lib/bio.h>
#include <lib/nftl.h>
#include <lib/partition.h>
#include <malloc.h>
#include <nandx.h>
#include <nandx_errno.h>
#include <platform/addressmap.h>
#include <platform/boot_trap.h>
#include <platform/nand.h>
#include <reg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

static mutex_t nand_mutex = MUTEX_INITIAL_VALUE(nand_mutex);

#define NAND_BBT_SCAN_MAXBLOCKS 4

/* nand ioctls, nand does not support generic bio ioctls(mapping),
 * so just use the same value
 */
enum nand_ioctl_num {
    NAND_IOCTL_GET_ERASE_SIZE = 0x100,
    NAND_IOCTL_FORCE_FORMAT_ALL,
};
struct nand_bdev {
    struct bdev dev;
    int firstblk;
    int blkcnt;
};
struct nandx_info g_nandi;
typedef int (*func_nandx_operation)(u8 *, u8 *, u64, size_t);
static inline struct nand_bdev *dev_to_ndev(struct bdev *dev)
{
    return containerof(dev, struct nand_bdev, dev);
}
static off_t convert_to_abs_addr(struct bdev *dev, off_t offset)
{
        struct nand_bdev *ndev = dev_to_ndev(dev);

        return offset + ndev->firstblk * g_nandi.block_size;
}
static ssize_t erase_operation(off_t offset,
                ssize_t len, bool force_erase)
{
    u32 block_size = g_nandi.block_size;
    size_t bytes_erase = 0;
    int ret;

    while (len) {
        if (nandx_is_bad_block(offset) && (!force_erase)) {
            dprintf(CRITICAL, "block(0x%llx) is bad, not erase\n", offset);
        } else {
            ret = nandx_erase(offset, block_size);
            if (ret < 0)
                dprintf(CRITICAL, "erase fail at blk %lld, force:%d\n",
                    offset, force_erase);
            else
                bytes_erase += block_size;
        }
        offset += block_size;
        len -= block_size;
    }
    dprintf(SPEW, "%s end, bytes_erase:%zd\n", __func__, bytes_erase);
    return bytes_erase;
}
static ssize_t nand_force_erase(void)
{
    ssize_t ret;

    mutex_acquire(&nand_mutex);
    ret = erase_operation(0,
        g_nandi.total_size - NAND_BBT_SCAN_MAXBLOCKS * g_nandi.block_size, true);
    mutex_release(&nand_mutex);
    return ret;
}
static ssize_t nand_erase(struct nftl_info *nftl, off_t offset, ssize_t len)
{
    ssize_t ret;

    mutex_acquire(&nand_mutex);
    ret = erase_operation(offset, len, false);
    mutex_release(&nand_mutex);
    return ret;
}
static ssize_t rw_operation(void *buf,
                off_t offset, ssize_t len, bool read)
{
    struct nandx_split32 split = {0};
    func_nandx_operation operation;
    int ret;
    u8 *lbuf = (u8 *)buf;
    u32 val;
    u64 i, pages;

    operation = read ? nandx_read : nandx_write;
    nandx_split(&split, offset, (u32)len, val, g_nandi.page_size);
    if (split.head_len) {
        ret = operation(lbuf, NULL, split.head, split.head_len);
        if (ret < 0) {
            dprintf(CRITICAL,
            "nand %s error (%d)!, is_read(%d), offset(0x%llx), len(0x%lx)\n",
            __func__, ret, read, offset, len);
            if (ret == -ENANDREAD || ret == -ENANDWRITE)
                ret = -EIO;

            return ret;
        }
        lbuf += split.head_len;
    }
    if (split.body_len) {
        pages = div_down(split.body_len, g_nandi.page_size);
        for (i = 0; i < pages; i++) {
            ret = operation(lbuf + i * g_nandi.page_size, NULL,
                  split.body + i * g_nandi.page_size,
                  g_nandi.page_size);
            if (ret < 0) {
                dprintf(CRITICAL,
                "nand %s error (%d)!, is_read(%d), offset(0x%llx), len(0x%lx)\n",
                __func__, ret, read, offset, len);
                if (ret == -ENANDREAD || ret == -ENANDWRITE)
                    ret = -EIO;

                return ret;
            }
        }
        lbuf += split.body_len;
    }
    if (split.tail_len) {
        ret = operation(lbuf, NULL, split.tail, split.tail_len);
        if (ret < 0) {
            dprintf(CRITICAL,
            "nand %s error (%d)!, is_read(%d), offset(0x%llx), len(0x%lx)\n",
            __func__, ret, read, offset, len);
            if (ret == -ENANDREAD || ret == -ENANDWRITE)
                ret = -EIO;

            return ret;
        }
    }
    return len;
}
static ssize_t nand_read(struct nftl_info *nftl, void *buf,
                off_t offset, ssize_t len)
{
    ssize_t ret;

    mutex_acquire(&nand_mutex);
    ret = rw_operation(buf, offset, len, true);
    mutex_release(&nand_mutex);
    return ret;
}
static ssize_t nand_write(struct nftl_info *nftl, const void *buf,
                    off_t offset, ssize_t len)
{
    ssize_t ret;

    mutex_acquire(&nand_mutex);
    ret = rw_operation((void *)buf, offset, len, false);
    mutex_release(&nand_mutex);
    return ret;
}

static int nand_ioctl(struct nftl_info *nftl, int request, void *argp)
{
    int ret = 0;

    dprintf(INFO, "%s, %d, request:%d, name:%s\n",
         __func__, __LINE__, request, nftl->name);
    switch (request) {
    case NAND_IOCTL_GET_ERASE_SIZE:
        *(unsigned int *)argp = g_nandi.block_size;
        break;
    case NAND_IOCTL_FORCE_FORMAT_ALL:
        ret = nand_force_erase();
        break;
    default:
        return -EOPNOTSUPP;
    }
    return ret;
}
static void nand_gpio_init(u8 nand_type)
{
    /* For NFI GPIO Pinmux setting */
#define NFI_GPIO_MODE26         (GPIO_BASE + 0x4A0)
#define NFI_GPIO_MODE27         (GPIO_BASE + 0x4B0)
#define NFI_GPIO_MODE28         (GPIO_BASE + 0x4C0)
#define SNFI_GPIO_MODE27        (GPIO_BASE + 0x4B0)
#define SNFI_GPIO_MODE28        (GPIO_BASE + 0x4C0)

#define GPIO_DRV_CFG0           (IOCFG_TL_BASE)
#define GPIO_DRV_CFG1           (IOCFG_TL_BASE + 0x10)
#define GPIO_PD_CFG0            (IOCFG_TL_BASE + 0x40)
#define GPIO_PU_CFG0            (IOCFG_TL_BASE + 0x50)
#define GPIO_RDSEL_CFG0         (IOCFG_TL_BASE + 0x60)
#define GPIO_TDSEL_CFG0         (IOCFG_TL_BASE + 0x80)

    if (nand_type == NAND_SLC) {
        nandx_set_bits32(NFI_GPIO_MODE26, 0x70000000, (1 << 28));
        /* NCEB1, NLD7, NLD6, NLD5, NLD4, NLD3, NLD2, NLD1 */
        nandx_set_bits32(NFI_GPIO_MODE27, 0x77777777,
                        (1 << 28) | (1 << 24) | (1 << 20) | (1 << 16) |
                        (1 << 12) | (1 << 8) | (1 << 4) | (1 << 0));
        /* NCLE, NALE, NWEB, NREB, NRNB, NCEB0 */
        nandx_set_bits32(NFI_GPIO_MODE28, 0x777777,
                        (1 << 20) | (1 << 16) | (1 << 12) | (1 << 8) |
                        (1 << 4) | (1 << 0));
        /* Pull up/down setting */
        /*        25     24     23     22     21     20     19     18     17     16
         *        NWEB   NRNB   NREB   NLD7   NLD6   NLD5   NLD4   NLD3   NLD2   NLD1
         *        15     14     13     12     11
         *        NLD0   NCLE   NCEB1  NCEB0  NALE
         */
        nandx_set_bits32(GPIO_PU_CFG0, 0x03FFF800,
                        (0x3 << 24) | (0x8 << 20) | (0x0 << 16) | (0x3 << 12) |
                        (0x0 << 8));
        nandx_set_bits32(GPIO_PD_CFG0, 0x03FFF800,
                        (0x0 << 24) | (0x7 << 20) | (0xF << 16) | (0xC << 12) |
                        (0x8 << 8));
        /* Driving setting */
        nandx_set_bits32(GPIO_DRV_CFG0, 0x3FFF8000,
                        (0x1 << 27) | (0x1 << 24) | (0x1 << 21) | (0x1 << 18) |
                        (0x1 << 15));
        nandx_set_bits32(GPIO_DRV_CFG1, 0xFFFF,
                        (0x1 << 9) | (0x1 << 6) | (0x1 << 3) | (0x1));
        /* TDSEL/RDSEL -> default */
        nandx_set_bits32(GPIO_RDSEL_CFG0, 0xFF,
                        (0x0 << 6) | (0x0 << 4) | (0x0 << 2) | (0x0));
        nandx_set_bits32(GPIO_TDSEL_CFG0, 0xFFFF,
                        (0x0 << 12) | (0x0 << 8) | (0x0 << 4) | (0x0));
    } else if (nand_type == NAND_SPI) {
        nandx_set_bits32(SNFI_GPIO_MODE27, 0x70000000, (2 << 28));
        /* SNFI_MISO, SNFI_CLK, SNFI_HOLD, SNFI_WP, SNFI_CS */
        nandx_set_bits32(SNFI_GPIO_MODE28, 0x77777,
            (2 << 16) | (2 << 12) | (2 << 8) | (2 << 4) | (2 << 0));
        /* Pull up/down setting */
        /* 25          24          23
         *  NWEB(CLK)    NRNB(WP)   NREB(HOLD)
         *  13          12          11
         *  NCEB1(MOSI)  NCEB0(CS)  NALE(MISO)
         */
        nandx_set_bits32(GPIO_PU_CFG0, 0x03803800,
            (0x1 << 24) | (0x8 << 20) | (0x1 << 12) | (0x0 << 8));
        nandx_set_bits32(GPIO_PD_CFG0, 0x03803800,
            (0x2 << 24) | (0x0 << 20) | (0x2 << 12) | (0x8 << 8));
        /* Driving setting */
        nandx_set_bits32(GPIO_DRV_CFG0, 0x38FF8000,
            (0x1 << 27) | (0x1 << 21) | (0x1 << 18) | (0x1 << 15));
        nandx_set_bits32(GPIO_DRV_CFG1, 0x3F, (0x1 << 3) | (0x1));
        /* TDSEL/RDSEL -> default */
        nandx_set_bits32(GPIO_RDSEL_CFG0, 0xF0, (0x0 << 6) | (0x0 << 4));
        nandx_set_bits32(GPIO_TDSEL_CFG0, 0xFF00, (0x0 << 12) | (0x0 << 8));
    } else {
    }
}
static u32 nand_clock_init(void)
{
#define NFI1x_CLK_UPDATE        (IO_BASE + 0x08)
#define NFI1x_CLK_SET           (IO_BASE + 0x94)
#define NFI1x_CLK_CLR           (IO_BASE + 0x98)

    writel(0x7000000, NFI1x_CLK_CLR);
    nandx_set_bits32(NFI1x_CLK_SET, 0x7000000, 0x6000000);
    writel(0x10, NFI1x_CLK_UPDATE);

    /* should return correct value */
    return 156 * 1000 * 1000;
}
static int nand_is_bad_block(struct nftl_info *nftl, u32 page)
{
    int ret;

    mutex_acquire(&nand_mutex);
    ret = bbt_is_bad(&g_nandi, (off_t)page * g_nandi.page_size);
    mutex_release(&nand_mutex);
    return ret;
}
static int nand_block_markbad(struct nftl_info *info, u32 page)
{
    int ret = 0;

    mutex_acquire(&nand_mutex);
    if (bbt_is_bad(&g_nandi, (off_t)page * g_nandi.page_size)) {
        mutex_release(&nand_mutex);
        return 0;
    } else {
       /* Mark block bad in BBT */
       ret = bbt_mark_bad(&g_nandi, (off_t)page * g_nandi.page_size);
    }
    mutex_release(&nand_mutex);
    return ret;
}

static int get_boot_device(void)
{
    u32 val;

    val = readl(TRAP_TEST_RD) & BOOT_TRAP_MASK;
    if (val == BOOT_FROM_SNAND)
        return NAND_SPI;
    else if (val == BOOT_FROM_PNAND)
        return NAND_SLC;
    else
        return -1;
}

int nand_init_device(void)
{
    struct nftl_info *nftl;
    int ret, arg;
    struct nfi_resource res = {
            NANDX_MT6880, NULL,
            (void *)NFIECC_BASE, 197,
            (void *)NFI_BASE, 196,
            26000000, NULL, 0, 32, NAND_SLC
    };

    dprintf(SPEW, "%s @ %d ...\n", __func__, __LINE__);
    nand_gpio_init(res.nand_type);
    res.clock_1x = nand_clock_init();

    ret = nandx_init(&res);
    if (ret) {
        dprintf(CRITICAL, "nandx init error (%d)!\n", ret);
        return ret;
    }

    arg = 0xf;
    nandx_ioctl(NFI_CTRL_IOCON, &arg);
    arg = 1;
    nandx_ioctl(NFI_CTRL_NFI_IRQ, &arg);
    nandx_ioctl(NFI_CTRL_DMA, &arg);
    nandx_ioctl(NFI_CTRL_ECC, &arg);
    nandx_ioctl(NFI_CTRL_BAD_MARK_SWAP, &arg);
    nandx_ioctl(CORE_CTRL_NAND_INFO, &g_nandi);
    ret = scan_bbt(&g_nandi);
    if (ret) {
        dprintf(CRITICAL, "bbt init error (%d)!\n", ret);
        return ret;
    }

    nftl = nftl_add_master("nand0");
    if (!nftl)
        return -ENOMEM;
    nftl->erase_size = g_nandi.block_size;
    nftl->write_size = g_nandi.page_size;
    nftl->total_size = g_nandi.total_size;
    nftl->block_isbad = nand_is_bad_block;
    nftl->block_markbad = nand_block_markbad;
    nftl->erase = nand_erase;
    nftl->read = nand_read;
    nftl->write = nand_write;
    nftl->ioctl = nand_ioctl;
    ret = nftl_mount_bdev(nftl);
    if (ret)
        dprintf(CRITICAL, "nftl mount bdev fail.\n");

    ret = partition_nand_publish(nftl->name,
                nftl->total_size -
                (NAND_BBT_SCAN_MAXBLOCKS + 2) * nftl->erase_size);
    if (ret < 0)
        dprintf(CRITICAL, "partition nand publish fail %d.\n", ret);

    return ret;
}
