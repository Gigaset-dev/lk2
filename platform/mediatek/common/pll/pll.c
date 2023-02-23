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
#include <assert.h>
#include <platform/pll.h>
#include <platform/reg.h>

#define GENMASK(h, l) ((1ul << ((h) + 1)) - (1ul << (l)))

void mux_set_sel(const struct mux *mux, u32 sel)
{
    u32 mask = GENMASK(mux->mux_width - 1, 0);
    u32 val = read32(mux->reg);

    assert((sel & mask) == sel);

    if (mux->set_reg && mux->clr_reg) {
        write32(mux->clr_reg, mask << mux->mux_shift);
        write32(mux->set_reg, sel << mux->mux_shift);
    } else {
        val &= ~(mask << mux->mux_shift);
        val |= (sel & mask) << mux->mux_shift;
        write32(mux->reg, val);
    }

    if (mux->upd_reg)
        write32(mux->upd_reg, 1 << mux->upd_shift);
}

#define CLK26M_HZ (26 * MHz)

static void pll_calc_values(const struct pll *pll, unsigned int *pcw, unsigned int *postdiv,
                            unsigned int freq)
{
    const unsigned int fin_hz = CLK26M_HZ;
    const unsigned int *div_rate = pll->div_rate;
    unsigned int val;

    assert(freq <= div_rate[0]);
    assert(freq >= 1 * GHz / 16);

    for (val = 1; div_rate[val] != 0; val++) {
        if (freq > div_rate[val])
            break;
    }
    val--;
    *postdiv = val;

    /* _pcw = freq * 2^postdiv / fin * 2^pcwbits_fractional */
    val += pll->pcwbits - PCW_INTEGER_BITS;

    *pcw = ((u64)freq << val) / fin_hz;
}

static void pll_set_rate_regs(const struct pll *pll, unsigned int pcw, unsigned int postdiv)
{
    unsigned int val;

    /* set postdiv */
    val = read32(pll->div_reg);
    val &= ~(PLL_POSTDIV_MASK << pll->div_shift);
    val |= postdiv << pll->div_shift;

    /* set postdiv and pcw at the same time if on the same register */
    if (pll->div_reg != pll->pcw_reg) {
        write32(pll->div_reg, val);
        val = read32(pll->pcw_reg);
    }

    /* set pcw */
    val &= ~GENMASK(pll->pcw_shift + pll->pcwbits - 1, pll->pcw_shift);
    val |= pcw << pll->pcw_shift;
    write32(pll->pcw_reg, val);

    pll_set_pcw_change(pll);
}

void pll_set_rate(const struct pll *pll, unsigned int rate)
{
    unsigned int pcw, postdiv;

    pll_calc_values(pll, &pcw, &postdiv, rate);
    pll_set_rate_regs(pll, pcw, postdiv);
}
