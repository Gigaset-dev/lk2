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
#include <errno.h>
#include <malloc.h>
#include <platform/timer.h>
#include <regmap.h>
#include <sys/types.h>
#include <trace.h>

#include "mtk_charger_intf.h"

#define MT6375_DRV_VERSION      "1.0.0"

#define LOCAL_TRACE     0

#ifndef DIV_ROUND_UP
#define DIV_ROUND_UP(n, d) (((n) + (d) - 1) / (d))
#endif

#define MT6375_VID                 0x70
#define MT6375_REV_E1              0x01
#define MT6375_REV_E2              0x02

#define MT6375_REG_DEV_INFO        0x100
#define MT6375_REG_TM_PAS_CODE1    0x107
#define MT6375_REG_CHG_HD_PP5      0x11F
#define MT6375_REG_CHG_TOP1        0x120
#define MT6375_REG_CHG_TOP2        0x121
#define MT6375_REG_CHG_AICR        0x122
#define MT6375_REG_CHG_MIVR        0x123
#define MT6375_REG_CHG_PREC        0x124
#define MT6375_REG_CHG_VCHG        0x125
#define MT6375_REG_CHG_ICHG        0x126
#define MT6375_REG_CHG_EOC         0x128
#define MT6375_REG_CHG_WDT         0x12A
#define MT6375_REG_CHG_STAT        0x134
#define MT6375_REG_CHG_DUMY0       0x135
#define MT6375_REG_CHG_HD_BUCK5    0x140
#define MT6375_REG_ADC_CONFG1      0x1A4
#define MT6375_REG_ADC_CONFG3      0x1A6
#define MT6375_REG_ADC_EN1         0x1A7
#define MT6375_REG_ADC_EN2         0x1A8
#define MT6375_REG_ADC_CH1_RPT_H   0x1AA
#define MT6375_REG_ADC_IRQ         0x1D5
#define MT6375_REG_CHG_STAT0       0x1E0
#define MT6375_REG_CHG_STAT1       0x1E1

#define MT6375_MSK_VID           0xF0
#define MT6375_MSK_CHIP_REV      0x0F
#define MT6375_MSK_CHG_EN        BIT(0xFF, 0)
#define MT6375_MSK_BUCK_EN       BIT(0xFF, 1)
#define MT6375_MSK_PP_PG_FLAG    BIT(0xFF, 7)
#define MT6375_MSK_ILIM_EN       BIT(0xFF, 7)
#define MT6375_MSK_AICR          0x7F
#define MT6375_MSK_MIVR          0x7F
#define MT6375_MSK_IPREC         0x1F
#define MT6375_MSK_CV            0x7F
#define MT6375_MSK_CC            0x3F
#define MT6375_MSK_WDT_RST       BIT(0xFF, 2)
#define MT6375_MSK_WDT_EN        BIT(0xFF, 3)
#define MT6375_MSK_IC_STAT       0x0F
#define MT6375_MSK_IEOC          0xF0
#define MT6375_MSK_CLK_FREQ      0xC0
#define MT6375_MSK_COMP_CLAMP    0x03
#define MT6375_MSK_BUCK_RAMPOFT  0xC0
#define MT6375_MSK_PPSNS_MAIN    0x1F
#define MT6375_MSK_ADC_DONEI     BIT(0xFF, 4)
#define MT6375_MSK_ADCEN         BIT(0xFF, 7)
#define MT6375_MSK_ADC_ONESHOT_SEL    0xF0
#define MT6375_ADC_CHAN_CHGVINDIV5    1

static int aicr = 500;
static struct regmap *regmap;
static u8 rev_id;

enum mt6375_chg_reg_field {
    /* MT6375_REG_CHG_TOP1 */
    F_CHG_EN, F_BUCK_EN,
    /* MT6375_REG_CHG_TOP2 */
    F_CLK_FREQ,
    /* MT6375_REG_CHG_AICR */
    F_ILIM_EN, F_IAICR,
    /* MT6375_REG_CHG_MIVR */
    F_VMIVR,
    /* MT6375_REG_CHG_IPREC */
    F_IPREC,
    /* MT6375_REG_CHG_VCHG */
    F_CV,
    /* MT6375_REG_CHG_ICHG */
    F_CC,
    /* MT6375_REG_CHG_EOC */
    F_IEOC,
    /* MT6375_REG_CHG_WDT */
    F_WDT_RST, F_WDT_EN,
    /* MT6375_REG_CHG_STAT */
    F_IC_STAT,
    F_MAX,
};

struct mt6375_chg_range {
    u32 min;
    u32 max;
    u32 step;
    u32 offset;
    const u32 *table;
    u32 num_table;
    bool round_up;
};

