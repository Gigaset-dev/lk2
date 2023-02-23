/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#define LOG_TAG "POSTMASK"

#include <lcm_drv.h>
#include "ddp_reg.h"
#include "ddp_matrix_para.h"
#include "ddp_info.h"
#include "ddp_log.h"

#include "ddp_postmask.h"
#include "disp_drv_platform.h"


#define POSTMASK_MASK_MAX_NUM    96
#define POSTMASK_GRAD_MAX_NUM    192
#define POSTMASK_DRAM_MODE

static unsigned int postmask_bg_color = 0xff000000;
static unsigned int postmask_mask_color = 0xff000000;

static int postmask_reg_offset(enum DISP_MODULE_ENUM module)
{
    if (module == DISP_MODULE_POSTMASK1)
        return DISP_POSTMASK_INDEX_OFFSET;

    return 0;
}

int postmask_dump_reg(enum DISP_MODULE_ENUM module)
{
    int offset = postmask_reg_offset(module);

    DDPDUMP("== DISP %s REGS ==\n", ddp_get_module_name(module));
    DDPDUMP("(0x0)0x%08x 0x%08x 0x%08x 0x%08x\n",
        DISP_REG_GET(DISP_REG_POSTMASK_EN + offset),
        DISP_REG_GET(DISP_REG_POSTMASK_RESET + offset),
        DISP_REG_GET(DISP_REG_POSTMASK_INTEN + offset),
        DISP_REG_GET(DISP_REG_POSTMASK_INTSTA + offset));
    DDPDUMP("(0x20)0x%08x\n",
        DISP_REG_GET(DISP_REG_POSTMASK_CFG + offset));
    DDPDUMP("(0x30)0x%08x\n",
        DISP_REG_GET(DISP_REG_POSTMASK_SIZE + offset));
    DDPDUMP("(0x40)0x%08x 0x%08x 0x%08x\n",
        DISP_REG_GET(DISP_REG_POSTMASK_SRAM_CFG + offset),
        DISP_REG_GET(DISP_REG_POSTMASK_MASK_SHIFT + offset),
        DISP_REG_GET(DISP_REG_POSTMASK_GRAD_SHIFT + offset));
    DDPDUMP("(0x50)0x%08x 0x%08x 0x%08x\n",
        DISP_REG_GET(DISP_REG_POSTMASK_BLEND_CFG + offset),
        DISP_REG_GET(DISP_REG_POSTMASK_ROI_BGCLR + offset),
        DISP_REG_GET(DISP_REG_POSTMASK_MASK_CLR + offset));
    DDPDUMP("(0xA0)0x%08x 0x%08x\n",
        DISP_REG_GET(DISP_REG_POSTMASK_STATUS + offset),
        DISP_REG_GET(DISP_REG_POSTMASK_INPUT_COUNT + offset));
    DDPDUMP("(0xB0)0x%08x 0x%08x 0x%08x\n",
        DISP_REG_GET(DISP_REG_POSTMASK_DEBUG_0 + offset),
        DISP_REG_GET(DISP_REG_POSTMASK_DEBUG_1 + offset),
        DISP_REG_GET(DISP_REG_POSTMASK_DEBUG_2 + offset));
    DDPDUMP("(0x100)0x%08x 0x%08x 0x%08x 0x%08x\n",
        DISP_REG_GET(DISP_REG_POSTMASK_MEM_ADDR + offset),
        DISP_REG_GET(DISP_REG_POSTMASK_MEM_LENGTH + offset),
        DISP_REG_GET(DISP_REG_POSTMASK_RDMA_FIFO_CTRL + offset),
        DISP_REG_GET(DISP_REG_POSTMASK_MEM_GMC_SETTING2 + offset));
    DDPDUMP("(0x110)0x%08x\n",
        DISP_REG_GET(DISP_REG_POSTMASK_PAUSE_REGION + offset));
    DDPDUMP("(0x130)0x%08x 0x%08x\n",
        DISP_REG_GET(DISP_REG_POSTMASK_RDMA_GREQ_NUM + offset),
        DISP_REG_GET(DISP_REG_POSTMASK_RDMA_GREQ_URG_NUM + offset));
    DDPDUMP("(0x140)0x%08x 0x%08x 0x%08x\n",
        DISP_REG_GET(DISP_REG_POSTMASK_RDMA_ULTRA_SRC + offset),
        DISP_REG_GET(DISP_REG_POSTMASK_RDMA_BUF_LOW_TH + offset),
        DISP_REG_GET(DISP_REG_POSTMASK_RDMA_BUF_HIGH_TH + offset));

    return 0;
}

