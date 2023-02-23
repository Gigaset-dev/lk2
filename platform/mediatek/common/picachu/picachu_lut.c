/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
/*****************************************************************************
 *
 *****************************************************************************/
#if LK_AS_BL2_EXT
#include <platform/picachu_reg.h>
#else
#include "picachu_reg.h"
#endif
#include "picachu_log.h"
#include "picachu_misc.h"
#include "picachu_para.h"


/*****************************************************************************
 *
 *****************************************************************************/
static unsigned int MASK_BIT(unsigned int size, unsigned int offset)
{
    unsigned int i;
    unsigned int mask = 0;

    for (i = 0; i < size; i++)
        mask |= (0x1 << i);

    mask = mask << offset;

    return mask;
}


static void read_lut_point(
    unsigned int real_len,
    unsigned int point_word_size,
    unsigned int total_point,
    struct picachu_lut_header *lut_header)
{
    unsigned int i;
    unsigned int cluster_id;
    unsigned int point_id;
    unsigned int freq;
    unsigned int volt;
    unsigned int offset;
    unsigned int value;
    unsigned int *lut_point_start;

    lut_point_start = (unsigned int *)(get_picachu_lut_db_start() + real_len);
    for (i = 0; i < total_point; i++) {
        value = *lut_point_start;
        if (point_word_size == 1) {
            cluster_id = (value & MASK_BIT(lut_header->cluster_id_bit, 0));
            offset = lut_header->cluster_id_bit;

            point_id = (value & MASK_BIT(lut_header->point_id_bit, offset))
                >> offset;
            offset += lut_header->point_id_bit;

            freq = (value & MASK_BIT(lut_header->frequency_bit, offset))
                >> offset;
            offset += lut_header->frequency_bit;

            volt = (value & MASK_BIT(lut_header->voltage_bit, offset))
                >> offset;

            PICACHU_INFO("LUT: cluster_id=%u point_id=%u freq=%u volt=%u(%d)\n",
                cluster_id, point_id, freq, volt, volt *
                picachu_get_pmic_resolution());
        } else {
            cluster_id = (value & MASK_BIT(lut_header->cluster_id_bit, 0));
            offset = lut_header->cluster_id_bit;

            if (offset + lut_header->point_id_bit <= 32) {
                point_id = (value & MASK_BIT(lut_header->point_id_bit, offset))
                    >> offset;
                offset += lut_header->point_id_bit;
            } else {
                lut_point_start++;
                value = *lut_point_start;
                point_id = (value & MASK_BIT(lut_header->point_id_bit, 0));
                offset = lut_header->point_id_bit;
            }

            if (offset + lut_header->frequency_bit <= 32) {
                freq = (value & MASK_BIT(lut_header->frequency_bit, offset))
                >> offset;
                offset += lut_header->frequency_bit;
            } else {
                lut_point_start++;
                value = *lut_point_start;
                freq = (value & MASK_BIT(lut_header->frequency_bit, 0));
                offset = lut_header->frequency_bit;
            }

            if (offset + lut_header->voltage_bit <= 32) {
                volt = (value & MASK_BIT(lut_header->voltage_bit, offset))
                >> offset;
            } else {
                lut_point_start++;
                value = *lut_point_start;
                volt = (value & MASK_BIT(lut_header->voltage_bit, 0));
            }
        }
        lut_point_start++;

        lut_update_vf_table(cluster_id, point_id, freq, volt);
    }
}


static int found_hrid_lut(
    unsigned int *hrid,
    struct picachu_lut_header *lut_header)
{
    unsigned int real_len = 0;
    char *lut_db_start = get_picachu_lut_db_start();
    unsigned int *ptr;
    unsigned int idx = 0;
    unsigned int found = 0;
    unsigned int total_point;
    unsigned int hrid_macth_cnt;
    unsigned int total_bits;
    unsigned int point_word_size;

    if (lut_db_start == NULL || lut_header == NULL) {
        PICACHU_INFO("lut_db_start=%p lut_header=%p\n", lut_db_start, lut_header);
        return -1;
    }

    total_bits = lut_header->cluster_id_bit +
            lut_header->point_id_bit +
            lut_header->frequency_bit +
            lut_header->voltage_bit;
    point_word_size = total_bits / 32;
    if ((point_word_size * 32) < total_bits)
        point_word_size++;

    while (real_len < lut_header->db_size) {
        total_point = 0;
        hrid_macth_cnt = 0;
        ptr = (unsigned int *)(lut_db_start + real_len);
        if (((*ptr) & 0xFFFFFF00) == 0xA5BEEF00) {
            total_point = (*ptr) & 0xFF;
            real_len += sizeof(unsigned int);
            ptr = (unsigned int *)(lut_db_start + real_len);
            for (idx = 0; idx < lut_header->hrid_num; idx++) {
                PICACHU_DEBUG("LUT: hrid[%d]=0x%X\n", idx, *ptr);
                if (hrid[idx] == *ptr)
                    hrid_macth_cnt++;

                real_len += sizeof(unsigned int);
                ptr = (unsigned int *)(lut_db_start + real_len);
            }

            if (hrid_macth_cnt != lut_header->hrid_num)
                real_len += (total_point * point_word_size * sizeof(unsigned int));
            else {
                found = 1;
                PICACHU_INFO("LUT: hrid match\n");
                break;
            }
        } else
            real_len += sizeof(unsigned int);

    }

