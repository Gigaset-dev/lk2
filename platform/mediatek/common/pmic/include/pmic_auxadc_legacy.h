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

#pragma once

#include <sys/types.h>

/* PMIC MT635x AUXADC channels */
#define AUXADC_BATADC       0x00
#define AUXADC_ISENSE       0x01
#define AUXADC_VCDT         0x02
#define AUXADC_BAT_TEMP     0x03
#define AUXADC_BATID        0x04
#define AUXADC_CHIP_TEMP    0x05
#define AUXADC_VCORE_TEMP   0x06
#define AUXADC_VPROC_TEMP   0x07
#define AUXADC_VGPU_TEMP    0x08
#define AUXADC_ACCDET       0x09
#define AUXADC_VDCXO        0x0a
#define AUXADC_TSX_TEMP     0x0b
#define AUXADC_HPOFS_CAL    0x0c
#define AUXADC_DCXO_TEMP    0x0d
#define AUXADC_VBIF         0x0e
#define AUXADC_IMP          0x0f

#define AUXADC_CHAN_MIN     AUXADC_BATADC
#define AUXADC_CHAN_MAX     AUXADC_IMP

enum val_type {
    PROCESSED,
    RAW,
};

struct auxadc_regs {
    u16 rqst_reg;
    u16 rqst_shift;
    u16 out_reg;
};

#define PMIC_AUXADC_REG(_ch_name, _chip, _rqst_reg, _rqst_shift, _out_reg) \
    [AUXADC_##_ch_name] = {                 \
        .rqst_reg = _chip##_##_rqst_reg,    \
        .rqst_shift = _rqst_shift,          \
        .out_reg = _chip##_##_out_reg,      \
    }                                       \

struct auxadc_device_chan_spec {
    int channel;
    unsigned char r_ratio[2];
    unsigned short avg_num;
    const struct auxadc_regs *regs;
};

#define PMIC_AUXADC_CHAN_SPEC(_ch_name, r0, r1, _avg_num) \
    {                                   \
        .channel = AUXADC_##_ch_name,   \
        .r_ratio = {r0, r1},            \
        .avg_num = _avg_num,            \
        .regs = &auxadc_regs_tbl[AUXADC_##_ch_name], \
    }

int pmic_get_auxadc_value_legacy(int channel, enum val_type type);
int pmic_auxadc_device_register_legacy(
    const struct auxadc_device_chan_spec *dev_chans, int num_chans);
