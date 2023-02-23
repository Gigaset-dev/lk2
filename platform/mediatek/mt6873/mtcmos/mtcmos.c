/*
 * Copyright (c) 2020 MediaTek Inc.
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
    uint32_t reserved0[91];
    uint32_t pwr_status;
    uint32_t pwr_status_2nd;
    uint32_t reserved1[99];
    uint32_t md1_pwr_con;
    uint32_t reserved2[21];
    uint32_t adsp_pwr_con;
    uint32_t reserved3[15];
    uint32_t md_ext_buck_iso_con;
    uint32_t ext_buck_iso_con;
};

static struct mtk_spm_regs *const mtk_spm = (void *)SPM_BASE;

STATIC_ASSERT(__offsetof(struct mtk_spm_regs, pwr_status) == 0x16C);
STATIC_ASSERT(__offsetof(struct mtk_spm_regs, pwr_status_2nd) == 0x170);
STATIC_ASSERT(__offsetof(struct mtk_spm_regs, md1_pwr_con) == 0x300);
STATIC_ASSERT(__offsetof(struct mtk_spm_regs, adsp_pwr_con) == 0x358);
STATIC_ASSERT(__offsetof(struct mtk_spm_regs, md_ext_buck_iso_con) == 0x398);
STATIC_ASSERT(__offsetof(struct mtk_spm_regs, ext_buck_iso_con) == 0x39C);

struct mtk_infra_ao_regs {
    uint32_t reserved0[136];
    uint32_t infra_topaxi_protecten;
    uint32_t reserved1[1];
    uint32_t infra_topaxi_protecten_sta1;
    uint32_t reserved2[9];
    uint32_t infra_topaxi_protecten_1;
    uint32_t reserved3[1];
    uint32_t infra_topaxi_protecten_sta1_1;
    uint32_t reserved4[17];
    uint32_t infra_topaxi_protecten_set;
    uint32_t infra_topaxi_protecten_clr;
    uint32_t infra_topaxi_protecten_1_set;
    uint32_t infra_topaxi_protecten_1_clr;
    uint32_t reserved5[281];
    uint32_t infra_topaxi_protecten_2_set;
    uint32_t infra_topaxi_protecten_2_clr;
    uint32_t reserved6[2];
    uint32_t infra_topaxi_protecten_sta1_2;
    uint32_t reserved7[279];
    uint32_t infra_topaxi_protecten_vdnr_set;
    uint32_t infra_topaxi_protecten_vdnr_clr;
    uint32_t infra_topaxi_protecten_vdnr_sta0;
    uint32_t infra_topaxi_protecten_vdnr_sta1;
};

static struct mtk_infra_ao_regs *const mtk_infra_ao = (void *)INFRACFG_AO_BASE;

STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, infra_topaxi_protecten) == 0x220);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, infra_topaxi_protecten_sta1) == 0x228);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, infra_topaxi_protecten_1) == 0x250);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, infra_topaxi_protecten_sta1_1) == 0x258);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, infra_topaxi_protecten_set) == 0x2A0);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, infra_topaxi_protecten_clr) == 0x2A4);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, infra_topaxi_protecten_1_set) == 0x2A8);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, infra_topaxi_protecten_1_clr) == 0x2AC);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, infra_topaxi_protecten_2_set) == 0x714);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, infra_topaxi_protecten_2_clr) == 0x718);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, infra_topaxi_protecten_sta1_2) == 0x724);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, infra_topaxi_protecten_vdnr_set) == 0xB84);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, infra_topaxi_protecten_vdnr_clr) == 0xB88);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, infra_topaxi_protecten_vdnr_sta0) == 0xB8C);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, infra_topaxi_protecten_vdnr_sta1) == 0xB90);

enum {
    SRAM_ISOINT_B = 1U << 6,
    SRAM_CKISO    = 1U << 5,
    PWR_CLK_DIS   = 1U << 4,
    PWR_ON_2ND    = 1U << 3,
    PWR_ON        = 1U << 2,
    PWR_ISO       = 1U << 1,
    PWR_RST_B     = 1U << 0
};

enum {
    MD1_PROT_STEP1_0_MASK = 0x1 << 7,
    MD1_PROT_STEP2_0_MASK = ((0x1 << 2) | (0x1 << 14) | (0x1 << 22)),
    ADSP_PROT_STEP1_0_MASK = 0x1 << 3,
};

enum {
    MD1_PROT_STEP1_0_ACK_MASK = 0x1 << 7,
    MD1_PROT_STEP2_0_ACK_MASK = ((0x1 << 2) | (0x1 << 14) | (0x1 << 22)),
    ADSP_PROT_STEP1_0_ACK_MASK = 0x1 << 3,
};

enum pwr_sta {
    MD1_PWR_STA_MASK = 0x1 << 0,
    ADSP_PWR_STA_MASK = 0x1 << 22,
};

enum {
    ADSP_SRAM_PDN = 0x1 << 8,
    ADSP_SRAM_SLEEP_B = 0x1 << 9,
};

enum {
    ADSP_SRAM_PDN_ACK = 0x1 << 12,
    ADSP_SRAM_PDN_ACK_BIT0 = 0x1 << 12,
    ADSP_SRAM_SLEEP_B_ACK = 0x1 << 13,
    ADSP_SRAM_SLEEP_B_ACK_BIT0 = 0x1 << 13,
};

void spm_mtcmos_ctrl_md1_enable(void)
{
    clrbits32(&mtk_spm->md_ext_buck_iso_con, 0x1 << 0);
    clrbits32(&mtk_spm->md_ext_buck_iso_con, 0x1 << 1);
    setbits32(&mtk_spm->md1_pwr_con, PWR_RST_B);
    setbits32(&mtk_spm->md1_pwr_con, PWR_ON);
    while ((read32(&mtk_spm->pwr_status) & MD1_PWR_STA_MASK) != MD1_PWR_STA_MASK)
        ;
    write32(&mtk_infra_ao->infra_topaxi_protecten_vdnr_clr, MD1_PROT_STEP2_0_MASK);
    write32(&mtk_infra_ao->infra_topaxi_protecten_clr, MD1_PROT_STEP1_0_MASK);
}

void spm_mtcmos_ctrl_md1_disable(void)
{
    write32(&mtk_infra_ao->infra_topaxi_protecten_set, MD1_PROT_STEP1_0_MASK);
    while ((read32(&mtk_infra_ao->infra_topaxi_protecten_sta1)
        & MD1_PROT_STEP1_0_ACK_MASK)
        != MD1_PROT_STEP1_0_ACK_MASK)
            ;
    write32(&mtk_infra_ao->infra_topaxi_protecten_vdnr_set, MD1_PROT_STEP2_0_MASK);
    while ((read32(&mtk_infra_ao->infra_topaxi_protecten_vdnr_sta1)
        & MD1_PROT_STEP2_0_ACK_MASK)
        != MD1_PROT_STEP2_0_ACK_MASK)
            ;
    clrbits32(&mtk_spm->md1_pwr_con, PWR_ON);
    while (read32(&mtk_spm->pwr_status) & MD1_PWR_STA_MASK)
        ;
    setbits32(&mtk_spm->md_ext_buck_iso_con, 0x1 << 0);
    setbits32(&mtk_spm->md_ext_buck_iso_con, 0x1 << 1);
    clrbits32(&mtk_spm->md1_pwr_con, PWR_RST_B);
}

void spm_mtcmos_ctrl_adsp_enable(void)
{
    clrbits32(&mtk_spm->ext_buck_iso_con, 0x1 << 2);
    setbits32(&mtk_spm->adsp_pwr_con, PWR_ON);
    setbits32(&mtk_spm->adsp_pwr_con, PWR_ON_2ND);
    while ((read32(&mtk_spm->pwr_status) & ADSP_PWR_STA_MASK) != ADSP_PWR_STA_MASK)
        ;
    while ((read32(&mtk_spm->pwr_status_2nd) & ADSP_PWR_STA_MASK) != ADSP_PWR_STA_MASK)
        ;
    clrbits32(&mtk_spm->adsp_pwr_con, PWR_CLK_DIS);
    clrbits32(&mtk_spm->adsp_pwr_con, PWR_ISO);
    setbits32(&mtk_spm->adsp_pwr_con, PWR_RST_B);
    clrbits32(&mtk_spm->adsp_pwr_con, 0x1 << 8);
    while (read32(&mtk_spm->adsp_pwr_con) & ADSP_SRAM_PDN_ACK_BIT0)
        ;
    setbits32(&mtk_spm->adsp_pwr_con, SRAM_ISOINT_B);
    clrbits32(&mtk_spm->adsp_pwr_con, SRAM_CKISO);
    write32(&mtk_infra_ao->infra_topaxi_protecten_2_clr, ADSP_PROT_STEP1_0_MASK);
}

void spm_mtcmos_ctrl_adsp_disable(void)
{
    write32(&mtk_infra_ao->infra_topaxi_protecten_2_set, ADSP_PROT_STEP1_0_MASK);
    while ((read32(&mtk_infra_ao->infra_topaxi_protecten_sta1_2)
        & ADSP_PROT_STEP1_0_ACK_MASK)
        != ADSP_PROT_STEP1_0_ACK_MASK)
            ;
    setbits32(&mtk_spm->adsp_pwr_con, SRAM_CKISO);
    clrbits32(&mtk_spm->adsp_pwr_con, SRAM_ISOINT_B);
    clrbits32(&mtk_spm->adsp_pwr_con, ADSP_SRAM_SLEEP_B);
    while (read32(&mtk_spm->adsp_pwr_con) & ADSP_SRAM_SLEEP_B_ACK)
        ;
    setbits32(&mtk_spm->adsp_pwr_con, PWR_ISO);
    setbits32(&mtk_spm->adsp_pwr_con, PWR_CLK_DIS);
    clrbits32(&mtk_spm->adsp_pwr_con, PWR_RST_B);
    clrbits32(&mtk_spm->adsp_pwr_con, PWR_ON);
    clrbits32(&mtk_spm->adsp_pwr_con, PWR_ON_2ND);
    while (read32(&mtk_spm->pwr_status) & ADSP_PWR_STA_MASK)
        ;
    while (read32(&mtk_spm->pwr_status_2nd) & ADSP_PWR_STA_MASK)
        ;
}
