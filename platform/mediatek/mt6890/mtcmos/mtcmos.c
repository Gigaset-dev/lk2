/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the License file or at
 * https://opensource.org/license/MIT
 */
#include <assert.h>
#include <mtcmos.h>
#include <platform/addressmap.h>
#include <platform/reg.h>

#define  SPM_PROJECT_CODE    0xB16
#define  POWERON_CONFIG_EN   SPM_BASE

struct mtk_spm_regs {
    uint32_t reserved0[91];
    uint32_t pwr_status;
    uint32_t pwr_status_2nd;
    uint32_t reserved1[99];
    uint32_t md1_pwr_con;
    uint32_t reserved2[18];
    uint32_t netsys_pwr_con;
    uint32_t dis_pwr_con;
    uint32_t aud_pwr_con;
    uint32_t reserved3[8];
    uint32_t netsys_s_sram_con;
    uint32_t reserved4[7];
    uint32_t md_ext_buck_iso_con;
};

static struct mtk_spm_regs *const mtk_spm = (void *)SPM_BASE;

STATIC_ASSERT(__offsetof(struct mtk_spm_regs, pwr_status) == 0x16C);
STATIC_ASSERT(__offsetof(struct mtk_spm_regs, pwr_status_2nd) == 0x170);
STATIC_ASSERT(__offsetof(struct mtk_spm_regs, md1_pwr_con) == 0x300);
STATIC_ASSERT(__offsetof(struct mtk_spm_regs, netsys_pwr_con) == 0x34c);
STATIC_ASSERT(__offsetof(struct mtk_spm_regs, dis_pwr_con) == 0x350);
STATIC_ASSERT(__offsetof(struct mtk_spm_regs, aud_pwr_con) == 0x354);
STATIC_ASSERT(__offsetof(struct mtk_spm_regs, netsys_s_sram_con) == 0x378);
STATIC_ASSERT(__offsetof(struct mtk_spm_regs, md_ext_buck_iso_con) == 0x398);

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
    uint32_t reserved5[8];
    uint32_t infra_topaxi_protecten_mm;
    uint32_t infra_topaxi_protecten_mm_set;
    uint32_t infra_topaxi_protecten_mm_clr;
    uint32_t reserved6[3];
    uint32_t infra_topaxi_protecten_mm_sta0;
    uint32_t infra_topaxi_protecten_mm_sta1;
    uint32_t reserved7[265];
    uint32_t infra_topaxi_protecten_2_set;
    uint32_t infra_topaxi_protecten_2_clr;
    uint32_t reserved8[1];
    uint32_t infra_topaxi_protecten_sta0_2;
    uint32_t infra_topaxi_protecten_sta1_2;
    uint32_t reserved9[278];
    uint32_t infra_topaxi_protecten_infra_vdnr;
    uint32_t infra_topaxi_protecten_infra_vdnr_set;
    uint32_t infra_topaxi_protecten_infra_vdnr_clr;
    uint32_t infra_topaxi_protecten_infra_vdnr_sta0;
    uint32_t infra_topaxi_protecten_infra_vdnr_sta1;
    uint32_t reserved10[3];
    uint32_t infra_topaxi_protecten_infra_vdnr_1;
    uint32_t infra_topaxi_protecten_infra_vdnr_1_set;
    uint32_t infra_topaxi_protecten_infra_vdnr_1_clr;
    uint32_t infra_topaxi_protecten_infra_vdnr_1_sta0;
    uint32_t infra_topaxi_protecten_infra_vdnr_1_sta1;
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
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, infra_topaxi_protecten_mm) == 0x02D0);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, infra_topaxi_protecten_mm_set) == 0x02D4);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, infra_topaxi_protecten_mm_clr) == 0x02D8);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, infra_topaxi_protecten_mm_sta0) == 0x02E8);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, infra_topaxi_protecten_mm_sta1) == 0x02EC);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, infra_topaxi_protecten_2_set) == 0x0714);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, infra_topaxi_protecten_2_clr) == 0x0718);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, infra_topaxi_protecten_sta0_2) == 0x0720);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs, infra_topaxi_protecten_sta1_2) == 0x0724);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs,
                         infra_topaxi_protecten_infra_vdnr) == 0x0B80);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs,
                         infra_topaxi_protecten_infra_vdnr_set) == 0x0B84);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs,
                         infra_topaxi_protecten_infra_vdnr_clr) == 0x0B88);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs,
                         infra_topaxi_protecten_infra_vdnr_sta0) == 0x0B8c);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs,
                         infra_topaxi_protecten_infra_vdnr_sta1) == 0x0B90);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs,
                         infra_topaxi_protecten_infra_vdnr_1) == 0x0BA0);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs,
                         infra_topaxi_protecten_infra_vdnr_1_set) == 0x0BA4);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs,
                         infra_topaxi_protecten_infra_vdnr_1_clr) == 0x0BA8);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs,
                         infra_topaxi_protecten_infra_vdnr_1_sta0) == 0x0BAc);
