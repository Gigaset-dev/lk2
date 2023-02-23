/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
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
    uint32_t kernel_with_mkimg_hdr;
};

const char *get_bootimg_partition_name(uint32_t bootimg_type);

uint32_t bootimg_hdr_valid(uint8_t *buf);
int load_bootinfo_bootimg_hdr(struct bootimg_hdr *buf);
void parse_boot_opt(void);
int is_64bit_kernel(void);

uint32_t get_page_sz(void);
vaddr_t get_kernel_target_addr(void);
vaddr_t get_kernel_addr(void);
int32_t get_kernel_sz(void);
uint32_t get_kernel_real_sz(void);
uint32_t get_bootimg_sz(void);
vaddr_t get_tags_addr(void);
uint32_t get_header_version(void);
uint32_t get_dtb_size(void);
uint64_t get_dtb_addr(void);
void set_bootimg_loaded(vaddr_t addr);
void set_bootimg_verified(void);
void set_bootimg_verify_skipped(void);
vaddr_t get_bootimg_load_addr(void);
