/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <assert.h>
#include <mtcmos.h>
#include <platform/addressmap.h>
#include <platform/reg.h>

struct mtk_spm_regs {
    uint32_t reserved0[896];
    uint32_t md1_pwr_con;
    uint32_t reserved1[5];
    uint32_t adsp_top_pwr_con;
    uint32_t adsp_infra_pwr_con;
    uint32_t adsp_ao_pwr_con;
    uint32_t reserved2[19];
    uint32_t mmpro_pwr_con;
    uint32_t reserved3[17];
    uint32_t mfg0_pwr_con;
    uint32_t mfg1_pwr_con;
    uint32_t reserved4[18];
    uint32_t emi_hre_sram_con;
    uint32_t reserved5[8];
    uint32_t md_ext_buck_iso_con;
    uint32_t ext_buck_iso_con;
};

static struct mtk_spm_regs *const mtk_spm = (void *)SPM_BASE;

STATIC_ASSERT(__offsetof(struct mtk_spm_regs, md1_pwr_con) == 0xE00);
STATIC_ASSERT(__offsetof(struct mtk_spm_regs, adsp_top_pwr_con) == 0xE18);
STATIC_ASSERT(__offsetof(struct mtk_spm_regs, adsp_infra_pwr_con) == 0xE1C);
STATIC_ASSERT(__offsetof(struct mtk_spm_regs, adsp_ao_pwr_con) == 0xE20);
STATIC_ASSERT(__offsetof(struct mtk_spm_regs, mmpro_pwr_con) == 0xE70);
STATIC_ASSERT(__offsetof(struct mtk_spm_regs, mfg0_pwr_con) == 0xEB8);
STATIC_ASSERT(__offsetof(struct mtk_spm_regs, mfg1_pwr_con) == 0xEBC);
STATIC_ASSERT(__offsetof(struct mtk_spm_regs, emi_hre_sram_con) == 0xF08);
STATIC_ASSERT(__offsetof(struct mtk_spm_regs, md_ext_buck_iso_con) == 0xF2C);
STATIC_ASSERT(__offsetof(struct mtk_spm_regs, ext_buck_iso_con) == 0xF30);

struct mtk_infra_ao_regs {
    uint32_t reserved0[773];
    uint32_t mm_protecten_set0;
    uint32_t mm_protecten_clr0;
    uint32_t mm_protecten_sta0;
    uint32_t mm_protecten_sta1;
    uint32_t mm_protecten_set1;
    uint32_t mm_protecten_clr1;
    uint32_t reserved1[6];
    uint32_t infra_protecten_set0;
    uint32_t infra_protecten_clr0;
    uint32_t infra_protecten_sta0;
    uint32_t reserved2[1];
    uint32_t infra_protecten_set1;
    uint32_t infra_protecten_clr1;
    uint32_t infra_protecten_sta1;
    uint32_t reserved3[17];
    uint32_t md_mfg_protecten_set0;
    uint32_t md_mfg_protecten_clr0;
    uint32_t md_mfg_protecten_sta0;
};

struct mtk_vlpcfg_regs {
    uint32_t reserved0[133];
    uint32_t vlp_protecten_set;
    uint32_t vlp_protecten_clr;
    uint32_t vlp_protecten_sta0;
    uint32_t vlp_protecten_sta1;
    uint32_t reserved1[4];
    uint32_t vlp_protecten1_set;
    uint32_t vlp_protecten1_clr;
    uint32_t vlp_protecten1_sta0;
    uint32_t vlp_protecten1_sta1;
};


static struct mtk_infra_ao_regs *const mtk_infra_ao = (void *)INFRACFG_AO_BASE;

STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, mm_protecten_set0) == 0xC14);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, mm_protecten_clr0) == 0xC18);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, mm_protecten_sta0) == 0xC1C);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, mm_protecten_sta1) == 0xC20);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, mm_protecten_set1) == 0xC24);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, mm_protecten_clr1) == 0xC28);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, infra_protecten_set0) == 0xC44);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, infra_protecten_clr0) == 0xC48);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, infra_protecten_sta0) == 0xC4C);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, infra_protecten_set1) == 0xC54);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, infra_protecten_clr1) == 0xC58);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, infra_protecten_sta1) == 0xC5C);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, md_mfg_protecten_set0) == 0xCA4);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, md_mfg_protecten_clr0) == 0xCA8);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, md_mfg_protecten_sta0) == 0xCAC);

static struct mtk_vlpcfg_regs *const mtk_vlpcfg = (void *)VLPCFG_BASE;

