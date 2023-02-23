/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#define LOG_TAG "OVL"

#include <stdbool.h>
#include "ddp_reg.h"
#include "ddp_matrix_para.h"
#include "ddp_info.h"
#include "ddp_log.h"
#include "ddp_ovl.h"
#include "disp_drv_platform.h"

#define OVL_REG_BACK_MAX  (40)

#define OVL_LAYER_OFFSET        (0x20)
#define OVL_RDMA_DEBUG_OFFSET   (0x4)

enum OVL_COLOR_SPACE {
    OVL_COLOR_SPACE_RGB = 0,
    OVL_COLOR_SPACE_YUV,
};

enum OVL_INPUT_FORMAT {
    OVL_INPUT_FORMAT_BGR565     = 0,
    OVL_INPUT_FORMAT_RGB888     = 1,
    OVL_INPUT_FORMAT_RGBA8888   = 2,
    OVL_INPUT_FORMAT_ARGB8888   = 3,
    OVL_INPUT_FORMAT_VYUY       = 4,
    OVL_INPUT_FORMAT_YVYU       = 5,

    OVL_INPUT_FORMAT_RGB565     = 6,
    OVL_INPUT_FORMAT_BGR888     = 7,
    OVL_INPUT_FORMAT_BGRA8888   = 8,
    OVL_INPUT_FORMAT_ABGR8888   = 9,
    OVL_INPUT_FORMAT_UYVY       = 10,
    OVL_INPUT_FORMAT_YUYV       = 11,

    OVL_INPUT_FORMAT_RGBA4444    = 12,
    OVL_INPUT_FORMAT_BGRA4444    = 13,

    OVL_INPUT_FORMAT_UNKNOWN    = 32,
};

struct OVL_REG {
    unsigned int address;
    unsigned int value;
};

static int reg_back_cnt[OVL_NUM];
static struct OVL_REG reg_back[OVL_NUM][OVL_REG_BACK_MAX];
static enum OVL_INPUT_FORMAT ovl_input_fmt_convert(enum DpColorFormat  fmt);
static unsigned int ovl_input_fmt_byte_swap(enum OVL_INPUT_FORMAT inputFormat);
static unsigned int ovl_input_fmt_bpp(enum OVL_INPUT_FORMAT inputFormat);
static enum OVL_COLOR_SPACE ovl_input_fmt_color_space(enum OVL_INPUT_FORMAT inputFormat);
static unsigned int ovl_input_fmt_reg_value(enum OVL_INPUT_FORMAT inputFormat);
static void ovl_store_regs(int idx);
static void ovl_restore_regs(int idx, void *handle);

static enum OVL_INPUT_FORMAT ovl_input_fmt_convert(enum DpColorFormat  fmt)
{
    enum OVL_INPUT_FORMAT ovl_fmt = OVL_INPUT_FORMAT_UNKNOWN;

    switch (fmt) {
    case eBGR565:
        ovl_fmt = OVL_INPUT_FORMAT_BGR565;
        break;
    case eRGB565:
        ovl_fmt = OVL_INPUT_FORMAT_RGB565;
        break;
    case eRGB888:
        ovl_fmt = OVL_INPUT_FORMAT_RGB888;
        break;
    case eBGR888:
        ovl_fmt = OVL_INPUT_FORMAT_BGR888;
        break;
    case eRGBA4444:
        ovl_fmt = OVL_INPUT_FORMAT_RGBA4444;
        break;
    case eBGRA4444:
        ovl_fmt = OVL_INPUT_FORMAT_BGRA4444;
        break;
    case eRGBA8888:
        ovl_fmt = OVL_INPUT_FORMAT_RGBA8888;
        break;
    case eBGRA8888:
        ovl_fmt = OVL_INPUT_FORMAT_BGRA8888;
        break;
    case eARGB8888:
        ovl_fmt = OVL_INPUT_FORMAT_ARGB8888;
        break;
    case eABGR8888:
        ovl_fmt = OVL_INPUT_FORMAT_ABGR8888;
        break;
    case eVYUY:
        ovl_fmt = OVL_INPUT_FORMAT_VYUY;
        break;
    case eYVYU:
        ovl_fmt = OVL_INPUT_FORMAT_YVYU;
        break;
    case eUYVY:
        ovl_fmt = OVL_INPUT_FORMAT_UYVY;
        break;
    case eYUY2:
        ovl_fmt = OVL_INPUT_FORMAT_YUYV;
        break;
    default:
        DDPERR("ovl_fmt_convert fmt=%d, ovl_fmt=%d\n", fmt, ovl_fmt);
        break;
    }
    return ovl_fmt;
}

unsigned long ovl_base_addr(enum DISP_MODULE_ENUM module)
{
    switch (module) {
    case DISP_MODULE_OVL0:
        return DISPSYS_OVL0_BASE;
    case DISP_MODULE_OVL1:
        return DISPSYS_OVL1_BASE;
    case DISP_MODULE_OVL0_2L:
        return DISPSYS_OVL0_2L_BASE;
    case DISP_MODULE_OVL1_2L:
        return DISPSYS_OVL1_2L_BASE;
    default:
        return 0;
    }
}

unsigned long mmsys_ovl_ultra_offset(enum DISP_MODULE_ENUM module)
{
    switch (module) {
    case DISP_MODULE_OVL0:
        return FLD_OVL0_ULTRA_SEL;
    case DISP_MODULE_OVL0_2L:
        return FLD_OVL0_2L_ULTRA_SEL;
    case DISP_MODULE_OVL1_2L:
        return FLD_OVL1_2L_ULTRA_SEL;
    default:
        return 0;
    }
    return 0;
}

static enum DpColorFormat ovl_input_fmt(enum OVL_INPUT_FORMAT fmt, int swap)
{
    switch (fmt) {
    case OVL_INPUT_FORMAT_BGR565:
        return swap ? eBGR565 : eRGB565;
    case OVL_INPUT_FORMAT_RGB888:
        return swap ? eRGB888 : eBGR888;
    case OVL_INPUT_FORMAT_RGBA8888:
        return swap ? eRGBA8888 : eBGRA8888;
    case OVL_INPUT_FORMAT_ARGB8888:
        return swap ? eARGB8888 : eABGR8888;
    case OVL_INPUT_FORMAT_VYUY:
        return swap ? eVYUY : eUYVY;
    case OVL_INPUT_FORMAT_YVYU:
        return swap ? eYVYU : eYUY2;
    case OVL_INPUT_FORMAT_RGBA4444:
        return swap ? eRGBA4444 : eBGRA4444;
    default:
        DDPERR("%s fmt=%d, swap=%d\n", __func__, fmt, swap);
        break;
    }
    return eRGB888;
}


static const char *ovl_intput_format_name(enum OVL_INPUT_FORMAT fmt, int swap)
{
    switch (fmt) {
    case OVL_INPUT_FORMAT_BGR565:
        return swap ? "eBGR565" : "eRGB565";
    case OVL_INPUT_FORMAT_RGB565:
        return "eRGB565";
    case OVL_INPUT_FORMAT_RGB888:
        return swap ? "eRGB888" : "eBGR888";
    case OVL_INPUT_FORMAT_BGR888:
        return "eBGR888";
    case OVL_INPUT_FORMAT_RGBA8888:
        return swap ? "eRGBA8888" : "eBGRA8888";
    case OVL_INPUT_FORMAT_BGRA8888:
        return "eBGRA8888";
    case OVL_INPUT_FORMAT_ARGB8888:
        return swap ? "eARGB8888" : "eABGR8888";
    case OVL_INPUT_FORMAT_ABGR8888:
        return "eABGR8888";
    case OVL_INPUT_FORMAT_VYUY:
        return swap ? "eVYUY" : "eUYVY";
    case OVL_INPUT_FORMAT_UYVY:
        return "eUYVY";
    case OVL_INPUT_FORMAT_YVYU:
        return swap ? "eYVYU" : "eYUY2";
    case OVL_INPUT_FORMAT_YUYV:
        return "eYUY2";
    case OVL_INPUT_FORMAT_RGBA4444:
        return swap ? "eRGBA4444" : "eBGRA4444";
    default:
        DDPERR("ovl_intput_fmt unknown fmt=%d, swap=%d\n", fmt, swap);
        break;
    }
    return "unknown";
}


static unsigned int ovl_index(enum DISP_MODULE_ENUM module)
{
    int idx = 0;
    unsigned long base = ovl_base_addr(module);
#if 0
    switch (module) {
    case DISP_MODULE_OVL0:
        idx = 0;
        break;
    case DISP_MODULE_OVL0_2L:
        idx = 1;
        break;
    case DISP_MODULE_OVL1:
        idx = 2;
        break;
    case DISP_MODULE_OVL1_2L:
        idx = 3;
        break;
    case DISP_MODULE_OVL2_2L:
        idx = 4;
        break;
    case DISP_MODULE_OVL3_2L:
        idx = 5;
        break;
    default:
        DDPERR("invalid module=%d\n", module);// invalid module
        ASSERT(0);
    }
#else
    idx = (base - DISPSYS_OVL0_BASE) / DISP_INDEX_OFFSET;
#endif
    return idx;
}

