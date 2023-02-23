/*
 * Copyright (c) 2022 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once

#include <platform/mcupm_plat.h>

#define MCUPM_GPR_SIZE            0x00000064
#define SRAM_GPR_SIZE_4B          0x4        //4 Bytes
#define IPI_MBOX_TOTAL            8
#define SRAM_RESERVED_20B         0x00000014 //20 Bytes
#define MBOX_SLOT_SIZE_4B         0x00000004
#define SRAM_SLOT_SIZE_80B        0x00000014 //0x14 = 20 slots = 80 Bytes
#define PIN_S_SIZE                SRAM_SLOT_SIZE_80B
#define PIN_R_SIZE                SRAM_SLOT_SIZE_80B
#define MBOX_TABLE_SIZE           (PIN_S_SIZE + PIN_R_SIZE)

#define GPR_BASE_ADDR(x)          (MCUPM_SRAM_BASE + \
        MCUPM_SRAM_SIZE - \
        (IPI_MBOX_TOTAL * MBOX_TABLE_SIZE * MBOX_SLOT_SIZE_4B) - \
        MCUPM_GPR_SIZE + \
        (x * SRAM_GPR_SIZE_4B))

// Log @ CSRAM
#define MCUPM_EEM_LOG_ADDR        (SRAM_BASE + 0x00012800) // 0x0011_2800
#define MCUPM_EEM_LOG_SIZE        (0xC00)

#define MCUPM_PICACHU_LOG_ADDR    (SRAM_BASE + 0x00013400) // 0x0011_3400
#define MCUPM_PICACHU_LOG_SIZE    (0xC00)
