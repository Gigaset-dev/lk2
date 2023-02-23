/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <debug.h>
#include <err.h>
#include <lk/init.h>
#include <pmic_psc.h>
#include <spmi_common.h>
#include <trace.h>

#include "registers.h"

#define LOCAL_TRACE 0

static struct spmi_device *sdev;

static const struct pmic_psc_reg mt6363_psc_regs[] = {
    [RG_PWRKEY_KEY_MODE] = {
        .reg_addr = MT6363_STRUP_CON12,
        .reg_mask = 0x3,
        .reg_shift = 4,
    },
    PMIC_PSC_REG(RG_PWRKEY_RST_EN, MT6363_STRUP_CON11, 2),
    PMIC_PSC_REG(RG_PWRHOLD, MT6363_PPCCTL0, 0),
    PMIC_PSC_REG(RG_CRST, MT6363_PPCCTL1, 0),
    PMIC_PSC_REG(JUST_SMART_RST, MT6363_STRUP_CON4, 1),
    PMIC_PSC_REG(JUST_PWRKEY_RST, MT6363_STRUP_CON4, 2),
    PMIC_PSC_REG(RG_CLR_JUST_SMART_RST, MT6363_STRUP_CON4, 3),
    PMIC_PSC_REG(CLR_JUST_RST, MT6363_STRUP_CON4, 4),
    PMIC_PSC_REG(RG_SMART_RST_SDN_EN, MT6363_STRUP_CON12, 1),
    PMIC_PSC_REG(RG_SMART_RST_MODE, MT6363_STRUP_CON12, 2),
    [RG_RSV_SWREG] = {
        .reg_addr = MT6363_STRUP_CON9,
        .reg_mask = 0xff,
        .reg_shift = 0,
    },
};

static int mt6363_psc_read_field(u16 reg, u16 *val, u16 mask, u16 shift)
{
    u8 rdata = 0;
    int ret = 0;

    ret = spmi_ext_register_readl(sdev, reg, &rdata, 1);
    if (ret < 0)
        return ret;

    rdata &= (mask << shift);
    *val = (rdata >> shift);

    return 0;
}

static int mt6363_psc_write_field(u16 reg, u16 val, u16 mask, u16 shift)
{
    u8 org = 0;
    int ret = 0;

    ret = spmi_ext_register_readl(sdev, reg, &org, 1);
    if (ret < 0)
        return ret;

    org &= ~(mask << shift);
    org |= (val << shift);

    ret = spmi_ext_register_writel(sdev, reg, &org, 1);
    if (ret < 0)
        return ret;

    return 0;
}

static const struct pmic_psc_config mt6363_psc_config = {
    .read_field = mt6363_psc_read_field,
    .write_field = mt6363_psc_write_field,
    .regs = mt6363_psc_regs,
};

static void mt6363_psc_init(uint level)
{
    sdev = get_spmi_device(SPMI_MASTER_1, SPMI_SLAVE_4);
    if (!sdev)
        LTRACEF_LEVEL(CRITICAL, "%s: get spmi device fail\n", __func__);
    pmic_psc_register(&mt6363_psc_config);
}

LK_INIT_HOOK(mt6363_psc, mt6363_psc_init, LK_INIT_LEVEL_PLATFORM_EARLY);