STATIC_ASSERT(__offsetof(struct mtk_vlpcfg_regs, vlp_protecten_set) == 0x214);
STATIC_ASSERT(__offsetof(struct mtk_vlpcfg_regs, vlp_protecten_clr) == 0x218);
STATIC_ASSERT(__offsetof(struct mtk_vlpcfg_regs, vlp_protecten_sta0) == 0x21c);
STATIC_ASSERT(__offsetof(struct mtk_vlpcfg_regs, vlp_protecten_sta1) == 0x220);
STATIC_ASSERT(__offsetof(struct mtk_vlpcfg_regs, vlp_protecten1_set) == 0x234);
STATIC_ASSERT(__offsetof(struct mtk_vlpcfg_regs, vlp_protecten1_clr) == 0x238);
STATIC_ASSERT(__offsetof(struct mtk_vlpcfg_regs, vlp_protecten1_sta0) == 0x23c);
STATIC_ASSERT(__offsetof(struct mtk_vlpcfg_regs, vlp_protecten1_sta1) == 0x240);


enum {
    PWR_ACK_2ND   = 1U << 31,
    PWR_ACK       = 1U << 30,
    SRAM_PDN_ACK_S = 1U << 14,
    SRAM_SLEEP_B_ACK = 1U << 13,
    SRAM_PDN_ACK  = 1U << 12,
    SRAM_PDN_S    = 1U << 10,
    SRAM_SLEEP_B  = 1U << 9,
    SRAM_PDN      = 1U << 8,
    SRAM_ISOINT_B = 1U << 6,
    SRAM_CKISO    = 1U << 5,
    PWR_CLK_DIS   = 1U << 4,
    PWR_ON_2ND    = 1U << 3,
    PWR_ON        = 1U << 2,
    PWR_ISO       = 1U << 1,
    PWR_RST_B     = 1U << 0
};

enum {
    ADSP_INFRA_PROT_STEP1_0_MASK = ((0x1 << 18) | (0x1 << 19) | (0x1 << 22) |
                                    (0x1 << 26) | (0x1 << 27) | (0x1 << 28) |
                                    (0x1 << 29) | (0x1 << 30)),
    ADSP_INFRA_PROT_STEP1_1_MASK = ((0x1 << 8)),
    ADSP_INFRA_PROT_STEP2_0_MASK = ((0x1 << 20) | (0x1 << 21) | (0x1 << 25) | (0x1 << 31)),
    ADSP_INFRA_PROT_STEP2_1_MASK = ((0x1 << 4) | (0x1 << 5)),
    ADSP_TOP_PROT_STEP1_0_MASK = ((0x1 << 14) | (0x1 << 15) | (0x1 << 16) | (0x1 << 17)),
    ADSP_TOP_PROT_STEP2_0_MASK = ((0x1 << 23) | (0x1 << 24)),
    MD1_PROT_STEP1_0_MASK = ((0x1 << 9)),
    MFG0_PROT_STEP1_0_MASK = ((0x1 << 4)),
    MFG0_PROT_STEP2_0_MASK = ((0x1 << 9)),
    MFG1_PROT_STEP1_0_MASK = ((0x1 << 0)),
    MFG1_PROT_STEP2_0_MASK = ((0x1 << 1)),
    MFG1_PROT_STEP3_0_MASK = ((0x1 << 2)),
    MFG1_PROT_STEP4_0_MASK = ((0x1 << 3)),
    MM_INFRA_PROT_STEP1_0_MASK = ((0x1 << 11)),
    MM_INFRA_PROT_STEP2_0_MASK = ((0x1 << 14) | (0x1 << 15)),
    MM_INFRA_PROT_STEP4_0_MASK = ((0x1 << 15) | (0x1 << 17) | (0x1 << 25)),
    MM_INFRA_PROT_STEP4_1_MASK = ((0x1 << 19) | (0x1 << 29)),
    MM_PROC_PROT_STEP1_0_MASK = ((0x1 << 10) | (0x1 << 11)),
    MM_PROC_PROT_STEP2_0_MASK = ((0x1 << 12) | (0x1 << 13)),
};

