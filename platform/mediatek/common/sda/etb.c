/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#if ARCH_ARM
#include <arch/arm.h>
#endif
#if ARCH_ARM64
#include <arch/arm64.h>
#endif

#include <debug.h>
#include <malloc.h>
//#include <plat_sram_flag.h>
#include <platform/addressmap.h>
#include <platform/etb.h>
#include <platform/plat_debug.h>
#include <platform/sec_devinfo.h>
#include <reg.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "utils.h"

//static unsigned int etb_users[MAX_NR_ETB];

#ifndef TRACER_ETB_BUF_LENGTH

#define TRACER_ETB_BUF_LENGTH   (0x0)
#define DBG_BASE                (0x0)

#endif

const struct plt_cfg_etb cfg_etb = {
    .nr_etb = 1,
    .total_etb_sz = TRACER_ETB_BUF_LENGTH,
    .dbgao_base = (DBG_BASE+0x0001a000),
    .dem_base = (DBG_BASE+0x00040000),
    .etb = {
        {(DBG_BASE+0x00044000), 0},
    },
};

static void get_etb_users(void)
{
#if 0
    unsigned int i, offset = 0;
    unsigned long plat_sram_flag0;

    /* get the user id from plat_sram_flag0 */
    if (cfg_pc_latch.plat_sram_flag0) {
        plat_sram_flag0 = readl(cfg_pc_latch.plat_sram_flag0);
        for (i = 0; i <= cfg_etb.nr_etb-1; ++i) {
            if (cfg_etb.etb[i].support_multi_user == 1 && offset <= 9) {
                /* plat_sram_flag0 is for the first 10 ETBs that supports multi-user */
                etb_users[i] = extract_n2mbits(plat_sram_flag0, 2+offset*3, 4+offset*3);
                offset++;
            } else {
                /* user id 7 stands for "reserved" */
                etb_users[i] = 7;
            }
        }
    }
#endif
}

static unsigned int unlock_etb(unsigned long base)
{
    if (base == 0)
        return 0;

    writel(ETB_UNLOCK_MAGIC, base + ETB_LAR);
    DSB;

    if (readl(base + ETB_LSR) != 0x1)
        return 0;

    return 1;
}

static unsigned int lock_etb(unsigned long base)
{
    if (base == 0)
        return 0;

    DSB;
    writel(0, base + ETB_LAR);

    return 1;
}

