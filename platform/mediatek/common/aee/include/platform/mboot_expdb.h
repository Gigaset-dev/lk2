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

#include <compiler.h>
#include <stdbool.h>
#include <sys/types.h>

/* expdb */
#define AEE_IPANIC_LABEL "expdb"

/* reserved expdb for control block and pl/lk log */
#define EXPDB_RESERVED_OTHER    (3 * 1024 * 1024)

/* the minimum writing size to expdb */
#define TRUNK_SZ                    0x1000

ssize_t expdb_write(uint64_t paddr, unsigned long sz);
void expdb_set_offset(uint64_t offset);
uint64_t expdb_get_offset(void);
u32 expdb_get_block_size(void);
u32 expdb_get_part_size(void);
bool is_expdb_nand(void);
bool is_expdb_valid(void);
void expdb_erase_db_part(void);
void expdb_flush_trunk_buf(void);
void expdb_update_panic_header(uint64_t offset, void *data, u32 sz);

/* aee lk dump */
typedef bool (*CALLBACK)(uint64_t paddr, unsigned long sz);

struct mboot_expdb_init {
    const char *name;
    ssize_t size;
    void (*hook)(CALLBACK plat_dump_write);
} __ALIGNED(sizeof(void *));

#define AEE_EXPDB_INIT_HOOK(_filename, _filesize, _hook)        \
    const struct mboot_expdb_init _init_struct_##_filename      \
        __ALIGNED(sizeof(void *)) __SECTION(".mboot_expdb") = { \
            .name = #_filename, \
            .size = _filesize,  \
            .hook = _hook,      \
        }

#define AEE_EXPDB_INIT_HOOK_SUFFIX(_filename, _suffix, _filesize, _hook)  \
    const struct mboot_expdb_init _init_struct_##_filename                \
        __ALIGNED(sizeof(void *)) __SECTION(".mboot_expdb") = {           \
            .name = #_filename"."#_suffix, \
            .size = _filesize,  \
            .hook = _hook,      \
        }
