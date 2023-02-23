/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <bits.h>
#include <compiler.h>
#include <debug.h>
#include <emi_addr2dram.h>
#include <libfdt.h>
#include <reg.h>

#include "emi_addr2dram_v2.h"
#include "emi_util.h"

#define EMI_CONA_CHN_POS_0 2
#define EMI_CONA_COL 4
#define EMI_CONA_COL2ND 6
#define EMI_CONA_CHN_EN 8
#define EMI_CONA_ROW 12
#define EMI_CONA_ROW2ND 14
#define EMI_CONA_DUAL_RANK_EN_CHN1 16
#define EMI_CONA_DUAL_RANK_EN 17
#define EMI_CONA_CHN1_COL 20
#define EMI_CONA_CHN1_COL2ND 22
#define EMI_CONA_ROW_EXT0 24
#define EMI_CONA_ROW2ND_EXT0 25
#define EMI_CONA_CHN1_ROW 28
#define EMI_CONA_CHN1_ROW2ND 30

#define EMI_CONH_CHN1_ROW_EXT0 4
#define EMI_CONH_CHN1_ROW2ND_EXT0 5
#define EMI_CONH_CHNAB_RANK0_SIZE 16
#define EMI_CONH_CHNAB_RANK1_SIZE 20
#define EMI_CONH_CHNCD_RANK0_SIZE 24
#define EMI_CONH_CHNCD_RANK1_SIZE 28

#define EMI_CONK_CHNAB_RANK0_SIZE_EXT 16
#define EMI_CONK_CHNAB_RANK1_SIZE_EXT 20
#define EMI_CONK_CHNCD_RANK0_SIZE_EXT 24
#define EMI_CONK_CHNCD_RANK1_SIZE_EXT 28

#define EMI_CHN_CONA_DUAL_RANK_EN 0
#define EMI_CHN_CONA_DW32_EN 1
#define EMI_CHN_CONA_ROW_EXT0 2
#define EMI_CHN_CONA_ROW2ND_EXT0 3
#define EMI_CHN_CONA_COL 4
#define EMI_CHN_CONA_COL2ND 6
#define EMI_CHN_CONA_RANK0_SIZE_EXT 8
#define EMI_CHN_CONA_RANK1_SIZE_EXT 9
#define EMI_CHN_CONA_16BANK_MODE 10
#define EMI_CHN_CONA_16BANK_MODE_2ND 11
#define EMI_CHN_CONA_ROW 12
#define EMI_CHN_CONA_ROW2ND 14
#define EMI_CHN_CONA_RANK0_SZ 16
#define EMI_CHN_CONA_RANK1_SZ 20
#define EMI_CHN_CONA_4BANK_MODE 24
#define EMI_CHN_CONA_4BANK_MODE_2ND 25
#define EMI_CHN_CONA_RANK_POS 27
#define EMI_CHN_CONA_BG1_BK3_POS 31

#define EMI_CHN_CONC_SCRM_EN 7

#define MTK_EMI_DRAM_OFFSET 0x40000000
#define MTK_EMI_DISPATCH_RULE 0x0
#define MTK_EMI_HASH_RULE 0x7

static unsigned int emi_a2d_con_offset[] = {
    /* central EMI CONA, CONF, CONH, CONH_2ND, CONK, SCRM_EXT */
    0x00, 0x28, 0x38, 0x3c, 0x50, 0x7b4
};

static unsigned int emi_a2d_chn_con_offset[] = {
    /* channel EMI CONA, CONC, CONC_2ND */
    0x00, 0x10, 0x14
};

