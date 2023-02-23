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

#define POWER_ON                 1
#define POWER_DOWN              0

struct mtk_infracfg_ao_regs {
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
    uint32_t reserved5[280];
    uint32_t infra_topaxi_protecten_2;
    uint32_t infra_topaxi_protecten_2_set;
    uint32_t infra_topaxi_protecten_2_clr;
    uint32_t reserved6[2];
    uint32_t infra_topaxi_protecten_sta1_2;
    uint32_t reserved7[278];
    uint32_t infra_topaxi_protecten_infra_vdnr;
    uint32_t infra_topaxi_protecten_infra_vdnr_set;
    uint32_t infra_topaxi_protecten_infra_vdnr_clr;
    uint32_t reserved8[1];
    uint32_t infra_topaxi_protecten_infra_vdnr_sta1;
    uint32_t reserved9[8];
    uint32_t infra_topaxi_protecten_sub_infra_vdnr;
    uint32_t infra_topaxi_protecten_sub_infra_vdnr_set;
    uint32_t infra_topaxi_protecten_sub_infra_vdnr_clr;
    uint32_t reserved10[1];
    uint32_t infra_topaxi_protecten_sub_infra_vdnr_sta1;
};

static struct mtk_infracfg_ao_regs *const mtk_infracfg_ao = (void *)INFRACFG_AO_BASE;

STATIC_ASSERT(__offsetof(struct mtk_infracfg_ao_regs, infra_topaxi_protecten)
        == 0x0220);
STATIC_ASSERT(__offsetof(struct mtk_infracfg_ao_regs, infra_topaxi_protecten_sta1)
        == 0x0228);
STATIC_ASSERT(__offsetof(struct mtk_infracfg_ao_regs, infra_topaxi_protecten_1)
        == 0x0250);
STATIC_ASSERT(__offsetof(struct mtk_infracfg_ao_regs, infra_topaxi_protecten_sta1_1)
        == 0x0258);
STATIC_ASSERT(__offsetof(struct mtk_infracfg_ao_regs, infra_topaxi_protecten_set)
        == 0x02A0);
STATIC_ASSERT(__offsetof(struct mtk_infracfg_ao_regs, infra_topaxi_protecten_clr)
        == 0x02A4);
STATIC_ASSERT(__offsetof(struct mtk_infracfg_ao_regs, infra_topaxi_protecten_1_set)
        == 0x02A8);
STATIC_ASSERT(__offsetof(struct mtk_infracfg_ao_regs, infra_topaxi_protecten_1_clr)
        == 0x02AC);
STATIC_ASSERT(__offsetof(struct mtk_infracfg_ao_regs, infra_topaxi_protecten_2)
        == 0x0710);
STATIC_ASSERT(__offsetof(struct mtk_infracfg_ao_regs, infra_topaxi_protecten_2_set)
        == 0x0714);
STATIC_ASSERT(__offsetof(struct mtk_infracfg_ao_regs, infra_topaxi_protecten_2_clr)
        == 0x0718);
STATIC_ASSERT(__offsetof(struct mtk_infracfg_ao_regs, infra_topaxi_protecten_sta1_2)
        == 0x0724);
STATIC_ASSERT(__offsetof(struct mtk_infracfg_ao_regs, infra_topaxi_protecten_infra_vdnr)
        == 0xB80);
STATIC_ASSERT(__offsetof(struct mtk_infracfg_ao_regs, infra_topaxi_protecten_infra_vdnr_set)
        == 0xB84);
STATIC_ASSERT(__offsetof(struct mtk_infracfg_ao_regs, infra_topaxi_protecten_infra_vdnr_clr)
        == 0xB88);
STATIC_ASSERT(__offsetof(struct mtk_infracfg_ao_regs, infra_topaxi_protecten_infra_vdnr_sta1)
        == 0xB90);
STATIC_ASSERT(__offsetof(struct mtk_infracfg_ao_regs, infra_topaxi_protecten_sub_infra_vdnr)
        == 0xBB4);
STATIC_ASSERT(__offsetof(struct mtk_infracfg_ao_regs, infra_topaxi_protecten_sub_infra_vdnr_set)
        == 0xBB8);
STATIC_ASSERT(__offsetof(struct mtk_infracfg_ao_regs, infra_topaxi_protecten_sub_infra_vdnr_clr)
        == 0xBBC);
