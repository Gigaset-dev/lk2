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

#include <stdint.h>

#define AEE_IPANIC_LABEL "expdb"
#define MRDUMP_CONFIG_EXPDB_OFFSET 3145728 //3MB

#define MRDUMP_ALLOCATE_SIZE "mrdump_allocate_size"
#define MRDUMP_MEM_SIZE "mrdump_mem_size"
#define MRDUMP_OUTPUT "mrdump_output"
#define MRDUMP_LBAOOO "mrdump_lbaooo"
#define MRDUMP_DUMP_TYPE "mrdump_dump_type"
#define MRDUMP_FB_TIMEOUT "mrdump_fb_timeout"
#define MRDUMP_CONFIG_SIG 0x3043524d

struct mrdump_expdb_config {
    uint32_t sig;
    uint32_t size;

    char output_dev[64];
    char mem_size[16];
    char allocate_size[16];
    char lbaooo[24];
    char dump_type[32];
    char fb_timeout[32];
    char unused[3904]; // add to 4096K
};

int mrdump_set_env(const char *name, const char *value);
int mrdump_get_env(const char *name, char *out, int out_len);
unsigned char aee_get_exp_type(void);
unsigned int aee_get_wdt_status(void);