#define REG_FIELD(_reg, _msk) \
{ \
    .reg = _reg, \
    .msk = _msk, \
}

static const struct {
    u16 reg;
    u8 msk;
} mt6375_chg_reg_fields[F_MAX] = {
    [F_CHG_EN] = REG_FIELD(MT6375_REG_CHG_TOP1, MT6375_MSK_CHG_EN),
    [F_BUCK_EN] = REG_FIELD(MT6375_REG_CHG_TOP1, MT6375_MSK_BUCK_EN),
    [F_IAICR] = REG_FIELD(MT6375_REG_CHG_AICR, MT6375_MSK_AICR),
    [F_ILIM_EN] = REG_FIELD(MT6375_REG_CHG_AICR, MT6375_MSK_ILIM_EN),
    [F_VMIVR] = REG_FIELD(MT6375_REG_CHG_MIVR, MT6375_MSK_MIVR),
    [F_IPREC] = REG_FIELD(MT6375_REG_CHG_PREC, MT6375_MSK_IPREC),
    [F_CV] = REG_FIELD(MT6375_REG_CHG_VCHG, MT6375_MSK_CV),
    [F_CC] = REG_FIELD(MT6375_REG_CHG_ICHG, MT6375_MSK_CC),
    [F_IEOC] = REG_FIELD(MT6375_REG_CHG_EOC, MT6375_MSK_IEOC),
    [F_WDT_RST] = REG_FIELD(MT6375_REG_CHG_WDT, MT6375_MSK_WDT_RST),
    [F_WDT_EN] = REG_FIELD(MT6375_REG_CHG_WDT, MT6375_MSK_WDT_EN),
    [F_IC_STAT] = REG_FIELD(MT6375_REG_CHG_STAT, MT6375_MSK_IC_STAT),
};

#define MT6375_CHG_RANGE(_min, _max, _step, _offset, _ru) \
{ \
    .min = _min, \
    .max = _max, \
    .step = _step, \
    .offset = _offset, \
    .round_up = _ru, \
}

static const struct mt6375_chg_range mt6375_chg_ranges[F_MAX] = {
    [F_IAICR] = MT6375_CHG_RANGE(100, 3225, 25, 2, false),
    [F_VMIVR] = MT6375_CHG_RANGE(3900, 13400, 100, 0, true),
    [F_IPREC] = MT6375_CHG_RANGE(100, 1000, 50, 1, false),
    [F_CV] = MT6375_CHG_RANGE(3900, 4710, 10, 0, false),
    [F_CC] = MT6375_CHG_RANGE(300, 3150, 50, 6, false),
    [F_IEOC] = MT6375_CHG_RANGE(100, 800, 50, 1, false),
};

static inline bool mt6375_chg_is_ranged_field(enum mt6375_chg_reg_field fd)
{
    switch (fd) {
    case F_IAICR:
    case F_VMIVR:
    case F_IPREC:
    case F_CV:
    case F_CC:
    case F_IEOC:
        return true;
    default:
        return false;
    }
}

static inline u32 ffs(u32 x)
{
    int r = 1;

    if (!x)
        return 0;
    if (!(x & 0xffff)) {
        x >>= 16;
        r += 16;
    }
    if (!(x & 0xff)) {
        x >>= 8;
        r += 8;
    }
    if (!(x & 0xf)) {
        x >>= 4;
        r += 4;
    }
    if (!(x & 3)) {
        x >>= 2;
        r += 2;
    }
    if (!(x & 1)) {
        x >>= 1;
        r += 1;
    }
    return r;
}

static inline u8 mt6375_chg_val_toreg(const struct mt6375_chg_range *range,
        u32 val)
{
    u32 i;
    u8 reg;

    if (range->table) {
        if (val <= range->table[0])
            return 0;
        for (i = 1; i < range->num_table - 1; i++) {
            if (val == range->table[i])
                return i;
            if (val > range->table[i] && val < range->table[i + 1])
                return range->round_up ? i + 1 : i;
        }
        return range->num_table - 1;
    }
    if (val <= range->min)
        reg = 0;
    else if (val >= range->max)
        reg = (range->max - range->min) / range->step;
    else if (range->round_up)
        reg = DIV_ROUND_UP(val - range->min, range->step);
    else
        reg = (val - range->min) / range->step;
    return reg + range->offset;
}

static inline u32 mt6375_chg_reg_toval(const struct mt6375_chg_range *range,
        u8 reg)
{
    return range->table ? range->table[reg] :
        range->min + range->step * (reg - range->offset);
}

