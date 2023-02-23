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
#include <errno.h>
#include <mt_i2c.h>
#include <platform/timer.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <trace.h>

#include "mt6360_pmu_charger.h"

#define LOCAL_TRACE 0

#define MT6360_PMU_CHARGER_LK_DRV_VERSION "1.0.0_MTK"

/* ================= */
/* Internal variable */
/* ================= */

enum mt6360_chg_mode {
    MT6360_CHG_MODE_UNKNOWN,
    MT6360_CHG_MODE_TRICHG,
    MT6360_CHG_MODE_PRECHG,
    MT6360_CHG_MODE_FASTCHG,
};

static const char * const mt6360_chg_mode_name[] = {
    "unknown",
    "trickle-charge",
    "pre-charge",
    "fast-charge",
};

struct mt6360_pmu_chg_info {
    u8 device_id;
    u8 vid;
    u32 iprec;
    u32 ichg;
    u32 ichg_dis_chg;
    bool chg_en;
    u32 aicr;
    u32 mivr;
    u32 cv;
    bool wkard_en;
};

enum mt6360_iinlmtsel {
    MT6360_IIMLMTSEL_AICR_3250 = 0,
    MT6360_IIMLMTSEL_CHR_TYPE,
    MT6360_IINLMTSEL_AICR,
    MT6360_IINLMTSEL_LOWER_LEVEL, /* lower of above three */
};

enum mt6360_charging_status {
    MT6360_CHG_STATUS_READY = 0,
    MT6360_CHG_STATUS_PROGRESS,
    MT6360_CHG_STATUS_DONE,
    MT6360_CHG_STATUS_FAULT,
    MT6360_CHG_STATUS_MAX,
};

static struct mt6360_pmu_chg_info g_mpci = {
    .device_id = -1,
    .vid = 0x00,
    .iprec = 150,
    .ichg = 2000,
    .aicr = 500,
    .mivr = 4500,
    .cv = 4350,
    .wkard_en = false,
    .ichg_dis_chg = 2000,
    .chg_en = true,
};

/* Charging status name */
static const char *mt6360_chg_status_name[MT6360_CHG_STATUS_MAX] = {
    "ready", "progress", "done", "fault",
};

/* ========================= */
/* I2C operations */
/* ========================= */
static int mt6360_i2c_write_byte(struct mt6360_pmu_chg_info *info,
                                 u8 cmd, u8 data)
{
    int ret = 0;
    u8 write_buf[2] = {cmd, data};

    ret = mtk_i2c_write(MT6360_I2C_ID, MT6360_SLAVE_ADDR,
                        write_buf, 2, NULL);
    if (ret != 0)
        LTRACEF("%s: I2CW[0x%02X] = 0x%02X failed, code = %d\n",
                __func__, cmd, data, ret);
    else
        LTRACEF("%s: I2CW[0x%02X] = 0x%02X\n",
                __func__, cmd, data);
    return ret;
}

static int mt6360_i2c_read_byte(struct mt6360_pmu_chg_info *info,
                                u8 cmd, u8 *data)
{
    int ret = 0;
    u8 ret_data = cmd;

    ret = mtk_i2c_write_read(MT6360_I2C_ID, MT6360_SLAVE_ADDR,
                            &ret_data, 1, 1, NULL);
    if (ret != 0)
        LTRACEF("%s: I2CR[0x%02X] failed, code = %d\n",
                __func__, cmd, ret);
    else {
        LTRACEF("%s: I2CR[0x%02X] = 0x%02X\n",
                __func__, cmd, ret_data);
        *data = ret_data;
    }
    return ret;
}

static int mt6360_i2c_block_write(struct mt6360_pmu_chg_info *info,
                                  u8 cmd, int len, const u8 *data)
{
    unsigned char *write_buf;
    int ret = 0;

    write_buf = malloc((len + 1));
    if (!write_buf) {
        LTRACEF("%s: Unable to allocate write_buf\n", __func__);
        return -1;
    }

    *write_buf = cmd;
    memcpy(write_buf + 1, data, len);
    ret = mtk_i2c_write(MT6360_I2C_ID, MT6360_SLAVE_ADDR,
                        write_buf, len + 1, NULL);
    free(write_buf);

    return ret;
}

