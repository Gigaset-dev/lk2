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
#include "bootconfig.h"
#include "decompressor.h"
#include "io_task.h"
#include "meta.h"
#include <app.h>
#include <app/boot_info.h>
#include <app/boot_mode.h>
#include <app/load_vfy_boot.h>
#include <app/vboot_state.h>
#include <arch/mmu.h>
#include <arch/mp_mediatek.h>
#include <arch/ops.h>
#include <assert.h>
#include <atm.h>
#include <bootctrl.h>
#include <dconfig.h>
#include <debug.h>
#include <dtb.h>
#include <err.h>
#include <fastboot_entry.h>
#include <kernel/event.h>
#include <kernel/thread.h>
#include <kernel/vm.h>
#include <lib/dpc.h>
#include <lib/kcmdline.h>
#include <libfdt.h>
#include <lock_state.h>
#include <mblock.h>
#include <platform/boot_mode.h>
#include <platform/memory_layout.h>
#include <platform/mrdump_params.h>
#ifdef WITH_PLATFORM_MEDIATEK_COMMON_VIDEO
#include <platform/video.h>
#endif
#include <platform_mtk.h>
#include <profiling.h>
#include <sboot.h>
#include <set_fdt.h>
#include <smc.h>
#include <smc_id_table.h>
#include <stdlib.h>
#include <string.h>
#include <sysenv.h>
#include <trace.h>
#include <write_protect.h>

#define LOCAL_TRACE 2

#define EXTRACT_UBYTE(x, n)     ((uint32_t)((uint8_t *)&x)[n])

static int g_is_64bit_kernel = 1;
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

static lk_time_t get_lk_boot_time(void)
{
    return current_time() - get_lk_start_time();
}

static void boot_time_profile(void *fdt)
{
    int offset = 0, nodeoffset = 0, ret = 0;
    struct bootprof {
        const char *name;
        lk_time_t (*get_time)(void);
    } time_profile[] = {
        {"pl_t",        get_pl_boot_time},
        {"logo_t",      get_logo_time},
        {"lk_t",        get_lk_boot_time},
        {"bl2_ext_t",   get_bl2_ext_boot_time},
        {"tfa_t",       get_tfa_boot_time},
        {"sec_os_t",    get_sec_os_boot_time},
        {"gz_t",        get_gz_boot_time} };

    offset = fdt_path_offset(fdt, "/");
    ASSERT(offset >= 0);

    nodeoffset = fdt_add_subnode(fdt, offset, "bootprof");
    ASSERT(nodeoffset >= 0);

    for (unsigned int i = 0; i < countof(time_profile); i++) {
        dprintf(INFO, "name: %s, time: %d\n", time_profile[i].name, time_profile[i].get_time());
        fdt32_t time = cpu_to_fdt32(time_profile[i].get_time());

        ret = fdt_setprop(fdt, nodeoffset, time_profile[i].name, &time, sizeof(time));
        ASSERT(ret >= 0);
    }
}

static void boot_mode_cmdline(void)
{
    u8 b_mode = get_boot_mode();
    int err = 0;

    if ((b_mode == META_BOOT) || (b_mode == FACTORY_BOOT)) {
        /* Set androidboot.usbconfig */
        set_usbconfig(b_mode);
        kcmdline_append("androidboot.init_rc=/system_ext/etc/init/hw/meta_init.system.rc");
    }

    if (b_mode != RECOVERY_BOOT)
        kcmdline_append("androidboot.force_normal_boot=1");

    if (b_mode == NORMAL_BOOT) {
        if (get_atm_enable_status())
            kcmdline_append("androidboot.atm=enable");
        else {
            err = write_protect_flow(b_mode, get_boot_state());
            if (err)
                LTRACEF("write protect result = %d\n", err);
            kcmdline_append("androidboot.atm=disabled");
        }
    }
}

