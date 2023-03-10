/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

/* DISP Mutex */
#define DISP_MUTEX_TOTAL      (10)
#define DISP_MUTEX_DDP_FIRST  (0)
#define DISP_MUTEX_DDP_LAST   (3) //modify from 4 to 3, cause 4 is used for OVL0/OVL1 SW trigger
#define DISP_MUTEX_DDP_COUNT  (4)
#define DISP_MUTEX_MDP_FIRST  (5)
#define DISP_MUTEX_MDP_COUNT  (5)

/* DISP MODULE */
enum DISP_MODULE_ENUM {
    /* must start from 0 */
    DISP_MODULE_OVL0 = 0, /* 0 */
    DISP_MODULE_OVL0_2L,
    DISP_MODULE_OVL0_2L_VIRTUAL0,
    DISP_MODULE_OVL0_VIRTUAL0,
    DISP_MODULE_RSZ0,

    DISP_MODULE_OVL1, /* 5 */
    DISP_MODULE_OVL1_2L,
    DISP_MODULE_OVL1_2L_VIRTUAL0,
    DISP_MODULE_OVL1_VIRTUAL0,
    DISP_MODULE_RSZ1,

    DISP_MODULE_RDMA0, /* 10 */
    DISP_MODULE_RDMA0_VIRTUAL0,
    DISP_MODULE_WDMA0,
    DISP_MODULE_COLOR0,
    DISP_MODULE_CCORR0,

    DISP_MODULE_RDMA1, /* 15 */
    DISP_MODULE_RDMA1_VIRTUAL0,
    DISP_MODULE_WDMA1,
    DISP_MODULE_COLOR1,
    DISP_MODULE_CCORR1,

    DISP_MODULE_AAL0, /* 20 */
    DISP_MODULE_MDP_AAL0,
    DISP_MODULE_GAMMA0,
    DISP_MODULE_POSTMASK0,
    DISP_MODULE_DITHER0,

    DISP_MODULE_DITHER0_VIRTUAL0, /* 25 */
    DISP_MODULE_AAL1,
    DISP_MODULE_MDP_AAL1,
    DISP_MODULE_GAMMA1,
    DISP_MODULE_POSTMASK1,

    DISP_MODULE_DITHER1, /* 30 */
    DISP_MODULE_DITHER1_VIRTUAL0,
    DISP_MODULE_CCORR1_1,
    DISP_MODULE_CCORR0_1,
    DISP_MODULE_CM0,

    DISP_MODULE_CM1, /* 35 */
    DISP_MODULE_SPR0,
    DISP_MODULE_SPR1,
    DISP_MODULE_TDSHP0,
    DISP_MODULE_TDSHP1,

    DISP_MODULE_C3D0, /* 40 */
    DISP_MODULE_C3D1,
    DISP_MODULE_SPLIT0,
    DISP_MODULE_DSI0,
    DISP_MODULE_DSI1,

    DISP_MODULE_DSIDUAL, /* 45 */
    DISP_MODULE_PWM0,
    DISP_MODULE_CONFIG,
    DISP_MODULE_MUTEX,
    DISP_MODULE_SMI_COMMON,

    DISP_MODULE_SMI_LARB0, /* 50 */
    DISP_MODULE_SMI_LARB1,
    DISP_MODULE_MIPI0,
    DISP_MODULE_MIPI1,
    DISP_MODULE_DPI,

    DISP_MODULE_OVL2_2L, /* 55 */
    DISP_MODULE_OVL3_2L,
    DISP_MODULE_RDMA4,
    DISP_MODULE_RDMA5,
    DISP_MODULE_MDP_RDMA4,

    DISP_MODULE_MDP_RDMA5, /* 60 */
    DISP_MODULE_MDP_RSZ4,
    DISP_MODULE_MDP_RSZ5,
    DISP_MODULE_MERGE0,
    DISP_MODULE_MERGE1,

    DISP_MODULE_DP_INTF, /* 65 */
    DISP_MODULE_DSC,
    DISP_MODULE_DSC_VIRTUAL0,
    DISP_MODULE_DSC_VIRTUAL1,
    DISP_MODULE_OVL0_VIRTUAL1,

    DISP_MODULE_RDMA0_OUT_RELAY, /* 70 */
    DISP_MODULE_PQ0_VIRTUAL,
    DISP_MODULE_MAIN0_VIRTUAL,
    DISP_MODULE_DLI_ASYNC0,
    DISP1_MODULE_CONFIG,

    DISP1_MODULE_MUTEX, /* 75 */
    DISP1_MODULE_SMI_COMMON,
    DISP_MODULE_SMI_LARB20,
    DISP_MODULE_SMI_LARB21,
    DISP1_MODULE_OVL0,

    DISP1_MODULE_OVL0_2L, /* 80 */
    DISP1_MODULE_OVL0_VIRTUAL0,
    DISP1_MODULE_OVL0_VIRTUAL1,
    DISP1_MODULE_RDMA0,
    DISP1_MODULE_RDMA0_OUT_RELAY,

    DISP1_MODULE_PQ0_VIRTUAL, /* 85 */
    DISP1_MODULE_MAIN0_VIRTUAL,
    DISP1_MODULE_DLO_ASYNC0,
    DISP1_MODULE_OVL1_2L,

    DISP_MODULE_UNKNOWN,
    DISP_MODULE_NUM
};

enum dst_module_type {
    DST_MOD_REAL_TIME,
    DST_MOD_WDMA,
};

static inline int check_ddp_module(enum DISP_MODULE_ENUM module)
{
    return module < DISP_MODULE_UNKNOWN;
}

enum OVL_LAYER_SOURCE {
    OVL_LAYER_SOURCE_MEM = 0,
    OVL_LAYER_SOURCE_RESERVED = 1,
    OVL_LAYER_SOURCE_SCL = 2,
    OVL_LAYER_SOURCE_PQ = 3,
};

enum CMDQ_SWITCH {
    CMDQ_DISABLE = 0,
    CMDQ_ENABLE
};

enum CMDQ_STATE {
    CMDQ_WAIT_LCM_TE,
    CMDQ_BEFORE_STREAM_SOF,
    CMDQ_WAIT_STREAM_EOF_EVENT,
    CMDQ_CHECK_IDLE_AFTER_STREAM_EOF,
    CMDQ_AFTER_STREAM_EOF,
    CMDQ_ESD_CHECK_READ,
    CMDQ_ESD_CHECK_CMP,
    CMDQ_ESD_ALLC_SLOT,
    CMDQ_ESD_FREE_SLOT,
    CMDQ_STOP_VDO_MODE,
    CMDQ_START_VDO_MODE,
    CMDQ_DSI_RESET,
    CMDQ_AFTER_STREAM_SOF,
    CMDQ_DSI_LFR_MODE,
};

