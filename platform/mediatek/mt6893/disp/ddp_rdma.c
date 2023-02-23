/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#define LOG_TAG "RDMA"

#include <stdbool.h>
#include "ddp_info.h"
#include "ddp_matrix_para.h"
#include "ddp_reg.h"
#include "ddp_log.h"
#include "ddp_rdma.h"
#include "ddp_dump.h"
//#include "platform/mt_clkmgr.h"
//#include "disp_drv_platform.h"
//#include "primary_display.h"


enum RDMA_INPUT_FORMAT {
    RDMA_INPUT_FORMAT_BGR565    = 0,
    RDMA_INPUT_FORMAT_RGB888    = 1,
    RDMA_INPUT_FORMAT_RGBA8888  = 2,
    RDMA_INPUT_FORMAT_ARGB8888  = 3,
    RDMA_INPUT_FORMAT_VYUY      = 4,
    RDMA_INPUT_FORMAT_YVYU      = 5,

    RDMA_INPUT_FORMAT_RGB565    = 6,
    RDMA_INPUT_FORMAT_BGR888    = 7,
    RDMA_INPUT_FORMAT_BGRA8888  = 8,
    RDMA_INPUT_FORMAT_ABGR8888  = 9,
    RDMA_INPUT_FORMAT_UYVY      = 10,
    RDMA_INPUT_FORMAT_YUYV      = 11,

    RDMA_INPUT_FORMAT_UNKNOWN    = 32,
};

static enum RDMA_INPUT_FORMAT rdma_input_format_convert(enum DpColorFormat  fmt);
static unsigned int rdma_input_format_byte_swap(enum RDMA_INPUT_FORMAT inputFormat);
static unsigned int rdma_input_format_bpp(enum RDMA_INPUT_FORMAT inputFormat);
static unsigned int rdma_input_format_color_space(enum RDMA_INPUT_FORMAT inputFormat);
static unsigned int rdma_input_format_reg_value(enum RDMA_INPUT_FORMAT inputFormat);
static const char *rdma_intput_format_name(int reg);

static unsigned int rdma_index(enum DISP_MODULE_ENUM module)
{
    int idx = 0;

    switch (module) {
    case DISP_MODULE_RDMA0:
        idx = 0;
        break;
    case DISP_MODULE_RDMA1:
        idx = 1;
        break;
    default:
        DDPERR("invalid rdma module=%d\n", module);// invalid module
        ASSERT(0);
    }
    return idx;
}

int RDMAStart(enum DISP_MODULE_ENUM module, void *handle)
{
    unsigned int idx = rdma_index(module);
    unsigned int regval;

    ASSERT(idx <= RDMA_INSTANCES);

    regval = REG_FLD_VAL(INT_STATUS_FLD_REG_UPDATE_INT_FLAG, 0) |
        REG_FLD_VAL(INT_STATUS_FLD_FRAME_START_INT_FLAG, 1) |
        REG_FLD_VAL(INT_STATUS_FLD_FRAME_END_INT_FLAG, 1) |
        REG_FLD_VAL(INT_STATUS_FLD_EOF_ABNORMAL_INT_FLAG, 1) |
        REG_FLD_VAL(INT_STATUS_FLD_FIFO_UNDERFLOW_INT_FLAG, 1) |
        REG_FLD_VAL(INT_STATUS_FLD_TARGET_LINE_INT_FLAG, 0) |
        REG_FLD_VAL(INT_STATUS_FLD_FIFO_EMPTY_INT_FLAG, 0);

    DISP_REG_SET(handle, idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_INT_ENABLE, regval);
    DISP_REG_SET_FIELD(handle, GLOBAL_CON_FLD_ENGINE_EN,
               idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_GLOBAL_CON, 1);
    return 0;
}

int RDMAStop(enum DISP_MODULE_ENUM module, void *handle)
{
    unsigned int idx = rdma_index(module);

    ASSERT(idx <= RDMA_INSTANCES);

    DISP_REG_SET_FIELD(handle, GLOBAL_CON_FLD_ENGINE_EN,
                        idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_GLOBAL_CON, 0);
    DISP_REG_SET(handle, idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_INT_ENABLE, 0);
    DISP_REG_SET(handle, idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_INT_STATUS, 0);
    return 0;
}

int RDMAReset(enum DISP_MODULE_ENUM module, void *handle)
{
    unsigned int delay_cnt = 0;
    unsigned int idx = rdma_index(module);

    ASSERT(idx <= RDMA_INSTANCES);

    DISP_REG_SET_FIELD(handle, GLOBAL_CON_FLD_SOFT_RESET,
                        idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_GLOBAL_CON, 1);
    while ((DISP_REG_GET(idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_GLOBAL_CON)
            & 0x700) == 0x100) { //polling RESET_STATE is 0
        delay_cnt++;
        if (delay_cnt > 10000) {
            DDPERR("%s(%d) timeout, stage 1! DISP_REG_RDMA_GLOBAL_CON=0x%x\n", __func__, idx,
                    DISP_REG_GET(idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_GLOBAL_CON));
            break;
        }
    }
    DISP_REG_SET_FIELD(handle, GLOBAL_CON_FLD_SOFT_RESET,
                        idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_GLOBAL_CON, 0);
    delay_cnt = 0;
    while ((DISP_REG_GET(idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_GLOBAL_CON) & 0x700)
            != 0x100) {
        delay_cnt++;
        if (delay_cnt > 10000) {
            DDPERR("%s(%d) timeout, stage 2! DISP_REG_RDMA_GLOBAL_CON=0x%x\n", __func__,
                idx, DISP_REG_GET(idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_GLOBAL_CON));
            break;
        }
    }

    return 0;
}

