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

#define LOCAL_TRACE             0

#define MT6368_PMIC_AUXADC_ADC_OUT_CH12_L_ADDR  (0x10d2)
#define MT6368_PMIC_SDMADC_EXT_THR_SRC_SEL_ADDR (0x11c4)
#define MT6368_AUXADC_RQST1                     (0x1109)

#define AUXADC_RDY_BIT          (1 << 15)
/* 0.385us(2.89MHz) x 42T (SPL(100K=29T)+ADC_compare(12T)+sync(1T)) ~= 16us */
#define AUXADC_AVG_TIME         16
#define AUXADC_POLL_TIME        100
#define AUXADC_TIMEOUT_US       32000
#define VOLT_FULL               1840

#define EXT_THR_SEL_DEF_VALUE   0x80
#define EXT_THR_PURES_SHIFT     3

#define SDMADC_CHAN_SPEC(_chan, src_sel) \
{ \
    .channel = AUXADC_CHAN_##_chan, \
    .hw_info = { \
        .ref_volt = VOLT_FULL, \
        .min_time = 128 * AUXADC_AVG_TIME, \
        .max_time = AUXADC_TIMEOUT_US, \
        .poll_time = AUXADC_POLL_TIME, \
        .ratio = { 1, 1 }, \
        .enable_reg = MT6368_AUXADC_RQST1, \
        .enable_mask = (1 << 4), \
        .ready_reg = MT6368_PMIC_AUXADC_ADC_OUT_CH12_L_ADDR, \
        .ready_mask = AUXADC_RDY_BIT, \
        .value_reg = MT6368_PMIC_AUXADC_ADC_OUT_CH12_L_ADDR, \
        .res = 15, \
    }, \
    .sdmadc_hw_info = { \
        .set_reg =  MT6368_PMIC_SDMADC_EXT_THR_SRC_SEL_ADDR, \
        .cmd = EXT_THR_SEL_DEF_VALUE | src_sel, \
    }, \
}

static const struct auxadc_chan_spec mt6368_sdmadc_chan_specs[] = {
    SDMADC_CHAN_SPEC(VIN1, 1),
    SDMADC_CHAN_SPEC(VIN2, 2),
};

static struct spmi_device *sdev;


static int mt6368_auxadc_write_regs(const struct auxadc_device *auxadc_dev,
                                    u32 reg, const u8 *val, size_t len)
{
    return spmi_ext_register_writel(sdev, reg, val, len);
}

static int mt6368_auxadc_read_regs(const struct auxadc_device *auxadc_dev,
                                   u32 reg, u8 *val, size_t len)
{
    return spmi_ext_register_readl(sdev, reg, val, len);
}

static int mt6368_sdmadc_read(const struct auxadc_device *auxadc_dev,
                              const struct auxadc_chan_spec *chan, int *val,
                              enum sdmadc_pures pures, enum auxadc_val_type type)
{
    int ret;
    u8 wdata;
    u32 regval;
    u32 elapsed = 0;

    if (chan->hw_info.res > AUXADC_RES_MAX)
        return ERR_INVALID_ARGS;

    wdata = chan->sdmadc_hw_info.cmd | (pures << EXT_THR_PURES_SHIFT);
    ret = auxadc_dev->write_regs(auxadc_dev, chan->sdmadc_hw_info.set_reg,
                                 &wdata, 1);
    wdata = chan->hw_info.enable_mask;
    ret |= auxadc_dev->write_regs(auxadc_dev, chan->hw_info.enable_reg,
                                  &wdata, 1);
    if (ret < 0)
        return ret;
    spin(chan->hw_info.min_time);
    elapsed += chan->hw_info.min_time;

    while (1) {
        ret = auxadc_dev->read_regs(auxadc_dev, chan->hw_info.ready_reg,
                                    (u8 *)&regval, 2);
        if (ret < 0)
            goto retry;
        if (regval & chan->hw_info.ready_mask)
            break;
retry:
        if (elapsed > chan->hw_info.max_time)
            return ret < 0 ? ret : ERR_TIMED_OUT;
        spin(chan->hw_info.poll_time);
        elapsed += chan->hw_info.poll_time;
    }
    regval &= BIT_MASK(chan->hw_info.res);
    switch (type) {
    case AUXADC_VAL_RAW:
        *val = regval;
        break;
    case AUXADC_VAL_PROCESSED:
        *val = ((regval * chan->hw_info.ratio[0] * chan->hw_info.ref_volt) /
            chan->hw_info.ratio[1]) >> chan->hw_info.res;
        break;
    }
    LTRACEF("name:%s, channel=%d, SDMADC_CON0=0x%x, adc_raw=0x%x, val=%d\n",
            auxadc_dev->name, chan->channel,
            chan->sdmadc_hw_info.cmd | (pures << EXT_THR_PURES_SHIFT), regval, *val);
    return 0;
}

static struct auxadc_device mt6368_sdmadc_device = {
    .name = "second_pmic_sdmadc",
    .chans = mt6368_sdmadc_chan_specs,
    .num_chans = countof(mt6368_sdmadc_chan_specs),
    .write_regs = mt6368_auxadc_write_regs,
    .read_regs = mt6368_auxadc_read_regs,
    .sdmadc_read = mt6368_sdmadc_read,
};

static void mt6368_sdmadc_init(uint level)
{
    int ret;

    sdev = get_spmi_device(SPMI_MASTER_1, SPMI_SLAVE_5);
    if (!sdev)
        dprintf(CRITICAL, "%s: get spmi device fail\n", __func__);

    ret = pmic_auxadc_device_register(&mt6368_sdmadc_device);
    if (ret < 0)
        dprintf(CRITICAL, "%s failed to register auxadc device\n", __func__);
}
LK_INIT_HOOK(mt6368_sdmadc, mt6368_sdmadc_init, LK_INIT_LEVEL_PLATFORM_EARLY + 1);