int postmask_dump_analysis(enum DISP_MODULE_ENUM module)
{
    int offset = postmask_reg_offset(module);

    DDPDUMP("== DISP %s ANALYSIS ==\n", ddp_get_module_name(module));
    DDPDUMP("en=%d,cfg=0x%x,size=(%dx%d)\n",
        DISP_REG_GET(DISP_REG_POSTMASK_EN + offset) & 0x1,
        DISP_REG_GET(DISP_REG_POSTMASK_CFG + offset),
        (DISP_REG_GET(DISP_REG_POSTMASK_SIZE + offset) >> 16) & 0x1fff,
        DISP_REG_GET(DISP_REG_POSTMASK_SIZE + offset) & 0x1fff);
    DDPDUMP("blend_cfg=0x%x,bg=0x%x,mask=0x%x\n",
        DISP_REG_GET(DISP_REG_POSTMASK_BLEND_CFG + offset),
        DISP_REG_GET(DISP_REG_POSTMASK_ROI_BGCLR + offset),
        DISP_REG_GET(DISP_REG_POSTMASK_MASK_CLR + offset));
    DDPDUMP("fifo_cfg=%d,gmc=0x%x,threshold=(0x%x,0x%x)\n",
        DISP_REG_GET(DISP_REG_POSTMASK_RDMA_FIFO_CTRL + offset),
        DISP_REG_GET(DISP_REG_POSTMASK_MEM_GMC_SETTING2 + offset),
        DISP_REG_GET(DISP_REG_POSTMASK_RDMA_BUF_LOW_TH + offset),
        DISP_REG_GET(DISP_REG_POSTMASK_RDMA_BUF_HIGH_TH + offset));
    DDPDUMP("mem_addr=0x%x,length=0x%x\n",
        DISP_REG_GET(DISP_REG_POSTMASK_MEM_ADDR + offset),
        DISP_REG_GET(DISP_REG_POSTMASK_MEM_LENGTH + offset));
    DDPDUMP("status=0x%x,cur_pos=0x%x\n",
        DISP_REG_GET(DISP_REG_POSTMASK_STATUS + offset),
        DISP_REG_GET(DISP_REG_POSTMASK_INPUT_COUNT + offset));

    return 0;
}

static int postmask_dump(enum DISP_MODULE_ENUM module, int level)
{
    postmask_dump_analysis(module);
    postmask_dump_reg(module);

    return 0;
}

static int POSTMASKClockOn(enum DISP_MODULE_ENUM module, void *handle)
{
    int offset = postmask_reg_offset(module);

    ddp_enable_module_clock(module);

    DDPMSG("%s CG:%d(0x%x)\n", __func__,
        (DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON1 + offset) >> 14) & 0x1,
        DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON1 + offset));

    return 0;
}

static int POSTMASKClockOff(enum DISP_MODULE_ENUM module, void *handle)
{
    int offset = postmask_reg_offset(module);

    ddp_disable_module_clock(module);

    DDPMSG("%s CG:%d(0x%x)\n", __func__,
        (DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON1 + offset) >> 14) & 0x1,
        DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON1 + offset));

    return 0;
}

static int POSTMASKInit(enum DISP_MODULE_ENUM module, void *handle)
{
    return POSTMASKClockOn(module, handle);
}

