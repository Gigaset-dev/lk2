/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <compiler.h>
#include <platform/pll_common.h>
#include <stdint.h>

struct mtk_topckgen_regs {
    uint32_t reserved1[1];
    uint32_t clk_cfg_update;    /* 0x0004 */
    uint32_t clk_cfg_update1;
    uint32_t clk_cfg_update2;
    uint32_t clk_cfg_update3;
    uint32_t clk_cfg_update4;
    uint32_t reserved2[2];
    uint32_t clk_cfg_0;         /* 0x0020 */
    uint32_t clk_cfg_0_set;
    uint32_t clk_cfg_0_clr;
    uint32_t clk_cfg_1;
    uint32_t clk_cfg_1_set;
    uint32_t clk_cfg_1_clr;
    uint32_t clk_cfg_2;
    uint32_t clk_cfg_2_set;
    uint32_t clk_cfg_2_clr;
    uint32_t clk_cfg_3;
    uint32_t clk_cfg_3_set;
    uint32_t clk_cfg_3_clr;
    uint32_t clk_cfg_4;
    uint32_t clk_cfg_4_set;
    uint32_t clk_cfg_4_clr;
    uint32_t clk_cfg_5;
    uint32_t clk_cfg_5_set;
    uint32_t clk_cfg_5_clr;
    uint32_t clk_cfg_6;
    uint32_t clk_cfg_6_set;
    uint32_t clk_cfg_6_clr;
    uint32_t clk_cfg_7;
    uint32_t clk_cfg_7_set;
    uint32_t clk_cfg_7_clr;
    uint32_t clk_cfg_8;
    uint32_t clk_cfg_8_set;
    uint32_t clk_cfg_8_clr;
    uint32_t clk_cfg_9;
    uint32_t clk_cfg_9_set;
    uint32_t clk_cfg_9_clr;
    uint32_t clk_cfg_10;
    uint32_t clk_cfg_10_set;
    uint32_t clk_cfg_10_clr;
    uint32_t clk_cfg_11;
    uint32_t clk_cfg_11_set;
    uint32_t clk_cfg_11_clr;
    uint32_t clk_cfg_12;
    uint32_t clk_cfg_12_set;
    uint32_t clk_cfg_12_clr;
    uint32_t clk_cfg_13;
    uint32_t clk_cfg_13_set;
    uint32_t clk_cfg_13_clr;
    uint32_t clk_cfg_14;
    uint32_t clk_cfg_14_set;
    uint32_t clk_cfg_14_clr;
    uint32_t clk_cfg_15;
    uint32_t clk_cfg_15_set;
    uint32_t clk_cfg_15_clr;
    uint32_t clk_cfg_16;
    uint32_t clk_cfg_16_set;
    uint32_t clk_cfg_16_clr;
    uint32_t clk_cfg_17;
    uint32_t clk_cfg_17_set;
    uint32_t clk_cfg_17_clr;
    uint32_t clk_cfg_18;
    uint32_t clk_cfg_18_set;
    uint32_t clk_cfg_18_clr;
    uint32_t clk_cfg_19;
    uint32_t clk_cfg_19_set;
    uint32_t clk_cfg_19_clr;
    uint32_t clk_cfg_20;
    uint32_t clk_cfg_20_set;
    uint32_t clk_cfg_20_clr;
    uint32_t clk_cfg_21;
    uint32_t clk_cfg_21_set;
    uint32_t clk_cfg_21_clr;
    uint32_t clk_cfg_22;
    uint32_t clk_cfg_22_set;
    uint32_t clk_cfg_22_clr;
    uint32_t clk_cfg_23;
    uint32_t clk_cfg_23_set;
    uint32_t clk_cfg_23_clr;
    uint32_t clk_cfg_24;
    uint32_t clk_cfg_24_set;
    uint32_t clk_cfg_24_clr;
    uint32_t clk_cfg_25;
    uint32_t clk_cfg_25_set;
    uint32_t clk_cfg_25_clr;
    uint32_t clk_cfg_26;
    uint32_t clk_cfg_26_set;
    uint32_t clk_cfg_26_clr;
    uint32_t clk_cfg_27;
    uint32_t clk_cfg_27_set;
    uint32_t clk_cfg_27_clr;
    uint32_t clk_cfg_28;
    uint32_t clk_cfg_28_set;
    uint32_t clk_cfg_28_clr;
    uint32_t clk_cfg_29;
    uint32_t clk_cfg_29_set;
    uint32_t clk_cfg_29_clr;
    uint32_t clk_cfg_30;
    uint32_t clk_cfg_30_set;
    uint32_t clk_cfg_30_clr;
    uint32_t clk_cfg_31;
    uint32_t clk_cfg_31_set;
    uint32_t clk_cfg_31_clr;
    uint32_t clk_cfg_32;
    uint32_t clk_cfg_32_set;
    uint32_t clk_cfg_32_clr;
    uint32_t clk_cfg_33;
    uint32_t clk_cfg_33_set;
    uint32_t clk_cfg_33_clr;
    uint32_t clk_cfg_34;
    uint32_t clk_cfg_34_set;
    uint32_t clk_cfg_34_clr;
    uint32_t clk_cfg_35;
    uint32_t clk_cfg_35_set;
    uint32_t clk_cfg_35_clr;
    uint32_t clk_cfg_36;
    uint32_t clk_cfg_36_set;
    uint32_t clk_cfg_36_clr;
    uint32_t clk_cfg_37;
    uint32_t clk_cfg_37_set;
    uint32_t clk_cfg_37_clr;
    uint32_t reserved3[7];
    uint32_t clk_extck_reg;     /* 0x0204 */
    uint32_t reserved4[1];
    uint32_t clk_dbg_cfg;       /* 0x020c */
    uint32_t reserved5[2];
    uint32_t clk26cali_0;       /* 0x0218 */
    uint32_t clk26cali_1;
    uint32_t reserved6[3];
    uint32_t clk_misc_cfg_0;    /* 0x022c */
    uint32_t reserved7[2];
    uint32_t clk_misc_cfg_1;    /* 0x0238 */
    uint32_t reserved8[2];
    uint32_t clk_misc_cfg_2;    /* 0x0244 */
    uint32_t reserved9[2];
    uint32_t clk_misc_cfg_3;    /* 0x0250 */
    uint32_t reserved10[2];
    uint32_t clk_misc_cfg_6;    /* 0x025c */
    uint32_t reserved11[1];
    uint32_t clk_scp_cfg_0;     /* 0x0264 */
    uint32_t reserved12[13];
    uint32_t clkmon_clk_sel;    /* 0x029c */
    uint32_t clkmon_k1;
    uint32_t reserved13[6];
    uint32_t cksta_reg_0;       /* 0x02bc */
    uint32_t cksta_reg_1;
    uint32_t cksta_reg_2;
    uint32_t cksta_reg_3;
    uint32_t cksta_reg_4;
    uint32_t reserved14[20];
    uint32_t clk_auddiv_0;      /* 0x0320 */
    uint32_t clk_auddiv_1;
    uint32_t clk_auddiv_2;
    uint32_t aud_top_cfg;
    uint32_t aud_top_mon;
    uint32_t clk_auddiv_3;
    uint32_t clk_auddiv_4;
};

