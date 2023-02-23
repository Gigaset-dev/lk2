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
#include <pmic_keys.h>
#include <spmi_common.h>
#include <trace.h>

#include "registers.h"

#define LOCAL_TRACE 0

static struct spmi_device *sdev;

static const struct pmic_key_reg mt6363_key_regs[] = {
    PMIC_KEY_REG(PWRKEY_DEB, MT6363_TOPSTATUS, 1),
    PMIC_KEY_REG(HOMEKEY_DEB, MT6363_TOPSTATUS, 3),
    PMIC_KEY_REG(HOMEKEY2_DEB, MT6363_TOPSTATUS, 4),
    PMIC_KEY_REG(RG_INT_STATUS_PWRKEY, MT6363_PSC_TOP_INT_STATUS0, 0),
    PMIC_KEY_REG(RG_INT_STATUS_HOMEKEY, MT6363_PSC_TOP_INT_STATUS0, 1),
    PMIC_KEY_REG(RG_INT_STATUS_HOMEKEY_2, MT6363_PSC_TOP_INT_STATUS0, 2),
};

static int mt6363_key_read_field(u16 reg, u16 *val, u16 mask, u16 shift)
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

static int mt6363_key_write_field(u16 reg, u16 val, u16 mask, u16 shift)
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

static const struct pmic_key_config mt6363_key_cfg = {
    .read_field = mt6363_key_read_field,
    .write_field = mt6363_key_write_field,
    .regs = mt6363_key_regs,
};

static void mt6363_keys_init(uint level)
{
    sdev = get_spmi_device(SPMI_MASTER_1, SPMI_SLAVE_4);
    if (!sdev)
        LTRACEF_LEVEL(CRITICAL, "%s: get spmi device fail\n", __func__);
    pmic_key_register(&mt6363_key_cfg);
}

LK_INIT_HOOK(mt6363_keys, mt6363_keys_init, LK_INIT_LEVEL_PLATFORM_EARLY);
