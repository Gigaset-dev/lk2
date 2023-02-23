/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include "ddp_dither.h"
#include "ddp_log.h"
#include "ddp_path.h"
#include "ddp_reg.h"
#include "disp_drv_platform.h"


#define DITHER_REG(reg_base, index) ((reg_base) + 0x100 + (index) * 4)

static int disp_dither_reg_offset(enum DISP_MODULE_ENUM module)
{
    if (module == DISP_MODULE_DITHER1)
        return DISP_DITHER_INDEX_OFFSET;

    return 0;
}

static void disp_dither_init(enum DISP_MODULE_ENUM module, int width, int height,
                             unsigned int dither_bpp, void *cmdq)
{
    int offset = disp_dither_reg_offset(module);
    unsigned long reg_base = DISPSYS_DITHER0_BASE + offset;
    unsigned int enable;

    DISP_REG_MASK(cmdq, DITHER_REG(reg_base, 5), 0x00000000, ~0);
    DISP_REG_MASK(cmdq, DITHER_REG(reg_base, 6), 0x00003004, ~0);
    DISP_REG_MASK(cmdq, DITHER_REG(reg_base, 7), 0x00000000, ~0);
    DISP_REG_MASK(cmdq, DITHER_REG(reg_base, 8), 0x00000000, ~0);
    DISP_REG_MASK(cmdq, DITHER_REG(reg_base, 9), 0x00000000, ~0);
    DISP_REG_MASK(cmdq, DITHER_REG(reg_base, 10), 0x00000000, ~0);
    DISP_REG_MASK(cmdq, DITHER_REG(reg_base, 11), 0x00000000, ~0);
    DISP_REG_MASK(cmdq, DITHER_REG(reg_base, 12), 0x00000011, ~0);
    DISP_REG_MASK(cmdq, DITHER_REG(reg_base, 13), 0x00000000, ~0);
    DISP_REG_MASK(cmdq, DITHER_REG(reg_base, 14), 0x00000000, ~0);

    enable = 0x1;
    if (dither_bpp == 16) { /* 565 */
        DISP_REG_MASK(cmdq, DITHER_REG(reg_base, 15), 0x50500001, ~0);
        DISP_REG_MASK(cmdq, DITHER_REG(reg_base, 16), 0x50504040, ~0);
        DISP_REG_MASK(cmdq, DITHER_REG(reg_base, 0),  0x00000001, ~0);
    } else if (dither_bpp == 18) { /* 666 */
        DISP_REG_MASK(cmdq, DITHER_REG(reg_base, 15), 0x40400001, ~0);
        DISP_REG_MASK(cmdq, DITHER_REG(reg_base, 16), 0x40404040, ~0);
        DISP_REG_MASK(cmdq, DITHER_REG(reg_base, 0),  0x00000001, ~0);
    } else if (dither_bpp == 24) { /* 888 */
        DISP_REG_MASK(cmdq, DITHER_REG(reg_base, 15), 0x20200001, ~0);
        DISP_REG_MASK(cmdq, DITHER_REG(reg_base, 16), 0x20202020, ~0);
        DISP_REG_MASK(cmdq, DITHER_REG(reg_base, 0),  0x00000001, ~0);
    } else if (dither_bpp > 24) {
        DDPMSG("[DITHER] High depth LCM (bpp = %d), no dither\n", dither_bpp);
        enable = 1;
    } else {
        DDPMSG("[DITHER] invalid dither bpp = %d\n", dither_bpp);
        /* Bypass dither */
        DISP_REG_MASK(cmdq, DITHER_REG(reg_base, 0), 0x00000000, ~0);
        enable = 0;
    }

    DISP_REG_MASK(cmdq, DISP_REG_DITHER_EN + offset, enable, 0x1);
    DISP_REG_MASK(cmdq, DISP_REG_DITHER_CFG + offset, enable << 1, 1 << 1);
    DISP_REG_SET(cmdq, DISP_REG_DITHER_SIZE + offset, (width << 16) | height);
}


static int disp_dither_config(enum DISP_MODULE_ENUM module, struct disp_ddp_path_config *pConfig,
                            void *cmdq)
{
    DDPMSG("config dither dirty = %d\n", pConfig->dst_dirty);
    if (pConfig->dst_dirty) {
        disp_dither_init(module, pConfig->dst_w, pConfig->dst_h,
                         pConfig->lcm_bpp, cmdq);
    }

    return 0;
}

static int disp_dither_start(enum DISP_MODULE_ENUM module, void *handle)
{
    int offset = disp_dither_reg_offset(module);

#ifdef LK_BYPASS_SHADOW_REG
    DISP_REG_SET_FIELD(handle, FLD_DITHER_BYPASS_SHADOW,
              DISP_REG_DITHER_SHADOW + offset, 0x1);
    DISP_REG_SET_FIELD(handle, FLD_DITHER_READ_WRK_REG,
              DISP_REG_DITHER_SHADOW + offset, 0x1);

#endif

    return 0;
}

static int disp_dither_bypass(enum DISP_MODULE_ENUM module, int bypass)
{
    int offset = disp_dither_reg_offset(module);
    int relay = 0;

    if (bypass)
        relay = 1;

    DISP_REG_MASK(NULL, DISP_REG_DITHER_CFG + offset, relay, 0x1);
    DDPMSG("%s(bypass = %d)", __func__, bypass);

    return 0;
}

static int disp_dither_clock_on(enum DISP_MODULE_ENUM module, void *handle)
{
    ddp_enable_module_clock(module);
    return 0;
}

static int disp_dither_clock_off(enum DISP_MODULE_ENUM module, void *handle)
{
    ddp_disable_module_clock(module);
    return 0;
}


struct DDP_MODULE_DRIVER ddp_driver_dither = {
    .start          = disp_dither_start,
    .config         = disp_dither_config,
    .bypass         = disp_dither_bypass,
    .init           = disp_dither_clock_on,
    .deinit         = disp_dither_clock_off,
    .power_on       = disp_dither_clock_on,
    .power_off      = disp_dither_clock_off,
};

