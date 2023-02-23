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
#include <platform/irq.h>  /* for UFS_IRQ_ID */
#include <platform/round.h>
#include <reg.h>
#include <stdio.h>
#include <ufs_cfg.h>
#include <ufs_platform.h>

#define CMDLINE_BUF_SIZE 128
#define EFUSE_M_HW_RES0             (EFUSE_BASE+0x0260)
/*
 * Setting kernel command line of boot device
 */
static void ufs_commandline_bootdevice(void)
{
    int ret = 0;
    char cmdline_buf[CMDLINE_BUF_SIZE];

    ret = snprintf(cmdline_buf, CMDLINE_BUF_SIZE,
        "loop.max_part=7 androidboot.boot_devices=bootdevice,soc/%08lx.ufshci,%08lx.ufshci",
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
    dprintf(ALWAYS, "kcmdline append %s\n", cmdline_buf);
    return;
}

void ufs_init_mphy(void)
{
#ifndef UFS_CFG_FPGA_PLATFORM
    unsigned int k, s, m;
    unsigned int m3p5db, m3p5db_dem, m6db, m6db_dem;
    unsigned int value;
    unsigned int i;
    unsigned long reg;
    unsigned int res0;

    /*
     * Default dump efuse value for debug
     * Without efuse write, may have ufs stabiltiy issue
     */
    res0 = readl(EFUSE_M_HW_RES0);
    dprintf(ALWAYS, "[UFS] %s: LK ufs res0 value=0x%x\n", __func__, res0);

    /* clear debug feature for rx latch in exit h8 */
    reg = UFS_MPHY_BASE + 0x1010;
    value = readl(reg) & (~(1<<7));
    writel(value, reg);

    /* Lane 0 TX Setting efsue not appply, reset to default */
    if ((res0 & 0x01) == 0) {
        /*
         * rg_mp_tx_rt_0db_dem[3:0] = 4'b0000 (0)
         * rg_mp_tx_rt_0db[4:0] = 5'b11110 (30)
         */
        reg = UFS_MPHY_BASE + 0x803C;
        value = (readl(reg) & (~0xF1F)) | 0x1E;
        writel(value, reg);

        /* RG_UFS_BG_TRIM_CODE[5:0] = 6'b010011 */
        reg = UFS_MPHY_BASE + 0x1004;
        value = (readl(reg) & (~0x3F000000)) | 0x13000000;
        writel(value, reg);
    }

    /* Lane 1 TX Setting efsue not appply, reset to default */
    if ((res0 & 0x02) == 0) {
        /*
         * rg_mp_tx_rt_0db_dem[3:0] = 4'b0000 (0)
         * rg_mp_tx_rt_0db[4:0] = 5'b11110 (30)
         */
        reg = UFS_MPHY_BASE + 0x813C;
        value = (readl(reg) & (~0xF1F)) | 0x1E;
        writel(value, reg);
    }

    /* Lane 0/1 TX -3.5dB and -6dB Setting */
    for (i = 0; i < 2; i++) {
        reg = UFS_MPHY_BASE + 0x803C + (i * 0x100);
        value = readl(reg);

        /*
         * rg_mp_tx_rt_0db[4:0] = m
         * rg_mp_tx_rt_dem_0db[3:0] = s
         * If s = 0, k = m - 4, EX: s = 0
         * If 0 < s <= 2, k = 27 + s, EX: s = 1, 2
         * If s > 2, k = 27 + s - 6, EX: s = 9 ~ 15
         */
        m = (value) & 0x1F;
        s = (value >> 8) & 0xF;
        if (s == 0)
            k = m - 4;
        else if (s <= 2)
            k = 27 + s;
        else
            k = 27 + s - 6;

        /*
         * rg_mp_tx_rt_35db[4:0] = D2B[Round(K*5/6) +4]
         * rg_mp_tx_rt_35db_dem[3:0] = D2B[Round(K*1/6) +6]
         *
         * rg_mp_tx_rt_6db[4:0] = D2B[Round(K*3/4) +4]
         * rg_mp_tx_rt_6db_dem[3:0] = D2B[Round(K*1/4) +6]
         */
        m3p5db = (DIV_ROUND_CLOSEST(k * 5, 6) + 4) & 0x1F;
        m3p5db_dem = (DIV_ROUND_CLOSEST(k, 6) + 6) & 0xF;

        m6db = (DIV_ROUND_CLOSEST(k * 3, 4) + 4) & 0x1F;
        m6db_dem = (DIV_ROUND_CLOSEST(k, 4) + 6) & 0xF;

        reg = UFS_MPHY_BASE + 0x8038 + (i * 0x100);
        value = (m6db_dem << 24) | (m6db << 16) |
                (m3p5db_dem << 8) | m3p5db;
        writel(value, reg);
    }

    /*
     * Sync length
     */
    reg = UFS_MPHY_BASE + 0x2088;
    value = (readl(reg) & (~0xFF000000)) | 0x4A000000;
    writel(value, reg);
    reg = UFS_MPHY_BASE + 0x2094;
    value = (readl(reg) & (~0x0000FFFF)) | 0x00004A4A;
    writel(value, reg);
    reg = UFS_MPHY_BASE + 0x2098;
    value = (readl(reg) & (~0xFF000000)) | 0x4A000000;
    writel(value, reg);

    /*
     * FSM
     */
    for (i = 0; i < 2; i++) {
        reg = UFS_MPHY_BASE + 0xA034 + (i * 0x100);
        value = (readl(reg) & (~0x7FF)) | 0x1C1;
        writel(value, reg);
        reg = UFS_MPHY_BASE + 0xA038 + (i * 0x100);
        value = (readl(reg) & (~0x7FF0000)) | 0x1C20000;
        writel(value, reg);
        reg = UFS_MPHY_BASE + 0xA03C + (i * 0x100);
        value = (readl(reg) & (~0x7FF)) | 0x1C7;
        writel(value, reg);
        reg = UFS_MPHY_BASE + 0xA03C + (i * 0x100);
        value = (readl(reg) & (~0x7FF0000)) | 0x1F90000;
        writel(value, reg);
        reg = UFS_MPHY_BASE + 0xA044 + (i * 0x100);
        value = (readl(reg) & (~0x7FF0000)) | 0x1EF0000;
        writel(value, reg);
        reg = UFS_MPHY_BASE + 0xA0CC + (i * 0x100);
        value = (readl(reg) & (~0x7FF)) | 0x1FF;
        writel(value, reg);
    }

    /*
     * CDR
     */
    for (i = 0; i < 2; i++) {
        writel(0x1A191104, UFS_MPHY_BASE + 0xA054 + (i * 0x100));
        writel(0x1A191105, UFS_MPHY_BASE + 0xA058 + (i * 0x100));
        writel(0x09080602, UFS_MPHY_BASE + 0xA05C + (i * 0x100));
        writel(0x09080602, UFS_MPHY_BASE + 0xA060 + (i * 0x100));
        writel(0x0A0A0A04, UFS_MPHY_BASE + 0xA064 + (i * 0x100));
        writel(0x0A0A0A05, UFS_MPHY_BASE + 0xA068 + (i * 0x100));
    }

    /*
     * TH8
     */
    reg = UFS_MPHY_BASE + 0x2090;
    value = (readl(reg) & (~0xFF0000)) | 0x10000;
    writel(value, reg);
    reg = UFS_MPHY_BASE + 0x2098;
    value = readl(reg) & (~0x7);
    writel(value, reg);

    /*
     * RxLsPrepareLength
     */
    reg = UFS_MPHY_BASE + 0x208C;
    value = (readl(reg) & (~0xF00)) | 0x900;
    writel(value, reg);

    /*
     * TMinActivate
     */
    reg = UFS_MPHY_BASE + 0x208C;
    value = (readl(reg) & (~0xF000000)) | 0x1000000;
    writel(value, reg);

    /*
     * ADAPT K
     */
    for (i = 0; i < 2; i++) {
        /* rg_mp_rx_eq_dg_r1_hsg4[5:0] = d'32(0x20) => d'48(0x30) */
        /* rg_mp_rx_eq_dg_r2_hsg4[5:0] = d'32(0x20) => d'48(0x30) */
        reg = UFS_MPHY_BASE + 0xA0DC + (i * 0x100);
        value = (readl(reg) & (~0x3F003F00)) | 0x30003000;
        writel(value, reg);
    }
    /* rg_mp_mib_rx_hs_adapt_refresh_capability[7:0]=0x8C(220us) => 0x8E(880us) */
    /* rg_mp_mib_rx_hs_adapt_initial_capability[7:0]=0x8C(220us) => 0x8E(880us) */
    reg = UFS_MPHY_BASE + 0x209C;
    value = (readl(reg) & (~0xFFFF0000)) | 0x8E8E0000;
    writel(value, reg);

    /*
     * Lane 0/1 RG_MP_RX_CDR_PI_SKEW[5:0] = 00 -> 0x3F for better RX JTOL performance
     */
    for (i = 0; i < 2; i++) {
        reg = UFS_MPHY_BASE + 0xB000 + (i * 0x100);
        value = (readl(reg) & (~0x3F000000)) | 0x3F000000;
        writel(value, reg);
    }

#if 0
    /* Dump this setting after bring up for DE check */
    hexdump((void *)EFUSE_M_HW_RES0, 0x18);
    hexdump((void *)UFS_MPHY_BASE + 0x0000, 0xE8);
    hexdump((void *)UFS_MPHY_BASE + 0x1000, 0x84);
    hexdump((void *)UFS_MPHY_BASE + 0x2000, 0xD8);
    hexdump((void *)UFS_MPHY_BASE + 0x3000, 0x24);
    hexdump((void *)UFS_MPHY_BASE + 0xB000, 0x14);
    hexdump((void *)UFS_MPHY_BASE + 0xB100, 0x14);
    hexdump((void *)UFS_MPHY_BASE + 0x9100, 0x8);
    hexdump((void *)UFS_MPHY_BASE + 0xA000, 0xEC);
    hexdump((void *)UFS_MPHY_BASE + 0xA100, 0xEC);
    hexdump((void *)UFS_MPHY_BASE + 0x8000, 0xC8);
    hexdump((void *)UFS_MPHY_BASE + 0x8100, 0xC8);
#endif
#endif
    ufs_commandline_bootdevice();
}

int ufs_get_irq_id(void)
{
    return UFS_IRQ_ID;
}
