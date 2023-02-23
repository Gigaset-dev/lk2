/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include "adsp_plat.h"
#include <mtcmos.h>
#include <platform/adsp_reg.h>
#include <platform/reg.h>
#include <trace.h>

#define LOCAL_TRACE (0)

void adsp_set_devapc_domain(void)
{
    /* tinysys devapc domain setting: adsp part only */
    LTRACEF("%s set tinysys devapc\n", LOG_TAG);
    writel(DOMAIN_ADSP, R_ADSP_DOMAIN);
}

void adsp_power_on(void)
{
    LTRACEF("%s[%s] %s\n", LOG_TAG, SUB_PLATFORM, __func__);
    /* power on ADSP MTCMOS by spm */
    spm_mtcmos_ctrl_adsp_shutdown_enable();
    /* ADSPPLL on */
    setbits32(ADSPPLL_CON3, (0x1 << 0));
    udelay(30);
    clrbits32(ADSPPLL_CON3, (0x1 << 1));
    udelay(1);
    setbits32(ADSPPLL_CON0, (0x1 << 0));
    /* CLKMUX */
    setbits32(CLK_CFG_13_CLR, (0x7 << ADSP_SEL_OFFSET));
    setbits32(CLK_CFG_13_SET, (ADSPPLL_CK << ADSP_SEL_OFFSET));
    setbits32(CLK_CFG_UPDATE1, (1 << ADSP_CK_UPDATE));
    /* ADSP CLK CG */
    clrbits32(AUDIODSP_CK_CG, (CG_ENABLE << CG_OFFSET));
}

void adsp_power_off(void)
{
    LTRACEF("%s[%s] %s\n", LOG_TAG, SUB_PLATFORM, __func__);
    /* ADSP CLK CG */
    setbits32(AUDIODSP_CK_CG, (CG_ENABLE << CG_OFFSET));
    /* ADSP SEL */
    setbits32(CLK_CFG_13_CLR, (0x7 << ADSP_SEL_OFFSET));
    setbits32(CLK_CFG_13_SET, (CLK_26M_CK << ADSP_SEL_OFFSET));
    setbits32(CLK_CFG_UPDATE1, (1 << ADSP_CK_UPDATE));
    /* ADSPPLL off */
    clrbits32(ADSPPLL_CON0, (0x1 << 0));
    setbits32(ADSPPLL_CON3, (0x1 << 1));
    clrbits32(ADSPPLL_CON3, (0x1 << 0));
    /* power off ADSP MTCMOS by spm: dormant off */
    spm_mtcmos_ctrl_adsp_dormant_disable();
}