enum {
    ADSP_INFRA_PROT_STEP1_0_ACK_MASK = ((0x1 << 18) | (0x1 << 19) | (0x1 << 22) |
                                        (0x1 << 26) | (0x1 << 27) | (0x1 << 28) |
                                        (0x1 << 29) | (0x1 << 30)),
    ADSP_INFRA_PROT_STEP1_1_ACK_MASK = ((0x1 << 8)),
    ADSP_INFRA_PROT_STEP2_0_ACK_MASK = ((0x1 << 20) | (0x1 << 21) | (0x1 << 25) | (0x1 << 31)),
    ADSP_INFRA_PROT_STEP2_1_ACK_MASK = ((0x1 << 4) | (0x1 << 5)),
    ADSP_TOP_PROT_STEP1_0_ACK_MASK = ((0x1 << 14) | (0x1 << 15) | (0x1 << 16) | (0x1 << 17)),
    ADSP_TOP_PROT_STEP2_0_ACK_MASK = ((0x1 << 23) | (0x1 << 24)),
    MD1_PROT_STEP1_0_ACK_MASK = ((0x1 << 9)),
    MFG0_PROT_STEP1_0_ACK_MASK = ((0x1 << 4)),
    MFG0_PROT_STEP2_0_ACK_MASK = ((0x1 << 9)),
    MFG1_PROT_STEP1_0_ACK_MASK = ((0x1 << 0)),
    MFG1_PROT_STEP2_0_ACK_MASK = ((0x1 << 1)),
    MFG1_PROT_STEP3_0_ACK_MASK = ((0x1 << 2)),
    MFG1_PROT_STEP4_0_ACK_MASK = ((0x1 << 3)),
    MM_INFRA_PROT_STEP1_0_ACK_MASK = ((0x1 << 11)),
    MM_INFRA_PROT_STEP2_0_ACK_MASK = ((0x1 << 14) | (0x1 << 15)),
    MM_INFRA_PROT_STEP4_0_ACK_MASK = ((0x1 << 15) | (0x1 << 17) | (0x1 << 25)),
    MM_INFRA_PROT_STEP4_1_ACK_MASK = ((0x1 << 19) | (0x1 << 29)),
    MM_PROC_PROT_STEP1_0_ACK_MASK =  ((0x1 << 10) | (0x1 << 11)),
    MM_PROC_PROT_STEP2_0_ACK_MASK =  ((0x1 << 12) | (0x1 << 13)),
};

enum {
    ADSP_TOP_SRAM_PDN_ACK = 0x1 << 12,
    MM_PROC_SRAM_PDN_ACK = 0x1 << 12,
    MFG0_SRAM_PDN_ACK = 0x1 << 12,
    MFG1_SRAM_PDN_ACK = 0x1 << 12,
};

enum {
    ADSP_TOP_SRAM_SLEEP_B_ACK = 0x1 << 13,
    MFG0_SRAM_SLEEP_B_ACK = 0x1 << 13,
};

enum {
    MM_PROC_SRAM_SLEEP_8 = 0x1 << 8,
    MM_PROC_SRAM_SLEEP_9 = 0x1 << 9,
    MM_PROC_SRAM_SLEEP_10 = 0x1 << 10,
    MM_PROC_SRAM_SLEEP_11 = 0x1 << 11,
    MM_PROC_SRAM_SLEEP_20 = 0x1 << 20,
    MM_PROC_SRAM_SLEEP_21 = 0x1 << 21,
    MM_PROC_SRAM_SLEEP_22 = 0x1 << 22,
    MM_PROC_SRAM_SLEEP_23 = 0x1 << 23,
};

void spm_mtcmos_ctrl_md1_enable(void)
{
    clrbits32(&mtk_spm->md_ext_buck_iso_con, 0x1 << 0);
    clrbits32(&mtk_spm->md_ext_buck_iso_con, 0x1 << 1);
    setbits32(&mtk_spm->md1_pwr_con, PWR_RST_B);
    setbits32(&mtk_spm->md1_pwr_con, PWR_ON);
    while ((read32(&mtk_spm->md1_pwr_con) & PWR_ACK) != PWR_ACK)
        ;
    write32(&mtk_infra_ao->infra_protecten_clr1, MD1_PROT_STEP1_0_MASK);
}

void spm_mtcmos_ctrl_md1_disable(void)
{
    write32(&mtk_infra_ao->infra_protecten_set1, MD1_PROT_STEP1_0_MASK);
    while ((read32(&mtk_infra_ao->infra_protecten_sta1)
        & MD1_PROT_STEP1_0_ACK_MASK)
        != MD1_PROT_STEP1_0_ACK_MASK)
            ;
    clrbits32(&mtk_spm->md1_pwr_con, PWR_ON);
    while (read32(&mtk_spm->md1_pwr_con) & PWR_ACK)
        ;
    setbits32(&mtk_spm->md_ext_buck_iso_con, 0x1 << 0);
    setbits32(&mtk_spm->md_ext_buck_iso_con, 0x1 << 1);
    clrbits32(&mtk_spm->md1_pwr_con, PWR_RST_B);
}

void spm_mtcmos_ctrl_adsp_top_shutdown_enable(void)
{
    setbits32(&mtk_spm->adsp_top_pwr_con, PWR_ON);
    setbits32(&mtk_spm->adsp_top_pwr_con, PWR_ON_2ND);
    while ((read32(&mtk_spm->adsp_top_pwr_con) & PWR_ACK) != PWR_ACK)
        ;
    while ((read32(&mtk_spm->adsp_top_pwr_con) & PWR_ACK_2ND) != PWR_ACK_2ND)
        ;
    clrbits32(&mtk_spm->adsp_top_pwr_con, PWR_CLK_DIS);
    clrbits32(&mtk_spm->adsp_top_pwr_con, PWR_ISO);
    setbits32(&mtk_spm->adsp_top_pwr_con, PWR_RST_B);
    clrbits32(&mtk_spm->adsp_top_pwr_con, SRAM_PDN);
    while (read32(&mtk_spm->adsp_top_pwr_con) & ADSP_TOP_SRAM_PDN_ACK)
        ;
    write32(&mtk_vlpcfg->vlp_protecten_clr, ADSP_TOP_PROT_STEP2_0_MASK);
    write32(&mtk_vlpcfg->vlp_protecten_clr, ADSP_TOP_PROT_STEP1_0_MASK);
}