static int copy_from_etb(unsigned int index, char *buf, int *wp, unsigned int max_buf_size)
{
    unsigned long depth, etb_rp, etb_wp, ret, base;
    unsigned long nr_words, nr_unaligned_bytes;
    unsigned int i;
    //DEF_PLAT_SRAM_FLAG* plat = NULL;

    if (buf == NULL || wp == NULL || (index+1 > cfg_etb.nr_etb))
        return -1;

    base = cfg_etb.etb[index].base;

    if (base == 0)
        return -1;

    if (unlock_etb(base) != 1) {
        dprintf(CRITICAL, "[ETB%d] unlock etb failed\n", index);
        return 0;
    }

    ret = readl(base + ETB_STATUS);
    etb_rp = readl(base + ETB_READADDR);
    etb_wp = readl(base + ETB_WRITEADDR);

    /* depth is counted in byte */
    if (ret & 0x1)
        depth = readl(base + ETB_DEPTH) << 2;
    else
        depth = CIRC_CNT(etb_wp, etb_rp, readl(base + ETB_DEPTH) << 2);

    dprintf(INFO, "[ETB%d] depth = 0x%lx bytes (etb status = 0x%lx, rp = 0x%lx, wp = 0x%lx)\n",
            index, depth, ret, etb_rp, etb_wp);

    if (depth == 0)
        return 0;

    *wp += snprintf(buf + *wp, max_buf_size - *wp,
                    "\n******************* ETB%d @0x%08lx ********************\n",
                    index, base);

    /* disable ETB before dump */
    writel(0x0, base + ETB_CTRL);

    nr_words = depth / 4;
    nr_unaligned_bytes = depth % 4;
    for (i = 0; i < nr_words; ++i)
        *wp += snprintf(buf + *wp, max_buf_size - *wp, "%08lx\n",
                        (unsigned long) readl(base + ETB_READMEM));

    if (nr_unaligned_bytes != 0)
        /* ETB depth is not word-aligned -> not expected */
        *wp += snprintf(buf + *wp, max_buf_size - *wp,
                        "[warning] etb depth is not word-aligned!\n");

    *wp += snprintf(buf + *wp, max_buf_size - *wp,
                    "\n\ndepth = 0x%lx bytes (etb status = 0x%lx, rp = 0x%lx, wp = 0x%lx)\n",
                    depth, ret, etb_rp, etb_wp);

    writel(0x0, base + ETB_TRIGGERCOUNT);
    writel(0x0, base + ETB_READADDR);
    writel(0x0, base + ETB_WRITEADDR);
    DSB;

    if (lock_etb(base) != 1)
        dprintf(INFO, "lock etb failed -> ignore due to no impact to system\n");
#if 0
    if (!get_dbg_info_base)
        dprintf(CRITICAL, "[etb] get_dbg_info_base is NULL\n");

    plat = (DEF_PLAT_SRAM_FLAG *)get_dbg_info_base(PLAT_SRAM_FLAG_KEY);

    if (!plat) {
        dprintf(CRITICAL, "[etb] get_dbg_info_base(PLAT_SRAM_FLAG_KEY) is NULL\n");
        return 0;
    }
#endif
    base = cfg_etb.dbgao_base;
#if 0
    *wp += snprintf(buf + *wp, max_buf_size - *wp,
                    "\nstatus1 = 0x%x, status2 = 0x%x, time_h = 0x%x, time_l = 0x%x, stage = 0x%x,
                    kernel_start_time = 0x%x\n",
                    readl(base+0x88), readl(base+0x8c), readl(base+0x94), readl(base+0x90),
                    readl(base+0x98), plat->plat_sram_flag0);
#else
    *wp += snprintf(buf + *wp, max_buf_size - *wp,
                  "\nstatus1 = 0x%x, status2 = 0x%x, time_h = 0x%x, time_l = 0x%x, stage = 0x%x\n",
                    readl(base+0x88), readl(base+0x8c), readl(base+0x94),
                    readl(base+0x90), readl(base+0x98));
#endif
    *wp += snprintf(buf + *wp, max_buf_size - *wp, "\n");
    return 1;
}

int etb_get(void **data, int *len)
{
    int ret;
    /*
     * every 4-byte word would be output in 9 characters (e.g. "ffffabcd\n")
     * -> etb_sz * 3 is sufficient for all the etb content
     */
    unsigned long total_sz_to_dump = cfg_etb.total_etb_sz * 3;

    if (len == NULL || data == NULL)
        return -1;

    *len = 0;
    *data = NULL;
    if (cfg_etb.total_etb_sz == 0 || cfg_etb.nr_etb == 0 || cfg_etb.dem_base == 0)
        return 0;

    *data = malloc(total_sz_to_dump);
    if (*data == NULL)
        return 0;

    /* enable ATB clock */
    writel(0x1, cfg_etb.dbgao_base + DEM_ATB_CLK);

    /* enable ATB clock */
    writel(0x1, cfg_etb.dem_base + DEM_ATB_CLK);

    /* get the information of ETB users from sram flag */
    //get_etb_users();

    ret = copy_from_etb(0, *data, len, total_sz_to_dump);
    if (ret < 0 || (*len > 0 && ((unsigned long)*len > total_sz_to_dump))) {
        if (*len > 0 && ((unsigned long)*len > total_sz_to_dump))
            *len = total_sz_to_dump;
        return ret;
    }

    return 1;
}

void etb_put(void **data)
{
    free(*data);
}
