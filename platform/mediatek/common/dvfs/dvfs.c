/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <arch/ops.h>
#include <debug.h>
#include <libfdt.h>
#include <mtk_charger.h>
#include <mtk_dvfs.h>
#include <platform/mboot_expdb.h>
#include <platform/reg.h>
#include <sys/types.h>

#define BIT_LOW_BATTERY 12

static void save_hvfs_data(CALLBACK dev_write)
{
    paddr_t buf;
    unsigned long len;

    buf = (paddr_t)(CSRAM_BASE_PHY);
    len = CSRAM_SIZE;

    if (!dev_write((uint64_t)buf, len))
        dprintf(CRITICAL, "hvfs dump fail\n");
}
AEE_EXPDB_INIT_HOOK(SYS_CPUHVFS_RAW, 0x3000, save_hvfs_data);

/* in case, mtk_charger is not yet ready */
__WEAK bool is_battery_voltage_low(void)
{
    return false;
}

__WEAK unsigned int get_cpu_level(void)
{
    return 0;
}

void dvfs_init(void *fdt)
{
    int offset = 0;
    int val = 0;
    unsigned int lv;
    int len;
    char *status = NULL;

    if (fdt != NULL) {
        offset = fdt_node_offset_by_compatible(
                fdt, -1, "mediatek,cpufreq-hw");
        if (offset >= 0) {
            status = (char *)fdt_getprop(fdt, offset, "dvfs-config", &len);
            if (!status)
                val = 0x01;
            else if (strncmp(status, "dvfs-disable", len) == 0)
                val = 0x10;
            else if (strncmp(status, "aging", len) == 0)
                val = 0x41;
            else if (strncmp(status, "mcl50", len) == 0)
                val = 0x21;
            else if (strncmp(status, "eem-disable", len) == 0)
                val = 0x11;
            else
                val = 0x01;
        } else{
            dprintf(CRITICAL, "[%s]dvfs:not found dvfs node!,disable dvfs anyway\n",
                __func__);
        }
    } else {
        dprintf(CRITICAL, "[%s]dvfs:no device tree!,disable dvfs anyway\n",
                __func__);
    }

    /*
     * When is_battery_voltage_low() returns true, it means the battery
     * voltage is too low to enable dvfs at high frequency safely.
     */
    if (is_battery_voltage_low())
        val |= (0x1 << BIT_LOW_BATTERY);

    lv = get_cpu_level();

    val = (lv << 8) + val;

    write32((CSRAM_BASE + 0x8), 0x55aa55aa);
    write32((CSRAM_BASE + 0xC), val);
    arch_clean_invalidate_cache_range((addr_t)CSRAM_BASE, CACHE_LINE);

    return;
}

