/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <debug.h>
#include <err.h>
#include <errno.h>
#include <lib/kcmdline.h>
#include <lk/init.h>
#include <platform/round.h>
#include <reg.h>
#include <stdio.h>
#include <ufs_cfg.h>
#include <ufs_platform.h>

#define CMDLINE_BUF_SIZE 128

/*
 * Setting kernel command line of boot device
 */
static bool appended;
void ufs_set_bootdevice(void)
{
    int ret = 0;
    char cmdline_buf[CMDLINE_BUF_SIZE];

    if (appended) {
        dprintf(ALWAYS, "%s: UFS already appends kcmdline. Ignored.\n", __func__);
        return;
    }

    ret = snprintf(cmdline_buf, CMDLINE_BUF_SIZE,
        "androidboot.boot_devices=bootdevice,soc/%08lx.ufshci,%08lx.ufshci",
            (unsigned long)((UFS_HCI_BASE & ~IO_BASE) | IO_BASE_PHY),
            (unsigned long)((UFS_HCI_BASE & ~IO_BASE) | IO_BASE_PHY));
    if (ret < 0) {
        dprintf(ALWAYS, "Partition append bootdevice to command line fail");
        return;
    }

    ret = kcmdline_append(cmdline_buf);
    if (ret != NO_ERROR) {
        dprintf(ALWAYS, "kcmdline append %s failed, rc=%d\n", cmdline_buf, ret);
        return;
    }
    dprintf(ALWAYS, "kcmdline appended: %s\n", cmdline_buf);

    appended = true;
    return;
}

void ufs_init_mphy(void)
{
#ifndef UFS_CFG_FPGA_PLATFORM
    unsigned int k, s, m;
    unsigned int k56, k16_2, k34, k14_2;
    unsigned int value;

    value = readl(UFS_MPHY_BASE + 0x1010);
    value &= ~(1 << 15);
    writel(value, UFS_MPHY_BASE + 0x1010);

    value = readl(UFS_MPHY_BASE + 0xB004);
    value &= ~(1 << 8);
    writel(value, UFS_MPHY_BASE + 0xB004);

    value = readl(UFS_MPHY_BASE + 0xB104);
    value &= ~(1 << 8);
    writel(value, UFS_MPHY_BASE + 0xB104);

    /* TX Setting, lane = 0 */
    value = readl(UFS_MPHY_BASE + 0x8030);
    m = (value >> 24) & 0x1F;
    s = (value >> 29) & 0x7;
    k = m + s + 2;

    k56 = DIV_ROUND_CLOSEST(k * 5, 6) & 0x1F;
    k16_2 = (DIV_ROUND_CLOSEST(k, 6) - 2) & 0x7;
    k34 = DIV_ROUND_CLOSEST(k * 3, 4) & 0x1F;
    k14_2 = (DIV_ROUND_CLOSEST(k, 4) - 2) & 0x7;

    value = (readl(UFS_MPHY_BASE + 0x8030) & 0xFFFF0000) |
        (k16_2 << 13) | (k56 << 8) | (k16_2 << 5) | k56;
    writel(value, UFS_MPHY_BASE + 0x8030);
    value = (k14_2 << 20) | (k14_2 << 16) | (k34 << 8) | k34;
    writel(value, UFS_MPHY_BASE + 0x8034);


    /* TX Setting, lane = 1 */
    value = readl(UFS_MPHY_BASE + 0x8130);
    m = (value >> 24) & 0x1F;
    s = (value >> 29) & 0x7;
    k = m + s + 2;

    k56 = DIV_ROUND_CLOSEST(k * 5, 6) & 0x1F;
    k16_2 = (DIV_ROUND_CLOSEST(k, 6) - 2) & 0x7;
    k34 = DIV_ROUND_CLOSEST(k * 3, 4) & 0x1F;
    k14_2 = (DIV_ROUND_CLOSEST(k, 4) - 2) & 0x7;

    value = (readl(UFS_MPHY_BASE + 0x8130) & 0xFFFF0000) |
        (k16_2 << 13) | (k56 << 8) | (k16_2 << 5) | k56;
    writel(value, UFS_MPHY_BASE + 0x8130);
    value = (k14_2 << 20) | (k14_2 << 16) | (k34 << 8) | k34;
    writel(value, UFS_MPHY_BASE + 0x8134);
#endif
}
