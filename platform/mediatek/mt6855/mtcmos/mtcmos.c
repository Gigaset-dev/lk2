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
    uint32_t reserved0[784];
    uint32_t infrasys_protect_en_0;
    uint32_t infrasys_protect_en_0_set;
    uint32_t infrasys_protect_en_0_clr;
    uint32_t infrasys_protect_rdy_sta_0;
    uint32_t infrasys_protect_en_1;
    uint32_t infrasys_protect_en_1_set;
    uint32_t infrasys_protect_en_1_clr;
    uint32_t infrasys_protect_rdy_sta_1;
    uint32_t reserved1[16];
    uint32_t md_mfgsys_protect_en_0;
    uint32_t md_mfgsys_protect_en_0_set;
    uint32_t md_mfgsys_protect_en_0_clr;
    uint32_t md_mfgsys_protect_rdy_sta_0;
};

static struct mtk_infracfg_ao_regs *const mtk_infracfg_ao = (void *)INFRACFG_AO_BASE;

STATIC_ASSERT(__offsetof(struct mtk_infracfg_ao_regs, infrasys_protect_en_0)
        == 0x0C40);
STATIC_ASSERT(__offsetof(struct mtk_infracfg_ao_regs, infrasys_protect_en_0_set)
        == 0x0C44);
STATIC_ASSERT(__offsetof(struct mtk_infracfg_ao_regs, infrasys_protect_en_0_clr)
        == 0x0C48);
STATIC_ASSERT(__offsetof(struct mtk_infracfg_ao_regs, infrasys_protect_rdy_sta_0)
        == 0x0C4C);
STATIC_ASSERT(__offsetof(struct mtk_infracfg_ao_regs, infrasys_protect_en_1)
        == 0x0C50);
STATIC_ASSERT(__offsetof(struct mtk_infracfg_ao_regs, infrasys_protect_en_1_set)
        == 0x0C54);
STATIC_ASSERT(__offsetof(struct mtk_infracfg_ao_regs, infrasys_protect_en_1_clr)
        == 0x0C58);
STATIC_ASSERT(__offsetof(struct mtk_infracfg_ao_regs, infrasys_protect_rdy_sta_1)
        == 0x0C5C);
STATIC_ASSERT(__offsetof(struct mtk_infracfg_ao_regs, md_mfgsys_protect_en_0)
        == 0x0CA0);
STATIC_ASSERT(__offsetof(struct mtk_infracfg_ao_regs, md_mfgsys_protect_en_0_set)
        == 0x0CA4);
STATIC_ASSERT(__offsetof(struct mtk_infracfg_ao_regs, md_mfgsys_protect_en_0_clr)
        == 0x0CA8);
STATIC_ASSERT(__offsetof(struct mtk_infracfg_ao_regs, md_mfgsys_protect_rdy_sta_0)
        == 0x0CAC);

struct mtk_spm_regs {
    uint32_t reserved0[896];
    uint32_t md1_pwr_con;
    uint32_t reserved1[4];
    uint32_t audio_pwr_con;
    uint32_t reserved2[22];
    uint32_t mm_proc_pwr_con;
    uint32_t reserved3[17];
    uint32_t mfg0_pwr_con;
    uint32_t mfg1_pwr_con;
    uint32_t reserved4[27];
    uint32_t md_buck_iso_con;
    uint32_t reserved5[1];
    uint32_t pwr_status;
    uint32_t pwr_status_2nd;
    uint32_t xpu_pwr_status;
    uint32_t xpu_pwr_status_2nd;
};

static struct mtk_spm_regs *const mtk_spm = (void *)SPM_BASE;

STATIC_ASSERT(__offsetof(struct mtk_spm_regs, md1_pwr_con)
        == 0xE00);
STATIC_ASSERT(__offsetof(struct mtk_spm_regs, audio_pwr_con)
        == 0xE14);
STATIC_ASSERT(__offsetof(struct mtk_spm_regs, mm_proc_pwr_con)
        == 0xE70);
STATIC_ASSERT(__offsetof(struct mtk_spm_regs, mfg0_pwr_con)
        == 0xEB8);
STATIC_ASSERT(__offsetof(struct mtk_spm_regs, mfg1_pwr_con)
        == 0xEBC);
