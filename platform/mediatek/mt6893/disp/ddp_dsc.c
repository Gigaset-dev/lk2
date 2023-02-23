/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#define LOG_TAG "DSC"
#include "ddp_reg.h"
#include "ddp_info.h"
#include "ddp_log.h"

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

    val = 0x10001;
#ifdef DISP_PRIM_DUAL_PIPE
    if (params->slice_mode == 1)
        val = 0x10009;
#endif
    DISP_REG_SET(handle, base + DISP_REG_DSC_CON, val);

    init_delay_limit =
        ((128 + (params->xmit_delay + 2) / 3) * 3 +
        params->slice_width - 1) / params->slice_width;
    init_delay_height =
        (init_delay_limit > 15) ? 15 : init_delay_limit;

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

    return 0;
}

int dsc_dump(enum DISP_MODULE_ENUM module, int level)
{
    unsigned long base = DISPSYS_DSC_BASE;

    DDPDUMP("== START: DISP DSC REGS ==\n");
    DDPDUMP("0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x0, readl(base + 0x0),
        0x4, readl(base + 0x4),
        0x8, readl(base + 0x8),
        0xC, readl(base + 0xC));

    DDPDUMP("0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x10, readl(base + 0x10),
        0x18, readl(base + 0x18),
        0x1C, readl(base + 0x1C));

    DDPDUMP("0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x20, readl(base + 0x20),
        0x24, readl(base + 0x24),
        0x28, readl(base + 0x28),
        0x2C, readl(base + 0x2C));

    DDPDUMP("0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x30, readl(base + 0x30),
        0x34, readl(base + 0x34),
        0x38, readl(base + 0x38));

    DDPDUMP("0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x80, readl(base + 0x80),
        0x84, readl(base + 0x84),
        0x88, readl(base + 0x88),
        0x8C, readl(base + 0x8C));

    DDPDUMP("0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x90, readl(base + 0x90),
        0x94, readl(base + 0x94),
        0x98, readl(base + 0x98),
        0x9C, readl(base + 0x9C));

    DDPDUMP("0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0xA0, readl(base + 0xA0),
        0xA4, readl(base + 0xA4),
        0xA8, readl(base + 0xA8),
        0xAC, readl(base + 0xAC));

    DDPDUMP("0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0xB0, readl(base + 0xB0),
        0xB4, readl(base + 0xB4),
        0xB8, readl(base + 0xB8),
        0xBC, readl(base + 0xBC));

    DDPDUMP("0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0xC0, readl(base + 0xC0),
        0xC4, readl(base + 0xC4),
        0xC8, readl(base + 0xC8),
        0xCC, readl(base + 0xCC));

    DDPDUMP("0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x100, readl(base + 0x100),
        0x104, readl(base + 0x104),
        0x108, readl(base + 0x108),
        0x10C, readl(base + 0x10C));

    DDPDUMP("0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x110, readl(base + 0x110),
        0x114, readl(base + 0x114),
        0x118, readl(base + 0x118),
        0x11C, readl(base + 0x11C));

    DDPDUMP("0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x120, readl(base + 0x120),
        0x124, readl(base + 0x124),
        0x128, readl(base + 0x128),
        0x12C, readl(base + 0x12C));

    DDPDUMP("0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x130, readl(base + 0x130),
        0x134, readl(base + 0x134),
        0x138, readl(base + 0x138),
        0x13C, readl(base + 0x13C));

    DDPDUMP("0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x140, readl(base + 0x140),
        0x144, readl(base + 0x144),
        0x148, readl(base + 0x148),
        0x14C, readl(base + 0x14C));

    DDPDUMP("0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        0x150, readl(base + 0x150),
        0x154, readl(base + 0x154),
        0x158, readl(base + 0x158),
        0x15C, readl(base + 0x15C));
    DDPDUMP("0x%04x=0x%08x\n", 0x200, readl(base + 0x200));

    return 0;
}

struct DDP_MODULE_DRIVER ddp_driver_dsc = {
    .init = dsc_init,
    .deinit = dsc_deinit,
    .config = dsc_config,
    .dump_info = dsc_dump,
};

