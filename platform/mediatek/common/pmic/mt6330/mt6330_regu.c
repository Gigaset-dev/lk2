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
#include <pmic_regu.h>
#include <spmi_common.h>
#include <trace.h>
#include "registers.h"

#define LOCAL_TRACE 0

static const struct pmic_regu_reg *priv_regs;
static struct spmi_device *spmi_sdev;

static const struct pmic_regu_reg mt6330_regu_regs[] = {
    PMIC_REGU_REG(RG_LDO_VEFUSE_EN, MT6330_LDO_VEFUSE_CON0, 0),
    PMIC_REGU_REG(DA_VEFUSE_B_EN, MT6330_LDO_VEFUSE_MON, 0),
    PMIC_REGU_REG(RG_VEFUSE_VOCAL, MT6330_VEFUSE_ANA_CON0, 0),
    PMIC_REGU_REG(RG_VEFUSE_VOSEL, MT6330_VEFUSE_ANA_CON1, 0),
};

static int pmic_read_interface(u32 RegNum, u8 *val, u8 MASK, u8 SHIFT)
{
    u8 rdata = 0;
    int ret = 0;

    ret = spmi_ext_register_readl(spmi_sdev, RegNum, &rdata, 1);
    if (ret < 0) {
        LTRACEF_LEVEL(CRITICAL, "%s: spmi read fail, addr: %d, ret: %d\n",
                 __func__, RegNum, ret);
        return ret;
    }
    rdata &= (MASK << SHIFT);
    *val = (rdata >> SHIFT);

    return 0;
}

static int pmic_config_interface(u32 RegNum, u8 val, u8 MASK, u8 SHIFT)
{
    u8 org = 0;
    int ret = 0;

    ret = spmi_ext_register_readl(spmi_sdev, RegNum, &org, 1);
    if (ret < 0) {
        LTRACEF_LEVEL(CRITICAL, "%s: spmi read fail, addr: %d, ret: %d\n",
                 __func__, RegNum, ret);
        return ret;
    }
    org &= ~(MASK << SHIFT);
    org |= (val << SHIFT);

    ret = spmi_ext_register_writel(spmi_sdev, RegNum, &org, 1);
    if (ret < 0) {
        LTRACEF_LEVEL(CRITICAL, "%s: spmi write fail, addr: %d, ret: %d\n",
                 __func__, RegNum, ret);
        return ret;
    }
    return 0;
}

int vefuse_set_vol(unsigned char vosel)
{
    if (!priv_regs)
        return ERR_NOT_CONFIGURED;
    pmic_config_interface(priv_regs[RG_VEFUSE_VOSEL].reg_addr, vosel, 0xff, 0);

    return 0;
}

int vefuse_set_cal(unsigned char vocal)
{
    if (!priv_regs)
        return ERR_NOT_CONFIGURED;
    pmic_config_interface(priv_regs[RG_VEFUSE_VOCAL].reg_addr, vocal, 0xff, 0);

    return 0;
}

int vefuse_en(unsigned char en)
{
    if (!priv_regs)
        return ERR_NOT_CONFIGURED;
    pmic_config_interface(priv_regs[RG_LDO_VEFUSE_EN].reg_addr, en,
                          priv_regs[RG_LDO_VEFUSE_EN].reg_mask,
                          priv_regs[RG_LDO_VEFUSE_EN].reg_shift);

    return 0;
}

unsigned char vefuse_is_enabled(void)
{
    u8 val = 0;

    if (!priv_regs)
        return ERR_NOT_CONFIGURED;
    pmic_read_interface(priv_regs[DA_VEFUSE_B_EN].reg_addr,
                        &val,
                        priv_regs[DA_VEFUSE_B_EN].reg_mask,
                        priv_regs[DA_VEFUSE_B_EN].reg_shift);
    return val;
}

static int pmic_regu_regs_register(const struct pmic_regu_reg *regs,
                           int mstid, int slvid)
{
    unsigned char val = 0;

    spmi_sdev = get_spmi_device(mstid, slvid);
    if (!spmi_sdev) {
        LTRACEF_LEVEL(CRITICAL, "%s: get spmi device fail\n",
                      __func__);
        return -1;
    }
    if (!regs)
        return ERR_INVALID_ARGS;
    priv_regs = regs;
    LTRACEF_LEVEL(INFO, "%s: init done!\n", __func__);

    return 0;
}

static void mt6330_regu_init(uint level)
{
    int mstid, slvid;

    mstid = SPMI_MASTER_1;
    slvid = SPMI_SLAVE_4;
    pmic_regu_regs_register(mt6330_regu_regs, mstid, slvid);
}

LK_INIT_HOOK(mt6330_regu, mt6330_regu_init, LK_INIT_LEVEL_PLATFORM_EARLY);
