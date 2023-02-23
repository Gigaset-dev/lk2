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

#include <platform/addressmap.h>
#include <compiler.h>
#include <stdint.h>

struct mt6779_mcucfg_regs {
    uint32_t reserved1[10624];
    uint32_t sec_pol_ctl_en[20];
    uint32_t int_pol_ctl[20];
};

STATIC_ASSERT(__offsetof(struct mt6779_mcucfg_regs, sec_pol_ctl_en) == 0xa600);
STATIC_ASSERT(__offsetof(struct mt6779_mcucfg_regs, int_pol_ctl) == 0xa650);

static struct mt6779_mcucfg_regs *const mcucfg = (void *)MCUCFG_BASE;
