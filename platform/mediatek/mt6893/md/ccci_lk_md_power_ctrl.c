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

#define INFRA_AO_MD_SRCCLKENA   (INFRACFG_AO_BASE+0x0F0C)
#define INFRA_MISC2             (0xF0C)

#define SPM_POWER_ON_CONFIG_EN (SPM_BASE + 0x0)
#define SPM_POWER_ON_VAL1      (SPM_BASE + 0x8)

#define INFRA_AO_MD_SRCCLKENA   (INFRACFG_AO_BASE+0x0F0C)
#define INFRA_MISC2             (0xF0C)

static void internal_md_power_down(void)
{
    ALWAYS_LOG("[Power Off] Start, call spm_mtcmos_ctrl_md1_disable\n");
    /* 0. power off md */
    spm_mtcmos_ctrl_md1_disable();
    ALWAYS_LOG("[Power Off] spm_mtcmos_ctrl_md1_disable done\n");

    /* 1. release srcclkena */
    clrbits32(INFRA_AO_MD_SRCCLKENA, 0xFF);
    ALWAYS_LOG("[Power Off]MD-off:src clk ena = 0x%X\n", read32(INFRA_AO_MD_SRCCLKENA));
}

static void md_src_clk_ena(void)
{
    ALWAYS_LOG("[Power On]MD srcclkena setting start\n");
    /* MD srcclkena setting: [7:4]=4'h0110, [3:0]=4'h1101 */
    clrbits32(INFRA_AO_MD_SRCCLKENA, 0xFF);
    setbits32(INFRA_AO_MD_SRCCLKENA, 0x21);
    ALWAYS_LOG("[Power On]MD srcclkena setting:0x%x\n", read32(INFRA_AO_MD_SRCCLKENA));
}

static void md_pwr_o1(void)
{
    unsigned int val;

    ALWAYS_LOG("[Power On]SPM O1 start\n");
    write32(SPM_POWER_ON_CONFIG_EN, 0x0B160001);
    val = read32(SPM_POWER_ON_VAL1);
    val |= (0x1 << 21);
    write32(SPM_POWER_ON_VAL1, val);
    ALWAYS_LOG("[Power On]SPM O1 val:0x%x\n", read32(SPM_POWER_ON_VAL1));
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

    /* 2. Config. md_srcclkena */
    md_src_clk_ena();

    /* 3. power on MD MTCMOS by spm */
    spm_mtcmos_ctrl_md1_enable();
    ALWAYS_LOG("[Power On]MD MTCMOS power on done!\n");
}

void ccci_test_md_power_on(void)
{
    ALWAYS_LOG("[Power On]Enter test md power on mode\n");
    /* Config. md_srcclkena */
    md_src_clk_ena();

    /* Config O1 */
    md_pwr_o1();

    /* 3. power on MD MTCMOS by spm */
    spm_mtcmos_ctrl_md1_enable();
    ALWAYS_LOG("[Power On]MD MTCMOS power on done!\n");
}