static unsigned long offset;
static unsigned long max_mb;
static unsigned long disph = MTK_EMI_DISPATCH_RULE;
static unsigned long hash = MTK_EMI_HASH_RULE;
static unsigned int chn_bit_position;
static unsigned int chn_en;
static unsigned int chn_scrm_en;
static unsigned int magics[11];
static unsigned int dual_rank_en;
static unsigned int dw32_en;
static unsigned int bg1_bk3_pos;
static unsigned int rank_pos;
static unsigned int magics2[7];
static unsigned int rank0_row_width, rank0_bank_width, rank0_col_width;
static unsigned int rank0_size_MB, rank0_bg_16bank_mode;
static unsigned int rank1_row_width, rank1_bank_width, rank1_col_width;
static unsigned int rank1_size_MB, rank1_bg_16bank_mode;

__WEAK int emi_get_addr2dram_data_without_dts(struct emi_addr2dram_data *data)
{
    return 0;
}

static void prepare_a2d(void)
{
    static int prepared;
    struct emi_addr2dram_data addr2dram_data;
    void *fdt;
    const unsigned int mask_6b = 0x3f, mask_4b = 0xf, mask_2b = 0x3;
    int off, off2, len, array_len, i;
    unsigned int *data;
    unsigned int emi_con_base, emi_chn_con_base;
    unsigned long emi_cona, emi_conf, emi_conh, emi_conh_2nd, emi_conk, emi_scrm_ext;
    unsigned long emi_chn_cona, emi_chn_conc, emi_chn_conc_2nd;
    int tmp;
    int col, col2nd, row, row2nd, row_ext0, row2nd_ext0;
    int rank0_size, rank1_size, rank0_size_ext, rank1_size_ext;
    int chn_4bank_mode, chn_bg_16bank_mode, chn_bg_16bank_mode_2nd;
    int b11s, b12s, b13s, b14s, b15s, b16s;
    int b8s, b11s_ext, b12s_ext, b13s_ext, b14s_ext, b15s_ext, b16s_ext;
    unsigned long ch0_rk0_sz, ch0_rk1_sz;
    unsigned long ch1_rk0_sz, ch1_rk1_sz;

    if (prepared)
        return;

    fdt = emi_get_fdt();
    if (fdt) {
        array_len = countof(emicen_compatible);
        for (i = 0; i < array_len; i++) {
            off = fdt_path_offset(fdt, emicen_compatible[i]);
            if (off >= 0)
                break;
        }

        if (i >= array_len) {
            EMI_DBG("%s couldn't find the emicen node\n", __func__);
            return;
        }

        array_len = countof(emichn_compatible);
        for (i = 0; i < array_len; i++) {
            off2 = fdt_path_offset(fdt, emichn_compatible[i]);
            if (off2 >= 0)
                break;
        }

        if (i >= array_len) {
            EMI_DBG("%s couldn't find the emichn node\n", __func__);
            return;
        }

        data = (unsigned int *)fdt_getprop(fdt, off, "reg", &len);
        if (!data || !len) {
            EMI_DBG("%s couldn't find property reg\n", __func__);
            return;
        }

        emi_con_base = fdt32_to_cpu(data[1]);

        array_len = countof(emi_a2d_con_offset);
        data = (unsigned int *)fdt_getprop(fdt, off, "a2d_conf_offset", &len);
        if (data && len) {
            for (i = 0; i < array_len; i++)
                emi_a2d_con_offset[i] = fdt32_to_cpu(data[i]);
        }

        emi_cona = readl(emi_con_base + emi_a2d_con_offset[0]);
        emi_conf = readl(emi_con_base + emi_a2d_con_offset[1]);
        emi_conh = readl(emi_con_base + emi_a2d_con_offset[2]);
        emi_conh_2nd = readl(emi_con_base + emi_a2d_con_offset[3]);
        emi_conk = readl(emi_con_base + emi_a2d_con_offset[4]);
        emi_scrm_ext = readl(emi_con_base + emi_a2d_con_offset[5]);

        data = (unsigned int *)fdt_getprop(fdt, off2, "reg", &len);
        if (!data || !len) {
            EMI_DBG("%s couldn't find property reg\n", __func__);
            return;
        }

        emi_chn_con_base = fdt32_to_cpu(data[1]);
        array_len = countof(emi_a2d_chn_con_offset);
        data = (unsigned int *)fdt_getprop(fdt, off, "a2d_chn_conf_offset", &len);
        if (data && len) {
            for (i = 0; i < array_len; i++)
                emi_a2d_chn_con_offset[i] = fdt32_to_cpu(data[i]);
        }

        emi_chn_cona = readl(emi_chn_con_base + emi_a2d_chn_con_offset[0]);
        emi_chn_conc = readl(emi_chn_con_base + emi_a2d_chn_con_offset[1]);
        emi_chn_conc_2nd = readl(emi_chn_con_base + emi_a2d_chn_con_offset[2]);

        data = (unsigned int *)fdt_getprop(fdt, off, "a2d_disph", &len);
        if (data && len)
            disph = fdt32_to_cpu(*data);

        data = (unsigned int *)fdt_getprop(fdt, off, "a2d_hash", &len);
        if (data && len)
            hash = fdt32_to_cpu(*data);
    } else {
        if (!emi_get_addr2dram_data_without_dts(&addr2dram_data)) {
            EMI_DBG("%s couldn't get addr2dram data !!!\n", __func__);
            return;
        } else {
            emi_cona = addr2dram_data.emi_cona;
            emi_conf = addr2dram_data.emi_conf;
            emi_conh = addr2dram_data.emi_conh;
            emi_conk = addr2dram_data.emi_conk;
            emi_chn_cona = addr2dram_data.emi_chn_cona;
            emi_chn_conc = addr2dram_data.emi_chn_conc;
            emi_chn_conc_2nd = addr2dram_data.emi_chn_conc_2nd;
            disph = addr2dram_data.disph;
            hash = addr2dram_data.hash;
        }
    }

    offset = MTK_EMI_DRAM_OFFSET;

    tmp = (emi_cona >> EMI_CONA_CHN_POS_0) & mask_2b;
    switch (tmp) {
    case 3:
        chn_bit_position = 12;
        break;
    case 2:
        chn_bit_position = 9;
        break;
    case 1:
        chn_bit_position = 8;
        break;
    default:
        chn_bit_position = 7;
        break;
    }

    chn_en = (emi_cona >> EMI_CONA_CHN_EN) & mask_2b;

    magics[2] = (emi_conh_2nd >> 24) & mask_4b;
    magics[3] = emi_conf & mask_4b;
    magics[4] = (emi_conf >> 4) & mask_4b;
    magics[5] = (emi_conf >> 8) & mask_4b;
    magics[6] = (emi_conf >> 12) & mask_4b;
    magics[7] = (emi_conf >> 16) & mask_4b;
    magics[8] = (emi_conf >> 20) & mask_4b;
    magics[9] = (emi_conf >> 24) & mask_4b;
    magics[10] = (emi_conf >> 28) & mask_4b;
    magics[0] = emi_scrm_ext & mask_6b;
    magics[1] = (emi_scrm_ext >> 6) & mask_6b;
    magics[2] += ((emi_scrm_ext >> 12) & mask_2b) << 4;
    magics[3] += ((emi_scrm_ext >> 14) & mask_2b) << 4;
    magics[4] += ((emi_scrm_ext >> 16) & mask_2b) << 4;
    magics[5] += ((emi_scrm_ext >> 18) & mask_2b) << 4;
    magics[6] += ((emi_scrm_ext >> 20) & mask_2b) << 4;
    magics[7] += ((emi_scrm_ext >> 22) & mask_2b) << 4;
    magics[8] += ((emi_scrm_ext >> 24) & mask_2b) << 4;
    magics[9] += ((emi_scrm_ext >> 26) & mask_2b) << 4;
    magics[10] += ((emi_scrm_ext >> 28) & mask_2b) << 4;

    dual_rank_en =
            bitmap_test(&emi_chn_cona, EMI_CHN_CONA_DUAL_RANK_EN) ?  1 : 0;
    dw32_en = bitmap_test(&emi_chn_cona, EMI_CHN_CONA_DW32_EN) ? 1 : 0;
    row_ext0 = bitmap_test(&emi_chn_cona, EMI_CHN_CONA_ROW_EXT0) ? 1 : 0;
    row2nd_ext0 = bitmap_test(&emi_chn_cona, EMI_CHN_CONA_ROW2ND_EXT0) ? 1 : 0;
    col = (emi_chn_cona >> EMI_CHN_CONA_COL) & mask_2b;
    col2nd = (emi_chn_cona >> EMI_CHN_CONA_COL2ND) & mask_2b;
    rank0_size_ext =
            bitmap_test(&emi_chn_cona, EMI_CHN_CONA_RANK0_SIZE_EXT) ? 1 : 0;
    rank1_size_ext =
            bitmap_test(&emi_chn_cona, EMI_CHN_CONA_RANK1_SIZE_EXT) ? 1 : 0;
    chn_bg_16bank_mode =
            bitmap_test(&emi_chn_cona, EMI_CHN_CONA_16BANK_MODE) ? 1 : 0;
    chn_bg_16bank_mode_2nd =
            bitmap_test(&emi_chn_cona, EMI_CHN_CONA_16BANK_MODE_2ND) ? 1 : 0;
    row = (emi_chn_cona >> EMI_CHN_CONA_ROW) & mask_2b;
    row2nd = (emi_chn_cona >> EMI_CHN_CONA_ROW2ND) & mask_2b;
    rank0_size = (emi_chn_cona >> EMI_CHN_CONA_RANK0_SZ) & mask_4b;
    rank1_size = (emi_chn_cona >> EMI_CHN_CONA_RANK1_SZ) & mask_4b;
    chn_4bank_mode =
            bitmap_test(&emi_chn_cona, EMI_CHN_CONA_4BANK_MODE) ? 1 : 0;
    rank_pos = bitmap_test(&emi_chn_cona, EMI_CHN_CONA_RANK_POS) ? 1 : 0;
    bg1_bk3_pos =
            bitmap_test(&emi_chn_cona, EMI_CHN_CONA_BG1_BK3_POS) ? 1 : 0;
    chn_scrm_en = bitmap_test(&emi_chn_conc, EMI_CHN_CONC_SCRM_EN);
    b11s = (emi_chn_conc >> 8) & mask_4b;
    b12s = (emi_chn_conc >> 12) & mask_4b;
    b13s = (emi_chn_conc >> 16) & mask_4b;
    b14s = (emi_chn_conc >> 20) & mask_4b;
    b15s = (emi_chn_conc >> 24) & mask_4b;
    b16s = (emi_chn_conc >> 28) & mask_4b;

    b11s_ext = (emi_chn_conc_2nd >> 4) & mask_2b;
    b12s_ext = (emi_chn_conc_2nd >> 6) & mask_2b;
    b13s_ext = (emi_chn_conc_2nd >> 8) & mask_2b;
    b14s_ext = (emi_chn_conc_2nd >> 10) & mask_2b;
    b15s_ext = (emi_chn_conc_2nd >> 12) & mask_2b;
    b16s_ext = (emi_chn_conc_2nd >> 14) & mask_2b;
    b8s = (emi_chn_conc_2nd >> 16) & mask_6b;

    magics2[0] = b8s;
    magics2[1] = b11s_ext * 16 + b11s;
    magics2[2] = b12s_ext * 16 + b12s;
    magics2[3] = b13s_ext * 16 + b13s;
    magics2[4] = b14s_ext * 16 + b14s;
    magics2[5] = b15s_ext * 16 + b15s;
    magics2[6] = b16s_ext * 16 + b16s;

    rank0_row_width = row_ext0 * 4 + row + 13;
    rank0_bank_width = (chn_bg_16bank_mode == 1) ? 4 :
            (chn_4bank_mode == 1) ? 2 : 3;
    rank0_col_width = col + 9;
    rank0_bg_16bank_mode = chn_bg_16bank_mode;
    rank0_size_MB = (rank0_size_ext * 16 + rank0_size) * 256;
    if (!(rank0_size_MB)) {
        tmp = rank0_row_width + rank0_bank_width;
        tmp += rank0_col_width + dw32_en;
        rank0_size_MB = 2 << (tmp - 20);
    }

    rank1_row_width = row2nd_ext0 * 4 + row2nd + 13;
    rank1_bank_width = (chn_bg_16bank_mode_2nd == 1) ? 4 :
            (chn_4bank_mode == 1) ? 2 : 3;
    rank1_col_width = col2nd + 9;
    rank1_bg_16bank_mode = chn_bg_16bank_mode_2nd;
    rank1_size_MB = (rank1_size_ext * 16 + rank1_size) * 256;
    if (!(rank1_size_MB)) {
        tmp = rank1_row_width + rank1_bank_width;
        tmp += rank1_col_width + dw32_en;
        rank1_size_MB = 2 << (tmp - 20);
    }

    if (rank0_size_MB)
        ch0_rk0_sz = rank0_size_MB;
    else {
        tmp = rank0_row_width + rank0_bank_width;
        tmp += rank0_col_width + dw32_en ? 2 : 1;
        tmp -= 20;
        ch0_rk0_sz = 1 << tmp;
    }

    ch1_rk0_sz = ch0_rk0_sz;

    if (rank1_size_MB)
        ch0_rk1_sz = rank1_size_MB;
    else {
        tmp = rank1_row_width + rank1_bank_width;
        tmp += rank1_col_width + dw32_en ? 2 : 1;
        tmp -= 20;
        ch0_rk1_sz = 1 << tmp;
    }

    ch1_rk1_sz = ch0_rk1_sz;

    max_mb = ch0_rk0_sz;
    if (dual_rank_en)
        max_mb += ch0_rk1_sz;

    if (chn_en)
        max_mb += ch1_rk0_sz + ((dual_rank_en) ? ch1_rk1_sz : 0);

    if (disph)
        max_mb *= 2;

    prepared = 1;
}

