/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <platform/addressmap.h>

#define UFS_MMIO_PERICFG_BASE   (PERICFG_BASE)
#define UFS_HCI_BASE            (UFS0_BASE)
#define UFS_MPHY_BASE           (UFS0_MPHY_BASE)

enum {
    REG_UFS_PERICFG             = 0x448,
    REG_UFS_PERICFG_RST_N_BIT   = 3,
    REG_UFS_PERICFG_LDO_N_BIT   = 0xF,
    REG_UFS_PERICFG_LP_N_BIT    = 0xF
};

#define TRAP_UFS_FIRST  (0x3U << 17) /* [18:17] 0: UFS, 1: eMMC, 2: SF */

#define GPIO_BANK_BASE (GPIO_BASE + 0x000006F0)

#define EFUSE_VERSION  (EFUSE_BASE + 0x07CC)

#define UFS_MPHY_MP_GLB_DIG_58     (UFS_MPHY_BASE + 0x0058)
#define UFS_MPHY_MP_GLB_ANA_04     (UFS_MPHY_BASE + 0x1004)
#define UFS_MPHY_MP_LN_DIG_RX_08_1 (UFS_MPHY_BASE + 0xA108)
#define UFS_MPHY_MP_LN_DIG_RX_1C_1 (UFS_MPHY_BASE + 0xA11C)
#define UFS_MPHY_MP_LN_DIG_RX_20_1 (UFS_MPHY_BASE + 0xA120)
#define UFS_MPHY_MP_LN_DIG_RX_48_1 (UFS_MPHY_BASE + 0xA148)
#define UFS_MPHY_MP_LN_DIG_RX_80_1 (UFS_MPHY_BASE + 0xA180)
#define UFS_MPHY_MP_LN_DIG_TX_30   (UFS_MPHY_BASE + 0x8030)
#define UFS_MPHY_MP_LN_DIG_TX_34   (UFS_MPHY_BASE + 0x8034)
#define UFS_MPHY_MP_LN_DIG_TX_04_1 (UFS_MPHY_BASE + 0x8104)
#define UFS_MPHY_MP_LN_DIG_TX_30_1 (UFS_MPHY_BASE + 0x8130)
#define UFS_MPHY_MP_LN_DIG_TX_34_1 (UFS_MPHY_BASE + 0x8134)
#define UFS_MPHY_MP_LN_DIG_TX_A4_1 (UFS_MPHY_BASE + 0x81A4)
