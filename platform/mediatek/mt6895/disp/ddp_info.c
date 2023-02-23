/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#define LOG_TAG "INFO"

#include "ddp_info.h"
#include "ddp_log.h"
#include "disp_drv_platform.h"

struct ddp_module ddp_modules[DISP_MODULE_NUM] = {
        {DISP_MODULE_OVL0, "ovl0", 1, &ddp_driver_ovl}, /* 0 */
        {DISP_MODULE_OVL0_2L, "ovl0_2l", 1, &ddp_driver_ovl},
        {DISP_MODULE_OVL0_2L_VIRTUAL0, "ovl0_2l_virt", 1, NULL},
        {DISP_MODULE_OVL0_VIRTUAL0, "ovl0_virt", 1, NULL},
        {DISP_MODULE_RSZ0, "disp_rsz0", 1, NULL},

        {DISP_MODULE_OVL1, "ovl1", 1, &ddp_driver_ovl}, /* 5 */
        {DISP_MODULE_OVL1_2L, "ovl1_2l", 1, &ddp_driver_ovl},
        {DISP_MODULE_OVL1_2L_VIRTUAL0, "ovl1_2l_virt", 1, NULL},
        {DISP_MODULE_OVL1_VIRTUAL0, "ovl1_virt", 1, NULL},
        {DISP_MODULE_RSZ1, "disp_rsz1", 1, NULL},

        {DISP_MODULE_RDMA0, "rdma0", 1, &ddp_driver_rdma}, /* 10 */
        {DISP_MODULE_RDMA0_VIRTUAL0, "rdma0_virt", 1, NULL},
        {DISP_MODULE_WDMA0, "wdma0", 1, NULL},
        {DISP_MODULE_COLOR0, "color0", 1, &ddp_driver_color},
        {DISP_MODULE_CCORR0, "ccorr0", 1, &ddp_driver_ccorr},

        {DISP_MODULE_RDMA1, "rdma1", 1, &ddp_driver_rdma}, /* 15 */
        {DISP_MODULE_RDMA1_VIRTUAL0, "rdma1_virt", 1, NULL},
        {DISP_MODULE_WDMA1, "wdma1", 1, NULL},
        {DISP_MODULE_COLOR1, "color1", 1, &ddp_driver_color},
        {DISP_MODULE_CCORR1, "ccorr1", 1, &ddp_driver_ccorr},

        {DISP_MODULE_AAL0, "aal0", 1, &ddp_driver_aal}, /* 20 */
        {DISP_MODULE_MDP_AAL0, "mdp_aal0", 1, &ddp_driver_aal},
        {DISP_MODULE_GAMMA0, "gamma0", 1, &ddp_driver_gamma},
        {DISP_MODULE_POSTMASK0, "postmask0", 1, &ddp_driver_postmask},
        {DISP_MODULE_DITHER0, "dither0", 1, &ddp_driver_dither},

        {DISP_MODULE_DITHER0_VIRTUAL0, "dither0_virt", 1, NULL}, /* 25 */
        {DISP_MODULE_AAL1, "aal1", 1, &ddp_driver_aal},
        {DISP_MODULE_MDP_AAL1, "mdp_aal1", 1, &ddp_driver_aal},
        {DISP_MODULE_GAMMA1, "gamma1", 1, &ddp_driver_gamma},
        {DISP_MODULE_POSTMASK1, "postmask1", 1, &ddp_driver_postmask},

        {DISP_MODULE_DITHER1, "dither1", 1, &ddp_driver_dither}, /* 30 */
        {DISP_MODULE_DITHER1_VIRTUAL0, "dither1_virt", 1, NULL},
        {DISP_MODULE_CCORR1_1, "ccorr1_1", 1, &ddp_driver_ccorr},
        {DISP_MODULE_CCORR0_1, "ccorr0_1", 1, &ddp_driver_ccorr},
        {DISP_MODULE_CM0, "cm0", 1, &ddp_driver_cm},

        {DISP_MODULE_CM1, "cm1", 1, &ddp_driver_cm}, /* 35 */
        {DISP_MODULE_SPR0, "spr0", 1, &ddp_driver_spr},
        {DISP_MODULE_SPR1, "spr1", 1, &ddp_driver_spr},
        {DISP_MODULE_TDSHP0, "tdshp0", 1, &ddp_driver_tdshp},
        {DISP_MODULE_TDSHP1, "tdshp1", 1, &ddp_driver_tdshp},

