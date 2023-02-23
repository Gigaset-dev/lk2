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

#include "emi_addr2dram_v1.h"
#include "emi_util.h"

#define MTK_EMI_DRAM_OFFSET 0x40000000
#define MTK_EMI_DISPATCH_RULE 0x0
#define MTK_EMI_HASH_RULE 0x7

static unsigned int emi_a2d_con_offset[] = {
    0x00, 0x28, 0x38, 0x3c, 0x50,
};

static unsigned long offset;
static unsigned long max_mb;
static unsigned long disph = MTK_EMI_DISPATCH_RULE;
static unsigned int magics[8];
static unsigned int cas;
static unsigned long hash = MTK_EMI_HASH_RULE;
static unsigned int chab_rk0_sz, chab_rk1_sz;
static unsigned int chcd_rk0_sz, chcd_rk1_sz;
static unsigned int channels;
static unsigned int dualrk_ch0, dualrk_ch1;
static unsigned int chn_hash_lsb, chnpos;
static unsigned int chab_row_mask[2], chcd_row_mask[2];
static unsigned int chab_col_mask[2], chcd_col_mask[2];
static unsigned int dw32;
static unsigned int chn_4bank_mode;

__WEAK int emi_get_addr2dram_data_without_dts(struct emi_addr2dram_data *data)
{
    return 0;
}

static inline unsigned long __ffs(unsigned long word)
{
    return (word & (~word + 1));
}

static void prepare_a2d(void)
{
    static int prepared;
    struct emi_addr2dram_data addr2dram_data;
    void *fdt;
    int off, len, array_len, i;
    unsigned int *data;
    unsigned int emi_con_base;
    const unsigned int mask = 0x0000000f;
    unsigned long emi_cona, emi_conf, emi_conh, emi_conh_2nd, emi_conk;
    unsigned int tmp;

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

        data = (unsigned int *)fdt_getprop(fdt, off, "reg", &len);
        if (!data || !len) {
            EMI_DBG("%s couldn't find property reg\n", __func__);
            return;
        }
        emi_con_base = fdt32_to_cpu(data[1]);

        array_len = countof(emi_a2d_con_offset);
        data = (unsigned int *)fdt_getprop(fdt, off, "a2d_conf_offset", &len);
        if (data && len == array_len) {
            for (i = 0; i < array_len; i++)
                emi_a2d_con_offset[i] = fdt32_to_cpu(data[i]);
        } else {
            EMI_DBG("%s couldn't find a2d_conf_offset data\n", __func__);
            return;
        }

        emi_cona = readl(emi_con_base + emi_a2d_con_offset[0]);
        emi_conf = readl(emi_con_base + emi_a2d_con_offset[1]);
        emi_conh = readl(emi_con_base + emi_a2d_con_offset[2]);
        emi_conh_2nd = readl(emi_con_base + emi_a2d_con_offset[3]);
        emi_conk = readl(emi_con_base + emi_a2d_con_offset[4]);

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
            emi_conh_2nd = addr2dram_data.emi_conh_2nd;
            emi_conk = addr2dram_data.emi_conk;
            disph = addr2dram_data.disph;
            hash = addr2dram_data.hash;
        }
    }

    offset = MTK_EMI_DRAM_OFFSET;

    magics[0] = emi_conf & mask;
    magics[1] = (emi_conf >> 4) & mask;
    magics[2] = (emi_conf >> 8) & mask;
    magics[3] = (emi_conf >> 12) & mask;
    magics[4] = (emi_conf >> 16) & mask;
    magics[5] = (emi_conf >> 20) & mask;
    magics[6] = (emi_conf >> 24) & mask;
    magics[7] = (emi_conf >> 28) & mask;

    dw32 = bitmap_test(&emi_cona, 1) ? 1 : 0;

    channels = (emi_cona >> 8) & 0x3;
    cas = (emi_cona >> 18) & 0x3;
    cas += dw32 << 2;
    cas += ((emi_cona >> 26) & 1) << 3;
    cas = cas << 28;
    cas = cas << channels;

    dualrk_ch0 = bitmap_test(&emi_cona, 17) ? 1 : 0;
    dualrk_ch1 = bitmap_test(&emi_cona, 16) ? 1 : 0;

    chn_hash_lsb = 7 + (hash & (~hash + 1));
    if (hash)
        chnpos = chn_hash_lsb;
    else {
        chnpos = bitmap_test(&emi_cona, 3) ? 2 : 0;
        chnpos |= bitmap_test(&emi_cona, 2) ? 1 : 0;
    }

    tmp = (emi_conh >> 16) & 0xf;
    tmp += ((emi_conk >> 16) & 0xf) << 4;
    if (tmp)
        chab_rk0_sz = tmp << 8;
    else {
        tmp = (emi_cona >> 4) & 0x3;
        tmp += (emi_cona >> 12) & 0x3;
        tmp += bitmap_test(&emi_cona, 24) ? 4 : 0;
        tmp += dw32;
        tmp += 7;
        chab_rk0_sz = 1 << tmp;
    }

    tmp = (emi_conh >> 20) & 0xf;
    tmp += ((emi_conk >> 20) & 0xf) << 4;
    if (tmp)
        chab_rk1_sz = tmp << 8;
    else if (!bitmap_test(&emi_cona, 17))
        chab_rk1_sz = 0;
    else {
        tmp = (emi_cona >> 6) & 0x3;
        tmp += (emi_cona >> 14) & 0x3;
        tmp += bitmap_test(&emi_cona, 25) ? 4 : 0;
        tmp += dw32;
        tmp += 7;
        chab_rk1_sz = 1 << tmp;
    }

    tmp = (emi_conh >> 24) & 0xf;
    tmp += ((emi_conk >> 24) & 0xf) << 4;
    if (tmp)
        chcd_rk0_sz = tmp << 8;
    else {
        tmp = (emi_cona >> 20) & 0x3;
        tmp += (emi_cona >> 28) & 0x3;
        tmp += bitmap_test(&emi_conh, 4) ? 4 : 0;
        tmp += dw32;
        tmp += 7;
        chcd_rk0_sz = 1 << tmp;
    }

    tmp = (emi_conh >> 28) & 0xf;
    tmp += ((emi_conk >> 28) & 0xf) << 4;
    if (tmp)
        chcd_rk1_sz = tmp << 8;
    else if (!bitmap_test(&emi_cona, 16))
        chcd_rk1_sz = 0;
    else {
        tmp = (emi_cona >> 22) & 0x3;
        tmp += (emi_cona >> 30) & 0x3;
        tmp += bitmap_test(&emi_conh, 5) ? 4 : 0;
        tmp += dw32;
        tmp += 7;
        chcd_rk1_sz = 1 << tmp;
    }

    max_mb = chab_rk0_sz + chab_rk1_sz;
    max_mb += chcd_rk0_sz + chcd_rk0_sz;
    if ((channels > 1) || (disph > 0))
        max_mb *= 2;

    chab_row_mask[0] = (emi_cona >> 12) & 3;
    chab_row_mask[0] += bitmap_test(&emi_cona, 24) ? 4 : 0;
    chab_row_mask[0] += 13;
    chab_row_mask[1] = (emi_cona >> 14) & 3;
    chab_row_mask[1] += bitmap_test(&emi_cona, 25) ? 4 : 0;
    chab_row_mask[1] += 13;
    chcd_row_mask[0] = (emi_cona >> 28) & 3;
    chcd_row_mask[0] += bitmap_test(&emi_conh, 4) ? 4 : 0;
    chcd_row_mask[0] += 13;
    chcd_row_mask[1] = (emi_cona >> 30) & 3;
    chcd_row_mask[1] += bitmap_test(&emi_conh, 5) ? 4 : 0;
    chcd_row_mask[1] += 13;

    chab_col_mask[0] = (emi_cona >> 4) & 3;
    chab_col_mask[0] += 9;
    chab_col_mask[1] = (emi_cona >> 6) & 3;
    chab_col_mask[1] += 9;

    chcd_col_mask[0] = (emi_cona >> 20) & 3;
    chcd_col_mask[0] += 9;
    chcd_col_mask[1] = (emi_cona >> 22) & 3;
    chcd_col_mask[1] += 9;

    chn_4bank_mode = bitmap_test(&emi_conh_2nd, 6) ? 1 : 0;

    prepared = 1;
}

