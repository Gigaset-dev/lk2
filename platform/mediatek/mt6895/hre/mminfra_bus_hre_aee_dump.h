/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once
#include <platform/addressmap.h>

#define MMINFRA_BUS_HRE_ADDR_BASE                MMINFRA_BUS_HRE_BASE
#define MMINFRA_BUS_HRE_DATA_SZ                  0x2800   /* 10KB */
#define MMINFRA_BUS_HRE_SRAM_ADDR_MIN            0x400
#define MMINFRA_BUS_HRE_SRAM_ADDR_MAX            0x13FC

// 0x13FC - 0x400 (MMINFRA_BUS_HRE_SRAM_ADDR_MAX - MMINFRA_BUS_HRE_SRAM_ADDR_MIN)
#define MMINFRA_BUS_HRE_SRAM_SZ                  0xFFC
#define MMINFRA_BUS_HRE_SRAM_DUMP_CYCLE_SZ       0x100

#define MMINFRA_BUS_HRE_DUMP_START               (MMINFRA_BUS_HRE_ADDR_BASE + 0x200)
#define MMINFRA_BUS_HRE_DUMP_END                 (MMINFRA_BUS_HRE_ADDR_BASE + 0x330)
#define MMINFRA_BUS_HRE_HRE_CLK_DCM_CTRL         (MMINFRA_BUS_HRE_ADDR_BASE + 0x10C)
#define MMINFRA_BUS_HRE_DEBUG_EN                 (MMINFRA_BUS_HRE_ADDR_BASE + 0x118)
#define MMINFRA_BUS_HRE_SRAM_BASE_EXTEND_EN      (MMINFRA_BUS_HRE_ADDR_BASE + 0x12C)
#define MMINFRA_BUS_HRE_SRAM_BASE_EXTEND         (MMINFRA_BUS_HRE_ADDR_BASE + 0x130)

#define MMINFRA_BUS_HRE_DUMP_BASE      (MMINFRA_BUS_HRE_ADDR_BASE + MMINFRA_BUS_HRE_SRAM_ADDR_MIN)
#define MMINFRA_BUS_HRE_EXTEND_BASE    (MMINFRA_BUS_HRE_ADDR_BASE + MMINFRA_BUS_HRE_SRAM_ADDR_MIN)

#define MM_HRE_SRAM_CON  (SPM_BASE + 0xf18)
