/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#define CCU_DRAM_SIZE       0x00400000  /* mblock_reserve requires max 4 MB */
#define CCU_SRAM_SIZE       0x20000
#define CCU_DRAM_ALIGN      0x00100000  /* due to l1c align = 1MB */
#define CCU_EMI_REGION      20
#define DRAM_BASE           0x80000000
#define DRAM_BOUND_LOWER    0x240000000ULL
#define DRAM_BOUND_UPPER    0x340000000ULL
#define CCU_DRAM_BASE       0x40000000

#define IMG_NAME_CCU        "ccu"
#define DRAM_NAME_CCU       "mm_ccu_reserved"
#define CCU_MAGIC_NO        0x30554343    /* "CCU0", little-endian. */
#define BASE_MASK           0xFF000000

#define DDR_ALIGN    CCU_DRAM_ALIGN
#define DDR_ALIGN_MASK (DDR_ALIGN-1)

#define CCU_NO_ERROR        0
#define CCU_ERR_MAGIC_NO    -101
#define CCU_ERR_TCM_SIZE    -102
#define CCU_ERR_SRAM_ADDR   -103
#define CCU_ERR_DRAM_MVA    -104
#define CCU_ERR_ATF_IOMMU   -105
#define CCU_ERR_REQ_INIT    -106

enum CCU_SMC_REQ {
    CCU_SMC_REQ_NONE = 0,
    CCU_SMC_REQ_INIT,
    CCU_SMC_REQ_LOAD,
    CCU_SMC_REQ_RUN,
    CCU_SMC_REQ_CLEAR_INT,
    CCU_SMC_REQ_MAX,
};

struct ccu_sect {
    uint32_t    offset;
    uint32_t    daddr;
    uint32_t    size;
};

#define SECT_MAX    20

struct ccu_img_hdr {
    uint32_t    magicno;
    uint32_t    isp_ver;
    uint32_t    sect_no;
    struct ccu_sect     sectors[SECT_MAX];
};
