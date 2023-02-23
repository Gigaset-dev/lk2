/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once

#include <platform/addressmap.h>
#include <compiler.h>
#include <stdint.h>

struct mt6873_mcucfg_regs {
    uint32_t reserved1[640];
    uint32_t sec_pol_ctl_en[20];
    uint32_t reserved2[12];
    uint32_t int_pol_ctl[20];
};

STATIC_ASSERT(__offsetof(struct mt6873_mcucfg_regs, sec_pol_ctl_en) == 0xa00);
STATIC_ASSERT(__offsetof(struct mt6873_mcucfg_regs, int_pol_ctl) == 0xa80);

static struct mt6873_mcucfg_regs *const mcucfg = (void *)MCUCFG_BASE;