int OVLStart(enum DISP_MODULE_ENUM module, void *handle)
{
    int idx = ovl_index(module);

    DISP_REG_SET_FIELD(handle, EN_FLD_OVL_EN, idx*DISP_INDEX_OFFSET + DISP_REG_OVL_EN, 0x1);

    DISP_REG_SET(handle, idx*DISP_INDEX_OFFSET + DISP_REG_OVL_INTEN,
             0x1E | REG_FLD_VAL(INTEN_FLD_ABNORMAL_SOF, 1));

    DISP_REG_SET_FIELD(handle, FLD_RDMA_BURST_CON1_BURST16_EN,
               idx * DISP_INDEX_OFFSET +
               DISP_REG_OVL_RDMA_BURST_CON1, 1);
    DISP_REG_SET_FIELD(handle, DATAPATH_CON_FLD_LAYER_SMI_ID_EN,
               idx * DISP_INDEX_OFFSET +
               DISP_REG_OVL_DATAPATH_CON, 1);
    DISP_REG_SET_FIELD(handle, DATAPATH_CON_FLD_GCLAST_EN,
               idx * DISP_INDEX_OFFSET +
               DISP_REG_OVL_DATAPATH_CON, 1);
    DISP_REG_SET_FIELD(handle, DATAPATH_CON_FLD_OUTPUT_CLAMP,
               idx * DISP_INDEX_OFFSET +
               DISP_REG_OVL_DATAPATH_CON, 1);
    /* DISP_REG_SET_FIELD(handle, FLD_FBDC_8XE_MODE,
     *          idx * DISP_INDEX_OFFSET +
     *         DISP_REG_OVL_FBDC_CFG1, 1);
     */

#if 0
    DDPMSG("ovl start done idx = %d, addr = 0x%x\n", idx, idx*DISP_INDEX_OFFSET + DISP_REG_OVL_EN);
    DDPMSG("ovl start done addr0 = 0x%x\n", DISP_INDEX_OFFSET);
    DDPMSG("ovl start done addr1 = 0x%x\n", idx*DISP_INDEX_OFFSET);
    DDPMSG("ovl start done addr2 = 0x%x\n", DISP_REG_OVL_EN);
    DDPMSG("ovl start done addr3 = 0x%x\n", (idx*DISP_INDEX_OFFSET) + DISP_REG_OVL_EN);
    DDPMSG("ovl start done addr4 = 0x%x\n", idx*DISP_INDEX_OFFSET+DISP_REG_OVL_EN);
#endif

    return 0;
}

int OVLStop(enum DISP_MODULE_ENUM module, void *handle)
{
    int idx = ovl_index(module);

    DISP_REG_SET(handle, idx*DISP_INDEX_OFFSET+DISP_REG_OVL_INTEN, 0x00);
    DISP_REG_SET(handle, idx*DISP_INDEX_OFFSET+DISP_REG_OVL_EN, 0x00);
    DISP_REG_SET(handle, idx*DISP_INDEX_OFFSET+DISP_REG_OVL_INTSTA, 0x00);

    return 0;
}

int OVLReset(enum DISP_MODULE_ENUM module, void *handle)
{
#define OVL_IDLE (0x3)
    unsigned int delay_cnt = 0;
    int idx = ovl_index(module);
    /*always use cpu do reset*/
    DISP_CPU_REG_SET(idx*DISP_INDEX_OFFSET+DISP_REG_OVL_RST, 0x1);              // soft reset
    DISP_CPU_REG_SET(idx*DISP_INDEX_OFFSET+DISP_REG_OVL_RST, 0x0);
    while (!(DISP_REG_GET(idx*DISP_INDEX_OFFSET+DISP_REG_OVL_FLOW_CTRL_DBG) & OVL_IDLE)) {
        delay_cnt++;
        spin(10);
        if (delay_cnt > 2000) {
            DDPERR("OVL%dReset() timeout!\n", idx);
            break;
        }
    }
    return 0;
}

int OVLROI(enum DISP_MODULE_ENUM module,
           unsigned int bgW,
           unsigned int bgH,
           unsigned int bgColor,
           void *handle)
{
    int idx = ovl_index(module);

    if ((bgW > OVL_MAX_WIDTH) || (bgH > OVL_MAX_HEIGHT)) {
        DDPERR("%s, exceed OVL max size, w=%d, h=%d\n", __func__, bgW, bgH);
        ASSERT(0);
    }

    DISP_REG_SET(handle, idx*DISP_INDEX_OFFSET+DISP_REG_OVL_ROI_SIZE, bgH<<16 | bgW);

    DISP_REG_SET(handle, idx*DISP_INDEX_OFFSET+DISP_REG_OVL_ROI_BGCLR, bgColor);

    return 0;
}

int OVLLayerSwitch(enum DISP_MODULE_ENUM module,
                   unsigned int layer,
                   unsigned int en,
                   void *handle)
{
    int idx = ovl_index(module);
    unsigned int addr = idx * DISP_INDEX_OFFSET + DISP_REG_OVL_SRC_CON;

    ASSERT(layer <= 3);

    switch (layer) {
    case 0:
        DISP_REG_SET_FIELD(handle, SRC_CON_FLD_L0_EN, addr, en);
        break;
    case 1:
        DISP_REG_SET_FIELD(handle, SRC_CON_FLD_L1_EN, addr, en);
        break;
    case 2:
        DISP_REG_SET_FIELD(handle, SRC_CON_FLD_L2_EN, addr, en);
        break;
    case 3:
        DISP_REG_SET_FIELD(handle, SRC_CON_FLD_L3_EN, addr, en);
        break;
    default:
        DDPERR("invalid layer=%d\n", layer);           // invalid layer
        ASSERT(0);
    }

    return 0;
}

int OVL3DConfig(enum DISP_MODULE_ENUM module,
                unsigned int layer_id,
                unsigned int en_3d,
                unsigned int landscape,
                unsigned int r_first,
                void *handle)
{

    return 0;
}

int OVLLayerConfig(enum DISP_MODULE_ENUM module,
                   unsigned int layer,
                   unsigned int source,
                   enum DpColorFormat  format,
                   unsigned int addr,
                   unsigned int src_x,     // ROI x offset
                   unsigned int src_y,     // ROI y offset
                   unsigned int src_pitch,
                   unsigned int dst_x,     // ROI x offset
                   unsigned int dst_y,     // ROI y offset
                   unsigned int dst_w,     // ROT width
                   unsigned int dst_h,     // ROI height
                   unsigned int keyEn,
                   unsigned int key,   // color key
                   unsigned int aen,       // alpha enable
                   unsigned char alpha,
                   void *handle)
{

    int idx = ovl_index(module);
    unsigned int value = 0;
    enum OVL_INPUT_FORMAT fmt  = ovl_input_fmt_convert(format);
    unsigned int bpp           = ovl_input_fmt_bpp(fmt);
    unsigned int input_swap    = ovl_input_fmt_byte_swap(fmt);
    unsigned int input_fmt     = ovl_input_fmt_reg_value(fmt);
    enum OVL_COLOR_SPACE space = ovl_input_fmt_color_space(fmt);
    unsigned int offset = 0;
    /*0100 MTX_JPEG_TO_RGB (YUV FUll TO RGB)*/
    int color_matrix           = 0x4;

    unsigned int idx_offset  = idx*DISP_INDEX_OFFSET;
    unsigned int layer_offset = idx_offset + layer * 0x20;

#ifdef MTK_LCM_PHYSICAL_ROTATION_HW
    unsigned int bg_h, bg_w;
#endif

    ASSERT((dst_w <= OVL_MAX_WIDTH) &&
           (dst_h <= OVL_MAX_HEIGHT) &&
           (layer <= 3));

    if (addr == 0) {
        DDPERR("source from memory, but addr is 0!\n");
        ASSERT(0);
    }
    DDPDBG("ovl%d, layer=%d, source=%s, off(x=%d,y=%d), dst(%d,%d,%d,%d), pitch=%d, fmt=%s\n",
           idx,
           layer,
           (source == 0)?"memory":"constant_color",
           src_x,
           src_y,
           dst_x,
           dst_y,
           dst_w,
           dst_h,
           src_pitch,
           ovl_intput_format_name(fmt, input_swap));
    DDPDBG("addr=%x, keyEn=%d, key=%d, aen=%d, alpha=%d\n",
           addr,
           keyEn,
           key,
           aen,
           alpha);

    if (source == OVL_LAYER_SOURCE_RESERVED) { //==1, means constant color
        if (aen == 0)
            DDPERR("dim layer ahpha enable should be 1!\n");

        if (fmt != OVL_INPUT_FORMAT_RGB565) {
            DDPERR("dim layer format should be RGB565");
            fmt = OVL_INPUT_FORMAT_RGB565;
        }
    }

    DISP_REG_SET(handle, DISP_REG_OVL_RDMA0_CTRL+layer_offset, 0x1);

    value = (REG_FLD_VAL((L_CON_FLD_LARC), (source))     |
             REG_FLD_VAL((L_CON_FLD_CFMT), (input_fmt))  |
             REG_FLD_VAL((L_CON_FLD_AEN), (aen))         |
             REG_FLD_VAL((L_CON_FLD_APHA), (alpha))      |
             REG_FLD_VAL((L_CON_FLD_SKEN), (keyEn))     |
             REG_FLD_VAL((L_CON_FLD_BTSW), (input_swap)));

    if (fmt == OVL_INPUT_FORMAT_RGBA4444)
        value |= REG_FLD_VAL((L_CON_FLD_CLRFMT_MAN), (1));

    if (space == OVL_COLOR_SPACE_YUV)
        value = value | REG_FLD_VAL((L_CON_FLD_MTX), (color_matrix));

#ifdef MTK_LCM_PHYSICAL_ROTATION_HW
    value |= (REG_FLD_VAL((L_CON_FLD_VIRTICAL_FLIP), 1) |
              REG_FLD_VAL((L_CON_FLD_HORI_FLIP), 1));
#endif

    DISP_REG_SET(handle, DISP_REG_OVL_L0_CON+layer_offset, value);

#ifdef MTK_LCM_PHYSICAL_ROTATION_HW
    bg_h = DISP_REG_GET(idx_offset + DISP_REG_OVL_ROI_SIZE);
    bg_w = bg_h & 0xFFFF;
    bg_h = bg_h >> 16;
    DISP_REG_SET(handle, DISP_REG_OVL_L0_OFFSET+layer_offset,
                 (bg_h-dst_h-dst_y)<<16 | (bg_w-dst_w-dst_x));
    offset = (src_x+dst_w)*bpp + (src_y+dst_h-1)*src_pitch - 1;
#else
    DISP_REG_SET(handle, DISP_REG_OVL_L0_OFFSET+layer_offset,
                 dst_y<<16 | dst_x);
    offset = src_x*bpp+src_y*src_pitch;
#endif

    DISP_REG_SET(handle, DISP_REG_OVL_L0_ADDR+layer_offset,
                 addr+offset);
    DISP_REG_SET(handle, DISP_REG_OVL_L0_SRC_SIZE+layer_offset,
                 dst_h<<16 | dst_w);

    DISP_REG_SET(handle, DISP_REG_OVL_L0_SRCKEY+layer_offset,
                 key);

    value = REG_FLD_VAL((L_PITCH_FLD_LSP), (src_pitch));
    if (fmt == OVL_INPUT_FORMAT_RGBA4444) {
        value |= REG_FLD_VAL(L_PITCH_FLD_SRGB_SEL, (1)) |
            REG_FLD_VAL(L_PITCH_FLD_DRGB_SEL, (2)) |
            REG_FLD_VAL(L_PITCH_FLD_SURFL_EN, (1));
    }

    DISP_REG_SET(handle, DISP_REG_OVL_L0_PITCH+layer_offset, value);

    return 0;
}

