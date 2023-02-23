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
#include <debug.h>
#include <platform/infracfg.h>
#include <platform/mcucfg.h>
#include <platform/pll.h>
#include <platform/reg.h>

struct mtk_pericfg_reg {
    uint32_t reserved[131];
    uint32_t periaxi_si0_ctl;
};

STATIC_ASSERT(__offsetof(struct mtk_pericfg_reg, periaxi_si0_ctl) == 0x20c);

static struct mtk_pericfg_reg *const pericfg = (void *)PERICFG_BASE;

enum mux_id {
    /* CLK_CFG_0 */
    TOP_AXI_SEL = 0,
    TOP_MEM_SEL,
    TOP_MM_SEL,
    TOP_SCP_SEL,
    /* CLK_CFG_1 */
    TOP_MFG_SEL,
    TOP_ATB_SEL,
    TOP_CAMTG_SEL,
    TOP_CAMTG1_SEL,
    /* CLK_CFG_2 */
    TOP_UART_SEL,
    TOP_SPI_SEL,
    TOP_MSDC50_0_HCLK_SEL,
    TOP_MSDC2_2_HCLK_SEL,
    /* CLK_CFG_3 */
    TOP_MSDC50_0_SEL,
    TOP_MSDC50_2_SEL,
    TOP_MSDC30_1_SEL,
    TOP_AUDIO_SEL,
    /* CLK_CFG_4 */
    TOP_AUD_INTBUS_SEL,
    TOP_AUD_1_SEL,
    TOP_AUD_2_SEL,
    TOP_AUD_ENGEN1_SEL,
    /* CLK_CFG_5 */
    TOP_AUD_ENGEN2_SEL,
    TOP_AUD_SPDIF_SEL,
    TOP_DISP_PWM_SEL,
    /* CLK_CFG_6 */
    TOP_DXCC_SEL,
    TOP_SSUSB_SYS_SEL,
    TOP_SSUSB_XHCI_SEL,
    TOP_SPM_SEL,
    /* CLK_CFG_7 */
    TOP_I2C_SEL,
    TOP_PWM_SEL,
    TOP_SENINF_SEL,
    TOP_AES_FDE_SEL,
    /* CLK_CFG_8 */
    TOP_CAMTM_SEL,
    TOP_DPI0_SEL,
    TOP_DPI1_SEL,
    TOP_DSP_SEL,
    /* CLK_CFG_9 */
    TOP_NFI2X_SEL,
    TOP_NFIECC_SEL,
    TOP_ECC_SEL,
    TOP_ETH_SEL,
    /* CLK_CFG_10 */
    TOP_GCPU_SEL,
    TOP_GCPU_CPM_SEL,
    TOP_APU_SEL,
    TOP_APU_IF_SEL,
    TOP_NR_MUX
};

#define MUX(_id, _reg, _mux_shift, _mux_width, _upd_reg, _upd_shift) \
    [_id] = {                                                        \
        .reg = &mtk_topckgen->_reg,                                  \
        .mux_shift = _mux_shift,                                     \
        .mux_width = _mux_width,                                     \
        .upd_reg = &mtk_topckgen->_upd_reg,                          \
        .upd_shift = _upd_shift,                                     \
    }

