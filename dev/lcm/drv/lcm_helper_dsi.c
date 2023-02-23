/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include "lcm_helper.h"

#define ESD_CHECK_NUM (3)

static char *mtk_dsi_lcm_list;
static unsigned int mtk_dsi_lcm_count;

int mtk_dsi_lcm_list_init(void)
{
    int count = 0;

    count = mtk_lcm_list_count(MTK_LCM_NAME_DSI_KEY);
    if (count <= 0)
        return count;

    LCM_MALLOC(mtk_dsi_lcm_list, count * MTK_LCM_NAME_LENGTH);
    if (mtk_dsi_lcm_list == NULL) {
        LCM_MSG("%s, failed to allocate buffer, count:%u\n",
            __func__, count);
        return -ENOMEM;
    }

    mtk_dsi_lcm_count = mtk_lcm_list_parsing(mtk_dsi_lcm_list,
                    count, MTK_LCM_NAME_DSI_KEY);

    return mtk_dsi_lcm_count;
}

unsigned int mtk_lcm_get_dsi_count(void)
{
    return mtk_dsi_lcm_count;
}

const char *mtk_lcm_get_dsi_name(unsigned int id)
{
    if (id >= mtk_lcm_get_dsi_count()) {
        LCM_MSG("%s, invalid lcm index:%u\n", __func__, id);
        return NULL;
    }
    return mtk_dsi_lcm_list + id * MTK_LCM_NAME_LENGTH;
}

/* parsing of LCM_DSC_CONFIG_PARAMS */
static int parse_lcm_dsi_dsc_mode(const void *fdt, int nodeoffset,
            LCM_DSC_CONFIG_PARAMS *dsc_params)
{
    unsigned int dsc_enable = 0;

    if (fdt == NULL || dsc_params == NULL)
        return 0;

    lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-dsc_enable_lk",
            &dsc_enable);
    if (dsc_enable == 0)
        return 0;

    if (lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-lk-dsc_ver",
            &dsc_params->ver) <= 0)
        lcm_fdt_getprop_u32(fdt, nodeoffset,
                "lcm-params-dsi-dsc_ver",
                &dsc_params->ver);

    if (lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-lk-dsc_slice_width",
            &dsc_params->slice_width) <= 0)
        lcm_fdt_getprop_u32(fdt, nodeoffset,
                "lcm-params-dsi-dsc_slice_width",
                &dsc_params->slice_width);

    if (lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-lk-dsc_bit_per_pixel",
            &dsc_params->bit_per_pixel) <= 0)
        lcm_fdt_getprop_u32(fdt, nodeoffset,
                "lcm-params-dsi-dsc_bit_per_pixel",
                &dsc_params->bit_per_pixel);

    if (lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-lk-dsc_slice_mode",
            &dsc_params->slice_mode) <= 0)
        lcm_fdt_getprop_u32(fdt, nodeoffset,
                "lcm-params-dsi-dsc_slice_mode",
                &dsc_params->slice_mode);

    if (lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-lk-dsc_rgb_swap",
            &dsc_params->rgb_swap) <= 0)
        lcm_fdt_getprop_u32(fdt, nodeoffset,
                "lcm-params-dsi-dsc_rgb_swap",
                &dsc_params->rgb_swap);

    if (lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-lk-dsc_cfg",
            &dsc_params->dsc_cfg) <= 0)
        lcm_fdt_getprop_u32(fdt, nodeoffset,
                "lcm-params-dsi-dsc_cfg",
                &dsc_params->dsc_cfg);

    if (lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-lk-dsc_line_buf_depth",
            &dsc_params->dsc_line_buf_depth) <= 0)
        lcm_fdt_getprop_u32(fdt, nodeoffset,
                "lcm-params-dsi-dsc_line_buf_depth",
                &dsc_params->dsc_line_buf_depth);

    if (lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-lk-dsc_bit_per_channel",
            &dsc_params->bit_per_channel) <= 0)
        lcm_fdt_getprop_u32(fdt, nodeoffset,
                "lcm-params-dsi-dsc_bit_per_channel",
                &dsc_params->bit_per_channel);

    if (lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-lk-dsc_rct_on",
            &dsc_params->rct_on) <= 0)
        lcm_fdt_getprop_u32(fdt, nodeoffset,
                "lcm-params-dsi-dsc_rct_on",
                &dsc_params->rct_on);

    if (lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-lk-dsc_bp_enable",
            &dsc_params->bp_enable) <= 0)
        lcm_fdt_getprop_u32(fdt, nodeoffset,
                "lcm-params-dsi-dsc_bp_enable",
                &dsc_params->bp_enable);

    if (lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-lk-dsc_pic_height",
            &dsc_params->pic_height) <= 0)
        lcm_fdt_getprop_u32(fdt, nodeoffset,
                "lcm-params-dsi-dsc_pic_height",
                &dsc_params->pic_height);

    if (lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-lk-dsc_pic_width",
            &dsc_params->pic_width) <= 0)
        lcm_fdt_getprop_u32(fdt, nodeoffset,
                "lcm-params-dsi-dsc_pic_width",
                &dsc_params->pic_width);

    if (lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-lk-dsc_slice_height",
            &dsc_params->slice_height) <= 0)
        lcm_fdt_getprop_u32(fdt, nodeoffset,
                "lcm-params-dsi-dsc_slice_height",
                &dsc_params->slice_height);

    if (lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-lk-dsc_chunk_size",
            &dsc_params->chunk_size) <= 0)
        lcm_fdt_getprop_u32(fdt, nodeoffset,
                "lcm-params-dsi-dsc_chunk_size",
                &dsc_params->chunk_size);

    if (lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-lk-dsc_dec_delay",
            &dsc_params->dec_delay) <= 0)
        lcm_fdt_getprop_u32(fdt, nodeoffset,
                "lcm-params-dsi-dsc_dec_delay",
                &dsc_params->dec_delay);

    if (lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-lk-dsc_xmit_delay",
            &dsc_params->xmit_delay) <= 0)
        lcm_fdt_getprop_u32(fdt, nodeoffset,
                "lcm-params-dsi-dsc_xmit_delay",
                &dsc_params->xmit_delay);

    if (lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-lk-dsc_scale_value",
            &dsc_params->scale_value) <= 0)
        lcm_fdt_getprop_u32(fdt, nodeoffset,
                "lcm-params-dsi-dsc_scale_value",
                &dsc_params->scale_value);

    if (lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-lk-dsc_increment_interval",
            &dsc_params->increment_interval) <= 0)
        lcm_fdt_getprop_u32(fdt, nodeoffset,
                "lcm-params-dsi-dsc_increment_interval",
                &dsc_params->increment_interval);

    if (lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-lk-dsc_line_bpg_offset",
            &dsc_params->line_bpg_offset) <= 0)
        lcm_fdt_getprop_u32(fdt, nodeoffset,
                "lcm-params-dsi-dsc_line_bpg_offset",
                &dsc_params->line_bpg_offset);

    if (lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-lk-dsc_decrement_interval",
            &dsc_params->decrement_interval) <= 0)
        lcm_fdt_getprop_u32(fdt, nodeoffset,
                "lcm-params-dsi-dsc_decrement_interval",
                &dsc_params->decrement_interval);

    if (lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-lk-dsc_nfl_bpg_offset",
            &dsc_params->nfl_bpg_offset) <= 0)
        lcm_fdt_getprop_u32(fdt, nodeoffset,
                "lcm-params-dsi-dsc_nfl_bpg_offset",
                &dsc_params->nfl_bpg_offset);

    if (lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-lk-dsc_slice_bpg_offset",
            &dsc_params->slice_bpg_offset) <= 0)
        lcm_fdt_getprop_u32(fdt, nodeoffset,
                "lcm-params-dsi-dsc_slice_bpg_offset",
                &dsc_params->slice_bpg_offset);

    if (lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-lk-dsc_initial_offset",
            &dsc_params->initial_offset) <= 0)
        lcm_fdt_getprop_u32(fdt, nodeoffset,
                "lcm-params-dsi-dsc_initial_offset",
                &dsc_params->initial_offset);

    if (lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-lk-dsc_final_offset",
            &dsc_params->final_offset) <= 0)
        lcm_fdt_getprop_u32(fdt, nodeoffset,
                "lcm-params-dsi-dsc_final_offset",
                &dsc_params->final_offset);

    if (lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-lk-dsc_flatness_minqp",
            &dsc_params->flatness_minqp) <= 0)
        lcm_fdt_getprop_u32(fdt, nodeoffset,
                "lcm-params-dsi-dsc_flatness_minqp",
                &dsc_params->flatness_minqp);

    if (lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-lk-dsc_flatness_maxqp",
            &dsc_params->flatness_maxqp) <= 0)
        lcm_fdt_getprop_u32(fdt, nodeoffset,
                "lcm-params-dsi-dsc_flatness_maxqp",
                &dsc_params->flatness_maxqp);

    if (lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-lk-dsc_rc_model_size",
            &dsc_params->rc_model_size) <= 0)
        lcm_fdt_getprop_u32(fdt, nodeoffset,
                "lcm-params-dsi-dsc_rc_model_size",
                &dsc_params->rc_model_size);

    if (lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-lk-dsc_rc_edge_factor",
            &dsc_params->rc_edge_factor) <= 0)
        lcm_fdt_getprop_u32(fdt, nodeoffset,
                "lcm-params-dsi-dsc_rc_edge_factor",
                &dsc_params->rc_edge_factor);

    if (lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-lk-dsc_rc_quant_incr_limit0",
            &dsc_params->rc_quant_incr_limit0) <= 0)
        lcm_fdt_getprop_u32(fdt, nodeoffset,
                "lcm-params-dsi-dsc_rc_quant_incr_limit0",
                &dsc_params->rc_quant_incr_limit0);

    if (lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-lk-dsc_rc_quant_incr_limit1",
            &dsc_params->rc_quant_incr_limit1) <= 0)
        lcm_fdt_getprop_u32(fdt, nodeoffset,
                "lcm-params-dsi-dsc_rc_quant_incr_limit1",
                &dsc_params->rc_quant_incr_limit1);

    if (lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-lk-dsc_rc_tgt_offset_hi",
            &dsc_params->rc_tgt_offset_hi) <= 0)
        lcm_fdt_getprop_u32(fdt, nodeoffset,
                "lcm-params-dsi-dsc_rc_tgt_offset_hi",
                &dsc_params->rc_tgt_offset_hi);

    if (lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-lk-dsc_rc_tgt_offset_lo",
            &dsc_params->rc_tgt_offset_lo) <= 0)
        lcm_fdt_getprop_u32(fdt, nodeoffset,
                "lcm-params-dsi-dsc_rc_tgt_offset_lo",
                &dsc_params->rc_tgt_offset_lo);

    return 1;
}

