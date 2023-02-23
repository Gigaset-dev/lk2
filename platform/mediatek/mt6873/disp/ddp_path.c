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

#define DDP_ENING_NUM    (20) /* max module num of path */

#define DDP_MOUT_MAX     8
#define DDP_SOUT_MAX     8
#define DDP_MUTEX_MAX    4

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
    DISP_MODULE_OVL0_2L, DISP_MODULE_OVL0,
    DISP_MODULE_RDMA0,
    DISP_MODULE_COLOR0, DISP_MODULE_CCORR0,
#ifdef MTK_DRE30_SUPPORT
    DISP_MODULE_MDP_AAL4,
#endif
    DISP_MODULE_AAL0, DISP_MODULE_GAMMA0,
    DISP_MODULE_POSTMASK0, DISP_MODULE_DITHER0,
    DISP_MODULE_DSC,
    DISP_MODULE_PWM0, DISP_MODULE_DSI0,
    -1,
};

int module_list_scenario[DDP_SCENARIO_MAX][DDP_ENING_NUM] = {
    /* DDP_SCENARIO_PRIMARY_DISP */
    {
        DISP_MODULE_OVL0_2L,
        DISP_MODULE_OVL0, DISP_MODULE_RDMA0,
        DISP_MODULE_COLOR0, DISP_MODULE_CCORR0,
#ifdef MTK_DRE30_SUPPORT
        DISP_MODULE_MDP_AAL4,
#endif
        DISP_MODULE_AAL0, DISP_MODULE_GAMMA0,
        DISP_MODULE_POSTMASK0, DISP_MODULE_DITHER0,
        DISP_MODULE_PWM0, DISP_MODULE_DSI0,
        -1,
    },

    /* DDP_SCENARIO_PRIMARY_BYPASS_PQ_DISP */
    {
        DISP_MODULE_OVL0_2L, DISP_MODULE_OVL0,
        DISP_MODULE_RDMA0,
        DISP_MODULE_PWM0, DISP_MODULE_DSI0,
        -1,
    },

    /* DDP_SCENARIO_PRIMARY_RDMA0_COLOR0_DISP0 */
    {
        DISP_MODULE_RDMA0,
#ifdef DISP_COLOR_ON
        DISP_MODULE_COLOR0,
#endif
        DISP_MODULE_CCORR0, DISP_MODULE_MDP_AAL4,
        DISP_MODULE_AAL0, DISP_MODULE_GAMMA0,
        DISP_MODULE_POSTMASK0, DISP_MODULE_DITHER0,
        DISP_MODULE_PWM0, DISP_MODULE_DSI0,
        -1,
    },

    /* DDP_SCENARIO_PRIMARY_RDMA0_DISP */
    {
        DISP_MODULE_RDMA0,
        DISP_MODULE_PWM0, DISP_MODULE_DSI0,
        -1,
    },

    /* DDP_SCENARIO_PRIMARY_OVL_MEMOUT */
    {
        DISP_MODULE_OVL0_2L, DISP_MODULE_OVL0,
        DISP_MODULE_WDMA0,
        -1,
    },

    /* DDP_SCENARIO_PRIMARY0_DISP0_MDP_AAL4 */
    {
        DISP_MODULE_OVL0_2L, DISP_MODULE_OVL0,
        DISP_MODULE_RDMA0,
#ifdef DISP_COLOR_ON
        DISP_MODULE_COLOR0,
#endif
        DISP_MODULE_CCORR0, DISP_MODULE_MDP_AAL4,
        DISP_MODULE_AAL0, DISP_MODULE_GAMMA0,
        DISP_MODULE_POSTMASK0, DISP_MODULE_DITHER0,
        DISP_MODULE_PWM0, DISP_MODULE_DSI0,
        -1,
    },

    /* DDP_SCENARIO_PRIMARY0_OVL_PQ_MEMOUT */
    {
        DISP_MODULE_OVL0_2L, DISP_MODULE_OVL0,
        DISP_MODULE_RDMA0,
#ifdef DISP_COLOR_ON
        DISP_MODULE_COLOR0,
#endif
        DISP_MODULE_CCORR0, DISP_MODULE_MDP_AAL4,
        DISP_MODULE_AAL0, DISP_MODULE_GAMMA0,
        DISP_MODULE_POSTMASK0, DISP_MODULE_DITHER0,
        DISP_MODULE_WDMA0, -1,
    },

    /* DDP_SCENARIO_PRIMARY_ALL */
    {
        DISP_MODULE_OVL0_2L, DISP_MODULE_OVL0,
        DISP_MODULE_WDMA0,
        DISP_MODULE_RDMA0,
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
        DISP_MODULE_OVL2_2L, DISP_MODULE_DPI,
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
};

/* 1st para is mout's input, 2nd para is mout's output */
static struct mout_t mout_map[] = {
    /* DISP_TOVL0_OUT0_MOUT_EN */
    {DISP_MODULE_OVL0_2L,
        {{DISP_MODULE_RDMA0, 1 << 0},
         {DISP_MODULE_RSZ0, 1 << 1},
         {DISP_MODULE_WDMA0, 1 << 2},
         {DISP_MODULE_UFBC_WDMA0, 1 << 3},
         {DISP_MODULE_MDP_HDR4, 1 << 4}, {-1, 0} },
        0, 0},

    /* DISP_TOVL0_OUT1_MOUT_EN */
    {DISP_MODULE_OVL0,
        {{DISP_MODULE_RDMA0, 1 << 0},
         {DISP_MODULE_RSZ0, 1 << 1},
         {DISP_MODULE_WDMA0, 1 << 2},
         {DISP_MODULE_UFBC_WDMA0, 1 << 3},
         {DISP_MODULE_MDP_HDR4, 1 << 4}, {-1, 0} },
        0, 0},

    /* DISP_RSZ0_MOUT_EN */
    {DISP_MODULE_RSZ0,
        {{DISP_MODULE_RDMA0, 1 << 0},
         {DISP_MODULE_WDMA0, 1 << 1},
         {DISP_MODULE_UFBC_WDMA0, 1 << 2},
         {DISP_MODULE_RDMA2_VIRTUAL, 1 << 3},
         {DISP_MODULE_MDP_HDR4, 1 << 4}, {-1, 0} },
        0, 0},

    /* DISP_DITHER0_MOUT_EN */
    {DISP_MODULE_DITHER0,
        {{DISP_MODULE_DSI0, 1 << 0},
         {DISP_MODULE_DSC, 1 << 1},
         {DISP_MODULE_WDMA0, 1 << 2},
         {DISP_MODULE_UFBC_WDMA0, 1 << 3}, {-1, 0} },
        0, 0},

    /* DISP_OVL2_2L_OUT0_MOUT_EN */
    {DISP_MODULE_DITHER0,
        {{DISP_MODULE_RDMA4, 1 << 0},
         {DISP_MODULE_WDMA0, 1 << 1},
         {DISP_MODULE_UFBC_WDMA0, 1 << 2}, {-1, 0} },
        0, 0},
};

static struct sel_t sel_out_map[] = {
    /* DISP_RDMA2_RSZ0_RSZ1_SOUT_SEL */
    {DISP_MODULE_RDMA2_VIRTUAL, {DISP_MODULE_OVL0_2L,
        DISP_MODULE_OVL0, -1}, 0, 0},

    /* DISP_REG_CONFIG_DISP_MDP_TDSHP4_SOUT_SEL */
    {DISP_MODULE_Y2R0, {DISP_MODULE_OVL0_2L,
        DISP_MODULE_OVL0, -1}, 0, 0},

    /* DISP_RDMA0_RSZ0_SOUT_SEL */
    {DISP_MODULE_RDMA0, {DISP_MODULE_DSI0,
        DISP_MODULE_COLOR0, -1}, 0, 0},

    /* DISP_CCORR0_SOUT_SEL */
    {DISP_MODULE_CCORR0, {DISP_MODULE_MDP_AAL4,
        DISP_MODULE_AAL0, -1}, 0, 0},

    /* MDP_AAL4_SOUT_SEL */
    {DISP_MODULE_MDP_HDR4, {DISP_MODULE_MDP_RSZ4,
        DISP_MODULE_MDP_AAL4, -1}, 0, 0},

    /* DISP_MDP_AAL4_SOUT_SEL */
    {DISP_MODULE_MDP_AAL4, {DISP_MODULE_AAL0,
        DISP_MODULE_MDP_RSZ4, -1}, 0, 0},

    /* DISP_Y2R0_SOUT_SEL */
    {DISP_MODULE_Y2R0, {DISP_MODULE_DMDP_TDSHP4_SOUT,
        DISP_MODULE_RDMA0, -1}, 0, 0},
};

/* 1st para is sout's output, 2nd para is sout's input */
static struct sel_t sel_in_map[] = {
    /* DISP_OVL0_2L_UFOD_SEL_IN */
    {DISP_MODULE_OVL0_2L, {DISP_MODULE_RDMA2_VIRTUAL,
     DISP_MODULE_Y2R0, -1}, 0, 0},

    /* DISP_OVL0_UFOD_SEL_IN */
    {DISP_MODULE_OVL0, {DISP_MODULE_RDMA2_VIRTUAL,
     DISP_MODULE_Y2R0, -1}, 0, 0},

    /* DISP_OVL0_OVL0_2L_PQOUT_SEL_IN */
    {DISP_MODULE_MDP_HDR4, {DISP_MODULE_OVL0_2L,
     DISP_MODULE_OVL0, -1}, 0, 0},

    /* DISP_RSZ0_SEL_IN */
    {DISP_MODULE_RSZ0, {DISP_MODULE_OVL0_2L,
     DISP_MODULE_OVL0, -1}, 0, 0},

    /* DISP_RDMA0_SEL_IN */
    {DISP_MODULE_RDMA0, {DISP_MODULE_OVL0,
     DISP_MODULE_RSZ0, DISP_MODULE_Y2R0,
     DISP_MODULE_OVL0_2L, -1}, 0, 0},

    /* DISP_AAL0_SEL_IN */
    {DISP_MODULE_AAL0, {DISP_MODULE_MDP_AAL4,
     DISP_MODULE_CCORR0, -1}, 0, 0},

    /* DSI0_SEL_IN */
    {DISP_MODULE_DSI0, {DISP_MODULE_RDMA0,
     DISP_MODULE_DITHER0, DISP_MODULE_DSC, -1}, 0, 0},

    /* DISP_WDMA0_SEL_IN */
    {DISP_MODULE_WDMA0, {DISP_MODULE_DITHER0,
     DISP_MODULE_RSZ0, DISP_MODULE_OVL0_2L,
     DISP_MODULE_OVL0, -1}, 0, 0},

    /* DISP_UFBC_WDMA0_SEL_IN */
    {DISP_MODULE_WDMA0, {DISP_MODULE_DITHER0,
     DISP_MODULE_RSZ0, DISP_MODULE_OVL0_2L,
     DISP_MODULE_OVL0, -1}, 0, 0},

    /* DISP_TOVL0_PQOUT_MDP_RDMA4_SEL_IN */
    {DISP_MODULE_MDP_HDR4, {DISP_MODULE_MDP_RDMA4,
     DISP_MODULE_OVL0_OVL0_2L_VIRTUAL, DISP_MODULE_RSZ0, -1}, 0, 0},

    /* DISP_MDP_AAL4_MDP_HDR4_SEL_IN */
    {DISP_MODULE_MDP_RSZ4, {DISP_MODULE_MDP_HDR4,
     DISP_MODULE_MDP_AAL4, -1}, 0, 0},

    /* DISP_MDP_AAL4_SEL_IN */
    {DISP_MODULE_MDP_AAL4, {DISP_MODULE_CCORR0,
     DISP_MODULE_MDP_HDR4, -1}, 0, 0},
};

int ddp_path_init(void)
{
    int i;

    /* mout */
    mout_map[0].reg =
        (unsigned long *)DISP_REG_CONFIG_DISP_TOVL0_OUT0_MOUT_EN;
    mout_map[1].reg =
        (unsigned long *)DISP_REG_CONFIG_DISP_TOVL0_OUT1_MOUT_EN;
    mout_map[2].reg =
        (unsigned long *)DISP_REG_CONFIG_DISP_RSZ0_MOUT_EN;
    mout_map[3].reg =
        (unsigned long *)DISP_REG_CONFIG_DISP_DITHER0_MOUT_EN;
    mout_map[4].reg =
        (unsigned long *)DISP_REG_CONFIG_DISP_OVL2_2L_OUT0_MOUT_EN;

    /* sel_out */
    sel_out_map[0].reg =
        (unsigned long *)DISP_REG_CONFIG_DISP_RDMA2_RSZ0_RSZ1_SOUT_SEL;
    sel_out_map[1].reg =
        (unsigned long *)DISP_REG_CONFIG_DISP_MDP_TDSHP4_SOUT_SEL;
    sel_out_map[2].reg =
        (unsigned long *)DISP_REG_CONFIG_DISP_RDMA0_RSZ0_SOUT_SEL;
    sel_out_map[3].reg =
        (unsigned long *)DISP_REG_CONFIG_DISP_CCORR0_SOUT_SEL;
    sel_out_map[4].reg =
        (unsigned long *)DISP_REG_CONFIG_MDP_AAL4_SOUT_SEL;
    sel_out_map[5].reg =
        (unsigned long *)DISP_REG_CONFIG_DISP_MDP_AAL4_SOUT_SEL;
    sel_out_map[6].reg =
        (unsigned long *)DISP_REG_CONFIG_DISP_Y2R0_SOUT_SEL;

    /* sel_in */
    sel_in_map[0].reg =
        (unsigned long *)DISP_REG_CONFIG_DISP_OVL0_2L_UFOD_SEL_IN;
    sel_in_map[1].reg =
        (unsigned long *)DISP_REG_CONFIG_DISP_OVL0_UFOD_SEL_IN;
    sel_in_map[2].reg =
        (unsigned long *)DISP_REG_CONFIG_DISP_OVL0_OVL0_2L_PQOUT_SEL_IN;
    sel_in_map[3].reg =
        (unsigned long *)DISP_REG_CONFIG_DISP_RSZ0_SEL_IN;
    sel_in_map[4].reg =
        (unsigned long *)DISP_REG_CONFIG_DISP_RDMA0_SEL_IN;
    sel_in_map[5].reg =
        (unsigned long *)DISP_REG_CONFIG_DISP_AAL0_SEL_IN;
    sel_in_map[6].reg =
        (unsigned long *)DISP_REG_CONFIG_DSI0_SEL_IN;
    sel_in_map[7].reg =
        (unsigned long *)DISP_REG_CONFIG_DISP_WDMA0_SEL_IN;
    sel_in_map[8].reg =
        (unsigned long *)DISP_REG_CONFIG_UFBC_WDMA0_SEL_IN;
    sel_in_map[9].reg =
        (unsigned long *)DISP_REG_CONFIG_DISP_TOVL0_PQOUT_MDP_RDMA4_SEL_IN;
    sel_in_map[10].reg =
        (unsigned long *)DISP_REG_CONFIG_DISP_MDP_AAL4_MDP_HDR4_SEL_IN;
    sel_in_map[11].reg =
        (unsigned long *)DISP_REG_CONFIG_DISP_MDP_AAL4_SEL_IN;

    for (i = 0; i < 16; i++) {
        DISP_REG_SET_FIELD(NULL, REG_FLD_MMU_EN,
            DISP_REG_SMI_LARB0_NON_SEC_CON + i * 4, 0x0);
        DISP_REG_SET_FIELD(NULL, REG_FLD_MMU_EN,
            DISP_REG_SMI_LARB1_NON_SEC_CON + i * 4, 0x0);
    }

    return 0;
}

static struct module_map_t module_mutex_map[DISP_MODULE_NUM] = {
    {DISP_MODULE_CONFIG, -1, 0},
    {DISP_MODULE_MUTEX, -1, 0},
    {DISP_MODULE_SMI_COMMON, -1, 0},
    {DISP_MODULE_SMI_LARB0, -1, 0},
    {DISP_MODULE_SMI_LARB1, -1, 0},

    {DISP_MODULE_OVL0, 0, 0},
    {DISP_MODULE_OVL0_2L, 1, 0},
    {DISP_MODULE_RDMA0, 2, 0},
    {DISP_MODULE_RSZ0, 3, 0},
    {DISP_MODULE_COLOR0, 4, 0},

    {DISP_MODULE_CCORR0, 5, 0},
    {DISP_MODULE_AAL0, 6, 0},
    {DISP_MODULE_GAMMA0, 7, 0},
    {DISP_MODULE_POSTMASK0, 8, 0},
    {DISP_MODULE_DITHER0, 9, 0},

    {DISP_MODULE_DSC, 10, 0},
    {DISP_MODULE_DSC_WRAP0_CORE1, 11, 0},
    {DISP_MODULE_DSI0, 12, 0},
    {DISP_MODULE_WDMA0, 13, 0},
    {DISP_MODULE_UFBC_WDMA0, 14, 0},

    {DISP_MODULE_OVL2_2L, 16, 0},
    {DISP_MODULE_RDMA4, 17, 0},
    {DISP_MODULE_DPI, 18, 0},
    {DISP_MODULE_MDP_RDMA4, 19, 0},
    {DISP_MODULE_MDP_HDR4, 20, 0},

    {DISP_MODULE_MDP_RSZ4, 21, 0},
    {DISP_MODULE_MDP_AAL4, 22, 0},
    {DISP_MODULE_MDP_TDSHP4, 23, 0},
    {DISP_MODULE_MDP_COLOR4, 24, 0},
    {DISP_MODULE_Y2R0, 25, 0},

    {DISP_MODULE_PWM0, 15, 0},
    {DISP_MODULE_DSIDUAL, -1, 0},
    {DISP_MODULE_DSI1, -1, 0},
    {DISP_MODULE_MIPI0, -1, 0},
    {DISP_MODULE_MIPI1, -1, 0},

    {DISP_MODULE_OVL0_2L_VIRTUAL0, -1, 0},
    {DISP_MODULE_OVL0_VIRTUAL0, -1, 0},
    {DISP_MODULE_OVL0_OVL0_2L_VIRTUAL, -1, 0},
    {DISP_MODULE_RDMA2_VIRTUAL, -1, 0},
    {DISP_MODULE_DMDP_TDSHP4_SOUT, -1, 0},

    {DISP_MODULE_SPLIT0, -1, 0},
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

int ddp_get_module_num(enum DDP_SCENARIO_ENUM scenario)
{
    return ddp_get_module_num_l(module_list_scenario[scenario]);
}

static void ddp_print_scenario(enum DDP_SCENARIO_ENUM scenario)
{
    int i = 0;
    char path[512] = {'\0'};
    int num = ddp_get_module_num(scenario);

    for (i = 0; i < num; i++) {
        strncat(path, ddp_get_module_name(module_list_scenario[scenario][i]),
             (sizeof(path) - strlen(path) - 1));
    }

    DDPMSG("scenario %s have modules: %s\n", ddp_get_scenario_name(scenario), path);
}

static int ddp_find_module_index(enum DDP_SCENARIO_ENUM ddp_scenario, enum DISP_MODULE_ENUM module)
{
    int i = 0;

    for (i = 0; i < DDP_ENING_NUM; i++) {
        if (module_list_scenario[ddp_scenario][i] == (int)module)
            return i;

    }
    DDPDBG("find module: can not find module %s on scenario %s\n", ddp_get_module_name(module),
           ddp_get_scenario_name(ddp_scenario));
    return -1;
}

/* set display interface when kernel init */
int ddp_set_dst_module(enum DDP_SCENARIO_ENUM scenario, enum DISP_MODULE_ENUM dst_module)
{
    int i = 0;

    DDPMSG("%s, scenario=%s, dst_module=%s\n", __func__,
           ddp_get_scenario_name(scenario), ddp_get_module_name(dst_module));
    if (ddp_find_module_index(scenario, dst_module) > 0) {
        DDPDBG("%s is already on path\n", ddp_get_module_name(dst_module));
        return 0;
    }
    i = ddp_get_module_num_l(module_list_scenario[scenario]) - 1;
    ASSERT(i >= 0);
    if (dst_module == DISP_MODULE_DSIDUAL) {
        if (i < (DDP_ENING_NUM - 1)) {
            module_list_scenario[scenario][i++] = DISP_MODULE_SPLIT0;
        } else {
            DDPERR("set dst module over up bound\n");
            return -1;
        }
    } else {
        if (ddp_get_dst_module(scenario) == DISP_MODULE_DSIDUAL) {
            if (i >= 1) {
                module_list_scenario[scenario][i--] = -1;
            } else {
                DDPERR("set dst module over low bound\n");
                return -1;
            }
        }
    }
    module_list_scenario[scenario][i] = dst_module;
    if (scenario == DDP_SCENARIO_PRIMARY_ALL)
        ddp_set_dst_module(DDP_SCENARIO_PRIMARY_DISP, dst_module);

    ddp_print_scenario(scenario);
    return 0;
}

enum DISP_MODULE_ENUM ddp_get_dst_module(enum DDP_SCENARIO_ENUM ddp_scenario)
{
    enum DISP_MODULE_ENUM module_name = DISP_MODULE_UNKNOWN;
    int module_num = ddp_get_module_num_l(module_list_scenario[ddp_scenario]) - 1;

    if (module_num >= 0)
        module_name = module_list_scenario[ddp_scenario][module_num];

    return module_name;
}

int *ddp_get_scenario_list(enum DDP_SCENARIO_ENUM ddp_scenario)
{
    return module_list_scenario[ddp_scenario];
}

int ddp_is_module_in_scenario(enum DDP_SCENARIO_ENUM ddp_scenario, enum DISP_MODULE_ENUM module)
{
    int i = 0;

    for (i = 0; i < DDP_ENING_NUM; i++) {
        if (module_list_scenario[ddp_scenario][i] == (int)module)
            return 1;

    }
    return 0;
}

int ddp_insert_module(enum DDP_SCENARIO_ENUM ddp_scenario, enum DISP_MODULE_ENUM place,
              enum DISP_MODULE_ENUM module)
{
    int i = DDP_ENING_NUM - 1;
    int idx = ddp_find_module_index(ddp_scenario, place);

    if (idx < 0) {
        DDPERR("error: %s , place=%s is not in scenario %s!\n",
               __func__, ddp_get_module_name(place), ddp_get_scenario_name(ddp_scenario));
        return -1;
    }

    for (i = 0; i < DDP_ENING_NUM; i++) {
        if (module_list_scenario[ddp_scenario][i] == (int)module) {
            DDPERR("error: %s , module=%s is already in scenario %s!\n",
                   __func__, ddp_get_module_name(module), ddp_get_scenario_name(ddp_scenario));
            return -1;
        }
    }

    /* should have empty room for insert */
    ASSERT(module_list_scenario[ddp_scenario][DDP_ENING_NUM - 1] == -1);

    for (i = DDP_ENING_NUM - 2; i >= idx; i--)
        module_list_scenario[ddp_scenario][i + 1] = module_list_scenario[ddp_scenario][i];
    module_list_scenario[ddp_scenario][idx] = module;

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

    if (idx < 0) {
        DDPERR("%s, can not find module %s in scenario %s\n", __func__,
               ddp_get_module_name(module), ddp_get_scenario_name(ddp_scenario));
        return -1;
    }

    for (i = idx; i < DDP_ENING_NUM - 1; i++)
        module_list_scenario[ddp_scenario][i] = module_list_scenario[ddp_scenario][i + 1];
    module_list_scenario[ddp_scenario][DDP_ENING_NUM - 1] = -1;

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
    DDPDBG("path connect on scenario %s\n", ddp_get_scenario_name(scenario));
    if (scenario == DDP_SCENARIO_PRIMARY_ALL) {
        ddp_connect_path_l(module_list_scenario[DDP_SCENARIO_PRIMARY_DISP], handle);
        ddp_connect_path_l(module_list_scenario[DDP_SCENARIO_PRIMARY_OVL_MEMOUT], handle);
    } else if (scenario == DDP_SCENARIO_SUB_ALL) {
        ddp_connect_path_l(module_list_scenario[DDP_SCENARIO_SUB_DISP], handle);
        ddp_connect_path_l(module_list_scenario[DDP_SCENARIO_SUB_OVL2_2L_MEMOUT], handle);
    } else {
        ddp_connect_path_l(module_list_scenario[scenario], handle);
    }
    return;
}

void ddp_disconnect_path(enum DDP_SCENARIO_ENUM scenario, void *handle)
{
    DDPDBG("path disconnect on scenario %s\n", ddp_get_scenario_name(scenario));

    if (scenario == DDP_SCENARIO_PRIMARY_ALL) {
        ddp_disconnect_path_l(module_list_scenario[DDP_SCENARIO_PRIMARY_DISP], handle);
        ddp_disconnect_path_l(module_list_scenario[DDP_SCENARIO_PRIMARY_OVL_MEMOUT],
                      handle);
    } else if (scenario == DDP_SCENARIO_SUB_ALL) {
        ddp_disconnect_path_l(module_list_scenario[DDP_SCENARIO_SUB_DISP], handle);
        ddp_disconnect_path_l(module_list_scenario[DDP_SCENARIO_SUB_OVL2_2L_MEMOUT], handle);
    } else {
        ddp_disconnect_path_l(module_list_scenario[scenario], handle);
    }
    return;
}

void ddp_check_path(enum DDP_SCENARIO_ENUM scenario)
{
    DDPDBG("path check path on scenario %s\n", ddp_get_scenario_name(scenario));

    if (scenario == DDP_SCENARIO_PRIMARY_ALL) {
        ddp_check_path_l(module_list_scenario[DDP_SCENARIO_PRIMARY_DISP]);
        ddp_check_path_l(module_list_scenario[DDP_SCENARIO_PRIMARY_OVL_MEMOUT]);
    } else {
        ddp_check_path_l(module_list_scenario[scenario]);
    }
    return;
}

void ddp_check_mutex(int mutex_id, enum DDP_SCENARIO_ENUM scenario, enum DDP_MODE mode)
{
    DDPDBG("check mutex %d on scenario %s\n", mutex_id, ddp_get_scenario_name(scenario));
    ddp_check_mutex_l(mutex_id, module_list_scenario[scenario], mode);
    return;
}

int ddp_mutex_set(int mutex_id, enum DDP_SCENARIO_ENUM scenario, enum DDP_MODE mode, void *handle)
{
    if (scenario < DDP_SCENARIO_MAX) {
        return ddp_mutex_set_l(mutex_id, module_list_scenario[scenario], mode, handle);
    } else {
        DDPERR("Invalid scenario %d when setting mutex\n", scenario);
        return -1;
    }
    return 0;
}

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

int ddp_mutex_reset(int mutex_id, void *handle)
{
    DDPDBG("mutex %d reset\n", mutex_id);
    DISP_REG_SET(handle, DISP_REG_CONFIG_MUTEX_RST(mutex_id), 1);
    DISP_REG_SET(handle, DISP_REG_CONFIG_MUTEX_RST(mutex_id), 0);

    return 0;
}

int ddp_is_moudule_in_mutex(int mutex_id, enum DISP_MODULE_ENUM module)
{
    int ret = 0;
    uint32_t real_value = 0;

    if (mutex_id < DISP_MUTEX_DDP_FIRST || mutex_id > DISP_MUTEX_DDP_LAST) {
        DDPDUMP("error:check_moudule_in_mute fail:exceed mutex max (0 ~ %d)\n",
            DISP_MUTEX_DDP_LAST);
        return ret;
    }

    if (module_mutex_map[module].mod_num == 0)
        real_value = DISP_REG_GET(DISP_REG_CONFIG_MUTEX_MOD0(mutex_id));

    if (1 == ((real_value >> module_mutex_map[module].bit) & 0x01))
        ret = 1;

    return ret;
}


int ddp_mutex_add_module(int mutex_id, enum DISP_MODULE_ENUM module, void *handle)
{
    int value = 0;

    if (module < DISP_MODULE_UNKNOWN) {
        if (module_mutex_map[module].bit != -1) {
            DDPDBG("module %s added to mutex %d\n", ddp_get_module_name(module),
                   mutex_id);
            value |= (1 << module_mutex_map[module].bit);
            if (module_mutex_map[module].mod_num == 0)
                DISP_REG_MASK(handle, DISP_REG_CONFIG_MUTEX_MOD0(mutex_id), value, value);
            else if (module_mutex_map[module].mod_num == 1)
                DISP_REG_MASK(handle, DISP_REG_CONFIG_MUTEX_MOD1(mutex_id), value, value);
        }
    }
    return 0;
}

int ddp_mutex_remove_module(int mutex_id, enum DISP_MODULE_ENUM module, void *handle)
{
    int value = 0;

    if (module < DISP_MODULE_UNKNOWN) {
        if (module_mutex_map[module].bit != -1) {
            DDPDBG("module %s added to mutex %d\n", ddp_get_module_name(module),
                   mutex_id);
            value |= (1 << module_mutex_map[module].bit);
            if (module_mutex_map[module].mod_num == 0)
                DISP_REG_MASK(handle, DISP_REG_CONFIG_MUTEX_MOD0(mutex_id), 0, value);
            else if (module_mutex_map[module].mod_num == 1)
                DISP_REG_MASK(handle, DISP_REG_CONFIG_MUTEX_MOD1(mutex_id), 0, value);
        }

    }
    return 0;
}

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

int ddp_mutex_enable(int mutex_id, enum DDP_SCENARIO_ENUM scenario, void *handle)
{
    return ddp_mutex_enable_l(mutex_id, handle);
}

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

int ddp_check_engine_status(int mutexID)
{
    /* check engines' clock bit &  enable bit & status bit before unlock mutex */
    /* should not needed, in comdq do? */
    int result = 0;
    return result;
}

int ddp_path_top_clock_on(void)
{
    DDPMSG("ddp path top clock on\n");
    //spm_mtcmos_ctrl_dis(1);
    ddp_enable_module_clock(DISP_MODULE_SMI_COMMON);
    ddp_enable_module_clock(DISP_MODULE_SMI_LARB0);

    DDPMSG("ddp CG0:%x CG1:%x\n", DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON0),
        DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON1));

#if 0  /* Force CG always on */
    DDPMSG("[FPGA Only] before power on MMSYS:0x%x,0x%x,0x%x\n",
           DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON0),
           DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON1),
           DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON2));

    DISP_REG_SET(NULL, DISP_REG_CONFIG_MMSYS_CG_CLR0, 0xFFFFFFFF);
    DISP_REG_SET(NULL, DISP_REG_CONFIG_MMSYS_CG_CLR1, 0xFFFFFFFF);
    DISP_REG_SET(NULL, DISP_REG_CONFIG_MMSYS_CG_CLR2, 0xFFFFFFFF);
    /*DISP_REG_SET(NULL, DISP_REG_CONFIG_MMSYS_CG_CLR2, 0xFFFFFFFF);*/
    DDPMSG("[FPGA Only] before power on MMSYS:0x%x,0x%x,0x%x\n",
           DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON0),
           DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON1),
           DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON2));
#endif

    DDPMSG("SMI subcom before config:(0x%x,0x%x)\n",
           DISP_REG_GET(DISP_REG_SMI_SUBCOM0_L1LEN),
           DISP_REG_GET(DISP_REG_SMI_SUBCOM1_L1LEN));

    DISP_REG_SET(NULL, DISP_REG_SMI_SUBCOM0_L1LEN, 0XA);
    DISP_REG_SET(NULL, DISP_REG_SMI_SUBCOM1_L1LEN, 0XA);

    DDPMSG("SMI subcom after config:(0x%x,0x%x)\n",
           DISP_REG_GET(DISP_REG_SMI_SUBCOM0_L1LEN),
           DISP_REG_GET(DISP_REG_SMI_SUBCOM1_L1LEN));

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
    for (i = 0; i < 4; i++)
        DISP_REG_SET_FIELD(0, REG_FLD_MMU_EN, DISP_REG_SMI_LARB0_MMU_EN + i*4, 0);
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

    for (i = 0; i < DDP_ENING_NUM; i++)
        module_list_scenario[DDP_SCENARIO_PRIMARY_DISP][i] =
            primary_disp_dsc_module_list[i];
}