void spm_mtcmos_ctrl_adsp_top_dormant_enable(void)
{
    setbits32(&mtk_spm->adsp_top_pwr_con, PWR_ON);
    setbits32(&mtk_spm->adsp_top_pwr_con, PWR_ON_2ND);
    while ((read32(&mtk_spm->adsp_top_pwr_con) & PWR_ACK) != PWR_ACK)
        ;
    while ((read32(&mtk_spm->adsp_top_pwr_con) & PWR_ACK_2ND) != PWR_ACK_2ND)
        ;
    clrbits32(&mtk_spm->adsp_top_pwr_con, PWR_CLK_DIS);
    clrbits32(&mtk_spm->adsp_top_pwr_con, PWR_ISO);
    setbits32(&mtk_spm->adsp_top_pwr_con, PWR_RST_B);
    setbits32(&mtk_spm->adsp_top_pwr_con, SRAM_SLEEP_B);
    while ((read32(&mtk_spm->adsp_top_pwr_con) & ADSP_TOP_SRAM_SLEEP_B_ACK)
        != ADSP_TOP_SRAM_SLEEP_B_ACK)
            ;
    setbits32(&mtk_spm->adsp_top_pwr_con, SRAM_ISOINT_B);
    clrbits32(&mtk_spm->adsp_top_pwr_con, SRAM_CKISO);
    write32(&mtk_vlpcfg->vlp_protecten_clr, ADSP_TOP_PROT_STEP2_0_MASK);
    write32(&mtk_vlpcfg->vlp_protecten_clr, ADSP_TOP_PROT_STEP1_0_MASK);
}

void spm_mtcmos_ctrl_adsp_top_dormant_disable(void)
{
    write32(&mtk_vlpcfg->vlp_protecten_set, ADSP_TOP_PROT_STEP1_0_MASK);
    while ((read32(&mtk_vlpcfg->vlp_protecten_sta1)
        & ADSP_TOP_PROT_STEP1_0_ACK_MASK)
        != ADSP_TOP_PROT_STEP1_0_ACK_MASK)
            ;
    write32(&mtk_vlpcfg->vlp_protecten_set, ADSP_TOP_PROT_STEP2_0_MASK);
    while ((read32(&mtk_vlpcfg->vlp_protecten_sta1)
        & ADSP_TOP_PROT_STEP2_0_ACK_MASK)
        != ADSP_TOP_PROT_STEP2_0_ACK_MASK)
            ;
    setbits32(&mtk_spm->adsp_top_pwr_con, SRAM_CKISO);
    clrbits32(&mtk_spm->adsp_top_pwr_con, SRAM_ISOINT_B);
    //udelay(1);
    clrbits32(&mtk_spm->adsp_top_pwr_con, SRAM_SLEEP_B);
    while (read32(&mtk_spm->adsp_top_pwr_con) & ADSP_TOP_SRAM_SLEEP_B_ACK)
        ;
    setbits32(&mtk_spm->adsp_top_pwr_con, PWR_ISO);
    setbits32(&mtk_spm->adsp_top_pwr_con, PWR_CLK_DIS);
    clrbits32(&mtk_spm->adsp_top_pwr_con, PWR_RST_B);
    clrbits32(&mtk_spm->adsp_top_pwr_con, PWR_ON);
    clrbits32(&mtk_spm->adsp_top_pwr_con, PWR_ON_2ND);
    while (read32(&mtk_spm->adsp_top_pwr_con) & PWR_ACK)
        ;
    while (read32(&mtk_spm->adsp_top_pwr_con) & PWR_ACK_2ND)
        ;
}

void spm_mtcmos_ctrl_adsp_ao_enable(void)
{
    setbits32(&mtk_spm->adsp_ao_pwr_con, PWR_ON);
    setbits32(&mtk_spm->adsp_ao_pwr_con, PWR_ON_2ND);
    while ((read32(&mtk_spm->adsp_ao_pwr_con) & PWR_ACK) != PWR_ACK)
        ;
    while ((read32(&mtk_spm->adsp_ao_pwr_con) & PWR_ACK_2ND) != PWR_ACK_2ND)
        ;
    clrbits32(&mtk_spm->adsp_ao_pwr_con, PWR_CLK_DIS);
    clrbits32(&mtk_spm->adsp_ao_pwr_con, PWR_ISO);
    setbits32(&mtk_spm->adsp_ao_pwr_con, PWR_RST_B);
    clrbits32(&mtk_spm->adsp_ao_pwr_con, SRAM_PDN);
}