static unsigned int ovl_input_fmt_byte_swap(enum OVL_INPUT_FORMAT inputFormat)
{
    int input_swap = 0;

    switch (inputFormat) {
    case OVL_INPUT_FORMAT_BGR565:
    case OVL_INPUT_FORMAT_RGB888:
    case OVL_INPUT_FORMAT_RGBA8888:
    case OVL_INPUT_FORMAT_ARGB8888:
    case OVL_INPUT_FORMAT_BGRA4444:
    case OVL_INPUT_FORMAT_VYUY:
    case OVL_INPUT_FORMAT_YVYU:
        input_swap = 1;
        break;
    case OVL_INPUT_FORMAT_RGB565:
    case OVL_INPUT_FORMAT_BGR888:
    case OVL_INPUT_FORMAT_BGRA8888:
    case OVL_INPUT_FORMAT_ABGR8888:
    case OVL_INPUT_FORMAT_RGBA4444:
    case OVL_INPUT_FORMAT_UYVY:
    case OVL_INPUT_FORMAT_YUYV:
        input_swap = 0;
        break;
    default:
        DDPERR("unknown input ovl format is %d\n", inputFormat);
        ASSERT(0);
    }
    return input_swap;
}

static unsigned int ovl_input_fmt_bpp(enum OVL_INPUT_FORMAT inputFormat)
{
    int bpp = 0;

    switch (inputFormat) {
    case OVL_INPUT_FORMAT_BGR565:
    case OVL_INPUT_FORMAT_RGB565:
    case OVL_INPUT_FORMAT_VYUY:
    case OVL_INPUT_FORMAT_UYVY:
    case OVL_INPUT_FORMAT_YVYU:
    case OVL_INPUT_FORMAT_YUYV:
    case OVL_INPUT_FORMAT_RGBA4444:
    case OVL_INPUT_FORMAT_BGRA4444:
        bpp = 2;
        break;
    case OVL_INPUT_FORMAT_RGB888:
    case OVL_INPUT_FORMAT_BGR888:
        bpp = 3;
        break;
    case OVL_INPUT_FORMAT_RGBA8888:
    case OVL_INPUT_FORMAT_BGRA8888:
    case OVL_INPUT_FORMAT_ARGB8888:
    case OVL_INPUT_FORMAT_ABGR8888:
        bpp = 4;
        break;
    default:
        DDPERR("unknown ovl input format = %d\n", inputFormat);
        ASSERT(0);
    }
    return  bpp;
}

static enum OVL_COLOR_SPACE ovl_input_fmt_color_space(enum OVL_INPUT_FORMAT inputFormat)
{
    enum OVL_COLOR_SPACE space = OVL_COLOR_SPACE_RGB;

    switch (inputFormat) {
    case OVL_INPUT_FORMAT_BGR565:
    case OVL_INPUT_FORMAT_RGB565:
    case OVL_INPUT_FORMAT_RGB888:
    case OVL_INPUT_FORMAT_BGR888:
    case OVL_INPUT_FORMAT_BGRA4444:
    case OVL_INPUT_FORMAT_RGBA4444:
    case OVL_INPUT_FORMAT_RGBA8888:
    case OVL_INPUT_FORMAT_BGRA8888:
    case OVL_INPUT_FORMAT_ARGB8888:
    case OVL_INPUT_FORMAT_ABGR8888:
        space = OVL_COLOR_SPACE_RGB;
        break;
    case OVL_INPUT_FORMAT_VYUY:
    case OVL_INPUT_FORMAT_UYVY:
    case OVL_INPUT_FORMAT_YVYU:
    case OVL_INPUT_FORMAT_YUYV:
        space = OVL_COLOR_SPACE_YUV;
        break;
    default:
        DDPERR("unknown ovl input format = %d\n", inputFormat);
        ASSERT(0);
    }
    return space;
}

static unsigned int ovl_input_fmt_reg_value(enum OVL_INPUT_FORMAT inputFormat)
{
    int reg_value = 0;

    switch (inputFormat) {
    case OVL_INPUT_FORMAT_BGR565:
    case OVL_INPUT_FORMAT_RGB565:
        reg_value = 0x0;
        break;
    case OVL_INPUT_FORMAT_RGB888:
    case OVL_INPUT_FORMAT_BGR888:
        reg_value = 0x1;
        break;
    case OVL_INPUT_FORMAT_RGBA8888:
    case OVL_INPUT_FORMAT_BGRA8888:
        reg_value = 0x2;
        break;
    case OVL_INPUT_FORMAT_ARGB8888:
    case OVL_INPUT_FORMAT_ABGR8888:
        reg_value = 0x3;
        break;
    case OVL_INPUT_FORMAT_VYUY:
    case OVL_INPUT_FORMAT_UYVY:
    case OVL_INPUT_FORMAT_RGBA4444:
    case OVL_INPUT_FORMAT_BGRA4444:
        reg_value = 0x4;
        break;
    case OVL_INPUT_FORMAT_YVYU:
    case OVL_INPUT_FORMAT_YUYV:
        reg_value = 0x5;
        break;
    default:
        DDPERR("unknown ovl input format is %d\n", inputFormat);
        ASSERT(0);
    }
    return reg_value;
}

void OVLRegStore(enum DISP_MODULE_ENUM module)
{
    int i = 0;
    int idx = ovl_index(module);

    const unsigned long regs[] = {
        //start
        DISP_REG_OVL_INTEN, DISP_REG_OVL_EN, DISP_REG_OVL_DATAPATH_CON,
        // roi
        DISP_REG_OVL_ROI_SIZE, DISP_REG_OVL_ROI_BGCLR,
        //layers enable
        DISP_REG_OVL_SRC_CON,
        //layer0
        DISP_REG_OVL_RDMA0_CTRL, DISP_REG_OVL_L0_CON, DISP_REG_OVL_L0_SRC_SIZE,
        DISP_REG_OVL_L0_OFFSET, DISP_REG_OVL_L0_ADDR, DISP_REG_OVL_L0_PITCH,
        DISP_REG_OVL_L0_SRCKEY,
        //layer1
        DISP_REG_OVL_RDMA1_CTRL, DISP_REG_OVL_L1_CON, DISP_REG_OVL_L1_SRC_SIZE,
        DISP_REG_OVL_L1_OFFSET, DISP_REG_OVL_L1_ADDR, DISP_REG_OVL_L1_PITCH,
        DISP_REG_OVL_L1_SRCKEY,
        //layer2
        DISP_REG_OVL_RDMA2_CTRL, DISP_REG_OVL_L2_CON, DISP_REG_OVL_L2_SRC_SIZE,
        DISP_REG_OVL_L2_OFFSET, DISP_REG_OVL_L2_ADDR, DISP_REG_OVL_L2_PITCH,
        DISP_REG_OVL_L2_SRCKEY,
        //layer3
        DISP_REG_OVL_RDMA3_CTRL, DISP_REG_OVL_L3_CON, DISP_REG_OVL_L3_SRC_SIZE,
        DISP_REG_OVL_L3_OFFSET, DISP_REG_OVL_L3_ADDR, DISP_REG_OVL_L3_PITCH,
        DISP_REG_OVL_L3_SRCKEY,
    };

    reg_back_cnt[idx] = sizeof(regs)/sizeof(unsigned long);
    ASSERT(reg_back_cnt[idx]  <= OVL_REG_BACK_MAX);


    for (i = 0; i < reg_back_cnt[idx]; i++) {
        reg_back[idx][i].address = regs[i];
        reg_back[idx][i].value   = DISP_REG_GET(regs[i]);
    }
    DDPDBG("store %c cnt registers on ovl %d", reg_back_cnt[idx], idx);

}

void OVLRegRestore(enum DISP_MODULE_ENUM module, void *handle)
{
    int idx = ovl_index(module);
    int i = reg_back_cnt[idx];

    while (i > 0) {
        i--;
        DISP_REG_SET(handle, reg_back[idx][i].address, reg_back[idx][i].value);
    }
    DDPDBG("restore %d cnt registers on ovl %d", reg_back_cnt[idx], idx);
    reg_back_cnt[idx] = 0;
}