static void androidboot_cmdline(void)
{
    #define TMPBUF_SIZE 100
    const char *suffix = get_suffix();
    char tmpbuf[TMPBUF_SIZE];
    int n;

    if (strlen(suffix) > 0) {
        n = snprintf(tmpbuf, sizeof(tmpbuf) - 1, "androidboot.slot_suffix=%s", suffix);
        if (n < 0)
            LTRACEF("snprintf error.\n");
        else
            kcmdline_append(tmpbuf);
    }

    n = snprintf(tmpbuf, sizeof(tmpbuf) - 1, "androidboot.serialno=%s", get_serialno());
    if (n < 0)
        LTRACEF("snprintf error.\n");
    else
        kcmdline_append(tmpbuf);

    n = snprintf(tmpbuf, sizeof(tmpbuf) - 1, "androidboot.dtb_idx=0 androidboot.dtbo_idx=%d",
                get_dtbo_index());
    if (n < 0)
        LTRACEF("snprintf error.\n");
    else
        kcmdline_append(tmpbuf);
}

int boot_linux_fdt(void *kernel, unsigned int *tags,
                unsigned int machtype, void *ramdisk,
                unsigned int ramdisk_sz, void *droidboot_kernel, off_t droidboot_kernel_size)
{
    void *fdt = (void *)LK_DT_BASE;
    int i;
    const char *temp;
    status_t err = NO_ERROR;
    int offset;
    void (*entry)(unsigned int, unsigned int, unsigned int*) = kernel;
    size_t kernel_max_size = LK_KERNEL_64_SIZE;
    vaddr_t kernel_load_addr;
    u64 kernel_addr_mb;
    void *kernel_va;
    uint32_t zimage_size;
    u64 dtb_addr_mb;
    vaddr_t dtb_addr = 0;
    vaddr_t recovery_dtbo_addr = 0;
    uint32_t dtb_size;
    char *dtb_ptr;
    void *dtbo_ptr = NULL;
    uint32_t dtbo_size = 0;
    void *overlayed_dtb = NULL;
    void *dconfig_overlayed_dtb = NULL;
    const uint32_t fdt_magic = FDT_MAGIC;
    uint32_t bootconfig_sz = 0;
    int current_lock_state = DEVICE_STATE_LOCKED;

    /*
     * [FIXME] No need to backup/restore lock state in normal case.
     */
    err = sec_query_device_lock_adapter(&current_lock_state);
    if (err)
        LTRACEF("Read backup lock state failed\n");
    else
        LTRACEF("Read backup lock state: %d\n", current_lock_state);

    kernel_load_addr = get_kernel_addr();
    zimage_size = get_kernel_real_sz();
    LTRACEF("kernel_sz=0x%08x\n", zimage_size);

    switch (get_header_version()) {
    case BOOT_HEADER_VERSION_TWO:
        dtb_addr = kernel_load_addr
                + get_kernel_sz()
                + get_ramdisk_sz()
                + get_recovery_dtbo_sz();

        recovery_dtbo_addr = kernel_load_addr
                + get_kernel_sz()
                + get_ramdisk_sz();
        break;
    case BOOT_HEADER_VERSION_THREE:
    case BOOT_HEADER_VERSION_FOUR:
        dtb_addr = get_vendor_ramdisk_addr()
                + get_vendor_ramdisk_sz();
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
    if (g_is_64bit_kernel) {
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
        MBLOCK_NO_LIMIT, (paddr_t)tags, 0, "mb_dtb");

    if (dtb_addr_mb != (u64)LK_DT_BASE_PHY) {
        LTRACEF("fdt (0x%x) is not taken from mb (0x%llx)\n",
            LK_DT_BASE_PHY, dtb_addr_mb);
        ASSERT(0);
    }

    if (!vaddr_to_paddr(fdt)) {
        /* If the fdt vaddr is not yet mapped,
         * allocate desired virtual space, and MMU mapping with it
         */
        err = vmm_alloc_physical(vmm_get_kernel_aspace(),
                "vm_dtb", LK_DT_SIZE,
                &fdt, PAGE_SIZE_SHIFT,
                (paddr_t)tags,
                VMM_FLAG_VALLOC_SPECIFIC,
                ARCH_MMU_FLAG_CACHED);
    }

    if (err != NO_ERROR)
        panic("err=%d, vm_dtb (0x%p)\n", err, fdt);

setup_dt:
    /*
     * set overlayed_dtb init value as dtb base in case later dtbo
     * load or overlay might fail, then still can play with dconfig
     */
    overlayed_dtb = (void *)dtb_addr;

    /* load dtbo from v2 recovery or else external dtbo */
    if (is_ab_enable() == false && get_boot_mode() == RECOVERY_BOOT)
        err = parse_dtbo((void *)recovery_dtbo_addr, NULL, &dtbo_ptr, &dtbo_size);
    else
        err = load_dtbo("dtbo", "dtbo", NULL, &dtbo_ptr, &dtbo_size);

    dprintf(ALWAYS, "dtbo_size=%u\n", dtbo_size);
    if (err != NO_ERROR) {
        dprintf(CRITICAL, "load_dtbo fail, fdt size(%d)\n", fdt_totalsize(overlayed_dtb));
        goto setup_dconfig_dt;
    }

    err = dtb_overlay((void *)dtb_addr, dtb_size, dtbo_ptr, dtbo_size, &overlayed_dtb);
    ASSERT(err == NO_ERROR);

setup_dconfig_dt:
    dconfig_overlayed_dtb = dconfig_dtb_overlay(overlayed_dtb, fdt_totalsize(overlayed_dtb), 1);
    if (dconfig_overlayed_dtb) {
        /* free previous allocated from dtb_overlay and adopt new one */
        if (overlayed_dtb != (void *)dtb_addr)
            free(overlayed_dtb);
        overlayed_dtb = dconfig_overlayed_dtb;
    }

    err = fdt_open_into(overlayed_dtb, fdt, LK_DT_SIZE);
    if (err == NO_ERROR)
        dprintf(ALWAYS, "DT overlay ok, fdt size(%d)\n", fdt_totalsize(fdt));

    /*
     * since fdt_open_into is done, free previous overlay buffer as long
     * as it's not the static base dtb_addr (from bootimg load address)
     */
    if (overlayed_dtb != (void *)dtb_addr)
        free(overlayed_dtb);
    if (dtbo_ptr)
        free(dtbo_ptr);

io_task_start:
    /* parallelize io loading tasks through dpc routine */
    dpc_queue(io_task_start, fdt, DPC_FLAG_NORESCHED);

kernel_map:
    /* reserve kernel target buffer with mblock */
    kernel_addr_mb = mblock_alloc(kernel_max_size, PAGE_SIZE,
        MBLOCK_NO_LIMIT, (paddr_t)kernel, 0, "mb_kernel");

    if (kernel_addr_mb != (paddr_t)kernel) {
        LTRACEF("kernel_addr (0x%p) is not taken from mb (0x%llx)\n",
            kernel, kernel_addr_mb);
        ASSERT(0);
    }

    if (g_is_64bit_kernel)
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
                (paddr_t)kernel,
                VMM_FLAG_VALLOC_SPECIFIC,
                ARCH_MMU_FLAG_CACHED);
    } else {
        err = NO_ERROR;
    }

    if (err != NO_ERROR)
        panic("err=%d, vm_kernel (0x%p)\n", err, kernel_va);