static int mt6360_i2c_block_read(struct mt6360_pmu_chg_info *info,
                                 u8 cmd, int len, u8 *data)
{
    data[0] = cmd;
    return mtk_i2c_write_read(MT6360_I2C_ID, MT6360_SLAVE_ADDR,
                            data, 1, len, NULL);
}

static int mt6360_i2c_update_bits(struct mt6360_pmu_chg_info *info,
                                  u8 cmd, u8 mask, u8 data)
{
    int ret = 0;
    u8 reg_data = 0;

    ret = mt6360_i2c_read_byte(info, cmd, &reg_data);
    if (ret != 0)
        return ret;
    reg_data = reg_data & 0xFF;
    reg_data &= ~mask;
    reg_data |= (data & mask);
    return mt6360_i2c_write_byte(info, cmd, reg_data);
}

static inline int mt6360_set_bit(struct mt6360_pmu_chg_info *info,
                                 u8 cmd, u8 mask)
{
    return mt6360_i2c_update_bits(info, cmd, mask, mask);
}

static inline int mt6360_clr_bit(struct mt6360_pmu_chg_info *info,
                                 u8 cmd, u8 mask)
{
    return mt6360_i2c_update_bits(info, cmd, mask, 0x00);
}

/* ================== */
/* internal functions */
/* ================== */
static int mt6360_enable_hidden_mode(struct mt6360_pmu_chg_info *info, bool en)
{
    return mt6360_i2c_write_byte(info, MT6360_PMU_TM_PAS_CODE1, en ? 0x69 : 0);
}

static bool mt6360_is_hw_exist(struct mt6360_pmu_chg_info *info)
{
    int ret = 0;
    u8 vid = 0, rev_id = 0;
    u8 data = 0;

    ret = mt6360_i2c_read_byte(info, MT6360_PMU_DEV_INFO, &data);
    if (ret != 0)
        return false;
    vid = data & 0xF0;
    rev_id = data & 0x0F;
    if (vid != MT6360_VENDOR_ID) {
        LTRACEF("%s: vid is not match(%d)\n", __func__, vid);
        return false;
    }
    LTRACEF("%s: rev_id = %d\n", __func__, rev_id);

    info->device_id = rev_id;
    info->vid = vid;
    return true;
}

static int mt6360_enable_ilim(struct mt6360_pmu_chg_info *info, bool enable)
{
    LTRACEF("%s: enable ilim = %d\n", __func__, enable);
    return (enable ? mt6360_set_bit : mt6360_clr_bit)
           (info, MT6360_PMU_CHG_CTRL3, MT6360_MASK_ILIM_EN);
}

/* Select IINLMTSEL to use AICR */
static int mt6360_select_input_current_limit(
    struct mt6360_pmu_chg_info *info, enum mt6360_iinlmtsel sel)
{
    LTRACEF("%s: select input current limit = %d\n",
            __func__, sel);
    return mt6360_i2c_update_bits(info,
                                  MT6360_PMU_CHG_CTRL2,
                                  MT6360_MASK_IINLMTSEL,
                                  sel << MT6360_SHFT_IINLMTSEL);
}

static int mt6360_set_cv(struct mt6360_pmu_chg_info *info, u32 uV)
{
    u8 data = 0;

    if (uV >= 3900)
        data = (uV - 3900) / 10;
    if (data > MT6360_VOREG_MAXVAL)
        data = MT6360_VOREG_MAXVAL;
    LTRACEF("%s: cv = %d\n", __func__, uV);
    return mt6360_i2c_update_bits(info,
                                  MT6360_PMU_CHG_CTRL4,
                                  MT6360_MASK_VOREG,
                                  data << MT6360_SHFT_VOREG);
}

static int mt6360_enable_wdt(struct mt6360_pmu_chg_info *info, bool en)
{
    LTRACEF("%s: en = %d\n", __func__, en);
    return (en ? mt6360_set_bit : mt6360_clr_bit)
           (info, MT6360_PMU_CHG_CTRL13, MT6360_MASK_CHG_WDT_EN);
}

static int mt6360_get_charging_status(struct mt6360_pmu_chg_info *info,
                                      enum mt6360_charging_status *chg_stat)
{
    int ret = 0;
    u8 data = 0;

    ret = mt6360_i2c_read_byte(info, MT6360_PMU_CHG_STAT, &data);
    if (ret != 0)
        return ret;
    *chg_stat = (data & MT6360_MASK_CHG_STAT) >> MT6360_SHFT_CHG_STAT;
    return ret;
}

