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

struct mtk_infracfg_regs {
    uint32_t reserved1[29];
    uint32_t peri_bus_dcm_ctrl;
    uint32_t reserved2[2];
    uint32_t module_sw_cg_0_set;
    uint32_t module_sw_cg_0_clr;
    uint32_t module_sw_cg_1_set;
    uint32_t module_sw_cg_1_clr;
    uint32_t reserved3[5];
    uint32_t module_sw_cg_2_set;
    uint32_t module_sw_cg_2_clr;
    uint32_t module_sw_cg_2_sta;
    uint32_t reserved4[5];
    uint32_t module_sw_cg_3_set;
    uint32_t module_sw_cg_3_clr;
    uint32_t module_sw_cg_3_sta;
    uint32_t reservd5[1];
    uint32_t module_sw_cg_4_set;
    uint32_t module_sw_cg_4_clr;
    uint32_t module_sw_cg_4_sta;
    uint32_t reserved5[72];
    uint32_t topaxi_si0_ctl;
};

STATIC_ASSERT(__offsetof(struct mtk_infracfg_regs, peri_bus_dcm_ctrl) == 0x74);
STATIC_ASSERT(__offsetof(struct mtk_infracfg_regs, module_sw_cg_0_clr) == 0x84);
STATIC_ASSERT(__offsetof(struct mtk_infracfg_regs, module_sw_cg_1_clr) == 0x8c);
STATIC_ASSERT(__offsetof(struct mtk_infracfg_regs, module_sw_cg_2_clr) == 0xa8);
STATIC_ASSERT(__offsetof(struct mtk_infracfg_regs, module_sw_cg_3_clr) == 0xc8);
STATIC_ASSERT(__offsetof(struct mtk_infracfg_regs, module_sw_cg_4_clr) == 0xd8);
STATIC_ASSERT(__offsetof(struct mtk_infracfg_regs, topaxi_si0_ctl) == 0x200);

static struct mtk_infracfg_regs *const infracfg = (void *)INFRACFG_AO_BASE;
