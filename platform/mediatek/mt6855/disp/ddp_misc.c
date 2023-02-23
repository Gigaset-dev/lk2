/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#define LOG_TAG "MISC"

#include "ddp_reg.h"
#include "ddp_misc.h"
#include "disp_drv_platform.h"
#include "ddp_log.h"

#include <debug.h>

void ddp_bypass_tdshp(enum DISP_MODULE_ENUM module,
    unsigned int width, unsigned int height, void *handle)
{
    DDPMSG("bypass tdshp\n");
    DISP_REG_SET(NULL, DISP_TDSHP0_DISP_TDSHP_INPUT_SIZE, width << 16 | height);
    DISP_REG_SET(NULL, DISP_TDSHP0_DISP_TDSHP_OUTPUT_SIZE, width << 16 | height);
    DISP_REG_SET(NULL, DISP_TDSHP0_DISP_TDSHP_OUTPUT_OFFSET, 0x0);
    DISP_REG_SET(NULL, DISP_TDSHP0_DISP_TDSHP_00, 0x0);
    DISP_REG_SET(NULL, DISP_TDSHP0_DISP_TDSHP_CFG, 0x1);
    DISP_REG_SET(NULL, DISP_TDSHP0_DISP_TDSHP_CTRL, 0xfffffffd);
}

static int ddp_tdshp_start(enum DISP_MODULE_ENUM module, void *handle)
{

#ifdef LK_BYPASS_SHADOW_REG
    DISP_REG_SET_FIELD(handle, FLD_TDSHP_BYPASS_SHADOW,
              DISP_REG_TDSHP_SHADOW_CTRL, 0x1);
    DISP_REG_SET_FIELD(handle, FLD_TDSHP_READ_WRK_REG,
              DISP_REG_TDSHP_SHADOW_CTRL, 0x1);
#endif

    return 0;
}

static int config_tdshp(enum DISP_MODULE_ENUM module, struct disp_ddp_path_config *pConfig,
                            void *cmdq)
{
    DDPMSG("config tdshp dirty = %d\n", pConfig->dst_dirty);
    if (!pConfig->dst_dirty)
        return 0;

    ddp_bypass_tdshp(module, pConfig->dst_w, pConfig->dst_h, cmdq);
    return 0;
}

void ddp_bypass_color(enum DISP_MODULE_ENUM module,
   unsigned int width, unsigned int height, void *handle)
{
    DDPMSG("bypass color\n");
    DISP_REG_SET(NULL, DISP_COLOR_CFG_MAIN, 1<<7);
    DISP_REG_SET(NULL, DISP_COLOR_INTERNAL_IP_WIDTH, width);
    DISP_REG_SET(NULL, DISP_COLOR_INTERNAL_IP_HEIGHT, height);
    DISP_REG_SET(NULL, DISP_COLOR_START, 0x1);
}

static int ddp_color_start(enum DISP_MODULE_ENUM module, void *handle)
{

#ifdef LK_BYPASS_SHADOW_REG
    DISP_REG_SET_FIELD(handle, FLD_COLOR_BYPASS_SHADOW,
              DISP_REG_COLOR_SHADOW_CTRL, 0x1);
    DISP_REG_SET_FIELD(handle, FLD_COLOR_READ_WRK_REG,
              DISP_REG_COLOR_SHADOW_CTRL, 0x1);
#endif

    return 0;
}

static int config_color(enum DISP_MODULE_ENUM module, struct disp_ddp_path_config *pConfig,
                            void *cmdq)
{
    DDPMSG("config color dirty = %d\n", pConfig->dst_dirty);
    if (!pConfig->dst_dirty)
        return 0;

    ddp_bypass_color(module, pConfig->dst_w, pConfig->dst_h, cmdq);
    return 0;
}

void ddp_bypass_ccorr(enum DISP_MODULE_ENUM module,
    unsigned int width, unsigned int height, void *handle)
{
    DDPMSG("bypass ccorr\n");
    DISP_REG_MASK(NULL, DISP_REG_CCORR_CFG, 1, 1);
    DISP_REG_SET(NULL, DISP_REG_CCORR_SIZE, (width<<16)|height);
    DISP_REG_SET(NULL, DISP_REG_CCORR_EN, 1);
}
static int ddp_ccorr_start(enum DISP_MODULE_ENUM module, void *handle)
{

#ifdef LK_BYPASS_SHADOW_REG
    DISP_REG_SET_FIELD(handle, FLD_CCORR_BYPASS_SHADOW,
              DISP_REG_CCORR_SHADOW_CTRL, 0x1);
    DISP_REG_SET_FIELD(handle, FLD_CCORR_READ_WRK_REG,
              DISP_REG_CCORR_SHADOW_CTRL, 0x1);

#endif

    return 0;
}

static int config_ccorr(enum DISP_MODULE_ENUM module, struct disp_ddp_path_config *pConfig,
                            void *cmdq)
{
    DDPMSG("config ccorr dirty = %d\n", pConfig->dst_dirty);
    if (!pConfig->dst_dirty)
        return 0;

    ddp_bypass_ccorr(module, pConfig->dst_w, pConfig->dst_h, cmdq);
    return 0;
}

void ddp_bypass_aal(enum DISP_MODULE_ENUM module,
    unsigned int width, unsigned int height, void *handle)
{
    DDPMSG("bypass aal\n");
    DISP_REG_SET_FIELD(handle, CFG_FLD_RELAY_MODE,   DISP_AAL_CFG, 0x1);
    DISP_REG_SET(handle, DISP_AAL_SIZE, (width<<16)|height);
    DISP_REG_SET(handle, DISP_AAL_OUTPUT_SIZE, (width << 16) | height);
    DISP_REG_SET(handle, DISP_AAL_EN, 0x1);
}

