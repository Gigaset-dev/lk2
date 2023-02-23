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
    DISP_MODULE_CONFIG = 0,
    DISP_MODULE_MUTEX,
    DISP_MODULE_SMI_COMMON,
    DISP_MODULE_SMI_LARB0,
    DISP_MODULE_SMI_LARB1,

    DISP_MODULE_OVL0,
    DISP_MODULE_OVL0_2L,
    DISP_MODULE_RDMA0,
    DISP_MODULE_RSZ0,
    DISP_MODULE_COLOR0,

    DISP_MODULE_CCORR0,
    DISP_MODULE_AAL0,
    DISP_MODULE_GAMMA0,
    DISP_MODULE_POSTMASK0,

    DISP_MODULE_DITHER0,
    DISP_MODULE_DSC_WRAP0_CORE0,
    DISP_MODULE_DSI0,

    DISP_MODULE_WDMA0,
    DISP_MODULE_PWM0,
    DISP_MODULE_MIPI0,
    DISP_MODULE_SPLIT0,
    DISP_MODULE_UNKNOWN,

    /*just for build pass*/
    DISP_MODULE_DSI1,
    DISP_MODULE_DSIDUAL,

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

enum OVL_LAYER_SECURE_MODE {
    OVL_LAYER_NORMAL_BUFFER = 0,
    OVL_LAYER_SECURE_BUFFER = 1,
    OVL_LAYER_PROTECTED_BUFFER = 2
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

enum DDP_IRQ_LEVEL {
    DDP_IRQ_LEVEL_ALL = 0,
    DDP_IRQ_LEVEL_NONE,
    DDP_IRQ_LEVEL_ERROR
};