void spm_mtcmos_ctrl_adsp_infra_enable(void)
{
    setbits32(&mtk_spm->adsp_infra_pwr_con, PWR_ON);
    setbits32(&mtk_spm->adsp_infra_pwr_con, PWR_ON_2ND);
    while ((read32(&mtk_spm->adsp_infra_pwr_con) & PWR_ACK) != PWR_ACK)
        ;
    while ((read32(&mtk_spm->adsp_infra_pwr_con) & PWR_ACK_2ND) != PWR_ACK_2ND)
        ;
    clrbits32(&mtk_spm->adsp_infra_pwr_con, PWR_CLK_DIS);
    clrbits32(&mtk_spm->adsp_infra_pwr_con, PWR_ISO);
    setbits32(&mtk_spm->adsp_infra_pwr_con, PWR_RST_B);
    clrbits32(&mtk_spm->adsp_infra_pwr_con, SRAM_PDN);
    write32(&mtk_vlpcfg->vlp_protecten_clr, ADSP_INFRA_PROT_STEP2_0_MASK);
    write32(&mtk_infra_ao->infra_protecten_clr0, ADSP_INFRA_PROT_STEP2_1_MASK);
    write32(&mtk_vlpcfg->vlp_protecten_clr, ADSP_INFRA_PROT_STEP1_0_MASK);
    write32(&mtk_infra_ao->infra_protecten_clr1, ADSP_INFRA_PROT_STEP1_1_MASK);
}

void spm_mtcmos_ctrl_adsp_infra_disable(void)
{
    write32(&mtk_vlpcfg->vlp_protecten_set, ADSP_INFRA_PROT_STEP1_0_MASK);
    while ((read32(&mtk_vlpcfg->vlp_protecten_sta1)
        & ADSP_INFRA_PROT_STEP1_0_ACK_MASK)
        != ADSP_INFRA_PROT_STEP1_0_ACK_MASK)
            ;
    write32(&mtk_infra_ao->infra_protecten_set1, ADSP_INFRA_PROT_STEP1_1_MASK);
    while ((read32(&mtk_infra_ao->infra_protecten_sta1)
        & ADSP_INFRA_PROT_STEP1_1_ACK_MASK)
        != ADSP_INFRA_PROT_STEP1_1_ACK_MASK)
            ;
    write32(&mtk_vlpcfg->vlp_protecten_set, ADSP_INFRA_PROT_STEP2_0_MASK);
    while ((read32(&mtk_vlpcfg->vlp_protecten_sta1)
        & ADSP_INFRA_PROT_STEP2_0_ACK_MASK)
        != ADSP_INFRA_PROT_STEP2_0_ACK_MASK)
            ;
    write32(&mtk_infra_ao->infra_protecten_set0, ADSP_INFRA_PROT_STEP2_1_MASK);
    while ((read32(&mtk_infra_ao->infra_protecten_sta0)
        & ADSP_INFRA_PROT_STEP2_1_ACK_MASK)
        != ADSP_INFRA_PROT_STEP2_1_ACK_MASK)
            ;
    setbits32(&mtk_spm->adsp_infra_pwr_con, SRAM_PDN);
    setbits32(&mtk_spm->adsp_infra_pwr_con, PWR_ISO);
    setbits32(&mtk_spm->adsp_infra_pwr_con, PWR_CLK_DIS);
    clrbits32(&mtk_spm->adsp_infra_pwr_con, PWR_RST_B);
    clrbits32(&mtk_spm->adsp_infra_pwr_con, PWR_ON);
    clrbits32(&mtk_spm->adsp_infra_pwr_con, PWR_ON_2ND);
    while (read32(&mtk_spm->adsp_infra_pwr_con) & PWR_ACK)
        ;
    while (read32(&mtk_spm->adsp_infra_pwr_con) & PWR_ACK_2ND)
        ;
}

void spm_mtcmos_ctrl_mm_proc_shutdown_enable(void)
{
    setbits32(&mtk_spm->mmpro_pwr_con, PWR_ON);
    setbits32(&mtk_spm->mmpro_pwr_con, PWR_ON_2ND);
    while ((read32(&mtk_spm->mmpro_pwr_con) & PWR_ACK) != PWR_ACK)
        ;
    while ((read32(&mtk_spm->mmpro_pwr_con) & PWR_ACK_2ND) != PWR_ACK_2ND)
        ;
    clrbits32(&mtk_spm->mmpro_pwr_con, PWR_CLK_DIS);
    clrbits32(&mtk_spm->mmpro_pwr_con, PWR_ISO);
    setbits32(&mtk_spm->mmpro_pwr_con, PWR_RST_B);
    clrbits32(&mtk_spm->mmpro_pwr_con, SRAM_PDN);

    clrbits32(&mtk_spm->emi_hre_sram_con, MM_PROC_SRAM_SLEEP_20);
    clrbits32(&mtk_spm->emi_hre_sram_con, MM_PROC_SRAM_SLEEP_21);
    clrbits32(&mtk_spm->emi_hre_sram_con, MM_PROC_SRAM_SLEEP_22);
    clrbits32(&mtk_spm->emi_hre_sram_con, MM_PROC_SRAM_SLEEP_23);

    while ((read32(&mtk_spm->mmpro_pwr_con) & MM_PROC_SRAM_PDN_ACK))
        ;
    clrbits32(&mtk_spm->mmpro_pwr_con, SRAM_CKISO);
    write32(&mtk_vlpcfg->vlp_protecten_clr, MM_PROC_PROT_STEP2_0_MASK);
    write32(&mtk_vlpcfg->vlp_protecten_clr, MM_PROC_PROT_STEP1_0_MASK);
}

