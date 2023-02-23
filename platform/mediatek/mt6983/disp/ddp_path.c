/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#define LOG_TAG "ddp_path"

/* #include "mt_irq.h" */
#include "disp_drv_platform.h"
#include "ddp_reg.h"
#include "ddp_reg_pq.h"
#include "ddp_path.h"
#include "ddp_info.h"
#include "ddp_log.h"

extern struct ddp_module ddp_modules[DISP_MODULE_NUM];

#define DDP_ENING_NUM    (30) /* max module num of path */

#define DDP_MOUT_MAX     8
#define DDP_SOUT_MAX     8
#define DDP_MUTEX_MAX    4

/* CHIST path select*/
#define DISP_CHIST_FROM_RDMA_POS  0
#define DISP_CHIST_FROM_POSTMASK  1
#define DISP_CHIST_FROM_DITHER    2

/* customer modify:chist0 after ovl, chist1 after dither*/
#define CHIST0_PATH_CONNECT DISP_CHIST_FROM_DITHER
#define CHIST1_PATH_CONNECT DISP_CHIST_FROM_RDMA_POS

/* struct for mutex mod */
struct module_map_t {
    enum DISP_MODULE_ENUM module;
    int bit;
    int mod_num;
};

struct m_to_b {
    int m;
    int v;
};

struct mout_t {
    int id;
    struct m_to_b out_id_bit_map[DDP_MOUT_MAX];
    unsigned long *reg;
    unsigned int reg_val;
};

struct sel_t {
    int id;
    int id_bit_map[DDP_SOUT_MAX];
    unsigned long *reg;
    unsigned int reg_val;
};

int primary_disp_dsc_module_list[DDP_ENING_NUM] = {
    DISP_MODULE_CHIST0, DISP_MODULE_CHIST1,
    DISP_MODULE_OVL0_2L, DISP_MODULE_OVL0, DISP_MODULE_OVL0_VIRTUAL0,
    DISP_MODULE_OVL0_VIRTUAL1,
#if 1
    DISP_MODULE_RDMA0,
#endif
    DISP_MODULE_TDSHP0, DISP_MODULE_COLOR0,
    DISP_MODULE_CCORR0, DISP_MODULE_CCORR1,
    DISP_MODULE_C3D0, DISP_MODULE_MDP_AAL0,
    DISP_MODULE_AAL0, DISP_MODULE_GAMMA0,
    DISP_MODULE_POSTMASK0, DISP_MODULE_DITHER0,
    DISP_MODULE_CM0, DISP_MODULE_SPR0,
    DISP_MODULE_PQ0_VIRTUAL, DISP_MODULE_DSC,
    DISP_MODULE_MAIN0_VIRTUAL,
#if 0
    DISP_MODULE_RDMA0, DISP_MODULE_RDMA0_OUT_RELAY,
#endif
    DISP_MODULE_PWM0, DISP_MODULE_DSI0,
    -1,
};

int primary_disp_dsc_left_module_list[DDP_ENING_NUM] = {
    DISP_MODULE_CHIST0, DISP_MODULE_CHIST1,
    DISP_MODULE_OVL0_2L, DISP_MODULE_OVL0, DISP_MODULE_OVL0_VIRTUAL0,
    DISP_MODULE_OVL0_VIRTUAL1, DISP_MODULE_RDMA0,
    DISP_MODULE_TDSHP0, DISP_MODULE_COLOR0,
    DISP_MODULE_CCORR0, DISP_MODULE_CCORR1,
    DISP_MODULE_C3D0, DISP_MODULE_MDP_AAL0,
    DISP_MODULE_AAL0, DISP_MODULE_GAMMA0,
    DISP_MODULE_POSTMASK0, DISP_MODULE_DITHER0,
    DISP_MODULE_CM0, DISP_MODULE_SPR0,
    DISP_MODULE_PQ0_VIRTUAL, DISP_MODULE_DSC,
    DISP_MODULE_MAIN0_VIRTUAL,
    DISP_MODULE_PWM0, DISP_MODULE_DSI0,
    -1,
};

int primary_disp_dsc_right_module_list[DDP_ENING_NUM] = {
    DISP1_MODULE_CHIST0, DISP1_MODULE_CHIST1,
    DISP1_MODULE_OVL0_2L, DISP1_MODULE_OVL0, DISP1_MODULE_OVL0_VIRTUAL0,
    DISP1_MODULE_OVL0_VIRTUAL1, DISP1_MODULE_RDMA0,
    DISP_MODULE_TDSHP1, DISP_MODULE_COLOR1,
    DISP_MODULE_CCORR0_1, DISP_MODULE_CCORR1_1,
    DISP_MODULE_C3D1, DISP_MODULE_MDP_AAL1,
    DISP_MODULE_AAL1, DISP_MODULE_GAMMA1,
    DISP_MODULE_POSTMASK1, DISP_MODULE_DITHER1,
    DISP_MODULE_CM1, DISP_MODULE_SPR1,
    DISP1_MODULE_PQ0_VIRTUAL, DISP1_MODULE_DLO_ASYNC0,
    DISP_MODULE_DLI_ASYNC0, DISP_MODULE_DSC,
    DISP_MODULE_MAIN0_VIRTUAL,
    DISP_MODULE_DSI0,
    -1,
};

int module_list_scenario[DDP_SCENARIO_MAX][DDP_ENING_NUM] = {
    /* DDP_SCENARIO_PRIMARY_DISP */
    {
        DISP_MODULE_CHIST0, DISP_MODULE_CHIST1,
        DISP_MODULE_OVL0_2L, DISP_MODULE_OVL0, DISP_MODULE_OVL0_VIRTUAL0,
        DISP_MODULE_OVL0_VIRTUAL1,
#if 1
        DISP_MODULE_RDMA0,
#endif
        DISP_MODULE_TDSHP0, DISP_MODULE_COLOR0,
        DISP_MODULE_CCORR0, DISP_MODULE_CCORR1,
        DISP_MODULE_C3D0, DISP_MODULE_MDP_AAL0,
        DISP_MODULE_AAL0, DISP_MODULE_GAMMA0,
        DISP_MODULE_POSTMASK0, DISP_MODULE_DITHER0,
        DISP_MODULE_CM0, DISP_MODULE_SPR0,
        DISP_MODULE_PQ0_VIRTUAL, DISP_MODULE_MAIN0_VIRTUAL,
#if 0
        DISP_MODULE_RDMA0, DISP_MODULE_RDMA0_OUT_RELAY,
#endif
        DISP_MODULE_PWM0, DISP_MODULE_DSI0,
        -1,
    },

    /* DDP_SCENARIO_PRIMARY_BYPASS_PQ_DISP */
    {
        DISP_MODULE_OVL0_2L, DISP_MODULE_OVL0,
        DISP_MODULE_OVL0_VIRTUAL0,
        DISP_MODULE_RDMA0, DISP_MODULE_RDMA0_VIRTUAL0,
        DISP_MODULE_PWM0, DISP_MODULE_DSI0,
        -1,
    },

    /* DDP_SCENARIO_PRIMARY_RDMA0_COLOR0_DISP0 */
    {
        DISP_MODULE_RDMA0, DISP_MODULE_RDMA0_VIRTUAL0,
#ifdef DISP_COLOR_ON
        DISP_MODULE_COLOR0,
#endif
        DISP_MODULE_CCORR0, DISP_MODULE_MDP_AAL0,
        DISP_MODULE_AAL0, DISP_MODULE_GAMMA0,
        DISP_MODULE_POSTMASK0, DISP_MODULE_DITHER0,
        DISP_MODULE_PWM0, DISP_MODULE_DSI0,
        -1,
    },

    /* DDP_SCENARIO_PRIMARY_RDMA0_DISP */
    {
        DISP_MODULE_RDMA0, DISP_MODULE_RDMA0_OUT_RELAY,
        DISP_MODULE_PWM0, DISP_MODULE_DSI0,
        -1,
    },

    /* DDP_SCENARIO_PRIMARY_OVL_MEMOUT */
    {
        DISP_MODULE_OVL0_2L, DISP_MODULE_OVL0,
        DISP_MODULE_OVL0_VIRTUAL0,
        DISP_MODULE_WDMA0,
        -1,
    },

    /* DDP_SCENARIO_PRIMARY0_DISP0_MDP_AAL4 */
    {
        DISP_MODULE_OVL0_2L, DISP_MODULE_OVL0,
        DISP_MODULE_OVL0_VIRTUAL0,
        DISP_MODULE_RDMA0, DISP_MODULE_RDMA0_VIRTUAL0,
#ifdef DISP_COLOR_ON
        DISP_MODULE_COLOR0,
#endif
        DISP_MODULE_CCORR0, DISP_MODULE_MDP_AAL0,
        DISP_MODULE_AAL0, DISP_MODULE_GAMMA0,
        DISP_MODULE_POSTMASK0, DISP_MODULE_DITHER0,
        DISP_MODULE_PWM0, DISP_MODULE_DSI0,
        -1,
    },

    /* DDP_SCENARIO_PRIMARY0_OVL_PQ_MEMOUT */
    {
        DISP_MODULE_OVL0_2L, DISP_MODULE_OVL0,
        DISP_MODULE_OVL0_VIRTUAL0,
        DISP_MODULE_RDMA0, DISP_MODULE_RDMA0_VIRTUAL0,
#ifdef DISP_COLOR_ON
        DISP_MODULE_COLOR0,
#endif
        DISP_MODULE_CCORR0, DISP_MODULE_MDP_AAL0,
        DISP_MODULE_AAL0, DISP_MODULE_GAMMA0,
        DISP_MODULE_POSTMASK0, DISP_MODULE_DITHER0,
        DISP_MODULE_WDMA0, -1,
    },

    /* DDP_SCENARIO_PRIMARY_ALL */
    {
        DISP_MODULE_OVL0_2L, DISP_MODULE_OVL0,
        DISP_MODULE_OVL0_VIRTUAL0,
        DISP_MODULE_WDMA0,
        DISP_MODULE_RDMA0, DISP_MODULE_RDMA0_VIRTUAL0,
#ifdef DISP_COLOR_ON
        DISP_MODULE_COLOR0,
#endif
        DISP_MODULE_CCORR0, DISP_MODULE_AAL0, DISP_MODULE_GAMMA0,
        DISP_MODULE_POSTMASK0, DISP_MODULE_DITHER0,
        DISP_MODULE_PWM0, DISP_MODULE_DSI0,
        -1,
    },

    /* DDP_SCENARIO_SUB_DISP */
    {
        DISP_MODULE_OVL0_2L, DISP_MODULE_DPI,
        -1,
    },

    /* DDP_SCENARIO_SUB_RDMA1_DISP */
    {
        DISP_MODULE_DPI,
        -1,
    },

    /* DDP_SCENARIO_SUB_OVL2_2L_MEMOUT */
    {
        DISP_MODULE_OVL2_2L, DISP_MODULE_WDMA0,
        -1,
    },

    /* DDP_SCENARIO_SUB_ALL */
    {
        DISP_MODULE_OVL0_2L, DISP_MODULE_WDMA0,
        DISP_MODULE_DPI,
        -1,
    },

    /* DDP_SCENARIO_PRIMARY1_DISP */
    {
        DISP_MODULE_OVL1, DISP_MODULE_OVL1_2L,
        DISP_MODULE_OVL1_VIRTUAL0,
        DISP_MODULE_RDMA1, DISP_MODULE_RDMA1_VIRTUAL0,
#if 0
#ifdef DISP_COLOR_ON
        DISP_MODULE_COLOR0,
#endif
        DISP_MODULE_CCORR0, DISP_MODULE_AAL0, DISP_MODULE_GAMMA0,
        DISP_MODULE_POSTMASK0, DISP_MODULE_DITHER0,
#endif
        DISP_MODULE_PWM0, DISP_MODULE_DSI1,
        -1,
    },

    /* DDP_SCENARIO_PRIMARY_RDMA1_COLOR1_DISP1 */
    {
        DISP_MODULE_RDMA1, DISP_MODULE_RDMA1_VIRTUAL0,
#ifdef DISP_COLOR_ON
        DISP_MODULE_COLOR1,
#endif
        DISP_MODULE_CCORR1, DISP_MODULE_MDP_AAL1,
        DISP_MODULE_AAL1, DISP_MODULE_GAMMA1,
        DISP_MODULE_POSTMASK1, DISP_MODULE_DITHER1,
        DISP_MODULE_PWM0, DISP_MODULE_DSI1,
        -1,
    },

    /* DDP_SCENARIO_PRIMARY_RDMA1_COLOR1_DISP0 */
    {
        DISP_MODULE_RDMA1, DISP_MODULE_RDMA1_VIRTUAL0,
#ifdef DISP_COLOR_ON
        DISP_MODULE_COLOR1,
#endif
        DISP_MODULE_CCORR1, DISP_MODULE_MDP_AAL1,
        DISP_MODULE_AAL1, DISP_MODULE_GAMMA1,
        DISP_MODULE_POSTMASK1, DISP_MODULE_DITHER1,
        DISP_MODULE_PWM0, DISP_MODULE_DSI0,
        -1,
    },

    /* DDP_SCENARIO_PRIMARY1_OVL_MEMOUT */
    {
        DISP_MODULE_OVL1, DISP_MODULE_OVL1_2L,
        DISP_MODULE_OVL1_VIRTUAL0,
        DISP_MODULE_WDMA1,
        -1,
    },

    /* DDP_SCENARIO_PRIMARY1_DISP0_MDP_AAL5 */
    {
        DISP_MODULE_OVL1, DISP_MODULE_OVL1_2L,
        DISP_MODULE_OVL1_VIRTUAL0,
        DISP_MODULE_RDMA1, DISP_MODULE_RDMA1_VIRTUAL0,
#ifdef DISP_COLOR_ON
        DISP_MODULE_COLOR1,
#endif
        DISP_MODULE_CCORR1, DISP_MODULE_MDP_AAL1,
        DISP_MODULE_AAL1, DISP_MODULE_GAMMA1,
        DISP_MODULE_POSTMASK1, DISP_MODULE_DITHER1,
        DISP_MODULE_PWM0, DISP_MODULE_DSI0,
        -1,
    },

    /* DDP_SCENARIO_PRIMARY1_ALL */
    {
        DISP_MODULE_OVL1, DISP_MODULE_OVL1_2L,
        DISP_MODULE_OVL1_VIRTUAL0,
        DISP_MODULE_WDMA1,
        DISP_MODULE_RDMA1, DISP_MODULE_RDMA1_VIRTUAL0,
#ifdef DISP_COLOR_ON
        DISP_MODULE_COLOR1,
#endif
        DISP_MODULE_CCORR1, DISP_MODULE_AAL1, DISP_MODULE_GAMMA1,
        DISP_MODULE_POSTMASK1, DISP_MODULE_DITHER1,
        DISP_MODULE_PWM0, DISP_MODULE_DSI1,
        -1,
    },

    /* DDP_SCENARIO_PRIMARY_DISP_LEFT */
    {
        DISP_MODULE_OVL0_2L, DISP_MODULE_OVL0, DISP_MODULE_OVL0_VIRTUAL0,
        DISP_MODULE_RDMA0, DISP_MODULE_RDMA0_VIRTUAL0,
        DISP_MODULE_COLOR0, DISP_MODULE_CCORR0,
        DISP_MODULE_AAL0, DISP_MODULE_GAMMA0,
        DISP_MODULE_POSTMASK0, DISP_MODULE_DITHER0,
        DISP_MODULE_PWM0, DISP_MODULE_DSI0,
        -1,
    },

    /* DDP_SCENARIO_PRIMARY_DISP_RIGHT */
    {
        DISP_MODULE_OVL1_2L, DISP_MODULE_OVL1, DISP_MODULE_OVL1_VIRTUAL0,
        DISP_MODULE_RDMA1, DISP_MODULE_RDMA1_VIRTUAL0,
        DISP_MODULE_COLOR1, DISP_MODULE_CCORR1,
        DISP_MODULE_AAL1, DISP_MODULE_GAMMA1,
        DISP_MODULE_POSTMASK1, DISP_MODULE_DITHER1,
        DISP_MODULE_DSI1,
        -1, -1,
    },
};