        {DISP_MODULE_C3D0, "c3d0", 1, &ddp_driver_c3d}, /* 40 */
        {DISP_MODULE_C3D1, "c3d1", 1, &ddp_driver_c3d},
        {DISP_MODULE_SPLIT0, "split0", 0, NULL},
        {DISP_MODULE_DSI0, "dsi0", 1, &ddp_driver_dsi0},
        {DISP_MODULE_DSI1, "dsi1", 1, NULL},

        {DISP_MODULE_DSIDUAL, "dsidual", 0, NULL}, /* 45 */
        {DISP_MODULE_PWM0, "pwm0", 0, NULL},
        {DISP_MODULE_CONFIG, "config", 0, NULL},
        {DISP_MODULE_MUTEX, "mutex", 0, NULL},
        {DISP_MODULE_SMI_COMMON, "sim_common", 0, NULL},

        {DISP_MODULE_SMI_LARB0, "smi_larb0", 0, NULL}, /* 50 */
        {DISP_MODULE_SMI_LARB1, "smi_larb1", 0, NULL},
        {DISP_MODULE_MIPI0, "mipi0", 0, NULL},
        {DISP_MODULE_MIPI1, "mipi1", 0, NULL},
        {DISP_MODULE_DPI, "dpi", 0, NULL},

        {DISP_MODULE_OVL2_2L, "ovl2_2l", 1, &ddp_driver_ovl}, /* 55 */
        {DISP_MODULE_OVL3_2L, "ovl3_2l", 1, &ddp_driver_ovl},
        {DISP_MODULE_RDMA4, "rdma4", 1, &ddp_driver_rdma},
        {DISP_MODULE_RDMA5, "rdma5", 1, &ddp_driver_rdma},
        {DISP_MODULE_MDP_RDMA4, "mdp_rdma4", 0, &ddp_driver_rdma},

        {DISP_MODULE_MDP_RDMA5, "mdp_rdma5", 0, &ddp_driver_rdma}, /* 60 */
        {DISP_MODULE_MDP_RSZ4, "mdp_rsz4", 0, &ddp_driver_rdma},
        {DISP_MODULE_MDP_RSZ5, "mdp_rsz5", 0, &ddp_driver_rdma},
        {DISP_MODULE_MERGE0, "merge0", 0, &ddp_driver_rdma},
        {DISP_MODULE_MERGE1, "merge1", 0, &ddp_driver_rdma},

        {DISP_MODULE_DP_INTF, "dp_intf", 0, &ddp_driver_rdma}, /* 65 */
        {DISP_MODULE_DSC, "dsc", 1, &ddp_driver_dsc},
        {DISP_MODULE_DSC_VIRTUAL0, "dsc_virt", 1, NULL},
        {DISP_MODULE_DSC_VIRTUAL1, "dsc_virt1", 1, NULL},
        {DISP_MODULE_OVL0_VIRTUAL1, "ovl0_virt1", 1, NULL},

        {DISP_MODULE_RDMA0_OUT_RELAY, "rdma0_orelay", 1, NULL}, /* 70 */
        {DISP_MODULE_PQ0_VIRTUAL, "pq0_virt", 1, NULL},
        {DISP_MODULE_MAIN0_VIRTUAL, "main0_virt", 1, NULL},
        {DISP_MODULE_DLI_ASYNC0, "disp0_dli_async0", 0, NULL},
        {DISP1_MODULE_CONFIG, "mmsys1", 0, NULL},

        {DISP1_MODULE_MUTEX, "mutex1", 0, NULL},  /* 75 */
        {DISP1_MODULE_SMI_COMMON, "smi_common1", 0, NULL},
        {DISP_MODULE_SMI_LARB20, "smi_larb20", 0, NULL},
        {DISP_MODULE_SMI_LARB21, "smi_larb21", 0, NULL},
        {DISP1_MODULE_OVL0, "disp1_ovl0", 1, &ddp_driver_ovl},