STATIC_ASSERT(__offsetof(struct mtk_infra_ao_regs,
                         infra_topaxi_protecten_infra_vdnr_1_sta1) == 0x0BB0);

/* Define MTCMOS power control */
enum {
    SLPB_CLAMP    = 1U << 7,
    VPROC_EXT_OFF = 1U << 7,
    SRAM_ISOINT_B = 1U << 6,
    SRAM_CKISO    = 1U << 5,
    PWR_CLK_DIS   = 1U << 4,
    PWR_ON_2ND    = 1U << 3,
    PWR_ON        = 1U << 2,
    PWR_ISO       = 1U << 1,
    PWR_RST_B     = 1U << 0
};

/* Define MTCMOS Bus Protect Mask */
enum {
    MD1_PROT_STEP1_0_MASK = 0x1 << 7,
    MD1_PROT_STEP2_0_MASK = ((0x1 << 2)|(0x1 << 10)),
    MD1_PROT_STEP2_1_MASK = 0x1 << 6,
    DIS_PROT_STEP1_0_MASK = 0x1 << 10,
    DIS_PROT_STEP2_0_MASK = 0x1 << 6,
    NETSYS_PROT_STEP1_0_MASK = ((0x1 << 8)|(0x1 << 9)|(0x1 << 12)|(0x1 << 13)|(0x1 << 14)),
    NETSYS_PROT_STEP2_0_MASK = ((0x1 << 10)|(0x1 << 11)|(0x1 << 17)|(0x1 << 18)|(0x1 << 19)),
    NETSYS_PROT_STEP3_0_MASK = ((0x1 << 15)|(0x1 << 23)),
    NETSYS_PROT_STEP4_0_MASK = ((0x1 << 10)|(0x1 << 11)|(0x1 << 15)),
};

enum {
    MD1_PROT_STEP1_0_ACK_MASK = 0x1 << 7,
    MD1_PROT_STEP2_0_ACK_MASK = ((0x1 << 2)|(0x1 << 10)),
    MD1_PROT_STEP2_1_ACK_MASK = 0x1 << 6,
    DIS_PROT_STEP1_0_ACK_MASK = 0x1 << 10,
    DIS_PROT_STEP2_0_ACK_MASK = 0x1 << 6,
    NETSYS_PROT_STEP1_0_ACK_MASK = ((0x1 << 8)|(0x1 << 9)|(0x1 << 12)|(0x1 << 13)|(0x1 << 14)),
    NETSYS_PROT_STEP2_0_ACK_MASK = ((0x1 << 10)|(0x1 << 11)|(0x1 << 17)|(0x1 << 18)|(0x1 << 19)),
    NETSYS_PROT_STEP3_0_ACK_MASK = ((0x1 << 15)|(0x1 << 23)),
    NETSYS_PROT_STEP4_0_ACK_MASK = ((0x1 << 10)|(0x1 << 11)|(0x1 << 15)),
};

enum {
    MD1_SRAM_PDN = 0x1 << 8,
    MD1_SRAM_PDN_ACK = 0x0 << 12,
    MD1_SRAM_PDN_ACK_BIT0 = 0x1 << 12,
    DIS_SRAM_PDN = 0x1 << 8,
    DIS_SRAM_PDN_ACK = 0x1 << 12,
    DIS_SRAM_PDN_ACK_BIT0 = 0x1 << 12,
    AUDIO_SRAM_PDN = 0x1 << 8,
    AUDIO_SRAM_PDN_ACK = 0x1 << 12,
    AUDIO_SRAM_PDN_ACK_BIT0 = 0x1 << 12,
    NETSYS_SRAM_PDN = 0x1 << 16,
    NETSYS_SRAM_PDN_ACK = 0x1 << 24,
    NETSYS_SRAM_PDN_ACK_BIT0 = 0x1 << 24,
    NETSYS_SRAM_PDN2 = 0x1 << 17,
    NETSYS_SRAM_PDN2_ACK = 0x1 << 25,
    NETSYS_SRAM_PDN2_ACK_BIT0 = 0x1 << 25,
};

