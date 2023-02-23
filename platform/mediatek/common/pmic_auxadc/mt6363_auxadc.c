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
#include <pmic_auxadc.h>
#include <spmi_common.h>
#include <string.h>
#include <trace.h>

#define LOCAL_TRACE             1

#define PMIC_AUXADC_ADC_OUT_CH0_L_ADDR     (0x1088)
#define PMIC_AUXADC_ADC_OUT_CH3_L_ADDR     (0x108e)
#define PMIC_AUXADC_ADC_OUT_CH4_L_ADDR     (0x1090)
#define PMIC_AUXADC_ADC_OUT_CH11_L_ADDR    (0x109e)
#define PMIC_AUXADC_ADC_OUT_CH14_L_ADDR    (0x10d8)
#define PMIC_AUXADC_ADC_OUT_CH4_BY_THR1_L_ADDR  (0x10c4)
#define PMIC_AUXADC_ADC_OUT_CH4_BY_THR2_L_ADDR  (0x10c6)
#define PMIC_AUXADC_ADC_OUT_CH4_BY_THR3_L_ADDR  (0x10c8)
#define MT6363_AUXADC_RQST0                (0x1108)
#define MT6363_AUXADC_RQST1                (0x1109)
#define MT6363_AUXADC_RQST3                (0x110c)

#define AUXADC_RDY_BIT          (1 << 15)
/* 0.385us(2.89MHz) x 48T (SPL(ch0=35T)+ADC_compare(12T)+sync(1T)) ~= 19us */
#define AUXADC_AVG_TIME         19
#define AUXADC_POLL_TIME        100
#define AUXADC_TIMEOUT_US       32000
#define VOLT_FULL               1840

#define AUXADC_CHAN_SPEC(_chan, r0, r1, avg_num, _res, \
                         _enable_reg, _enable_mask, _value_reg) \
{ \
    .channel = AUXADC_CHAN_##_chan, \
    .hw_info = { \
        .ref_volt = VOLT_FULL, \
        .min_time = (avg_num) * AUXADC_AVG_TIME, \
        .max_time = AUXADC_TIMEOUT_US, \
        .poll_time = AUXADC_POLL_TIME, \
        .ratio = { r0, r1 }, \
        .enable_reg = _enable_reg, \
        .enable_mask = _enable_mask, \
        .ready_reg = _value_reg, \
        .ready_mask = AUXADC_RDY_BIT, \
        .value_reg = _value_reg, \
        .res = _res, \
    }, \
}

static const struct auxadc_chan_spec mt6363_auxadc_chan_specs[] = {
    AUXADC_CHAN_SPEC(BATADC, 4, 1, 64, 15, MT6363_AUXADC_RQST0, BIT(0xFF, 0),
                     PMIC_AUXADC_ADC_OUT_CH0_L_ADDR),
    AUXADC_CHAN_SPEC(BAT_TEMP, 3, 2, 32, 12, MT6363_AUXADC_RQST0, BIT(0xFF, 3),
                     PMIC_AUXADC_ADC_OUT_CH3_L_ADDR),
    AUXADC_CHAN_SPEC(CHIP_TEMP, 1, 1, 32, 12, MT6363_AUXADC_RQST0, BIT(0xFF, 4),
                     PMIC_AUXADC_ADC_OUT_CH4_L_ADDR),
    AUXADC_CHAN_SPEC(VCORE_TEMP, 1, 1, 32, 12, MT6363_AUXADC_RQST3, BIT(0xFF, 0),
                     PMIC_AUXADC_ADC_OUT_CH4_BY_THR1_L_ADDR),
    AUXADC_CHAN_SPEC(VPROC_TEMP, 1, 1, 32, 12, MT6363_AUXADC_RQST3, BIT(0xFF, 1),
                     PMIC_AUXADC_ADC_OUT_CH4_BY_THR2_L_ADDR),
    AUXADC_CHAN_SPEC(VGPU_TEMP, 1, 1, 32, 12, MT6363_AUXADC_RQST3, BIT(0xFF, 2),
                     PMIC_AUXADC_ADC_OUT_CH4_BY_THR3_L_ADDR),
    AUXADC_CHAN_SPEC(VTREF, 3, 2, 32, 12, MT6363_AUXADC_RQST1, BIT(0xFF, 3),
                     PMIC_AUXADC_ADC_OUT_CH11_L_ADDR),
    AUXADC_CHAN_SPEC(VSYSSNS, 3, 1, 64, 15, MT6363_AUXADC_RQST1, BIT(0xFF, 6),
                     PMIC_AUXADC_ADC_OUT_CH14_L_ADDR),
};

static struct spmi_device *sdev;

static int mt6363_auxadc_write_regs(const struct auxadc_device *auxadc_dev,
                                    u32 reg, const u8 *val, size_t len)
{
    return spmi_ext_register_writel(sdev, reg, val, len);
}

static int mt6363_auxadc_read_regs(const struct auxadc_device *auxadc_dev,
                                   u32 reg, u8 *val, size_t len)
{
    return spmi_ext_register_readl(sdev, reg, val, len);
}

static struct auxadc_device mt6363_auxadc_device = {
    .name = "main_pmic",
    .chans = mt6363_auxadc_chan_specs,
    .num_chans = countof(mt6363_auxadc_chan_specs),
    .write_regs = mt6363_auxadc_write_regs,
    .read_regs = mt6363_auxadc_read_regs,
};

static void mt6363_auxadc_init(uint level)
{
    int ret;

    sdev = get_spmi_device(SPMI_MASTER_1, SPMI_SLAVE_4);
    if (!sdev)
        LTRACEF_LEVEL(CRITICAL, "%s: get spmi device fail\n", __func__);

    ret = pmic_auxadc_device_register(&mt6363_auxadc_device);
    if (ret < 0)
        LTRACEF("%s failed to register auxadc device\n", __func__);
}
LK_INIT_HOOK(mt6363_auxadc, mt6363_auxadc_init, LK_INIT_LEVEL_PLATFORM_EARLY + 1);
