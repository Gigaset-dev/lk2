/*
 * Copyright (c) 2019 MediaTek Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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
