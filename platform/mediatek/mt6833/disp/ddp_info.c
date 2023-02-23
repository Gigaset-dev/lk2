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

struct ddp_module  ddp_modules[DISP_MODULE_NUM] = {
        {DISP_MODULE_CONFIG, "disp_config", 0, NULL},
        {DISP_MODULE_MUTEX, "mutex", 0, NULL},
        {DISP_MODULE_SMI_COMMON, "sim_common", 0, NULL},
        {DISP_MODULE_SMI_LARB0, "smi_larb0", 0, NULL},
        {DISP_MODULE_SMI_LARB1, "smi_larb1", 0, NULL},

        {DISP_MODULE_OVL0, "ovl0", 1, &ddp_driver_ovl},
        {DISP_MODULE_OVL0_2L, "ovl0_2l", 1, &ddp_driver_ovl},
        {DISP_MODULE_RDMA0, "rdma0", 1, &ddp_driver_rdma},
        {DISP_MODULE_RSZ0, "disp_rsz0", 1, NULL},
        {DISP_MODULE_COLOR0, "color0", 1, &ddp_driver_color},

        {DISP_MODULE_CCORR0, "ccorr0", 1, &ddp_driver_ccorr},
        {DISP_MODULE_AAL0, "aal0", 1, &ddp_driver_aal},
        {DISP_MODULE_GAMMA0, "gamma0", 1, &ddp_driver_gamma},
        {DISP_MODULE_POSTMASK0, "postmask0", 1, &ddp_driver_postmask},
        {DISP_MODULE_DITHER0, "dither0", 1, &ddp_driver_dither},

        {DISP_MODULE_DSI0, "dsi0", 1, &ddp_driver_dsi0},
        {DISP_MODULE_WDMA0, "wdma0", 1, NULL},
        {DISP_MODULE_PWM0, "pwm0", 0, NULL},
        {DISP_MODULE_MIPI0, "mipi0", 0, NULL},
        {DISP_MODULE_DSIDUAL, "dsidual", 0, NULL},
        {DISP_MODULE_DSI1, "dsi1", 0, NULL},

        {DISP_MODULE_SPLIT0, "split0", 0, NULL},
        {DISP_MODULE_UNKNOWN, "unknown", 0, NULL},
};


struct DDP_MODULE_DRIVER  *ddp_modules_driver[DISP_MODULE_NUM] = {0};
/* PMMSYS_CONFIG_REGS g_MMSYS_CONFIG_BASE = (PMMSYS_CONFIG_REGS)MMSYS_CONFIG_BASE; */
//PDISP_SPLIT_REGS   g_DISP_SPLIT_BASE   = (PDISP_SPLIT_REGS)DISP_SPLIT0_BASE;
/* PDISP_DSI0_REGS    g_DISP_DSI0_BASE    = (PDISP_DSI0_REGS)DSI0_BASE; */
struct DISP_MUTEX_REGS *g_DISP_MUTEX_BASE = (struct DISP_MUTEX_REGS *)MM_MUTEX_BASE;
struct DISP_OVL0_REGS *g_DISP_OVL0_BASE = (struct DISP_OVL0_REGS *)OVL0_BASE;
/* Pmipi_tx_config_REGS g_mipi_tx_config0_BASE = (Pmipi_tx_config_REGS)MIPI_TX0_BASE; */

