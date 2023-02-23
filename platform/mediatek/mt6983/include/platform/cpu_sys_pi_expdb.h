/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once

#include <platform/mcupm_plat.h>

#define MCUPM_GPR_SIZE            0x00000064
#define SRAM_GPR_SIZE_4B          0x4         //4 Bytes
#define IPI_MBOX_TOTAL            8
#define SRAM_RESERVED_20B         0x00000014  //20 Bytes
#define MBOX_SLOT_SIZE_4B         0x00000004
#define SRAM_SLOT_SIZE_80B        0x00000014      //0x14 = 20 slots = 20*4Bytes = 80 Bytes
#define PIN_S_SIZE                SRAM_SLOT_SIZE_80B
#define PIN_R_SIZE                SRAM_SLOT_SIZE_80B
#define MBOX_TABLE_SIZE           (PIN_S_SIZE + PIN_R_SIZE)

#define GPR_BASE_ADDR(x)          (MCUPM_SRAM_BASE + \
        MCUPM_SRAM_SIZE - \
        (IPI_MBOX_TOTAL * MBOX_TABLE_SIZE * MBOX_SLOT_SIZE_4B) - \
        MCUPM_GPR_SIZE + \
        (x * SRAM_GPR_SIZE_4B))

// Log @ CSRAM
#define MCUPM_EEM_LOG_ADDR        (SRAM_BASE + 0x00012400) // 0x0011_2400
#define MCUPM_EEM_LOG_SIZE        (0xA00)

#define MCUPM_PICACHU_LOG_ADDR    (SRAM_BASE + 0x00012E00) // 0x0011_2E00
#define MCUPM_PICACHU_LOG_SIZE    (0xE00)

// Log @ MCUPM TCM
#define MCUPM_TCM_LOG_CNT         2

#define MCUPM_SES_LOG_ADDR        (MCUPM_SRAM_BASE + (read32(GPR_BASE_ADDR(5))))
#define MCUPM_SES_LOG_SIZE        ((read32(GPR_BASE_ADDR(6))) / MCUPM_TCM_LOG_CNT)

#define MCUPM_DVCS_LOG_ADDR       (MCUPM_SRAM_BASE + (read32(GPR_BASE_ADDR(5))) + \
                                   MCUPM_SES_LOG_SIZE)
#define MCUPM_DVCS_LOG_SIZE       ((read32(GPR_BASE_ADDR(6))) / MCUPM_TCM_LOG_CNT)

