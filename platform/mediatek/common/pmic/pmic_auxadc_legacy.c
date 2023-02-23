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

#include <compiler.h>
#include <debug.h>
#include <err.h>
#include <pmic_auxadc.h>
#include <pmic_auxadc_legacy.h>
#include <pmic_wrap_common.h>

#define AUXADC_RDY_BIT          (1 << 15)

#define AUXADC_DEF_R_RATIO      1
#define AUXADC_DEF_AVG_NUM      8

#define AUXADC_AVG_TIME_US      10
#define AUXADC_POLL_DELAY_US    100
#define AUXADC_TIMEOUT_US       32000
#define VOLT_FULL               1800
#define IMP_STOP_DELAY_US       150

/*
 * @ch_name: HW channel name
 * @ch_num:  HW channel number
 * @res:     ADC resolution
 * @r_ratio: resistance ratio, represented by r_ratio[0] / r_ratio[1]
 * @avg_num: sampling times of AUXADC measurments then average it
 * @regs:    request and data output registers for this channel
 */
struct auxadc_channels {
    /* AUXADC channel attribute */
    const char *ch_name;
    unsigned char ch_num;
    unsigned char res;
    unsigned char r_ratio[2];
    unsigned short avg_num;
    const struct auxadc_regs *regs;
};

#define PMIC_AUXADC_CHANNEL(_ch_name, _ch_num, _res) \
    [AUXADC_##_ch_name] = {                          \
        .ch_name = #_ch_name,                        \
        .ch_num = _ch_num,                           \
        .res = _res,                                 \
    }                                                \

/*
 * The array represents all possible AUXADC channels found
 * in the supported PMICs.
 */
static struct auxadc_channels auxadc_chans[] = {
    PMIC_AUXADC_CHANNEL(BATADC, 0, 15),
    PMIC_AUXADC_CHANNEL(ISENSE, 0, 15),
    PMIC_AUXADC_CHANNEL(VCDT, 2, 12),
    PMIC_AUXADC_CHANNEL(BAT_TEMP, 3, 12),
    PMIC_AUXADC_CHANNEL(BATID, 3, 12),
    PMIC_AUXADC_CHANNEL(CHIP_TEMP, 4, 12),
    PMIC_AUXADC_CHANNEL(VCORE_TEMP, 4, 12),
    PMIC_AUXADC_CHANNEL(VPROC_TEMP, 4, 12),
    PMIC_AUXADC_CHANNEL(VGPU_TEMP, 4, 12),
    PMIC_AUXADC_CHANNEL(ACCDET, 5, 12),
    PMIC_AUXADC_CHANNEL(VDCXO, 6, 12),
    PMIC_AUXADC_CHANNEL(TSX_TEMP, 7, 15),
    PMIC_AUXADC_CHANNEL(HPOFS_CAL, 9, 15),
    PMIC_AUXADC_CHANNEL(DCXO_TEMP, 10, 15),
    PMIC_AUXADC_CHANNEL(VBIF, 11, 12),
    PMIC_AUXADC_CHANNEL(IMP, 0, 15),
};

/*
 * @channel: channel number, refer to the auxadc_chans index
             pass from struct iio_chan_spec.channel
 * @val:     pointer to output value
 */
static int get_auxadc_out(const struct auxadc_channels *auxadc_chan, u16 *val)
{
    int ret = 0;
    int count = 0;

    pwrap_write(auxadc_chan->regs->rqst_reg, 1 << auxadc_chan->regs->rqst_shift);
    spin(auxadc_chan->avg_num * AUXADC_AVG_TIME_US);

    while (1) {
        pwrap_read(auxadc_chan->regs->out_reg, val);
        if (*val & AUXADC_RDY_BIT)
            break;
        if (count > AUXADC_TIMEOUT_US / AUXADC_POLL_DELAY_US) {
            dprintf(INFO, "[PMIC](%d)Time out!\n", auxadc_chan->ch_num);
            ret = ERR_TIMED_OUT;
            break;
        }
        spin(AUXADC_POLL_DELAY_US);
        count++;
    }
    *val &= (1 << auxadc_chan->res) - 1;

    return ret;
}

