/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <platform/addressmap.h>
#include <platform/pll_common.h>

/* Read/write byte limitation, by platform */
#define PMIF_BYTECNT_MAX    2

/* indicate which number SW channel start, by project */
#define PMIF_SWINF_0_CHAN_NO    4
/* MD: 0, security domain: 1, AP: 2 */
#define PMIF_AP_SWINF_NO    2

struct cali {
    unsigned int dly;
    unsigned int pol;
};

enum {
    PMIF_SPI,
    PMIF_SPMI,
};

enum {
    SPMI_CK_NO_DLY = 0,
    SPMI_CK_DLY_1T,
};

enum {
    SPMI_CK_POL_NEG = 0,
    SPMI_CK_POL_POS,
};

struct mtk_rgu_regs {
    u32 reserved[36];
    u32 wdt_swsysrst2;
};
static struct mtk_rgu_regs * const mtk_rgu = (void *)RGU_BASE;
STATIC_ASSERT(__offsetof(struct mtk_rgu_regs, wdt_swsysrst2) == 0x90);

struct mtk_iocfg_bm_regs {
    u32 reserved[4];
    u32 drv_cfg1;
    u32 drv_cfg1_set;
    u32 drv_cfg1_clr;
    u32 reserved0[1];
    u32 drv_cfg2;
    u32 drv_cfg2_set;
    u32 drv_cfg2_clr;
    u32 reserved1[1];
    u32 drv_cfg3;
    u32 drv_cfg3_set;
    u32 drv_cfg3_clr;
    u32 reserved2[1];
    u32 eh_cfg;
    u32 eh_cfg_set;
    u32 eh_cfg_clr;
    u32 reserved3[9];
    u32 ies_cfg1;
    u32 ies_cfg1_set;
    u32 ies_cfg1_clr;
    u32 reserved5[5];
    u32 pd_cfg1;
    u32 pd_cfg1_set;
    u32 pd_cfg1_clr;
    u32 reserved6[5];
    u32 pu_cfg1;
    u32 pu_cfg1_set;
    u32 pu_cfg1_clr;
    u32 reserved7[1];
    u32 rdsel_cfg0;
    u32 rdsel_cfg0_set;
    u32 rdsel_cfg0_clr;
    u32 reserved8[9];
    u32 smt_cfg0;
    u32 smt_cfg0_set;
    u32 smt_cfg0_clr;
    u32 reserved9[5];
    u32 tdsel_cfg1;
    u32 tdsel_cfg1_set;
    u32 tdsel_cfg1_clr;
};

static struct mtk_iocfg_bm_regs * const mtk_iocfg = (void *)IOCFG_BM_BASE;
STATIC_ASSERT(__offsetof(struct mtk_iocfg_bm_regs, drv_cfg1) == 0x10);
STATIC_ASSERT(__offsetof(struct mtk_iocfg_bm_regs, drv_cfg2) == 0x20);
STATIC_ASSERT(__offsetof(struct mtk_iocfg_bm_regs, drv_cfg3) == 0x30);
STATIC_ASSERT(__offsetof(struct mtk_iocfg_bm_regs, eh_cfg) == 0x40);
STATIC_ASSERT(__offsetof(struct mtk_iocfg_bm_regs, ies_cfg1) == 0x70);
STATIC_ASSERT(__offsetof(struct mtk_iocfg_bm_regs, pd_cfg1) == 0x90);
STATIC_ASSERT(__offsetof(struct mtk_iocfg_bm_regs, pu_cfg1) == 0xB0);
STATIC_ASSERT(__offsetof(struct mtk_iocfg_bm_regs, rdsel_cfg0) == 0xC0);
STATIC_ASSERT(__offsetof(struct mtk_iocfg_bm_regs, smt_cfg0) == 0xF0);
STATIC_ASSERT(__offsetof(struct mtk_iocfg_bm_regs, tdsel_cfg1) == 0x110);