STATIC_ASSERT(__offsetof(struct mtk_spm_regs, md_buck_iso_con)
        == 0xF2C);
STATIC_ASSERT(__offsetof(struct mtk_spm_regs, pwr_status)
        == 0xF34);
STATIC_ASSERT(__offsetof(struct mtk_spm_regs, pwr_status_2nd)
        == 0xF38);
STATIC_ASSERT(__offsetof(struct mtk_spm_regs, xpu_pwr_status)
        == 0xF3C);
STATIC_ASSERT(__offsetof(struct mtk_spm_regs, xpu_pwr_status_2nd)
        == 0xF40);

struct mtk_vlpcfg_regs {
    uint32_t reserved0[132];
    uint32_t vlp_topaxi_protecten;
    uint32_t vlp_topaxi_protecten_set;
    uint32_t vlp_topaxi_protecten_clr;
    uint32_t reserved1[1];
    uint32_t vlp_topaxi_protecten_sta1;
};

static struct mtk_vlpcfg_regs *const mtk_vlpcfg = (void *)VLPCFG_BASE;

STATIC_ASSERT(__offsetof(struct mtk_vlpcfg_regs, vlp_topaxi_protecten)
        == 0x0210);
STATIC_ASSERT(__offsetof(struct mtk_vlpcfg_regs, vlp_topaxi_protecten_set)
        == 0x0214);
STATIC_ASSERT(__offsetof(struct mtk_vlpcfg_regs, vlp_topaxi_protecten_clr)
        == 0x0218);
STATIC_ASSERT(__offsetof(struct mtk_vlpcfg_regs, vlp_topaxi_protecten_sta1)
        == 0x0220);

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
    AUDIO_PWR_STA_MASK = 1 << 5,
    MD1_PWR_STA_MASK = 1 << 0,
    MFG0_PWR_STA_MASK = 1 << 1,
    MFG1_PWR_STA_MASK = 1 << 2,
    MM_PROC_PWR_STA_MASK = 1 << 28,
};

enum {
    AUDIO_SRAM_PDN = 1 << 8,
    AUDIO_SRAM_PDN_ACK = 1 << 12,
    MFG0_SRAM_PDN = 1 << 8,
    MFG0_SRAM_PDN_ACK = 1 << 12,
    MFG0_SRAM_SLP_B = 1 << 9,
    MFG0_SRAM_SLP_B_ACK = 1 << 13,
    MFG1_SRAM_PDN = 1 << 8,
    MFG1_SRAM_PDN_ACK = 1 << 12,
    MM_PROC_SRAM_PDN = 1 << 8,
    MM_PROC_SRAM_PDN_ACK = 1 << 12,
    MM_PROC_SRAM_SLP_B = 1 << 9,
    MM_PROC_SRAM_SLP_B_ACK = 1 << 13,
};

enum {
    MD1_PROT_STEP0_0_MASK = 1 << 9,
    MD1_PROT_STEP0_0_ACK_MASK = 1 << 9,
    MFG0_PROT_STEP0_0_MASK = 1 << 4,
    MFG0_PROT_STEP0_0_ACK_MASK = 1 << 4,
    MFG0_PROT_STEP1_0_MASK = 1 << 9,
    MFG0_PROT_STEP1_0_ACK_MASK = 1 << 9,
    MFG1_PROT_STEP0_0_MASK = 1 << 0,
    MFG1_PROT_STEP0_0_ACK_MASK = 1 << 0,
    MFG1_PROT_STEP1_0_MASK = 1 << 1,
    MFG1_PROT_STEP1_0_ACK_MASK = 1 << 1,
    MFG1_PROT_STEP2_0_MASK = 1 << 2,
    MFG1_PROT_STEP2_0_ACK_MASK = 1 << 2,
    MFG1_PROT_STEP3_0_MASK = 1 << 3,
    MFG1_PROT_STEP3_0_ACK_MASK = 1 << 3,
    MM_PROC_PROT_STEP0_0_MASK = ((1 << 7) | (1 << 8)),
    MM_PROC_PROT_STEP0_0_ACK_MASK = ((1 << 7) | (1 << 8)),
    MM_PROC_PROT_STEP1_0_MASK = ((1 << 9) | (1 << 10)),
    MM_PROC_PROT_STEP1_0_ACK_MASK = ((1 << 9) | (1 << 10)),
};

