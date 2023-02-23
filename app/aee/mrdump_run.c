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

#include <debug.h>
#include <fastboot_entry.h>
#include <lib/cksum.h>
#include <lib/pl_boottags.h>
#include <lib/zlib.h>
#include <lib/zutil.h>
#include <malloc.h>
#include <mblock.h>
#include <platform/addressmap.h>
#include <platform/aee_common.h>
#include <platform/mboot_params.h>
#include <platform/mrdump_expdb.h>
#include <platform/mrdump_params.h>
#ifdef WITH_PLATFORM_MEDIATEK_COMMON_PCIE
#include <platform/mtk_pcie.h>
#endif
#include <platform/reg.h>
#ifdef WITH_PLATFORM_MEDIATEK_COMMON_VIDEO
#include <platform/video.h>
#endif
#include <platform/wdt.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <trace.h>

#include "aee.h"
#include "mrdump_private.h"

#define LOCAL_TRACE 0

#ifdef MTK_MRDUMP_ENABLE
#define MB (1024UL*1024UL)
#define SIZE_1MB  MB
#define SIZE_64MB (64 * MB)

static int output_device;
struct mrdump_control_block *mrdump_cblock;

const char *mrdump_mode2string(uint8_t mode)
{
    switch (mode) {
    case AEE_REBOOT_MODE_NORMAL:
        return "NORMAL-BOOT";

    case AEE_REBOOT_MODE_KERNEL_OOPS:
        return "KERNEL-OOPS";

    case AEE_REBOOT_MODE_KERNEL_PANIC:
        return "KERNEL-PANIC";

    case AEE_REBOOT_MODE_NESTED_EXCEPTION:
        return "NESTED-CPU-EXCEPTION";

    case AEE_REBOOT_MODE_WDT:
        return "HWT";

    case AEE_REBOOT_MODE_EXCEPTION_KDUMP:
        return "MANUALDUMP";

    case AEE_REBOOT_MODE_MRDUMP_KEY:
        return "MRDUMP_KEY";

    case AEE_REBOOT_MODE_HANG_DETECT:
        return "KERNEL-HANG-DETECT";

    default:
        return "UNKNOWN-BOOT";
    }
}

static uint64_t mrdump_mem_size(void)
{
    uint64_t total_dump_size = mblock_get_memory_size();
    char mem_size_param[8] = {0};

    if (mrdump_get_env("mrdump_mem_size", mem_size_param, sizeof(mem_size_param)))
        LTRACEF_LEVEL(ALWAYS, "%s: mrdump_get_env fail\n", __func__);

    if (strcmp(mem_size_param, "")) {
        uint64_t mem_size = ((uint64_t) strtol(mem_size_param, (char **) NULL, 0) * SIZE_1MB);

        voprintf_info("Memory dump size set to %uM\n", (unsigned int) (mem_size / SIZE_1MB));
        if (mem_size >= SIZE_64MB) {
            /* minimum 64m */
            total_dump_size = MIN(total_dump_size, mem_size);
        }
    }
    return total_dump_size;
}

static struct kzip_addlist *mrdump_memlist_fill(struct mrdump_control_block *mrdump_cblock)
{
    struct kzip_addlist *memlist = malloc(sizeof(struct kzip_addlist) * 4);

    if (memlist == NULL)
        return NULL;
    void *bufp = malloc(MRDUMP_CORE_HEADER_SIZE);

    if (bufp == NULL) {
        free(memlist);
        return NULL;
    }

    memset(bufp, 0, MRDUMP_CORE_HEADER_SIZE);

    memlist[0].address = (uint64_t)(uintptr_t) bufp;
    memlist[0].size = MRDUMP_CORE_HEADER_SIZE;
    memlist[0].type = MEM_NO_MAP;
    memlist[0].memmap = 0;
    memlist[0].pageflags = 0;
    memlist[0].struct_page_size = 0;
    memlist[1].address = (uint64_t)(uintptr_t)(mrdump_cb_addr());
    memlist[1].size = mrdump_cb_size();
    memlist[1].type = MEM_NO_MAP;
    memlist[1].memmap = 0;
    memlist[1].pageflags = 0;
    memlist[1].struct_page_size = 0;
    memlist[2].address = DRAM_BASE_PHY;
    memlist[2].size = mrdump_mem_size();
    memlist[2].type = MEM_DO_MAP;
    memlist[2].memmap = mrdump_cblock->machdesc.memmap;
    memlist[2].pageflags = mrdump_cblock->machdesc.pageflags;
    memlist[2].struct_page_size = mrdump_cblock->machdesc.struct_page_size;
    memlist[3].address = 0;
    memlist[3].size = 0;
    memlist[3].type = MEM_NO_MAP;
    memlist[3].memmap = 0;
    memlist[3].pageflags = 0;
    memlist[3].struct_page_size = 0;

    return memlist;
}

static void mrdump_memlist_free(struct kzip_addlist *memlist)
{
        free((void *)(uintptr_t)memlist[0].address);
        free(memlist);
}