static int ddp_aal_start(enum DISP_MODULE_ENUM module, void *handle)
{
#ifdef LK_BYPASS_SHADOW_REG
    DISP_REG_SET_FIELD(handle, FLD_AAL_BYPASS_SHADOW,
              DISP_AAL_SHADOW_CTL, 0x1);
    DISP_REG_SET_FIELD(handle, FLD_AAL_READ_WRK_REG,
              DISP_AAL_SHADOW_CTL, 0x1);
#endif

    return 0;
}

static int config_aal(enum DISP_MODULE_ENUM module, struct disp_ddp_path_config *pConfig,
                        void *cmdq)
{
    DDPMSG("config aal dirty = %d\n", pConfig->dst_dirty);

    if (!pConfig->dst_dirty)
        return 0;

    ddp_bypass_aal(module, pConfig->dst_w, pConfig->dst_h, cmdq);
    return 0;
}

void ddp_bypass_gamma(enum DISP_MODULE_ENUM module,
    unsigned int width, unsigned int height, void *handle)
{
    DDPMSG("bypass gamma\n");
    DISP_REG_MASK(NULL, DISP_REG_GAMMA_CFG, 1, 1);
    DISP_REG_SET(NULL, DISP_REG_GAMMA_SIZE, (width<<16)|height);
    DISP_REG_SET(handle, DISP_REG_GAMMA_EN, 0x1);
}

static int ddp_gamma_start(enum DISP_MODULE_ENUM module, void *handle)
{
#ifdef LK_BYPASS_SHADOW_REG
    DISP_REG_SET_FIELD(handle, FLD_GAMMA_BYPASS_SHADOW,
              DISP_REG_GAMMA_SHADOW_CTRL, 0x1);
    DISP_REG_SET_FIELD(handle, FLD_GAMMA_READ_WRK_REG,
              DISP_REG_GAMMA_SHADOW_CTRL, 0x1);
#endif

    return 0;
}

static int config_gamma(enum DISP_MODULE_ENUM module, struct disp_ddp_path_config *pConfig,
                            void *cmdq)
{
    DDPMSG("config gamma dirty = %d\n", pConfig->dst_dirty);
    if (!pConfig->dst_dirty)
        return 0;

    ddp_bypass_gamma(module, pConfig->dst_w, pConfig->dst_h, cmdq);
    return 0;
}

static int clock_on(enum DISP_MODULE_ENUM module, void *handle)
{
    ddp_enable_module_clock(module);
    return 0;
}

static int clock_off(enum DISP_MODULE_ENUM module, void *handle)
{
    ddp_disable_module_clock(module);
    return 0;
}

///////////////////////////////////////////////////////////

// tdshp
struct DDP_MODULE_DRIVER ddp_driver_tdshp = {
    .init            = clock_on,
    .deinit          = clock_off,
    .config          = config_tdshp,
    .start           = ddp_tdshp_start,
    .trigger         = NULL,
    .stop            = NULL,
    .reset           = NULL,
    .power_on        = clock_on,
    .power_off       = clock_off,
    .is_idle         = NULL,
    .is_busy         = NULL,
    .dump_info       = NULL,
    .bypass          = NULL,
    .build_cmdq      = NULL,
    .set_lcm_utils   = NULL,
};


// color
struct DDP_MODULE_DRIVER ddp_driver_color = {
    .init            = clock_on,
    .deinit          = clock_off,
    .config          = config_color,
    .start           = ddp_color_start,
    .trigger         = NULL,
    .stop            = NULL,
    .reset           = NULL,
    .power_on        = clock_on,
    .power_off       = clock_off,
    .is_idle         = NULL,
    .is_busy         = NULL,
    .dump_info       = NULL,
    .bypass          = NULL,
    .build_cmdq      = NULL,
    .set_lcm_utils   = NULL,
};

// ccorr
struct DDP_MODULE_DRIVER ddp_driver_ccorr = {
    .init            = clock_on,
    .deinit          = clock_off,
    .config          = config_ccorr,
    .start           = ddp_ccorr_start,
    .trigger         = NULL,
    .stop            = NULL,
    .reset           = NULL,
    .power_on        = clock_on,
    .power_off       = clock_off,
    .is_idle         = NULL,
    .is_busy         = NULL,
    .dump_info       = NULL,
    .bypass          = NULL,
    .build_cmdq      = NULL,
    .set_lcm_utils   = NULL,
};

// aal
struct DDP_MODULE_DRIVER ddp_driver_aal = {
    .init            = clock_on,
    .deinit          = clock_off,
    .config          = config_aal,
    .start           = ddp_aal_start,
    .trigger         = NULL,
    .stop            = NULL,
    .reset           = NULL,
    .power_on        = clock_on,
    .power_off       = clock_off,
    .is_idle         = NULL,
    .is_busy         = NULL,
    .dump_info       = NULL,
    .bypass          = NULL,
    .build_cmdq      = NULL,
    .set_lcm_utils   = NULL,
};

// gamma
struct DDP_MODULE_DRIVER ddp_driver_gamma = {
    .init            = clock_on,
    .deinit          = clock_off,
    .config          = config_gamma,
    .start           = ddp_gamma_start,
    .trigger         = NULL,
    .stop            = NULL,
    .reset           = NULL,
    .power_on        = clock_on,
    .power_off       = clock_off,
    .is_idle         = NULL,
    .is_busy         = NULL,
    .dump_info       = NULL,
    .bypass          = NULL,
    .build_cmdq      = NULL,
    .set_lcm_utils   = NULL,
};
