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
#include <app/boot_info.h>
#include <app/load_vfy_boot.h>
#include <assert.h>
#include <bootimg.h>
#include <load_image.h>
#include <mkimg.h>
#include <stdbool.h>
#include <string.h>
#include <trace.h>

#define LOCAL_TRACE 2

static struct bootimg_hdr g_bootimg_hdr;
static vendor_boot_img_hdr g_vendor_bootimg_hdr;
static struct boot_info g_boot_info;

const char *get_bootimg_partition_name(uint32_t bootimg_type)
{
    const char *result = NULL;

    switch (bootimg_type) {
    case BOOTIMG_TYPE_RECOVERY:
        result = "recovery";
        break;
    case BOOTIMG_TYPE_BOOT:
        result = "boot";
        break;
    default:
        break;
    }

    return result;
}

int load_bootinfo_from_hdr(void *buf)
{
    assert(buf);
    if (g_boot_info.hdr_loaded)
        return 0;

    /* check magic number to load boot or vendor_boot header */
    if (strncmp((char *)buf, BOOTIMG_MAGIC, BOOTIMG_MAGIC_SZ) == 0) {
        memcpy(&g_bootimg_hdr, buf, sizeof(struct bootimg_hdr));
        if (g_bootimg_hdr.header_version < BOOT_HEADER_VERSION_THREE)
            g_boot_info.hdr_loaded = 1;
        LTRACEF("boot image header loaded\n");
    } else if (strncmp((char *)buf, VENDOR_BOOT_MAGIC,
               VENDOR_BOOT_MAGIC_SIZE) == 0) {
        memcpy(&g_vendor_bootimg_hdr, buf, sizeof(vendor_boot_img_hdr));
        g_boot_info.hdr_loaded = 1;
        LTRACEF("vendor_boot image header loaded\n");
    } else {
        LTRACEF("invalid boot image header\n");
        return -1;
    }

    return 0;
}

uint32_t mkimg_hdr_valid(uint8_t *buf)
{
    union mkimg_hdr *l_mkimg_hdr = (union mkimg_hdr *)buf;

    if (l_mkimg_hdr->info.magic == MKIMG_MAGIC)
        return 1;
    else
        return 0;
}

uint32_t get_dtb_size(void)
{
    ASSERT(g_boot_info.hdr_loaded);

    if (get_header_version() >=  BOOT_HEADER_VERSION_THREE)
        return (uint32_t)g_vendor_bootimg_hdr.v3.dtb_size;
    else
        return (uint32_t)g_bootimg_hdr.dtb_size;
}

uint64_t get_dtb_addr(void)
{
    ASSERT(g_boot_info.hdr_loaded);

    if (get_header_version() >=  BOOT_HEADER_VERSION_THREE)
        return (uint64_t)g_vendor_bootimg_hdr.v3.dtb_addr;
    else
        return (uint64_t)g_bootimg_hdr.dtb_addr;
}

int32_t get_recovery_dtbo_sz(void)
{
    uint32_t page_sz = 0;
    uint32_t recovery_dtbo_size = 0;
    int32_t out_recovery_dtbo_size = 0;

    ASSERT(g_boot_info.hdr_loaded);

    recovery_dtbo_size = g_bootimg_hdr.recovery_dtbo_size;
    page_sz = g_bootimg_hdr.page_sz;

    out_recovery_dtbo_size = (int32_t)(((recovery_dtbo_size + page_sz - 1) / page_sz) *
                              page_sz);
    return out_recovery_dtbo_size;
}

uint32_t get_recovery_dtbo_real_sz(void)
{
    ASSERT(g_boot_info.hdr_loaded);

    return (uint32_t)g_bootimg_hdr.recovery_dtbo_size;
}

uint64_t get_recovery_dtbo_offset(void)
{
    ASSERT(g_boot_info.hdr_loaded);

    return (uint64_t)g_bootimg_hdr.recovery_dtbo_offset;
}

uint32_t get_page_sz(void)
{
    ASSERT(g_boot_info.hdr_loaded);

    if (get_header_version() >=  BOOT_HEADER_VERSION_THREE)
        return (uint32_t)g_vendor_bootimg_hdr.v3.page_sz;
    else
        return (uint32_t)g_bootimg_hdr.page_sz;
}