/* parsing of struct mtk_dsi_phy_timcon */
static void parse_lcm_dsi_phy_timcon(const void *fdt, int nodeoffset,
            LCM_DSI_PARAMS *params)
{
    if (fdt == NULL || params == NULL)
        return;

    lcm_fdt_getprop_u8(fdt, nodeoffset,
            "lcm-params-dsi-phy_timcon_lk_hs_trail",
            &params->HS_TRAIL);
    lcm_fdt_getprop_u8(fdt, nodeoffset,
            "lcm-params-dsi-phy_timcon_lk_hs_zero",
            &params->HS_ZERO);
    lcm_fdt_getprop_u8(fdt, nodeoffset,
            "lcm-params-dsi-phy_timcon_lk_hs_prpr",
            &params->HS_PRPR);
    lcm_fdt_getprop_u8(fdt, nodeoffset,
            "lcm-params-dsi-phy_timcon_lk_lpx",
            &params->LPX);
    lcm_fdt_getprop_u8(fdt, nodeoffset,
            "lcm-params-dsi-phy_timcon_lk_ta_sack",
            &params->TA_SACK);
    lcm_fdt_getprop_u8(fdt, nodeoffset,
            "lcm-params-dsi-phy_timcon_lk_ta_get",
            &params->TA_GET);
    lcm_fdt_getprop_u8(fdt, nodeoffset,
            "lcm-params-dsi-phy_timcon_lk_ta_sure",
            &params->TA_SURE);
    lcm_fdt_getprop_u8(fdt, nodeoffset,
            "lcm-params-dsi-phy_timcon_lk_ta_go",
            &params->TA_GO);
    lcm_fdt_getprop_u8(fdt, nodeoffset,
            "lcm-params-dsi-phy_timcon_lk_clk_trail",
            &params->CLK_TRAIL);
    lcm_fdt_getprop_u8(fdt, nodeoffset,
            "lcm-params-dsi-phy_timcon_lk_clk_zero",
            &params->CLK_ZERO);
    lcm_fdt_getprop_u8(fdt, nodeoffset,
            "lcm-params-dsi-phy_timcon_lk_lpx_wait",
            &params->LPX_WAIT);
    lcm_fdt_getprop_u8(fdt, nodeoffset,
            "lcm-params-dsi-phy_timcon_lk_cont_det",
            &params->CONT_DET);
    lcm_fdt_getprop_u8(fdt, nodeoffset,
            "lcm-params-dsi-phy_timcon_lk_clk_hs_prpr",
            &params->CLK_HS_PRPR);
    lcm_fdt_getprop_u8(fdt, nodeoffset,
            "lcm-params-dsi-phy_timcon_lk_clk_hs_post",
            &params->CLK_HS_POST);
    lcm_fdt_getprop_u8(fdt, nodeoffset,
            "lcm-params-dsi-phy_timcon_lk_clk_hs_exit",
            &params->CLK_HS_EXIT);
    lcm_fdt_getprop_u8(fdt, nodeoffset,
            "lcm-params-dsi-phy_timcon_lk_da_hs_exit",
            &params->DA_HS_EXIT);
}

/* parsing of struct dynamic_mipi_params */
static void parse_lcm_dsi_dyn(const void *fdt, int nodeoffset,
            LCM_DSI_PARAMS *params)
{
    if (fdt == NULL  || params == NULL)
        return;

    lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-dyn_switch_en",
            &params->dynamic_switch_mipi);

    lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-dyn_vsa",
            &params->vertical_sync_active_dyn);
    lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-dyn_vbp",
            &params->vertical_backporch_dyn);
    lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-dyn_vfp",
            &params->vertical_frontporch_dyn);
    lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-dyn_vac",
            &params->vertical_active_line_dyn);

    lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-dyn_hsa",
            &params->horizontal_sync_active_dyn);
    lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-dyn_hbp",
            &params->horizontal_backporch_dyn);
    lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-dyn_hfp",
            &params->horizontal_frontporch_dyn);
    lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-dyn_hac",
            &params->horizontal_active_pixel_dyn);

    lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-dyn_pll_clk",
            &params->PLL_CLOCK_dyn);
    lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-dyn_data_rate",
            &params->data_rate_dyn);
}