kernel_relocate:
    if (g_is_64bit_kernel) {
        LTRACEF("decompress kernel image...\n");
        if(droidboot_kernel_size==0){
            if (decompress_kernel((unsigned char *)(kernel_load_addr),
                    kernel_va, (int)zimage_size, (int)kernel_max_size))
                panic("decompress kernel image fail!!!\n");
        } else {
             if (decompress_kernel(droidboot_kernel,
                    kernel_va, (int)droidboot_kernel_size, (int)kernel_max_size))
                panic("decompress kernel image fail!!!\n");
        }
    } else
        memcpy(kernel_va, (void *)kernel_load_addr, zimage_size);

    /* free the relocate buffer */
    if (get_bootimg_load_addr())
        free((void *)get_bootimg_load_addr());

    if (get_vendor_bootimg_load_addr())
        free((void *)get_vendor_bootimg_load_addr());

#if SELINUX_STATUS == 1
    kcmdline_append("androidboot.selinux=disabled");
#elif SELINUX_STATUS == 2
    kcmdline_append("androidboot.selinux=permissive");
#endif
    boot_mode_cmdline();
    androidboot_cmdline();
    bootreason_cmdline();

    /* mt_boot needs to wait io_task finish its job */
    wait_for_io_task();

    /* For those who need to set fdt prop before jumping into kernel, use SET_FDT_INIT(name, hook)
     * to register their hook function. set_fdt would invoke each hook function to do the job.
     */
    set_fdt(fdt);

    /* release temp mblocks */
    mblock_free((paddr_t)LK_MEM_BASE_PHY);
    mblock_free((paddr_t)kernel);
    mblock_free((paddr_t)tags);
    if (ramdisk_sz)
        mblock_free((paddr_t)ramdisk);

    /* free pl-boottag and pl-drambuf when mrdump is disable */
    mrdump_free_reserved();

    /* Setup fdt for all mblock alloc.
     * Must be place the function as later as possible.
     * Make sure all mblock_alloc behavior can be included.
     */
    mblock_fdt_append(fdt);

    err = kcmdline_finalized(fdt);
    ASSERT(err == NO_ERROR);

    offset = fdt_path_offset(fdt, "/chosen");

    err = fdt_setprop_cell(fdt, offset, "linux,initrd-start", (uint32_t)ramdisk);
    ASSERT(err >= 0);

    /* bootconfig parses the final fdt bootargs, so bootconfig_finalized() needs to
     * execute after kcmdline_finalized(). This is mandatory since Android 12 new launch
     */
    bootconfig_sz = bootconfig_finalized(fdt, (void *)LK_RAMDISK_BASE + ramdisk_sz);

    err = fdt_setprop_cell(fdt, offset, "linux,initrd-end",
                            (uint32_t)ramdisk + ramdisk_sz + bootconfig_sz);
    ASSERT(err >= 0);

    /* Finalize boot time profile before fdt_pack. boot_time_profile cannot be registered via
     * SET_FDT_INIT_HOOK since its execution sequence is not guaranteed to be the final one.
     */
    boot_time_profile(fdt);
    err = fdt_pack(fdt);
    ASSERT(err >= 0);

    LTRACEF("booting linux @ %p, ramdisk @ %p (%d) dtb @ %p (%d)\n",
            kernel, ramdisk, ramdisk_sz, tags, fdt_totalsize(fdt));
    LTRACEF("lk boot mode = %d\n", get_boot_mode());
    LTRACEF("lk finished --> jump to linux kernel %s\n\n",
            g_is_64bit_kernel ? "64Bit" : "32Bit");

    err = sec_set_device_lock_adapter(current_lock_state);
    if (err)
        LTRACEF("Restore lock state failed\n");
    else
        LTRACEF("Restore lock state successfully\n");

    err = sec_otp_ver_update_adapter((uint32_t)get_boot_mode());
    if (err)
        LTRACEF("[SEC] Failed to update OTP version, err = 0x%x\n", err);

