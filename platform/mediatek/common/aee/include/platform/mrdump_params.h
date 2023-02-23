/*
 * Copyright (c) 2020 MediaTek Inc.
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

#include <mblock.h>

#define AEE_LK_NAME "aee_lk"

#define DEBUG_KINFO_COMPATIBLE "google,debug-kinfo"
#define DEBUG_KINFO_DTS_NAME   "debug-kinfo"
#define DEBUG_KINFO_DTS_PATH   "/debug-kinfo"
#define DEBUG_KINFO_RESERVED   "aee_debug_kinfo"

#define MBLOCK_RESERVED_INFO_SIZE 0x8000

enum {
    MRDUMP_RUNTIME_ENABLE           = 0,
    MRDUMP_SUCCESS_ENABLE           = 1,
    MRDUMP_ALWAYS_ENABLE            = 2,
    MRDUMP_RUNTIME_DISABLE          = 3,
    MRDUMP_ALWAYS_DISABLE,
    MRDUMP_NO_SOCID_FROM_CHIP       = 5,
    MRDUMP_SEC_IMG_AUTH_INIT_ERROR  = 7,
    MRDUMP_DCONFIG_MALLOC_ERROR     = 9,
    MRDUMP_MBOOT_LOAD_PART_ERROR    = 11,
    MRDUMP_DCONFIG_IMG_VERIFY_ERROR = 13,
    MRDUMP_DCONFIG_SOCID_CERT_ERROR = 15,
    MRDUMP_DCNFIG_SOCID_MISMATCH    = 17,
    MRDUMP_NOT_VALID_IMG            = 19,
};

#define MRDUMP_DEV_UNKNOWN      0
#define MRDUMP_DEV_NONE         1
#define MRDUMP_DEV_NULL         2
#define MRDUMP_DEV_ISTORAGE     3
#define MRDUMP_DEV_UNUSED       4
#define MRDUMP_DEV_USB          5
#define MRDUMP_DEV_PARTITION    6
#define MRDUMP_DEV_PCIE         7

#define MRDUMP_GO_DUMP "MRDUMP11"

struct mrdump_reserved_info {
    uint32_t crc32;
    uint32_t mblock_reserved_num;
    struct reserved_t reserved[MBLOCK_RESERVED_NUM_MAX];
};

void mrdump_free_reserved(void);
void mrdump_debug_kinfo_setup(void *fdt);
void mrdump_save_reserved_info(void);
int mrdump_get_reserved_info(char *name, u64 *start, u64 *size);
int mrdump_get_reserved_info_by_index(int index, char *name, u64 *start, u64 *size);
int mrdump_get_default_output_device(void);
int mrdump_set_output_device(const char *output_dev);
