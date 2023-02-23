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
#pragma once
#include <bootimg.h>
#include <stdint.h>
#include <sys/types.h>

struct boot_info {
    uint32_t type;
    uint32_t hdr_loaded;
    uint32_t img_loaded;
    uint32_t verified;
    uint32_t vfy_skipped;
    vaddr_t bootimg_load_addr;
    vaddr_t vendor_bootimg_load_addr;
    uint32_t recovery_dtbo_loaded;
    uint32_t kernel_with_mkimg_hdr;
};

const char *get_bootimg_partition_name(uint32_t bootimg_type);

int load_bootinfo_from_hdr(void *buf);

uint32_t get_page_sz(void);
uint32_t get_kernel_target_addr(void);
vaddr_t get_kernel_addr(void);
int32_t get_kernel_sz(void);
uint32_t get_kernel_real_sz(void);
uint32_t get_ramdisk_target_addr(void);
vaddr_t get_ramdisk_addr(void);
vaddr_t get_vendor_ramdisk_addr(void);
uint32_t get_boot_ramdisk_real_sz(void);
int32_t get_vendor_ramdisk_sz(void);
uint32_t get_vendor_ramdisk_real_sz(void);
int32_t get_ramdisk_sz(void);
uint32_t get_ramdisk_real_sz(void);
uint32_t get_bootimg_sz(void);
uint32_t get_tags_addr(void);
uint32_t get_header_version(void);
int32_t get_recovery_dtbo_sz(void);
uint32_t get_recovery_dtbo_real_sz(void);
uint64_t get_recovery_dtbo_offset(void);
uint32_t get_recovery_dtbo_loaded(void);
uint32_t get_dtb_size(void);
uint64_t get_dtb_addr(void);
void set_recovery_dtbo_loaded(void);
void set_bootimg_loaded(vaddr_t boot_addr, vaddr_t vendorboot_addr);
void set_bootimg_verified(void);
void set_bootimg_verify_skipped(void);
vaddr_t get_bootimg_load_addr(void);
vaddr_t get_vendor_bootimg_load_addr(void);
uint8_t *get_bootimg_cmdline(void);
uint8_t *get_vendor_bootimg_cmdline(void);
