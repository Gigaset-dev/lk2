/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once

#include <platform/addressmap.h>

enum {
    GIC_DIST_BASE      = GIC_BASE     + 0,
    GIC_REDIS_BASE     = GIC_BASE     + 0x00040000,
    GIC_REDIS_BASE_PHY = GIC_BASE_PHY + 0x00040000,
};

#define MAX_INT 840