/* 1st para is mout's input, 2nd para is mout's output */
static struct mout_t mout_map[] = {
    /* DISP_OVL0_BLEND_MOUT_EN */
    {DISP_MODULE_OVL0_VIRTUAL0,
        {{DISP_MODULE_RSZ0, 1 << 0},
         {DISP_MODULE_OVL0_VIRTUAL1, 1 << 4},
         {-1, 0} },
        0, 0},

    /* DISP_RDMA0_POS_MOUT_EN */
    {DISP_MODULE_OVL0_VIRTUAL1,
        {{DISP_MODULE_RDMA0, 1 << 1},
         {DISP_MODULE_TDSHP0, 1 << 0},
         {-1, 0} },
        0, 0},

    /* DISP_POSTMASK0_MOUT_EN */
    {DISP_MODULE_POSTMASK0,
        {{DISP_MODULE_DITHER0, 1 << 0},
         {-1, 0} },
        0, 0},

    /* DISP_DITHER0_MOUT_EN */
    {DISP_MODULE_DITHER0,
        {{DISP_MODULE_CM0, 1 << 0},
         {-1, 0} },
        0, 0},

    /* DISP_SPR0_MOUT_EN */
    {DISP_MODULE_SPR0,
        {{DISP_MODULE_PQ0_VIRTUAL, 1 << 2},
         {DISP_MODULE_DSC, 1 << 2},
         {-1, 0} },
        0, 0},

    /* DISP_DSC_WRAP0_MOUT_EN */
    {DISP_MODULE_DSC,
        {{DISP_MODULE_MAIN0_VIRTUAL, 1 << 0},
         {-1, 0} },
        0, 0},

    /* DISP1 path */
    /* DISP1_OVL0_BLEND_MOUT_EN */
    {DISP1_MODULE_OVL0_VIRTUAL0,
         {{DISP1_MODULE_OVL0_VIRTUAL1, 1 << 4},
         {-1, 0} },
        0, 0},

    /* DISP1_RDMA0_POS_MOUT_EN */
    {DISP1_MODULE_OVL0_VIRTUAL1,
        {{DISP1_MODULE_RDMA0, 1 << 1},
         {-1, 0} },
        0, 0},

    /* DISP1_POSTMASK0_MOUT_EN */
    {DISP_MODULE_POSTMASK1,
        {{DISP_MODULE_DITHER1, 1 << 0},
         {-1, 0} },
        0, 0},

    /* DISP1_DITHER0_MOUT_EN */
    {DISP_MODULE_DITHER1,
        {{DISP_MODULE_CM1, 1 << 0},
         {-1, 0} },
        0, 0},

    /* DISP1_SPR0_MOUT_EN */
    {DISP_MODULE_SPR1,
        {{DISP1_MODULE_PQ0_VIRTUAL, 1 << 2},
         {-1, 0} },
        0, 0},
};

static struct sel_t sel_out_map[] = {
    /* DISP_RDMA0_SOUT_SEL */
    {DISP_MODULE_RDMA0,
        {DISP_MODULE_RDMA0_OUT_RELAY,
         DISP_MODULE_TDSHP0,
         -1},
        0, 0},

    /* DISP_TDSHP0_SOUT_SEL */
    {DISP_MODULE_TDSHP0,
        {DISP_MODULE_C3D0,
         DISP_MODULE_COLOR0,
         -1},
        0, 0},

    /* DISP_CCORR1_SOUT_SEL */
    {DISP_MODULE_CCORR1,
        {DISP_MODULE_MDP_AAL0,
         DISP_MODULE_C3D0,
         -1},
        0, 0},

    /* DISP_C3D0_SOUT_SEL */
    {DISP_MODULE_C3D0,
        {DISP_MODULE_COLOR0,
         DISP_MODULE_MDP_AAL0,
         -1},
        0, 0},

    /* DISP_PQ0_SOUT_SEL */
    {DISP_MODULE_PQ0_VIRTUAL,
        {DISP_MODULE_UNKNOWN,
         DISP_MODULE_MAIN0_VIRTUAL,
         DISP_MODULE_UNKNOWN,
         DISP_MODULE_UNKNOWN,
         DISP_MODULE_DSC,
         -1},
        0, 0},

    /* DISP_MAIN0_SOUT_SEL */
    {DISP_MODULE_MAIN0_VIRTUAL,
        {DISP_MODULE_RDMA0,
         DISP_MODULE_DSI0,
         -1},
        0, 0},

    /* DISP_DLI0_SOUT_SEL */
    {DISP1_MODULE_DLO_ASYNC0,
        {DISP_MODULE_UNKNOWN,
         DISP_MODULE_DSC,
         -1},
        0, 0},

    /* DISPSYS1 path */
    /* DISP1_RDMA0_SOUT_SEL */
    {DISP1_MODULE_RDMA0,
        {DISP1_MODULE_RDMA0_OUT_RELAY,
         DISP_MODULE_TDSHP1,
         -1},
        0, 0},

    /* DISP1_TDSHP0_SOUT_SEL */
    {DISP_MODULE_TDSHP1,
        {DISP_MODULE_C3D1,
         DISP_MODULE_COLOR1,
         -1},
        0, 0},

    /* DISP1_CCORR1_SOUT_SEL */
    {DISP_MODULE_CCORR1_1,
        {DISP_MODULE_MDP_AAL1,
         DISP_MODULE_C3D1,
         -1},
        0, 0},

    /* DISP1_C3D0_SOUT_SEL */
    {DISP_MODULE_C3D1,
        {DISP_MODULE_COLOR1,
         DISP_MODULE_MDP_AAL1,
         -1},
        0, 0},

    /* DISP1_PQ0_SOUT_SEL */
    {DISP1_MODULE_PQ0_VIRTUAL,
        {DISP1_MODULE_DLO_ASYNC0,
         DISP1_MODULE_MAIN0_VIRTUAL,
         -1},
        0, 0},

    /* DISP1_MAIN0_SOUT_SEL */
    {DISP1_MODULE_MAIN0_VIRTUAL,
        {DISP_MODULE_UNKNOWN,
         DISP_MODULE_DSI1,
         -1},
        0, 0},
};

/* 1st para is sout's output, 2nd para is sout's input */
static struct sel_t sel_in_map[] = {
    /* DISP_MAIN_OVL_DISP_PQ0_SEL_IN */
    {DISP_MODULE_OVL0_VIRTUAL0,
        {DISP_MODULE_UNKNOWN,
         DISP_MODULE_UNKNOWN,
         DISP_MODULE_UNKNOWN,
         DISP_MODULE_OVL0,
         -1},
        0, 0},

    /* DISP_PQ0_SEL_IN */
    {DISP_MODULE_OVL0_VIRTUAL1,
        {DISP_MODULE_UNKNOWN,
         DISP_MODULE_OVL0_VIRTUAL0,
         -1},
        0, 0},

    /* DISP_RDMA0_SEL_IN */
    {DISP_MODULE_RDMA0,
        {DISP_MODULE_MAIN0_VIRTUAL,
         DISP_MODULE_OVL0_VIRTUAL1,
         -1},
        0, 0},

    /* DISP_DSI0_SEL_IN */
    {DISP_MODULE_DSI0,
        {DISP_MODULE_RDMA0_OUT_RELAY,
         DISP_MODULE_MAIN0_VIRTUAL,
         -1},
        0, 0},

    /* DISP_RDMA0_POS_SEL_IN */
    {DISP_MODULE_TDSHP0,
        {DISP_MODULE_OVL0_VIRTUAL1,
         DISP_MODULE_RDMA0,
         -1},
        0, 0},

    /* DISP_COLOR0_SEL_IN */
    {DISP_MODULE_COLOR0,
        {DISP_MODULE_C3D0,
         DISP_MODULE_TDSHP0,
         -1},
        0, 0},

