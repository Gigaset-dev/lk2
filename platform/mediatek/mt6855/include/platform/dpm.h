/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once

#include <platform/addressmap.h>

#define DRAM_CHANNEL_NUM         2
/*platform dependent address*/
#define DPM_DRAM_SIZE            0x00010000  // mblock_reserve requires max 64KB
#define DPM_DM_OFFSET            0x8000  // 32KB
#define DPM_CH_OFFSET            (1 << 20)

#define DPM_CFG_CH0              (DPM_CFG_CH0_BASE)
#define DPM_BARGS_CH0_REG0       ((DPM_CFG_CH0_BASE) + 0x6004)
#define DPM_BARGS_CH0_REG1       ((DPM_CFG_CH0_BASE) + 0x6008)


#define DPM_VERSION              0x00000000  // DPM version
#define DPM_HWID                 0x00006855  // DPM HWID
#define DPM_NUM                  0x00000001  // DPM number
#define DPM_HEAD_SIZE            0x00000020  // DPM header size

#define DRAM_ADDR_MAX            0xFFFFFFFF  // max address can be remap
#define DPM_DM_OFFSET            0x8000  // 32KB
#define DPM_MAX_LEN              0x600   // dpm db cfg
#define DPM_DBG_LEN              0x48    // dpm dbg latch
#define DPM_CFG1_LEN             0x14    // dpm cfg1 latch
#define DPM_CFG2_LEN             0x8     // dpm cfg2 latch
#define DRAM_CHANNEL             0x2     // dramc AO latch
#define DDRPHY_LATCH_LEN         0x20    // ddrphy AO latch

#define DPM_DBG_LATCH_CH0        ((DPM_CFG_CH0_BASE) + 0x7380)
#define DPM_CFG1_CH0             ((DPM_CFG_CH0_BASE) + 0x64)
#define DPM_CFG2_CH0             ((DPM_CFG_CH0_BASE) + 0x14C)

#define DDRPHY_AO_CH0            (DDRPHY_AO_CH0_BASE)
#define DDRPHY_LATCH_OFFSET      0x1600
#define CHANNEL_OFFSET           0x10000
/*image name definition*/
#define DPM_PM                   "dpmpm"
#define DPM_DM                   "dpmdm"
#define DPM_PT                   "dpmpt"
#define DPM_IMG                  "dpm"

#define DPMPT_LENGTH             0x40000
#define DPM_BUF_LENGTH           (0x9000 * DPM_NUM)