static const struct mux muxes[] = {
    /* CLK_CFG_0 */
    MUX(TOP_AXI_SEL, clk_cfg_0, 0, 2, clk_cfg_update, 0),
    MUX(TOP_MEM_SEL, clk_cfg_0, 8, 2, clk_cfg_update, 1),
    MUX(TOP_MM_SEL, clk_cfg_0, 16, 3, clk_cfg_update, 2),
    MUX(TOP_SCP_SEL, clk_cfg_0, 24, 3, clk_cfg_update, 3),
    /* CLK_CFG_1 */
    MUX(TOP_MFG_SEL, clk_cfg_1, 0, 2, clk_cfg_update, 4),
    MUX(TOP_ATB_SEL, clk_cfg_1, 8, 2, clk_cfg_update, 5),
    MUX(TOP_CAMTG_SEL, clk_cfg_1, 16, 3, clk_cfg_update, 6),
    MUX(TOP_CAMTG1_SEL, clk_cfg_1, 24, 3, clk_cfg_update, 7),
    /* CLK_CFG_2 */
    MUX(TOP_UART_SEL, clk_cfg_2, 0, 1, clk_cfg_update, 8),
    MUX(TOP_SPI_SEL, clk_cfg_2, 8, 2, clk_cfg_update, 9),
    MUX(TOP_MSDC50_0_HCLK_SEL, clk_cfg_2, 16, 3, clk_cfg_update, 10),
    MUX(TOP_MSDC2_2_HCLK_SEL, clk_cfg_2, 24, 3, clk_cfg_update, 11),
    /* CLK_CFG_3 */
    MUX(TOP_MSDC50_0_SEL, clk_cfg_3, 0, 3, clk_cfg_update, 12),
    MUX(TOP_MSDC50_2_SEL, clk_cfg_3, 8, 3, clk_cfg_update, 13),
    MUX(TOP_MSDC30_1_SEL, clk_cfg_3, 16, 3, clk_cfg_update, 14),
    MUX(TOP_AUDIO_SEL, clk_cfg_3, 24, 2, clk_cfg_update, 15),
    /* CLK_CFG_4 */
    MUX(TOP_AUD_INTBUS_SEL, clk_cfg_4, 0, 2, clk_cfg_update, 16),
    MUX(TOP_AUD_1_SEL, clk_cfg_4, 8, 1, clk_cfg_update, 17),
    MUX(TOP_AUD_2_SEL, clk_cfg_4, 16, 1, clk_cfg_update, 18),
    MUX(TOP_AUD_ENGEN1_SEL, clk_cfg_4, 24, 2, clk_cfg_update, 19),
    /* CLK_CFG_5 */
    MUX(TOP_AUD_ENGEN2_SEL, clk_cfg_5, 0, 2, clk_cfg_update, 20),
    MUX(TOP_AUD_SPDIF_SEL, clk_cfg_5, 8, 1, clk_cfg_update, 21),
    MUX(TOP_DISP_PWM_SEL, clk_cfg_5, 16, 2, clk_cfg_update, 22),
    /* CLK_CFG_6 */
    MUX(TOP_DXCC_SEL, clk_cfg_6, 0, 2, clk_cfg_update, 24),
    MUX(TOP_SSUSB_SYS_SEL, clk_cfg_6, 8, 2, clk_cfg_update, 25),
    MUX(TOP_SSUSB_XHCI_SEL, clk_cfg_6, 16, 2, clk_cfg_update, 26),
    MUX(TOP_SPM_SEL, clk_cfg_6, 24, 1, clk_cfg_update, 27),
    /* CLK_CFG_7 */
    MUX(TOP_I2C_SEL, clk_cfg_7, 0, 3, clk_cfg_update, 28),
    MUX(TOP_PWM_SEL, clk_cfg_7, 8, 2, clk_cfg_update, 29),
    MUX(TOP_SENINF_SEL, clk_cfg_7, 16, 2, clk_cfg_update, 30),
    MUX(TOP_AES_FDE_SEL, clk_cfg_7, 24, 3, clk_cfg_update, 31),
    /* CLK_CFG_8 */
    MUX(TOP_CAMTM_SEL, clk_cfg_8, 0, 2, clk_cfg_update1, 0),
    MUX(TOP_DPI0_SEL, clk_cfg_8, 8, 3, clk_cfg_update1, 1),
    MUX(TOP_DPI1_SEL, clk_cfg_8, 16, 3, clk_cfg_update1, 2),
    MUX(TOP_DSP_SEL, clk_cfg_8, 24, 3, clk_cfg_update1, 3),
    /* CLK_CFG_9 */
    MUX(TOP_NFI2X_SEL, clk_cfg_9, 0, 3, clk_cfg_update1, 4),
    MUX(TOP_NFIECC_SEL, clk_cfg_9, 8, 3, clk_cfg_update1, 5),
    MUX(TOP_ECC_SEL, clk_cfg_9, 16, 3, clk_cfg_update1, 6),
    MUX(TOP_ETH_SEL, clk_cfg_9, 24, 3, clk_cfg_update1, 7),
    /* CLK_CFG_10 */
    MUX(TOP_GCPU_SEL, clk_cfg_10, 0, 3, clk_cfg_update1, 8),
    MUX(TOP_GCPU_CPM_SEL, clk_cfg_10, 8, 2, clk_cfg_update1, 9),
    MUX(TOP_APU_SEL, clk_cfg_10, 16, 3, clk_cfg_update1, 10),
    MUX(TOP_APU_IF_SEL, clk_cfg_10, 24, 3, clk_cfg_update1, 11),
};