        {DISP1_MODULE_OVL0_2L, "disp1_ovl0_2l", 1, &ddp_driver_ovl}, /* 80 */
        {DISP1_MODULE_OVL0_VIRTUAL0, "disp1_ovl0_virt0", 1, NULL},
        {DISP1_MODULE_OVL0_VIRTUAL1, "disp1_ovl0_virt1", 1, NULL},
        {DISP1_MODULE_RDMA0, "disp1_rdma0", 1, &ddp_driver_rdma},
        {DISP1_MODULE_RDMA0_OUT_RELAY, "disp1_rdma0_orelay", 1, NULL},

        {DISP1_MODULE_PQ0_VIRTUAL, "disp1_pq0_virt", 1, NULL}, /* 85 */
        {DISP1_MODULE_MAIN0_VIRTUAL, "disp1_main0_virt", 1, NULL},
        {DISP1_MODULE_DLO_ASYNC0, "disp1_dlo_async0", 1, NULL},
        {DISP1_MODULE_OVL1_2L, "disp1_ovl1_2l", 1, &ddp_driver_ovl},

        {DISP_MODULE_UNKNOWN, "unknown", 0, NULL},
};


struct DDP_MODULE_DRIVER  *ddp_modules_driver[DISP_MODULE_NUM] = {0};
struct DISP_MUTEX_REGS *g_DISP_MUTEX_BASE = (struct DISP_MUTEX_REGS *)MM_MUTEX_BASE;
struct DISP_MUTEX_REGS *g_DISP1_MUTEX_BASE = (struct DISP_MUTEX_REGS *)MM1_MUTEX_BASE;
struct DISP_OVL0_REGS *g_DISP_OVL0_BASE = (struct DISP_OVL0_REGS *)OVL0_BASE;
struct DISP_OVL0_REGS *g_DISP1_OVL0_BASE = (struct DISP_OVL0_REGS *)DISP1_OVL0_BASE;

