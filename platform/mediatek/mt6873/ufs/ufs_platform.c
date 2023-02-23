/*
 * Copyright (c) 2020 MediaTek Inc.
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

void ufs_init_mphy(void)
{
    unsigned int efuse_v;
    unsigned int k, s, m;
    unsigned int k56, k16_2, k34, k14_2;
    unsigned int value;

    /* Check & Apply efuse patch */
    efuse_v = readl(EFUSE_VERSION);

    s = (efuse_v >> 31) & 0x1;
    m = efuse_v & 0xFF;

    if (s && m >= 0x5 && m <= 0x7) {
        writel((readl(UFS_MPHY_MP_GLB_DIG_58) & ~0xF0) | (0x3 << 4),
            UFS_MPHY_MP_GLB_DIG_58);
        writel((readl(UFS_MPHY_MP_LN_DIG_TX_A4_1) & ~0x7) | 0x4,
            UFS_MPHY_MP_LN_DIG_TX_A4_1);
        writel((readl(UFS_MPHY_MP_LN_DIG_RX_08_1) & ~0xC),
            UFS_MPHY_MP_LN_DIG_RX_08_1);
        writel((readl(UFS_MPHY_MP_LN_DIG_RX_08_1) & ~0x4000),
            UFS_MPHY_MP_LN_DIG_RX_08_1);
        writel((readl(UFS_MPHY_MP_LN_DIG_RX_1C_1) & ~0x300000) | (0x2 << 20),
            UFS_MPHY_MP_LN_DIG_RX_1C_1);
        writel((readl(UFS_MPHY_MP_LN_DIG_RX_20_1) & ~0xC0000000) | (0x2 << 30),
            UFS_MPHY_MP_LN_DIG_RX_20_1);
        writel((readl(UFS_MPHY_MP_LN_DIG_RX_80_1) & ~0x1FF0000) | (0x102 << 16),
            UFS_MPHY_MP_LN_DIG_RX_80_1);
        writel((readl(UFS_MPHY_MP_LN_DIG_TX_04_1) & ~0x1000),
            UFS_MPHY_MP_LN_DIG_TX_04_1);
        writel((readl(UFS_MPHY_MP_LN_DIG_RX_48_1) & ~0xE00) | (0x4 << 9),
            UFS_MPHY_MP_LN_DIG_RX_48_1);
        writel((readl(UFS_MPHY_MP_LN_DIG_TX_04_1) & ~0x10),
            UFS_MPHY_MP_LN_DIG_TX_04_1);
    }

    /* MP_GLB_ANA_04 RG_MP_LV_SEL[2:0] = 0x3 */
    value = readl(UFS_MPHY_MP_GLB_ANA_04);
    value = (value & ~(0x7 << 20)) | (0x3 << 20);
    writel(value, UFS_MPHY_MP_GLB_ANA_04);

    /* TX Setting, lane = 0 */
    value = readl(UFS_MPHY_MP_LN_DIG_TX_30);
    m = (value >> 24) & 0x1F;
    s = (value >> 29) & 0x7;
    k = m + s + 2;

    k56 = DIV_ROUND_CLOSEST(k * 5, 6) & 0x1F;
    k16_2 = (DIV_ROUND_CLOSEST(k, 6) - 2) & 0x7;
    k34 = DIV_ROUND_CLOSEST(k * 3, 4) & 0x1F;
    k14_2 = (DIV_ROUND_CLOSEST(k, 4) - 2) & 0x7;

    value = (readl(UFS_MPHY_MP_LN_DIG_TX_30) & 0xFFFF0000) |
        (k16_2 << 13) | (k56 << 8) | (k16_2 << 5) | k56;
    writel(value, UFS_MPHY_MP_LN_DIG_TX_30);
    value = (k14_2 << 20) | (k14_2 << 16) | (k34 << 8) | k34;
    writel(value, UFS_MPHY_MP_LN_DIG_TX_34);


    /* TX Setting, lane = 1 */
    value = readl(UFS_MPHY_MP_LN_DIG_TX_30_1);
    m = (value >> 24) & 0x1F;
    s = (value >> 29) & 0x7;
    k = m + s + 2;

    k56 = DIV_ROUND_CLOSEST(k * 5, 6) & 0x1F;
    k16_2 = (DIV_ROUND_CLOSEST(k, 6) - 2) & 0x7;
    k34 = DIV_ROUND_CLOSEST(k * 3, 4) & 0x1F;
    k14_2 = (DIV_ROUND_CLOSEST(k, 4) - 2) & 0x7;

    value = (readl(UFS_MPHY_MP_LN_DIG_TX_30_1) & 0xFFFF0000) |
        (k16_2 << 13) | (k56 << 8) | (k16_2 << 5) | k56;
    writel(value, UFS_MPHY_MP_LN_DIG_TX_30_1);
    value = (k14_2 << 20) | (k14_2 << 16) | (k34 << 8) | k34;
    writel(value, UFS_MPHY_MP_LN_DIG_TX_34_1);

}
/*
 * Setting kernel command line of boot device
 */
static void ufs_commandline_bootdevice(uint level)
{
    int ret = 0;
    char cmdline_buf[CMDLINE_BUF_SIZE];

    ret = snprintf(cmdline_buf, CMDLINE_BUF_SIZE,
        "androidboot.boot_devices=bootdevice,soc/%08lx.ufshci,%08lx.ufshci",
            (UFS_HCI_BASE & ~IO_BASE) | IO_BASE_PHY, (UFS_HCI_BASE & ~IO_BASE) | IO_BASE_PHY);
    if (ret < 0) {
        dprintf(ALWAYS, "Partition append bootdevice to command line fail");
        return;
    }

    ret = kcmdline_append(cmdline_buf);
    if (ret != NO_ERROR) {
        dprintf(ALWAYS, "kcmdline append %s failed, rc=%d\n", cmdline_buf, ret);
        return;
    }
    return;
}

LK_INIT_HOOK(init_ufs, &ufs_commandline_bootdevice, LK_INIT_LEVEL_APPS - 1);
