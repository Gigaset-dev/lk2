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
#include <libfdt.h>
#include <mt_gauge.h>
#include <mtk_battery.h>
#include <regmap.h>
#include <stdbool.h>
#include <sys/types.h>
#include <trace.h>

#define LOCAL_TRACE 1

#ifndef mdelay
#define mdelay(x)       spin((x) * 1000)
#endif

#define BM_TOP_RST_CON2          0x221
#define FGADC_GAINERR_CAL_OUT    0x258
#define FGADC_ANA_ELR0     0x25B
#define FGADC_ANA_ELR1     0x25D
#define FGADC_ANA_ELR2     0x25F
#define FGADC_ANA_ELR3     0x261
#define FGADC_ANA_ELR4     0x263
#define FGADC_CON1         0x26E
#define FGADC_CON2         0x26F
#define FGADC_CON3         0x270
#define FGADC_CAR_CON0     0x278
#define FGADC_R_CON0       0x2E5
#define FGADC_CUR_CON0     0x2E7
#define FGADC_GAIN_CON0    0x2F6

#define RG_FGADC_RST_SRC_SEL_MASK    BIT(0xFF, 0)
#define FG_SW_READ_PRE_MASK          BIT(0xFF, 0)
#define FG_SW_CLEAR_MASK             BIT(0xFF, 3)
#define FG_LATCHDATA_ST_MASK         BIT(0xFF, 7)
#define FG_ON_MASK                   BIT(0xFF, 0)
#define FG_CHARGE_RST_MASK           BIT(0xFF, 2)
#define FG_CAL_MASK                  0x0C
#define FORCE_AUTO_CAL               0x04
#define AUTO_CAL                     0x08
#define FGADC_CURRENT_RETRY_MAX      1000

static bool b_is_charging;
static struct regmap *regmap;

enum {
        R_FG_5MOHM,
        R_FG_2MOHM,
        R_FG_1MOHM,
        R_FG_0_5MOHM,
        R_FG_MAX,
};

/* unit is 0.1mohm */
static const u32 r_fg_values[R_FG_MAX] = {
    50, 20, 10, 5,
};

static u8 find_r_fg_sel(struct fuel_gauge_custom_data *fg_cust_data)
{
    int i;

    for (i = R_FG_5MOHM; i < R_FG_MAX; i++) {
        if ((u32)fg_cust_data->r_fg_value == r_fg_values[i])
            return i;
    }
    return R_FG_5MOHM;
}

static bool is_r_fg_need_scaling(struct fuel_gauge_custom_data *fg_cust_data)
{
    int i;

    for (i = R_FG_5MOHM; i < R_FG_MAX; i++) {
        if ((u32)fg_cust_data->r_fg_value == r_fg_values[i])
            return false;
    }
    return true;
}

bool fg_charging_state(void)
{
    return b_is_charging;
}

static inline int mt6375_get_regmap(void)
{
    if (!regmap) {
        regmap = regmap_get_by_name("mt6375");
        if (!regmap)
            return ERR_NOT_VALID;
    }
    return 0;
}

