/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
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

#define BOOTOPT "bootopt="
#define KERNEL_64_BIT "64"

static struct bootimg_hdr g_bootimg_hdr;
static struct boot_info g_boot_info;
static int g_is_64bit_kernel = 1;

const char *get_bootimg_partition_name(uint32_t bootimg_type)
{
    const char *result = NULL;

    switch (bootimg_type) {
    case BOOTIMG_TYPE_BOOT:
        result = "boot";
        break;
    default:
        break;
    }

    return result;
}

int load_bootinfo_bootimg_hdr(struct bootimg_hdr *buf)
{
    assert(buf);
    if (g_boot_info.hdr_loaded)
        return 0;

    if (false == bootimg_hdr_valid((uint8_t *)buf)) {
        LTRACEF("invalid boot image header\n");
        return -1;
    }
    memcpy(&g_bootimg_hdr, buf, sizeof(struct bootimg_hdr));
    g_boot_info.hdr_loaded = 1;
    return 0;
}

uint32_t bootimg_hdr_valid(uint8_t *buf)
{
    if (strncmp((char *)buf, BOOTIMG_MAGIC, BOOTIMG_MAGIC_SZ) == 0)
        return 1;
    else
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

    return (uint32_t)g_bootimg_hdr.dtb_size;
}

uint64_t get_dtb_addr(void)
{
    ASSERT(g_boot_info.hdr_loaded);

    return (uint64_t)g_bootimg_hdr.dtb_addr;
}

uint32_t get_page_sz(void)
{
    ASSERT(g_boot_info.hdr_loaded);

    return (uint32_t)g_bootimg_hdr.page_sz;
}

/* get final kernel image location (after relocation) */
vaddr_t get_kernel_target_addr(void)
{
    ASSERT(g_boot_info.hdr_loaded);

    return (vaddr_t)g_bootimg_hdr.kernel_addr;
}

vaddr_t get_bootimg_load_addr(void)
{
    ASSERT(g_boot_info.hdr_loaded);
    ASSERT(g_boot_info.img_loaded);
    return g_boot_info.bootimg_load_addr;
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

uint32_t get_bootimg_sz(void)
{
    uint32_t page_sz = 0;
    uint32_t kernel_sz = 0;

    ASSERT(g_boot_info.hdr_loaded);

    page_sz = get_page_sz();
    kernel_sz = (uint32_t)g_bootimg_hdr.kernel_sz;
    kernel_sz = (uint32_t)(((kernel_sz + page_sz - 1) / page_sz) *
                           page_sz);

    return page_sz + kernel_sz;
}

vaddr_t get_tags_addr(void)
{
    ASSERT(g_boot_info.hdr_loaded);
    return (vaddr_t)g_bootimg_hdr.tags_addr;
}

static char *get_cmdline(void)
{
    ASSERT(g_boot_info.hdr_loaded);

    /* ensure commandline is terminated */
    g_bootimg_hdr.cmdline[BOOTIMG_ARGS_SZ - 1] = 0;

    return (char *)g_bootimg_hdr.cmdline;
}

uint32_t get_header_version(void)
{
    ASSERT(g_boot_info.hdr_loaded);
    return (uint32_t)g_bootimg_hdr.header_version;
}

void set_bootimg_loaded(vaddr_t addr)
{
    ASSERT(g_boot_info.hdr_loaded);

    g_boot_info.img_loaded = 1;
    g_boot_info.bootimg_load_addr = addr;
    return;
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

void parse_boot_opt(void)
{
    char *pos = get_cmdline();

    pos = strstr(pos, BOOTOPT);
    for (int i = 0; i < 2; i++) {
        if (pos != NULL) { //guard strstr or loop strchr finds nothing
            pos = strchr(pos, ',');
            if (pos != NULL)
                pos += 1; //skip comma
        }
    }

    if (pos != NULL) { //guard strchr finds nothing
        if (strncmp(pos, KERNEL_64_BIT, strlen(KERNEL_64_BIT)) == 0)
            g_is_64bit_kernel = 1;
        else
            g_is_64bit_kernel = 0;
    }
}

int is_64bit_kernel(void)
{
    return g_is_64bit_kernel;
}