STATIC_ASSERT(__offsetof(struct mtk_topckgen_regs, clk_cfg_0) == 0x0020);
STATIC_ASSERT(__offsetof(struct mtk_topckgen_regs, clk_extck_reg) == 0x0204);
STATIC_ASSERT(__offsetof(struct mtk_topckgen_regs, clk26cali_0) == 0x0218);
STATIC_ASSERT(__offsetof(struct mtk_topckgen_regs, clk_misc_cfg_0) == 0x022c);
STATIC_ASSERT(__offsetof(struct mtk_topckgen_regs, clk_misc_cfg_1) == 0x0238);
STATIC_ASSERT(__offsetof(struct mtk_topckgen_regs, clk_misc_cfg_2) == 0x0244);
STATIC_ASSERT(__offsetof(struct mtk_topckgen_regs, clk_misc_cfg_3) == 0x0250);
STATIC_ASSERT(__offsetof(struct mtk_topckgen_regs, clk_misc_cfg_6) == 0x025c);
STATIC_ASSERT(__offsetof(struct mtk_topckgen_regs, clkmon_clk_sel) == 0x029c);
STATIC_ASSERT(__offsetof(struct mtk_topckgen_regs, cksta_reg_0) == 0x02bc);
STATIC_ASSERT(__offsetof(struct mtk_topckgen_regs, clk_auddiv_0) == 0x0320);
STATIC_ASSERT(__offsetof(struct mtk_topckgen_regs, clk_auddiv_4) == 0x0338);

