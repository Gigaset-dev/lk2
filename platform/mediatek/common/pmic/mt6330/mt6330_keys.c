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

static struct spmi_device *spmi_sdev;

static const struct pmic_key_reg mt6330_key_regs[] = {
    PMIC_KEY_REG(PWRKEY_DEB, MT6330_TOPSTATUS, 1),
    PMIC_KEY_REG(HOMEKEY_DEB, MT6330_TOPSTATUS, 3),
    PMIC_KEY_REG(RG_INT_STATUS_PWRKEY, MT6330_PSC_TOP_INT_STATUS0, 0),
    PMIC_KEY_REG(RG_INT_STATUS_HOMEKEY, MT6330_PSC_TOP_INT_STATUS0, 1),
};

static int mt6330_key_read_field(u16 reg, u16 *val, u16 mask, u16 shift)
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

static int mt6330_key_write_field(u16 reg, u16 val, u16 mask, u16 shift)
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

static const struct pmic_key_config mt6330_key_config = {
    .read_field = mt6330_key_read_field,
    .write_field = mt6330_key_write_field,
    .regs = mt6330_key_regs,
};


static void mt6330_keys_init(uint level)
{
    int ret;
    int mstid, slvid;

    mstid = SPMI_MASTER_1;
    slvid = SPMI_SLAVE_4;

    spmi_sdev = get_spmi_device(mstid, slvid);
    if (!spmi_sdev)
        LTRACEF_LEVEL(CRITICAL, "%s: get spmi device fail\n", __func__);

    ret = pmic_key_register(&mt6330_key_config);
    if (ret)
        LTRACEF_LEVEL(CRITICAL, "%s fail\n", __func__);
}

LK_INIT_HOOK(mt6330_keys, mt6330_keys_init, LK_INIT_LEVEL_PLATFORM_EARLY);