int ddp_enable_module_clock(enum DISP_MODULE_ENUM module)
{
    /*todo CG for PQ module*/

#ifdef DISP_HELPER_STAGE_NORMAL_LK
    switch (module) {
        /* MMSYS_CG_CON0 */
    case DISP_MODULE_SMI_COMMON:
        //DISP_REG_SET_FIELD(NULL, FLD_CG1_SMI_COMMON, DISP_REG_CONFIG_MMSYS_CG_CLR1, 1);
        //DISP_REG_SET_FIELD(NULL, FLD_CG1_SMI_GALS, DISP_REG_CONFIG_MMSYS_CG_CLR1, 1);
        //DISP_REG_SET_FIELD(NULL, FLD_CG1_SMI_INFRA, DISP_REG_CONFIG_MMSYS_CG_CLR1, 1);
        //DISP_REG_SET_FIELD(NULL, FLD_CG1_SMI_IOMMU, DISP_REG_CONFIG_MMSYS_CG_CLR1, 1);
        break;
    case DISP_MODULE_SMI_LARB0:
        // DISP_REG_SET_FIELD(NULL, FLD_CG0_SMI_LARB0,
        //                      DISP_REG_CONFIG_MMSYS_CG_CLR0, 1);
        break;
    case DISP_MODULE_SMI_LARB1:
         //DISP_REG_SET_FIELD(NULL,MMSYS_CG_FLD_CG0_SMI_LARB1,
         //                     DISP_REG_CONFIG_MMSYS_CG_CLR0, 1);
        break;
    case DISP_MODULE_OVL0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_DISP_OVL0, DISP_REG_CONFIG_MMSYS_CG_CLR0, 1);
        break;
    case DISP_MODULE_OVL0_2L:
        DISP_REG_SET_FIELD(NULL, FLD_CG1_DISP_OVL0_2L, DISP_REG_CONFIG_MMSYS_CG_CLR0, 1);
        break;
    case DISP_MODULE_OVL1_2L:
        DISP_REG_SET_FIELD(NULL, FLD_CG1_DISP_OVL1_2L, DISP_REG_CONFIG_MMSYS_CG_CLR0, 1);
        break;
    case DISP_MODULE_RDMA0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_DISP_RDMA0, DISP_REG_CONFIG_MMSYS_CG_CLR1, 1);
        break;
    case DISP_MODULE_RDMA1:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_DISP_RDMA1, DISP_REG_CONFIG_MMSYS_CG_CLR1, 1);
        break;
    case DISP_MODULE_WDMA0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_DISP_WDMA0, DISP_REG_CONFIG_MMSYS_CG_CLR1, 1);
        break;
    case DISP_MODULE_COLOR0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_DISP_COLOR0, DISP_REG_CONFIG_MMSYS_CG_CLR0, 1);
        break;
    case DISP_MODULE_CCORR0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_DISP_CCORR0, DISP_REG_CONFIG_MMSYS_CG_CLR0, 1);
        break;
    case DISP_MODULE_CCORR1:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_DISP_CCORR1, DISP_REG_CONFIG_MMSYS_CG_CLR0, 1);
        break;
    case DISP_MODULE_AAL0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_DISP_AAL0, DISP_REG_CONFIG_MMSYS_CG_CLR1, 1);
        break;
    case DISP_MODULE_GAMMA0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_DISP_GAMMA0, DISP_REG_CONFIG_MMSYS_CG_CLR1, 1);
        break;
    case DISP_MODULE_POSTMASK0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_POSTMASK0, DISP_REG_CONFIG_MMSYS_CG_CLR0, 1);
        break;
    case DISP_MODULE_DITHER0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_DITHER0, DISP_REG_CONFIG_MMSYS_CG_CLR0, 1);
        break;
    case DISP_MODULE_DSI0:
        DISP_REG_SET_FIELD(NULL, FLD_CG2_26MHZ_URGENT, DISP_REG_CONFIG_MMSYS_CG_CLR2, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG2_26MHZ_URGENT, DISP1_REG_CONFIG_MMSYS_CG_CLR2, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG0_DSI0_MM, DISP_REG_CONFIG_MMSYS_CG_CLR0, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG2_DSI0, DISP_REG_CONFIG_MMSYS_CG_CLR2, 1);
        break;
    case DISP_MODULE_DSI1:
        break;
    case DISP_MODULE_DSIDUAL:
        DISP_REG_SET_FIELD(NULL, FLD_CG2_26MHZ_URGENT, DISP_REG_CONFIG_MMSYS_CG_CLR2, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG2_26MHZ_URGENT, DISP1_REG_CONFIG_MMSYS_CG_CLR2, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG0_DSI0_MM, DISP_REG_CONFIG_MMSYS_CG_CLR0, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG2_DSI0, DISP_REG_CONFIG_MMSYS_CG_CLR2, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG2_DSI1, DISP_REG_CONFIG_MMSYS_CG_CLR2, 1);
        break;
    case DISP_MODULE_RSZ0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_DISP_RSZ0, DISP_REG_CONFIG_MMSYS_CG_CLR0, 1);
        break;
    case DISP_MODULE_DSC:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_DISP_DSC_WRAP0, DISP_REG_CONFIG_MMSYS_CG_CLR1, 1);
        break;
    default:
        DDPERR("enable module clock unknown module %d\n", module);
    }
#endif
    return 0;
}