static int mt6360_get_mivr(struct mt6360_pmu_chg_info *info, u32 *mivr)
{
    int ret = 0;
    u8 data = 0;

    ret = mt6360_i2c_read_byte(info, MT6360_PMU_CHG_CTRL6, &data);
    if (ret != 0)
        return ret;
    data = (data & MT6360_MASK_MIVR) >> MT6360_SHFT_MIVR;
    *mivr = 3900 + (data * 100);
    return ret;
}

static int mt6360_get_ieoc(struct mt6360_pmu_chg_info *info, u32 *ieoc)
{
    int ret = 0;
    u8 data = 0;

    ret = mt6360_i2c_read_byte(info, MT6360_PMU_CHG_CTRL9, &data);
    if (ret != 0)
        return ret;
    data = (data & MT6360_MASK_IEOC) >> MT6360_SHFT_IEOC;
    *ieoc = 100 + (data * 50);
    return ret;
}

static int mt6360_is_charging_enable(struct mt6360_pmu_chg_info *info,
                                     bool *enable)
{
    int ret = 0;
    u8 data = 0;

    ret = mt6360_i2c_read_byte(info, MT6360_PMU_CHG_CTRL2, &data);
    if (ret != 0)
        return ret;
    *enable = (data & MT6360_MASK_CHG_EN) ? true : false;
    return ret;
}

/* =========================================================== */
/* The following is implementation for interface of mtk_charger */
/* =========================================================== */
int mtk_charger_set_ichg(u32 ichg)
{
    struct mt6360_pmu_chg_info *info = &g_mpci;
    int ret = 0;
    u8 data = 0;

    LTRACEF("%s: ichg = %d\n", __func__, ichg);
    if (ichg >= 100)
        data = (ichg - 100) / 100;
    if (data > MT6360_ICHG_MAXVAL)
        data = MT6360_ICHG_MAXVAL;
    ret = mt6360_i2c_update_bits(info,
                                 MT6360_PMU_CHG_CTRL7,
                                 MT6360_MASK_ICHG,
                                 data << MT6360_SHFT_ICHG);
    if (ret < 0)
        LTRACEF("%s: fail(%d)\n", __func__, ret);
    else
        info->ichg = ichg;
    return ret;
}

int mtk_charger_enable_charging(bool enable)
{
    struct mt6360_pmu_chg_info *info = &g_mpci;
    int ret = 0;
    u32 ichg_ramp_t = 0;
    u8 data = 0;

    if (info->chg_en == enable) {
        LTRACEF("%s: is the same, en = %d\n", __func__, enable);
        return 0;
    }
    ret = mt6360_i2c_read_byte(info, MT6360_PMU_CHRDET_STAT, &data);
    if (ret != 0)
        goto bypass_wkard;
    if (!(data & MT6360_MASK_CHRDET_EXT_EVT)) {
        LTRACEF("%s: pwr_rdy = 0, bypass wkard\n", __func__);
        goto bypass_wkard;
    }
    if (info->ichg < 500) {
        LTRACEF("%s: ichg < 500, bypass wkard\n", __func__);
        goto bypass_wkard;
    }

    /* Workaround for avoid vsys overshoot when charge disable */
    if (!enable) {
        info->ichg_dis_chg = info->ichg;
        ichg_ramp_t = (info->ichg - 500) / 50 * 2;
        /* Set ichg 500mA */
        ret = mt6360_i2c_update_bits(info,
                                     MT6360_PMU_CHG_CTRL7,
                                     MT6360_MASK_ICHG,
                                     0x04 << MT6360_SHFT_ICHG);
        if (ret < 0) {
            LTRACEF("%s: set ichg fail\n", __func__);
            return ret;
        }
        spin(ichg_ramp_t * 1000);
    } else {
        if (info->ichg == info->ichg_dis_chg) {
            ret = mtk_charger_set_ichg(info->ichg);
            if (ret < 0) {
                LTRACEF("%s: set ichg fail\n", __func__);
                return ret;
            }
        }
    }

bypass_wkard:
    ret = (enable ? mt6360_set_bit : mt6360_clr_bit)
          (info, MT6360_PMU_CHG_CTRL2, MT6360_MASK_CHG_EN);
    if (ret < 0)
        LTRACEF("%s: fail, en = %d\n", __func__, enable);
    else
        info->chg_en = enable;
    return ret;
}