enum {
    DBG_ID_MD1 = 0,
    DBG_ID_AUDIO = 1,
    DBG_ID_MM_PROC_SHUTDOWN = 2,
    DBG_ID_MM_PROC_DORMANT = 3,
    DBG_ID_MFG0_SHUTDOWN = 4,
    DBG_ID_MFG0_DORMANT = 5,
    DBG_ID_MFG1 = 6,
    DBG_ID_NUM = 7,
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
        dprintf(CRITICAL, "infrasys_protect_en_0=%08x\n",
                read32(&mtk_infracfg_ao->infrasys_protect_en_0));
        dprintf(CRITICAL, "infrasys_protect_rdy_sta_0=%08x\n",
                read32(&mtk_infracfg_ao->infrasys_protect_rdy_sta_0));
        dprintf(CRITICAL, "infrasys_protect_en_1=%08x\n",
                read32(&mtk_infracfg_ao->infrasys_protect_en_1));
        dprintf(CRITICAL, "infrasys_protect_rdy_sta_1=%08x\n",
                read32(&mtk_infracfg_ao->infrasys_protect_rdy_sta_1));
        dprintf(CRITICAL, "md_mfgsys_protect_en_0=%08x\n",
                read32(&mtk_infracfg_ao->md_mfgsys_protect_en_0));
        dprintf(CRITICAL, "md_mfgsys_protect_rdy_sta_0=%08x\n",
                read32(&mtk_infracfg_ao->md_mfgsys_protect_rdy_sta_0));
        dprintf(CRITICAL, "md1_pwr_con=%08x\n",
                read32(&mtk_spm->md1_pwr_con));
        dprintf(CRITICAL, "audio_pwr_con=%08x\n",
                read32(&mtk_spm->audio_pwr_con));
        dprintf(CRITICAL, "mm_proc_pwr_con=%08x\n",
                read32(&mtk_spm->mm_proc_pwr_con));
        dprintf(CRITICAL, "mfg0_pwr_con=%08x\n",
                read32(&mtk_spm->mfg0_pwr_con));
        dprintf(CRITICAL, "mfg1_pwr_con=%08x\n",
                read32(&mtk_spm->mfg1_pwr_con));
        dprintf(CRITICAL, "md_buck_iso_con=%08x\n",
                read32(&mtk_spm->md_buck_iso_con));
        dprintf(CRITICAL, "pwr_status=%08x\n",
                read32(&mtk_spm->pwr_status));
        dprintf(CRITICAL, "pwr_status_2nd=%08x\n",
                read32(&mtk_spm->pwr_status_2nd));
        dprintf(CRITICAL, "xpu_pwr_status=%08x\n",
                read32(&mtk_spm->xpu_pwr_status));
        dprintf(CRITICAL, "xpu_pwr_status_2nd=%08x\n",
                read32(&mtk_spm->xpu_pwr_status_2nd));
        dprintf(CRITICAL, "vlp_topaxi_protecten=%08x\n",
                read32(&mtk_vlpcfg->vlp_topaxi_protecten));
        dprintf(CRITICAL, "vlp_topaxi_protecten_sta1=%08x\n",
                read32(&mtk_vlpcfg->vlp_topaxi_protecten_sta1));

        dprintf(CRITICAL, "%s %d MTCMOS hang at %s flow step %d\n",
            "[clkmgr]",
            DBG_ID,
            DBG_STA ? "pwron":"pdn",
            DBG_STEP);
    }

    if (!log_dump)
        loop_cnt++;
}

