/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <chip_id.h>
#include <debug.h>
#include "plat_mtk_dvfs.h"
#include <platform/sec_devinfo.h>

#define DEVINFO_CPU_LEVEL_IDX  (7) /* Efuse Segment 0x11EE0050[7:0] */
#define DEVINFO_CPU_LEVEL_MASK (0xff)

#define DEVINFO_EFUSE_PTPOD14  (64) /* Efuse Segment 0x11EE05B8[31:0]*/

unsigned int get_cpu_level(void)
{
    enum dvfs_table_mapping cpu_level = PRITABLE_3050M;
    u32 ver = get_chip_sw_ver();
    u32 segment = get_devinfo_with_index(DEVINFO_CPU_LEVEL_IDX) & DEVINFO_CPU_LEVEL_MASK;
    u32 ptp14 = get_devinfo_with_index(DEVINFO_EFUSE_PTPOD14);

    if (ver >= 0x0001) {
        /* only for E2 chips */
        switch (segment) {
        case 0x2: /* MT6985W */
        case 0x4: /* MT6983Z/T */
            cpu_level = PRITABLE_3200M;
            break;
        case 0x3: /* MT6983W */
            if (ptp14 == 0)
                cpu_level = PRITABLE_3200M;
            else
                cpu_level = PRITABLE_3350M;
            break;
        default:
            cpu_level = PRITABLE_3050M;
        }
    }

    /* These are settings for flavor project */
    if (CFG_CPU_DVFS_LEVEL == 1)
        cpu_level = PRITABLE_3200M;

    dprintf(ALWAYS, "[%s]cpu_level=%d, ver=%u, seg=%u, ptp14=%u\n", __func__,
            cpu_level, ver, segment, ptp14);

    return cpu_level;
}
