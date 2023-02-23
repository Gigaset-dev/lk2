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
#include <platform/pll.h>
#include <platform/reg.h>

const char *abist_array[] = {
    "AD_ARMPLL_CK",
    "0",
    "AD_MAINPLLCK",
    "AD_CSI0A_CDPHY_DELAYCAL_CK",
    "AD_CSI0B_CDPHY_DELAYCAL_CK",
    "0",
    "AD_USB20_CLK480M",
    "AD_USB20_CLK480M_1P",
    "AD_MADADC_26MCKO",
    "AD_MAINPLL_H546M_CK",
    "AD_MAINPLL_H364M_CK",
    "AD_MAINPLL_H218P4M_CK",
    "AD_MAINPLL_H156M_CK",
    "AD_UNIVPLL_1248M_CK",
    "AD_USB20_192M_CK",
    "AD_UNIVPLL_624M_CK",
    "AD_UNIVPLL_416M_CK",
    "AD_UNIVPLL_249P6M_CK",
    "AD_UNIVPLL_178P3M_CK",
    "AD_SYS_26M_CK",
    "AD_CSI1A_DPHY_DELAYCAL_CK",
    "AD_CSI1B_DPHY_DELAYCAL_CK",
    "AD_CSI2A_DPHY_DELAYCAL_CK",
    "AD_CSI2B_DPHY_DELAYCAL_CK",
    "RTC32K",
    "AD_MMPLL_CK",
    "AD_MFGPLL_CK",
    "AD_MSDCPLL_CK",
    "AD_DSI0_LNTC_DSICLK",
    "AD_DSI0_MPPLL_TST_CK",
    "AD_APPLLGP_TST_CK",
    "AD_APLL1_180P6336M_CK",
    "AD_APLL1_196P608M_CK",
    "AD_MADCKO_TEST",
    "AD_MPLL_208M_CK",
    "Armpll_ll_mon_ck",
    "vad_clk_i",
    "msdc01_in_ck",
    "0",
    "msdc11_in_ck",
    "msdc12_in_ck",
    "AD_PLLGP_TST_CK",
    "AD_LVDSTX_CLKDIG_CTS",
    "AD_LVDSTX_CLKDIG",
    "AD_VPLL_DPIX_CK",
    "DA_USB20_48M_DIV_CK",
    "DA_UNIV_48M_DIV_CK",
    "DA_MPLL_104M_DIV_CK",
    "DA_MPLL_52M_DIV_CK",
    "DA_PLLGP_CPU_CK_MON",
    "trng_freq_debug_out0",
    "trng_freq_debug_out1",
    "AD_LVDSTX_MONCLK",
    "AD_VPLL_MONREF_CK",
    "AD_VPLL_MONFBK_CK",
    "AD_LVDSPLL_300M_CK",
    "AD_DSPPLL_CK",
    "AD_APUPLL_CK",
};

const char *ckgen_array[] = {
    "axi_ck",
    "mem_ck",
    "mm_ck",
    "scp_ck",
    "mfg_ck",
    "atb_ck",
    "camtg_ck",
    "camtg1_ck",
    "uart_ck",
    "f_fspi_ck",
    "msdc50_0_hclk_ck",
    "fmsdc2_2_hclk_ck",
    "msdc50_0_ck",
    "msdc50_2_ck",
    "msdc30_1_ck",
    "audio_ck",
    "aud_intbus_ck",
    "aud_1_ck",
    "aud_2_ck",
    "aud_engen1_ck",
    "aud_engen2_ck",
    "hf_faud_spdif_ck",
    "disp_pwm_ck",
    "sspm_ck",
    "dxcc_ck",
    "ssusb_sys_ck",
    "ssusb_xhci_ck",
    "spm_ck",
    "i2c_ck",
    "pwm_ck",
    "seninf_ck",
    "aes_fde_ck",
    "camtm_ck",
    "dpi0_ck",
    "dpi1_ck",
    "dsp_ck",
    "nfi2x_ck",
    "nfiecc_ck",
    "ecc_ck",
    "eth_ck",
    "gcpu_ck",
    "gcpu_cpm_ck",
    "apu_ck",
    "apu_if_ck",
    "mbist_diag_clk",
    "f_ufs_mp_sap_cfg_ck",
    "f_ufs_tick1us_ck",
    "NULL",
    "NULL",
    "NULL",
    "NULL",
};

enum freq_type {
    CKGEN = 0,
    ABIST,
};

static unsigned int mt_get_freq(unsigned int id, enum freq_type type)
{
    unsigned int clk_dbg_cfg, clk_misc_cfg_0, clk26cali_0, clk26cali_1;
    unsigned int shift, i = 0, output;

    /* Save default value */
    clk_dbg_cfg = read32(&mtk_topckgen->clk_dbg_cfg);
    clk_misc_cfg_0 = read32(&mtk_topckgen->clk_misc_cfg_0);
    clk26cali_0 = read32(&mtk_topckgen->clk26cali_0);
    clk26cali_1 = read32(&mtk_topckgen->clk26cali_1);

    shift = (type == CKGEN) ? 8 : 16;
    clrsetbits32(&mtk_topckgen->clk_dbg_cfg, (0x3f << shift) | (0x3),
                 (id << shift) | ((type == CKGEN) ? 1 : 0));
    clrsetbits32(&mtk_topckgen->clk_misc_cfg_0, 0xff << 24,
                 ((type == CKGEN) ? 0 : 0x3) << 24);
    write32(&mtk_topckgen->clk26cali_0, 0x1000);
    write32(&mtk_topckgen->clk26cali_0, 0x1010);
    /* Wait frequency meter finish */
    while (read32(&mtk_topckgen->clk26cali_0) & 0x10) {
        spin(10000);
        if (++i > 10)
            break;
    }
    output = (read32(&mtk_topckgen->clk26cali_1) & 0xffff) * 26000 / 1024;
    if (type != CKGEN)
        output *= 4;

    /* Restore default value */
    write32(&mtk_topckgen->clk_dbg_cfg, clk_dbg_cfg);
    write32(&mtk_topckgen->clk_misc_cfg_0, clk_misc_cfg_0);
    write32(&mtk_topckgen->clk26cali_0, clk26cali_0);
    write32(&mtk_topckgen->clk26cali_1, clk26cali_1);

    return output;
}

void mt_fmeter_dump(void)
{
    unsigned int i;

    dprintf(CRITICAL, "abist (KHz):\n");
    for (i = 1; i <= countof(abist_array); i++)
        dprintf(CRITICAL, "[%u] %s: %u\n", i, abist_array[i - 1],
                mt_get_freq(i, ABIST));

    dprintf(CRITICAL, "ckgen (KHz):\n");
    for (i = 1; i <= countof(ckgen_array); i++)
        dprintf(CRITICAL, "[%u] %s: %u\n", i, ckgen_array[i - 1],
                mt_get_freq(i, CKGEN));
}