/* parse dsi fps mode: drm_display_mode */
static void parse_lcm_dsi_fps_mode(const void *fdt, int nodeoffset,
            int lk_offset,
            LCM_DSI_PARAMS *params)
{
    if (fdt == NULL || params == NULL)
        return;

    lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-vertical_sync_active",
            &params->vertical_sync_active);
    lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-vertical_backporch",
            &params->vertical_backporch);
    lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-vertical_frontporch",
            &params->vertical_frontporch);
    lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-vfp_for_low_power",
            &params->vertical_frontporch_for_low_power);
    params->vertical_vfp_lp = params->vertical_frontporch_for_low_power;

    lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-vertical_active_line",
            &params->vertical_active_line);

    lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-horizontal_sync_active",
            &params->horizontal_sync_active);
    lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-horizontal_backporch",
            &params->horizontal_backporch);
    lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-horizontal_frontporch",
            &params->horizontal_frontporch);
    lcm_fdt_getprop_u32(fdt, lk_offset,
            "lcm-params-dsi-lk_horizontal_blanking_pixel",
            &params->horizontal_blanking_pixel);
    lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-horizontal_active_pixel",
            &params->horizontal_active_pixel);
    lcm_fdt_getprop_u32(fdt, lk_offset,
            "lcm-params-dsi-lk_bllp",
            &params->horizontal_bllp);

    lcm_fdt_getprop_u32(fdt, lk_offset,
            "lcm-params-dsi-lk_horizontal_sync_active_byte",
            &params->horizontal_sync_active_byte);
    lcm_fdt_getprop_u32(fdt, lk_offset,
            "lcm-params-dsi-lk_horizontal_backporch_byte",
            &params->horizontal_backporch_byte);
    lcm_fdt_getprop_u32(fdt, lk_offset,
            "lcm-params-dsi-lk_horizontal_frontporch_byte",
            &params->horizontal_frontporch_byte);
    lcm_fdt_getprop_u32(fdt, lk_offset,
            "lcm-params-dsi-lk_horizontal_sync_active_word_count",
            &params->horizontal_sync_active_word_count);
    lcm_fdt_getprop_u32(fdt, lk_offset,
            "lcm-params-dsi-lk_horizontal_backporch_word_count",
            &params->horizontal_backporch_word_count);
    lcm_fdt_getprop_u32(fdt, lk_offset,
            "lcm-params-dsi-lk_horizontal_frontporch_word_count",
            &params->horizontal_frontporch_word_count);
}

