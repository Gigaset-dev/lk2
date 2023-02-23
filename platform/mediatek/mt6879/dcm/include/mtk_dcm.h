/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

/* #define DCM_DEFAULT_ALL_OFF */
/* #define DCM_BRINGUP */
/* BUS DVT only mode */
/* #define BUSDVT_ONLY_MD */

#define DCM_OFF (0)
#define DCM_ON (1)

/* Note: ENABLE_DCM_IN_LK is used in kernel if DCM is enabled in LK */
#define ENABLE_DCM_IN_LK
#ifdef ENABLE_DCM_IN_LK
#define INIT_DCM_TYPE_BY_K      0
#endif

enum {
    ARMCORE_DCM_OFF = DCM_OFF,
    ARMCORE_DCM_MODE1 = DCM_ON,
    ARMCORE_DCM_MODE2 = DCM_ON+1,
};

enum {
    INFRA_DCM_OFF = DCM_OFF,
    INFRA_DCM_ON = DCM_ON,
};

enum {
    PERI_DCM_OFF = DCM_OFF,
    PERI_DCM_ON = DCM_ON,
};

enum {
    MCUSYS_ACP_DCM_OFF = DCM_OFF,
    MCUSYS_ACP_DCM_ON = DCM_ON,
};

enum {
    MCUSYS_ADB_DCM_OFF = DCM_OFF,
    MCUSYS_ADB_DCM_ON = DCM_ON,
};

enum {
    MCUSYS_BUS_DCM_OFF = DCM_OFF,
    MCUSYS_BUS_DCM_ON = DCM_ON,
};

enum {
    MCUSYS_CBIP_DCM_OFF = DCM_OFF,
    MCUSYS_CBIP_DCM_ON = DCM_ON,
};

enum {
    MCUSYS_CORE_DCM_OFF = DCM_OFF,
    MCUSYS_CORE_DCM_ON = DCM_ON,
};

enum {
    MCUSYS_IO_DCM_OFF = DCM_OFF,
    MCUSYS_IO_DCM_ON = DCM_ON,
};

enum {
    MCUSYS_CPC_PBI_DCM_OFF = DCM_OFF,
    MCUSYS_CPC_PBI_DCM_ON = DCM_ON,
};

enum {
    MCUSYS_CPC_TURBO_DCM_OFF = DCM_OFF,
    MCUSYS_CPC_TURBO_DCM_ON = DCM_ON,
};

enum {
    MCUSYS_STALL_DCM_OFF = DCM_OFF,
    MCUSYS_STALL_DCM_ON = DCM_ON,
};

enum {
    MCUSYS_APB_DCM_OFF = DCM_OFF,
    MCUSYS_APB_DCM_ON = DCM_ON,
};

enum {
    MCUSYS_DCM_OFF = DCM_OFF,
    MCUSYS_DCM_ON = DCM_ON,
};

enum {
    VLP_DCM_OFF = DCM_OFF,
    VLP_DCM_ON = DCM_ON,
};

enum {
    DRAMC_AO_DCM_OFF = DCM_OFF,
    DRAMC_AO_DCM_ON = DCM_ON,
};

enum {
    DDRPHY_DCM_OFF = DCM_OFF,
    DDRPHY_DCM_ON = DCM_ON,
};

enum {
    EMI_DCM_OFF = DCM_OFF,
    EMI_DCM_ON = DCM_ON,
};

enum {
    STALL_DCM_OFF = DCM_OFF,
    STALL_DCM_ON = DCM_ON,
};

enum {
    BIG_CORE_DCM_OFF = DCM_OFF,
    BIG_CORE_DCM_ON = DCM_ON,
};

enum {
    GIC_SYNC_DCM_OFF = DCM_OFF,
    GIC_SYNC_DCM_ON = DCM_ON,
};

enum {
    LAST_CORE_DCM_OFF = DCM_OFF,
    LAST_CORE_DCM_ON = DCM_ON,
};

enum {
    RGU_DCM_OFF = DCM_OFF,
    RGU_DCM_ON = DCM_ON,
};

enum {
    TOPCKG_DCM_OFF = DCM_OFF,
    TOPCKG_DCM_ON = DCM_ON,
};

enum {
    LPDMA_DCM_OFF = DCM_OFF,
    LPDMA_DCM_ON = DCM_ON,
};

enum {
    PWRAP_DCM_OFF = DCM_OFF,
    PWRAP_DCM_ON = DCM_ON,
};

enum {
    MCSI_DCM_OFF = DCM_OFF,
    MCSI_DCM_ON = DCM_ON,
};

enum {
    BUSDVT_DCM_OFF = DCM_OFF,
    BUSDVT_DCM_ON = DCM_ON,
};