    /* DISP_C3D0_SEL_IN */
    {DISP_MODULE_C3D0,
        {DISP_MODULE_TDSHP0,
         DISP_MODULE_CCORR1,
         -1},
        0, 0},

    /* DISP_MDP_AAL0_SEL_IN */
    {DISP_MODULE_MDP_AAL0,
        {DISP_MODULE_CCORR1,
         DISP_MODULE_C3D0,
         -1},
        0, 0},

    /* DISP_MAIN0_SEL_IN */
    {DISP_MODULE_MAIN0_VIRTUAL,
        {DISP_MODULE_UNKNOWN,
         DISP_MODULE_PQ0_VIRTUAL,
         DISP_MODULE_UNKNOWN,
         DISP_MODULE_DSC,
         -1},
        0, 0},

    /* DISP_DSC_WRAP0_L_SEL_IN */
    {DISP_MODULE_PQ0_VIRTUAL,
        {DISP_MODULE_DSC,
         -1},
        0, 0},

    /* DISP_DSC_WRAP0_R_SEL_IN */
    {DISP1_MODULE_DLO_ASYNC0,
        {DISP_MODULE_DSC,
         -1},
        0, 0},

    /* DISPSYS1 path */
    /* DISP1_MAIN_OVL_DISP_PQ0_SEL_IN */
    {DISP1_MODULE_OVL0_VIRTUAL0,
        {DISP_MODULE_UNKNOWN,
         DISP_MODULE_UNKNOWN,
         DISP_MODULE_UNKNOWN,
         DISP1_MODULE_OVL0,
         -1},
        0, 0},

    /* DISP1_PQ0_SEL_IN */
    {DISP1_MODULE_OVL0_VIRTUAL1,
        {DISP_MODULE_UNKNOWN,
         DISP1_MODULE_OVL0_VIRTUAL0,
         -1},
        0, 0},

    /* DISP1_RDMA0_SEL_IN */
    {DISP1_MODULE_RDMA0,
        {DISP_MODULE_UNKNOWN,
         DISP1_MODULE_OVL0_VIRTUAL1,
         -1},
        0, 0},

    /* DISP_DSI1_SEL_IN */
    {DISP_MODULE_DSI1,
        {DISP1_MODULE_RDMA0_OUT_RELAY,
         DISP1_MODULE_MAIN0_VIRTUAL,
         -1},
        0, 0},

    /* DISP1_RDMA0_POS_SEL_IN */
    {DISP_MODULE_TDSHP1,
        {DISP_MODULE_UNKNOWN,
         DISP1_MODULE_RDMA0,
         -1},
        0, 0},

    /* DISP1_COLOR0_SEL_IN */
    {DISP_MODULE_COLOR1,
        {DISP_MODULE_C3D1,
         DISP_MODULE_TDSHP1,
         -1},
        0, 0},

    /* DISP1_C3D0_SEL_IN */
    {DISP_MODULE_C3D1,
        {DISP_MODULE_TDSHP1,
         DISP_MODULE_CCORR1_1,
         -1},
        0, 0},

    /* DISP1_MDP_AAL0_SEL_IN */
    {DISP_MODULE_MDP_AAL1,
        {DISP_MODULE_CCORR1_1,
         DISP_MODULE_C3D1,
         -1},
        0, 0},

    /* DISP_MAIN0_SEL_IN */
    {DISP1_MODULE_MAIN0_VIRTUAL,
        {DISP_MODULE_UNKNOWN,
         DISP1_MODULE_PQ0_VIRTUAL,
         -1},
        0, 0},
};

int ddp_path_init(void)
{
    int i;

    /* mout */
    mout_map[0].reg = (unsigned long *)DISP_REG_CONFIG_DISP_OVL0_BLEND_MOUT_EN;
    mout_map[1].reg = (unsigned long *)DISP_REG_CONFIG_DISP_RDMA0_POS_MOUT_EN;
    mout_map[2].reg = (unsigned long *)DISP_REG_CONFIG_DISP_POSTMASK0_MOUT_EN;
    mout_map[3].reg = (unsigned long *)DISP_REG_CONFIG_DISP_DITHER0_MOUT_EN;
    mout_map[4].reg = (unsigned long *)DISP_REG_CONFIG_DISP_SPR0_MOUT_EN;
    mout_map[5].reg = (unsigned long *)DISP_REG_CONFIG_DISP_DSC_WRAP0_MOUT_EN;

    mout_map[6].reg = (unsigned long *)DISP1_REG_CONFIG_DISP_OVL0_BLEND_MOUT_EN;
    mout_map[7].reg = (unsigned long *)DISP1_REG_CONFIG_DISP_RDMA0_POS_MOUT_EN;
    mout_map[8].reg = (unsigned long *)DISP1_REG_CONFIG_DISP_POSTMASK0_MOUT_EN;
    mout_map[9].reg = (unsigned long *)DISP1_REG_CONFIG_DISP_DITHER0_MOUT_EN;
    mout_map[10].reg = (unsigned long *)DISP1_REG_CONFIG_DISP_SPR0_MOUT_EN;


    /* sel_out */
    sel_out_map[0].reg =
        (unsigned long *)DISP_REG_CONFIG_DISP_RDMA0_SOUT_SEL;
    sel_out_map[1].reg =
        (unsigned long *)DISP_REG_CONFIG_DISP_TDSHP0_SOUT_SEL;
    sel_out_map[2].reg =
        (unsigned long *)DISP_REG_CONFIG_DISP_CCORR1_SOUT_SEL;
    sel_out_map[3].reg =
        (unsigned long *)DISP_REG_CONFIG_DISP_C3D0_SOUT_SEL;
    sel_out_map[4].reg =
        (unsigned long *)DISP_REG_CONFIG_DISP_PQ0_SOUT_SEL;
    sel_out_map[5].reg =
        (unsigned long *)DISP_REG_CONFIG_DISP_MAIN0_SOUT_SEL;
    sel_out_map[6].reg =
        (unsigned long *)DISP_REG_CONFIG_DISP_DLI0_SOUT_SEL;

    sel_out_map[7].reg =
        (unsigned long *)DISP1_REG_CONFIG_DISP_RDMA0_SOUT_SEL;
    sel_out_map[8].reg =
        (unsigned long *)DISP1_REG_CONFIG_DISP_TDSHP0_SOUT_SEL;
    sel_out_map[9].reg =
        (unsigned long *)DISP1_REG_CONFIG_DISP_CCORR1_SOUT_SEL;
    sel_out_map[10].reg =
        (unsigned long *)DISP1_REG_CONFIG_DISP_C3D0_SOUT_SEL;
    sel_out_map[11].reg =
        (unsigned long *)DISP1_REG_CONFIG_DISP_PQ0_SOUT_SEL;
    sel_out_map[12].reg =
        (unsigned long *)DISP1_REG_CONFIG_DISP_MAIN0_SOUT_SEL;


    /* sel_in */
    sel_in_map[0].reg =
        (unsigned long *)DISP_REG_CONFIG_DISP_MAIN_OVL_DISP_PQ0_SEL_IN;
    sel_in_map[1].reg =
        (unsigned long *)DISP_REG_CONFIG_DISP_PQ0_SEL_IN;
    sel_in_map[2].reg =
        (unsigned long *)DISP_REG_CONFIG_DISP_RDMA0_SEL_IN;
    sel_in_map[3].reg =
        (unsigned long *)DISP_REG_CONFIG_DISP_DSI0_SEL_IN;
    sel_in_map[4].reg =
        (unsigned long *)DISP_REG_CONFIG_DISP_RDMA0_POS_SEL_IN;
    sel_in_map[5].reg =
        (unsigned long *)DISP_REG_CONFIG_DISP_COLOR0_SEL_IN;
    sel_in_map[6].reg =
        (unsigned long *)DISP_REG_CONFIG_DISP_C3D0_SEL_IN;
    sel_in_map[7].reg =
        (unsigned long *)DISP_REG_CONFIG_DISP_MDP_AAL0_SEL_IN;
    sel_in_map[8].reg =
        (unsigned long *)DISP_REG_CONFIG_DISP_MAIN0_SEL_IN;
    sel_in_map[9].reg =
        (unsigned long *)DISP_REG_CONFIG_DISP_DSC_WRAP0_L_SEL_IN;
    sel_in_map[10].reg =
        (unsigned long *)DISP_REG_CONFIG_DISP_DSC_WRAP0_R_SEL_IN;

    sel_in_map[11].reg =
        (unsigned long *)DISP1_REG_CONFIG_DISP_MAIN_OVL_DISP_PQ0_SEL_IN;
    sel_in_map[12].reg =
        (unsigned long *)DISP1_REG_CONFIG_DISP_PQ0_SEL_IN;
    sel_in_map[13].reg =
        (unsigned long *)DISP1_REG_CONFIG_DISP_RDMA0_SEL_IN;
    sel_in_map[14].reg =
        (unsigned long *)DISP1_REG_CONFIG_DISP_DSI0_SEL_IN;
    sel_in_map[15].reg =
        (unsigned long *)DISP1_REG_CONFIG_DISP_RDMA0_POS_SEL_IN;
    sel_in_map[16].reg =
        (unsigned long *)DISP1_REG_CONFIG_DISP_COLOR0_SEL_IN;
    sel_in_map[17].reg =
        (unsigned long *)DISP1_REG_CONFIG_DISP_C3D0_SEL_IN;
    sel_in_map[18].reg =
        (unsigned long *)DISP1_REG_CONFIG_DISP_MDP_AAL0_SEL_IN;
    sel_in_map[19].reg =
        (unsigned long *)DISP1_REG_CONFIG_DISP_MAIN0_SEL_IN;

    for (i = 0; i < 16; i++) {
        DISP_REG_SET_FIELD(NULL, REG_FLD_MMU_EN,
            DISP_REG_SMI_LARB0_NON_SEC_CON + i * 4, 0x0);
        DISP_REG_SET_FIELD(NULL, REG_FLD_MMU_EN,
            DISP_REG_SMI_LARB1_NON_SEC_CON + i * 4, 0x0);
    }

    return 0;
}

static struct module_map_t module_mutex_map[DISP_MODULE_NUM] = {
    {DISP_MODULE_OVL0, 0, 0}, /* 0 */
    {DISP_MODULE_OVL0_2L, 2, 0},
    {DISP_MODULE_OVL0_2L_VIRTUAL0, -1, 0},
    {DISP_MODULE_OVL0_VIRTUAL0, -1, 0},
    {DISP_MODULE_RSZ0, 3, 0},

    {DISP_MODULE_OVL1, -1, 0}, /* 5 */
    {DISP_MODULE_OVL1_2L, 1, 0},//
    {DISP_MODULE_OVL1_2L_VIRTUAL0, -1, 0},
    {DISP_MODULE_OVL1_VIRTUAL0, -1, 0},
    {DISP_MODULE_RSZ1, 3, 0},

    {DISP_MODULE_RDMA0, 4, 0}, /* 10 */
    {DISP_MODULE_RDMA0_VIRTUAL0, -1, 0},
    {DISP_MODULE_WDMA0, 23, 0},
    {DISP_MODULE_COLOR0, 7, 0},
    {DISP_MODULE_CCORR0, 8, 0},

    {DISP_MODULE_RDMA1, 27, 0}, /* 15 */
    {DISP_MODULE_RDMA1_VIRTUAL0, -1, 0},
    {DISP_MODULE_WDMA1, 30, 0},
    {DISP_MODULE_COLOR1, 7, 20},
    {DISP_MODULE_CCORR1, 9, 0},

    {DISP_MODULE_AAL0, 11, 0}, /* 20 */
    {DISP_MODULE_MDP_AAL0, 10, 0},
    {DISP_MODULE_GAMMA0, 12, 0},
    {DISP_MODULE_POSTMASK0, 13, 0},
    {DISP_MODULE_DITHER0, 14, 0},

