/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#define LOG_TAG "CM"
#include "ddp_info.h"
#include "ddp_log.h"
#include "ddp_reg.h"
#include "ddp_reg_cm.h"
#include "disp_drv_platform.h"
#include "primary_display.h"

#define CM_REG(reg_base, index) ((reg_base) + 0x100 + (index) * 4)

static int disp_cm_reg_offset(enum DISP_MODULE_ENUM module)
{
    return 0;
}

static int disp_cm_clock_on(enum DISP_MODULE_ENUM module, void *handle)
{
    DDPDBG("%s clock_on\n", ddp_get_module_name(module));

    ddp_enable_module_clock(module);

    return 0;
}

static int disp_cm_clock_off(enum DISP_MODULE_ENUM module, void *handle)
{
    DDPDBG("%s clock_off\n", ddp_get_module_name(module));

    ddp_disable_module_clock(module);

    return 0;
}

static int disp_cm_init(enum DISP_MODULE_ENUM module, void *handle)
{
    return disp_cm_clock_on(module, handle);
}

static int disp_cm_deinit(enum DISP_MODULE_ENUM module, void *handle)
{
    return disp_cm_clock_off(module, handle);
}

static int disp_cm_config(enum DISP_MODULE_ENUM module, struct disp_ddp_path_config *pConfig,
    void *handle)
{
    int offset = disp_cm_reg_offset(module);
    unsigned long base = DISPSYS_CM0_BASE + offset;
    unsigned int height = pConfig->dst_h, width = pConfig->dst_w;

    LCM_DSI_PARAMS *dsi = &pConfig->dsi_config;
    struct lcm_cm_config_params *params = &dsi->cm_params;

    DISP_REG_SET(handle, base + DISP_REG_CM_SHADOW, 0x00000006);
    DISP_REG_SET(handle, base + DISP_REG_CM_SIZE, width << 16 | height);
    DISP_REG_SET_FIELD(handle, CON_FLD_CM_EN,
            base + DISP_REG_CM_EN, 0x1);
    DISP_REG_SET_FIELD(handle, CM_ENGINE_EN,
            base + DISP_REG_CM_CFG, 0x1);
    if (dsi->cm_enable  == 0 || dsi->cm_params.relay == 1) {
        DISP_REG_SET_FIELD(handle, CON_FLD_RELAY_MODE,
            base + DISP_REG_CM_CFG, 0x1);
        return 0;
    }

    DISP_REG_SET_FIELD(handle, CON_FLD_CM_GAMMA_ROUND_EN,
            base + DISP_REG_CM_CFG, 0x1);
    DISP_REG_SET_FIELD(handle, CON_FLD_RELAY_MODE,
            base + DISP_REG_CM_CFG, 0x0);
    if (dsi->cm_params.bits_switch == 1)
        DISP_REG_SET_FIELD(handle, CM_8B_SWITCH,
            base + DISP_REG_CM_CFG, 0x1);

    DISP_REG_SET(handle, base + DISP_REG_CM_COEF_0, params->cm_c00 << 16 | params->cm_c01);
    DISP_REG_SET(handle, base + DISP_REG_CM_COEF_1, params->cm_c02 << 16 | params->cm_c10);
    DISP_REG_SET(handle, base + DISP_REG_CM_COEF_2, params->cm_c11 << 16 | params->cm_c12);
    DISP_REG_SET(handle, base + DISP_REG_CM_COEF_3, params->cm_c20 << 16 | params->cm_c21);
    DISP_REG_SET(handle, base + DISP_REG_CM_COEF_4, params->cm_c22 << 16 |
                    params->cm_coeff_round_en << 1 | params->cm_gray_en);
    DISP_REG_SET(handle, base + DISP_REG_CM_PRECISION, params->cm_precision_mask);

    return 0;
}

static int disp_cm_dump(enum DISP_MODULE_ENUM module, int level)
{
    int offset = disp_cm_reg_offset(module);
    unsigned long base = DISPSYS_CM0_BASE + offset;
    u32 k = 0;

    DDPDUMP("== START: DISP CM0 REGS ==\n");
    for (k = 0; k < 0x110; k += 16) {
        DDPDUMP("0x%04x: 0x%08x 0x%08x 0x%08x 0x%08x\n", k,
                readl(base + k),
                readl(base + k + 0x4),
                readl(base + k + 0x8),
                readl(base + k + 0xc));
    }
    return 0;
}

struct DDP_MODULE_DRIVER ddp_driver_cm = {
    .init = disp_cm_init,
    .deinit = disp_cm_deinit,
    .config = disp_cm_config,
    .dump_info = disp_cm_dump,
};

