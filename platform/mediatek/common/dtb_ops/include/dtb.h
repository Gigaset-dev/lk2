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

#include <dt_table.h>
#include <stdbool.h>
#include <stdint.h>

#define DTB_MAX_SIZE  (512*1024)
#define DTBO_MAX_SIZE (1024*1024)

typedef int32_t (*parse_dtbo_func)(struct dt_table_header *,
                uint32_t, uint32_t *, uint32_t *, uint32_t *);

int32_t dtb_overlay(void *main_dtb_addr, uint32_t main_dtb_size,
                void *dtbo_addr, uint32_t dtbo_size, void **merged_dtb);
int32_t load_dtbo(const char *part_name, const char *img_name,
                parse_dtbo_func cb, void **dtbo_ptr, uint32_t *dtbo_size);
int32_t parse_dtbo(void *buf, parse_dtbo_func cb, void **dtbo_ptr, uint32_t *dtbo_size);
uint32_t get_dtbo_index(void);

