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
#include <arch/mmu.h>
#include <err.h>
#include <errno.h>
#include <kernel/vm.h>
#include <lib/bio.h>
#include <platform/addressmap.h>
#include <platform/aee_common.h>
#include <platform/mboot_params.h>
#include <platform/mboot_expdb.h>
#if !IS_64BIT
#include <platform/mmu_lpae.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum EXPDB_TYPE {
    AEE_INVALID = 0,
    AEE_NAND,
    AEE_EMMC
};

#define NAND_IOCTL_GET_ERASE_SIZE   0x100
#define NAND_TYPE_PREF              "nand0"

static int blk_type;

/*
 * Why trunk buffer is needed:
 * 1. bio_write can not handle paddr beyond 4G for 32bit lk, so we transfer the
 * data to it first.
 * 2. NAND storage minimum writing size sould be aligned with it's page size.
 */
static uint8_t _trunk[TRUNK_SZ];
static uint8_t *trunk = _trunk;
static u32 trunk_used;

bool is_expdb_nand(void)
{
    bdev_t *dev_expdb;
    bool is_nand = false;

    if (blk_type == AEE_NAND)
        return true;
    else if (blk_type != AEE_INVALID) /* init done && not NAND */
        return false;

    dev_expdb = bio_open_by_label(AEE_IPANIC_LABEL);
    if (!dev_expdb) {
        LOG("kedump: Partition [%s] is not exist.\n", AEE_IPANIC_LABEL);
        return is_nand; /* default value */
    }

    if (!strncmp(dev_expdb->name, NAND_TYPE_PREF, strlen(NAND_TYPE_PREF))) {
        blk_type = AEE_NAND;
        is_nand = true;
        LOG("kedump: nand device\n");
    } else {
    /* assume that other cases were EMMC */
        blk_type = AEE_EMMC;
        is_nand = false;
    }
    bio_close(dev_expdb);

    return is_nand;
}

bool is_expdb_valid(void)
{
    bdev_t *dev_expdb;
    struct ipanic_header iheader;
    ssize_t len = 0;
    bool ret = false;

    dev_expdb = bio_open_by_label(AEE_IPANIC_LABEL);
    if (!dev_expdb) {
        LOG("kedump: Partition [%s] is not exist.\n", AEE_IPANIC_LABEL);
        return false;
    }

    len = bio_read(dev_expdb, (void *)&iheader, 0, sizeof(struct ipanic_header));
    if (len < (ssize_t)sizeof(struct ipanic_header)) {
        LOG("kedump: WARN - read size from expdb %ld.\n", len);
        goto err;
    }
    if (iheader.magic == AEE_IPANIC_MAGIC && iheader.version >= AEE_IPANIC_PHDR_VERSION)
        ret = true;
err:
    bio_close(dev_expdb);
    return ret;
}

void expdb_erase_db_part(void)
{
    bdev_t *dev_expdb;
    s32 part_sz;

    dev_expdb = bio_open_by_label(AEE_IPANIC_LABEL);
    if (!dev_expdb) {
        LOG("kedump: erase failed, partition [%s] is not exist.\n",
            AEE_IPANIC_LABEL);
        return;
    }
    part_sz = dev_expdb->total_size - EXPDB_RESERVED_OTHER;
    bio_erase(dev_expdb, 0, part_sz);
    bio_close(dev_expdb);
}

u32 expdb_get_block_size(void)
{
    bdev_t *dev_expdb;
    u32 blk_sz;

    dev_expdb = bio_open_by_label(AEE_IPANIC_LABEL);
    if (!dev_expdb) {
        LOG("kedump: Partition [%s] is not exist.\n", AEE_IPANIC_LABEL);
        return 0;
    }
    if (!strncmp(dev_expdb->name, NAND_TYPE_PREF, strlen(NAND_TYPE_PREF))) {
        /*
         * we get the min erase size as block size for nand,
         * the dev->block_size for nand is 4k
         */
        bio_ioctl(*(bdev_t **)(dev_expdb + 1), NAND_IOCTL_GET_ERASE_SIZE,
                  (void *)&blk_sz);
    } else {
        blk_sz = dev_expdb->block_size;
    }
    bio_close(dev_expdb);
    return blk_sz;
}