void mrdump_run(void)
{
    int err;

#ifdef WITH_PLATFORM_MEDIATEK_COMMON_VIDEO
    video_set_color(0xFFFF7F00, 0xFFFF0000);
    video_clear();
    video_set_cursor(0, 0);
#endif

    voprintf_info("Kdump triggerd by '%s' (address:%x, size:%lluM)\n",
              mrdump_mode2string(mrdump_cblock->crash_record.reboot_mode),
              DRAM_BASE_PHY, mrdump_mem_size() / 0x100000UL);

    /* check machdesc crc */
    uint32_t mcrc = crc32(0xffffffff, (const unsigned char *)&mrdump_cblock->machdesc,
                  sizeof(struct mrdump_machdesc)) ^ 0xffffffff;
    if (mcrc != mrdump_cblock->machdesc_crc) {
        voprintf_error("Control block machdesc field CRC error (%08x, %08x).\n",
                   mcrc, mrdump_cblock->machdesc_crc);
        return;
    }

    struct kzip_addlist *memlist_cmm = mrdump_memlist_fill_cmm(mrdump_cblock);
    struct kzip_addlist *memlist = mrdump_memlist_fill(mrdump_cblock);

    if (memlist == NULL) {
        mrdump_memlist_free_cmm(memlist_cmm);

        LTRACEF_LEVEL(ALWAYS, "Cannot allcate memlist memory.\n");
        return;
    }

    mrdump_core_header_init(mrdump_cblock, memlist);

    switch (output_device) {
    case  MRDUMP_DEV_NONE:
        LTRACEF_LEVEL(1, "Output to None (disabled)\n");
        err = 0;
        break;
    case MRDUMP_DEV_NULL:
        break;
    case MRDUMP_DEV_ISTORAGE:
        err = mrdump_ext4_output(mrdump_cblock, memlist, memlist_cmm, mrdump_dev_emmc_ext4());
        if (err)
            voprintf_error("mrdump_ext4_output fail (%d)\n", err);
        break;
#if defined(WITH_PLATFORM_MEDIATEK_COMMON_PCIE) && defined(WITH_MRDUMP_PCIE)
    case MRDUMP_DEV_PCIE:
        err = mrdump_pcie_output(mrdump_cblock, memlist, memlist_cmm);
        if (err)
            voprintf_error("mrdump_pcie_output fail (%d)\n", err);
        break;
#endif
    case MRDUMP_DEV_USB:
        err = mrdump_usb_output(mrdump_cblock, memlist, memlist_cmm);
        if (err)
            voprintf_error("mrdump_usb_output fail (%d)\n", err);
        break;
#if defined(MRDUMP_PARTITION_ENABLE) && LK_DEBUGLEVEL > 0
    case MRDUMP_DEV_PARTITION:
        err = mrdump_partition_output(mrdump_cblock, memlist, memlist_cmm,
                                              mrdump_dev_partition("mrdump"));
        if (err)
            voprintf_error("mrdump_partition_output fail (%d)\n", err);
        break;
#endif
    default:
        voprintf_error("Unsupport device id %d\n", output_device);
        err = -1;
    }

    mrdump_memlist_free(memlist);
    mrdump_memlist_free_cmm(memlist_cmm);

    mtk_wdt_restart_timer();

#ifdef WITH_PLATFORM_MEDIATEK_COMMON_VIDEO
    video_clear_screen();
    video_set_cursor(0, 0);
#endif
}

int mrdump_detection(void)
{
    if (aee_check_enable() != AEE_ENABLE_FULL) {
        voprintf_debug("mrdump disabled %d\n", aee_check_enable());
        return -1;
    }

    output_device = mrdump_get_default_output_device();
    voprintf_info("output_device : %d\n",
                  output_device);
    if (output_device == MRDUMP_DEV_UNKNOWN)
        return -1;

    mrdump_cblock = aee_mrdump_get_params();
    if (mrdump_cblock == NULL) {
        dprintf(CRITICAL, "MT-RAMDUMP control block not found\n");
        return -1;
    }

    uint8_t reboot_mode = mrdump_cblock->crash_record.reboot_mode;

    if (!aee_ddr_reserve_enable()) {
        LTRACEF_LEVEL(ALWAYS, "DDR reserve mode disabled\n");
        return -1;
    }

    if (!aee_ddr_reserve_success()) {
        LTRACEF_LEVEL(ALWAYS, "DDR reserve mode failed\n");
        return -1;
    }

    voprintf_info("sram record with mode %d\n", reboot_mode);
    switch (reboot_mode) {
    case AEE_REBOOT_MODE_GZ_WDT:
    case AEE_REBOOT_MODE_WDT:
    case AEE_REBOOT_MODE_KERNEL_OOPS:
    case AEE_REBOOT_MODE_KERNEL_PANIC:
    case AEE_REBOOT_MODE_NESTED_EXCEPTION:
    case AEE_REBOOT_MODE_EXCEPTION_KDUMP:
    case AEE_REBOOT_MODE_MRDUMP_KEY:
    case AEE_REBOOT_MODE_GZ_KE:
    case AEE_REBOOT_MODE_HANG_DETECT:
        goto end;
    case AEE_REBOOT_MODE_NORMAL: {
        /* MRDUMP_KEY reboot*/
        if (mboot_params_reboot_by_mrdump_key()) {
            mrdump_cblock->crash_record.reboot_mode = AEE_REBOOT_MODE_MRDUMP_KEY;
            voprintf_info("set reboot_mode to MRDUMP_KEY\n");
            goto end;
        }

        return -1;
    }
    default:
        voprintf_error("Unsupport exception type\n");
        return -1;
    }

end:
    if (output_device == MRDUMP_DEV_USB) {
        notify_enter_fastboot();
        return 0;
    }
#if defined(WITH_PLATFORM_MEDIATEK_COMMON_PCIE) && defined(WITH_MRDUMP_PCIE)
    if (output_device == MRDUMP_DEV_PCIE) {
        dipc_set_lk_post(DEV_LK_BRANCH);
        LTRACEF_LEVEL(ALWAYS, "sent MHCCIF\n");
        notify_enter_fastboot();
        return 0;
    }
#endif
    return 1;
}
#else
int mrdump_detection(void)
{
    voprintf_debug("MRDUMP is not support\n");
    return -1;
}
void mrdump_run(void)
{
    voprintf_debug("MRDUMP is not support\n");
    return;
}
#endif