/* parse dsi fps ext params: mtk_panel_params */
static void do_parse_lcm_dsi_lk_params(const void *fdt, int nodeoffset,
            int lk_offset, LCM_DSI_PARAMS *params)
{
    char prop[128] = { 0 };
    u32 temp[10] = {0};
    u8 temp2[10] = {0};
    unsigned int i = 0, j = 0;
    int len = 0, ret = 0;

    if (fdt == NULL || params == NULL)
        return;

    lcm_fdt_getprop_u32(fdt, lk_offset,
            "lcm-params-dsi-lk_mode",
            &params->mode);
    lcm_fdt_getprop_u32(fdt, lk_offset,
            "lcm-params-dsi-lk_switch_mode",
            &params->switch_mode);
    lcm_fdt_getprop_u32(fdt, lk_offset,
            "lcm-params-dsi-lk_switch_mode_enable",
            &params->switch_mode_enable);
    lcm_fdt_getprop_u32(fdt, lk_offset,
            "lcm-params-dsi-lk_dsi_wmem_conti",
            &params->DSI_WMEM_CONTI);
    lcm_fdt_getprop_u32(fdt, lk_offset,
            "lcm-params-dsi-lk_dsi_rmem_conti",
            &params->DSI_RMEM_CONTI);
    lcm_fdt_getprop_u32(fdt, lk_offset,
            "lcm-params-dsi-lk_vc_num",
            &params->VC_NUM);

    len = lcm_fdt_getprop_u32_array(fdt, lk_offset,
            "lcm-params-dsi-lk_data_format",
            (u32 *)&temp[0], 4);
    if (len != 4) {
        LCM_MSG("%s, %d: invalid data format:%d\n",
            __func__, __LINE__, len);
    } else {
        params->data_format.color_order = temp[0];
        params->data_format.trans_seq = temp[1];
        params->data_format.padding = temp[2];
        params->data_format.format = temp[3];
    }

    lcm_fdt_getprop_u32(fdt, lk_offset,
            "lcm-params-dsi-lk_intermediat_buffer_num",
            &params->intermediat_buffer_num);
    lcm_fdt_getprop_u32(fdt, lk_offset,
            "lcm-params-dsi-lk_ps",
            &params->PS);
    lcm_fdt_getprop_u32(fdt, lk_offset,
            "lcm-params-dsi-lk_word_count",
            &params->word_count);
    lcm_fdt_getprop_u32(fdt, lk_offset,
            "lcm-params-dsi-lk_packet_size",
            &params->packet_size);

    lcm_fdt_getprop_u32(fdt, lk_offset,
            "lcm-params-dsi-lk_pll_select",
            &params->pll_select);

    len = lcm_fdt_getprop_u32_array(fdt, lk_offset,
            "lcm-params-dsi-lk_pll_div",
            (u32 *)&temp[0], 2);
    if (len == 2) {
        params->pll_div1 = temp[0];
        params->pll_div2 = temp[1];
    }

    lcm_fdt_getprop_u32(fdt, lk_offset,
            "lcm-params-dsi-lk_fbk_div",
            &params->fbk_div);
    lcm_fdt_getprop_u32(fdt, lk_offset,
            "lcm-params-dsi-lk_fbk_sel",
            &params->fbk_sel);

    len = lcm_fdt_getprop_u32_array(fdt, lk_offset,
            "lcm-params-dsi-lk_rg",
            (u32 *)&temp[0], 3);
    if (len == 3) {
        params->rg_bir = temp[0];
        params->rg_bic = temp[1];
        params->rg_bp = temp[2];
    }

    if (lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-lk-pll_clock",
            &params->PLL_CLOCK) <= 0)
        lcm_fdt_getprop_u32(fdt, nodeoffset,
                "lcm-params-dsi-pll_clock",
                &params->PLL_CLOCK);

    if (lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-lk-data_rate",
            &params->data_rate) <= 0)
        lcm_fdt_getprop_u32(fdt, nodeoffset,
                "lcm-params-dsi-data_rate",
                &params->data_rate);

    lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-output_mode",
            &params->output_mode);
    lcm_fdt_getprop_u32(fdt, lk_offset,
            "lcm-params-dsi-lk_dsi_clock",
            &params->dsi_clock);

    lcm_fdt_getprop_u32(fdt, lk_offset,
            "lcm-params-dsi-lk_ssc_disable",
            &params->ssc_disable);
    lcm_fdt_getprop_u32(fdt, lk_offset,
            "lcm-params-dsi-lk_ssc_range",
            &params->ssc_range);

    lcm_fdt_getprop_u32(fdt, lk_offset,
            "lcm-params-dsi-lk_compatibility_for_nvk",
            &params->compatibility_for_nvk);
    lcm_fdt_getprop_u32(fdt, lk_offset,
            "lcm-params-dsi-lk_cont_clock",
            &params->cont_clock);
    lcm_fdt_getprop_u32(fdt, lk_offset,
            "lcm-params-dsi-lk_ufoe_enable",
            &params->ufoe_enable);
    if (params->ufoe_enable != 0) {
        len = lcm_fdt_getprop_u32_array(fdt, lk_offset,
                "lcm-params-dsi-lk_ufoe_params",
                (u32 *)&temp[0], 4);
        if (len != 4) {
            LCM_MSG("%s, %d: invalid ufoe params:%d\n",
                __func__, __LINE__, len);
        } else {
            params->ufoe_params.compress_ratio = temp[0];
            params->ufoe_params.lr_mode_en = temp[1];
            params->ufoe_params.vlc_disable = temp[2];
            params->ufoe_params.vlc_config = temp[3];
        }
    }

    lcm_fdt_getprop_u32(fdt, lk_offset,
            "lcm-params-dsi-lk_edp_panel",
            &params->edp_panel);
    /* esd check */
    lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-cust_esd_check",
            &params->customization_esd_check_enable);
    lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-esd_check_enable",
            &params->esd_check_enable);
    if (params->esd_check_enable != 0) {
        for (i = 0; i < ESD_CHECK_NUM; i++) {
            ret = snprintf(prop, sizeof(prop),
                "lcm-params-dsi-lcm_esd_check_table%u", i);
            if (ret < 0 || (size_t)ret >= sizeof(prop))
                LCM_MSG("%s, snprintf failed, ret:%d\n", __func__, ret);
            len = lcm_fdt_getprop_u8_array(fdt, nodeoffset,
                    prop, &temp2);
            if (len <= 0) {
                LCM_DBG("%s, %d: the %d esd table of invalid cmd:%d\n",
                    __func__, __LINE__, i, len);
                continue;
            }
            params->lcm_esd_check_table[i].cmd = temp2[0];
            params->lcm_esd_check_table[i].count = temp2[1];
            if (params->lcm_esd_check_table[i].count == 0)
                continue;
            if (len != params->lcm_esd_check_table[i].count + 2) {
                LCM_DBG("%s, %d: the %d esd table of invalid cmd length:%d\n",
                    __func__, __LINE__, i, len);
                params->lcm_esd_check_table[i].count = 0;
                continue;
            }
            for (j = 0;
                 j < params->lcm_esd_check_table[i].count; j++)
                params->lcm_esd_check_table[i].para_list[j] =
                    temp2[j + 2];
#if 0
            for (j = 0;
                 j < params->lcm_esd_check_table[i].count; j++)
                params->lcm_esd_check_table[i].mask_list[j] =
                    temp2[j + 2 +
                    params->lcm_esd_check_table[i].count];
#endif
        }
    }

    lcm_fdt_getprop_u32(fdt, lk_offset,
            "lcm-params-dsi-lk_lcm_int_te_monitor",
            &params->lcm_int_te_monitor);
    lcm_fdt_getprop_u32(fdt, lk_offset,
            "lcm-params-dsi-lk_lcm_int_te_period",
            &params->lcm_int_te_period);
    lcm_fdt_getprop_u32(fdt, lk_offset,
            "lcm-params-dsi-lk_lcm_ext_te_monitor",
            &params->lcm_ext_te_monitor);
    lcm_fdt_getprop_u32(fdt, lk_offset,
            "lcm-params-dsi-lk_lcm_ext_te_period",
            &params->lcm_ext_te_enable);

    lcm_fdt_getprop_u32(fdt, lk_offset,
            "lcm-params-dsi-lk_noncont_clock",
            &params->noncont_clock);
    lcm_fdt_getprop_u32(fdt, lk_offset,
            "lcm-params-dsi-lk_noncont_clock_period",
            &params->noncont_clock_period);
    lcm_fdt_getprop_u32(fdt, lk_offset,
            "lcm-params-dsi-lk_clk_lp_per_line_enable",
            &params->clk_lp_per_line_enable);
    lcm_fdt_getprop_u32(fdt, lk_offset,
            "lcm-params-dsi-lk_dual_dsi_type",
            &params->dual_dsi_type);
    lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-lane_swap_en",
            &params->lane_swap_en);
    /* lane swap */
    if (params->lane_swap_en != 0) {
        for (i = 0; i < MIPITX_PHY_PORT_NUM; i++) {
            ret = snprintf(prop, sizeof(prop),
                "lcm-params-dsi-lane_swap%u", i);
            if (ret < 0 || (size_t)ret >= sizeof(prop))
                LCM_MSG("%s, snprintf failed, ret:%d\n", __func__, ret);
            len = lcm_fdt_getprop_u32_array(fdt, nodeoffset, prop,
                    (u32 *)&params->lane_swap[i][0],
                    MIPITX_PHY_LANE_NUM);
            if (len != MIPITX_PHY_LANE_NUM)
                LCM_MSG("%s, %d: the %d mipitx phy lane of invalid len:%d\n",
                    __func__, __LINE__, i, len);
        }
    }

    lcm_fdt_getprop_u32(fdt, lk_offset,
            "lcm-params-dsi-lk_pll_clock_lp",
            &params->PLL_CLOCK_lp);
    lcm_fdt_getprop_u32(fdt, lk_offset,
            "lcm-params-dsi-lk_ulps_sw_enable",
            &params->ulps_sw_enable);
    lcm_fdt_getprop_u32(fdt, lk_offset,
            "lcm-params-dsi-lk_null_packet_en",
            &params->null_packet_en);

    lcm_fdt_getprop_u32(fdt, lk_offset,
            "lcm-params-dsi-lk_mixmode_enable",
            &params->mixmode_enable);
    lcm_fdt_getprop_u32(fdt, lk_offset,
            "lcm-params-dsi-lk_mixmode_mipi_clock",
            &params->mixmode_mipi_clock);
    lcm_fdt_getprop_u32(fdt, lk_offset,
            "lcm-params-dsi-lk_pwm_fps",
            &params->pwm_fps);
    lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-vrefresh",
            &params->fps);
    lcm_fdt_getprop_u32(fdt, lk_offset,
            "lcm-params-dsi-lk_vact_fps",
            &params->vact_fps);
    lcm_fdt_getprop_u32(fdt, lk_offset,
            "lcm-params-dsi-lk_send_frame_enable",
            &params->send_frame_enable);

    lcm_fdt_getprop_u32(fdt, lk_offset,
            "lcm-params-dsi-lk_lfr_enable",
            &params->lfr_enable);
    lcm_fdt_getprop_u32(fdt, lk_offset,
            "lcm-params-dsi-lk_lfr_mode",
            &params->lfr_mode);
    lcm_fdt_getprop_u32(fdt, lk_offset,
            "lcm-params-dsi-lk_lfr_type",
            &params->lfr_type);
    lcm_fdt_getprop_u32(fdt, lk_offset,
            "lcm-params-dsi-lk_lfr_skip_num",
            &params->lfr_skip_num);

    lcm_fdt_getprop_u32(fdt, lk_offset,
            "lcm-params-dsi-lk_ext_te_edge",
            &params->ext_te_edge);
    lcm_fdt_getprop_u32(fdt, lk_offset,
            "lcm-params-dsi-lk_eint_disable",
            &params->eint_disable);

    len = lcm_fdt_getprop_u32_array(fdt, lk_offset,
            "lcm-params-dsi-lk_phy_sel",
            (u32 *)&temp[0], 4);
    if (len != 4) {
        LCM_MSG("%s, %d: invalid phy sel:%d\n",
            __func__, __LINE__, len);
    } else {
        params->PHY_SEL0 = temp[0];
        params->PHY_SEL1 = temp[1];
        params->PHY_SEL2 = temp[2];
        params->PHY_SEL3 = temp[3];
    }
}

