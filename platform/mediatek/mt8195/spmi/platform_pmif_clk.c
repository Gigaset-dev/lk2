/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <assert.h>
#include <err.h>
#include <compiler.h>
#include <lk/init.h>
#include <pmif_common.h>
#include <spmi_common.h>
#include <platform/reg.h>
#include <platform/pll.h>

#define ULPOSC_EN_SHFT          0
#define ULPOSC_RST_SHFT         1
#define ULPOSC_CG_EN_SHFT       2
#define ULPOSC_CLK_SEL_SHFT     3

#define HIGH_DIS_SUB            1

/* PLL_ULPOSC_CON0 */
#define RG_OSC_CALI_MSK         0x7f
#define RG_OSC_CALI_SHFT        0
#define RG_OSC_IBAND_MSK        0x7f
#define RG_OSC_IBAND_SHFT       7
#define RG_OSC_FBAND_MSK        0xf
#define RG_OSC_FBAND_SHFT       14
#define RG_OSC_DIV_MSK          0x3f
#define RG_OSC_DIV_SHFT         18
#define RG_OSC_CP_EN_MSK        0x1
#define RG_OSC_CP_EN_SHFT       24
#define RG_OSC_MOD_MSK          0x3
#define RG_OSC_MOD_SHFT         25
#define RG_OSC_DIV2_EN_MSK      0x1
#define RG_OSC_DIV2_EN_SHFT     27

/* PLL_ULPOSC_CON1 */
#define RG_OSC_RSV1_MSK         0xff
#define RG_OSC_RSV1_SHFT        0
#define RG_OSC_RSV2_MSK         0xff
#define RG_OSC_RSV2_SHFT        8
#define RG_OSC_32KCALI_MSK      0xff
#define RG_OSC_32KCALI_SHFT     16
#define RG_OSC_BIAS_MSK         0xff
#define RG_OSC_BIAS_SHFT        24

#define SPM_PROJECT_CODE        0xb16

/* ulposc1 freq et to 262Mhz */
#define FREQ_248MHZ         248

#define CP_EN               0x0
/*
 * 18.3333Mhz * (osc_div + 2) = target(ideal model)
 * simulation result : osc_div = d'14
 */
#define OSC_DIV             0xE
#define FBAND_VAL           0x2
#define IBAND_VAL           0x52

/* ulposc cali value start from middle of [0:7] */
#define CAL_START_VAL       0x40
#define CAL_MAX_VAL         RG_OSC_CALI_MSK
#define DIV2_EN             0x0
#define MOD_VAL             0x0
#define RSV2_VAL            0x0
#define RSV1_VAL            0x29
#define CALI32K_VAL         0x0
#define BIAS_VAL            0x41
/* calibation tolerance rate, unit: 0.1% */
#define CAL_TOL_RATE        0x28

#define abs(a) (((a) < 0) ? -(a) : (a))
#define FREQ_METER_ABIST_AD_OSC_CK  48

static void pmif_ulposc_config(void)
{
    /* Config ULPOSC_CON0 */

    /* OSC_DIV2_EN= 1b'0 */
    clrbits32(&mtk_apmixed->ulposc1_con0, RG_OSC_DIV2_EN_MSK << RG_OSC_DIV2_EN_SHFT);
    setbits32(&mtk_apmixed->ulposc1_con0, (DIV2_EN & RG_OSC_DIV2_EN_MSK)
              << RG_OSC_DIV2_EN_SHFT);

    /* OSC_MOD = 00 */
    clrbits32(&mtk_apmixed->ulposc1_con0, RG_OSC_MOD_MSK << RG_OSC_MOD_SHFT);
    setbits32(&mtk_apmixed->ulposc1_con0, (MOD_VAL & RG_OSC_MOD_MSK)
              << RG_OSC_MOD_SHFT);

    /* OSC_CP_EN = 0 */
    clrbits32(&mtk_apmixed->ulposc1_con0, RG_OSC_CP_EN_MSK << RG_OSC_CP_EN_SHFT);
    setbits32(&mtk_apmixed->ulposc1_con0, CP_EN & (RG_OSC_CP_EN_MSK)
              << RG_OSC_CP_EN_SHFT);

    /* OSC_DIV = 1110 */
    clrbits32(&mtk_apmixed->ulposc1_con0, RG_OSC_DIV_MSK << RG_OSC_DIV_SHFT);
    setbits32(&mtk_apmixed->ulposc1_con0, (OSC_DIV & RG_OSC_DIV_MSK)
              << RG_OSC_DIV_SHFT);

    /* OSC_FBAND = 0010 */
    clrbits32(&mtk_apmixed->ulposc1_con0, RG_OSC_FBAND_MSK << RG_OSC_FBAND_SHFT);
    setbits32(&mtk_apmixed->ulposc1_con0, (FBAND_VAL & RG_OSC_FBAND_MSK)
              << RG_OSC_FBAND_SHFT);

    /* OSC_IBAND = 1010010 */
    clrbits32(&mtk_apmixed->ulposc1_con0, RG_OSC_IBAND_MSK << RG_OSC_IBAND_SHFT);
    setbits32(&mtk_apmixed->ulposc1_con0, (IBAND_VAL & RG_OSC_IBAND_MSK)
              << RG_OSC_IBAND_SHFT);

    /* OSC_CALI = 1000000 */
    clrbits32(&mtk_apmixed->ulposc1_con0, RG_OSC_CALI_MSK << RG_OSC_CALI_SHFT);
    setbits32(&mtk_apmixed->ulposc1_con0, (CAL_START_VAL & RG_OSC_CALI_MSK)
              << RG_OSC_CALI_SHFT);

    /* Config ULPOSC_CON1 */

    /* OSC_BIAS = 8b'01000001 */
    clrbits32(&mtk_apmixed->ulposc1_con1, RG_OSC_BIAS_MSK << RG_OSC_BIAS_SHFT);
    setbits32(&mtk_apmixed->ulposc1_con1, (BIAS_VAL & RG_OSC_BIAS_MSK)
              << RG_OSC_BIAS_SHFT);

    /* OSC_32KCALI = 8b'00000000 */
    clrbits32(&mtk_apmixed->ulposc1_con1, RG_OSC_32KCALI_MSK << RG_OSC_32KCALI_SHFT);
    setbits32(&mtk_apmixed->ulposc1_con1, (CALI32K_VAL & RG_OSC_32KCALI_MSK)
              << RG_OSC_32KCALI_SHFT);

    /* OSC_RSV2= 8b'00000000 */
    clrbits32(&mtk_apmixed->ulposc1_con1, RG_OSC_RSV2_MSK << RG_OSC_RSV2_SHFT);
    setbits32(&mtk_apmixed->ulposc1_con1, (RSV2_VAL & RG_OSC_RSV2_MSK)
              << RG_OSC_RSV2_SHFT);

    /* OSC_RSV1= 8b'00101001 */
    clrbits32(&mtk_apmixed->ulposc1_con1, RG_OSC_RSV1_MSK << RG_OSC_RSV1_SHFT);
    setbits32(&mtk_apmixed->ulposc1_con1, (RSV1_VAL & RG_OSC_RSV1_MSK)
              << RG_OSC_RSV1_SHFT);
}