int mtk_charger_get_ichg(u32 *ichg)
{
    struct mt6360_pmu_chg_info *info = &g_mpci;
    int ret = 0;
    u8 data = 0;

    ret = mt6360_i2c_read_byte(info, MT6360_PMU_CHG_CTRL7, &data);
    if (ret != 0)
        return ret;
    data = (data & MT6360_MASK_ICHG) >> MT6360_SHFT_ICHG;
    *ichg = 100 + (data * 100);
    return ret;
}

static int mtk_charger_set_iprec(u32 iprec)
{
    struct mt6360_pmu_chg_info *info = &g_mpci;
    u8 data = 0;

    LTRACEF("%s: iprec = %d\n", __func__, iprec);
    if (iprec >= 100)
        data = (iprec - 100) / 50;
    if (data > MT6360_IPREC_MAXVAL)
        data = MT6360_IPREC_MAXVAL;
    return mt6360_i2c_update_bits(info,
                                  MT6360_PMU_CHG_CTRL8,
                                  MT6360_MASK_IPREC,
                                  data << MT6360_SHFT_IPREC);
}

static enum mt6360_chg_mode mt6360_get_charging_mode(
    struct mt6360_pmu_chg_info *info)
{
    int ret = 0;
    u8 data = 0;

    LTRACEF("%s\n", __func__);

    ret = mt6360_i2c_read_byte(info, MT6360_PMU_CHG_STAT, &data);
    if (ret < 0)
        return MT6360_CHG_MODE_UNKNOWN;
    if (data & MT6360_MASK_VBAT_TRI)
        return MT6360_CHG_MODE_TRICHG;
    return (data & MT6360_MASK_VBAT_LVL) ? MT6360_CHG_MODE_FASTCHG :
           MT6360_CHG_MODE_PRECHG;
}

static int mtk_charger_set_ichg_ext(u32 ichg)
{
    struct mt6360_pmu_chg_info *info = &g_mpci;
    enum mt6360_chg_mode cur_mode = MT6360_CHG_MODE_UNKNOWN;

    LTRACEF("%s: ichg = %u\n", __func__, ichg);
    cur_mode = mt6360_get_charging_mode(info);
    if (cur_mode == MT6360_CHG_MODE_UNKNOWN) {
        LTRACEF("%s: get charging mode fail\n", __func__);
        return -EINVAL;
    }
    info->ichg = ichg;
    if (cur_mode != MT6360_CHG_MODE_FASTCHG) {
        LTRACEF("%s: not in fast-charge mode\n", __func__);
        return 0;
    }
    if (info->wkard_en) {
        LTRACEF("%s: in work around\n", __func__);
        return 0;
    }
    return mtk_charger_set_ichg(ichg);
}

int mtk_charger_get_aicr(u32 *aicr)
{
    struct mt6360_pmu_chg_info *info = &g_mpci;
    int ret = 0;
    u8 data = 0;

    ret = mt6360_i2c_read_byte(info, MT6360_PMU_CHG_CTRL3, &data);
    if (ret != 0)
        return ret;
    data = (data & MT6360_MASK_AICR) >> MT6360_SHFT_AICR;
    *aicr = 100 + (data * 50);
    return ret;
}

static int mt6360_adc_get_process_val(struct mt6360_pmu_chg_info *info,
                                      enum mt6360_adc_channel chan, u32 *val)
{
    int ret = 0;
    u8 data = 0;

    switch (chan) {
    case MT6360_ADC_USBID:
    case MT6360_ADC_VREF_TS:
    case MT6360_ADC_TS:
        *val *= 1250;
        break;
    case MT6360_ADC_TEMP_JC:
        *val -= 40;
        break;
    case MT6360_ADC_VBAT:
    case MT6360_ADC_VSYS:
    case MT6360_ADC_CHG_VDDP:
        *val *= 1250;
        break;
    case MT6360_ADC_VBUSDIV5:
        *val *= 6250;
        break;
    case MT6360_ADC_VBUSDIV2:
    case MT6360_ADC_IBAT:
        *val *= 2500;
        break;
    case MT6360_ADC_IBUS:
        ret = mt6360_i2c_read_byte(info, MT6360_PMU_CHG_CTRL3, &data);
        if (ret < 0)
            return ret;
        if (((data & 0xfc) >> 2) < 0x6)
            *val *= 1900;
        else
            *val *= 2500;
        break;
    default:
        LTRACEF("%s: unknown channel\n", __func__);
        break;
    }
    return ret;
}