int ddp_enable_module_clock(enum DISP_MODULE_ENUM module)
{
#ifdef DISP_HELPER_STAGE_NORMAL_LK
    switch (module) {
    /* MMSYS_CG_CON0 */
    case DISP_MODULE_SMI_COMMON:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_DISP_MUTEX0, DISP_REG_CONFIG_MMSYS_CG_CLR0, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG0_DISPSYS_CONFIG, DISP_REG_CONFIG_MMSYS_CG_CLR0, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG0_SMI_COMMON, DISP_REG_CONFIG_MMSYS_CG_CLR0, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG0_SMI_GALS, DISP_REG_CONFIG_MMSYS_CG_CLR0, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG0_SMI_INFRA, DISP_REG_CONFIG_MMSYS_CG_CLR0, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG0_SMI_IOMMU, DISP_REG_CONFIG_MMSYS_CG_CLR1, 1);
        break;
    case DISP_MODULE_SMI_LARB0:
        //DISP_REG_SET_FIELD(NULL, FLD_CG0_SMI_LARB0,
        //DISP_REG_CONFIG_MMSYS_CG_CLR0, 1);
        break;
    case DISP_MODULE_SMI_LARB1:
         //DISP_REG_SET_FIELD(NULL,MMSYS_CG_FLD_CG0_SMI_LARB1,
         //DISP_REG_CONFIG_MMSYS_CG_CLR0, 1);
        break;
    case DISP_MODULE_OVL0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_OVL0, DISP_REG_CONFIG_MMSYS_CG_CLR0, 1);
        break;
    case DISP_MODULE_OVL0_2L:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_OVL0_2L, DISP_REG_CONFIG_MMSYS_CG_CLR0, 1);
        break;
    case DISP_MODULE_RDMA0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_DISP_RDMA0, DISP_REG_CONFIG_MMSYS_CG_CLR0, 1);
        break;
    case DISP_MODULE_WDMA0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_DISP_WDMA0, DISP_REG_CONFIG_MMSYS_CG_CLR0, 1);
        break;
    case DISP_MODULE_COLOR0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_COLOR0, DISP_REG_CONFIG_MMSYS_CG_CLR0, 1);
        break;
    case DISP_MODULE_CCORR0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_CCORR0, DISP_REG_CONFIG_MMSYS_CG_CLR0, 1);
        break;
    case DISP_MODULE_AAL0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_DISP_AAL0, DISP_REG_CONFIG_MMSYS_CG_CLR0, 1);
        break;
    case DISP_MODULE_GAMMA0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_DISP_GAMMA0, DISP_REG_CONFIG_MMSYS_CG_CLR0, 1);
        break;
    case DISP_MODULE_POSTMASK0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_POSTMASK0, DISP_REG_CONFIG_MMSYS_CG_CLR0, 1);
        break;
    case DISP_MODULE_DITHER0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_DITHER0, DISP_REG_CONFIG_MMSYS_CG_CLR0, 1);
        break;
    case DISP_MODULE_DSI0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_DSI0, DISP_REG_CONFIG_MMSYS_CG_CLR0, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG2_DSI0, DISP_REG_CONFIG_MMSYS_CG_CLR2, 1);
        //DISP_REG_SET(NULL, DISP_REG_CONFIG_MMSYS_CG_CLR0, 0xffffffff);
        //DISP_REG_SET(NULL, DISP_REG_CONFIG_MMSYS_CG_CLR1, 0xffffffff);
        //DISP_REG_SET(NULL, DISP_REG_CONFIG_MMSYS_CG_CLR2, 0xffffffff);
        break;
    case DISP_MODULE_RSZ0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_DISP_RSZ0, DISP_REG_CONFIG_MMSYS_CG_CLR0, 1);
        break;
    default:
        DDPERR("enable module clock unknown module %d\n", module);
    }
#endif
    DDPMSG("enable %s clk, CG0:0x%x, CG1:0x%x, CG2:0x%x\n",
           ddp_get_module_name(module),
           DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON0),
           DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON1),
           DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON2));
    return 0;
}

