/*
 * Copyright (c) 2022 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <debug.h>
#include <emi_mpu.h>
#include <lib/kcmdline.h>
#include <lk/init.h>
#include <mblock.h>
#include <sys/types.h>
#include <sysenv.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define MTK_DM_OTA_PAGE (16)
#define MTK_DM_OTA_PAGE_SIZE (4096 * PAGE_SIZE)
#define MTK_DM_OTA_PATTTEN "dm_ota"
/*
 * reserved memory and set portect for speical usage in kernel
 */
static void save_dm_ota_reserved_mem(uint level)
{
    struct emi_region_info_t region_info;
    u64 ota_base_addr_pa = 0;

    // default close reserved memory for OTA while first bootup
    if (!get_env(MTK_DM_OTA_PATTTEN))
        set_env(MTK_DM_OTA_PATTTEN, "0");

    if (!strncmp(get_env(MTK_DM_OTA_PATTTEN), "1", sizeof("1"))) {
        kcmdline_append("mtk_kcopyd_quirk=mediatek,dm_ota");

        // reserved mem for ota dm_kcopyd
        ota_base_addr_pa = mblock_alloc(MTK_DM_OTA_PAGE_SIZE, PAGE_SIZE, MBLOCK_NO_LIMIT,
            0, MBLOCK_MAP, MTK_DM_OTA_PATTTEN);
        if (!ota_base_addr_pa) {
            dprintf(CRITICAL, "[DM_OTA] mblock allocation failed\n");
            return;
        }

        /* use emi mpu to protect dm_kcopyd in OTA usage */
        dprintf(CRITICAL, "[DM_OTA] addr=0x%x size=0x%x\n",
            ota_base_addr_pa, (unsigned long)MTK_DM_OTA_PAGE_SIZE);
        region_info.start = (unsigned long long)ota_base_addr_pa;
        region_info.end = (unsigned long long)(ota_base_addr_pa + MTK_DM_OTA_PAGE_SIZE);
        region_info.region = MTK_DM_OTA_PAGE;
        emi_mpu_set_protection(&region_info);
    } else {
        dprintf(CRITICAL, "[DM_OTA] do not reserved\n");
    }
}

LK_INIT_HOOK(init_dm_ota, &save_dm_ota_reserved_mem, LK_INIT_LEVEL_APPS - 1);