struct mtk_spmi_mst_reg {
    u32 op_st_ctrl;
    u32 grp_id_en;
    u32 op_st_sta;
    u32 mst_sampl;
    u32 mst_req_en;
    u32 rcs_ctrl;
    u32 reserved1[10];
    u32 rec_ctrl;
    u32 rec0;
    u32 rec1;
    u32 rec2;
    u32 rec3;
    u32 rec4;
    u32 reserved2[41];
    u32 mst_dbg;
};
static struct mtk_spmi_mst_reg * const mtk_spmi_mst = (void *)SPMI_MST_BASE;
STATIC_ASSERT(__offsetof(struct mtk_spmi_mst_reg, rec_ctrl) == 0x40);
STATIC_ASSERT(__offsetof(struct mtk_spmi_mst_reg, mst_dbg) == 0xfc);

struct mtk_pmif_regs {
    u32 init_done;
    u32 reserved1[5];
    u32 inf_busy_sta;
    u32 other_busy_sta_0;
    u32 other_busy_sta_1;
    u32 inf_en;
    u32 other_inf_en;
    u32 inf_cmd_per_0;
    u32 inf_cmd_per_1;
    u32 inf_cmd_per_2;
    u32 inf_cmd_per_3;
    u32 inf_max_bytecnt_per_0;
    u32 inf_max_bytecnt_per_1;
    u32 inf_max_bytecnt_per_2;
    u32 inf_max_bytecnt_per_3;
    u32 staupd_ctrl;
    u32 reserved2[48];
    u32 int_gps_auxadc_cmd_addr;
    u32 int_gps_auxadc_cmd;
    u32 int_gps_auxadc_rdata_addr;
    u32 reserved3[13];
    u32 arb_en;
    u32 reserved4[35];
    u32 lat_cnter_en;
    u32 lat_limit_loading;
    u32 lat_limit_0;
    u32 lat_limit_1;
    u32 lat_limit_2;
    u32 lat_limit_3;
    u32 lat_limit_4;
    u32 lat_limit_5;
    u32 lat_limit_6;
    u32 lat_limit_7;
    u32 lat_limit_8;
    u32 lat_limit_9;
    u32 reserved5[99];
    u32 crc_ctrl;
    u32 crc_sta;
    u32 sig_mode;
    u32 pmic_sig_addr;
    u32 pmic_sig_val;
    u32 reserved6[2];
    u32 cmdissue_en;
    u32 reserved7[10];
    u32 timer_ctrl;
    u32 timer_sta_0;
    u32 timer_sta_1;
    u32 sleep_protection_ctrl;
    u32 reserved8[5];
    u32 spi_mode_ctrl;
    u32 reserved9[2];
    u32 pmic_eint_sta_addr;
    u32 reserved10[2];
    u32 irq_event_en_0;
    u32 irq_flag_raw_0;
    u32 irq_flag_0;
    u32 irq_clr_0;
    u32 reserved11[276];
    u32 swinf_acc;
    u32 swinf_wdata;
    u32 reserved12[3];
    u32 swinf_rdata;
    u32 reserved13[3];
    u32 swinf_vld_clr;
    u32 swinf_sta;
};

static struct mtk_pmif_regs * const mtk_pmif = (void *)PMIF_SPMI_BASE;
static struct mtk_pmif_regs * const mtk_pwrap = (void *)PWRAP_BASE;

STATIC_ASSERT(__offsetof(struct mtk_pmif_regs, inf_busy_sta) == 0x18);
STATIC_ASSERT(__offsetof(struct mtk_pmif_regs, inf_en) == 0x24);
STATIC_ASSERT(__offsetof(struct mtk_pmif_regs, inf_cmd_per_0) == 0x2C);
STATIC_ASSERT(__offsetof(struct mtk_pmif_regs, arb_en) == 0x150);
STATIC_ASSERT(__offsetof(struct mtk_pmif_regs, lat_cnter_en) == 0x1E0);
STATIC_ASSERT(__offsetof(struct mtk_pmif_regs, cmdissue_en) == 0x3B8);
STATIC_ASSERT(__offsetof(struct mtk_pmif_regs, timer_ctrl) == 0x3E4);
STATIC_ASSERT(__offsetof(struct mtk_pmif_regs, sleep_protection_ctrl) == 0x3F0);
STATIC_ASSERT(__offsetof(struct mtk_pmif_regs, spi_mode_ctrl) == 0x408);

