/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <debug.h>
#include <lk/init.h>
#include <pmic_psc.h>
#include <trace.h>
#include "registers.h"

#define LOCAL_TRACE 0

static const struct pmic_psc_reg mt6357_psc_regs[] = {
    PMIC_PSC_REG(RG_PWRKEY_KEY_MODE, MT6357_TOP_RST_MISC, 8),
    PMIC_PSC_REG(RG_PWRKEY_RST_EN, MT6357_TOP_RST_MISC, 9),
    PMIC_PSC_REG(RG_PWRHOLD, MT6357_PPCCTL0, 0),
    PMIC_PSC_REG(RG_CRST, MT6357_PPCCTL0, 8),
    PMIC_PSC_REG(JUST_SMART_RST, MT6357_STRUP_CON4, 13),
    PMIC_PSC_REG(JUST_PWRKEY_RST, MT6357_STRUP_CON4, 14),
    PMIC_PSC_REG(RG_CLR_JUST_SMART_RST, MT6357_STRUP_CON4, 3),
    PMIC_PSC_REG(CLR_JUST_RST, MT6357_STRUP_CON4, 4),
    PMIC_PSC_REG(RG_SMART_RST_SDN_EN, MT6357_STRUP_CON12, 12),
    PMIC_PSC_REG(RG_SMART_RST_MODE, MT6357_STRUP_CON12, 13),
    [RG_CPS_W_KEY] = { .reg_addr = 0 },
    PMIC_PSC_REG(SWCID, MT6357_SWCID, 0),
};

static int mt6357_psc_read_field(u16 reg, u16 *val, u16 mask, u16 shift)
{
    *val = pwrap_read_field(reg, mask, shift);

    return 0;
}

static const struct pmic_psc_config mt6357_psc_config = {
    .read_field = mt6357_psc_read_field,
    .write_field = pwrap_write_field,
    .regs = mt6357_psc_regs,
};

static void mt6357_psc_init(uint level)
{
    int ret;

    ret = pmic_psc_register(&mt6357_psc_config);
    if (ret)
        LTRACEF_LEVEL(CRITICAL, "%s fail\n", __func__);
}

LK_INIT_HOOK(mt6357_psc, mt6357_psc_init, LK_INIT_LEVEL_PLATFORM_EARLY);
