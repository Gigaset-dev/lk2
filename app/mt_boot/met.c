/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <assert.h>
#include <debug.h>
#include <libfdt.h>
#include <mblock.h>
#include <platform_mtk.h>
#include <set_fdt.h>
#include <stdlib.h>
#include <trace.h>
#include <app/vboot_state.h>

#define LOCAL_TRACE 0

#define MET_DRAM_ADDR_MAX 0x9FFFFFFF

enum met_res_ram_type {
    mcupm,
    sspm,
    adsp,
    MET_RES_RAM_TYPE_NUM
};

struct met_res_ram_s {
    int node;
    unsigned long long ram_start_offset;
};

struct met_res_ram_s met_res_ram_list[MET_RES_RAM_TYPE_NUM];

static void met_set_fdt(void *fdt)
{
    int node, parent;
    int idx = -1, len = 0;
    unsigned int *fdtptr = NULL;
    unsigned int ram_size = 0, total_ram_size = 0;
    unsigned long long base_ram_start = 0;
    unsigned long long tmp = 0;
    uint32_t boot_state;

    /* not reserve met debug ram if user load with secure boot */
    if (LK_DEBUGLEVEL == 0) {
        boot_state = get_boot_state();

        if (boot_state != BOOT_STATE_ORANGE) {
            dprintf(CRITICAL, "met set_fdt skip if user load with boot state not orange\n");
            return;
        }
    }

    /* check met_res_ram parent node exist */
    parent = fdt_node_offset_by_compatible(fdt, -1, "mediatek,met_res_ram");
    if (parent < 0) {
        dprintf(CRITICAL, "met set_fdt skip if met_res_ram is not set\n");
        return;
    }

    fdt_for_each_subnode(node, fdt, parent) {
        /* check met_res_ram child node exist */
        if (node < 0) {
            dprintf(CRITICAL, "met set_fdt skip if something wrong with met_res_ram subnode\n");
            return;
        }

        LTRACEF("met check subnode %s\n", fdt_get_name(fdt, node, NULL));

        /* check reserved ram size */
        fdtptr = (unsigned int *)fdt_getprop(fdt, node, "size", &len);
        if (fdtptr == NULL) {
            dprintf(CRITICAL, "met set_fdt skip if res ram size is not set\n");
            continue;
        }

        ram_size = fdt32_to_cpu(*(unsigned int *)fdtptr);
        if (ram_size <= 0) {
            dprintf(CRITICAL, "met set_fdt skip if res ram size <= 0\n");
            continue;
        }

        /* store res ram info */
        idx++;
        if (idx >= MET_RES_RAM_TYPE_NUM) {
            dprintf(CRITICAL, "met set_fdt skip if met res ram type >= %d\n", MET_RES_RAM_TYPE_NUM);
            return;
        }
        met_res_ram_list[idx].node = node;
        met_res_ram_list[idx].ram_start_offset = total_ram_size;

        total_ram_size += ram_size;
    }

    if (total_ram_size == 0) {
        dprintf(CRITICAL, "met set_fdt skip if total res ram size is zero\n");
        return;
    }

    /* try to allocate memory */
    base_ram_start = mblock_alloc(total_ram_size, PAGE_SIZE,
                                MET_DRAM_ADDR_MAX, 0, 0, "debug_met_res-ram");

    if (base_ram_start == 0) {
        dprintf(CRITICAL, "met_res_ram allocation failed\n");
        return;
    }

    /* write back res addr to dts */
    for (; idx >= 0; idx--) {
        tmp = cpu_to_fdt64(base_ram_start + met_res_ram_list[idx].ram_start_offset);
        if (fdt_setprop(fdt, met_res_ram_list[idx].node, "start", &tmp, sizeof(tmp)) < 0) {
            dprintf(CRITICAL, "met set start addr fails\n");
            assert(0);
        }
    }
}
SET_FDT_INIT_HOOK(met_set_fdt, met_set_fdt);