STATIC_ASSERT(__offsetof(struct mtk_pmif_regs, swinf_acc) == 0x880);
STATIC_ASSERT(__offsetof(struct mtk_pmif_regs, swinf_wdata) == 0x884);
STATIC_ASSERT(__offsetof(struct mtk_pmif_regs, swinf_rdata) == 0x894);
STATIC_ASSERT(__offsetof(struct mtk_pmif_regs, swinf_vld_clr) == 0x8A4);
STATIC_ASSERT(__offsetof(struct mtk_pmif_regs, swinf_sta) == 0x8A8);

struct mtk_infra_ao_regs {
    u32 reserved1[20];
    u32 infra_globalcon_dcmctl; /* 0x0050 */
    u32 reserved2[7];
    u32 infra_bus_dcm_ctrl; /* 0x0070 */
    u32 peri_bus_dcm_ctrl;
    u32 mem_dcm_ctrl;
    u32 dfs_mem_dcm_ctrl;
    u32 module_sw_cg_0_set;
    u32 module_sw_cg_0_clr;
    u32 module_sw_cg_1_set;
    u32 module_sw_cg_1_clr;
    u32 module_sw_cg_0_sta;
    u32 module_sw_cg_1_sta;
    u32 module_clk_sel;
    u32 mem_cg_ctrl;
    u32 p2p_rx_clk_on;
    u32 module_sw_cg_2_set;
    u32 module_sw_cg_2_clr;
    u32 module_sw_cg_2_sta;
    u32 reserved3[1];
    u32 dramc_wbr;  /* 0x00b4 */
    u32 reserved4[2];
    u32 module_sw_cg_3_set; /* 0x00c0 */
    u32 module_sw_cg_3_clr;
    u32 module_sw_cg_3_sta;
    u32 reserved5[5];
    u32 module_sw_cg_4_set; /* 0x00e0 */
    u32 module_sw_cg_4_clr;
    u32 module_sw_cg_4_sta;
    u32 reserved6[5];
    u32 i2c_dbtool_misc;    /* 0x0100 */
    u32 md_sleep_ctrl_mask;
    u32 pmicw_clock_ctrl;
    u32 reserved7[5];
    u32 infra_globalcon_rst0_set;   /* 0x0120 */
    u32 infra_globalcon_rst0_clr;
    u32 infra_globalcon_rst0_sta;
    u32 reserved8[1];
    u32 infra_globalcon_rst1_set;   /* 0x0130 */
    u32 infra_globalcon_rst1_clr;
    u32 infra_globalcon_rst1_sta;
    u32 reserved9[1];
    u32 infra_globalcon_rst2_set;   /* 0x0140 */
    u32 infra_globalcon_rst2_clr;
    u32 infra_globalcon_rst2_sta;
    u32 reserved10[1];
    u32 infra_globalcon_rst3_set;   /* 0x0150 */
    u32 infra_globalcon_rst3_clr;
    u32 infra_globalcon_rst3_sta;
    u32 reserved11[37];
    u32 infra_nna0_slave_gals_ctrl; /* 0x01f0 */
    u32 infra_nna1_slave_gals_ctrl;
    u32 infra_nna0_master_gals_ctrl;
    u32 infra_nna1_master_gals_ctrl;
    u32 infra_topaxi_si0_ctl;
    u32 infra_topaxi_si1_ctl;
    u32 infra_topaxi_mdbus_ctl;
    u32 infra_mci_si0_ctl;
    u32 infra_mci_si1_ctl;
    u32 infra_mci_si2_ctl;
    u32 infra_mci_async_ctl;
    u32 infra_mci_cg_mfg_sec_sta;
    u32 infra_topaxi_protecten;
    u32 infra_topaxi_protecten_sta0;
    u32 infra_topaxi_protecten_sta1;
    u32 reserved12[1];
    u32 infra_apb_async_sta;    /* 0x0230 */
    u32 infra_topaxi_si2_ctl;
    u32 infra_topaxi_fmem_mdhw_ctrl;
    u32 infra_conn_gals_ctl;
    u32 infra_mci_trans_con_read;
    u32 infra_mci_trans_con_write;
    u32 infra_mci_id_remap_con;
    u32 infra_mci_emi_trans_con;
    u32 infra_topaxi_protecten_1;
    u32 infra_topaxi_protecten_sta0_1;
    u32 infra_topaxi_protecten_sta1_1;
    u32 reserved13[1];
    u32 infra_topaxi_aslice_ctrl;   /* 0x0260 */
    u32 infra_topaxi_aslice_ctrl_1;
    u32 infra_topaxi_aslice_ctrl_2;
    u32 infra_topaxi_aslice_ctrl_3;
    u32 infra_topaxi_mi_ctrl;
    u32 infra_topaxi_cbip_aslice_ctrl;
    u32 infra_topaxi_cbip_slice_ctrl;
    u32 infra_top_master_sideband;
    u32 infra_ssusb_dev;
    u32 reserved14[1];
    u32 infra_topaxi_emi_gmc_l2c_ctrl;  /* 0x0288 */
    u32 infra_topaxi_cbip_slice_ctrl_1;
    u32 infra_mfg_slave_gals_ctrl;
    u32 infra_mfg_master_m0_gals_ctrl;
    u32 infra_mfg_master_m1_gals_ctrl;
    u32 infra_top_master_sideband_1;
    u32 infra_topaxi_protecten_set;
    u32 infra_topaxi_protecten_clr;
    u32 infra_topaxi_protecten_1_set;
    u32 infra_topaxi_protecten_1_clr;
    u32 infra_topaxi_cbip_slice_ctrl_2;
    u32 reserved15[1];
    u32 infra_topaxi_aslice_ctrl_4; /* 0x02b8 */
    u32 reserved16[1];
    u32 infra_topaxi_protecten_mcu; /* 0x02c0 */
    u32 infra_topaxi_protecten_mcu_set;
    u32 infra_topaxi_protecten_mcu_clr;
    u32 reserved17[1];
    u32 infra_topaxi_protecten_mm;  /* 0x02d0 */
    u32 infra_topaxi_protecten_mm_set;
    u32 infra_topaxi_protecten_mm_clr;
    u32 reserved18[1];
    u32 infra_topaxi_protecten_mcu_sta0;    /* 0x02e0 */
    u32 infra_topaxi_protecten_mcu_sta1;
    u32 infra_topaxi_protecten_mm_sta0;
    u32 infra_topaxi_protecten_mm_sta1;
    u32 reserved19[1];
    u32 infra_apu_master_m0_gals_ctl;   /* 0x02f4 */
    u32 infra_apu_master_m1_gals_ctl;
    u32 infra_topaxi_bus_dbg_con_ao;
    u32 md1_bank0_map0;
    u32 md1_bank0_map1;
    u32 md1_bank0_map2;
    u32 md1_bank0_map3;
    u32 md1_bank1_map0;
    u32 md1_bank1_map1;
    u32 md1_bank1_map2;
    u32 md1_bank1_map3;
    u32 md1_bank4_map0;
    u32 md1_bank4_map1;
    u32 md1_bank4_map2;
    u32 md1_bank4_map3;
    u32 md2_bank0_map0;
    u32 md2_bank0_map1;
    u32 md2_bank0_map2;
    u32 md2_bank0_map3;
    u32 reserved20[4];
    u32 md2_bank4_map0; /* 0x0350 */
    u32 md2_bank4_map1;
    u32 md2_bank4_map2;
    u32 md2_bank4_map3;
    u32 c2k_config;
    u32 c2k_status;
    u32 c2k_spm_ctrl;
    u32 reserved21[1];
    u32 ap2md_dummy;    /* 0x0370 */
    u32 reserved22[3];
    u32 conn_map0;  /* 0x0380 */
    u32 cldma_map0;
    u32 conn_map1;
    u32 conn_bus_con;
    u32 mcusys_dfd_map;
    u32 conn_map2;
    u32 conn_map3;
    u32 conn_map4;
    u32 module_clk_sel_set;
    u32 module_clk_sel_clr;
    u32 pmicw_clock_ctrl_set;
    u32 pmicw_clock_ctrl_clr;
    u32 dramc_wbr_set;
    u32 dramc_wbr_clr;
    u32 topaxi_si0_ctl_set;
    u32 topaxi_si0_ctl_clr;
    u32 topaxi_si1_ctl_set;
    u32 topaxi_si1_ctl_clr;
    u32 reserved23[14];
    u32 peri_cci_sideband_con;  /* 0x0400 */
    u32 mfg_cci_sideband_con;
    u32 reserved24[2];
    u32 infra_pwm_cksw_ctrl;    /* 0x0410 */
    u32 reserved25[59];
    u32 infra_ao_dbg_con0;  /* 0x0500 */
    u32 infra_ao_dbg_con1;
    u32 infra_ao_dbg_con2;
    u32 infra_ao_dbg_con3;
    u32 md_dbg_ck_con;
    u32 infra_ao_dbg_sta;
    u32 reserved26[58];
    u32 mfg_misc_con;   /* 0x0600 */
    u32 reserved27[3];
    u32 infracfg_ao_iommu_0;    /* 0x0610 */
    u32 infracfg_ao_iommu_1;
    u32 reserved28[58];
    u32 infra_rsvd0;    /* 0x0700 */
    u32 infra_rsvd1;
    u32 infra_rsvd2;
    u32 infra_rsvd3;
    u32 infra_topaxi_protecten_2;
    u32 infra_topaxi_protecten_set_2;
    u32 infra_topaxi_protecten_clr_2;
    u32 reserved29[1];
    u32 infra_topaxi_protecten_sta0_2;  /* 0x0720 */
    u32 infra_topaxi_protecten_sta1_2;
    u32 reserved30[2];
    u32 infra_globalcon_rst4_set;   /* 0x0730 */
    u32 infra_globalcon_rst4_clr;
    u32 infra_globalcon_rst4_sta;
    u32 infra_ao_sec_rst_con4;
    u32 reserved31[16];
    u32 mcu2emi_m0_parity;  /* 0x0780 */
    u32 mcu2emi_m0_parity_dbg_aw_1;
    u32 mcu2emi_m0_parity_dbg_aw_2;
    u32 mcu2emi_m0_parity_dbg_ar_1;
    u32 mcu2emi_m0_parity_dbg_ar_2;
    u32 mcu2emi_m1_parity;
    u32 mcu2emi_m1_parity_dbg_aw_1;
    u32 mcu2emi_m1_parity_dbg_aw_2;
    u32 mcu2emi_m1_parity_dbg_ar_1;
    u32 mcu2emi_m1_parity_dbg_ar_2;
    u32 mcu2ifr_reg_parity;
    u32 mcu2ifr_reg_parity_dbg_aw_1;
    u32 mcu2ifr_reg_parity_dbg_aw_2;
    u32 mcu2ifr_reg_parity_dbg_ar_1;
    u32 mcu2ifr_reg_parity_dbg_ar_2;
    u32 ifr_l3c2mcu_parity;
    u32 ifr_l3c2mcu_parity_dbg_r_1;
    u32 reserved32[27];
    u32 gcpu_aor_ctrl;  /* 0x0830 */
    u32 gcpu_aor_lock_sbc_pubk_hv;
    u32 reserved33[2];
    u32 gcpu_aor_sbc_pubk_hv0;  /* 0x0840 */
    u32 gcpu_aor_sbc_pubk_hv1;
    u32 gcpu_aor_sbc_pubk_hv2;
    u32 gcpu_aor_sbc_pubk_hv3;
    u32 gcpu_aor_sbc_pubk_hv4;
    u32 gcpu_aor_sbc_pubk_hv5;
    u32 gcpu_aor_sbc_pubk_hv6;
    u32 gcpu_aor_sbc_pubk_hv7;
    u32 gcpu_aor_sbc_pubk_hv8;
    u32 gcpu_aor_sbc_pubk_hv9;
    u32 gcpu_aor_sbc_pubk_hv10;
    u32 gcpu_aor_sbc_pubk_hv11;
    u32 reserved34[36];
    u32 infra_bonding;  /* 0x0900 */
    u32 reserved35[63];
    u32 infra_ao_scpsys_apb_async_sta;  /* 0x0a00 */
    u32 infra_ao_md32_tx_apb_async_sta;
    u32 infra_ao_md32_rx_apb_async_sta;
    u32 infra_ao_cksys_apb_async_sta;
    u32 infra_ao_pmic_wrap_tx_apb_async_sta;
    u32 infra_mcu2apu_asl0_ctl;
    u32 infra_mcu2reg_asl0_ctl;
    u32 infra_mcu_decoder_infra_ctl;
    u32 infra_mcu_decoder_sta0;
    u32 infra_mcu_decoder_sta1;
    u32 infra_idle_async_bit_en_0;
    u32 infra_apu_slave_gals_ctrl;
    u32 infra_aximem_idle_bit_en_0;
    u32 infra_mcu_path_sync_ctl;
    u32 infra_conn2ap_int_mask;
    u32 infra_mcu_pwr_ctl_mask;
    u32 infra_md_rsv;
    u32 reserved36[7];
    u32 infra_mem_26m_cksel;    /* 0x0a60 */
    u32 reserved37[39];
    u32 pll_ulposc_con0;    /* 0x0b00 */
    u32 pll_ulposc_con1;
    u32 reserved38[2];
    u32 pll_auxadc_con0;    /* 0x0b10 */
    u32 scp_infra_irq_set;
    u32 scp_infra_irq_clr;
    u32 scp_infra_ctrl;
    u32 reserved39[24];
    u32 infra_topaxi_protecten_vdnr;    /* 0x0b80 */
    u32 infra_topaxi_protecten_vdnr_set;
    u32 infra_topaxi_protecten_vdnr_clr;
    u32 infra_topaxi_protecten_vdnr_sta0;
    u32 infra_topaxi_protecten_vdnr_sta1;
    u32 reserved40[3];
    u32 infra_topaxi_protecten_vdnr_1;  /* 0x0ba0 */
    u32 infra_topaxi_protecten_vdnr_set_1;
    u32 infra_topaxi_protecten_vdnr_clr_1;
    u32 infra_topaxi_protecten_vdnr_sta0_1;
    u32 infra_topaxi_protecten_vdnr_sta1_1;
    u32 infra_topaxi_protecten_vdnr_2;
    u32 infra_topaxi_protecten_vdnr_set_2;
    u32 infra_topaxi_protecten_vdnr_clr_2;
    u32 infra_topaxi_protecten_vdnr_sta0_2;
    u32 infra_topaxi_protecten_vdnr_sta1_2;
    u32 infra_topaxi_protecten_sub_infra_vdnr;
    u32 infra_topaxi_protecten_sub_infra_vdnr_set;
    u32 infra_topaxi_protecten_sub_infra_vdnr_clr;
    u32 infra_topaxi_protecten_sub_infra_vdnr_sta0;
    u32 infra_topaxi_protecten_sub_infra_vdnr_sta1;
    u32 reserved41[9];
    u32 cldma_ctrl; /* 0x0c00 */
    u32 reserved42[63];
    u32 infrabus_dbg0;  /* 0x0d00 */
    u32 infrabus_dbg1;
    u32 infrabus_dbg2;
    u32 infrabus_dbg3;
    u32 infrabus_dbg4;
    u32 infrabus_dbg5;
    u32 infrabus_dbg6;
    u32 infrabus_dbg7;
    u32 infrabus_dbg8;
    u32 infrabus_dbg9;
    u32 infrabus_dbg10;
    u32 infrabus_dbg11;
    u32 infrabus_dbg12;
    u32 infrabus_dbg13;
    u32 infrabus_dbg14;
    u32 infrabus_dbg15;
    u32 infrabus_dbg16;
    u32 infrabus_dbg17;
    u32 infrabus_dbg18;
    u32 infrabus_dbg19;
    u32 infrabus_dbg20;
    u32 infrabus_dbg21;
    u32 infrabus_dbg22;
    u32 infrabus_dbg23;
    u32 infrabus_dbg24;
    u32 infrabus_dbg25;
    u32 infrabus_dbg26;
    u32 infrabus_dbg27;
    u32 infrabus_dbg28;
    u32 infrabus_dbg29;
    u32 infrabus_dbg30;
    u32 infrabus_dbg31;
    u32 infrabus_dbg32;
    u32 infrabus_dbg33;
    u32 infrabus_dbg34;
    u32 infrabus_dbg35;
    u32 infrabus_dbg36;
    u32 infrabus_dbg37;
    u32 infrabus_dbg38;
    u32 infrabus_dbg39;
    u32 infrabus_dbg40;
    u32 infrabus_dbg41;
    u32 infrabus_dbg42;
    u32 infrabus_dbg43;
    u32 infrabus_dbg44;
    u32 infrabus_dbg45;
    u32 reserved43[4];
    u32 infra_topaxi_protecten_mm_2;    /* 0x0dc8 */
    u32 infra_topaxi_protecten_mm_set_2;
    u32 infra_topaxi_protecten_mm_clr_2;
    u32 infra_topaxi_protecten_mm_sta0_2;
    u32 infra_topaxi_protecten_mm_sta1_2;
    u32 reserved44[5];
    u32 infrabus_dbg_mask2; /* 0x0df0 */
    u32 reserved45[67];
    u32 infra_misc; /* 0x0f00 */
    u32 infra_acp;
    u32 misc_config;
    u32 infra_misc2;
    u32 mdsys_misc_con;
    u32 reserved46[27];
    u32 infra_ao_sec_con;   /* 0x0f80 */
    u32 infra_ao_sec_cg_con0;
    u32 infra_ao_sec_cg_con1;
    u32 infra_ao_sec_rst_con0;
    u32 infra_ao_sec_rst_con1;
    u32 infra_ao_sec_rst_con2;
    u32 reserved47[1];
    u32 infra_ao_sec_cg_con2;   /* 0x0f9c */
    u32 infra_ao_sec_rst_con3;
    u32 infra_ao_sec_cg_con3;
    u32 reserved48[2];
    u32 infra_ao_sec_hyp;   /* 0x0fb0 */
    u32 infra_ao_sec_mfg_hyp;
};