static inline unsigned int use_a2d_magic_v2(unsigned long addr,
        unsigned long magic,
        unsigned int bit)
{
    unsigned int ret;

    ret = bitmap_test(&addr, bit) ? 1 : 0;
    ret ^= (bitmap_test(&addr, 16) & bitmap_test(&magic, 0)) ? 1 : 0;
    ret ^= (bitmap_test(&addr, 17) & bitmap_test(&magic, 1)) ? 1 : 0;
    ret ^= (bitmap_test(&addr, 18) & bitmap_test(&magic, 2)) ? 1 : 0;
    ret ^= (bitmap_test(&addr, 19) & bitmap_test(&magic, 3)) ? 1 : 0;
    ret ^= (bitmap_test(&addr, 20) & bitmap_test(&magic, 4)) ? 1 : 0;
    ret ^= (bitmap_test(&addr, 21) & bitmap_test(&magic, 5)) ? 1 : 0;

    return ret;
}

static inline unsigned long a2d_rm_bit(unsigned long taddr, int bit)
{
    unsigned long ret;

    ret = taddr;
    bitmap_clear(&ret, bit);

    ret = ret >> (bit + 1);
    ret = ret << bit;
    ret = ret & ~((1UL << bit) - 1);

    ret = ret | (taddr & ((1UL << bit) - 1));

    return ret;
}