static int POSTMASKDeinit(enum DISP_MODULE_ENUM module, void *handle)
{
    return POSTMASKClockOff(module, handle);
}

int POSTMASKStart(enum DISP_MODULE_ENUM module, void *handle)
{
    int offset = postmask_reg_offset(module);

    DISP_REG_SET_FIELD(handle, EN_FLD_POSTMASK_EN,
                DISP_REG_POSTMASK_EN + offset, 1);

    DISP_REG_SET(handle, DISP_REG_POSTMASK_INTEN + offset,
        REG_FLD_VAL(PM_INTEN_FLD_PM_IF_FME_END_INTEN, 1) |
        REG_FLD_VAL(PM_INTEN_FLD_PM_FME_CPL_INTEN, 1) |
        REG_FLD_VAL(PM_INTEN_FLD_PM_START_INTEN, 1) |
        REG_FLD_VAL(PM_INTEN_FLD_PM_ABNORMAL_SOF_INTEN, 1) |
        REG_FLD_VAL(PM_INTEN_FLD_RDMA_FME_UND_INTEN, 1) |
        REG_FLD_VAL(PM_INTEN_FLD_RDMA_EOF_ABNORMAL_INTEN, 1));

    DDPMSG("%s en:%d(0x%x)\n", __func__,
        DISP_REG_GET(DISP_REG_POSTMASK_EN + offset) & 0x1,
        DISP_REG_GET(DISP_REG_POSTMASK_EN + offset));

    return 0;
}

int POSTMASKStop(enum DISP_MODULE_ENUM module, void *handle)
{
    int offset = postmask_reg_offset(module);

    DISP_REG_SET(handle, DISP_REG_POSTMASK_INTEN + offset, 0);
    DISP_REG_SET_FIELD(handle, EN_FLD_POSTMASK_EN,
                DISP_REG_POSTMASK_EN + offset, 0);
    DISP_REG_SET(handle, DISP_REG_POSTMASK_INTSTA + offset, 0);

    DDPMSG("%s en:%d(0x%x)\n", __func__,
        DISP_REG_GET(DISP_REG_POSTMASK_EN + offset) & 0x1,
        DISP_REG_GET(DISP_REG_POSTMASK_EN + offset));

    return 0;
}

