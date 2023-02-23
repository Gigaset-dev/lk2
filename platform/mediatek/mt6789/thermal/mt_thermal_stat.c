/*
 *  Copyright (c) 2022 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <arch/ops.h>
#include <debug.h>
#include <init_mtk.h>
#include <kernel/vm.h>
#include <platform/addressmap.h>
#include <platform/mboot_expdb.h>
#include <reg.h>
#include <string.h>
#include <stdlib.h>


#define THERM_STAT_LOG(fmt, args...)  dprintf(CRITICAL, "[ThermalStat]" fmt, ##args)


/* SRAM for Thermal */
#define THERMAL_STAT_SRAM_BASE  (SRAM_BASE + 0x00011000) //0x111000
#define THERMAL_STAT_SRAM_LEN   0x400
#define THERMAL_STAT_BUF_LENGTH 0x1000


void thermal_stat_cls_sram(uint level)
{
    int i = 0;
    unsigned int pattern = 0xffffffff;

    for (i = 0; i < THERMAL_STAT_SRAM_LEN ; i = i + 4)
        memcpy((void *)THERMAL_STAT_SRAM_BASE + i, &pattern, 4);

}

MT_LK_INIT_HOOK_BL2_EXT(thermal_stat_cls_sram, thermal_stat_cls_sram,
        LK_INIT_LEVEL_PLATFORM);

static void save_thermal_stat_log(CALLBACK dev_write)
{
    char *buf = NULL;
    unsigned long len;

    int i = 0;
    unsigned int val = 0;

    len = 0;
    buf = (char *) malloc(THERMAL_STAT_BUF_LENGTH);
    if (buf == NULL)
        return;

    for (i = 0; i < (int)(THERMAL_STAT_SRAM_LEN/sizeof(int)); i++) {
        val = readl(THERMAL_STAT_SRAM_BASE + i * sizeof(int));
        len += snprintf(buf + len, THERMAL_STAT_BUF_LENGTH - len, "%x ", val);
        if (i%12 == 0)
            len += snprintf(buf + len, THERMAL_STAT_BUF_LENGTH - len, "\n");
    }

    if (!dev_write(vaddr_to_paddr(buf), len))
        THERM_STAT_LOG("thermal log dump fail\n");

    free(buf);
}

AEE_EXPDB_INIT_HOOK(SYS_THERMAL_STAT_RAW, THERMAL_STAT_BUF_LENGTH, save_thermal_stat_log);