STATIC_ASSERT(__offsetof(struct mtk_infracfg_ao_regs, infra_topaxi_protecten_sub_infra_vdnr_sta1)
        == 0xBC4);

struct mtk_spm_regs {
    uint32_t reserved0[91];
    uint32_t pwr_status;
    uint32_t pwr_status_2nd;
    uint32_t reserved1[99];
    uint32_t md1_pwr_con;
    uint32_t reserved2[1];
    uint32_t mfg0_pwr_con;
    uint32_t mfg1_pwr_con;
    uint32_t reserved3[17];
    uint32_t audio_pwr_con;
    uint32_t adsp_pwr_con;
    uint32_t reserved4[15];
    uint32_t md_ext_buck_iso_con;
};

static struct mtk_spm_regs *const mtk_spm = (void *)SPM_BASE;

STATIC_ASSERT(__offsetof(struct mtk_spm_regs, pwr_status)
        == 0x16C);
STATIC_ASSERT(__offsetof(struct mtk_spm_regs, pwr_status_2nd)
        == 0x170);
STATIC_ASSERT(__offsetof(struct mtk_spm_regs, md1_pwr_con)
        == 0x300);
STATIC_ASSERT(__offsetof(struct mtk_spm_regs, mfg0_pwr_con)
        == 0x308);
STATIC_ASSERT(__offsetof(struct mtk_spm_regs, mfg1_pwr_con)
        == 0x30C);
STATIC_ASSERT(__offsetof(struct mtk_spm_regs, audio_pwr_con)
        == 0x354);
STATIC_ASSERT(__offsetof(struct mtk_spm_regs, adsp_pwr_con)
        == 0x358);
STATIC_ASSERT(__offsetof(struct mtk_spm_regs, md_ext_buck_iso_con)
        == 0x398);

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
    ADSP_PWR_STA_MASK = 1 << 22,
    AUDIO_PWR_STA_MASK = 1 << 21,
    MD1_PWR_STA_MASK = 1 << 0,
    MFG0_PWR_STA_MASK = 1 << 2,
    MFG1_PWR_STA_MASK = 1 << 3,
};

enum {
    ADSP_SRAM_PDN = 1 << 8,
    ADSP_SRAM_PDN_ACK = 1 << 12,
    ADSP_SRAM_SLP_B = 1 << 9,
    ADSP_SRAM_SLP_B_ACK = 1 << 13,
    AUDIO_SRAM_PDN = 1 << 8,
    AUDIO_SRAM_PDN_ACK = 1 << 12,
    MFG0_SRAM_PDN = 1 << 8,
    MFG0_SRAM_PDN_ACK = 1 << 12,
    MFG1_SRAM_PDN = 1 << 8,
    MFG1_SRAM_PDN_ACK = 1 << 12,
};

enum {
    ADSP_PROT_STEP0_0_MASK = 1 << 3,
    ADSP_PROT_STEP0_0_ACK_MASK = 1 << 3,
    AUDIO_PROT_STEP0_0_MASK = 1 << 4,
    AUDIO_PROT_STEP0_0_ACK_MASK = 1 << 4,
    MD1_PROT_STEP0_0_MASK = 1 << 7,
    MD1_PROT_STEP0_0_ACK_MASK = 1 << 7,
    MD1_PROT_STEP1_0_MASK = ((1 << 10) | (1 << 21) | (1 << 29)),
    MD1_PROT_STEP1_0_ACK_MASK = ((1 << 10) | (1 << 21) | (1 << 29)),
    MD1_PROT_STEP2_0_MASK = ((1 << 6) | (1 << 7)),
    MD1_PROT_STEP2_0_ACK_MASK = ((1 << 6) | (1 << 7)),
    MFG1_PROT_STEP0_0_MASK = ((1 << 19) | (1 << 20) | (1 << 21)),
    MFG1_PROT_STEP0_0_ACK_MASK = ((1 << 19) | (1 << 20) | (1 << 21)),
    MFG1_PROT_STEP1_0_MASK = ((1 << 5) | (1 << 6)),
    MFG1_PROT_STEP1_0_ACK_MASK = ((1 << 5) | (1 << 6)),
    MFG1_PROT_STEP2_0_MASK = ((1 << 21) | (1 << 22)),
    MFG1_PROT_STEP2_0_ACK_MASK = ((1 << 21) | (1 << 22)),
    MFG1_PROT_STEP3_0_MASK = 1 << 7,
    MFG1_PROT_STEP3_0_ACK_MASK = 1 << 7,
    MFG1_PROT_STEP4_0_MASK = ((1 << 17) | (1 << 19)),
    MFG1_PROT_STEP4_0_ACK_MASK = ((1 << 17) | (1 << 19)),
};