static inline int mt6375_chg_field_set(enum mt6375_chg_reg_field fd, u32 val)
{
    u8 regval;
    u32 idx = fd;

    if (mt6375_chg_is_ranged_field(idx))
        regval = mt6375_chg_val_toreg(&mt6375_chg_ranges[idx], val);
    else
        regval = val;
    regval <<= ffs(mt6375_chg_reg_fields[idx].msk) - 1;
    return regmap_update_bits(regmap, mt6375_chg_reg_fields[idx].reg,
            mt6375_chg_reg_fields[idx].msk, regval);
}

static inline int mt6375_chg_field_get(enum mt6375_chg_reg_field fd, u32 *val)
{
    int ret;
    u32 regval = 0, idx = fd;

    ret = regmap_read(regmap, mt6375_chg_reg_fields[idx].reg, &regval);
    if (ret < 0)
        return ret;
    regval = (regval & mt6375_chg_reg_fields[idx].msk) >>
        (ffs(mt6375_chg_reg_fields[idx].msk) - 1);
    if (mt6375_chg_is_ranged_field(idx))
        *val = mt6375_chg_reg_toval(&mt6375_chg_ranges[idx], regval);
    else
        *val = regval;
    return 0;
}

static int mt6375_check_devid(void)
{
    int ret;
    u32 val = 0;
    u8 vid;

    ret = regmap_read(regmap, MT6375_REG_DEV_INFO, &val);
    if (ret < 0)
        return ret;
    vid = val & MT6375_MSK_VID;
    if (vid != MT6375_VID) {
        LTRACEF("%s vendor id 0x%X is incorrect\n", __func__, vid);
        return ERR_NOT_VALID;
    }
    rev_id = val & MT6375_MSK_CHIP_REV;
    LTRACEF("%s rev=0x%x\n", __func__, rev_id);
    return 0;
}

static int mt6375_enable_hm(bool en)
{
    return regmap_write(regmap, MT6375_REG_TM_PAS_CODE1, en ? 0x69 : 0);
}

static int mt6375_enable_ilim(bool en)
{
    LTRACEF("%s en=%d\n", __func__, en);
    return mt6375_chg_field_set(F_ILIM_EN, en);
}

static int mt6375_set_cv(u32 mV)
{
    LTRACEF("%s cv=%d\n", __func__, mV);
    return mt6375_chg_field_set(F_CV, mV);
}

static int mt6375_set_iprec(u32 mA)
{
    LTRACEF("%s iprec=%d\n", __func__, mA);
    return mt6375_chg_field_set(F_IPREC, mA);
}

static inline int mt6375_config_adc_oneshot(u8 chan)
{
    int ret;
    u32 val;

    ret = regmap_clr_bits(regmap, MT6375_REG_ADC_CONFG1, MT6375_MSK_ADCEN);
    if (ret < 0)
        return ret;
    val = chan << (ffs(MT6375_MSK_ADC_ONESHOT_SEL) - 1);
    ret = regmap_update_bits(regmap, MT6375_REG_ADC_CONFG3,
                             MT6375_MSK_ADC_ONESHOT_SEL, val);
    if (ret < 0)
        return ret;

    return regmap_set_bits(regmap, MT6375_REG_ADC_CONFG1, MT6375_MSK_ADCEN);
}

static inline int mt6375_get_adc_report(u8 chan, u32 *val)
{
    int ret;
    u16 reg = MT6375_REG_ADC_CH1_RPT_H + (chan - 1) * 2;
    u8 rpt[2] = { 0 };

    ret = regmap_bulk_read(regmap, reg, rpt, 2);
    if (ret < 0)
        return ret;
    switch (chan) {
    case MT6375_ADC_CHAN_CHGVINDIV5:
        *val = ((rpt[0] << 8) | rpt[1]) * 6250;
        break;
    default:
        *val = 0;
        break;
    }
    return 0;
}

struct init_table {
    u32 addr;
    u8 mask;
    u8 buck;
};

static const struct init_table mt6375_e1_init_tbl[] = {
    { 0x121, 0xC0, 0x01 },
    { 0x135, 0x03, 0x00 },
    { 0x140, 0xC0, 0x01 },
    { 0x11F, 0x1F, 0x02 },
};

static const struct init_table mt6375_e2_init_tbl[] = {
    { 0x121, 0xC0, 0x01 },
    { 0x135, 0x03, 0x00 },
    { 0x140, 0xC0, 0x01 },
};

