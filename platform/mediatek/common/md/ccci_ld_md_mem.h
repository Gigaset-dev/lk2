/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

struct md_mem_blk {
    unsigned int offset;
    unsigned int size;
    unsigned int inf_flag;
    unsigned int attr_flag;
    unsigned long long ap_phy;
};

// ======= ATTR Flags =====================================================
#define ATTR_WHOLE              (1 << 0)
#define ATTR_DSP_RESERVED       (1 << 1)
#define ATTR_PADDING_MEMORY     (1 << 2)
#define ATTR_PADDING_AT_MIDDLE  (1 << 3)
#define ATTR_PADDING_RETRIEVE   (1 << 4)
#define ATTR_DRDI_SRC           (1 << 5)
#define ATTR_DRDI_RT_DES        (1 << 6)
#define ATTR_RAMDISK            (1 << 7)
#define ATTR_OPTIONAL           (1 << 8)

// ======= INFO Flags =====================================================
#define INF_MD_REGION_0         (1 << 0)
#define INF_MD_REGION_1         (1 << 1)
#define INF_MD_REGION_2         (1 << 2)
#define INF_MD_REGION_3         (1 << 3)
#define INF_MD_REGION_4         (1 << 4)
#define INF_MD_REGION_5         (1 << 5)
#define INF_MD_REGION_6         (1 << 6)
#define INF_MD_REGION_7         (1 << 7)

#define INF_DSP_REGION_0        (1 << 8)
#define INF_DSP_REGION_1        (1 << 9)

int ccci_mem_layout_init(int num);
void ccci_mem_layout_de_init(void);
int ccci_create_1st_mem_node(unsigned int size);
int ccci_insert_mem_node(unsigned int offset, unsigned int size,
                         unsigned int inf_flag, unsigned int attr_flag);
struct md_mem_blk *ccci_get_1st_sub_mem_blk(unsigned int inf_flags,
                                            unsigned int attr_flags);
int ccci_update_base_to_mem_tbl(unsigned long long pa_base);
void ccci_mem_blk_inf_dump(void);
void ccci_mem_blk_tmp_list_dump(void);
struct md_mem_blk *ccci_mem_layout_tbl_get(void);
unsigned int ccci_layout_md_region_to_flag(unsigned int id);