/* get final kernel image location (after relocation) */
uint32_t get_kernel_target_addr(void)
{
    ASSERT(g_boot_info.hdr_loaded);

    if (get_header_version() >=  BOOT_HEADER_VERSION_THREE)
        return (uint32_t)g_vendor_bootimg_hdr.v3.kernel_addr;
    else
        return (uint32_t)g_bootimg_hdr.kernel_addr;
}

vaddr_t get_bootimg_load_addr(void)
{
    ASSERT(g_boot_info.hdr_loaded);
    ASSERT(g_boot_info.img_loaded);
    return g_boot_info.bootimg_load_addr;
}

vaddr_t get_vendor_bootimg_load_addr(void)
{
    ASSERT(g_boot_info.hdr_loaded);
    ASSERT(g_boot_info.img_loaded);
    return g_boot_info.vendor_bootimg_load_addr;
}

uint8_t *get_bootimg_cmdline(void)
{
    ASSERT(g_boot_info.hdr_loaded);
    return (strlen(g_bootimg_hdr.cmdline) > 0) ? g_bootimg_hdr.cmdline : NULL;
}

uint8_t *get_vendor_bootimg_cmdline(void)
{
    ASSERT(g_boot_info.hdr_loaded);
    return (strlen(g_vendor_bootimg_hdr.v3.cmdline) > 0) ? g_vendor_bootimg_hdr.v3.cmdline : NULL;
}

/* get kernel image address when it's loaded */
vaddr_t get_kernel_addr(void)
{
    vaddr_t kernel_addr = 0;

    ASSERT(g_boot_info.hdr_loaded);
    ASSERT(g_boot_info.img_loaded);

    kernel_addr = g_boot_info.bootimg_load_addr + get_page_sz();

    return (vaddr_t)kernel_addr;
}

int32_t get_kernel_sz(void)
{
    uint32_t page_sz = 0;
    uint32_t kernel_sz = 0;
    int32_t out_kernel_sz = 0;

    ASSERT(g_boot_info.hdr_loaded);

    kernel_sz = g_bootimg_hdr.kernel_sz;
    page_sz = get_page_sz();

    out_kernel_sz = (int32_t)(((kernel_sz + page_sz - 1) / page_sz) *
                              page_sz);
    return out_kernel_sz;
}

uint32_t get_kernel_real_sz(void)
{
    ASSERT(g_boot_info.hdr_loaded);
    return g_bootimg_hdr.kernel_sz;
}

/* get final ramdisk image location (after relocation) */
uint32_t get_ramdisk_target_addr(void)
{
    ASSERT(g_boot_info.hdr_loaded);

    if (get_header_version() >=  BOOT_HEADER_VERSION_THREE)
        return (uint32_t)g_vendor_bootimg_hdr.v3.ramdisk_addr;
    else
        return (uint32_t)g_bootimg_hdr.ramdisk_addr;
}

/* get ramdisk image address when it's loaded */
vaddr_t get_ramdisk_addr(void)
{
    int32_t kernel_sz = 0;
    vaddr_t ramdisk_addr = 0;

    ASSERT(g_boot_info.hdr_loaded);
    ASSERT(g_boot_info.img_loaded);

    kernel_sz = get_kernel_sz();
    if (kernel_sz == 0)
        return 0;

    ramdisk_addr = g_boot_info.bootimg_load_addr + get_page_sz() +
                   (uint32_t)kernel_sz;

    if (g_boot_info.kernel_with_mkimg_hdr)
        ramdisk_addr -= MKIMG_HDR_SZ;

    return (vaddr_t)ramdisk_addr;
}

vaddr_t get_vendor_ramdisk_addr(void)
{
    vaddr_t ramdisk_addr = 0;

    ASSERT(g_boot_info.hdr_loaded);
    ASSERT(g_boot_info.img_loaded);

    ramdisk_addr = g_boot_info.vendor_bootimg_load_addr + get_page_sz();

    return (vaddr_t)ramdisk_addr;
}

