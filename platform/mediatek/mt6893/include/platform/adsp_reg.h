/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <platform/addressmap.h>

#define ADSP_DRAM_IMG_ADDR_MAX          0x80000000
#define ADSP_DRAM_SHARED_ADDR_MAX       0xFFFFFFFF
#define ADSP_DRAM_ADDR_ALIGN            0x02000000  //adsp dram remap alignment
#define RESERVED_MEM_ALIGN              0x10000
#define ADSP_MPU_MEM_ALIGN              0x1000
#define MPU_REGION_ID_ADSP_RO_MEM       (28)
#define MPU_REGION_ID_ADSP_SHARED_MEM   (30)

/* init */
#define ENABLE_ADSP_EMI_PROTECTION      (0)  //disable after EMI API confirm


/* MACRO Definition */
#define ADSPPLL_CON0                    (APMIXED_BASE + 0x0370)
#define ADSPPLL_CON1                    (APMIXED_BASE + 0x0374)
#define ADSPPLL_CON3                    (APMIXED_BASE + 0x037C)

#define CLK_CFG_UPDATE1                 (IO_BASE + 0x0008)
#define ADSP_CK_UPDATE                  (22)
#define CLK_CFG_13                      (IO_BASE + 0x00E0)
#define CLK_CFG_13_SET                  (IO_BASE + 0x00E4)
#define CLK_CFG_13_CLR                  (IO_BASE + 0x00E8)
#define ADSP_SEL_OFFSET                 (8)
#define CLK_26M_CK                      (0)
#define ADSPPLL_CK                      (6)

#define AUDIODSP_CK_CG                  (SCP_BASE + 0x20180)
#define CG_OFFSET                       (0)
#define CG_ENABLE                       (1)

/* adsp CFGREG define */
#define ADSP_CFGREG_SW_RSTN             (ADSP_BASE + 0x0000)
#define ADSP_SW_RSTN                    (0x33)

/* tinysys devapc */
#define R_SEC_DOMAIN                    (SCP_BASE + 0xA5080)
#define S_CORE0_DOMAIN                  (0)
#define S_CORE1_DOMAIN                  (4)
#define S_DMA0_DOMAIN                   (12)
#define S_DMA1_DOMAIN                   (16)
#define S_SPI0_DOMAIN                   (20)
#define S_SPI1_DOMAIN                   (24)
#define S_SPI2_DOMAIN                   (27)
#define DOMAIN_SCP                      ((1 << S_SPI2_DOMAIN) | \
                                        (1 << S_SPI1_DOMAIN) | \
                                        (1 << S_SPI0_DOMAIN) | \
                                        (1 << S_SPI0_DOMAIN) | \
                                        (1 << S_DMA1_DOMAIN) | \
                                        (1 << S_DMA0_DOMAIN) | \
                                        (1 << S_CORE1_DOMAIN) | \
                                        (1 << S_CORE0_DOMAIN))
#define R_ADSP_DOMAIN                   (SCP_BASE + 0xA5088)
#define S_ADSP_DOMAIN                   (0)
#define DOMAIN_ADSP                     (0 << S_ADSP_DOMAIN)