void rdma_cal_golden_setting(unsigned int idx, unsigned int width,
    unsigned int height, unsigned int Bpp, unsigned int *gs,
    unsigned int is_vdo)
{
    /* fixed variable */
    unsigned int mmsys_clk = 208;
    unsigned int pre_ultra_low_us = 245, pre_ultra_high_us = 255;
    unsigned int ultra_low_us = 230, ultra_high_us = 245;
    unsigned int urgent_low_us = 113, urgent_high_us = 117;
    unsigned int if_fps = 60;
    unsigned int FP = 1000;
    unsigned int fifo_size = 2240;

    /* input variable */
    bool is_dc = 0;

    unsigned int fill_rate = 0; /* 100 times */
    unsigned int consume_rate = 0; /* 100 times */

    /* critical variable calc */
    if (is_dc)
        fill_rate = 96 * mmsys_clk; /* FIFO depth / us */
    else
        fill_rate = 96 * mmsys_clk * 3 / 16; /* FIFO depth / us */


#if 0
    consume_rate = width * height * if_fps * Bpp;
    do_div(consume_rate, 1000);
    consume_rate *= 125;
    do_div(consume_rate, 16 * 1000);
#else
    consume_rate = width * height * if_fps;
    consume_rate /= 1000;
    consume_rate *= (Bpp * 125);
    consume_rate /= (16 * 1000);
#endif

    /* RDMA golden setting calculation */
    /* DISP_RDMA_MEM_GMC_SETTING_0 */
    gs[GS_RDMA_PRE_ULTRA_TH_LOW] =
        consume_rate * (pre_ultra_low_us) / FP;
    gs[GS_RDMA_PRE_ULTRA_TH_HIGH] =
        consume_rate * (pre_ultra_high_us) / FP;
    if (is_vdo) {
        gs[GS_RDMA_VALID_TH_FORCE_PRE_ULTRA] = 0;
        gs[GS_RDMA_VDE_FORCE_PRE_ULTRA] = 1;
    } else {
        gs[GS_RDMA_VALID_TH_FORCE_PRE_ULTRA] = 1;
        gs[GS_RDMA_VDE_FORCE_PRE_ULTRA] = 0;
    }

    /* DISP_RDMA_MEM_GMC_SETTING_1 */
    gs[GS_RDMA_ULTRA_TH_LOW] =
        consume_rate * (ultra_low_us) / FP;
    gs[GS_RDMA_ULTRA_TH_HIGH] = gs[GS_RDMA_PRE_ULTRA_TH_LOW];
    if (is_vdo) {
        gs[GS_RDMA_VALID_TH_BLOCK_ULTRA] = 0;
        gs[GS_RDMA_VDE_BLOCK_ULTRA] = 1;
    } else {
        gs[GS_RDMA_VALID_TH_BLOCK_ULTRA] = 1;
        gs[GS_RDMA_VDE_BLOCK_ULTRA] = 0;
    }

    /* DISP_RDMA_FIFO_CON */
    if (is_vdo)
        gs[GS_RDMA_OUTPUT_VALID_FIFO_TH] = 0;
    else
        gs[GS_RDMA_OUTPUT_VALID_FIFO_TH] =
            gs[GS_RDMA_PRE_ULTRA_TH_LOW];
    gs[GS_RDMA_FIFO_SIZE] = fifo_size;
    gs[GS_RDMA_FIFO_UNDERFLOW_EN] = 1;

    /* DISP_RDMA_MEM_GMC_SETTING_2 */
    /* do not min this value with 256 to avoid hrt fail in
     * dc mode under SODI CG mode
     */
    gs[GS_RDMA_ISSUE_REQ_TH] =
        gs[GS_RDMA_FIFO_SIZE] - gs[GS_RDMA_PRE_ULTRA_TH_LOW];

    /* DISP_RDMA_THRESHOLD_FOR_SODI */
    gs[GS_RDMA_TH_LOW_FOR_SODI] =
        consume_rate * (ultra_low_us + 50) / FP;
    gs[GS_RDMA_TH_HIGH_FOR_SODI] = (gs[GS_RDMA_FIFO_SIZE] *
        FP - (fill_rate - consume_rate) * 12) / FP;
    if (gs[GS_RDMA_TH_HIGH_FOR_SODI] < gs[GS_RDMA_PRE_ULTRA_TH_HIGH])
        gs[GS_RDMA_TH_HIGH_FOR_SODI] = gs[GS_RDMA_PRE_ULTRA_TH_HIGH];

    /* DISP_RDMA_THRESHOLD_FOR_DVFS */
    gs[GS_RDMA_TH_LOW_FOR_DVFS] = gs[GS_RDMA_PRE_ULTRA_TH_LOW];
    gs[GS_RDMA_TH_HIGH_FOR_DVFS] = gs[GS_RDMA_PRE_ULTRA_TH_LOW] + 1;

    /* DISP_RDMA_SRAM_SEL */
    gs[GS_RDMA_SRAM_SEL] = 0;

    /* DISP_RDMA_DVFS_SETTING_PREULTRA */
    gs[GS_RDMA_DVFS_PRE_ULTRA_TH_LOW] =
        consume_rate * (pre_ultra_low_us + 40) / FP;
    gs[GS_RDMA_DVFS_PRE_ULTRA_TH_HIGH] =
        consume_rate * (pre_ultra_high_us + 40) / FP;

    /* DISP_RDMA_DVFS_SETTING_ULTRA */
    gs[GS_RDMA_DVFS_ULTRA_TH_LOW] =
        consume_rate * (ultra_low_us + 40) / FP;
    gs[GS_RDMA_DVFS_ULTRA_TH_HIGH] = gs[GS_RDMA_DVFS_PRE_ULTRA_TH_LOW];

    /* DISP_RDMA_LEAVE_DRS_SETTING */
    gs[GS_RDMA_IS_DRS_STATUS_TH_LOW] =
        consume_rate * (pre_ultra_low_us + 20) / FP;
    gs[GS_RDMA_IS_DRS_STATUS_TH_HIGH] =
        consume_rate * (pre_ultra_low_us + 20) / FP;

    /* DISP_RDMA_ENTER_DRS_SETTING */
    gs[GS_RDMA_NOT_DRS_STATUS_TH_LOW] =
        consume_rate * (ultra_high_us + 40) / FP;
    gs[GS_RDMA_NOT_DRS_STATUS_TH_HIGH] =
        consume_rate * (ultra_high_us + 40) / FP;

    /* DISP_RDMA_MEM_GMC_SETTING_3 */
    gs[GS_RDMA_URGENT_TH_LOW] =
        consume_rate * urgent_low_us / FP;
    gs[GS_RDMA_URGENT_TH_HIGH] =
        consume_rate * urgent_high_us / FP;

    /* DISP_RDMA_GREQ_URG_NUM_SEL */
    gs[GS_RDMA_LAYER_SMI_ID_EN] = 1;
}
void rdma_set_ultra(unsigned int idx, unsigned int width, unsigned int height,
    unsigned int bpp, unsigned int frame_rate, unsigned int dsi_mode,
    void *handle)
{
    unsigned int gs[GS_RDMA_FLD_NUM] = {0};
    unsigned int val = 0;
    unsigned int offset = idx * DISP_RDMA_INDEX_OFFSET;

    if (idx == 1) {
        DDPMSG("RDMA1 golden setting not support yet\n");
        return;
    }

    /* calculate golden setting
     * force use vdo mode setting here
     */
    rdma_cal_golden_setting(idx, width, height, bpp, gs, (dsi_mode > 0));

    /* set golden setting */
    val = gs[GS_RDMA_PRE_ULTRA_TH_LOW] +
        (gs[GS_RDMA_PRE_ULTRA_TH_HIGH] << 16) +
        (gs[GS_RDMA_VALID_TH_FORCE_PRE_ULTRA] << 30) +
        (gs[GS_RDMA_VDE_FORCE_PRE_ULTRA] << 31);
    DISP_REG_SET(handle, offset + DISP_REG_RDMA_MEM_GMC_SETTING_0, val);

    val = gs[GS_RDMA_ULTRA_TH_LOW] +
        (gs[GS_RDMA_ULTRA_TH_HIGH] << 16) +
        (gs[GS_RDMA_VALID_TH_BLOCK_ULTRA] << 30) +
        (gs[GS_RDMA_VDE_BLOCK_ULTRA] << 31);
    DISP_REG_SET(handle, offset + DISP_REG_RDMA_MEM_GMC_SETTING_1, val);

    val = gs[GS_RDMA_ISSUE_REQ_TH];
    DISP_REG_SET(handle, offset + DISP_REG_RDMA_MEM_GMC_SETTING_2, val);

    val = gs[GS_RDMA_OUTPUT_VALID_FIFO_TH] +
        (gs[GS_RDMA_FIFO_SIZE] << 16) +
        (gs[GS_RDMA_FIFO_UNDERFLOW_EN] << 31);
    DISP_REG_SET(handle, offset + DISP_REG_RDMA_FIFO_CON, val);

    val = gs[GS_RDMA_TH_LOW_FOR_SODI] +
        (gs[GS_RDMA_TH_HIGH_FOR_SODI] << 16);
    DISP_REG_SET(handle, offset + DISP_REG_RDMA_THRESHOLD_FOR_SODI, val);

    val = gs[GS_RDMA_TH_LOW_FOR_DVFS] +
        (gs[GS_RDMA_TH_HIGH_FOR_DVFS] << 16);
    DISP_REG_SET(handle, offset + DISP_REG_RDMA_THRESHOLD_FOR_DVFS, val);

    if (idx == 0)
        DISP_REG_SET(handle, DISP_REG_RDMA_SRAM_SEL,
            gs[GS_RDMA_SRAM_SEL]);


    val = gs[GS_RDMA_DVFS_PRE_ULTRA_TH_LOW] +
        (gs[GS_RDMA_DVFS_PRE_ULTRA_TH_HIGH] << 16);
    DISP_REG_SET(handle, offset + DISP_REG_RDMA_DVFS_SETTING_PRE, val);

    val = gs[GS_RDMA_DVFS_ULTRA_TH_LOW] +
        (gs[GS_RDMA_DVFS_ULTRA_TH_HIGH] << 16);
    DISP_REG_SET(handle, offset + DISP_REG_RDMA_DVFS_SETTING_ULTRA, val);

    val = gs[GS_RDMA_IS_DRS_STATUS_TH_LOW] +
        (gs[GS_RDMA_IS_DRS_STATUS_TH_HIGH] << 16);
    DISP_REG_SET(handle, offset + DISP_REG_RDMA_LEAVE_DRS_SETTING, val);

    val = gs[GS_RDMA_NOT_DRS_STATUS_TH_LOW] +
        (gs[GS_RDMA_NOT_DRS_STATUS_TH_HIGH] << 16);
    DISP_REG_SET(handle, offset + DISP_REG_RDMA_ENTER_DRS_SETTING, val);

    val = gs[GS_RDMA_URGENT_TH_LOW] +
        (gs[GS_RDMA_URGENT_TH_HIGH] << 16);
    DISP_REG_SET(handle, offset + DISP_REG_RDMA_MEM_GMC_SETTING_3, val);

    val = gs[GS_RDMA_LAYER_SMI_ID_EN];
    DISP_REG_SET_FIELD(handle, FLD_RG_LAYER_SMI_ID_EN,
            offset + DISP_REG_RDMA_GREQ_URG_NUM_SEL, val);
}

