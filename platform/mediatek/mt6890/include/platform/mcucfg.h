/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once

#include <platform/addressmap.h>
#include <compiler.h>
#include <stdint.h>

struct mtk_mcucfg_regs {
    uint32_t reserved1[13696];
    uint32_t sec_pol_ctl_en[26];
    uint32_t int_pol_ctl[26];
};

STATIC_ASSERT(__offsetof(struct mtk_mcucfg_regs, sec_pol_ctl_en) == 0xd600);
STATIC_ASSERT(__offsetof(struct mtk_mcucfg_regs, int_pol_ctl) == 0xd668);

static struct mtk_mcucfg_regs *const mcucfg = (void *)MCUCFG_BASE;