static struct mtk_infra_ao_regs * const mtk_infracfg_ao = (void *)INFRACFG_AO_BASE;

STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, infra_globalcon_dcmctl) == 0x0050);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, infra_bus_dcm_ctrl) == 0x0070);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, module_sw_cg_3_set) == 0x00c0);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, module_sw_cg_4_set) == 0x00e0);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, i2c_dbtool_misc) == 0x0100);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, infra_globalcon_rst0_set) == 0x0120);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, infra_nna0_slave_gals_ctrl) == 0x01f0);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, md2_bank4_map0) == 0x0350);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, conn_map0) == 0x0380);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, peri_cci_sideband_con) == 0x0400);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, infra_pwm_cksw_ctrl) == 0x0410);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, infra_ao_dbg_con0) == 0x0500);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, mfg_misc_con) == 0x0600);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, infracfg_ao_iommu_0) == 0x0610);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, infra_rsvd0) == 0x0700);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, infra_globalcon_rst4_set) == 0x0730);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, mcu2emi_m0_parity) == 0x0780);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, gcpu_aor_ctrl) == 0x0830);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, gcpu_aor_sbc_pubk_hv0) == 0x0840);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, infra_bonding) == 0x0900);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, infra_ao_scpsys_apb_async_sta) == 0x0a00);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, infra_mem_26m_cksel) == 0x0a60);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, pll_ulposc_con0) == 0x0b00);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, pll_auxadc_con0) == 0x0b10);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, infra_topaxi_protecten_vdnr) == 0x0b80);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, infra_topaxi_protecten_vdnr_1) == 0x0ba0);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, cldma_ctrl) == 0x0c00);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, infrabus_dbg0) == 0x0d00);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, infra_topaxi_protecten_mm_2) == 0x0dc8);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, infrabus_dbg_mask2) == 0x0df0);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, infra_misc) == 0x0f00);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, infra_ao_sec_con) == 0x0f80);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, infra_ao_sec_hyp) == 0x0fb0);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, infra_ao_sec_mfg_hyp) == 0x0fb4);

struct mtk_spm_regs {
    u32 poweron_config_en;
    u32 reserved[400];
    u32 ulposc_con;
};

static struct mtk_spm_regs * const mtk_spm = (void *)SPM_BASE;
STATIC_ASSERT(__offsetof(struct mtk_spm_regs, ulposc_con) == 0x644);

struct mtk_scp_regs {
    u32 reserved[27];
    u32 scp_clk_on_ctrl;
};
static struct mtk_scp_regs * const mtk_scp = (void *)SCPCFG_BASE;
STATIC_ASSERT(__offsetof(struct mtk_scp_regs, scp_clk_on_ctrl) == 0x6C);

void pmif_clk_init(void);
void platform_pmif_spmi_init(void);