void spm_mtcmos_ctrl_md1_enable(void)
{
#ifndef PROJECT_TYPE_FPGA
    DBG_ID = DBG_ID_MD1;
    DBG_STA = POWER_ON;
    DBG_STEP = 0;

    clrbits32(&mtk_spm->md_buck_iso_con, 0x3);
    setbits32(&mtk_spm->md1_pwr_con, PWR_RST_B);
    setbits32(&mtk_spm->md1_pwr_con, PWR_ON);
    #ifndef IGNORE_MTCMOS_CHECK
    while ((read32(&mtk_spm->pwr_status) & MD1_PWR_STA_MASK) != MD1_PWR_STA_MASK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    write32(&mtk_infracfg_ao->infrasys_protect_en_1_clr, MD1_PROT_STEP0_0_MASK);
#endif
}

void spm_mtcmos_ctrl_md1_disable(void)
{
#ifndef PROJECT_TYPE_FPGA
    DBG_ID = DBG_ID_MD1;
    DBG_STA = POWER_DOWN;
    DBG_STEP = 0;

    write32(&mtk_infracfg_ao->infrasys_protect_en_1_set, MD1_PROT_STEP0_0_MASK);
    #ifndef IGNORE_MTCMOS_CHECK
    while ((read32(&mtk_infracfg_ao->infrasys_protect_rdy_sta_1)
            & MD1_PROT_STEP0_0_ACK_MASK)
            != MD1_PROT_STEP0_0_ACK_MASK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    clrbits32(&mtk_spm->md1_pwr_con, PWR_ON);
    #ifndef IGNORE_MTCMOS_CHECK
    while (read32(&mtk_spm->pwr_status) & MD1_PWR_STA_MASK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    setbits32(&mtk_spm->md_buck_iso_con, 0x3);
    clrbits32(&mtk_spm->md1_pwr_con, PWR_RST_B);
#endif
}

void spm_mtcmos_ctrl_audio_enable(void)
{
#ifndef PROJECT_TYPE_FPGA
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
#endif
}

void spm_mtcmos_ctrl_audio_disable(void)
{
#ifndef PROJECT_TYPE_FPGA
    DBG_ID = DBG_ID_AUDIO;
    DBG_STA = POWER_DOWN;
    DBG_STEP = 0;

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
#endif
}

void spm_mtcmos_ctrl_mm_proc_shutdown_enable(void)
{
#ifndef PROJECT_TYPE_FPGA
    DBG_ID = DBG_ID_MM_PROC_SHUTDOWN;
    DBG_STA = POWER_ON;
    DBG_STEP = 0;

    setbits32(&mtk_spm->mm_proc_pwr_con, PWR_ON);
    setbits32(&mtk_spm->mm_proc_pwr_con, PWR_ON_2ND);
    #ifndef IGNORE_MTCMOS_CHECK
    while ((read32(&mtk_spm->pwr_status) & MM_PROC_PWR_STA_MASK) != MM_PROC_PWR_STA_MASK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    while ((read32(&mtk_spm->pwr_status_2nd) & MM_PROC_PWR_STA_MASK) != MM_PROC_PWR_STA_MASK)
        ;
    clrbits32(&mtk_spm->mm_proc_pwr_con, PWR_CLK_DIS);
    clrbits32(&mtk_spm->mm_proc_pwr_con, PWR_ISO);
    setbits32(&mtk_spm->mm_proc_pwr_con, PWR_RST_B);
    clrbits32(&mtk_spm->mm_proc_pwr_con, MM_PROC_SRAM_PDN);
    #ifndef IGNORE_MTCMOS_CHECK
    while (read32(&mtk_spm->mm_proc_pwr_con) & MM_PROC_SRAM_PDN_ACK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    setbits32(&mtk_spm->mm_proc_pwr_con, SRAM_ISOINT_B);
    clrbits32(&mtk_spm->mm_proc_pwr_con, SRAM_CKISO);
    write32(&mtk_vlpcfg->vlp_topaxi_protecten_clr, MM_PROC_PROT_STEP1_0_MASK);
    write32(&mtk_vlpcfg->vlp_topaxi_protecten_clr, MM_PROC_PROT_STEP0_0_MASK);
#endif
}

void spm_mtcmos_ctrl_mm_proc_shutdown_disable(void)
{
#ifndef PROJECT_TYPE_FPGA
    DBG_ID = DBG_ID_MM_PROC_SHUTDOWN;
    DBG_STA = POWER_DOWN;
    DBG_STEP = 0;

    write32(&mtk_vlpcfg->vlp_topaxi_protecten_set, MM_PROC_PROT_STEP0_0_MASK);
    #ifndef IGNORE_MTCMOS_CHECK
    while ((read32(&mtk_vlpcfg->vlp_topaxi_protecten_sta1)
            & MM_PROC_PROT_STEP0_0_ACK_MASK)
            != MM_PROC_PROT_STEP0_0_ACK_MASK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    write32(&mtk_vlpcfg->vlp_topaxi_protecten_set, MM_PROC_PROT_STEP1_0_MASK);
    #ifndef IGNORE_MTCMOS_CHECK
    while ((read32(&mtk_vlpcfg->vlp_topaxi_protecten_sta1)
            & MM_PROC_PROT_STEP1_0_ACK_MASK)
            != MM_PROC_PROT_STEP1_0_ACK_MASK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    setbits32(&mtk_spm->mm_proc_pwr_con, SRAM_CKISO);
    clrbits32(&mtk_spm->mm_proc_pwr_con, SRAM_ISOINT_B);
    setbits32(&mtk_spm->mm_proc_pwr_con, MM_PROC_SRAM_PDN);
    #ifndef IGNORE_MTCMOS_CHECK
    while (read32(&mtk_spm->mm_proc_pwr_con) & MM_PROC_SRAM_PDN_ACK != MM_PROC_SRAM_PDN_ACK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    setbits32(&mtk_spm->mm_proc_pwr_con, PWR_ISO);
    setbits32(&mtk_spm->mm_proc_pwr_con, PWR_CLK_DIS);
    clrbits32(&mtk_spm->mm_proc_pwr_con, PWR_RST_B);
    clrbits32(&mtk_spm->mm_proc_pwr_con, PWR_ON);
    clrbits32(&mtk_spm->mm_proc_pwr_con, PWR_ON_2ND);
    #ifndef IGNORE_MTCMOS_CHECK
    while (read32(&mtk_spm->pwr_status) & MM_PROC_PWR_STA_MASK)
        ram_console_update();

    INCREASE_STEPS;
    while (read32(&mtk_spm->pwr_status_2nd) & MM_PROC_PWR_STA_MASK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
#endif
}

void spm_mtcmos_ctrl_mm_proc_dormant_enable(void)
{
#ifndef PROJECT_TYPE_FPGA
    DBG_ID = DBG_ID_MM_PROC_DORMANT;
    DBG_STA = POWER_ON;
    DBG_STEP = 0;

    setbits32(&mtk_spm->mm_proc_pwr_con, PWR_ON);
    setbits32(&mtk_spm->mm_proc_pwr_con, PWR_ON_2ND);
    #ifndef IGNORE_MTCMOS_CHECK
    while ((read32(&mtk_spm->pwr_status) & MM_PROC_PWR_STA_MASK) != MM_PROC_PWR_STA_MASK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    while ((read32(&mtk_spm->pwr_status_2nd) & MM_PROC_PWR_STA_MASK) != MM_PROC_PWR_STA_MASK)
        ;
    clrbits32(&mtk_spm->mm_proc_pwr_con, PWR_CLK_DIS);
    clrbits32(&mtk_spm->mm_proc_pwr_con, PWR_ISO);
    setbits32(&mtk_spm->mm_proc_pwr_con, PWR_RST_B);
    setbits32(&mtk_spm->mm_proc_pwr_con, MM_PROC_SRAM_SLP_B);
    #ifndef IGNORE_MTCMOS_CHECK
    while (read32(&mtk_spm->mm_proc_pwr_con) & MM_PROC_SRAM_SLP_B_ACK != MM_PROC_SRAM_SLP_B_ACK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    setbits32(&mtk_spm->mm_proc_pwr_con, SRAM_ISOINT_B);
    clrbits32(&mtk_spm->mm_proc_pwr_con, SRAM_CKISO);
    write32(&mtk_vlpcfg->vlp_topaxi_protecten_clr, MM_PROC_PROT_STEP1_0_MASK);
    write32(&mtk_vlpcfg->vlp_topaxi_protecten_clr, MM_PROC_PROT_STEP0_0_MASK);
#endif
}

void spm_mtcmos_ctrl_mm_proc_dormant_disable(void)
{
#ifndef PROJECT_TYPE_FPGA
    DBG_ID = DBG_ID_MM_PROC_DORMANT;
    DBG_STA = POWER_DOWN;
    DBG_STEP = 0;

    write32(&mtk_vlpcfg->vlp_topaxi_protecten_set, MM_PROC_PROT_STEP0_0_MASK);
    #ifndef IGNORE_MTCMOS_CHECK
    while ((read32(&mtk_vlpcfg->vlp_topaxi_protecten_sta1)
            & MM_PROC_PROT_STEP0_0_ACK_MASK)
            != MM_PROC_PROT_STEP0_0_ACK_MASK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    write32(&mtk_vlpcfg->vlp_topaxi_protecten_set, MM_PROC_PROT_STEP1_0_MASK);
    #ifndef IGNORE_MTCMOS_CHECK
    while ((read32(&mtk_vlpcfg->vlp_topaxi_protecten_sta1)
            & MM_PROC_PROT_STEP1_0_ACK_MASK)
            != MM_PROC_PROT_STEP1_0_ACK_MASK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    setbits32(&mtk_spm->mm_proc_pwr_con, SRAM_CKISO);
    clrbits32(&mtk_spm->mm_proc_pwr_con, SRAM_ISOINT_B);
    clrbits32(&mtk_spm->mm_proc_pwr_con, MM_PROC_SRAM_SLP_B);
    #ifndef IGNORE_MTCMOS_CHECK
    while (read32(&mtk_spm->mm_proc_pwr_con) & MM_PROC_SRAM_SLP_B_ACK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    setbits32(&mtk_spm->mm_proc_pwr_con, PWR_ISO);
    setbits32(&mtk_spm->mm_proc_pwr_con, PWR_CLK_DIS);
    clrbits32(&mtk_spm->mm_proc_pwr_con, PWR_RST_B);
    clrbits32(&mtk_spm->mm_proc_pwr_con, PWR_ON);
    clrbits32(&mtk_spm->mm_proc_pwr_con, PWR_ON_2ND);
    #ifndef IGNORE_MTCMOS_CHECK
    while (read32(&mtk_spm->pwr_status) & MM_PROC_PWR_STA_MASK)
        ram_console_update();

    INCREASE_STEPS;
    while (read32(&mtk_spm->pwr_status_2nd) & MM_PROC_PWR_STA_MASK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
#endif
}

void spm_mtcmos_ctrl_mfg0_shutdown_enable(void)
{
#ifndef PROJECT_TYPE_FPGA
    DBG_ID = DBG_ID_MFG0_SHUTDOWN;
    DBG_STA = POWER_ON;
    DBG_STEP = 0;

    setbits32(&mtk_spm->mfg0_pwr_con, PWR_ON);
    setbits32(&mtk_spm->mfg0_pwr_con, PWR_ON_2ND);
    #ifndef IGNORE_MTCMOS_CHECK
    while ((read32(&mtk_spm->xpu_pwr_status) & MFG0_PWR_STA_MASK) != MFG0_PWR_STA_MASK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    while ((read32(&mtk_spm->xpu_pwr_status_2nd) & MFG0_PWR_STA_MASK) != MFG0_PWR_STA_MASK)
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
    setbits32(&mtk_spm->mfg0_pwr_con, SRAM_ISOINT_B);
    clrbits32(&mtk_spm->mfg0_pwr_con, SRAM_CKISO);
    write32(&mtk_infracfg_ao->infrasys_protect_en_0_clr, MFG0_PROT_STEP1_0_MASK);
    write32(&mtk_infracfg_ao->md_mfgsys_protect_en_0_clr, MFG0_PROT_STEP0_0_MASK);
#endif
}

void spm_mtcmos_ctrl_mfg0_shutdown_disable(void)
{
#ifndef PROJECT_TYPE_FPGA
    DBG_ID = DBG_ID_MFG0_SHUTDOWN;
    DBG_STA = POWER_DOWN;
    DBG_STEP = 0;

    write32(&mtk_infracfg_ao->md_mfgsys_protect_en_0_set, MFG0_PROT_STEP0_0_MASK);
    #ifndef IGNORE_MTCMOS_CHECK
    while ((read32(&mtk_infracfg_ao->md_mfgsys_protect_rdy_sta_0)
            & MFG0_PROT_STEP0_0_ACK_MASK)
            != MFG0_PROT_STEP0_0_ACK_MASK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    write32(&mtk_infracfg_ao->infrasys_protect_en_0_set, MFG0_PROT_STEP1_0_MASK);
    #ifndef IGNORE_MTCMOS_CHECK
    while ((read32(&mtk_infracfg_ao->infrasys_protect_rdy_sta_0)
            & MFG0_PROT_STEP1_0_ACK_MASK)
            != MFG0_PROT_STEP1_0_ACK_MASK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    setbits32(&mtk_spm->mfg0_pwr_con, SRAM_CKISO);
    clrbits32(&mtk_spm->mfg0_pwr_con, SRAM_ISOINT_B);
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
    while (read32(&mtk_spm->xpu_pwr_status) & MFG0_PWR_STA_MASK)
        ram_console_update();

    INCREASE_STEPS;
    while (read32(&mtk_spm->xpu_pwr_status_2nd) & MFG0_PWR_STA_MASK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
#endif
}

void spm_mtcmos_ctrl_mfg0_dormant_enable(void)
{
#ifndef PROJECT_TYPE_FPGA
    DBG_ID = DBG_ID_MFG0_DORMANT;
    DBG_STA = POWER_ON;
    DBG_STEP = 0;

    setbits32(&mtk_spm->mfg0_pwr_con, PWR_ON);
    setbits32(&mtk_spm->mfg0_pwr_con, PWR_ON_2ND);
    #ifndef IGNORE_MTCMOS_CHECK
    while ((read32(&mtk_spm->xpu_pwr_status) & MFG0_PWR_STA_MASK) != MFG0_PWR_STA_MASK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    while ((read32(&mtk_spm->xpu_pwr_status_2nd) & MFG0_PWR_STA_MASK) != MFG0_PWR_STA_MASK)
        ;
    clrbits32(&mtk_spm->mfg0_pwr_con, PWR_CLK_DIS);
    clrbits32(&mtk_spm->mfg0_pwr_con, PWR_ISO);
    setbits32(&mtk_spm->mfg0_pwr_con, PWR_RST_B);
    setbits32(&mtk_spm->mfg0_pwr_con, MFG0_SRAM_SLP_B);
    #ifndef IGNORE_MTCMOS_CHECK
    while (read32(&mtk_spm->mfg0_pwr_con) & MFG0_SRAM_SLP_B_ACK != MFG0_SRAM_SLP_B_ACK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    setbits32(&mtk_spm->mfg0_pwr_con, SRAM_ISOINT_B);
    clrbits32(&mtk_spm->mfg0_pwr_con, SRAM_CKISO);
    write32(&mtk_infracfg_ao->infrasys_protect_en_0_clr, MFG0_PROT_STEP1_0_MASK);
    write32(&mtk_infracfg_ao->md_mfgsys_protect_en_0_clr, MFG0_PROT_STEP0_0_MASK);
#endif
}

void spm_mtcmos_ctrl_mfg0_dormant_disable(void)
{
#ifndef PROJECT_TYPE_FPGA
    DBG_ID = DBG_ID_MFG0_DORMANT;
    DBG_STA = POWER_DOWN;
    DBG_STEP = 0;

    write32(&mtk_infracfg_ao->md_mfgsys_protect_en_0_set, MFG0_PROT_STEP0_0_MASK);
    #ifndef IGNORE_MTCMOS_CHECK
    while ((read32(&mtk_infracfg_ao->md_mfgsys_protect_rdy_sta_0)
            & MFG0_PROT_STEP0_0_ACK_MASK)
            != MFG0_PROT_STEP0_0_ACK_MASK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    write32(&mtk_infracfg_ao->infrasys_protect_en_0_set, MFG0_PROT_STEP1_0_MASK);
    #ifndef IGNORE_MTCMOS_CHECK
    while ((read32(&mtk_infracfg_ao->infrasys_protect_rdy_sta_0)
            & MFG0_PROT_STEP1_0_ACK_MASK)
            != MFG0_PROT_STEP1_0_ACK_MASK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    setbits32(&mtk_spm->mfg0_pwr_con, SRAM_CKISO);
    clrbits32(&mtk_spm->mfg0_pwr_con, SRAM_ISOINT_B);
    clrbits32(&mtk_spm->mfg0_pwr_con, MFG0_SRAM_SLP_B);
    #ifndef IGNORE_MTCMOS_CHECK
    while (read32(&mtk_spm->mfg0_pwr_con) & MFG0_SRAM_SLP_B_ACK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    setbits32(&mtk_spm->mfg0_pwr_con, PWR_ISO);
    setbits32(&mtk_spm->mfg0_pwr_con, PWR_CLK_DIS);
    clrbits32(&mtk_spm->mfg0_pwr_con, PWR_RST_B);
    clrbits32(&mtk_spm->mfg0_pwr_con, PWR_ON);
    clrbits32(&mtk_spm->mfg0_pwr_con, PWR_ON_2ND);
    #ifndef IGNORE_MTCMOS_CHECK
    while (read32(&mtk_spm->xpu_pwr_status) & MFG0_PWR_STA_MASK)
        ram_console_update();

    INCREASE_STEPS;
    while (read32(&mtk_spm->xpu_pwr_status_2nd) & MFG0_PWR_STA_MASK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
#endif
}

void spm_mtcmos_ctrl_mfg1_enable(void)
{
#ifndef PROJECT_TYPE_FPGA
    DBG_ID = DBG_ID_MFG1;
    DBG_STA = POWER_ON;
    DBG_STEP = 0;

    setbits32(&mtk_spm->mfg1_pwr_con, PWR_ON);
    setbits32(&mtk_spm->mfg1_pwr_con, PWR_ON_2ND);
    #ifndef IGNORE_MTCMOS_CHECK
    while ((read32(&mtk_spm->xpu_pwr_status) & MFG1_PWR_STA_MASK) != MFG1_PWR_STA_MASK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    while ((read32(&mtk_spm->xpu_pwr_status_2nd) & MFG1_PWR_STA_MASK) != MFG1_PWR_STA_MASK)
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
    write32(&mtk_infracfg_ao->md_mfgsys_protect_en_0_clr, MFG1_PROT_STEP3_0_MASK);
    write32(&mtk_infracfg_ao->md_mfgsys_protect_en_0_clr, MFG1_PROT_STEP2_0_MASK);
    write32(&mtk_infracfg_ao->md_mfgsys_protect_en_0_clr, MFG1_PROT_STEP1_0_MASK);
    write32(&mtk_infracfg_ao->md_mfgsys_protect_en_0_clr, MFG1_PROT_STEP0_0_MASK);
#endif
}

void spm_mtcmos_ctrl_mfg1_disable(void)
{
#ifndef PROJECT_TYPE_FPGA
    DBG_ID = DBG_ID_MFG1;
    DBG_STA = POWER_DOWN;
    DBG_STEP = 0;

    write32(&mtk_infracfg_ao->md_mfgsys_protect_en_0_set, MFG1_PROT_STEP0_0_MASK);
    #ifndef IGNORE_MTCMOS_CHECK
    while ((read32(&mtk_infracfg_ao->md_mfgsys_protect_rdy_sta_0)
            & MFG1_PROT_STEP0_0_ACK_MASK)
            != MFG1_PROT_STEP0_0_ACK_MASK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    write32(&mtk_infracfg_ao->md_mfgsys_protect_en_0_set, MFG1_PROT_STEP1_0_MASK);
    #ifndef IGNORE_MTCMOS_CHECK
    while ((read32(&mtk_infracfg_ao->md_mfgsys_protect_rdy_sta_0)
            & MFG1_PROT_STEP1_0_ACK_MASK)
            != MFG1_PROT_STEP1_0_ACK_MASK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    write32(&mtk_infracfg_ao->md_mfgsys_protect_en_0_set, MFG1_PROT_STEP2_0_MASK);
    #ifndef IGNORE_MTCMOS_CHECK
    while ((read32(&mtk_infracfg_ao->md_mfgsys_protect_rdy_sta_0)
            & MFG1_PROT_STEP2_0_ACK_MASK)
            != MFG1_PROT_STEP2_0_ACK_MASK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    write32(&mtk_infracfg_ao->md_mfgsys_protect_en_0_set, MFG1_PROT_STEP3_0_MASK);
    #ifndef IGNORE_MTCMOS_CHECK
    while ((read32(&mtk_infracfg_ao->md_mfgsys_protect_rdy_sta_0)
            & MFG1_PROT_STEP3_0_ACK_MASK)
            != MFG1_PROT_STEP3_0_ACK_MASK)
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
    while (read32(&mtk_spm->xpu_pwr_status) & MFG1_PWR_STA_MASK)
        ram_console_update();

    INCREASE_STEPS;
    while (read32(&mtk_spm->xpu_pwr_status_2nd) & MFG1_PWR_STA_MASK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
#endif
}