/* parse dsi fps ext params: mtk_panel_params */
static void parse_lcm_dsi_fpga_settings(const void *fdt, int nodeoffset,
            LCM_DSI_PARAMS *params)
{
#ifdef PROJECT_TYPE_FPGA
    u32 temp[2] = { 0 };
    int len = 0;

    if (params == NULL || fdt == NULL)
        return;

    len = lcm_fdt_getprop_u32_array(fdt, nodeoffset,
            "lcm-params-dsi-lk_pll_div",
            (u32 *)&temp[0], 2);
    if (len == 2) {
        params->pll_div1 = temp[0];
        params->pll_div2 = temp[1];
    }

    lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-lk_fbk_div",
            &params->fbk_div);
#endif
}

static void parse_lcm_dsi_fps_setting(const void *fdt,
    int nodeoffset, unsigned int phy_type,
    LCM_DSI_PARAMS *params, const char *mode_key,
    struct mtk_panel_misc_data *misc)
{
    int child_offset = 0, ret = 0;
    char child[128] = { 0 };

    if (phy_type == MTK_LCM_MIPI_CPHY)
        params->IsCphy = 1;
    else
        params->IsCphy = 0;

    lcm_fdt_getprop_u32(fdt, nodeoffset,
            "lcm-params-dsi-voltage",
            &misc->voltage);

    /* fps params */
    ret = snprintf(child, sizeof(child) - 1,
             "lcm-params-dsi-lk-params-%s", mode_key);
    if (ret < 0 || (size_t)ret >= sizeof(child))
        LCM_MSG("%s, snprintf failed, ret:%d\n", __func__, ret);
    child_offset = fdt_subnode_offset(fdt, nodeoffset, child);
    if (child_offset < 0) {
        LCM_MSG("%s: invalid %s, %d\n", __func__, child, child_offset);
    } else {
        LCM_DBG("%s, parsing child:%s\n", __func__, child);
        parse_lcm_dsi_fps_mode(fdt, nodeoffset, child_offset, params);
        do_parse_lcm_dsi_lk_params(fdt, nodeoffset, child_offset, params);
    }

    /* dsc params */
    ret = snprintf(child, sizeof(child) - 1,
            "lcm-params-dsi-dsc-params-%s", mode_key);
    if (ret < 0 || (size_t)ret >= sizeof(child))
        LCM_MSG("%s, snprintf failed, ret:%d\n", __func__, ret);
    child_offset = fdt_subnode_offset(fdt, nodeoffset, child);
    if (child_offset < 0) {
        LCM_MSG("%s: invalid %s, %d-%d\n",
            __func__, child, nodeoffset, child_offset);
    } else {
        LCM_DBG("%s, parsing child:%s, %d-%d\n",
            __func__, child, nodeoffset, child_offset);
        params->dsc_enable = parse_lcm_dsi_dsc_mode(fdt, child_offset,
            &params->dsc_params);
    }

    /* phy timcon */
    ret = snprintf(child, sizeof(child) - 1,
            "lcm-params-dsi-phy-timcon-params-%s", mode_key);
    if (ret < 0 || (size_t)ret >= sizeof(child))
        LCM_MSG("%s, snprintf failed, ret:%d\n", __func__, ret);
    child_offset = fdt_subnode_offset(fdt, nodeoffset, child);
    if (child_offset < 0) {
        LCM_MSG("%s: invalid %s, %d-%d\n",
            __func__, child, nodeoffset, child_offset);
    } else {
        LCM_DBG("%s, parsing child:%s, %d-%d\n",
            __func__, child, nodeoffset, child_offset);
        parse_lcm_dsi_phy_timcon(fdt, child_offset, params);
    }

    /* dyn */
    ret = snprintf(child, sizeof(child) - 1,
            "lcm-params-dsi-dyn-params-%s", mode_key);
    if (ret < 0 || (size_t)ret >= sizeof(child))
        LCM_MSG("%s, snprintf failed, ret:%d\n", __func__, ret);
    child_offset = fdt_subnode_offset(fdt, nodeoffset, child);
    if (child_offset < 0) {
        LCM_MSG("%s: invalid %s, %d-%d\n",
            __func__, child, nodeoffset, child_offset);
    } else {
        LCM_DBG("%s, parsing child:%s, %d-%d\n",
            __func__, child, nodeoffset, child_offset);
        parse_lcm_dsi_dyn(fdt, child_offset, params);
    }

    /* fpga settings*/
    ret = snprintf(child, sizeof(child) - 1,
            "lcm-params-dsi-fpga-params-%s", mode_key);
    if (ret < 0 || (size_t)ret >= sizeof(child))
        LCM_MSG("%s, snprintf failed, ret:%d\n", __func__, ret);
    child_offset = fdt_subnode_offset(fdt, nodeoffset, child);
    if (child_offset < 0) {
        LCM_MSG("%s: invalid %s, %d-%d\n",
            __func__, child, nodeoffset, child_offset);
    } else {
        LCM_DBG("%s, parsing child:%s, %d-%d\n",
            __func__, child, nodeoffset, child_offset);
        parse_lcm_dsi_fpga_settings(fdt, child_offset, params);
    }
}

