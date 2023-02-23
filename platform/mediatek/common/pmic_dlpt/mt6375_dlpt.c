/*
 * copyright (c) 2021 mediatek inc.
 *
 * use of this source code is governed by a mit-style
 * license that can be found in the license file or at
 * https://opensource.org/licenses/mit
 */

#include <bits.h>
#include <debug.h>
#include <err.h>
#include <lk/init.h>
#include <mtk_battery.h>
#include <platform/wait.h>
#include <pmic_dlpt.h>
#include <regmap.h>
#include <spmi_common.h>
#include <stdbool.h>
#include <trace.h>

#include "mt6359_dlpt_reg.h"
#include "mt6363_dlpt_reg.h"

#define LOCAL_TRACE     0

#define AUXADC_OUT_IMP_AVG        0x41C
#define AUXADC_IMP0               0x4A8
#define AUXADC_IMP1               0x4A9
#define HK_TOP_INT_CON1_SET       0x314
#define HK_TOP_INT_CON1_CLR       0x315
#define HK_TOP_INT_RAW_STATUS1    0x31F
#define AUXADC_SOURCE_LBATSEL_MASK  BIT(0xFF, 0)
#define AUXADC_IMP_CHSEL_MASK       BIT(0xFF, 4)
#define AUXADC_IMP_PRDSEL_MASK      0x0C
#define AUXADC_IMP_CNTSEL_MASK      0x03
#define INT_RAW_AUXADC_IMP          BIT(0xFF, 0)
#define ADC_IMP_ENMASK              BIT(0xFF, 0)

/* CNT_SEL = 1/2/4/8 times; PRD_SEL = 6/8/10/12 ms */
#define IMP_CNT_SEL     0
#define IMP_PRD_SEL     0
#define IMP_TIMEOUT_MS  30

#define PTIM_AVG_SIZE   5
#define EWW_TO_M(n)       ((n) / 10)

static const u8 cnt_table[4] = {1, 2, 4, 8};
static const u8 prd_table[4] = {6, 8, 10, 12};
static struct regmap *regmap;

static bool mt6375_is_imp_irq_triggered(void)
{
    int ret;
    u32 val = 0;

    ret = regmap_read(regmap, HK_TOP_INT_RAW_STATUS1, &val);
    return ret != 0 ? false : (val & INT_RAW_AUXADC_IMP);
}

static int mt6375_imp_conv(u32 *vbat, u32 *ibat)
{
    int ret = 0;
    u32 remain_time_ms;
    u16 raw_val = 0;

    regmap_update_bits(regmap, AUXADC_IMP1, AUXADC_IMP_CNTSEL_MASK,
                       IMP_CNT_SEL);
    regmap_update_bits(regmap, AUXADC_IMP1, AUXADC_IMP_PRDSEL_MASK,
                       IMP_PRD_SEL);

    ret = regmap_write(regmap, AUXADC_IMP0, 0);
    if (ret < 0) {
        dprintf(CRITICAL, "failed to clear auxadc imp\n");
        return ret;
    }

    ret = regmap_write(regmap, HK_TOP_INT_CON1_CLR, INT_RAW_AUXADC_IMP);
    if (ret < 0) {
        dprintf(CRITICAL, "failed to clear IMP IRQ\n");
        return ret;
    }

    ret = regmap_write(regmap, HK_TOP_INT_CON1_SET, INT_RAW_AUXADC_IMP);
    if (ret < 0) {
        dprintf(CRITICAL, "failed to enable IMP IRQ\n");
        return ret;
    }

    /* start setting */
    ret = regmap_write(regmap, AUXADC_IMP0, ADC_IMP_ENMASK);
    if (ret < 0) {
        dprintf(CRITICAL, "failed to enable auxadc imp\n");
        return ret;
    }

    /* wait IMPEDANCE to start measurement */
    spin(cnt_table[IMP_CNT_SEL] * prd_table[IMP_PRD_SEL] * 1000);

    /* polling IRQ status */
    remain_time_ms = wait_ms(mt6375_is_imp_irq_triggered(), IMP_TIMEOUT_MS);
    if (!remain_time_ms) {
        dprintf(CRITICAL, "do_ptim over %d ms\n", IMP_TIMEOUT_MS);
        return ERR_TIMED_OUT;
    }

    ret = regmap_bulk_read(regmap, AUXADC_OUT_IMP_AVG, (u8 *)&raw_val, 2);
    if (ret < 0) {
        dprintf(CRITICAL, "failed to read imp vbat\n");
        return ret;
    }
    *vbat = (raw_val & 0x7FFF) * 7360 >> 15;
    *vbat *= 10; /* make unit 0.1mV */

    /* stop setting */
    ret = regmap_write(regmap, AUXADC_IMP0, 0);
    if (ret < 0) {
        dprintf(CRITICAL, "failed to disable auxadc imp\n");
        return ret;
    }

    gauge_read_IM_current((void *)ibat);
    LTRACEF("%s bat %d cur %d in %dms\n", __func__, *vbat, *ibat,
            IMP_TIMEOUT_MS - remain_time_ms);
    return ret;
}

#ifdef PMIC_MT6359_DUMMY_LOAD
#include <pmic_wrap_common.h>
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
#endif /* PMIC_MT6359_DUMMY_LOAD */

#ifdef PMIC_MT6363_DUMMY_LOAD
static struct spmi_device *sdev;

static int mt6363_dlpt_write_reg(u32 reg, u8 val)
{
    return spmi_ext_register_writel(sdev, reg, &val, 1);
}

static int mt6363_dlpt_read_reg(u32 reg, u8 *val)
{
    return spmi_ext_register_readl(sdev, reg, val, 1);
}

static void mt6363_enable_dummy_load(bool en)
{
    if (!sdev) {
        sdev = get_spmi_device(SPMI_MASTER_1, SPMI_SLAVE_4);
        if (!sdev) {
            LTRACEF_LEVEL(CRITICAL, "%s: get spmi device fail\n", __func__);
            return;
        }
    }

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
#endif /* PMIC_MT6363_DUMMY_LOAD */

static void mt6375_enable_dummy_load(bool en)
{
        /* dummy load function is still in main PMIC */
#ifdef PMIC_MT6359_DUMMY_LOAD
        mt6359_enable_dummy_load(en);
#elif defined(PMIC_MT6363_DUMMY_LOAD)
        mt6363_enable_dummy_load(en);
#endif
}

static const struct pmic_dlpt_dev mt6375_dlpt_dev = {
    .imp_conv = mt6375_imp_conv,
    .enable_dummy_load = mt6375_enable_dummy_load,
};

static void mt6375_dlpt_init(void *fdt)
{
    regmap = regmap_get_by_name("mt6375");
    if (!regmap) {
        LTRACEF("failed to get mt6375 regmap\n");
        return;
    }

    pmic_dlpt_device_register(&mt6375_dlpt_dev);
}
LK_INIT_HOOK(mt6375_dlpt, mt6375_dlpt_init, LK_INIT_LEVEL_PLATFORM - 1);

void do_ptim_gauge(unsigned int *vbat, unsigned int *ibat)
{
    int i, j;
    u32 ptim_vbat = 0, ptim_ibat = 0;
    u32 vbats[PTIM_AVG_SIZE] = {0};
    u32 ibats[PTIM_AVG_SIZE] = {0};

    for (i = 0; i < PTIM_AVG_SIZE; i++) {
        mt6375_imp_conv(&ptim_vbat, &ptim_ibat);

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