enum {
    DBG_ID_MFG0 = 0,
    DBG_ID_MFG1 = 1,
    DBG_ID_AUDIO = 2,
    DBG_ID_ADSP_SHUTDOWN = 3,
    DBG_ID_ADSP_DORMANT = 4,
    DBG_ID_MD1 = 5,
    DBG_ID_NUM = 6,
};

#define ID_MADK   0xFF000000
#define STA_MASK  0x00F00000
#define STEP_MASK 0x000000FF

#define INCREASE_STEPS \
    do { \
        DBG_STEP++; \
        loop_cnt = 0; \
    } while (0)

static int DBG_ID;
static int DBG_STA;
static int DBG_STEP;
static unsigned int loop_cnt;

static void ram_console_update(void)
{
    static bool log_dump;

    if (loop_cnt > 100000 && !log_dump) {
        log_dump = true;
        dprintf(CRITICAL, "infra_topaxi_protecten=%08x\n",
                read32(&mtk_infracfg_ao->infra_topaxi_protecten));
        dprintf(CRITICAL, "infra_topaxi_protecten_sta1=%08x\n",
                read32(&mtk_infracfg_ao->infra_topaxi_protecten_sta1));
        dprintf(CRITICAL, "infra_topaxi_protecten_1=%08x\n",
                read32(&mtk_infracfg_ao->infra_topaxi_protecten_1));
        dprintf(CRITICAL, "infra_topaxi_protecten_sta1_1=%08x\n",
                read32(&mtk_infracfg_ao->infra_topaxi_protecten_sta1_1));
        dprintf(CRITICAL, "infra_topaxi_protecten_2=%08x\n",
                read32(&mtk_infracfg_ao->infra_topaxi_protecten_2));
        dprintf(CRITICAL, "infra_topaxi_protecten_sta1_2=%08x\n",
                read32(&mtk_infracfg_ao->infra_topaxi_protecten_sta1_2));
        dprintf(CRITICAL, "infra_topaxi_protecten_infra_vdnr=%08x\n",
                read32(&mtk_infracfg_ao->infra_topaxi_protecten_infra_vdnr));
        dprintf(CRITICAL, "infra_topaxi_protecten_infra_vdnr_sta1=%08x\n",
                read32(&mtk_infracfg_ao->infra_topaxi_protecten_infra_vdnr_sta1));
        dprintf(CRITICAL, "infra_topaxi_protecten_sub_infra_vdnr=%08x\n",
                read32(&mtk_infracfg_ao->infra_topaxi_protecten_sub_infra_vdnr));
        dprintf(CRITICAL, "infra_topaxi_protecten_sub_infra_vdnr_sta1=%08x\n",
                read32(&mtk_infracfg_ao->infra_topaxi_protecten_sub_infra_vdnr_sta1));
        dprintf(CRITICAL, "pwr_status=%08x\n",
                read32(&mtk_spm->pwr_status));
        dprintf(CRITICAL, "pwr_status_2nd=%08x\n",
                read32(&mtk_spm->pwr_status_2nd));
        dprintf(CRITICAL, "md1_pwr_con=%08x\n",
                read32(&mtk_spm->md1_pwr_con));
        dprintf(CRITICAL, "mfg0_pwr_con=%08x\n",
                read32(&mtk_spm->mfg0_pwr_con));
        dprintf(CRITICAL, "mfg1_pwr_con=%08x\n",
                read32(&mtk_spm->mfg1_pwr_con));
        dprintf(CRITICAL, "audio_pwr_con=%08x\n",
                read32(&mtk_spm->audio_pwr_con));
        dprintf(CRITICAL, "adsp_pwr_con=%08x\n",
                read32(&mtk_spm->adsp_pwr_con));
        dprintf(CRITICAL, "md_ext_buck_iso_con=%08x\n",
                read32(&mtk_spm->md_ext_buck_iso_con));

        dprintf(CRITICAL, "%s %d MTCMOS hang at %s flow step %d\n",
            "[clkmgr]",
            DBG_ID,
            DBG_STA ? "pwron":"pdn",
            DBG_STEP);
    }

    if (!log_dump)
        loop_cnt++;
}

