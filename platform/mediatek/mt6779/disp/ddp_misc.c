/*
* Copyright (c) 2019 MediaTek Inc.
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files
* (the "Software"), to deal in the Software without restriction,
* including without limitation the rights to use, copy, modify, merge,
* publish, distribute, sublicense, and/or sell copies of the Software,
* and to permit persons to whom the Software is furnished to do so,
* subject to the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#define LOG_TAG "MISC"

#include "ddp_reg.h"
#include "ddp_misc.h"
#include "ddp_log.h"

#include <debug.h>


void ddp_bypass_color(int idx, unsigned int width, unsigned int height, void *handle)
{
    DDPDBG("bypass color\n");
    DISP_REG_SET(NULL, DISP_COLOR_CFG_MAIN, 1 << 7); //bypass color engine
    DISP_REG_SET(NULL, DISP_COLOR_INTERNAL_IP_WIDTH, width); //bypass color engine
    DISP_REG_SET(NULL, DISP_COLOR_INTERNAL_IP_HEIGHT, height); //bypass color engine
    DISP_REG_SET(NULL, DISP_COLOR_START, 0x1); // start
}

void ddp_bypass_ccorr(unsigned int width, unsigned int height, void *handle)
{
    DDPDBG("bypass ccorr\n");
    DISP_REG_MASK(NULL, DISP_REG_CCORR_CFG, 1, 1);
    DISP_REG_SET(NULL, DISP_REG_CCORR_SIZE, (width << 16) | height); //bypass color engine
    DISP_REG_SET(NULL, DISP_REG_CCORR_EN, 1);
}

void ddp_bypass_aal(unsigned int width, unsigned int height, void *handle)
{
    DDPDBG("bypass aal\n");
    DISP_REG_SET_FIELD(handle, CFG_FLD_RELAY_MODE, DISP_AAL_CFG, 0x1); //relay_mode
    DISP_REG_SET(NULL, DISP_AAL_SIZE, (width << 16) | height); //bypass color engine
    DISP_REG_SET(handle, DISP_AAL_EN, 0x1); //bypass mode
    DISP_REG_SET(NULL, DISP_AAL_OUTPUT_SIZE, (width << 16) | height); //set output size
}

void ddp_bypass_gamma(unsigned int width, unsigned int height, void *handle)
{
    DDPDBG("bypass gamma\n");
    DISP_REG_MASK(NULL, DISP_REG_GAMMA_CFG, 1, 1);
    DISP_REG_SET(NULL, DISP_REG_GAMMA_SIZE, (width << 16) | height); //bypass color engine
    DISP_REG_SET(handle, DISP_REG_GAMMA_EN, 0x1); //bypass mode
}

static int config_color(DISP_MODULE_ENUM module, disp_ddp_path_config *pConfig, void *cmdq)
{
    int color_idx;
    if (module == DISP_MODULE_COLOR0)
        color_idx = 0;
    else
        color_idx = 1;

    DDPDBG("config color dirty = %d\n", pConfig->dst_dirty);
    if (!pConfig->dst_dirty) {
        return 0;
    }
    ddp_bypass_color(color_idx, pConfig->dst_w, pConfig->dst_h, cmdq);
    return 0;
}

static int config_ccorr(DISP_MODULE_ENUM module, disp_ddp_path_config *pConfig, void *cmdq)
{
    DDPDBG("config ccorr dirty = %d\n", pConfig->dst_dirty);
    if (!pConfig->dst_dirty) {
        return 0;
    }
    ddp_bypass_ccorr(pConfig->dst_w, pConfig->dst_h, cmdq);
    return 0;
}

static int config_aal(DISP_MODULE_ENUM module, disp_ddp_path_config *pConfig, void *cmdq)
{
    DDPDBG("config aal dirty = %d\n", pConfig->dst_dirty);
    if (!pConfig->dst_dirty) {
        return 0;
    }
    ddp_bypass_aal(pConfig->dst_w, pConfig->dst_h, cmdq);
    return 0;
}

static int config_gamma(DISP_MODULE_ENUM module, disp_ddp_path_config *pConfig, void *cmdq)
{
    DDPDBG("config gamma dirty = %d\n", pConfig->dst_dirty);
    if (!pConfig->dst_dirty) {
        return 0;
    }
    ddp_bypass_gamma(pConfig->dst_w, pConfig->dst_h, cmdq);
    return 0;
}

static int clock_on(DISP_MODULE_ENUM module, void *handle)
{
    ddp_enable_module_clock(module);
    return 0;
}

static int clock_off(DISP_MODULE_ENUM module, void *handle)
{
    ddp_disable_module_clock(module);
    return 0;
}

///////////////////////////////////////////////////////////

// color
DDP_MODULE_DRIVER ddp_driver_color = {
    .init            = clock_on,
    .deinit          = clock_off,
    .config          = config_color,
    .start           = NULL,
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
DDP_MODULE_DRIVER ddp_driver_ccorr = {
    .init            = clock_on,
    .deinit          = clock_off,
    .config          = config_ccorr,
    .start           = NULL,
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
DDP_MODULE_DRIVER ddp_driver_aal = {
    .init            = clock_on,
    .deinit          = clock_off,
    .config          = config_aal,
    .start           = NULL,
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
DDP_MODULE_DRIVER ddp_driver_gamma = {
    .init            = clock_on,
    .deinit          = clock_off,
    .config          = config_gamma,
    .start           = NULL,
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
