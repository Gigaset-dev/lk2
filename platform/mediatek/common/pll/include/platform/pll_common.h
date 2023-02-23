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

#define KHz (1000U)
#define MHz (1000*KHz)
#define GHz (1000*MHz)

/* These need to be macros for use in static initializers. */
#define mtk_topckgen ((struct mtk_topckgen_regs *)CKSYS_BASE)
#define mtk_apmixed  ((struct mtk_apmixed_regs *)APMIXED_BASE)

enum {
    PLL_EN = 1 << 0,
};

enum {
    PLL_PWR_ON = 1 << 0,
    PLL_ISO    = 1 << 1,
};

enum {
    NO_RSTB_SHIFT = 255,
};

enum {
    PLL_POSTDIV_MASK = 0x7,
};

enum {
    PLL_PCW_CHG = 1 << 31,
};

struct mux {
    void *reg;
    void *set_reg;
    void *clr_reg;
    void *upd_reg;
    unsigned char mux_shift;
    unsigned char mux_width;
    unsigned char upd_shift;
};

struct pll {
    void *reg;
    void *pwr_reg;
    void *div_reg;
    void *pcw_reg;
    const unsigned int *div_rate;
    unsigned char rstb_shift;
    unsigned char pcwbits;
    unsigned char div_shift;
    unsigned char pcw_shift;
};

#define PLL(_id, _reg, _pwr_reg, _rstb, _pcwbits, _div_reg, _div_shift, \
            _pcw_reg, _pcw_shift, _div_rate)                            \
    [_id] = {                                                           \
        .reg = &mtk_apmixed->_reg,                                      \
        .pwr_reg = &mtk_apmixed->_pwr_reg,                              \
        .rstb_shift = _rstb,                                            \
        .pcwbits = _pcwbits,                                            \
        .div_reg = &mtk_apmixed->_div_reg,                              \
        .div_shift = _div_shift,                                        \
        .pcw_reg = &mtk_apmixed->_pcw_reg,                              \
        .pcw_shift = _pcw_shift,                                        \
        .div_rate = _div_rate,                                          \
    }

void mux_set_sel(const struct mux *mux, u32 sel);
void pll_set_rate(const struct pll *pll, unsigned int rate);

void pll_set_pcw_change(const struct pll *pll);
void mt_pll_init(void);
