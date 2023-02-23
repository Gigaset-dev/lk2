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
#include <mtk_battery.h>
#include <platform/wait.h>
#include <pmic_dlpt.h>
#include <spmi_common.h>
#include <trace.h>

#include "mt6363_dlpt_reg.h"

#define LOCAL_TRACE     0

/* SRC_SEL = 0(BATSNS) / 1(ISENSE) */
#define SRC_SEL         0
/* CNT_SEL = 1/2/4/8 times; PRD_SEL = 6/8/10/12 ms */
#define IMP_CNT_SEL     0
#define IMP_PRD_SEL     0
#define IMP_TIMEOUT_MS  30
#define IMP_ADC_OUT_RES 15
/* IMP VBAT unit is 0.1mV */
#define VOLT_FULL       18400

#ifndef mdelay
#define mdelay(x)       spin((x) * 1000)
#endif

static const u8 vbat_r_ratio[2] = {4, 1};

static struct spmi_device *sdev;

static int mt6363_dlpt_write_reg(u32 reg, u8 val)
{
    return spmi_ext_register_writel(sdev, reg, &val, 1);
}

static int mt6363_dlpt_read_reg(u32 reg, u8 *val)
{
    return spmi_ext_register_readl(sdev, reg, val, 1);
}

static bool mt6363_is_imp_ready(void)
{
    int ret;
    u8 rdata = 0;

    if (mt6363_dlpt_read_reg(MT6363_AUXADC_IMP1, &rdata) == 0)
        return rdata & AUXADC_IMP_RDY_MASK;

    return false;
}

static int mt6363_imp_conv(u32 *vbat, u32 *ibat)
{
    int ret = 0;
    u32 remain_time_ms;
    u8 work_val[2] = {0};

    /* start setting */
    mt6363_dlpt_write_reg(MT6363_AUXADC_IMP0, 1);

    /* polling IRQ status */
    remain_time_ms = wait_ms(mt6363_is_imp_ready(), IMP_TIMEOUT_MS);
    if (!remain_time_ms) {
        dprintf(CRITICAL, "do_ptim over %d ms\n", IMP_TIMEOUT_MS);
        ret = ERR_TIMED_OUT;
    }
    spmi_ext_register_readl(sdev, MT6363_AUXADC_ADC42_L, &work_val[0], 2);
    *vbat = work_val[0] | (work_val[1] << 8);
    *vbat |= 0x7FFF;

    /* stop setting */
    mt6363_dlpt_write_reg(MT6363_AUXADC_IMP0, 0);

    *vbat = *vbat * vbat_r_ratio[0] * VOLT_FULL;
    *vbat = (*vbat / vbat_r_ratio[1]) >> IMP_ADC_OUT_RES;

    gauge_read_IM_current((void *)ibat);

    LTRACEF("[do_ptim] bat %d cur %d in %dms\n",
            *vbat, *ibat, IMP_TIMEOUT_MS - remain_time_ms);
    return ret;
}

static void mt6363_enable_dummy_load(bool en)
{
    if (en) {
        /* enable isink */
        mt6363_dlpt_write_reg(MT6363_ISINK_EN_CTRL0, ISINK_EN_CTRL0_MASK);
        mt6363_dlpt_write_reg(MT6363_ISINK_EN_CTRL1, ISINK_EN_CTRL1_MASK);
    } else {
        /* disable isink */
        mt6363_dlpt_write_reg(MT6363_ISINK_EN_CTRL0, 0);
        mt6363_dlpt_write_reg(MT6363_ISINK_EN_CTRL1, 0);
    }
}

static const struct pmic_dlpt_dev mt6363_dlpt_dev = {
    .imp_conv = mt6363_imp_conv,
    .enable_dummy_load = mt6363_enable_dummy_load,
};

static void mt6363_dlpt_init(uint level)
{
    u8 buf;

    sdev = get_spmi_device(SPMI_MASTER_1, SPMI_SLAVE_4);
    if (!sdev)
        LTRACEF_LEVEL(CRITICAL, "%s: get spmi device fail\n", __func__);

    /* initial setting */
    buf = (SRC_SEL << 4) | ((IMP_CNT_SEL & 0x3) << 2) | (IMP_PRD_SEL & 0x3);
    mt6363_dlpt_write_reg(MT6363_AUXADC_IMP1, buf);
    spmi_ext_register_writel_field(sdev, MT6363_AUXADC_CON36, SRC_SEL,
                                   AUXADC_SOURCE_LBAT_SEL_MASK, 0);
    pmic_dlpt_device_register(&mt6363_dlpt_dev);
}

LK_INIT_HOOK(mt6363_dlpt, mt6363_dlpt_init, LK_INIT_LEVEL_PLATFORM - 1);