void spm_mtcmos_ctrl_mfg0_enable(void)
{
    DBG_ID = DBG_ID_MFG0;
    DBG_STA = POWER_ON;
    DBG_STEP = 0;

    setbits32(&mtk_spm->mfg0_pwr_con, PWR_ON);
    setbits32(&mtk_spm->mfg0_pwr_con, PWR_ON_2ND);
    #ifndef IGNORE_MTCMOS_CHECK
    while ((read32(&mtk_spm->pwr_status) & MFG0_PWR_STA_MASK) != MFG0_PWR_STA_MASK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    while ((read32(&mtk_spm->pwr_status_2nd) & MFG0_PWR_STA_MASK) != MFG0_PWR_STA_MASK)
        ;
    clrbits32(&mtk_spm->mfg0_pwr_con, PWR_CLK_DIS);
    clrbits32(&mtk_spm->mfg0_pwr_con, PWR_ISO);
    setbits32(&mtk_spm->mfg0_pwr_con, PWR_RST_B);
    clrbits32(&mtk_spm->mfg0_pwr_con, MFG0_SRAM_PDN);
    #ifndef IGNORE_MTCMOS_CHECK
    while (read32(&mtk_spm->mfg0_pwr_con) & MFG0_SRAM_PDN_ACK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
}

void spm_mtcmos_ctrl_mfg0_disable(void)
{
    DBG_ID = DBG_ID_MFG0;
    DBG_STA = POWER_DOWN;
    DBG_STEP = 0;

    setbits32(&mtk_spm->mfg0_pwr_con, MFG0_SRAM_PDN);
    #ifndef IGNORE_MTCMOS_CHECK
    while (read32(&mtk_spm->mfg0_pwr_con) & MFG0_SRAM_PDN_ACK != MFG0_SRAM_PDN_ACK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    setbits32(&mtk_spm->mfg0_pwr_con, PWR_ISO);
    setbits32(&mtk_spm->mfg0_pwr_con, PWR_CLK_DIS);
    clrbits32(&mtk_spm->mfg0_pwr_con, PWR_RST_B);
    clrbits32(&mtk_spm->mfg0_pwr_con, PWR_ON);
    clrbits32(&mtk_spm->mfg0_pwr_con, PWR_ON_2ND);
    #ifndef IGNORE_MTCMOS_CHECK
    while (read32(&mtk_spm->pwr_status) & MFG0_PWR_STA_MASK)
        ram_console_update();

    INCREASE_STEPS;
    while (read32(&mtk_spm->pwr_status_2nd) & MFG0_PWR_STA_MASK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
}

void spm_mtcmos_ctrl_mfg1_enable(void)
{
    DBG_ID = DBG_ID_MFG1;
    DBG_STA = POWER_ON;
    DBG_STEP = 0;

    setbits32(&mtk_spm->mfg1_pwr_con, PWR_ON);
    setbits32(&mtk_spm->mfg1_pwr_con, PWR_ON_2ND);
    #ifndef IGNORE_MTCMOS_CHECK
    while ((read32(&mtk_spm->pwr_status) & MFG1_PWR_STA_MASK) != MFG1_PWR_STA_MASK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    while ((read32(&mtk_spm->pwr_status_2nd) & MFG1_PWR_STA_MASK) != MFG1_PWR_STA_MASK)
        ;
    clrbits32(&mtk_spm->mfg1_pwr_con, PWR_CLK_DIS);
    clrbits32(&mtk_spm->mfg1_pwr_con, PWR_ISO);
    setbits32(&mtk_spm->mfg1_pwr_con, PWR_RST_B);
    clrbits32(&mtk_spm->mfg1_pwr_con, MFG1_SRAM_PDN);
    #ifndef IGNORE_MTCMOS_CHECK
    while (read32(&mtk_spm->mfg1_pwr_con) & MFG1_SRAM_PDN_ACK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    write32(&mtk_infracfg_ao->infra_topaxi_protecten_sub_infra_vdnr_clr, MFG1_PROT_STEP4_0_MASK);
    write32(&mtk_infracfg_ao->infra_topaxi_protecten_2_clr, MFG1_PROT_STEP3_0_MASK);
    write32(&mtk_infracfg_ao->infra_topaxi_protecten_clr, MFG1_PROT_STEP2_0_MASK);
    write32(&mtk_infracfg_ao->infra_topaxi_protecten_2_clr, MFG1_PROT_STEP1_0_MASK);
    write32(&mtk_infracfg_ao->infra_topaxi_protecten_1_clr, MFG1_PROT_STEP0_0_MASK);
}

void spm_mtcmos_ctrl_mfg1_disable(void)
{
    DBG_ID = DBG_ID_MFG1;
    DBG_STA = POWER_DOWN;
    DBG_STEP = 0;

    write32(&mtk_infracfg_ao->infra_topaxi_protecten_1_set, MFG1_PROT_STEP0_0_MASK);
    #ifndef IGNORE_MTCMOS_CHECK
    while ((read32(&mtk_infracfg_ao->infra_topaxi_protecten_sta1_1)
            & MFG1_PROT_STEP0_0_ACK_MASK)
            != MFG1_PROT_STEP0_0_ACK_MASK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    write32(&mtk_infracfg_ao->infra_topaxi_protecten_2_set, MFG1_PROT_STEP1_0_MASK);
    #ifndef IGNORE_MTCMOS_CHECK
    while ((read32(&mtk_infracfg_ao->infra_topaxi_protecten_sta1_2)
            & MFG1_PROT_STEP1_0_ACK_MASK)
            != MFG1_PROT_STEP1_0_ACK_MASK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    write32(&mtk_infracfg_ao->infra_topaxi_protecten_set, MFG1_PROT_STEP2_0_MASK);
    #ifndef IGNORE_MTCMOS_CHECK
    while ((read32(&mtk_infracfg_ao->infra_topaxi_protecten_sta1)
            & MFG1_PROT_STEP2_0_ACK_MASK)
            != MFG1_PROT_STEP2_0_ACK_MASK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    write32(&mtk_infracfg_ao->infra_topaxi_protecten_2_set, MFG1_PROT_STEP3_0_MASK);
    #ifndef IGNORE_MTCMOS_CHECK
    while ((read32(&mtk_infracfg_ao->infra_topaxi_protecten_sta1_2)
            & MFG1_PROT_STEP3_0_ACK_MASK)
            != MFG1_PROT_STEP3_0_ACK_MASK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    write32(&mtk_infracfg_ao->infra_topaxi_protecten_sub_infra_vdnr_set, MFG1_PROT_STEP4_0_MASK);
    #ifndef IGNORE_MTCMOS_CHECK
    while ((read32(&mtk_infracfg_ao->infra_topaxi_protecten_sub_infra_vdnr_sta1)
            & MFG1_PROT_STEP4_0_ACK_MASK)
            != MFG1_PROT_STEP4_0_ACK_MASK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    setbits32(&mtk_spm->mfg1_pwr_con, MFG1_SRAM_PDN);
    #ifndef IGNORE_MTCMOS_CHECK
    while (read32(&mtk_spm->mfg1_pwr_con) & MFG1_SRAM_PDN_ACK != MFG1_SRAM_PDN_ACK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    setbits32(&mtk_spm->mfg1_pwr_con, PWR_ISO);
    setbits32(&mtk_spm->mfg1_pwr_con, PWR_CLK_DIS);
    clrbits32(&mtk_spm->mfg1_pwr_con, PWR_RST_B);
    clrbits32(&mtk_spm->mfg1_pwr_con, PWR_ON);
    clrbits32(&mtk_spm->mfg1_pwr_con, PWR_ON_2ND);
    #ifndef IGNORE_MTCMOS_CHECK
    while (read32(&mtk_spm->pwr_status) & MFG1_PWR_STA_MASK)
        ram_console_update();

    INCREASE_STEPS;
    while (read32(&mtk_spm->pwr_status_2nd) & MFG1_PWR_STA_MASK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
}

void spm_mtcmos_ctrl_audio_enable(void)
{
    DBG_ID = DBG_ID_AUDIO;
    DBG_STA = POWER_ON;
    DBG_STEP = 0;

    setbits32(&mtk_spm->audio_pwr_con, PWR_ON);
    setbits32(&mtk_spm->audio_pwr_con, PWR_ON_2ND);
    #ifndef IGNORE_MTCMOS_CHECK
    while ((read32(&mtk_spm->pwr_status) & AUDIO_PWR_STA_MASK) != AUDIO_PWR_STA_MASK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    while ((read32(&mtk_spm->pwr_status_2nd) & AUDIO_PWR_STA_MASK) != AUDIO_PWR_STA_MASK)
        ;
    clrbits32(&mtk_spm->audio_pwr_con, PWR_CLK_DIS);
    clrbits32(&mtk_spm->audio_pwr_con, PWR_ISO);
    setbits32(&mtk_spm->audio_pwr_con, PWR_RST_B);
    clrbits32(&mtk_spm->audio_pwr_con, AUDIO_SRAM_PDN);
    #ifndef IGNORE_MTCMOS_CHECK
    while (read32(&mtk_spm->audio_pwr_con) & AUDIO_SRAM_PDN_ACK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    write32(&mtk_infracfg_ao->infra_topaxi_protecten_2_clr, AUDIO_PROT_STEP0_0_MASK);
}

void spm_mtcmos_ctrl_audio_disable(void)
{
    DBG_ID = DBG_ID_AUDIO;
    DBG_STA = POWER_DOWN;
    DBG_STEP = 0;

    write32(&mtk_infracfg_ao->infra_topaxi_protecten_2_set, AUDIO_PROT_STEP0_0_MASK);
    #ifndef IGNORE_MTCMOS_CHECK
    while ((read32(&mtk_infracfg_ao->infra_topaxi_protecten_sta1_2)
            & AUDIO_PROT_STEP0_0_ACK_MASK)
            != AUDIO_PROT_STEP0_0_ACK_MASK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    setbits32(&mtk_spm->audio_pwr_con, AUDIO_SRAM_PDN);
    #ifndef IGNORE_MTCMOS_CHECK
    while (read32(&mtk_spm->audio_pwr_con) & AUDIO_SRAM_PDN_ACK != AUDIO_SRAM_PDN_ACK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    setbits32(&mtk_spm->audio_pwr_con, PWR_ISO);
    setbits32(&mtk_spm->audio_pwr_con, PWR_CLK_DIS);
    clrbits32(&mtk_spm->audio_pwr_con, PWR_RST_B);
    clrbits32(&mtk_spm->audio_pwr_con, PWR_ON);
    clrbits32(&mtk_spm->audio_pwr_con, PWR_ON_2ND);
    #ifndef IGNORE_MTCMOS_CHECK
    while (read32(&mtk_spm->pwr_status) & AUDIO_PWR_STA_MASK)
        ram_console_update();

    INCREASE_STEPS;
    while (read32(&mtk_spm->pwr_status_2nd) & AUDIO_PWR_STA_MASK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
}

void spm_mtcmos_ctrl_adsp_shutdown_enable(void)
{
    DBG_ID = DBG_ID_ADSP_SHUTDOWN;
    DBG_STA = POWER_ON;
    DBG_STEP = 0;

    setbits32(&mtk_spm->adsp_pwr_con, PWR_ON);
    setbits32(&mtk_spm->adsp_pwr_con, PWR_ON_2ND);
    #ifndef IGNORE_MTCMOS_CHECK
    while ((read32(&mtk_spm->pwr_status) & ADSP_PWR_STA_MASK) != ADSP_PWR_STA_MASK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    while ((read32(&mtk_spm->pwr_status_2nd) & ADSP_PWR_STA_MASK) != ADSP_PWR_STA_MASK)
        ;
    clrbits32(&mtk_spm->adsp_pwr_con, PWR_CLK_DIS);
    clrbits32(&mtk_spm->adsp_pwr_con, PWR_ISO);
    setbits32(&mtk_spm->adsp_pwr_con, PWR_RST_B);
    clrbits32(&mtk_spm->adsp_pwr_con, ADSP_SRAM_PDN);
    #ifndef IGNORE_MTCMOS_CHECK
    while (read32(&mtk_spm->adsp_pwr_con) & ADSP_SRAM_PDN_ACK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    setbits32(&mtk_spm->adsp_pwr_con, SRAM_ISOINT_B);
    clrbits32(&mtk_spm->adsp_pwr_con, SRAM_CKISO);
    write32(&mtk_infracfg_ao->infra_topaxi_protecten_2_clr, ADSP_PROT_STEP0_0_MASK);
}

void spm_mtcmos_ctrl_adsp_shutdown_disable(void)
{
    DBG_ID = DBG_ID_ADSP_SHUTDOWN;
    DBG_STA = POWER_DOWN;
    DBG_STEP = 0;

    write32(&mtk_infracfg_ao->infra_topaxi_protecten_2_set, ADSP_PROT_STEP0_0_MASK);
    #ifndef IGNORE_MTCMOS_CHECK
    while ((read32(&mtk_infracfg_ao->infra_topaxi_protecten_sta1_2)
            & ADSP_PROT_STEP0_0_ACK_MASK)
            != ADSP_PROT_STEP0_0_ACK_MASK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    setbits32(&mtk_spm->adsp_pwr_con, SRAM_CKISO);
    clrbits32(&mtk_spm->adsp_pwr_con, SRAM_ISOINT_B);
    setbits32(&mtk_spm->adsp_pwr_con, ADSP_SRAM_PDN);
    #ifndef IGNORE_MTCMOS_CHECK
    while (read32(&mtk_spm->adsp_pwr_con) & ADSP_SRAM_PDN_ACK != ADSP_SRAM_PDN_ACK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    setbits32(&mtk_spm->adsp_pwr_con, PWR_ISO);
    setbits32(&mtk_spm->adsp_pwr_con, PWR_CLK_DIS);
    clrbits32(&mtk_spm->adsp_pwr_con, PWR_RST_B);
    clrbits32(&mtk_spm->adsp_pwr_con, PWR_ON);
    clrbits32(&mtk_spm->adsp_pwr_con, PWR_ON_2ND);
    #ifndef IGNORE_MTCMOS_CHECK
    while (read32(&mtk_spm->pwr_status) & ADSP_PWR_STA_MASK)
        ram_console_update();

    INCREASE_STEPS;
    while (read32(&mtk_spm->pwr_status_2nd) & ADSP_PWR_STA_MASK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
}

void spm_mtcmos_ctrl_adsp_dormant_enable(void)
{
    DBG_ID = DBG_ID_ADSP_DORMANT;
    DBG_STA = POWER_ON;
    DBG_STEP = 0;

    setbits32(&mtk_spm->adsp_pwr_con, PWR_ON);
    setbits32(&mtk_spm->adsp_pwr_con, PWR_ON_2ND);
    #ifndef IGNORE_MTCMOS_CHECK
    while ((read32(&mtk_spm->pwr_status) & ADSP_PWR_STA_MASK) != ADSP_PWR_STA_MASK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    while ((read32(&mtk_spm->pwr_status_2nd) & ADSP_PWR_STA_MASK) != ADSP_PWR_STA_MASK)
        ;
    clrbits32(&mtk_spm->adsp_pwr_con, PWR_CLK_DIS);
    clrbits32(&mtk_spm->adsp_pwr_con, PWR_ISO);
    setbits32(&mtk_spm->adsp_pwr_con, PWR_RST_B);
    setbits32(&mtk_spm->adsp_pwr_con, ADSP_SRAM_SLP_B);
    #ifndef IGNORE_MTCMOS_CHECK
    while (read32(&mtk_spm->adsp_pwr_con) & ADSP_SRAM_SLP_B_ACK != ADSP_SRAM_SLP_B_ACK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    setbits32(&mtk_spm->adsp_pwr_con, SRAM_ISOINT_B);
    clrbits32(&mtk_spm->adsp_pwr_con, SRAM_CKISO);
    write32(&mtk_infracfg_ao->infra_topaxi_protecten_2_clr, ADSP_PROT_STEP0_0_MASK);
}

void spm_mtcmos_ctrl_adsp_dormant_disable(void)
{
    DBG_ID = DBG_ID_ADSP_DORMANT;
    DBG_STA = POWER_DOWN;
    DBG_STEP = 0;

    write32(&mtk_infracfg_ao->infra_topaxi_protecten_2_set, ADSP_PROT_STEP0_0_MASK);
    #ifndef IGNORE_MTCMOS_CHECK
    while ((read32(&mtk_infracfg_ao->infra_topaxi_protecten_sta1_2)
            & ADSP_PROT_STEP0_0_ACK_MASK)
            != ADSP_PROT_STEP0_0_ACK_MASK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    setbits32(&mtk_spm->adsp_pwr_con, SRAM_CKISO);
    clrbits32(&mtk_spm->adsp_pwr_con, SRAM_ISOINT_B);
    clrbits32(&mtk_spm->adsp_pwr_con, ADSP_SRAM_SLP_B);
    #ifndef IGNORE_MTCMOS_CHECK
    while (read32(&mtk_spm->adsp_pwr_con) & ADSP_SRAM_SLP_B_ACK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    setbits32(&mtk_spm->adsp_pwr_con, PWR_ISO);
    setbits32(&mtk_spm->adsp_pwr_con, PWR_CLK_DIS);
    clrbits32(&mtk_spm->adsp_pwr_con, PWR_RST_B);
    clrbits32(&mtk_spm->adsp_pwr_con, PWR_ON);
    clrbits32(&mtk_spm->adsp_pwr_con, PWR_ON_2ND);
    #ifndef IGNORE_MTCMOS_CHECK
    while (read32(&mtk_spm->pwr_status) & ADSP_PWR_STA_MASK)
        ram_console_update();

    INCREASE_STEPS;
    while (read32(&mtk_spm->pwr_status_2nd) & ADSP_PWR_STA_MASK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
}

void spm_mtcmos_ctrl_md1_enable(void)
{
    DBG_ID = DBG_ID_MD1;
    DBG_STA = POWER_ON;
    DBG_STEP = 0;

    clrbits32(&mtk_spm->md_ext_buck_iso_con, 0x3);
    setbits32(&mtk_spm->md1_pwr_con, PWR_RST_B);
    setbits32(&mtk_spm->md1_pwr_con, PWR_ON);
    #ifndef IGNORE_MTCMOS_CHECK
    while ((read32(&mtk_spm->pwr_status) & MD1_PWR_STA_MASK) != MD1_PWR_STA_MASK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    write32(&mtk_infracfg_ao->infra_topaxi_protecten_sub_infra_vdnr_clr, MD1_PROT_STEP2_0_MASK);
    write32(&mtk_infracfg_ao->infra_topaxi_protecten_infra_vdnr_clr, MD1_PROT_STEP1_0_MASK);
    write32(&mtk_infracfg_ao->infra_topaxi_protecten_clr, MD1_PROT_STEP0_0_MASK);
}

void spm_mtcmos_ctrl_md1_disable(void)
{
    DBG_ID = DBG_ID_MD1;
    DBG_STA = POWER_DOWN;
    DBG_STEP = 0;

    write32(&mtk_infracfg_ao->infra_topaxi_protecten_set, MD1_PROT_STEP0_0_MASK);
    #ifndef IGNORE_MTCMOS_CHECK
    while ((read32(&mtk_infracfg_ao->infra_topaxi_protecten_sta1)
            & MD1_PROT_STEP0_0_ACK_MASK)
            != MD1_PROT_STEP0_0_ACK_MASK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    write32(&mtk_infracfg_ao->infra_topaxi_protecten_infra_vdnr_set, MD1_PROT_STEP1_0_MASK);
    #ifndef IGNORE_MTCMOS_CHECK
    while ((read32(&mtk_infracfg_ao->infra_topaxi_protecten_infra_vdnr_sta1)
            & MD1_PROT_STEP1_0_ACK_MASK)
            != MD1_PROT_STEP1_0_ACK_MASK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    write32(&mtk_infracfg_ao->infra_topaxi_protecten_sub_infra_vdnr_set, MD1_PROT_STEP2_0_MASK);
    #ifndef IGNORE_MTCMOS_CHECK
    while ((read32(&mtk_infracfg_ao->infra_topaxi_protecten_sub_infra_vdnr_sta1)
            & MD1_PROT_STEP2_0_ACK_MASK)
            != MD1_PROT_STEP2_0_ACK_MASK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    clrbits32(&mtk_spm->md1_pwr_con, PWR_ON);
    #ifndef IGNORE_MTCMOS_CHECK
    while (read32(&mtk_spm->pwr_status) & MD1_PWR_STA_MASK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    setbits32(&mtk_spm->md_ext_buck_iso_con, 0x3);
    clrbits32(&mtk_spm->md1_pwr_con, PWR_RST_B);
}