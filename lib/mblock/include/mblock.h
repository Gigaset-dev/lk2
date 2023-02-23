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

#include <sys/types.h>

#define MBLOCK_NO_LIMIT 0xffffffffffffffff
#define MBLOCK_BELOW_3G 0xC0000000
#define MBLOCK_RESERVED_NAME_SIZE 128
#define MBLOCK_RESERVED_NUM_MAX  128

#define MBLOCK_NO_MAP   0
#define MBLOCK_MAP      1
#define MBLOCK_REUSABLE 2

struct reserved_t {
    u64 start;
    u64 size;
    u32 mapping;
    char name[MBLOCK_RESERVED_NAME_SIZE];
};

void mblock_show(void);
u64 mblock_alloc(u64 reserved_size, u64 align, u64 limit,
                 u64 expected_address, u32 mapping, const char *name);
u64 mblock_alloc_range(u64 reserved_size, u64 align, u64 lower_bound, u64 limit,
                 u64 expected_address, u32 mapping, const char *name);
int mblock_resize(u64 addr, u64 oldsize, u64 newsize);

u64 mblock_get_memory_size(void);
u64 mblock_get_memory_start(void);
int mblock_free(u64 addr);
int mblock_free_partial(u64 addr, u64 size);
const struct reserved_t *mblock_query_reserved_by_idx(int index);
const struct reserved_t *mblock_query_reserved_by_name(
    const char *name, struct reserved_t *index);
int mblock_query_reserved_count(const char *name);
void mblock_fdt_append(void *fdt);
u64 mblock_change_map_type(u64 addr, u64 size, u32 mapping, const char *name);
int mblock_pre_alloc_phandle(const char *device_name, const char *mblock_name);
