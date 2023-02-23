/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once

#define MAX_PARTITION_COUNT 128
#define MAX_PARTITION_NAME_LEN 64
#define PT_SIG      0x50547633           //"PTv3"
#define MPT_SIG     0x4D505433           //"MPT3"
#define PT_SIG_SIZE 8

#define is_valid_mpt(buf) ((*(uint32_t *)(buf)) == MPT_SIG)
#define is_valid_pt(buf) ((*(uint32_t *)(buf)) == PT_SIG)
#define is_valid_pt_v1(buf) ((*(uint32_t *)(buf)) == PT_SIG_V1)

struct pt_resident {
    unsigned char name[MAX_PARTITION_NAME_LEN];    /* partition name */
    unsigned long long size;                       /* partition size */
    unsigned long long part_id;                    /* partition region */
    unsigned long long offset;                     /* partition start */
    unsigned long long mask_flags;                 /* partition flags */
};