u32 expdb_get_part_size(void)
{
    bdev_t *dev_expdb;
    s32 part_sz;

    dev_expdb = bio_open_by_label(AEE_IPANIC_LABEL);
    if (!dev_expdb) {
        LOG("kedump: Partition [%s] is not exist.\n", AEE_IPANIC_LABEL);
        return 0;
    }

    dprintf(INFO, "kedump: expdb_total:0x%llx, blk_size:0x%zx, blk_shift:0x%zx, blk_count:0x%x\n",
            dev_expdb->total_size, dev_expdb->block_size,
            dev_expdb->block_shift, dev_expdb->block_count);

    //reserved expdb for others
    part_sz = dev_expdb->total_size - EXPDB_RESERVED_OTHER;
    bio_close(dev_expdb);
    if (part_sz < 0) {
        LOG("kedump: partition size(%llx) is lesser then reserved!(%llx)\n",
                dev_expdb->total_size, (unsigned long long)EXPDB_RESERVED_OTHER);
        return 0;
    }

    return (u32)part_sz;
}

static uint64_t woffset;
/* write the whole trunk into expdb and update the woffset if trunk contained valid data */
void expdb_flush_trunk_buf(void)
{
    bdev_t *dev_expdb;
    int len;

    if (trunk_used == 0)
        return;

    dev_expdb = bio_open_by_label(AEE_IPANIC_LABEL);
    if (!dev_expdb) {
        LOG("kedump: flush partition [%s] is not exist.\n", AEE_IPANIC_LABEL);
        return;
    }

    len = bio_write(dev_expdb, (void *)trunk, woffset - trunk_used, TRUNK_SZ);
    if (len < 0)
        LOG("kedump: flush failed %d, @0x%llx\n", len, woffset);

    woffset = woffset + TRUNK_SZ - trunk_used; /* update woffset */
    trunk_used = 0;
    memset(trunk, 0, TRUNK_SZ);
    bio_close(dev_expdb);
}

/*
 * Be careful with this API.
 * Now that the global buffer -trunk- is necessary, So we have to flush the trunk by
 * expdb_flush_trunk_buf() before calling expdb_set_offset(), or the data in trunk would
 *.be inserted to the head of the new data waiting to be written.
 */
void expdb_set_offset(uint64_t offset)
{
    woffset = offset;
}

uint64_t expdb_get_offset(void)
{
    return woffset;
}

void expdb_update_panic_header(uint64_t offset, void *data, u32 sz)
{
    bdev_t *dev_expdb;
    ssize_t len;

    dev_expdb = bio_open_by_label(AEE_IPANIC_LABEL);
    if (!dev_expdb) {
        LOG("kedump: partition [%s] is not exist.\n",
            AEE_IPANIC_LABEL);
        return;
    }
    len = bio_write(dev_expdb, data, offset, sz);
    if (len < 0)
        LOG("kedump: failed to update panic header\n");
    bio_close(dev_expdb);
}

/*
 *The minimun writing size is aligned with TRUNK_SZ.
 *
 * If the data size is less than TRUNK_SZ, then the data would be stored in globle buffer
 * named trunk.
 */