uint32_t get_boot_ramdisk_real_sz(void)
{
    ASSERT(g_boot_info.hdr_loaded);
    ASSERT(g_boot_info.img_loaded);

    if (get_header_version() >=  BOOT_HEADER_VERSION_THREE) {
        struct boot_img_hdr_v3 *bootimg_hdr_v3;

        bootimg_hdr_v3 = (struct boot_img_hdr_v3 *)&g_bootimg_hdr;
        return (uint32_t)bootimg_hdr_v3->ramdisk_size;
    } else
        return (uint32_t)g_bootimg_hdr.ramdisk_sz;
}

int32_t get_boot_ramdisk_sz(void)
{
    uint32_t page_sz = get_page_sz();
    uint32_t ramdisk_sz = get_boot_ramdisk_real_sz();
    int32_t out_ramdisk_sz = 0;

    ASSERT(g_boot_info.hdr_loaded);

    out_ramdisk_sz = (int32_t)(((ramdisk_sz + page_sz - 1) / page_sz) *
                               page_sz);
    return out_ramdisk_sz;
}

uint32_t get_vendor_ramdisk_real_sz(void)
{
    ASSERT(g_boot_info.hdr_loaded);
    return (uint32_t)g_vendor_bootimg_hdr.v3.vendor_ramdisk_size;
}

int32_t get_vendor_ramdisk_sz(void)
{
    uint32_t page_sz = get_page_sz();
    uint32_t ramdisk_sz = get_vendor_ramdisk_real_sz();
    int32_t out_ramdisk_sz = 0;

    ASSERT(g_boot_info.hdr_loaded);

    out_ramdisk_sz = (int32_t)(((ramdisk_sz + page_sz - 1) / page_sz) *
                               page_sz);
    return out_ramdisk_sz;
}

int32_t get_ramdisk_sz(void)
{
    uint32_t page_sz = 0;
    uint32_t ramdisk_sz = 0;
    int32_t out_ramdisk_sz = 0;

    ASSERT(g_boot_info.hdr_loaded);

    ramdisk_sz = get_ramdisk_real_sz();
    page_sz = get_page_sz();

    out_ramdisk_sz = (int32_t)(((ramdisk_sz + page_sz - 1) / page_sz) *
                               page_sz);

    return out_ramdisk_sz;
}

uint32_t get_ramdisk_real_sz(void)
{
    uint32_t real_sz;
    ASSERT(g_boot_info.hdr_loaded);

    real_sz = get_boot_ramdisk_real_sz();

    /* V3 later: boot's ramdisk size + vendor_boots ramdisk size*/
    if (get_header_version() >=  BOOT_HEADER_VERSION_THREE)
        real_sz += get_vendor_ramdisk_real_sz();

    return real_sz;
}

uint32_t get_tags_addr(void)
{
    ASSERT(g_boot_info.hdr_loaded);

    if (get_header_version() >=  BOOT_HEADER_VERSION_THREE)
        return (uint32_t)g_vendor_bootimg_hdr.v3.tags_addr;
    else
        return (uint32_t)g_bootimg_hdr.tags_addr;
}

uint32_t get_header_version(void)
{
    ASSERT(g_boot_info.hdr_loaded);
    /* here assume header_version of boot/vendor_boot are the same */
    return (uint32_t)g_bootimg_hdr.header_version;
}

void set_bootimg_loaded(vaddr_t boot_addr, vaddr_t vendorboot_addr)
{
    ASSERT(g_boot_info.hdr_loaded);

    g_boot_info.img_loaded = 1;
    g_boot_info.bootimg_load_addr = boot_addr;

    if (get_header_version() >= BOOT_HEADER_VERSION_THREE) {
        ASSERT(vendorboot_addr);
        g_boot_info.vendor_bootimg_load_addr = vendorboot_addr;
    }

    return;
}

void set_recovery_dtbo_loaded(void)
{
    g_boot_info.recovery_dtbo_loaded = 1;
    return;
}

uint32_t get_recovery_dtbo_loaded(void)
{
    return (uint32_t)g_boot_info.recovery_dtbo_loaded;
}

void set_bootimg_verified(void)
{
    g_boot_info.verified = 1;
    return;
}

void set_bootimg_verify_skipped(void)
{
    g_boot_info.vfy_skipped = 1;
    return;
}