#if WITH_SMP
    /*
     * [FIXME] find better place to call this, not all platform need to do
     * cpu handover.
     */
    plat_mp_bootcpu_handover(0); /* switch boot cpu back to cpu 0 */
#endif

    if (g_is_64bit_kernel)
        arch_chain_load(exitfn_smc_to_next64, MTK_SIP_KERNEL_BOOT,
                        (ulong)entry, (ulong)tags, 0);
    else
        arch_chain_load(exitfn_smc_to_next32, MTK_SIP_KERNEL_BOOT,
                        (ulong)entry, (ulong)machtype, (ulong)tags);

    return 0;
}

int boot_linux_from_storage(void)
{
    int ret = 0;
    status_t err = NO_ERROR;
    uint32_t kernel_target_addr = 0;
    uint32_t ramdisk_target_addr = 0;
    uint32_t tags_target_addr = 0;
    vaddr_t ramdisk_addr = 0;
    uint32_t ramdisk_sz = 0;
    u64 ramdisk_addr_mb = 0;
    void *ramdisk_va = (void *)LK_RAMDISK_BASE;
    uint32_t ramdisk_map_sz = 0;

    /* load and verify BOOTIMG_TYPE_RECOVERY,
     * only if it's RECOVERY_BOOT on non-AB load
     */
    bool is_ab_enabled = (strlen(get_suffix()) == 0) ? false : true;

    if ((get_boot_mode() == RECOVERY_BOOT) && (!is_ab_enabled)) {
        PROFILING_START("load recovery image");
        ret = load_vfy_boot(BOOTIMG_TYPE_RECOVERY);
        ASSERT(ret >= 0);
        PROFILING_END();
    } else {
        PROFILING_START("load boot image");
        ret = load_vfy_boot(BOOTIMG_TYPE_BOOT);
        ASSERT(ret >= 0);
        PROFILING_END();
    }

    /* append build time cmdline from boot/vendor_boot hdr */
    kcmdline_append((const char *)get_bootimg_cmdline());
    if (get_header_version() >=  BOOT_HEADER_VERSION_THREE)
        kcmdline_append((const char *)get_vendor_bootimg_cmdline());

    kernel_target_addr = get_kernel_target_addr();
    ramdisk_target_addr = get_ramdisk_target_addr();
    ramdisk_addr = get_ramdisk_addr();
    ramdisk_sz = get_ramdisk_real_sz();
    tags_target_addr = get_tags_addr();

    ASSERT(kernel_target_addr != 0);
    ASSERT(ramdisk_target_addr != 0);
    ASSERT(ramdisk_addr != 0);

    /* for system as root */
    if (!ramdisk_sz)
        goto jump;

ramdisk_map:
    if (get_header_version() >= BOOT_HEADER_VERSION_FOUR)
        ramdisk_map_sz += BOOTCONFIG_LEN;

    ramdisk_map_sz += ramdisk_sz;

    if (ramdisk_map_sz > LK_RAMDISK_SIZE) {
        LTRACEF("ramdisk_map_sz (0x%x) cannot overrun LK_RAMDISK_SIZE (0x%x)\n",
            ramdisk_map_sz, LK_RAMDISK_SIZE);
        ASSERT(0);
    }

    /* reserve ramdisk target buffer with mblock */
    ramdisk_addr_mb = mblock_alloc(ROUNDUP(ramdisk_map_sz, PAGE_SIZE),
        PAGE_SIZE, MBLOCK_NO_LIMIT, ramdisk_target_addr, 0, "mb_ramdisk");

    if (ramdisk_addr_mb != (u64)LK_RAMDISK_BASE_PHY) {
        LTRACEF("ramdisk_addr (0x%x) is not taken from mb (0x%llx)\n",
            LK_RAMDISK_BASE_PHY, ramdisk_addr_mb);
        ASSERT(0);
    }

    if (!vaddr_to_paddr(ramdisk_va)) {
        /* If the va is not mapped,
         * allocate desired virtual space, and MMU mapping with it
         */
        err = vmm_alloc_physical(vmm_get_kernel_aspace(),
                "vm_ramdisk", ROUNDUP(ramdisk_map_sz, PAGE_SIZE),
                &ramdisk_va, PAGE_SIZE_SHIFT,
                (paddr_t)ramdisk_target_addr,
                VMM_FLAG_VALLOC_SPECIFIC,
                ARCH_MMU_FLAG_CACHED);
    }

    if (err != NO_ERROR || ramdisk_va != (void *)LK_RAMDISK_BASE) {
        panic("err=%d, vm_ramdisk (0x%p) expected (0x%p)\n",
            err, ramdisk_va, (void *)LK_RAMDISK_BASE);
    }

ramdisk_relocate:
    /* relocate rootfs:
     * After boot image v3, load the generic ramdisk into memory
     * immediately following the vendor ramdisk.
     * Otherwise, only load the generic ramdisk (from boot image).
     */
    if (get_header_version() >=  BOOT_HEADER_VERSION_THREE) {
        memcpy((void *)ramdisk_va,
               (void *)get_vendor_ramdisk_addr(),
               (size_t)get_vendor_ramdisk_real_sz());
        ramdisk_va += get_vendor_ramdisk_real_sz();
    }
    memcpy((void *)ramdisk_va,
           (void *)get_ramdisk_addr(),
           (size_t)get_boot_ramdisk_real_sz());

jump:
    boot_linux_fdt((void *)(uintptr_t)kernel_target_addr,
                   (unsigned int *)(uintptr_t)tags_target_addr,
                   6779,//board_machtype(),
                   (void *)(uintptr_t)ramdisk_target_addr,
                   ramdisk_sz, NULL, 0);

    while (1)
        ;

    return 0;
}

