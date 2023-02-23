/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <platform/addressmap.h>

#define DRAM_4GB                        (0x100000000ULL)
#define DRAM_9GB                        (0x240000000ULL)
#define DRAM_13GB                       (0x340000000ULL)

/* Support low memory address */
#define ADSP_DRAM_LOWER_BOUND_L         (0)
#define ADSP_DRAM_UPPER_BOUND_L         (DRAM_4GB)
#define ADSP_DRAM_IMG_ADDR_L_MAX        (0x80000000)
#define ADSP_DRAM_SHARED_ADDR_L_MAX     (ADSP_DRAM_UPPER_BOUND_L - 1)
/* Support high memory address */
#define ADSP_DRAM_LOWER_BOUND_H         (DRAM_9GB)
#define ADSP_DRAM_UPPER_BOUND_H         (DRAM_13GB)
#define ADSP_DRAM_IMG_ADDR_H_MAX        (ADSP_DRAM_LOWER_BOUND_H + 0x40000000)
#define ADSP_DRAM_SHARED_ADDR_H_MAX     (ADSP_DRAM_UPPER_BOUND_H - 1)

#define ADSP_DRAM_REMAP_LIMIT           (0x80000000)
#define ADSP_DRAM_ADDR_ALIGN            0x02000000  //adsp dram remap alignment
#define RESERVED_MEM_ALIGN              0x10000
#define ADSP_MPU_MEM_ALIGN              0x1000
#define MPU_REGION_ID_ADSP_RO_MEM       (28)
#define MPU_REGION_ID_ADSP_SHARED_MEM   (29)

/* MACRO Definition */
#define ADSPPLL_CON0                    (APMIXED_BASE + 0x0380)
#define ADSPPLL_CON1                    (APMIXED_BASE + 0x0384)
#define ADSPPLL_CON3                    (APMIXED_BASE + 0x038C)

#define CLKCFG_UPDATE                   (IO_BASE + 0x0008)
#define CLKCFG_ADSP_SEL                 (IO_BASE + 0x0100)
#define CLKCFG_ADSP_SEL_SET             (IO_BASE + 0x0104)
#define CLKCFG_ADSP_SEL_CLR             (IO_BASE + 0x0108)
#define ADSP_CK_UPDATE                  (30)
#define ADSP_SEL_OFFSET                 (8)
#define ADSPPLL_CK                      (7)
#define CLK_26M_CK                      (0)

/* adsp CFGREG define */
#define ADSP_CFGREG_SW_RSTN             (ADSP_BASE + 0x0000)
#define ADSP_HIFI3_IO_CONFIG            (ADSP_BASE + 0x000C)
#define ADSP_PC_E_C0                    (ADSP_BASE + 0x00C4)

#define ADSP_HIFI3_ALTVEC_C0            (ADSP_BASE + 0xB004)
#define ADSP_HIFI3_ALTVEC_C1            (ADSP_BASE + 0xB008)
#define ADSP_ALTVECSEL                  (ADSP_BASE + 0xB00C) //bit 0: core1, bit 1:core0

#define AUDIO_BUS_DSP2EMI_REMAP0        (ADSP_BASE + 0xFA00)
#define AUDIO_BUS_DSP2EMI_REMAP1        (ADSP_BASE + 0xFA04)
#define AUDIO_BUS_DMA2EMI_REMAP0        (ADSP_BASE + 0xFA08)
#define AUDIO_BUS_DMA2EMI_REMAP1        (ADSP_BASE + 0xFA0C)

#define ADSP_SW_RSTN                    (0x33)
#define ADSP_A_RUNSTALL_BIT             (0x1 << 31)
#define ADSP_B_RUNSTALL_BIT             (0x1 << 30)
#define ALTVECSET_BIT_C0                (0x1 << 1)