    {DISP_MODULE_DITHER0_VIRTUAL0, -1, 0}, /* 25 */
    {DISP_MODULE_AAL1, 11, 20},
    {DISP_MODULE_MDP_AAL1, 10, 20},
    {DISP_MODULE_GAMMA1, 12, 20},
    {DISP_MODULE_POSTMASK1, 13, 20},

    {DISP_MODULE_DITHER1, 14, 20}, /* 30 */
    {DISP_MODULE_DITHER1_VIRTUAL0, -1, 0},
    {DISP_MODULE_CCORR1_1, 9, 20},
    {DISP_MODULE_CCORR0_1, 8, 20},
    {DISP_MODULE_CM0, 17, 0},

    {DISP_MODULE_CM1, 17, 20},  /* 35 */
    {DISP_MODULE_SPR0, 18, 0},
    {DISP_MODULE_SPR1, 18, 20},
    {DISP_MODULE_TDSHP0, 5, 0},
    {DISP_MODULE_TDSHP1, 5, 20},

    {DISP_MODULE_C3D0, 6, 0},  /* 40 */
    {DISP_MODULE_C3D1, 6, 20},
    {DISP_MODULE_SPLIT0, -1, 0},
    {DISP_MODULE_DSI0, 22, 0},
    {DISP_MODULE_DSI1, 22, 20},

    {DISP_MODULE_DSIDUAL, -1, 0}, /* 45 */
    {DISP_MODULE_PWM0, 9, 1},
    {DISP_MODULE_CONFIG, -1, 0},
    {DISP_MODULE_MUTEX, -1, 0},
    {DISP_MODULE_SMI_COMMON, -1, 0},

    {DISP_MODULE_SMI_LARB0, -1, 0}, /* 50 */
    {DISP_MODULE_SMI_LARB1, -1, 0},
    {DISP_MODULE_MIPI0, -1, 0},
    {DISP_MODULE_MIPI1, -1, 0},
    {DISP_MODULE_DPI, 29, 0},

    {DISP_MODULE_OVL2_2L, -1, 0}, /* 55 */
    {DISP_MODULE_OVL3_2L, -1, 0},
    {DISP_MODULE_RDMA4, -1, 0},
    {DISP_MODULE_RDMA5, -1, 0},
    {DISP_MODULE_MDP_RDMA4, -1, 0},

    {DISP_MODULE_MDP_RDMA5, -1, 0}, /* 60 */
    {DISP_MODULE_MDP_RSZ4, -1, 0},
    {DISP_MODULE_MDP_RSZ5, -1, 0},
    {DISP_MODULE_MERGE0, 21, 0},
    {DISP_MODULE_MERGE1, 21, 20},

    {DISP_MODULE_DP_INTF, 28, 0}, /* 65 */
    {DISP_MODULE_DSC, 19, 0}, //2 //core1:19, core2:20
    {DISP_MODULE_DSC_VIRTUAL0, -1, 0},
    {DISP_MODULE_DSC_VIRTUAL1, -1, 0},
    {DISP_MODULE_OVL0_VIRTUAL1, -1, 0},

    {DISP_MODULE_RDMA0_OUT_RELAY, -1, 0}, /* 70 */
    {DISP_MODULE_PQ0_VIRTUAL, -1, 0},
    {DISP_MODULE_MAIN0_VIRTUAL, -1, 0},
    {DISP_MODULE_DLI_ASYNC0, 1, 1},
    {DISP1_MODULE_CONFIG, -1, 0},

    {DISP1_MODULE_MUTEX, -1, 0},  /* 75 */
    {DISP1_MODULE_SMI_COMMON, -1, 0},
    {DISP_MODULE_SMI_LARB20, -1, 0},
    {DISP_MODULE_SMI_LARB21, -1, 0},
    {DISP1_MODULE_OVL0, 0, 20},

    {DISP1_MODULE_OVL0_2L, 2, 20}, /* 80 */
    {DISP1_MODULE_OVL0_VIRTUAL0, -1, 0},
    {DISP1_MODULE_OVL0_VIRTUAL1, -1, 0},
    {DISP1_MODULE_RDMA0, 4, 20},
    {DISP1_MODULE_RDMA0_OUT_RELAY, -1, 0},

    {DISP1_MODULE_PQ0_VIRTUAL, -1, 0},  /* 85 */
    {DISP1_MODULE_MAIN0_VIRTUAL, -1, 0},
    {DISP1_MODULE_DLO_ASYNC0, 5, 21},
    {DISP_MODULE_CHIST0, 15, 0},
    {DISP_MODULE_CHIST1, 16, 0},

    {DISP1_MODULE_CHIST0, 15, 20},  /* 90 */
    {DISP1_MODULE_CHIST1, 16, 20},

    {DISP_MODULE_UNKNOWN, -1, 0},
};


const char *ddp_get_scenario_name(enum DDP_SCENARIO_ENUM scenario)
{
    switch (scenario) {
    /* primary display */
    case DDP_SCENARIO_PRIMARY_DISP:
        return "primary_disp";
    case DDP_SCENARIO_PRIMARY_BYPASS_PQ_DISP:
        return "primary_bypass_pq";
    case DDP_SCENARIO_PRIMARY_RDMA0_COLOR0_DISP0:
        return "primary_rdma0_color0_disp0";
    case DDP_SCENARIO_PRIMARY_RDMA0_DISP:
        return "primary_rdma0_disp";
    case DDP_SCENARIO_PRIMARY_OVL_MEMOUT:
        return "primary_ovl_memout";
    case DDP_SCENARIO_PRIMARY_ALL:
        return "primary_all";

    /* sub display */
    case DDP_SCENARIO_SUB_DISP:
        return "sub_disp";
    case DDP_SCENARIO_SUB_RDMA1_DISP:
        return "sub_rdma1_disp";
    case DDP_SCENARIO_SUB_OVL2_2L_MEMOUT:
        return "sub_ovl2_2l_memout";
    case DDP_SCENARIO_SUB_ALL:
        return "sub_all";
    case DDP_SCENARIO_PRIMARY1_ALL:
        return "primary_disp1";
    case DDP_SCENARIO_PRIMARY_RDMA1_COLOR1_DISP1:
        return "primary_rdma1_color1_disp1";
    case DDP_SCENARIO_PRIMARY_RDMA1_COLOR1_DISP0:
        return "primary_rdma1_color1_disp0";
    case DDP_SCENARIO_PRIMARY_DISP_LEFT:
        return "primary_disp_left";
    case DDP_SCENARIO_PRIMARY_DISP_RIGHT:
        return "primary_disp_right";
    /* others */
    default:
        DDPMSG("invalid scenario id=%d\n", scenario);
        return "unknown";
    }
}

int ddp_is_scenario_on_primary(enum DDP_SCENARIO_ENUM scenario)
{
    int on_primary = 0;

    switch (scenario) {
    case DDP_SCENARIO_PRIMARY_DISP:
    case DDP_SCENARIO_PRIMARY_RDMA0_DISP:
    case DDP_SCENARIO_PRIMARY_OVL_MEMOUT:
    case DDP_SCENARIO_PRIMARY_ALL:
    case DDP_SCENARIO_PRIMARY_DISP_LEFT:
        on_primary = 1;
        break;
    default:
        DDPMSG("invalid scenario id=%d\n", scenario);
    }

    return on_primary;

}

const char *ddp_get_mutex_sof_name(unsigned int regval)
{
    if (regval == SOF_VAL_MUTEX0_SOF_SINGLE_MODE)
        return "single";
    else if (regval == SOF_VAL_MUTEX0_SOF_FROM_DSI0)
        return "dsi0";
    else if (regval == SOF_VAL_MUTEX0_SOF_FROM_DSI1)
        return "dsi1";
    else if (regval == SOF_VAL_MUTEX0_SOF_FROM_DPI)
        return "dpi";

    DDPDUMP("%s, unknown reg=%d\n", __func__, regval);
    return "unknown";
}

const char *ddp_get_mode_name(enum DDP_MODE ddp_mode)
{
    switch (ddp_mode) {
    case DDP_VIDEO_MODE:
        return "vido_mode";
    case DDP_CMD_MODE:
        return "cmd_mode";
    default:
        DDPMSG("invalid ddp mode =%d\n", ddp_mode);
        return "unknown";
    }
}

bool ddp_path_is_dual(enum DDP_SCENARIO_ENUM ddp_scenario)
{
    if (ddp_scenario == DDP_SCENARIO_PRIMARY_DISP_LEFT)
        return true;
    return false;
}

enum DDP_SCENARIO_ENUM ddp_get_dual_module(enum DDP_SCENARIO_ENUM ddp_scenario)
{
    if (ddp_scenario == DDP_SCENARIO_PRIMARY_DISP_LEFT)
        return DDP_SCENARIO_PRIMARY_DISP_RIGHT;

    DDPERR("scenario not dual path\n");
    return -1;
}

static int ddp_get_module_num_l(int *module_list)
{
    unsigned int num = 0;

    while (*(module_list + num) != -1) {
        num++;

        if (num == DDP_ENING_NUM)
            break;
    }
    return num;
}

static void ddp_connect_chist(void *handle)
{
    unsigned int mout = 0;

    DISP_REG_SET(handle, DISP_REG_CONFIG_DISP_CHIST0_SEL_IN,
                                 (uint16_t) CHIST0_PATH_CONNECT);
    DISP_REG_SET(handle, DISP1_REG_CONFIG_DISP_CHIST0_SEL_IN,
                                 (uint16_t) CHIST0_PATH_CONNECT);
    DISP_REG_SET(handle, DISP_REG_CONFIG_DISP_CHIST1_SEL_IN,
                                 (uint16_t) CHIST1_PATH_CONNECT);
    DISP_REG_SET(handle, DISP1_REG_CONFIG_DISP_CHIST1_SEL_IN,
                                 (uint16_t) CHIST1_PATH_CONNECT);

    if (CHIST0_PATH_CONNECT == DISP_CHIST_FROM_RDMA_POS) {
        mout = DISP_REG_GET(DISP_REG_CONFIG_DISP_RDMA0_POS_MOUT_EN);
        DISP_REG_SET(handle, DISP_REG_CONFIG_DISP_RDMA0_POS_MOUT_EN, (mout | 1 << 2));
        DISP_REG_SET(handle, DISP1_REG_CONFIG_DISP_RDMA0_POS_MOUT_EN, (mout | 1 << 2));
    } else if (CHIST0_PATH_CONNECT == DISP_CHIST_FROM_DITHER) {
        mout = DISP_REG_GET(DISP_REG_CONFIG_DISP_DITHER0_MOUT_EN);
        DISP_REG_SET(handle, DISP_REG_CONFIG_DISP_DITHER0_MOUT_EN, (mout | 1 << 1));
        DISP_REG_SET(handle, DISP1_REG_CONFIG_DISP_DITHER0_MOUT_EN, (mout | 1 << 1));
    }

    if (CHIST1_PATH_CONNECT == DISP_CHIST_FROM_RDMA_POS) {
        mout = DISP_REG_GET(DISP_REG_CONFIG_DISP_RDMA0_POS_MOUT_EN);
        DISP_REG_SET(handle, DISP_REG_CONFIG_DISP_RDMA0_POS_MOUT_EN, (mout | 1 << 3));
        DISP_REG_SET(handle, DISP1_REG_CONFIG_DISP_RDMA0_POS_MOUT_EN, (mout | 1 << 3));
    } else if (CHIST1_PATH_CONNECT == DISP_CHIST_FROM_DITHER) {
        mout = DISP_REG_GET(DISP_REG_CONFIG_DISP_DITHER0_MOUT_EN);
        DISP_REG_SET(handle, DISP_REG_CONFIG_DISP_DITHER0_MOUT_EN, (mout | 1 << 2));
        DISP_REG_SET(handle, DISP1_REG_CONFIG_DISP_DITHER0_MOUT_EN, (mout | 1 << 2));
    }
}

