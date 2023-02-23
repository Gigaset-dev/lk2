/*
 * Copyright (c) 2022 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <platform/dpm.h>
#include <platform/reg.h>
#include <trace.h>
#include <platform/addressmap.h>

#define LOCAL_TRACE  1


#define DRAMC_MCU_SPM_CON_OFST          0x3BC
#define DRAMC_MCU_SRAM_CLKISO           (1U << 0)
#define DRAMC_MCU_SRAM_ISOINT_B_LSB     (1U << 1)
#define DRAMC_MCU_SRAM_SLEEP_B_LSB      (1U << 4)
//#define DRAMC_MCU_SRAM_SLEEP_B_ACK_LSB  (1U << 13)


void wake_dpm_sram_up(void)
{
  int loop = 0;

  LTRACEF("[DPM] %s start1\n", __func__);
  write32((SPM_BASE+DRAMC_MCU_SPM_CON_OFST),
          read32(SPM_BASE+DRAMC_MCU_SPM_CON_OFST) | DRAMC_MCU_SRAM_SLEEP_B_LSB);
  LTRACEF("[DPM] %s start\n", __func__);
  while ((0 == (read32(SPM_BASE+DRAMC_MCU_SPM_CON_OFST) & DRAMC_MCU_SRAM_SLEEP_B_LSB)))
          loop = loop + 1;
  LTRACEF("[DPM] %s end\n", __func__);
  write32((SPM_BASE+DRAMC_MCU_SPM_CON_OFST),
          read32(SPM_BASE+DRAMC_MCU_SPM_CON_OFST) | DRAMC_MCU_SRAM_ISOINT_B_LSB);
  write32((SPM_BASE+DRAMC_MCU_SPM_CON_OFST),
          read32(SPM_BASE+DRAMC_MCU_SPM_CON_OFST) & (~DRAMC_MCU_SRAM_CLKISO));
}


