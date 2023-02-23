/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <platform/addressmap.h>

#define SPM_PWR_STS_ADDR    (SPM_BASE + 0x910)
#define SPM_PWR_STS_MASK    0x0007F800
#define SPM_PWR_STS_OFFSET  11

const unsigned long cpu_dbg_sel[] = {
    (MCUCFG_BASE + 0x0220),
    (MCUCFG_BASE + 0x0A20),
    (MCUCFG_BASE + 0x1220),
    (MCUCFG_BASE + 0x1A20),
    (MCUCFG_BASE + 0x2220),
    (MCUCFG_BASE + 0x2A20),
    (MCUCFG_BASE + 0x3220),
    (MCUCFG_BASE + 0x3A20)
};

const unsigned long cpu_dbg_mon[] = {
    (MCUCFG_BASE + 0x0224),
    (MCUCFG_BASE + 0x0A24),
    (MCUCFG_BASE + 0x1224),
    (MCUCFG_BASE + 0x1A24),
    (MCUCFG_BASE + 0x2224),
    (MCUCFG_BASE + 0x2A24),
    (MCUCFG_BASE + 0x3224),
    (MCUCFG_BASE + 0x3A24)
};

const struct plt_cfg_pc_latch cfg_pc_latch = {
    .nr_max_core = 8,
    .nr_max_big_core = 4,
    .core_dbg_sel = (unsigned long *) cpu_dbg_sel,
    .core_dbg_mon = (unsigned long *) cpu_dbg_mon,
    .version = LASTPC_V1,
};

enum {
    LCORE_SCR_EL3     = 0x0,
    LCORE_SCTLR_EL3   = 0x1,
    LCORE_FAR_EL3_L   = 0x2,
    LCORE_FAR_EL3_H   = 0x3,
    LCORE_ESR_EL3     = 0x4,
    LCORE_CPSR        = 0x5,
    LCORE_ELR_L       = 0x6,
    LCORE_ELR_H       = 0x7,
    LCORE_LR_L        = 0x8,
    LCORE_LR_H        = 0x9,
    LCORE_SP_L        = 0xA,
    LCORE_SP_H        = 0xB,
    LCORE_PC_L        = 0xC,
    LCORE_PC_H        = 0xD
};

enum {
    BCORE_SCR_EL3_L   = 0x0,
    BCORE_SCR_EL3_H   = 0x1,
    BCORE_SCTLR_EL3_L = 0x2,
    BCORE_SCTLR_EL3_H = 0x3,
    BCORE_FAR_EL3_L   = 0x4,
    BCORE_FAR_EL3_H   = 0x5,
    BCORE_ESR_EL3_L   = 0x6,
    BCORE_ESR_EL3_H   = 0x7,
    BCORE_PC_L        = 0x8,
    BCORE_PC_H        = 0x9,
    BCORE_CPSR        = 0xA,
    BCORE_ELR_L       = 0xB,
    BCORE_ELR_H       = 0xC,
    BCORE_PCSR_L      = 0xD,
    BCORE_PCSR_H      = 0xE
};