static int POSTMASKConfig(enum DISP_MODULE_ENUM module,
                struct disp_ddp_path_config *pConfig,
                void *handle)
{
    unsigned int value = 0;
#ifdef MTK_ROUND_CORNER_SUPPORT
    LCM_ROUND_CORNER * round_corner;
#ifndef POSTMASK_DRAM_MODE
    unsigned int i = 0;
    unsigned char *p;
    unsigned int num = 0;
#else
    void *p_addr;
    unsigned int p_size;
#endif
#endif
    int offset = postmask_reg_offset(module);

    if (!pConfig->dst_dirty)
        return 0;

#ifdef MTK_ROUND_CORNER_SUPPORT
    round_corner = &(pConfig->round_corner_params);
    DDPMSG("%s size:(%d,%d), en:%d, pattern:(%d,%d), mem:(%p,%d)\n",
           __func__, pConfig->dst_w, pConfig->dst_h,
           round_corner->round_corner_en,
           round_corner->h,
           round_corner->h_bot,
           round_corner->lt_addr,
           round_corner->tp_size);
#endif

    value = (REG_FLD_VAL((PM_BLEND_CFG_FLD_A_EN), 1) |
         REG_FLD_VAL((PM_BLEND_CFG_FLD_PARGB_BLD), 0) |
         REG_FLD_VAL((PM_BLEND_CFG_FLD_CONST_BLD), 0));
    DISP_REG_SET(handle, DISP_REG_POSTMASK_BLEND_CFG + offset, value);

    DISP_REG_SET(handle, DISP_REG_POSTMASK_ROI_BGCLR + offset, postmask_bg_color);
    DISP_REG_SET(handle, DISP_REG_POSTMASK_MASK_CLR + offset, postmask_mask_color);

    value = (REG_FLD_VAL((PM_SIZE_FLD_HSIZE), pConfig->dst_w) |
         REG_FLD_VAL((PM_SIZE_FLD_VSIZE), pConfig->dst_h));
    DISP_REG_SET(handle, DISP_REG_POSTMASK_SIZE + offset, value);

#if defined(MTK_ROUND_CORNER_SUPPORT)
    if (round_corner->round_corner_en == 1) {
#if !defined(DISP_HW_RC)
        DDPERR("unsupport round corner mode\n");
        return -1;
#endif

        value = (REG_FLD_VAL((PM_PAUSE_REGION_FLD_RDMA_PAUSE_START),
             round_corner->h) |
             REG_FLD_VAL((PM_PAUSE_REGION_FLD_RDMA_PAUSE_END),
             pConfig->dst_h - round_corner->h_bot));
        DISP_REG_SET(handle, DISP_REG_POSTMASK_PAUSE_REGION + offset, value);

        value = (REG_FLD_VAL((PM_MEM_GMC_FLD_ISSUE_REQ_TH), 63) |
             REG_FLD_VAL((PM_MEM_GMC_FLD_ISSUE_REQ_TH_URG), 63) |
             REG_FLD_VAL((PM_MEM_GMC_FLD_REQ_TH_PREULTRA), 0) |
             REG_FLD_VAL((PM_MEM_GMC_FLD_REQ_TH_ULTRA), 1) |
             REG_FLD_VAL((PM_MEM_GMC_FLD_FORCE_REQ_TH), 0));
        DISP_REG_SET(handle, DISP_REG_POSTMASK_MEM_GMC_SETTING2 + offset, value);

        value = (REG_FLD_VAL((PM_GREQ_FLD_GREQ_NUM), 7) |
             REG_FLD_VAL((PM_GREQ_FLD_GREQ_URG_NUM), 7) |
             REG_FLD_VAL((PM_GREQ_FLD_GREQ_NUM_SHT_VAL), 1) |
             REG_FLD_VAL((PM_GREQ_FLD_GREQ_NUM_SHT), 0) |
             REG_FLD_VAL((PM_GREQ_FLD_OSTD_GREQ_NUM), 0xFF) |
             REG_FLD_VAL((PM_GREQ_FLD_GREQ_DIS_CNT), 1) |
             REG_FLD_VAL((PM_GREQ_FLD_GREQ_STOP_EN), 0) |
             REG_FLD_VAL((PM_GREQ_FLD_GRP_END_STOP), 1) |
             REG_FLD_VAL((PM_GREQ_FLD_GRP_BRK_STOP), 1) |
             REG_FLD_VAL((PM_GREQ_FLD_IOBUF_FLUSH_PREULTRA), 1) |
             REG_FLD_VAL((PM_GREQ_FLD_IOBUF_FLUSH_ULTRA), 1));
        DISP_REG_SET(handle, DISP_REG_POSTMASK_RDMA_GREQ_NUM + offset, value);

        value = (REG_FLD_VAL((PM_GREQ_URG_FLD_ARB_GREQ_URG_TH), 0) |
             REG_FLD_VAL((PM_GREQ_URG_FLD_ARB_URG_BIAS), 0));
        DISP_REG_SET(handle, DISP_REG_POSTMASK_RDMA_GREQ_URG_NUM + offset, value);

        value = (REG_FLD_VAL((PM_ULTRA_FLD_PREULTRA_BUF_SRC), 0) |
             REG_FLD_VAL((PM_ULTRA_FLD_PREULTRA_SMI_SRC), 1) |
             REG_FLD_VAL((PM_ULTRA_FLD_PREULTRA_ROI_END_SRC), 0) |
             REG_FLD_VAL((PM_ULTRA_FLD_PREULTRA_RDMA_SRC), 0) |
             REG_FLD_VAL((PM_ULTRA_FLD_ULTRA_BUF_SRC), 0) |
             REG_FLD_VAL((PM_ULTRA_FLD_ULTRA_SMI_SRC), 1) |
             REG_FLD_VAL((PM_ULTRA_FLD_ULTRA_ROI_END_SRC), 0) |
             REG_FLD_VAL((PM_ULTRA_FLD_ULTRA_RDMA_SRC), 0));
        DISP_REG_SET(handle, DISP_REG_POSTMASK_RDMA_ULTRA_SRC + offset, value);

        value = (REG_FLD_VAL((PM_TH_FLD_RDMA_ULTRA_LOW_TH), 0xFFF) |
             REG_FLD_VAL((PM_TH_FLD_RDMA_PREULTRA_LOW_TH), 0xFFF));
        DISP_REG_SET(handle, DISP_REG_POSTMASK_RDMA_BUF_LOW_TH + offset, value);

        value = (REG_FLD_VAL((PM_TH_FLD_RDMA_PREULTRA_HIGH_TH), 0xFFF) |
             REG_FLD_VAL((PM_TH_FLD_RDMA_PREULTRA_HIGH_DIS), 0));
        DISP_REG_SET(handle, DISP_REG_POSTMASK_RDMA_BUF_HIGH_TH + offset, value);

#ifdef POSTMASK_DRAM_MODE
        value = (REG_FLD_VAL((PM_CFG_FLD_RELAY_MODE), 0) |
             REG_FLD_VAL((PM_CFG_FLD_DRAM_MODE), 1) |
             REG_FLD_VAL((PM_CFG_FLD_BGCLR_IN_SEL), 1) |
             REG_FLD_VAL((PM_CFG_FLD_GCLAST_EN), 1) |
             REG_FLD_VAL((PM_CFG_FLD_STALL_CG_ON), 1));
        DISP_REG_SET(handle, DISP_REG_POSTMASK_CFG + offset, value);
#ifdef DISP_PRIM_DUAL_PIPE
        if (pConfig->dsi_config.output_mode == MTK_PANEL_DUAL_PORT ||
                (pConfig->dsi_config.dsc_enable == 1 &&
                pConfig->dsi_config.dsc_params.slice_mode == 1)) {

            if (module == DISP_MODULE_POSTMASK0) {
                p_addr = round_corner->lt_addr_l;
                p_size = round_corner->tp_size_l;
            } else if (module == DISP_MODULE_POSTMASK1) {
                p_addr = round_corner->lt_addr_r;
                p_size = round_corner->tp_size_r;
            }
        } else
#endif
        {
            p_addr = round_corner->lt_addr;
            p_size = round_corner->tp_size;
        }

        DISP_REG_SET(handle, DISP_REG_POSTMASK_MEM_ADDR + offset,
             p_addr);
        DISP_REG_SET(handle, DISP_REG_POSTMASK_MEM_LENGTH + offset,
             p_size);
#else
        value = (REG_FLD_VAL((PM_CFG_FLD_RELAY_MODE), 0) |
             REG_FLD_VAL((PM_CFG_FLD_DRAM_MODE), 0) |
             REG_FLD_VAL((PM_CFG_FLD_BGCLR_IN_SEL), 1) |
             REG_FLD_VAL((PM_CFG_FLD_GCLAST_EN), 1) |
             REG_FLD_VAL((PM_CFG_FLD_STALL_CG_ON), 1));
        DISP_REG_SET(handle, DISP_REG_POSTMASK_CFG + offset, value);

        value = (REG_FLD_VAL((PM_SRAM_CFG_FLD_MASK_NUM_SW_SET),
             round_corner->h) |
             REG_FLD_VAL((PM_SRAM_CFG_FLD_MASK_L_TOP_EN), 1) |
             REG_FLD_VAL((PM_SRAM_CFG_FLD_MASK_L_BOTTOM_EN), 1) |
             REG_FLD_VAL((PM_SRAM_CFG_FLD_MASK_R_TOP_EN), 1) |
             REG_FLD_VAL((PM_SRAM_CFG_FLD_MASK_R_BOTTOM_EN), 1));
        DISP_REG_SET(handle, DISP_REG_POSTMASK_SRAM_CFG + offset, value);

        num = POSTMASK_MASK_MAX_NUM;
        for (i = 0; i < num; i++)
            DISP_REG_SET(handle, DISP_REG_POSTMASK_NUM(i) + offset, 0x1F001F00);

        num = POSTMASK_GRAD_MAX_NUM;
        for (i = 0; i < num; i++)
            DISP_REG_SET(handle, DISP_REG_POSTMASK_GRAD_VAL(i) + offset, 0x0);

#if 0
        num = round_corner->tp_size >> 2;
        for (i = 0; i < num; i++) {
            p = round_corner->lt_addr + (i * 4);

            value = (((*p & 0xFF) << 24) |
                ((*(p+1) & 0xFF) << 16) |
                ((*(p+2) & 0xFF) << 8) |
                (*(p+3) & 0xFF));
            DISP_REG_SET(handle, DISP_REG_POSTMASK_NUM(i), value);
        }
#endif
#endif
    } else {
        /*enable BYPASS postmask*/
        value = (REG_FLD_VAL((PM_CFG_FLD_RELAY_MODE), 1) |
             REG_FLD_VAL((PM_CFG_FLD_DRAM_MODE), 1) |
             REG_FLD_VAL((PM_CFG_FLD_BGCLR_IN_SEL), 1) |
             REG_FLD_VAL((PM_CFG_FLD_GCLAST_EN), 1) |
             REG_FLD_VAL((PM_CFG_FLD_STALL_CG_ON), 1));
        DISP_REG_SET(handle, DISP_REG_POSTMASK_CFG + offset, value);
    }
#else
    /*enable BYPASS postmask*/
    value = (REG_FLD_VAL((PM_CFG_FLD_RELAY_MODE), 1) |
         REG_FLD_VAL((PM_CFG_FLD_DRAM_MODE), 1) |
         REG_FLD_VAL((PM_CFG_FLD_BGCLR_IN_SEL), 1) |
         REG_FLD_VAL((PM_CFG_FLD_GCLAST_EN), 1) |
         REG_FLD_VAL((PM_CFG_FLD_STALL_CG_ON), 1));
    DISP_REG_SET(handle, DISP_REG_POSTMASK_CFG + offset, value);
#endif

    return 0;
}

