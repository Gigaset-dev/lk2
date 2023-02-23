/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <debug.h>
#include <mtcmos.h>
#include <platform/addressmap.h>
#include <platform/reg.h>
#include <platform/sec_devinfo.h>

#include "ccci_ld_md_log.h"

#define LOCAL_TRACE 0

#define POWERON_CONFIG_EN       (SPM_BASE + 0x0000)
#define SPM_POWER_ON_VAL1       (SPM_BASE + 0x0008)

/* only for disable modem project */

#define ccci_write32(a, v)    writel(v, a)
#define ccci_read32(a)        readl(a)

static int md_cd_topclkgen_off(void)
{
    unsigned int reg_value;

    ALWAYS_LOG("[POWER OFF]%s start\n", __func__);

    reg_value = ccci_read32(TOPCLK_BASE);
    reg_value |= ((1<<8) | (1<<9));

    ccci_write32(TOPCLK_BASE, reg_value);

    ALWAYS_LOG("[POWER OFF]%s end: set md1_clk_mod = 0x%x\n",
        __func__, ccci_read32(TOPCLK_BASE));

    return 0;
}

static void internal_md_power_down(void)
{
    ALWAYS_LOG("[Power Off] Start, call spm_mtcmos_ctrl_md1_disable\n");
    /* step1.power off md mtcmos */
    spm_mtcmos_ctrl_md1_disable();
    ALWAYS_LOG("[Power Off] spm_mtcmos_ctrl_md1_disable done\n");

    /* step2.disable MD input 26M&32K */
    md_cd_topclkgen_off();
}

int ccci_get_md_dev_en(void)
{
    unsigned int val;

    val = get_devinfo_with_index(6);
    ALWAYS_LOG("[Power On]Dev en:%d\n", val);

    if ((val & (0x1 << 0)) == 0)
        return 1;
    return 0;
}

void ccci_md_power_off(void)
{
    if (ccci_get_md_dev_en())
        internal_md_power_down();
    else
        ALWAYS_LOG("MD effused, no need power off\n");
}

void ccci_md_power_on(void)
{
    ALWAYS_LOG("[Power On]Enter MD power on\n");
    /* 1. pmic in pre-loader by pmic */

    /* 2. configure md_srclkena setting(on), O1 force on */
    write32(POWERON_CONFIG_EN, 0x0B160001);
    setbits32(SPM_POWER_ON_VAL1, (0x1 << 14));

    /* 3.Enable MD 26M & 32K */
    clrbits32(TOPCLK_BASE, ((1<<8) | (1<<9)));

    /* 4. power on MD MTCMOS by spm */
    spm_mtcmos_ctrl_md1_enable();

    ALWAYS_LOG("[Power On]MD MTCMOS power on done!\n");
}

void ccci_test_md_power_on(void)
{
    ALWAYS_LOG("[Power On]Enter test md power on mode\n");
    /* 1. pmic in pre-loader by pmic */

    /* 2. configure md_srclkena setting(on), O1 force on */
    write32(POWERON_CONFIG_EN, 0x0B160001);
    setbits32(SPM_POWER_ON_VAL1, (0x1 << 14));

    /* 3.Enable MD 26M & 32K */
    clrbits32(TOPCLK_BASE, ((1<<8) | (1<<9)));

    /* 4. power on MD MTCMOS by spm */
    spm_mtcmos_ctrl_md1_enable();

    ALWAYS_LOG("[Power On]MD MTCMOS power on done!\n");
}
