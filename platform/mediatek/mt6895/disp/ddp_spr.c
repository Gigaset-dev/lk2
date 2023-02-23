/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#define LOG_TAG "SPR"
#include "ddp_info.h"
#include "ddp_log.h"
#include "ddp_reg.h"
#include "ddp_reg_spr.h"
#include "disp_drv_platform.h"
#include "primary_display.h"

#define SPR_REG(reg_base, index) ((reg_base) + 0x100 + (index) * 4)

static int disp_spr_reg_offset(enum DISP_MODULE_ENUM module)
{
    if (module == DISP_MODULE_SPR1)
        return DISP_SPR_INDEX_OFFSET;

    return 0;
}

static int disp_spr_clock_on(enum DISP_MODULE_ENUM module, void *handle)
{
    DDPDBG("%s clock_on\n", ddp_get_module_name(module));

    ddp_enable_module_clock(module);

    return 0;
}

static int disp_spr_clock_off(enum DISP_MODULE_ENUM module, void *handle)
{
    DDPDBG("%s clock_off\n", ddp_get_module_name(module));

    ddp_disable_module_clock(module);

    return 0;
}

static int disp_spr_init(enum DISP_MODULE_ENUM module, void *handle)
{
    return disp_spr_clock_on(module, handle);
}

static int disp_spr_deinit(enum DISP_MODULE_ENUM module, void *handle)
{
    return disp_spr_clock_off(module, handle);
}

