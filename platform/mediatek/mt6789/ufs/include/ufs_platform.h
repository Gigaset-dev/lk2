/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <platform/addressmap.h>

#define UFS_HCI_BASE            (UFS0_BASE)
#define UFS_MPHY_BASE           (UFS0_MPHY_BASE)

/*
 * Because reset address change from pericfg+0x448 to ufs_ao_config+0x90
 * we simply change base address and not change variable name
 */
#define UFS_MMIO_PERICFG_BASE   (PERICFG_BASE)

enum {
    REG_UFS_PERICFG             = 0x448,
    REG_UFS_PERICFG_RST_N_BIT   = 3,
    REG_UFS_PERICFG_LDO_N_BIT   = 0xF,
    REG_UFS_PERICFG_LP_N_BIT    = 0xF
};

#define TRAP_UFS_FIRST  (1U << 10) /* bit 10 0: UFS, bit 10 1: eMMC */

#define GPIO_BANK_BASE (GPIO_BASE + 0x000006F0)
