/*
 * Copyright (c) 2021 MediaTek Inc.
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

#include <debug.h>
#include <err.h>
#include <init_mtk.h>
#include <lib/kcmdline.h>
#include <lk/init.h>
#include <pmic_psc.h>
#include <pmic_wrap_common.h>
#include <trace.h>
#include "registers.h"

#define LOCAL_TRACE 0

static const struct pmic_psc_reg mt6358_psc_regs[] = {
    PMIC_PSC_REG(RG_PWRKEY_KEY_MODE, MT6358_TOP_RST_MISC, 8),
    PMIC_PSC_REG(RG_PWRKEY_RST_EN, MT6358_TOP_RST_MISC, 9),
    PMIC_PSC_REG(RG_PWRHOLD, MT6358_PPCCTL0, 0),
    PMIC_PSC_REG(RG_CRST, MT6358_PPCCTL0, 8),
    PMIC_PSC_REG(JUST_SMART_RST, MT6358_STRUP_CON4, 13),
    PMIC_PSC_REG(JUST_PWRKEY_RST, MT6358_STRUP_CON4, 14),
    PMIC_PSC_REG(RG_CLR_JUST_SMART_RST, MT6358_STRUP_CON4, 3),
    PMIC_PSC_REG(CLR_JUST_RST, MT6358_STRUP_CON4, 4),
    PMIC_PSC_REG(RG_SMART_RST_SDN_EN, MT6358_STRUP_CON12, 12),
    PMIC_PSC_REG(RG_SMART_RST_MODE, MT6358_STRUP_CON12, 13),
    [RG_CPS_W_KEY] = { .reg_addr = 0 },
    PMIC_PSC_REG(SWCID, MT6358_SWCID, 0),
};

static int mt6358_psc_read_field(u16 reg, u16 *val, u16 mask, u16 shift)
{
    *val = pwrap_read_field(reg, mask, shift);

    return 0;
}

static const struct pmic_psc_config mt6358_psc_config = {
    .read_field = mt6358_psc_read_field,
    .write_field = pwrap_write_field,
    .regs = mt6358_psc_regs,
};

static void mt6358_psc_init(uint level)
{
    int ret;

    ret = pmic_psc_register(&mt6358_psc_config);
    if (ret)
        LTRACEF_LEVEL(CRITICAL, "%s fail\n", __func__);
}

LK_INIT_HOOK(mt6358_psc, mt6358_psc_init, LK_INIT_LEVEL_PLATFORM_EARLY);

#ifdef CHECK_HAS_BATTERY_REMOVED
static void mt6358_psc_set_cmdline(uint level)
{
    if (pwrap_read_field(MT6358_TOP_RST_STATUS, 0x7, 0) == 0x7)
        kcmdline_append("has_battery_removed=0");
    else
        kcmdline_append("has_battery_removed=1");
    pwrap_write(MT6358_TOP_RST_STATUS, 0x4F);
}

MT_LK_INIT_HOOK(BL33, mt6358_psc_cmdline, mt6358_psc_set_cmdline, LK_INIT_LEVEL_TARGET);
#endif