int parse_lcm_params_dsi(const void *fdt, int nodeoffset,
        LCM_DSI_PARAMS *params, struct mtk_panel_misc_data *misc)
{
    unsigned int phy_type = 0, default_mode = 0;
    unsigned int mode_count = 0, i = 0, id = 0;
    unsigned int *mode_list = NULL;
    char mode_node[128] = { 0 };
    char mode_key[128] = { 0 };
    int mode_offset = 0, len = 0;
    unsigned int width = 0, height = 0, fps = 0;
    unsigned int resolution[2] = { 0 };

    if (fdt == NULL || params == NULL || misc == NULL)
        return -EINVAL;

    memset(params, 0, sizeof(LCM_DSI_PARAMS));

    lcm_fdt_getprop_u32(fdt, nodeoffset, "lcm-params-dsi-phy_type",
            &phy_type);
    lcm_fdt_getprop_u32(fdt, nodeoffset, "lcm-params-dsi-lanes",
            &params->LANE_NUM);
    lcm_fdt_getprop_u32(fdt, nodeoffset, "lcm-params-dsi-need_fake_resolution",
            &misc->is_fake);
    lcm_fdt_getprop_u32_array(fdt, nodeoffset, "lcm-params-dsi-fake_resolution",
            &resolution[0], 2);
    misc->fake_width = resolution[0];
    misc->fake_height = resolution[1];

#if 0
    len = fdt_stringlist_count(fdt, nodeoffset, "pinctrl-names");
    if (len <= 0)
        LCM_MSG("%s, %d, lcm_pinctrl_name is null, %d\n",
            __func__, __LINE__, len);

    if (len > 0) {
        LCM_MALLOC(params->lcm_pinctrl_name, (len + 1) * MTK_LCM_NAME_LENGTH);
        if (params->lcm_pinctrl_name == NULL) {
            LCM_MSG("%s, %d, failed to allocate lcm_pinctrl_name\n",
                __func__, __LINE__);
            return -ENOMEM;
        }

        for (i = 0; i < len; i++) {
            params->lcm_pinctrl_name[i] = fdt_stringlist_get(fdt, nodeoffset,
                    "pinctrl-names", i, &ret);
            if (ret < 0 || params->lcm_pinctrl_name[i] == NULL) {
                LCM_MSG("%s, %d, failed to get lcm_pinctrl_name:%d, ret:%d\n",
                    __func__, __LINE__, i, ret);
                return ret;
            }
        }
        params->lcm_pinctrl_count = len;
    }
#endif
    lcm_fdt_getprop_u32(fdt, nodeoffset, "lcm-params-dsi-default_mode",
            &default_mode);
    lcm_fdt_getprop_u32(fdt, nodeoffset, "lcm-params-dsi-mode_count",
            &mode_count);
    if (mode_count == 0 ||
        default_mode >= mode_count) {
        LCM_MSG("%s,%d: invalid default mode:%u, count:%u\n",
            __func__, __LINE__, default_mode, mode_count);
        return -EFAULT;
    }

    LCM_MALLOC(mode_list, mode_count * MTK_LCM_MODE_UNIT * sizeof(u32));
    if (mode_list == NULL) {
        LCM_MSG("%s, %d, failed to allocate mode_list\n",
            __func__, __LINE__);
        return -ENOMEM;
    }

    len = lcm_fdt_getprop_u32_array(fdt, nodeoffset,
            "lcm-params-dsi-mode_list",
            (u32 *)mode_list, mode_count * MTK_LCM_MODE_UNIT);
    if ((unsigned int)len != mode_count * MTK_LCM_MODE_UNIT) {
        LCM_MSG("%s,%d: failed to to parse fps mode, len:%d, count:%u\n",
            __func__, __LINE__, len, mode_count);
        LCM_FREE(mode_list, sizeof(mode_list));
        return -EFAULT;
    }

    for (i = 0; i < mode_count; i++) {
        if (mode_list[i * MTK_LCM_MODE_UNIT] == default_mode) {
            id = mode_list[i * MTK_LCM_MODE_UNIT];
            width = mode_list[i * MTK_LCM_MODE_UNIT + 1];
            height = mode_list[i * MTK_LCM_MODE_UNIT + 2];
            fps = mode_list[i * MTK_LCM_MODE_UNIT + 3];
            break;
        }
    }
    LCM_FREE(mode_list, sizeof(mode_list));
    len = snprintf(mode_key, sizeof(mode_key) - 1,
         "%u-%u-%u-%u", id, width, height, fps);
    if (len < 0 || (size_t)len >= sizeof(mode_key))
        LCM_MSG("%s, snprintf failed, ret:%d\n", __func__, len);
    len = snprintf(mode_node, sizeof(mode_node) - 1,
         "lcm-params-dsi-fps-%s", mode_key);
    if (len < 0 || (size_t)len >= sizeof(mode_node))
        LCM_MSG("%s, snprintf failed, ret:%d\n", __func__, len);
    mode_offset = fdt_subnode_offset(fdt, nodeoffset, mode_node);
    if (mode_offset < 0) {
        LCM_MSG("%s: invalid %s, %d\n", __func__, mode_node, mode_offset);
        return -EFAULT;
    }

    parse_lcm_dsi_fps_setting(fdt, mode_offset,
            phy_type, params, mode_key, misc);

    return 0;
}

int parse_lcm_ops_dsi(const void *fdt, int nodeoffset,
        struct mtk_lcm_ops_dsi *ops,
        LCM_DSI_PARAMS *params,
        struct mtk_panel_cust *cust)
{
    int len = 0, ret = 0;

    if (fdt == NULL || params == NULL) {
        LCM_MSG("%s:%d, ERROR: invalid dsi params\n",
            __FILE__, __LINE__);
        return -EINVAL;
    }

    memset(ops, 0, sizeof(struct mtk_lcm_ops_dsi));
    ret = parse_lcm_ops_func(fdt, nodeoffset,
                    &ops->prepare, "prepare_table",
                    MTK_LCM_FUNC_DSI, cust,
#ifdef LCM_SET_DISPLAY_ON_DELAY
                    MTK_LCM_PHASE_LK_DISPLAY_ON_DELAY |
#endif
                    MTK_LCM_PHASE_LK);
    if (ret < 0) {
        LCM_MSG("%s, %d failed to parsing prepare_table\n",
            __func__, __LINE__);
        return ret;
    }

    ret = parse_lcm_ops_func(fdt, nodeoffset,
                    &ops->unprepare, "unprepare_table",
                    MTK_LCM_FUNC_DSI, cust,
                    MTK_LCM_PHASE_LK);
    if (ret < 0) {
        LCM_MSG("%s, %d failed to parsing unprepare_table\n",
           __func__, __LINE__);
        return ret;
    }

    ret = parse_lcm_ops_func(fdt, nodeoffset,
                    &ops->set_display_on, "set_display_on_table",
                    MTK_LCM_FUNC_DSI, cust,
#ifdef LCM_SET_DISPLAY_ON_DELAY
                    MTK_LCM_PHASE_LK_DISPLAY_ON_DELAY |
#endif
                    MTK_LCM_PHASE_LK);
    if (ret < 0) {
        LCM_MSG("%s, %d failed to parsing set_display_on_table\n",
            __func__, __LINE__);
        return ret;
    }

    lcm_fdt_getprop_u32(fdt, nodeoffset, "set_backlight_mask",
                &ops->set_backlight_mask);

    ret = parse_lcm_ops_func(fdt, nodeoffset,
                    &ops->set_backlight_cmdq,
                    "set_backlight_cmdq_table",
                    MTK_LCM_FUNC_DSI, cust,
                    MTK_LCM_PHASE_LK);
     if (ret < 0) {
            LCM_MSG("%s, %d failed to parsing set_backlight_cmd_table\n",
                __func__, __LINE__);
        return ret;
    }

    ret = parse_lcm_ops_func(fdt, nodeoffset,
                    &ops->lcm_update, "lcm_update_table",
                    MTK_LCM_FUNC_DSI, cust,
                    MTK_LCM_PHASE_LK);
     if (ret < 0) {
         LCM_MSG("%s, %d failed to parsing lcm_update_table\n",
                __func__, __LINE__);
        return ret;
    }

    LCM_MALLOC(ops->compare_id_value_data,
               MTK_PANEL_COMPARE_ID_LENGTH);
    if (ops->compare_id_value_data == NULL) {
        LCM_MSG("%s,%d: failed to allocate compare id data\n",
            __func__, __LINE__);
        return -ENOMEM;
    }
    len = lcm_fdt_getprop_u8_array_safe(fdt, nodeoffset,
                "compare_id_value_data",
                ops->compare_id_value_data,
                MTK_PANEL_COMPARE_ID_LENGTH);
    if (len > 0 &&
        len < MTK_PANEL_COMPARE_ID_LENGTH) {
        ops->compare_id_value_length = len;

        ret = parse_lcm_ops_func(fdt, nodeoffset,
                        &ops->compare_id, "compare_id_table",
                        MTK_LCM_FUNC_DSI, cust,
                        MTK_LCM_PHASE_LK);
        if (ret < 0) {
            LCM_MSG("%s, %d failed to parsing compare_id_table\n",
                    __func__, __LINE__);
            return ret;
        }
    } else {
        ops->compare_id_value_length = 0;
        LCM_MSG("%s, %d, failed to get compare id, len:%d\n",
            __func__, __LINE__, len);
    }

    return ret;
}

