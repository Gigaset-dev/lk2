/*
 * Copyright (c) 2020 MediaTek Inc.
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

#include <assert.h>
#include <debug.h>
#ifdef WITH_PLATFORM_MEDIATEK_COMMON_DCONFIG
#include <dconfig.h>
#endif
#include <err.h>
#include <kernel/vm.h>
#include <malloc.h>
#include <platform/aee_common.h>
#include <platform/mboot_params.h>
#include <platform/mrdump.h>
#include <platform/mrdump_expdb.h>
#include <platform/mrdump_params.h>
#include <platform/reg.h>
#include <string.h>
#include <trace.h>

#define LOCAL_TRACE 0

int mrdump_set_output_device(const char *output_dev)
{
    int retval = MRDUMP_DEV_UNKNOWN;
    int ret;

    if (strcmp(output_dev, "none") == 0)
        retval = MRDUMP_DEV_NONE;
    else if (strcmp(output_dev, "null") == 0)
        retval = MRDUMP_DEV_NULL;
    else if (strcmp(output_dev, "usb") == 0)
        retval = MRDUMP_DEV_USB;
    else if (strcmp(output_dev, "pcie") == 0)
        retval = MRDUMP_DEV_PCIE;
    else if (strcmp(output_dev, "partition") == 0)
        retval = MRDUMP_DEV_PARTITION;
    else if (strcmp(output_dev, "internal-storage") == 0)
        retval = MRDUMP_DEV_ISTORAGE;

    if (retval != MRDUMP_DEV_UNKNOWN) {
        ret = mrdump_set_env("mrdump_output", output_dev);
        if (!ret)
            LTRACEF_LEVEL(ALWAYS, "%s: defaults: %s\n", __func__, output_dev);
        else
            LTRACEF_LEVEL(ALWAYS, "Set output_dev: %s failed\n", output_dev);
    } else
        LTRACEF_LEVEL(ALWAYS, "%s: unknown output device %s\n", __func__, output_dev);

    return retval;
}

int mrdump_get_default_output_device(void)
{
    char output_device_param[64] = "";

    if (mrdump_get_env("mrdump_output", output_device_param, sizeof(output_device_param)))
        LTRACEF_LEVEL(ALWAYS, "%s: mrdump_get_env fail\n", __func__);

    if (strcmp(output_device_param, "")) {
        if (strcmp(output_device_param, "none") == 0)
            return MRDUMP_DEV_NONE;
        if (strcmp(output_device_param, "null") == 0)
            return MRDUMP_DEV_NULL;
        if (strcmp(output_device_param, "internal-storage") == 0)
            return MRDUMP_DEV_ISTORAGE;
        if (strcmp(output_device_param, "usb") == 0)
            return MRDUMP_DEV_USB;
        if (strcmp(output_device_param, "pcie") == 0)
            return MRDUMP_DEV_PCIE;
        if (strcmp(output_device_param, "partition") == 0)
            return MRDUMP_DEV_PARTITION;
        LTRACEF_LEVEL(ALWAYS, "%s: unknown output device %s\n", __func__,
                output_device_param);
        return MRDUMP_DEV_UNKNOWN;
    }
#if defined(MRDUMP_DEFAULT_NONE)
    return mrdump_set_output_device("none");
#elif defined(MRDUMP_DEFAULT_NULL)
    return mrdump_set_output_device("null");
#elif defined(MRDUMP_DEFAULT_PCIE)
    return mrdump_set_output_device("pcie");
#elif defined(MRDUMP_DEFAULT_USB)
    return mrdump_set_output_device("usb");
#elif defined(MRDUMP_DEFAULT_MRDUMP_PARTITION)
    return mrdump_set_output_device("partition");
#elif defined(MRDUMP_DEFAULT_DATA_PARTITION)
    return mrdump_set_output_device("internal-storage");
#else
#if defined(MRDUMP_PARTITION_ENABLE) && LK_DEBUGLEVEL > 0
    return mrdump_set_output_device("partition");
#else
    return mrdump_set_output_device("internal-storage");
#endif
#endif
}

int mrdump_get_reserved_info_by_index(int index, char *name, u64 *start, u64 *size)
{
    const struct reserved_t *mblock_reserved;
    struct mrdump_reserved_info *mrdump_reserved_info;
    struct kernel_info *mtk_kernel_info;
    void *mblock_va = NULL;
    int i;
    uint32_t crc32, mycrc32;
    status_t err;

    if (index > MBLOCK_RESERVED_NUM_MAX) {
        LTRACEF_LEVEL(ALWAYS, "indexi is invalid (%d > %d)\n", index, MBLOCK_RESERVED_NUM_MAX);
        return -1;
    }

    mblock_reserved = mblock_query_reserved_by_name(AEE_LK_NAME, 0);
    if (!mblock_reserved) {
        LTRACEF_LEVEL(ALWAYS, "fail to get %s reserved info\n", AEE_LK_NAME);
        ASSERT(0);
    }

    err = vmm_alloc_physical(vmm_get_kernel_aspace(),
            "vm_mblock_reserved_info", ROUNDUP(mblock_reserved->size, PAGE_SIZE),
            &mblock_va, PAGE_SIZE_SHIFT,
            (paddr_t)mblock_reserved->start, 0,
            ARCH_MMU_FLAG_CACHED);
    if (err != NO_ERROR) {
        LTRACEF_LEVEL(ALWAYS, "vm_mblock_reserved_info  vmm_alloc_physical failed\n");
        ASSERT(0);
    }

    /*
     * mblock reserved info offset = aee-lk base + aee-lk size - size of mblock
     */
    mrdump_reserved_info =
        (struct mrdump_reserved_info *)(mblock_va +
        mblock_reserved->size - MBLOCK_RESERVED_INFO_SIZE);

    /* crc32 check */
    mycrc32 = mrdump_reserved_info->crc32;
    mrdump_reserved_info->crc32 = 0xffffffff;
    crc32 = calculate_crc32((void *)mrdump_reserved_info,
                                               sizeof(struct mrdump_reserved_info));
    if (mycrc32 != crc32) {
        LTRACEF_LEVEL(ALWAYS, "crc32 check fail 0x%x != 0x%x\n", crc32, mycrc32);
        vmm_free_region(vmm_get_kernel_aspace(), (vaddr_t)mblock_va);
        return -1;
    } else
        mrdump_reserved_info->crc32 = mycrc32;

    if (index >= mrdump_reserved_info->mblock_reserved_num) {
        LTRACEF_LEVEL(ALWAYS, "index(%d) is large than mblock_reserved_num(%d)\n", index,
                                                    mrdump_reserved_info->mblock_reserved_num);
        vmm_free_region(vmm_get_kernel_aspace(), (vaddr_t)mblock_va);
        return -1;
    }

    *start = mrdump_reserved_info->reserved[index].start;
    *size = mrdump_reserved_info->reserved[index].size;
    strncpy(name, mrdump_reserved_info->reserved[index].name, MBLOCK_RESERVED_NAME_SIZE);

    vmm_free_region(vmm_get_kernel_aspace(), (vaddr_t)mblock_va);
    return 0;
}