int  OVLClockOn(enum DISP_MODULE_ENUM module, void *handle)
{
    int idx = ovl_index(module);

    ddp_enable_module_clock(module);
    DDPMSG("OVL%dClockOn CG 0x%x\n", idx, DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON0));
    return 0;
}

int  OVLClockOff(enum DISP_MODULE_ENUM module, void *handle)
{
    int idx = ovl_index(module);

    DDPMSG("OVL%dClockOff\n", idx);
    //store registers
    // OVLRegRestore(module, handle);
    DISP_REG_SET(handle, idx * DISP_INDEX_OFFSET + DISP_REG_OVL_EN, 0x00);
    OVLReset(module, handle);
    DISP_REG_SET(handle, idx*DISP_INDEX_OFFSET+DISP_REG_OVL_INTEN, 0x00);
    DISP_REG_SET(handle, idx * DISP_INDEX_OFFSET + DISP_REG_OVL_INTSTA, 0x00);
    ddp_disable_module_clock(module);
    return 0;
}

int  OVLInit(enum DISP_MODULE_ENUM module, void *handle)
{
    //power on, no need to care clock
    ddp_enable_module_clock(module);
    return 0;
}

int OVLDeInit(enum DISP_MODULE_ENUM module, void *handle)
{
    int idx = ovl_index(module);

    DDPMSG("OVL%dDeInit close CG\n", idx);
    ddp_disable_module_clock(module);
    return 0;
}

static inline int is_module_ovl(enum DISP_MODULE_ENUM module)
{
    if (module == DISP_MODULE_OVL0 ||
        module == DISP_MODULE_OVL0_2L ||
        module == DISP_MODULE_OVL1 ||
        module == DISP_MODULE_OVL1_2L)
        return 1;
    else
        return 0;
}

int OVLConnect(enum DISP_MODULE_ENUM module, enum DISP_MODULE_ENUM prev,
        enum DISP_MODULE_ENUM next, int connect, void *handle)
{
    int idx = ovl_index(module);

    if (connect && is_module_ovl(prev))
        DISP_REG_SET_FIELD(handle, DATAPATH_CON_FLD_BGCLR_IN_SEL,
                   idx * DISP_INDEX_OFFSET + DISP_REG_OVL_DATAPATH_CON, 1);
    else
        DISP_REG_SET_FIELD(handle, DATAPATH_CON_FLD_BGCLR_IN_SEL,
                   idx * DISP_INDEX_OFFSET + DISP_REG_OVL_DATAPATH_CON, 0);

    if (connect && is_module_ovl(next)) {
        switch (module) {
        case DISP_MODULE_OVL0:
            DISP_REG_SET_FIELD(handle, FLD_CON_OVL0, DISP_REG_CONFIG_MMSYS_OVL_CON, 0x2);
            break;
        case DISP_MODULE_OVL0_2L:
            DISP_REG_SET_FIELD(handle, FLD_CON_OVL0_2L, DISP_REG_CONFIG_MMSYS_OVL_CON, 0x2);
            break;
        case DISP_MODULE_OVL1:
            DISP_REG_SET_FIELD(handle, FLD_CON_OVL1, DISP_REG_CONFIG_MMSYS_OVL_CON, 0x2);
            break;
        case DISP_MODULE_OVL1_2L:
            DISP_REG_SET_FIELD(handle, FLD_CON_OVL1_2L, DISP_REG_CONFIG_MMSYS_OVL_CON, 0x2);
            break;
        case DISP_MODULE_OVL2_2L:
            break;
        case DISP_MODULE_OVL3_2L:
            DISP_REG_SET_FIELD(handle, FLD_CON_OVL3_2L, DISP_REG_CONFIG_MMSYS_OVL_CON, 0x2);
            break;
        default:
            DDPERR("invalid module=%d\n", module);// invalid module
            ASSERT(0);
        }
    } else {
        switch (module) {
        case DISP_MODULE_OVL0:
            DISP_REG_SET_FIELD(handle, FLD_CON_OVL0, DISP_REG_CONFIG_MMSYS_OVL_CON, 0x1);
            break;
        case DISP_MODULE_OVL0_2L:
            DISP_REG_SET_FIELD(handle, FLD_CON_OVL0_2L, DISP_REG_CONFIG_MMSYS_OVL_CON, 0x1);
            break;
        case DISP_MODULE_OVL1:
            DISP_REG_SET_FIELD(handle, FLD_CON_OVL1, DISP_REG_CONFIG_MMSYS_OVL_CON, 0x1);
            break;
        case DISP_MODULE_OVL1_2L:
            DISP_REG_SET_FIELD(handle, FLD_CON_OVL1_2L, DISP_REG_CONFIG_MMSYS_OVL_CON, 0x1);
            break;
        case DISP_MODULE_OVL2_2L:
            break;
        case DISP_MODULE_OVL3_2L:
            DISP_REG_SET_FIELD(handle, FLD_CON_OVL3_2L, DISP_REG_CONFIG_MMSYS_OVL_CON, 0x1);
            break;
        default:
            DDPERR("invalid module=%d\n", module);// invalid module
            ASSERT(0);
        }
    }
    return 0;
}