int ddp_disable_module_clock(enum DISP_MODULE_ENUM module)
{
#ifdef DISP_HELPER_STAGE_NORMAL_LK
    switch (module) {
    /* MMSYS_CG_CON0 */
    case DISP_MODULE_SMI_COMMON:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_DISP_MUTEX0, DISP_REG_CONFIG_MMSYS_CG_SET0, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG0_DISPSYS_CONFIG, DISP_REG_CONFIG_MMSYS_CG_SET0, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG0_SMI_IOMMU, DISP_REG_CONFIG_MMSYS_CG_SET1, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG0_SMI_INFRA, DISP_REG_CONFIG_MMSYS_CG_SET0, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG0_SMI_GALS, DISP_REG_CONFIG_MMSYS_CG_SET0, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG0_SMI_COMMON, DISP_REG_CONFIG_MMSYS_CG_SET0, 1);
        break;
    case DISP_MODULE_SMI_LARB0:
        //DISP_REG_SET_FIELD(NULL, FLD_CG0_SMI_LARB0,
        //DISP_REG_CONFIG_MMSYS_CG_SET0, 1);
        break;
    case DISP_MODULE_SMI_LARB1:
        //DISP_REG_SET_FIELD(NULL,MMSYS_CG_FLD_CG0_SMI_LARB1,
        //DISP_REG_CONFIG_MMSYS_CG_SET0, 1);
        break;
    case DISP_MODULE_OVL0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_OVL0, DISP_REG_CONFIG_MMSYS_CG_SET0, 1);
        break;
    case DISP_MODULE_OVL0_2L:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_OVL0_2L, DISP_REG_CONFIG_MMSYS_CG_SET0, 1);
        break;
    case DISP_MODULE_RDMA0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_DISP_RDMA0, DISP_REG_CONFIG_MMSYS_CG_SET0, 1);
        break;
    case DISP_MODULE_WDMA0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_DISP_WDMA0, DISP_REG_CONFIG_MMSYS_CG_SET0, 1);
        break;
    case DISP_MODULE_COLOR0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_COLOR0, DISP_REG_CONFIG_MMSYS_CG_SET0, 1);
        break;
    case DISP_MODULE_CCORR0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_CCORR0, DISP_REG_CONFIG_MMSYS_CG_SET0, 1);
        break;
    case DISP_MODULE_AAL0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_DISP_AAL0, DISP_REG_CONFIG_MMSYS_CG_SET0, 1);
        break;
    case DISP_MODULE_GAMMA0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_DISP_GAMMA0, DISP_REG_CONFIG_MMSYS_CG_SET0, 1);
        break;
    case DISP_MODULE_POSTMASK0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_POSTMASK0, DISP_REG_CONFIG_MMSYS_CG_SET0, 1);
        break;
    case DISP_MODULE_DITHER0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_DITHER0, DISP_REG_CONFIG_MMSYS_CG_SET0, 1);
        break;
    case DISP_MODULE_DSI0:
        DISP_REG_SET_FIELD(NULL, FLD_CG2_DSI0, DISP_REG_CONFIG_MMSYS_CG_SET2, 1);
        DISP_REG_SET_FIELD(NULL, FLD_CG0_DSI0, DISP_REG_CONFIG_MMSYS_CG_SET0, 1);
        break;
    case DISP_MODULE_RSZ0:
        DISP_REG_SET_FIELD(NULL, FLD_CG0_DISP_RSZ0, DISP_REG_CONFIG_MMSYS_CG_SET0, 1);
        break;
    default:
        DDPERR("disable module clock unknown module %d\n", module);
    }
#endif
    DDPMSG("disable %s clk, CG0:0x%x, CG1:0x%x\n, CG2:0x%x",
           ddp_get_module_name(module),
           DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON0),
           DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON1),
           DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON2));
    return 0;
}


unsigned int is_ddp_module(enum DISP_MODULE_ENUM module)
{
    if (module >= 0 && module < DISP_MODULE_NUM)
        return 1;
    else
        return 0;
}

char *ddp_get_module_name(enum DISP_MODULE_ENUM module)
{
    if (is_ddp_module(module))
        return ddp_modules[module].module_name;

    DDPMSG("invalid module id=%d", module);
    return "unknown";
}


void ddp_init_modules_driver(void)
{
    unsigned int i = 0;

    for (i = 0; i < DISP_MODULE_NUM; i++)
        ddp_modules_driver[i] = ddp_modules[i].module_driver;
}