int emi_addr2dram_v2(unsigned long addr, struct emi_addr_map *map)
{
    unsigned long saddr, taddr, bgaddr, noraddr;
    unsigned long tmp;
    int emi_tpos, chn_tpos;

    prepare_a2d();

    if (!map)
        return -1;
    else {
        map->emi = -1;
        map->channel = -1;
        map->rank = -1;
        map->bank = -1;
        map->row = -1;
        map->column = -1;
    }

    if (addr < offset)
        return -1;

    addr -= offset;
    if ((addr >> 20) > max_mb)
        return -1;

    saddr = addr;
    bitmap_clear(&saddr, 6);
    bitmap_clear(&saddr, 7);
    bitmap_clear(&saddr, 8);
    bitmap_clear(&saddr, 9);
    bitmap_clear(&saddr, 10);
    bitmap_clear(&saddr, 11);
    bitmap_clear(&saddr, 12);
    bitmap_clear(&saddr, 13);
    bitmap_clear(&saddr, 14);
    bitmap_clear(&saddr, 15);
    bitmap_clear(&saddr, 16);
    saddr |= use_a2d_magic_v2(addr, magics[0], 6) << 6;
    saddr |= use_a2d_magic_v2(addr, magics[1], 7) << 7;
    saddr |= use_a2d_magic_v2(addr, magics[2], 8) << 8;
    saddr |= use_a2d_magic_v2(addr, magics[3], 9) << 9;
    saddr |= use_a2d_magic_v2(addr, magics[4], 10) << 10;
    saddr |= use_a2d_magic_v2(addr, magics[5], 11) << 11;
    saddr |= use_a2d_magic_v2(addr, magics[6], 12) << 12;
    saddr |= use_a2d_magic_v2(addr, magics[7], 13) << 13;
    saddr |= use_a2d_magic_v2(addr, magics[8], 14) << 14;
    saddr |= use_a2d_magic_v2(addr, magics[9], 15) << 15;
    saddr |= use_a2d_magic_v2(addr, magics[10], 16) << 16;

    if (!hash) {
        map->channel = bitmap_test(&saddr, chn_bit_position) ? 1 : 0;
        chn_tpos = chn_bit_position;
    } else {
        tmp = (bitmap_test(&saddr, 8) && bitmap_test(&hash, 0)) ? 1 : 0;
        tmp ^= (bitmap_test(&saddr, 9) && bitmap_test(&hash, 1)) ? 1 : 0;
        tmp ^= (bitmap_test(&saddr, 10) && bitmap_test(&hash, 2)) ? 1 : 0;
        tmp ^= (bitmap_test(&saddr, 11) && bitmap_test(&hash, 3)) ? 1 : 0;
        map->channel = tmp;

        if (bitmap_test(&hash, 0))
            chn_tpos = 8;
        else if (bitmap_test(&hash, 1))
            chn_tpos = 9;
        else if (bitmap_test(&hash, 2))
            chn_tpos = 10;
        else if (bitmap_test(&hash, 3))
            chn_tpos = 11;
        else
            chn_tpos = -1;
    }

    if (!disph) {
        map->emi = 0;
        emi_tpos = -1;
    } else {
        tmp = (bitmap_test(&saddr, 8) && bitmap_test(&disph, 0)) ? 1 : 0;
        tmp ^= (bitmap_test(&saddr, 9) && bitmap_test(&disph, 1)) ? 1 : 0;
        tmp ^= (bitmap_test(&saddr, 10) && bitmap_test(&disph, 2)) ? 1 : 0;
        tmp ^= (bitmap_test(&saddr, 11) && bitmap_test(&disph, 3)) ? 1 : 0;
        map->emi = tmp;

        if (bitmap_test(&disph, 0))
            emi_tpos = 8;
        else if (bitmap_test(&disph, 1))
            emi_tpos = 9;
        else if (bitmap_test(&disph, 2))
            emi_tpos = 10;
        else if (bitmap_test(&disph, 3))
            emi_tpos = 11;
        else
            emi_tpos = -1;
    }

    taddr = saddr;
    if (!disph) {
        if (!chn_en)
            taddr = saddr;
        else
            taddr = a2d_rm_bit(taddr, chn_tpos);
    } else {
        if ((chn_tpos < 0) || (emi_tpos < 0))
            return -1;

        if (!chn_en)
            taddr = a2d_rm_bit(taddr, emi_tpos);
        else if (emi_tpos > chn_tpos) {
            taddr = a2d_rm_bit(taddr, emi_tpos);
            taddr = a2d_rm_bit(taddr, chn_tpos);
        } else {
            taddr = a2d_rm_bit(taddr, chn_tpos);
            taddr = a2d_rm_bit(taddr, emi_tpos);
        }
    }

    saddr = taddr;
    if (chn_scrm_en) {
        bitmap_clear(&saddr, 8);
        bitmap_clear(&saddr, 11);
        bitmap_clear(&saddr, 12);
        bitmap_clear(&saddr, 13);
        bitmap_clear(&saddr, 14);
        bitmap_clear(&saddr, 15);
        bitmap_clear(&saddr, 16);
        saddr |= use_a2d_magic_v2(taddr, magics2[0], 8) << 8;
        saddr |= use_a2d_magic_v2(taddr, magics2[1], 11) << 11;
        saddr |= use_a2d_magic_v2(taddr, magics2[2], 12) << 12;
        saddr |= use_a2d_magic_v2(taddr, magics2[3], 13) << 13;
        saddr |= use_a2d_magic_v2(taddr, magics2[4], 14) << 14;
        saddr |= use_a2d_magic_v2(taddr, magics2[5], 15) << 15;
        saddr |= use_a2d_magic_v2(taddr, magics2[6], 16) << 16;
    }
    if (!dual_rank_en)
        map->rank = 0;
    else {
        if (!rank_pos) {
            map->rank = ((saddr >> 20) >= rank0_size_MB) ?
                    1 : 0;
        } else {
            tmp = 1 + dw32_en;
            tmp += rank0_col_width + rank0_bank_width;
            map->rank = saddr >> tmp;
        }
    }

    tmp = (map->rank)
            ? rank1_bg_16bank_mode
            : rank0_bg_16bank_mode;
    if (tmp) {
        bgaddr = a2d_rm_bit(saddr, 8);
        map->column = (bgaddr >> (1 + dw32_en))
                % (1 << ((map->rank)
                ? rank1_col_width
                : rank0_col_width));

        tmp = (map->rank) ? rank1_col_width : rank0_col_width;
        tmp = (bgaddr >> (1 + dw32_en + tmp))
                % (1 << ((map->rank)
                ? rank1_bank_width - 1
                : rank0_bank_width - 1));
        map->bank = bitmap_test(&tmp, (bg1_bk3_pos) ? 0 : 1) ? 1 : 0;
        map->bank += bitmap_test(&tmp, (bg1_bk3_pos) ? 1 : 2) ? 2 : 0;
        map->bank += bitmap_test(&saddr, 8) ? 4 : 0;
        map->bank += bitmap_test(&tmp, (bg1_bk3_pos) ? 2 : 0) ? 8 : 0;
    } else {
        map->column = (saddr >> (1 + dw32_en))
                % (1 << ((map->rank)
                ? rank1_col_width
                : rank0_col_width));

        tmp = (map->rank) ? rank1_col_width : rank0_col_width;
        map->bank = (saddr >> (1 + dw32_en + tmp))
                % (1 << ((map->rank)
                ? rank1_bank_width
                : rank0_bank_width));
    }

    if (!rank_pos) {
        noraddr = (map->rank) ?
                saddr - (rank0_size_MB << 20) : saddr;
    } else {
        tmp = 1 + dw32_en;
        tmp += (map->rank) ? rank1_bank_width : rank0_bank_width;
        tmp += (map->rank) ? rank1_col_width : rank0_col_width;
        noraddr = a2d_rm_bit(saddr, tmp);
    }
    tmp = 1 + dw32_en;
    tmp += (map->rank) ? rank1_bank_width : rank0_bank_width;
    tmp += (map->rank) ? rank1_col_width : rank0_col_width;
    noraddr = noraddr >> tmp;
    tmp = (map->rank) ? rank1_row_width : rank0_row_width;
    map->row = noraddr % (1 << tmp);

    return 0;
}