void ovl_dump_golden_setting(enum DISP_MODULE_ENUM module)
{
    unsigned long base = ovl_base_addr(module);

    base -= DISPSYS_OVL0_BASE;

    DDPDUMP("-- %s Golden Setting --\n", ddp_get_module_name(module));
    DDPDUMP("RDMA0_MEM_GMC_SETTING1\n");
    DDPDUMP("[9:0]:%x [25:16]:%x [28]:%x [31]:%x\n",
        DISP_REG_GET_FIELD(FLD_OVL_RDMA_MEM_GMC_ULTRA_THRESHOLD,
        base + DISP_REG_OVL_RDMA0_MEM_GMC_SETTING),
        DISP_REG_GET_FIELD(FLD_OVL_RDMA_MEM_GMC_PRE_ULTRA_THRESHOLD,
        base + DISP_REG_OVL_RDMA0_MEM_GMC_SETTING),
        DISP_REG_GET_FIELD(
            FLD_OVL_RDMA_MEM_GMC_ULTRA_THRESHOLD_HIGH_OFS,
        base + DISP_REG_OVL_RDMA0_MEM_GMC_SETTING),
        DISP_REG_GET_FIELD(
            FLD_OVL_RDMA_MEM_GMC_PRE_ULTRA_THRESHOLD_HIGH_OFS,
        base + DISP_REG_OVL_RDMA0_MEM_GMC_SETTING));


    DDPDUMP("RDMA0_FIFO_CTRL\n");
    DDPDUMP("[9:0]:%u [25:16]:%u\n",
        DISP_REG_GET_FIELD(FLD_OVL_RDMA_FIFO_THRD,
            base + DISP_REG_OVL_RDMA0_FIFO_CTRL),
        DISP_REG_GET_FIELD(FLD_OVL_RDMA_FIFO_SIZE,
            base + DISP_REG_OVL_RDMA0_FIFO_CTRL));

    DDPDUMP("RDMA0_MEM_GMC_SETTING2\n");
    DDPDUMP("[11:0]:%u [27:16]:%u [28]:%u [29]:%u [30]:%u\n",
        DISP_REG_GET_FIELD(FLD_OVL_RDMA_MEM_GMC2_ISSUE_REQ_THRES,
            base + DISP_REG_OVL_RDMA0_MEM_GMC_S2),
        DISP_REG_GET_FIELD(FLD_OVL_RDMA_MEM_GMC2_ISSUE_REQ_THRES_URG,
            base + DISP_REG_OVL_RDMA0_MEM_GMC_S2),
        DISP_REG_GET_FIELD(FLD_OVL_RDMA_MEM_GMC2_REQ_THRES_PREULTRA,
            base + DISP_REG_OVL_RDMA0_MEM_GMC_S2),
        DISP_REG_GET_FIELD(FLD_OVL_RDMA_MEM_GMC2_REQ_THRES_ULTRA,
            base + DISP_REG_OVL_RDMA0_MEM_GMC_S2),
        DISP_REG_GET_FIELD(FLD_OVL_RDMA_MEM_GMC2_FORCE_REQ_THRES,
            base + DISP_REG_OVL_RDMA0_MEM_GMC_S2));

    DDPDUMP("OVL_RDMA_BURST_CON1\n");
    DDPDUMP("[28]:%u\n",
        DISP_REG_GET_FIELD(FLD_RDMA_BURST_CON1_BURST16_EN,
            base + DISP_REG_OVL_RDMA_BURST_CON1));

    DDPDUMP("RDMA_GREQ_NUM\n");
    DDPDUMP("[3:0]%u [7:4]%u [11:8]%u [15:12]%u [23:16]%x [26:24]%u\n",
        DISP_REG_GET_FIELD(FLD_OVL_RDMA_GREQ_LAYER0_GREQ_NUM,
            base + DISP_REG_OVL_RDMA_GREQ_NUM),
        DISP_REG_GET_FIELD(FLD_OVL_RDMA_GREQ_LAYER1_GREQ_NUM,
            base + DISP_REG_OVL_RDMA_GREQ_NUM),
        DISP_REG_GET_FIELD(FLD_OVL_RDMA_GREQ_LAYER2_GREQ_NUM,
            base + DISP_REG_OVL_RDMA_GREQ_NUM),
        DISP_REG_GET_FIELD(FLD_OVL_RDMA_GREQ_LAYER3_GREQ_NUM,
            base + DISP_REG_OVL_RDMA_GREQ_NUM),
        DISP_REG_GET_FIELD(FLD_OVL_RDMA_GREQ_OSTD_GREQ_NUM,
            base + DISP_REG_OVL_RDMA_GREQ_NUM),
        DISP_REG_GET_FIELD(FLD_OVL_RDMA_GREQ_GREQ_DIS_CNT,
            base + DISP_REG_OVL_RDMA_GREQ_NUM));

    DDPDUMP("[27]%u [28]%u [29]%u [30]%u [31]%u\n",
        DISP_REG_GET_FIELD(FLD_OVL_RDMA_GREQ_STOP_EN,
            base + DISP_REG_OVL_RDMA_GREQ_NUM),
        DISP_REG_GET_FIELD(FLD_OVL_RDMA_GREQ_GRP_END_STOP,
            base + DISP_REG_OVL_RDMA_GREQ_NUM),
        DISP_REG_GET_FIELD(FLD_OVL_RDMA_GREQ_GRP_BRK_STOP,
            base + DISP_REG_OVL_RDMA_GREQ_NUM),
        DISP_REG_GET_FIELD(FLD_OVL_RDMA_GREQ_IOBUF_FLUSH_PREULTRA,
            base + DISP_REG_OVL_RDMA_GREQ_NUM),
        DISP_REG_GET_FIELD(FLD_OVL_RDMA_GREQ_IOBUF_FLUSH_ULTRA,
            base + DISP_REG_OVL_RDMA_GREQ_NUM));

    DDPDUMP("RDMA_GREQ_URG_NUM\n");
    DDPDUMP("[3:0]:%u [7:4]:%u [11:8]:%u [15:12]:%u [25:16]:%u [28]:%u\n",
        DISP_REG_GET_FIELD(FLD_OVL_RDMA_GREQ_LAYER0_GREQ_URG_NUM,
            base + DISP_REG_OVL_RDMA_GREQ_URG_NUM),
        DISP_REG_GET_FIELD(FLD_OVL_RDMA_GREQ_LAYER1_GREQ_URG_NUM,
            base + DISP_REG_OVL_RDMA_GREQ_URG_NUM),
        DISP_REG_GET_FIELD(FLD_OVL_RDMA_GREQ_LAYER2_GREQ_URG_NUM,
            base + DISP_REG_OVL_RDMA_GREQ_URG_NUM),
        DISP_REG_GET_FIELD(FLD_OVL_RDMA_GREQ_LAYER3_GREQ_URG_NUM,
            base + DISP_REG_OVL_RDMA_GREQ_URG_NUM),
        DISP_REG_GET_FIELD(FLD_OVL_RDMA_GREQ_ARG_GREQ_URG_TH,
            base + DISP_REG_OVL_RDMA_GREQ_URG_NUM),
        DISP_REG_GET_FIELD(FLD_OVL_RDMA_GREQ_ARG_URG_BIAS,
                base + DISP_REG_OVL_RDMA_GREQ_URG_NUM));

    DDPDUMP("RDMA_ULTRA_SRC\n");
    DDPDUMP("[1:0]%u [3:2]%u [5:4]%u [7:6]%u [9:8]%u\n",
        DISP_REG_GET_FIELD(FLD_OVL_RDMA_PREULTRA_BUF_SRC,
            base + DISP_REG_OVL_RDMA_ULTRA_SRC),
        DISP_REG_GET_FIELD(FLD_OVL_RDMA_PREULTRA_SMI_SRC,
            base + DISP_REG_OVL_RDMA_ULTRA_SRC),
        DISP_REG_GET_FIELD(FLD_OVL_RDMA_PREULTRA_ROI_END_SRC,
            base + DISP_REG_OVL_RDMA_ULTRA_SRC),
        DISP_REG_GET_FIELD(FLD_OVL_RDMA_PREULTRA_RDMA_SRC,
            base + DISP_REG_OVL_RDMA_ULTRA_SRC),
        DISP_REG_GET_FIELD(FLD_OVL_RDMA_ULTRA_BUF_SRC,
            base + DISP_REG_OVL_RDMA_ULTRA_SRC));
    DDPDUMP("[11:10]%u [13:12]%u [15:14]%u\n",
        DISP_REG_GET_FIELD(FLD_OVL_RDMA_ULTRA_SMI_SRC,
            base + DISP_REG_OVL_RDMA_ULTRA_SRC),
        DISP_REG_GET_FIELD(FLD_OVL_RDMA_ULTRA_ROI_END_SRC,
            base + DISP_REG_OVL_RDMA_ULTRA_SRC),
        DISP_REG_GET_FIELD(FLD_OVL_RDMA_ULTRA_RDMA_SRC,
            base + DISP_REG_OVL_RDMA_ULTRA_SRC));


    DDPDUMP("RDMA0_BUF_LOW\n");
    DDPDUMP("[11:0]:%x [23:12]:%x\n",
        DISP_REG_GET_FIELD(FLD_OVL_RDMA_BUF_LOW_ULTRA_TH,
            base + DISP_REG_OVL_RDMAn_BUF_LOW(0)),
        DISP_REG_GET_FIELD(FLD_OVL_RDMA_BUF_LOW_PREULTRA_TH,
            base + DISP_REG_OVL_RDMAn_BUF_LOW(0)));

    DDPDUMP("RDMA0_BUF_HIGH\n");
    DDPDUMP("[23:12]:%x [31]:%x\n",
        DISP_REG_GET_FIELD(FLD_OVL_RDMA_BUF_HIGH_PREULTRA_TH,
            base + DISP_REG_OVL_RDMAn_BUF_HIGH(0)),
        DISP_REG_GET_FIELD(FLD_OVL_RDMA_BUF_HIGH_PREULTRA_DIS,
            base + DISP_REG_OVL_RDMAn_BUF_HIGH(0)));

    DDPDUMP("OVL_EN\n");
    DDPDUMP("[18]:%x [19]:%x\n",
        DISP_REG_GET_FIELD(EN_FLD_BLOCK_EXT_ULTRA,
            base + DISP_REG_OVL_EN),
        DISP_REG_GET_FIELD(EN_FLD_BLOCK_EXT_PREULTRA,
            base + DISP_REG_OVL_EN));


    DDPDUMP("DATAPATH_CON\n");
    DDPDUMP("[0]:%u, [3]:%u [24]:%u [25]:%u [26]:%u\n",
        DISP_REG_GET_FIELD(DATAPATH_CON_FLD_LAYER_SMI_ID_EN,
            base + DISP_REG_OVL_DATAPATH_CON),
        DISP_REG_GET_FIELD(DATAPATH_CON_FLD_OUTPUT_NO_RND,
            base + DISP_REG_OVL_DATAPATH_CON),
        DISP_REG_GET_FIELD(DATAPATH_CON_FLD_GCLAST_EN,
            base + DISP_REG_OVL_DATAPATH_CON),
        DISP_REG_GET_FIELD(DATAPATH_CON_FLD_HDR_GCLAST_EN,
            base + DISP_REG_OVL_DATAPATH_CON),
        DISP_REG_GET_FIELD(DATAPATH_CON_FLD_OUTPUT_CLAMP,
            base + DISP_REG_OVL_DATAPATH_CON));

    DDPDUMP("OVL_FBDC_CFG1\n");
    DDPDUMP("[24]:%u\n", DISP_REG_GET_FIELD(FLD_FBDC_8XE_MODE,
            base + DISP_REG_OVL_FBDC_CFG1));
}

