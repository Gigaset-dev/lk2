/*
 *  Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <arch/ops.h>
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
#define THERMAL_SRAM_BASE  (SRAM_BASE + 0x00014000)
#define THERMAL_SRAM_LEN   0x400
#define THERMAL_BUF_LENGTH 0x1000
#define THERMAL_ATC_SRAM_BASE   (THERMAL_SRAM_BASE + 0x280)
#define THERMAL_ATC_SRAM_LEN    (17 * 4)



void thermal_cls_sram(uint level)
{
    int i = 0;
    unsigned int pattern = 0x27bc86aa;

//  THERM_LOG("%s, %p\n", __func__, SRAM_BASE);

    for (i = 0; i < THERMAL_SRAM_LEN ; i = i + 4)
        memcpy((void *)THERMAL_SRAM_BASE + i, &pattern, 4);

    arch_clean_invalidate_cache_range((addr_t)THERMAL_ATC_SRAM_BASE, THERMAL_ATC_SRAM_LEN);

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

    for (i = 0; i < (int)(THERMAL_SRAM_LEN/sizeof(int)); i++) {
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

