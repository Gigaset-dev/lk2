/*
 * copyright (c) 2021 mediatek inc.
 *
 * use of this source code is governed by a mit-style
 * license that can be found in the license file or at
 * https://opensource.org/licenses/mit
 */

#include <bits.h>
#include <compiler.h>
#include <debug.h>
#include <err.h>
#include <lk/init.h>
#include <pmic_auxadc.h>
#include <regmap.h>
#include <trace.h>

#define LOCAL_TRACE    0

#define AUXADC_OUT_CH3     0x408
#define AUXADC_OUT_CH11    0x40A
#define AUXADC_OUT_CH0     0x410
#define AUXADC_RQST0       0x438
#define VREF_ENMASK        BIT(0xFF, 4)
#define BATON_ENMASK       BIT(0xFF, 3)
#define BATSNS_ENMASK      BIT(0xFF, 0)
#define ADC_OUT_RDY        BIT(0xFF, 7)
#define AUXADC_MAX_TIME    (11*1000)
#define AUXADC_MIN_TIME    1000
#define AUXADC_POLL_TIME   100

#define AUXADC_CHAN_SPEC(_chan, _ratio, _enable_mask, _value_reg, _res) \
{ \
    .channel = AUXADC_CHAN_##_chan, \
    .hw_info = { \
        .ref_volt = 1, \
        .min_time = AUXADC_MIN_TIME, \
        .max_time = AUXADC_MAX_TIME, \
        .poll_time = AUXADC_POLL_TIME, \
        .ratio = { _ratio, 1 }, \
        .enable_reg = AUXADC_RQST0, \
        .enable_mask = _enable_mask, \
        .ready_reg = _value_reg + 1, \
        .ready_mask = ADC_OUT_RDY, \
        .value_reg = _value_reg, \
        .res = _res, \
    }, \
}

static int mt6375_auxadc_read_regs(const struct auxadc_device *auxadc_dev,
                                   u32 reg, u8 *val, size_t len)
{
    struct regmap *regmap = pmic_auxadc_get_drvdata(auxadc_dev);

    return regmap_bulk_read(regmap, reg, val, len);
}

static int mt6375_auxadc_write_regs(const struct auxadc_device *auxadc_dev,
                                    u32 reg, const u8 *val, size_t len)
{
    struct regmap *regmap = pmic_auxadc_get_drvdata(auxadc_dev);

    return regmap_bulk_write(regmap, reg, val, len);
}

static const struct auxadc_chan_spec mt6375_auxadc_chan_specs[] = {
    AUXADC_CHAN_SPEC(BATADC, 7360, BATSNS_ENMASK, AUXADC_OUT_CH0, 15),
    AUXADC_CHAN_SPEC(BAT_TEMP, 2760, BATON_ENMASK, AUXADC_OUT_CH3, 12),
    AUXADC_CHAN_SPEC(VBIF, 2760, VREF_ENMASK, AUXADC_OUT_CH11, 12),
};

static struct auxadc_device mt6375_auxadc_device = {
    .name = "subpmic",
    .chans = mt6375_auxadc_chan_specs,
    .num_chans = countof(mt6375_auxadc_chan_specs),
    .read_regs = mt6375_auxadc_read_regs,
    .write_regs = mt6375_auxadc_write_regs,
};

static void mt6375_auxadc_init(uint level)
{
    int ret;
    struct regmap *regmap;

    regmap = regmap_get_by_name("mt6375");
    if (!regmap)
        LTRACEF("%s failed to get regmap\n", __func__);

    pmic_auxadc_set_drvdata(&mt6375_auxadc_device, regmap);
    ret = pmic_auxadc_device_register(&mt6375_auxadc_device);
    if (ret < 0)
        LTRACEF("%s failed to register auxadc device\n", __func__);
}
LK_INIT_HOOK(mt6375_auxadc, mt6375_auxadc_init, LK_INIT_LEVEL_PLATFORM_EARLY);
