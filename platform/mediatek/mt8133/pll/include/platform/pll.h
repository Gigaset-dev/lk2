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
#include <platform/pll_common.h>
#include <stdint.h>

struct mtk_topckgen_regs {
    uint32_t clk_mod;
    uint32_t clk_cfg_update;
    uint32_t clk_cfg_update1;
    uint32_t reserved1[13];
    uint32_t clk_cfg_0;
    uint32_t reserved2[3];
    uint32_t clk_cfg_1;
    uint32_t reserved3[3];
    uint32_t clk_cfg_2;
    uint32_t reserved4[3];
    uint32_t clk_cfg_3;
    uint32_t reserved5[3];
    uint32_t clk_cfg_4;
    uint32_t reserved6[3];
    uint32_t clk_cfg_5;
    uint32_t reserved7[3];
    uint32_t clk_cfg_6;
    uint32_t reserved8[3];
    uint32_t clk_cfg_7;
    uint32_t reserved9[3];
    uint32_t clk_cfg_8;
    uint32_t reserved10[3];
    uint32_t clk_cfg_9;
    uint32_t reserved11[3];
    uint32_t clk_cfg_10;
    uint32_t reserved12[8];
    uint32_t clk_misc_cfg_0;
    uint32_t clk_misc_cfg_1;
    uint32_t clk_dbg_cfg;
    uint32_t reserved13[68];
    uint32_t clk26cali_0;
    uint32_t clk26cali_1;
    uint32_t reserved14[62];
    uint32_t clk_auddiv_0;
};

STATIC_ASSERT(__offsetof(struct mtk_topckgen_regs, clk_cfg_update) == 0x4);
STATIC_ASSERT(__offsetof(struct mtk_topckgen_regs, clk_cfg_update1) == 0x8);
STATIC_ASSERT(__offsetof(struct mtk_topckgen_regs, clk_cfg_0) == 0x40);
STATIC_ASSERT(__offsetof(struct mtk_topckgen_regs, clk_cfg_1) == 0x50);
STATIC_ASSERT(__offsetof(struct mtk_topckgen_regs, clk_cfg_2) == 0x60);
STATIC_ASSERT(__offsetof(struct mtk_topckgen_regs, clk_cfg_3) == 0x70);
STATIC_ASSERT(__offsetof(struct mtk_topckgen_regs, clk_cfg_4) == 0x80);
STATIC_ASSERT(__offsetof(struct mtk_topckgen_regs, clk_cfg_5) == 0x90);
STATIC_ASSERT(__offsetof(struct mtk_topckgen_regs, clk_cfg_6) == 0xa0);
STATIC_ASSERT(__offsetof(struct mtk_topckgen_regs, clk_cfg_7) == 0xb0);
STATIC_ASSERT(__offsetof(struct mtk_topckgen_regs, clk_cfg_8) == 0xc0);
STATIC_ASSERT(__offsetof(struct mtk_topckgen_regs, clk_cfg_9) == 0xd0);
STATIC_ASSERT(__offsetof(struct mtk_topckgen_regs, clk_cfg_10) == 0xe0);
STATIC_ASSERT(__offsetof(struct mtk_topckgen_regs, clk_misc_cfg_0) == 0x104);
STATIC_ASSERT(__offsetof(struct mtk_topckgen_regs, clk_dbg_cfg) == 0x10c);
STATIC_ASSERT(__offsetof(struct mtk_topckgen_regs, clk26cali_0) == 0x220);
STATIC_ASSERT(__offsetof(struct mtk_topckgen_regs, clk26cali_1) == 0x224);
STATIC_ASSERT(__offsetof(struct mtk_topckgen_regs, clk_auddiv_0) == 0x320);

struct mtk_apmixed_regs {
    uint32_t ap_pll_con0;
    uint32_t reserved1[16];
    uint32_t pllon_con0;
    uint32_t reserved2[111];
    uint32_t ap_pllgp_con1;
    uint32_t univpll_con0;
    uint32_t univpll_con1;
    uint32_t univpll_con2;
    uint32_t univpll_con3;
    uint32_t mfgpll_con0;
    uint32_t mfgpll_con1;
    uint32_t mfgpll_con2;
    uint32_t mfgpll_con3;
    uint32_t mainpll_con0;
    uint32_t mainpll_con1;
    uint32_t mainpll_con2;
    uint32_t mainpll_con3;
    uint32_t reserved3[51];
    uint32_t ap_pllgp2_con1;
    uint32_t reserved4[1];
    uint32_t armpll_con0;
    uint32_t armpll_con1;
    uint32_t armpll_con2;
    uint32_t armpll_con3;
    uint32_t apll1_con0;
    uint32_t apll1_con1;
    uint32_t apll1_con2;
    uint32_t apll1_con3;
    uint32_t apll1_con4;
    uint32_t mmpll_con0;
    uint32_t mmpll_con1;
    uint32_t mmpll_con2;
    uint32_t mmpll_con3;
    uint32_t mpll_con0;
    uint32_t mpll_con1;
    uint32_t mpll_con2;
    uint32_t mpll_con3;
    uint32_t msdcpll_con0;
    uint32_t msdcpll_con1;
    uint32_t msdcpll_con2;
    uint32_t msdcpll_con3;
    uint32_t apll2_con0;
    uint32_t apll2_con1;
    uint32_t apll2_con2;
    uint32_t apll2_con3;
    uint32_t apll2_con4;
    uint32_t lvdspll_con0;
    uint32_t lvdspll_con1;
    uint32_t lvdspll_con2;
    uint32_t lvdspll_con3;
    uint32_t reserved5[3];
    uint32_t dsppll_con0;
    uint32_t dsppll_con1;
    uint32_t dsppll_con2;
    uint32_t dsppll_con3;
    uint32_t apupll_con0;
    uint32_t apupll_con1;
    uint32_t apupll_con2;
    uint32_t apupll_con3;
};

STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, ap_pll_con0) == 0x0);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, pllon_con0) == 0x44);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, ap_pllgp_con1) == 0x204);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, univpll_con0) == 0x208);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, mfgpll_con0) == 0x218);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, mainpll_con0) == 0x228);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, ap_pllgp2_con1) == 0x304);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, armpll_con0) == 0x30c);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, apll1_con0) == 0x31c);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, mmpll_con0) == 0x330);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, mpll_con0) == 0x340);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, msdcpll_con0) == 0x350);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, apll2_con0) == 0x360);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, lvdspll_con0) == 0x374);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, dsppll_con0) == 0x390);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, apupll_con0) == 0x3a0);

enum {
        PCW_INTEGER_BITS = 8,
};

void mt_fmeter_dump(void);
