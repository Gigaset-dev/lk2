/*
 * Copyright (c) 2022 MediaTek Inc.
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
    uint32_t reserved5[8];
    uint32_t infra_topaxi_protecten_mm;
    uint32_t infra_topaxi_protecten_mm_set;
    uint32_t infra_topaxi_protecten_mm_clr;
    uint32_t reserved6[4];
    uint32_t infra_topaxi_protecten_mm_sta1;
    uint32_t reserved7[264];
    uint32_t infra_topaxi_protecten_2;
    uint32_t infra_topaxi_protecten_2_set;
    uint32_t infra_topaxi_protecten_2_clr;
    uint32_t reserved8[2];
    uint32_t infra_topaxi_protecten_sta1_2;
    uint32_t reserved9[278];
    uint32_t infra_topaxi_protecten_infra_vdnr;
    uint32_t infra_topaxi_protecten_infra_vdnr_set;
    uint32_t infra_topaxi_protecten_infra_vdnr_clr;
    uint32_t reserved10[1];
    uint32_t infra_topaxi_protecten_infra_vdnr_sta1;
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
STATIC_ASSERT(__offsetof(struct mtk_infracfg_ao_regs, infra_topaxi_protecten_mm)
        == 0x02D0);
STATIC_ASSERT(__offsetof(struct mtk_infracfg_ao_regs, infra_topaxi_protecten_mm_set)
        == 0x02D4);
STATIC_ASSERT(__offsetof(struct mtk_infracfg_ao_regs, infra_topaxi_protecten_mm_clr)
        == 0x02D8);
STATIC_ASSERT(__offsetof(struct mtk_infracfg_ao_regs, infra_topaxi_protecten_mm_sta1)
        == 0x02EC);
STATIC_ASSERT(__offsetof(struct mtk_infracfg_ao_regs, infra_topaxi_protecten_2)
        == 0x0710);
STATIC_ASSERT(__offsetof(struct mtk_infracfg_ao_regs, infra_topaxi_protecten_2_set)
        == 0x0714);
STATIC_ASSERT(__offsetof(struct mtk_infracfg_ao_regs, infra_topaxi_protecten_2_clr)
        == 0x0718);
STATIC_ASSERT(__offsetof(struct mtk_infracfg_ao_regs, infra_topaxi_protecten_sta1_2)
        == 0x0724);
STATIC_ASSERT(__offsetof(struct mtk_infracfg_ao_regs, infra_topaxi_protecten_infra_vdnr)
        == 0x0B80);
STATIC_ASSERT(__offsetof(struct mtk_infracfg_ao_regs, infra_topaxi_protecten_infra_vdnr_set)
        == 0x0B84);
STATIC_ASSERT(__offsetof(struct mtk_infracfg_ao_regs, infra_topaxi_protecten_infra_vdnr_clr)
        == 0x0B88);
STATIC_ASSERT(__offsetof(struct mtk_infracfg_ao_regs, infra_topaxi_protecten_infra_vdnr_sta1)
        == 0x0B90);

struct mtk_spm_regs {
    uint32_t reserved0[91];
    uint32_t pwr_status;
    uint32_t pwr_status_2nd;
    uint32_t reserved1[99];
    uint32_t md1_pwr_con;
    uint32_t reserved2[1];
    uint32_t mfg0_pwr_con;
    uint32_t mfg1_pwr_con;
    uint32_t mfg2_pwr_con;
    uint32_t mfg3_pwr_con;
    uint32_t reserved3[15];
    uint32_t dis_pwr_con;
    uint32_t reserved4[16];
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
STATIC_ASSERT(__offsetof(struct mtk_spm_regs, mfg2_pwr_con)
        == 0x310);
STATIC_ASSERT(__offsetof(struct mtk_spm_regs, mfg3_pwr_con)
        == 0x314);
STATIC_ASSERT(__offsetof(struct mtk_spm_regs, dis_pwr_con)
        == 0x354);
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
    DIS_PWR_STA_MASK = 1U << 21,
    MD1_PWR_STA_MASK = 1U << 0,
    MFG0_PWR_STA_MASK = 1U << 2,
    MFG1_PWR_STA_MASK = 1U << 3,
    MFG2_PWR_STA_MASK = 1U << 4,
    MFG3_PWR_STA_MASK = 1U << 5,
};

enum {
    DIS_SRAM_PDN = 1U << 8,
    DIS_SRAM_PDN_ACK = 1U << 12,
    MFG0_SRAM_PDN = 1U << 8,
    MFG0_SRAM_PDN_ACK = 1U << 12,
    MFG1_SRAM_PDN = 1U << 8,
    MFG1_SRAM_PDN_ACK = 1U << 12,
    MFG2_SRAM_PDN = 1U << 8,
    MFG2_SRAM_PDN_ACK = 1U << 12,
    MFG3_SRAM_PDN = 1U << 8,
    MFG3_SRAM_PDN_ACK = 1U << 12,
};

enum {
    DIS_PROT_STEP0_0_MASK = ((1U << 10) | (1U << 12)),
    DIS_PROT_STEP0_0_ACK_MASK = ((1U << 10) | (1U << 12)),
    DIS_PROT_STEP1_0_MASK = ((1U << 6) | (1U << 23)),
    DIS_PROT_STEP1_0_ACK_MASK = ((1U << 6) | (1U << 23)),
    MD1_PROT_STEP0_0_MASK = 1U << 7,
    MD1_PROT_STEP0_0_ACK_MASK = 1U << 7,
    MD1_PROT_STEP1_0_MASK = ((1U << 0) | (1U << 10) | (1U << 15)),
    MD1_PROT_STEP1_0_ACK_MASK = ((1U << 0) | (1U << 10) | (1U << 15)),
    MFG1_PROT_STEP0_0_MASK = 1U << 21,
    MFG1_PROT_STEP0_0_ACK_MASK = 1U << 21,
    MFG1_PROT_STEP1_0_MASK = ((1U << 5) | (1U << 6)),
    MFG1_PROT_STEP1_0_ACK_MASK = ((1U << 5) | (1U << 6)),
    MFG1_PROT_STEP2_0_MASK = ((1U << 21) | (1U << 22)),
    MFG1_PROT_STEP2_0_ACK_MASK = ((1U << 21) | (1U << 22)),
    MFG1_PROT_STEP3_0_MASK = 1U << 7,
    MFG1_PROT_STEP3_0_ACK_MASK = 1U << 7,
};

enum {
    DBG_ID_MD1 = 0,
    DBG_ID_MFG0 = 1,
    DBG_ID_MFG1 = 2,
    DBG_ID_MFG2 = 3,
    DBG_ID_MFG3 = 4,
    DBG_ID_DIS = 5,
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
        dprintf(CRITICAL, "infra_topaxi_protecten_mm=%08x\n",
                read32(&mtk_infracfg_ao->infra_topaxi_protecten_mm));
        dprintf(CRITICAL, "infra_topaxi_protecten_mm_sta1=%08x\n",
                read32(&mtk_infracfg_ao->infra_topaxi_protecten_mm_sta1));
        dprintf(CRITICAL, "infra_topaxi_protecten_2=%08x\n",
                read32(&mtk_infracfg_ao->infra_topaxi_protecten_2));
        dprintf(CRITICAL, "infra_topaxi_protecten_sta1_2=%08x\n",
                read32(&mtk_infracfg_ao->infra_topaxi_protecten_sta1_2));
        dprintf(CRITICAL, "infra_topaxi_protecten_infra_vdnr=%08x\n",
                read32(&mtk_infracfg_ao->infra_topaxi_protecten_infra_vdnr));
        dprintf(CRITICAL, "infra_topaxi_protecten_infra_vdnr_sta1=%08x\n",
                read32(&mtk_infracfg_ao->infra_topaxi_protecten_infra_vdnr_sta1));
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
        dprintf(CRITICAL, "mfg2_pwr_con=%08x\n",
                read32(&mtk_spm->mfg2_pwr_con));
        dprintf(CRITICAL, "mfg3_pwr_con=%08x\n",
                read32(&mtk_spm->mfg3_pwr_con));
        dprintf(CRITICAL, "dis_pwr_con=%08x\n",
                read32(&mtk_spm->dis_pwr_con));
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

void spm_mtcmos_ctrl_md1_enable(void)
{
#ifndef PROJECT_TYPE_FPGA
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
    write32(&mtk_infracfg_ao->infra_topaxi_protecten_infra_vdnr_clr, MD1_PROT_STEP1_0_MASK);
    write32(&mtk_infracfg_ao->infra_topaxi_protecten_clr, MD1_PROT_STEP0_0_MASK);
#endif
}

void spm_mtcmos_ctrl_md1_disable(void)
{
#ifndef PROJECT_TYPE_FPGA
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
    clrbits32(&mtk_spm->md1_pwr_con, PWR_ON);
    #ifndef IGNORE_MTCMOS_CHECK
    while (read32(&mtk_spm->pwr_status) & MD1_PWR_STA_MASK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    setbits32(&mtk_spm->md_ext_buck_iso_con, 0x3);
    clrbits32(&mtk_spm->md1_pwr_con, PWR_RST_B);
#endif
}

void spm_mtcmos_ctrl_mfg0_enable(void)
{
#ifndef PROJECT_TYPE_FPGA
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
#endif
}

void spm_mtcmos_ctrl_mfg0_disable(void)
{
#ifndef PROJECT_TYPE_FPGA
    DBG_ID = DBG_ID_MFG0;
    DBG_STA = POWER_DOWN;
    DBG_STEP = 0;

    setbits32(&mtk_spm->mfg0_pwr_con, MFG0_SRAM_PDN);
    #ifndef IGNORE_MTCMOS_CHECK
    while ((read32(&mtk_spm->mfg0_pwr_con) & MFG0_SRAM_PDN_ACK) != MFG0_SRAM_PDN_ACK)
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
    write32(&mtk_infracfg_ao->infra_topaxi_protecten_2_clr, MFG1_PROT_STEP3_0_MASK);
    write32(&mtk_infracfg_ao->infra_topaxi_protecten_clr, MFG1_PROT_STEP2_0_MASK);
    write32(&mtk_infracfg_ao->infra_topaxi_protecten_2_clr, MFG1_PROT_STEP1_0_MASK);
    write32(&mtk_infracfg_ao->infra_topaxi_protecten_1_clr, MFG1_PROT_STEP0_0_MASK);
#endif
}

void spm_mtcmos_ctrl_mfg1_disable(void)
{
#ifndef PROJECT_TYPE_FPGA
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
    setbits32(&mtk_spm->mfg1_pwr_con, MFG1_SRAM_PDN);
    #ifndef IGNORE_MTCMOS_CHECK
    while ((read32(&mtk_spm->mfg1_pwr_con) & MFG1_SRAM_PDN_ACK) != MFG1_SRAM_PDN_ACK)
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
#endif
}

void spm_mtcmos_ctrl_mfg2_enable(void)
{
#ifndef PROJECT_TYPE_FPGA
    DBG_ID = DBG_ID_MFG2;
    DBG_STA = POWER_ON;
    DBG_STEP = 0;

    setbits32(&mtk_spm->mfg2_pwr_con, PWR_ON);
    setbits32(&mtk_spm->mfg2_pwr_con, PWR_ON_2ND);
    #ifndef IGNORE_MTCMOS_CHECK
    while ((read32(&mtk_spm->pwr_status) & MFG2_PWR_STA_MASK) != MFG2_PWR_STA_MASK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    while ((read32(&mtk_spm->pwr_status_2nd) & MFG2_PWR_STA_MASK) != MFG2_PWR_STA_MASK)
        ;
    clrbits32(&mtk_spm->mfg2_pwr_con, PWR_CLK_DIS);
    clrbits32(&mtk_spm->mfg2_pwr_con, PWR_ISO);
    setbits32(&mtk_spm->mfg2_pwr_con, PWR_RST_B);
    clrbits32(&mtk_spm->mfg2_pwr_con, MFG2_SRAM_PDN);
    #ifndef IGNORE_MTCMOS_CHECK
    while (read32(&mtk_spm->mfg2_pwr_con) & MFG2_SRAM_PDN_ACK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
#endif
}

void spm_mtcmos_ctrl_mfg2_disable(void)
{
#ifndef PROJECT_TYPE_FPGA
    DBG_ID = DBG_ID_MFG2;
    DBG_STA = POWER_DOWN;
    DBG_STEP = 0;

    setbits32(&mtk_spm->mfg2_pwr_con, MFG2_SRAM_PDN);
    #ifndef IGNORE_MTCMOS_CHECK
    while ((read32(&mtk_spm->mfg2_pwr_con) & MFG2_SRAM_PDN_ACK) != MFG2_SRAM_PDN_ACK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    setbits32(&mtk_spm->mfg2_pwr_con, PWR_ISO);
    setbits32(&mtk_spm->mfg2_pwr_con, PWR_CLK_DIS);
    clrbits32(&mtk_spm->mfg2_pwr_con, PWR_RST_B);
    clrbits32(&mtk_spm->mfg2_pwr_con, PWR_ON);
    clrbits32(&mtk_spm->mfg2_pwr_con, PWR_ON_2ND);
    #ifndef IGNORE_MTCMOS_CHECK
    while (read32(&mtk_spm->pwr_status) & MFG2_PWR_STA_MASK)
        ram_console_update();

    INCREASE_STEPS;
    while (read32(&mtk_spm->pwr_status_2nd) & MFG2_PWR_STA_MASK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
#endif
}

void spm_mtcmos_ctrl_mfg3_enable(void)
{
#ifndef PROJECT_TYPE_FPGA
    DBG_ID = DBG_ID_MFG3;
    DBG_STA = POWER_ON;
    DBG_STEP = 0;

    setbits32(&mtk_spm->mfg3_pwr_con, PWR_ON);
    setbits32(&mtk_spm->mfg3_pwr_con, PWR_ON_2ND);
    #ifndef IGNORE_MTCMOS_CHECK
    while ((read32(&mtk_spm->pwr_status) & MFG3_PWR_STA_MASK) != MFG3_PWR_STA_MASK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    while ((read32(&mtk_spm->pwr_status_2nd) & MFG3_PWR_STA_MASK) != MFG3_PWR_STA_MASK)
        ;
    clrbits32(&mtk_spm->mfg3_pwr_con, PWR_CLK_DIS);
    clrbits32(&mtk_spm->mfg3_pwr_con, PWR_ISO);
    setbits32(&mtk_spm->mfg3_pwr_con, PWR_RST_B);
    clrbits32(&mtk_spm->mfg3_pwr_con, MFG3_SRAM_PDN);
    #ifndef IGNORE_MTCMOS_CHECK
    while (read32(&mtk_spm->mfg3_pwr_con) & MFG3_SRAM_PDN_ACK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
#endif
}

void spm_mtcmos_ctrl_mfg3_disable(void)
{
#ifndef PROJECT_TYPE_FPGA
    DBG_ID = DBG_ID_MFG3;
    DBG_STA = POWER_DOWN;
    DBG_STEP = 0;

    setbits32(&mtk_spm->mfg3_pwr_con, MFG3_SRAM_PDN);
    #ifndef IGNORE_MTCMOS_CHECK
    while ((read32(&mtk_spm->mfg3_pwr_con) & MFG3_SRAM_PDN_ACK) != MFG3_SRAM_PDN_ACK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    setbits32(&mtk_spm->mfg3_pwr_con, PWR_ISO);
    setbits32(&mtk_spm->mfg3_pwr_con, PWR_CLK_DIS);
    clrbits32(&mtk_spm->mfg3_pwr_con, PWR_RST_B);
    clrbits32(&mtk_spm->mfg3_pwr_con, PWR_ON);
    clrbits32(&mtk_spm->mfg3_pwr_con, PWR_ON_2ND);
    #ifndef IGNORE_MTCMOS_CHECK
    while (read32(&mtk_spm->pwr_status) & MFG3_PWR_STA_MASK)
        ram_console_update();

    INCREASE_STEPS;
    while (read32(&mtk_spm->pwr_status_2nd) & MFG3_PWR_STA_MASK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
#endif
}

void spm_mtcmos_ctrl_dis_enable(void)
{
#ifndef PROJECT_TYPE_FPGA
    DBG_ID = DBG_ID_DIS;
    DBG_STA = POWER_ON;
    DBG_STEP = 0;

    setbits32(&mtk_spm->dis_pwr_con, PWR_ON);
    setbits32(&mtk_spm->dis_pwr_con, PWR_ON_2ND);
    #ifndef IGNORE_MTCMOS_CHECK
    while ((read32(&mtk_spm->pwr_status) & DIS_PWR_STA_MASK) != DIS_PWR_STA_MASK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    while ((read32(&mtk_spm->pwr_status_2nd) & DIS_PWR_STA_MASK) != DIS_PWR_STA_MASK)
        ;
    clrbits32(&mtk_spm->dis_pwr_con, PWR_CLK_DIS);
    clrbits32(&mtk_spm->dis_pwr_con, PWR_ISO);
    setbits32(&mtk_spm->dis_pwr_con, PWR_RST_B);
    clrbits32(&mtk_spm->dis_pwr_con, DIS_SRAM_PDN);
    #ifndef IGNORE_MTCMOS_CHECK
    while (read32(&mtk_spm->dis_pwr_con) & DIS_SRAM_PDN_ACK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    write32(&mtk_infracfg_ao->infra_topaxi_protecten_clr, DIS_PROT_STEP1_0_MASK);
    write32(&mtk_infracfg_ao->infra_topaxi_protecten_mm_clr, DIS_PROT_STEP0_0_MASK);
#endif
}

void spm_mtcmos_ctrl_dis_disable(void)
{
#ifndef PROJECT_TYPE_FPGA
    DBG_ID = DBG_ID_DIS;
    DBG_STA = POWER_DOWN;
    DBG_STEP = 0;

    write32(&mtk_infracfg_ao->infra_topaxi_protecten_mm_set, DIS_PROT_STEP0_0_MASK);
    #ifndef IGNORE_MTCMOS_CHECK
    while ((read32(&mtk_infracfg_ao->infra_topaxi_protecten_mm_sta1)
            & DIS_PROT_STEP0_0_ACK_MASK)
            != DIS_PROT_STEP0_0_ACK_MASK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    write32(&mtk_infracfg_ao->infra_topaxi_protecten_set, DIS_PROT_STEP1_0_MASK);
    #ifndef IGNORE_MTCMOS_CHECK
    while ((read32(&mtk_infracfg_ao->infra_topaxi_protecten_sta1)
            & DIS_PROT_STEP1_0_ACK_MASK)
            != DIS_PROT_STEP1_0_ACK_MASK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    setbits32(&mtk_spm->dis_pwr_con, DIS_SRAM_PDN);
    #ifndef IGNORE_MTCMOS_CHECK
    while ((read32(&mtk_spm->dis_pwr_con) & DIS_SRAM_PDN_ACK) != DIS_SRAM_PDN_ACK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
    setbits32(&mtk_spm->dis_pwr_con, PWR_ISO);
    setbits32(&mtk_spm->dis_pwr_con, PWR_CLK_DIS);
    clrbits32(&mtk_spm->dis_pwr_con, PWR_RST_B);
    clrbits32(&mtk_spm->dis_pwr_con, PWR_ON);
    clrbits32(&mtk_spm->dis_pwr_con, PWR_ON_2ND);
    #ifndef IGNORE_MTCMOS_CHECK
    while (read32(&mtk_spm->pwr_status) & DIS_PWR_STA_MASK)
        ram_console_update();

    INCREASE_STEPS;
    while (read32(&mtk_spm->pwr_status_2nd) & DIS_PWR_STA_MASK)
        ram_console_update();

    INCREASE_STEPS;
    #endif
#endif
}