/* config mout/msel to creat a compelte path */
static void ddp_connect_path_l(int *module_list, void *handle)
{
    unsigned int j, k;
    int step = 0;
    unsigned int mout = 0;
    unsigned int reg_mout = 0;
    unsigned int mout_idx = 0;
    int module_num = ddp_get_module_num_l(module_list);

    DDPDBG("connect_path: %s to %s\n", ddp_get_module_name(module_list[0]),
           ddp_get_module_name(module_list[module_num - 1]));
    /* connect mout */
    for (int i = 0; i < module_num - 1; i++) {
        for (j = 0; j < ARRAY_SIZE(mout_map); j++) {
            if (module_list[i] == mout_map[j].id) {
                /* find next module which can be connected */
                step = i + 1;
                while (ddp_modules[module_list[step]].can_connect == 0
                       && step < module_num) {
                    step++;
                }
                ASSERT(step < module_num);
                mout = mout_map[j].reg_val;
                for (k = 0; k < DDP_MOUT_MAX; k++) {
                    if (mout_map[j].out_id_bit_map[k].m == -1)
                        break;
                    if (mout_map[j].out_id_bit_map[k].m == module_list[step]) {
                        mout |= mout_map[j].out_id_bit_map[k].v;
                        reg_mout |= mout;
                        mout_idx = j;
                        DDPDBG("connect mout %s to %s  bits 0x%x\n",
                               ddp_get_module_name(module_list[i]),
                               ddp_get_module_name(module_list[step]),
                               reg_mout);
                        break;
                    }
                }
                mout_map[j].reg_val = mout;
                mout = 0;
            }
        }
        if (reg_mout) {
            DISP_REG_SET(handle, mout_map[mout_idx].reg, reg_mout);
            reg_mout = 0;
            mout_idx = 0;
        }

    }
    /* connect out select */
    for (int i = 0; i < module_num - 1; i++) {
        for (j = 0; j < ARRAY_SIZE(sel_out_map); j++) {
            if (module_list[i] == sel_out_map[j].id) {
                step = i + 1;
                /* find next module which can be connected */
                while (ddp_modules[module_list[step]].can_connect == 0
                       && step < module_num) {
                    step++;
                }
                ASSERT(step < module_num);
                for (k = 0; k < DDP_SOUT_MAX; k++) {
                    if (sel_out_map[j].id_bit_map[k] == -1)
                        break;
                    if (sel_out_map[j].id_bit_map[k] == module_list[step]) {
                        DDPDBG("connect out_s %s to %s, value=%d\n",
                               ddp_get_module_name(module_list[i]),
                               ddp_get_module_name(module_list[step]), k);
                        DISP_REG_SET(handle, sel_out_map[j].reg,
                                 (uint16_t) k);
                        break;
                    }
                }
            }
        }
    }
    /* connect input select */
    for (int i = 1; i < module_num; i++) {
        for (j = 0; j < ARRAY_SIZE(sel_in_map); j++) {
            if (module_list[i] == sel_in_map[j].id) {
                int found = 0;

                step = i - 1;
                /* find next module which can be connected */
                while (ddp_modules[module_list[step]].can_connect == 0 && step > 0)
                    step--;

                ASSERT(step >= 0);
                for (k = 0; k < DDP_SOUT_MAX; k++) {
                    if (sel_in_map[j].id_bit_map[k] == -1)
                        break;
                    if (sel_in_map[j].id_bit_map[k] == module_list[step]) {
                        DDPDBG("connect in_s %s to %s, value=%d\n",
                               ddp_get_module_name(module_list[step]),
                               ddp_get_module_name(module_list[i]), k);
                        DISP_REG_SET(handle, sel_in_map[j].reg,
                                 (uint16_t) k);
                        found = 1;
                        break;
                    }
                }
                if (!found)
                    DDPERR("%s error: %s sel_in not set\n", __func__,
                        ddp_get_module_name(module_list[i]));
            }
        }
    }
    // add for chist connect
    ddp_connect_chist(handle);
}

static void ddp_check_path_l(int *module_list)
{
    unsigned int j, k;
    int step = 0;
    int valid = 0;
    unsigned int mout;
    unsigned int path_error = 0;
    int module_num = ddp_get_module_num_l(module_list);

    DDPDUMP("check_path: %s to %s\n", ddp_get_module_name(module_list[0])
        , ddp_get_module_name(module_list[module_num - 1]));
    /* check mout */
    for (int i = 0; i < module_num - 1; i++) {
        for (j = 0; j < ARRAY_SIZE(mout_map); j++) {
            if (module_list[i] == mout_map[j].id) {
                mout = 0;
                /* find next module which can be connected */
                step = i + 1;
                while (ddp_modules[module_list[step]].can_connect == 0
                       && step < module_num) {
                    step++;
                }
                ASSERT(step < module_num);
                for (k = 0; k < DDP_MOUT_MAX; k++) {
                    if (mout_map[j].out_id_bit_map[k].m == -1)
                        break;
                    if (mout_map[j].out_id_bit_map[k].m == module_list[step]) {
                        mout |= mout_map[j].out_id_bit_map[k].v;
                        valid = 1;
                        break;
                    }
                }
                if (valid == 0) {
                    DDPDUMP("err: %s mout, connect next module %s failed\n",
                            ddp_get_module_name(module_list[i]),
                            ddp_get_module_name(module_list[i+1]));
                    path_error += 1;
                } else if (valid == 1) {
                    valid = 0;
                    if ((DISP_REG_GET(mout_map[j].reg) & mout) == 0) {
                        path_error += 1;
                        DDPDUMP("error:%s mout, expect=0x%x, real=0x%x\n",
                            ddp_get_module_name(module_list[i]),
                            mout, DISP_REG_GET(mout_map[j].reg));
                    } else if (DISP_REG_GET(mout_map[j].reg) != mout) {
                        DDPDUMP
                            ("warning: %s mout expect=0x%x, real=0x%x\n",
                             ddp_get_module_name(module_list[i]), mout,
                             DISP_REG_GET(mout_map[j].reg));
                    }
                }
                break;
            }
        }
    }
    /* check out select */
    for (int i = 0; i < module_num - 1; i++) {
        for (j = 0; j < ARRAY_SIZE(sel_out_map); j++) {
            if (module_list[i] != sel_out_map[j].id)
                continue;
            /* find next module which can be connected */
            step = i + 1;
            while (ddp_modules[module_list[step]].can_connect == 0
                   && step < module_num) {
                step++;
            }
            ASSERT(step < module_num);
            for (k = 0; k < DDP_SOUT_MAX; k++) {
                if (sel_out_map[j].id_bit_map[k] == -1) {
                    path_error += 1;
                    DDPDUMP
                        ("error:out_s %s not connect to %s\n",
                         ddp_get_module_name(module_list[i]),
                          ddp_get_module_name(module_list[step]));
                    break;
                }
                if (sel_out_map[j].id_bit_map[k] == module_list[step]) {
                    if (DISP_REG_GET(sel_out_map[j].reg) != k) {
                        path_error += 1;
                        DDPDUMP
                            ("error:out_s %s not connect to %s, expect=0x%x, real=0x%x\n",
                             ddp_get_module_name(module_list[i]),
                             ddp_get_module_name(module_list[step]),
                             k, DISP_REG_GET(sel_out_map[j].reg));
                    }
                    break;
                }
            }
        }
    }
    /* check input select */
    for (int i = 1; i < module_num; i++) {
        for (j = 0; j < ARRAY_SIZE(sel_in_map); j++) {
            if (module_list[i] != sel_in_map[j].id)
                continue;
            /* find next module which can be connected */
            step = i - 1;
            while (ddp_modules[module_list[step]].can_connect == 0 && step > 0)
                step--;
            ASSERT(step >= 0);
            for (k = 0; k < DDP_SOUT_MAX; k++) {
                if (sel_in_map[j].id_bit_map[k] == -1) {
                    path_error += 1;
                    DDPDUMP
                           ("error:in_s %s not connect to %s\n",
                            ddp_get_module_name(module_list[i]),
                            ddp_get_module_name(module_list[step]));
                    break;
                }
                if (sel_in_map[j].id_bit_map[k] == module_list[step]) {
                    if (DISP_REG_GET(sel_in_map[j].reg) != k) {
                        path_error += 1;
                        DDPDUMP("error:in_s %s not conn to %s,expect0x%x,real0x%x\n",
                             ddp_get_module_name(module_list[step]),
                             ddp_get_module_name(module_list[i]), k,
                             DISP_REG_GET(sel_in_map[j].reg));
                    }
                    break;
                }
            }
        }
    }
    if (path_error == 0) {
        DDPDUMP("path: %s to %s is connected\n", ddp_get_module_name(module_list[0]),
            ddp_get_module_name(module_list[module_num - 1]));
    } else {
        DDPDUMP("path: %s to %s not connected!!!\n", ddp_get_module_name(module_list[0]),
            ddp_get_module_name(module_list[module_num - 1]));
    }
}

static void ddp_disconnect_path_l(int *module_list, void *handle)
{
    unsigned int j, k;
    int step = 0;
    unsigned int mout = 0;
    unsigned int reg_mout = 0;
    unsigned int mout_idx = 0;
    int module_num = ddp_get_module_num_l(module_list);

    DDPDBG("disconnect_path: %s to %s\n", ddp_get_module_name(module_list[0]),
           ddp_get_module_name(module_list[module_num - 1]));
    for (int i = 0; i < module_num - 1; i++) {
        for (j = 0; j < ARRAY_SIZE(mout_map); j++) {
            if (module_list[i] == mout_map[j].id) {
                /* find next module which can be connected */
                step = i + 1;
                while (ddp_modules[module_list[step]].can_connect == 0
                       && step < module_num) {
                    step++;
                }
                ASSERT(step < module_num);
                for (k = 0; k < DDP_MOUT_MAX; k++) {
                    if (mout_map[j].out_id_bit_map[k].m == -1)
                        break;
                    if (mout_map[j].out_id_bit_map[k].m == module_list[step]) {
                        mout |= mout_map[j].out_id_bit_map[k].v;
                        reg_mout |= mout;
                        mout_idx = j;
                        DDPDBG("disconnect mout %s to %s\n",
                               ddp_get_module_name(module_list[i]),
                               ddp_get_module_name(module_list[step]));
                        break;
                    }
                }
                /* update mout_value */
                mout_map[j].reg_val &= ~mout;
                mout = 0;
            }
        }
        if (reg_mout) {
            DISP_REG_SET(handle, mout_map[mout_idx].reg, mout_map[mout_idx].reg_val);
            reg_mout = 0;
            mout_idx = 0;
        }
    }
}

static int ddp_get_mutex_src(enum DISP_MODULE_ENUM dest_module, enum DDP_MODE ddp_mode,
                 unsigned int *SOF_src, unsigned int *EOF_src)
{
    unsigned int src_from_dst_module = 0;

    if (dest_module == DISP_MODULE_WDMA0) {

        if (ddp_mode == DDP_VIDEO_MODE)
            DISP_LOG_W("%s: dst_mode=%s, but is video mode !!\n", __func__,
                   ddp_get_module_name(dest_module));

        *SOF_src = *EOF_src = SOF_VAL_MUTEX0_SOF_SINGLE_MODE;
        return 0;
    }

    if (dest_module == DISP_MODULE_DSI0 || dest_module == DISP_MODULE_DSIDUAL) {
        src_from_dst_module = SOF_VAL_MUTEX0_SOF_FROM_DSI0;
    } else {
        DDPERR("get mutex sof, invalid param dst module = %s(%d), dis mode %s\n",
               ddp_get_module_name(dest_module), dest_module, ddp_get_mode_name(ddp_mode));
        ASSERT(0);
    }

    if (ddp_mode == DDP_CMD_MODE) {
        *SOF_src = SOF_VAL_MUTEX0_SOF_SINGLE_MODE;
        if (0/*disp_helper_get_option(DISP_OPT_MUTEX_EOF_EN_FOR_CMD_MODE)*/)
            *EOF_src = src_from_dst_module;
        else
            *EOF_src = SOF_VAL_MUTEX0_EOF_SINGLE_MODE;

    } else {
        *SOF_src = *EOF_src = src_from_dst_module;
    }

    return 0;
}