    if (found == 1)
        read_lut_point(real_len, point_word_size, total_point, lut_header);

    return found;
}


/*****************************************************************************
 *
 *****************************************************************************/
int check_lut(void)
{
    unsigned int found = 0;
    struct picachu_lut_header *lut_header = NULL;
    unsigned int hrid[4] = {0, 0, 0, 0};

    lut_header = get_picachu_lut_header();
    if (lut_header) {
        hrid[0] = picachu_read32(PICACHU_EFUSE_HRID0);
        hrid[1] = picachu_read32(PICACHU_EFUSE_HRID1);
        hrid[2] = picachu_read32(PICACHU_EFUSE_HRID2);
        hrid[3] = picachu_read32(PICACHU_EFUSE_HRID3);
    } else {
        PICACHU_INFO("%s: look up table not found\n", __func__);
    }

    PICACHU_INFO("LUT: hrid[0]=0x%X hrid[1]=0x%X hrid[2]=0x%X hrid[3]=0x%X\n",
        hrid[0], hrid[1], hrid[2], hrid[3]);

    if (hrid[0] == 0 && hrid[1] == 0 && hrid[2] == 0 && hrid[3] == 0) {
        PICACHU_WARN("HARD ID is empty\n");
        return -1;
    }

    found = found_hrid_lut(hrid, lut_header);
    PICACHU_INFO("%s: found=%d\n", __func__, found);

    return found;
}


#ifdef SUPPORT_DVCS_LUT
/*****************************************************************************
 *
 *****************************************************************************/
static unsigned int dvcs_mincode_tbl[MAX_MINCODE_TBL_SIZE];


unsigned int *get_dvcs_mincode_tbl()
{
    return dvcs_mincode_tbl;
}


static void read_dvcs_lut_point(
    unsigned int real_len,
    unsigned int point_word_size,
    unsigned int total_point,
    struct picachu_dvcs_lut_header *dvcs_lut_header)
{
    unsigned int i;
    unsigned int voltage;
    unsigned int tj;
    unsigned int fc;
    unsigned int cc;
    unsigned int nptp_id;
    unsigned int offset;
    unsigned int value;
    unsigned int *lut_point_start;

    lut_point_start = (unsigned int *)(void *)(get_picachu_dvcs_lut_db_start() + real_len);
    for (i = 0; i < total_point; i++) {
        value = *lut_point_start;
        if (point_word_size == 1) {
            dvcs_mincode_tbl[i] = value;
            voltage = (value & MASK_BIT(dvcs_lut_header->voltage_bits, 0));
            offset = dvcs_lut_header->voltage_bits;

            tj = (value & MASK_BIT(dvcs_lut_header->tj_bits, offset))
                >> offset;
            offset += dvcs_lut_header->tj_bits;

            fc = (value & MASK_BIT(dvcs_lut_header->fc_id_bits, offset))
                >> offset;
            offset += dvcs_lut_header->fc_id_bits;

            cc = (value & MASK_BIT(dvcs_lut_header->cc_id_bits, offset))
                >> offset;
            offset += dvcs_lut_header->cc_id_bits;

            nptp_id = (value & MASK_BIT(dvcs_lut_header->nptp_id_bits, offset))
                >> offset;

            PICACHU_INFO("DVCS LUT: nptp_id=%u cc=%u fc=%u tj=%u voltage=%u value=%x\n",
                nptp_id, cc, fc, tj, voltage * picachu_get_pmic_resolution(), dvcs_mincode_tbl[i]);
        } else {
            voltage = (value & MASK_BIT(dvcs_lut_header->voltage_bits, 0));
            offset = dvcs_lut_header->voltage_bits;

            if (offset + dvcs_lut_header->tj_bits <= 32) {
                tj = (value & MASK_BIT(dvcs_lut_header->tj_bits, offset))
                    >> offset;
                offset += dvcs_lut_header->tj_bits;
            } else {
                lut_point_start++;
                value = *lut_point_start;
                tj = (value & MASK_BIT(dvcs_lut_header->tj_bits, 0));
                offset = dvcs_lut_header->tj_bits;
            }

            if (offset + dvcs_lut_header->fc_id_bits <= 32) {
                fc = (value & MASK_BIT(dvcs_lut_header->fc_id_bits, offset))
                >> offset;
                offset += dvcs_lut_header->fc_id_bits;
            } else {
                lut_point_start++;
                value = *lut_point_start;
                fc = (value & MASK_BIT(dvcs_lut_header->fc_id_bits, 0));
                offset = dvcs_lut_header->fc_id_bits;
            }

            if (offset + dvcs_lut_header->cc_id_bits <= 32) {
                cc = (value & MASK_BIT(dvcs_lut_header->cc_id_bits, offset))
                >> offset;
                offset += dvcs_lut_header->cc_id_bits;
            } else {
                lut_point_start++;
                value = *lut_point_start;
                cc = (value & MASK_BIT(dvcs_lut_header->cc_id_bits, 0));
                offset += dvcs_lut_header->cc_id_bits;
            }

            if (offset + dvcs_lut_header->nptp_id_bits <= 32) {
                nptp_id = (value & MASK_BIT(dvcs_lut_header->nptp_id_bits, offset))
                >> offset;
            } else {
                lut_point_start++;
                value = *lut_point_start;
                nptp_id = (value & MASK_BIT(dvcs_lut_header->nptp_id_bits, 0));
            }

            PICACHU_INFO("DVCS LUT: nptp_id=%u cc=%u fc=%u tj=%u voltage=%u\\n",
                nptp_id, cc, fc, tj, voltage * picachu_get_pmic_resolution());
        }
        lut_point_start++;
    }
}