static u32 pmif_get_ulposc_freq_mhz(u32 cali_val)
{
    u32 result = 0;

    /* set calibration value */
    clrbits32(&mtk_apmixed->ulposc1_con0, RG_OSC_CALI_MSK << RG_OSC_CALI_SHFT);
    setbits32(&mtk_apmixed->ulposc1_con0, (cali_val & RG_OSC_CALI_MSK)
              << RG_OSC_CALI_SHFT);
    spin(50);
    result = mt_get_freq(FREQ_METER_ABIST_AD_OSC_CK, ABIST);

    return result / 1000;
}

static int pmif_ulposc_cali(void)
{
    u32 current_val = 0, min = 0, max = CAL_MAX_VAL, middle;
    int ret = 0, diff_by_min, diff_by_max, cal_result;

    do {
        middle = (min + max) / 2;
        if (middle == min)
            break;

        current_val = pmif_get_ulposc_freq_mhz(middle);
        if (current_val > FREQ_248MHZ)
            max = middle;
        else
            min = middle;
    } while (min <= max);

    diff_by_min = pmif_get_ulposc_freq_mhz(min) - FREQ_248MHZ;
    diff_by_min = abs(diff_by_min);

    diff_by_max = pmif_get_ulposc_freq_mhz(max) - FREQ_248MHZ;
    diff_by_max = abs(diff_by_max);

    if (diff_by_min < diff_by_max) {
        cal_result = min;
        current_val = pmif_get_ulposc_freq_mhz(min);
    } else {
        cal_result = max;
        current_val = pmif_get_ulposc_freq_mhz(max);
    }

    /* check if calibrated value is in the range of target value +- 15% */
    if (current_val < (FREQ_248MHZ * (1000 - CAL_TOL_RATE) / 1000) ||
            current_val > (FREQ_248MHZ * (1000 + CAL_TOL_RATE) / 1000)) {
        dprintf(ALWAYS, "[%s] calibration fail: %dM\n", __func__, current_val);
        ret = 1;
    }

    return ret;
}

static int pmif_init_ulposc(void)
{
    /* calibrate ULPOSC1 */
    pmif_ulposc_config();

    /* enable spm swinf */
    write32(&mtk_spm->poweron_config_en, (SPM_PROJECT_CODE << 16) | (0x1 << 0));

    /* turn on ulposc */
    setbits32(&mtk_spm->ulposc_con, 0x1 << ULPOSC_EN_SHFT);
    spin(100);
    setbits32(&mtk_scp->scp_clk_on_ctrl, 0x1 << HIGH_DIS_SUB);
    setbits32(&mtk_spm->ulposc_con, 0x1 << ULPOSC_CG_EN_SHFT);

    return pmif_ulposc_cali();
}

void pmif_clk_init(void)
{
    if (pmif_init_ulposc()) {
        dprintf(ALWAYS, "[%s] init fail\n", __func__);
        return;
    }

    /* turn off pmic_cg_tmr, cg_ap, cg_md, cg_conn clock */
    write32(&mtk_infracfg_ao->module_sw_cg_0_set, 0x0000000f);
    write32(&mtk_topckgen->clk_cfg_9_clr, (0x1 << 7) | (0x1 << 4) | (0x7 << 0));
    write32(&mtk_topckgen->clk_cfg_update1, 0x1 << 4);

    /* use ULPOSC1 clock */
    write32(&mtk_infracfg_ao->pmicw_clock_ctrl_clr, 0xf);

    /* toggle SPI/SPMI sw reset */
    write32(&mtk_infracfg_ao->infra_globalcon_rst2_set, 0x1);
    write32(&mtk_infracfg_ao->infra_globalcon_rst2_clr, 0x1);
    write32(&mtk_infracfg_ao->infra_globalcon_rst2_set, 0x1 << 14);
    write32(&mtk_infracfg_ao->infra_globalcon_rst2_clr, 0x1 << 14);

    /* turn on pmic_cg_tmr, cg_ap, cg_md, cg_conn clock */
    write32(&mtk_infracfg_ao->module_sw_cg_0_clr, 0x0000000f);
}