int ddp_disable_module_clock(enum DISP_MODULE_ENUM module)
{
    /*todo CG for PQ module*/

#ifdef DISP_HELPER_STAGE_NORMAL_LK
    switch (module) {
        /* MMSYS_CG_CON0 */
    case DISP_MODULE_SMI_COMMON:
        //DISP_REG_SET_FIELD(NULL, FLD_CG1_SMI_IOMMU, DISP_REG_CONFIG_MMSYS_CG_SET1, 1);
        //DISP_REG_SET_FIELD(NULL, FLD_CG1_SMI_INFRA, DISP_REG_CONFIG_MMSYS_CG_SET1, 1);
        //DISP_REG_SET_FIELD(NULL, FLD_CG1_SMI_GALS, DISP_REG_CONFIG_MMSYS_CG_SET1, 1);
        //DISP_REG_SET_FIELD(NULL, FLD_CG1_SMI_COMMON, DISP_REG_CONFIG_MMSYS_CG_SET1, 1);
        break;
    case DISP_MODULE_SMI_LARB0:
        /* DISP_REG_SET_FIELD(NULL, FLD_CG0_SMI_LARB0, DISP_REG_CONFIG_MMSYS_CG_SET0, 1); */
        break;
    case DISP_MODULE_SMI_LARB1:
        /* DISP_REG_SET_FIELD(NULL,MMSYS_CG_FLD_CG0_SMI_LARB1, DISP_REG_CONFIG_MMSYS_CG_SET0, 1); */
        break;
    case DISP_MODULE_OVL0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_DISP_OVL0, DISP_REG_CONFIG_MMSYS_CG_SET0, 1);
        break;
    case DISP_MODULE_OVL0_2L:
        DISP_REG_SET_FIELD(NULL, FLD_CG1_DISP_OVL0_2L, DISP_REG_CONFIG_MMSYS_CG_SET0, 1);
        break;
    case DISP_MODULE_OVL1_2L:
        DISP_REG_SET_FIELD(NULL, FLD_CG1_DISP_OVL1_2L, DISP_REG_CONFIG_MMSYS_CG_SET0, 1);
        break;
    case DISP_MODULE_RDMA0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_DISP_RDMA0, DISP_REG_CONFIG_MMSYS_CG_SET1, 1);
        break;
    case DISP_MODULE_WDMA0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_DISP_WDMA0, DISP_REG_CONFIG_MMSYS_CG_SET1, 1);
        break;
    case DISP_MODULE_COLOR0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_DISP_COLOR0, DISP_REG_CONFIG_MMSYS_CG_SET0, 1);
        break;
    case DISP_MODULE_CCORR0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_DISP_CCORR0, DISP_REG_CONFIG_MMSYS_CG_SET0, 1);
        break;
    case DISP_MODULE_AAL0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_DISP_AAL0, DISP_REG_CONFIG_MMSYS_CG_SET1, 1);
        break;
    case DISP_MODULE_GAMMA0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_DISP_GAMMA0, DISP_REG_CONFIG_MMSYS_CG_SET1, 1);
        break;
    case DISP_MODULE_POSTMASK0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_POSTMASK0, DISP_REG_CONFIG_MMSYS_CG_SET0, 1);
        break;
    case DISP_MODULE_DITHER0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_DITHER0, DISP_REG_CONFIG_MMSYS_CG_SET0, 1);
        break;
    case DISP_MODULE_DSI0:
        DISP_REG_SET_FIELD(NULL, FLD_CG2_DSI0, DISP_REG_CONFIG_MMSYS_CG_SET2, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG0_DSI0_MM, DISP_REG_CONFIG_MMSYS_CG_SET0, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG2_26MHZ_URGENT, DISP_REG_CONFIG_MMSYS_CG_SET2, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG2_26MHZ_URGENT, DISP1_REG_CONFIG_MMSYS_CG_SET2, 1);
        break;
    case DISP_MODULE_DSI1:
        break;
    case DISP_MODULE_DSIDUAL:
        break;
    case DISP_MODULE_RSZ0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_DISP_RSZ0, DISP_REG_CONFIG_MMSYS_CG_SET0, 1);
        break;
    default:
        DDPERR("%s unknown module %d\n", __func__, module);
    }
#endif
    DDPMSG("disable %s clk, CG0 0x%x, CG1 0x%x",
           ddp_get_module_name(module),
           DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON0),
           DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON1));
    return 0;
}


unsigned int is_ddp_module(enum DISP_MODULE_ENUM module)
{
    if (module < DISP_MODULE_NUM)
        return 1;
    else
        return 0;
}

const char *ddp_get_module_name(enum DISP_MODULE_ENUM module)
{
    unsigned int mod = (unsigned int)module;
    if (is_ddp_module(module)) {
        return ddp_modules[module].module_name;
    } else {
        DDPMSG("invalid module id=%d", module);
        return "unknown";
    }
}


void ddp_init_modules_driver(void)
{
    unsigned int i = 0;

    for (i = 0; i < DISP_MODULE_NUM; i++)
        ddp_modules_driver[i] = ddp_modules[i].module_driver;

    DISP_REG_SET(NULL, DISP_REG_CONFIG_GCE_EVENT_SEL, 0x3);
    DISP_REG_SET(NULL, DISP_REG_CONFIG_BYPASS_MUX_SHADOW, 1);
    DISP_REG_SET(NULL, DISP1_REG_CONFIG_GCE_EVENT_SEL, 0x3);
    DISP_REG_SET(NULL, DISP1_REG_CONFIG_BYPASS_MUX_SHADOW, 1);
}