void OVLDump(enum DISP_MODULE_ENUM module)
{
#if 0
    int idx = ovl_index(module);
    unsigned long module_base = 0;
    const char *module_name = ddp_get_module_name(module);

    module_base = idx * DISP_INDEX_OFFSET + DISP_OVL0_OVL_STA;

    DDPDUMP("== START: DISP %s registers ==\n", module_name);
    DDPDUMP("%s 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        module_name,
        0x0, readl(module_base + 0x0),
        0x4, readl(module_base + 0x4),
        0x8, readl(module_base + 0x8),
        0xC, readl(module_base + 0xC));
    DDPDUMP("%s 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        module_name,
        0x10, readl(module_base + 0x10),
        0x14, readl(module_base + 0x14),
        0x20, readl(module_base + 0x20),
        0x24, readl(module_base + 0x24));
    DDPDUMP("%s 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        module_name,
        0x28, readl(module_base + 0x28),
        0x2C, readl(module_base + 0x2C),
        0x30, readl(module_base + 0x30),
        0x34, readl(module_base + 0x34));
    DDPDUMP("%s 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        module_name,
        0x38, readl(module_base + 0x38),
        0x3C, readl(module_base + 0x3C),
        0xF40, readl(module_base + 0xF40),
        0x44, readl(module_base + 0x44));
    DDPDUMP("%s 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        module_name,
        0x48, readl(module_base + 0x48),
        0x4C, readl(module_base + 0x4C),
        0x50, readl(module_base + 0x50),
        0x54, readl(module_base + 0x54));
    DDPDUMP("%s 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        module_name,
        0x58, readl(module_base + 0x58),
        0x5C, readl(module_base + 0x5C),
        0xF60, readl(module_base + 0xF60),
        0x64, readl(module_base + 0x64));
    DDPDUMP("%s 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        module_name,
        0x68, readl(module_base + 0x68),
        0x6C, readl(module_base + 0x6C),
        0x70, readl(module_base + 0x70),
        0x74, readl(module_base + 0x74));
    DDPDUMP("%s 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        module_name,
        0x78, readl(module_base + 0x78),
        0x7C, readl(module_base + 0x7C),
        0xF80, readl(module_base + 0xF80),
        0x84, readl(module_base + 0x84));
    DDPDUMP("%s 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        module_name,
        0x88, readl(module_base + 0x88),
        0x8C, readl(module_base + 0x8C),
        0x90, readl(module_base + 0x90),
        0x94, readl(module_base + 0x94));
    DDPDUMP("%s 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        module_name,
        0x98, readl(module_base + 0x98),
        0x9C, readl(module_base + 0x9C),
        0xFa0, readl(module_base + 0xFa0),
        0xa4, readl(module_base + 0xa4));
    DDPDUMP("%s 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        module_name,
        0xa8, readl(module_base + 0xa8),
        0xAC, readl(module_base + 0xAC),
        0xc0, readl(module_base + 0xc0),
        0xc8, readl(module_base + 0xc8));
    DDPDUMP("%s 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        module_name,
        0xcc, readl(module_base + 0xcc),
        0xd0, readl(module_base + 0xd0),
        0xe0, readl(module_base + 0xe0),
        0xe8, readl(module_base + 0xe8));
    DDPDUMP("%s 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        module_name,
        0xec, readl(module_base + 0xec),
        0xf0, readl(module_base + 0xf0),
        0x100, readl(module_base + 0x100),
        0x108, readl(module_base + 0x108));
    DDPDUMP("%s 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        module_name,
        0x10c, readl(module_base + 0x10c),
        0x110, readl(module_base + 0x110),
        0x120, readl(module_base + 0x120),
        0x128, readl(module_base + 0x128));
    DDPDUMP("%s 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        module_name,
        0x12c, readl(module_base + 0x12c),
        0x130, readl(module_base + 0x130),
        0x134, readl(module_base + 0x134),
        0x138, readl(module_base + 0x138));
    DDPDUMP("%s 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        module_name,
        0x13c, readl(module_base + 0x13c),
        0x140, readl(module_base + 0x140),
        0x144, readl(module_base + 0x144),
        0x148, readl(module_base + 0x148));
    DDPDUMP("%s 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        module_name,
        0x14c, readl(module_base + 0x14c),
        0x150, readl(module_base + 0x150),
        0x154, readl(module_base + 0x154),
        0x158, readl(module_base + 0x158));
    DDPDUMP("%s 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        module_name,
        0x15c, readl(module_base + 0x15c),
        0x160, readl(module_base + 0x160),
        0x164, readl(module_base + 0x164),
        0x168, readl(module_base + 0x168));
    DDPDUMP("%s 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        module_name,
        0x16c, readl(module_base + 0x16c),
        0x170, readl(module_base + 0x170),
        0x174, readl(module_base + 0x174),
        0x178, readl(module_base + 0x178));
    DDPDUMP("%s 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        module_name,
        0x17c, readl(module_base + 0x17c),
        0x180, readl(module_base + 0x180),
        0x184, readl(module_base + 0x184),
        0x188, readl(module_base + 0x188));
    DDPDUMP("%s 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        module_name,
        0x18c, readl(module_base + 0x18c),
        0x190, readl(module_base + 0x190),
        0x194, readl(module_base + 0x194),
        0x198, readl(module_base + 0x198));
    DDPDUMP("%s 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        module_name,
        0x19c, readl(module_base + 0x19c),
        0x1a0, readl(module_base + 0x1a0),
        0x1a4, readl(module_base + 0x1a4),
        0x1a8, readl(module_base + 0x1a8));
    DDPDUMP("%s 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        module_name,
        0x1ac, readl(module_base + 0x1ac),
        0x1b0, readl(module_base + 0x1b0),
        0x1b4, readl(module_base + 0x1b4),
        0x1b8, readl(module_base + 0x1b8));
    DDPDUMP("%s 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        module_name,
        0x1bc, readl(module_base + 0x1bc),
        0x1c0, readl(module_base + 0x1c0),
        0x1c4, readl(module_base + 0x1c4),
        0x1c8, readl(module_base + 0x1c8));
    DDPDUMP("%s 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        module_name,
        0x1cc, readl(module_base + 0x1cc),
        0x1d0, readl(module_base + 0x1d0),
        0x1d4, readl(module_base + 0x1d4),
        0x1dc, readl(module_base + 0x1dc));
    DDPDUMP("%s 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        module_name,
        0x1e0, readl(module_base + 0x1e0),
        0x1e4, readl(module_base + 0x1e4),
        0x1e8, readl(module_base + 0x1e8),
        0x1ec, readl(module_base + 0x1ec));
    DDPDUMP("%s 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        module_name,
        0x1F0, readl(module_base + 0x1F0),
        0x1F4, readl(module_base + 0x1F4),
        0x1F8, readl(module_base + 0x1F8),
        0x1FC, readl(module_base + 0x1FC));
    DDPDUMP("%s 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        module_name,
        0x200, readl(module_base + 0x200),
        0x208, readl(module_base + 0x208),
        0x20C, readl(module_base + 0x20C),
        0x210, readl(module_base + 0x210));
    DDPDUMP("%s 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        module_name,
        0x214, readl(module_base + 0x214),
        0x218, readl(module_base + 0x218),
        0x21C, readl(module_base + 0x21C),
        0x220, readl(module_base + 0x220));
    DDPDUMP("%s 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        module_name,
        0x224, readl(module_base + 0x224),
        0x228, readl(module_base + 0x228),
        0x22C, readl(module_base + 0x22C),
        0x230, readl(module_base + 0x230));
    DDPDUMP("%s 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        module_name,
        0x234, readl(module_base + 0x234),
        0x238, readl(module_base + 0x238),
        0x240, readl(module_base + 0x240),
        0x244, readl(module_base + 0x244));
    DDPDUMP("%s 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        module_name,
        0x24c, readl(module_base + 0x24c),
        0x250, readl(module_base + 0x250),
        0x254, readl(module_base + 0x254),
        0x258, readl(module_base + 0x258));
    DDPDUMP("%s 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        module_name,
        0x25c, readl(module_base + 0x25c),
        0x260, readl(module_base + 0x260),
        0x264, readl(module_base + 0x264),
        0x268, readl(module_base + 0x268));
    DDPDUMP("%s 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        module_name,
        0x26C, readl(module_base + 0x26C),
        0x270, readl(module_base + 0x270),
        0x280, readl(module_base + 0x280),
        0x284, readl(module_base + 0x284));
    DDPDUMP("%s 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        module_name,
        0x288, readl(module_base + 0x288),
        0x28C, readl(module_base + 0x28C),
        0x290, readl(module_base + 0x290),
        0x29C, readl(module_base + 0x29C));
    DDPDUMP("%s 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        module_name,
        0x2A0, readl(module_base + 0x2A0),
        0x2A4, readl(module_base + 0x2A4),
        0x2B0, readl(module_base + 0x2B0),
        0x2B4, readl(module_base + 0x2B4));
    DDPDUMP("%s 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        module_name,
        0x2B8, readl(module_base + 0x2B8),
        0x2BC, readl(module_base + 0x2BC),
        0x2C0, readl(module_base + 0x2C0),
        0x2C4, readl(module_base + 0x2C4));
    DDPDUMP("%s 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        module_name,
        0x2C8, readl(module_base + 0x2C8),
        0x324, readl(module_base + 0x324),
        0x330, readl(module_base + 0x330),
        0x334, readl(module_base + 0x334));
    DDPDUMP("%s 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        module_name,
        0x338, readl(module_base + 0x338),
        0x33C, readl(module_base + 0x33C),
        0xFB0, readl(module_base + 0xFB0),
        0x344, readl(module_base + 0x344));
    DDPDUMP("%s 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        module_name,
        0x348, readl(module_base + 0x348),
        0x34C, readl(module_base + 0x34C),
        0x350, readl(module_base + 0x350),
        0x354, readl(module_base + 0x354));
    DDPDUMP("%s 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        module_name,
        0x358, readl(module_base + 0x358),
        0x35C, readl(module_base + 0x35C),
        0xFB4, readl(module_base + 0xFB4),
        0x364, readl(module_base + 0x364));
    DDPDUMP("%s 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        module_name,
        0x368, readl(module_base + 0x368),
        0x36C, readl(module_base + 0x36C),
        0x370, readl(module_base + 0x370),
        0x374, readl(module_base + 0x374));
    DDPDUMP("%s 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        module_name,
        0x378, readl(module_base + 0x378),
        0x37C, readl(module_base + 0x37C),
        0xFB8, readl(module_base + 0xFB8),
        0x384, readl(module_base + 0x384));
    DDPDUMP("%s 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        module_name,
        0x388, readl(module_base + 0x388),
        0x38C, readl(module_base + 0x38C),
        0x390, readl(module_base + 0x390),
        0x394, readl(module_base + 0x394));
    DDPDUMP("%s 0x%04x=0x%08x, 0x%04x=0x%08x\n",
        module_name,
        0x398, readl(module_base + 0x398),
        0xFC0, readl(module_base + 0xFC0));
    DDPDUMP("-- END: DISP %s registers --\n", module_name);
#endif

    ovl_dump_golden_setting(module);
}

