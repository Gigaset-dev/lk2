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

static struct spmi_device *spmi_sdev;

static const struct pmic_psc_reg mt6330_psc_regs[] = {
    PMIC_PSC_REG(RG_PWRKEY_KEY_MODE, MT6330_TOP_RST_MISC0, 0),
    PMIC_PSC_REG(RG_PWRKEY_RST_EN, MT6330_TOP_RST_MISC0, 1),
    PMIC_PSC_REG(RG_PWRHOLD, MT6330_PPCCTL0, 0),
    PMIC_PSC_REG(RG_CRST, MT6330_PPCCTL1, 0),
    PMIC_PSC_REG(JUST_SMART_RST, MT6330_STRUP_CON4, 1),
    PMIC_PSC_REG(JUST_PWRKEY_RST, MT6330_STRUP_CON4, 2),
    PMIC_PSC_REG(RG_CLR_JUST_SMART_RST, MT6330_STRUP_CON4, 3),
    PMIC_PSC_REG(CLR_JUST_RST, MT6330_STRUP_CON4, 4),
    PMIC_PSC_REG(RG_SMART_RST_SDN_EN, MT6330_STRUP_CON12, 1),
    PMIC_PSC_REG(RG_SMART_RST_MODE, MT6330_STRUP_CON12, 2),
    [RG_CPS_W_KEY] = { .reg_addr = 0 },
    PMIC_PSC_REG(SWCID, MT6330_SWCID0, 0),
};

static int mt6330_psc_read_field(u16 reg, u16 *val, u16 mask, u16 shift)
{
    u8 rdata = 0;
    int ret = 0;

    ret = spmi_ext_register_readl(spmi_sdev, reg, &rdata, 1);
    if (ret < 0) {
        LTRACEF_LEVEL(CRITICAL, "%s: spmi read fail, addr: %d, ret: %d\n",
                      __func__, reg, ret);
        return ret;
    }
    rdata &= (mask << shift);
    *val = (rdata >> shift);

    return 0;
}

static int mt6330_psc_write_field(u16 reg, u16 val, u16 mask, u16 shift)
{
    u8 org = 0;
    int ret = 0;

    ret = spmi_ext_register_readl(spmi_sdev, reg, &org, 1);
    if (ret < 0) {
        LTRACEF_LEVEL(CRITICAL, "%s: spmi read fail, addr: %d, ret: %d\n",
                      __func__, reg, ret);
        return ret;
    }
    org &= ~(mask << shift);
    org |= (val << shift);

    ret = spmi_ext_register_writel(spmi_sdev, reg, &org, 1);
    if (ret < 0) {
        LTRACEF_LEVEL(CRITICAL, "%s: spmi write fail, addr: %d, ret: %d\n",
                      __func__, reg, ret);
        return ret;
    }

    return 0;
}

static const struct pmic_psc_config mt6330_psc_config = {
    .read_field = mt6330_psc_read_field,
    .write_field = mt6330_psc_write_field,
    .regs = mt6330_psc_regs,
};

static void mt6330_psc_init(uint level)
{
    int ret;
    int mstid, slvid;

    mstid = SPMI_MASTER_1;
    slvid = SPMI_SLAVE_4;

    spmi_sdev = get_spmi_device(mstid, slvid);
    if (!spmi_sdev)
        LTRACEF_LEVEL(CRITICAL, "%s: get spmi device fail\n", __func__);

    ret = pmic_psc_register(&mt6330_psc_config);
    if (ret)
        LTRACEF_LEVEL(CRITICAL, "%s fail\n", __func__);

}

LK_INIT_HOOK(mt6330_psc, mt6330_psc_init, LK_INIT_LEVEL_PLATFORM_EARLY);