static int mt6375_config_rfg_gain_error(void)
{
    int ret;
    u16 efuse_gainerr = 0, gainerr = 0;
    u32 fixed_car, _r_fg_sel = 0, r_fg_sel;
    struct fuel_gauge_custom_data *fg_cust_data = get_battery_cust_data();
    static const u16 gainerr_reg[R_FG_MAX] = {
        FGADC_ANA_ELR0,
        FGADC_ANA_ELR1,
        FGADC_ANA_ELR2,
        FGADC_ANA_ELR3,
    };

    ret = mt6375_get_regmap();
    if (ret < 0)
        return ret;

    r_fg_sel = find_r_fg_sel(fg_cust_data);
    /*
     * Read gain error and gain error selector
     * to check whether reset happened or not
     */
    if (fg_cust_data->curr_measure_20a)
        ret = regmap_bulk_read(regmap, gainerr_reg[R_FG_2MOHM],
                               (u8 *)&efuse_gainerr, 2);
    else
        ret = regmap_bulk_read(regmap, gainerr_reg[r_fg_sel],
                               (u8 *)&efuse_gainerr, 2);
    if (ret < 0) {
        LTRACEF("%s failed to read efuse gainerr\n", __func__);
        goto config;
    }
    ret = regmap_bulk_read(regmap, FGADC_GAIN_CON0, (u8 *)&gainerr, 2);
    if (ret < 0) {
        LTRACEF("%s failed to read gainerr\n", __func__);
        goto config;
    }
    LTRACEF("%s gainerr(%d), efuse gainerr(%d)\n", __func__, gainerr,
            efuse_gainerr);
    if (gainerr != efuse_gainerr)
        goto config;

    ret = regmap_read(regmap, FGADC_ANA_ELR4, &_r_fg_sel);
    if (ret < 0) {
        LTRACEF("%s failed to read rfg selector\n", __func__);
        goto config;
    }
    LTRACEF("%s rfg selector (%d,%d)\n", __func__, _r_fg_sel, r_fg_sel);
    if (fg_cust_data->curr_measure_20a || r_fg_sel == R_FG_2MOHM) {
        if (_r_fg_sel != R_FG_5MOHM)
            goto config;
    } else if (r_fg_sel != _r_fg_sel)
        goto config;
    LTRACEF("%s no need to config rfg gainerr\n", __func__);
    return 0;

config:
    /* read CAR before selecting different gainerr */
    ret = regmap_bulk_read(regmap, FGADC_CAR_CON0, (u8 *)&fixed_car, 4);
    if (ret < 0)
        LTRACEF("%s failed to read CAR\n", __func__);

    /* Apply corresponding gain error and resistor selector */
    ret = regmap_bulk_write(regmap, FGADC_GAIN_CON0, (u8 *)&efuse_gainerr, 2);
    if (ret < 0) {
        LTRACEF("%s failed to set gainerr\n", __func__);
        return ret;
    }

    if (fg_cust_data->curr_measure_20a || r_fg_sel == R_FG_2MOHM)
        ret = regmap_write(regmap, FGADC_ANA_ELR4, R_FG_5MOHM);
    else
        ret = regmap_write(regmap, FGADC_ANA_ELR4, r_fg_sel);
    if (ret < 0) {
        LTRACEF("%s failed to set rfg selector\n", __func__);
        return ret;
    }

    LTRACEF("%s FG ON = 0\n", __func__);
    ret = regmap_clr_bits(regmap, FGADC_CON1, FG_ON_MASK);
    if (ret < 0) {
        LTRACEF("%s failed to disable FG ON\n", __func__);
        return ret;
    }

    ret = regmap_set_bits(regmap, FGADC_CON2, FG_CHARGE_RST_MASK);
    if (ret < 0) {
        LTRACEF("%s failed to set reset CAR\n", __func__);
        return ret;
    }

    ret = regmap_clr_bits(regmap, FGADC_CON2, FG_CHARGE_RST_MASK);
    if (ret < 0) {
        LTRACEF("%s failed to clr reset CAR\n", __func__);
        return ret;
    }

    spin(200);

    LTRACEF("%s FG ON = 1\n", __func__);
    ret = regmap_set_bits(regmap, FGADC_CON1, FG_ON_MASK);
    if (ret < 0) {
        LTRACEF("%s failed to enable FG ON\n", __func__);
        return ret;
    }

    mdelay(33);

    LTRACEF("%s successfully\n", __func__);
    return 0;
}

static bool fg_adc_is_data_ready(void)
{
    int ret;
    u32 val = 0;

    ret = mt6375_get_regmap();
    if (ret < 0)
        return ret;
    ret = regmap_read(regmap, FGADC_CON2, &val);
    return (ret < 0) ? false : (val & FG_LATCHDATA_ST_MASK);
}

void fgauge_read_IM_current(void *data)
{
    u16 uvalue16 = 0;
    signed int dvalue = 0;
    signed int ori_curr = 0;
    signed int curr_rfg = 0;
    long long Temp_Value = 0;
    struct fuel_gauge_custom_data *fg_cust_data;

    fg_cust_data = get_battery_cust_data();

    if (mt6375_get_regmap() < 0)
        return;
    regmap_bulk_read(regmap, FGADC_R_CON0, (u8 *)&uvalue16, 2);

    /*calculate the real world data    */
    dvalue = (unsigned int) uvalue16;
    if (dvalue == 0) {
        Temp_Value = (long long) dvalue;
        b_is_charging = false;
    } else if (dvalue > 32767) {
        /* > 0x8000 */
        Temp_Value = (long long) (dvalue - 65535);
        Temp_Value = Temp_Value - (Temp_Value * 2);
        b_is_charging = false;
    } else {
        Temp_Value = (long long) dvalue;
        b_is_charging = true;
    }

    Temp_Value = Temp_Value * fg_cust_data->unit_fgcurrent;
    Temp_Value = Temp_Value/100000;
    dvalue = (unsigned int) Temp_Value;

    ori_curr = dvalue;
    curr_rfg = dvalue;

    /* Auto adjust value */
    if (is_r_fg_need_scaling(fg_cust_data)) {
        dvalue = (dvalue * r_fg_values[R_FG_5MOHM]) / fg_cust_data->r_fg_value;
        curr_rfg = dvalue;
    }

    dvalue = ((dvalue * fg_cust_data->car_tune_value) / 1000);

    if (b_is_charging == true) {
        LTRACEF("[%s](charging)FG_CURRENT:0x%x,curr:[%d,%d,%d] mA, Rfg:%d ratio:%d\n",
            __func__, uvalue16, ori_curr, curr_rfg, dvalue, fg_cust_data->r_fg_value,
            fg_cust_data->car_tune_value);

    } else {
        LTRACEF("[%s](discharg)FG_CURRENT:0x%x,curr:[%d,%d,%d] mA, Rfg:%d ratio:%d\n",
            __func__, uvalue16, ori_curr, curr_rfg, dvalue, fg_cust_data->r_fg_value,
            fg_cust_data->car_tune_value);
    }

    *(signed int *) (data) = dvalue;
}

