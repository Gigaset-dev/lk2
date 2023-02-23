/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once
#include <platform/addressmap.h>

#define INFRA_BUS_HRE_ADDR_BASE                INFRA_BUS_HRE_BASE
#define INFRA_BUS_HRE_DATA_SZ                  0x2800   /* 10KB */
#define INFRA_BUS_HRE_SRAM_ADDR_MIN            0x400
#define INFRA_BUS_HRE_SRAM_ADDR_MAX            0xBFC

// 0xBFC - 0x400 (INFRA_BUS_HRE_SRAM_ADDR_MAX - INFRA_BUS_HRE_SRAM_ADDR_MIN)
#define INFRA_BUS_HRE_SRAM_SZ                  0x7FC
#define INFRA_BUS_HRE_SRAM_DUMP_CYCLE_SZ       0x100

#define INFRA_BUS_HRE_DUMP_START               (INFRA_BUS_HRE_ADDR_BASE + 0x200)
#define INFRA_BUS_HRE_DUMP_END                 (INFRA_BUS_HRE_ADDR_BASE + 0x330)
#define INFRA_BUS_HRE_HRE_CLK_DCM_CTRL         (INFRA_BUS_HRE_ADDR_BASE + 0x10C)
#define INFRA_BUS_HRE_DEBUG_EN                 (INFRA_BUS_HRE_ADDR_BASE + 0x118)
#define INFRA_BUS_HRE_SRAM_BASE_EXTEND_EN      (INFRA_BUS_HRE_ADDR_BASE + 0x12C)
#define INFRA_BUS_HRE_SRAM_BASE_EXTEND         (INFRA_BUS_HRE_ADDR_BASE + 0x130)

#define INFRA_BUS_HRE_DUMP_BASE             (INFRA_BUS_HRE_ADDR_BASE + INFRA_BUS_HRE_SRAM_ADDR_MIN)
#define INFRA_BUS_HRE_EXTEND_BASE           (INFRA_BUS_HRE_ADDR_BASE + INFRA_BUS_HRE_SRAM_ADDR_MIN)