struct mux_sel {
    enum mux_id id;
    unsigned int sel;
};

static const struct mux_sel mux_sels[] = {
    /* CLK_CFG_0 */
    { .id = TOP_AXI_SEL, .sel = 1 },
    { .id = TOP_MEM_SEL, .sel = 1 },
    { .id = TOP_MM_SEL, .sel = 1 },
    { .id = TOP_SCP_SEL, .sel = 6 },
    /* CLK_CFG_1 */
    { .id = TOP_MFG_SEL, .sel = 1 },
    { .id = TOP_ATB_SEL, .sel = 2 },
    { .id = TOP_CAMTG_SEL, .sel = 2 },
    { .id = TOP_CAMTG1_SEL, .sel = 2 },
    /* CLK_CFG_2 */
    { .id = TOP_UART_SEL, .sel = 1 },
    { .id = TOP_SPI_SEL, .sel = 1 },
    { .id = TOP_MSDC50_0_HCLK_SEL, .sel = 1 },
    { .id = TOP_MSDC2_2_HCLK_SEL, .sel = 1 },
    /* CLK_CFG_3 */
    { .id = TOP_MSDC50_0_SEL, .sel = 1 },
    { .id = TOP_MSDC50_2_SEL, .sel = 1 },
    { .id = TOP_MSDC30_1_SEL, .sel = 2 },
    { .id = TOP_AUDIO_SEL, .sel = 1 },
    /* CLK_CFG_4 */
    { .id = TOP_AUD_INTBUS_SEL, .sel = 1 },
    { .id = TOP_AUD_1_SEL, .sel = 1 },
    { .id = TOP_AUD_2_SEL, .sel = 1 },
    { .id = TOP_AUD_ENGEN1_SEL, .sel = 3 },
    /* CLK_CFG_5 */
    { .id = TOP_AUD_ENGEN2_SEL, .sel = 3 },
    { .id = TOP_AUD_SPDIF_SEL, .sel = 1 },
    { .id = TOP_DISP_PWM_SEL, .sel = 1 },
    /* CLK_CFG_6 */
    { .id = TOP_DXCC_SEL, .sel = 1 },
    { .id = TOP_SSUSB_SYS_SEL, .sel = 3 },
    { .id = TOP_SSUSB_XHCI_SEL, .sel = 3 },
    { .id = TOP_SPM_SEL, .sel = 1 },
    /* CLK_CFG_7 */
    { .id = TOP_I2C_SEL, .sel = 2 },
    { .id = TOP_PWM_SEL, .sel = 2 },
    { .id = TOP_SENINF_SEL, .sel = 2 },
    { .id = TOP_AES_FDE_SEL, .sel = 1 },
    /* CLK_CFG_8 */
    { .id = TOP_CAMTM_SEL, .sel = 3 },
    { .id = TOP_DPI0_SEL, .sel = 1 },
    { .id = TOP_DPI1_SEL, .sel = 1 },
    { .id = TOP_DSP_SEL, .sel = 2 },
    /* CLK_CFG_9 */
    { .id = TOP_NFI2X_SEL, .sel = 3 },
    { .id = TOP_NFIECC_SEL, .sel = 4 },
    { .id = TOP_ECC_SEL, .sel = 2 },
    { .id = TOP_ETH_SEL, .sel = 4 },
    /* CLK_CFG_10 */
    { .id = TOP_GCPU_SEL, .sel = 3 },
    { .id = TOP_GCPU_CPM_SEL, .sel = 1 },
    { .id = TOP_APU_SEL, .sel = 1 },
    { .id = TOP_APU_IF_SEL, .sel = 1 },
};