static int found_hrid_dvcs_lut(
    unsigned int *hrid,
    struct picachu_dvcs_lut_header *dvcs_lut_header)
{
    unsigned int real_len = 0;
    char *dvcs_lut_db_start = get_picachu_dvcs_lut_db_start();
    unsigned int *ptr;
    unsigned int idx = 0;
    unsigned int found = 0;
    unsigned int total_point;
    unsigned int hrid_macth_cnt;
    unsigned int total_bits;
    unsigned int point_word_size;

    unsigned int record_number = 0;

    if (dvcs_lut_db_start == NULL || dvcs_lut_header == NULL) {
        PICACHU_INFO("dvcs_lut_db_start=%p lut_header=%p\n", dvcs_lut_db_start, dvcs_lut_header);
        return -1;
    }

    total_bits = dvcs_lut_header->nptp_id_bits +
            dvcs_lut_header->cc_id_bits +
            dvcs_lut_header->fc_id_bits +
            dvcs_lut_header->tj_bits +
            dvcs_lut_header->voltage_bits;
    point_word_size = total_bits / 32;
    if ((point_word_size * 32) < total_bits)
        point_word_size++;

    while (real_len < dvcs_lut_header->db_size) {
        total_point = 0;
        hrid_macth_cnt = 0;
        ptr = (unsigned int *)(void *)(dvcs_lut_db_start + real_len);
        if (((*ptr) & 0xFFFFFF00) == 0xCA1CA100) {
            record_number++;
            total_point = (*ptr) & 0xFF;
            real_len += sizeof(unsigned int);
            ptr = (unsigned int *)(void *)(dvcs_lut_db_start + real_len);
            for (idx = 0; idx < dvcs_lut_header->hrid_num; idx++) {
                PICACHU_DEBUG("DVCS LUT: [%d] hrid[%d]=0x%X\n", record_number, idx, *ptr);
                if (hrid[idx] == *ptr)
                    hrid_macth_cnt++;

                real_len += sizeof(unsigned int);
                ptr = (unsigned int *)(void *)(dvcs_lut_db_start + real_len);
            }

            if (hrid_macth_cnt != dvcs_lut_header->hrid_num)
                real_len += (total_point * point_word_size * sizeof(unsigned int));
            else {
                found = 1;
                PICACHU_INFO("DVCS LUT: hrid match\n");
                break;
            }
        } else
            real_len += sizeof(unsigned int);

    }

    if (found == 1)
        read_dvcs_lut_point(real_len, point_word_size, total_point, dvcs_lut_header);

    return found;
}

int check_dvcs_lut(void)
{
    unsigned int found = 0;
    struct picachu_dvcs_lut_header *dvcs_lut_header = NULL;
    unsigned int hrid[4] = {0, 0, 0, 0};

    dvcs_lut_header = get_picachu_dvcs_lut_header();
    if (dvcs_lut_header) {
        hrid[0] = picachu_read32(PICACHU_EFUSE_HRID0);
        hrid[1] = picachu_read32(PICACHU_EFUSE_HRID1);
        hrid[2] = picachu_read32(PICACHU_EFUSE_HRID2);
        hrid[3] = picachu_read32(PICACHU_EFUSE_HRID3);
    } else {
        PICACHU_INFO("%s: look up table not found\n", __func__);
    }

    PICACHU_INFO("DVCS LUT: hrid[0]=0x%X hrid[1]=0x%X hrid[2]=0x%X hrid[3]=0x%X\n",
        hrid[0], hrid[1], hrid[2], hrid[3]);

    if (hrid[0] == 0 && hrid[1] == 0 && hrid[2] == 0 && hrid[3] == 0) {
        PICACHU_WARN("HARD ID is empty\n");
        return -1;
    }

    found = found_hrid_dvcs_lut(hrid, dvcs_lut_header);
    PICACHU_INFO("%s: found=%d\n", __func__, found);

    return found;
}
#endif