int POSTMASKReset(enum DISP_MODULE_ENUM module, void *handle)
{
    int offset = postmask_reg_offset(module);

    DISP_REG_SET_FIELD(handle, RESET_FLD_POSTMASK_RESET,
        DISP_REG_POSTMASK_RESET + offset, 1);
    DISP_REG_SET_FIELD(handle, RESET_FLD_POSTMASK_RESET,
        DISP_REG_POSTMASK_RESET + offset, 0);
    DDPMSG("%s done\n", __func__);

    return 0;
}

int POSTMASKBypass(enum DISP_MODULE_ENUM module, int bypass)
{
    int offset = postmask_reg_offset(module);

    DISP_REG_SET_FIELD(NULL, PM_CFG_FLD_RELAY_MODE,
        DISP_REG_POSTMASK_CFG + offset, 1);
    DDPMSG("%s done\n", __func__);

    return 0;
}

struct DDP_MODULE_DRIVER ddp_driver_postmask = {
    .module          = DISP_MODULE_POSTMASK0,
    .init            = POSTMASKInit,
    .deinit          = POSTMASKDeinit,
    .config          = POSTMASKConfig,
    .start         = POSTMASKStart,
    .trigger         = NULL,
    .stop             = POSTMASKStop,
    .reset           = POSTMASKReset,
    .power_on        = POSTMASKClockOn,
    .power_off       = POSTMASKClockOff,
    .is_idle         = NULL,
    .is_busy         = NULL,
    .dump_info       = postmask_dump,
    .bypass          = POSTMASKBypass,
    .build_cmdq      = NULL,
    .set_lcm_utils   = NULL,
};
