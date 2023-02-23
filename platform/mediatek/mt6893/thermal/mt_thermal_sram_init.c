/*
 *  Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */


#include <debug.h>
#include "init_mtk.h"
#include <kernel/vm.h>
#include "lk/init.h"
#include <platform/addressmap.h>
#include <platform/mboot_expdb.h>
#include <reg.h>
#include <string.h>
#include <stdlib.h>


#define THERM_LOG(fmt, args...)  dprintf(CRITICAL, "[Thermal]" fmt, ##args)


/* SRAM for Thermal */
#define THERMAL_SRAM_BASE  (SRAM_BASE + 0x0000F000)
#define THERMAL_SRAM_ADDR1 THERMAL_SRAM_BASE //0x0010F000
#define THERMAL_SRAM_LEN1  0x160

#define THERMAL_SRAM_ADDR2 (THERMAL_SRAM_BASE + 0x16C) //0x0010F16C
#define THERMAL_SRAM_LEN2  0x294

#define THERMAL_SRAM_LEN   0x400
#define THERMAL_BUF_LENGTH 0x1000


void thermal_cls_sram(uint level)
{
    unsigned int len1, len2;

    len1 = (unsigned int)THERMAL_SRAM_LEN1;
    len2 = (unsigned int)THERMAL_SRAM_LEN2;

    THERM_LOG("%s\n", __func__);

    /* To skip ATC DATA */
    memset((void *)THERMAL_SRAM_ADDR1, 0, len1);
    memset((void *)THERMAL_SRAM_ADDR2, 0, len2);

}

MT_LK_INIT_HOOK_BL2_EXT(thermal_cls_sram, thermal_cls_sram,
        LK_INIT_LEVEL_PLATFORM);

static void save_thermal_log(CALLBACK dev_write)
{
    char *buf = NULL;
    unsigned long len;

    int i = 0;
    unsigned int val = 0;

    len = 0;
    buf = (char *) malloc(THERMAL_BUF_LENGTH);
    if (buf == NULL)
        return;

    for (i = 0; i < (THERMAL_SRAM_LEN)/sizeof(int); i++) {
        val = readl(THERMAL_SRAM_BASE + i * sizeof(int));
        len += snprintf(buf + len, THERMAL_BUF_LENGTH - len, "%d:%d "
            , i+1, val);
        if (i%4 == 3)
            len += snprintf(buf + len, THERMAL_BUF_LENGTH - len, "\n");
    }

    if (!dev_write(vaddr_to_paddr(buf), len))
        THERM_LOG("thermal log dump fail\n");

    free(buf);
}

AEE_EXPDB_INIT_HOOK(SYS_THERMAL_LOG, THERMAL_BUF_LENGTH, save_thermal_log);