/* id: mutex ID, 0~5 */
static int ddp_mutex_set_l(int mutex_id, int *module_list, enum DDP_MODE ddp_mode, void *handle)
{
    int i = 0;
    unsigned int value0 = 0;
    unsigned int value1 = 0;
    unsigned int value20 = 0, value21 = 0;
    unsigned int sof_val;
    unsigned int sof_src, eof_src;
    int module_num = ddp_get_module_num_l(module_list);

    ddp_get_mutex_src(module_list[module_num - 1], ddp_mode, &sof_src, &eof_src);
    if (mutex_id < DISP_MUTEX_DDP_FIRST || mutex_id > DISP_MUTEX_DDP_LAST) {
        DDPERR("exceed mutex max (0 ~ %d)\n", DISP_MUTEX_DDP_LAST);
        return -1;
    }

    for (i = 0; i < module_num; i++) {
        if (module_mutex_map[module_list[i]].bit != -1) {
            DDPDBG("module %s added to mutex %d\n", ddp_get_module_name(module_list[i]),
                   mutex_id);
            if (module_mutex_map[module_list[i]].mod_num == 0) {
                if (module_mutex_map[module_list[i]].module == DISP_MODULE_DSIDUAL) {
                    /* DISP MODULE enum must start from 0 */
                    value0 |= (1 << module_mutex_map[DISP_MODULE_DSI0].bit);
                    value0 |= (1 << module_mutex_map[DISP_MODULE_DSI1].bit);
                } else {
                    value0 |= (1 << module_mutex_map[module_list[i]].bit);
                }
            } else if (module_mutex_map[module_list[i]].mod_num == 1) {
                /* DISP_MODULE_DSIDUAL is special */
                if (module_mutex_map[module_list[i]].module == DISP_MODULE_DSIDUAL) {
                     /* DISP MODULE enum must start from 0 */
                    value1 |= (1 << module_mutex_map[DISP_MODULE_DSI0].bit);
                    value1 |= (1 << module_mutex_map[DISP_MODULE_DSI1].bit);
                } else {
                    value1 |= (1 << module_mutex_map[module_list[i]].bit);
                }
            } else if (module_mutex_map[module_list[i]].mod_num == 20) {
               /* DISP_MODULE_DSIDUAL is special */
                if (module_mutex_map[module_list[i]].module == DISP_MODULE_DSIDUAL) {
                     /* DISP MODULE enum must start from 0 */
                    value20 |= (1 << module_mutex_map[DISP_MODULE_DSI0].bit);
                    value20 |= (1 << module_mutex_map[DISP_MODULE_DSI1].bit);
                } else {
                    value20 |= (1 << module_mutex_map[module_list[i]].bit);
                }
            } else if (module_mutex_map[module_list[i]].mod_num == 21) {
               /* DISP_MODULE_DSIDUAL is special */
                if (module_mutex_map[module_list[i]].module == DISP_MODULE_DSIDUAL) {
                     /* DISP MODULE enum must start from 0 */
                    value21 |= (1 << module_mutex_map[DISP_MODULE_DSI0].bit);
                    value21 |= (1 << module_mutex_map[DISP_MODULE_DSI1].bit);
                } else {
                    value21 |= (1 << module_mutex_map[module_list[i]].bit);
                }
            }
        } else {
            DDPDBG("module %s not added to mutex %d\n",
                   ddp_get_module_name(module_list[i]), mutex_id);
        }
    }
    DISP_REG_SET(handle, DISP_REG_CONFIG_MUTEX_MOD0(mutex_id), value0);
    DISP_REG_SET(handle, DISP_REG_CONFIG_MUTEX_MOD1(mutex_id), value1);

    sof_val = REG_FLD_VAL(SOF_FLD_MUTEX0_SOF, sof_src);
    sof_val |= REG_FLD_VAL(SOF_FLD_MUTEX0_EOF, eof_src);
    DISP_REG_SET(handle, DISP_REG_CONFIG_MUTEX_SOF(mutex_id), sof_val);

    if (value20 || value21) {
       DISP_REG_SET(handle, DISP1_REG_CONFIG_MUTEX_MOD0(mutex_id), value20);
       DISP_REG_SET(handle, DISP1_REG_CONFIG_MUTEX_MOD1(mutex_id), value21);
    }

    DDPDBG("mutex %d value=0x%x, sof=%s, eof=%s\n", mutex_id,
           value0, ddp_get_mutex_sof_name(sof_src), ddp_get_mutex_sof_name(eof_src));
    return 0;
}

static void ddp_check_mutex_l(int mutex_id, int *module_list, enum DDP_MODE ddp_mode)
{
    int i = 0;
    uint32_t real_value0 = 0;
    uint32_t real_value1 = 0;
    uint32_t expect_value0 = 0;
    uint32_t expect_value1 = 0;
    unsigned int real_sof, real_eof, val;
    unsigned int expect_sof, expect_eof;
    int module_num = ddp_get_module_num_l(module_list);

    if (mutex_id < DISP_MUTEX_DDP_FIRST || mutex_id > DISP_MUTEX_DDP_LAST) {
        DDPDUMP("error:check mutex fail:exceed mutex max (0 ~ %d)\n", DISP_MUTEX_DDP_LAST);
        return;
    }
    real_value0 = DISP_REG_GET(DISP_REG_CONFIG_MUTEX_MOD0(mutex_id));
    real_value1 = DISP_REG_GET(DISP_REG_CONFIG_MUTEX_MOD1(mutex_id));
    for (i = 0; i < module_num; i++) {
        if (module_mutex_map[module_list[i]].bit != -1) {
            if (module_mutex_map[module_list[i]].mod_num == 0) {
                if (module_mutex_map[module_list[i]].module == DISP_MODULE_DSIDUAL) {
                     /* DISP MODULE enum must start from 0 */
                    expect_value0 |= (1 << module_mutex_map[DISP_MODULE_DSI0].bit);
                    expect_value0 |= (1 << module_mutex_map[DISP_MODULE_DSI1].bit);
                } else {
                    expect_value0 |= (1 << module_mutex_map[module_list[i]].bit);
                }
            } else if (module_mutex_map[module_list[i]].mod_num == 1) {
                if (module_mutex_map[module_list[i]].module == DISP_MODULE_DSIDUAL) {
                     /* DISP MODULE enum must start from 0 */
                    expect_value1 |= (1 << module_mutex_map[DISP_MODULE_DSI0].bit);
                    expect_value1 |= (1 << module_mutex_map[DISP_MODULE_DSI1].bit);
                } else {
                    expect_value1 |= (1 << module_mutex_map[module_list[i]].bit);
                }
            }
        }
    }
    if (expect_value0 != real_value0)
        DDPDUMP("error:mutex %d error: expect0 0x%x, real0 0x%x\n", mutex_id, expect_value0,
            real_value0);

    if (expect_value1 != real_value1)
        DDPDUMP("error:mutex %d error: expect1 0x%x, real1 0x%x\n", mutex_id, expect_value1,
            real_value1);

    val = DISP_REG_GET(DISP_REG_CONFIG_MUTEX_SOF(mutex_id));
    real_sof = REG_FLD_VAL_GET(SOF_FLD_MUTEX0_SOF, val);
    real_eof = REG_FLD_VAL_GET(SOF_FLD_MUTEX0_EOF, val);
    ddp_get_mutex_src(module_list[module_num - 1], ddp_mode, &expect_sof, &expect_eof);
    if (expect_sof != real_sof)
        DDPDUMP("error:mutex %d sof error: expect %s, real %s\n", mutex_id,
            ddp_get_mutex_sof_name(expect_sof), ddp_get_mutex_sof_name(real_sof));
    if (expect_eof != real_eof)
        DDPDUMP("error:mutex %d eof error: expect %s, real %s\n", mutex_id,
            ddp_get_mutex_sof_name(expect_eof), ddp_get_mutex_sof_name(real_eof));

}

static int ddp_mutex_enable_l(int mutex_idx, void *handle)
{
    DDPDBG("mutex %d enable\n", mutex_idx);
    DISP_REG_SET(handle, DISP_REG_CONFIG_MUTEX_CFG, 0);
    DISP_REG_SET(handle, DISP_REG_CONFIG_MUTEX_EN(mutex_idx), 1);


    return 0;
}

static int ddp1_mutex_enable_l(int mutex_idx, void *handle)
{
    DDPDBG("mutex %d enable\n", mutex_idx);
    DISP_REG_SET(handle, DISP1_REG_CONFIG_MUTEX_CFG, 0);
    DISP_REG_SET(handle, DISP1_REG_CONFIG_MUTEX_EN(mutex_idx), 1);


    return 0;
}

int ddp_get_module_num(enum DDP_SCENARIO_ENUM scenario)
{
    return ddp_get_module_num_l(module_list_scenario[scenario]);
}

static void ddp_print_scenario(enum DDP_SCENARIO_ENUM scenario)
{
    unsigned int i = 0, len = 0;
    char path[512] = {'\0'};
    char module_name[512] = {'\0'};
    unsigned int num = ddp_get_module_num(scenario);
    unsigned int secna = (unsigned int)scenario;

    for (i = 0; i < num; i++) {
        len = sprintf(module_name, "%d", module_list_scenario[secna][i]);
        if (len + strlen(path) < sizeof(path))
            strncat(path, module_name, len);
        memset(module_name, '\0', sizeof(module_name));
    }

    DDPMSG("scenario %s have modules: %s\n", ddp_get_scenario_name(scenario), path);
}

static int ddp_find_module_index(enum DDP_SCENARIO_ENUM ddp_scenario, enum DISP_MODULE_ENUM module)
{
    unsigned int i = 0;
    unsigned int ddp_scena = (unsigned int)ddp_scenario;

    for (i = 0; i < DDP_ENING_NUM; i++) {
        if (module_list_scenario[ddp_scena][i] == (int)module)
            return i;

    }
    DDPDBG("find module: can not find module %s on scenario %s\n", ddp_get_module_name(module),
           ddp_get_scenario_name(ddp_scenario));
    return -1;
}

/* set display interface when kernel init */
int ddp_set_dst_module(enum DDP_SCENARIO_ENUM scenario, enum DISP_MODULE_ENUM dst_module)
{
    unsigned int i = 0;
    unsigned int secna = (unsigned int)scenario;

    DDPMSG("%s, scenario=%s, dst_module=%s\n", __func__,
           ddp_get_scenario_name(scenario), ddp_get_module_name(dst_module));
    if (ddp_find_module_index(scenario, dst_module) > 0) {
        DDPDBG("%s is already on path\n", ddp_get_module_name(dst_module));
        return 0;
    }
    i = ddp_get_module_num_l(module_list_scenario[secna]) - 1;

    if (dst_module == DISP_MODULE_DSIDUAL) {
        if (i < (DDP_ENING_NUM - 1)) {
            module_list_scenario[secna][i++] = DISP_MODULE_SPLIT0;
        } else {
            DDPERR("set dst module over up bound\n");
            return -1;
        }
    } else {
        if (ddp_get_dst_module(scenario) == DISP_MODULE_DSIDUAL) {
            if (i >= 1) {
                module_list_scenario[secna][i--] = -1;
            } else {
                DDPERR("set dst module over low bound\n");
                return -1;
            }
        }
    }
    module_list_scenario[secna][i] = dst_module;
    if (scenario == DDP_SCENARIO_PRIMARY_ALL)
        ddp_set_dst_module(DDP_SCENARIO_PRIMARY_DISP, dst_module);

    ddp_print_scenario(scenario);
    return 0;
}

enum DISP_MODULE_ENUM ddp_get_dst_module(enum DDP_SCENARIO_ENUM ddp_scenario)
{
    enum DISP_MODULE_ENUM module_name = DISP_MODULE_UNKNOWN;
    unsigned int ddp_scena = (unsigned int)ddp_scenario;
    int module_num = ddp_get_module_num_l(module_list_scenario[ddp_scena]) - 1;

