/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once
#include <platform/addressmap.h>

#define SECURE_REGION_SA0_BASE                  (EMI_SMPU_BASE+0x0)
#define SECURE_REGION_SA1_BASE                  (EMI_SMPU_BASE+0x4)
#define SECURE_REGION_SA0(set, bit)             (SECURE_REGION_SA0_BASE + set*0x200 + bit*0x10)
#define SECURE_REGION_SA1(set, bit)             (SECURE_REGION_SA1_BASE + set*0x200 + bit*0x10)
#define SECURE_REGION_EA0_BASE                  (EMI_SMPU_BASE+0x8)
#define SECURE_REGION_EA1_BASE                  (EMI_SMPU_BASE+0xc)
#define SECURE_REGION_EA0(set, bit)             (SECURE_REGION_EA0_BASE + set*0x200 + bit*0x10)
#define SECURE_REGION_EA1(set, bit)             (SECURE_REGION_EA1_BASE + set*0x200 + bit*0x10)

#define SUB_SECURE_REGION_SA0_BASE              (SUB_EMI_SMPU_BASE+0x0)
#define SUB_SECURE_REGION_SA1_BASE              (SUB_EMI_SMPU_BASE+0x4)
#define SUB_SECURE_REGION_SA0(set, bit)         (SUB_SECURE_REGION_SA0_BASE + set*0x200 + bit*0x10)
#define SUB_SECURE_REGION_SA1(set, bit)         (SUB_SECURE_REGION_SA1_BASE + set*0x200 + bit*0x10)
#define SUB_SECURE_REGION_EA0_BASE              (SUB_EMI_SMPU_BASE+0x8)
#define SUB_SECURE_REGION_EA1_BASE              (SUB_EMI_SMPU_BASE+0xc)
#define SUB_SECURE_REGION_EA0(set, bit)         (SUB_SECURE_REGION_EA0_BASE + set*0x200 + bit*0x10)
#define SUB_SECURE_REGION_EA1(set, bit)         (SUB_SECURE_REGION_EA1_BASE + set*0x200 + bit*0x10)

#define SECURE_REGION_ENABLE0                   (EMI_SMPU_BASE+0x1d8)
#define SECURE_REGION_ENABLE(region)            (SECURE_REGION_ENABLE0 + region*0x200)
#define SUB_SECURE_REGION_ENABLE0               (SUB_EMI_SMPU_BASE+0x1d8)
#define SUB_SECURE_REGION_ENABLE(region)        (SUB_SECURE_REGION_ENABLE0 + region*0x200)

#define SMPU_READ_AID                           (EMI_SMPU2_BASE+0x3d4)
#define SUB_SMPU_READ_AID                       (SUB_EMI_SMPU2_BASE+0x3d4)
#define SMPU_READ_ADDR_MSB                      (EMI_SMPU2_BASE+0x3c4)
#define SMPU_READ_ADDR_H                        (EMI_SMPU2_BASE+0x3c8)
#define SMPU_READ_ADDR_L                        (EMI_SMPU2_BASE+0x3cc)
#define SUB_SMPU_READ_ADDR_MSB                  (SUB_EMI_SMPU2_BASE+0x3c4)
#define SUB_SMPU_READ_ADDR_H                    (SUB_EMI_SMPU2_BASE+0x3c8)
#define SUB_SMPU_READ_ADDR_L                    (SUB_EMI_SMPU2_BASE+0x3cc)
#define SMPU_READ_CLEAR                         (EMI_SMPU2_BASE+0x3c0)
#define SUB_SMPU_READ_CLEAR                     (SUB_EMI_SMPU2_BASE+0x3c0)

#define EMI_MPU_CLEAR                           (EMI_APB_BASE+0x1f0)
#define EMI_MPU_HP_CLEAR                        (EMI_APB_BASE+0x1fc)
#define EMI_MPU_CLEAR_CONTENT                   (EMI_APB_BASE+0x200)
#define EMI_MPU_DOMAIN_ABORT                    (EMI_APB_BASE+0x160)
#define SUB_EMI_MPU_CLEAR                       (SUB_EMI_APB_BASE+0x1f0)
#define SUB_EMI_MPU_HP_CLEAR                    (SUB_EMI_APB_BASE+0x1fc)
#define SUB_EMI_MPU_CLEAR_CONTENT               (SUB_EMI_APB_BASE+0x200)
#define SUB_EMI_MPU_DOMAIN_ABORT                (SUB_EMI_APB_BASE+0x160)


#define ADDR_BIT 16
#define ADDR_MASK 0xffff
#define REGION_ENABLE (1 << 15)
#define MPU_REGION_NUMBER_PER_SET ADDR_BIT
#define SUB_EMI_SMPU

/* SLC parity dump feature setting */
#define SLC_SRAM_PARITY_ERR     0x0
#define SLC_SRAM_PARITY_ERR_OFF 13
#define SLC_SRAM_PARITY_ERR_BIT 2
#define SLC_ERROR_STATUS        0xc00
#define SLC_ERROR_STATUS_LENGTH 0xa0