int mrdump_get_reserved_info(char *name, u64 *start, u64 *size)
{
    const struct reserved_t *mblock_reserved;
    struct mrdump_reserved_info *mrdump_reserved_info;
    struct kernel_info *mtk_kernel_info;
    void *mblock_va = NULL;
    int i;
    uint32_t crc32, mycrc32;
    status_t err;

    mblock_reserved = mblock_query_reserved_by_name(AEE_LK_NAME, 0);
    if (!mblock_reserved) {
        LTRACEF_LEVEL(ALWAYS, "fail to get %s reserved info\n", AEE_LK_NAME);
        ASSERT(0);
    }

    err = vmm_alloc_physical(vmm_get_kernel_aspace(),
            "vm_mblock_reserved_info", ROUNDUP(mblock_reserved->size, PAGE_SIZE),
            &mblock_va, PAGE_SIZE_SHIFT,
            (paddr_t)mblock_reserved->start, 0,
            ARCH_MMU_FLAG_CACHED);
    if (err != NO_ERROR) {
        LTRACEF_LEVEL(ALWAYS, "vm_mblock_reserved_info  vmm_alloc_physical failed\n");
        ASSERT(0);
    }

    /*
     * mblock reserved info offset = aee-lk base + aee-lk size - size of mblock
     */
    mrdump_reserved_info =
        (struct mrdump_reserved_info *)(mblock_va +
        mblock_reserved->size - MBLOCK_RESERVED_INFO_SIZE);

    /* crc32 check */
    mycrc32 = mrdump_reserved_info->crc32;
    mrdump_reserved_info->crc32 = 0xffffffff;
    crc32 = calculate_crc32((void *)mrdump_reserved_info,
                                               sizeof(struct mrdump_reserved_info));
    if (mycrc32 != crc32) {
        LTRACEF_LEVEL(ALWAYS, "crc32 check fail 0x%x != 0x%x\n", crc32, mycrc32);
        vmm_free_region(vmm_get_kernel_aspace(), (vaddr_t)mblock_va);
        return -1;
    } else
        mrdump_reserved_info->crc32 = mycrc32;

    for (i = 0; i < MBLOCK_RESERVED_NUM_MAX; i++) {
        if (strncmp(name, mrdump_reserved_info->reserved[i].name, MBLOCK_RESERVED_NAME_SIZE) == 0) {
           *start = mrdump_reserved_info->reserved[i].start;
           *size = mrdump_reserved_info->reserved[i].size;
           LTRACEF_LEVEL(ALWAYS,
                "mrdump show :mblock-R[%d].start: 0x%llx, size: 0x%llx name:%s\n", i,
                mrdump_reserved_info->reserved[i].start, mrdump_reserved_info->reserved[i].size,
                mrdump_reserved_info->reserved[i].name);
           break;
        }
    }

    vmm_free_region(vmm_get_kernel_aspace(), (vaddr_t)mblock_va);
    return 0;
}