void spm_mtcmos_ctrl_mm_proc_shutdown_disable(void)
{
    write32(&mtk_vlpcfg->vlp_protecten_set, MM_PROC_PROT_STEP1_0_MASK);
    while ((read32(&mtk_vlpcfg->vlp_protecten_sta1)
        & MM_PROC_PROT_STEP1_0_ACK_MASK)
        != MM_PROC_PROT_STEP1_0_ACK_MASK)
            ;
    write32(&mtk_vlpcfg->vlp_protecten_set, MM_PROC_PROT_STEP2_0_MASK);
    while ((read32(&mtk_vlpcfg->vlp_protecten_sta1)
        & MM_PROC_PROT_STEP2_0_ACK_MASK)
        != MM_PROC_PROT_STEP2_0_ACK_MASK)
            ;
    setbits32(&mtk_spm->mmpro_pwr_con, SRAM_CKISO);
    setbits32(&mtk_spm->mmpro_pwr_con, SRAM_PDN);

    setbits32(&mtk_spm->emi_hre_sram_con, MM_PROC_SRAM_SLEEP_20);
    setbits32(&mtk_spm->emi_hre_sram_con, MM_PROC_SRAM_SLEEP_21);
    setbits32(&mtk_spm->emi_hre_sram_con, MM_PROC_SRAM_SLEEP_22);
    setbits32(&mtk_spm->emi_hre_sram_con, MM_PROC_SRAM_SLEEP_23);

    while ((read32(&mtk_spm->mmpro_pwr_con) & MM_PROC_SRAM_PDN_ACK) != MM_PROC_SRAM_PDN_ACK)
        ;
    setbits32(&mtk_spm->mmpro_pwr_con, PWR_ISO);
    setbits32(&mtk_spm->mmpro_pwr_con, PWR_CLK_DIS);
    clrbits32(&mtk_spm->mmpro_pwr_con, PWR_RST_B);
    clrbits32(&mtk_spm->mmpro_pwr_con, PWR_ON);
    clrbits32(&mtk_spm->mmpro_pwr_con, PWR_ON_2ND);
    while (read32(&mtk_spm->mmpro_pwr_con) & PWR_ACK)
        ;
    while (read32(&mtk_spm->mmpro_pwr_con) & PWR_ACK_2ND)
        ;
}

void spm_mtcmos_ctrl_mm_proc_dormant_enable(void)
{
    setbits32(&mtk_spm->mmpro_pwr_con, PWR_ON);
    setbits32(&mtk_spm->mmpro_pwr_con, PWR_ON_2ND);
    while ((read32(&mtk_spm->mmpro_pwr_con) & PWR_ACK) != PWR_ACK)
        ;
    while ((read32(&mtk_spm->mmpro_pwr_con) & PWR_ACK_2ND) != PWR_ACK_2ND)
        ;
    clrbits32(&mtk_spm->mmpro_pwr_con, PWR_CLK_DIS);
    clrbits32(&mtk_spm->mmpro_pwr_con, PWR_ISO);
    setbits32(&mtk_spm->mmpro_pwr_con, PWR_RST_B);
    clrbits32(&mtk_spm->mmpro_pwr_con, SRAM_PDN);
    while ((read32(&mtk_spm->mmpro_pwr_con) & MM_PROC_SRAM_PDN_ACK))
        ;
    setbits32(&mtk_spm->emi_hre_sram_con, MM_PROC_SRAM_SLEEP_8);
    setbits32(&mtk_spm->emi_hre_sram_con, MM_PROC_SRAM_SLEEP_9);
    setbits32(&mtk_spm->emi_hre_sram_con, MM_PROC_SRAM_SLEEP_10);
    setbits32(&mtk_spm->emi_hre_sram_con, MM_PROC_SRAM_SLEEP_11);

    setbits32(&mtk_spm->mmpro_pwr_con, SRAM_ISOINT_B);
    clrbits32(&mtk_spm->mmpro_pwr_con, SRAM_CKISO);
    write32(&mtk_vlpcfg->vlp_protecten_clr, MM_PROC_PROT_STEP2_0_MASK);
    write32(&mtk_vlpcfg->vlp_protecten_clr, MM_PROC_PROT_STEP1_0_MASK);
}