static void ovl_printf_status(unsigned int status)
{
    DDPDUMP("=OVL_FLOW_CONTROL_DEBUG=:\n");
    DDPDUMP("addcon_idle:%d,blend_idle:%d,out_valid:%d,out_ready:%d,out_idle:%d\n",
        (status >> 10) & (0x1),
        (status >> 11) & (0x1),
        (status >> 12) & (0x1), (status >> 13) & (0x1), (status >> 15) & (0x1)
        );
    DDPDUMP("rdma3_idle:%d,rdma2_idle:%d,rdma1_idle:%d, rdma0_idle:%d,rst:%d\n",
        (status >> 16) & (0x1),
        (status >> 17) & (0x1),
        (status >> 18) & (0x1), (status >> 19) & (0x1), (status >> 20) & (0x1)
        );
    DDPDUMP("trig:%d,frame_hwrst_done:%d,frame_swrst_done:%d,frame_underrun:%d,frame_done:%d\n",
        (status >> 21) & (0x1),
        (status >> 23) & (0x1),
        (status >> 24) & (0x1), (status >> 25) & (0x1), (status >> 26) & (0x1)
        );
    DDPDUMP("ovl_running:%d,ovl_start:%d,ovl_clr:%d,reg_update:%d,ovl_upd_reg:%d\n",
        (status >> 27) & (0x1),
        (status >> 28) & (0x1),
        (status >> 29) & (0x1), (status >> 30) & (0x1), (status >> 31) & (0x1)
        );

    DDPDUMP("ovl_fms_state:\n");
    switch (status & 0x3ff) {
    case 0x1:
        DDPDUMP("idle\n");
        break;
    case 0x2:
        DDPDUMP("wait_SOF\n");
        break;
    case 0x4:
        DDPDUMP("prepare\n");
        break;
    case 0x8:
        DDPDUMP("reg_update\n");
        break;
    case 0x10:
        DDPDUMP("eng_clr(internal reset)\n");
        break;
    case 0x20:
        DDPDUMP("eng_act(processing)\n");
        break;
    case 0x40:
        DDPDUMP("h_wait_w_rst\n");
        break;
    case 0x80:
        DDPDUMP("s_wait_w_rst\n");
        break;
    case 0x100:
        DDPDUMP("h_w_rst\n");
        break;
    case 0x200:
        DDPDUMP("s_w_rst\n");
        break;
    default:
        DDPDUMP("ovl_fsm_unknown\n");
        break;
    }

}

static void ovl_print_ovl_rdma_status(unsigned int status)
{
    DDPDUMP("wram_rst_cs:0x%x,layer_greq:0x%x,out_data:0x%x,",
        status & 0x7, (status >> 3) & 0x1, (status >> 4) & 0xffffff);
    DDPDUMP("out_ready:0x%x,out_valid:0x%x,smi_busy:0x%x,smi_greq:0x%x\n",
        (status >> 28) & 0x1, (status >> 29) & 0x1, (status >> 30) & 0x1,
        (status >> 31) & 0x1);
}


static void ovl_dump_layer_info(int layer, unsigned long layer_offset)
{
    DDPDUMP("layer%d: w=%d,h=%d,x=%d,y=%d,pitch=%d,addr=0x%x,fmt=%s,source=%s,aen=%d,alpha=%d\n",
         layer, DISP_REG_GET(layer_offset + DISP_REG_OVL_L0_SRC_SIZE) & 0xfff,
         (DISP_REG_GET(layer_offset + DISP_REG_OVL_L0_SRC_SIZE) >> 16) & 0xfff,
         DISP_REG_GET(layer_offset + DISP_REG_OVL_L0_OFFSET) & 0xfff,
         (DISP_REG_GET(layer_offset + DISP_REG_OVL_L0_OFFSET) >> 16) & 0xfff,
         DISP_REG_GET(layer_offset + DISP_REG_OVL_L0_PITCH) & 0xffff,
         DISP_REG_GET(layer_offset + DISP_REG_OVL_L0_ADDR),
         ovl_intput_format_name(
                       DISP_REG_GET_FIELD(L_CON_FLD_CFMT, DISP_REG_OVL_L0_CON+layer_offset),
                       DISP_REG_GET_FIELD(L_CON_FLD_BTSW, DISP_REG_OVL_L0_CON+layer_offset)),

         (DISP_REG_GET_FIELD(L_CON_FLD_LARC, DISP_REG_OVL_L0_CON + layer_offset) ==
          0) ? "memory" : "constant_color", DISP_REG_GET_FIELD(L_CON_FLD_AEN,
                                   DISP_REG_OVL_L0_CON +
                                   layer_offset),

         DISP_REG_GET_FIELD(L_CON_FLD_APHA, DISP_REG_OVL_L0_CON + layer_offset)
        );
}


void ovl_dump_analysis(enum DISP_MODULE_ENUM module)
{
    int i = 0;
    unsigned int layer_offset = 0;
    unsigned int rdma_offset = 0;
    int idx = ovl_index(module);
    unsigned int offset = idx * DISP_INDEX_OFFSET;
    unsigned int src_on = DISP_REG_GET(DISP_REG_OVL_SRC_CON+offset);
    unsigned int rdma_ctrl;

    DDPDUMP("==DISP %s ANALYSIS==\n", ddp_get_module_name(module));
    DDPDUMP("ovl_en=%d,layer_enable(%d,%d,%d,%d),bg(w=%d, h=%d),",
        DISP_REG_GET(DISP_REG_OVL_EN + offset),
        DISP_REG_GET(DISP_REG_OVL_SRC_CON + offset) & 0x1,
        (DISP_REG_GET(DISP_REG_OVL_SRC_CON + offset) >> 1) & 0x1,
        (DISP_REG_GET(DISP_REG_OVL_SRC_CON + offset) >> 2) & 0x1,
        (DISP_REG_GET(DISP_REG_OVL_SRC_CON + offset) >> 3) & 0x1,
        DISP_REG_GET(DISP_REG_OVL_ROI_SIZE + offset) & 0xfff,
        (DISP_REG_GET(DISP_REG_OVL_ROI_SIZE + offset) >> 16) & 0xfff);
    DDPDUMP("cur_pos(x=%d,y=%d),layer_hit(%d,%d,%d,%d),bg_mode=%s,sta=0x%x\n",
        DISP_REG_GET_FIELD(ADDCON_DBG_FLD_ROI_X, DISP_REG_OVL_ADDCON_DBG + offset),
        DISP_REG_GET_FIELD(ADDCON_DBG_FLD_ROI_Y, DISP_REG_OVL_ADDCON_DBG + offset),
        DISP_REG_GET_FIELD(ADDCON_DBG_FLD_L0_WIN_HIT, DISP_REG_OVL_ADDCON_DBG + offset),
        DISP_REG_GET_FIELD(ADDCON_DBG_FLD_L1_WIN_HIT, DISP_REG_OVL_ADDCON_DBG + offset),
        DISP_REG_GET_FIELD(ADDCON_DBG_FLD_L2_WIN_HIT, DISP_REG_OVL_ADDCON_DBG + offset),
        DISP_REG_GET_FIELD(ADDCON_DBG_FLD_L3_WIN_HIT, DISP_REG_OVL_ADDCON_DBG + offset),
        DISP_REG_GET_FIELD(DATAPATH_CON_FLD_BGCLR_IN_SEL,
                   DISP_REG_OVL_DATAPATH_CON + offset) ? "directlink" : "const",
        DISP_REG_GET(DISP_REG_OVL_STA + offset)
        );
    for (i = 0; i < 4; i++) {
        layer_offset = i * OVL_LAYER_OFFSET + offset;
        rdma_offset = i * OVL_RDMA_DEBUG_OFFSET + offset;
        if (src_on & (0x1 << i))
            ovl_dump_layer_info(i, layer_offset);
        else
            DDPDUMP("layer%d: disabled\n", i);

        rdma_ctrl = DISP_REG_GET(layer_offset + DISP_REG_OVL_RDMA0_CTRL);

        DDPDUMP("ovl rdma%d status:(en=%d, fifo_used %d, GMC=0x%x)\n", i,
            REG_FLD_VAL_GET(RDMA0_CTRL_FLD_RDMA_EN, rdma_ctrl),
            REG_FLD_VAL_GET(RDMA0_CTRL_FLD_RMDA_FIFO_USED_SZ, rdma_ctrl),
            DISP_REG_GET(layer_offset + DISP_REG_OVL_RDMA0_MEM_GMC_SETTING));
        ovl_print_ovl_rdma_status(DISP_REG_GET(DISP_REG_OVL_RDMA0_DBG + rdma_offset));
    }
    ovl_printf_status(DISP_REG_GET(DISP_REG_OVL_FLOW_CTRL_DBG + offset));
}

static inline unsigned long ovl_layer_num(enum DISP_MODULE_ENUM module)
{
    switch (module) {
    case DISP_MODULE_OVL0:
    case DISP_MODULE_OVL1:
        return 4;
    case DISP_MODULE_OVL0_2L:
    case DISP_MODULE_OVL1_2L:
    case DISP_MODULE_OVL2_2L:
    case DISP_MODULE_OVL3_2L:
        return 2;
    default:
        DDPERR("invalid ovl module=%d\n", module);
        ASSERT(0);
    }
    return 0;
}

void ovl_cal_golden_setting(unsigned int *gs)
{
    /* OVL_RDMA_MEM_GMC_SETTING_1 */
    gs[GS_OVL_RDMA_ULTRA_TH] = 0x3ff;
    gs[GS_OVL_RDMA_PRE_ULTRA_TH] = 0x3ff;

    /* OVL_RDMA_FIFO_CTRL */
    gs[GS_OVL_RDMA_FIFO_THRD] = 0;
    gs[GS_OVL_RDMA_FIFO_SIZE] = 384;

    /* OVL_RDMA_MEM_GMC_SETTING_2 */
    gs[GS_OVL_RDMA_ISSUE_REQ_TH] = 255;
    gs[GS_OVL_RDMA_ISSUE_REQ_TH_URG] = 127;
    gs[GS_OVL_RDMA_REQ_TH_PRE_ULTRA] = 0;
    gs[GS_OVL_RDMA_REQ_TH_ULTRA] = 1;
    gs[GS_OVL_RDMA_FORCE_REQ_TH] = 0;

    /* OVL_RDMA_GREQ_NUM */
    gs[GS_OVL_RDMA_GREQ_NUM] = 0xF1FF7777;

    /* OVL_RDMA_GREQURG_NUM */
    gs[GS_OVL_RDMA_GREQ_URG_NUM] = 0x7777;

    /* OVL_RDMA_ULTRA_SRC */
    gs[GS_OVL_RDMA_ULTRA_SRC] = 0x8040;

    /* OVL_RDMA_BUF_LOW_TH */
    gs[GS_OVL_RDMA_ULTRA_LOW_TH] = 0;
    gs[GS_OVL_RDMA_PRE_ULTRA_LOW_TH] = 0;

    /* OVL_RDMA_BUF_HIGH_TH */
    gs[GS_OVL_RDMA_PRE_ULTRA_HIGH_TH] = 0;
    gs[GS_OVL_RDMA_PRE_ULTRA_HIGH_DIS] = 1;

    /* OVL_EN */
    gs[GS_OVL_BLOCK_EXT_ULTRA] = 0;
    gs[GS_OVL_BLOCK_EXT_PRE_ULTRA] = 0;
}