static int disp_spr_config(enum DISP_MODULE_ENUM module, struct disp_ddp_path_config *pConfig,
    void *handle)
{
    int offset = disp_spr_reg_offset(module);
    unsigned long base = DISPSYS_SPR0_BASE + offset;
    unsigned int height = pConfig->dst_h, width = pConfig->dst_w;
    unsigned int i = 0, j = 0;
    u32 reg_val;

    LCM_DSI_PARAMS *dsi = &pConfig->dsi_config;
    struct lcm_spr_config_params *params = &dsi->spr_params;
    struct lcm_spr_color_params *spr_color_params = NULL;

    if (!pConfig->dst_dirty)
        return 0;

    DISP_REG_SET_FIELD(handle, SPR_FORCE_COMMIT,
            base + DISP_REG_SPR_EN, 0x1);
    DISP_REG_SET_FIELD(handle, SPR_BYPASS_SHADOW,
            base + DISP_REG_SPR_EN, 0x1);
    DISP_REG_SET_FIELD(handle, CON_FLD_SPR_EN,
            base + DISP_REG_SPR_EN, 0x1);
    DISP_REG_SET_FIELD(handle, SPR_LUT_EN,
            base + DISP_REG_SPR_EN, 0x1);
    DISP_REG_SET_FIELD(handle, SPR_STALL_CG_ON,
            base + DISP_REG_SPR_CFG, 0x1);
    DISP_REG_SET_FIELD(handle, RG_BYPASS_DITHER,
            base + DISP_REG_SPR_CFG, 0x1);
    DISP_REG_SET(handle, base + DISP_REG_SPR_ROI_SIZE, height << 16 | width);
    DISP_REG_SET(handle, base + DISP_REG_SPR_CTRL, 0x10010000);

    DISP_REG_SET_FIELD(handle, CROP_OUT_HSIZE,
        base + DISP_REG_SPR_RDY_SEL, width);
    DISP_REG_SET_FIELD(handle, CROP_OUT_VSIZE,
        base + DISP_REG_SPR_RDY_SEL_EN, height);

    if (dsi->spr_enable  == 0 || dsi->spr_params.relay == 1) {
        DISP_REG_SET_FIELD(handle, SPR_RELAY_MODE,
            base + DISP_REG_SPR_EN, 0x1);
        return 0;
    }
    DISP_REG_SET_FIELD(handle, SPR_RELAY_MODE,
                base + DISP_REG_SPR_EN, 0x0);

    reg_val = (!!params->postalign_6type_mode_en << 31) |
            (!!params->padding_repeat_en << 28) |
            (!!params->postalign_en << (20 + params->spr_format_type)) |
            (!!params->postalign_en << 16) |
            (!!params->bypass_dither << 5) |
            (!!params->rgb_swap << 4) |
            (!!params->outdata_res_sel << 2) |
            (!!params->indata_res_sel << 1) | 1;
    DISP_REG_SET(handle,
            base + DISP_REG_SPR_CFG, reg_val);
    DISP_REG_SET_FIELD(handle, SPR_SPECIALCASEEN,
            base + DISP_REG_SPR_CTRL, params->specialcaseen);
    switch (params->spr_format_type) {
    case MTK_PANEL_RGBG_BGRG_TYPE:
        reg_val = 0x00050502;
        break;
    case MTK_PANEL_BGRG_RGBG_TYPE:
        reg_val = 0x00500052;
        break;
    case MTK_PANEL_RGBRGB_BGRBGR_TYPE:
        reg_val = 0x03154203;
        break;
    case MTK_PANEL_BGRBGR_RGBRGB_TYPE:
        reg_val = 0x04203153;
        break;
    case MTK_PANEL_RGBRGB_BRGBRG_TYPE:
        reg_val = 0x04200423;
        break;
    case MTK_PANEL_BRGBRG_RGBRGB_TYPE:
        reg_val = 0x00424203;
        break;
    default:
        reg_val = 0x03154203;
        break;
    }
    DISP_REG_SET(handle, base + DISP_REG_SPR_ARRANGE1, reg_val);
    for (j = 0; j < SPR_COLOR_PARAMS_TYPE_NUM; j++) {
        spr_color_params = &params->spr_color_params[j];
        switch (spr_color_params->spr_color_params_type) {
        case SPR_WEIGHT_SET:
            for (i = 0; i < spr_color_params->count; i += 4) {
                reg_val = (spr_color_params->para_list[i]) |
                        (spr_color_params->para_list[i+1] << 8) |
                        (spr_color_params->para_list[i+2] << 16) |
                        (spr_color_params->para_list[i+3] << 24);
                DISP_REG_SET(handle, base + DISP_REG_SPR_WEIGHT0 + 0x4 * i/4, reg_val);
            }
            break;
        case SPR_BORDER_SET:
            for (i = 0; i < 72; i += 8) {
                reg_val = (spr_color_params->para_list[i]) |
                        (spr_color_params->para_list[i+1] << 4) |
                        (spr_color_params->para_list[i+2] << 8) |
                        (spr_color_params->para_list[i+3] << 12) |
                        (spr_color_params->para_list[i+4] << 16) |
                        (spr_color_params->para_list[i+5] << 20) |
                        (spr_color_params->para_list[i+6] << 24) |
                        (spr_color_params->para_list[i+7] << 28);
                DISP_REG_SET(handle, base + DISP_REG_SPR_BORDER0 + 0x4 * i/8, reg_val);
            }
            for (i = 72; i < spr_color_params->count; i += 4) {
                reg_val = (spr_color_params->para_list[i]) |
                        (spr_color_params->para_list[i+1] << 8) |
                        (spr_color_params->para_list[i+2] << 16) |
                        (spr_color_params->para_list[i+3] << 24);
                DISP_REG_SET(handle, base + DISP_REG_SPR_BORDER9 + 0x4 * (i - 72)/4, reg_val);
            }
            break;
        case SPR_SPE_SET:
            for (i = 0; i < spr_color_params->count; i += 4) {
               reg_val = (spr_color_params->para_list[i]) |
                        (spr_color_params->para_list[i+1] << 8) |
                        (spr_color_params->para_list[i+2] << 16) |
                        (spr_color_params->para_list[i+3] << 24);
                DISP_REG_SET(handle, base + DISP_REG_SPR_SPE0 + 0x4 * i/4, reg_val);
            }
            break;
        default:
            break;
        }
    }

    return 0;
}

static int disp_spr_dump(enum DISP_MODULE_ENUM module, int level)
{
    int offset = disp_spr_reg_offset(module);
    unsigned long base = DISPSYS_SPR0_BASE + offset;
    u32 k = 0;

    DDPDUMP("== START: DISP SPR0 REGS ==\n");

    for (k = 0; k < 0x350; k += 16) {
        DDPDUMP("0x%04x: 0x%08x 0x%08x 0x%08x 0x%08x\n", k,
                readl(base + k),
                readl(base + k + 0x4),
                readl(base + k + 0x8),
                readl(base + k + 0xc));
    }

    return 0;
}

struct DDP_MODULE_DRIVER ddp_driver_spr = {
    .init = disp_spr_init,
    .deinit = disp_spr_deinit,
    .config = disp_spr_config,
    .dump_info = disp_spr_dump,
};

