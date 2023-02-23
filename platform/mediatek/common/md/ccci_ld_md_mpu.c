/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <compiler.h>
#include <malloc.h>
#include <string.h>

#include "ccci_ld_md_chk_hdr.h"
#include "ccci_ld_md_log.h"
#include "ccci_ld_md_mem.h"
#include "ccci_ld_md_mpu.h"

#include "ccci_lk_plat.h"

#define LOCAL_TRACE 0

static int s_mpu_padding_region_tbl[MAX_PADDING_NUM_V6_HDR];
static int s_mpu_padding_region_num, s_free_mpu_region;

static int get_free_region_id(void)
{
    int i;

    if (s_free_mpu_region < s_mpu_padding_region_num) {
        i = s_free_mpu_region;
        s_free_mpu_region++;
        return s_mpu_padding_region_tbl[i];
    }

    return -1;
}

static int md_mpu_cfg_helper(int id, unsigned long long start,
                             unsigned int size, int flag)
{
    int mpu_id = -1;

    if (flag) {
        mpu_id = get_free_region_id();
        if (mpu_id < 0) {
            ALWAYS_LOG("Padding MPU region not enough\n");
            return -1;
        }
    }

    return ccci_plat_update_md_mpu_setting(id, start, size, mpu_id);
}

static int add_retrieve_flag_with_mpu(struct md_mem_blk tbl[], unsigned int num,
        unsigned int mdi_pad_num, unsigned int mpu_num)
{
    unsigned int i, j, max_size;
    int max_id;

    if (mdi_pad_num <= mpu_num) {
        for (j = 0; j < num; j++) {
            if (tbl[j].attr_flag & ATTR_PADDING_AT_MIDDLE)
                tbl[j].attr_flag |= ATTR_PADDING_RETRIEVE;
        }
        return 0;
    }

    for (j = 0; j < mpu_num; j++) {
        max_id = -1;
        max_size = 0;
        for (i = 0; i < num; i++) {
            if (tbl[i].attr_flag & ATTR_PADDING_RETRIEVE)
                continue;

            if (tbl[i].attr_flag & ATTR_PADDING_AT_MIDDLE) {
                if (max_size < tbl[i].size) {
                    max_size = tbl[i].size;
                    max_id = i;
                }
            }
        }
        if (max_size)
            tbl[max_id].attr_flag |= ATTR_PADDING_RETRIEVE;
    }
    return 0;
}

static int apply_md_mpu_region_setting(unsigned int id,
                                       unsigned long long start,
                                       unsigned int size, int flag)
{
    /* Index mapping for bin header(check header)
     * 0: MCU_RO_HW_RO(ROM_DSP)
     * 1: MCU_RO_HW_RW
     * 2: MCU_RW_HW_RO
     * 3: MCU_RW_HW_RW
     */
    switch (id) {
    case 0:
        return md_mpu_cfg_helper(MD_MCU_RO_HW_RO, start, size, flag);

    case 1:
        return md_mpu_cfg_helper(MD_MCU_RO_HW_RW, start, size, flag);

    case 2:
        return md_mpu_cfg_helper(MD_MCU_RW_HW_RO, start, size, flag);

    case 3:
       return  md_mpu_cfg_helper(MD_MCU_RW_HW_RW, start, size, flag);

    default:
        CCCI_TRACE_LOG("MD region:%d not implement\n", id);
        return -1;
    }
}

static int apply_dsp_mpu_region_setting(unsigned int id,
                                        unsigned long long start,
                                        unsigned int size)
{
    switch (id) {
    case 0:
        return md_mpu_cfg_helper(MD_DSP_RO, start, size, 0);

    case 1:
        return md_mpu_cfg_helper(MD_DSP_RW, start, size, 0);

    default:
        CCCI_TRACE_LOG("DSP region:%d not implement\n", id);
        return -1;
    }
}

static int apply_drdi_mpu_region_setting(unsigned long long start,
                                         unsigned int size)
{
    return md_mpu_cfg_helper(MD_DRDI, start, size, 0);
}

static int md_region_parse_helper(unsigned int id, struct md_mem_blk *blk, int num,
                                  unsigned int offset, int is_1st)
{
    unsigned long long start_addr;
    unsigned int region_flag, size;
    int i = offset;

    region_flag = ccci_layout_md_region_to_flag(id);
    if (!region_flag) {
        CCCI_TRACE_LOG("Get zero flag\n");
        return -1;
    }

    /* Find start addr */
    while (1) {
        if (i >= num) {
            CCCI_TRACE_LOG("Can't find proper blk-0\n");
            return -1;
        }
        if (!(blk[i].inf_flag & region_flag)) {
            i++;
            continue;
        }
        if (blk[i].attr_flag & ATTR_PADDING_RETRIEVE) {
            i++;
            continue;
        }
        start_addr = blk[i].ap_phy;
        size = blk[i].size;
        break;
    }

    /* Find end addr */
    while (1) {
        i++;
        if (i >= num)
            /* End 1, apply MPU setting */
            return apply_md_mpu_region_setting(id, start_addr, size, !is_1st);

        if (!(blk[i].inf_flag & region_flag)) {
            /* End 2, apply MPU setting */
            if (apply_md_mpu_region_setting(id, start_addr, size, !is_1st) < 0)
                return -1;
            return 0;
        }
        if (blk[i].attr_flag & ATTR_PADDING_RETRIEVE) {
            /* End or middle padding, apply MPU setting */
            if (apply_md_mpu_region_setting(id, start_addr, size, !is_1st) < 0)
                return -1;
            i++;
            return i;
        }
        /* Padding, but can't retrieve */
        size += blk[i].size;
    }
}