static int ovl_golden_setting(enum DISP_MODULE_ENUM module, void *cmdq)
{
    unsigned long baddr = ovl_base_addr(module);
    unsigned int regval;
    unsigned int gs[GS_OVL_FLD_NUM];
    int i, layer_num;
    unsigned long Lx_base;

    baddr -= DISPSYS_OVL0_BASE;

    layer_num = ovl_layer_num(module);

    /* calculate ovl golden setting */
    ovl_cal_golden_setting(gs);

    /* OVL_RDMA_MEM_GMC_SETTING_1 */
    regval = gs[GS_OVL_RDMA_ULTRA_TH] +
        (gs[GS_OVL_RDMA_PRE_ULTRA_TH] << 16);
    for (i = 0; i < layer_num; i++) {
        Lx_base = i * OVL_LAYER_OFFSET + baddr;

        DISP_REG_SET(cmdq, Lx_base +
                 DISP_REG_OVL_RDMA0_MEM_GMC_SETTING, regval);
    }

    /* OVL_RDMA_FIFO_CTRL */
    regval = gs[GS_OVL_RDMA_FIFO_THRD] +
        (gs[GS_OVL_RDMA_FIFO_SIZE] << 16);
    for (i = 0; i < layer_num; i++) {
        Lx_base = i * OVL_LAYER_OFFSET + baddr;

        DISP_REG_SET(cmdq, Lx_base + DISP_REG_OVL_RDMA0_FIFO_CTRL,
                 regval);
    }

    /* OVL_RDMA_MEM_GMC_SETTING_2 */
    regval = gs[GS_OVL_RDMA_ISSUE_REQ_TH] +
        (gs[GS_OVL_RDMA_ISSUE_REQ_TH_URG] << 16) +
        (gs[GS_OVL_RDMA_REQ_TH_PRE_ULTRA] << 28) +
        (gs[GS_OVL_RDMA_REQ_TH_ULTRA] << 29) +
        (gs[GS_OVL_RDMA_FORCE_REQ_TH] << 30);
    for (i = 0; i < layer_num; i++)
        DISP_REG_SET(cmdq, baddr + DISP_REG_OVL_RDMA0_MEM_GMC_S2 +
            i * 4, regval);

    /* DISP_REG_OVL_RDMA_GREQ_NUM */
    regval = gs[GS_OVL_RDMA_GREQ_NUM];
    DISP_REG_SET(cmdq, baddr + DISP_REG_OVL_RDMA_GREQ_NUM, regval);

    /* DISP_REG_OVL_RDMA_GREQ_URG_NUM */
    regval = gs[GS_OVL_RDMA_GREQ_URG_NUM];
    DISP_REG_SET(cmdq, baddr + DISP_REG_OVL_RDMA_GREQ_URG_NUM, regval);

    /* DISP_REG_OVL_RDMA_ULTRA_SRC */
    regval = gs[GS_OVL_RDMA_ULTRA_SRC];
    DISP_REG_SET(cmdq, baddr + DISP_REG_OVL_RDMA_ULTRA_SRC, regval);

    /* DISP_REG_OVL_RDMAn_BUF_LOW */
    regval = gs[GS_OVL_RDMA_ULTRA_LOW_TH] +
        (gs[GS_OVL_RDMA_PRE_ULTRA_LOW_TH] << 12);

    for (i = 0; i < layer_num; i++)
        DISP_REG_SET(cmdq, baddr + DISP_REG_OVL_RDMAn_BUF_LOW(i),
                 regval);

    /* DISP_REG_OVL_RDMAn_BUF_HIGH */
    regval = (gs[GS_OVL_RDMA_PRE_ULTRA_HIGH_TH] << 12) +
        (gs[GS_OVL_RDMA_PRE_ULTRA_HIGH_DIS] << 31);

    for (i = 0; i < layer_num; i++)
        DISP_REG_SET(cmdq, baddr + DISP_REG_OVL_RDMAn_BUF_HIGH(i),
                 regval);

    /* OVL_EN */
    regval = gs[GS_OVL_BLOCK_EXT_ULTRA] +
        (gs[GS_OVL_BLOCK_EXT_PRE_ULTRA] << 1);
    DISP_REG_SET_FIELD(cmdq, REG_FLD_MSB_LSB(19, 18), baddr +
        DISP_REG_OVL_EN, regval);

    /* Set ultra_sel of ovl0 & ovl0_2l to RDMA0 if path is DL with rsz
     * OVL0_2l -> RSZ -> OVL0 -> RDMA0 -> DSI
     */
    DISP_REG_SET_FIELD(cmdq, mmsys_ovl_ultra_offset(module),
        DISP_REG_CONFIG_MMSYS_MISC, 1);

    return 0;
}

static int OVLConfig_l(enum DISP_MODULE_ENUM module, struct disp_ddp_path_config *pConfig,
                        void *handle)
{
    int enabled_layers = 0;
    unsigned int local_layer = 0, global_layer = 0;
    int idx = ovl_index(module);

    if (pConfig->dst_dirty) {
        OVLROI(module,
               pConfig->dst_w, // width
               pConfig->dst_h, // height
               0xFF000000,    // background color
               handle);
    }

    if (!pConfig->ovl_dirty)
        return 0;

    for (global_layer = 0; global_layer < TOTAL_OVL_LAYER_NUM; global_layer++) {
        if (!(pConfig->ovl_layer_scanned & (1 << global_layer)))
            break;
    }
    if (global_layer > TOTAL_OVL_LAYER_NUM - ovl_layer_num(module)) {
        DDPERR("%s: %s scan error, layer_scanned=%u\n", __func__,
               ddp_get_module_name(module), pConfig->ovl_layer_scanned);
        ASSERT(0);
    }

    for (local_layer = 0; local_layer < ovl_layer_num(module); local_layer++, global_layer++) {
        struct OVL_CONFIG_STRUCT *ovl_cfg = &pConfig->ovl_config[global_layer];

        pConfig->ovl_layer_scanned |= (1 << global_layer);

        if (ovl_cfg->layer_en == 0)
            continue;
        if (ovl_cfg->addr == 0 || ovl_cfg->dst_w == 0 || ovl_cfg->dst_h == 0) {
            DDPERR("ovl parameter invalidate, addr=0x%x, w=%d, h=%d\n",
                   ovl_cfg->addr,
                   ovl_cfg->dst_w,
                   ovl_cfg->dst_h);
            return -1;
        }

        DDPDBG("module %d,L%d,en=%d,src=%d,fmt=%d,addr=0x%x,x=%d,y=%d,pitch=%d,dst(%d,%d,%d,%d)\n",
               module,
               local_layer,
               ovl_cfg->layer_en,
               ovl_cfg->source,
               ovl_cfg->fmt,
               ovl_cfg->addr,
               ovl_cfg->src_x,
               ovl_cfg->src_y,
               ovl_cfg->src_pitch,
               ovl_cfg->dst_x,
               ovl_cfg->dst_y,
               ovl_cfg->dst_w,
               ovl_cfg->dst_h);
        DDPDBG("keyEn=%d,key=%d,aen=%d,alpha=%d\n",
               ovl_cfg->keyEn,
               ovl_cfg->key,
               ovl_cfg->aen,
               ovl_cfg->alpha);
        OVLLayerConfig(module,
                   local_layer,
                   ovl_cfg->source,
                   ovl_cfg->fmt,
                   ovl_cfg->addr,
                   ovl_cfg->src_x,
                   ovl_cfg->src_y,
                   ovl_cfg->src_pitch,
                   ovl_cfg->dst_x,
                   ovl_cfg->dst_y,
                   ovl_cfg->dst_w,
                   ovl_cfg->dst_h,
                   ovl_cfg->keyEn,
                   ovl_cfg->key,
                   ovl_cfg->aen,
                   ovl_cfg->alpha,
                   handle);
//         print_layer_config_args(module, local_layer, ovl_cfg);
//        ovl_layer_config(module, local_layer, has_sec_layer, ovl_cfg, handle);

        enabled_layers |= 1 << local_layer;

    }

    DISP_REG_SET(handle, idx * DISP_INDEX_OFFSET + DISP_REG_OVL_SRC_CON, enabled_layers);

    ovl_golden_setting(module, handle);

    return 0;
}

struct DDP_MODULE_DRIVER ddp_driver_ovl = {
    .module          = DISP_MODULE_OVL0,
    .init            = OVLInit,
    .deinit          = OVLDeInit,
    .config          = OVLConfig_l,
    .start           = OVLStart,
    .trigger         = NULL,
    .stop            = OVLStop,
    .reset           = NULL,
    .power_on        = OVLClockOn,
    .power_off       = OVLClockOff,
    .is_idle         = NULL,
    .is_busy         = NULL,
    .dump_info       = NULL,
    .bypass          = NULL,
    .build_cmdq      = NULL,
    .set_lcm_utils   = NULL,
    .connect         = OVLConnect,
};

