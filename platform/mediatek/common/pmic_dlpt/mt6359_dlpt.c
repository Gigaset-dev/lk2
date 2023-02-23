/*
 * Copyright (c) 2019 MediaTek Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <debug.h>
#include <err.h>
#include <lk/init.h>
#include <mtk_battery.h>
#include <platform/wait.h>
#include <pmic_dlpt.h>
#include <pmic_wrap_common.h>
#include <trace.h>

#include "mt6359_dlpt_reg.h"

#define LOCAL_TRACE     0

/* SRC_SEL = 0(BATSNS) / 1(ISENSE) */
#define SRC_SEL         0
/* CNT_SEL = 1/2/4/8 times; PRD_SEL = 6/8/10/12 ms */
#define IMP_CNT_SEL     0
#define IMP_PRD_SEL     0
#define IMP_TIMEOUT_MS  30
#define IMP_ADC_OUT_RES 15
/* IMP VBAT unit is 0.1mV */
#define VOLT_FULL       18000
#define PTIM_AVG_SIZE   5
#define EWW_TO_M(n)     ((n) / 10)

#ifndef mdelay
#define mdelay(x)       spin((x) * 1000)
#endif

static const u16 cnt_table[4] = {1, 2, 4, 8};
static const u16 prd_table[4] = {6, 8, 10, 12};
static const u8 vbat_r_ratio[2] = {7, 2};

static int mt6359_imp_conv(u32 *vbat, u32 *ibat)
{
    int ret = 0;
    u32 remain_time_ms;

    /* start setting */
    pwrap_write(MT6359_AUXADC_IMP0, 1);

    /* wait IMPEDANCE to start measurement */
    mdelay(cnt_table[IMP_CNT_SEL] * prd_table[IMP_PRD_SEL]);

    /* polling IRQ status */
    remain_time_ms = wait_ms(pwrap_read_field(MT6359_AUXADC_IMP1, 0x1, 15),
                             IMP_TIMEOUT_MS);
    if (!remain_time_ms) {
        dprintf(CRITICAL, "do_ptim over %d ms\n", IMP_TIMEOUT_MS);
        ret = ERR_TIMED_OUT;
    }
    *vbat = pwrap_read_field(MT6359_AUXADC_IMP3, 0x7FFF, 0);

    /* stop setting */
    pwrap_write(MT6359_AUXADC_IMP0, 0);

    *vbat = *vbat * vbat_r_ratio[0] * VOLT_FULL;
    *vbat = (*vbat / vbat_r_ratio[1]) >> IMP_ADC_OUT_RES;

    gauge_read_IM_current((void *)ibat);

    LTRACEF("[do_ptim] bat %d cur %d in %dms\n",
            *vbat, *ibat, IMP_TIMEOUT_MS - remain_time_ms);
    return ret;
}

static void mt6359_enable_dummy_load(bool en)
{
    if (en) {
        /* enable isink step */
        pwrap_write_field(MT6359_ISINK0_CON1, ISINK_STEP_MASK,
                          ISINK_STEP_MASK, ISINK_STEP_SHIFT);
        pwrap_write_field(MT6359_ISINK1_CON1, ISINK_STEP_MASK,
                          ISINK_STEP_MASK, ISINK_STEP_SHIFT);

        /* enable isink */
        pwrap_write_field(MT6359_ISINK_EN_CTRL_SMPL, ISINK_BIAS_EN_MASK,
                          ISINK_BIAS_EN_MASK, ISINK_BIAS_EN_SHIFT);
        pwrap_write_field(MT6359_ISINK_EN_CTRL_SMPL, ISINK_EN_MASK,
                          ISINK_EN_MASK, ISINK_EN_SHIFT);
    } else {
        /* disable isink */
        pwrap_write_field(MT6359_ISINK_EN_CTRL_SMPL, 0,
                          ISINK_BIAS_EN_MASK, ISINK_BIAS_EN_SHIFT);
        pwrap_write_field(MT6359_ISINK_EN_CTRL_SMPL, 0,
                          ISINK_EN_MASK, ISINK_EN_SHIFT);
    }
}

static const struct pmic_dlpt_dev mt6359_dlpt_dev = {
    .imp_conv = mt6359_imp_conv,
    .enable_dummy_load = mt6359_enable_dummy_load,
};

static void mt6359_dlpt_init(uint level)
{
    /* initial setting */
    pwrap_write_field(MT6359_AUXADC_IMP1, IMP_CNT_SEL, 0x3, 2);
    pwrap_write_field(MT6359_AUXADC_IMP1, IMP_PRD_SEL, 0x3, 0);
    pwrap_write_field(MT6359_AUXADC_CON16, SRC_SEL, 0x1, 0);
    pwrap_write_field(MT6359_AUXADC_IMP1, SRC_SEL, 0x1, 4);
    pmic_dlpt_device_register(&mt6359_dlpt_dev);
}

LK_INIT_HOOK(mt6359_dlpt, mt6359_dlpt_init, LK_INIT_LEVEL_PLATFORM - 1);

void do_ptim_gauge(unsigned int *vbat, unsigned int *ibat)
{
    int i, j;
    u32 ptim_vbat = 0, ptim_ibat = 0;
    u32 vbats[PTIM_AVG_SIZE] = {0};
    u32 ibats[PTIM_AVG_SIZE] = {0};

    for (i = 0; i < PTIM_AVG_SIZE; i++) {
        mt6359_imp_conv(&ptim_vbat, &ptim_ibat);

        /* insertion sort */
        for (j = i; j > 0; j--) {
            if (ptim_vbat < vbats[j - 1])
                vbats[j] = vbats[j - 1];
            else
                break;
        }
        vbats[j] = ptim_vbat;

        /* insertion sort */
        for (j = i; j > 0; j--) {
            if (ptim_ibat < ibats[j - 1])
                ibats[j] = ibats[j - 1];
            else
                break;
        }
        ibats[j] = ptim_ibat;
    }

    *vbat = vbats[PTIM_AVG_SIZE >> 1];
    *ibat = ibats[PTIM_AVG_SIZE >> 1];
    dprintf(CRITICAL, "%s %d(%d, %d, %d, %d, %d)mV, %d(%d, %d, %d, %d, %d)mA\n",
            __func__, EWW_TO_M(*vbat), EWW_TO_M(vbats[0]),
            EWW_TO_M(vbats[1]), EWW_TO_M(vbats[2]), EWW_TO_M(vbats[3]),
            EWW_TO_M(vbats[4]), EWW_TO_M(*ibat), EWW_TO_M(ibats[0]),
            EWW_TO_M(ibats[1]), EWW_TO_M(ibats[2]), EWW_TO_M(ibats[3]),
            EWW_TO_M(ibats[4]));
}
