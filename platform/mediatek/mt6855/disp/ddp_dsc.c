/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#define LOG_TAG "DSC"

#include "ddp_info.h"
#include "ddp_log.h"
#include "ddp_reg.h"
#include "ddp_reg_dsc.h"
#include "disp_drv_platform.h"
#include "primary_display.h"

int dsc_clock_on(enum DISP_MODULE_ENUM module, void *handle)
{
    DDPDBG("%s clock_on\n", ddp_get_module_name(module));

    ddp_enable_module_clock(module);

    return 0;
}

int dsc_clock_off(enum DISP_MODULE_ENUM module, void *handle)
{
    DDPDBG("%s clock_off\n", ddp_get_module_name(module));

    ddp_disable_module_clock(module);

    return 0;
}

int dsc_init(enum DISP_MODULE_ENUM module, void *handle)
{
    return dsc_clock_on(module, handle);
}

int dsc_deinit(enum DISP_MODULE_ENUM module, void *handle)
{
    return dsc_clock_off(module, handle);
}

int dsc_config(enum DISP_MODULE_ENUM module, struct disp_ddp_path_config *pConfig,
    void *handle)
{
    unsigned long base = DISPSYS_DSC_BASE;

    unsigned int init_delay_limit, init_delay_height;
    unsigned int pic_group_width_m1;
    unsigned int pic_height_m1, pic_height_ext_m1, pic_height_ext_num;
    unsigned int slice_group_width_m1;
    unsigned int pad_num;
    unsigned int val;
    unsigned int enc_slice_width, enc_pic_width;

    LCM_DSI_PARAMS *dsi = &pConfig->dsi_config;
    LCM_DSC_CONFIG_PARAMS *params = &dsi->dsc_params;
    unsigned int height = params->pic_height;
    unsigned int width = params->pic_width;

    if (dsi->dsc_enable == 0) {
        DISP_REG_SET_FIELD(handle, CON_FLD_DSC_EN,
            base + DISP_REG_DSC_CON, 0x1);
        DISP_REG_SET_FIELD(handle, CON_FLD_DISP_DSC_RELAY,
            base + DISP_REG_DSC_CON, 0x1);
        DISP_REG_SET(handle, base + DISP_REG_DSC_PIC_H, height - 1);
        DISP_REG_SET(handle, base + DISP_REG_DSC_PIC_W, width);

        return 0;
    }
    if (params->ver == 2)
        val = 0x14081;
    else
        val = 0x14001;
#ifdef DISP_PRIM_DUAL_PIPE
    if (params->slice_mode == 1)
        val = val + 0x8;
#endif
    DISP_REG_SET(handle, base + DISP_REG_DSC_CON, val);

    init_delay_limit =
        ((128 + (params->xmit_delay + 2) / 3) * 3 +
        params->slice_width - 1) / params->slice_width;
    init_delay_height =
        (init_delay_limit > 15) ? 15 : init_delay_limit;
    if (primary_display_is_video_mode())
        init_delay_height = 1;
    else
        init_delay_height = 4;

    val = params->slice_mode + (init_delay_height << 8) + (1 << 16);
    DISP_REG_SET(handle, base + DISP_REG_DSC_MODE, val);

    pic_group_width_m1 = (width + 2) / 3 - 1;
    DISP_REG_SET(handle, base + DISP_REG_DSC_PIC_W,
        (pic_group_width_m1 << 16) + width);

    pic_height_m1 = height - 1;
    pic_height_ext_num = (height + params->slice_height - 1) /
        params->slice_height;
    pic_height_ext_m1 = pic_height_ext_num * params->slice_height - 1;
    DISP_REG_SET(handle, base + DISP_REG_DSC_PIC_H,
        (pic_height_ext_m1 << 16) + pic_height_m1);

    slice_group_width_m1 = (params->slice_width + 2) / 3 - 1;
    DISP_REG_SET(handle, base + DISP_REG_DSC_SLICE_W,
        (slice_group_width_m1 << 16) + params->slice_width);

    DISP_REG_SET(handle, base + DISP_REG_DSC_SLICE_H,
        ((params->slice_width % 3) << 30) +
        ((pic_height_ext_num - 1) << 16) + params->slice_height - 1);

    DISP_REG_SET(handle, base + DISP_REG_DSC_SPR, 0);
    enc_slice_width = params->slice_width;
    enc_pic_width = enc_slice_width * (params->slice_mode + 1);
    if (enc_pic_width < 1440)
        DISP_REG_SET(handle, base + DISP_REG_DSC_OBUF, 0x800002d9);
    val = (enc_pic_width << 16) + enc_slice_width;
    DISP_REG_SET(handle, base + DISP_REG_DSC_ENC_WIDTH, val);

    DISP_REG_SET(handle, base + DISP_REG_DSC_CHUNK_SIZE,
        params->chunk_size);
    DISP_REG_SET(handle, base + DISP_REG_DSC_BUF_SIZE,
        params->chunk_size * params->slice_height);

    pad_num = (params->chunk_size + 2) / 3 * 3 - params->chunk_size;
    DISP_REG_SET(handle, base + DISP_REG_DSC_PAD, pad_num);

    if (params->dsc_cfg)
        DISP_REG_SET(handle, base + DISP_REG_DSC_CFG, params->dsc_cfg);
    else
        DISP_REG_SET(handle, base + DISP_REG_DSC_CFG, 0x22);

    if ((params->ver & 0xf) == 2)
        DISP_REG_SET_FIELD(handle, DSC_FLD_VER,
            base + DISP_REG_DSC_SHADOW, 0x2); /* DSC V1.2 */
    else
        DISP_REG_SET_FIELD(handle, DSC_FLD_VER,
            base + DISP_REG_DSC_SHADOW, 0x1); /* DSC V1.1 */

    /* set PPS */
    val = params->dsc_line_buf_depth + (params->bit_per_channel << 4) +
        (params->bit_per_pixel << 8) + (params->rct_on << 18) +
        (params->bp_enable << 19);
    DISP_REG_SET(handle, base + DISP_REG_DSC_PPS0, val);

    val = (params->xmit_delay) + (params->dec_delay << 16);
    DISP_REG_SET(handle, base + DISP_REG_DSC_PPS1, val);

    val = (params->scale_value) + (params->increment_interval << 16);
    DISP_REG_SET(handle, base + DISP_REG_DSC_PPS2, val);

    val = (params->decrement_interval) + (params->line_bpg_offset << 16);
    DISP_REG_SET(handle, base + DISP_REG_DSC_PPS3, val);

    val = (params->nfl_bpg_offset) + (params->slice_bpg_offset << 16);
    DISP_REG_SET(handle, base + DISP_REG_DSC_PPS4, val);

    val = (params->initial_offset) + (params->final_offset << 16);
    DISP_REG_SET(handle, base + DISP_REG_DSC_PPS5, val);

    val = (params->flatness_minqp) + (params->flatness_maxqp << 8) +
        (params->rc_model_size << 16);
    DISP_REG_SET(handle, base + DISP_REG_DSC_PPS6, val);

    val = (params->rc_edge_factor) + (params->rc_quant_incr_limit0 << 8) +
        (params->rc_quant_incr_limit1 << 16) +
        (params->rc_tgt_offset_hi << 24) +
        (params->rc_tgt_offset_lo << 28);
    DISP_REG_SET(handle, base + DISP_REG_DSC_PPS7, val);

    if (params->bit_per_channel == 10) {
        //10bpc_to_8bpp_20_slice_h
        DISP_REG_SET(handle, base + DISP_REG_DSC_PPS8, 0x382a1c0e);
        DISP_REG_SET(handle, base + DISP_REG_DSC_PPS9, 0x69625446);
        DISP_REG_SET(handle, base + DISP_REG_DSC_PPS10, 0x7b797770);
        DISP_REG_SET(handle, base + DISP_REG_DSC_PPS11, 0x00007e7d);
        DISP_REG_SET(handle, base + DISP_REG_DSC_PPS12, 0x01040900);
        DISP_REG_SET(handle, base + DISP_REG_DSC_PPS13, 0xF9450125);
        DISP_REG_SET(handle, base + DISP_REG_DSC_PPS14, 0xE967F167);
        DISP_REG_SET(handle, base + DISP_REG_DSC_PPS15, 0xE187E167);
        DISP_REG_SET(handle, base + DISP_REG_DSC_PPS16, 0xD9C7E1A7);
        DISP_REG_SET(handle, base + DISP_REG_DSC_PPS17, 0xD1E9D9C9);
        DISP_REG_SET(handle, base + DISP_REG_DSC_PPS18, 0xD20DD1E9);
        DISP_REG_SET(handle, base + DISP_REG_DSC_PPS19, 0x0000D230);
    } else {
        DISP_REG_SET(handle, base + DISP_REG_DSC_PPS8, 0x382a1c0e);
        DISP_REG_SET(handle, base + DISP_REG_DSC_PPS9, 0x69625446);
        DISP_REG_SET(handle, base + DISP_REG_DSC_PPS10, 0x7b797770);
        DISP_REG_SET(handle, base + DISP_REG_DSC_PPS11, 0x7e7d);
        DISP_REG_SET(handle, base + DISP_REG_DSC_PPS12, 0x800880);
        DISP_REG_SET(handle, base + DISP_REG_DSC_PPS13, 0xf8c100a1);
        DISP_REG_SET(handle, base + DISP_REG_DSC_PPS14, 0xe8e3f0e3);
        DISP_REG_SET(handle, base + DISP_REG_DSC_PPS15, 0xe103e0e3);
        DISP_REG_SET(handle, base + DISP_REG_DSC_PPS16, 0xd943e123);
        DISP_REG_SET(handle, base + DISP_REG_DSC_PPS17, 0xd185d965);
        DISP_REG_SET(handle, base + DISP_REG_DSC_PPS18, 0xd1a7d1a5);
        DISP_REG_SET(handle, base + DISP_REG_DSC_PPS19, 0xd1ed);
    }

    return 0;
}

int dsc_dump(enum DISP_MODULE_ENUM module, int level)
{
    unsigned long base = DISPSYS_DSC_BASE;
    int k;

    DDPDUMP("== START: DISP DSC REGS ==\n");
    for (k = 0; k < 0x230; k += 16) {
        DDPDUMP("0x%04x: 0x%08x 0x%08x 0x%08x 0x%08x\n", k,
            readl(base + k),
            readl(base + k + 0x4),
            readl(base + k + 0x8),
            readl(base + k + 0xc));
    }

    return 0;
}

struct DDP_MODULE_DRIVER ddp_driver_dsc = {
    .init = dsc_init,
    .deinit = dsc_deinit,
    .config = dsc_config,
    .dump_info = dsc_dump,
};

