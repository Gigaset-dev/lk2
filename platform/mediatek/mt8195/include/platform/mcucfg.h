/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once

#include <compiler.h>
#include <platform/addressmap.h>
#include <stdint.h>

struct mtk_mcucfg_regs {
    uint32_t reserved1[400];
    uint32_t aclken_div;
    uint32_t reserved2[87];
    uint32_t mp0_pll_div_cfg;
    uint32_t mp1_pll_div_cfg;
    uint32_t reserved3[6];
    uint32_t bus_pll_div_cfg;
    uint32_t reserved4[143];
    uint32_t sec_pol_ctl_en[20];
    uint32_t reserved5[12];
    uint32_t int_pol_ctl[20];
};

STATIC_ASSERT(__offsetof(struct mtk_mcucfg_regs, sec_pol_ctl_en) == 0xa00);
STATIC_ASSERT(__offsetof(struct mtk_mcucfg_regs, int_pol_ctl) == 0xa80);
STATIC_ASSERT(__offsetof(struct mtk_mcucfg_regs, aclken_div) == 0x640);
STATIC_ASSERT(__offsetof(struct mtk_mcucfg_regs, mp0_pll_div_cfg) == 0x7a0);
STATIC_ASSERT(__offsetof(struct mtk_mcucfg_regs, bus_pll_div_cfg) == 0x7c0);
STATIC_ASSERT(__offsetof(struct mtk_mcucfg_regs, sec_pol_ctl_en) == 0xa00);
STATIC_ASSERT(__offsetof(struct mtk_mcucfg_regs, int_pol_ctl) == 0xa80);

static struct mtk_mcucfg_regs *const mcucfg = (void *)MCUCFG_BASE;