static int mtk_charger_get_adc(int chan, u32 *val)
{
    struct mt6360_pmu_chg_info *info = &g_mpci;
    u8 tmp[3], rpt[3];
    u8 data = 0;
    int i, ret = 0, max_retry_cnt = 20;

    LTRACEF("%s: ++\n", __func__);
    if (chan >= MT6360_ADC_MAX) {
        LTRACEF("%s: undefined channel(%d)\n",
                __func__, chan);
        ret = -EINVAL;
        goto err_adc_init;
    }
    /* first reset all channels and adc en */
    memset(tmp, 0, sizeof(tmp));
    ret = mt6360_i2c_block_write(info, MT6360_PMU_ADC_CONFIG, 3, tmp);
    if (ret < 0) {
        LTRACEF("%s: reset all channels fail\n", __func__);
        goto err_adc_init;
    }
    /* enable adc_donei irq */
    ret = mt6360_clr_bit(info, MT6360_PMU_CHG_MASK6, MT6360_MASK_ADC_DONE);
    if (ret < 0) {
        LTRACEF("%s: unmask adc_donei fail\n", __func__);
        goto err_adc_conv;
    }
    /* select preferred channel : vbus */
    ret = mt6360_i2c_update_bits(info,
                                 MT6360_PMU_ADC_RPT_1, 0xf0, chan << 4);
    if (ret < 0) {
        LTRACEF("%s: select prefer channel fail\n", __func__);
        goto err_adc_conv;
    }
    /* enable vbus adc channel and adc_en */
    tmp[0] |= (1 << 7);
    if ((chan / 8) > 0)
        tmp[0] |= (1 << (chan % 8));
    else
        tmp[1] |= (1 << (chan % 8));
    ret = mt6360_i2c_block_write(info, MT6360_PMU_ADC_CONFIG, 2, tmp);
    if (ret < 0) {
        LTRACEF("%s: enable vbus adc and adc en fail\n", __func__);
        goto err_adc_conv;
    }
    /* wait adc conversion done */
    for (i = 0; i < max_retry_cnt; i++) {
        spin(30000);
        /* read adc conversion donei event */
        ret = mt6360_i2c_read_byte(info, MT6360_PMU_CHG_IRQ6, &data);
        if (ret < 0)
            goto err_adc_conv;
        if (!(data & MT6360_MASK_ADC_DONE))
            continue;
        ret = mt6360_set_bit(info, MT6360_PMU_CHG_IRQ6,
                             MT6360_MASK_ADC_DONE);
        if (ret < 0) {
            LTRACEF("%s: clear adc donei irq fail\n", __func__);
            goto err_adc_conv;
        }
        memset(rpt, 0, sizeof(rpt));
        ret = mt6360_i2c_block_read(info, MT6360_PMU_ADC_RPT_1, 3, rpt);
        if (ret < 0)
            goto err_adc_conv;
        if ((rpt[0] & 0x0f) != chan) {
            LTRACEF("%s: not want channel report(%d)\n",
                    __func__, rpt[0]);
            continue;
        }
        *val = ((rpt[1] << 8) | rpt[2]);
        ret = mt6360_adc_get_process_val(info, chan, val);
        break;
    }
    if (i == max_retry_cnt) {
        LTRACEF("%s: reach adc retry cnt\n", __func__);
        ret = -EBUSY;
        goto err_adc_conv;
    }
err_adc_conv:
    tmp[0] &= ~(0x7);
    tmp[1] = 0;
    mt6360_i2c_block_write(info, MT6360_PMU_ADC_CONFIG, 2, tmp);
    mt6360_set_bit(info, MT6360_PMU_CHG_MASK6, MT6360_MASK_ADC_DONE);
err_adc_init:
    LTRACEF("%s: --\n", __func__);
    return ret;
}

int mtk_charger_get_vbus(u32 *vbus)
{
    return mtk_charger_get_adc(MT6360_ADC_VBUSDIV5, vbus);
}