struct mtk_apmixed_regs {
    uint32_t ap_pll_con0;
    uint32_t ap_pll_con1;
    uint32_t ap_pll_con2;
    uint32_t ap_pll_con3;
    uint32_t ap_pll_con4;
    uint32_t ap_pll_con5;
    uint32_t clksq_stb_con0;
    uint32_t clksq_stb_con1;
    uint32_t armpll_ll_con0;
    uint32_t armpll_ll_con1;
    uint32_t armpll_ll_con2;
    uint32_t armpll_ll_con3;
    uint32_t ccipll_con0;
    uint32_t pll_chg_con0;
    uint32_t reserved1[6];
    uint32_t pllon_con0;        /* 0x0050 */
    uint32_t pllon_con1;
    uint32_t pllon_con2;
    uint32_t pllon_con3;
    uint32_t reserved2[4];
    uint32_t armpll_bl_con0;    /* 0x0070 */
    uint32_t armpll_bl_con1;
    uint32_t armpll_bl_con2;
    uint32_t armpll_bl_con3;
    uint32_t armpll_bl_con4;
    uint32_t reserved3[3];
    uint32_t ap_pllgp2_con0;    /* 0x0090 */
    uint32_t reserved4[3];
    uint32_t tvdpll1_con0;      /* 0x00a0 */
    uint32_t tvdpll1_con1;
    uint32_t tvdpll1_con2;
    uint32_t tvdpll1_con3;
    uint32_t tvdpll1_con4;
    uint32_t reserved5[3];
    uint32_t tvdpll2_con0;      /* 0x00c0 */
    uint32_t tvdpll2_con1;
    uint32_t tvdpll2_con2;
    uint32_t tvdpll2_con3;
    uint32_t tvdpll2_con4;
    uint32_t reserved6[3];
    uint32_t mmpll_con0;        /* 0x00e0 */
    uint32_t mmpll_con1;
    uint32_t mmpll_con2;
    uint32_t mmpll_con3;
    uint32_t mmpll_con4;
    uint32_t reserved7[3];
    uint32_t img_pll_con0;      /* 0x0100 */
    uint32_t img_pll_con1;
    uint32_t img_pll_con2;
    uint32_t img_pll_con3;
    uint32_t img_pll_con4;
    uint32_t reserved8[3];
    uint32_t ap_pllgp3_con0;    /* 0x0120 */
    uint32_t reserved9[11];
    uint32_t dgipll_con0;       /* 0x0150 */
    uint32_t dgipll_con1;
    uint32_t dgipll_con2;
    uint32_t dgipll_con3;
    uint32_t dgipll_con4;
    uint32_t reserved10[11];
    uint32_t respll_con0;       /* 0x0190 */
    uint32_t respll_con1;
    uint32_t respll_con2;
    uint32_t respll_con3;
    uint32_t usb1pll_con0;
    uint32_t usb1pll_con1;
    uint32_t usb1pll_con2;
    uint32_t usb1pll_con3;
    uint32_t usb1pll_con4;
    uint32_t reserved11[7];
    uint32_t mainpll_con0;      /* 0x01d0 */
    uint32_t mainpll_con1;
    uint32_t mainpll_con2;
    uint32_t mainpll_con3;
    uint32_t mainpll_con4;
    uint32_t reserved12[3];
    uint32_t univpll_con0;      /* 0x01f0 */
    uint32_t univpll_con1;
    uint32_t univpll_con2;
    uint32_t univpll_con3;
    uint32_t ap_pllgp1_con0;
    uint32_t reserved13[43];
    uint32_t ulposc1_con0;      /* 0x02b0 */
    uint32_t ulposc1_con1;
    uint32_t reserved14[2];
    uint32_t ulposc2_con0;      /* 0x02c0 */
    uint32_t ulposc2_con1;
    uint32_t reserved15[22];
    uint32_t respll_con4;       /* 0x0320 */
    uint32_t reserved16[3];
    uint32_t ap_pllgp4_con0;    /* 0x0330 */
    uint32_t reserved17[3];
    uint32_t mfg_pll_con0;      /* 0x0340 */
    uint32_t mfg_pll_con1;
    uint32_t mfg_pll_con2;
    uint32_t mfg_pll_con3;
    uint32_t mfg_pll_con4;
    uint32_t reserved18[3];
    uint32_t ethpll_con0;       /* 0x0360 */
    uint32_t ethpll_con1;
    uint32_t ethpll_con2;
    uint32_t ethpll_con3;
    uint32_t ethpll_con4;
    uint32_t reserved19[7];
    uint32_t nnapll_con0;       /* 0x0390 */
    uint32_t nnapll_con1;
    uint32_t nnapll_con2;
    uint32_t nnapll_con3;
    uint32_t nnapll_con4;
    uint32_t reserved20[39];
    uint32_t ap_auxadc_con0;    /* 0x0440 */
    uint32_t ap_auxadc_con1;
    uint32_t ap_auxadc_con2;
    uint32_t ap_auxadc_con3;
    uint32_t ap_auxadc_con4;
    uint32_t ap_auxadc_con5;
    uint32_t reserved21[6];
    uint32_t apll1_tuner_con0;  /* 0x0470 */
    uint32_t apll2_tuner_con0;
    uint32_t apll3_tuner_con0;
    uint32_t apll4_tuner_con0;
    uint32_t apll5_tuner_con0;
    uint32_t reserved22[15];
    uint32_t pll_pwr_con0;      /* 0x04c0 */
    uint32_t pll_pwr_con1;
    uint32_t reserved23[2];
    uint32_t pll_iso_con0;      /* 0x04d0 */
    uint32_t pll_iso_con1;
    uint32_t reserved24[2];
    uint32_t pll_stb_con0;      /* 0x04e0 */
    uint32_t reserved25[3];
    uint32_t div_stb_con0;      /* 0x04f0 */
    uint32_t reserved26[3];
    uint32_t ap_abist_mon_con0; /* 0x0500 */
    uint32_t ap_abist_mon_con1;
    uint32_t ap_abist_mon_con2;
    uint32_t ap_abist_mon_con3;
    uint32_t occscan_con0;
    uint32_t occscan_con1;
    uint32_t occscan_con2;
    uint32_t occscan_con3;
    uint32_t mcu_occscan_con0;
    uint32_t mcu_occscan_con1;
    uint32_t reserved27[6];
    uint32_t cksys_occ_sel0;    /* 0x0540 */
    uint32_t cksys_occ_sel1;
    uint32_t cksys_occ_sel2;
    uint32_t cksys_occ_sel3;
    uint32_t cksys_occ_sel4;
    uint32_t cksys_occ_sel5;
    uint32_t cksys_occ_sel6;
    uint32_t cksys_occ_sel7;
    uint32_t cksys_occ_tstsel0;
    uint32_t cksys_occ_tstsel1;
    uint32_t cksys_occ_tstsel2;
    uint32_t cksys_occ_tstsel3;
    uint32_t cksys_occ_tstsel4;
    uint32_t reserved28[3];
    uint32_t clkdiv_con0;       /* 0x0580 */
    uint32_t clkdiv_con1;
    uint32_t rsv_rw0_con4;
    uint32_t reserved29[1];
    uint32_t rsv_rw0_con0;      /* 0x0590 */
    uint32_t rsv_rw0_con1;
    uint32_t rsv_rw0_con2;
    uint32_t rsv_rw0_con3;
    uint32_t rsv_ro_con0;
    uint32_t rsv_ro_con1;
    uint32_t rsv_ro_con2;
    uint32_t rsv_ro_con3;
    uint32_t reserved30[20];
    uint32_t armpll_ll_con4;    /* 0x0600 */
    uint32_t reserved31[12];
    uint32_t ccipll_con1;       /* 0x0634 */
    uint32_t ccipll_con2;
    uint32_t ccipll_con3;
    uint32_t ccipll_con4;
    uint32_t reserved32[47];
    uint32_t univpll_con4;      /* 0x0700 */
    uint32_t reserved33[3];
    uint32_t msdcpll_con0;      /* 0x0710 */
    uint32_t msdcpll_con1;
    uint32_t msdcpll_con2;
    uint32_t msdcpll_con3;
    uint32_t msdcpll_con4;
    uint32_t reserved34[7];
    uint32_t apll4_con0;        /* 0x0740 */
    uint32_t apll4_con1;
    uint32_t apll4_con2;
    uint32_t apll4_con3;
    uint32_t apll4_con4;
    uint32_t reserved35[3];
    uint32_t apll3_con0;        /* 0x0760 */
    uint32_t apll3_con1;
    uint32_t apll3_con2;
    uint32_t apll3_con3;
    uint32_t apll3_con4;
    uint32_t reserved36[3];
    uint32_t apll2_con0;        /* 0x0780 */
    uint32_t apll2_con1;
    uint32_t apll2_con2;
    uint32_t apll2_con3;
    uint32_t apll2_con4;
    uint32_t reserved37[3];
    uint32_t apll5_con0;        /* 0x07a0 */
    uint32_t apll5_con1;
    uint32_t apll5_con2;
    uint32_t apll5_con3;
    uint32_t apll5_con4;
    uint32_t reserved38[3];
    uint32_t apll1_con0;        /* 0x07c0 */
    uint32_t apll1_con1;
    uint32_t apll1_con2;
    uint32_t apll1_con3;
    uint32_t apll1_con4;
    uint32_t reserved39[3];
    uint32_t adsppll_con0;      /* 0x07e0 */
    uint32_t adsppll_con1;
    uint32_t adsppll_con2;
    uint32_t adsppll_con3;
    uint32_t adsppll_con4;
    uint32_t reserved40[3];
    uint32_t mpll_con0;         /* 0x0800 */
    uint32_t mpll_con1;
    uint32_t mpll_con2;
    uint32_t mpll_con3;
    uint32_t mpll_con4;
    uint32_t reserved41[23];
    uint32_t hdmipll2_con0;     /* 0x0870 */
    uint32_t hdmipll2_con1;
    uint32_t hdmipll2_con2;
    uint32_t hdmipll2_con3;
    uint32_t hdmipll2_con4;
    uint32_t reserved42[3];
    uint32_t vdecpll_con0;      /* 0x0890 */
    uint32_t vdecpll_con1;
    uint32_t vdecpll_con2;
    uint32_t vdecpll_con3;
    uint32_t vdecpll_con4;
    uint32_t reserved43[7];
    uint32_t hdmipll1_con0;     /* 0x08c0 */
    uint32_t hdmipll1_con1;
    uint32_t hdmipll1_con2;
    uint32_t hdmipll1_con3;
    uint32_t hdmipll1_con4;
    uint32_t reserved44[3];
    uint32_t hdmirx_apll_con0;  /* 0x08e0 */
    uint32_t hdmirx_apll_con1;
    uint32_t hdmirx_apll_con2;
    uint32_t hdmirx_apll_con3;
    uint32_t hdmirx_apll_con4;
    uint32_t reserved45[291];
    uint32_t occscan_con4;      /* 0x0d80 */
    uint32_t occscan_con5;
    uint32_t occscan_con6;
    uint32_t occscan_con7;
    uint32_t occscan_con8;
    uint32_t occscan_con9;
    uint32_t occscan_con10;
    uint32_t occscan_con11;
    uint32_t occscan_con12;
    uint32_t occscan_con13;
    uint32_t occscan_con14;
    uint32_t occscan_con15;
    uint32_t ref_clk_con0;
    uint32_t reserved46[3];
    uint32_t apll1_con5;        /* 0x0dc0 */
    uint32_t apll2_con5;
    uint32_t apll3_con5;
    uint32_t apll4_con5;
    uint32_t apll5_con5;
    uint32_t hdmirx_apll_con5;
};

STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, pllon_con0) == 0x0050);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, armpll_bl_con0) == 0x0070);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, ap_pllgp2_con0) == 0x0090);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, tvdpll1_con0) == 0x00a0);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, tvdpll2_con0) == 0x00c0);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, mmpll_con0) == 0x00e0);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, img_pll_con0) == 0x0100);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, ap_pllgp3_con0) == 0x0120);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, dgipll_con0) == 0x0150);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, respll_con0) == 0x0190);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, mainpll_con0) == 0x01d0);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, univpll_con0) == 0x01f0);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, ulposc1_con0) == 0x02b0);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, ulposc2_con0) == 0x02c0);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, respll_con4) == 0x0320);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, ap_pllgp4_con0) == 0x0330);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, mfg_pll_con0) == 0x0340);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, ethpll_con0) == 0x0360);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, nnapll_con0) == 0x0390);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, ap_auxadc_con0) == 0x0440);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, apll1_tuner_con0) == 0x0470);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, pll_pwr_con0) == 0x04c0);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, pll_iso_con0) == 0x04d0);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, pll_stb_con0) == 0x04e0);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, div_stb_con0) == 0x04f0);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, ap_abist_mon_con0) == 0x0500);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, cksys_occ_sel0) == 0x0540);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, clkdiv_con0) == 0x0580);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, armpll_ll_con4) == 0x0600);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, ccipll_con1) == 0x0634);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, univpll_con4) == 0x0700);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, msdcpll_con0) == 0x0710);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, apll4_con0) == 0x0740);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, apll3_con0) == 0x0760);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, apll2_con0) == 0x0780);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, apll5_con0) == 0x07a0);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, apll1_con0) == 0x07c0);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, adsppll_con0) == 0x07e0);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, mpll_con0) == 0x0800);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, hdmipll2_con0) == 0x0870);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, vdecpll_con0) == 0x0890);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, hdmipll1_con0) == 0x08c0);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, hdmirx_apll_con0) == 0x08e0);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, occscan_con4) == 0x0d80);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, apll1_con5) == 0x0dc0);
STATIC_ASSERT(__offsetof(struct mtk_apmixed_regs, hdmirx_apll_con5) == 0x0dd4);

enum {
    PCW_INTEGER_BITS = 8,
};

enum freq_type {
    CKGEN = 0,
    ABIST,
};

/**
 * mt_get_freq - get frequency of clock from fmeter
 * @id: clock identify in specific frequency type
 * @type: frequency type [ABIST|CKGEN]
 *
 * Returns non-zero value on successful, else zero.
 */
unsigned int mt_get_freq(unsigned int id, enum freq_type type);
void mt_fmeter_dump(void);