/* Define MTCMOS Power Status Mask */
enum pwr_sta {
    MD1_PWR_STA_MASK = 0x1 << 0,
    NETSYS_PWR_STA_MASK = 0x1 << 19,
    DIS_PWR_STA_MASK = 0x1 << 20,
    AUDIO_PWR_STA_MASK = 0x1 << 21,
};

int spm_mtcmos_ctrl_md1(enum PWR_STA state)
{
    int err = 0;

    write32(POWERON_CONFIG_EN, (SPM_PROJECT_CODE << 16) | (0x1 << 0));

    if (state == STA_POWER_DOWN) {
        write32(&mtk_infra_ao->infra_topaxi_protecten_set, MD1_PROT_STEP1_0_MASK);
        while ((read32(&mtk_infra_ao->infra_topaxi_protecten_sta1)
            & MD1_PROT_STEP1_0_ACK_MASK)
            != MD1_PROT_STEP1_0_ACK_MASK)
                ;
        write32(&mtk_infra_ao->infra_topaxi_protecten_infra_vdnr_set, MD1_PROT_STEP2_0_MASK);
        while ((read32(&mtk_infra_ao->infra_topaxi_protecten_infra_vdnr_sta1)
            & MD1_PROT_STEP2_0_ACK_MASK)
            != MD1_PROT_STEP2_0_ACK_MASK)
                ;
        write32(&mtk_infra_ao->infra_topaxi_protecten_infra_vdnr_1_set, MD1_PROT_STEP2_1_MASK);
        while ((read32(&mtk_infra_ao->infra_topaxi_protecten_infra_vdnr_1_sta1)
            & MD1_PROT_STEP2_1_ACK_MASK)
            != MD1_PROT_STEP2_1_ACK_MASK)
                ;
        clrbits32(&mtk_spm->md1_pwr_con, PWR_ON);
        while (read32(&mtk_spm->pwr_status) & MD1_PWR_STA_MASK)
            ;
        setbits32(&mtk_spm->md_ext_buck_iso_con, 0x1 << 0);
        setbits32(&mtk_spm->md_ext_buck_iso_con, 0x1 << 1);
        clrbits32(&mtk_spm->md1_pwr_con, PWR_RST_B);
    } else {
        clrbits32(&mtk_spm->md_ext_buck_iso_con, 0x1 << 0);
        clrbits32(&mtk_spm->md_ext_buck_iso_con, 0x1 << 1);
        setbits32(&mtk_spm->md1_pwr_con, PWR_RST_B);
        setbits32(&mtk_spm->md1_pwr_con, PWR_ON);
        while ((read32(&mtk_spm->pwr_status) & MD1_PWR_STA_MASK) != MD1_PWR_STA_MASK)
            ;
        write32(&mtk_infra_ao->infra_topaxi_protecten_infra_vdnr_clr, MD1_PROT_STEP2_0_MASK);
        write32(&mtk_infra_ao->infra_topaxi_protecten_infra_vdnr_1_clr, MD1_PROT_STEP2_1_MASK);
        write32(&mtk_infra_ao->infra_topaxi_protecten_clr, MD1_PROT_STEP1_0_MASK);
    }
    return err;
}