static int mt6375_set_buck_param(void)
{
    const struct init_table *init_tbl;
    int i, ret, tbl_size;
    u8 val;

    if (rev_id == MT6375_REV_E1) {
        init_tbl = mt6375_e1_init_tbl;
        tbl_size = countof(mt6375_e1_init_tbl);
    } else {
        init_tbl = mt6375_e2_init_tbl;
        tbl_size = countof(mt6375_e2_init_tbl);
    }

    ret = mt6375_enable_hm(true);
    if (ret < 0)
        return ret;
    for (i = 0; i < tbl_size; i++) {
        val = init_tbl[i].buck << (ffs(init_tbl[i].mask) - 1);
        ret = regmap_update_bits(regmap,
                                 init_tbl[i].addr,
                                 init_tbl[i].mask,
                                 val);
        if (ret < 0) {
            LTRACEF("failed to set reg0x%X=0x%X, msk0x%X\n", init_tbl[i].addr,
                    val, init_tbl[i].mask);
        }
    }
    mt6375_enable_hm(false);
    return ret;
}

static int mt6375_chg_init_setting(void)
{
    int ret;
    struct {
        u32 iprec;
        u32 ichg;
        u32 aicr;
        u32 mivr;
        u32 cv;
    } init_data = {
        .iprec = 150,
        .ichg = 500,
        .aicr = 500,
        .mivr = 4500,
        .cv = 4350,
    };

    LTRACEF("%s\n", __func__);
    ret = mt6375_set_cv(init_data.cv);
    if (ret < 0) {
        LTRACEF("%s failed to set cv\n", __func__);
        return ret;
    }
    ret = mtk_charger_set_aicr(init_data.aicr);
    if (ret < 0) {
        LTRACEF("%s failed to set aicr\n", __func__);
        return ret;
    }
    ret = mt6375_enable_ilim(false);
    if (ret < 0) {
        LTRACEF("%s failed to disable ilim\n", __func__);
        return ret;
    }
    ret = mtk_charger_set_mivr(init_data.mivr);
    if (ret < 0) {
        LTRACEF("%s failed to set mivr\n", __func__);
        return ret;
    }
    ret = mt6375_set_iprec(init_data.iprec);
    if (ret < 0) {
        LTRACEF("%s failed to set iprec\n", __func__);
        return ret;
    }
    ret = mtk_charger_set_ichg(init_data.ichg);
    if (ret < 0) {
        LTRACEF("%s failed to set ichg\n", __func__);
        return ret;
    }
    ret = mt6375_set_buck_param();
    if (ret < 0)
        LTRACEF("%s failed to set buck param\n", __func__);
    return ret;
}

int mtk_charger_set_ichg(u32 mA)
{
    LTRACEF("%s ichg=%d\n", __func__, mA);
    return mt6375_chg_field_set(F_CC, mA);
}

int mtk_charger_enable_charging(bool en)
{
    LTRACEF("%s en=%d\n", __func__, en);
    return mt6375_chg_field_set(F_CHG_EN, en);
}

int mtk_charger_get_ichg(u32 *mA)
{
    return mt6375_chg_field_get(F_CC, mA);
}

int mtk_charger_get_aicr(u32 *mA)
{
    return mt6375_chg_field_get(F_IAICR, mA);
}

int mtk_charger_set_aicr(u32 mA)
{
    int ret;

    LTRACEF("%s aicr=%d\n", __func__, mA);
    ret = mt6375_chg_field_set(F_IAICR, mA);
    if (ret < 0)
        return ret;
    aicr = mA;
    return 0;
}

int mtk_charger_get_vbus(u32 *vbus)
{
    int i, ret;
    const int max_retry_cnt = 5;
    u32 val = 0;

    LTRACEF("%s\n", __func__);
    /* config onshot channel and irq reported channel */
    ret = mt6375_config_adc_oneshot(MT6375_ADC_CHAN_CHGVINDIV5);
    if (ret < 0)
        return ret;

    for (i = 0; i < max_retry_cnt; i++) {
        spin(1000);
        /* read adc conversion done event */
        ret = regmap_read(regmap, MT6375_REG_ADC_CONFG3, &val);
        if (ret < 0)
            continue;
        val &= MT6375_MSK_ADC_ONESHOT_SEL;
        if (!val)
            break;
    }
    if (i == max_retry_cnt) {
        LTRACEF("%s failed to start vbus adc\n", __func__);
        return ERR_BUSY;
    }
    /* read adc report */
    ret = mt6375_get_adc_report(MT6375_ADC_CHAN_CHGVINDIV5, vbus);
    if (ret < 0)
        LTRACEF("%s failed to get adc vbus report\n", __func__);
    return ret;
}

