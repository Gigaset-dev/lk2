/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <nand_device.h>
#include <nandx_core.h>
#include <nandx_util.h>

#include "device_slc.h"

/* onfi nand timing mode */
static struct slc_timing_mode timing_mode[] = {
    {
        .addr = 0x01,
        .mode = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05}
    },
};

/* onfi nand basic commands */
static struct nand_cmds onfi_cmds = {
    .reset = 0xff,
    .read_id = 0x90,
    .read_status = 0x70,
    .read_param_page = 0xec,
    .set_feature = 0xef,
    .get_feature = 0xee,
    .read_1st = 0x00,
    .read_2nd = 0x30,
    .random_out_1st = 0x05,
    .random_out_2nd = 0xe0,
    .program_1st = 0x80,
    .program_2nd = 0x10,
    .erase_1st = 0x60,
    .erase_2nd = 0xd0,
    .read_cache = 0x31,
    .read_cache_last = 0x3f,
    .program_cache = 0x15,
};

/* onfi nand extend commands */
static struct slc_extend_cmds onfi_extend_cmds = {
    .read_multi_1st = 0x00,
    .read_multi_2nd = 0x32,
    .program_multi_1st = 0x80,
    .program_multi_2nd = 0x11,
    .erase_multi_1st = 0x60,
    .erase_multi_2nd = 0xd1,
    .read_status_enhanced = 0x78
};

/* toggle nand basic commands */
static struct nand_cmds toggle_cmds = {
    .reset = 0xff,
    .read_id = 0x90,
    .read_status = 0x70,
    .read_param_page = 0x03,
    .set_feature = 0xef,
    .get_feature = 0xee,
    .read_1st = 0x00,
    .read_2nd = 0x30,
    .random_out_1st = 0x05,
    .random_out_2nd = 0xe0,
    .program_1st = 0x80,
    .program_2nd = 0x10,
    .erase_1st = 0x60,
    .erase_2nd = 0xd0,
    .read_cache = 0x31,
    .read_cache_last = 0x3f,
    .program_cache = 0x15,
};

/* toggle nand extend commands */
static struct slc_extend_cmds toggle_extend_cmds = {
    .read_multi_1st = 0x60,
    .read_multi_2nd = 0x30,
    .program_multi_1st = 0x80,
    .program_multi_2nd = 0x11,
    .erase_multi_1st = 0x60,
    .erase_multi_2nd = -1,
    .read_status_enhanced = 0xf1
};

static struct nand_endurance slc_endurance = {
    .pe_cycle = 60000,
    .ecc_req = 12,
    .max_bitflips = 8,
};

static struct nand_addressing slc_addressing = {
    .row_bit_start = 0,
    .block_bit_start = 6,
    .plane_bit_start = 6,
    .lun_bit_start = 18,
};

static struct nand_sdr_timing sdr_timing[6] = {
    { 40,  30,   0,  50,  50,  30, 120,  50,  50,  20,  20, 100, 100},
    { 30,  15,   0,  25,  25,  15,  80,  25,  25,  10,  10,  45,  50},
    { 25,  15,   0,  20,  17,  15,  80,  15,  15,  10,  10,  35,  35},
    { 20,  10,   0,  15,  15,  10,  60,  10,  10,   5,   5,  30,  30},
    { 20,  10,   0,  12,  12,  10,  60,  12,  12,   5,   5,  25,  25},
    { 16,   7,   0,  10,  10,   7,  60,  10,  10,   5,   5,  20,  20},
};

static struct nand_sdr_timing sdr_timing_micron[4] = {
    { 40,  30,   0,  50,  50,  30, 120,  50,  50,  20,  20, 100, 100},
    { 30,  15,   0,  25,  25,  15,  80,  25,  25,  10,  10,  45,  50},
    { 25,  15,   0,  20,  17,  15,  80,  15,  15,  10,  10,  35,  35},
    { 25,  10,   0,  15,  15,  10,  80,  10,  10,   5,   5,  30,  30},
};

static struct nand_sdr_timing sdr_timing_winbond[3] = {
    { 40,  30,   0,  50,  50,  30, 120,  50,  50,  20,  20, 100, 100},
    { 30,  15,   0,  25,  25,  15,  80,  25,  25,  10,  10,  45,  50},
    { 25,  15,   0,  20,  17,  15,  80,  15,  15,  10,  10,  35,  35},
};

static struct nand_status slc_status = {
    .array_busy = nandx_bit(5),
    .write_protect = nandx_bit(7),
    .erase_fail = nandx_bit(0),
    .program_fail = nandx_bit(0),
};

static struct nand_array_timing slc_array_timing = {
    .tRST = 500,
    .tWHR = 1,
    .tR = 25,
    .tRCBSY = 25,
    .tFEAT = 1,
    .tPROG = 600,
    .tPCBSY = 600,
    .tBERS = 10000,
    .tDBSY = 1,
};