// fixme: spec has no RDMA format, fix enum definition here
int RDMAConfig(enum DISP_MODULE_ENUM module,
               enum RDMA_MODE mode,
               unsigned int address,
               enum DpColorFormat  inFormat,
               unsigned int pitch,
               unsigned int width,
               unsigned int height,
               unsigned int ufoe_enable,
               unsigned int dsi_mode,
               void *handle)  // ourput setting
{

    unsigned int output_is_yuv = 0;
    enum RDMA_INPUT_FORMAT inputFormat = rdma_input_format_convert(inFormat);
    unsigned int  bpp = rdma_input_format_bpp(inputFormat);
    unsigned int input_is_yuv = rdma_input_format_color_space(inputFormat);
    unsigned int input_swap = rdma_input_format_byte_swap(inputFormat);
    unsigned int input_format_reg = rdma_input_format_reg_value(inputFormat);
    unsigned int idx = rdma_index(module);

    //DDPDBG("%s idx %d, mode %d, addr 0x%x, in_fmt %d, in_swap %u, pitch %u, w %u, h %u\n",
    //           __func__, idx, mode, address, inputFormat, input_swap, pitch,width, height);
    ASSERT(idx <= RDMA_INSTANCES);
    if ((width > RDMA_MAX_WIDTH) || (height > RDMA_MAX_HEIGHT)) {
        DDPERR("RDMA input overflow, w=%d, h=%d, max_w=%d, max_h=%d\n", width, height,
            RDMA_MAX_WIDTH, RDMA_MAX_HEIGHT);
    }
    if (input_is_yuv == 1 && output_is_yuv == 0) {
        DISP_REG_SET_FIELD(handle, SIZE_CON_0_FLD_MATRIX_ENABLE,
                            idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_SIZE_CON_0, 1);
        DISP_REG_SET_FIELD(handle, SIZE_CON_0_FLD_MATRIX_INT_MTX_SEL,
                            idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_SIZE_CON_0, 0x6);
        // set color conversion matrix
        DISP_REG_SET(handle, idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_C00,
                        coef_rdma_601_y2r[0][0]);
        DISP_REG_SET(handle, idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_C01,
                        coef_rdma_601_y2r[0][1]);
        DISP_REG_SET(handle, idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_C02,
                        coef_rdma_601_y2r[0][2]);
        DISP_REG_SET(handle, idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_C10,
                    coef_rdma_601_y2r[1][0]);
        DISP_REG_SET(handle, idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_C11,
                    coef_rdma_601_y2r[1][1]);
        DISP_REG_SET(handle, idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_C12,
                    coef_rdma_601_y2r[1][2]);
        DISP_REG_SET(handle, idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_C20,
                    coef_rdma_601_y2r[2][0]);
        DISP_REG_SET(handle, idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_C21,
                    coef_rdma_601_y2r[2][1]);
        DISP_REG_SET(handle, idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_C22,
                    coef_rdma_601_y2r[2][2]);

        DISP_REG_SET(handle, idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_PRE_ADD_0,
                        coef_rdma_601_y2r[3][0]);
        DISP_REG_SET(handle, idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_PRE_ADD_1,
                        coef_rdma_601_y2r[3][1]);
        DISP_REG_SET(handle, idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_PRE_ADD_2,
                        coef_rdma_601_y2r[3][2]);
        DISP_REG_SET(handle, idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_POST_ADD_0,
                        coef_rdma_601_y2r[4][0]);
        DISP_REG_SET(handle, idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_POST_ADD_1,
                        coef_rdma_601_y2r[4][1]);
        DISP_REG_SET(handle, idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_POST_ADD_2,
                        coef_rdma_601_y2r[4][2]);
    } else if (input_is_yuv == 0 && output_is_yuv == 1) {
        DISP_REG_SET_FIELD(handle, SIZE_CON_0_FLD_MATRIX_ENABLE,
                            idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_SIZE_CON_0, 1);
        DISP_REG_SET_FIELD(handle, SIZE_CON_0_FLD_MATRIX_INT_MTX_SEL,
                            idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_SIZE_CON_0, 0x2);
        // set color conversion matrix
        DISP_REG_SET(handle, idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_C00,
                    coef_rdma_601_r2y[0][0]);
        DISP_REG_SET(handle, idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_C01,
                    coef_rdma_601_r2y[0][1]);
        DISP_REG_SET(handle, idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_C02,
                    coef_rdma_601_r2y[0][2]);
        DISP_REG_SET(handle, idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_C10,
                    coef_rdma_601_r2y[1][0]);
        DISP_REG_SET(handle, idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_C11,
                    coef_rdma_601_r2y[1][1]);
        DISP_REG_SET(handle, idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_C12,
                    coef_rdma_601_r2y[1][2]);
        DISP_REG_SET(handle, idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_C20,
                    coef_rdma_601_r2y[2][0]);
        DISP_REG_SET(handle, idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_C21,
                    coef_rdma_601_r2y[2][1]);
        DISP_REG_SET(handle, idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_C22,
                    coef_rdma_601_r2y[2][2]);

        DISP_REG_SET(handle, idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_PRE_ADD_0,
                    coef_rdma_601_r2y[3][0]);
        DISP_REG_SET(handle, idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_PRE_ADD_1,
                    coef_rdma_601_r2y[3][1]);
        DISP_REG_SET(handle, idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_PRE_ADD_2,
                    coef_rdma_601_r2y[3][2]);
        DISP_REG_SET(handle, idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_POST_ADD_0,
                    coef_rdma_601_r2y[4][0]);
        DISP_REG_SET(handle, idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_POST_ADD_1,
                    coef_rdma_601_r2y[4][1]);
        DISP_REG_SET(handle, idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_POST_ADD_2,
                    coef_rdma_601_r2y[4][2]);
    } else {
        DISP_REG_SET_FIELD(handle, SIZE_CON_0_FLD_MATRIX_ENABLE,
                            idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_SIZE_CON_0, 0);
        DISP_REG_SET_FIELD(handle, SIZE_CON_0_FLD_MATRIX_INT_MTX_SEL,
                            idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_SIZE_CON_0, 0);
    }

    DISP_REG_SET_FIELD(handle, GLOBAL_CON_FLD_MODE_SEL,
                        idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_GLOBAL_CON, mode);
    // FORMAT & SWAP only works when RDMA memory mode, set both to 0 when RDMA direct link mode.
    DISP_REG_SET_FIELD(handle, MEM_CON_FLD_MEM_MODE_INPUT_FORMAT,
                        idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_MEM_CON,
                        ((mode == RDMA_MODE_DIRECT_LINK) ? 0 : input_format_reg & 0xf));
    DISP_REG_SET_FIELD(handle, MEM_CON_FLD_MEM_MODE_INPUT_SWAP,
                        idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_MEM_CON,
                        ((mode == RDMA_MODE_DIRECT_LINK) ? 0 : input_swap));
    DISP_REG_SET(handle, idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_MEM_START_ADDR, address);
    DISP_REG_SET(handle, idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_MEM_SRC_PITCH, pitch);
    DISP_REG_SET(handle, idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_INT_ENABLE, 0x1F);
    DISP_REG_SET_FIELD(handle, SIZE_CON_0_FLD_OUTPUT_FRAME_WIDTH,
                        idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_SIZE_CON_0, width);
    DISP_REG_SET_FIELD(handle, SIZE_CON_1_FLD_OUTPUT_FRAME_HEIGHT,
                        idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_SIZE_CON_1, height);

    rdma_set_ultra(idx, width, height, bpp, 60, dsi_mode, handle);
#if 0
    if (1) { //UFOE bypassed, enable RDMA underflow intr, else disable RDMA underflow intr
        DISP_REG_SET_FIELD(handle, FIFO_CON_FLD_FIFO_UNDERFLOW_EN,
                            idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_FIFO_CON, 1);
        DISP_REG_SET_FIELD(handle, FIFO_CON_FLD_OUTPUT_VALID_FIFO_THRESHOLD,
                            idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_FIFO_CON, 16);
    } else {
        DISP_REG_SET_FIELD(handle, FIFO_CON_FLD_FIFO_UNDERFLOW_EN,
                            idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_FIFO_CON, 1);
        //FHD:304, HD:203, QHD:151
        DISP_REG_SET_FIELD(handle, FIFO_CON_FLD_OUTPUT_VALID_FIFO_THRESHOLD,
                            idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_FIFO_CON, width*3*3/16/2);
    }

    DISP_REG_SET_FIELD(handle, FIFO_CON_FLD_FIFO_PSEUDO_SIZE, idx * DISP_RDMA_INDEX_OFFSET
                        + DISP_REG_RDMA_FIFO_CON, 384);
#endif

    return 0;
}