    if (module_num >= 0)
        module_name = module_list_scenario[ddp_scena][module_num];

    return module_name;
}

int *ddp_get_scenario_list(enum DDP_SCENARIO_ENUM ddp_scenario)
{
    unsigned int ddp_scena = (unsigned int)ddp_scenario;
    return module_list_scenario[ddp_scena];
}

int ddp_is_module_in_scenario(enum DDP_SCENARIO_ENUM ddp_scenario, enum DISP_MODULE_ENUM module)
{
    int i = 0;
    unsigned int ddp_scena = (unsigned int)ddp_scenario;

    for (i = 0; i < DDP_ENING_NUM; i++) {
        if (module_list_scenario[ddp_scena][i] == (int)module)
            return 1;

    }
    return 0;
}

int ddp_insert_module(enum DDP_SCENARIO_ENUM ddp_scenario, enum DISP_MODULE_ENUM place,
              enum DISP_MODULE_ENUM module)
{
    int i = DDP_ENING_NUM - 1;
    int idx = ddp_find_module_index(ddp_scenario, place);
    unsigned int ddp_scena = (unsigned int)ddp_scenario;

    if (idx < 0) {
        DDPERR("error: %s , place=%s is not in scenario %s!\n",
               __func__, ddp_get_module_name(place), ddp_get_scenario_name(ddp_scenario));
        return -1;
    }

    for (i = 0; i < DDP_ENING_NUM; i++) {
        if (module_list_scenario[ddp_scena][i] == (int)module) {
            DDPERR("error: %s , module=%s is already in scenario %s!\n",
                   __func__, ddp_get_module_name(module), ddp_get_scenario_name(ddp_scenario));
            return -1;
        }
    }

    /* should have empty room for insert */
    ASSERT(module_list_scenario[ddp_scena][DDP_ENING_NUM - 1] == -1);

    for (i = DDP_ENING_NUM - 2; i >= idx; i--) {
        module_list_scenario[ddp_scena][i + 1] =
            module_list_scenario[ddp_scena][i];
    }
    module_list_scenario[ddp_scena][idx] = module;

    {
        int *modules = ddp_get_scenario_list(ddp_scenario);
        int module_num = ddp_get_module_num(ddp_scenario);

        DDPMSG("after insert module, module list is:\n");
        for (i = 0; i < module_num; i++)
            DDPMSG("%s-", ddp_get_module_name(modules[i]));
    }

    return 0;
}

int ddp_remove_module(enum DDP_SCENARIO_ENUM ddp_scenario, enum DISP_MODULE_ENUM module)
{
    int i = 0;
    int idx = ddp_find_module_index(ddp_scenario, module);
    unsigned int ddp_scena = (unsigned int)ddp_scenario;

    if (idx < 0) {
        DDPERR("%s, can not find module %s in scenario %s\n", __func__,
               ddp_get_module_name(module), ddp_get_scenario_name(ddp_scenario));
        return -1;
    }

    for (i = idx; i < DDP_ENING_NUM - 1; i++) {
        module_list_scenario[ddp_scena][i] =
            module_list_scenario[ddp_scena][i + 1];
    }
    module_list_scenario[ddp_scena][DDP_ENING_NUM - 1] = -1;

    {
        int *modules = ddp_get_scenario_list(ddp_scenario);
        int module_num = ddp_get_module_num(ddp_scenario);

        DDPMSG("after remove module, module list is:\n");
        for (i = 0; i < module_num; i++)
            DDPMSG("%s-", ddp_get_module_name(modules[i]));
    }
    return 0;
}

void ddp_connect_path(enum DDP_SCENARIO_ENUM scenario, void *handle)
{
    unsigned int scn = (unsigned int)scenario;
    DDPDBG("path connect on scenario %s\n", ddp_get_scenario_name(scenario));
    if (scenario == DDP_SCENARIO_PRIMARY_ALL) {
        ddp_connect_path_l(module_list_scenario[DDP_SCENARIO_PRIMARY_DISP], handle);
        ddp_connect_path_l(module_list_scenario[DDP_SCENARIO_PRIMARY_OVL_MEMOUT],
                        handle);
    } else if (scenario == DDP_SCENARIO_PRIMARY1_ALL) {
        ddp_connect_path_l(module_list_scenario[DDP_SCENARIO_PRIMARY_DISP], handle);
        ddp_connect_path_l(module_list_scenario[DDP_SCENARIO_PRIMARY_OVL_MEMOUT],
                        handle);
    } else if (scenario == DDP_SCENARIO_SUB_ALL) {
        ddp_connect_path_l(module_list_scenario[DDP_SCENARIO_SUB_DISP], handle);
        ddp_connect_path_l(module_list_scenario[DDP_SCENARIO_SUB_OVL2_2L_MEMOUT],
                        handle);
    } else {
        ddp_connect_path_l(module_list_scenario[scn], handle);
    }
    return;
}

void ddp_disconnect_path(enum DDP_SCENARIO_ENUM scenario, void *handle)
{
    unsigned int scn = (unsigned int)scenario;
    DDPDBG("path disconnect on scenario %s\n", ddp_get_scenario_name(scenario));

    if (scenario == DDP_SCENARIO_PRIMARY_ALL) {
        ddp_disconnect_path_l(module_list_scenario[DDP_SCENARIO_PRIMARY_DISP],
                        handle);
        ddp_disconnect_path_l(module_list_scenario[DDP_SCENARIO_PRIMARY_OVL_MEMOUT],
                      handle);
    } else if (scenario == DDP_SCENARIO_SUB_ALL) {
        ddp_disconnect_path_l(module_list_scenario[DDP_SCENARIO_SUB_DISP], handle);
        ddp_disconnect_path_l(module_list_scenario[DDP_SCENARIO_SUB_OVL2_2L_MEMOUT],
                        handle);
    } else {
        ddp_disconnect_path_l(module_list_scenario[scn], handle);
    }
    return;
}

void ddp_check_path(enum DDP_SCENARIO_ENUM scenario)
{
    unsigned int scn = (unsigned int)scenario;
    DDPDBG("path check path on scenario %s\n", ddp_get_scenario_name(scenario));

    if (scenario == DDP_SCENARIO_PRIMARY_ALL) {
        ddp_check_path_l(module_list_scenario[DDP_SCENARIO_PRIMARY_DISP]);
        ddp_check_path_l(module_list_scenario[DDP_SCENARIO_PRIMARY_OVL_MEMOUT]);
    } else if (scenario == DDP_SCENARIO_PRIMARY_DISP_LEFT) {
        ddp_check_path_l(module_list_scenario[DDP_SCENARIO_PRIMARY_DISP_LEFT]);
        ddp_check_path_l(module_list_scenario[DDP_SCENARIO_PRIMARY_DISP_RIGHT]);
    } else {
        ddp_check_path_l(module_list_scenario[scn]);
    }
    return;
}

void ddp_check_mutex(int mutex_id, enum DDP_SCENARIO_ENUM scenario, enum DDP_MODE mode)
{
    unsigned int scn = (unsigned int)scenario;
    DDPDBG("check mutex %d on scenario %s\n", mutex_id, ddp_get_scenario_name(scenario));
    ddp_check_mutex_l(mutex_id, module_list_scenario[scn], mode);
    return;
}

int ddp_mutex_set(int mutex_id, enum DDP_SCENARIO_ENUM scenario, enum DDP_MODE mode, void *handle)
{
    int ret;
    unsigned int scn = (unsigned int)scenario;

    if (scenario < DDP_SCENARIO_MAX) {
        ret = ddp_mutex_set_l(mutex_id, module_list_scenario[scn], mode, handle);
        if (ddp_path_is_dual(scenario)) {
            unsigned int sof_val, sof_src = 0, eof_src = 0;
            enum DDP_SCENARIO_ENUM r_scenario = ddp_get_dual_module(scenario);
            unsigned int r_scena = (unsigned int)r_scenario;
            int *module_list = module_list_scenario[r_scena];
            int module_num = ddp_get_module_num(r_scenario);

            ret = ddp_mutex_add_module_by_scenario(mutex_id, r_scenario,
                                        handle);
            ret = ddp_get_mutex_src(module_list[module_num - 1], mode, &sof_src, &eof_src);

            sof_src = (sof_src == SOF_VAL_MUTEX0_SOF_FROM_DSI0) ?
                SOF_VAL_MUTEX0_SOF_FROM_DSI1 : sof_src;
            eof_src = (eof_src == SOF_VAL_MUTEX0_EOF_FROM_DSI0) ?
                SOF_VAL_MUTEX0_EOF_FROM_DSI1 : eof_src;

            sof_val = REG_FLD_VAL(SOF_FLD_MUTEX0_SOF, sof_src);
            sof_val |= REG_FLD_VAL(SOF_FLD_MUTEX0_EOF, eof_src);

            DISP_REG_SET(handle, DISP1_REG_CONFIG_MUTEX_SOF(mutex_id), sof_val);
        }
        return ret;
    } else {
        DDPERR("Invalid scenario %d when setting mutex\n", scenario);
        return -1;
    }
    return 0;
}

#if 0 //no use
int ddp_mutex_Interrupt_enable(int mutex_id, void *handle)
{

    DDPDBG("mutex %d interrupt enable\n", mutex_id);
    DISP_REG_MASK(handle, DISP_REG_CONFIG_MUTEX_INTEN, 0x1 << mutex_id, 0x1 << mutex_id);
    DISP_REG_MASK(handle, DISP_REG_CONFIG_MUTEX_INTEN, 1 << (mutex_id + DISP_MUTEX_TOTAL),
              0x1 << (mutex_id + DISP_MUTEX_TOTAL));
    return 0;
}

int ddp_mutex_Interrupt_disable(int mutex_id, void *handle)
{
    DDPDBG("mutex %d interrupt disenable\n", mutex_id);
    DISP_REG_MASK(handle, DISP_REG_CONFIG_MUTEX_INTEN, 0, 0x1 << mutex_id);
    DISP_REG_MASK(handle, DISP_REG_CONFIG_MUTEX_INTEN, 0, 0x1 << (mutex_id + DISP_MUTEX_TOTAL));
    return 0;
}
#endif

int ddp_mutex_reset(int mutex_id, void *handle)
{
    DDPDBG("mutex %d reset\n", mutex_id);
    DISP_REG_SET(handle, DISP_REG_CONFIG_MUTEX_RST(mutex_id), 1);
    DISP_REG_SET(handle, DISP_REG_CONFIG_MUTEX_RST(mutex_id), 0);

    return 0;
}

int ddp1_mutex_reset(int mutex_id, void *handle)
{
    DDPDBG("mutex %d reset\n", mutex_id);
    DISP_REG_SET(handle, DISP1_REG_CONFIG_MUTEX_RST(mutex_id), 1);
    DISP_REG_SET(handle, DISP1_REG_CONFIG_MUTEX_RST(mutex_id), 0);

    return 0;
}

int ddp_is_moudule_in_mutex(int mutex_id, enum DISP_MODULE_ENUM module)
{
    int ret = 0;
    uint32_t real_value = 0;
    unsigned int mod = (unsigned int)module;

    if (mutex_id < DISP_MUTEX_DDP_FIRST || mutex_id > DISP_MUTEX_DDP_LAST) {
        DDPDUMP("error:check_moudule_in_mute fail:exceed mutex max (0 ~ %d)\n",
            DISP_MUTEX_DDP_LAST);
        return ret;
    }

    if (module_mutex_map[mod].mod_num == 0)
        real_value = DISP_REG_GET(DISP_REG_CONFIG_MUTEX_MOD0(mutex_id));

    if (1 == ((real_value >> module_mutex_map[mod].bit) & 0x01))
        ret = 1;

    return ret;
}


