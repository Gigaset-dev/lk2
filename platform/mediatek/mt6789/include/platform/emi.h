/*
 * Copyright (c) 2021-2022 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once
#include <platform/addressmap.h>

/* #define DISABLE_EMI_MPU */

#define EMI_MPU_CTRL                            (EMI_MPU_BASE + 0x000)
#define EMI_MPU_DBG                             (EMI_MPU_BASE + 0x004)
#define EMI_MPU_SA0                             (EMI_MPU_BASE + 0x100)
#define EMI_MPU_EA0                             (EMI_MPU_BASE + 0x200)
#define EMI_MPU_SA(region)                      (EMI_MPU_SA0 + (region*4))
#define EMI_MPU_EA(region)                      (EMI_MPU_EA0 + (region*4))
#define EMI_MPU_APC0                            (EMI_MPU_BASE + 0x300)
#define EMI_MPU_APC(region, dgroup)             (EMI_MPU_APC0 + (region*4) + (dgroup*0x100))
#define EMI_MPU_CTRL_D0                         (EMI_MPU_BASE + 0x800)
#define EMI_MPU_CTRL_D(domain)                  (EMI_MPU_CTRL_D0 + (domain*4))
#define EMI_RG_MASK_D0                          (EMI_MPU_BASE + 0x900)
#define EMI_RG_MASK_D(domain)                   (EMI_RG_MASK_D0 + (domain*4))
#define EMI_MPU_START                           (0x000)
#define EMI_MPU_END                             (0x93C)

#define EMI_MPU_MPUS                            (EMI_APB_BASE + 0x1F0)
#define EMI_MPU_MPUT                            (EMI_APB_BASE + 0x1F8)
#define EMI_MPU_MPUT_2ND                        (EMI_APB_BASE + 0x1FC)

/* the SW supports max num of domain and region to 2048 and 256 respectively */
#define EMI_MPU_DOMAIN_NUM        16
#define EMI_MPU_REGION_NUM        32
#define EMI_MPU_ALIGN_BITS        16