enum {
    MUX_MASK = 0x3 << 9,
    MUX_SRC_ARMPLL = 0x1 << 9,
};

enum pll_id {
    APMIXED_ARMPLL,
    APMIXED_MFGPLL,
    APMIXED_MPLL,
    APMIXED_MAINPLL,
    APMIXED_UNIVPLL,
    APMIXED_MSDCPLL,
    APMIXED_MMPLL,
    APMIXED_APLL1,
    APMIXED_APLL2,
    APMIXED_LVDSPLL,
    APMIXED_DSPPLL,
    APMIXED_APUPLL,
    APMIXED_PLL_MAX
};

static const unsigned int pll_div_rate[] = {
    3800UL * MHz,
    1900 * MHz,
    950 * MHz,
    475 * MHz,
    237500 * KHz,
    0,
};

static const unsigned int armpll_div_rate[] = {
    3800UL * MHz,
    1500 * MHz,
    750 * MHz,
    375 * MHz,
    182500 * KHz,
    0,
};

static const unsigned int mfgpll_div_rate[] = {
    3800UL * MHz,
    1600 * MHz,
    800 * MHz,
    400 * MHz,
    200 * MHz,
    0,
};

static const unsigned int dsppll_div_rate[] = {
    3800UL * MHz,
    1600 * MHz,
    600 * MHz,
    400 * MHz,
    200 * MHz,
    0,
};

static const struct pll plls[] = {
    PLL(APMIXED_ARMPLL, armpll_con0, armpll_con3, NO_RSTB_SHIFT, 22,
        armpll_con1, 24, armpll_con1, 0, armpll_div_rate),
    PLL(APMIXED_MFGPLL, mfgpll_con0, mfgpll_con3, NO_RSTB_SHIFT, 22,
        mfgpll_con1, 24, mfgpll_con1, 0, mfgpll_div_rate),
    PLL(APMIXED_MPLL, mpll_con0, mpll_con3, NO_RSTB_SHIFT, 22,
        mpll_con1, 24, mpll_con1, 0, pll_div_rate),
    PLL(APMIXED_MAINPLL, mainpll_con0, mainpll_con3, 23, 22,
        mainpll_con1, 24, mainpll_con1, 0, pll_div_rate),
    PLL(APMIXED_UNIVPLL, univpll_con0, univpll_con3, 23, 22,
        univpll_con1, 24, univpll_con1, 0, pll_div_rate),
    PLL(APMIXED_MSDCPLL, msdcpll_con0, msdcpll_con3, NO_RSTB_SHIFT, 22,
        msdcpll_con1, 24, msdcpll_con1, 0, pll_div_rate),
    PLL(APMIXED_MMPLL, mmpll_con0, mmpll_con3, NO_RSTB_SHIFT, 22,
        mmpll_con1, 24, mmpll_con1, 0, pll_div_rate),
    PLL(APMIXED_APLL1, apll1_con0, apll1_con4, NO_RSTB_SHIFT, 32,
        apll1_con1, 24, apll1_con2, 0, pll_div_rate),
    PLL(APMIXED_APLL2, apll2_con0, apll2_con4, NO_RSTB_SHIFT, 32,
        apll2_con1, 24, apll2_con2, 0, pll_div_rate),
    PLL(APMIXED_LVDSPLL, lvdspll_con0, lvdspll_con3, NO_RSTB_SHIFT, 22,
        lvdspll_con1, 24, lvdspll_con1, 0, pll_div_rate),
    PLL(APMIXED_DSPPLL, dsppll_con0, dsppll_con3, NO_RSTB_SHIFT, 22,
        dsppll_con1, 24, dsppll_con1, 0, dsppll_div_rate),
    PLL(APMIXED_APUPLL, apupll_con0, apupll_con3, NO_RSTB_SHIFT, 22,
        apupll_con1, 24, apupll_con1, 0, pll_div_rate),
};

struct rate {
    enum pll_id id;
    unsigned int rate;
};

