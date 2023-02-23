/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <sys/types.h>

#define PCM_FIRMWARE_VERSION_SIZE 128

enum SPMFW_MAGIC {
    SPM_FW_MAGIC_1 = 0x53504D32,
    SPM_FW_MAGIC_2 = 0x53504D33,
};

#define SPM_FW_UNIT     4

struct pcm_desc {
    const char *version;    /* PCM code version */
    u32 *base;              /* binary array base */
    u32 base_dma;   /* dma addr of base */
    u32 pmem_words;
    u32 total_words;
    u32 pmem_start;
    u32 dmem_start;
};


#pragma pack(push)
#pragma pack(2)
struct spm_fw_header {
    unsigned int magic;
    unsigned int size;
    char name[56];
};
#pragma pack(pop)

int aee_get_spmfw_version(char *dst, int dst_len);