int mtk_charger_set_aicr(u32 aicr)
{
    struct mt6360_pmu_chg_info *info = &g_mpci;
    u8 data = 0;

    LTRACEF("%s: aicr = %d\n", __func__, aicr);
    if (aicr >= 100)
        data = (aicr - 100) / 50;
    if (data > MT6360_AICR_MAXVAL)
        data = MT6360_AICR_MAXVAL;
    return mt6360_i2c_update_bits(info,
                                  MT6360_PMU_CHG_CTRL3,
                                  MT6360_MASK_AICR,
                                  data << MT6360_SHFT_AICR);
}

static int mtk_charger_set_aicr_ext(u32 aicr)
{
    struct mt6360_pmu_chg_info *info = &g_mpci;
    enum mt6360_chg_mode cur_mode = MT6360_CHG_MODE_UNKNOWN;

    LTRACEF("%s: aicr = %d\n", __func__, aicr);
    cur_mode = mt6360_get_charging_mode(info);
    if (cur_mode == MT6360_CHG_MODE_UNKNOWN) {
        LTRACEF("%s: get charging mode fail\n", __func__);
        return -EINVAL;
    }
    info->aicr = aicr;
    if (cur_mode != MT6360_CHG_MODE_FASTCHG) {
        LTRACEF("%s: not in fast-charge mode\n", __func__);
        return 0;
    }
    if (info->wkard_en) {
        LTRACEF("%s: in work around\n", __func__);
        return 0;
    }
    return mtk_charger_set_aicr(aicr);
}

int mtk_charger_set_mivr(u32 mivr)
{
    struct mt6360_pmu_chg_info *info = &g_mpci;
    u8 data = 0;

    LTRACEF("%s: mivr = %d\n", __func__, mivr);
    if (mivr >= 3900)
        data = (mivr - 3900) / 100;
    if (data > MT6360_MIVR_MAXVAL)
        data = MT6360_MIVR_MAXVAL;
    return mt6360_i2c_update_bits(info,
                                  MT6360_PMU_CHG_CTRL6,
                                  MT6360_MASK_MIVR,
                                  data << MT6360_SHFT_MIVR);
}

int mtk_charger_enable_power_path(bool enable)
{
    struct mt6360_pmu_chg_info *info = &g_mpci;

    LTRACEF("%s: enable = %d\n", __func__, enable);
    return (enable ? mt6360_clr_bit : mt6360_set_bit)
           (info, MT6360_PMU_CHG_CTRL1, MT6360_MASK_FORCE_SLEEP);
}

int mtk_charger_reset_pumpx(bool reset)
{
    struct mt6360_pmu_chg_info *info = &g_mpci;

    if (reset)
        mtk_charger_get_aicr(&info->aicr);
    return mtk_charger_set_aicr(reset ? 100 : info->aicr);
}

int mtk_charger_enable_wdt(bool en)
{
    struct mt6360_pmu_chg_info *info = &g_mpci;

    return mt6360_enable_wdt(info, en);
}

int mtk_charger_dump_register(void)
{
    struct mt6360_pmu_chg_info *info = &g_mpci;
    enum mt6360_charging_status chg_status = MT6360_CHG_STATUS_READY;
    u32 ichg = 0, aicr = 0, mivr = 0, ieoc = 0;
    bool chg_en = 0;
    int ret = 0;

    ret = mtk_charger_get_ichg(&ichg);
    ret = mtk_charger_get_aicr(&aicr);
    ret = mt6360_get_mivr(info, &mivr);
    ret = mt6360_get_ieoc(info, &ieoc);
    ret = mt6360_is_charging_enable(info, &chg_en);
    ret = mt6360_get_charging_status(info, &chg_status);
    LTRACEF("%s: ICHG = %dmA, AICR = %dmA, MIVR = %dmV, IEOC = %dmA\n",
            __func__, ichg, aicr, mivr, ieoc);

    LTRACEF("%s: CHG_EN = %d, CHG_STATUS = %s\n",
            __func__, chg_en, mt6360_chg_status_name[chg_status]);
    return ret;
}

