/*
 * copyright (c) 2021 mediatek inc.
 *
 * use of this source code is governed by a mit-style
 * license that can be found in the license file or at
 * https://opensource.org/licenses/mit
 */

#pragma once

#include <sys/types.h>

#define AUXADC_RES_MAX  31
/**
 * enum auxadc_channels - enumeration of auxadc channels
 */
enum auxadc_channel {
    AUXADC_CHAN_BATADC,
    AUXADC_CHAN_ISENSE,
    AUXADC_CHAN_VCDT,
    AUXADC_CHAN_BAT_TEMP,
    AUXADC_CHAN_BATID,
    AUXADC_CHAN_CHIP_TEMP,
    AUXADC_CHAN_VCORE_TEMP,
    AUXADC_CHAN_VPROC_TEMP,
    AUXADC_CHAN_VGPU_TEMP,
    AUXADC_CHAN_ACCDET,
    AUXADC_CHAN_VDCXO,
    AUXADC_CHAN_TSX_TEMP,
    AUXADC_CHAN_HPOFS_CAL,
    AUXADC_CHAN_DCXO_TEMP,
    AUXADC_CHAN_VBIF,
    AUXADC_CHAN_VTREF,
    AUXADC_CHAN_IMP,
    AUXADC_CHAN_VSYSSNS,
    AUXADC_CHAN_VIN1,
    AUXADC_CHAN_VIN2,
    AUXADC_CHAN_VIN3,
    AUXADC_CHAN_VIN4,
    AUXADC_CHAN_VIN5,
    AUXADC_CHAN_VIN6,
    AUXADC_CHAN_VIN7,
    AUXADC_CHAN_MAX,
};

/**
 * enum auxadc_val_type - enumeration of auxadc value's type
 */
enum auxadc_val_type {
    AUXADC_VAL_PROCESSED,
    AUXADC_VAL_RAW,
};

/*
 * enum sdmadc_pures - enumeration of sdmadc pull up resistor
 */
enum sdmadc_pures {
    SDMADC_100K,
    SDMADC_30K,
    SDMADC_400K,
    SDMADC_OPEN,
};

/**
 * struct auxadc_chan_spec - specification of an auxadc channel
 * @channel:        What auxadc_channel is it.
 * @hw_info:        Hardware design level information
 * @hw_info.ref_volt:       Reference voltage
 * @hw_info.ratio:          Resistor Ratio.
 *                          If read operation is not provided, the processed
 *                          value will be calculated as following:
 *                          (raw value * ratio[0] * ref_volt) / ratio[1]
 * @hw_info.max_time:       Maximum wait time(us).
 * @hw_info.min_time:       Minimum wait time(us).
 * @hw_info.poll_time:      Wait time(us) between every polling during retry.
 * @hw_info.enable_reg:     Register address of enable control.
 * @hw_info.enable_mask:    Mask of enable control.
 * @hw_info.ready_reg:      Register address of channel's ready status.
 * @hw_info.ready_mask:     Mask of ready status.
 * @hw_info.value_reg:      Register address of raw value.
 * @hw_info.res:            Resolution of this channel (number of valid bits).
 */
struct auxadc_chan_spec {
    int channel;
    struct {
        u32 ref_volt;
        u32 ratio[2];
        u32 max_time;
        u32 min_time;
        u32 poll_time;
        u32 enable_reg;
        u32 enable_mask;
        u32 ready_reg;
        u32 ready_mask;
        u32 value_reg;
        u8 res;
    } hw_info;
    struct {
        u32 set_reg;
        u32 cmd;
    } sdmadc_hw_info;
};

/**
 * struct auxadc_device - structure of an auxadc device
 * @name:       Name of this auxadc device, which is also used for user to look
 *              up for.
 * @chans:      All channels supported by this device.
 * @num_chans:  Number of channels.
 * @write_regs:  Operation to write to multiple registers.
 * @read_regs:   Operation to read from multiple registers.
 * @read:       Operation to read value from a channel.
 */
struct auxadc_device {
    const char *name;
    const struct auxadc_chan_spec *chans;
    int num_chans;
    void *driver_data;
    int (*write_regs)(const struct auxadc_device *auxadc_dev, u32 reg,
                      const u8 *val, size_t len);
    int (*read_regs)(const struct auxadc_device *auxadc_dev, u32 reg, u8 *val,
                     size_t len);
    int (*read)(const struct auxadc_device *auxadc_dev,
                const struct auxadc_chan_spec *chan, int *val,
                enum auxadc_val_type type);
    int (*sdmadc_read)(const struct auxadc_device *auxadc_dev,
                       const struct auxadc_chan_spec *chan, int *val,
                       enum sdmadc_pures pures, enum auxadc_val_type type);
};

static inline void *
pmic_auxadc_get_drvdata(const struct auxadc_device *auxadc_dev)
{
    return auxadc_dev->driver_data;
}

static inline void pmic_auxadc_set_drvdata(struct auxadc_device *auxadc_dev,
                                           void *data)
{
    auxadc_dev->driver_data = data;
}

struct auxadc;

int pmic_auxadc_read_value(const struct auxadc *auxadc, int channel, int *val,
                           enum auxadc_val_type type);
int pmic_auxadc_read_value_ext(int channel, int *val,
                               enum auxadc_val_type type);
int pmic_sdmadc_read_value(const struct auxadc *auxadc, int channel, int *val,
                           enum sdmadc_pures pures, enum auxadc_val_type type);
struct auxadc *pmic_auxadc_get_by_name(const char *name);
int pmic_auxadc_device_register(const struct auxadc_device *auxadc_dev);
int pmic_auxadc_device_unregister(struct auxadc *auxadc);
