/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once
#include <list.h>
#include <sys/types.h>

struct nftl_info {
    char *name;
    char *label;
    u64 total_size;
    u32 erase_size;
    u32 write_size;
    int (*block_isbad)(struct nftl_info *info, u32 page);
    int (*block_markbad)(struct nftl_info *info, u32 page);
    ssize_t (*erase)(struct nftl_info *info, off_t offset, ssize_t len);
    ssize_t (*read)(struct nftl_info *info, void *buf, off_t offset,
                    ssize_t len);
    ssize_t (*write)(struct nftl_info *info, const void *buf, off_t offset,
                     ssize_t len);
    int (*ioctl)(struct nftl_info *info, int request, void *argp);
};
int nftl_mount_bdev(struct nftl_info *info);
struct nftl_info *nftl_search_by_address(u64 address, u64 *start);
struct nftl_info *nftl_open(const char *name);
void nftl_close(struct nftl_info *info);
int nftl_add_part(const char *main_part, const char *sub_part,
                  const char *sub_label, u64 offset, u64 len);
struct nftl_info *nftl_add_master(const char *name);
int nftl_delete_part(const char *name);
void nftl_dump_parts(void);
int nftl_block_mapping(struct nftl_info *info, u32 *page);
int nftl_block_isbad(struct nftl_info *info, u32 page);
int nftl_block_markbad(struct nftl_info *info, u32 page);
ssize_t nftl_erase(struct nftl_info *info, off_t offset, ssize_t len);
ssize_t nftl_read(struct nftl_info *info, void *buf, off_t offset, ssize_t len);
ssize_t nftl_write(struct nftl_info *info, const void *buf, off_t offset,
                   ssize_t len);
int nftl_ioctl(struct nftl_info *info, int request, void *argp);
int nftl_partition_get_offset(const char *name);
int nftl_partition_get_size(const char *name);