static const struct rate rates[] = {
    { .id = APMIXED_ARMPLL, .rate = 1400 * MHz },
    { .id = APMIXED_MFGPLL, .rate = 560 * MHz },
    { .id = APMIXED_MSDCPLL, .rate = 390 * MHz },
    { .id = APMIXED_MMPLL, .rate = 320 * MHz },
    { .id = APMIXED_DSPPLL, .rate = 400 * MHz },
    { .id = APMIXED_APUPLL, .rate = 400 * MHz },
};

void pll_set_pcw_change(const struct pll *pll)
{
    setbits32(pll->div_reg, PLL_PCW_CHG);
}

void mt_pll_init(void)
{
    size_t i;

    write32(&mcucfg->aclken_div, 0x12); // MCU Bus DIV2
    setbits32(&mtk_apmixed->ap_pll_con0, 0x1); // CLKSQ_EN
    spin(100);
    setbits32(&mtk_apmixed->ap_pll_con0, 0x2); // CLKSQ_LPF_EN

    for (i = 0; i < APMIXED_PLL_MAX; i++)
        setbits32(plls[i].pwr_reg, PLL_PWR_ON);

    spin(30);

    for (i = 0; i < APMIXED_PLL_MAX; i++)
        clrbits32(plls[i].pwr_reg, PLL_ISO);

    spin(1);

    for (i = 0; i < countof(rates); i++)
        pll_set_rate(&plls[rates[i].id], rates[i].rate);

    for (i = 0; i < APMIXED_PLL_MAX; i++)
        setbits32(plls[i].reg, PLL_EN);

    setbits32(&mtk_apmixed->mainpll_con0, 0xff000180); // MAINPLL_DIV_EN
    setbits32(&mtk_apmixed->univpll_con0, 0xff000080); // UNIVPLL_DIV_EN

    spin(20);

    for (i = 0; i < APMIXED_PLL_MAX; i++) {
        if (plls[i].rstb_shift != NO_RSTB_SHIFT)
            setbits32(plls[i].reg, 0x1 << plls[i].rstb_shift);
    }

    spin(20);

    clrbits32(&mtk_apmixed->pllon_con0, 0x1 << 6); // use HW delay mode (by_maindiv_dly)

    clrsetbits32(&mcucfg->mp0_pll_div_cfg, MUX_MASK, MUX_SRC_ARMPLL);
    clrsetbits32(&mcucfg->mp1_pll_div_cfg, MUX_MASK, MUX_SRC_ARMPLL);
    clrsetbits32(&mcucfg->bus_pll_div_cfg, MUX_MASK, MUX_SRC_ARMPLL);

    for (i = 0; i < countof(mux_sels); i++)
        mux_set_sel(&muxes[mux_sels[i].id], mux_sels[i].sel);

    setbits32(&mtk_topckgen->clk_misc_cfg_0, 0x00f13f78);
    clrbits32(&mtk_topckgen->clk_auddiv_0, 0x3f9ff);

    setbits32(&infracfg->topaxi_si0_ctl, 0x80000000);
    clrbits32(&infracfg->peri_bus_dcm_ctrl, 0x4);
    write32(&infracfg->module_sw_cg_0_clr, 0x9dff877f);
    write32(&infracfg->module_sw_cg_1_clr, 0x879c7f96);
    write32(&infracfg->module_sw_cg_2_clr, 0x3ffc87dd);
    write32(&infracfg->module_sw_cg_3_clr, 0x7fffffbf);
    write32(&infracfg->module_sw_cg_4_clr, 0x00000fff);

    setbits32(&pericfg->periaxi_si0_ctl, 0x80000000);

    /* USB fixup */
    setbits32(&mtk_apmixed->univpll_con0, 0x1 << 8);
    clrbits32(&mtk_apmixed->ap_pllgp2_con1, 0x7 << 3);
    spin(20);
    setbits32(&mtk_apmixed->ap_pllgp_con1, 0x1 << 0);
    setbits32(&mtk_apmixed->ap_pllgp_con1, 0x1 << 1);

#if FMETER_EN
    mt_fmeter_dump();
#endif
}
