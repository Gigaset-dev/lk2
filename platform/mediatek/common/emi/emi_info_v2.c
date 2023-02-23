/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <assert.h>
#include <debug.h>
#include <dramc_common_v2.h>
#include <lib/pl_boottags.h>
#include <libfdt.h>
#include <platform/addressmap.h>
#include <set_fdt.h>
#include <trace.h>

#include "emi_util.h"

static struct emi_info emi_info_arg;
static void *g_fdt;

#if (MPU_INFO_VER == 1)
static int set_fdt_emimpu_info(void *fdt)
{
    int ret;
    int offset;
    const unsigned long long dram_base = DRAM_BASE_PHY;
    unsigned int dram_start[2];
    unsigned int dram_end[2];
    unsigned long long dram_size;
    unsigned int i;

    offset = fdt_path_offset(fdt, "/soc/emimpu");
    if (offset < 0) {
        offset = fdt_path_offset(fdt, "/emimpu");
        if (offset < 0)
            ASSERT(0);
    }

    dram_start[0] = cpu_to_fdt32((unsigned int)(dram_base >> 32));
    dram_start[1] = cpu_to_fdt32((unsigned int)(dram_base & 0xFFFFFFFF));

    dram_size = 0;
    for (i = 0; i < emi_info_arg.rk_cnt; i++)
        dram_size += emi_info_arg.rk_size[i];

    dram_size += dram_base - 1;

    dram_end[0] = cpu_to_fdt32((unsigned int)(dram_size >> 32));
    dram_end[1] = cpu_to_fdt32((unsigned int)(dram_size & 0xFFFFFFFF));

    ret = fdt_setprop(fdt, offset, "dram_start",
            dram_start, sizeof(dram_start));
    if (ret < 0)
        ASSERT(0);

    ret = fdt_setprop(fdt, offset, "dram_end",
            dram_end, sizeof(dram_end));
    if (ret < 0)
        ASSERT(0);

    return ret;
}
#endif

static int set_fdt_emiisu_info(void *fdt)
{
    int ret;
    int offset;
    struct isu_info_t *isu_info = &(emi_info_arg.isu_info);
    unsigned int buf_size;
    unsigned int buf_addr[2];
    unsigned int ver_addr[2];
    unsigned int con_addr[2];

    offset = fdt_path_offset(fdt, "/soc/emiisu");
    if (offset < 0) {
        offset = fdt_path_offset(fdt, "/emiisu");
        if (offset < 0)
            ASSERT(0);
    }

    buf_size = cpu_to_fdt32(isu_info->buf_size);

    buf_addr[0] = cpu_to_fdt32(
            (unsigned int)(isu_info->buf_addr >> 32));
    buf_addr[1] = cpu_to_fdt32(
            (unsigned int)(isu_info->buf_addr & 0xFFFFFFFF));

    ver_addr[0] = cpu_to_fdt32(
            (unsigned int)(isu_info->ver_addr >> 32));
    ver_addr[1] = cpu_to_fdt32(
            (unsigned int)(isu_info->ver_addr & 0xFFFFFFFF));

    con_addr[0] = cpu_to_fdt32(
            (unsigned int)(isu_info->con_addr >> 32));
    con_addr[1] = cpu_to_fdt32(
            (unsigned int)(isu_info->con_addr & 0xFFFFFFFF));

    ret = fdt_setprop(fdt, offset, "buf_size",
            &buf_size, sizeof(buf_size));
    if (ret < 0)
        ASSERT(0);

    ret = fdt_setprop(fdt, offset, "buf_addr",
            buf_addr, sizeof(buf_addr));
    if (ret < 0)
        ASSERT(0);

    ret = fdt_setprop(fdt, offset, "ver_addr",
            ver_addr, sizeof(ver_addr));
    if (ret < 0)
        ASSERT(0);

    ret = fdt_setprop(fdt, offset, "con_addr",
            con_addr, sizeof(con_addr));
    if (ret < 0)
        ASSERT(0);

    return ret;
}

void *emi_get_fdt()
{
    return g_fdt;
}

int set_fdt_emi_info(void *fdt)
{
    int ret;
    int offset, array_len;
    unsigned int i;
    unsigned int ch_cnt;
    unsigned int rk_cnt;
    unsigned long long rank_size;
    unsigned int rk_size[DRAMC_MAX_RK][2];

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

    ch_cnt = cpu_to_fdt32(emi_info_arg.ch_cnt);
    rk_cnt = cpu_to_fdt32(emi_info_arg.rk_cnt);

    for (i = 0; i < DRAMC_MAX_RK; i++) {
        if (i < emi_info_arg.rk_cnt) {
            rank_size = emi_info_arg.rk_size[i];
            rk_size[i][0] = cpu_to_fdt32(
                    (unsigned int)(rank_size >> 32));
            rk_size[i][1] = cpu_to_fdt32(
                    (unsigned int)(rank_size & 0xFFFFFFFF));
        } else {
            rk_size[i][0] = cpu_to_fdt32(0);
            rk_size[i][1] = cpu_to_fdt32(0);
        }
    }

    /* pass parameter to kernel */
    ret = fdt_setprop(fdt, offset, "ch_cnt", &ch_cnt, sizeof(ch_cnt));
    if (ret < 0)
        ASSERT(0);

    ret = fdt_setprop(fdt, offset, "rk_cnt", &rk_cnt, sizeof(rk_cnt));
    if (ret < 0)
        ASSERT(0);

    ret = fdt_setprop(fdt, offset, "rk_size", rk_size, sizeof(rk_size));
    if (ret < 0)
        ASSERT(0);

#if (MPU_INFO_VER == 1)
    ret = set_fdt_emimpu_info(fdt);
    if (ret < 0)
        ASSERT(0);
#endif

    ret = set_fdt_emiisu_info(fdt);
    if (ret < 0)
        ASSERT(0);

    return ret;
}
SET_FDT_INIT_HOOK(set_fdt_emi_info, set_fdt_emi_info);