int boot_linux_from_ram(void *droidboot_kernel, off_t droidboot_kernel_size, off_t droidboot_ramdisk_size)
{
    int ret = 0;
    status_t err = NO_ERROR;
    uint32_t kernel_target_addr = 0;
    uint32_t ramdisk_target_addr = 0;
    uint32_t tags_target_addr = 0;
    vaddr_t ramdisk_addr = 0;
    uint32_t ramdisk_sz = 0;
    u64 ramdisk_addr_mb = 0;
    void *ramdisk_va = (void *)LK_RAMDISK_BASE;
    uint32_t ramdisk_map_sz = 0;

    /* load and verify BOOTIMG_TYPE_RECOVERY,
     * only if it's RECOVERY_BOOT on non-AB load
     */
    bool is_ab_enabled = (strlen(get_suffix()) == 0) ? false : true;

    if ((get_boot_mode() == RECOVERY_BOOT) && (!is_ab_enabled)) {
        PROFILING_START("load recovery image");
        ret = load_vfy_boot(BOOTIMG_TYPE_RECOVERY);
        ASSERT(ret >= 0);
        PROFILING_END();
    } else {
        PROFILING_START("load boot image");
        ret = load_vfy_boot(BOOTIMG_TYPE_BOOT);
        ASSERT(ret >= 0);
        PROFILING_END();
    }

    /* append build time cmdline from boot/vendor_boot hdr */
    kcmdline_append((const char *)get_bootimg_cmdline());
    if (get_header_version() >=  BOOT_HEADER_VERSION_THREE)
        kcmdline_append((const char *)get_vendor_bootimg_cmdline());

    kernel_target_addr = get_kernel_target_addr();
    ramdisk_target_addr = get_ramdisk_target_addr();
    ramdisk_addr = get_ramdisk_addr();
    ramdisk_sz = get_ramdisk_real_sz();
    tags_target_addr = get_tags_addr();

    ASSERT(kernel_target_addr != 0);
    ASSERT(ramdisk_target_addr != 0);
    ASSERT(ramdisk_addr != 0);

    /* for system as root */
    if (!ramdisk_sz)
        goto jump;

ramdisk_map:
    if (get_header_version() >= BOOT_HEADER_VERSION_FOUR)
        ramdisk_map_sz += BOOTCONFIG_LEN;

    ramdisk_map_sz += ramdisk_sz;

    if (ramdisk_map_sz > LK_RAMDISK_SIZE) {
        LTRACEF("ramdisk_map_sz (0x%x) cannot overrun LK_RAMDISK_SIZE (0x%x)\n",
            ramdisk_map_sz, LK_RAMDISK_SIZE);
        ASSERT(0);
    }

    /* reserve ramdisk target buffer with mblock */
    ramdisk_addr_mb = mblock_alloc(ROUNDUP(ramdisk_map_sz, PAGE_SIZE),
        PAGE_SIZE, MBLOCK_NO_LIMIT, ramdisk_target_addr, 0, "mb_ramdisk");

    if (ramdisk_addr_mb != (u64)LK_RAMDISK_BASE_PHY) {
        LTRACEF("ramdisk_addr (0x%x) is not taken from mb (0x%llx)\n",
            LK_RAMDISK_BASE_PHY, ramdisk_addr_mb);
        ASSERT(0);
    }

    if (!vaddr_to_paddr(ramdisk_va)) {
        /* If the va is not mapped,
         * allocate desired virtual space, and MMU mapping with it
         */
        err = vmm_alloc_physical(vmm_get_kernel_aspace(),
                "vm_ramdisk", ROUNDUP(ramdisk_map_sz, PAGE_SIZE),
                &ramdisk_va, PAGE_SIZE_SHIFT,
                (paddr_t)ramdisk_target_addr,
                VMM_FLAG_VALLOC_SPECIFIC,
                ARCH_MMU_FLAG_CACHED);
    }

    if (err != NO_ERROR || ramdisk_va != (void *)LK_RAMDISK_BASE) {
        panic("err=%d, vm_ramdisk (0x%p) expected (0x%p)\n",
            err, ramdisk_va, (void *)LK_RAMDISK_BASE);
    }

ramdisk_relocate:
    /* relocate rootfs:
     * After boot image v3, load the generic ramdisk into memory
     * immediately following the vendor ramdisk.
     * Otherwise, only load the generic ramdisk (from boot image).
     */
 /*   if (get_header_version() >=  BOOT_HEADER_VERSION_THREE) {
        memcpy((void *)ramdisk_va,
               (void *)get_vendor_ramdisk_addr(),
               (size_t)get_vendor_ramdisk_real_sz());
        ramdisk_va += get_vendor_ramdisk_real_sz();
    }*/
    memcpy((void *)ramdisk_va,
           (void *)droidboot_kernel+droidboot_kernel_size,
           (size_t)droidboot_ramdisk_size);

jump:
    boot_linux_fdt((void *)(uintptr_t)kernel_target_addr,
                   (unsigned int *)(uintptr_t)tags_target_addr,
                   6779,//board_machtype(),
                   (void *)(uintptr_t)ramdisk_target_addr,
                   droidboot_ramdisk_size, droidboot_kernel, droidboot_kernel_size);

    while (1)
        ;

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

static void show_boot_mode(void)
{
#ifdef WITH_PLATFORM_MEDIATEK_COMMON_VIDEO
    #define BUF_LEN 60
    char str_buf[BUF_LEN+1];
    int n;

    switch (get_boot_mode()) {
    case NORMAL_BOOT:
        n = snprintf(str_buf, BUF_LEN, " => NORMAL MODE\n");
        break;
    case META_BOOT:
        n = snprintf(str_buf, BUF_LEN, " => META MODE\n");
        break;
    case FACTORY_BOOT:
        n = snprintf(str_buf, BUF_LEN, " => FACTORY MODE\n");
        break;
    case RECOVERY_BOOT:
        n = snprintf(str_buf, BUF_LEN, " => RECOVERY MODE\n");
        break;
    case KERNEL_POWER_OFF_CHARGING_BOOT:
        n = snprintf(str_buf, BUF_LEN, " => POWER OFF CHARGING MODE\n");
        break;
    case FASTBOOT_BOOT:
        n = snprintf(str_buf, BUF_LEN, " => FASTBOOT MODE\n");
        break;
    default:
        n = snprintf(str_buf, BUF_LEN, " => UNKNOWN BOOT\n");
        break;
    }
    if (n < 0)
        LTRACEF("snprintf error.\n");
    if (strlen(str_buf) > 0)
        video_printf(str_buf);
#endif
}

void mt_boot_init(const struct app_descriptor *app)
{
#if WITH_SMP
    /*
     * [FIXME] find better place to call this, not all platform need to do
     * cpu handover. The new bootcpu also needs to be handled properly to
     * be compatible for all platforms.
     */
    plat_mp_bootcpu_handover(7); /* cpu 7 as new boot cpu */
#endif
    PROFILING_START("mt_boot_init");
    boot_mode_check();
//prize add by lipengpeng 20220812 start 
    //show_boot_mode();
//prize add by lipengpeng 20220812 end 
    if (get_boot_mode() == FASTBOOT_BOOT)
        notify_enter_fastboot();
    else
        notify_boot_linux();
    PROFILING_END();
}

void mt_boot_entry(const struct app_descriptor *app, void *args)
{
    /* Wait for boot linux event */
    wait_for_boot_linux();
    /* Will not return */
    mtk_wdt_config(0);
    droidboot_init();
    droidboot_show_dualboot_menu();
    boot_linux_from_storage();
}

APP_START(mt_boot)
.init = mt_boot_init,
.entry = mt_boot_entry,
.flags = APP_FLAG_CUSTOM_STACK_SIZE,
.stack_size = 8192,
 APP_END