int RDMAWait(enum DISP_MODULE_ENUM module, void *handle)
{
    unsigned int cnt = 0;
    unsigned int idx = rdma_index(module);
    // polling interrupt status
    while ((DISP_REG_GET(idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_INT_STATUS) & 0x4) != 0x4) {
        cnt++;
        if (cnt > 1000) {
            DDPERR("RDMA%dWait timeout!\n", idx);
            DISP_REG_SET(handle, idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_INT_STATUS, 0x0);
            return -1;
        }
    }
    DDPMSG(" RDMA%dWait cnt=%d\n", idx, cnt);
    DISP_REG_SET(handle, idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_INT_STATUS, 0x0);

    return 0;
}

void RDMASetTargetLine(enum DISP_MODULE_ENUM module, unsigned int line, void *handle)
{
    unsigned int idx = rdma_index(module);

    DISP_REG_SET(handle, idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_TARGET_LINE, line);
}

static int RDMAPollingInterrupt(enum DISP_MODULE_ENUM module, int bit, int timeout)
{
    unsigned int idx = rdma_index(module);
    unsigned int cnt = 0;
    unsigned int regval = 0;

    if (timeout <= 0) {
        while ((DISP_REG_GET(idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_INT_STATUS) & bit) == 0)
            ;
        cnt = 1;
    } else {
        // time need to update
        cnt = timeout*1000/100;
        while (cnt > 0) {
            cnt--;
            regval = DISP_REG_GET(idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_INT_STATUS);
            if (regval & bit) {
                DISP_CPU_REG_SET(idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_INT_STATUS, ~regval);
                break;
            }
            spin(100);
        }
    }
    //should clear?
    DDPMSG(" RDMA%d polling interrupt ret =%d\n", idx, cnt);
    return cnt;
}