int ddp_mutex_add_module(int mutex_id, enum DISP_MODULE_ENUM module, void *handle)
{
    int value = 0;
    unsigned int mod = (unsigned int)module;

    if (module < DISP_MODULE_UNKNOWN) {
        if (module_mutex_map[mod].bit != -1) {
            DDPDBG("module %s added to mutex %d\n", ddp_get_module_name(module),
                   mutex_id);
            value |= (1 << module_mutex_map[mod].bit);
            if (module_mutex_map[mod].mod_num == 0)
                DISP_REG_MASK(handle, DISP_REG_CONFIG_MUTEX_MOD0(mutex_id), value, value);
            else if (module_mutex_map[mod].mod_num == 1)
                DISP_REG_MASK(handle, DISP_REG_CONFIG_MUTEX_MOD1(mutex_id), value, value);
            else if (module_mutex_map[mod].mod_num == 20) /* For DISPSYS1 MOD0 */
                DISP_REG_MASK(handle, DISP1_REG_CONFIG_MUTEX_MOD0(mutex_id), value, value);
            else if (module_mutex_map[mod].mod_num == 21) /* For DISPSYS1 MOD1 */
                DISP_REG_MASK(handle, DISP1_REG_CONFIG_MUTEX_MOD1(mutex_id), value, value);
        }
    }
    return 0;
}

int ddp_mutex_add_module_by_scenario(int mutex_id, enum DDP_SCENARIO_ENUM scenario, void *handle)
{
    int module_num = ddp_get_module_num_l(module_list_scenario[scenario]);
    int i;
    unsigned int scn = (unsigned int)scenario;

    for (i = 0 ; i < module_num ; i++)
        ddp_mutex_add_module(mutex_id, module_list_scenario[scn][i], handle);

    return 0;
}

#if 0
int ddp_mutex_remove_module(int mutex_id, enum DISP_MODULE_ENUM module, void *handle)
{
    int value = 0;
    unsigned int mod = (unsigned int)module;

    if (module < DISP_MODULE_UNKNOWN) {
        if (module_mutex_map[mod].bit != -1) {
            DDPDBG("module %s added to mutex %d\n", ddp_get_module_name(module),
                   mutex_id);
            value |= (1 << module_mutex_map[mod].bit);
            if (module_mutex_map[mod].mod_num == 0)
                DISP_REG_MASK(handle, DISP_REG_CONFIG_MUTEX_MOD0(mutex_id), 0, value);
            else if (module_mutex_map[mod].mod_num == 1)
                DISP_REG_MASK(handle, DISP_REG_CONFIG_MUTEX_MOD1(mutex_id), 0, value);
        }

    }
    return 0;
}
#endif

int ddp_mutex_clear(int mutex_id, void *handle)
{
    DDPDBG("mutex %d clear\n", mutex_id);
    DISP_REG_SET(handle, DISP_REG_CONFIG_MUTEX_MOD0(mutex_id), 0);
    DISP_REG_SET(handle, DISP_REG_CONFIG_MUTEX_SOF(mutex_id), 0);
    /* enough or not */

    /*reset mutex */
    ddp_mutex_reset(mutex_id, handle);
    return 0;
}

int ddp1_mutex_clear(int mutex_id, void *handle)
{
    DDPDBG("mutex %d clear\n", mutex_id);
    DISP_REG_SET(handle, DISP1_REG_CONFIG_MUTEX_MOD0(mutex_id), 0);
    DISP_REG_SET(handle, DISP1_REG_CONFIG_MUTEX_SOF(mutex_id), 0);
    /* enough or not */

    /*reset mutex */
    ddp1_mutex_reset(mutex_id, handle);
    return 0;
}

int ddp_mutex_enable(int mutex_id, enum DDP_SCENARIO_ENUM scenario, void *handle)
{

    if (ddp_path_is_dual(scenario))
        ddp1_mutex_enable_l(mutex_id, handle);

    return ddp_mutex_enable_l(mutex_id, handle);
}

#if 0
int ddp_mutex_disable(int mutex_id, enum DDP_SCENARIO_ENUM scenario, void *handle)
{
    DDPDBG("mutex %d disable\n", mutex_id);
    DISP_REG_SET(handle, DISP_REG_CONFIG_MUTEX_EN(mutex_id), 0);
    return 0;
}

int ddp_mutex_set_sof_wait(int mutex_id, void *handle, int wait)
{
    if (mutex_id < DISP_MUTEX_DDP_FIRST || mutex_id > DISP_MUTEX_DDP_LAST) {
        DDPERR("exceed mutex max (0 ~ %d)\n", DISP_MUTEX_DDP_LAST);
        return -1;
    }

    DISP_REG_SET_FIELD(handle, SOF_FLD_MUTEX0_SOF_WAIT, DISP_REG_CONFIG_MUTEX_SOF(mutex_id), wait);
    return 0;
}
#endif

int ddp_check_engine_status(int mutexID)
{
    /* check engines' clock bit &  enable bit & status bit before unlock mutex */
    /* should not needed, in comdq do? */
    int result = 0;
    return result;
}

static void ddp_smi_config(void)
{
    int offset = 0x100, value = 0xA;

    DDPMSG("SMI subcom before config:\n");
    DDPMSG("(0x%lx,0x%lx,0x%lx,0x%lx,0x%lx,0x%lx)=(0x%x,0x%x,0x%x,0x%x,0x%x,0x%x)\n",
           SMI_SUBCOM0_BASE + offset,
           SMI_SUBCOM1_BASE + offset,
           SMI_SUBCOM4_BASE + offset,
           SMI_SUBCOM5_BASE + offset,
           SMI_SUBCOM3_SYS_BASE + offset,
           SMI_SUBCOM4_SYS_BASE + offset,
           DISP_REG_GET(SMI_SUBCOM0_BASE + offset),
           DISP_REG_GET(SMI_SUBCOM1_BASE + offset),
           DISP_REG_GET(SMI_SUBCOM4_BASE + offset),
           DISP_REG_GET(SMI_SUBCOM5_BASE + offset),
           DISP_REG_GET(SMI_SUBCOM3_SYS_BASE + offset),
           DISP_REG_GET(SMI_SUBCOM4_SYS_BASE + offset));

    DISP_REG_SET(NULL, SMI_SUBCOM0_BASE + offset, value);
    DISP_REG_SET(NULL, SMI_SUBCOM1_BASE + offset, value);
    DISP_REG_SET(NULL, SMI_SUBCOM4_BASE + offset, value);
    DISP_REG_SET(NULL, SMI_SUBCOM5_BASE + offset, value);
    DISP_REG_SET(NULL, SMI_SUBCOM3_SYS_BASE + offset, value);
    DISP_REG_SET(NULL, SMI_SUBCOM4_SYS_BASE + offset, value);

    DDPMSG("SMI subcom after config:\n");
    DDPMSG("(0x%lx,0x%lx,0x%lx,0x%lx,0x%lx,0x%lx)=(0x%x,0x%x,0x%x,0x%x,0x%x,0x%x)\n",
           SMI_SUBCOM0_BASE + offset,
           SMI_SUBCOM1_BASE + offset,
           SMI_SUBCOM4_BASE + offset,
           SMI_SUBCOM5_BASE + offset,
           SMI_SUBCOM3_SYS_BASE + offset,
           SMI_SUBCOM4_SYS_BASE + offset,
           DISP_REG_GET(SMI_SUBCOM0_BASE + offset),
           DISP_REG_GET(SMI_SUBCOM1_BASE + offset),
           DISP_REG_GET(SMI_SUBCOM4_BASE + offset),
           DISP_REG_GET(SMI_SUBCOM5_BASE + offset),
           DISP_REG_GET(SMI_SUBCOM3_SYS_BASE + offset),
           DISP_REG_GET(SMI_SUBCOM4_SYS_BASE + offset));
}

int ddp_path_top_clock_on(void)
{
    DDPMSG("ddp path top clock on\n");
    //spm_mtcmos_ctrl_dis(1);
    ddp_enable_module_clock(DISP_MODULE_SMI_COMMON);
    ddp_enable_module_clock(DISP_MODULE_SMI_LARB0);

    DDPMSG("ddp CG0:%x CG1:%x\n", DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON0),
        DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON1));

#if 1  /* Force CG always on */
    DDPMSG("[FPGA Only] before power on MMSYS:0x%x,0x%x,0x%x\n",
           DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON0),
           DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON1),
           DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON2));

    DISP_REG_SET(NULL, DISP_REG_CONFIG_MMSYS_CG_CLR0, 0xFFFFFFFF);
    DISP_REG_SET(NULL, DISP_REG_CONFIG_MMSYS_CG_CLR1, 0xFFFFFFFF);
    DISP_REG_SET(NULL, DISP_REG_CONFIG_MMSYS_CG_CLR2, 0xFFFFFFFF);
    DDPMSG("[FPGA Only] before power on MMSYS:0x%x,0x%x,0x%x\n",
           DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON0),
           DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON1),
           DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON2));
#endif

    ddp_smi_config();

    /* SODI config */
    DISP_REG_SET(NULL, DISP_REG_CONFIG_MMSYS_SODI_REQ_MASK, 0xF500);
    DISP_REG_SET(NULL, DISP1_REG_CONFIG_MMSYS_SODI_REQ_MASK, 0xF500);
    DISP_REG_SET(NULL, DISP_REG_CONFIG_MMSYS_EMI_REQ_CTL, 0xDF);
    DISP_REG_SET(NULL, DISP1_REG_CONFIG_MMSYS_EMI_REQ_CTL, 0xDF);
    DISP_REG_MASK(NULL, DISP_REG_CONFIG_MMSYS_EMI_REQ_CTL, 0x0, 0x3FFFFC);
    DISP_REG_MASK(NULL, DISP1_REG_CONFIG_MMSYS_EMI_REQ_CTL, 0x0, 0x3FFFFC);
    return 0;

}

int ddp_path_top_clock_off(void)
{
    DDPMSG("ddp path top clock off\n");
    ddp_disable_module_clock(DISP_MODULE_SMI_LARB0);
    ddp_disable_module_clock(DISP_MODULE_SMI_COMMON);
    DDPMSG("ddp CG0:%x CG1:%x\n", DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON0),
        DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON1));
    return 0;

}

int ddp_path_m4u_off(void)
{
    int i;

    DDPMSG("ddp path m4u off\n");
#ifdef MTKFB_NO_M4U
    /* display ports bypass m4u.
     * ==== FBI WARNING: ==========
     * This function is hard code of m4u port setting !!!
     * Please ask M4U owner about it for new chip porting !!!
     */

    /*
     *   M4U_PORT_DISP_OVL0,
     *   M4U_PORT_DISP_2L_OVL0_LARB0,
     *   M4U_PORT_DISP_RDMA0,
     *   M4U_PORT_DISP_WDMA0,
     */
    for (i = 0; i < 13; i++) {
        DISP_REG_SET_FIELD(0, REG_FLD_MMU_EN, DISP_REG_SMI_LARB0_MMU_EN + i*4, 0);
        DISP_REG_SET_FIELD(0, REG_FLD_MMU_EN, DISP_REG_SMI_LARB1_MMU_EN + i*4, 0);
    }
    /*
     *   M4U_PORT_DISP_OVL1_2L,
     *   M4U_PORT_DISP_RDMA1,
     *   M4U_PORT_DISP_2L_OVL0,
     */

    /* not use SMI_LARB1 */
    /*
     *    for (i = 0; i < 3; i++)
     *       DISP_REG_SET_FIELD(0, REG_FLD_MMU_EN, DISP_REG_SMI_LARB1_MMU_EN + i*4, 0);
     */
#endif
    return 0;
}

void primary_disp_module_list_insert_dsc(void)
{
    unsigned int i = 0;

    for (i = 0; i < DDP_ENING_NUM; i++) {
        module_list_scenario[DDP_SCENARIO_PRIMARY_DISP][i] =
            primary_disp_dsc_module_list[i];
        module_list_scenario[DDP_SCENARIO_PRIMARY_DISP_LEFT][i] =
            primary_disp_dsc_left_module_list[i];
        module_list_scenario[DDP_SCENARIO_PRIMARY_DISP_RIGHT][i] =
            primary_disp_dsc_right_module_list[i];
    }
}