/* dump dsi dsc mode settings */
static void dump_lcm_dsi_dsc_mode(LCM_DSC_CONFIG_PARAMS *dsc_params)
{
    LCM_MSG("-------------  dsi dsc_params -------------\n");
    LCM_MSG("enable:1, ver:%u, slice:%u, rgb_swap:%u, cfg:%u\n",
        dsc_params->ver,
        dsc_params->slice_mode,
        dsc_params->rgb_swap,
        dsc_params->dsc_cfg);
    LCM_MSG("rct_on:%u, bit_ch:%u, depth:%u, bp_enable:%u, bit_pixel:%u\n",
        dsc_params->rct_on,
        dsc_params->bit_per_channel,
        dsc_params->dsc_line_buf_depth,
        dsc_params->bp_enable,
        dsc_params->bit_per_pixel);
    LCM_MSG("pic_h=%u, pic_w=%u, slice_h=%u, slice_w=%u, size=%u\n",
        dsc_params->pic_height,
        dsc_params->pic_width,
        dsc_params->slice_height,
        dsc_params->slice_width,
        dsc_params->chunk_size);
    LCM_MSG("xmi_delay=%u, dec_delay=%u, scale=%u, inc=%u, dec=%u\n",
        dsc_params->xmit_delay,
        dsc_params->dec_delay,
        dsc_params->scale_value,
        dsc_params->increment_interval,
        dsc_params->decrement_interval);
    LCM_MSG("offset:(line=%u, nfl=%u, slice=%u, init=%u, final=%u)\n",
        dsc_params->line_bpg_offset,
        dsc_params->nfl_bpg_offset,
        dsc_params->slice_bpg_offset,
        dsc_params->initial_offset,
        dsc_params->final_offset);
    LCM_MSG("flatness:(min=%u, max=%u), rc:(size=%u, edge=%u)\n",
        dsc_params->flatness_minqp,
        dsc_params->flatness_maxqp,
        dsc_params->rc_model_size,
        dsc_params->rc_edge_factor);
    LCM_MSG("quant:(limit0=%u, limit1=%u), tgt:(hi=%u, lo=%u)\n",
        dsc_params->rc_quant_incr_limit0,
        dsc_params->rc_quant_incr_limit1,
        dsc_params->rc_tgt_offset_hi,
        dsc_params->rc_tgt_offset_lo);
}

/* dump dsi phy_timcon settings */
static void dump_lcm_dsi_phy_timcon(LCM_DSI_PARAMS *params)
{
    LCM_MSG("-------------  dsi phy_timcon -------------\n");
    LCM_MSG("hs:(trial=0x%x, prpr=0x%x, zero=0x%x)\n",
        params->HS_TRAIL,
        params->HS_PRPR,
        params->HS_ZERO);
    LCM_MSG("lpx=0x%x, ta:(get=0x%x, sure=0x%x, go=0x%x)\n",
        params->LPX,
        params->TA_GET,
        params->TA_SURE,
        params->TA_GO);
    LCM_MSG("da_hs_exit=0x%x, lpx_wait=0x%x, cont_det=0x%x\n",
        params->DA_HS_EXIT,
        params->LPX_WAIT,
        params->CONT_DET);
    LCM_MSG("clk:(trail=0x%x, zero=0x%x, prpr=0x%x, exit=0x%x, post=0x%x)\n",
        params->CLK_TRAIL,
        params->CLK_ZERO,
        params->CLK_HS_PRPR,
        params->CLK_HS_EXIT,
        params->CLK_HS_POST);
}

/* dump dsi dyn settings */
static void dump_lcm_dsi_dyn(LCM_DSI_PARAMS *params)
{
    if (params->dynamic_switch_mipi == 0) {
        LCM_MSG("%s: dynamic switch is disabled\n", __func__);
        return;
    }
    LCM_MSG("-------------  dsi params -------------\n");
    LCM_MSG("enable=%u, pll_clk=%u, data_rate=%u\n",
        params->dynamic_switch_mipi,
        params->PLL_CLOCK_dyn,
        params->data_rate_dyn);
    LCM_MSG("vertical:(vsa=%u, vbp=%u, vfp=%u, vact=%u)\n",
        params->vertical_sync_active_dyn,
        params->vertical_backporch_dyn,
        params->vertical_frontporch_dyn,
        params->vertical_active_line_dyn);
    LCM_MSG("horizontal:(hsa=%u, hbp=%u, hfp=%u, hact=%u)\n",
        params->horizontal_sync_active_dyn,
        params->horizontal_backporch_dyn,
        params->horizontal_frontporch_dyn,
        params->horizontal_active_pixel_dyn);
}

/* dump dsi fps mode settings */
static void dump_lcm_dsi_fps_mode(LCM_DSI_PARAMS *params)
{
    LCM_MSG("-------------  fps mode -------------\n");
    LCM_MSG("vertical:(vsa=%u, vbp=%u, vfp=%u, vfp_lp=%u, vact=%u)\n",
        params->vertical_sync_active,
        params->vertical_backporch,
        params->vertical_frontporch,
        params->vertical_frontporch_for_low_power,
        params->vertical_active_line);
    LCM_MSG("horizontal:(hsa=%u, hbp=%u, hfp=%u, hact=%u)\n",
        params->horizontal_sync_active,
        params->horizontal_backporch,
        params->horizontal_frontporch,
        params->horizontal_active_pixel);
    LCM_MSG("horizontal byte:(hsa=%u, hbp=%u, hfp=%u)\n",
        params->horizontal_sync_active_byte,
        params->horizontal_backporch_byte,
        params->horizontal_frontporch_byte);
    LCM_MSG("horizontal word:(hsa=%u, hbp=%u, hfp=%u)\n",
        params->horizontal_sync_active_word_count,
        params->horizontal_backporch_word_count,
        params->horizontal_frontporch_word_count);
    LCM_MSG("hblank=%u, hbllp=%d\n",
        params->horizontal_blanking_pixel,
        params->horizontal_bllp);
}

/* dump dsi fps params */
static void dump_lcm_dsi_fps_params(LCM_DSI_PARAMS *params)
{
    unsigned int i = 0;

    LCM_MSG("-------------  fps params -------------\n");
    LCM_MSG("pll_clk=%u, data_rate=%u, ssc:(dis=%u, range=%u)\n",
        params->PLL_CLOCK,
        params->data_rate,
        params->ssc_disable,
        params->ssc_range);
    LCM_MSG("is_cphy=%u, lane_swap_en=%u, lane_swap:\n",
        params->IsCphy,
        params->lane_swap_en);

    /* lane swap */
    if (params->lane_swap_en != 0) {
        for (i = 0; i < MIPITX_PHY_PORT_NUM; i++) {
            LCM_MSG("lane_swap0~5(%u, %u, %u, %u, %u,%u)\n",
                params->lane_swap[i][MIPITX_PHY_LANE_0],
                params->lane_swap[i][MIPITX_PHY_LANE_1],
                params->lane_swap[i][MIPITX_PHY_LANE_2],
                params->lane_swap[i][MIPITX_PHY_LANE_3],
                params->lane_swap[i][MIPITX_PHY_LANE_CK],
                params->lane_swap[i][MIPITX_PHY_LANE_RX]);
        }
    }
}

