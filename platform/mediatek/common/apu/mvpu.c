/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <lib/bio.h>
#include <arch/ops.h>
#include <assert.h>
#include <compiler.h>
#include <ctype.h>
#include <debug.h>
#include <err.h>
#include <kernel/vm.h>
#include <libfdt.h>
#include <load_image.h>
#include <malloc.h>
#include <mblock.h>
#include <partition_utils.h>
#include <stdlib.h>
#include <string.h>
#include <trace.h>

// MVPU:
/* basic */
#define MODULE_NAME "[APU_MVPU] "

/* utility */
#define LOG_CRI(format, args...)  dprintf(CRITICAL, MODULE_NAME format, ##args)
#define LOG_INF(format, args...)  dprintf(INFO, MODULE_NAME format, ##args)

#define MB(x)   (x*1024*1024)
#define LK_MBLK_SIZE_MVPU_ALGO    MB(64)

#define ADDR_ALIGN     (0x00010000)
#define ADDR_MAX       (0xC0000000)


int mvpu_prop_off(void *fdt)
{
    int off = 0;

    off = fdt_path_offset(fdt, "/mvpu");
    if (off < 0) {
        dprintf(CRITICAL, "%s: mvpu not found, so searching soc:\n", __func__);
        off = fdt_path_offset(fdt, "soc/mvpu");
        if (off < 0) {
            dprintf(CRITICAL, "%s: mvpu not found in soc\n", __func__);
            return -ERR_NOT_FOUND;
        }
    }
    return NO_ERROR;
}

static off_t get_part_size(const char *part_name)
{
    int i;
    bdev_t *dev;
    off_t ret = 0;
    int name_list_cnt = 0;
    char *name_list[NUM_PARTITIONS] = {NULL};
    const char *label;

    partition_get_name_list((char **)&name_list, &name_list_cnt);

    label = NULL;
    for (i = 0; i < name_list_cnt; i++) {
        if (!strncmp(name_list[i], part_name, strlen(part_name))) {
            label = name_list[i];
            break;
        }
    }

    if (!label) {
        dprintf(CRITICAL, "%s: unable to locate %s\n", __func__, part_name);
        return -ERR_NOT_FOUND;
    }
    dev = bio_open_by_label(label);
    if (dev) {
        ret = dev->total_size;
        LOG_INF("%s: part total_size size = 0x%llx\n", __func__, ret);
        bio_close(dev);
    } else {
        dprintf(CRITICAL, "%s: unable to get part size of %s\n", __func__, part_name);
        return -ERR_NOT_FOUND;
    }

    return ret;
}


// presupposition:
//      1. the control parameters don't parse on LK.
//      2. MVPU on LK only does memory alloc and init.
status_t apu_load_mvpu(void *fdt)
{
    status_t ret = NO_ERROR;
    bdev_t *dev = NULL;

    // 1. check mvpu is in this platform.
    if (!fdt)
        return ERR_NOT_VALID;

    if (mvpu_prop_off(fdt) < 0) {
        LOG_INF("MVPU is not supported on this platform\n");
        return NO_ERROR;
    }

    dev = bio_open_by_label("mvpu_algo_a");
    if (!dev) {
        LOG_INF("mvpu_algo partition is not created on this project\n");
        return NO_ERROR;
    }

    // 2. allocate the mb with fixed size 64 MB.
    vaddr_t mblock_pa = 0;
    off_t mblock_sz = MB(27);
    mblock_pa = mblock_alloc(mblock_sz, ADDR_ALIGN, ADDR_MAX, 0, 1, "apu_mvpu_algo");

    if (!mblock_pa) {
        LOG_CRI("mblock allocation failed\n");
        ret = ERR_NO_MEMORY;
        goto exit;
    }

    // 3. map physical address to a virtual address
    void *mblock_va_ptr = NULL;

    ret = vmm_alloc_physical(vmm_get_kernel_aspace(),
                             "vm-mvpu",
                             mblock_sz,
                             &mblock_va_ptr,
                             PAGE_SIZE_SHIFT,
                             (paddr_t)mblock_pa,
                             0,
                             ARCH_MMU_FLAG_CACHED);

    if (ret != NO_ERROR) {
        LOG_CRI("vmm_alloc_physical failed\n");
        goto err_vmm_alloc;
    }

    memset(mblock_va_ptr, 0, mblock_sz);

    // 4. load img from partition into allocated mblock
    ret = load_partition("mvpu_algo", "mvpu_algo", (void *)mblock_va_ptr, mblock_sz);
    if (ret < 0) {
        dprintf(CRITICAL, "apusys load mvpu_algo from partition failed, ret: %d\n", ret);
        goto err_vmm_alloc;
    } else {
        ret = NO_ERROR;
    }

    /* flush to DRAM */
    arch_clean_invalidate_cache_range((addr_t)mblock_va_ptr, mblock_sz);

exit:
    return ret;

err_vmm_alloc:
    if (mblock_pa)
        mblock_free(mblock_pa);

    return ret;
}

