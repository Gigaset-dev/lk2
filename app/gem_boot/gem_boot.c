/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <app.h>
#include <app/boot_info.h>
#include <app/boot_mode.h>
#include <app/load_vfy_boot.h>
#include <assert.h>
#include <arch/mmu.h>
#include <arch/ops.h>
#include <debug.h>
#include "decompressor.h"
#include <dtb.h>
#include <err.h>
#include "io_task.h"
#include <kernel/event.h>
#include <kernel/vm.h>
#include <kernel/thread.h>
#include <libfdt.h>
#include <lib/dpc.h>
#include <lib/kcmdline.h>
#include <mblock.h>
#include <platform/boot_mode.h>
#include <platform_mtk.h>
#include <platform/memory_layout.h>
#include <profiling.h>
#include <set_fdt.h>
#include <smc.h>
#include <smc_id_table.h>
#include <string.h>
#include <stdlib.h>
#include <trace.h>

#define LOCAL_TRACE 2

#define EXTRACT_UBYTE(x, n)     ((uint32_t)((uint8_t *)&x)[n])

static event_t boot_linux_event = EVENT_INITIAL_VALUE(boot_linux_event, false, 0);

#define KERNEL_64BITS 1
#define KERNEL_32BITS 0

static void exitfn_smc_to_next32(ulong smc_fid, ulong kernel_ep,
                                   ulong machtype, ulong fdt_addr)
{
#if IS_64BIT
    /* x0 ~ x3 already in place, set x4 */
    __asm__ volatile("mov x4, %0\n\t"
#else
    /* r0 ~ r3 already in place, set r4 */
    __asm__ volatile("mov r4, %0\n\t"
#endif
                     "smc #0\n\t"
                     : : "I" (KERNEL_32BITS));
}

static void exitfn_smc_to_next64(ulong smc_fid, ulong kernel_ep,
                                   ulong fdt_addr, ulong unused)
{
#if IS_64BIT
    /* x0 ~ x3 already in place, set x4 */
    __asm__ volatile("mov x4, %0\n\t"
#else
    /* r0 ~ r3 already in place, set r4 */
    __asm__ volatile("mov r4, %0\n\t"
#endif
                     "smc #0\n\t"
                     : : "I" (KERNEL_64BITS));
}

static void boot_time_cmdline(void)
{
    char tmpbuf[30];

    snprintf(tmpbuf, 30, "%s%d", "bootprof.pl_t=", get_pl_boot_time());
    kcmdline_append(tmpbuf);
    snprintf(tmpbuf, 30, "%s%d", "bootprof.logo_t=", get_logo_time());
    kcmdline_append(tmpbuf);
    snprintf(tmpbuf, 30, "%s%d", "bootprof.lk_t=", (current_time() - get_lk_start_time()));
    kcmdline_append(tmpbuf);
}

int boot_linux_fdt(void *kernel, unsigned int *tags,
                unsigned int machtype)
{
    void *fdt = (void *)LK_DT_BASE;
    int i;
    const char *temp;
    status_t err = NO_ERROR;
    void (*entry)(unsigned int, unsigned int, unsigned int*) = kernel;
    size_t kernel_max_size = LK_KERNEL_64_SIZE;
    vaddr_t kernel_load_addr;
    u64 kernel_addr_mb;
    void *kernel_va;
    uint32_t zimage_size;
    u64 dtb_addr_mb;
    vaddr_t dtb_addr = 0;
    uint32_t dtb_size;
    char *dtb_ptr;
    void *dtbo_ptr = NULL;
    uint32_t dtbo_size = 0;
    void *overlayed_dtb = NULL;
    const uint32_t fdt_magic = FDT_MAGIC;

    kernel_load_addr = get_kernel_addr();
    zimage_size = get_kernel_real_sz();
    LTRACEF("kernel_sz=0x%08x\n", zimage_size);

    switch (get_header_version()) {
    case BOOT_HEADER_VERSION_TWO:
        dtb_addr = kernel_load_addr
                + get_kernel_sz()
                + 0  //not used get_ramdisk_sz() in gem_boot but keep in mind.
                + 0; //not used get_recovery_dtbo_sz() in gem_boot but keep in mind.
        break;
    default:
        goto search_dt;
        break;
    }

    dtb_size = get_dtb_size();
    if (dtb_size == 0)
        panic("can't find v%d dtb\n", get_header_version());

    if (fdt_magic(dtb_addr) == DT_TABLE_MAGIC) {
        struct dt_table_header *dt_tbl_hdr;
        uint32_t dt_entry_count, dt_offset;

        dt_tbl_hdr = (struct dt_table_header *)dtb_addr;
        dt_entry_count = fdt32_to_cpu(dt_tbl_hdr->dt_entry_count);
        dt_offset = sizeof(struct dt_table_header)
                    + dt_entry_count * sizeof(struct dt_table_entry);
        dtb_addr += dt_offset;
        dtb_size -= dt_offset;
    }
    goto dt_map;

search_dt:
    if (is_64bit_kernel()) {
        LTRACEF_LEVEL(2, "64 bits kernel\n");

        if ((paddr_t)kernel & 0x7FFFF) {
            panic("64 bit kernel can't boot at 0x%08lx\n",
                    (paddr_t)kernel);
        }

        dtb_ptr = (char *)(kernel_load_addr + zimage_size);
        dtb_ptr -= 4;
        for (i = 0; i < (DTB_MAX_SIZE - 4); i++, dtb_ptr--) {
             /* dtb append after image.gz may not 4 byte alignment */
            if (*(dtb_ptr + 3) == EXTRACT_UBYTE(fdt_magic, 0) &&
                    *(dtb_ptr + 2) == EXTRACT_UBYTE(fdt_magic, 1) &&
                    *(dtb_ptr + 1) == EXTRACT_UBYTE(fdt_magic, 2) &&
                    *(dtb_ptr + 0) == EXTRACT_UBYTE(fdt_magic, 3)) {
                dtb_addr = (vaddr_t)dtb_ptr;
                break;
            }
        }
        if (dtb_addr == 0)
            LTRACEF("Can't find main device tree!\n");
        temp = dtb_ptr + 4;
        dtb_size = (uint32_t)(*(temp) << 24) + (uint32_t)(*(temp + 1) << 16) +
                   (uint32_t)(*(temp + 2) << 8) + (uint32_t)(*(temp + 3));

        zimage_size -= dtb_size;
    } else {
        LTRACEF_LEVEL(2, "32 bits kernel\n");
        zimage_size = *(uint32_t *)(kernel_load_addr + 0x2c) -
                      *(uint32_t *)(kernel_load_addr + 0x28);

        dtb_addr = kernel_load_addr + zimage_size;
        dtb_ptr = (char *)dtb_addr;
        /* It is not sure zImage is four byte alignment */
        if (!(*(dtb_ptr + 3) == EXTRACT_UBYTE(fdt_magic, 0) &&
                *(dtb_ptr + 2) == EXTRACT_UBYTE(fdt_magic, 1) &&
                *(dtb_ptr + 1) == EXTRACT_UBYTE(fdt_magic, 2) &&
                *(dtb_ptr + 0) == EXTRACT_UBYTE(fdt_magic, 3)))
            LTRACEF("Can't find main device tree!\n");
        temp = dtb_ptr + 4;
        dtb_size = (uint32_t)(*(temp) << 24) + (uint32_t)(*(temp + 1) << 16) +
                   (uint32_t)(*(temp + 2) << 8) + (uint32_t)(*(temp + 3));
    }
    LTRACEF("dtb_addr=0x%08lx, dtb_size=0x%08X\n", dtb_addr, dtb_size);
    LTRACEF("kernel_load_addr=0x%08lx, zimage_size=0x%08x\n", kernel_load_addr, zimage_size);

dt_map:
    /* reserve DT buffer with mblock */
    dtb_addr_mb = mblock_alloc(LK_DT_SIZE, PAGE_SIZE,
        DYNAMIC_LIMIT, (paddr_t)tags, 0, "gb_dtb_");

        /* Set BOOTIMG_OFFSET_BASE & BOOTIMG_OFFSET_TAGS to 0 for dynamic kernel loading */
        if (tags != NULL) {
            if (dtb_addr_mb != (u64)LK_DT_BASE_PHY) {
                panic("fdt (0x%x) is not taken from mb (0x%llx)\n",
                        LK_DT_BASE_PHY, dtb_addr_mb);
            }
        }

    if (!vaddr_to_paddr(fdt)) {
        /* If the fdt vaddr is not yet mapped,
         * allocate desired virtual space, and MMU mapping with it
         */
        err = vmm_alloc_physical(vmm_get_kernel_aspace(),
                "vm_dtb", LK_DT_SIZE,
                &fdt, PAGE_SIZE_SHIFT,
                (paddr_t)dtb_addr_mb,
                VMM_FLAG_VALLOC_SPECIFIC,
                ARCH_MMU_FLAG_CACHED);
    }

    if (err != NO_ERROR)
        panic("err=%d, vm_dtb (0x%p)\n", err, fdt);

setup_dt:
    err = load_dtbo("dtbo", "dtbo", NULL, &dtbo_ptr, &dtbo_size);
    LTRACEF("dtbo_size=%u\n", dtbo_size);
    if (err < 0) {
        fdt_open_into((void *)dtb_addr, fdt, DTB_MAX_SIZE);
        LTRACEF("load_dtbo fail, fdt size(%d)\n", fdt_totalsize(fdt));
        goto io_task_start;
    }

    err = dtb_overlay((void *)dtb_addr, dtb_size, dtbo_ptr,
                    dtbo_size, &overlayed_dtb);
    ASSERT(err == NO_ERROR);

    if (overlayed_dtb != NULL) {
        fdt_open_into(overlayed_dtb, fdt, DTB_MAX_SIZE);
        free(overlayed_dtb);
        LTRACEF("DT overlay ok, fdt size(%d)\n", fdt_totalsize(fdt));
    }
    free(dtbo_ptr);

io_task_start:
    /* parallelize io loading tasks through dpc routine */
    dpc_queue(io_task_start, fdt, DPC_FLAG_NORESCHED);

kernel_map:
    /* reserve kernel target buffer with mblock */
    if (kernel == NULL)
        kernel_max_size = DYNAMIC_KERNEL_MAX_SIZE;

    kernel_addr_mb = mblock_alloc(kernel_max_size, KERNEL_ALIGN,
        DYNAMIC_LIMIT, (paddr_t)kernel, 0, "gb_kernel_");

    if (kernel == NULL) {
        /* apply dynamic loading when address from bootimage equal to 0 */
        entry = (void *)kernel_addr_mb;

        /* allocate available virtual space, and MMU mapping with it */
        err = vmm_alloc_physical(vmm_get_kernel_aspace(),
                "vm_kernel", kernel_max_size,
                &kernel_va, PAGE_SIZE_SHIFT,
                (paddr_t)kernel_addr_mb, 0,
                ARCH_MMU_FLAG_CACHED);
    } else {
        if (kernel_addr_mb != (paddr_t)kernel) {
            panic("kernel_addr (0x%p) is not taken from mb (0x%llx)\n",
                kernel, kernel_addr_mb);
        }

        if (is_64bit_kernel())
            kernel_va = (void *)LK_KERNEL_64_BASE;
        else
            kernel_va = (void *)LK_KERNEL_32_BASE;

        if (!vaddr_to_paddr(kernel_va)) {
            /* If kernel_va is not mapped,
             * allocate desired virtual space, and MMU mapping with it
             */
            err = vmm_alloc_physical(vmm_get_kernel_aspace(),
                    "vm_kernel", kernel_max_size,
                    &kernel_va, PAGE_SIZE_SHIFT,
                    (paddr_t)kernel_addr_mb,
                    VMM_FLAG_VALLOC_SPECIFIC,
                    ARCH_MMU_FLAG_CACHED);
        } else {
            err = NO_ERROR;
        }
    }

    if (err != NO_ERROR)
        panic("err=%d, vm_kernel (0x%p)\n", err, kernel_va);

kernel_relocate:
    if (is_64bit_kernel()) {
        LTRACEF("decompress kernel image...\n");
        if (decompress_kernel((unsigned char *)(kernel_load_addr),
                kernel_va, (int)zimage_size, (int)kernel_max_size))
            panic("decompress kernel image fail!!!\n");
    } else
        memcpy(kernel_va, (void *)kernel_load_addr, zimage_size);

    /* free the relocate buffer */
    if (get_bootimg_load_addr())
        free((void *)get_bootimg_load_addr());

    /* release temp mblocks */
    mblock_free((paddr_t)kernel_addr_mb);
    mblock_free((paddr_t)dtb_addr_mb);

    /* gem_boot needs to wait io_task finish its job */
    wait_for_io_task();

    /* platform_sec_uninit needs to be in front of mblock_fdt_append(fdt);
     * otherwise crypto_hw_engine_disable would fail to mblock_free,
     * since mblock has been already appended into dtb.
     */
    platform_sec_uninit();

    /* For those who need to set fdt prop before jumping into kernel, use SET_FDT_INIT(name, hook)
     * to register their hook function. set_fdt would invoke each hook function to do the job.
     */
    set_fdt(fdt);

    /* Setup fdt for all mblock alloc.
     * Must be place the function as later as possible.
     * Make sure all mblock_alloc behavior can be included.
     */
    mblock_fdt_append(fdt);

    /* Set preloader, logo, and lk boot times in cmdline. */
    boot_time_cmdline();

    err = kcmdline_finalized(fdt);
    ASSERT(err == NO_ERROR);

    err = fdt_pack(fdt);
    ASSERT(err >= 0);

    platform_sec_post_init();

    LTRACEF("booting linux @ %p, dtb @ %llx (%d)\n",
            entry, dtb_addr_mb, fdt_totalsize(fdt));
    LTRACEF("lk boot mode = %d\n", get_boot_mode());
    LTRACEF("lk finished --> jump to linux kernel %s\n\n",
        is_64bit_kernel() ? "64Bit" : "32Bit");

    if (is_64bit_kernel())
        arch_chain_load(exitfn_smc_to_next64, MTK_SIP_KERNEL_BOOT,
            (ulong)entry, (ulong)dtb_addr_mb, 0);
    else
        arch_chain_load(exitfn_smc_to_next32, MTK_SIP_KERNEL_BOOT,
            (ulong)entry, (ulong)machtype, (ulong)dtb_addr_mb);

    while (1)
        ;

    return 0;
}

int boot_linux_from_storage(void)
{
    int ret = 0;
    vaddr_t kernel_target_addr = 0;
    vaddr_t tags_target_addr = 0;

    PROFILING_START("load boot image");
    ret = load_vfy_boot(BOOTIMG_TYPE_BOOT);
    ASSERT(ret >= 0);
    PROFILING_END();

    kernel_target_addr = get_kernel_target_addr();
    tags_target_addr = get_tags_addr();

jump:
    boot_linux_fdt((void *)kernel_target_addr,
                   (unsigned int *)tags_target_addr,
                   6880);//board_machtype()

    return 0;
}

void notify_boot_linux(void)
{
    LTRACEF("Notify boot linux.\n");
    event_signal(&boot_linux_event, false);
}

static void wait_for_boot_linux(void)
{
    LTRACEF("Wait for boot linux event...\n");
    event_wait(&boot_linux_event);
    event_unsignal(&boot_linux_event);
    LTRACEF("Ready to boot linux...\n");
}

void gem_boot_init(const struct app_descriptor *app)
{
    PROFILING_START("gem_boot_init");
    boot_mode_check();
    notify_boot_linux();
    PROFILING_END();
}

void gem_boot_entry(const struct app_descriptor *app, void *args)
{
    /* Wait for boot linux event */
    wait_for_boot_linux();
    /* Will not return */
    boot_linux_from_storage();
}

APP_START(gem_boot)
.init = gem_boot_init,
.entry = gem_boot_entry,
.flags = APP_FLAG_CUSTOM_STACK_SIZE,
.stack_size = 8192,
 APP_END
