/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <assert.h>
#include <debug.h>
#include <dramc_common_v1.h>
#include <lib/pl_boottags.h>
#include <libfdt.h>
#include <platform/addressmap.h>
#include <set_fdt.h>
#include <trace.h>

#include "emi_util.h"

static struct emi_info emi_info_arg;
static void *g_fdt;

void *emi_get_fdt()
{
    return g_fdt;
}

int set_fdt_emi_info(void *fdt)
{
    int ret;
    int offset, array_len;
    unsigned int i;
    unsigned int dram_type;
    unsigned int ch_num;
    unsigned int rk_num;
    unsigned int rank_size[MAX_RK];

    get_emi_info(&emi_info_arg);

    assert(emi_info_arg.magic_tail == 0x20190831);

    if (!fdt)
        ASSERT(0);

    array_len = countof(emicen_compatible);
    for (i = 0; i < array_len; i++) {
        offset = fdt_path_offset(fdt, emicen_compatible[i]);
        if (offset >= 0)
            break;
    }

    if (offset < 0)
        ASSERT(0);

    g_fdt = fdt;

    dram_type = cpu_to_fdt32(emi_info_arg.dram_type);
    ch_num = cpu_to_fdt32(emi_info_arg.ch_num);
    rk_num = cpu_to_fdt32(emi_info_arg.rk_num);

    /* unit of rank size: 1Gb (128MB) */
    for (i = 0; i < MAX_RK; i++) {
        if (i < rk_num) {
            rank_size[i] = (emi_info_arg.rank_size[i] >> 27) & 0xFFFFFFFF;
            rank_size[i] = cpu_to_fdt32(rank_size[i]);
        } else {
            rank_size[i] = cpu_to_fdt32(0);
        }
    }

    /* pass parameter to kernel */
    ret = fdt_setprop(fdt, offset, "emi_info,dram_type", &dram_type, sizeof(dram_type));
    if (ret < 0)
        ASSERT(0);

    ret = fdt_setprop(fdt, offset, "emi_info,ch_num", &ch_num, sizeof(ch_num));
    if (ret < 0)
        ASSERT(0);

    ret = fdt_setprop(fdt, offset, "emi_info,rk_num", &rk_num, sizeof(rk_num));
    if (ret < 0)
        ASSERT(0);

    ret = fdt_setprop(fdt, offset, "emi_info,rank_size", rank_size, sizeof(rank_size));
    if (ret < 0)
        ASSERT(0);

    return ret;
}
SET_FDT_INIT_HOOK(set_fdt_emi_info, set_fdt_emi_info);