void spm_mtcmos_ctrl_mm_proc_dormant_disable(void)
{
    write32(&mtk_vlpcfg->vlp_protecten_set, MM_PROC_PROT_STEP1_0_MASK);
    while ((read32(&mtk_vlpcfg->vlp_protecten_sta1)
        & MM_PROC_PROT_STEP1_0_ACK_MASK)
        != MM_PROC_PROT_STEP1_0_ACK_MASK)
            ;
    write32(&mtk_vlpcfg->vlp_protecten_set, MM_PROC_PROT_STEP2_0_MASK);
    while ((read32(&mtk_vlpcfg->vlp_protecten_sta1)
        & MM_PROC_PROT_STEP2_0_ACK_MASK)
        != MM_PROC_PROT_STEP2_0_ACK_MASK)
            ;
    setbits32(&mtk_spm->mmpro_pwr_con, SRAM_CKISO);
    clrbits32(&mtk_spm->mmpro_pwr_con, SRAM_ISOINT_B);
    setbits32(&mtk_spm->mmpro_pwr_con, SRAM_PDN);
    while ((read32(&mtk_spm->mmpro_pwr_con) & MM_PROC_SRAM_PDN_ACK) != MM_PROC_SRAM_PDN_ACK)
        ;

    clrbits32(&mtk_spm->emi_hre_sram_con, MM_PROC_SRAM_SLEEP_8);
    clrbits32(&mtk_spm->emi_hre_sram_con, MM_PROC_SRAM_SLEEP_9);
    clrbits32(&mtk_spm->emi_hre_sram_con, MM_PROC_SRAM_SLEEP_10);
    clrbits32(&mtk_spm->emi_hre_sram_con, MM_PROC_SRAM_SLEEP_11);

    setbits32(&mtk_spm->mmpro_pwr_con, PWR_ISO);
    setbits32(&mtk_spm->mmpro_pwr_con, PWR_CLK_DIS);
    clrbits32(&mtk_spm->mmpro_pwr_con, PWR_RST_B);
    clrbits32(&mtk_spm->mmpro_pwr_con, PWR_ON);
    clrbits32(&mtk_spm->mmpro_pwr_con, PWR_ON_2ND);
    while (read32(&mtk_spm->mmpro_pwr_con) & PWR_ACK)
        ;
    while (read32(&mtk_spm->mmpro_pwr_con) & PWR_ACK_2ND)
        ;
}

void spm_mtcmos_ctrl_mfg0_shutdown_enable(void)
{
    setbits32(&mtk_spm->mfg0_pwr_con, PWR_ON);
    setbits32(&mtk_spm->mfg0_pwr_con, PWR_ON_2ND);
    while ((read32(&mtk_spm->mfg0_pwr_con) & PWR_ACK) != PWR_ACK)
        ;
    while ((read32(&mtk_spm->mfg0_pwr_con) & PWR_ACK_2ND) != PWR_ACK_2ND)
        ;
    clrbits32(&mtk_spm->mfg0_pwr_con, PWR_CLK_DIS);
    clrbits32(&mtk_spm->mfg0_pwr_con, PWR_ISO);
    setbits32(&mtk_spm->mfg0_pwr_con, PWR_RST_B);
    clrbits32(&mtk_spm->mfg0_pwr_con, SRAM_PDN);
    while (read32(&mtk_spm->mfg0_pwr_con) & MFG0_SRAM_PDN_ACK)
        ;
    setbits32(&mtk_spm->mfg0_pwr_con, PWR_ON);
    setbits32(&mtk_spm->mfg0_pwr_con, PWR_ON);
    write32(&mtk_infra_ao->infra_protecten_clr0, MFG0_PROT_STEP2_0_MASK);
    write32(&mtk_infra_ao->md_mfg_protecten_clr0, MFG0_PROT_STEP1_0_MASK);
}

