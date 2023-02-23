/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once

#include <platform/addressmap.h>

#define MCUCFG_BASE_PHY     (GIC_BASE_PHY + 0x00530000)
#define MP0_CLUSTER_CFG0    (MCUCFG_BASE_PHY + 0xC8D0)
#define L3_SHARE_EN         9
#define L3_SHARE_PRE_EN     8
