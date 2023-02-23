/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <debug.h>
#include <lk/init.h>
#include <pmic_keys.h>
#include <pmic_wrap_common.h>
#include <trace.h>
#include "registers.h"

#define LOCAL_TRACE 0

static const struct pmic_key_reg mt6357_key_regs[] = {
    PMIC_KEY_REG(PWRKEY_DEB, MT6357_TOPSTATUS, 1),
    PMIC_KEY_REG(HOMEKEY_DEB, MT6357_TOPSTATUS, 3),
    PMIC_KEY_REG(RG_INT_STATUS_PWRKEY, MT6357_PSC_TOP_INT_STATUS0, 0),
    PMIC_KEY_REG(RG_INT_STATUS_HOMEKEY, MT6357_PSC_TOP_INT_STATUS0, 1),
};

static int mt6357_key_read_field(u16 reg, u16 *val, u16 mask, u16 shift)
{
    *val = pwrap_read_field(reg, mask, shift);

    return 0;
}

static const struct pmic_key_config mt6357_key_cfg = {
    .read_field = mt6357_key_read_field,
    .write_field = pwrap_write_field,
    .regs = mt6357_key_regs,
};

static void mt6357_keys_init(uint level)
{
    int ret;

    ret = pmic_key_register(&mt6357_key_cfg);
    if (ret)
        LTRACEF_LEVEL(CRITICAL, "%s fail\n", __func__);
}

LK_INIT_HOOK(mt6357_keys, mt6357_keys_init, LK_INIT_LEVEL_PLATFORM_EARLY);