int spm_mtcmos_ctrl_dis(enum PWR_STA state)
{
    int err = 0;

    write32(POWERON_CONFIG_EN, (SPM_PROJECT_CODE << 16) | (0x1 << 0));

    if (state == STA_POWER_DOWN) {
        write32(&mtk_infra_ao->infra_topaxi_protecten_mm_set, DIS_PROT_STEP1_0_MASK);
        while ((read32(&mtk_infra_ao->infra_topaxi_protecten_mm_sta1)
            & DIS_PROT_STEP1_0_ACK_MASK)
            != DIS_PROT_STEP1_0_ACK_MASK)
                ;
        write32(&mtk_infra_ao->infra_topaxi_protecten_set, DIS_PROT_STEP2_0_MASK);
        while ((read32(&mtk_infra_ao->infra_topaxi_protecten_sta1)
            & DIS_PROT_STEP2_0_ACK_MASK)
            != DIS_PROT_STEP2_0_ACK_MASK)
                ;
        setbits32(&mtk_spm->dis_pwr_con, DIS_SRAM_PDN);
        while ((read32(&mtk_spm->dis_pwr_con)
            & DIS_SRAM_PDN_ACK)
            != DIS_SRAM_PDN_ACK)
                ;
        setbits32(&mtk_spm->dis_pwr_con, PWR_ISO);
        setbits32(&mtk_spm->dis_pwr_con, PWR_CLK_DIS);
        clrbits32(&mtk_spm->dis_pwr_con, PWR_RST_B);
        clrbits32(&mtk_spm->dis_pwr_con, PWR_ON);
        clrbits32(&mtk_spm->dis_pwr_con, PWR_ON_2ND);
        while ((read32(&mtk_spm->pwr_status) & DIS_PWR_STA_MASK)
            || (read32(&mtk_spm->pwr_status_2nd) & DIS_PWR_STA_MASK))
                ;
    } else {
        setbits32(&mtk_spm->dis_pwr_con, PWR_ON);
        setbits32(&mtk_spm->dis_pwr_con, PWR_ON_2ND);
        while (((read32(&mtk_spm->pwr_status)
            & DIS_PWR_STA_MASK)
            != DIS_PWR_STA_MASK)
            || ((read32(&mtk_spm->pwr_status_2nd)
                & DIS_PWR_STA_MASK)
                != DIS_PWR_STA_MASK))
                    ;
        clrbits32(&mtk_spm->dis_pwr_con, PWR_CLK_DIS);
        clrbits32(&mtk_spm->dis_pwr_con, PWR_ISO);
        setbits32(&mtk_spm->dis_pwr_con, PWR_RST_B);
        clrbits32(&mtk_spm->dis_pwr_con, 0x1 << 8);
        while (read32(&mtk_spm->dis_pwr_con) & DIS_SRAM_PDN_ACK_BIT0)
            ;
        write32(&mtk_infra_ao->infra_topaxi_protecten_clr, DIS_PROT_STEP2_0_MASK);
        write32(&mtk_infra_ao->infra_topaxi_protecten_mm_clr, DIS_PROT_STEP1_0_MASK);
    }
    return err;
}

int spm_mtcmos_ctrl_audio(enum PWR_STA state)
{
    int err = 0;

    write32(POWERON_CONFIG_EN, (SPM_PROJECT_CODE << 16) | (0x1 << 0));

    if (state == STA_POWER_DOWN) {
        setbits32(&mtk_spm->aud_pwr_con, AUDIO_SRAM_PDN);
        while ((read32(&mtk_spm->aud_pwr_con)
            & AUDIO_SRAM_PDN_ACK)
            != AUDIO_SRAM_PDN_ACK)
                ;
        setbits32(&mtk_spm->aud_pwr_con, PWR_ISO);
        setbits32(&mtk_spm->aud_pwr_con, PWR_CLK_DIS);
        clrbits32(&mtk_spm->aud_pwr_con, PWR_RST_B);
        clrbits32(&mtk_spm->aud_pwr_con, PWR_ON);
        clrbits32(&mtk_spm->aud_pwr_con, PWR_ON_2ND);
        while ((read32(&mtk_spm->pwr_status) & AUDIO_PWR_STA_MASK)
            || (read32(&mtk_spm->pwr_status_2nd) & AUDIO_PWR_STA_MASK))
                ;
    } else {
        setbits32(&mtk_spm->aud_pwr_con, PWR_ON);
        setbits32(&mtk_spm->aud_pwr_con, PWR_ON_2ND);
        while (((read32(&mtk_spm->pwr_status)
            & AUDIO_PWR_STA_MASK)
            != AUDIO_PWR_STA_MASK)
            || ((read32(&mtk_spm->pwr_status_2nd)
                & AUDIO_PWR_STA_MASK)
                != AUDIO_PWR_STA_MASK))
                    ;
        clrbits32(&mtk_spm->aud_pwr_con, PWR_CLK_DIS);
        clrbits32(&mtk_spm->aud_pwr_con, PWR_ISO);
        setbits32(&mtk_spm->aud_pwr_con, PWR_RST_B);
        clrbits32(&mtk_spm->aud_pwr_con, 0x1 << 8);
        while (read32(&mtk_spm->aud_pwr_con) & AUDIO_SRAM_PDN_ACK_BIT0)
            ;
    }
    return err;
}


