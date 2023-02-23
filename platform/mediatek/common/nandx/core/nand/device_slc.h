/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once
#include <nand_device.h>
#include <nandx_core.h>
#include <sys/types.h>

#define SLC_TIMING_NUM  (CHIP_TIMING_MODE5 + 1)

/*
 * timing mode
 *   onfi nand has timing mode setting by set feature
 *   but device would adjust it's timing mode when host sends timing on slc nand
 * @addr: feature address for timing mode
 * @mode: feature value for mode selection
 */
struct slc_timing_mode {
    u8 addr;
    u8 mode[SLC_TIMING_NUM];
};

/*
 * drive strength
 *    control signal strength, the greater the value, the higher the
 *    signal strength
 *    use SLC_DRIVE_STRENGTH macro to assign the value on device table
 * @ addr: feature address for nand device drive strength
 * @ normal: default value by manufacture
 * @ high: higher drive strength than default
 * @ middle: lower drive strength than default
 * @ low: the lowest drive strength
 */
struct slc_drive_strength {
    u8 addr;
    u8 normal;
    u8 high;
    u8 middle;
    u8 low;
};

/*
 * extend cmds
 * @read_multi_1st: first command of multi-plane read operation
 * @read_multi_2nd: second command of multi-plane read operation
 * @program_multi_1st: first command of multi-plane program operation
 * @program_multi_2nd: second command of multi-plane program operation
 * @erase_multi_1st: first command of multi-plane erase operation
 * @erase_multi_2nd: second command of multi-plane erase operation
 * @read_status_enhanced: enhanced command of read status
 */
struct slc_extend_cmds {
    short read_multi_1st;
    short read_multi_2nd;
    short program_multi_1st;
    short program_multi_2nd;
    short erase_multi_1st;
    short erase_multi_2nd;
    short read_status_enhanced;
};

/*
 * device_slc
 *    configurations of slc nand device table
 * @dev: base information of nand device
 * @drive_strength: feature information of nand drive strength
 * @timing_mode: feature information of nand timing mode
 * @extend_cmds: extended the nand base commands
 * @timing: nand operated sdr timing setting for NFI
 */
struct device_slc {
    struct nand_device dev;
    struct slc_drive_strength drive_strength;
    struct slc_extend_cmds *extend_cmds;
    enum chip_ctrl_timing_mode default_mode;
    struct slc_timing_mode *timing_mode;
    struct nand_sdr_timing *timing;
};

#define SLC_DRIVE_STRENGTH(_addr, _normal, _high, _middle, _low) \
    { _addr, _normal, _high, _middle, _low }

static inline struct device_slc *device_to_slc(struct nand_device *dev)
{
    return containerof(dev, struct device_slc, dev);
}

