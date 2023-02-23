/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <debug.h>
#include <errno.h>
#include "lastpc.h"
#include <malloc.h>
#include <platform/dfd_mcu.h>
#include <platform/plat_lastpc.h>
#include <reg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

unsigned long plt_get_cpu_power_status_at_wdt(void)
{
    unsigned long bitmask = 0xff, ret;

    ret = readl(SPM_PWR_STS_ADDR);
    bitmask = (ret & SPM_PWR_STS_MASK) >> SPM_PWR_STS_OFFSET;

    return bitmask;
}

int default_lastpc_dump(const struct plt_cfg_pc_latch *self, char *buf, int *wp)
{
    unsigned int i;
    unsigned long long pc_value_h, fp_value_h, sp_value_h;
    unsigned long long pc_value, fp_value, sp_value;
    unsigned long core_dbg_sel, core_dbg_mon;
    unsigned long cpu_power_status = 0;

    int is_64bit_kernel = 1;

    /* mcusys registers would be corrupted by DFD */
    if (dfd_internal_dump_before_reboot()) {
        *wp += dfd_internal_dump_get_decoded_lastpc(buf + *wp, LASTPC_BUF_LENGTH - *wp);
        return 1;
    }

    /* get the power status information */
    cpu_power_status = plt_get_cpu_power_status_at_wdt();

    for (i = 0; i < self->nr_max_core; ++i) {
        /* if CPUX is not powered on before reboot --> skip */
        if (extract_n2mbits(cpu_power_status, i, i) == 0) {
            *wp += snprintf(buf + *wp, LASTPC_BUF_LENGTH - *wp,
                       "[LAST PC] CORE_%d PC = 0x0 (core pwr off), FP = 0x0, SP = 0x0\n", i);
            continue;
        }

        core_dbg_sel = self->core_dbg_sel[i];
        core_dbg_mon = self->core_dbg_mon[i];
        if ((core_dbg_sel == 0) || (core_dbg_mon == 0))
            continue;

        if (i < (self->nr_max_core - self->nr_max_big_core)) { /* for little core */
            writel(LCORE_PC_H, core_dbg_sel);
            pc_value_h = readl(core_dbg_mon);
            writel(LCORE_PC_L, core_dbg_sel);
            pc_value = (pc_value_h << 32) | readl(core_dbg_mon);
        } else { /* for big core */
            writel(BCORE_PC_H, core_dbg_sel);
            pc_value_h = readl(core_dbg_mon);
            writel(BCORE_PC_L, core_dbg_sel);
            pc_value = (pc_value_h << 32) | readl(core_dbg_mon);
        }

        if (self->dump_pc_only) {
            *wp += snprintf(buf + *wp, LASTPC_BUF_LENGTH - *wp,
                       "[LAST PC] CORE_%d PC = 0x%016llx\n", i, pc_value);
            continue;
        }

        /* get the 64bit/32bit kernel information from bootopt */
        if (is_64bit_kernel) {
            if (i < (self->nr_max_core - self->nr_max_big_core)) {
                writel(LCORE_LR_H, core_dbg_sel);
                fp_value_h = readl(core_dbg_mon);
                writel(LCORE_LR_L, core_dbg_sel);
                fp_value = (fp_value_h << 32) | readl(core_dbg_mon);

                writel(LCORE_SP_H, core_dbg_sel);
                sp_value_h = readl(core_dbg_mon);
                writel(LCORE_SP_L, core_dbg_sel);
                sp_value = (sp_value_h << 32) | readl(core_dbg_mon);
            } else {
#if 0 //what value for core_dbg_sel to select FP and SP?
                writel(BCORE_FP_H, core_dbg_sel);
                fp_value_h = readl(core_dbg_mon);
                writel(BCORE_FP_L, core_dbg_sel);
                fp_value = (fp_value_h << 32) | readl(core_dbg_mon);

                writel(BCORE_SP_H, core_dbg_sel);
                sp_value_h = readl(core_dbg_mon);
                writel(BCORE_SP_L, core_dbg_sel);
                sp_value = (sp_value_h << 32) | readl(core_dbg_mon);
#else
                fp_value = 0;
                sp_value = 0;
#endif
            }
            *wp += snprintf(buf + *wp, LASTPC_BUF_LENGTH - *wp,
                       "[LAST PC] CORE_%d PC = 0x%016llx, FP = 0x%016llx, SP = 0x%016llx\n",
                       i, pc_value, fp_value, sp_value);
        } else {
            if (i < (self->nr_max_core - self->nr_max_big_core)) {
                writel(LCORE_LR_L, core_dbg_sel);
                fp_value = readl(core_dbg_mon);

                writel(LCORE_SP_L, core_dbg_sel);
                sp_value = readl(core_dbg_mon);
            } else {
#if 0
                writel(BCORE_FP_L, core_dbg_sel);
                fp_value = readl(core_dbg_mon);

                writel(BCORE_SP_L, core_dbg_sel);
                sp_value = readl(core_dbg_mon);
#else
                fp_value = 0;
                sp_value = 0;
#endif
            }
            *wp += snprintf(buf + *wp, LASTPC_BUF_LENGTH - *wp,
                       "[LAST PC] CORE_%d PC = 0x%016llx, FP = 0x%08llx, SP = 0x%08llx\n",
                       i, pc_value, fp_value, sp_value);
        }
    }

    *wp += snprintf(buf + *wp, LASTPC_BUF_LENGTH - *wp, "\n");
    return 1;
}

static int lastpc_dump(char *buf, int *wp)
{
    if (buf == NULL || wp == NULL)
        return -1;

    *wp += snprintf(buf + *wp, LASTPC_BUF_LENGTH - *wp,
               "\n*************************** lastpc ***************************\n");

    if (cfg_pc_latch.dump)
        cfg_pc_latch.dump(&cfg_pc_latch, buf, wp);
    else
        default_lastpc_dump(&cfg_pc_latch, buf, wp);

    return 1;
}

int lastpc_get(void **data, int *len)
{
    int ret;
    *len = 0;
    *data = malloc(LASTPC_BUF_LENGTH);
    if (*data == NULL)
        return 0;

    ret = lastpc_dump(*data, len);
    if (ret < 0 || *len > LASTPC_BUF_LENGTH) {
        *len = (*len > LASTPC_BUF_LENGTH) ? LASTPC_BUF_LENGTH : *len;
        return ret;
    }

    return 1;
}

void lastpc_put(void **data)
{
    free(*data);
}