static unsigned int use_a2d_magic(unsigned long addr, unsigned int bit)
{
    unsigned long magic;
    unsigned int ret;

    magic = magics[((bit >= 9) & (bit <= 16)) ? (bit - 9) : 0];

    ret = bitmap_test(&addr, bit) ? 1 : 0;
    ret ^= (bitmap_test(&addr, 16) && bitmap_test(&magic, 0)) ? 1 : 0;
    ret ^= (bitmap_test(&addr, 17) && bitmap_test(&magic, 1)) ? 1 : 0;
    ret ^= (bitmap_test(&addr, 18) && bitmap_test(&magic, 2)) ? 1 : 0;
    ret ^= (bitmap_test(&addr, 19) && bitmap_test(&magic, 3)) ? 1 : 0;

    return ret;
}

int emi_addr2dram_v1(unsigned long addr, struct emi_addr_map *map)
{
    unsigned int tmp;
    unsigned long saddr, bfraddr, chnaddr;
    unsigned int max_rk0_sz;
    unsigned int row_mask, col_mask;
    bool ch_ab_not_cd;

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
    else
        addr -= offset;

    if ((addr >> 20) > max_mb)
        return -1;

    tmp = (bitmap_test(&addr, 8) & bitmap_test(&disph, 0)) ? 1 : 0;
    tmp ^= (bitmap_test(&addr, 9) & bitmap_test(&disph, 1)) ? 1 : 0;
    tmp ^= (bitmap_test(&addr, 10) & bitmap_test(&disph, 2)) ? 1 : 0;
    tmp ^= (bitmap_test(&addr, 11) & bitmap_test(&disph, 3)) ? 1 : 0;
    map->emi = tmp;

    saddr = addr;
    bitmap_clear(&saddr, 9);
    bitmap_clear(&saddr, 10);
    bitmap_clear(&saddr, 11);
    bitmap_clear(&saddr, 12);
    bitmap_clear(&saddr, 13);
    bitmap_clear(&saddr, 14);
    bitmap_clear(&saddr, 15);
    bitmap_clear(&saddr, 16);
    saddr |= use_a2d_magic(addr, 9) << 9;
    saddr |= use_a2d_magic(addr, 10) << 10;
    saddr |= use_a2d_magic(addr, 11) << 11;
    saddr |= use_a2d_magic(addr, 12) << 12;
    saddr |= use_a2d_magic(addr, 13) << 13;
    saddr |= use_a2d_magic(addr, 14) << 14;
    saddr |= use_a2d_magic(addr, 15) << 15;
    saddr |= use_a2d_magic(addr, 16) << 16;

    if (disph <= 0)
        bfraddr = saddr;
    else {
        tmp = 7 + __ffs(disph);
        bfraddr = (saddr >> (tmp + 1)) << tmp;
        bfraddr += saddr & ((1 << tmp) - 1);
    }

    if (bfraddr < cas)
        return -1;

    if (!channels)
        map->channel = channels;
    else if (hash) {
        tmp = (bitmap_test(&addr, 8) && bitmap_test(&hash, 0)) ? 1 : 0;
        tmp ^= (bitmap_test(&addr, 9) && bitmap_test(&hash, 1)) ? 1 : 0;
        tmp ^= (bitmap_test(&addr, 10) && bitmap_test(&hash, 2)) ? 1 : 0;
        tmp ^= (bitmap_test(&addr, 11) && bitmap_test(&hash, 3)) ? 1 : 0;
        map->channel = tmp;
    } else {
        if (channels == 1) {
            tmp = 0;
            switch (chnpos) {
            case 0:
                tmp = 7;
                break;
            case 1:
                tmp = 8;
                break;
            case 2:
                tmp = 9;
                break;
            case 3:
                tmp = 12;
                break;
            default:
                return -1;
            }
            map->channel = (bfraddr >> tmp) % 2;
        } else if (channels == 2) {
            tmp = 0;
            switch (chnpos) {
            case 0:
                tmp = 7;
                break;
            case 1:
                tmp = 8;
                break;
            case 2:
                tmp = 9;
                break;
            case 3:
                tmp = 12;
                break;
            default:
                return -1;
            }
            map->channel = (bfraddr >> tmp) % 4;
        } else
            return -1;
    }

    if (map->channel > 1)
        ch_ab_not_cd = 0;
    else {
        if (map->channel == 1)
            ch_ab_not_cd = (channels > 1) ? 1 : 0;
        else
            ch_ab_not_cd = 1;
    }

    max_rk0_sz = (ch_ab_not_cd) ? chab_rk0_sz : chcd_rk0_sz;
    max_rk0_sz = max_rk0_sz << 20;

    if (!channels)
        chnaddr = bfraddr;
    else if (chnpos > 3) {
        tmp = chn_hash_lsb;
        chnaddr = bfraddr >> (tmp + 1);
        chnaddr = chnaddr << tmp;
        chnaddr += bfraddr & ((1 << tmp) - 1);
    } else if (channels == 1 || channels == 2) {
        tmp = 0;
        switch (chnpos) {
        case 0:
            tmp = 7;
            break;
        case 1:
            tmp = 8;
            break;
        case 2:
            tmp = 9;
            break;
        case 3:
            tmp = 12;
            break;
        default:
            break;
        }

        chnaddr = bfraddr >> (tmp + (channels - 1));
        chnaddr = chnaddr << tmp;
        chnaddr += bfraddr & ((1 << tmp) - 1);
    } else
        return -1;

    if ((map->channel) ? !dualrk_ch1 : !dualrk_ch0)
        map->rank = 0;
    else {
        if (chnaddr > max_rk0_sz)
            map->rank = 1;
        else
            map->rank = 0;
    }

    row_mask = (ch_ab_not_cd) ?
            ((map->rank) ? chab_row_mask[1] : chab_row_mask[0]) :
            ((map->rank) ? chcd_row_mask[1] : chcd_row_mask[0]);
    col_mask = (ch_ab_not_cd) ?
            ((map->rank) ? chab_col_mask[1] : chab_col_mask[0]) :
            ((map->rank) ? chcd_col_mask[1] : chcd_col_mask[0]);

    tmp = chnaddr - (max_rk0_sz * map->rank);
    tmp /= 1 << (dw32 + 1 + col_mask + 3);
    tmp &= (1 << row_mask) - 1;
    map->row = tmp;

    tmp = chnaddr;
    tmp /= 1 << (dw32 + 1 + col_mask);
    tmp &= ((!chn_4bank_mode) ? 8 : 4) - 1;
    map->bank = tmp;

    tmp = chnaddr;
    tmp /= 1 << (dw32 + 1);
    tmp &= (1 << col_mask) - 1;
    map->column = tmp;

    return 0;
}