enum {
    ARMCORE_DCM = 0,
    MCUSYS_DCM,
    INFRA_DCM,
    PERI_DCM,
    MCUSYS_ACP_DCM,
    MCUSYS_ADB_DCM,
    MCUSYS_BUS_DCM,
    MCUSYS_CBIP_DCM,
    MCUSYS_CORE_DCM,
    MCUSYS_IO_DCM,
    MCUSYS_CPC_PBI_DCM,
    MCUSYS_CPC_TURBO_DCM,
    MCUSYS_STALL_DCM,
    MCUSYS_APB_DCM,
    VLP_DCM,
    EMI_DCM,
    DRAMC_DCM,
    DDRPHY_DCM,
    STALL_DCM,
    BIG_CORE_DCM,
    GIC_SYNC_DCM,
    LAST_CORE_DCM,
    RGU_DCM,
    TOPCKG_DCM,
    LPDMA_DCM,
    MCSI_DCM,
    BUSDVT_DCM,
    NR_DCM,
};

enum {
    ARMCORE_DCM_TYPE   = (1U << ARMCORE_DCM),
    MCUSYS_DCM_TYPE    = (1U << MCUSYS_DCM),
    INFRA_DCM_TYPE     = (1U << INFRA_DCM),
    PERI_DCM_TYPE      = (1U << PERI_DCM),
    MCUSYS_ACP_DCM_TYPE       = (1U << MCUSYS_ACP_DCM),
    MCUSYS_ADB_DCM_TYPE       = (1U << MCUSYS_ADB_DCM),
    MCUSYS_BUS_DCM_TYPE       = (1U << MCUSYS_BUS_DCM),
    MCUSYS_CBIP_DCM_TYPE      = (1U << MCUSYS_CBIP_DCM),
    MCUSYS_CORE_DCM_TYPE      = (1U << MCUSYS_CORE_DCM),
    MCUSYS_IO_DCM_TYPE        = (1U << MCUSYS_IO_DCM),
    MCUSYS_CPC_PBI_DCM_TYPE   = (1U << MCUSYS_CPC_PBI_DCM),
    MCUSYS_CPC_TURBO_DCM_TYPE = (1U << MCUSYS_CPC_TURBO_DCM),
    MCUSYS_STALL_DCM_TYPE     = (1U << MCUSYS_STALL_DCM),
    MCUSYS_APB_DCM_TYPE       = (1U << MCUSYS_APB_DCM),
    VLP_DCM_TYPE       = (1U << VLP_DCM),
    EMI_DCM_TYPE       = (1U << EMI_DCM),
    DRAMC_DCM_TYPE     = (1U << DRAMC_DCM),
    DDRPHY_DCM_TYPE    = (1U << DDRPHY_DCM),
    STALL_DCM_TYPE     = (1U << STALL_DCM),
    BIG_CORE_DCM_TYPE  = (1U << BIG_CORE_DCM),
    GIC_SYNC_DCM_TYPE  = (1U << GIC_SYNC_DCM),
    LAST_CORE_DCM_TYPE = (1U << LAST_CORE_DCM),
    RGU_DCM_TYPE       = (1U << RGU_DCM),
    TOPCKG_DCM_TYPE    = (1U << TOPCKG_DCM),
    LPDMA_DCM_TYPE     = (1U << LPDMA_DCM),
    MCSI_DCM_TYPE      = (1U << MCSI_DCM),
    BUSDVT_DCM_TYPE    = (1U << BUSDVT_DCM),
    NR_DCM_TYPE        = NR_DCM,
};

enum {
    DCM_CPU_CLUSTER_LL      = (1U << 0),
    DCM_CPU_CLUSTER_L       = (1U << 1),
    DCM_CPU_CLUSTER_B       = (1U << 2),
};

int dcm_armcore(int mode);
int dcm_infra(int on);
int dcm_peri(int on);
int dcm_mcusys_acp(int on);
int dcm_mcusys_adb(int on);
int dcm_mcusys_bus(int on);
int dcm_mcusys_cbip(int on);
int dcm_mcusys_core(int on);
int dcm_mcusys_io(int on);
int dcm_mcusys_cpc_pbi(int on);
int dcm_mcusys_cpc_turbo(int on);
int dcm_mcusys_stall(int on);
int dcm_mcusys_apb(int on);
int dcm_vlp(int on);
int dcm_mcusys(int on);
int dcm_dramc_ao(int on);
int dcm_emi(int on);
int dcm_ddrphy(int on);
int dcm_stall(int on);
int dcm_big_core(int on);
int dcm_gic_sync(int on);
int dcm_last_core(int on);
int dcm_rgu(int on);
int dcm_topckg(int on);
int dcm_lpdma(int on);

void dcm_infracfg_ao_emi_indiv(int on);

int mt_dcm_init(void *fdt);
void mt_dcm_disable(void);
void mt_dcm_restore(void);

/* unit of frequency is MHz */
int sync_dcm_set_cpu_freq(unsigned int cci, unsigned int mp0, unsigned int mp1, unsigned int mp2);
int sync_dcm_set_cpu_div(unsigned int cci, unsigned int mp0, unsigned int mp1, unsigned int mp2);
int sync_dcm_set_cci_freq(unsigned int cci);
int sync_dcm_set_mp0_freq(unsigned int mp0);
int sync_dcm_set_mp1_freq(unsigned int mp1);
int sync_dcm_set_mp2_freq(unsigned int mp2);