int fgauge_get_current(bool *fg_is_charging, int *data)
{
    int rv;
    unsigned short uvalue16 = 0;
    signed int dvalue = 0;
    signed int curr_rfg = 0;
    signed int ori_curr = 0;
    int m = 0;
    long long Temp_Value = 0;
    unsigned int ret = 0;
    struct fuel_gauge_custom_data *fg_cust_data;

    fg_cust_data = get_battery_cust_data();

    LTRACEF("[fgauge_read_current],cartune:%d,%d,%d\n",
        fg_cust_data->car_tune_value, fg_cust_data->r_fg_value,
        fg_cust_data->unit_fgcurrent);

    rv = mt6375_get_regmap();
    if (rv < 0)
        return rv;

    regmap_set_bits(regmap, FGADC_CON3, FG_SW_READ_PRE_MASK);

    m = 0;

    while (!fg_adc_is_data_ready()) {
        m++;
        if (m > 1000) {
            LTRACEF("[%s]fg_adc_is_data_ready timeout 1 !%d\n", __func__, ret);
            break;
        }
    }

    regmap_bulk_read(regmap, FGADC_CUR_CON0, (u8 *)&uvalue16, 2);
    regmap_set_bits(regmap, FGADC_CON3, FG_SW_CLEAR_MASK);
    regmap_clr_bits(regmap, FGADC_CON3, FG_SW_READ_PRE_MASK);

    m = 0;
    while (fg_adc_is_data_ready()) {
        m++;
        if (m > 1000) {
            LTRACEF("[%s] fg_adc_is_data_ready 2! ret:%d\n", __func__, ret);
            break;
        }
    }

    regmap_clr_bits(regmap, FGADC_CON3, FG_SW_CLEAR_MASK);

    /*calculate the real world data    */
    dvalue = (unsigned int) uvalue16;
    if (dvalue == 0) {
        Temp_Value = (long long) dvalue;
        *fg_is_charging = false;
    } else if (dvalue > 32767) {
        /* > 0x8000 */
        Temp_Value = (long long) (dvalue - 65535);
        Temp_Value = Temp_Value - (Temp_Value * 2);
        *fg_is_charging = false;
    } else {
        Temp_Value = (long long) dvalue;
        *fg_is_charging = true;
    }

    Temp_Value = Temp_Value * fg_cust_data->unit_fgcurrent;
    Temp_Value = Temp_Value / 100000;
    dvalue = (unsigned int) Temp_Value;

    ori_curr = dvalue;
    curr_rfg = dvalue;

    /* Auto adjust value */
    if (is_r_fg_need_scaling(fg_cust_data)) {
        dvalue = (dvalue * r_fg_values[R_FG_5MOHM]) / fg_cust_data->r_fg_value;
        curr_rfg = dvalue;
    }

    dvalue = ((dvalue * fg_cust_data->car_tune_value) / 1000);

    *data = dvalue;
    LTRACEF("[%s]is_charge:%d Ori_curr:0x%x,curr:[%d,%d,Final:%d] Rfg:%d ratio:%d\n",
        __func__,
        *fg_is_charging, uvalue16, ori_curr, curr_rfg, dvalue,
        fg_cust_data->r_fg_value, fg_cust_data->car_tune_value);

    return 0;
}

int dis_gm3_src_sel(bool is_dis)
{
    int ret;
    u32 val = 0, new_val;

    ret = mt6375_get_regmap();
    if (ret < 0)
        return ret;

    ret = regmap_read(regmap, BM_TOP_RST_CON2, &val);
    if (ret < 0)
        return ret;

    val &= RG_FGADC_RST_SRC_SEL_MASK;
    if (is_dis && val)
        regmap_clr_bits(regmap, BM_TOP_RST_CON2, RG_FGADC_RST_SRC_SEL_MASK);

    new_val = regmap_read(regmap, BM_TOP_RST_CON2, &new_val);
    new_val &= RG_FGADC_RST_SRC_SEL_MASK;

    LTRACEF("%s: is_dis:%d reset_sel=%d new=%d\n", __func__, is_dis, val,
            new_val);
    return 0;
}

int fgauge_get_dtsi_offset(void *fdt)
{
    return fdt_node_offset_by_compatible(fdt, -1, "mediatek,mt6375-gauge");
}

int fbattery_init(void)
{
    mt6375_config_rfg_gain_error();
    return 0;
}