static enum RDMA_INPUT_FORMAT rdma_input_format_convert(enum DpColorFormat  fmt)
{
    enum RDMA_INPUT_FORMAT rdma_fmt = RDMA_INPUT_FORMAT_RGB565;

    switch (fmt) {
    case eBGR565:
        rdma_fmt = RDMA_INPUT_FORMAT_BGR565;
        break;
    case eRGB888:
        rdma_fmt = RDMA_INPUT_FORMAT_RGB888;
        break;
    case eRGBA8888:
        rdma_fmt = RDMA_INPUT_FORMAT_RGBA8888;
        break;
    case eARGB8888:
        rdma_fmt = RDMA_INPUT_FORMAT_ARGB8888;
        break;
    case eVYUY:
        rdma_fmt = RDMA_INPUT_FORMAT_VYUY;
        break;
    case eYVYU:
        rdma_fmt = RDMA_INPUT_FORMAT_YVYU;
        break;
    case eRGB565:
        rdma_fmt = RDMA_INPUT_FORMAT_RGB565;
        break;
    case eBGR888:
        rdma_fmt = RDMA_INPUT_FORMAT_BGR888;
        break;
    case eBGRA8888:
        rdma_fmt = RDMA_INPUT_FORMAT_BGRA8888;
        break;
    case eABGR8888:
        rdma_fmt = RDMA_INPUT_FORMAT_ABGR8888;
        break;
    case eUYVY:
        rdma_fmt = RDMA_INPUT_FORMAT_UYVY;
        break;
    case eYUY2:
        rdma_fmt = RDMA_INPUT_FORMAT_YUYV;
        break;
    default:
        DDPERR("rdma_fmt_convert fmt=%d, rdma_fmt=%d\n", fmt, rdma_fmt);
    }
    return rdma_fmt;
}

static unsigned int rdma_input_format_byte_swap(enum RDMA_INPUT_FORMAT inputFormat)
{
    int input_swap = 0;

    switch (inputFormat) {
    case RDMA_INPUT_FORMAT_BGR565:
    case RDMA_INPUT_FORMAT_RGB888:
    case RDMA_INPUT_FORMAT_RGBA8888:
    case RDMA_INPUT_FORMAT_ARGB8888:
    case RDMA_INPUT_FORMAT_VYUY:
    case RDMA_INPUT_FORMAT_YVYU:
        input_swap = 1;
        break;
    case RDMA_INPUT_FORMAT_RGB565:
    case RDMA_INPUT_FORMAT_BGR888:
    case RDMA_INPUT_FORMAT_BGRA8888:
    case RDMA_INPUT_FORMAT_ABGR8888:
    case RDMA_INPUT_FORMAT_UYVY:
    case RDMA_INPUT_FORMAT_YUYV:
        input_swap = 0;
        break;
    default:
        DDPERR("unknown RDMA input format is %d\n", inputFormat);
        ASSERT(0);
    }
    return input_swap;
}

static unsigned int rdma_input_format_bpp(enum RDMA_INPUT_FORMAT inputFormat)
{
    int bpp = 0;

    switch (inputFormat) {
    case RDMA_INPUT_FORMAT_BGR565:
    case RDMA_INPUT_FORMAT_RGB565:
    case RDMA_INPUT_FORMAT_VYUY:
    case RDMA_INPUT_FORMAT_UYVY:
    case RDMA_INPUT_FORMAT_YVYU:
    case RDMA_INPUT_FORMAT_YUYV:
        bpp = 2;
        break;
    case RDMA_INPUT_FORMAT_RGB888:
    case RDMA_INPUT_FORMAT_BGR888:
        bpp = 3;
        break;
    case RDMA_INPUT_FORMAT_ARGB8888:
    case RDMA_INPUT_FORMAT_ABGR8888:
    case RDMA_INPUT_FORMAT_RGBA8888:
    case RDMA_INPUT_FORMAT_BGRA8888:
        bpp = 4;
        break;
    default:
        DDPERR("unknown RDMA input format = %d\n", inputFormat);
        ASSERT(0);
    }
    return  bpp;
}

static unsigned int rdma_input_format_color_space(enum RDMA_INPUT_FORMAT inputFormat)
{
    int space = 0;

    switch (inputFormat) {
    case RDMA_INPUT_FORMAT_BGR565:
    case RDMA_INPUT_FORMAT_RGB565:
    case RDMA_INPUT_FORMAT_RGB888:
    case RDMA_INPUT_FORMAT_BGR888:
    case RDMA_INPUT_FORMAT_RGBA8888:
    case RDMA_INPUT_FORMAT_BGRA8888:
    case RDMA_INPUT_FORMAT_ARGB8888:
    case RDMA_INPUT_FORMAT_ABGR8888:
        space = 0;
        break;
    case RDMA_INPUT_FORMAT_VYUY:
    case RDMA_INPUT_FORMAT_UYVY:
    case RDMA_INPUT_FORMAT_YVYU:
    case RDMA_INPUT_FORMAT_YUYV:
        space = 1;
        break;
    default:
        DDPERR("unknown RDMA input format = %d\n", inputFormat);
        ASSERT(0);
    }
    return space;
}

static unsigned int rdma_input_format_reg_value(enum RDMA_INPUT_FORMAT inputFormat)
{
    int reg_value = 0;

    switch (inputFormat) {
    case RDMA_INPUT_FORMAT_BGR565:
    case RDMA_INPUT_FORMAT_RGB565:
        reg_value = 0x0;
        break;
    case RDMA_INPUT_FORMAT_RGB888:
    case RDMA_INPUT_FORMAT_BGR888:
        reg_value = 0x1;
        break;
    case RDMA_INPUT_FORMAT_RGBA8888:
    case RDMA_INPUT_FORMAT_BGRA8888:
        reg_value = 0x2;
        break;
    case RDMA_INPUT_FORMAT_ARGB8888:
    case RDMA_INPUT_FORMAT_ABGR8888:
        reg_value = 0x3;
        break;
    case RDMA_INPUT_FORMAT_VYUY:
    case RDMA_INPUT_FORMAT_UYVY:
        reg_value = 0x4;
        break;
    case RDMA_INPUT_FORMAT_YVYU:
    case RDMA_INPUT_FORMAT_YUYV:
        reg_value = 0x5;
        break;
    default:
        DDPERR("unknown RDMA input format is %d\n", inputFormat);
        ASSERT(0);
    }
    return reg_value;
}

