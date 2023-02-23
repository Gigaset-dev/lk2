/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once

#include <platform/addressmap.h>

#define MCUCFG_PAR_WRAP     (MCUCFG_BASE_PHY)
#define MP0_CLUSTER_CFG0    (MCUCFG_PAR_WRAP + 0x60)
#define L3_SHARE_FULLnHALF  0X0
#define L3_SHARE_EN         0x1
#define L3_SHARE_PRE_EN     0x2
#define CLUST_DIS_VAL       0x3
#define CLUST_DIS_SHIFT     0x4