int pmic_get_auxadc_value_legacy(int channel, enum val_type type)
{
    u16 auxadc_out = 0;
    int val;
    const struct auxadc_channels *auxadc_chan;

    if (channel < AUXADC_CHAN_MIN || channel > AUXADC_CHAN_MAX) {
        dprintf(CRITICAL, "[PMIC]invalid channel number %d\n", channel);
        return ERR_INVALID_ARGS;
    }
    auxadc_chan = &auxadc_chans[channel];
    get_auxadc_out(auxadc_chan, &auxadc_out);
    switch (type) {
    case PROCESSED:
        val = (int)auxadc_out * auxadc_chan->r_ratio[0] * VOLT_FULL;
        val = (val / auxadc_chan->r_ratio[1]) >> auxadc_chan->res;
        break;
    case RAW:
        val = (int)auxadc_out;
        break;
    default:
        dprintf(CRITICAL, "[PMIC]invalid type %d of channel %d\n", type, channel);
        return ERR_INVALID_ARGS;
    }
    dprintf(INFO, "name:%s, channel=%d, adc_out=0x%x, adc_result=%d\n",
            auxadc_chan->ch_name, auxadc_chan->ch_num, auxadc_out, val);
    return val;
}

static const enum val_type val_type_mapping[] = {
    [AUXADC_VAL_PROCESSED] = PROCESSED,
    [AUXADC_VAL_RAW] = RAW,
};

static struct auxadc_chan_spec pmic_auxadc_chan_specs[AUXADC_CHAN_MAX];

static int pmic_auxadc_read(const struct auxadc_device *auxadc_dev,
                            const struct auxadc_chan_spec *chan, int *val,
                            enum auxadc_val_type type)
{
    *val = pmic_get_auxadc_value_legacy(chan->channel,
                                        val_type_mapping[(u8)type]);
    return 0;
}

static void pmic_auxadc_init_chan_spec(void)
{
    int i;

    for (i = 0; i < AUXADC_CHAN_MAX; i++)
        pmic_auxadc_chan_specs[i].channel = i;
}

static struct auxadc_device pmic_auxadc_device = {
    .name = "pwrap_pmic",
    .chans = pmic_auxadc_chan_specs,
    .num_chans = countof(pmic_auxadc_chan_specs),
    .read = pmic_auxadc_read,
};

int pmic_auxadc_device_register_legacy(
    const struct auxadc_device_chan_spec *dev_chans, int num_chans)
{
#ifndef PROJECT_TYPE_FPGA
    int i, ret;
    int channel;

    if (!dev_chans || num_chans < 0)
        return ERR_INVALID_ARGS;
    for (i = 0; i < num_chans; i++, dev_chans++) {
        channel = dev_chans->channel;
        if (channel < AUXADC_CHAN_MIN || channel > AUXADC_CHAN_MAX) {
            dprintf(INFO, "[PMIC]failed to register channel %d\n", channel);
            continue;
        }

        auxadc_chans[channel].r_ratio[0] = dev_chans->r_ratio[0];
        auxadc_chans[channel].r_ratio[1] = dev_chans->r_ratio[1];

        if (dev_chans->avg_num)
            auxadc_chans[channel].avg_num = dev_chans->avg_num;
        else
            auxadc_chans[channel].avg_num = AUXADC_DEF_AVG_NUM;
        auxadc_chans[channel].regs = dev_chans->regs;
    }

    pmic_auxadc_init_chan_spec();
    ret = pmic_auxadc_device_register(&pmic_auxadc_device);
    if (ret < 0)
        dprintf(INFO, "[PMIC]failed to register auxadc device\n");
    return ret;
#else
    return 0;
#endif
}