void spm_mtcmos_ctrl_mfg0_dormant_disable(void)
{
    write32(&mtk_infra_ao->md_mfg_protecten_set0, MFG0_PROT_STEP1_0_MASK);
    while ((read32(&mtk_infra_ao->md_mfg_protecten_sta0)
        & MFG0_PROT_STEP1_0_ACK_MASK)
        != MFG0_PROT_STEP1_0_ACK_MASK)
            ;
    write32(&mtk_infra_ao->infra_protecten_set0, MFG0_PROT_STEP2_0_MASK);
    while ((read32(&mtk_infra_ao->infra_protecten_sta0)
        & MFG0_PROT_STEP2_0_ACK_MASK)
        != MFG0_PROT_STEP2_0_ACK_MASK)
            ;
    setbits32(&mtk_spm->mfg0_pwr_con, SRAM_CKISO);
    clrbits32(&mtk_spm->mfg0_pwr_con, SRAM_ISOINT_B);
    //udelay(1);
    clrbits32(&mtk_spm->mfg0_pwr_con, SRAM_SLEEP_B);
    while (read32(&mtk_spm->mfg0_pwr_con) & MFG0_SRAM_SLEEP_B_ACK)
        ;
    setbits32(&mtk_spm->mfg0_pwr_con, PWR_ISO);
    setbits32(&mtk_spm->mfg0_pwr_con, PWR_CLK_DIS);
    clrbits32(&mtk_spm->mfg0_pwr_con, PWR_RST_B);
    clrbits32(&mtk_spm->mfg0_pwr_con, PWR_ON);
    clrbits32(&mtk_spm->mfg0_pwr_con, PWR_ON_2ND);
    while (read32(&mtk_spm->mfg0_pwr_con) & PWR_ACK)
        ;
    while (read32(&mtk_spm->mfg0_pwr_con) & PWR_ACK_2ND)
        ;
}

void spm_mtcmos_ctrl_mfg1_enable(void)
{
    setbits32(&mtk_spm->mfg1_pwr_con, PWR_ON);
    setbits32(&mtk_spm->mfg1_pwr_con, PWR_ON_2ND);
    while ((read32(&mtk_spm->mfg1_pwr_con) & PWR_ACK) != PWR_ACK)
        ;
    while ((read32(&mtk_spm->mfg1_pwr_con) & PWR_ACK_2ND) != PWR_ACK_2ND)
        ;
    clrbits32(&mtk_spm->mfg1_pwr_con, PWR_CLK_DIS);
    clrbits32(&mtk_spm->mfg1_pwr_con, PWR_ISO);
    setbits32(&mtk_spm->mfg1_pwr_con, PWR_RST_B);
    clrbits32(&mtk_spm->mfg1_pwr_con, SRAM_PDN);
    while ((read32(&mtk_spm->mfg1_pwr_con) & MFG1_SRAM_PDN_ACK))
        ;
    write32(&mtk_infra_ao->md_mfg_protecten_clr0, MFG1_PROT_STEP4_0_MASK);
    write32(&mtk_infra_ao->md_mfg_protecten_clr0, MFG1_PROT_STEP3_0_MASK);
    write32(&mtk_infra_ao->md_mfg_protecten_clr0, MFG1_PROT_STEP2_0_MASK);
    write32(&mtk_infra_ao->md_mfg_protecten_clr0, MFG1_PROT_STEP1_0_MASK);
}

void spm_mtcmos_ctrl_mfg1_disable(void)
{
    write32(&mtk_infra_ao->md_mfg_protecten_set0, MFG1_PROT_STEP1_0_MASK);
    while ((read32(&mtk_infra_ao->md_mfg_protecten_sta0)
        & MFG1_PROT_STEP1_0_ACK_MASK)
        != MFG1_PROT_STEP1_0_ACK_MASK)
            ;
    write32(&mtk_infra_ao->md_mfg_protecten_set0, MFG1_PROT_STEP2_0_MASK);
    while ((read32(&mtk_infra_ao->md_mfg_protecten_sta0)
        & MFG1_PROT_STEP2_0_ACK_MASK)
        != MFG1_PROT_STEP2_0_ACK_MASK)
            ;
    write32(&mtk_infra_ao->md_mfg_protecten_set0, MFG1_PROT_STEP3_0_MASK);
    while ((read32(&mtk_infra_ao->md_mfg_protecten_sta0)
        & MFG1_PROT_STEP3_0_ACK_MASK)
        != MFG1_PROT_STEP3_0_ACK_MASK)
            ;
    write32(&mtk_infra_ao->md_mfg_protecten_set0, MFG1_PROT_STEP4_0_MASK);
    while ((read32(&mtk_infra_ao->md_mfg_protecten_sta0)
        & MFG1_PROT_STEP4_0_ACK_MASK)
        != MFG1_PROT_STEP4_0_ACK_MASK)
            ;
    setbits32(&mtk_spm->mfg1_pwr_con, SRAM_PDN);
    while ((read32(&mtk_spm->mfg1_pwr_con)
        & MFG1_SRAM_PDN_ACK)
        != MFG1_SRAM_PDN_ACK)
            ;
    setbits32(&mtk_spm->mfg1_pwr_con, PWR_ISO);
    setbits32(&mtk_spm->mfg1_pwr_con, PWR_CLK_DIS);
    clrbits32(&mtk_spm->mfg1_pwr_con, PWR_RST_B);
    clrbits32(&mtk_spm->mfg1_pwr_con, PWR_ON);
    clrbits32(&mtk_spm->mfg1_pwr_con, PWR_ON_2ND);
    while (read32(&mtk_spm->mfg1_pwr_con) & PWR_ACK)
        ;
    while (read32(&mtk_spm->mfg1_pwr_con) & PWR_ACK_2ND)
        ;
}
