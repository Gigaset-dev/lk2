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
#include <pmic_auxadc.h>
#include <string.h>

#define AUXADC_SLOT_MAX 10

struct auxadc {
    const struct auxadc_device *dev;
    u32 support_chans;
};

static struct auxadc auxadc_tbl[AUXADC_SLOT_MAX];

static struct auxadc *auxadc_register(const struct auxadc_device *auxadc_dev)
{
    int i;
    struct auxadc *auxadc = NULL;
    const struct auxadc_chan_spec *chan;

    for (i = 0; i < AUXADC_SLOT_MAX; i++) {
        if (auxadc_tbl[i].dev &&
            !strcmp(auxadc_tbl[i].dev->name, auxadc_dev->name))
            return NULL;
        else if (!auxadc && !auxadc_tbl[i].dev)
            auxadc = &auxadc_tbl[i];
    }
    if (auxadc) {
        auxadc->dev = auxadc_dev;
        for (i = 0; i < auxadc_dev->num_chans; i++) {
            chan = &auxadc_dev->chans[i];
            if (chan->channel > 31)
                return NULL;
            atomic_or((int *)&auxadc->support_chans, 1 << (chan->channel));
        }
    }
    return auxadc;
}

static int auxadc_read(const struct auxadc_device *auxadc_dev,
                       const struct auxadc_chan_spec *chan, int *val,
                       enum auxadc_val_type type)
{
    int ret;
    u32 regval;
    u32 elapsed = 0;
    size_t len = (chan->hw_info.res + 7) / 8;

    if (chan->hw_info.res > AUXADC_RES_MAX)
        return ERR_INVALID_ARGS;

    ret = auxadc_dev->write_regs(auxadc_dev, chan->hw_info.enable_reg,
                                 (u8 *)&chan->hw_info.enable_mask, 1);
    if (ret < 0)
        return ret;
    spin(chan->hw_info.min_time);
    elapsed += chan->hw_info.min_time;

    while (1) {
        if (chan->hw_info.ready_reg != chan->hw_info.value_reg) {
            ret = auxadc_dev->read_regs(auxadc_dev, chan->hw_info.ready_reg,
                                        (u8 *)&regval, 1);
        } else {
            ret = auxadc_dev->read_regs(auxadc_dev, chan->hw_info.ready_reg,
                                        (u8 *)&regval, len);
        }
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
    if (chan->hw_info.ready_reg != chan->hw_info.value_reg) {
        ret = auxadc_dev->read_regs(auxadc_dev, chan->hw_info.value_reg,
                                    (u8 *)&regval, len);
        if (ret < 0)
            return ret;
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
    return 0;
}

int pmic_auxadc_read_value(const struct auxadc *auxadc, int channel, int *val,
                           enum auxadc_val_type type)
{
    int i;
    const struct auxadc_chan_spec *chan = NULL;

    if (!auxadc || !auxadc->dev)
        return ERR_INVALID_ARGS;

    for (i = 0; i < auxadc->dev->num_chans; i++) {
        if (auxadc->dev->chans[i].channel == channel) {
            chan = &auxadc->dev->chans[i];
            break;
        }
    }
    if (!chan)
        return ERR_INVALID_ARGS;
    return (auxadc->dev->read ? auxadc->dev->read : auxadc_read)
            (auxadc->dev, chan, val, type);
}

int pmic_auxadc_read_value_ext(int channel, int *val,
                               enum auxadc_val_type type)
{
    int i;
    struct auxadc *auxadc;

    for (i = 0; i < AUXADC_SLOT_MAX; i++) {
        auxadc = &auxadc_tbl[i];
        if (auxadc->dev && BIT_SET(auxadc->support_chans, channel))
            break;
    }
    if (i == AUXADC_SLOT_MAX)
        return ERR_INVALID_ARGS;
    return pmic_auxadc_read_value(auxadc, channel, val, type);
}

int pmic_sdmadc_read_value(const struct auxadc *auxadc, int channel, int *val,
                           enum sdmadc_pures pures, enum auxadc_val_type type)
{
    int i;
    const struct auxadc_chan_spec *chan = NULL;

    if (!auxadc || !auxadc->dev)
        return ERR_INVALID_ARGS;
    if (!auxadc->dev->sdmadc_read)
        return ERR_NOT_SUPPORTED;

    for (i = 0; i < auxadc->dev->num_chans; i++) {
        if (auxadc->dev->chans[i].channel == channel) {
            chan = &auxadc->dev->chans[i];
            break;
        }
    }
    if (!chan || !chan->sdmadc_hw_info.set_reg)
        return ERR_INVALID_ARGS;
    if (pures > SDMADC_OPEN)
        return ERR_INVALID_ARGS;
    return auxadc->dev->sdmadc_read(auxadc->dev, chan, val, pures, type);
}

struct auxadc *pmic_auxadc_get_by_name(const char *name)
{
    int i;

    if (!name)
        return NULL;

    for (i = 0; i < AUXADC_SLOT_MAX; i++) {
        if (auxadc_tbl[i].dev && !strcmp(auxadc_tbl[i].dev->name, name))
            return &auxadc_tbl[i];
    }
    return NULL;
}

int pmic_auxadc_device_register(const struct auxadc_device *auxadc_dev)
{
#ifndef PROJECT_TYPE_FPGA
    int i;
    struct auxadc *auxadc;
    const struct auxadc_chan_spec *chan;

    if (!auxadc_dev)
        return ERR_INVALID_ARGS;

    if (!auxadc_dev->name || !auxadc_dev->chans || auxadc_dev->num_chans < 0)
        return ERR_INVALID_ARGS;

    if ((!auxadc_dev->read_regs || !auxadc_dev->write_regs) &&
        !auxadc_dev->read)
        return ERR_INVALID_ARGS;

    for (i = 0; i < auxadc_dev->num_chans; i++) {
        chan = &auxadc_dev->chans[i];
        if (chan->channel < 0 || chan->channel >= AUXADC_CHAN_MAX)
            return ERR_INVALID_ARGS;
        if (!auxadc_dev->read &&
            (chan->hw_info.ratio[1] == 0 || chan->hw_info.res > AUXADC_RES_MAX))
            return ERR_INVALID_ARGS;
    }

    auxadc = auxadc_register(auxadc_dev);
    if (!auxadc)
        return ERR_INVALID_ARGS;
#endif
    return 0;
}

int pmic_auxadc_device_unregister(struct auxadc *auxadc)
{
    if (!auxadc)
        return ERR_INVALID_ARGS;

    auxadc->dev = NULL;
    auxadc->support_chans = 0;
    return 0;
}