static const char *rdma_intput_format_name(int reg)
{
    switch (reg) {
    case 0:
        return "rgb565";
    case 1:
        return "rgb888";
    case 2:
        return "rgba8888";
    case 3:
        return "argb8888";
    case 4:
        return "uyvy";
    case 5:
        return "yuyv";
    default:
        DDPMSG("rdma unknown reg=%d\n", reg);
        return "unknown";
    }
}

static int RDMAClockOn(enum DISP_MODULE_ENUM module, void *handle)
{
    unsigned int idx = rdma_index(module);

    ddp_enable_module_clock(module);
    DDPMSG("RDMA%dClockOn CG 0x%x\n", idx, DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON0));
    return 0;
}
static int RDMAClockOff(enum DISP_MODULE_ENUM module, void *handle)
{
    unsigned int idx = rdma_index(module);

    DDPMSG("RDMA%dClockOff\n", idx);
    ddp_disable_module_clock(module);
    return 0;
}

static int RDMAInit(enum DISP_MODULE_ENUM module, void *handle)
{
    ddp_enable_module_clock(module);
    return 0;
}

static int RDMADeInit(enum DISP_MODULE_ENUM module, void *handle)
{
    unsigned int idx = rdma_index(module);

    DDPMSG("RDMA%dDeinit\n", idx);
    ddp_disable_module_clock(module);
    return 0;
}

void rdma_dump_golden_setting(enum DISP_MODULE_ENUM module)
{
    unsigned int idx = rdma_index(module);
    unsigned int offset = DISP_RDMA_INDEX_OFFSET * idx;

    DDPDUMP("RDMA%u golden setting\n", idx);
    DDPDUMP("GMC_SETTING_0 [11:0]:%u [27:16]:%u [30]:%u [31]:%u\n",
        DISP_REG_GET_FIELD(
            MEM_GMC_SETTING_0_FLD_PRE_ULTRA_THRESHOLD_LOW,
            offset + DISP_REG_RDMA_MEM_GMC_SETTING_0),
        DISP_REG_GET_FIELD(
            MEM_GMC_SETTING_0_FLD_PRE_ULTRA_THRESHOLD_HIGH,
            offset + DISP_REG_RDMA_MEM_GMC_SETTING_0),
        DISP_REG_GET_FIELD(
        MEM_GMC_SETTING_0_FLD_RG_VALID_THRESHOLD_FORCE_PREULTRA,
            offset + DISP_REG_RDMA_MEM_GMC_SETTING_0),
        DISP_REG_GET_FIELD(
            MEM_GMC_SETTING_0_FLD_RG_VDE_FORCE_PREULTRA,
            offset + DISP_REG_RDMA_MEM_GMC_SETTING_0));
    DDPDUMP("GMC_SETTING_1 [11:0]:%u [27:16]:%u [30]:%u [31]:%u\n",
        DISP_REG_GET_FIELD(MEM_GMC_SETTING_1_FLD_ULTRA_THRESHOLD_LOW,
            offset + DISP_REG_RDMA_MEM_GMC_SETTING_1),
        DISP_REG_GET_FIELD(MEM_GMC_SETTING_1_FLD_ULTRA_THRESHOLD_HIGH,
            offset + DISP_REG_RDMA_MEM_GMC_SETTING_1),
        DISP_REG_GET_FIELD(
            MEM_GMC_SETTING_1_FLD_RG_VALID_THRESHOLD_BLOCK_ULTRA,
            offset + DISP_REG_RDMA_MEM_GMC_SETTING_1),
        DISP_REG_GET_FIELD(MEM_GMC_SETTING_1_FLD_RG_VDE_BLOCK_ULTRA,
            offset + DISP_REG_RDMA_MEM_GMC_SETTING_1));
    DDPDUMP("GMC_SETTING_2 [13:0]:%u\n",
        DISP_REG_GET_FIELD(MEM_GMC_SETTING_2_FLD_ISSUE_REQ_THRESHOLD,
            offset + DISP_REG_RDMA_MEM_GMC_SETTING_2));
    DDPDUMP("FIFO_CON [11:0]:%u [27:16]:%d [31]:%u\n",
        DISP_REG_GET_FIELD(FIFO_CON_FLD_OUTPUT_VALID_FIFO_THRESHOLD,
            offset + DISP_REG_RDMA_FIFO_CON),
        DISP_REG_GET_FIELD(FIFO_CON_FLD_FIFO_PSEUDO_SIZE,
            offset + DISP_REG_RDMA_FIFO_CON),
        DISP_REG_GET_FIELD(FIFO_CON_FLD_FIFO_UNDERFLOW_EN,
            offset + DISP_REG_RDMA_FIFO_CON));
    DDPDUMP("THRSHOLD_SODI [11:0]:%u [27:16]:%u\n",
        DISP_REG_GET_FIELD(RDMA_THRESHOLD_FOR_DVFS_FLD_LOW,
            offset + DISP_REG_RDMA_THRESHOLD_FOR_SODI),
        DISP_REG_GET_FIELD(RDMA_THRESHOLD_FOR_DVFS_FLD_HIGH,
            offset + DISP_REG_RDMA_THRESHOLD_FOR_SODI));
    DDPDUMP("THRSHOLD_DVFS [11:0]:%u [27:16]:%u\n",
        DISP_REG_GET_FIELD(RDMA_THRESHOLD_FOR_DVFS_FLD_LOW,
            offset + DISP_REG_RDMA_THRESHOLD_FOR_DVFS),
        DISP_REG_GET_FIELD(RDMA_THRESHOLD_FOR_DVFS_FLD_HIGH,
            offset + DISP_REG_RDMA_THRESHOLD_FOR_DVFS));
    DDPDUMP("SRAM_SEL [0]:%u\n",
        DISP_REG_GET(offset + DISP_REG_RDMA_SRAM_SEL));
    DDPDUMP("DVFS_SETTING_PREULTRA [11:0]:%u [27:16]:%u\n",
        DISP_REG_GET_FIELD(RDMA_THRESHOLD_FOR_DVFS_FLD_LOW,
            offset + DISP_REG_RDMA_DVFS_SETTING_PRE),
        DISP_REG_GET_FIELD(RDMA_THRESHOLD_FOR_DVFS_FLD_HIGH,
            offset + DISP_REG_RDMA_DVFS_SETTING_PRE));
    DDPDUMP("DVFS_SETTING_ULTRA [11:0]:%u [27:16]:%u\n",
        DISP_REG_GET_FIELD(RDMA_THRESHOLD_FOR_DVFS_FLD_LOW,
            offset + DISP_REG_RDMA_DVFS_SETTING_ULTRA),
        DISP_REG_GET_FIELD(RDMA_THRESHOLD_FOR_DVFS_FLD_HIGH,
            offset + DISP_REG_RDMA_DVFS_SETTING_ULTRA));
    DDPDUMP("LEAVE_DRS_SETTING [11:0]:%u [27:16]:%u\n",
        DISP_REG_GET_FIELD(RDMA_THRESHOLD_FOR_DVFS_FLD_LOW,
            offset + DISP_REG_RDMA_LEAVE_DRS_SETTING),
        DISP_REG_GET_FIELD(RDMA_THRESHOLD_FOR_DVFS_FLD_HIGH,
            offset + DISP_REG_RDMA_LEAVE_DRS_SETTING));
    DDPDUMP("ENTER_DRS_SETTING [11:0]:%u [27:16]:%u\n",
        DISP_REG_GET_FIELD(RDMA_THRESHOLD_FOR_DVFS_FLD_LOW,
            offset + DISP_REG_RDMA_ENTER_DRS_SETTING),
        DISP_REG_GET_FIELD(RDMA_THRESHOLD_FOR_DVFS_FLD_HIGH,
            offset + DISP_REG_RDMA_ENTER_DRS_SETTING));
    DDPDUMP("GMC_SETTING_3 [11:0]:%u [27:16]:%u\n",
        DISP_REG_GET_FIELD(FLD_LOW_FOR_URGENT,
            offset + DISP_REG_RDMA_MEM_GMC_SETTING_3),
        DISP_REG_GET_FIELD(FLD_HIGH_FOR_URGENT,
            offset + DISP_REG_RDMA_MEM_GMC_SETTING_3));
}

