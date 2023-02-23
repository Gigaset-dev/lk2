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
#include <stdint.h>
#include <sys/types.h>

extern ulong lk_boot_args[4];

struct boot_tag_header {
    uint32_t size;
    uint32_t tag;
};

struct boot_tag {
    struct boot_tag_header hdr;
    void *data;
};

struct pl_boottags_init {
    uint id;
    void (*hook)(struct boot_tag *tag);
    const char *name;
} __ALIGNED(sizeof(void *));

#define PL_BOOTTAGS_INIT_HOOK(_name, _id, _hook)                        \
    const struct pl_boottags_init _init_struct_##_name                  \
                __ALIGNED(sizeof(void *)) __SECTION(".pl_boottags") = { \
        .id   = _id,    \
        .hook = _hook,  \
        .name = #_name, \
    };

struct pl_boottags_to_be_updated {
    uint id;
    void *data;
    const char *name;
} __ALIGNED(sizeof(void *));

#define PL_BOOTTAGS_TO_BE_UPDATED(_name, _id, _data)                                    \
    const struct pl_boottags_to_be_updated boot_tag_##_id                               \
                __ALIGNED(sizeof(void *)) __SECTION(".pl_boottags_to_be_updated") = {   \
        .id   = _id,    \
        .data = _data,  \
        .name = #_name, \
    };

void update_pl_boottags(struct boot_tag *start_addr);
