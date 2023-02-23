/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <platform/dpm.h>
#include <platform/reg.h>
#include <trace.h>

#define DRAMC_MCU_SPM_CON_OFST          0x3BC
#define DRAMC_MCU_SRAM_ISOINT_B_LSB     (1U << 1)
#define DRAMC_MCU_SRAM_SLEEP_B_LSB      (1U << 4)
#define DRAMC_MCU_SRAM_SLEEP_B_ACK_LSB  (1U << 13)

#define LOCAL_TRACE  0

void wake_dpm_sram_up(void)
{
    unsigned int reg = SPM_BASE + DRAMC_MCU_SPM_CON_OFST;

    write32(reg, read32(reg) | DRAMC_MCU_SRAM_SLEEP_B_LSB);
    while (0 == (read32(reg) & DRAMC_MCU_SRAM_SLEEP_B_ACK_LSB))
        LTRACEF("[dpm] waking up dpm SRAM\n");

    write32(reg, read32(reg) | DRAMC_MCU_SRAM_ISOINT_B_LSB);
}