void RDMADump(enum DISP_MODULE_ENUM module)
{
#if 0
    unsigned int idx = rdma_index(module);
    unsigned long module_base = DISPSYS_RDMA0_BASE + DISP_RDMA_INDEX_OFFSET * idx;


    /* dump working register */
    DDPDUMP("== START: DISP RDMA%d registers ==\n", idx);

    DDPDUMP("RDMA%d: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        idx,
        0x000, readl(module_base + 0x000),
        0x004, readl(module_base + 0x004),
        0x010, readl(module_base + 0x010),
        0x014, readl(module_base + 0x014));
    DDPDUMP("RDMA%d: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        idx,
        0x018, readl(module_base + 0x018),
        0x01c, readl(module_base + 0x01c),
        0x024, readl(module_base + 0x024),
        0x02C, readl(module_base + 0x02C));
    DDPDUMP("RDMA%d: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        idx,
        0x030, readl(module_base + 0x030),
        0x034, readl(module_base + 0x034),
        0x038, readl(module_base + 0x038),
        0x03C, readl(module_base + 0x03C));
    DDPDUMP("RDMA%d: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        idx,
        0x040, readl(module_base + 0x040),
        0x044, readl(module_base + 0x044),
        0x054, readl(module_base + 0x054),
        0x058, readl(module_base + 0x058));
    DDPDUMP("RDMA%d: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        idx,
        0x05C, readl(module_base + 0x05C),
        0x060, readl(module_base + 0x060),
        0x064, readl(module_base + 0x064),
        0x068, readl(module_base + 0x068));
    DDPDUMP("RDMA%d: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        idx,
        0x06C, readl(module_base + 0x06C),
        0x070, readl(module_base + 0x070),
        0x074, readl(module_base + 0x074),
        0x078, readl(module_base + 0x078));
    DDPDUMP("RDMA%d: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        idx,
        0x07C, readl(module_base + 0x07C),
        0x080, readl(module_base + 0x080),
        0x084, readl(module_base + 0x084),
        0x088, readl(module_base + 0x088));
    DDPDUMP("RDMA%d: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        idx,
        0x08C, readl(module_base + 0x08C),
        0x090, readl(module_base + 0x090),
        0x094, readl(module_base + 0x094),
        0xF00, readl(module_base + 0xF00));
    DDPDUMP("RDMA%d: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        idx,
        0x0a0, readl(module_base + 0x0a0),
        0x0a4, readl(module_base + 0x0a4),
        0x0a8, readl(module_base + 0x0a8),
        0x0ac, readl(module_base + 0x0ac));
    DDPDUMP("RDMA%d: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        idx,
        0x0b0, readl(module_base + 0x0b0),
        0x0b4, readl(module_base + 0x0b4),
        0x0bc, readl(module_base + 0x0bc),
        0x0c0, readl(module_base + 0x0c0));
    DDPDUMP("RDMA%d: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        idx,
        0x0d0, readl(module_base + 0x0d0),
        0x0d4, readl(module_base + 0x0d4),
        0x0d8, readl(module_base + 0x0d8),
        0x0dc, readl(module_base + 0x0dc));
    DDPDUMP("RDMA%d: 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        idx,
        0x0e0, readl(module_base + 0x0e0),
        0x0e4, readl(module_base + 0x0e4));

    DDPDUMP("-- END: DISP RDMA%d registers --\n", idx);
#endif
    rdma_dump_golden_setting(module);

}