int mrdump_check_enable(void)
{
#ifndef MTK_MRDUMP_ENABLE
    return MRDUMP_ALWAYS_DISABLE;
#else
#if MTK_AEE_LEVEL == 1
    const char *params = dconfig_getenv("mrdump_enable");

    if ((params == NULL) || (strcmp(params, "yes") != 0))
        return MRDUMP_RUNTIME_DISABLE;
    return MRDUMP_RUNTIME_ENABLE;
#elif MTK_AEE_LEVEL == 2
    return MRDUMP_ALWAYS_ENABLE;
#else
#error "Unknown MTK_AEE_LEVEL value"
#endif
#endif
}

int aee_check_enable(void)
{
#if MTK_AEE_LEVEL == 1
    int val = AEE_DEFAULT_SETTING;
    const char *params = dconfig_getenv("aee_enable");

    if (dconfig_socid_match()) {
        if (params != NULL) {
            if (strcmp(params, "mini") == 0) {
                LTRACEF_LEVEL(ALWAYS, "lk aee dcfg = mini\n");
                val = AEE_ENABLE_MINI;
            } else if (strcmp(params, "full") == 0) {
#ifndef MTK_MRDUMP_ENABLE
                LTRACEF_LEVEL(ALWAYS, "lk aee dcfg = mini\n");
                val = AEE_ENABLE_MINI;
#else
                LTRACEF_LEVEL(ALWAYS, "lk aee dcfg = full\n");
                val = AEE_ENABLE_FULL;
#endif
            } else if (strcmp(params, "no") == 0) {
                LTRACEF_LEVEL(ALWAYS, "lk aee dcfg = no\n");
                val = AEE_ENABLE_NO;
            }
        }
    }
    LTRACEF_LEVEL(ALWAYS, "enable %d\n", val);
    return val;
#elif MTK_AEE_LEVEL == 2
#ifndef MTK_MRDUMP_ENABLE
    LTRACEF_LEVEL(ALWAYS, "lk aee dcfg = mini (lv.2)\n");
    return AEE_ENABLE_MINI;
#else
    LTRACEF_LEVEL(ALWAYS, "lk aee dcfg = full (lv.2)\n");
    return AEE_ENABLE_FULL;
#endif
#else
#error "Unknown MTK_AEE_LEVEL value"
#endif
}
