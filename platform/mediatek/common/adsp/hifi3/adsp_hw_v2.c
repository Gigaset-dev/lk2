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
    LTRACEF("%s set adspsys devapc\n", LOG_TAG);
}

void adsp_power_on(void)
{
    /* power on ADSP MTCMOS by spm */
    spm_mtcmos_ctrl_adsp_ao_enable();
    spm_mtcmos_ctrl_adsp_infra_enable();
    spm_mtcmos_ctrl_adsp_top_shutdown_enable();
    /* set SRAM non-sleep from dormant disable */
    spm_mtcmos_ctrl_adsp_top_dormant_enable();

    /* ADSPPLL on */
    setbits32(ADSPPLL_CON3, (0x1 << 0));
    udelay(30);
    clrbits32(ADSPPLL_CON3, (0x1 << 1));
    udelay(1);
    setbits32(ADSPPLL_CON0, (0x1 << 0));
    /* CLKMUX */
    setbits32(CLKCFG_ADSP_SEL_CLR, (0x7 << ADSP_SEL_OFFSET));
    setbits32(CLKCFG_ADSP_SEL_SET, (ADSPPLL_CK << ADSP_SEL_OFFSET));
    setbits32(CLKCFG_UPDATE, (1 << ADSP_CK_UPDATE));
}

void adsp_power_off(void)
{
    /* ADSP SEL */
    setbits32(CLKCFG_ADSP_SEL_CLR, (0x7 << ADSP_SEL_OFFSET));
    setbits32(CLKCFG_ADSP_SEL_SET, (CLK_26M_CK << ADSP_SEL_OFFSET));
    setbits32(CLKCFG_UPDATE, (1 << ADSP_CK_UPDATE));
    /* ADSPPLL off */
    clrbits32(ADSPPLL_CON0, (0x1 << 0));
    setbits32(ADSPPLL_CON3, (0x1 << 1));
    clrbits32(ADSPPLL_CON3, (0x1 << 0));

#if 0 //bringup bypass
    /* power off ADSP MTCMOS by spm: dormant off */
    spm_mtcmos_ctrl_adsp_top_dormant_disable();
    spm_mtcmos_ctrl_adsp_infra_disable();
#else
    LTRACEF("%s bypass %s mtcmos ctrl()\n", LOG_TAG, __func__);
#endif
}