static int mt6360_fix_current_accuracy(bool enable)
{
    struct mt6360_pmu_chg_info *info = &g_mpci;
    u8 swit_feq[3] = { 0x80, 0x20, 0x00 };
    int i, ret = 0;

    if (!(info->wkard_en ^ enable))
        return 0;
    ret = mt6360_enable_hidden_mode(info, true);
    if (ret < 0) {
        LTRACEF("%s: enter hidden mode fail\n", __func__);
        return ret;
    }
    if (enable) {
        /* Set chg/otg switch frequence 0.75MHz */
        ret = mt6360_set_bit(info, MT6360_PMU_CHG_CTRL1, 0x80);
        if (ret < 0) {
            LTRACEF("%s: set switch feq to 0.75MHz fail\n",
                    __func__);
            goto out;
        }
        /* Set buck slope ratio to 0.6x */
        ret = mt6360_i2c_update_bits(info, MT6360_PMU_CHG_HIDDEN_CTRL10,
                                     0x07, 0);
        if (ret < 0) {
            LTRACEF("%s: set buck slope ratio 0.6x fail\n",
                    __func__);
            goto out;
        }
        /* Set Power path MOS gate driving slew rate to 2x */
        ret = mt6360_set_bit(info, MT6360_PMU_CHG_HIDDEN_CTRL11, 0xC0);
        if (ret < 0) {
            LTRACEF("%s:  set pp gate slew rate fail\n", __func__);
            goto out;
        }
    } else {
        /* Clear Power path MOS gate driving slew rate to 1x */
        ret = mt6360_clr_bit(info, MT6360_PMU_CHG_HIDDEN_CTRL11, 0xC0);
        if (ret < 0) {
            LTRACEF("%s:  clr pp gate slew rate fail\n", __func__);
            goto out;
        }
        /* set buck slope ratio step 0.6x to 1.8x */
        for (i = 0; i < 7; i++) {
            ret = mt6360_i2c_update_bits(info,
                                         MT6360_PMU_CHG_HIDDEN_CTRL10, 0x07, i);
            if (ret < 0) {
                LTRACEF("%s: step up buck slope ratio fail\n",
                        __func__);
                goto out;
            }
        }
        /* Set chg/otg switch frequence step 0.75->1.0->1.5MHz */
        for (i = 0; i < 3; i++) {
            ret = mt6360_i2c_update_bits(info, MT6360_PMU_CHG_CTRL1,
                                         0xa0, swit_feq[i]);
            if (ret < 0) {
                LTRACEF("%s: step down switch feq fail\n",
                        __func__);
                goto out;
            }
        }
    }
    info->wkard_en = enable;
out:
    mt6360_enable_hidden_mode(info, false);
    return (ret < 0) ? ret : 0;
}

int mtk_charger_check_charging_mode(void)
{
    struct mt6360_pmu_chg_info *info = &g_mpci;
    int ret = 0;
    static enum mt6360_chg_mode pre_mode = MT6360_CHG_MODE_UNKNOWN;
    enum mt6360_chg_mode cur_mode = MT6360_CHG_MODE_UNKNOWN;
    u8 data = 0;

    LTRACEF("%s\n", __func__);

    ret = mt6360_i2c_read_byte(info, MT6360_PMU_CHRDET_STAT, &data);
    if (ret < 0)
        return ret;
    if (!(data & MT6360_MASK_CHRDET_EXT_EVT)) {
        LTRACEF("%s: pwr_rdy = 0\n", __func__);
        return 0;
    }

    ret = mt6360_i2c_read_byte(info, MT6360_PMU_CHG_CTRL2, &data);
    if (ret < 0)
        return ret;
    if (!(data & MT6360_MASK_CFO_EN)) {
        LTRACEF("%s: CFO_EN = 0\n", __func__);
        return 0;
    }

    cur_mode = mt6360_get_charging_mode(info);
    if (cur_mode == MT6360_CHG_MODE_UNKNOWN) {
        LTRACEF("%s: get charging mode fail\n", __func__);
        return -EINVAL;
    }

    LTRACEF("%s: in %s mode, previously in %s mode\n", __func__,
            mt6360_chg_mode_name[cur_mode],
            mt6360_chg_mode_name[pre_mode]);
    if (cur_mode == pre_mode)
        return 0;
    switch (cur_mode) {
    case MT6360_CHG_MODE_TRICHG:
        ret = mtk_charger_set_ichg(info->iprec);
        if (ret < 0) {
            LTRACEF("%s: set ichg to %umA fail\n",
                    __func__, info->iprec);
            return ret;
        }
        ret = mt6360_fix_current_accuracy(true);
        if (ret < 0) {
            LTRACEF("%s: fix current accuracy fail\n", __func__);
            return ret;
        }
        break;
    case MT6360_CHG_MODE_PRECHG:
        ret = mtk_charger_set_ichg(info->iprec);
        if (ret < 0) {
            LTRACEF("%s: set ichg to %umA fail\n",
                    __func__, info->iprec);
            return ret;
        }
        ret = mt6360_fix_current_accuracy((info->iprec < 500) ?
                                          true : false);
        if (ret < 0) {
            LTRACEF("%s: fix current accuracy fail\n", __func__);
            return ret;
        }
        break;
    case MT6360_CHG_MODE_FASTCHG:
        ret = mt6360_fix_current_accuracy(false);
        if (ret < 0) {
            LTRACEF("%s: fix current accuracy fail\n", __func__);
            return ret;
        }
        ret = mtk_charger_set_aicr(info->aicr);
        if (ret < 0) {
            LTRACEF("%s: set aicr to %umA fail\n",
                    __func__, info->aicr);
            return ret;
        }
        ret = mtk_charger_set_ichg(info->ichg);
        if (ret < 0) {
            LTRACEF("%s: set ichg to %umA fail\n",
                    __func__, info->ichg);
            return ret;
        }
        break;
    default:
        LTRACEF("%s: Unknown mode\n", __func__);
        break;
    }
    pre_mode = cur_mode;
    return 0;
}

