/*
 * Copyright (c) 2020 MediaTek Inc.
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


void ddp_bypass_color(enum DISP_MODULE_ENUM module,
    unsigned int width, unsigned int height, void *handle)
{
    int offset = 0;

    if (module == DISP_MODULE_COLOR1)
        offset = DISP_COLOR_INDEX_OFFSET;

    DDPMSG("bypass color\n");
    DISP_REG_SET(NULL, DISP_COLOR_CFG_MAIN + offset, 1<<7); //bypass color engine
    DISP_REG_SET(NULL, DISP_COLOR_INTERNAL_IP_WIDTH + offset, width); //bypass color engine
    DISP_REG_SET(NULL, DISP_COLOR_INTERNAL_IP_HEIGHT + offset, height); //bypass color engine
    DISP_REG_SET(NULL, DISP_COLOR_START + offset, 0x1); // start
}

void ddp_bypass_ccorr(enum DISP_MODULE_ENUM module,
    unsigned int width, unsigned int height, void *handle)
{
    int offset = 0;

    switch (module) {
    case DISP_MODULE_CCORR1:
        offset = DISP_CCORR1_INDEX_OFFSET;
        break;
    case DISP_MODULE_CCORR0_1:
        offset = DISP_CCORR0_1_INDEX_OFFSET;
        break;
    case DISP_MODULE_CCORR1_1:
        offset = DISP_CCORR1_1_INDEX_OFFSET;
        break;
    default:
        break;
    }

    DDPMSG("bypass ccorr\n");
    DISP_REG_MASK(NULL, DISP_REG_CCORR_CFG + offset, 1, 1);
    DISP_REG_SET(NULL, DISP_REG_CCORR_SIZE + offset, (width<<16)|height); //bypass color engine
    DISP_REG_SET(NULL, DISP_REG_CCORR_EN + offset, 1);
}

void ddp_bypass_c3d(enum DISP_MODULE_ENUM module,
    unsigned int width, unsigned int height, void *handle)
{
    int offset = 0;

    if (module == DISP_MODULE_C3D1)
        offset = DISP_C3D_INDEX_OFFSET;

    DDPMSG("bypass C3D\n");
    DISP_REG_SET(NULL, DISP_REG_C3D0_C3D_SIZE + offset, width << 16 | height); //bypass c3d engine
    DISP_REG_SET(NULL, DISP_REG_C3D0_C3D_CFG + offset, 0X11); //bypass c3d engine
    DISP_REG_SET(NULL, DISP_REG_C3D0_C3D_EN + offset, 0x1); // start
}

void ddp_bypass_tdshp(enum DISP_MODULE_ENUM module,
    unsigned int width, unsigned int height, void *handle)
{
    int offset = 0;

    if (module == DISP_MODULE_TDSHP1)
        offset = DISP_TDSHP_INDEX_OFFSET;

    DDPMSG("bypass color\n");
    DISP_REG_SET(NULL, DISP_TDSHP0_DISP_TDSHP_INPUT_SIZE + offset, width << 16 | height);
    DISP_REG_SET(NULL, DISP_TDSHP0_DISP_TDSHP_OUTPUT_SIZE + offset, width << 16 | height);
    DISP_REG_SET(NULL, DISP_TDSHP0_DISP_TDSHP_OUTPUT_OFFSET + offset, 0x0); //bypass color engine
    DISP_REG_SET(NULL, DISP_TDSHP0_DISP_TDSHP_00 + offset, 0x0); // start
    DISP_REG_SET(NULL, DISP_TDSHP0_DISP_TDSHP_CFG + offset, 0x1); // start
    DISP_REG_SET(NULL, DISP_TDSHP0_DISP_TDSHP_CTRL + offset, 0xfffffffd); // start
}

static int ddp_ccorr_start(enum DISP_MODULE_ENUM module, void *handle)
{
    int offset = 0;

    switch (module) {
    case DISP_MODULE_CCORR1:
        offset = DISP_CCORR1_INDEX_OFFSET;
        break;
    case DISP_MODULE_CCORR0_1:
        offset = DISP_CCORR0_1_INDEX_OFFSET;
        break;
    case DISP_MODULE_CCORR1_1:
        offset = DISP_CCORR1_1_INDEX_OFFSET;
        break;
    default:
        break;
    }

#ifdef LK_BYPASS_SHADOW_REG
    DISP_REG_SET_FIELD(handle, FLD_CCORR_BYPASS_SHADOW,
              DISP_REG_CCORR_SHADOW_CTRL + offset, 0x1);
    DISP_REG_SET_FIELD(handle, FLD_CCORR_READ_WRK_REG,
              DISP_REG_CCORR_SHADOW_CTRL + offset, 0x1);

#endif

    return 0;
}
static int ddp_aal_start(enum DISP_MODULE_ENUM module, void *handle)
{
    int offset = 0;

    if (module == DISP_MODULE_AAL1)
        offset = DISP_AAL_INDEX_OFFSET;

#ifdef LK_BYPASS_SHADOW_REG
    DISP_REG_SET_FIELD(handle, FLD_AAL_BYPASS_SHADOW,
              DISP_REG_AAL_SHADOW_CTRL + offset, 0x1);
    DISP_REG_SET_FIELD(handle, FLD_AAL_READ_WRK_REG,
              DISP_REG_AAL_SHADOW_CTRL + offset, 0x1);

#endif

    return 0;
}

void ddp_bypass_mdp_aal(enum DISP_MODULE_ENUM module,
    unsigned int width, unsigned int height, void *handle)
{
    int offset = 0;

    if (module == DISP_MODULE_MDP_AAL1)
        offset = DISP_MDP_AAL_INDEX_OFFSET;
    DDPMSG("bypass mdp_aal\n");

    DISP_REG_SET(handle, DISP_MDP_AAL0_MDP_AAL_SIZE + offset, width << 16 | height);
    DISP_REG_SET(handle, DISP_MDP_AAL0_MDP_AAL_OUTPUT_SIZE + offset, width << 16 | height);
    DISP_REG_SET(handle, DISP_MDP_AAL0_MDP_AAL_EN + offset, 0x00000001);
    DISP_REG_SET(handle, DISP_MDP_AAL0_MDP_AAL_CFG + offset, 0x00400003);
    DISP_REG_SET(handle, DISP_MDP_AAL0_MDP_AAL_CFG_MAIN + offset, 0x00000000);
    DISP_REG_SET(handle, DISP_MDP_AAL0_MDP_AAL_DRE_BILATERAL + offset, 0x00000000);
}

void ddp_bypass_aal(enum DISP_MODULE_ENUM module,
    unsigned int width, unsigned int height, void *handle)
{
    int offset = 0;

    if (module == DISP_MODULE_AAL1)
        offset = DISP_AAL_INDEX_OFFSET;

    DDPMSG("bypass aal\n");
    DISP_REG_SET_FIELD(handle, CFG_FLD_RELAY_MODE,   DISP_AAL_CFG + offset, 0x1); //relay_mode
    DISP_REG_SET(handle, DISP_AAL_SIZE + offset, (width<<16)|height); //bypass color engine
    DISP_REG_SET(handle, DISP_AAL_OUTPUT_SIZE + offset, (width << 16) | height);
    DISP_REG_SET(handle, DISP_AAL_EN + offset, 0x1); //bypass mode
}

void ddp_bypass_gamma(enum DISP_MODULE_ENUM module,
    unsigned int width, unsigned int height, void *handle)
{
    int offset = 0;

    if (module == DISP_MODULE_GAMMA1)
        offset = DISP_GAMMA_INDEX_OFFSET;

    DDPMSG("bypass gamma\n");
    DISP_REG_MASK(NULL, DISP_REG_GAMMA_CFG + offset, 1, 1);
    DISP_REG_SET(NULL, DISP_REG_GAMMA_SIZE + offset, (width<<16)|height); //bypass color engine
    DISP_REG_SET(handle, DISP_REG_GAMMA_EN + offset, 0x1); //bypass mode
}
static int ddp_gamma_start(enum DISP_MODULE_ENUM module, void *handle)
{
    int offset = 0;

    if (module == DISP_MODULE_GAMMA1)
        offset = DISP_GAMMA_INDEX_OFFSET;

#ifdef LK_BYPASS_SHADOW_REG
    DISP_REG_SET_FIELD(handle, FLD_GAMMA_BYPASS_SHADOW,
              DISP_REG_GAMMA_SHADOW_CTRL + offset, 0x1);
    DISP_REG_SET_FIELD(handle, FLD_GAMMA_READ_WRK_REG,
              DISP_REG_GAMMA_SHADOW_CTRL + offset, 0x1);

#endif

    return 0;
}

static int ddp_color_start(enum DISP_MODULE_ENUM module, void *handle)
{
    int offset = 0;

    if (module == DISP_MODULE_COLOR1)
        offset = DISP_COLOR_INDEX_OFFSET;

#ifdef LK_BYPASS_SHADOW_REG
    DISP_REG_SET_FIELD(handle, FLD_COLOR_BYPASS_SHADOW,
              DISP_REG_COLOR_SHADOW_CTRL + offset, 0x1);
    DISP_REG_SET_FIELD(handle, FLD_COLOR_READ_WRK_REG,
              DISP_REG_COLOR_SHADOW_CTRL + offset, 0x1);

#endif

    return 0;
}

static int config_color(enum DISP_MODULE_ENUM module, struct disp_ddp_path_config *pConfig,
                            void *cmdq)
{
    int color_idx;

    if (module == DISP_MODULE_COLOR0)
        color_idx = 0;
    else
        color_idx = 1;

    DDPMSG("config color dirty = %d\n", pConfig->dst_dirty);
    if (!pConfig->dst_dirty)
        return 0;

    ddp_bypass_color(module, pConfig->dst_w, pConfig->dst_h, cmdq);
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

static int config_mdp_aal(enum DISP_MODULE_ENUM module, struct disp_ddp_path_config *pConfig,
                            void *cmdq)
{
    DDPMSG("config mdp_aal dirty = %d\n", pConfig->dst_dirty);
    if (!pConfig->dst_dirty)
        return 0;

    ddp_bypass_mdp_aal(module, pConfig->dst_w, pConfig->dst_h, cmdq);
    return 0;
}

static int config_aal(enum DISP_MODULE_ENUM module, struct disp_ddp_path_config *pConfig,
                        void *cmdq)
{
    if (module == DISP_MODULE_MDP_AAL0 || module == DISP_MODULE_MDP_AAL1) {
        config_mdp_aal(module, pConfig, cmdq);
        return 0;
    }

    DDPMSG("config aal dirty = %d\n", pConfig->dst_dirty);
    if (!pConfig->dst_dirty)
        return 0;

    ddp_bypass_aal(module, pConfig->dst_w, pConfig->dst_h, cmdq);
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

static int ddp_c3d_start(enum DISP_MODULE_ENUM module, void *handle)
{
    int offset = 0;

    if (module == DISP_MODULE_C3D1)
        offset = DISP_C3D_INDEX_OFFSET;

#ifdef LK_BYPASS_SHADOW_REG
    DISP_REG_SET_FIELD(handle, FLD_C3D_BYPASS_SHADOW,
              DISP_REG_C3D_SHADOW_CTRL + offset, 0x1);
    DISP_REG_SET_FIELD(handle, FLD_C3D_READ_WRK_REG,
              DISP_REG_C3D_SHADOW_CTRL + offset, 0x1);

#endif

    return 0;
}

static int config_c3d(enum DISP_MODULE_ENUM module, struct disp_ddp_path_config *pConfig,
                            void *cmdq)
{
    int c3d_idx;

    if (module == DISP_MODULE_C3D0)
        c3d_idx = 0;
    else
        c3d_idx = 1;

    DDPMSG("config c3d dirty = %d\n", pConfig->dst_dirty);
    if (!pConfig->dst_dirty)
        return 0;

    ddp_bypass_c3d(module, pConfig->dst_w, pConfig->dst_h, cmdq);
    return 0;
}

static int ddp_tdshp_start(enum DISP_MODULE_ENUM module, void *handle)
{
    int offset = 0;

    if (module == DISP_MODULE_TDSHP1)
        offset = DISP_TDSHP_INDEX_OFFSET;

#ifdef LK_BYPASS_SHADOW_REG
    DISP_REG_SET_FIELD(handle, FLD_TDSHP_BYPASS_SHADOW,
              DISP_REG_TDSHP_SHADOW_CTRL + offset, 0x1);
    DISP_REG_SET_FIELD(handle, FLD_TDSHP_READ_WRK_REG,
              DISP_REG_TDSHP_SHADOW_CTRL + offset, 0x1);

#endif

    return 0;
}

static int config_tdshp(enum DISP_MODULE_ENUM module, struct disp_ddp_path_config *pConfig,
                            void *cmdq)
{
    int tdshp_idx;

    if (module == DISP_MODULE_TDSHP0)
        tdshp_idx = 0;
    else
        tdshp_idx = 1;

    DDPMSG("config tdshp dirty = %d\n", pConfig->dst_dirty);
    if (!pConfig->dst_dirty)
        return 0;

    ddp_bypass_tdshp(module, pConfig->dst_w, pConfig->dst_h, cmdq);
    return 0;
}

void ddp_bypass_chist(enum DISP_MODULE_ENUM module,
    unsigned int width, unsigned int height, void *handle)
{
    int offset = 0;

    switch (module) {
    case DISP_MODULE_CHIST1:
        offset = DISP_CHIST1_INDEX_OFFSET;
        break;
    case DISP1_MODULE_CHIST0:
        offset = DISP1_CHIST0_INDEX_OFFSET;
        break;
    case DISP1_MODULE_CHIST1:
        offset = DISP1_CHIST1_INDEX_OFFSET;
        break;
    default:
        break;
    }

    DDPMSG("bypass chist moudle:%d\n", module);
    DISP_REG_SET(NULL, DISP_CHIST_EN + offset, 0x1);
    DISP_REG_SET(NULL, DISP_CHIST_SIZE + offset, width << 16 | height);
    DISP_REG_SET(NULL, DISP_CHIST_CFG + offset, 0x1); // bypass chist engine

}

static int ddp_chist_start(enum DISP_MODULE_ENUM module, void *handle)
{
    int offset = 0;

    switch (module) {
    case DISP_MODULE_CHIST1:
        offset = DISP_CHIST1_INDEX_OFFSET;
        break;
    case DISP1_MODULE_CHIST0:
        offset = DISP1_CHIST0_INDEX_OFFSET;
        break;
    case DISP1_MODULE_CHIST1:
        offset = DISP1_CHIST1_INDEX_OFFSET;
        break;
    default:
        break;
    }

#ifdef LK_BYPASS_SHADOW_REG
    DISP_REG_SET(NULL, DISP_CHIST_SHADOW_CTRL + offset, 0x1);
#endif

    return 0;
}

static int config_chist(enum DISP_MODULE_ENUM module, struct disp_ddp_path_config *pConfig,
                            void *cmdq)
{
    ddp_bypass_chist(module, pConfig->dst_w, pConfig->dst_h, cmdq);
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

// c3d
struct DDP_MODULE_DRIVER ddp_driver_c3d = {
    .init            = clock_on,
    .deinit          = clock_off,
    .config          = config_c3d,
    .start           = ddp_c3d_start,
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

// chist
struct DDP_MODULE_DRIVER ddp_driver_chist = {
    .init            = clock_on,
    .deinit          = clock_off,
    .config          = config_chist,
    .start           = ddp_chist_start,
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