static int md_region_parse_1st(unsigned int id, struct md_mem_blk *blk, int num)
{
    return md_region_parse_helper(id, blk, num, 0, 1);
}

static int md_region_parse_next(unsigned int id, struct md_mem_blk *blk,
                                int num, int offset)
{
    return md_region_parse_helper(id, blk, num, offset, 0);
}

static int apply_md_mpu_setting_by_regin_id(unsigned int id,
                                            struct md_mem_blk *blk, int num)
{
    int ret;

    ret = md_region_parse_1st(id, blk, num);
    if (ret <= 0)
        return 0;

    while (ret > 0) {
        ret = md_region_parse_next(id, blk, num, ret);
        if (ret < 0)
            break;
    }
    return 0;
}

static int apply_dsp_region_mpu(struct md_mem_blk *blk, int num)
{
    int i;
    int dsp_found = 0;

    for (i = 0; i < num; i++) {
        if (blk[i].attr_flag & ATTR_DSP_RESERVED) {
            dsp_found = 1;
            if (blk[i].attr_flag & ATTR_OPTIONAL) {
                ALWAYS_LOG("DSP is optional, skip MPU setting\n");
                return 0;
            }
        }
        if (blk[i].inf_flag & INF_DSP_REGION_0)
            break;
    }
    if (i >= num) {
        if (dsp_found) {
            CCCI_TRACE_LOG("Find DSP RO region fail\n");
            return -1;
        } else {
            CCCI_TRACE_LOG("No DSP RO region\n");
            return 0;
        }
    }
    apply_dsp_mpu_region_setting(0, blk[i].ap_phy, blk[i].size);

    for (; i < num; i++) {
        if (blk[i].inf_flag & INF_DSP_REGION_1)
            break;
    }
    if (i >= num) {
        CCCI_TRACE_LOG("Find DSP RW region fail\n");
        return -1;
    }

    return apply_dsp_mpu_region_setting(1, blk[i].ap_phy, blk[i].size);
}

static int apply_drdi_region_mpu(struct md_mem_blk *blk, int num)
{
    int i;

    for (i = 0; i < num; i++) {
        if (blk[i].attr_flag & ATTR_DRDI_RT_DES)
            break;
    }
    if (i >= num) {
        ALWAYS_LOG("Skep DRDI region cfg\n");
        return 0;
    }

    return apply_drdi_mpu_region_setting(blk[i].ap_phy, blk[i].size);
}

/* ========================================================================== */
/* Export to external functions                                               */
/* ========================================================================== */

int ccci_update_md_mpu_cfg_tbl(void *blk_tbl[], unsigned int num)
{
    int mpu_num, padding_with_mpu_num;
    unsigned int i;
    struct md_mem_blk *tbl;
    unsigned int md_region_flag = INF_MD_REGION_0 | INF_MD_REGION_1 | INF_MD_REGION_2 |
                                  INF_MD_REGION_3 | INF_MD_REGION_4 | INF_MD_REGION_5 |
                                  INF_MD_REGION_6 | INF_MD_REGION_7;

    tbl = (struct md_mem_blk *)blk_tbl;
    padding_with_mpu_num = 0;
    /* Calc padding with mpu number and mark retrieve directly */
    for (i = 0; i < num; i++) {
        if (tbl[i].attr_flag & ATTR_PADDING_AT_MIDDLE)
            padding_with_mpu_num++;
        else if (tbl[i].attr_flag & ATTR_PADDING_MEMORY) {
            if ((i < (num - 1)) && ((tbl[i].inf_flag & tbl[i + 1].inf_flag) & md_region_flag)) {
                padding_with_mpu_num++;
                tbl[i].attr_flag |= ATTR_PADDING_AT_MIDDLE;
            } else
                tbl[i].attr_flag |= ATTR_PADDING_RETRIEVE;
        }
    }

    if (padding_with_mpu_num) {
        mpu_num = ccci_plat_get_padding_mpu_region(s_mpu_padding_region_tbl,
                                          countof(s_mpu_padding_region_tbl));
        s_mpu_padding_region_num = mpu_num;
        add_retrieve_flag_with_mpu(tbl, num, padding_with_mpu_num, mpu_num);
    }

    ccci_mem_blk_inf_dump();

    if (apply_md_mpu_setting_by_regin_id(0, tbl, num) < 0)
        return -1;

    if (apply_md_mpu_setting_by_regin_id(1, tbl, num) < 0)
        return -1;

    if (apply_md_mpu_setting_by_regin_id(2, tbl, num) < 0)
        return -1;

    if (apply_md_mpu_setting_by_regin_id(3, tbl, num) < 0)
        return -1;

    if (apply_dsp_region_mpu(tbl, num) < 0)
        return -1;

    if (apply_drdi_region_mpu(tbl, num) < 0)
        return -1;

    return 0;
}