static struct device_slc slc_nand[] = {
    {
        NAND_DEVICE("MT29F4G08ABA",
            NAND_PACK_ID(0x2c, 0xdc, 0x80, 0xa6, 0x62, 0, 0, 0),
            5, NAND_IO8, 3, 2,
            1, 1, 1, 4096,
            KB(256), KB(4), 256, 1,
            &onfi_cmds, &slc_addressing, &slc_status,
            &slc_endurance, &slc_array_timing),
        SLC_DRIVE_STRENGTH(0x80, 0x00, 0x01, 0x02, 0x03),
        &onfi_extend_cmds,
        CHIP_TIMING_MODE0,
        &timing_mode[0],
        sdr_timing
    },
    {
        NAND_DEVICE("MT29F4G08ABBFAH4-IT:F",
            NAND_PACK_ID(0x2c, 0xac, 0x80, 0x26, 0x62, 0, 0, 0),
            5, NAND_IO8, 3, 2,
            1, 1, 1, 2048,
            KB(256), KB(4), 256, 1,
            &onfi_cmds, &slc_addressing, &slc_status,
            &slc_endurance, &slc_array_timing),
        SLC_DRIVE_STRENGTH(0x80, 0x00, 0x01, 0x02, 0x03),
        &onfi_extend_cmds,
        CHIP_TIMING_MODE3,
        &timing_mode[0],
        sdr_timing_micron
    },
    {
        NAND_DEVICE("MT29F8G08ABBCAH4-IT:C",
            NAND_PACK_ID(0x2c, 0xa3, 0x90, 0x26, 0x64, 0, 0, 0),
            5, NAND_IO8, 3, 2,
            1, 1, 2, 2048,
            KB(256), KB(4), 224, 1,
            &onfi_cmds, &slc_addressing, &slc_status,
            &slc_endurance, &slc_array_timing),
        SLC_DRIVE_STRENGTH(0x80, 0x00, 0x01, 0x02, 0x03),
        &onfi_extend_cmds,
        CHIP_TIMING_MODE3,
        &timing_mode[0],
        sdr_timing_micron
    },
    {
        NAND_DEVICE("NM4484NSPAXAE-3E",
            NAND_PACK_ID(0x90, 0xac, 0x90, 0x26, 0x76, 0, 0, 0),
            5, NAND_IO8, 3, 2,
            1, 1, 1, 2048,
            KB(256), KB(4), 256, 1,
            &onfi_cmds, &slc_addressing, &slc_status,
            &slc_endurance, &slc_array_timing),
        SLC_DRIVE_STRENGTH(0x80, 0x00, 0x01, 0x02, 0x03),
        &onfi_extend_cmds,
        CHIP_TIMING_MODE0,
        &timing_mode[0],
        sdr_timing
    },
    {
        NAND_DEVICE("NM4888KSPAXAI",
            NAND_PACK_ID(0x98, 0xa3, 0x91, 0x26, 0x76, 0, 0, 0),
            5, NAND_IO8, 3, 2,
            1, 1, 1, 4096,
            KB(256), KB(4), 256, 1,
            &onfi_cmds, &slc_addressing, &slc_status,
            &slc_endurance, &slc_array_timing),
        SLC_DRIVE_STRENGTH(0x80, 0x00, 0x01, 0x02, 0x03),
        &onfi_extend_cmds,
        CHIP_TIMING_MODE3,
        NULL,
        sdr_timing
    },
    {
        NAND_DEVICE("TC58NVG2S0HTA00",
            NAND_PACK_ID(0x98, 0xdc, 0x90, 0x26, 0x76, 0x16, 0, 0),
            6, NAND_IO8, 3, 2,
            1, 1, 1, 2048,
            KB(256), KB(4), 256, 1,
            &toggle_cmds, &slc_addressing, &slc_status,
            &slc_endurance, &slc_array_timing),
        SLC_DRIVE_STRENGTH(0x10, 0x04, 0x06, 0x04, 0x02),
        &toggle_extend_cmds,
        CHIP_TIMING_MODE0,
        NULL,
        sdr_timing
    },
    {
        NAND_DEVICE("W29N04GZBIBA",
            NAND_PACK_ID(0xef, 0xac, 0x90, 0x15, 0x54, 0, 0, 0),
            5, NAND_IO8, 3, 2,
            1, 1, 2, 2048,
            KB(128), KB(2), 64, 1,
            &onfi_cmds, &slc_addressing, &slc_status,
            &slc_endurance, &slc_array_timing),
        SLC_DRIVE_STRENGTH(0x80, 0x00, 0x01, 0x02, 0x03),
        &onfi_extend_cmds,
        CHIP_TIMING_MODE2,
        &timing_mode[0],
        sdr_timing_winbond
    },
    {
        NAND_DEVICE("W29N08GZBIBA",
            NAND_PACK_ID(0xef, 0xa3, 0x91, 0x15, 0x58, 0, 0, 0),
            5, NAND_IO8, 3, 2,
            1, 2, 2, 2048,
            KB(128), KB(2), 64, 1,
            &onfi_cmds, &slc_addressing, &slc_status,
            &slc_endurance, &slc_array_timing),
        SLC_DRIVE_STRENGTH(0x80, 0x00, 0x01, 0x02, 0x03),
        &onfi_extend_cmds,
        CHIP_TIMING_MODE2,
        &timing_mode[0],
        sdr_timing_winbond
    },
    {
        NAND_DEVICE("NO-DEVICE",
            NAND_PACK_ID(0, 0, 0, 0, 0, 0, 0, 0),
            0, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 1,
            &onfi_cmds, &slc_addressing, &slc_status,
            &slc_endurance, &slc_array_timing),
        SLC_DRIVE_STRENGTH(0x80, 0x00, 0x01, 0x02, 0x03),
        NULL,
        CHIP_TIMING_MODE0,
        NULL,
        sdr_timing
    }
};

struct nand_device *nand_get_device(int index)
{
    if (index < 0 || (u32)index >= countof(slc_nand))
        return NULL;

    return &slc_nand[index].dev;
}