static void dump_lcm_dsi_lk_params(LCM_DSI_PARAMS *params)
{
    unsigned int i = 0, j = 0;

    if (params == NULL)
        return;

    LCM_MSG("-------------  lk params -------------\n");
    LCM_MSG("mode=%u, switch_mode=%u, switch_enable=%u\n",
        params->mode, params->switch_mode,
        params->switch_mode_enable);
    LCM_MSG("WMEM=%u, RMEM=%u, VC_NUM=%u, LANE_NUM=%u\n",
        params->DSI_WMEM_CONTI, params->DSI_RMEM_CONTI,
        params->VC_NUM, params->LANE_NUM);
    LCM_MSG("color_order=%u, trans_seq=%u, padding=%u, format=%u\n",
        params->data_format.color_order,
        params->data_format.trans_seq,
        params->data_format.padding,
        params->data_format.format);
    LCM_MSG("int_buf_num=%u, ps=%u, word_count=%u, packt_size=%u\n",
        params->intermediat_buffer_num,
        params->PS,
        params->word_count,
        params->packet_size);
    LCM_MSG("pll_select=%u, div1=%u, div2=%u, fbk=(%u-%u)\n",
        params->pll_select,
        params->pll_div1, params->pll_div2,
        params->fbk_div, params->fbk_sel);
    LCM_MSG("rg=(bir:%u, bic:%u, bp:%u)\n",
        params->rg_bir, params->rg_bic,
        params->rg_bp);
    LCM_MSG("pll=%u, data_rate=%u, dsi_clock=%u)\n",
        params->PLL_CLOCK, params->data_rate,
        params->dsi_clock);
    LCM_MSG("ssc=(disable:%u, range:%u), comp_nvk=%u, cont_clock=%u\n",
        params->ssc_disable, params->ssc_range,
        params->compatibility_for_nvk,
        params->cont_clock);
    LCM_MSG("ufoe_en=%u, ufoe=(comp_ratio:%u, lr_mode_en=%u, vlc=(%u, %u)\n",
        params->ufoe_enable,
        params->ufoe_params.compress_ratio,
        params->ufoe_params.lr_mode_en,
        params->ufoe_params.vlc_disable,
        params->ufoe_params.vlc_config);
    LCM_MSG("edp=%u, lcm_int_te=(%u, %u), lcm_ext=(%u, %u)\n",
        params->edp_panel,
        params->lcm_int_te_monitor,
        params->lcm_int_te_period,
        params->lcm_ext_te_monitor,
        params->lcm_ext_te_enable);
    LCM_MSG("noncont_clk=(%u, %u), clk_lp_per_line_enable=%u, dual_dsi=%u\n",
        params->noncont_clock,
        params->noncont_clock_period,
        params->clk_lp_per_line_enable,
        params->dual_dsi_type);

    LCM_MSG("lane_swap_en=%u, lane_swap:\n",
        params->lane_swap_en);
    if (params->lane_swap_en != 0) {
        for (i = 0; i < MIPITX_PHY_PORT_NUM; i++) {
            for (j = 0; j < MIPITX_PHY_PORT_NUM; j++)
                LCM_MSG("%d ", params->lane_swap[i][j]);
            LCM_MSG("\n");
        }
    }

    LCM_MSG("pll_clock_lp=%u, ulps_sw_enable=%u, null_packet_en=%u\n",
        params->PLL_CLOCK_lp,
        params->ulps_sw_enable,
        params->null_packet_en);

    LCM_MSG("mixmode=(%u,%u), pwm_fps=%u, fps=%u, vact_fps=%u\n",
        params->mixmode_enable,
        params->mixmode_mipi_clock,
        params->pwm_fps, params->fps, params->vact_fps);

    LCM_MSG("send_frame_enable=%u, lfr=(%u, %u, %u, %u)\n",
        params->send_frame_enable,
        params->lfr_enable, params->lfr_mode,
        params->lfr_type, params->lfr_skip_num);

    LCM_MSG("ext_te_edge=%u, eint_enable=%u, PHY_SEL=(%u, %u, %u, %u)\n",
        params->ext_te_edge,
        params->eint_disable,
        params->PHY_SEL0, params->PHY_SEL1,
        params->PHY_SEL2, params->PHY_SEL3);
}

/* dump dsi fps settings*/
static void dump_lcm_dsi_fps_settings(LCM_DSI_PARAMS *params)
{
    if (params == NULL) {
        LCM_MSG("%s, %d, invalid params\n",
            __func__, __LINE__);
        return;
    }

    LCM_MSG("-------------- LCM DUMP FPS%u -------------\n",
        params->fps);
    if (params->fps == 0)
        return;

    dump_lcm_dsi_fps_mode(params);
    dump_lcm_dsi_fps_params(params);
    if (params->dsc_enable != 0)
        dump_lcm_dsi_dsc_mode(&params->dsc_params);
    else
        LCM_MSG("dsc is disabled\n");

    dump_lcm_dsi_phy_timcon(params);
    dump_lcm_dsi_dyn(params);
}

/* dump dsi settings*/
void dump_lcm_params_dsi(LCM_DSI_PARAMS *params,
    struct mtk_panel_cust *cust)
{
    if (params == NULL) {
        LCM_MSG("%s, %d: invalid params\n", __func__, __LINE__);
        return;
    }
    LCM_MSG("=========== LCM DSI DUMP =============\n");
    dump_lcm_dsi_lk_params(params);
    dump_lcm_dsi_fps_settings(params);

    if (cust != NULL &&
        cust->cust_enabled == 1 &&
        cust->dump_params != NULL) {
        LCM_MSG("---------------- LCM CUSTOMIZATION DUMP -------------\n");
        cust->dump_params();
    }

    LCM_MSG("======================================\n");
}

void dump_lcm_ops_dsi(struct mtk_lcm_ops_dsi *ops,
        LCM_DSI_PARAMS *params,
        struct mtk_panel_cust *cust)
{
    unsigned int i = 0;

    if (ops == NULL) {
        LCM_MSG("%s, %d: invalid params ops\n", __func__, __LINE__);
        return;
    }
    if (params == NULL) {
        LCM_MSG("%s, %d: invalid params ops\n", __func__, __LINE__);
        return;
    }

    LCM_MSG("================ DSI ops:0x%lx-0x%lx ===============\n",
        (unsigned long)ops, (unsigned long)params);
    dump_lcm_ops_func(&ops->prepare, cust, "prepare");
    dump_lcm_ops_func(&ops->unprepare, cust, "unprepare");
    dump_lcm_ops_func(&ops->set_backlight_cmdq, cust, "set_backlight");
    dump_lcm_ops_func(&ops->set_display_on, cust, "set_display_on");
    dump_lcm_ops_func(&ops->lcm_update, cust, "lcm_update");
    dump_lcm_ops_func(&ops->compare_id, cust, "compare_id");
    LCM_MSG(">>> compare_id_value_length=%u, data:\n",
        ops->compare_id_value_length);
    for (i = 0; i < ops->compare_id_value_length; i++)
        LCM_MSG(">>> compare_id_value[%u]= %u\n", i, ops->compare_id_value_data[i]);

    LCM_MSG("=======================================================\n");
}

void free_lcm_params_dsi(LCM_DSI_PARAMS *params)
{
    if (params == NULL) {
        LCM_DBG("%s:%d, ERROR: invalid params/ops\n",
            __FILE__, __LINE__);
        return;
    }
    memset(params, 0, sizeof(LCM_DSI_PARAMS));
}

void free_lcm_ops_dsi(struct mtk_lcm_ops_dsi *ops)
{
    if (ops == NULL) {
        LCM_DBG("%s:%d, ERROR: invalid params/ops\n",
            __FILE__, __LINE__);
        return;
    }

    if (ops->compare_id_value_length > 0 &&
        ops->compare_id_value_data != NULL) {
        LCM_FREE(ops->compare_id_value_data,
            ops->compare_id_value_length);
        ops->compare_id_value_length = 0;
    }

    free_lcm_ops_table(&ops->prepare, "prepare");
    free_lcm_ops_table(&ops->unprepare, "unprepare");
    free_lcm_ops_table(&ops->compare_id, "compare_id");
    free_lcm_ops_table(&ops->set_backlight_cmdq, "set_backlight_cmdq");
    free_lcm_ops_table(&ops->set_display_on, "set_display_on");
    free_lcm_ops_table(&ops->lcm_update, "lcm_update");

    LCM_FREE(ops, sizeof(struct mtk_lcm_ops_dsi));
}
