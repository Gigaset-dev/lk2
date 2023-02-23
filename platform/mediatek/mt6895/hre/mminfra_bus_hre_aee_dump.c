/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <debug.h>
#include <kernel/vm.h>
#include "mminfra_bus_hre_aee_dump.h"
#include <platform/mboot_expdb.h>
#include <reg.h>
#include <stdio.h>
#include <string.h>
#include <trace.h>

#define udelay(x)   spin(x)

#define LOCAL_TRACE 0
/*#define HRE_BRINGUP*/

static char *mminfra_bus_hre_dump_data(char *buf, int buf_size)
{
    char *buff_end = buf + buf_size;
    unsigned long i;

    for (i = MMINFRA_BUS_HRE_DUMP_START; i <= MMINFRA_BUS_HRE_DUMP_END; i += 4) {
        buf += snprintf(buf, buff_end - buf,
            "HRE regs(0x%x) = 0x%08x\n", (unsigned int)i, readl(i));
    }

    return buf;
}

static void save_last_mminfra_bus_hre_data(CALLBACK dev_write)
{
    unsigned long len;
    void *buf, *output;

    buf = malloc(MMINFRA_BUS_HRE_DATA_SZ);
    if (buf == NULL) {
        dprintf(CRITICAL, "[MMINFRA_BUS_HRE] can not alloc buffer\n");
        return;
    }

    output = mminfra_bus_hre_dump_data(buf, MMINFRA_BUS_HRE_DATA_SZ);
    len = output - buf;

    if (!dev_write(vaddr_to_paddr(buf), len))
        dprintf(CRITICAL, "[MMINFRA_BUS_HRE] AEE EXPDB dump fail\n");

    free(buf);
}

#ifndef HRE_BRINGUP
AEE_EXPDB_INIT_HOOK(SYS_LAST_MMINFRA_BUS_HRE_DATA, MMINFRA_BUS_HRE_DATA_SZ,
save_last_mminfra_bus_hre_data);
#endif

static void save_mminfra_bus_hre_sramdump(CALLBACK dev_write)
{
    unsigned long dump_pa, extend_pa;
    int size, dump_len;

    /* Wakeup MMInfra HRE SRAM */
    writel(readl(MM_HRE_SRAM_CON) | (0x1<<4), MM_HRE_SRAM_CON);
    udelay(2);
    writel(readl(MM_HRE_SRAM_CON) | (0x1<<1), MM_HRE_SRAM_CON);
    writel(readl(MM_HRE_SRAM_CON) & ~(0x1), MM_HRE_SRAM_CON);

    writel(0x1FF0A, MMINFRA_BUS_HRE_HRE_CLK_DCM_CTRL);
    writel(0x1, MMINFRA_BUS_HRE_DEBUG_EN);
    writel(0x1, MMINFRA_BUS_HRE_SRAM_BASE_EXTEND_EN);
    size = MMINFRA_BUS_HRE_SRAM_SZ;
    dump_len = MMINFRA_BUS_HRE_SRAM_DUMP_CYCLE_SZ;

    /* XXX_BASE is VA */
    dump_pa = vaddr_to_paddr((void *)MMINFRA_BUS_HRE_DUMP_BASE);
    extend_pa = vaddr_to_paddr((void *)MMINFRA_BUS_HRE_EXTEND_BASE);

    do {
        writel(extend_pa, MMINFRA_BUS_HRE_SRAM_BASE_EXTEND);

        if (!dev_write(dump_pa, dump_len)) {
            dprintf(CRITICAL, "[MMINFRA_BUS_HRE] AEE EXPDB SRAMdump fail\n");
            break;
        }
        extend_pa += MMINFRA_BUS_HRE_SRAM_DUMP_CYCLE_SZ;
        size = size - dump_len;
        if ((size - dump_len) < 0)
            dump_len = size;
    } while (size > 0);
}

#ifndef HRE_BRINGUP
AEE_EXPDB_INIT_HOOK(SYS_MMINFRA_BUS_HRE_SRAMDUMP, MMINFRA_BUS_HRE_SRAM_SZ,
save_mminfra_bus_hre_sramdump);
#endif