void rdma_dump_analysis(enum DISP_MODULE_ENUM module)
{
    unsigned int idx = rdma_index(module);
    unsigned int global_ctrl = DISP_REG_GET(idx * DISP_RDMA_INDEX_OFFSET +
                                    DISP_REG_RDMA_GLOBAL_CON);
    unsigned int bg0 = DISP_REG_GET(idx * DISP_RDMA_INDEX_OFFSET +
                                    DISP_REG_RDMA_BG_CON_0);
    unsigned int bg1 = DISP_REG_GET(idx * DISP_RDMA_INDEX_OFFSET +
                                    DISP_REG_RDMA_BG_CON_1);

    DDPDUMP("== DISP RDMA%d ANALYSIS ==\n", idx);
    DDPDUMP("rdma%d: en=%d,memory_mode=%d,smi_busy=%d,w=%d,h=%d,pitch=%d,addr=0x%x,fmt=%s\n",
        idx, REG_FLD_VAL_GET(GLOBAL_CON_FLD_ENGINE_EN, global_ctrl),
        REG_FLD_VAL_GET(GLOBAL_CON_FLD_MODE_SEL, global_ctrl),
        REG_FLD_VAL_GET(GLOBAL_CON_FLD_SMI_BUSY, global_ctrl),
        DISP_REG_GET(DISP_REG_RDMA_SIZE_CON_0 + DISP_RDMA_INDEX_OFFSET * idx) & 0xfff,
        DISP_REG_GET(DISP_REG_RDMA_SIZE_CON_1 + DISP_RDMA_INDEX_OFFSET * idx) & 0xfffff,
        DISP_REG_GET(DISP_REG_RDMA_MEM_SRC_PITCH + DISP_RDMA_INDEX_OFFSET * idx),
        DISP_REG_GET(DISP_REG_RDMA_MEM_START_ADDR + DISP_RDMA_INDEX_OFFSET * idx),
        rdma_intput_format_name((DISP_REG_GET(DISP_REG_RDMA_MEM_CON +
                                                DISP_RDMA_INDEX_OFFSET * idx) >> 4) & 0xf));
    DDPDUMP("fifo_min=%d, in_p=%d, in_l=%d,out_p=%d,out_l=%d,bg(t%d,b%d,l%d,r%d)\n",
        DISP_REG_GET(DISP_REG_RDMA_FIFO_LOG + DISP_RDMA_INDEX_OFFSET * idx),
        DISP_REG_GET(DISP_REG_RDMA_IN_P_CNT + DISP_RDMA_INDEX_OFFSET * idx),
        DISP_REG_GET(DISP_REG_RDMA_IN_LINE_CNT + DISP_RDMA_INDEX_OFFSET * idx),
        DISP_REG_GET(DISP_REG_RDMA_OUT_P_CNT + DISP_RDMA_INDEX_OFFSET * idx),
        DISP_REG_GET(DISP_REG_RDMA_OUT_LINE_CNT + DISP_RDMA_INDEX_OFFSET * idx),
        REG_FLD_VAL_GET(RDMA_BG_CON_1_TOP, bg1),
        REG_FLD_VAL_GET(RDMA_BG_CON_1_BOTTOM, bg1),
        REG_FLD_VAL_GET(RDMA_BG_CON_0_LEFT, bg0),
        REG_FLD_VAL_GET(RDMA_BG_CON_0_RIGHT, bg0)
        );
}

static int RDMAConfig_l(enum DISP_MODULE_ENUM module, struct disp_ddp_path_config *pConfig,
                        void *handle)
{
    struct RDMA_CONFIG_STRUCT *rdma_config = &pConfig->rdma_config;
    enum RDMA_MODE mode = rdma_config->address ? RDMA_MODE_MEMORY : RDMA_MODE_DIRECT_LINK;
    LCM_DSI_PARAMS *lcm_config = &(pConfig->dsi_config);

    if (pConfig->dst_dirty) {
        //config to direct link mode
        RDMAConfig(module,
                   RDMA_MODE_DIRECT_LINK, //  link mode
                   0,                     // address
                   eRGB888,               // inputFormat
                   0,                     // pitch
                   pConfig->dst_w,        // width
                   pConfig->dst_h,        // height
                   lcm_config->ufoe_enable,
                   lcm_config->mode,
                   handle);
    } else if (pConfig->rdma_dirty) {
        // decouple mode may use
        RDMAConfig(module,
                   mode,    //  link mode
                   (mode == RDMA_MODE_DIRECT_LINK) ? 0 : rdma_config->address,
                   (mode == RDMA_MODE_DIRECT_LINK) ? eRGB888 : rdma_config->inputFormat,
                   (mode == RDMA_MODE_DIRECT_LINK) ? 0 : rdma_config->pitch,
                   rdma_config->width,
                   rdma_config->height,
                   lcm_config->ufoe_enable,
                   lcm_config->mode,
                   handle);
    }

    return 0;
}

void rdma_enable_color_transform(enum DISP_MODULE_ENUM module)
{
    unsigned int idx = rdma_index(module);
    u32 value =  DISP_REG_GET(DISP_REG_RDMA_SIZE_CON_0 + DISP_RDMA_INDEX_OFFSET * idx);

    value = value | REG_FLD_VAL((SIZE_CON_0_FLD_MATRIX_EXT_MTX_EN), 1) |
            REG_FLD_VAL((SIZE_CON_0_FLD_MATRIX_ENABLE), 1);
    DISP_REG_SET(NULL, idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_SIZE_CON_0, value);
}

void rdma_disable_color_transform(enum DISP_MODULE_ENUM module)
{
    unsigned int idx = rdma_index(module);
    u32 value =  DISP_REG_GET(DISP_REG_RDMA_SIZE_CON_0 + DISP_RDMA_INDEX_OFFSET * idx);

    value = value | REG_FLD_VAL((SIZE_CON_0_FLD_MATRIX_EXT_MTX_EN), 0) |
            REG_FLD_VAL((SIZE_CON_0_FLD_MATRIX_ENABLE), 0);
    DISP_REG_SET(NULL, idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_SIZE_CON_0, value);
}

void rdma_set_color_matrix(enum DISP_MODULE_ENUM module,
                           struct rdma_color_matrix *matrix,
                           struct rdma_color_pre *pre,
                           struct rdma_color_post *post)
{
    unsigned int idx = rdma_index(module);

    DISP_REG_SET(NULL, idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_C00, matrix->C00);
    DISP_REG_SET(NULL, idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_C01, matrix->C01);
    DISP_REG_SET(NULL, idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_C02, matrix->C02);
    DISP_REG_SET(NULL, idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_C10, matrix->C10);
    DISP_REG_SET(NULL, idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_C11, matrix->C11);
    DISP_REG_SET(NULL, idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_C12, matrix->C12);
    DISP_REG_SET(NULL, idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_C20, matrix->C20);
    DISP_REG_SET(NULL, idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_C21, matrix->C21);
    DISP_REG_SET(NULL, idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_C22, matrix->C22);

    DISP_REG_SET(NULL, idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_PRE_ADD_0, pre->ADD0);
    DISP_REG_SET(NULL, idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_PRE_ADD_1, pre->ADD1);
    DISP_REG_SET(NULL, idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_PRE_ADD_2, pre->ADD2);

    DISP_REG_SET(NULL, idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_POST_ADD_0, post->ADD0);
    DISP_REG_SET(NULL, idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_POST_ADD_1, post->ADD1);
    DISP_REG_SET(NULL, idx * DISP_RDMA_INDEX_OFFSET + DISP_REG_RDMA_POST_ADD_2, post->ADD2);
}

struct DDP_MODULE_DRIVER ddp_driver_rdma = {
    .module          = DISP_MODULE_RDMA0,
    .init            = RDMAInit,
    .deinit          = RDMADeInit,
    .config          = RDMAConfig_l,
    .start           = RDMAStart,
    .trigger         = NULL,
    .stop            = RDMAStop,
    .reset           = RDMAReset,
    .power_on        = RDMAClockOn,
    .power_off       = RDMAClockOff,
    .is_idle         = NULL,
    .is_busy         = NULL,
    .dump_info       = NULL,
    .bypass          = NULL,
    .build_cmdq      = NULL,
    .set_lcm_utils   = NULL,
    .polling_irq     = RDMAPollingInterrupt,
};
