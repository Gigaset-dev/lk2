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
#include <assert.h>
#include <platform/addressmap.h>
#include <platform/reg.h>
#include <platform/mtcmos.h>

struct mtk_spm_regs {
    uint32_t reserved0[88];
    uint32_t pwr_status;
    uint32_t pwr_status_2nd;
    uint32_t reserved1[108];
    uint32_t md1_pwr_con;
    uint32_t reserved2[37];
    uint32_t md_ext_buck_iso_con;
};

static struct mtk_spm_regs *const mtk_spm = (void *)SPM_BASE;

STATIC_ASSERT(__offsetof(struct mtk_spm_regs, pwr_status) == 0x160);
STATIC_ASSERT(__offsetof(struct mtk_spm_regs, pwr_status_2nd) == 0x164);
STATIC_ASSERT(__offsetof(struct mtk_spm_regs, md1_pwr_con) == 0x318);
STATIC_ASSERT(__offsetof(struct mtk_spm_regs, md_ext_buck_iso_con) == 0x3B0);

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
    MD1_PROT_STEP2_0_MASK = 0x3 << 3,
    MD1_PROT_STEP2_1_MASK = 0x1 << 6,
};

enum {
    MD1_PROT_STEP1_0_ACK_MASK = 0x1 << 7,
    MD1_PROT_STEP2_0_ACK_MASK = 0x3 << 3,
    MD1_PROT_STEP2_1_ACK_MASK = 0x1 << 6,
};

enum pwr_sta {
    MD1_PWR_STA_MASK = 0x1 << 0,
};

void spm_mtcmos_ctrl_md1(enum PWR_STA state)
{
    if (state == STA_POWER_DOWN) {
        write32(&mtk_infra_ao->infra_topaxi_protecten_set, MD1_PROT_STEP1_0_MASK);
        while ((read32(&mtk_infra_ao->infra_topaxi_protecten_sta1)
            & MD1_PROT_STEP1_0_ACK_MASK)
            != MD1_PROT_STEP1_0_ACK_MASK)
                ;
        write32(&mtk_infra_ao->infra_topaxi_protecten_set, MD1_PROT_STEP2_0_MASK);
        while ((read32(&mtk_infra_ao->infra_topaxi_protecten_sta1)
            & MD1_PROT_STEP2_0_ACK_MASK)
            != MD1_PROT_STEP2_0_ACK_MASK)
                ;
        write32(&mtk_infra_ao->infra_topaxi_protecten_1_set, MD1_PROT_STEP2_1_MASK);
        while ((read32(&mtk_infra_ao->infra_topaxi_protecten_sta1_1)
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
        write32(&mtk_infra_ao->infra_topaxi_protecten_clr, MD1_PROT_STEP2_0_MASK);
        write32(&mtk_infra_ao->infra_topaxi_protecten_1_clr, MD1_PROT_STEP2_1_MASK);
        write32(&mtk_infra_ao->infra_topaxi_protecten_clr, MD1_PROT_STEP1_0_MASK);
    }
}