static int mt6360_chg_init_setting(struct mt6360_pmu_chg_info *info)
{
    int ret = 0;

    LTRACEF("%s: starts\n", __func__);
    ret = mt6360_enable_wdt(info, true);
    if (ret < 0) {
        LTRACEF("%s: enable wdt failed\n", __func__);
        goto out;
    }
    /* Select input current limit to referenced from AICR */
    ret = mt6360_select_input_current_limit(info,
                                            MT6360_IINLMTSEL_AICR);
    if (ret < 0) {
        LTRACEF("%s: select input current limit failed\n",
                __func__);
        goto out;
    }
    spin(5000);
    /* Disable HW iinlimit, use SW */
    ret = mt6360_enable_ilim(info, false);
    if (ret < 0) {
        LTRACEF("%s: disable ilim failed\n", __func__);
        goto out;
    }
    ret = mtk_charger_set_iprec(info->iprec);
    if (ret < 0) {
        LTRACEF("%s: set iprec failed\n", __func__);
        goto out;
    }
    /* Set ichg 500mA for vsys overshoot */
    ret = mtk_charger_set_ichg(500);
    if (ret < 0) {
        LTRACEF("%s: set ichg failed\n", __func__);
        goto out;
    }
    ret = mtk_charger_set_aicr(info->aicr);
    if (ret < 0) {
        LTRACEF("%s: set aicr failed\n", __func__);
        goto out;
    }
    ret = mtk_charger_set_mivr(info->mivr);
    if (ret < 0) {
        LTRACEF("%s: set mivr failed\n", __func__);
        goto out;
    }
    ret = mt6360_set_cv(info, info->cv);
    if (ret < 0) {
        LTRACEF("%s: set cv failed\n", __func__);
        goto out;
    }

    /* Fix pre-chg/fast-chg current loop not correct */
    ret = mtk_charger_check_charging_mode();
    if (ret < 0) {
        LTRACEF("%s: check charging mode fail\n", __func__);
        goto out;
    }

    /* Disable sys drop improvement for download mode */
    ret = mt6360_clr_bit(info, MT6360_PMU_CHG_CTRL20, MT6360_MASK_EN_SDI);
    if (ret < 0)
        LTRACEF("%s: disable sys drop improvement fail\n", __func__);
out:
    return ret;
}

int mtk_charger_init(void)
{
    int ret = 0;

    /* Check primary charger */
    if (!mt6360_is_hw_exist(&g_mpci))
        return -ENODEV;

    LTRACEF("%s: %s\n",
            __func__, MT6360_PMU_CHARGER_LK_DRV_VERSION);
    ret = mt6360_chg_init_setting(&g_mpci);
    if (ret < 0) {
        LTRACEF("%s: init setting fail\n", __func__);
        return ret;
    }

    return ret;
}