int spm_mtcmos_ctrl_netsys(enum PWR_STA state)
{
    int err = 0;

    /* TINFO="enable SPM register control" */
    write32(POWERON_CONFIG_EN, (SPM_PROJECT_CODE << 16) | (0x1 << 0));

    if (state == STA_POWER_DOWN) {
        write32(&mtk_infra_ao->infra_topaxi_protecten_2_set, NETSYS_PROT_STEP1_0_MASK);
        while ((read32(&mtk_infra_ao->infra_topaxi_protecten_sta1_2)
            & NETSYS_PROT_STEP1_0_ACK_MASK)
            != NETSYS_PROT_STEP1_0_ACK_MASK)
                ;
        write32(&mtk_infra_ao->infra_topaxi_protecten_set, NETSYS_PROT_STEP2_0_MASK);
        while ((read32(&mtk_infra_ao->infra_topaxi_protecten_sta1)
            & NETSYS_PROT_STEP2_0_ACK_MASK)
            != NETSYS_PROT_STEP2_0_ACK_MASK)
                ;
        write32(&mtk_infra_ao->infra_topaxi_protecten_set, NETSYS_PROT_STEP3_0_MASK);
        while ((read32(&mtk_infra_ao->infra_topaxi_protecten_sta1)
            & NETSYS_PROT_STEP3_0_ACK_MASK)
            != NETSYS_PROT_STEP3_0_ACK_MASK)
                ;
        write32(&mtk_infra_ao->infra_topaxi_protecten_2_set, NETSYS_PROT_STEP4_0_MASK);
        while ((read32(&mtk_infra_ao->infra_topaxi_protecten_sta1_2)
            & NETSYS_PROT_STEP4_0_ACK_MASK)
            != NETSYS_PROT_STEP4_0_ACK_MASK)
                ;
        setbits32(&mtk_spm->netsys_pwr_con, 0x1 << 8);
        setbits32(&mtk_spm->netsys_s_sram_con, NETSYS_SRAM_PDN);
        while ((read32(&mtk_spm->netsys_s_sram_con)
            & NETSYS_SRAM_PDN_ACK)
            != NETSYS_SRAM_PDN_ACK)
                ;
        setbits32(&mtk_spm->netsys_s_sram_con, NETSYS_SRAM_PDN2);
        while ((read32(&mtk_spm->netsys_s_sram_con)
            & NETSYS_SRAM_PDN2_ACK)
            != NETSYS_SRAM_PDN2_ACK)
                ;
        setbits32(&mtk_spm->netsys_pwr_con, PWR_ISO);
        setbits32(&mtk_spm->netsys_pwr_con, PWR_CLK_DIS);
        clrbits32(&mtk_spm->netsys_pwr_con, PWR_RST_B);
        clrbits32(&mtk_spm->netsys_pwr_con, PWR_ON);
        clrbits32(&mtk_spm->netsys_pwr_con, PWR_ON_2ND);
        while ((read32(&mtk_spm->pwr_status) & NETSYS_PWR_STA_MASK)
            || (read32(&mtk_spm->pwr_status_2nd) & NETSYS_PWR_STA_MASK))
                ;
    } else {
        setbits32(&mtk_spm->netsys_pwr_con, PWR_ON);
        setbits32(&mtk_spm->netsys_pwr_con, PWR_ON_2ND);
        while (((read32(&mtk_spm->pwr_status)
            & NETSYS_PWR_STA_MASK)
            != NETSYS_PWR_STA_MASK)
            || ((read32(&mtk_spm->pwr_status_2nd)
                & NETSYS_PWR_STA_MASK)
                != NETSYS_PWR_STA_MASK))
                    ;
        clrbits32(&mtk_spm->netsys_pwr_con, PWR_CLK_DIS);
        clrbits32(&mtk_spm->netsys_pwr_con, PWR_ISO);
        clrbits32(&mtk_spm->netsys_pwr_con, 0x1 << 8);
        setbits32(&mtk_spm->netsys_pwr_con, PWR_RST_B);
        clrbits32(&mtk_spm->netsys_s_sram_con, NETSYS_SRAM_PDN);
        while (read32(&mtk_spm->netsys_s_sram_con) & NETSYS_SRAM_PDN_ACK_BIT0)
            ;
        clrbits32(&mtk_spm->netsys_s_sram_con, NETSYS_SRAM_PDN2);
        while (read32(&mtk_spm->netsys_s_sram_con) & NETSYS_SRAM_PDN2_ACK_BIT0)
            ;
        write32(&mtk_infra_ao->infra_topaxi_protecten_2_clr, NETSYS_PROT_STEP4_0_MASK);
        write32(&mtk_infra_ao->infra_topaxi_protecten_clr, NETSYS_PROT_STEP3_0_MASK);
        write32(&mtk_infra_ao->infra_topaxi_protecten_clr, NETSYS_PROT_STEP2_0_MASK);
        write32(&mtk_infra_ao->infra_topaxi_protecten_2_clr, NETSYS_PROT_STEP1_0_MASK);
    }
    return err;
}
