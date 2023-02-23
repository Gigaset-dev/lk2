/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once

#include <platform/addressmap.h>

#define MCUPM_SRAM_BASE       (GIC_BASE + 0x00540000)
#define MCUPM_SW_RSTN         (GIC_BASE + 0x00580000)

/*
 * SPM config:
 * Enable register access key, POWERON_CONFIG_EN = 0x0B160001
 */
#define POWERON_CONFIG_EN     (SPM_BASE + 0x000) //0x1C001000
#define SPM_PROJECT_CODE      0xb16
#define SPM_REGWR_CFG_KEY     (SPM_PROJECT_CODE << 16)
#define BCLK_CG_EN_LSB        (1U << 0)       /* 1b */
// MCUPM Power Control Register
#define CPUEB_PWR_CON         (SPM_BASE + 0xEB4)
// CPUEB_PWR_CON (0x1C001000+0xEB4)
#define CPUEB_PWR_RST_B_LSB                 (1U << 0)       /* 1b */
#define CPUEB_PWR_ISO_LSB                   (1U << 1)       /* 1b */
#define CPUEB_PWR_ON_LSB                    (1U << 2)       /* 1b */
#define CPUEB_PWR_ON_2ND_LSB                (1U << 3)       /* 1b */
#define CPUEB_PWR_CLK_DIS_LSB               (1U << 4)       /* 1b */
#define CPUEB_SRAM_CKISO_LSB                (1U << 5)       /* 1b */
#define CPUEB_SRAM_ISOINT_B_LSB             (1U << 6)       /* 1b */
#define CPUEB_SRAM_PDN_LSB                  (1U << 8)       /* 1b */
#define CPUEB_SRAM_SLEEP_B_LSB              (1U << 9)       /* 1b */
#define SC_CPUEB_SRAM_PDN_ACK_LSB           (1U << 12)      /* 1b */
#define SC_CPUEB_SRAM_SLEEP_B_ACK_LSB       (1U << 13)      /* 1b */
#define SC_CPUEB_PWR_ACK_LSB                (1U << 30)      /* 1b */
#define SC_CPUEB_PWR_ACK_2ND_LSB            (1U << 31)      /* 1b */

#define MCUPM_SRAM_SIZE         0x30000     //192K Bytes

#define MCUPM_LOADER_SIZE       0x2300  // CFG_MTCMOS_OFF_SUPPORT
#define MCUPM_TCM_SZ            0x00030000  // 192K
#define MCUPM_IMG_OFS           0x2300  // CFG_MTCMOS_OFF_SUPPORT
#define MCUPM_RDMP_OFS          0x30000

/* Enable MCUPM Core Dump  */
#define MCUPM_Core_Dump         0
