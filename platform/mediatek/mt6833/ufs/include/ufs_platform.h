/*
 * Copyright (c) 2021 MediaTek Inc.
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

#define GPIO_BANK_BASE       (GPIO_BASE + 0x6F0)
#define TRAP_UFS_FIRST  (0x3U << 17) /* [18:17] 0: UFS, 1: eMMC, 2: SF */