int mtk_charger_set_mivr(u32 mV)
{
    LTRACEF("%s mivr=%d\n", __func__, mV);
    return mt6375_chg_field_set(F_VMIVR, mV);
}

int mtk_charger_enable_power_path(bool en)
{
    LTRACEF("%s en=%d\n", __func__, en);
    return mt6375_chg_field_set(F_BUCK_EN, en);
}

int mtk_charger_reset_pumpx(bool en)
{
    return mt6375_chg_field_set(F_IAICR, en ? 100 : aicr);
}

int mtk_charger_enable_wdt(bool en)
{
    LTRACEF("%s en=%d\n", __func__, en);
    return mt6375_chg_field_set(F_WDT_EN, en);
}

int mtk_charger_reset_wdt(void)
{
    return mt6375_chg_field_set(F_WDT_RST, 1);
}

#define DUMP_REG_BUF_SIZE    1024
int mtk_charger_dump_register(void)
{
    int ret;
    u32 i, val, regval;
    char *buf = malloc(DUMP_REG_BUF_SIZE);

    static struct {
        const char *name;
        const char *unit;
        enum mt6375_chg_reg_field fd;
    } settings[] = {
        { .fd = F_CC, .name = "CC", .unit = "mA" },
        { .fd = F_IAICR, .name = "AICR", .unit = "mA" },
        { .fd = F_VMIVR, .name = "MIVR", .unit = "mV" },
        { .fd = F_CV, .name = "CV", .unit = "mV" },
    };
    static struct {
        const u16 reg;
        const char *name;
    } regs[] = {
        { .reg = MT6375_REG_CHG_STAT0, .name = "CHG_STAT0" },
        { .reg = MT6375_REG_CHG_STAT1, .name = "CHG_STAT1" },
        { .reg = MT6375_REG_CHG_TOP1, .name = "CHG_TOP1" },
        { .reg = MT6375_REG_CHG_TOP2, .name = "CHG_TOP2" },
        { .reg = MT6375_REG_CHG_EOC, .name = "CHG_EOC" },
    };

    if (!buf)
        return -ENOMEM;
    memset(buf, 0, DUMP_REG_BUF_SIZE);
    for (i = 0; i < countof(settings); i++) {
        ret = mt6375_chg_field_get(settings[i].fd, &val);
        if (ret < 0) {
            LTRACEF("failed to get %s\n", settings[i].name);
            goto out;
        }
        if (i == countof(settings) - 1)
            ret = snprintf(buf + strlen(buf), DUMP_REG_BUF_SIZE, "%s = %d%s\n",
                           settings[i].name, val, settings[i].unit);
        else
            ret = snprintf(buf + strlen(buf), DUMP_REG_BUF_SIZE, "%s = %d%s, ",
                           settings[i].name, val, settings[i].unit);
        if (ret < 0)
            LTRACEF("failed to copy string to buf\n");
    }

    for (i = 0; i < countof(regs); i++) {
        ret = regmap_read(regmap, regs[i].reg, &regval);
        if (ret < 0) {
            LTRACEF("failed to read %s\n", regs[i].name);
            goto out;
        }
        if (i == countof(regs) - 1)
            ret = snprintf(buf + strlen(buf), DUMP_REG_BUF_SIZE, "%s = 0x%02X\n",
                           regs[i].name, regval);
        else
            ret = snprintf(buf + strlen(buf), DUMP_REG_BUF_SIZE, "%s = 0x%02X, ",
                           regs[i].name, regval);
        if (ret < 0)
            LTRACEF("failed to copy string to buf\n");
    }

    LTRACEF("%s %s", __func__, buf);
out:
    free(buf);
    return ret;
}

int mtk_charger_check_battery_plugout_once(bool *plugout_once)
{
    struct regmap *rmap;
    int ret;
    u32 regval = 0;

    rmap = regmap_get_by_name("mt6375");
    if (!rmap)
        return ERR_NOT_VALID;

    ret = regmap_read(rmap, MT6375_REG_CHG_TOP1, &regval);
    if (ret < 0)
        return ret;
    *plugout_once = regval & MT6375_MSK_PP_PG_FLAG ? false : true;
    return 0;
}

int mtk_charger_init(void)
{
    int ret = 0;

    LTRACEF("%s %s\n", __func__, MT6375_DRV_VERSION);

    regmap = regmap_get_by_name("mt6375");
    if (!regmap)
        return ERR_NOT_VALID;

    ret = mt6375_check_devid();
    if (ret < 0)
        return ret;

    ret = mt6375_chg_init_setting();
    if (ret < 0) {
        LTRACEF("%s failed to init setting\n", __func__);
        return ret;
    }
    return 0;
}