static ssize_t expdb_dev_write(uint64_t offset, uint64_t data, unsigned long sz)
{
    ssize_t size_written = 0; // for woffset control
    void *vaddr;
    vaddr_t memsrc;
    unsigned long total_rest = sz;
    unsigned long rest;
    bdev_t *dev_expdb;
    unsigned long part_sz;
    int map_ok;
    uint64_t start;
    uint64_t paddr = data;
    ssize_t len;
    unsigned long write_sz;

    if (!data)
          return 0;

    dev_expdb = bio_open_by_label(AEE_IPANIC_LABEL);
    if (!dev_expdb) {
        LOG("kedump: Partition [%s] is not exist.\n", AEE_IPANIC_LABEL);
        return -ENODEV;
    }

    if (dev_expdb->total_size < EXPDB_RESERVED_OTHER) {
        LOG("kedump: partition size(%llx) is lesser then reserved!(%llx)\n",
                dev_expdb->total_size, (unsigned long long)EXPDB_RESERVED_OTHER);
        bio_close(dev_expdb);
        return -ENOSPC;
    }

    part_sz = dev_expdb->total_size - EXPDB_RESERVED_OTHER; //reserved expdb for others
    if (offset >= part_sz || sz > part_sz - offset) {
        LOG("kedump: write oversize %lx -> %llx > %lx\n", sz, offset, part_sz);
        bio_close(dev_expdb);
        return -EFAULT;
    }

    do {
        /* minirdump will dump memory in DRAM, we must allocate it first.
         * for each loop the mapping size is AEE_MAP_SIZE.
         */
        start = ROUNDDOWN((uint64_t)paddr, (uint64_t)AEE_MAP_SIZE);
#if !IS_64BIT
        if (paddr >= SZ_4G) {
            /* we can only map 16M at one time for 32bit & pa>4G case */
            map_ok = vmm_alloc_physical(vmm_get_kernel_aspace(),
                    "ke_write", AEE_MAP_SIZE,
                    &vaddr, AEE_MAP_ALIGN,
                    0, VMM_FLAG_NO_MAP_PA, ARCH_MMU_FLAG_CACHED);
            if (map_ok != NO_ERROR) {
                LOG("vmm_alloc_physical map beyond 4G fail\n");
                bio_close(dev_expdb);
                return map_ok;
            }
            vmm_aspace_t *aspace = vaddr_to_aspace(vaddr);

            map_ok = arch_mmu_map_large(&aspace->arch_aspace, (vaddr_t)vaddr,
                                        start, AEE_MAP_SIZE / PAGE_SIZE,
                                        ARCH_MMU_FLAG_CACHED);
            if (map_ok < 0) {
                LOG("arch_mmu_map_large fail @0x%llx\n", paddr);
                bio_close(dev_expdb);
                vmm_free_region(vmm_get_kernel_aspace(), (vaddr_t)vaddr);
                return map_ok;
            }
        } else
#endif
        {
            map_ok = vmm_alloc_physical(vmm_get_kernel_aspace(),
                    "ke_write", AEE_MAP_SIZE,
                    &vaddr, AEE_MAP_ALIGN,
                    (paddr_t)start, 0, ARCH_MMU_FLAG_CACHED);
            if (map_ok != NO_ERROR) {
                LOG("kedump: map error\n");
                bio_close(dev_expdb);
                return map_ok;
            }
        }

        /* success to map pa to va, now we can write to expdb */
        memsrc = (vaddr_t)vaddr + (uint32_t)(paddr - start);
        LOG("kedump: data:0x%llx, size:0x%lx, offset:0x%llx, va:0x%lx\n",
                data, sz, offset, memsrc);

        rest = start + AEE_MAP_SIZE - paddr; // max write size for current loop

        /*
         * together with rounddown paddr by AEE_MAP_SIZE before vmm_alloc_physical,
         * the following line can guarantees that the whole memory area for each loop is
         * either under 4G or beyond 4G.
         */
        paddr += rest;

        if (rest > total_rest)
            rest = total_rest; // it means this is the last loop

        total_rest -= rest; // update total_rest for loop control

        /* handle the data of last writing */
        if (trunk_used > 0 && trunk_used < TRUNK_SZ) {
            if (rest + trunk_used < TRUNK_SZ) {
                write_sz = rest;
                memcpy((void *)(trunk + trunk_used), (void *)memsrc, write_sz);
                trunk_used += write_sz;
            } else {
                write_sz = TRUNK_SZ - trunk_used;
                memcpy((void *)(trunk + trunk_used), (void *)memsrc, write_sz);
                len = bio_write(dev_expdb, trunk,
                                offset - trunk_used, TRUNK_SZ);
                if (len < 0)
                    LOG("kedump: handle last data failed %ld\n", len);
                trunk_used = 0;
            }
            /* update the size and offset no matter write fail or success */
            memsrc += write_sz;
            size_written += write_sz; // for global expdb offset control
            offset += write_sz; // for expdb offset control in current function
            rest -= write_sz;
        }

        while (rest > 0) {
            memset(trunk, 0x0, TRUNK_SZ);
            if (rest < TRUNK_SZ) {
                write_sz = rest;
                trunk_used = write_sz;
                memcpy(trunk, (void *)memsrc, write_sz);
            } else {
                write_sz = TRUNK_SZ;
                memcpy(trunk, (void *)memsrc, write_sz);
                len = bio_write(dev_expdb, trunk, offset, write_sz);
                if (len < 0)
                    LOG("kedump: write err %ld, %lx/%lx@%llx -> 0x%llx\n",
                        len, size_written, sz, data, offset);
            }

            /* update the size and offset no matter write fail or success */
            size_written += write_sz; // for global expdb offset control
            offset += write_sz; // for expdb offset control in current function
            rest -= write_sz;
            memsrc += write_sz;
        }

        vmm_free_region(vmm_get_kernel_aspace(), (vaddr_t)vaddr);
    } while (total_rest > 0);

    bio_close(dev_expdb);
    return size_written;
}

/**
 * expdb_write - write debug info into expdb partition
 * @paddr: The physical start address of debug info memory.
 * @sz: The expected size written into expdb.
 *
 * @return: The actual size written into expdb.
 */
ssize_t expdb_write(uint64_t paddr, unsigned long sz)
{
    ssize_t write_sz;

    write_sz = expdb_dev_write(woffset, paddr, sz);
    if (write_sz > 0)
        woffset += write_sz;

    if ((u32)write_sz != sz)
        LOG("kedump[W]: write size not sync <0x%lx/0x%lx>\n", write_sz, sz);

    return write_sz;
}
