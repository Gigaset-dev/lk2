/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifndef REG_BASE_C_MODULE
// ----------------- DISP_OVL0 Bit field Definitions -------------------

#define PACKING

PACKING union DISP_OVL0_REG_OVL_STA
{
    PACKING struct
    {
        unsigned int OVL_RUN                   : 1;
        unsigned int RDMA0_IDLE                : 1;
        unsigned int RDMA1_IDLE                : 1;
        unsigned int RDMA2_IDLE                : 1;
        unsigned int RDMA3_IDLE                : 1;
        unsigned int rsv_5                     : 27;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_INTEN
{
    PACKING struct
    {
        unsigned int OVL_REG_CMT_INTEN         : 1;
        unsigned int OVL_FME_CPL_INTEN         : 1;
        unsigned int OVL_FME_UND_INTEN         : 1;
        unsigned int OVL_FME_SWRST_DONE_INTEN  : 1;
        unsigned int OVL_FME_HWRST_DONE_INTEN  : 1;
        unsigned int RDMA0_EOF_ABNORMAL_INTEN  : 1;
        unsigned int RDMA1_EOF_ABNORMAL_INTEN  : 1;
        unsigned int RDMA2_EOF_ABNORMAL_INTEN  : 1;
        unsigned int RDMA3_EOF_ABNORMAL_INTEN  : 1;
        unsigned int RDMA0_SMI_UNDERFLOW_INTEN : 1;
        unsigned int RDMA1_SMI_UNDERFLOW_INTEN : 1;
        unsigned int RDMA2_SMI_UNDERFLOW_INTEN : 1;
        unsigned int RDMA3_SMI_UNDERFLOW_INTEN : 1;
        unsigned int ABNORMAL_SOF_INTEN        : 1;
        unsigned int OVL_START_INTEN           : 1;
        unsigned int rsv_15                    : 17;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_INTSTA
{
    PACKING struct
    {
        unsigned int OVL_REG_CMT_INTSTA        : 1;
        unsigned int OVL_FME_CPL_INTSTA        : 1;
        unsigned int OVL_FME_UND_INTSTA        : 1;
        unsigned int OVL_FME_SWRST_DONE_INTSTA : 1;
        unsigned int OVL_FME_HWRST_DONE_INTSTA : 1;
        unsigned int RDMA0_EOF_ABNORMAL_INTSTA : 1;
        unsigned int RDMA1_EOF_ABNORMAL_INTSTA : 1;
        unsigned int RDMA2_EOF_ABNORMAL_INTSTA : 1;
        unsigned int RDMA3_EOF_ABNORMAL_INTSTA : 1;
        unsigned int RDMA0_SMI_UNDERFLOW_INTSTA : 1;
        unsigned int RDMA1_SMI_UNDERFLOW_INTSTA : 1;
        unsigned int RDMA2_SMI_UNDERFLOW_INTSTA : 1;
        unsigned int RDMA3_SMI_UNDERFLOW_INTSTA : 1;
        unsigned int ABNORMAL_SOF_INTSTA       : 1;
        unsigned int OVL_START_INTSTA          : 1;
        unsigned int rsv_15                    : 17;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_EN
{
    PACKING struct
    {
        unsigned int OVL_EN                    : 1;
        unsigned int rsv_1                     : 7;
        unsigned int HG_FOVL_CK_ON             : 1;
        unsigned int HG_FSMI_CK_ON             : 1;
        unsigned int HF_FOVL_CK_ON             : 1;
        unsigned int rsv_11                    : 5;
        unsigned int IGNORE_ABNORMAL_SOF       : 1;
        unsigned int rsv_17                    : 1;
        unsigned int BLOCK_EXT_ULTRA           : 1;
        unsigned int BLOCK_EXT_PREULTRA        : 1;
        unsigned int rsv_20                    : 12;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_TRIG
{
    PACKING struct
    {
        unsigned int OVL_SW_TRIG               : 1;
        unsigned int rsv_1                     : 7;
        unsigned int CRC_EN                    : 1;
        unsigned int CRC_CLR                   : 1;
        unsigned int rsv_10                    : 22;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_RST
{
    PACKING struct
    {
        unsigned int OVL_RST                   : 1;
        unsigned int rsv_1                     : 27;
        unsigned int OVL_SMI_RST               : 1;
        unsigned int OVL_SMI_HARD_RST          : 1;
        unsigned int OVL_SMI_IOBUF_RST         : 1;
        unsigned int OVL_SMI_IOBUF_HARD_RST    : 1;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_ROI_SIZE
{
    PACKING struct
    {
        unsigned int ROI_W                     : 13;
        unsigned int rsv_13                    : 3;
        unsigned int ROI_H                     : 13;
        unsigned int rsv_29                    : 3;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_DATAPATH_CON
{
    PACKING struct
    {
        unsigned int LAYER_SMI_ID_EN           : 1;
        unsigned int OVL_RANDOM_BGCLR_EN       : 1;
        unsigned int BGCLR_IN_SEL              : 1;
        unsigned int OUTPUT_NO_RND             : 1;
        unsigned int rsv_4                     : 4;
        unsigned int L0_GPU_MODE               : 1;
        unsigned int L1_GPU_MODE               : 1;
        unsigned int L2_GPU_MODE               : 1;
        unsigned int L3_GPU_MODE               : 1;
        unsigned int ADOBE_MODE                : 1;
        unsigned int ADOBE_LAYER               : 2;
        unsigned int OVL_GAMMA_OUT             : 1;
        unsigned int PQ_OUT_SEL                : 2;
        unsigned int rsv_18                    : 2;
        unsigned int RDMA0_OUT_SEL             : 1;
        unsigned int RDMA1_OUT_SEL             : 1;
        unsigned int RDMA2_OUT_SEL             : 1;
        unsigned int RDMA3_OUT_SEL             : 1;
        unsigned int GCLAST_EN                 : 1;
        unsigned int rsv_25                    : 1;
        unsigned int OUTPUT_CLAMP              : 1;
        unsigned int OUTPUT_INTERLACE          : 1;
        unsigned int rsv_28                    : 3;
        unsigned int WIDE_GAMUT_EN             : 1;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_ROI_BGCLR
{
    PACKING struct
    {
        unsigned int BLUE                      : 8;
        unsigned int GREEN                     : 8;
        unsigned int RED                       : 8;
        unsigned int ALPHA                     : 8;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_SRC_CON
{
    PACKING struct
    {
        unsigned int L0_EN                     : 1;
        unsigned int L1_EN                     : 1;
        unsigned int L2_EN                     : 1;
        unsigned int L3_EN                     : 1;
        unsigned int LC_EN                     : 1;
        unsigned int rsv_5                     : 3;
        unsigned int FORCE_RELAY_MODE          : 1;
        unsigned int RELAY_MODE_EN             : 1;
        unsigned int rsv_10                    : 22;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L0_CON
{
    PACKING struct
    {
        unsigned int ALPHA                     : 8;
        unsigned int ALPHA_EN                  : 1;
        unsigned int VERTICAL_FLIP_EN          : 1;
        unsigned int HORIZONTAL_FLIP_EN        : 1;
        unsigned int EXT_MTX_EN                : 1;
        unsigned int CLRFMT                    : 4;
        unsigned int INT_MTX_SEL               : 4;
        unsigned int EN_3D                     : 1;
        unsigned int LANDSCAPE                 : 1;
        unsigned int R_FIRST                   : 1;
        unsigned int CLRFMT_MAN                : 1;
        unsigned int BYTE_SWAP                 : 1;
        unsigned int RGB_SWAP                  : 1;
        unsigned int MTX_AUTO_DIS              : 1;
        unsigned int MTX_EN                    : 1;
        unsigned int LAYER_SRC                 : 2;
        unsigned int SRCKEY_EN                 : 1;
        unsigned int DSTKEY_EN                 : 1;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L0_SRCKEY
{
    PACKING struct
    {
        unsigned int SRCKEY                    : 32;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L0_SRC_SIZE
{
    PACKING struct
    {
        unsigned int L0_SRC_W                  : 13;
        unsigned int rsv_13                    : 3;
        unsigned int L0_SRC_H                  : 13;
        unsigned int rsv_29                    : 3;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L0_OFFSET
{
    PACKING struct
    {
        unsigned int L0_XOFF                   : 13;
        unsigned int rsv_13                    : 3;
        unsigned int L0_YOFF                   : 13;
        unsigned int rsv_29                    : 3;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L0_PITCH_MSB
{
    PACKING struct
    {
        unsigned int L0_SRC_PITCH_MSB          : 4;
        unsigned int rsv_4                     : 12;
        unsigned int L0_2ND_SUBBUF             : 1;
        unsigned int rsv_17                    : 3;
        unsigned int L0_YUV_TRANS              : 1;
        unsigned int rsv_21                    : 3;
        unsigned int L0_HDR_ADR_EN             : 1;
        unsigned int rsv_25                    : 7;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L0_PITCH
{
    PACKING struct
    {
        unsigned int L0_SRC_PITCH              : 16;
        unsigned int L0_SA_SEL                 : 2;
        unsigned int L0_SRGB_SEL               : 2;
        unsigned int L0_DA_SEL                 : 2;
        unsigned int L0_DRGB_SEL               : 2;
        unsigned int L0_SA_SEL_EXT             : 1;
        unsigned int L0_SRGB_SEL_EXT           : 1;
        unsigned int L0_DA_SEL_EXT             : 1;
        unsigned int L0_DRGB_SEL_EXT           : 1;
        unsigned int L0_CONST_BLD              : 1;
        unsigned int L0_SRGB_SEL_EXT2          : 1;
        unsigned int L0_BLEND_RND_SHT          : 1;
        unsigned int SURFL_EN                  : 1;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L0_TILE
{
    PACKING struct
    {
        unsigned int TILE_HEIGHT               : 20;
        unsigned int TILE_WIDTH_SEL            : 1;
        unsigned int TILE_EN                   : 1;
        unsigned int rsv_22                    : 2;
        unsigned int TILE_HORI_BLOCK_NUM       : 8;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L0_CLIP
{
    PACKING struct
    {
        unsigned int L0_SRC_LEFT_CLIP          : 8;
        unsigned int L0_SRC_RIGHT_CLIP         : 8;
        unsigned int L0_SRC_TOP_CLIP           : 8;
        unsigned int L0_SRC_BOTTOM_CLIP        : 8;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L1_CON
{
    PACKING struct
    {
        unsigned int ALPHA                     : 8;
        unsigned int ALPHA_EN                  : 1;
        unsigned int VERTICAL_FLIP_EN          : 1;
        unsigned int HORIZONTAL_FLIP_EN        : 1;
        unsigned int EXT_MTX_EN                : 1;
        unsigned int CLRFMT                    : 4;
        unsigned int INT_MTX_SEL               : 4;
        unsigned int EN_3D                     : 1;
        unsigned int LANDSCAPE                 : 1;
        unsigned int R_FIRST                   : 1;
        unsigned int CLRFMT_MAN                : 1;
        unsigned int BYTE_SWAP                 : 1;
        unsigned int RGB_SWAP                  : 1;
        unsigned int MTX_AUTO_DIS              : 1;
        unsigned int MTX_EN                    : 1;
        unsigned int LAYER_SRC                 : 2;
        unsigned int SRCKEY_EN                 : 1;
        unsigned int DSTKEY_EN                 : 1;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L1_SRCKEY
{
    PACKING struct
    {
        unsigned int SRCKEY                    : 32;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L1_SRC_SIZE
{
    PACKING struct
    {
        unsigned int L1_SRC_W                  : 13;
        unsigned int rsv_13                    : 3;
        unsigned int L1_SRC_H                  : 13;
        unsigned int rsv_29                    : 3;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L1_OFFSET
{
    PACKING struct
    {
        unsigned int L1_XOFF                   : 13;
        unsigned int rsv_13                    : 3;
        unsigned int L1_YOFF                   : 13;
        unsigned int rsv_29                    : 3;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L1_PITCH_MSB
{
    PACKING struct
    {
        unsigned int L1_SRC_PITCH_MSB          : 4;
        unsigned int rsv_4                     : 12;
        unsigned int L1_2ND_SUBBUF             : 1;
        unsigned int rsv_17                    : 3;
        unsigned int L1_YUV_TRANS              : 1;
        unsigned int rsv_21                    : 3;
        unsigned int L1_HDR_ADR_EN             : 1;
        unsigned int rsv_25                    : 7;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L1_PITCH
{
    PACKING struct
    {
        unsigned int L1_SRC_PITCH              : 16;
        unsigned int L1_SA_SEL                 : 2;
        unsigned int L1_SRGB_SEL               : 2;
        unsigned int L1_DA_SEL                 : 2;
        unsigned int L1_DRGB_SEL               : 2;
        unsigned int L1_SA_SEL_EXT             : 1;
        unsigned int L1_SRGB_SEL_EXT           : 1;
        unsigned int L1_DA_SEL_EXT             : 1;
        unsigned int L1_DRGB_SEL_EXT           : 1;
        unsigned int L1_CONST_BLD              : 1;
        unsigned int L1_SRGB_SEL_EXT2          : 1;
        unsigned int L1_BLEND_RND_SHT          : 1;
        unsigned int SURFL_EN                  : 1;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L1_TILE
{
    PACKING struct
    {
        unsigned int TILE_HEIGHT               : 20;
        unsigned int TILE_WIDTH_SEL            : 1;
        unsigned int TILE_EN                   : 1;
        unsigned int rsv_22                    : 2;
        unsigned int TILE_HORI_BLOCK_NUM       : 8;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L1_CLIP
{
    PACKING struct
    {
        unsigned int L1_SRC_LEFT_CLIP          : 8;
        unsigned int L1_SRC_RIGHT_CLIP         : 8;
        unsigned int L1_SRC_TOP_CLIP           : 8;
        unsigned int L1_SRC_BOTTOM_CLIP        : 8;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L2_CON
{
    PACKING struct
    {
        unsigned int ALPHA                     : 8;
        unsigned int ALPHA_EN                  : 1;
        unsigned int VERTICAL_FLIP_EN          : 1;
        unsigned int HORIZONTAL_FLIP_EN        : 1;
        unsigned int EXT_MTX_EN                : 1;
        unsigned int CLRFMT                    : 4;
        unsigned int INT_MTX_SEL               : 4;
        unsigned int EN_3D                     : 1;
        unsigned int LANDSCAPE                 : 1;
        unsigned int R_FIRST                   : 1;
        unsigned int CLRFMT_MAN                : 1;
        unsigned int BYTE_SWAP                 : 1;
        unsigned int RGB_SWAP                  : 1;
        unsigned int MTX_AUTO_DIS              : 1;
        unsigned int MTX_EN                    : 1;
        unsigned int LAYER_SRC                 : 2;
        unsigned int SRCKEY_EN                 : 1;
        unsigned int DSTKEY_EN                 : 1;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L2_SRCKEY
{
    PACKING struct
    {
        unsigned int SRCKEY                    : 32;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L2_SRC_SIZE
{
    PACKING struct
    {
        unsigned int L2_SRC_W                  : 13;
        unsigned int rsv_13                    : 3;
        unsigned int L2_SRC_H                  : 13;
        unsigned int rsv_29                    : 3;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L2_OFFSET
{
    PACKING struct
    {
        unsigned int L2_XOFF                   : 13;
        unsigned int rsv_13                    : 3;
        unsigned int L2_YOFF                   : 13;
        unsigned int rsv_29                    : 3;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L2_PITCH_MSB
{
    PACKING struct
    {
        unsigned int L2_SRC_PITCH_MSB          : 4;
        unsigned int rsv_4                     : 12;
        unsigned int L2_2ND_SUBBUF             : 1;
        unsigned int rsv_17                    : 3;
        unsigned int L2_YUV_TRANS              : 1;
        unsigned int rsv_21                    : 3;
        unsigned int L2_HDR_ADR_EN             : 1;
        unsigned int rsv_25                    : 7;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L2_PITCH
{
    PACKING struct
    {
        unsigned int L2_SRC_PITCH              : 16;
        unsigned int L2_SA_SEL                 : 2;
        unsigned int L2_SRGB_SEL               : 2;
        unsigned int L2_DA_SEL                 : 2;
        unsigned int L2_DRGB_SEL               : 2;
        unsigned int L2_SA_SEL_EXT             : 1;
        unsigned int L2_SRGB_SEL_EXT           : 1;
        unsigned int L2_DA_SEL_EXT             : 1;
        unsigned int L2_DRGB_SEL_EXT           : 1;
        unsigned int L2_CONST_BLD              : 1;
        unsigned int L2_SRGB_SEL_EXT2          : 1;
        unsigned int L2_BLEND_RND_SHT          : 1;
        unsigned int SURFL_EN                  : 1;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L2_TILE
{
    PACKING struct
    {
        unsigned int TILE_HEIGHT               : 20;
        unsigned int TILE_WIDTH_SEL            : 1;
        unsigned int TILE_EN                   : 1;
        unsigned int rsv_22                    : 2;
        unsigned int TILE_HORI_BLOCK_NUM       : 8;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L2_CLIP
{
    PACKING struct
    {
        unsigned int L2_SRC_LEFT_CLIP          : 8;
        unsigned int L2_SRC_RIGHT_CLIP         : 8;
        unsigned int L2_SRC_TOP_CLIP           : 8;
        unsigned int L2_SRC_BOTTOM_CLIP        : 8;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L3_CON
{
    PACKING struct
    {
        unsigned int ALPHA                     : 8;
        unsigned int ALPHA_EN                  : 1;
        unsigned int VERTICAL_FLIP_EN          : 1;
        unsigned int HORIZONTAL_FLIP_EN        : 1;
        unsigned int EXT_MTX_EN                : 1;
        unsigned int CLRFMT                    : 4;
        unsigned int INT_MTX_SEL               : 4;
        unsigned int EN_3D                     : 1;
        unsigned int LANDSCAPE                 : 1;
        unsigned int R_FIRST                   : 1;
        unsigned int CLRFMT_MAN                : 1;
        unsigned int BYTE_SWAP                 : 1;
        unsigned int RGB_SWAP                  : 1;
        unsigned int MTX_AUTO_DIS              : 1;
        unsigned int MTX_EN                    : 1;
        unsigned int LAYER_SRC                 : 2;
        unsigned int SRCKEY_EN                 : 1;
        unsigned int DSTKEY_EN                 : 1;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L3_SRCKEY
{
    PACKING struct
    {
        unsigned int SRCKEY                    : 32;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L3_SRC_SIZE
{
    PACKING struct
    {
        unsigned int L3_SRC_W                  : 13;
        unsigned int rsv_13                    : 3;
        unsigned int L3_SRC_H                  : 13;
        unsigned int rsv_29                    : 3;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L3_OFFSET
{
    PACKING struct
    {
        unsigned int L3_XOFF                   : 13;
        unsigned int rsv_13                    : 3;
        unsigned int L3_YOFF                   : 13;
        unsigned int rsv_29                    : 3;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L3_PITCH_MSB
{
    PACKING struct
    {
        unsigned int L3_SRC_PITCH_MSB          : 4;
        unsigned int rsv_4                     : 12;
        unsigned int L3_2ND_SUBBUF             : 1;
        unsigned int rsv_17                    : 3;
        unsigned int L3_YUV_TRANS              : 1;
        unsigned int rsv_21                    : 3;
        unsigned int L3_HDR_ADR_EN             : 1;
        unsigned int rsv_25                    : 7;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L3_PITCH
{
    PACKING struct
    {
        unsigned int L3_SRC_PITCH              : 16;
        unsigned int L3_SA_SEL                 : 2;
        unsigned int L3_SRGB_SEL               : 2;
        unsigned int L3_DA_SEL                 : 2;
        unsigned int L3_DRGB_SEL               : 2;
        unsigned int L3_SA_SEL_EXT             : 1;
        unsigned int L3_SRGB_SEL_EXT           : 1;
        unsigned int L3_DA_SEL_EXT             : 1;
        unsigned int L3_DRGB_SEL_EXT           : 1;
        unsigned int L3_CONST_BLD              : 1;
        unsigned int L3_SRGB_SEL_EXT2          : 1;
        unsigned int L3_BLEND_RND_SHT          : 1;
        unsigned int SURFL_EN                  : 1;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L3_TILE
{
    PACKING struct
    {
        unsigned int TILE_HEIGHT               : 20;
        unsigned int TILE_WIDTH_SEL            : 1;
        unsigned int TILE_EN                   : 1;
        unsigned int rsv_22                    : 2;
        unsigned int TILE_HORI_BLOCK_NUM       : 8;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L3_CLIP
{
    PACKING struct
    {
        unsigned int L3_SRC_LEFT_CLIP          : 8;
        unsigned int L3_SRC_RIGHT_CLIP         : 8;
        unsigned int L3_SRC_TOP_CLIP           : 8;
        unsigned int L3_SRC_BOTTOM_CLIP        : 8;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_RDMA0_CTRL
{
    PACKING struct
    {
        unsigned int RDMA0_EN                  : 1;
        unsigned int rsv_1                     : 3;
        unsigned int RDMA0_INTERLACE           : 1;
        unsigned int rsv_5                     : 11;
        unsigned int RDMA0_FIFO_USED_SIZE      : 12;
        unsigned int rsv_28                    : 4;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_RDMA0_MEM_GMC_SETTING1
{
    PACKING struct
    {
        unsigned int RDMA0_OSTD_ULTRA_TH       : 10;
        unsigned int rsv_10                    : 6;
        unsigned int RDMA0_OSTD_PREULTRA_TH    : 10;
        unsigned int rsv_26                    : 2;
        unsigned int RDMA0_ULTRA_THRESHOLD_HIGH_OFS : 1;
        unsigned int rsv_29                    : 2;
        unsigned int RDMA0_PRE_ULTRA_THRESHOLD_HIGH_OFS : 1;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_RDMA0_MEM_SLOW_CON
{
    PACKING struct
    {
        unsigned int RDMA0_SLOW_EN             : 1;
        unsigned int rsv_1                     : 15;
        unsigned int RDMA0_SLOW_CNT            : 16;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_RDMA0_FIFO_CTRL
{
    PACKING struct
    {
        unsigned int RDMA0_FIFO_THRD           : 10;
        unsigned int rsv_10                    : 6;
        unsigned int RDMA0_FIFO_SIZE           : 12;
        unsigned int rsv_28                    : 3;
        unsigned int RDMA0_FIFO_UND_EN         : 1;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_RDMA1_CTRL
{
    PACKING struct
    {
        unsigned int RDMA1_EN                  : 1;
        unsigned int rsv_1                     : 3;
        unsigned int RDMA1_INTERLACE           : 1;
        unsigned int rsv_5                     : 11;
        unsigned int RDMA1_FIFO_USED_SIZE      : 12;
        unsigned int rsv_28                    : 4;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_RDMA1_MEM_GMC_SETTING1
{
    PACKING struct
    {
        unsigned int RDMA1_OSTD_ULTRA_TH       : 10;
        unsigned int rsv_10                    : 6;
        unsigned int RDMA1_OSTD_PREULTRA_TH    : 10;
        unsigned int rsv_26                    : 2;
        unsigned int RDMA1_ULTRA_THRESHOLD_HIGH_OFS : 1;
        unsigned int rsv_29                    : 2;
        unsigned int RDMA1_PRE_ULTRA_THRESHOLD_HIGH_OFS : 1;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_RDMA1_MEM_SLOW_CON
{
    PACKING struct
    {
        unsigned int RDMA1_SLOW_EN             : 1;
        unsigned int rsv_1                     : 15;
        unsigned int RDMA1_SLOW_CNT            : 16;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_RDMA1_FIFO_CTRL
{
    PACKING struct
    {
        unsigned int RDMA1_FIFO_THRD           : 10;
        unsigned int rsv_10                    : 6;
        unsigned int RDMA1_FIFO_SIZE           : 12;
        unsigned int rsv_28                    : 3;
        unsigned int RDMA1_FIFO_UND_EN         : 1;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_RDMA2_CTRL
{
    PACKING struct
    {
        unsigned int RDMA2_EN                  : 1;
        unsigned int rsv_1                     : 3;
        unsigned int RDMA2_INTERLACE           : 1;
        unsigned int rsv_5                     : 11;
        unsigned int RDMA2_FIFO_USED_SIZE      : 12;
        unsigned int rsv_28                    : 4;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_RDMA2_MEM_GMC_SETTING1
{
    PACKING struct
    {
        unsigned int RDMA2_OSTD_ULTRA_TH       : 10;
        unsigned int rsv_10                    : 6;
        unsigned int RDMA2_OSTD_PREULTRA_TH    : 10;
        unsigned int rsv_26                    : 2;
        unsigned int RDMA2_ULTRA_THRESHOLD_HIGH_OFS : 1;
        unsigned int rsv_29                    : 2;
        unsigned int RDMA2_PRE_ULTRA_THRESHOLD_HIGH_OFS : 1;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_RDMA2_MEM_SLOW_CON
{
    PACKING struct
    {
        unsigned int RDMA2_SLOW_EN             : 1;
        unsigned int rsv_1                     : 15;
        unsigned int RDMA2_SLOW_CNT            : 16;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_RDMA2_FIFO_CTRL
{
    PACKING struct
    {
        unsigned int RDMA2_FIFO_THRD           : 10;
        unsigned int rsv_10                    : 6;
        unsigned int RDMA2_FIFO_SIZE           : 12;
        unsigned int rsv_28                    : 3;
        unsigned int RDMA2_FIFO_UND_EN         : 1;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_RDMA3_CTRL
{
    PACKING struct
    {
        unsigned int RDMA3_EN                  : 1;
        unsigned int rsv_1                     : 3;
        unsigned int RDMA3_INTERLACE           : 1;
        unsigned int rsv_5                     : 11;
        unsigned int RDMA3_FIFO_USED_SIZE      : 12;
        unsigned int rsv_28                    : 4;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_RDMA3_MEM_GMC_SETTING1
{
    PACKING struct
    {
        unsigned int RDMA3_OSTD_ULTRA_TH       : 10;
        unsigned int rsv_10                    : 6;
        unsigned int RDMA3_OSTD_PREULTRA_TH    : 10;
        unsigned int rsv_26                    : 2;
        unsigned int RDMA3_ULTRA_THRESHOLD_HIGH_OFS : 1;
        unsigned int rsv_29                    : 2;
        unsigned int RDMA3_PRE_ULTRA_THRESHOLD_HIGH_OFS : 1;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_RDMA3_MEM_SLOW_CON
{
    PACKING struct
    {
        unsigned int RDMA3_SLOW_EN             : 1;
        unsigned int rsv_1                     : 15;
        unsigned int RDMA3_SLOW_CNT            : 16;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_RDMA3_FIFO_CTRL
{
    PACKING struct
    {
        unsigned int RDMA3_FIFO_THRD           : 10;
        unsigned int rsv_10                    : 6;
        unsigned int RDMA3_FIFO_SIZE           : 12;
        unsigned int rsv_28                    : 3;
        unsigned int RDMA3_FIFO_UND_EN         : 1;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L0_Y2R_PARA_R0
{
    PACKING struct
    {
        unsigned int C_CF_RMY                  : 13;
        unsigned int rsv_13                    : 3;
        unsigned int C_CF_RMU                  : 13;
        unsigned int rsv_29                    : 3;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L0_Y2R_PARA_R1
{
    PACKING struct
    {
        unsigned int C_CF_RMV                  : 13;
        unsigned int rsv_13                    : 19;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L0_Y2R_PARA_G0
{
    PACKING struct
    {
        unsigned int C_CF_GMY                  : 13;
        unsigned int rsv_13                    : 3;
        unsigned int C_CF_GMU                  : 13;
        unsigned int rsv_29                    : 3;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L0_Y2R_PARA_G1
{
    PACKING struct
    {
        unsigned int C_CF_GMV                  : 13;
        unsigned int rsv_13                    : 19;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L0_Y2R_PARA_B0
{
    PACKING struct
    {
        unsigned int C_CF_BMY                  : 13;
        unsigned int rsv_13                    : 3;
        unsigned int C_CF_BMU                  : 13;
        unsigned int rsv_29                    : 3;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L0_Y2R_PARA_B1
{
    PACKING struct
    {
        unsigned int C_CF_BMV                  : 13;
        unsigned int rsv_13                    : 19;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L0_Y2R_PARA_YUV_A_0
{
    PACKING struct
    {
        unsigned int C_CF_YA                   : 9;
        unsigned int rsv_9                     : 7;
        unsigned int C_CF_UA                   : 9;
        unsigned int rsv_25                    : 7;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L0_Y2R_PARA_YUV_A_1
{
    PACKING struct
    {
        unsigned int C_CF_VA                   : 9;
        unsigned int rsv_9                     : 23;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L0_Y2R_PARA_RGB_A_0
{
    PACKING struct
    {
        unsigned int C_CF_RA                   : 9;
        unsigned int rsv_9                     : 7;
        unsigned int C_CF_GA                   : 9;
        unsigned int rsv_25                    : 7;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L0_Y2R_PARA_RGB_A_1
{
    PACKING struct
    {
        unsigned int C_CF_BA                   : 9;
        unsigned int rsv_9                     : 23;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L1_Y2R_PARA_R0
{
    PACKING struct
    {
        unsigned int C_CF_RMY                  : 13;
        unsigned int rsv_13                    : 3;
        unsigned int C_CF_RMU                  : 13;
        unsigned int rsv_29                    : 3;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L1_Y2R_PARA_R1
{
    PACKING struct
    {
        unsigned int C_CF_RMV                  : 13;
        unsigned int rsv_13                    : 19;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L1_Y2R_PARA_G0
{
    PACKING struct
    {
        unsigned int C_CF_GMY                  : 13;
        unsigned int rsv_13                    : 3;
        unsigned int C_CF_GMU                  : 13;
        unsigned int rsv_29                    : 3;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L1_Y2R_PARA_G1
{
    PACKING struct
    {
        unsigned int C_CF_GMV                  : 13;
        unsigned int rsv_13                    : 19;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L1_Y2R_PARA_B0
{
    PACKING struct
    {
        unsigned int C_CF_BMY                  : 13;
        unsigned int rsv_13                    : 3;
        unsigned int C_CF_BMU                  : 13;
        unsigned int rsv_29                    : 3;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L1_Y2R_PARA_B1
{
    PACKING struct
    {
        unsigned int C_CF_BMV                  : 13;
        unsigned int rsv_13                    : 19;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L1_Y2R_PARA_YUV_A_0
{
    PACKING struct
    {
        unsigned int C_CF_YA                   : 9;
        unsigned int rsv_9                     : 7;
        unsigned int C_CF_UA                   : 9;
        unsigned int rsv_25                    : 7;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L1_Y2R_PARA_YUV_A_1
{
    PACKING struct
    {
        unsigned int C_CF_VA                   : 9;
        unsigned int rsv_9                     : 23;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L1_Y2R_PARA_RGB_A_0
{
    PACKING struct
    {
        unsigned int C_CF_RA                   : 9;
        unsigned int rsv_9                     : 7;
        unsigned int C_CF_GA                   : 9;
        unsigned int rsv_25                    : 7;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L1_Y2R_PARA_RGB_A_1
{
    PACKING struct
    {
        unsigned int C_CF_BA                   : 9;
        unsigned int rsv_9                     : 23;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L2_Y2R_PARA_R0
{
    PACKING struct
    {
        unsigned int C_CF_RMY                  : 13;
        unsigned int rsv_13                    : 3;
        unsigned int C_CF_RMU                  : 13;
        unsigned int rsv_29                    : 3;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L2_Y2R_PARA_R1
{
    PACKING struct
    {
        unsigned int C_CF_RMV                  : 13;
        unsigned int rsv_13                    : 19;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L2_Y2R_PARA_G0
{
    PACKING struct
    {
        unsigned int C_CF_GMY                  : 13;
        unsigned int rsv_13                    : 3;
        unsigned int C_CF_GMU                  : 13;
        unsigned int rsv_29                    : 3;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L2_Y2R_PARA_G1
{
    PACKING struct
    {
        unsigned int C_CF_GMV                  : 13;
        unsigned int rsv_13                    : 19;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L2_Y2R_PARA_B0
{
    PACKING struct
    {
        unsigned int C_CF_BMY                  : 13;
        unsigned int rsv_13                    : 3;
        unsigned int C_CF_BMU                  : 13;
        unsigned int rsv_29                    : 3;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L2_Y2R_PARA_B1
{
    PACKING struct
    {
        unsigned int C_CF_BMV                  : 13;
        unsigned int rsv_13                    : 19;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L2_Y2R_PARA_YUV_A_0
{
    PACKING struct
    {
        unsigned int C_CF_YA                   : 9;
        unsigned int rsv_9                     : 7;
        unsigned int C_CF_UA                   : 9;
        unsigned int rsv_25                    : 7;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L2_Y2R_PARA_YUV_A_1
{
    PACKING struct
    {
        unsigned int C_CF_VA                   : 9;
        unsigned int rsv_9                     : 23;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L2_Y2R_PARA_RGB_A_0
{
    PACKING struct
    {
        unsigned int C_CF_RA                   : 9;
        unsigned int rsv_9                     : 7;
        unsigned int C_CF_GA                   : 9;
        unsigned int rsv_25                    : 7;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L2_Y2R_PARA_RGB_A_1
{
    PACKING struct
    {
        unsigned int C_CF_BA                   : 9;
        unsigned int rsv_9                     : 23;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L3_Y2R_PARA_R0
{
    PACKING struct
    {
        unsigned int C_CF_RMY                  : 13;
        unsigned int rsv_13                    : 3;
        unsigned int C_CF_RMU                  : 13;
        unsigned int rsv_29                    : 3;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L3_Y2R_PARA_R1
{
    PACKING struct
    {
        unsigned int C_CF_RMV                  : 13;
        unsigned int rsv_13                    : 19;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L3_Y2R_PARA_G0
{
    PACKING struct
    {
        unsigned int C_CF_GMY                  : 13;
        unsigned int rsv_13                    : 3;
        unsigned int C_CF_GMU                  : 13;
        unsigned int rsv_29                    : 3;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L3_Y2R_PARA_G1
{
    PACKING struct
    {
        unsigned int C_CF_GMV                  : 13;
        unsigned int rsv_13                    : 19;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L3_Y2R_PARA_B0
{
    PACKING struct
    {
        unsigned int C_CF_BMY                  : 13;
        unsigned int rsv_13                    : 3;
        unsigned int C_CF_BMU                  : 13;
        unsigned int rsv_29                    : 3;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L3_Y2R_PARA_B1
{
    PACKING struct
    {
        unsigned int C_CF_BMV                  : 13;
        unsigned int rsv_13                    : 19;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L3_Y2R_PARA_YUV_A_0
{
    PACKING struct
    {
        unsigned int C_CF_YA                   : 9;
        unsigned int rsv_9                     : 7;
        unsigned int C_CF_UA                   : 9;
        unsigned int rsv_25                    : 7;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L3_Y2R_PARA_YUV_A_1
{
    PACKING struct
    {
        unsigned int C_CF_VA                   : 9;
        unsigned int rsv_9                     : 23;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L3_Y2R_PARA_RGB_A_0
{
    PACKING struct
    {
        unsigned int C_CF_RA                   : 9;
        unsigned int rsv_9                     : 7;
        unsigned int C_CF_GA                   : 9;
        unsigned int rsv_25                    : 7;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L3_Y2R_PARA_RGB_A_1
{
    PACKING struct
    {
        unsigned int C_CF_BA                   : 9;
        unsigned int rsv_9                     : 23;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_DEBUG_MON_SEL
{
    PACKING struct
    {
        unsigned int DBG_MON_SEL               : 4;
        unsigned int rsv_4                     : 28;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_BLD_EXT
{
    PACKING struct
    {
        unsigned int L0_MINUS_BLD              : 1;
        unsigned int L1_MINUS_BLD              : 1;
        unsigned int L2_MINUS_BLD              : 1;
        unsigned int L3_MINUS_BLD              : 1;
        unsigned int LC_MINUS_BLD              : 1;
        unsigned int EL0_MINUS_BLD             : 1;
        unsigned int EL1_MINUS_BLD             : 1;
        unsigned int EL2_MINUS_BLD             : 1;
        unsigned int rsv_8                     : 24;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_RDMA0_MEM_GMC_SETTING2
{
    PACKING struct
    {
        unsigned int RDMA0_ISSUE_REQ_THRESHOLD : 12;
        unsigned int rsv_12                    : 4;
        unsigned int RDMA0_ISSUE_REQ_THRESHOLD_URG : 12;
        unsigned int RDMA0_REQ_THRESHOLD_PREULTRA : 1;
        unsigned int RDMA0_REQ_THRESHOLD_ULTRA : 1;
        unsigned int RDMA0_FORCE_REQ_THRESHOLD : 1;
        unsigned int rsv_31                    : 1;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_RDMA1_MEM_GMC_SETTING2
{
    PACKING struct
    {
        unsigned int RDMA1_ISSUE_REQ_THRESHOLD : 12;
        unsigned int rsv_12                    : 4;
        unsigned int RDMA1_ISSUE_REQ_THRESHOLD_URG : 12;
        unsigned int RDMA1_REQ_THRESHOLD_PREULTRA : 1;
        unsigned int RDMA1_REQ_THRESHOLD_ULTRA : 1;
        unsigned int RDMA1_FORCE_REQ_THRESHOLD : 1;
        unsigned int rsv_31                    : 1;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_RDMA2_MEM_GMC_SETTING2
{
    PACKING struct
    {
        unsigned int RDMA2_ISSUE_REQ_THRESHOLD : 12;
        unsigned int rsv_12                    : 4;
        unsigned int RDMA2_ISSUE_REQ_THRESHOLD_URG : 12;
        unsigned int RDMA2_REQ_THRESHOLD_PREULTRA : 1;
        unsigned int RDMA2_REQ_THRESHOLD_ULTRA : 1;
        unsigned int RDMA2_FORCE_REQ_THRESHOLD : 1;
        unsigned int rsv_31                    : 1;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_RDMA3_MEM_GMC_SETTING2
{
    PACKING struct
    {
        unsigned int RDMA3_ISSUE_REQ_THRESHOLD : 12;
        unsigned int rsv_12                    : 4;
        unsigned int RDMA3_ISSUE_REQ_THRESHOLD_URG : 12;
        unsigned int RDMA3_REQ_THRESHOLD_PREULTRA : 1;
        unsigned int RDMA3_REQ_THRESHOLD_ULTRA : 1;
        unsigned int RDMA3_FORCE_REQ_THRESHOLD : 1;
        unsigned int rsv_31                    : 1;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_RDMA_BURST_CON0
{
    PACKING struct
    {
        unsigned int BURST9A_32B               : 3;
        unsigned int rsv_3                     : 1;
        unsigned int BURST10A_32B              : 3;
        unsigned int rsv_7                     : 1;
        unsigned int BURST11A_32B              : 3;
        unsigned int rsv_11                    : 1;
        unsigned int BURST12A_32B              : 3;
        unsigned int rsv_15                    : 1;
        unsigned int BURST13A_32B              : 3;
        unsigned int rsv_19                    : 1;
        unsigned int BURST14A_32B              : 3;
        unsigned int rsv_23                    : 1;
        unsigned int BURST15A_32B              : 3;
        unsigned int rsv_27                    : 1;
        unsigned int BURST_128B_BOUND          : 1;
        unsigned int rsv_29                    : 3;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_RDMA_BURST_CON1
{
    PACKING struct
    {
        unsigned int BURST9A_N32B              : 3;
        unsigned int rsv_3                     : 1;
        unsigned int BURST10A_N32B             : 3;
        unsigned int rsv_7                     : 1;
        unsigned int BURST11A_N32B             : 3;
        unsigned int rsv_11                    : 1;
        unsigned int BURST12A_N32B             : 3;
        unsigned int rsv_15                    : 1;
        unsigned int BURST13A_N32B             : 3;
        unsigned int rsv_19                    : 1;
        unsigned int BURST14A_N32B             : 3;
        unsigned int rsv_23                    : 1;
        unsigned int BURST15A_N32B             : 3;
        unsigned int rsv_27                    : 5;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_RDMA_GREQ_NUM
{
    PACKING struct
    {
        unsigned int LAYER0_GREQ_NUM           : 4;
        unsigned int LAYER1_GREQ_NUM           : 4;
        unsigned int LAYER2_GREQ_NUM           : 4;
        unsigned int LAYER3_GREQ_NUM           : 4;
        unsigned int OSTD_GREQ_NUM             : 8;
        unsigned int GREQ_DIS_CNT              : 3;
        unsigned int GREQ_STOP_EN              : 1;
        unsigned int GRP_END_STOP              : 1;
        unsigned int GRP_BRK_STOP              : 1;
        unsigned int IOBUF_FLUSH_PREULTRA      : 1;
        unsigned int IOBUF_FLUSH_ULTRA         : 1;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_RDMA_GREQ_URG_NUM
{
    PACKING struct
    {
        unsigned int LAYER0_GREQ_URG_NUM       : 4;
        unsigned int LAYER1_GREQ_URG_NUM       : 4;
        unsigned int LAYER2_GREQ_URG_NUM       : 4;
        unsigned int LAYER3_GREQ_URG_NUM       : 4;
        unsigned int ARG_GREQ_URG_TH           : 10;
        unsigned int rsv_26                    : 2;
        unsigned int ARG_URG_BIAS              : 1;
        unsigned int GREQ_NUM_SHT_VAL          : 1;
        unsigned int GREQ_NUM_SHT              : 2;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_DUMMY_REG
{
    PACKING struct
    {
        unsigned int OVERLAY_DUMMY             : 32;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_GDRDY_PRD
{
    PACKING struct
    {
        unsigned int GDRDY_PRD                 : 24;
        unsigned int rsv_24                    : 8;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_RDMA_ULTRA_SRC
{
    PACKING struct
    {
        unsigned int PREULTRA_BUF_SRC          : 2;
        unsigned int PREULTRA_SMI_SRC          : 2;
        unsigned int PREULTRA_ROI_END_SRC      : 2;
        unsigned int PREULTRA_RDMA_SRC         : 2;
        unsigned int ULTRA_BUF_SRC             : 2;
        unsigned int ULTRA_SMI_SRC             : 2;
        unsigned int ULTRA_ROI_END_SRC         : 2;
        unsigned int ULTRA_RDMA_SRC            : 2;
        unsigned int rsv_16                    : 16;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_RDMA0_BUF_LOW_TH
{
    PACKING struct
    {
        unsigned int RDMA0_ULTRA_LOW_TH        : 12;
        unsigned int RDMA0_PREULTRA_LOW_TH     : 12;
        unsigned int rsv_24                    : 8;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_RDMA1_BUF_LOW_TH
{
    PACKING struct
    {
        unsigned int RDMA1_ULTRA_LOW_TH        : 12;
        unsigned int RDMA1_PREULTRA_LOW_TH     : 12;
        unsigned int rsv_24                    : 8;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_RDMA2_BUF_LOW_TH
{
    PACKING struct
    {
        unsigned int RDMA2_ULTRA_LOW_TH        : 12;
        unsigned int RDMA2_PREULTRA_LOW_TH     : 12;
        unsigned int rsv_24                    : 8;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_RDMA3_BUF_LOW_TH
{
    PACKING struct
    {
        unsigned int RDMA3_ULTRA_LOW_TH        : 12;
        unsigned int RDMA3_PREULTRA_LOW_TH     : 12;
        unsigned int rsv_24                    : 8;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_RDMA0_BUF_HIGH_TH
{
    PACKING struct
    {
        unsigned int rsv_0                     : 12;
        unsigned int RDMA0_PREULTRA_HIGH_TH    : 12;
        unsigned int rsv_24                    : 7;
        unsigned int RDMA0_PREULTRA_HIGH_DIS   : 1;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_RDMA1_BUF_HIGH_TH
{
    PACKING struct
    {
        unsigned int rsv_0                     : 12;
        unsigned int RDMA1_PREULTRA_HIGH_TH    : 12;
        unsigned int rsv_24                    : 7;
        unsigned int RDMA1_PREULTRA_HIGH_DIS   : 1;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_RDMA2_BUF_HIGH_TH
{
    PACKING struct
    {
        unsigned int rsv_0                     : 12;
        unsigned int RDMA2_PREULTRA_HIGH_TH    : 12;
        unsigned int rsv_24                    : 7;
        unsigned int RDMA2_PREULTRA_HIGH_DIS   : 1;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_RDMA3_BUF_HIGH_TH
{
    PACKING struct
    {
        unsigned int rsv_0                     : 12;
        unsigned int RDMA3_PREULTRA_HIGH_TH    : 12;
        unsigned int rsv_24                    : 7;
        unsigned int RDMA3_PREULTRA_HIGH_DIS   : 1;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_SMI_DBG
{
    PACKING struct
    {
        unsigned int SMI_FSM                   : 10;
        unsigned int rsv_10                    : 22;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_GREQ_LAYER_CNT
{
    PACKING struct
    {
        unsigned int LAYER0_GREQ_CNT           : 6;
        unsigned int rsv_6                     : 2;
        unsigned int LAYER1_GREQ_CNT           : 6;
        unsigned int rsv_14                    : 2;
        unsigned int LAYER2_GREQ_CNT           : 6;
        unsigned int rsv_22                    : 2;
        unsigned int LAYER3_GREQ_CNT           : 6;
        unsigned int rsv_30                    : 2;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_GDRDY_PRD_NUM
{
    PACKING struct
    {
        unsigned int GDRDY_PRD_NUM             : 24;
        unsigned int rsv_24                    : 8;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_DEBUG_FLAG
{
    PACKING struct
    {
        unsigned int rsv_0                     : 24;
        unsigned int FBDC0_SIZE_NG             : 1;
        unsigned int FBDC1_SIZE_NG             : 1;
        unsigned int FBDC2_SIZE_NG             : 1;
        unsigned int FBDC3_SIZE_NG             : 1;
        unsigned int rsv_28                    : 3;
        unsigned int DEBUG_FLAG_CLR            : 1;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_FLOW_CTRL_DBG
{
    PACKING struct
    {
        unsigned int FSM_STATE                 : 10;
        unsigned int ADDCON_IDLE               : 1;
        unsigned int BLEND_IDLE                : 1;
        unsigned int OVL_OUT_VALID             : 1;
        unsigned int OVL_OUT_READY             : 1;
        unsigned int rsv_14                    : 1;
        unsigned int OUT_IDLE                  : 1;
        unsigned int RDMA3_IDLE                : 1;
        unsigned int RDMA2_IDLE                : 1;
        unsigned int RDMA1_IDLE                : 1;
        unsigned int RDMA0_IDLE                : 1;
        unsigned int RST                       : 1;
        unsigned int TRIG                      : 1;
        unsigned int rsv_22                    : 1;
        unsigned int FRAME_HWRST_DONE          : 1;
        unsigned int FRAME_SWRST_DONE          : 1;
        unsigned int FRAME_UNDERRUN            : 1;
        unsigned int FRAME_DONE                : 1;
        unsigned int OVL_RUNNING               : 1;
        unsigned int OVL_START                 : 1;
        unsigned int OVL_CLR                   : 1;
        unsigned int REG_UPDATE                : 1;
        unsigned int OVL_UPD_REG               : 1;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_ADDCON_DBG
{
    PACKING struct
    {
        unsigned int ROI_X                     : 13;
        unsigned int rsv_13                    : 1;
        unsigned int L0_WIN_HIT                : 1;
        unsigned int L1_WIN_HIT                : 1;
        unsigned int ROI_Y                     : 13;
        unsigned int rsv_29                    : 1;
        unsigned int L2_WIN_HIT                : 1;
        unsigned int L3_WIN_HIT                : 1;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_RDMA0_DBG
{
    PACKING struct
    {
        unsigned int RDMA0_WRAM_RST_CS         : 3;
        unsigned int RDMA0_LAYER_GREQ          : 1;
        unsigned int RDMA0_OUT_DATA            : 24;
        unsigned int RDMA0_OUT_READY           : 1;
        unsigned int RDMA0_OUT_VALID           : 1;
        unsigned int RDMA0_SMI_BUSY            : 1;
        unsigned int SMI_GREQ                  : 1;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_RDMA1_DBG
{
    PACKING struct
    {
        unsigned int RDMA1_WRAM_RST_CS         : 3;
        unsigned int RDMA1_LAYER_GREQ          : 1;
        unsigned int RDMA1_OUT_DATA            : 24;
        unsigned int RDMA1_OUT_READY           : 1;
        unsigned int RDMA1_OUT_VALID           : 1;
        unsigned int RDMA1_SMI_BUSY            : 1;
        unsigned int SMI_GREQ                  : 1;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_RDMA2_DBG
{
    PACKING struct
    {
        unsigned int RDMA2_WRAM_RST_CS         : 3;
        unsigned int RDMA2_LAYER_GREQ          : 1;
        unsigned int RDMA2_OUT_DATA            : 24;
        unsigned int RDMA2_OUT_READY           : 1;
        unsigned int RDMA2_OUT_VALID           : 1;
        unsigned int RDMA2_SMI_BUSY            : 1;
        unsigned int SMI_GREQ                  : 1;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_RDMA3_DBG
{
    PACKING struct
    {
        unsigned int RDMA3_WRAM_RST_CS         : 3;
        unsigned int RDMA3_LAYER_GREQ          : 1;
        unsigned int RDMA3_OUT_DATA            : 24;
        unsigned int RDMA3_OUT_READY           : 1;
        unsigned int RDMA3_OUT_VALID           : 1;
        unsigned int RDMA3_SMI_BUSY            : 1;
        unsigned int SMI_GREQ                  : 1;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L0_CLR
{
    PACKING struct
    {
        unsigned int BLUE                      : 8;
        unsigned int GREEN                     : 8;
        unsigned int RED                       : 8;
        unsigned int ALPHA                     : 8;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L1_CLR
{
    PACKING struct
    {
        unsigned int BLUE                      : 8;
        unsigned int GREEN                     : 8;
        unsigned int RED                       : 8;
        unsigned int ALPHA                     : 8;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L2_CLR
{
    PACKING struct
    {
        unsigned int BLUE                      : 8;
        unsigned int GREEN                     : 8;
        unsigned int RED                       : 8;
        unsigned int ALPHA                     : 8;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L3_CLR
{
    PACKING struct
    {
        unsigned int BLUE                      : 8;
        unsigned int GREEN                     : 8;
        unsigned int RED                       : 8;
        unsigned int ALPHA                     : 8;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_LC_CLR
{
    PACKING struct
    {
        unsigned int BLUE                      : 8;
        unsigned int GREEN                     : 8;
        unsigned int RED                       : 8;
        unsigned int ALPHA                     : 8;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_CRC
{
    PACKING struct
    {
        unsigned int CRC_OUT                   : 31;
        unsigned int CRC_RDY                   : 1;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_LC_CON
{
    PACKING struct
    {
        unsigned int ALPHA                     : 8;
        unsigned int ALPHA_EN                  : 1;
        unsigned int rsv_9                     : 11;
        unsigned int EN_3D                     : 1;
        unsigned int LANDSCAPE                 : 1;
        unsigned int R_FIRST                   : 1;
        unsigned int rsv_23                    : 5;
        unsigned int LAYER_SRC                 : 2;
        unsigned int SRCKEY_EN                 : 1;
        unsigned int DSTKEY_EN                 : 1;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_LC_SRCKEY
{
    PACKING struct
    {
        unsigned int SRCKEY                    : 32;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_LC_SRC_SIZE
{
    PACKING struct
    {
        unsigned int LC_SRC_W                  : 13;
        unsigned int rsv_13                    : 3;
        unsigned int LC_SRC_H                  : 13;
        unsigned int rsv_29                    : 3;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_LC_OFFSET
{
    PACKING struct
    {
        unsigned int LC_XOFF                   : 12;
        unsigned int rsv_12                    : 4;
        unsigned int LC_YOFF                   : 12;
        unsigned int rsv_28                    : 4;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_LC_SRC_SEL
{
    PACKING struct
    {
        unsigned int CONST_LAYER_SEL           : 3;
        unsigned int rsv_3                     : 13;
        unsigned int LC_SA_SEL                 : 2;
        unsigned int LC_SRGB_SEL               : 2;
        unsigned int LC_DA_SEL                 : 2;
        unsigned int LC_DRGB_SEL               : 2;
        unsigned int LC_SA_SEL_EXT             : 1;
        unsigned int LC_SRGB_SEL_EXT           : 1;
        unsigned int LC_DA_SEL_EXT             : 1;
        unsigned int LC_DRGB_SEL_EXT           : 1;
        unsigned int LC_CONST_BLD              : 1;
        unsigned int LC_SRGB_SEL_EXT2          : 1;
        unsigned int LC_BLEND_RND_SHT          : 1;
        unsigned int SURFL_EN                  : 1;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_BANK_CON
{
    PACKING struct
    {
        unsigned int OVL_BANK_CON              : 5;
        unsigned int rsv_5                     : 27;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_FUNC_DCM0
{
    PACKING struct
    {
        unsigned int OVL_FUNC_DCM0             : 32;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_FUNC_DCM1
{
    PACKING struct
    {
        unsigned int OVL_FUNC_DCM1             : 32;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_DVFS_L0_ROI
{
    PACKING struct
    {
        unsigned int L0_DVFS_ROI_TB            : 13;
        unsigned int rsv_13                    : 3;
        unsigned int L0_DVFS_ROI_BB            : 13;
        unsigned int rsv_29                    : 3;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_DVFS_L1_ROI
{
    PACKING struct
    {
        unsigned int L1_DVFS_ROI_TB            : 13;
        unsigned int rsv_13                    : 3;
        unsigned int L1_DVFS_ROI_BB            : 13;
        unsigned int rsv_29                    : 3;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_DVFS_L2_ROI
{
    PACKING struct
    {
        unsigned int L2_DVFS_ROI_TB            : 13;
        unsigned int rsv_13                    : 3;
        unsigned int L2_DVFS_ROI_BB            : 13;
        unsigned int rsv_29                    : 3;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_DVFS_L3_ROI
{
    PACKING struct
    {
        unsigned int L3_DVFS_ROI_TB            : 13;
        unsigned int rsv_13                    : 3;
        unsigned int L3_DVFS_ROI_BB            : 13;
        unsigned int rsv_29                    : 3;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_DVFS_EL0_ROI
{
    PACKING struct
    {
        unsigned int EL0_DVFS_ROI_TB           : 13;
        unsigned int rsv_13                    : 3;
        unsigned int EL0_DVFS_ROI_BB           : 13;
        unsigned int rsv_29                    : 3;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_DVFS_EL1_ROI
{
    PACKING struct
    {
        unsigned int EL1_DVFS_ROI_TB           : 13;
        unsigned int rsv_13                    : 3;
        unsigned int EL1_DVFS_ROI_BB           : 13;
        unsigned int rsv_29                    : 3;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_DVFS_EL2_ROI
{
    PACKING struct
    {
        unsigned int EL2_DVFS_ROI_TB           : 13;
        unsigned int rsv_13                    : 3;
        unsigned int EL2_DVFS_ROI_BB           : 13;
        unsigned int rsv_29                    : 3;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_DATAPATH_EXT_CON
{
    PACKING struct
    {
        unsigned int EL0_EN                    : 1;
        unsigned int EL1_EN                    : 1;
        unsigned int EL2_EN                    : 1;
        unsigned int rsv_3                     : 5;
        unsigned int EL0_GPU_MODE              : 1;
        unsigned int EL1_GPU_MODE              : 1;
        unsigned int EL2_GPU_MODE              : 1;
        unsigned int rsv_11                    : 5;
        unsigned int EL0_LAYER_SEL             : 3;
        unsigned int rsv_19                    : 1;
        unsigned int EL1_LAYER_SEL             : 3;
        unsigned int rsv_23                    : 1;
        unsigned int EL2_LAYER_SEL             : 3;
        unsigned int rsv_27                    : 5;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_EL0_CON
{
    PACKING struct
    {
        unsigned int ALPHA                     : 8;
        unsigned int ALPHA_EN                  : 1;
        unsigned int VERTICAL_FLIP_EN          : 1;
        unsigned int HORIZONTAL_FLIP_EN        : 1;
        unsigned int EXT_MTX_EN                : 1;
        unsigned int CLRFMT                    : 4;
        unsigned int INT_MTX_SEL               : 4;
        unsigned int EN_3D                     : 1;
        unsigned int LANDSCAPE                 : 1;
        unsigned int R_FIRST                   : 1;
        unsigned int CLRFMT_MAN                : 1;
        unsigned int BYTE_SWAP                 : 1;
        unsigned int RGB_SWAP                  : 1;
        unsigned int MTX_AUTO_DIS              : 1;
        unsigned int MTX_EN                    : 1;
        unsigned int LAYER_SRC                 : 2;
        unsigned int SRCKEY_EN                 : 1;
        unsigned int DSTKEY_EN                 : 1;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_EL0_SRCKEY
{
    PACKING struct
    {
        unsigned int SRCKEY                    : 32;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_EL0_SRC_SIZE
{
    PACKING struct
    {
        unsigned int EL0_SRC_W                 : 13;
        unsigned int rsv_13                    : 3;
        unsigned int EL0_SRC_H                 : 13;
        unsigned int rsv_29                    : 3;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_EL0_OFFSET
{
    PACKING struct
    {
        unsigned int EL0_XOFF                  : 13;
        unsigned int rsv_13                    : 3;
        unsigned int EL0_YOFF                  : 13;
        unsigned int rsv_29                    : 3;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_EL0_PITCH_MSB
{
    PACKING struct
    {
        unsigned int EL0_SRC_PITCH_MSB         : 4;
        unsigned int rsv_4                     : 12;
        unsigned int EL0_2ND_SUBBUF            : 1;
        unsigned int rsv_17                    : 3;
        unsigned int EL0_YUV_TRANS             : 1;
        unsigned int rsv_21                    : 3;
        unsigned int EL0_HDR_ADR_EN            : 1;
        unsigned int rsv_25                    : 7;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_EL0_PITCH
{
    PACKING struct
    {
        unsigned int EL0_SRC_PITCH             : 16;
        unsigned int EL0_SA_SEL                : 2;
        unsigned int EL0_SRGB_SEL              : 2;
        unsigned int EL0_DA_SEL                : 2;
        unsigned int EL0_DRGB_SEL              : 2;
        unsigned int EL0_SA_SEL_EXT            : 1;
        unsigned int EL0_SRGB_SEL_EXT          : 1;
        unsigned int EL0_DA_SEL_EXT            : 1;
        unsigned int EL0_DRGB_SEL_EXT          : 1;
        unsigned int EL0_CONST_BLD             : 1;
        unsigned int EL0_SRGB_SEL_EXT2         : 1;
        unsigned int EL0_BLEND_RND_SHT         : 1;
        unsigned int SURFL_EN                  : 1;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_EL0_TILE
{
    PACKING struct
    {
        unsigned int TILE_HEIGHT               : 20;
        unsigned int TILE_WIDTH_SEL            : 1;
        unsigned int TILE_EN                   : 1;
        unsigned int rsv_22                    : 2;
        unsigned int TILE_HORI_BLOCK_NUM       : 8;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_EL0_CLIP
{
    PACKING struct
    {
        unsigned int EL0_SRC_LEFT_CLIP         : 8;
        unsigned int EL0_SRC_RIGHT_CLIP        : 8;
        unsigned int EL0_SRC_TOP_CLIP          : 8;
        unsigned int EL0_SRC_BOTTOM_CLIP       : 8;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_EL1_CON
{
    PACKING struct
    {
        unsigned int ALPHA                     : 8;
        unsigned int ALPHA_EN                  : 1;
        unsigned int VERTICAL_FLIP_EN          : 1;
        unsigned int HORIZONTAL_FLIP_EN        : 1;
        unsigned int EXT_MTX_EN                : 1;
        unsigned int CLRFMT                    : 4;
        unsigned int INT_MTX_SEL               : 4;
        unsigned int EN_3D                     : 1;
        unsigned int LANDSCAPE                 : 1;
        unsigned int R_FIRST                   : 1;
        unsigned int CLRFMT_MAN                : 1;
        unsigned int BYTE_SWAP                 : 1;
        unsigned int RGB_SWAP                  : 1;
        unsigned int MTX_AUTO_DIS              : 1;
        unsigned int MTX_EN                    : 1;
        unsigned int LAYER_SRC                 : 2;
        unsigned int SRCKEY_EN                 : 1;
        unsigned int DSTKEY_EN                 : 1;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_EL1_SRCKEY
{
    PACKING struct
    {
        unsigned int SRCKEY                    : 32;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_EL1_SRC_SIZE
{
    PACKING struct
    {
        unsigned int EL1_SRC_W                 : 13;
        unsigned int rsv_13                    : 3;
        unsigned int EL1_SRC_H                 : 13;
        unsigned int rsv_29                    : 3;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_EL1_OFFSET
{
    PACKING struct
    {
        unsigned int EL1_XOFF                  : 13;
        unsigned int rsv_13                    : 3;
        unsigned int EL1_YOFF                  : 13;
        unsigned int rsv_29                    : 3;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_EL1_PITCH_MSB
{
    PACKING struct
    {
        unsigned int EL1_SRC_PITCH_MSB         : 4;
        unsigned int rsv_4                     : 12;
        unsigned int EL1_2ND_SUBBUF            : 1;
        unsigned int rsv_17                    : 3;
        unsigned int EL1_YUV_TRANS             : 1;
        unsigned int rsv_21                    : 3;
        unsigned int EL1_HDR_ADR_EN            : 1;
        unsigned int rsv_25                    : 7;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_EL1_PITCH
{
    PACKING struct
    {
        unsigned int EL1_SRC_PITCH             : 16;
        unsigned int EL1_SA_SEL                : 2;
        unsigned int EL1_SRGB_SEL              : 2;
        unsigned int EL1_DA_SEL                : 2;
        unsigned int EL1_DRGB_SEL              : 2;
        unsigned int EL1_SA_SEL_EXT            : 1;
        unsigned int EL1_SRGB_SEL_EXT          : 1;
        unsigned int EL1_DA_SEL_EXT            : 1;
        unsigned int EL1_DRGB_SEL_EXT          : 1;
        unsigned int EL1_CONST_BLD             : 1;
        unsigned int EL1_SRGB_SEL_EXT2         : 1;
        unsigned int EL1_BLEND_RND_SHT         : 1;
        unsigned int SURFL_EN                  : 1;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_EL1_TILE
{
    PACKING struct
    {
        unsigned int TILE_HEIGHT               : 20;
        unsigned int TILE_WIDTH_SEL            : 1;
        unsigned int TILE_EN                   : 1;
        unsigned int rsv_22                    : 2;
        unsigned int TILE_HORI_BLOCK_NUM       : 8;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_EL1_CLIP
{
    PACKING struct
    {
        unsigned int EL1_SRC_LEFT_CLIP         : 8;
        unsigned int EL1_SRC_RIGHT_CLIP        : 8;
        unsigned int EL1_SRC_TOP_CLIP          : 8;
        unsigned int EL1_SRC_BOTTOM_CLIP       : 8;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_EL2_CON
{
    PACKING struct
    {
        unsigned int ALPHA                     : 8;
        unsigned int ALPHA_EN                  : 1;
        unsigned int VERTICAL_FLIP_EN          : 1;
        unsigned int HORIZONTAL_FLIP_EN        : 1;
        unsigned int EXT_MTX_EN                : 1;
        unsigned int CLRFMT                    : 4;
        unsigned int INT_MTX_SEL               : 4;
        unsigned int EN_3D                     : 1;
        unsigned int LANDSCAPE                 : 1;
        unsigned int R_FIRST                   : 1;
        unsigned int CLRFMT_MAN                : 1;
        unsigned int BYTE_SWAP                 : 1;
        unsigned int RGB_SWAP                  : 1;
        unsigned int MTX_AUTO_DIS              : 1;
        unsigned int MTX_EN                    : 1;
        unsigned int LAYER_SRC                 : 2;
        unsigned int SRCKEY_EN                 : 1;
        unsigned int DSTKEY_EN                 : 1;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_EL2_SRCKEY
{
    PACKING struct
    {
        unsigned int SRCKEY                    : 32;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_EL2_SRC_SIZE
{
    PACKING struct
    {
        unsigned int EL2_SRC_W                 : 13;
        unsigned int rsv_13                    : 3;
        unsigned int EL2_SRC_H                 : 13;
        unsigned int rsv_29                    : 3;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_EL2_OFFSET
{
    PACKING struct
    {
        unsigned int EL2_XOFF                  : 13;
        unsigned int rsv_13                    : 3;
        unsigned int EL2_YOFF                  : 13;
        unsigned int rsv_29                    : 3;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_EL2_PITCH_MSB
{
    PACKING struct
    {
        unsigned int EL2_SRC_PITCH_MSB         : 4;
        unsigned int rsv_4                     : 12;
        unsigned int EL2_2ND_SUBBUF            : 1;
        unsigned int rsv_17                    : 3;
        unsigned int EL2_YUV_TRANS             : 1;
        unsigned int rsv_21                    : 3;
        unsigned int EL2_HDR_ADR_EN            : 1;
        unsigned int rsv_25                    : 7;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_EL2_PITCH
{
    PACKING struct
    {
        unsigned int EL2_SRC_PITCH             : 16;
        unsigned int EL2_SA_SEL                : 2;
        unsigned int EL2_SRGB_SEL              : 2;
        unsigned int EL2_DA_SEL                : 2;
        unsigned int EL2_DRGB_SEL              : 2;
        unsigned int EL2_SA_SEL_EXT            : 1;
        unsigned int EL2_SRGB_SEL_EXT          : 1;
        unsigned int EL2_DA_SEL_EXT            : 1;
        unsigned int EL2_DRGB_SEL_EXT          : 1;
        unsigned int EL2_CONST_BLD             : 1;
        unsigned int EL2_SRGB_SEL_EXT2         : 1;
        unsigned int EL2_BLEND_RND_SHT         : 1;
        unsigned int SURFL_EN                  : 1;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_EL2_TILE
{
    PACKING struct
    {
        unsigned int TILE_HEIGHT               : 20;
        unsigned int TILE_WIDTH_SEL            : 1;
        unsigned int TILE_EN                   : 1;
        unsigned int rsv_22                    : 2;
        unsigned int TILE_HORI_BLOCK_NUM       : 8;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_EL2_CLIP
{
    PACKING struct
    {
        unsigned int EL2_SRC_LEFT_CLIP         : 8;
        unsigned int EL2_SRC_RIGHT_CLIP        : 8;
        unsigned int EL2_SRC_TOP_CLIP          : 8;
        unsigned int EL2_SRC_BOTTOM_CLIP       : 8;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_EL0_CLR
{
    PACKING struct
    {
        unsigned int BLUE                      : 8;
        unsigned int GREEN                     : 8;
        unsigned int RED                       : 8;
        unsigned int ALPHA                     : 8;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_EL1_CLR
{
    PACKING struct
    {
        unsigned int BLUE                      : 8;
        unsigned int GREEN                     : 8;
        unsigned int RED                       : 8;
        unsigned int ALPHA                     : 8;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_EL2_CLR
{
    PACKING struct
    {
        unsigned int BLUE                      : 8;
        unsigned int GREEN                     : 8;
        unsigned int RED                       : 8;
        unsigned int ALPHA                     : 8;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_SBCH
{
    PACKING struct
    {
        unsigned int L0_SBCH_UPDATE            : 1;
        unsigned int rsv_1                     : 3;
        unsigned int L1_SBCH_UPDATE            : 1;
        unsigned int rsv_5                     : 3;
        unsigned int L2_SBCH_UPDATE            : 1;
        unsigned int rsv_9                     : 3;
        unsigned int L3_SBCH_UPDATE            : 1;
        unsigned int rsv_13                    : 3;
        unsigned int L0_SBCH_TRANS_EN          : 1;
        unsigned int L0_SBCH_CNST_EN           : 1;
        unsigned int rsv_18                    : 2;
        unsigned int L1_SBCH_TRANS_EN          : 1;
        unsigned int L1_SBCH_CNST_EN           : 1;
        unsigned int rsv_22                    : 2;
        unsigned int L2_SBCH_TRANS_EN          : 1;
        unsigned int L2_SBCH_CNST_EN           : 1;
        unsigned int rsv_26                    : 2;
        unsigned int L3_SBCH_TRANS_EN          : 1;
        unsigned int L3_SBCH_CNST_EN           : 1;
        unsigned int rsv_30                    : 2;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_SBCH_EXT
{
    PACKING struct
    {
        unsigned int EL0_SBCH_UPDATE           : 1;
        unsigned int rsv_1                     : 3;
        unsigned int EL1_SBCH_UPDATE           : 1;
        unsigned int rsv_5                     : 3;
        unsigned int EL2_SBCH_UPDATE           : 1;
        unsigned int rsv_9                     : 7;
        unsigned int EL0_SBCH_TRANS_EN         : 1;
        unsigned int EL0_SBCH_CNST_EN          : 1;
        unsigned int rsv_18                    : 2;
        unsigned int EL1_SBCH_TRANS_EN         : 1;
        unsigned int EL1_SBCH_CNST_EN          : 1;
        unsigned int rsv_22                    : 2;
        unsigned int EL2_SBCH_TRANS_EN         : 1;
        unsigned int EL2_SBCH_CNST_EN          : 1;
        unsigned int rsv_26                    : 6;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_SBCH_CON
{
    PACKING struct
    {
        unsigned int rsv_0                     : 16;
        unsigned int L0_SBCH_TRANS_INVALID     : 1;
        unsigned int L1_SBCH_TRANS_INVALID     : 1;
        unsigned int L2_SBCH_TRANS_INVALID     : 1;
        unsigned int L3_SBCH_TRANS_INVALID     : 1;
        unsigned int EL0_SBCH_TRANS_INVALID    : 1;
        unsigned int EL1_SBCH_TRANS_INVALID    : 1;
        unsigned int EL2_SBCH_TRANS_INVALID    : 1;
        unsigned int rsv_23                    : 9;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L0_ADDR
{
    PACKING struct
    {
        unsigned int L0_ADDR                   : 32;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L1_ADDR
{
    PACKING struct
    {
        unsigned int L1_ADDR                   : 32;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L2_ADDR
{
    PACKING struct
    {
        unsigned int L2_ADDR                   : 32;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_L3_ADDR
{
    PACKING struct
    {
        unsigned int L3_ADDR                   : 32;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_EL0_ADDR
{
    PACKING struct
    {
        unsigned int EL0_ADDR                  : 32;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_EL1_ADDR
{
    PACKING struct
    {
        unsigned int EL1_ADDR                  : 32;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_EL2_ADDR
{
    PACKING struct
    {
        unsigned int EL2_ADDR                  : 32;
    } Bits;
    u32 Raw;
};

PACKING union DISP_OVL0_REG_OVL_SECURE
{
    PACKING struct
    {
        unsigned int L0_SECURE                 : 1;
        unsigned int L1_SECURE                 : 1;
        unsigned int L2_SECURE                 : 1;
        unsigned int L3_SECURE                 : 1;
        unsigned int rsv_4                     : 28;
    } Bits;
    u32 Raw;
};

// ----------------- DISP_OVL0  Grouping Definitions -------------------
struct DISP_OVL0_OVL_CFG {
    union DISP_OVL0_REG_OVL_INTEN         OVL_INTEN;           //1400B004
    union DISP_OVL0_REG_OVL_TRIG          OVL_TRIG;            //1400B010
    union DISP_OVL0_REG_OVL_RST           OVL_RST;             //1400B014
    union DISP_OVL0_REG_OVL_ROI_SIZE      OVL_ROI_SIZE;        //1400B020
    union DISP_OVL0_REG_OVL_DATAPATH_CON  OVL_DATAPATH_CON;    //1400B024
    union DISP_OVL0_REG_OVL_ROI_BGCLR     OVL_ROI_BGCLR;       //1400B028
    union DISP_OVL0_REG_OVL_SRC_CON       OVL_SRC_CON;         //1400B02C
    union DISP_OVL0_REG_OVL_L0_CON        OVL_L0_CON;          //1400B030
    union DISP_OVL0_REG_OVL_L0_SRCKEY     OVL_L0_SRCKEY;       //1400B034
    union DISP_OVL0_REG_OVL_L0_SRC_SIZE   OVL_L0_SRC_SIZE;     //1400B038
    union DISP_OVL0_REG_OVL_L0_OFFSET     OVL_L0_OFFSET;       //1400B03C
    union DISP_OVL0_REG_OVL_L0_PITCH_MSB  OVL_L0_PITCH_MSB;    //14008040
    union DISP_OVL0_REG_OVL_L0_PITCH      OVL_L0_PITCH;        //1400B044
    union DISP_OVL0_REG_OVL_L0_TILE       OVL_L0_TILE;         //1400B048
    union DISP_OVL0_REG_OVL_L0_CLIP       OVL_L0_CLIP;         //1400B04C
    union DISP_OVL0_REG_OVL_L1_CON        OVL_L1_CON;          //1400B050
    union DISP_OVL0_REG_OVL_L1_SRCKEY     OVL_L1_SRCKEY;       //1400B054
    union DISP_OVL0_REG_OVL_L1_SRC_SIZE   OVL_L1_SRC_SIZE;     //1400B058
    union DISP_OVL0_REG_OVL_L1_OFFSET     OVL_L1_OFFSET;       //1400B05C
    union DISP_OVL0_REG_OVL_L1_PITCH_MSB  OVL_L1_PITCH_MSB;    //14008060
    union DISP_OVL0_REG_OVL_L1_PITCH      OVL_L1_PITCH;        //1400B064
    union DISP_OVL0_REG_OVL_L1_TILE       OVL_L1_TILE;         //1400B068
    union DISP_OVL0_REG_OVL_L1_CLIP       OVL_L1_CLIP;         //1400B06C
    union DISP_OVL0_REG_OVL_L2_CON        OVL_L2_CON;          //1400B070
    union DISP_OVL0_REG_OVL_L2_SRCKEY     OVL_L2_SRCKEY;       //1400B074
    union DISP_OVL0_REG_OVL_L2_SRC_SIZE   OVL_L2_SRC_SIZE;     //1400B078
    union DISP_OVL0_REG_OVL_L2_OFFSET     OVL_L2_OFFSET;       //1400B07C
    union DISP_OVL0_REG_OVL_L2_PITCH_MSB  OVL_L2_PITCH_MSB;    //14008080
    union DISP_OVL0_REG_OVL_L2_PITCH      OVL_L2_PITCH;        //1400B084
    union DISP_OVL0_REG_OVL_L2_TILE       OVL_L2_TILE;         //1400B088
    union DISP_OVL0_REG_OVL_L2_CLIP       OVL_L2_CLIP;         //1400B08C
    union DISP_OVL0_REG_OVL_L3_CON        OVL_L3_CON;          //1400B090
    union DISP_OVL0_REG_OVL_L3_SRCKEY     OVL_L3_SRCKEY;       //1400B094
    union DISP_OVL0_REG_OVL_L3_SRC_SIZE   OVL_L3_SRC_SIZE;     //1400B098
    union DISP_OVL0_REG_OVL_L3_OFFSET     OVL_L3_OFFSET;       //1400B09C
    union DISP_OVL0_REG_OVL_L3_PITCH_MSB  OVL_L3_PITCH_MSB;    //140080A0
    union DISP_OVL0_REG_OVL_L3_PITCH      OVL_L3_PITCH;        //1400B0A4
    union DISP_OVL0_REG_OVL_L3_TILE       OVL_L3_TILE;         //1400B0A8
    union DISP_OVL0_REG_OVL_L3_CLIP       OVL_L3_CLIP;         //1400B0AC
    union DISP_OVL0_REG_OVL_RDMA0_CTRL    OVL_RDMA0_CTRL;      //1400B0C0
    union DISP_OVL0_REG_OVL_RDMA0_MEM_GMC_SETTING1 OVL_RDMA0_MEM_GMC_SETTING1; //1400B0C8
    union DISP_OVL0_REG_OVL_RDMA0_MEM_SLOW_CON OVL_RDMA0_MEM_SLOW_CON; //1400B0CC
    union DISP_OVL0_REG_OVL_RDMA0_FIFO_CTRL OVL_RDMA0_FIFO_CTRL; //1400B0D0
    union DISP_OVL0_REG_OVL_RDMA1_CTRL    OVL_RDMA1_CTRL;      //1400B0E0
    union DISP_OVL0_REG_OVL_RDMA1_MEM_GMC_SETTING1 OVL_RDMA1_MEM_GMC_SETTING1; //1400B0E8
    union DISP_OVL0_REG_OVL_RDMA1_MEM_SLOW_CON OVL_RDMA1_MEM_SLOW_CON; //1400B0EC
    union DISP_OVL0_REG_OVL_RDMA1_FIFO_CTRL OVL_RDMA1_FIFO_CTRL; //1400B0F0
    union DISP_OVL0_REG_OVL_RDMA2_CTRL    OVL_RDMA2_CTRL;      //1400B100
    union DISP_OVL0_REG_OVL_RDMA2_MEM_GMC_SETTING1 OVL_RDMA2_MEM_GMC_SETTING1; //1400B108
    union DISP_OVL0_REG_OVL_RDMA2_MEM_SLOW_CON OVL_RDMA2_MEM_SLOW_CON; //1400B10C
    union DISP_OVL0_REG_OVL_RDMA2_FIFO_CTRL OVL_RDMA2_FIFO_CTRL; //1400B110
    union DISP_OVL0_REG_OVL_RDMA3_CTRL    OVL_RDMA3_CTRL;      //1400B120
    union DISP_OVL0_REG_OVL_RDMA3_MEM_GMC_SETTING1 OVL_RDMA3_MEM_GMC_SETTING1; //1400B128
    union DISP_OVL0_REG_OVL_RDMA3_MEM_SLOW_CON OVL_RDMA3_MEM_SLOW_CON; //1400B12C
    union DISP_OVL0_REG_OVL_RDMA3_FIFO_CTRL OVL_RDMA3_FIFO_CTRL; //1400B130
    union DISP_OVL0_REG_OVL_L0_Y2R_PARA_R0 OVL_L0_Y2R_PARA_R0;  //1400B134
    union DISP_OVL0_REG_OVL_L0_Y2R_PARA_R1 OVL_L0_Y2R_PARA_R1;  //1400B138
    union DISP_OVL0_REG_OVL_L0_Y2R_PARA_G0 OVL_L0_Y2R_PARA_G0;  //1400B13C
    union DISP_OVL0_REG_OVL_L0_Y2R_PARA_G1 OVL_L0_Y2R_PARA_G1;  //1400B140
    union DISP_OVL0_REG_OVL_L0_Y2R_PARA_B0 OVL_L0_Y2R_PARA_B0;  //1400B144
    union DISP_OVL0_REG_OVL_L0_Y2R_PARA_B1 OVL_L0_Y2R_PARA_B1;  //1400B148
    union DISP_OVL0_REG_OVL_L0_Y2R_PARA_YUV_A_0 OVL_L0_Y2R_PARA_YUV_A_0; //1400B14C
    union DISP_OVL0_REG_OVL_L0_Y2R_PARA_YUV_A_1 OVL_L0_Y2R_PARA_YUV_A_1; //1400B150
    union DISP_OVL0_REG_OVL_L0_Y2R_PARA_RGB_A_0 OVL_L0_Y2R_PARA_RGB_A_0; //1400B154
    union DISP_OVL0_REG_OVL_L0_Y2R_PARA_RGB_A_1 OVL_L0_Y2R_PARA_RGB_A_1; //1400B158
    union DISP_OVL0_REG_OVL_L1_Y2R_PARA_R0 OVL_L1_Y2R_PARA_R0;  //1400B15C
    union DISP_OVL0_REG_OVL_L1_Y2R_PARA_R1 OVL_L1_Y2R_PARA_R1;  //1400B160
    union DISP_OVL0_REG_OVL_L1_Y2R_PARA_G0 OVL_L1_Y2R_PARA_G0;  //1400B164
    union DISP_OVL0_REG_OVL_L1_Y2R_PARA_G1 OVL_L1_Y2R_PARA_G1;  //1400B168
    union DISP_OVL0_REG_OVL_L1_Y2R_PARA_B0 OVL_L1_Y2R_PARA_B0;  //1400B16C
    union DISP_OVL0_REG_OVL_L1_Y2R_PARA_B1 OVL_L1_Y2R_PARA_B1;  //1400B170
    union DISP_OVL0_REG_OVL_L1_Y2R_PARA_YUV_A_0 OVL_L1_Y2R_PARA_YUV_A_0; //1400B174
    union DISP_OVL0_REG_OVL_L1_Y2R_PARA_YUV_A_1 OVL_L1_Y2R_PARA_YUV_A_1; //1400B178
    union DISP_OVL0_REG_OVL_L1_Y2R_PARA_RGB_A_0 OVL_L1_Y2R_PARA_RGB_A_0; //1400B17C
    union DISP_OVL0_REG_OVL_L1_Y2R_PARA_RGB_A_1 OVL_L1_Y2R_PARA_RGB_A_1; //1400B180
    union DISP_OVL0_REG_OVL_L2_Y2R_PARA_R0 OVL_L2_Y2R_PARA_R0;  //1400B184
    union DISP_OVL0_REG_OVL_L2_Y2R_PARA_R1 OVL_L2_Y2R_PARA_R1;  //1400B188
    union DISP_OVL0_REG_OVL_L2_Y2R_PARA_G0 OVL_L2_Y2R_PARA_G0;  //1400B18C
    union DISP_OVL0_REG_OVL_L2_Y2R_PARA_G1 OVL_L2_Y2R_PARA_G1;  //1400B190
    union DISP_OVL0_REG_OVL_L2_Y2R_PARA_B0 OVL_L2_Y2R_PARA_B0;  //1400B194
    union DISP_OVL0_REG_OVL_L2_Y2R_PARA_B1 OVL_L2_Y2R_PARA_B1;  //1400B198
    union DISP_OVL0_REG_OVL_L2_Y2R_PARA_YUV_A_0 OVL_L2_Y2R_PARA_YUV_A_0; //1400B19C
    union DISP_OVL0_REG_OVL_L2_Y2R_PARA_YUV_A_1 OVL_L2_Y2R_PARA_YUV_A_1; //1400B1A0
    union DISP_OVL0_REG_OVL_L2_Y2R_PARA_RGB_A_0 OVL_L2_Y2R_PARA_RGB_A_0; //1400B1A4
    union DISP_OVL0_REG_OVL_L2_Y2R_PARA_RGB_A_1 OVL_L2_Y2R_PARA_RGB_A_1; //1400B1A8
    union DISP_OVL0_REG_OVL_L3_Y2R_PARA_R0 OVL_L3_Y2R_PARA_R0;  //1400B1AC
    union DISP_OVL0_REG_OVL_L3_Y2R_PARA_R1 OVL_L3_Y2R_PARA_R1;  //1400B1B0
    union DISP_OVL0_REG_OVL_L3_Y2R_PARA_G0 OVL_L3_Y2R_PARA_G0;  //1400B1B4
    union DISP_OVL0_REG_OVL_L3_Y2R_PARA_G1 OVL_L3_Y2R_PARA_G1;  //1400B1B8
    union DISP_OVL0_REG_OVL_L3_Y2R_PARA_B0 OVL_L3_Y2R_PARA_B0;  //1400B1BC
    union DISP_OVL0_REG_OVL_L3_Y2R_PARA_B1 OVL_L3_Y2R_PARA_B1;  //1400B1C0
    union DISP_OVL0_REG_OVL_L3_Y2R_PARA_YUV_A_0 OVL_L3_Y2R_PARA_YUV_A_0; //1400B1C4
    union DISP_OVL0_REG_OVL_L3_Y2R_PARA_YUV_A_1 OVL_L3_Y2R_PARA_YUV_A_1; //1400B1C8
    union DISP_OVL0_REG_OVL_L3_Y2R_PARA_RGB_A_0 OVL_L3_Y2R_PARA_RGB_A_0; //1400B1CC
    union DISP_OVL0_REG_OVL_L3_Y2R_PARA_RGB_A_1 OVL_L3_Y2R_PARA_RGB_A_1; //1400B1D0
    union DISP_OVL0_REG_OVL_DEBUG_MON_SEL OVL_DEBUG_MON_SEL;   //1400B1D4
    union DISP_OVL0_REG_OVL_BLD_EXT       OVL_BLD_EXT;         //1400B1DC
    union DISP_OVL0_REG_OVL_RDMA0_MEM_GMC_SETTING2 OVL_RDMA0_MEM_GMC_SETTING2; //1400B1E0
    union DISP_OVL0_REG_OVL_RDMA1_MEM_GMC_SETTING2 OVL_RDMA1_MEM_GMC_SETTING2; //1400B1E4
    union DISP_OVL0_REG_OVL_RDMA2_MEM_GMC_SETTING2 OVL_RDMA2_MEM_GMC_SETTING2; //1400B1E8
    union DISP_OVL0_REG_OVL_RDMA3_MEM_GMC_SETTING2 OVL_RDMA3_MEM_GMC_SETTING2; //1400B1EC
    union DISP_OVL0_REG_OVL_RDMA_BURST_CON0 OVL_RDMA_BURST_CON0; //1400B1F0
    union DISP_OVL0_REG_OVL_RDMA_BURST_CON1 OVL_RDMA_BURST_CON1; //1400B1F4
    union DISP_OVL0_REG_OVL_RDMA_GREQ_NUM OVL_RDMA_GREQ_NUM;   //1400B1F8
    union DISP_OVL0_REG_OVL_RDMA_GREQ_URG_NUM OVL_RDMA_GREQ_URG_NUM; //1400B1FC
    union DISP_OVL0_REG_OVL_DUMMY_REG     OVL_DUMMY_REG;       //1400B200
    union DISP_OVL0_REG_OVL_GDRDY_PRD     OVL_GDRDY_PRD;       //1400B208
    union DISP_OVL0_REG_OVL_RDMA_ULTRA_SRC OVL_RDMA_ULTRA_SRC;  //1400B20C
    union DISP_OVL0_REG_OVL_RDMA0_BUF_LOW_TH OVL_RDMA0_BUF_LOW_TH; //1400B210
    union DISP_OVL0_REG_OVL_RDMA1_BUF_LOW_TH OVL_RDMA1_BUF_LOW_TH; //1400B214
    union DISP_OVL0_REG_OVL_RDMA2_BUF_LOW_TH OVL_RDMA2_BUF_LOW_TH; //1400B218
    union DISP_OVL0_REG_OVL_RDMA3_BUF_LOW_TH OVL_RDMA3_BUF_LOW_TH; //1400B21C
    union DISP_OVL0_REG_OVL_RDMA0_BUF_HIGH_TH OVL_RDMA0_BUF_HIGH_TH; //1400B220
    union DISP_OVL0_REG_OVL_RDMA1_BUF_HIGH_TH OVL_RDMA1_BUF_HIGH_TH; //1400B224
    union DISP_OVL0_REG_OVL_RDMA2_BUF_HIGH_TH OVL_RDMA2_BUF_HIGH_TH; //1400B228
    union DISP_OVL0_REG_OVL_RDMA3_BUF_HIGH_TH OVL_RDMA3_BUF_HIGH_TH; //1400B22C
    union DISP_OVL0_REG_OVL_SMI_DBG       OVL_SMI_DBG;         //1400B230
    union DISP_OVL0_REG_OVL_GREQ_LAYER_CNT OVL_GREQ_LAYER_CNT;  //1400B234
    union DISP_OVL0_REG_OVL_GDRDY_PRD_NUM OVL_GDRDY_PRD_NUM;   //1400B238
    union DISP_OVL0_REG_OVL_DEBUG_FLAG OVL_DEBUG_FLAG;         //1400823C
    union DISP_OVL0_REG_OVL_L0_CLR        OVL_L0_CLR;          //1400B25C
    union DISP_OVL0_REG_OVL_L1_CLR        OVL_L1_CLR;          //1400B260
    union DISP_OVL0_REG_OVL_L2_CLR        OVL_L2_CLR;          //1400B264
    union DISP_OVL0_REG_OVL_L3_CLR        OVL_L3_CLR;          //1400B268
    union DISP_OVL0_REG_OVL_LC_CLR        OVL_LC_CLR;          //1400B26C
    union DISP_OVL0_REG_OVL_CRC           OVL_CRC;             //1400B270
    union DISP_OVL0_REG_OVL_LC_CON        OVL_LC_CON;          //1400B280
    union DISP_OVL0_REG_OVL_LC_SRCKEY     OVL_LC_SRCKEY;       //1400B284
    union DISP_OVL0_REG_OVL_LC_SRC_SIZE   OVL_LC_SRC_SIZE;     //1400B288
    union DISP_OVL0_REG_OVL_LC_OFFSET     OVL_LC_OFFSET;       //1400B28C
    union DISP_OVL0_REG_OVL_LC_SRC_SEL    OVL_LC_SRC_SEL;      //1400B290
    union DISP_OVL0_REG_OVL_BANK_CON      OVL_BANK_CON;        //1400B29C
    union DISP_OVL0_REG_OVL_FUNC_DCM0     OVL_FUNC_DCM0;       //1400B2A0
    union DISP_OVL0_REG_OVL_FUNC_DCM1     OVL_FUNC_DCM1;       //1400B2A4
    union DISP_OVL0_REG_OVL_DVFS_L0_ROI   OVL_DVFS_L0_ROI;     //1400B2B0
    union DISP_OVL0_REG_OVL_DVFS_L1_ROI   OVL_DVFS_L1_ROI;     //1400B2B4
    union DISP_OVL0_REG_OVL_DVFS_L2_ROI   OVL_DVFS_L2_ROI;     //1400B2B8
    union DISP_OVL0_REG_OVL_DVFS_L3_ROI   OVL_DVFS_L3_ROI;     //1400B2BC
    union DISP_OVL0_REG_OVL_DVFS_EL0_ROI  OVL_DVFS_EL0_ROI;    //1400B2C0
    union DISP_OVL0_REG_OVL_DVFS_EL1_ROI  OVL_DVFS_EL1_ROI;    //1400B2C4
    union DISP_OVL0_REG_OVL_DVFS_EL2_ROI  OVL_DVFS_EL2_ROI;    //1400B2C8
    union DISP_OVL0_REG_OVL_DATAPATH_EXT_CON OVL_DATAPATH_EXT_CON; //1400B324
    union DISP_OVL0_REG_OVL_EL0_CON       OVL_EL0_CON;         //1400B330
    union DISP_OVL0_REG_OVL_EL0_SRCKEY    OVL_EL0_SRCKEY;      //1400B334
    union DISP_OVL0_REG_OVL_EL0_SRC_SIZE  OVL_EL0_SRC_SIZE;    //1400B338
    union DISP_OVL0_REG_OVL_EL0_OFFSET    OVL_EL0_OFFSET;      //1400B33C
    union DISP_OVL0_REG_OVL_EL0_PITCH_MSB  OVL_EL0_PITCH_MSB;  //14008340
    union DISP_OVL0_REG_OVL_EL0_PITCH     OVL_EL0_PITCH;       //1400B344
    union DISP_OVL0_REG_OVL_EL0_TILE      OVL_EL0_TILE;        //1400B348
    union DISP_OVL0_REG_OVL_EL0_CLIP      OVL_EL0_CLIP;        //1400B34C
    union DISP_OVL0_REG_OVL_EL1_CON       OVL_EL1_CON;         //1400B350
    union DISP_OVL0_REG_OVL_EL1_SRCKEY    OVL_EL1_SRCKEY;      //1400B354
    union DISP_OVL0_REG_OVL_EL1_SRC_SIZE  OVL_EL1_SRC_SIZE;    //1400B358
    union DISP_OVL0_REG_OVL_EL1_OFFSET    OVL_EL1_OFFSET;      //1400B35C
    union DISP_OVL0_REG_OVL_EL1_PITCH_MSB  OVL_EL1_PITCH_MSB;  //14008360
    union DISP_OVL0_REG_OVL_EL1_PITCH     OVL_EL1_PITCH;       //1400B364
    union DISP_OVL0_REG_OVL_EL1_TILE      OVL_EL1_TILE;        //1400B368
    union DISP_OVL0_REG_OVL_EL1_CLIP      OVL_EL1_CLIP;        //1400B36C
    union DISP_OVL0_REG_OVL_EL2_CON       OVL_EL2_CON;         //1400B370
    union DISP_OVL0_REG_OVL_EL2_SRCKEY    OVL_EL2_SRCKEY;      //1400B374
    union DISP_OVL0_REG_OVL_EL2_SRC_SIZE  OVL_EL2_SRC_SIZE;    //1400B378
    union DISP_OVL0_REG_OVL_EL2_OFFSET    OVL_EL2_OFFSET;      //1400B37C
    union DISP_OVL0_REG_OVL_EL2_PITCH_MSB  OVL_EL2_PITCH_MSB;  //14008380
    union DISP_OVL0_REG_OVL_EL2_PITCH     OVL_EL2_PITCH;       //1400B384
    union DISP_OVL0_REG_OVL_EL2_TILE      OVL_EL2_TILE;        //1400B388
    union DISP_OVL0_REG_OVL_EL2_CLIP      OVL_EL2_CLIP;        //1400B38C
    union DISP_OVL0_REG_OVL_EL0_CLR       OVL_EL0_CLR;         //1400B390
    union DISP_OVL0_REG_OVL_EL1_CLR       OVL_EL1_CLR;         //1400B394
    union DISP_OVL0_REG_OVL_EL2_CLR       OVL_EL2_CLR;         //1400B398
    union DISP_OVL0_REG_OVL_SBCH          OVL_SBCH;            //1400B3A0
    union DISP_OVL0_REG_OVL_SBCH_EXT      OVL_SBCH_EXT;        //1400B3A4
    union DISP_OVL0_REG_OVL_SBCH_CON      OVL_SBCH_CON;        //1400B3A8
    union DISP_OVL0_REG_OVL_L0_ADDR       OVL_L0_ADDR;         //1400BF40
    union DISP_OVL0_REG_OVL_L1_ADDR       OVL_L1_ADDR;         //1400BF60
    union DISP_OVL0_REG_OVL_L2_ADDR       OVL_L2_ADDR;         //1400BF80
    union DISP_OVL0_REG_OVL_L3_ADDR       OVL_L3_ADDR;         //1400BFA0
    union DISP_OVL0_REG_OVL_EL0_ADDR      OVL_EL0_ADDR;        //1400BFB0
    union DISP_OVL0_REG_OVL_EL1_ADDR      OVL_EL1_ADDR;        //1400BFB4
    union DISP_OVL0_REG_OVL_EL2_ADDR      OVL_EL2_ADDR;        //1400BFB8
    union DISP_OVL0_REG_OVL_SECURE        OVL_SECURE;          //1400BFC0
};

struct DISP_OVL0_OVL_STATUS {
    union DISP_OVL0_REG_OVL_STA           OVL_STA;             //1400B000
    union DISP_OVL0_REG_OVL_INTSTA        OVL_INTSTA;          //1400B008
};

struct DISP_OVL0_OVL_START {
    union DISP_OVL0_REG_OVL_EN            OVL_EN;              //1400B00C
};

// ----------------- DISP_OVL0 Register Definition -------------------
/* modify by RD */
struct DISP_OVL0_REGS {
    union DISP_OVL0_REG_OVL_STA           OVL_STA;             //14008000
    union DISP_OVL0_REG_OVL_INTEN         OVL_INTEN;           //14008004
    union DISP_OVL0_REG_OVL_INTSTA        OVL_INTSTA;          //14008008
    union DISP_OVL0_REG_OVL_EN            OVL_EN;              //1400800C
    union DISP_OVL0_REG_OVL_TRIG          OVL_TRIG;            //14008010
    union DISP_OVL0_REG_OVL_RST           OVL_RST;             //14008014
    u32                          rsv_8018[2];         //14008018..801C
    union DISP_OVL0_REG_OVL_ROI_SIZE      OVL_ROI_SIZE;        //14008020
    union DISP_OVL0_REG_OVL_DATAPATH_CON  OVL_DATAPATH_CON;    //14008024
    union DISP_OVL0_REG_OVL_ROI_BGCLR     OVL_ROI_BGCLR;       //14008028
    union DISP_OVL0_REG_OVL_SRC_CON       OVL_SRC_CON;         //1400802C
    union DISP_OVL0_REG_OVL_L0_CON        OVL_L0_CON;          //14008030
    union DISP_OVL0_REG_OVL_L0_SRCKEY     OVL_L0_SRCKEY;       //14008034
    union DISP_OVL0_REG_OVL_L0_SRC_SIZE   OVL_L0_SRC_SIZE;     //14008038
    union DISP_OVL0_REG_OVL_L0_OFFSET     OVL_L0_OFFSET;       //1400803C
    union DISP_OVL0_REG_OVL_L0_PITCH_MSB  OVL_L0_PITCH_MSB;    //14008040
    union DISP_OVL0_REG_OVL_L0_PITCH      OVL_L0_PITCH;        //14008044
    union DISP_OVL0_REG_OVL_L0_TILE       OVL_L0_TILE;         //14008048
    union DISP_OVL0_REG_OVL_L0_CLIP       OVL_L0_CLIP;         //1400804C
    union DISP_OVL0_REG_OVL_L1_CON        OVL_L1_CON;          //14008050
    union DISP_OVL0_REG_OVL_L1_SRCKEY     OVL_L1_SRCKEY;       //14008054
    union DISP_OVL0_REG_OVL_L1_SRC_SIZE   OVL_L1_SRC_SIZE;     //14008058
    union DISP_OVL0_REG_OVL_L1_OFFSET     OVL_L1_OFFSET;       //1400805C
    union DISP_OVL0_REG_OVL_L1_PITCH_MSB  OVL_L1_PITCH_MSB;    //14008060
    union DISP_OVL0_REG_OVL_L1_PITCH      OVL_L1_PITCH;        //14008064
    union DISP_OVL0_REG_OVL_L1_TILE       OVL_L1_TILE;         //14008068
    union DISP_OVL0_REG_OVL_L1_CLIP       OVL_L1_CLIP;         //1400806C
    union DISP_OVL0_REG_OVL_L2_CON        OVL_L2_CON;          //14008070
    union DISP_OVL0_REG_OVL_L2_SRCKEY     OVL_L2_SRCKEY;       //14008074
    union DISP_OVL0_REG_OVL_L2_SRC_SIZE   OVL_L2_SRC_SIZE;     //14008078
    union DISP_OVL0_REG_OVL_L2_OFFSET     OVL_L2_OFFSET;       //1400807C
    union DISP_OVL0_REG_OVL_L2_PITCH_MSB  OVL_L2_PITCH_MSB;    //14008080
    union DISP_OVL0_REG_OVL_L2_PITCH      OVL_L2_PITCH;        //14008084
    union DISP_OVL0_REG_OVL_L2_TILE       OVL_L2_TILE;         //14008088
    union DISP_OVL0_REG_OVL_L2_CLIP       OVL_L2_CLIP;         //1400808C
    union DISP_OVL0_REG_OVL_L3_CON        OVL_L3_CON;          //14008090
    union DISP_OVL0_REG_OVL_L3_SRCKEY     OVL_L3_SRCKEY;       //14008094
    union DISP_OVL0_REG_OVL_L3_SRC_SIZE   OVL_L3_SRC_SIZE;     //14008098
    union DISP_OVL0_REG_OVL_L3_OFFSET     OVL_L3_OFFSET;       //1400809C
    union DISP_OVL0_REG_OVL_L3_PITCH_MSB  OVL_L3_PITCH_MSB;    //140080A0
    union DISP_OVL0_REG_OVL_L3_PITCH      OVL_L3_PITCH;        //140080A4
    union DISP_OVL0_REG_OVL_L3_TILE       OVL_L3_TILE;         //140080A8
    union DISP_OVL0_REG_OVL_L3_CLIP       OVL_L3_CLIP;         //140080AC
    u32                          rsv_80B0[4];           //140080B0..80BC
    union DISP_OVL0_REG_OVL_RDMA0_CTRL    OVL_RDMA0_CTRL;      //140080C0
    u32                          rsv_80C4;              //140080C4
    union DISP_OVL0_REG_OVL_RDMA0_MEM_GMC_SETTING1 OVL_RDMA0_MEM_GMC_SETTING1; //140080C8
    union DISP_OVL0_REG_OVL_RDMA0_MEM_SLOW_CON OVL_RDMA0_MEM_SLOW_CON; //140080CC
    union DISP_OVL0_REG_OVL_RDMA0_FIFO_CTRL OVL_RDMA0_FIFO_CTRL; //140080D0
    u32                          rsv_80D4[3];           //140080D4..80DC
    union DISP_OVL0_REG_OVL_RDMA1_CTRL    OVL_RDMA1_CTRL;        //140080E0
    u32                          rsv_80E4;               //140080E4
    union DISP_OVL0_REG_OVL_RDMA1_MEM_GMC_SETTING1 OVL_RDMA1_MEM_GMC_SETTING1; //140080E8
    union DISP_OVL0_REG_OVL_RDMA1_MEM_SLOW_CON OVL_RDMA1_MEM_SLOW_CON; //140080EC
    union DISP_OVL0_REG_OVL_RDMA1_FIFO_CTRL OVL_RDMA1_FIFO_CTRL; //140080F0
    u32                          rsv_80F4[3];             //140080F4..80FC
    union DISP_OVL0_REG_OVL_RDMA2_CTRL    OVL_RDMA2_CTRL;        //14008100
    u32                          rsv_8104;               //14008104
    union DISP_OVL0_REG_OVL_RDMA2_MEM_GMC_SETTING1 OVL_RDMA2_MEM_GMC_SETTING1; //14008108
    union DISP_OVL0_REG_OVL_RDMA2_MEM_SLOW_CON OVL_RDMA2_MEM_SLOW_CON; //1400810C
    union DISP_OVL0_REG_OVL_RDMA2_FIFO_CTRL OVL_RDMA2_FIFO_CTRL; //14008110
    u32                          rsv_8114[3];           //14008114..811C
    union DISP_OVL0_REG_OVL_RDMA3_CTRL    OVL_RDMA3_CTRL;        //14008120
    u32                          rsv_8124;                 //14008124
    union DISP_OVL0_REG_OVL_RDMA3_MEM_GMC_SETTING1 OVL_RDMA3_MEM_GMC_SETTING1; //14008128
    union DISP_OVL0_REG_OVL_RDMA3_MEM_SLOW_CON OVL_RDMA3_MEM_SLOW_CON; //1400812C
    union DISP_OVL0_REG_OVL_RDMA3_FIFO_CTRL OVL_RDMA3_FIFO_CTRL;  //14008130
    union DISP_OVL0_REG_OVL_L0_Y2R_PARA_R0 OVL_L0_Y2R_PARA_R0;    //14008134
    union DISP_OVL0_REG_OVL_L0_Y2R_PARA_R1 OVL_L0_Y2R_PARA_R1;    //14008138
    union DISP_OVL0_REG_OVL_L0_Y2R_PARA_G0 OVL_L0_Y2R_PARA_G0;    //1400813C
    union DISP_OVL0_REG_OVL_L0_Y2R_PARA_G1 OVL_L0_Y2R_PARA_G1;    //14008140
    union DISP_OVL0_REG_OVL_L0_Y2R_PARA_B0 OVL_L0_Y2R_PARA_B0;    //14008144
    union DISP_OVL0_REG_OVL_L0_Y2R_PARA_B1 OVL_L0_Y2R_PARA_B1;    //14008148
    union DISP_OVL0_REG_OVL_L0_Y2R_PARA_YUV_A_0 OVL_L0_Y2R_PARA_YUV_A_0; //1400814C
    union DISP_OVL0_REG_OVL_L0_Y2R_PARA_YUV_A_1 OVL_L0_Y2R_PARA_YUV_A_1; //14008150
    union DISP_OVL0_REG_OVL_L0_Y2R_PARA_RGB_A_0 OVL_L0_Y2R_PARA_RGB_A_0; //14008154
    union DISP_OVL0_REG_OVL_L0_Y2R_PARA_RGB_A_1 OVL_L0_Y2R_PARA_RGB_A_1; //14008158
    union DISP_OVL0_REG_OVL_L1_Y2R_PARA_R0 OVL_L1_Y2R_PARA_R0;    //1400815C
    union DISP_OVL0_REG_OVL_L1_Y2R_PARA_R1 OVL_L1_Y2R_PARA_R1;    //14008160
    union DISP_OVL0_REG_OVL_L1_Y2R_PARA_G0 OVL_L1_Y2R_PARA_G0;    //14008164
    union DISP_OVL0_REG_OVL_L1_Y2R_PARA_G1 OVL_L1_Y2R_PARA_G1;    //14008168
    union DISP_OVL0_REG_OVL_L1_Y2R_PARA_B0 OVL_L1_Y2R_PARA_B0;    //1400816C
    union DISP_OVL0_REG_OVL_L1_Y2R_PARA_B1 OVL_L1_Y2R_PARA_B1;    //14008170
    union DISP_OVL0_REG_OVL_L1_Y2R_PARA_YUV_A_0 OVL_L1_Y2R_PARA_YUV_A_0; //14008174
    union DISP_OVL0_REG_OVL_L1_Y2R_PARA_YUV_A_1 OVL_L1_Y2R_PARA_YUV_A_1; //14008178
    union DISP_OVL0_REG_OVL_L1_Y2R_PARA_RGB_A_0 OVL_L1_Y2R_PARA_RGB_A_0; //1400817C
    union DISP_OVL0_REG_OVL_L1_Y2R_PARA_RGB_A_1 OVL_L1_Y2R_PARA_RGB_A_1; //14008180
    union DISP_OVL0_REG_OVL_L2_Y2R_PARA_R0 OVL_L2_Y2R_PARA_R0;    //14008184
    union DISP_OVL0_REG_OVL_L2_Y2R_PARA_R1 OVL_L2_Y2R_PARA_R1;    //14008188
    union DISP_OVL0_REG_OVL_L2_Y2R_PARA_G0 OVL_L2_Y2R_PARA_G0;    //1400818C
    union DISP_OVL0_REG_OVL_L2_Y2R_PARA_G1 OVL_L2_Y2R_PARA_G1;    //14008190
    union DISP_OVL0_REG_OVL_L2_Y2R_PARA_B0 OVL_L2_Y2R_PARA_B0;    //14008194
    union DISP_OVL0_REG_OVL_L2_Y2R_PARA_B1 OVL_L2_Y2R_PARA_B1;    //14008198
    union DISP_OVL0_REG_OVL_L2_Y2R_PARA_YUV_A_0 OVL_L2_Y2R_PARA_YUV_A_0; //1400819C
    union DISP_OVL0_REG_OVL_L2_Y2R_PARA_YUV_A_1 OVL_L2_Y2R_PARA_YUV_A_1; //140081A0
    union DISP_OVL0_REG_OVL_L2_Y2R_PARA_RGB_A_0 OVL_L2_Y2R_PARA_RGB_A_0; //140081A4
    union DISP_OVL0_REG_OVL_L2_Y2R_PARA_RGB_A_1 OVL_L2_Y2R_PARA_RGB_A_1; //140081A8
    union DISP_OVL0_REG_OVL_L3_Y2R_PARA_R0 OVL_L3_Y2R_PARA_R0;    //140081AC
    union DISP_OVL0_REG_OVL_L3_Y2R_PARA_R1 OVL_L3_Y2R_PARA_R1;    //140081B0
    union DISP_OVL0_REG_OVL_L3_Y2R_PARA_G0 OVL_L3_Y2R_PARA_G0;    //140081B4
    union DISP_OVL0_REG_OVL_L3_Y2R_PARA_G1 OVL_L3_Y2R_PARA_G1;    //140081B8
    union DISP_OVL0_REG_OVL_L3_Y2R_PARA_B0 OVL_L3_Y2R_PARA_B0;    //140081BC
    union DISP_OVL0_REG_OVL_L3_Y2R_PARA_B1 OVL_L3_Y2R_PARA_B1;    //140081C0
    union DISP_OVL0_REG_OVL_L3_Y2R_PARA_YUV_A_0 OVL_L3_Y2R_PARA_YUV_A_0; //140081C4
    union DISP_OVL0_REG_OVL_L3_Y2R_PARA_YUV_A_1 OVL_L3_Y2R_PARA_YUV_A_1; //140081C8
    union DISP_OVL0_REG_OVL_L3_Y2R_PARA_RGB_A_0 OVL_L3_Y2R_PARA_RGB_A_0; //140081CC
    union DISP_OVL0_REG_OVL_L3_Y2R_PARA_RGB_A_1 OVL_L3_Y2R_PARA_RGB_A_1; //140081D0
    union DISP_OVL0_REG_OVL_DEBUG_MON_SEL OVL_DEBUG_MON_SEL;                  //140081D4
    u32                          rsv_81D8;                          //140081D8
    union DISP_OVL0_REG_OVL_BLD_EXT       OVL_BLD_EXT;                      //140081DC
    union DISP_OVL0_REG_OVL_RDMA0_MEM_GMC_SETTING2 OVL_RDMA0_MEM_GMC_SETTING2; //140081E0
    union DISP_OVL0_REG_OVL_RDMA1_MEM_GMC_SETTING2 OVL_RDMA1_MEM_GMC_SETTING2; //140081E4
    union DISP_OVL0_REG_OVL_RDMA2_MEM_GMC_SETTING2 OVL_RDMA2_MEM_GMC_SETTING2; //140081E8
    union DISP_OVL0_REG_OVL_RDMA3_MEM_GMC_SETTING2 OVL_RDMA3_MEM_GMC_SETTING2; //140081EC
    union DISP_OVL0_REG_OVL_RDMA_BURST_CON0 OVL_RDMA_BURST_CON0; //140081F0
    union DISP_OVL0_REG_OVL_RDMA_BURST_CON1 OVL_RDMA_BURST_CON1; //140081F4
    union DISP_OVL0_REG_OVL_RDMA_GREQ_NUM OVL_RDMA_GREQ_NUM;     //140081F8
    union DISP_OVL0_REG_OVL_RDMA_GREQ_URG_NUM OVL_RDMA_GREQ_URG_NUM; //140081FC
    union DISP_OVL0_REG_OVL_DUMMY_REG     OVL_DUMMY_REG;       //14008200
    u32                          rsv_8204;            //14008204
    union DISP_OVL0_REG_OVL_GDRDY_PRD     OVL_GDRDY_PRD;         //14008208
    union DISP_OVL0_REG_OVL_RDMA_ULTRA_SRC OVL_RDMA_ULTRA_SRC; //1400820C
    union DISP_OVL0_REG_OVL_RDMA0_BUF_LOW_TH OVL_RDMA0_BUF_LOW;     //14008210
    union DISP_OVL0_REG_OVL_RDMA1_BUF_LOW_TH OVL_RDMA1_BUF_LOW;     //14008214
    union DISP_OVL0_REG_OVL_RDMA2_BUF_LOW_TH OVL_RDMA2_BUF_LOW;     //14008218
    union DISP_OVL0_REG_OVL_RDMA3_BUF_LOW_TH OVL_RDMA3_BUF_LOW;     //1400821C
    union DISP_OVL0_REG_OVL_RDMA0_BUF_HIGH_TH OVL_RDMA0_BUF_HIGH;     //14008220
    union DISP_OVL0_REG_OVL_RDMA1_BUF_HIGH_TH OVL_RDMA1_BUF_HIGH;     //14008224
    union DISP_OVL0_REG_OVL_RDMA2_BUF_HIGH_TH OVL_RDMA2_BUF_HIGH;     //14008228
    union DISP_OVL0_REG_OVL_RDMA3_BUF_HIGH_TH OVL_RDMA3_BUF_HIGH;     //1400822C
    union DISP_OVL0_REG_OVL_SMI_DBG        OVL_SMI_DBG;         //14008230
    union DISP_OVL0_REG_OVL_GREQ_LAYER_CNT OVL_GREQ_LAYER_CNT; //14008234
    union DISP_OVL0_REG_OVL_GDRDY_PRD_NUM OVL_GDRDY_PRD_NUM;     //14008238
    union DISP_OVL0_REG_OVL_DEBUG_FLAG OVL_DEBUG_FLAG;         //1400823C
    union DISP_OVL0_REG_OVL_FLOW_CTRL_DBG OVL_FLOW_CTRL_DBG;   //14008240
    union DISP_OVL0_REG_OVL_ADDCON_DBG    OVL_ADDCON_DBG;      //14008244
    u32                          rsv_8248;              //14008248
    union DISP_OVL0_REG_OVL_RDMA0_DBG     OVL_RDMA0_DBG;         //1400824C
    union DISP_OVL0_REG_OVL_RDMA1_DBG     OVL_RDMA1_DBG;         //14008250
    union DISP_OVL0_REG_OVL_RDMA2_DBG     OVL_RDMA2_DBG;         //14008254
    union DISP_OVL0_REG_OVL_RDMA3_DBG     OVL_RDMA3_DBG;         //14008258
    union DISP_OVL0_REG_OVL_L0_CLR        OVL_L0_CLR;          //1400825C
    union DISP_OVL0_REG_OVL_L1_CLR        OVL_L1_CLR;             //14008260
    union DISP_OVL0_REG_OVL_L2_CLR        OVL_L2_CLR;             //14008264
    union DISP_OVL0_REG_OVL_L3_CLR        OVL_L3_CLR;             //14008268
    union DISP_OVL0_REG_OVL_LC_CLR        OVL_LC_CLR;             //1400826C
    union DISP_OVL0_REG_OVL_CRC           OVL_CRC;              //14008270
    u32                          rsv_8274[3];           //14008274..827C
    union DISP_OVL0_REG_OVL_LC_CON        OVL_LC_CON;          //14008280
    union DISP_OVL0_REG_OVL_LC_SRCKEY     OVL_LC_SRCKEY;       //14008284
    union DISP_OVL0_REG_OVL_LC_SRC_SIZE   OVL_LC_SRC_SIZE;     //14008288
    union DISP_OVL0_REG_OVL_LC_OFFSET     OVL_LC_OFFSET;       //1400828C
    union DISP_OVL0_REG_OVL_LC_SRC_SEL    OVL_LC_SRC_SEL;      //14008290
    u32                          rsv_8294[2];           //14008294..8298
    union DISP_OVL0_REG_OVL_BANK_CON        OVL_BANK_CON;         //1400829C
    union DISP_OVL0_REG_OVL_FUNC_DCM0     OVL_FUNC_DCM0;         //140082A0
    union DISP_OVL0_REG_OVL_FUNC_DCM1     OVL_FUNC_DCM1;         //140082A4
    u32                          rsv_82A8[2];           //140082A8..82AC
    union DISP_OVL0_REG_OVL_DVFS_L0_ROI    OVL_DVFS_L0_ROI;     //140082B0
    union DISP_OVL0_REG_OVL_DVFS_L1_ROI    OVL_DVFS_L1_ROI;     //140082B4
    union DISP_OVL0_REG_OVL_DVFS_L2_ROI    OVL_DVFS_L2_ROI;     //140082B8
    union DISP_OVL0_REG_OVL_DVFS_L3_ROI    OVL_DVFS_L3_ROI;     //140082BC
    union DISP_OVL0_REG_OVL_DVFS_EL0_ROI    OVL_DVFS_EL0_ROI;     //140082C0
    union DISP_OVL0_REG_OVL_DVFS_EL1_ROI    OVL_DVFS_EL1_ROI;     //140082C4
    union DISP_OVL0_REG_OVL_DVFS_EL2_ROI    OVL_DVFS_EL2_ROI;     //140082C8
    u32                          rsv_82CC[22];              //140082CC..8320
    union DISP_OVL0_REG_OVL_DATAPATH_EXT_CON OVL_DATAPATH_EXT_CON; //14008324
    u32                          rsv_8328[2];               //14008328..832C
    union DISP_OVL0_REG_OVL_EL0_CON        OVL_EL0_CON;              //14008330
    union DISP_OVL0_REG_OVL_EL0_SRCKEY    OVL_EL0_SRCKEY;      //14008334
    union DISP_OVL0_REG_OVL_EL0_SRC_SIZE    OVL_EL0_SRC_SIZE;     //14008338
    union DISP_OVL0_REG_OVL_EL0_OFFSET    OVL_EL0_OFFSET;      //1400833C
    union DISP_OVL0_REG_OVL_EL0_PITCH_MSB  OVL_EL0_PITCH_MSB;  //14008340
    union DISP_OVL0_REG_OVL_EL0_PITCH     OVL_EL0_PITCH;       //14008344
    union DISP_OVL0_REG_OVL_EL0_TILE      OVL_EL0_TILE;        //14008348
    union DISP_OVL0_REG_OVL_EL0_CLIP      OVL_EL0_CLIP;        //1400834C
    union DISP_OVL0_REG_OVL_EL1_CON       OVL_EL1_CON;         //14008350
    union DISP_OVL0_REG_OVL_EL1_SRCKEY    OVL_EL1_SRCKEY;      //14008354
    union DISP_OVL0_REG_OVL_EL1_SRC_SIZE  OVL_EL1_SRC_SIZE;    //14008358
    union DISP_OVL0_REG_OVL_EL1_OFFSET    OVL_EL1_OFFSET;      //1400835C
    union DISP_OVL0_REG_OVL_EL1_PITCH_MSB  OVL_EL1_PITCH_MSB;  //14008360
    union DISP_OVL0_REG_OVL_EL1_PITCH     OVL_EL1_PITCH;         //14008364
    union DISP_OVL0_REG_OVL_EL1_TILE       OVL_EL1_TILE;         //14008368
    union DISP_OVL0_REG_OVL_EL1_CLIP       OVL_EL1_CLIP;         //1400836C
    union DISP_OVL0_REG_OVL_EL2_CON       OVL_EL2_CON;          //14008370
    union DISP_OVL0_REG_OVL_EL2_SRCKEY    OVL_EL2_SRCKEY;         //14008374
    union DISP_OVL0_REG_OVL_EL2_SRC_SIZE  OVL_EL2_SRC_SIZE;     //14008378
    union DISP_OVL0_REG_OVL_EL2_OFFSET    OVL_EL2_OFFSET;         //1400837C
    union DISP_OVL0_REG_OVL_EL2_PITCH_MSB  OVL_EL2_PITCH_MSB;  //14008380
    union DISP_OVL0_REG_OVL_EL2_PITCH     OVL_EL2_PITCH;         //14008384
    union DISP_OVL0_REG_OVL_EL2_TILE        OVL_EL2_TILE;         //14008388
    union DISP_OVL0_REG_OVL_EL2_CLIP        OVL_EL2_CLIP;         //1400838C
    union DISP_OVL0_REG_OVL_EL0_CLR        OVL_EL0_CLR;         //14008390
    union DISP_OVL0_REG_OVL_EL1_CLR        OVL_EL1_CLR;         //14008394
    union DISP_OVL0_REG_OVL_EL2_CLR        OVL_EL2_CLR;         //14008398
    u32                          rsv_839C;            //1400839C
    union DISP_OVL0_REG_OVL_SBCH            OVL_SBCH;              //140083A0
    union DISP_OVL0_REG_OVL_SBCH_EXT        OVL_SBCH_EXT;         //140083A4
    union DISP_OVL0_REG_OVL_SBCH_CON        OVL_SBCH_CON;         //140083A8
    u32                         rsv_83AC[741];         //140083AC..8F3C
    union DISP_OVL0_REG_OVL_L0_ADDR        OVL_L0_ADDR;         //14008F40
    u32                         rsv_8F44[7];          //14008F44..8F5C
    union DISP_OVL0_REG_OVL_L1_ADDR       OVL_L1_ADDR;          //14008F60
    u32                         rsv_8F64[7];          //14008F64..8F7C
    union DISP_OVL0_REG_OVL_L2_ADDR       OVL_L2_ADDR;          //14008F80
    u32                         rsv_8F84[7];          //14008F84..8F9C
    union DISP_OVL0_REG_OVL_L3_ADDR       OVL_L3_ADDR;          //14008FA0
    u32                         rsv_8FA4[3];           //14008FA4..8FAC
    union DISP_OVL0_REG_OVL_EL0_ADDR       OVL_EL0_ADDR;         //14008FB0
    union DISP_OVL0_REG_OVL_EL1_ADDR       OVL_EL1_ADDR;         //14008FB4
    union DISP_OVL0_REG_OVL_EL2_ADDR       OVL_EL2_ADDR;         //14008FB8
    u32                         rsv_8FBC;             //14008FBC
    union DISP_OVL0_REG_OVL_SECURE        OVL_SECURE;             //14008FC0
};

// ---------- DISP_OVL0 Enum Definitions      ----------
// ---------- DISP_OVL0 C Macro Definitions   ----------
extern struct DISP_OVL0_REGS *g_DISP_OVL0_BASE;

#define DISP_OVL0_BASE                       (g_DISP_OVL0_BASE)

#define DISP_OVL0_OVL_STA                    (addr_t)(&DISP_OVL0_BASE->OVL_STA) //8000
#define DISP_OVL0_OVL_INTEN                  (addr_t)(&DISP_OVL0_BASE->OVL_INTEN) //8004
#define DISP_OVL0_OVL_INTSTA                 (addr_t)(&DISP_OVL0_BASE->OVL_INTSTA) //8008
#define DISP_OVL0_OVL_EN                     (addr_t)(&DISP_OVL0_BASE->OVL_EN) //800C
#define DISP_OVL0_OVL_TRIG                   (addr_t)(&DISP_OVL0_BASE->OVL_TRIG) //8010
#define DISP_OVL0_OVL_RST                    (addr_t)(&DISP_OVL0_BASE->OVL_RST) //8014
#define DISP_OVL0_OVL_ROI_SIZE               (addr_t)(&DISP_OVL0_BASE->OVL_ROI_SIZE) //8020
#define DISP_OVL0_OVL_DATAPATH_CON           (addr_t)(&DISP_OVL0_BASE->OVL_DATAPATH_CON) //8024
#define DISP_OVL0_OVL_ROI_BGCLR              (addr_t)(&DISP_OVL0_BASE->OVL_ROI_BGCLR) //8028
#define DISP_OVL0_OVL_SRC_CON                (addr_t)(&DISP_OVL0_BASE->OVL_SRC_CON) //802C
#define DISP_OVL0_OVL_L0_CON                 (addr_t)(&DISP_OVL0_BASE->OVL_L0_CON) //8030
#define DISP_OVL0_OVL_L0_SRCKEY              (addr_t)(&DISP_OVL0_BASE->OVL_L0_SRCKEY) //8034
#define DISP_OVL0_OVL_L0_SRC_SIZE            (addr_t)(&DISP_OVL0_BASE->OVL_L0_SRC_SIZE) //8038
#define DISP_OVL0_OVL_L0_OFFSET              (addr_t)(&DISP_OVL0_BASE->OVL_L0_OFFSET) //803C
#define DISP_OVL0_OVL_L0_PITCH               (addr_t)(&DISP_OVL0_BASE->OVL_L0_PITCH) //8044
#define DISP_OVL0_OVL_L0_TILE                (addr_t)(&DISP_OVL0_BASE->OVL_L0_TILE) //8048
#define DISP_OVL0_OVL_L0_CLIP                (addr_t)(&DISP_OVL0_BASE->OVL_L0_CLIP) //804C
#define DISP_OVL0_OVL_L1_CON                 (addr_t)(&DISP_OVL0_BASE->OVL_L1_CON) //8050
#define DISP_OVL0_OVL_L1_SRCKEY              (addr_t)(&DISP_OVL0_BASE->OVL_L1_SRCKEY) //8054
#define DISP_OVL0_OVL_L1_SRC_SIZE            (addr_t)(&DISP_OVL0_BASE->OVL_L1_SRC_SIZE) //8058
#define DISP_OVL0_OVL_L1_OFFSET              (addr_t)(&DISP_OVL0_BASE->OVL_L1_OFFSET) //805C
#define DISP_OVL0_OVL_L1_PITCH               (addr_t)(&DISP_OVL0_BASE->OVL_L1_PITCH) //8064
#define DISP_OVL0_OVL_L1_TILE                (addr_t)(&DISP_OVL0_BASE->OVL_L1_TILE) //8068
#define DISP_OVL0_OVL_L1_CLIP                (addr_t)(&DISP_OVL0_BASE->OVL_L1_CLIP) //806C
#define DISP_OVL0_OVL_L2_CON                 (addr_t)(&DISP_OVL0_BASE->OVL_L2_CON) //8070
#define DISP_OVL0_OVL_L2_SRCKEY              (addr_t)(&DISP_OVL0_BASE->OVL_L2_SRCKEY) //8074
#define DISP_OVL0_OVL_L2_SRC_SIZE            (addr_t)(&DISP_OVL0_BASE->OVL_L2_SRC_SIZE) //8078
#define DISP_OVL0_OVL_L2_OFFSET              (addr_t)(&DISP_OVL0_BASE->OVL_L2_OFFSET) //807C
#define DISP_OVL0_OVL_L2_PITCH               (addr_t)(&DISP_OVL0_BASE->OVL_L2_PITCH) //8084
#define DISP_OVL0_OVL_L2_TILE                (addr_t)(&DISP_OVL0_BASE->OVL_L2_TILE) //8088
#define DISP_OVL0_OVL_L2_CLIP                (addr_t)(&DISP_OVL0_BASE->OVL_L2_CLIP) //808C
#define DISP_OVL0_OVL_L3_CON                 (addr_t)(&DISP_OVL0_BASE->OVL_L3_CON) //8090
#define DISP_OVL0_OVL_L3_SRCKEY              (addr_t)(&DISP_OVL0_BASE->OVL_L3_SRCKEY) //8094
#define DISP_OVL0_OVL_L3_SRC_SIZE            (addr_t)(&DISP_OVL0_BASE->OVL_L3_SRC_SIZE) //8098
#define DISP_OVL0_OVL_L3_OFFSET              (addr_t)(&DISP_OVL0_BASE->OVL_L3_OFFSET) //809C
#define DISP_OVL0_OVL_L3_PITCH               (addr_t)(&DISP_OVL0_BASE->OVL_L3_PITCH) //80A4
#define DISP_OVL0_OVL_L3_TILE                (addr_t)(&DISP_OVL0_BASE->OVL_L3_TILE) //80A8
#define DISP_OVL0_OVL_L3_CLIP                (addr_t)(&DISP_OVL0_BASE->OVL_L3_CLIP) //80AC
#define DISP_OVL0_OVL_RDMA0_CTRL             (addr_t)(&DISP_OVL0_BASE->OVL_RDMA0_CTRL) //80C0
#define DISP_OVL0_OVL_RDMA0_MEM_GMC_SETTING1                                    \
                    (addr_t)(&DISP_OVL0_BASE->OVL_RDMA0_MEM_GMC_SETTING1) //80C8
//80CC
#define DISP_OVL0_OVL_RDMA0_MEM_SLOW_CON     (addr_t)(&DISP_OVL0_BASE->OVL_RDMA0_MEM_SLOW_CON)
#define DISP_OVL0_OVL_RDMA0_FIFO_CTRL        (addr_t)(&DISP_OVL0_BASE->OVL_RDMA0_FIFO_CTRL) //80D0
#define DISP_OVL0_OVL_RDMA1_CTRL             (addr_t)(&DISP_OVL0_BASE->OVL_RDMA1_CTRL) //80E0
#define DISP_OVL0_OVL_RDMA1_MEM_GMC_SETTING1                                    \
                    (addr_t)(&DISP_OVL0_BASE->OVL_RDMA1_MEM_GMC_SETTING1) //80E8
//80EC
#define DISP_OVL0_OVL_RDMA1_MEM_SLOW_CON     (addr_t)(&DISP_OVL0_BASE->OVL_RDMA1_MEM_SLOW_CON)
#define DISP_OVL0_OVL_RDMA1_FIFO_CTRL        (addr_t)(&DISP_OVL0_BASE->OVL_RDMA1_FIFO_CTRL) //80F0
#define DISP_OVL0_OVL_RDMA2_CTRL             (addr_t)(&DISP_OVL0_BASE->OVL_RDMA2_CTRL) //8100
#define DISP_OVL0_OVL_RDMA2_MEM_GMC_SETTING1                                    \
                    (addr_t)(&DISP_OVL0_BASE->OVL_RDMA2_MEM_GMC_SETTING1) //8108
//810C
#define DISP_OVL0_OVL_RDMA2_MEM_SLOW_CON     (addr_t)(&DISP_OVL0_BASE->OVL_RDMA2_MEM_SLOW_CON)
#define DISP_OVL0_OVL_RDMA2_FIFO_CTRL        (addr_t)(&DISP_OVL0_BASE->OVL_RDMA2_FIFO_CTRL) //8110
#define DISP_OVL0_OVL_RDMA3_CTRL             (addr_t)(&DISP_OVL0_BASE->OVL_RDMA3_CTRL) //8120
#define DISP_OVL0_OVL_RDMA3_MEM_GMC_SETTING1                                    \
                    (addr_t)(&DISP_OVL0_BASE->OVL_RDMA3_MEM_GMC_SETTING1) //8128
//812C
#define DISP_OVL0_OVL_RDMA3_MEM_SLOW_CON     (addr_t)(&DISP_OVL0_BASE->OVL_RDMA3_MEM_SLOW_CON)
#define DISP_OVL0_OVL_RDMA3_FIFO_CTRL        (addr_t)(&DISP_OVL0_BASE->OVL_RDMA3_FIFO_CTRL) //8130
#define DISP_OVL0_OVL_L0_Y2R_PARA_R0         (addr_t)(&DISP_OVL0_BASE->OVL_L0_Y2R_PARA_R0) //8134
#define DISP_OVL0_OVL_L0_Y2R_PARA_R1         (addr_t)(&DISP_OVL0_BASE->OVL_L0_Y2R_PARA_R1) //8138
#define DISP_OVL0_OVL_L0_Y2R_PARA_G0         (addr_t)(&DISP_OVL0_BASE->OVL_L0_Y2R_PARA_G0) //813C
#define DISP_OVL0_OVL_L0_Y2R_PARA_G1         (addr_t)(&DISP_OVL0_BASE->OVL_L0_Y2R_PARA_G1) //8140
#define DISP_OVL0_OVL_L0_Y2R_PARA_B0         (addr_t)(&DISP_OVL0_BASE->OVL_L0_Y2R_PARA_B0) //8144
#define DISP_OVL0_OVL_L0_Y2R_PARA_B1         (addr_t)(&DISP_OVL0_BASE->OVL_L0_Y2R_PARA_B1) //8148
//814C
#define DISP_OVL0_OVL_L0_Y2R_PARA_YUV_A_0    (addr_t)(&DISP_OVL0_BASE->OVL_L0_Y2R_PARA_YUV_A_0)
//8150
#define DISP_OVL0_OVL_L0_Y2R_PARA_YUV_A_1    (addr_t)(&DISP_OVL0_BASE->OVL_L0_Y2R_PARA_YUV_A_1)
//8154
#define DISP_OVL0_OVL_L0_Y2R_PARA_RGB_A_0    (addr_t)(&DISP_OVL0_BASE->OVL_L0_Y2R_PARA_RGB_A_0)
//8158
#define DISP_OVL0_OVL_L0_Y2R_PARA_RGB_A_1    (addr_t)(&DISP_OVL0_BASE->OVL_L0_Y2R_PARA_RGB_A_1)
#define DISP_OVL0_OVL_L1_Y2R_PARA_R0         (addr_t)(&DISP_OVL0_BASE->OVL_L1_Y2R_PARA_R0) //815C
#define DISP_OVL0_OVL_L1_Y2R_PARA_R1         (addr_t)(&DISP_OVL0_BASE->OVL_L1_Y2R_PARA_R1) //8160
#define DISP_OVL0_OVL_L1_Y2R_PARA_G0         (addr_t)(&DISP_OVL0_BASE->OVL_L1_Y2R_PARA_G0) //8164
#define DISP_OVL0_OVL_L1_Y2R_PARA_G1         (addr_t)(&DISP_OVL0_BASE->OVL_L1_Y2R_PARA_G1) //8168
#define DISP_OVL0_OVL_L1_Y2R_PARA_B0         (addr_t)(&DISP_OVL0_BASE->OVL_L1_Y2R_PARA_B0) //816C
#define DISP_OVL0_OVL_L1_Y2R_PARA_B1         (addr_t)(&DISP_OVL0_BASE->OVL_L1_Y2R_PARA_B1) //8170
//8174
#define DISP_OVL0_OVL_L1_Y2R_PARA_YUV_A_0    (addr_t)(&DISP_OVL0_BASE->OVL_L1_Y2R_PARA_YUV_A_0)
//8178
#define DISP_OVL0_OVL_L1_Y2R_PARA_YUV_A_1    (addr_t)(&DISP_OVL0_BASE->OVL_L1_Y2R_PARA_YUV_A_1)
//817C
#define DISP_OVL0_OVL_L1_Y2R_PARA_RGB_A_0    (addr_t)(&DISP_OVL0_BASE->OVL_L1_Y2R_PARA_RGB_A_0)
//8180
#define DISP_OVL0_OVL_L1_Y2R_PARA_RGB_A_1    (addr_t)(&DISP_OVL0_BASE->OVL_L1_Y2R_PARA_RGB_A_1)
#define DISP_OVL0_OVL_L2_Y2R_PARA_R0         (addr_t)(&DISP_OVL0_BASE->OVL_L2_Y2R_PARA_R0) //8184
#define DISP_OVL0_OVL_L2_Y2R_PARA_R1         (addr_t)(&DISP_OVL0_BASE->OVL_L2_Y2R_PARA_R1) //8188
#define DISP_OVL0_OVL_L2_Y2R_PARA_G0         (addr_t)(&DISP_OVL0_BASE->OVL_L2_Y2R_PARA_G0) //818C
#define DISP_OVL0_OVL_L2_Y2R_PARA_G1         (addr_t)(&DISP_OVL0_BASE->OVL_L2_Y2R_PARA_G1) //8190
#define DISP_OVL0_OVL_L2_Y2R_PARA_B0         (addr_t)(&DISP_OVL0_BASE->OVL_L2_Y2R_PARA_B0) //8194
#define DISP_OVL0_OVL_L2_Y2R_PARA_B1         (addr_t)(&DISP_OVL0_BASE->OVL_L2_Y2R_PARA_B1) //8198
//819C
#define DISP_OVL0_OVL_L2_Y2R_PARA_YUV_A_0    (addr_t)(&DISP_OVL0_BASE->OVL_L2_Y2R_PARA_YUV_A_0)
//81A0
#define DISP_OVL0_OVL_L2_Y2R_PARA_YUV_A_1    (addr_t)(&DISP_OVL0_BASE->OVL_L2_Y2R_PARA_YUV_A_1)
//81A4
#define DISP_OVL0_OVL_L2_Y2R_PARA_RGB_A_0    (addr_t)(&DISP_OVL0_BASE->OVL_L2_Y2R_PARA_RGB_A_0)
//81A8
#define DISP_OVL0_OVL_L2_Y2R_PARA_RGB_A_1    (addr_t)(&DISP_OVL0_BASE->OVL_L2_Y2R_PARA_RGB_A_1)
#define DISP_OVL0_OVL_L3_Y2R_PARA_R0         (addr_t)(&DISP_OVL0_BASE->OVL_L3_Y2R_PARA_R0) //81AC
#define DISP_OVL0_OVL_L3_Y2R_PARA_R1         (addr_t)(&DISP_OVL0_BASE->OVL_L3_Y2R_PARA_R1) //81B0
#define DISP_OVL0_OVL_L3_Y2R_PARA_G0         (addr_t)(&DISP_OVL0_BASE->OVL_L3_Y2R_PARA_G0) //81B4
#define DISP_OVL0_OVL_L3_Y2R_PARA_G1         (addr_t)(&DISP_OVL0_BASE->OVL_L3_Y2R_PARA_G1) //81B8
#define DISP_OVL0_OVL_L3_Y2R_PARA_B0         (addr_t)(&DISP_OVL0_BASE->OVL_L3_Y2R_PARA_B0) //81BC
#define DISP_OVL0_OVL_L3_Y2R_PARA_B1         (addr_t)(&DISP_OVL0_BASE->OVL_L3_Y2R_PARA_B1) //81C0
//81C4
#define DISP_OVL0_OVL_L3_Y2R_PARA_YUV_A_0    (addr_t)(&DISP_OVL0_BASE->OVL_L3_Y2R_PARA_YUV_A_0)
//81C8
#define DISP_OVL0_OVL_L3_Y2R_PARA_YUV_A_1    (addr_t)(&DISP_OVL0_BASE->OVL_L3_Y2R_PARA_YUV_A_1)
//81CC
#define DISP_OVL0_OVL_L3_Y2R_PARA_RGB_A_0    (addr_t)(&DISP_OVL0_BASE->OVL_L3_Y2R_PARA_RGB_A_0)
//81D0
#define DISP_OVL0_OVL_L3_Y2R_PARA_RGB_A_1    (addr_t)(&DISP_OVL0_BASE->OVL_L3_Y2R_PARA_RGB_A_1)
#define DISP_OVL0_OVL_DEBUG_MON_SEL          (addr_t)(&DISP_OVL0_BASE->OVL_DEBUG_MON_SEL) //81D4
#define DISP_OVL0_OVL_BLD_EXT                (addr_t)(&DISP_OVL0_BASE->OVL_BLD_EXT) //81DC
#define DISP_OVL0_OVL_RDMA0_MEM_GMC_SETTING2                                    \
                    (addr_t)(&DISP_OVL0_BASE->OVL_RDMA0_MEM_GMC_SETTING2) //81E0
#define DISP_OVL0_OVL_RDMA1_MEM_GMC_SETTING2                                    \
                    (addr_t)(&DISP_OVL0_BASE->OVL_RDMA1_MEM_GMC_SETTING2) //81E4
#define DISP_OVL0_OVL_RDMA2_MEM_GMC_SETTING2                                    \
                    (addr_t)(&DISP_OVL0_BASE->OVL_RDMA2_MEM_GMC_SETTING2) //81E8
#define DISP_OVL0_OVL_RDMA3_MEM_GMC_SETTING2                                    \
                    (addr_t)(&DISP_OVL0_BASE->OVL_RDMA3_MEM_GMC_SETTING2) //81EC
#define DISP_OVL0_OVL_RDMA_BURST_CON0        (addr_t)(&DISP_OVL0_BASE->OVL_RDMA_BURST_CON0) //81F0
#define DISP_OVL0_OVL_RDMA_BURST_CON1        (addr_t)(&DISP_OVL0_BASE->OVL_RDMA_BURST_CON1) //81F4
#define DISP_OVL0_OVL_RDMA_GREQ_NUM          (addr_t)(&DISP_OVL0_BASE->OVL_RDMA_GREQ_NUM) //81F8
#define DISP_OVL0_OVL_RDMA_GREQ_URG_NUM      (addr_t)(&DISP_OVL0_BASE->OVL_RDMA_GREQ_URG_NUM) //81FC
#define DISP_OVL0_OVL_DUMMY_REG              (addr_t)(&DISP_OVL0_BASE->OVL_DUMMY_REG) //8200
#define DISP_OVL0_OVL_GDRDY_PRD              (addr_t)(&DISP_OVL0_BASE->OVL_GDRDY_PRD) //8208
#define DISP_OVL0_OVL_RDMA_ULTRA_SRC         (addr_t)(&DISP_OVL0_BASE->OVL_RDMA_ULTRA_SRC) //820C
#define DISP_OVL0_OVL_RDMA0_BUF_LOW          (addr_t)(&DISP_OVL0_BASE->OVL_RDMA0_BUF_LOW_TH) //8210
#define DISP_OVL0_OVL_RDMA1_BUF_LOW          (addr_t)(&DISP_OVL0_BASE->OVL_RDMA1_BUF_LOW_TH) //8214
#define DISP_OVL0_OVL_RDMA2_BUF_LOW          (addr_t)(&DISP_OVL0_BASE->OVL_RDMA2_BUF_LOW_TH) //8218
#define DISP_OVL0_OVL_RDMA3_BUF_LOW          (addr_t)(&DISP_OVL0_BASE->OVL_RDMA3_BUF_LOW_TH) //821C
#define DISP_OVL0_OVL_RDMA0_BUF_HIGH_TH      (addr_t)(&DISP_OVL0_BASE->OVL_RDMA0_BUF_HIGH_TH) //B220
#define DISP_OVL0_OVL_RDMA1_BUF_HIGH_TH      (addr_t)(&DISP_OVL0_BASE->OVL_RDMA1_BUF_HIGH_TH) //B224
#define DISP_OVL0_OVL_RDMA2_BUF_HIGH_TH      (addr_t)(&DISP_OVL0_BASE->OVL_RDMA2_BUF_HIGH_TH) //B228
#define DISP_OVL0_OVL_RDMA3_BUF_HIGH_TH      (addr_t)(&DISP_OVL0_BASE->OVL_RDMA3_BUF_HIGH_TH) //B22C
#define DISP_OVL0_OVL_SMI_DBG                (addr_t)(&DISP_OVL0_BASE->OVL_SMI_DBG) //8230
#define DISP_OVL0_OVL_GREQ_LAYER_CNT         (addr_t)(&DISP_OVL0_BASE->OVL_GREQ_LAYER_CNT) //8234
#define DISP_OVL0_OVL_GDRDY_PRD_NUM          (addr_t)(&DISP_OVL0_BASE->OVL_GDRDY_PRD_NUM) //8238
#define DISP_OVL0_OVL_FLOW_CTRL_DBG          (addr_t)(&DISP_OVL0_BASE->OVL_FLOW_CTRL_DBG) //8240
#define DISP_OVL0_OVL_ADDCON_DBG             (addr_t)(&DISP_OVL0_BASE->OVL_ADDCON_DBG) //8244
#define DISP_OVL0_OVL_RDMA0_DBG              (addr_t)(&DISP_OVL0_BASE->OVL_RDMA0_DBG) //824C
#define DISP_OVL0_OVL_RDMA1_DBG              (addr_t)(&DISP_OVL0_BASE->OVL_RDMA1_DBG) //8250
#define DISP_OVL0_OVL_RDMA2_DBG              (addr_t)(&DISP_OVL0_BASE->OVL_RDMA2_DBG) //8254
#define DISP_OVL0_OVL_RDMA3_DBG              (addr_t)(&DISP_OVL0_BASE->OVL_RDMA3_DBG) //8258
#define DISP_OVL0_OVL_L0_CLR                 (addr_t)(&DISP_OVL0_BASE->OVL_L0_CLR) //825C
#define DISP_OVL0_OVL_L1_CLR                 (addr_t)(&DISP_OVL0_BASE->OVL_L1_CLR) //8260
#define DISP_OVL0_OVL_L2_CLR                 (addr_t)(&DISP_OVL0_BASE->OVL_L2_CLR) //8264
#define DISP_OVL0_OVL_L3_CLR                 (addr_t)(&DISP_OVL0_BASE->OVL_L3_CLR) //8268
#define DISP_OVL0_OVL_LC_CLR                 (addr_t)(&DISP_OVL0_BASE->OVL_LC_CLR) //826C
#define DISP_OVL0_OVL_CRC                    (addr_t)(&DISP_OVL0_BASE->OVL_CRC) //8270
#define DISP_OVL0_OVL_LC_CON                 (addr_t)(&DISP_OVL0_BASE->OVL_LC_CON) //8280
#define DISP_OVL0_OVL_LC_SRCKEY              (addr_t)(&DISP_OVL0_BASE->OVL_LC_SRCKEY) //8284
#define DISP_OVL0_OVL_LC_SRC_SIZE            (addr_t)(&DISP_OVL0_BASE->OVL_LC_SRC_SIZE) //8288
#define DISP_OVL0_OVL_LC_OFFSET              (addr_t)(&DISP_OVL0_BASE->OVL_LC_OFFSET) //828C
#define DISP_OVL0_OVL_LC_SRC_SEL             (addr_t)(&DISP_OVL0_BASE->OVL_LC_SRC_SEL) //8290
#define DISP_OVL0_OVL_BANK_CON               (addr_t)(&DISP_OVL0_BASE->OVL_BANK_CON) //829C
#define DISP_OVL0_OVL_FUNC_DCM0              (addr_t)(&DISP_OVL0_BASE->OVL_FUNC_DCM0) //82A0
#define DISP_OVL0_OVL_FUNC_DCM1              (addr_t)(&DISP_OVL0_BASE->OVL_FUNC_DCM1) //82A4
#define DISP_OVL0_OVL_DVFS_L0_ROI            (addr_t)(&DISP_OVL0_BASE->OVL_DVFS_L0_ROI) //82B0
#define DISP_OVL0_OVL_DVFS_L1_ROI            (addr_t)(&DISP_OVL0_BASE->OVL_DVFS_L1_ROI) //82B4
#define DISP_OVL0_OVL_DVFS_L2_ROI            (addr_t)(&DISP_OVL0_BASE->OVL_DVFS_L2_ROI) //82B8
#define DISP_OVL0_OVL_DVFS_L3_ROI            (addr_t)(&DISP_OVL0_BASE->OVL_DVFS_L3_ROI) //82BC
#define DISP_OVL0_OVL_DVFS_EL0_ROI           (addr_t)(&DISP_OVL0_BASE->OVL_DVFS_EL0_ROI) //82C0
#define DISP_OVL0_OVL_DVFS_EL1_ROI           (addr_t)(&DISP_OVL0_BASE->OVL_DVFS_EL1_ROI) //82C4
#define DISP_OVL0_OVL_DVFS_EL2_ROI           (addr_t)(&DISP_OVL0_BASE->OVL_DVFS_EL2_ROI) //82C8
#define DISP_OVL0_OVL_DATAPATH_EXT_CON       (addr_t)(&DISP_OVL0_BASE->OVL_DATAPATH_EXT_CON) //8324
#define DISP_OVL0_OVL_EL0_CON                (addr_t)(&DISP_OVL0_BASE->OVL_EL0_CON) //8330
#define DISP_OVL0_OVL_EL0_SRCKEY             (addr_t)(&DISP_OVL0_BASE->OVL_EL0_SRCKEY) //8334
#define DISP_OVL0_OVL_EL0_SRC_SIZE           (addr_t)(&DISP_OVL0_BASE->OVL_EL0_SRC_SIZE) //8338
#define DISP_OVL0_OVL_EL0_OFFSET             (addr_t)(&DISP_OVL0_BASE->OVL_EL0_OFFSET) //833C
#define DISP_OVL0_OVL_EL0_PITCH              (addr_t)(&DISP_OVL0_BASE->OVL_EL0_PITCH) //8344
#define DISP_OVL0_OVL_EL0_TILE               (addr_t)(&DISP_OVL0_BASE->OVL_EL0_TILE) //8348
#define DISP_OVL0_OVL_EL0_CLIP               (addr_t)(&DISP_OVL0_BASE->OVL_EL0_CLIP) //834C
#define DISP_OVL0_OVL_EL1_CON                (addr_t)(&DISP_OVL0_BASE->OVL_EL1_CON) //8350
#define DISP_OVL0_OVL_EL1_SRCKEY             (addr_t)(&DISP_OVL0_BASE->OVL_EL1_SRCKEY) //8354
#define DISP_OVL0_OVL_EL1_SRC_SIZE           (addr_t)(&DISP_OVL0_BASE->OVL_EL1_SRC_SIZE) //8358
#define DISP_OVL0_OVL_EL1_OFFSET             (addr_t)(&DISP_OVL0_BASE->OVL_EL1_OFFSET) //835C
#define DISP_OVL0_OVL_EL1_PITCH              (addr_t)(&DISP_OVL0_BASE->OVL_EL1_PITCH) //8364
#define DISP_OVL0_OVL_EL1_TILE               (addr_t)(&DISP_OVL0_BASE->OVL_EL1_TILE) //8368
#define DISP_OVL0_OVL_EL1_CLIP               (addr_t)(&DISP_OVL0_BASE->OVL_EL1_CLIP) //836C
#define DISP_OVL0_OVL_EL2_CON                (addr_t)(&DISP_OVL0_BASE->OVL_EL2_CON) //8370
#define DISP_OVL0_OVL_EL2_SRCKEY             (addr_t)(&DISP_OVL0_BASE->OVL_EL2_SRCKEY) //8374
#define DISP_OVL0_OVL_EL2_SRC_SIZE           (addr_t)(&DISP_OVL0_BASE->OVL_EL2_SRC_SIZE) //8378
#define DISP_OVL0_OVL_EL2_OFFSET             (addr_t)(&DISP_OVL0_BASE->OVL_EL2_OFFSET) //837C
#define DISP_OVL0_OVL_EL2_PITCH              (addr_t)(&DISP_OVL0_BASE->OVL_EL2_PITCH) //8384
#define DISP_OVL0_OVL_EL2_TILE               (addr_t)(&DISP_OVL0_BASE->OVL_EL2_TILE) //8388
#define DISP_OVL0_OVL_EL2_CLIP               (addr_t)(&DISP_OVL0_BASE->OVL_EL2_CLIP) //838C
#define DISP_OVL0_OVL_EL0_CLR                (addr_t)(&DISP_OVL0_BASE->OVL_EL0_CLR) //8390
#define DISP_OVL0_OVL_EL1_CLR                (addr_t)(&DISP_OVL0_BASE->OVL_EL1_CLR) //8394
#define DISP_OVL0_OVL_EL2_CLR                (addr_t)(&DISP_OVL0_BASE->OVL_EL2_CLR) //8398
#define DISP_OVL0_OVL_SBCH                   (addr_t)(&DISP_OVL0_BASE->OVL_SBCH) //83A0
#define DISP_OVL0_OVL_SBCH_EXT               (addr_t)(&DISP_OVL0_BASE->OVL_SBCH_EXT) //83A4
#define DISP_OVL0_OVL_SBCH_CON               (addr_t)(&DISP_OVL0_BASE->OVL_SBCH_CON) //83A8
#define DISP_OVL0_OVL_L0_ADDR                (addr_t)(&DISP_OVL0_BASE->OVL_L0_ADDR) //8F40
#define DISP_OVL0_OVL_L1_ADDR                (addr_t)(&DISP_OVL0_BASE->OVL_L1_ADDR) //8F60
#define DISP_OVL0_OVL_L2_ADDR                (addr_t)(&DISP_OVL0_BASE->OVL_L2_ADDR) //8F80
#define DISP_OVL0_OVL_L3_ADDR                (addr_t)(&DISP_OVL0_BASE->OVL_L3_ADDR) //8FA0
#define DISP_OVL0_OVL_EL0_ADDR               (addr_t)(&DISP_OVL0_BASE->OVL_EL0_ADDR) //8FB0
#define DISP_OVL0_OVL_EL1_ADDR               (addr_t)(&DISP_OVL0_BASE->OVL_EL1_ADDR) //8FB4
#define DISP_OVL0_OVL_EL2_ADDR               (addr_t)(&DISP_OVL0_BASE->OVL_EL2_ADDR) //8FB8
#define DISP_OVL0_OVL_SECURE                 (addr_t)(&DISP_OVL0_BASE->OVL_SECURE) //8FC0




#endif


#define OVL_STA_FLD_RDMA3_IDLE                                 REG_FLD(1, 4)
#define OVL_STA_FLD_RDMA2_IDLE                                 REG_FLD(1, 3)
#define OVL_STA_FLD_RDMA1_IDLE                                 REG_FLD(1, 2)
#define OVL_STA_FLD_RDMA0_IDLE                                 REG_FLD(1, 1)
#define OVL_STA_FLD_OVL_RUN                                    REG_FLD(1, 0)

#define OVL_INTEN_FLD_OVL_START_INTEN                          REG_FLD(1, 14)
#define OVL_INTEN_FLD_ABNORMAL_SOF_INTEN                       REG_FLD(1, 13)
#define OVL_INTEN_FLD_RDMA3_SMI_UNDERFLOW_INTEN                REG_FLD(1, 12)
#define OVL_INTEN_FLD_RDMA2_SMI_UNDERFLOW_INTEN                REG_FLD(1, 11)
#define OVL_INTEN_FLD_RDMA1_SMI_UNDERFLOW_INTEN                REG_FLD(1, 10)
#define OVL_INTEN_FLD_RDMA0_SMI_UNDERFLOW_INTEN                REG_FLD(1, 9)
#define OVL_INTEN_FLD_RDMA3_EOF_ABNORMAL_INTEN                 REG_FLD(1, 8)
#define OVL_INTEN_FLD_RDMA2_EOF_ABNORMAL_INTEN                 REG_FLD(1, 7)
#define OVL_INTEN_FLD_RDMA1_EOF_ABNORMAL_INTEN                 REG_FLD(1, 6)
#define OVL_INTEN_FLD_RDMA0_EOF_ABNORMAL_INTEN                 REG_FLD(1, 5)
#define OVL_INTEN_FLD_OVL_FME_HWRST_DONE_INTEN                 REG_FLD(1, 4)
#define OVL_INTEN_FLD_OVL_FME_SWRST_DONE_INTEN                 REG_FLD(1, 3)
#define OVL_INTEN_FLD_OVL_FME_UND_INTEN                        REG_FLD(1, 2)
#define OVL_INTEN_FLD_OVL_FME_CPL_INTEN                        REG_FLD(1, 1)
#define OVL_INTEN_FLD_OVL_REG_CMT_INTEN                        REG_FLD(1, 0)

#define OVL_INTSTA_FLD_OVL_START_INTSTA                        REG_FLD(1, 14)
#define OVL_INTSTA_FLD_ABNORMAL_SOF_INTSTA                     REG_FLD(1, 13)
#define OVL_INTSTA_FLD_RDMA3_SMI_UNDERFLOW_INTSTA              REG_FLD(1, 12)
#define OVL_INTSTA_FLD_RDMA2_SMI_UNDERFLOW_INTSTA              REG_FLD(1, 11)
#define OVL_INTSTA_FLD_RDMA1_SMI_UNDERFLOW_INTSTA              REG_FLD(1, 10)
#define OVL_INTSTA_FLD_RDMA0_SMI_UNDERFLOW_INTSTA              REG_FLD(1, 9)
#define OVL_INTSTA_FLD_RDMA3_EOF_ABNORMAL_INTSTA               REG_FLD(1, 8)
#define OVL_INTSTA_FLD_RDMA2_EOF_ABNORMAL_INTSTA               REG_FLD(1, 7)
#define OVL_INTSTA_FLD_RDMA1_EOF_ABNORMAL_INTSTA               REG_FLD(1, 6)
#define OVL_INTSTA_FLD_RDMA0_EOF_ABNORMAL_INTSTA               REG_FLD(1, 5)
#define OVL_INTSTA_FLD_OVL_FME_HWRST_DONE_INTSTA               REG_FLD(1, 4)
#define OVL_INTSTA_FLD_OVL_FME_SWRST_DONE_INTSTA               REG_FLD(1, 3)
#define OVL_INTSTA_FLD_OVL_FME_UND_INTSTA                      REG_FLD(1, 2)
#define OVL_INTSTA_FLD_OVL_FME_CPL_INTSTA                      REG_FLD(1, 1)
#define OVL_INTSTA_FLD_OVL_REG_CMT_INTSTA                      REG_FLD(1, 0)

#define OVL_EN_FLD_BLOCK_EXT_PREULTRA                          REG_FLD(1, 19)
#define OVL_EN_FLD_BLOCK_EXT_ULTRA                             REG_FLD(1, 18)
#define OVL_EN_FLD_IGNORE_ABNORMAL_SOF                         REG_FLD(1, 16)
#define OVL_EN_FLD_HF_FOVL_CK_ON                               REG_FLD(1, 10)
#define OVL_EN_FLD_HG_FSMI_CK_ON                               REG_FLD(1, 9)
#define OVL_EN_FLD_HG_FOVL_CK_ON                               REG_FLD(1, 8)
#define OVL_EN_FLD_OVL_EN                                      REG_FLD(1, 0)

#define OVL_TRIG_FLD_CRC_CLR                                   REG_FLD(1, 9)
#define OVL_TRIG_FLD_CRC_EN                                    REG_FLD(1, 8)
#define OVL_TRIG_FLD_OVL_SW_TRIG                               REG_FLD(1, 0)

#define OVL_RST_FLD_OVL_SMI_IOBUF_HARD_RST                     REG_FLD(1, 31)
#define OVL_RST_FLD_OVL_SMI_IOBUF_RST                          REG_FLD(1, 30)
#define OVL_RST_FLD_OVL_SMI_HARD_RST                           REG_FLD(1, 29)
#define OVL_RST_FLD_OVL_SMI_RST                                REG_FLD(1, 28)
#define OVL_RST_FLD_OVL_RST                                    REG_FLD(1, 0)

#define OVL_ROI_SIZE_FLD_ROI_H                                 REG_FLD(13, 16)
#define OVL_ROI_SIZE_FLD_ROI_W                                 REG_FLD(13, 0)

#define OVL_DATAPATH_CON_FLD_WIDE_GAMUT_EN                     REG_FLD(1, 31)
#define OVL_DATAPATH_CON_FLD_OUTPUT_INTERLACE                  REG_FLD(1, 27)
#define OVL_DATAPATH_CON_FLD_OUTPUT_CLAMP                      REG_FLD(1, 26)
#define OVL_DATAPATH_CON_FLD_GCLAST_EN                         REG_FLD(1, 24)
#define OVL_DATAPATH_CON_FLD_RDMA3_OUT_SEL                     REG_FLD(1, 23)
#define OVL_DATAPATH_CON_FLD_RDMA2_OUT_SEL                     REG_FLD(1, 22)
#define OVL_DATAPATH_CON_FLD_RDMA1_OUT_SEL                     REG_FLD(1, 21)
#define OVL_DATAPATH_CON_FLD_RDMA0_OUT_SEL                     REG_FLD(1, 20)
#define OVL_DATAPATH_CON_FLD_PQ_OUT_SEL                        REG_FLD(2, 16)
#define OVL_DATAPATH_CON_FLD_OVL_GAMMA_OUT                     REG_FLD(1, 15)
#define OVL_DATAPATH_CON_FLD_ADOBE_LAYER                       REG_FLD(2, 13)
#define OVL_DATAPATH_CON_FLD_ADOBE_MODE                        REG_FLD(1, 12)
#define OVL_DATAPATH_CON_FLD_L3_GPU_MODE                       REG_FLD(1, 11)
#define OVL_DATAPATH_CON_FLD_L2_GPU_MODE                       REG_FLD(1, 10)
#define OVL_DATAPATH_CON_FLD_L1_GPU_MODE                       REG_FLD(1, 9)
#define OVL_DATAPATH_CON_FLD_L0_GPU_MODE                       REG_FLD(1, 8)
#define OVL_DATAPATH_CON_FLD_OUTPUT_NO_RND                     REG_FLD(1, 3)
#define OVL_DATAPATH_CON_FLD_BGCLR_IN_SEL                      REG_FLD(1, 2)
#define OVL_DATAPATH_CON_FLD_OVL_RANDOM_BGCLR_EN               REG_FLD(1, 1)
#define OVL_DATAPATH_CON_FLD_LAYER_SMI_ID_EN                   REG_FLD(1, 0)

#define OVL_ROI_BGCLR_FLD_ALPHA                                REG_FLD(8, 24)
#define OVL_ROI_BGCLR_FLD_RED                                  REG_FLD(8, 16)
#define OVL_ROI_BGCLR_FLD_GREEN                                REG_FLD(8, 8)
#define OVL_ROI_BGCLR_FLD_BLUE                                 REG_FLD(8, 0)

#define OVL_SRC_CON_FLD_RELAY_MODE_EN                          REG_FLD(1, 9)
#define OVL_SRC_CON_FLD_FORCE_RELAY_MODE                       REG_FLD(1, 8)
#define OVL_SRC_CON_FLD_LC_EN                                  REG_FLD(1, 4)
#define OVL_SRC_CON_FLD_L3_EN                                  REG_FLD(1, 3)
#define OVL_SRC_CON_FLD_L2_EN                                  REG_FLD(1, 2)
#define OVL_SRC_CON_FLD_L1_EN                                  REG_FLD(1, 1)
#define OVL_SRC_CON_FLD_L0_EN                                  REG_FLD(1, 0)

#define OVL_L0_CON_FLD_DSTKEY_EN                               REG_FLD(1, 31)
#define OVL_L0_CON_FLD_SRCKEY_EN                               REG_FLD(1, 30)
#define OVL_L0_CON_FLD_LAYER_SRC                               REG_FLD(2, 28)
#define OVL_L0_CON_FLD_MTX_EN                                  REG_FLD(1, 27)
#define OVL_L0_CON_FLD_MTX_AUTO_DIS                            REG_FLD(1, 26)
#define OVL_L0_CON_FLD_RGB_SWAP                                REG_FLD(1, 25)
#define OVL_L0_CON_FLD_BYTE_SWAP                               REG_FLD(1, 24)
#define OVL_L0_CON_FLD_CLRFMT_MAN                              REG_FLD(1, 23)
#define OVL_L0_CON_FLD_R_FIRST                                 REG_FLD(1, 22)
#define OVL_L0_CON_FLD_LANDSCAPE                               REG_FLD(1, 21)
#define OVL_L0_CON_FLD_EN_3D                                   REG_FLD(1, 20)
#define OVL_L0_CON_FLD_INT_MTX_SEL                             REG_FLD(4, 16)
#define OVL_L0_CON_FLD_CLRFMT                                  REG_FLD(4, 12)
#define OVL_L0_CON_FLD_EXT_MTX_EN                              REG_FLD(1, 11)
#define OVL_L0_CON_FLD_HORIZONTAL_FLIP_EN                      REG_FLD(1, 10)
#define OVL_L0_CON_FLD_VERTICAL_FLIP_EN                        REG_FLD(1, 9)
#define OVL_L0_CON_FLD_ALPHA_EN                                REG_FLD(1, 8)
#define OVL_L0_CON_FLD_ALPHA                                   REG_FLD(8, 0)

#define OVL_L0_SRCKEY_FLD_SRCKEY                               REG_FLD(32, 0)

#define OVL_L0_SRC_SIZE_FLD_L0_SRC_H                           REG_FLD(13, 16)
#define OVL_L0_SRC_SIZE_FLD_L0_SRC_W                           REG_FLD(13, 0)

#define OVL_L0_OFFSET_FLD_L0_YOFF                              REG_FLD(13, 16)
#define OVL_L0_OFFSET_FLD_L0_XOFF                              REG_FLD(13, 0)

#define OVL_L0_PITCH_FLD_SURFL_EN                              REG_FLD(1, 31)
#define OVL_L0_PITCH_FLD_L0_BLEND_RND_SHT                      REG_FLD(1, 30)
#define OVL_L0_PITCH_FLD_L0_SRGB_SEL_EXT2                      REG_FLD(1, 29)
#define OVL_L0_PITCH_FLD_L0_CONST_BLD                          REG_FLD(1, 28)
#define OVL_L0_PITCH_FLD_L0_DRGB_SEL_EXT                       REG_FLD(1, 27)
#define OVL_L0_PITCH_FLD_L0_DA_SEL_EXT                         REG_FLD(1, 26)
#define OVL_L0_PITCH_FLD_L0_SRGB_SEL_EXT                       REG_FLD(1, 25)
#define OVL_L0_PITCH_FLD_L0_SA_SEL_EXT                         REG_FLD(1, 24)
#define OVL_L0_PITCH_FLD_L0_DRGB_SEL                           REG_FLD(2, 22)
#define OVL_L0_PITCH_FLD_L0_DA_SEL                             REG_FLD(2, 20)
#define OVL_L0_PITCH_FLD_L0_SRGB_SEL                           REG_FLD(2, 18)
#define OVL_L0_PITCH_FLD_L0_SA_SEL                             REG_FLD(2, 16)
#define OVL_L0_PITCH_FLD_L0_SRC_PITCH                          REG_FLD(16, 0)

#define OVL_L0_TILE_FLD_TILE_HORI_BLOCK_NUM                    REG_FLD(8, 24)
#define OVL_L0_TILE_FLD_TILE_EN                                REG_FLD(1, 21)
#define OVL_L0_TILE_FLD_TILE_WIDTH_SEL                         REG_FLD(1, 20)
#define OVL_L0_TILE_FLD_TILE_HEIGHT                            REG_FLD(20, 0)

#define OVL_L0_CLIP_FLD_L0_SRC_BOTTOM_CLIP                     REG_FLD(8, 24)
#define OVL_L0_CLIP_FLD_L0_SRC_TOP_CLIP                        REG_FLD(8, 16)
#define OVL_L0_CLIP_FLD_L0_SRC_RIGHT_CLIP                      REG_FLD(8, 8)
#define OVL_L0_CLIP_FLD_L0_SRC_LEFT_CLIP                       REG_FLD(8, 0)

#define OVL_L1_CON_FLD_DSTKEY_EN                               REG_FLD(1, 31)
#define OVL_L1_CON_FLD_SRCKEY_EN                               REG_FLD(1, 30)
#define OVL_L1_CON_FLD_LAYER_SRC                               REG_FLD(2, 28)
#define OVL_L1_CON_FLD_MTX_EN                                  REG_FLD(1, 27)
#define OVL_L1_CON_FLD_MTX_AUTO_DIS                            REG_FLD(1, 26)
#define OVL_L1_CON_FLD_RGB_SWAP                                REG_FLD(1, 25)
#define OVL_L1_CON_FLD_BYTE_SWAP                               REG_FLD(1, 24)
#define OVL_L1_CON_FLD_CLRFMT_MAN                              REG_FLD(1, 23)
#define OVL_L1_CON_FLD_R_FIRST                                 REG_FLD(1, 22)
#define OVL_L1_CON_FLD_LANDSCAPE                               REG_FLD(1, 21)
#define OVL_L1_CON_FLD_EN_3D                                   REG_FLD(1, 20)
#define OVL_L1_CON_FLD_INT_MTX_SEL                             REG_FLD(4, 16)
#define OVL_L1_CON_FLD_CLRFMT                                  REG_FLD(4, 12)
#define OVL_L1_CON_FLD_EXT_MTX_EN                              REG_FLD(1, 11)
#define OVL_L1_CON_FLD_HORIZONTAL_FLIP_EN                      REG_FLD(1, 10)
#define OVL_L1_CON_FLD_VERTICAL_FLIP_EN                        REG_FLD(1, 9)
#define OVL_L1_CON_FLD_ALPHA_EN                                REG_FLD(1, 8)
#define OVL_L1_CON_FLD_ALPHA                                   REG_FLD(8, 0)

#define OVL_L1_SRCKEY_FLD_SRCKEY                               REG_FLD(32, 0)

#define OVL_L1_SRC_SIZE_FLD_L1_SRC_H                           REG_FLD(13, 16)
#define OVL_L1_SRC_SIZE_FLD_L1_SRC_W                           REG_FLD(13, 0)

#define OVL_L1_OFFSET_FLD_L1_YOFF                              REG_FLD(13, 16)
#define OVL_L1_OFFSET_FLD_L1_XOFF                              REG_FLD(13, 0)

#define OVL_L1_PITCH_FLD_SURFL_EN                              REG_FLD(1, 31)
#define OVL_L1_PITCH_FLD_L1_BLEND_RND_SHT                      REG_FLD(1, 30)
#define OVL_L1_PITCH_FLD_L1_SRGB_SEL_EXT2                      REG_FLD(1, 29)
#define OVL_L1_PITCH_FLD_L1_CONST_BLD                          REG_FLD(1, 28)
#define OVL_L1_PITCH_FLD_L1_DRGB_SEL_EXT                       REG_FLD(1, 27)
#define OVL_L1_PITCH_FLD_L1_DA_SEL_EXT                         REG_FLD(1, 26)
#define OVL_L1_PITCH_FLD_L1_SRGB_SEL_EXT                       REG_FLD(1, 25)
#define OVL_L1_PITCH_FLD_L1_SA_SEL_EXT                         REG_FLD(1, 24)
#define OVL_L1_PITCH_FLD_L1_DRGB_SEL                           REG_FLD(2, 22)
#define OVL_L1_PITCH_FLD_L1_DA_SEL                             REG_FLD(2, 20)
#define OVL_L1_PITCH_FLD_L1_SRGB_SEL                           REG_FLD(2, 18)
#define OVL_L1_PITCH_FLD_L1_SA_SEL                             REG_FLD(2, 16)
#define OVL_L1_PITCH_FLD_L1_SRC_PITCH                          REG_FLD(16, 0)

#define OVL_L1_TILE_FLD_TILE_HORI_BLOCK_NUM                    REG_FLD(8, 24)
#define OVL_L1_TILE_FLD_TILE_EN                                REG_FLD(1, 21)
#define OVL_L1_TILE_FLD_TILE_WIDTH_SEL                         REG_FLD(1, 20)
#define OVL_L1_TILE_FLD_TILE_HEIGHT                            REG_FLD(20, 0)

#define OVL_L1_CLIP_FLD_L1_SRC_BOTTOM_CLIP                     REG_FLD(8, 24)
#define OVL_L1_CLIP_FLD_L1_SRC_TOP_CLIP                        REG_FLD(8, 16)
#define OVL_L1_CLIP_FLD_L1_SRC_RIGHT_CLIP                      REG_FLD(8, 8)
#define OVL_L1_CLIP_FLD_L1_SRC_LEFT_CLIP                       REG_FLD(8, 0)

#define OVL_L2_CON_FLD_DSTKEY_EN                               REG_FLD(1, 31)
#define OVL_L2_CON_FLD_SRCKEY_EN                               REG_FLD(1, 30)
#define OVL_L2_CON_FLD_LAYER_SRC                               REG_FLD(2, 28)
#define OVL_L2_CON_FLD_MTX_EN                                  REG_FLD(1, 27)
#define OVL_L2_CON_FLD_MTX_AUTO_DIS                            REG_FLD(1, 26)
#define OVL_L2_CON_FLD_RGB_SWAP                                REG_FLD(1, 25)
#define OVL_L2_CON_FLD_BYTE_SWAP                               REG_FLD(1, 24)
#define OVL_L2_CON_FLD_CLRFMT_MAN                              REG_FLD(1, 23)
#define OVL_L2_CON_FLD_R_FIRST                                 REG_FLD(1, 22)
#define OVL_L2_CON_FLD_LANDSCAPE                               REG_FLD(1, 21)
#define OVL_L2_CON_FLD_EN_3D                                   REG_FLD(1, 20)
#define OVL_L2_CON_FLD_INT_MTX_SEL                             REG_FLD(4, 16)
#define OVL_L2_CON_FLD_CLRFMT                                  REG_FLD(4, 12)
#define OVL_L2_CON_FLD_EXT_MTX_EN                              REG_FLD(1, 11)
#define OVL_L2_CON_FLD_HORIZONTAL_FLIP_EN                      REG_FLD(1, 10)
#define OVL_L2_CON_FLD_VERTICAL_FLIP_EN                        REG_FLD(1, 9)
#define OVL_L2_CON_FLD_ALPHA_EN                                REG_FLD(1, 8)
#define OVL_L2_CON_FLD_ALPHA                                   REG_FLD(8, 0)

#define OVL_L2_SRCKEY_FLD_SRCKEY                               REG_FLD(32, 0)

#define OVL_L2_SRC_SIZE_FLD_L2_SRC_H                           REG_FLD(13, 16)
#define OVL_L2_SRC_SIZE_FLD_L2_SRC_W                           REG_FLD(13, 0)

#define OVL_L2_OFFSET_FLD_L2_YOFF                              REG_FLD(13, 16)
#define OVL_L2_OFFSET_FLD_L2_XOFF                              REG_FLD(13, 0)

#define OVL_L2_PITCH_FLD_SURFL_EN                              REG_FLD(1, 31)
#define OVL_L2_PITCH_FLD_L2_BLEND_RND_SHT                      REG_FLD(1, 30)
#define OVL_L2_PITCH_FLD_L2_SRGB_SEL_EXT2                      REG_FLD(1, 29)
#define OVL_L2_PITCH_FLD_L2_CONST_BLD                          REG_FLD(1, 28)
#define OVL_L2_PITCH_FLD_L2_DRGB_SEL_EXT                       REG_FLD(1, 27)
#define OVL_L2_PITCH_FLD_L2_DA_SEL_EXT                         REG_FLD(1, 26)
#define OVL_L2_PITCH_FLD_L2_SRGB_SEL_EXT                       REG_FLD(1, 25)
#define OVL_L2_PITCH_FLD_L2_SA_SEL_EXT                         REG_FLD(1, 24)
#define OVL_L2_PITCH_FLD_L2_DRGB_SEL                           REG_FLD(2, 22)
#define OVL_L2_PITCH_FLD_L2_DA_SEL                             REG_FLD(2, 20)
#define OVL_L2_PITCH_FLD_L2_SRGB_SEL                           REG_FLD(2, 18)
#define OVL_L2_PITCH_FLD_L2_SA_SEL                             REG_FLD(2, 16)
#define OVL_L2_PITCH_FLD_L2_SRC_PITCH                          REG_FLD(16, 0)

#define OVL_L2_TILE_FLD_TILE_HORI_BLOCK_NUM                    REG_FLD(8, 24)
#define OVL_L2_TILE_FLD_TILE_EN                                REG_FLD(1, 21)
#define OVL_L2_TILE_FLD_TILE_WIDTH_SEL                         REG_FLD(1, 20)
#define OVL_L2_TILE_FLD_TILE_HEIGHT                            REG_FLD(20, 0)

#define OVL_L2_CLIP_FLD_L2_SRC_BOTTOM_CLIP                     REG_FLD(8, 24)
#define OVL_L2_CLIP_FLD_L2_SRC_TOP_CLIP                        REG_FLD(8, 16)
#define OVL_L2_CLIP_FLD_L2_SRC_RIGHT_CLIP                      REG_FLD(8, 8)
#define OVL_L2_CLIP_FLD_L2_SRC_LEFT_CLIP                       REG_FLD(8, 0)

#define OVL_L3_CON_FLD_DSTKEY_EN                               REG_FLD(1, 31)
#define OVL_L3_CON_FLD_SRCKEY_EN                               REG_FLD(1, 30)
#define OVL_L3_CON_FLD_LAYER_SRC                               REG_FLD(2, 28)
#define OVL_L3_CON_FLD_MTX_EN                                  REG_FLD(1, 27)
#define OVL_L3_CON_FLD_MTX_AUTO_DIS                            REG_FLD(1, 26)
#define OVL_L3_CON_FLD_RGB_SWAP                                REG_FLD(1, 25)
#define OVL_L3_CON_FLD_BYTE_SWAP                               REG_FLD(1, 24)
#define OVL_L3_CON_FLD_CLRFMT_MAN                              REG_FLD(1, 23)
#define OVL_L3_CON_FLD_R_FIRST                                 REG_FLD(1, 22)
#define OVL_L3_CON_FLD_LANDSCAPE                               REG_FLD(1, 21)
#define OVL_L3_CON_FLD_EN_3D                                   REG_FLD(1, 20)
#define OVL_L3_CON_FLD_INT_MTX_SEL                             REG_FLD(4, 16)
#define OVL_L3_CON_FLD_CLRFMT                                  REG_FLD(4, 12)
#define OVL_L3_CON_FLD_EXT_MTX_EN                              REG_FLD(1, 11)
#define OVL_L3_CON_FLD_HORIZONTAL_FLIP_EN                      REG_FLD(1, 10)
#define OVL_L3_CON_FLD_VERTICAL_FLIP_EN                        REG_FLD(1, 9)
#define OVL_L3_CON_FLD_ALPHA_EN                                REG_FLD(1, 8)
#define OVL_L3_CON_FLD_ALPHA                                   REG_FLD(8, 0)

#define OVL_L3_SRCKEY_FLD_SRCKEY                               REG_FLD(32, 0)

#define OVL_L3_SRC_SIZE_FLD_L3_SRC_H                           REG_FLD(13, 16)
#define OVL_L3_SRC_SIZE_FLD_L3_SRC_W                           REG_FLD(13, 0)

#define OVL_L3_OFFSET_FLD_L3_YOFF                              REG_FLD(13, 16)
#define OVL_L3_OFFSET_FLD_L3_XOFF                              REG_FLD(13, 0)

#define OVL_L3_PITCH_FLD_SURFL_EN                              REG_FLD(1, 31)
#define OVL_L3_PITCH_FLD_L3_BLEND_RND_SHT                      REG_FLD(1, 30)
#define OVL_L3_PITCH_FLD_L3_SRGB_SEL_EXT2                      REG_FLD(1, 29)
#define OVL_L3_PITCH_FLD_L3_CONST_BLD                          REG_FLD(1, 28)
#define OVL_L3_PITCH_FLD_L3_DRGB_SEL_EXT                       REG_FLD(1, 27)
#define OVL_L3_PITCH_FLD_L3_DA_SEL_EXT                         REG_FLD(1, 26)
#define OVL_L3_PITCH_FLD_L3_SRGB_SEL_EXT                       REG_FLD(1, 25)
#define OVL_L3_PITCH_FLD_L3_SA_SEL_EXT                         REG_FLD(1, 24)
#define OVL_L3_PITCH_FLD_L3_DRGB_SEL                           REG_FLD(2, 22)
#define OVL_L3_PITCH_FLD_L3_DA_SEL                             REG_FLD(2, 20)
#define OVL_L3_PITCH_FLD_L3_SRGB_SEL                           REG_FLD(2, 18)
#define OVL_L3_PITCH_FLD_L3_SA_SEL                             REG_FLD(2, 16)
#define OVL_L3_PITCH_FLD_L3_SRC_PITCH                          REG_FLD(16, 0)

#define OVL_L3_TILE_FLD_TILE_HORI_BLOCK_NUM                    REG_FLD(8, 24)
#define OVL_L3_TILE_FLD_TILE_EN                                REG_FLD(1, 21)
#define OVL_L3_TILE_FLD_TILE_WIDTH_SEL                         REG_FLD(1, 20)
#define OVL_L3_TILE_FLD_TILE_HEIGHT                            REG_FLD(20, 0)

#define OVL_L3_CLIP_FLD_L3_SRC_BOTTOM_CLIP                     REG_FLD(8, 24)
#define OVL_L3_CLIP_FLD_L3_SRC_TOP_CLIP                        REG_FLD(8, 16)
#define OVL_L3_CLIP_FLD_L3_SRC_RIGHT_CLIP                      REG_FLD(8, 8)
#define OVL_L3_CLIP_FLD_L3_SRC_LEFT_CLIP                       REG_FLD(8, 0)

#define OVL_RDMA0_CTRL_FLD_RDMA0_FIFO_USED_SIZE                REG_FLD(12, 16)
#define OVL_RDMA0_CTRL_FLD_RDMA0_INTERLACE                     REG_FLD(1, 4)
#define OVL_RDMA0_CTRL_FLD_RDMA0_EN                            REG_FLD(1, 0)

#define OVL_RDMA0_MEM_GMC_SETTING1_FLD_RDMA0_PRE_ULTRA_THRESHOLD_HIGH_OFS REG_FLD(1, 31)
#define OVL_RDMA0_MEM_GMC_SETTING1_FLD_RDMA0_ULTRA_THRESHOLD_HIGH_OFS REG_FLD(1, 28)
#define OVL_RDMA0_MEM_GMC_SETTING1_FLD_RDMA0_OSTD_PREULTRA_TH  REG_FLD(10, 16)
#define OVL_RDMA0_MEM_GMC_SETTING1_FLD_RDMA0_OSTD_ULTRA_TH     REG_FLD(10, 0)

#define OVL_RDMA0_MEM_SLOW_CON_FLD_RDMA0_SLOW_CNT              REG_FLD(16, 16)
#define OVL_RDMA0_MEM_SLOW_CON_FLD_RDMA0_SLOW_EN               REG_FLD(1, 0)

#define OVL_RDMA0_FIFO_CTRL_FLD_RDMA0_FIFO_UND_EN              REG_FLD(1, 31)
#define OVL_RDMA0_FIFO_CTRL_FLD_RDMA0_FIFO_SIZE                REG_FLD(12, 16)
#define OVL_RDMA0_FIFO_CTRL_FLD_RDMA0_FIFO_THRD                REG_FLD(10, 0)

#define OVL_RDMA1_CTRL_FLD_RDMA1_FIFO_USED_SIZE                REG_FLD(12, 16)
#define OVL_RDMA1_CTRL_FLD_RDMA1_INTERLACE                     REG_FLD(1, 4)
#define OVL_RDMA1_CTRL_FLD_RDMA1_EN                            REG_FLD(1, 0)

#define OVL_RDMA1_MEM_GMC_SETTING1_FLD_RDMA1_PRE_ULTRA_THRESHOLD_HIGH_OFS REG_FLD(1, 31)
#define OVL_RDMA1_MEM_GMC_SETTING1_FLD_RDMA1_ULTRA_THRESHOLD_HIGH_OFS REG_FLD(1, 28)
#define OVL_RDMA1_MEM_GMC_SETTING1_FLD_RDMA1_OSTD_PREULTRA_TH  REG_FLD(10, 16)
#define OVL_RDMA1_MEM_GMC_SETTING1_FLD_RDMA1_OSTD_ULTRA_TH     REG_FLD(10, 0)

#define OVL_RDMA1_MEM_SLOW_CON_FLD_RDMA1_SLOW_CNT              REG_FLD(16, 16)
#define OVL_RDMA1_MEM_SLOW_CON_FLD_RDMA1_SLOW_EN               REG_FLD(1, 0)

#define OVL_RDMA1_FIFO_CTRL_FLD_RDMA1_FIFO_UND_EN              REG_FLD(1, 31)
#define OVL_RDMA1_FIFO_CTRL_FLD_RDMA1_FIFO_SIZE                REG_FLD(12, 16)
#define OVL_RDMA1_FIFO_CTRL_FLD_RDMA1_FIFO_THRD                REG_FLD(10, 0)

#define OVL_RDMA2_CTRL_FLD_RDMA2_FIFO_USED_SIZE                REG_FLD(12, 16)
#define OVL_RDMA2_CTRL_FLD_RDMA2_INTERLACE                     REG_FLD(1, 4)
#define OVL_RDMA2_CTRL_FLD_RDMA2_EN                            REG_FLD(1, 0)

#define OVL_RDMA2_MEM_GMC_SETTING1_FLD_RDMA2_PRE_ULTRA_THRESHOLD_HIGH_OFS REG_FLD(1, 31)
#define OVL_RDMA2_MEM_GMC_SETTING1_FLD_RDMA2_ULTRA_THRESHOLD_HIGH_OFS REG_FLD(1, 28)
#define OVL_RDMA2_MEM_GMC_SETTING1_FLD_RDMA2_OSTD_PREULTRA_TH  REG_FLD(10, 16)
#define OVL_RDMA2_MEM_GMC_SETTING1_FLD_RDMA2_OSTD_ULTRA_TH     REG_FLD(10, 0)

#define OVL_RDMA2_MEM_SLOW_CON_FLD_RDMA2_SLOW_CNT              REG_FLD(16, 16)
#define OVL_RDMA2_MEM_SLOW_CON_FLD_RDMA2_SLOW_EN               REG_FLD(1, 0)

#define OVL_RDMA2_FIFO_CTRL_FLD_RDMA2_FIFO_UND_EN              REG_FLD(1, 31)
#define OVL_RDMA2_FIFO_CTRL_FLD_RDMA2_FIFO_SIZE                REG_FLD(12, 16)
#define OVL_RDMA2_FIFO_CTRL_FLD_RDMA2_FIFO_THRD                REG_FLD(10, 0)

#define OVL_RDMA3_CTRL_FLD_RDMA3_FIFO_USED_SIZE                REG_FLD(12, 16)
#define OVL_RDMA3_CTRL_FLD_RDMA3_INTERLACE                     REG_FLD(1, 4)
#define OVL_RDMA3_CTRL_FLD_RDMA3_EN                            REG_FLD(1, 0)

#define OVL_RDMA3_MEM_GMC_SETTING1_FLD_RDMA3_PRE_ULTRA_THRESHOLD_HIGH_OFS REG_FLD(1, 31)
#define OVL_RDMA3_MEM_GMC_SETTING1_FLD_RDMA3_ULTRA_THRESHOLD_HIGH_OFS REG_FLD(1, 28)
#define OVL_RDMA3_MEM_GMC_SETTING1_FLD_RDMA3_OSTD_PREULTRA_TH  REG_FLD(10, 16)
#define OVL_RDMA3_MEM_GMC_SETTING1_FLD_RDMA3_OSTD_ULTRA_TH     REG_FLD(10, 0)

#define OVL_RDMA3_MEM_SLOW_CON_FLD_RDMA3_SLOW_CNT              REG_FLD(16, 16)
#define OVL_RDMA3_MEM_SLOW_CON_FLD_RDMA3_SLOW_EN               REG_FLD(1, 0)

#define OVL_RDMA3_FIFO_CTRL_FLD_RDMA3_FIFO_UND_EN              REG_FLD(1, 31)
#define OVL_RDMA3_FIFO_CTRL_FLD_RDMA3_FIFO_SIZE                REG_FLD(12, 16)
#define OVL_RDMA3_FIFO_CTRL_FLD_RDMA3_FIFO_THRD                REG_FLD(10, 0)

#define OVL_L0_Y2R_PARA_R0_FLD_C_CF_RMU                        REG_FLD(13, 16)
#define OVL_L0_Y2R_PARA_R0_FLD_C_CF_RMY                        REG_FLD(13, 0)

#define OVL_L0_Y2R_PARA_R1_FLD_C_CF_RMV                        REG_FLD(13, 0)

#define OVL_L0_Y2R_PARA_G0_FLD_C_CF_GMU                        REG_FLD(13, 16)
#define OVL_L0_Y2R_PARA_G0_FLD_C_CF_GMY                        REG_FLD(13, 0)

#define OVL_L0_Y2R_PARA_G1_FLD_C_CF_GMV                        REG_FLD(13, 0)

#define OVL_L0_Y2R_PARA_B0_FLD_C_CF_BMU                        REG_FLD(13, 16)
#define OVL_L0_Y2R_PARA_B0_FLD_C_CF_BMY                        REG_FLD(13, 0)

#define OVL_L0_Y2R_PARA_B1_FLD_C_CF_BMV                        REG_FLD(13, 0)

#define OVL_L0_Y2R_PARA_YUV_A_0_FLD_C_CF_UA                    REG_FLD(9, 16)
#define OVL_L0_Y2R_PARA_YUV_A_0_FLD_C_CF_YA                    REG_FLD(9, 0)

#define OVL_L0_Y2R_PARA_YUV_A_1_FLD_C_CF_VA                    REG_FLD(9, 0)

#define OVL_L0_Y2R_PARA_RGB_A_0_FLD_C_CF_GA                    REG_FLD(9, 16)
#define OVL_L0_Y2R_PARA_RGB_A_0_FLD_C_CF_RA                    REG_FLD(9, 0)

#define OVL_L0_Y2R_PARA_RGB_A_1_FLD_C_CF_BA                    REG_FLD(9, 0)

#define OVL_L1_Y2R_PARA_R0_FLD_C_CF_RMU                        REG_FLD(13, 16)
#define OVL_L1_Y2R_PARA_R0_FLD_C_CF_RMY                        REG_FLD(13, 0)

#define OVL_L1_Y2R_PARA_R1_FLD_C_CF_RMV                        REG_FLD(13, 0)

#define OVL_L1_Y2R_PARA_G0_FLD_C_CF_GMU                        REG_FLD(13, 16)
#define OVL_L1_Y2R_PARA_G0_FLD_C_CF_GMY                        REG_FLD(13, 0)

#define OVL_L1_Y2R_PARA_G1_FLD_C_CF_GMV                        REG_FLD(13, 0)

#define OVL_L1_Y2R_PARA_B0_FLD_C_CF_BMU                        REG_FLD(13, 16)
#define OVL_L1_Y2R_PARA_B0_FLD_C_CF_BMY                        REG_FLD(13, 0)

#define OVL_L1_Y2R_PARA_B1_FLD_C_CF_BMV                        REG_FLD(13, 0)

#define OVL_L1_Y2R_PARA_YUV_A_0_FLD_C_CF_UA                    REG_FLD(9, 16)
#define OVL_L1_Y2R_PARA_YUV_A_0_FLD_C_CF_YA                    REG_FLD(9, 0)

#define OVL_L1_Y2R_PARA_YUV_A_1_FLD_C_CF_VA                    REG_FLD(9, 0)

#define OVL_L1_Y2R_PARA_RGB_A_0_FLD_C_CF_GA                    REG_FLD(9, 16)
#define OVL_L1_Y2R_PARA_RGB_A_0_FLD_C_CF_RA                    REG_FLD(9, 0)

#define OVL_L1_Y2R_PARA_RGB_A_1_FLD_C_CF_BA                    REG_FLD(9, 0)

#define OVL_L2_Y2R_PARA_R0_FLD_C_CF_RMU                        REG_FLD(13, 16)
#define OVL_L2_Y2R_PARA_R0_FLD_C_CF_RMY                        REG_FLD(13, 0)

#define OVL_L2_Y2R_PARA_R1_FLD_C_CF_RMV                        REG_FLD(13, 0)

#define OVL_L2_Y2R_PARA_G0_FLD_C_CF_GMU                        REG_FLD(13, 16)
#define OVL_L2_Y2R_PARA_G0_FLD_C_CF_GMY                        REG_FLD(13, 0)

#define OVL_L2_Y2R_PARA_G1_FLD_C_CF_GMV                        REG_FLD(13, 0)

#define OVL_L2_Y2R_PARA_B0_FLD_C_CF_BMU                        REG_FLD(13, 16)
#define OVL_L2_Y2R_PARA_B0_FLD_C_CF_BMY                        REG_FLD(13, 0)

#define OVL_L2_Y2R_PARA_B1_FLD_C_CF_BMV                        REG_FLD(13, 0)

#define OVL_L2_Y2R_PARA_YUV_A_0_FLD_C_CF_UA                    REG_FLD(9, 16)
#define OVL_L2_Y2R_PARA_YUV_A_0_FLD_C_CF_YA                    REG_FLD(9, 0)

#define OVL_L2_Y2R_PARA_YUV_A_1_FLD_C_CF_VA                    REG_FLD(9, 0)

#define OVL_L2_Y2R_PARA_RGB_A_0_FLD_C_CF_GA                    REG_FLD(9, 16)
#define OVL_L2_Y2R_PARA_RGB_A_0_FLD_C_CF_RA                    REG_FLD(9, 0)

#define OVL_L2_Y2R_PARA_RGB_A_1_FLD_C_CF_BA                    REG_FLD(9, 0)

#define OVL_L3_Y2R_PARA_R0_FLD_C_CF_RMU                        REG_FLD(13, 16)
#define OVL_L3_Y2R_PARA_R0_FLD_C_CF_RMY                        REG_FLD(13, 0)

#define OVL_L3_Y2R_PARA_R1_FLD_C_CF_RMV                        REG_FLD(13, 0)

#define OVL_L3_Y2R_PARA_G0_FLD_C_CF_GMU                        REG_FLD(13, 16)
#define OVL_L3_Y2R_PARA_G0_FLD_C_CF_GMY                        REG_FLD(13, 0)

#define OVL_L3_Y2R_PARA_G1_FLD_C_CF_GMV                        REG_FLD(13, 0)

#define OVL_L3_Y2R_PARA_B0_FLD_C_CF_BMU                        REG_FLD(13, 16)
#define OVL_L3_Y2R_PARA_B0_FLD_C_CF_BMY                        REG_FLD(13, 0)

#define OVL_L3_Y2R_PARA_B1_FLD_C_CF_BMV                        REG_FLD(13, 0)

#define OVL_L3_Y2R_PARA_YUV_A_0_FLD_C_CF_UA                    REG_FLD(9, 16)
#define OVL_L3_Y2R_PARA_YUV_A_0_FLD_C_CF_YA                    REG_FLD(9, 0)

#define OVL_L3_Y2R_PARA_YUV_A_1_FLD_C_CF_VA                    REG_FLD(9, 0)

#define OVL_L3_Y2R_PARA_RGB_A_0_FLD_C_CF_GA                    REG_FLD(9, 16)
#define OVL_L3_Y2R_PARA_RGB_A_0_FLD_C_CF_RA                    REG_FLD(9, 0)

#define OVL_L3_Y2R_PARA_RGB_A_1_FLD_C_CF_BA                    REG_FLD(9, 0)

#define OVL_DEBUG_MON_SEL_FLD_DBG_MON_SEL                      REG_FLD(4, 0)

#define OVL_BLD_EXT_FLD_EL2_MINUS_BLD                          REG_FLD(1, 7)
#define OVL_BLD_EXT_FLD_EL1_MINUS_BLD                          REG_FLD(1, 6)
#define OVL_BLD_EXT_FLD_EL0_MINUS_BLD                          REG_FLD(1, 5)
#define OVL_BLD_EXT_FLD_LC_MINUS_BLD                           REG_FLD(1, 4)
#define OVL_BLD_EXT_FLD_L3_MINUS_BLD                           REG_FLD(1, 3)
#define OVL_BLD_EXT_FLD_L2_MINUS_BLD                           REG_FLD(1, 2)
#define OVL_BLD_EXT_FLD_L1_MINUS_BLD                           REG_FLD(1, 1)
#define OVL_BLD_EXT_FLD_L0_MINUS_BLD                           REG_FLD(1, 0)

#define OVL_RDMA0_MEM_GMC_SETTING2_FLD_RDMA0_FORCE_REQ_THRESHOLD REG_FLD(1, 30)
#define OVL_RDMA0_MEM_GMC_SETTING2_FLD_RDMA0_REQ_THRESHOLD_ULTRA REG_FLD(1, 29)
#define OVL_RDMA0_MEM_GMC_SETTING2_FLD_RDMA0_REQ_THRESHOLD_PREULTRA REG_FLD(1, 28)
#define OVL_RDMA0_MEM_GMC_SETTING2_FLD_RDMA0_ISSUE_REQ_THRESHOLD_URG REG_FLD(12, 16)
#define OVL_RDMA0_MEM_GMC_SETTING2_FLD_RDMA0_ISSUE_REQ_THRESHOLD REG_FLD(12, 0)

#define OVL_RDMA1_MEM_GMC_SETTING2_FLD_RDMA1_FORCE_REQ_THRESHOLD REG_FLD(1, 30)
#define OVL_RDMA1_MEM_GMC_SETTING2_FLD_RDMA1_REQ_THRESHOLD_ULTRA REG_FLD(1, 29)
#define OVL_RDMA1_MEM_GMC_SETTING2_FLD_RDMA1_REQ_THRESHOLD_PREULTRA REG_FLD(1, 28)
#define OVL_RDMA1_MEM_GMC_SETTING2_FLD_RDMA1_ISSUE_REQ_THRESHOLD_URG REG_FLD(12, 16)
#define OVL_RDMA1_MEM_GMC_SETTING2_FLD_RDMA1_ISSUE_REQ_THRESHOLD REG_FLD(12, 0)

#define OVL_RDMA2_MEM_GMC_SETTING2_FLD_RDMA2_FORCE_REQ_THRESHOLD REG_FLD(1, 30)
#define OVL_RDMA2_MEM_GMC_SETTING2_FLD_RDMA2_REQ_THRESHOLD_ULTRA REG_FLD(1, 29)
#define OVL_RDMA2_MEM_GMC_SETTING2_FLD_RDMA2_REQ_THRESHOLD_PREULTRA REG_FLD(1, 28)
#define OVL_RDMA2_MEM_GMC_SETTING2_FLD_RDMA2_ISSUE_REQ_THRESHOLD_URG REG_FLD(12, 16)
#define OVL_RDMA2_MEM_GMC_SETTING2_FLD_RDMA2_ISSUE_REQ_THRESHOLD REG_FLD(12, 0)

#define OVL_RDMA3_MEM_GMC_SETTING2_FLD_RDMA3_FORCE_REQ_THRESHOLD REG_FLD(1, 30)
#define OVL_RDMA3_MEM_GMC_SETTING2_FLD_RDMA3_REQ_THRESHOLD_ULTRA REG_FLD(1, 29)
#define OVL_RDMA3_MEM_GMC_SETTING2_FLD_RDMA3_REQ_THRESHOLD_PREULTRA REG_FLD(1, 28)
#define OVL_RDMA3_MEM_GMC_SETTING2_FLD_RDMA3_ISSUE_REQ_THRESHOLD_URG REG_FLD(12, 16)
#define OVL_RDMA3_MEM_GMC_SETTING2_FLD_RDMA3_ISSUE_REQ_THRESHOLD REG_FLD(12, 0)

#define OVL_RDMA_BURST_CON0_FLD_BURST_128B_BOUND               REG_FLD(1, 28)
#define OVL_RDMA_BURST_CON0_FLD_BURST15A_32B                   REG_FLD(3, 24)
#define OVL_RDMA_BURST_CON0_FLD_BURST14A_32B                   REG_FLD(3, 20)
#define OVL_RDMA_BURST_CON0_FLD_BURST13A_32B                   REG_FLD(3, 16)
#define OVL_RDMA_BURST_CON0_FLD_BURST12A_32B                   REG_FLD(3, 12)
#define OVL_RDMA_BURST_CON0_FLD_BURST11A_32B                   REG_FLD(3, 8)
#define OVL_RDMA_BURST_CON0_FLD_BURST10A_32B                   REG_FLD(3, 4)
#define OVL_RDMA_BURST_CON0_FLD_BURST9A_32B                    REG_FLD(3, 0)

#define OVL_RDMA_BURST_CON1_FLD_BURST15A_N32B                  REG_FLD(3, 24)
#define OVL_RDMA_BURST_CON1_FLD_BURST14A_N32B                  REG_FLD(3, 20)
#define OVL_RDMA_BURST_CON1_FLD_BURST13A_N32B                  REG_FLD(3, 16)
#define OVL_RDMA_BURST_CON1_FLD_BURST12A_N32B                  REG_FLD(3, 12)
#define OVL_RDMA_BURST_CON1_FLD_BURST11A_N32B                  REG_FLD(3, 8)
#define OVL_RDMA_BURST_CON1_FLD_BURST10A_N32B                  REG_FLD(3, 4)
#define OVL_RDMA_BURST_CON1_FLD_BURST9A_N32B                   REG_FLD(3, 0)

#define OVL_RDMA_GREQ_NUM_FLD_IOBUF_FLUSH_ULTRA                REG_FLD(1, 31)
#define OVL_RDMA_GREQ_NUM_FLD_IOBUF_FLUSH_PREULTRA             REG_FLD(1, 30)
#define OVL_RDMA_GREQ_NUM_FLD_GRP_BRK_STOP                     REG_FLD(1, 29)
#define OVL_RDMA_GREQ_NUM_FLD_GRP_END_STOP                     REG_FLD(1, 28)
#define OVL_RDMA_GREQ_NUM_FLD_GREQ_STOP_EN                     REG_FLD(1, 27)
#define OVL_RDMA_GREQ_NUM_FLD_GREQ_DIS_CNT                     REG_FLD(3, 24)
#define OVL_RDMA_GREQ_NUM_FLD_OSTD_GREQ_NUM                    REG_FLD(8, 16)
#define OVL_RDMA_GREQ_NUM_FLD_LAYER3_GREQ_NUM                  REG_FLD(4, 12)
#define OVL_RDMA_GREQ_NUM_FLD_LAYER2_GREQ_NUM                  REG_FLD(4, 8)
#define OVL_RDMA_GREQ_NUM_FLD_LAYER1_GREQ_NUM                  REG_FLD(4, 4)
#define OVL_RDMA_GREQ_NUM_FLD_LAYER0_GREQ_NUM                  REG_FLD(4, 0)

#define OVL_RDMA_GREQ_URG_NUM_FLD_GREQ_NUM_SHT                 REG_FLD(2, 30)
#define OVL_RDMA_GREQ_URG_NUM_FLD_GREQ_NUM_SHT_VAL             REG_FLD(1, 29)
#define OVL_RDMA_GREQ_URG_NUM_FLD_ARG_URG_BIAS                 REG_FLD(1, 28)
#define OVL_RDMA_GREQ_URG_NUM_FLD_ARG_GREQ_URG_TH              REG_FLD(10, 16)
#define OVL_RDMA_GREQ_URG_NUM_FLD_LAYER3_GREQ_URG_NUM          REG_FLD(4, 12)
#define OVL_RDMA_GREQ_URG_NUM_FLD_LAYER2_GREQ_URG_NUM          REG_FLD(4, 8)
#define OVL_RDMA_GREQ_URG_NUM_FLD_LAYER1_GREQ_URG_NUM          REG_FLD(4, 4)
#define OVL_RDMA_GREQ_URG_NUM_FLD_LAYER0_GREQ_URG_NUM          REG_FLD(4, 0)

#define OVL_DUMMY_REG_FLD_OVERLAY_DUMMY                        REG_FLD(32, 0)

#define OVL_GDRDY_PRD_FLD_GDRDY_PRD                            REG_FLD(24, 0)

#define OVL_RDMA_ULTRA_SRC_FLD_ULTRA_RDMA_SRC                  REG_FLD(2, 14)
#define OVL_RDMA_ULTRA_SRC_FLD_ULTRA_ROI_END_SRC               REG_FLD(2, 12)
#define OVL_RDMA_ULTRA_SRC_FLD_ULTRA_SMI_SRC                   REG_FLD(2, 10)
#define OVL_RDMA_ULTRA_SRC_FLD_ULTRA_BUF_SRC                   REG_FLD(2, 8)
#define OVL_RDMA_ULTRA_SRC_FLD_PREULTRA_RDMA_SRC               REG_FLD(2, 6)
#define OVL_RDMA_ULTRA_SRC_FLD_PREULTRA_ROI_END_SRC            REG_FLD(2, 4)
#define OVL_RDMA_ULTRA_SRC_FLD_PREULTRA_SMI_SRC                REG_FLD(2, 2)
#define OVL_RDMA_ULTRA_SRC_FLD_PREULTRA_BUF_SRC                REG_FLD(2, 0)

#define OVL_RDMA0_BUF_LOW_TH_FLD_RDMA0_PREULTRA_LOW_TH         REG_FLD(12, 12)
#define OVL_RDMA0_BUF_LOW_TH_FLD_RDMA0_ULTRA_LOW_TH            REG_FLD(12, 0)

#define OVL_RDMA1_BUF_LOW_TH_FLD_RDMA1_PREULTRA_LOW_TH         REG_FLD(12, 12)
#define OVL_RDMA1_BUF_LOW_TH_FLD_RDMA1_ULTRA_LOW_TH            REG_FLD(12, 0)

#define OVL_RDMA2_BUF_LOW_TH_FLD_RDMA2_PREULTRA_LOW_TH         REG_FLD(12, 12)
#define OVL_RDMA2_BUF_LOW_TH_FLD_RDMA2_ULTRA_LOW_TH            REG_FLD(12, 0)

#define OVL_RDMA3_BUF_LOW_TH_FLD_RDMA3_PREULTRA_LOW_TH         REG_FLD(12, 12)
#define OVL_RDMA3_BUF_LOW_TH_FLD_RDMA3_ULTRA_LOW_TH            REG_FLD(12, 0)

#define OVL_RDMA0_BUF_HIGH_TH_FLD_RDMA0_PREULTRA_HIGH_DIS      REG_FLD(1, 31)
#define OVL_RDMA0_BUF_HIGH_TH_FLD_RDMA0_PREULTRA_HIGH_TH       REG_FLD(12, 12)

#define OVL_RDMA1_BUF_HIGH_TH_FLD_RDMA1_PREULTRA_HIGH_DIS      REG_FLD(1, 31)
#define OVL_RDMA1_BUF_HIGH_TH_FLD_RDMA1_PREULTRA_HIGH_TH       REG_FLD(12, 12)

#define OVL_RDMA2_BUF_HIGH_TH_FLD_RDMA2_PREULTRA_HIGH_DIS      REG_FLD(1, 31)
#define OVL_RDMA2_BUF_HIGH_TH_FLD_RDMA2_PREULTRA_HIGH_TH       REG_FLD(12, 12)

#define OVL_RDMA3_BUF_HIGH_TH_FLD_RDMA3_PREULTRA_HIGH_DIS      REG_FLD(1, 31)
#define OVL_RDMA3_BUF_HIGH_TH_FLD_RDMA3_PREULTRA_HIGH_TH       REG_FLD(12, 12)

#define OVL_SMI_DBG_FLD_SMI_FSM                                REG_FLD(10, 0)

#define OVL_GREQ_LAYER_CNT_FLD_LAYER3_GREQ_CNT                 REG_FLD(6, 24)
#define OVL_GREQ_LAYER_CNT_FLD_LAYER2_GREQ_CNT                 REG_FLD(6, 16)
#define OVL_GREQ_LAYER_CNT_FLD_LAYER1_GREQ_CNT                 REG_FLD(6, 8)
#define OVL_GREQ_LAYER_CNT_FLD_LAYER0_GREQ_CNT                 REG_FLD(6, 0)

#define OVL_GDRDY_PRD_NUM_FLD_GDRDY_PRD_NUM                    REG_FLD(24, 0)

#define OVL_FLOW_CTRL_DBG_FLD_OVL_UPD_REG                      REG_FLD(1, 31)
#define OVL_FLOW_CTRL_DBG_FLD_REG_UPDATE                       REG_FLD(1, 30)
#define OVL_FLOW_CTRL_DBG_FLD_OVL_CLR                          REG_FLD(1, 29)
#define OVL_FLOW_CTRL_DBG_FLD_OVL_START                        REG_FLD(1, 28)
#define OVL_FLOW_CTRL_DBG_FLD_OVL_RUNNING                      REG_FLD(1, 27)
#define OVL_FLOW_CTRL_DBG_FLD_FRAME_DONE                       REG_FLD(1, 26)
#define OVL_FLOW_CTRL_DBG_FLD_FRAME_UNDERRUN                   REG_FLD(1, 25)
#define OVL_FLOW_CTRL_DBG_FLD_FRAME_SWRST_DONE                 REG_FLD(1, 24)
#define OVL_FLOW_CTRL_DBG_FLD_FRAME_HWRST_DONE                 REG_FLD(1, 23)
#define OVL_FLOW_CTRL_DBG_FLD_TRIG                             REG_FLD(1, 21)
#define OVL_FLOW_CTRL_DBG_FLD_RST                              REG_FLD(1, 20)
#define OVL_FLOW_CTRL_DBG_FLD_RDMA0_IDLE                       REG_FLD(1, 19)
#define OVL_FLOW_CTRL_DBG_FLD_RDMA1_IDLE                       REG_FLD(1, 18)
#define OVL_FLOW_CTRL_DBG_FLD_RDMA2_IDLE                       REG_FLD(1, 17)
#define OVL_FLOW_CTRL_DBG_FLD_RDMA3_IDLE                       REG_FLD(1, 16)
#define OVL_FLOW_CTRL_DBG_FLD_OUT_IDLE                         REG_FLD(1, 15)
#define OVL_FLOW_CTRL_DBG_FLD_OVL_OUT_READY                    REG_FLD(1, 13)
#define OVL_FLOW_CTRL_DBG_FLD_OVL_OUT_VALID                    REG_FLD(1, 12)
#define OVL_FLOW_CTRL_DBG_FLD_BLEND_IDLE                       REG_FLD(1, 11)
#define OVL_FLOW_CTRL_DBG_FLD_ADDCON_IDLE                      REG_FLD(1, 10)
#define OVL_FLOW_CTRL_DBG_FLD_FSM_STATE                        REG_FLD(10, 0)

#define OVL_ADDCON_DBG_FLD_L3_WIN_HIT                          REG_FLD(1, 31)
#define OVL_ADDCON_DBG_FLD_L2_WIN_HIT                          REG_FLD(1, 30)
#define OVL_ADDCON_DBG_FLD_ROI_Y                               REG_FLD(13, 16)
#define OVL_ADDCON_DBG_FLD_L1_WIN_HIT                          REG_FLD(1, 15)
#define OVL_ADDCON_DBG_FLD_L0_WIN_HIT                          REG_FLD(1, 14)
#define OVL_ADDCON_DBG_FLD_ROI_X                               REG_FLD(13, 0)

#define OVL_RDMA0_DBG_FLD_SMI_GREQ                             REG_FLD(1, 31)
#define OVL_RDMA0_DBG_FLD_RDMA0_SMI_BUSY                       REG_FLD(1, 30)
#define OVL_RDMA0_DBG_FLD_RDMA0_OUT_VALID                      REG_FLD(1, 29)
#define OVL_RDMA0_DBG_FLD_RDMA0_OUT_READY                      REG_FLD(1, 28)
#define OVL_RDMA0_DBG_FLD_RDMA0_OUT_DATA                       REG_FLD(24, 4)
#define OVL_RDMA0_DBG_FLD_RDMA0_LAYER_GREQ                     REG_FLD(1, 3)
#define OVL_RDMA0_DBG_FLD_RDMA0_WRAM_RST_CS                    REG_FLD(3, 0)

#define OVL_RDMA1_DBG_FLD_SMI_GREQ                             REG_FLD(1, 31)
#define OVL_RDMA1_DBG_FLD_RDMA1_SMI_BUSY                       REG_FLD(1, 30)
#define OVL_RDMA1_DBG_FLD_RDMA1_OUT_VALID                      REG_FLD(1, 29)
#define OVL_RDMA1_DBG_FLD_RDMA1_OUT_READY                      REG_FLD(1, 28)
#define OVL_RDMA1_DBG_FLD_RDMA1_OUT_DATA                       REG_FLD(24, 4)
#define OVL_RDMA1_DBG_FLD_RDMA1_LAYER_GREQ                     REG_FLD(1, 3)
#define OVL_RDMA1_DBG_FLD_RDMA1_WRAM_RST_CS                    REG_FLD(3, 0)

#define OVL_RDMA2_DBG_FLD_SMI_GREQ                             REG_FLD(1, 31)
#define OVL_RDMA2_DBG_FLD_RDMA2_SMI_BUSY                       REG_FLD(1, 30)
#define OVL_RDMA2_DBG_FLD_RDMA2_OUT_VALID                      REG_FLD(1, 29)
#define OVL_RDMA2_DBG_FLD_RDMA2_OUT_READY                      REG_FLD(1, 28)
#define OVL_RDMA2_DBG_FLD_RDMA2_OUT_DATA                       REG_FLD(24, 4)
#define OVL_RDMA2_DBG_FLD_RDMA2_LAYER_GREQ                     REG_FLD(1, 3)
#define OVL_RDMA2_DBG_FLD_RDMA2_WRAM_RST_CS                    REG_FLD(3, 0)

#define OVL_RDMA3_DBG_FLD_SMI_GREQ                             REG_FLD(1, 31)
#define OVL_RDMA3_DBG_FLD_RDMA3_SMI_BUSY                       REG_FLD(1, 30)
#define OVL_RDMA3_DBG_FLD_RDMA3_OUT_VALID                      REG_FLD(1, 29)
#define OVL_RDMA3_DBG_FLD_RDMA3_OUT_READY                      REG_FLD(1, 28)
#define OVL_RDMA3_DBG_FLD_RDMA3_OUT_DATA                       REG_FLD(24, 4)
#define OVL_RDMA3_DBG_FLD_RDMA3_LAYER_GREQ                     REG_FLD(1, 3)
#define OVL_RDMA3_DBG_FLD_RDMA3_WRAM_RST_CS                    REG_FLD(3, 0)

#define OVL_L0_CLR_FLD_ALPHA                                   REG_FLD(8, 24)
#define OVL_L0_CLR_FLD_RED                                     REG_FLD(8, 16)
#define OVL_L0_CLR_FLD_GREEN                                   REG_FLD(8, 8)
#define OVL_L0_CLR_FLD_BLUE                                    REG_FLD(8, 0)

#define OVL_L1_CLR_FLD_ALPHA                                   REG_FLD(8, 24)
#define OVL_L1_CLR_FLD_RED                                     REG_FLD(8, 16)
#define OVL_L1_CLR_FLD_GREEN                                   REG_FLD(8, 8)
#define OVL_L1_CLR_FLD_BLUE                                    REG_FLD(8, 0)

#define OVL_L2_CLR_FLD_ALPHA                                   REG_FLD(8, 24)
#define OVL_L2_CLR_FLD_RED                                     REG_FLD(8, 16)
#define OVL_L2_CLR_FLD_GREEN                                   REG_FLD(8, 8)
#define OVL_L2_CLR_FLD_BLUE                                    REG_FLD(8, 0)

#define OVL_L3_CLR_FLD_ALPHA                                   REG_FLD(8, 24)
#define OVL_L3_CLR_FLD_RED                                     REG_FLD(8, 16)
#define OVL_L3_CLR_FLD_GREEN                                   REG_FLD(8, 8)
#define OVL_L3_CLR_FLD_BLUE                                    REG_FLD(8, 0)

#define OVL_LC_CLR_FLD_ALPHA                                   REG_FLD(8, 24)
#define OVL_LC_CLR_FLD_RED                                     REG_FLD(8, 16)
#define OVL_LC_CLR_FLD_GREEN                                   REG_FLD(8, 8)
#define OVL_LC_CLR_FLD_BLUE                                    REG_FLD(8, 0)

#define OVL_CRC_FLD_CRC_RDY                                    REG_FLD(1, 31)
#define OVL_CRC_FLD_CRC_OUT                                    REG_FLD(31, 0)

#define OVL_LC_CON_FLD_DSTKEY_EN                               REG_FLD(1, 31)
#define OVL_LC_CON_FLD_SRCKEY_EN                               REG_FLD(1, 30)
#define OVL_LC_CON_FLD_LAYER_SRC                               REG_FLD(2, 28)
#define OVL_LC_CON_FLD_R_FIRST                                 REG_FLD(1, 22)
#define OVL_LC_CON_FLD_LANDSCAPE                               REG_FLD(1, 21)
#define OVL_LC_CON_FLD_EN_3D                                   REG_FLD(1, 20)
#define OVL_LC_CON_FLD_ALPHA_EN                                REG_FLD(1, 8)
#define OVL_LC_CON_FLD_ALPHA                                   REG_FLD(8, 0)

#define OVL_LC_SRCKEY_FLD_SRCKEY                               REG_FLD(32, 0)

#define OVL_LC_SRC_SIZE_FLD_LC_SRC_H                           REG_FLD(13, 16)
#define OVL_LC_SRC_SIZE_FLD_LC_SRC_W                           REG_FLD(13, 0)

#define OVL_LC_OFFSET_FLD_LC_YOFF                              REG_FLD(12, 16)
#define OVL_LC_OFFSET_FLD_LC_XOFF                              REG_FLD(12, 0)

#define OVL_LC_SRC_SEL_FLD_SURFL_EN                            REG_FLD(1, 31)
#define OVL_LC_SRC_SEL_FLD_LC_BLEND_RND_SHT                    REG_FLD(1, 30)
#define OVL_LC_SRC_SEL_FLD_LC_SRGB_SEL_EXT2                    REG_FLD(1, 29)
#define OVL_LC_SRC_SEL_FLD_LC_CONST_BLD                        REG_FLD(1, 28)
#define OVL_LC_SRC_SEL_FLD_LC_DRGB_SEL_EXT                     REG_FLD(1, 27)
#define OVL_LC_SRC_SEL_FLD_LC_DA_SEL_EXT                       REG_FLD(1, 26)
#define OVL_LC_SRC_SEL_FLD_LC_SRGB_SEL_EXT                     REG_FLD(1, 25)
#define OVL_LC_SRC_SEL_FLD_LC_SA_SEL_EXT                       REG_FLD(1, 24)
#define OVL_LC_SRC_SEL_FLD_LC_DRGB_SEL                         REG_FLD(2, 22)
#define OVL_LC_SRC_SEL_FLD_LC_DA_SEL                           REG_FLD(2, 20)
#define OVL_LC_SRC_SEL_FLD_LC_SRGB_SEL                         REG_FLD(2, 18)
#define OVL_LC_SRC_SEL_FLD_LC_SA_SEL                           REG_FLD(2, 16)
#define OVL_LC_SRC_SEL_FLD_CONST_LAYER_SEL                     REG_FLD(3, 0)

#define OVL_BANK_CON_FLD_OVL_BANK_CON                          REG_FLD(5, 0)

#define OVL_FUNC_DCM0_FLD_OVL_FUNC_DCM0                        REG_FLD(32, 0)

#define OVL_FUNC_DCM1_FLD_OVL_FUNC_DCM1                        REG_FLD(32, 0)

#define OVL_DVFS_L0_ROI_FLD_L0_DVFS_ROI_BB                     REG_FLD(13, 16)
#define OVL_DVFS_L0_ROI_FLD_L0_DVFS_ROI_TB                     REG_FLD(13, 0)

#define OVL_DVFS_L1_ROI_FLD_L1_DVFS_ROI_BB                     REG_FLD(13, 16)
#define OVL_DVFS_L1_ROI_FLD_L1_DVFS_ROI_TB                     REG_FLD(13, 0)

#define OVL_DVFS_L2_ROI_FLD_L2_DVFS_ROI_BB                     REG_FLD(13, 16)
#define OVL_DVFS_L2_ROI_FLD_L2_DVFS_ROI_TB                     REG_FLD(13, 0)

#define OVL_DVFS_L3_ROI_FLD_L3_DVFS_ROI_BB                     REG_FLD(13, 16)
#define OVL_DVFS_L3_ROI_FLD_L3_DVFS_ROI_TB                     REG_FLD(13, 0)

#define OVL_DVFS_EL0_ROI_FLD_EL0_DVFS_ROI_BB                   REG_FLD(13, 16)
#define OVL_DVFS_EL0_ROI_FLD_EL0_DVFS_ROI_TB                   REG_FLD(13, 0)

#define OVL_DVFS_EL1_ROI_FLD_EL1_DVFS_ROI_BB                   REG_FLD(13, 16)
#define OVL_DVFS_EL1_ROI_FLD_EL1_DVFS_ROI_TB                   REG_FLD(13, 0)

#define OVL_DVFS_EL2_ROI_FLD_EL2_DVFS_ROI_BB                   REG_FLD(13, 16)
#define OVL_DVFS_EL2_ROI_FLD_EL2_DVFS_ROI_TB                   REG_FLD(13, 0)

#define OVL_DATAPATH_EXT_CON_FLD_EL2_LAYER_SEL                 REG_FLD(3, 24)
#define OVL_DATAPATH_EXT_CON_FLD_EL1_LAYER_SEL                 REG_FLD(3, 20)
#define OVL_DATAPATH_EXT_CON_FLD_EL0_LAYER_SEL                 REG_FLD(3, 16)
#define OVL_DATAPATH_EXT_CON_FLD_EL2_GPU_MODE                  REG_FLD(1, 10)
#define OVL_DATAPATH_EXT_CON_FLD_EL1_GPU_MODE                  REG_FLD(1, 9)
#define OVL_DATAPATH_EXT_CON_FLD_EL0_GPU_MODE                  REG_FLD(1, 8)
#define OVL_DATAPATH_EXT_CON_FLD_EL2_EN                        REG_FLD(1, 2)
#define OVL_DATAPATH_EXT_CON_FLD_EL1_EN                        REG_FLD(1, 1)
#define OVL_DATAPATH_EXT_CON_FLD_EL0_EN                        REG_FLD(1, 0)

#define OVL_EL0_CON_FLD_DSTKEY_EN                              REG_FLD(1, 31)
#define OVL_EL0_CON_FLD_SRCKEY_EN                              REG_FLD(1, 30)
#define OVL_EL0_CON_FLD_LAYER_SRC                              REG_FLD(2, 28)
#define OVL_EL0_CON_FLD_MTX_EN                                 REG_FLD(1, 27)
#define OVL_EL0_CON_FLD_MTX_AUTO_DIS                           REG_FLD(1, 26)
#define OVL_EL0_CON_FLD_RGB_SWAP                               REG_FLD(1, 25)
#define OVL_EL0_CON_FLD_BYTE_SWAP                              REG_FLD(1, 24)
#define OVL_EL0_CON_FLD_CLRFMT_MAN                             REG_FLD(1, 23)
#define OVL_EL0_CON_FLD_R_FIRST                                REG_FLD(1, 22)
#define OVL_EL0_CON_FLD_LANDSCAPE                              REG_FLD(1, 21)
#define OVL_EL0_CON_FLD_EN_3D                                  REG_FLD(1, 20)
#define OVL_EL0_CON_FLD_INT_MTX_SEL                            REG_FLD(4, 16)
#define OVL_EL0_CON_FLD_CLRFMT                                 REG_FLD(4, 12)
#define OVL_EL0_CON_FLD_EXT_MTX_EN                             REG_FLD(1, 11)
#define OVL_EL0_CON_FLD_HORIZONTAL_FLIP_EN                     REG_FLD(1, 10)
#define OVL_EL0_CON_FLD_VERTICAL_FLIP_EN                       REG_FLD(1, 9)
#define OVL_EL0_CON_FLD_ALPHA_EN                               REG_FLD(1, 8)
#define OVL_EL0_CON_FLD_ALPHA                                  REG_FLD(8, 0)

#define OVL_EL0_SRCKEY_FLD_SRCKEY                              REG_FLD(32, 0)

#define OVL_EL0_SRC_SIZE_FLD_EL0_SRC_H                         REG_FLD(13, 16)
#define OVL_EL0_SRC_SIZE_FLD_EL0_SRC_W                         REG_FLD(13, 0)

#define OVL_EL0_OFFSET_FLD_EL0_YOFF                            REG_FLD(13, 16)
#define OVL_EL0_OFFSET_FLD_EL0_XOFF                            REG_FLD(13, 0)

#define OVL_EL0_PITCH_FLD_SURFL_EN                             REG_FLD(1, 31)
#define OVL_EL0_PITCH_FLD_EL0_BLEND_RND_SHT                    REG_FLD(1, 30)
#define OVL_EL0_PITCH_FLD_EL0_SRGB_SEL_EXT2                    REG_FLD(1, 29)
#define OVL_EL0_PITCH_FLD_EL0_CONST_BLD                        REG_FLD(1, 28)
#define OVL_EL0_PITCH_FLD_EL0_DRGB_SEL_EXT                     REG_FLD(1, 27)
#define OVL_EL0_PITCH_FLD_EL0_DA_SEL_EXT                       REG_FLD(1, 26)
#define OVL_EL0_PITCH_FLD_EL0_SRGB_SEL_EXT                     REG_FLD(1, 25)
#define OVL_EL0_PITCH_FLD_EL0_SA_SEL_EXT                       REG_FLD(1, 24)
#define OVL_EL0_PITCH_FLD_EL0_DRGB_SEL                         REG_FLD(2, 22)
#define OVL_EL0_PITCH_FLD_EL0_DA_SEL                           REG_FLD(2, 20)
#define OVL_EL0_PITCH_FLD_EL0_SRGB_SEL                         REG_FLD(2, 18)
#define OVL_EL0_PITCH_FLD_EL0_SA_SEL                           REG_FLD(2, 16)
#define OVL_EL0_PITCH_FLD_EL0_SRC_PITCH                        REG_FLD(16, 0)

#define OVL_EL0_TILE_FLD_TILE_HORI_BLOCK_NUM                   REG_FLD(8, 24)
#define OVL_EL0_TILE_FLD_TILE_EN                               REG_FLD(1, 21)
#define OVL_EL0_TILE_FLD_TILE_WIDTH_SEL                        REG_FLD(1, 20)
#define OVL_EL0_TILE_FLD_TILE_HEIGHT                           REG_FLD(20, 0)

#define OVL_EL0_CLIP_FLD_EL0_SRC_BOTTOM_CLIP                   REG_FLD(8, 24)
#define OVL_EL0_CLIP_FLD_EL0_SRC_TOP_CLIP                      REG_FLD(8, 16)
#define OVL_EL0_CLIP_FLD_EL0_SRC_RIGHT_CLIP                    REG_FLD(8, 8)
#define OVL_EL0_CLIP_FLD_EL0_SRC_LEFT_CLIP                     REG_FLD(8, 0)

#define OVL_EL1_CON_FLD_DSTKEY_EN                              REG_FLD(1, 31)
#define OVL_EL1_CON_FLD_SRCKEY_EN                              REG_FLD(1, 30)
#define OVL_EL1_CON_FLD_LAYER_SRC                              REG_FLD(2, 28)
#define OVL_EL1_CON_FLD_MTX_EN                                 REG_FLD(1, 27)
#define OVL_EL1_CON_FLD_MTX_AUTO_DIS                           REG_FLD(1, 26)
#define OVL_EL1_CON_FLD_RGB_SWAP                               REG_FLD(1, 25)
#define OVL_EL1_CON_FLD_BYTE_SWAP                              REG_FLD(1, 24)
#define OVL_EL1_CON_FLD_CLRFMT_MAN                             REG_FLD(1, 23)
#define OVL_EL1_CON_FLD_R_FIRST                                REG_FLD(1, 22)
#define OVL_EL1_CON_FLD_LANDSCAPE                              REG_FLD(1, 21)
#define OVL_EL1_CON_FLD_EN_3D                                  REG_FLD(1, 20)
#define OVL_EL1_CON_FLD_INT_MTX_SEL                            REG_FLD(4, 16)
#define OVL_EL1_CON_FLD_CLRFMT                                 REG_FLD(4, 12)
#define OVL_EL1_CON_FLD_EXT_MTX_EN                             REG_FLD(1, 11)
#define OVL_EL1_CON_FLD_HORIZONTAL_FLIP_EN                     REG_FLD(1, 10)
#define OVL_EL1_CON_FLD_VERTICAL_FLIP_EN                       REG_FLD(1, 9)
#define OVL_EL1_CON_FLD_ALPHA_EN                               REG_FLD(1, 8)
#define OVL_EL1_CON_FLD_ALPHA                                  REG_FLD(8, 0)

#define OVL_EL1_SRCKEY_FLD_SRCKEY                              REG_FLD(32, 0)

#define OVL_EL1_SRC_SIZE_FLD_EL1_SRC_H                         REG_FLD(13, 16)
#define OVL_EL1_SRC_SIZE_FLD_EL1_SRC_W                         REG_FLD(13, 0)

#define OVL_EL1_OFFSET_FLD_EL1_YOFF                            REG_FLD(13, 16)
#define OVL_EL1_OFFSET_FLD_EL1_XOFF                            REG_FLD(13, 0)

#define OVL_EL1_PITCH_FLD_SURFL_EN                             REG_FLD(1, 31)
#define OVL_EL1_PITCH_FLD_EL1_BLEND_RND_SHT                    REG_FLD(1, 30)
#define OVL_EL1_PITCH_FLD_EL1_SRGB_SEL_EXT2                    REG_FLD(1, 29)
#define OVL_EL1_PITCH_FLD_EL1_CONST_BLD                        REG_FLD(1, 28)
#define OVL_EL1_PITCH_FLD_EL1_DRGB_SEL_EXT                     REG_FLD(1, 27)
#define OVL_EL1_PITCH_FLD_EL1_DA_SEL_EXT                       REG_FLD(1, 26)
#define OVL_EL1_PITCH_FLD_EL1_SRGB_SEL_EXT                     REG_FLD(1, 25)
#define OVL_EL1_PITCH_FLD_EL1_SA_SEL_EXT                       REG_FLD(1, 24)
#define OVL_EL1_PITCH_FLD_EL1_DRGB_SEL                         REG_FLD(2, 22)
#define OVL_EL1_PITCH_FLD_EL1_DA_SEL                           REG_FLD(2, 20)
#define OVL_EL1_PITCH_FLD_EL1_SRGB_SEL                         REG_FLD(2, 18)
#define OVL_EL1_PITCH_FLD_EL1_SA_SEL                           REG_FLD(2, 16)
#define OVL_EL1_PITCH_FLD_EL1_SRC_PITCH                        REG_FLD(16, 0)

#define OVL_EL1_TILE_FLD_TILE_HORI_BLOCK_NUM                   REG_FLD(8, 24)
#define OVL_EL1_TILE_FLD_TILE_EN                               REG_FLD(1, 21)
#define OVL_EL1_TILE_FLD_TILE_WIDTH_SEL                        REG_FLD(1, 20)
#define OVL_EL1_TILE_FLD_TILE_HEIGHT                           REG_FLD(20, 0)

#define OVL_EL1_CLIP_FLD_EL1_SRC_BOTTOM_CLIP                   REG_FLD(8, 24)
#define OVL_EL1_CLIP_FLD_EL1_SRC_TOP_CLIP                      REG_FLD(8, 16)
#define OVL_EL1_CLIP_FLD_EL1_SRC_RIGHT_CLIP                    REG_FLD(8, 8)
#define OVL_EL1_CLIP_FLD_EL1_SRC_LEFT_CLIP                     REG_FLD(8, 0)

#define OVL_EL2_CON_FLD_DSTKEY_EN                              REG_FLD(1, 31)
#define OVL_EL2_CON_FLD_SRCKEY_EN                              REG_FLD(1, 30)
#define OVL_EL2_CON_FLD_LAYER_SRC                              REG_FLD(2, 28)
#define OVL_EL2_CON_FLD_MTX_EN                                 REG_FLD(1, 27)
#define OVL_EL2_CON_FLD_MTX_AUTO_DIS                           REG_FLD(1, 26)
#define OVL_EL2_CON_FLD_RGB_SWAP                               REG_FLD(1, 25)
#define OVL_EL2_CON_FLD_BYTE_SWAP                              REG_FLD(1, 24)
#define OVL_EL2_CON_FLD_CLRFMT_MAN                             REG_FLD(1, 23)
#define OVL_EL2_CON_FLD_R_FIRST                                REG_FLD(1, 22)
#define OVL_EL2_CON_FLD_LANDSCAPE                              REG_FLD(1, 21)
#define OVL_EL2_CON_FLD_EN_3D                                  REG_FLD(1, 20)
#define OVL_EL2_CON_FLD_INT_MTX_SEL                            REG_FLD(4, 16)
#define OVL_EL2_CON_FLD_CLRFMT                                 REG_FLD(4, 12)
#define OVL_EL2_CON_FLD_EXT_MTX_EN                             REG_FLD(1, 11)
#define OVL_EL2_CON_FLD_HORIZONTAL_FLIP_EN                     REG_FLD(1, 10)
#define OVL_EL2_CON_FLD_VERTICAL_FLIP_EN                       REG_FLD(1, 9)
#define OVL_EL2_CON_FLD_ALPHA_EN                               REG_FLD(1, 8)
#define OVL_EL2_CON_FLD_ALPHA                                  REG_FLD(8, 0)

#define OVL_EL2_SRCKEY_FLD_SRCKEY                              REG_FLD(32, 0)

#define OVL_EL2_SRC_SIZE_FLD_EL2_SRC_H                         REG_FLD(13, 16)
#define OVL_EL2_SRC_SIZE_FLD_EL2_SRC_W                         REG_FLD(13, 0)

#define OVL_EL2_OFFSET_FLD_EL2_YOFF                            REG_FLD(13, 16)
#define OVL_EL2_OFFSET_FLD_EL2_XOFF                            REG_FLD(13, 0)

#define OVL_EL2_PITCH_FLD_SURFL_EN                             REG_FLD(1, 31)
#define OVL_EL2_PITCH_FLD_EL2_BLEND_RND_SHT                    REG_FLD(1, 30)
#define OVL_EL2_PITCH_FLD_EL2_SRGB_SEL_EXT2                    REG_FLD(1, 29)
#define OVL_EL2_PITCH_FLD_EL2_CONST_BLD                        REG_FLD(1, 28)
#define OVL_EL2_PITCH_FLD_EL2_DRGB_SEL_EXT                     REG_FLD(1, 27)
#define OVL_EL2_PITCH_FLD_EL2_DA_SEL_EXT                       REG_FLD(1, 26)
#define OVL_EL2_PITCH_FLD_EL2_SRGB_SEL_EXT                     REG_FLD(1, 25)
#define OVL_EL2_PITCH_FLD_EL2_SA_SEL_EXT                       REG_FLD(1, 24)
#define OVL_EL2_PITCH_FLD_EL2_DRGB_SEL                         REG_FLD(2, 22)
#define OVL_EL2_PITCH_FLD_EL2_DA_SEL                           REG_FLD(2, 20)
#define OVL_EL2_PITCH_FLD_EL2_SRGB_SEL                         REG_FLD(2, 18)
#define OVL_EL2_PITCH_FLD_EL2_SA_SEL                           REG_FLD(2, 16)
#define OVL_EL2_PITCH_FLD_EL2_SRC_PITCH                        REG_FLD(16, 0)

#define OVL_EL2_TILE_FLD_TILE_HORI_BLOCK_NUM                   REG_FLD(8, 24)
#define OVL_EL2_TILE_FLD_TILE_EN                               REG_FLD(1, 21)
#define OVL_EL2_TILE_FLD_TILE_WIDTH_SEL                        REG_FLD(1, 20)
#define OVL_EL2_TILE_FLD_TILE_HEIGHT                           REG_FLD(20, 0)

#define OVL_EL2_CLIP_FLD_EL2_SRC_BOTTOM_CLIP                   REG_FLD(8, 24)
#define OVL_EL2_CLIP_FLD_EL2_SRC_TOP_CLIP                      REG_FLD(8, 16)
#define OVL_EL2_CLIP_FLD_EL2_SRC_RIGHT_CLIP                    REG_FLD(8, 8)
#define OVL_EL2_CLIP_FLD_EL2_SRC_LEFT_CLIP                     REG_FLD(8, 0)

#define OVL_EL0_CLR_FLD_ALPHA                                  REG_FLD(8, 24)
#define OVL_EL0_CLR_FLD_RED                                    REG_FLD(8, 16)
#define OVL_EL0_CLR_FLD_GREEN                                  REG_FLD(8, 8)
#define OVL_EL0_CLR_FLD_BLUE                                   REG_FLD(8, 0)

#define OVL_EL1_CLR_FLD_ALPHA                                  REG_FLD(8, 24)
#define OVL_EL1_CLR_FLD_RED                                    REG_FLD(8, 16)
#define OVL_EL1_CLR_FLD_GREEN                                  REG_FLD(8, 8)
#define OVL_EL1_CLR_FLD_BLUE                                   REG_FLD(8, 0)

#define OVL_EL2_CLR_FLD_ALPHA                                  REG_FLD(8, 24)
#define OVL_EL2_CLR_FLD_RED                                    REG_FLD(8, 16)
#define OVL_EL2_CLR_FLD_GREEN                                  REG_FLD(8, 8)
#define OVL_EL2_CLR_FLD_BLUE                                   REG_FLD(8, 0)

#define OVL_SBCH_FLD_L3_SBCH_CNST_EN                           REG_FLD(1, 29)
#define OVL_SBCH_FLD_L3_SBCH_TRANS_EN                          REG_FLD(1, 28)
#define OVL_SBCH_FLD_L2_SBCH_CNST_EN                           REG_FLD(1, 25)
#define OVL_SBCH_FLD_L2_SBCH_TRANS_EN                          REG_FLD(1, 24)
#define OVL_SBCH_FLD_L1_SBCH_CNST_EN                           REG_FLD(1, 21)
#define OVL_SBCH_FLD_L1_SBCH_TRANS_EN                          REG_FLD(1, 20)
#define OVL_SBCH_FLD_L0_SBCH_CNST_EN                           REG_FLD(1, 17)
#define OVL_SBCH_FLD_L0_SBCH_TRANS_EN                          REG_FLD(1, 16)
#define OVL_SBCH_FLD_L3_SBCH_UPDATE                            REG_FLD(1, 12)
#define OVL_SBCH_FLD_L2_SBCH_UPDATE                            REG_FLD(1, 8)
#define OVL_SBCH_FLD_L1_SBCH_UPDATE                            REG_FLD(1, 4)
#define OVL_SBCH_FLD_L0_SBCH_UPDATE                            REG_FLD(1, 0)

#define OVL_SBCH_EXT_FLD_EL2_SBCH_CNST_EN                      REG_FLD(1, 25)
#define OVL_SBCH_EXT_FLD_EL2_SBCH_TRANS_EN                     REG_FLD(1, 24)
#define OVL_SBCH_EXT_FLD_EL1_SBCH_CNST_EN                      REG_FLD(1, 21)
#define OVL_SBCH_EXT_FLD_EL1_SBCH_TRANS_EN                     REG_FLD(1, 20)
#define OVL_SBCH_EXT_FLD_EL0_SBCH_CNST_EN                      REG_FLD(1, 17)
#define OVL_SBCH_EXT_FLD_EL0_SBCH_TRANS_EN                     REG_FLD(1, 16)
#define OVL_SBCH_EXT_FLD_EL2_SBCH_UPDATE                       REG_FLD(1, 8)
#define OVL_SBCH_EXT_FLD_EL1_SBCH_UPDATE                       REG_FLD(1, 4)
#define OVL_SBCH_EXT_FLD_EL0_SBCH_UPDATE                       REG_FLD(1, 0)

#define OVL_SBCH_CON_FLD_EL2_SBCH_TRANS_INVALID                REG_FLD(1, 22)
#define OVL_SBCH_CON_FLD_EL1_SBCH_TRANS_INVALID                REG_FLD(1, 21)
#define OVL_SBCH_CON_FLD_EL0_SBCH_TRANS_INVALID                REG_FLD(1, 20)
#define OVL_SBCH_CON_FLD_L3_SBCH_TRANS_INVALID                 REG_FLD(1, 19)
#define OVL_SBCH_CON_FLD_L2_SBCH_TRANS_INVALID                 REG_FLD(1, 18)
#define OVL_SBCH_CON_FLD_L1_SBCH_TRANS_INVALID                 REG_FLD(1, 17)
#define OVL_SBCH_CON_FLD_L0_SBCH_TRANS_INVALID                 REG_FLD(1, 16)

#define OVL_L0_ADDR_FLD_L0_ADDR                                REG_FLD(32, 0)

#define OVL_L1_ADDR_FLD_L1_ADDR                                REG_FLD(32, 0)

#define OVL_L2_ADDR_FLD_L2_ADDR                                REG_FLD(32, 0)

#define OVL_L3_ADDR_FLD_L3_ADDR                                REG_FLD(32, 0)

#define OVL_EL0_ADDR_FLD_EL0_ADDR                              REG_FLD(32, 0)

#define OVL_EL1_ADDR_FLD_EL1_ADDR                              REG_FLD(32, 0)

#define OVL_EL2_ADDR_FLD_EL2_ADDR                              REG_FLD(32, 0)

#define OVL_SECURE_FLD_L3_SECURE                               REG_FLD(1, 3)
#define OVL_SECURE_FLD_L2_SECURE                               REG_FLD(1, 2)
#define OVL_SECURE_FLD_L1_SECURE                               REG_FLD(1, 1)
#define OVL_SECURE_FLD_L0_SECURE                               REG_FLD(1, 0)

#define OVL_STA_GET_RDMA3_IDLE(reg32)                  REG_FLD_GET(OVL_STA_FLD_RDMA3_IDLE, (reg32))
#define OVL_STA_GET_RDMA2_IDLE(reg32)                  REG_FLD_GET(OVL_STA_FLD_RDMA2_IDLE, (reg32))
#define OVL_STA_GET_RDMA1_IDLE(reg32)                  REG_FLD_GET(OVL_STA_FLD_RDMA1_IDLE, (reg32))
#define OVL_STA_GET_RDMA0_IDLE(reg32)                  REG_FLD_GET(OVL_STA_FLD_RDMA0_IDLE, (reg32))
#define OVL_STA_GET_OVL_RUN(reg32)                     REG_FLD_GET(OVL_STA_FLD_OVL_RUN, (reg32))

#define OVL_INTEN_GET_OVL_START_INTEN(reg32)                                            \
                REG_FLD_GET(OVL_INTEN_FLD_OVL_START_INTEN, (reg32))
#define OVL_INTEN_GET_ABNORMAL_SOF_INTEN(reg32)                                         \
                REG_FLD_GET(OVL_INTEN_FLD_ABNORMAL_SOF_INTEN, (reg32))
#define OVL_INTEN_GET_RDMA3_SMI_UNDERFLOW_INTEN(reg32)                                  \
                REG_FLD_GET(OVL_INTEN_FLD_RDMA3_SMI_UNDERFLOW_INTEN, (reg32))
#define OVL_INTEN_GET_RDMA2_SMI_UNDERFLOW_INTEN(reg32)                                  \
                REG_FLD_GET(OVL_INTEN_FLD_RDMA2_SMI_UNDERFLOW_INTEN, (reg32))
#define OVL_INTEN_GET_RDMA1_SMI_UNDERFLOW_INTEN(reg32)                                  \
                REG_FLD_GET(OVL_INTEN_FLD_RDMA1_SMI_UNDERFLOW_INTEN, (reg32))
#define OVL_INTEN_GET_RDMA0_SMI_UNDERFLOW_INTEN(reg32)                                  \
                REG_FLD_GET(OVL_INTEN_FLD_RDMA0_SMI_UNDERFLOW_INTEN, (reg32))
#define OVL_INTEN_GET_RDMA3_EOF_ABNORMAL_INTEN(reg32)                                   \
                REG_FLD_GET(OVL_INTEN_FLD_RDMA3_EOF_ABNORMAL_INTEN, (reg32))
#define OVL_INTEN_GET_RDMA2_EOF_ABNORMAL_INTEN(reg32)                                   \
                REG_FLD_GET(OVL_INTEN_FLD_RDMA2_EOF_ABNORMAL_INTEN, (reg32))
#define OVL_INTEN_GET_RDMA1_EOF_ABNORMAL_INTEN(reg32)                                   \
                REG_FLD_GET(OVL_INTEN_FLD_RDMA1_EOF_ABNORMAL_INTEN, (reg32))
#define OVL_INTEN_GET_RDMA0_EOF_ABNORMAL_INTEN(reg32)                                   \
                REG_FLD_GET(OVL_INTEN_FLD_RDMA0_EOF_ABNORMAL_INTEN, (reg32))
#define OVL_INTEN_GET_OVL_FME_HWRST_DONE_INTEN(reg32)                                   \
                REG_FLD_GET(OVL_INTEN_FLD_OVL_FME_HWRST_DONE_INTEN, (reg32))
#define OVL_INTEN_GET_OVL_FME_SWRST_DONE_INTEN(reg32)                                   \
                REG_FLD_GET(OVL_INTEN_FLD_OVL_FME_SWRST_DONE_INTEN, (reg32))
#define OVL_INTEN_GET_OVL_FME_UND_INTEN(reg32)                                          \
                REG_FLD_GET(OVL_INTEN_FLD_OVL_FME_UND_INTEN, (reg32))
#define OVL_INTEN_GET_OVL_FME_CPL_INTEN(reg32)                                          \
                REG_FLD_GET(OVL_INTEN_FLD_OVL_FME_CPL_INTEN, (reg32))
#define OVL_INTEN_GET_OVL_REG_CMT_INTEN(reg32)                                          \
                REG_FLD_GET(OVL_INTEN_FLD_OVL_REG_CMT_INTEN, (reg32))

#define OVL_INTSTA_GET_OVL_START_INTSTA(reg32)                                          \
                REG_FLD_GET(OVL_INTSTA_FLD_OVL_START_INTSTA, (reg32))
#define OVL_INTSTA_GET_ABNORMAL_SOF_INTSTA(reg32)                                       \
                REG_FLD_GET(OVL_INTSTA_FLD_ABNORMAL_SOF_INTSTA, (reg32))
#define OVL_INTSTA_GET_RDMA3_SMI_UNDERFLOW_INTSTA(reg32)                                \
                REG_FLD_GET(OVL_INTSTA_FLD_RDMA3_SMI_UNDERFLOW_INTSTA, (reg32))
#define OVL_INTSTA_GET_RDMA2_SMI_UNDERFLOW_INTSTA(reg32)                                \
                REG_FLD_GET(OVL_INTSTA_FLD_RDMA2_SMI_UNDERFLOW_INTSTA, (reg32))
#define OVL_INTSTA_GET_RDMA1_SMI_UNDERFLOW_INTSTA(reg32)                                \
                REG_FLD_GET(OVL_INTSTA_FLD_RDMA1_SMI_UNDERFLOW_INTSTA, (reg32))
#define OVL_INTSTA_GET_RDMA0_SMI_UNDERFLOW_INTSTA(reg32)                                \
                REG_FLD_GET(OVL_INTSTA_FLD_RDMA0_SMI_UNDERFLOW_INTSTA, (reg32))
#define OVL_INTSTA_GET_RDMA3_EOF_ABNORMAL_INTSTA(reg32)                                 \
                REG_FLD_GET(OVL_INTSTA_FLD_RDMA3_EOF_ABNORMAL_INTSTA, (reg32))
#define OVL_INTSTA_GET_RDMA2_EOF_ABNORMAL_INTSTA(reg32)                                 \
                REG_FLD_GET(OVL_INTSTA_FLD_RDMA2_EOF_ABNORMAL_INTSTA, (reg32))
#define OVL_INTSTA_GET_RDMA1_EOF_ABNORMAL_INTSTA(reg32)                                 \
                REG_FLD_GET(OVL_INTSTA_FLD_RDMA1_EOF_ABNORMAL_INTSTA, (reg32))
#define OVL_INTSTA_GET_RDMA0_EOF_ABNORMAL_INTSTA(reg32)                                 \
                REG_FLD_GET(OVL_INTSTA_FLD_RDMA0_EOF_ABNORMAL_INTSTA, (reg32))
#define OVL_INTSTA_GET_OVL_FME_HWRST_DONE_INTSTA(reg32)                                 \
                REG_FLD_GET(OVL_INTSTA_FLD_OVL_FME_HWRST_DONE_INTSTA, (reg32))
#define OVL_INTSTA_GET_OVL_FME_SWRST_DONE_INTSTA(reg32)                                 \
                REG_FLD_GET(OVL_INTSTA_FLD_OVL_FME_SWRST_DONE_INTSTA, (reg32))
#define OVL_INTSTA_GET_OVL_FME_UND_INTSTA(reg32)                                        \
                REG_FLD_GET(OVL_INTSTA_FLD_OVL_FME_UND_INTSTA, (reg32))
#define OVL_INTSTA_GET_OVL_FME_CPL_INTSTA(reg32)                                        \
                REG_FLD_GET(OVL_INTSTA_FLD_OVL_FME_CPL_INTSTA, (reg32))
#define OVL_INTSTA_GET_OVL_REG_CMT_INTSTA(reg32)                                        \
                REG_FLD_GET(OVL_INTSTA_FLD_OVL_REG_CMT_INTSTA, (reg32))

#define OVL_EN_GET_BLOCK_EXT_PREULTRA(reg32)   REG_FLD_GET(OVL_EN_FLD_BLOCK_EXT_PREULTRA, (reg32))
#define OVL_EN_GET_BLOCK_EXT_ULTRA(reg32)      REG_FLD_GET(OVL_EN_FLD_BLOCK_EXT_ULTRA, (reg32))
#define OVL_EN_GET_IGNORE_ABNORMAL_SOF(reg32)  REG_FLD_GET(OVL_EN_FLD_IGNORE_ABNORMAL_SOF, (reg32))
#define OVL_EN_GET_HF_FOVL_CK_ON(reg32)        REG_FLD_GET(OVL_EN_FLD_HF_FOVL_CK_ON, (reg32))
#define OVL_EN_GET_HG_FSMI_CK_ON(reg32)        REG_FLD_GET(OVL_EN_FLD_HG_FSMI_CK_ON, (reg32))
#define OVL_EN_GET_HG_FOVL_CK_ON(reg32)        REG_FLD_GET(OVL_EN_FLD_HG_FOVL_CK_ON, (reg32))
#define OVL_EN_GET_OVL_EN(reg32)               REG_FLD_GET(OVL_EN_FLD_OVL_EN, (reg32))

#define OVL_TRIG_GET_CRC_CLR(reg32)            REG_FLD_GET(OVL_TRIG_FLD_CRC_CLR, (reg32))
#define OVL_TRIG_GET_CRC_EN(reg32)             REG_FLD_GET(OVL_TRIG_FLD_CRC_EN, (reg32))
#define OVL_TRIG_GET_OVL_SW_TRIG(reg32)        REG_FLD_GET(OVL_TRIG_FLD_OVL_SW_TRIG, (reg32))

#define OVL_RST_GET_OVL_SMI_IOBUF_HARD_RST(reg32)                                       \
                REG_FLD_GET(OVL_RST_FLD_OVL_SMI_IOBUF_HARD_RST, (reg32))
#define OVL_RST_GET_OVL_SMI_IOBUF_RST(reg32)   REG_FLD_GET(OVL_RST_FLD_OVL_SMI_IOBUF_RST, (reg32))
#define OVL_RST_GET_OVL_SMI_HARD_RST(reg32)    REG_FLD_GET(OVL_RST_FLD_OVL_SMI_HARD_RST, (reg32))
#define OVL_RST_GET_OVL_SMI_RST(reg32)         REG_FLD_GET(OVL_RST_FLD_OVL_SMI_RST, (reg32))
#define OVL_RST_GET_OVL_RST(reg32)             REG_FLD_GET(OVL_RST_FLD_OVL_RST, (reg32))

#define OVL_ROI_SIZE_GET_ROI_H(reg32)          REG_FLD_GET(OVL_ROI_SIZE_FLD_ROI_H, (reg32))
#define OVL_ROI_SIZE_GET_ROI_W(reg32)          REG_FLD_GET(OVL_ROI_SIZE_FLD_ROI_W, (reg32))

#define OVL_DATAPATH_CON_GET_WIDE_GAMUT_EN(reg32)                                       \
                REG_FLD_GET(OVL_DATAPATH_CON_FLD_WIDE_GAMUT_EN, (reg32))
#define OVL_DATAPATH_CON_GET_OUTPUT_INTERLACE(reg32)                                    \
                REG_FLD_GET(OVL_DATAPATH_CON_FLD_OUTPUT_INTERLACE, (reg32))
#define OVL_DATAPATH_CON_GET_OUTPUT_CLAMP(reg32)                                        \
                REG_FLD_GET(OVL_DATAPATH_CON_FLD_OUTPUT_CLAMP, (reg32))
#define OVL_DATAPATH_CON_GET_GCLAST_EN(reg32)                                           \
                REG_FLD_GET(OVL_DATAPATH_CON_FLD_GCLAST_EN, (reg32))
#define OVL_DATAPATH_CON_GET_RDMA3_OUT_SEL(reg32)                                       \
                REG_FLD_GET(OVL_DATAPATH_CON_FLD_RDMA3_OUT_SEL, (reg32))
#define OVL_DATAPATH_CON_GET_RDMA2_OUT_SEL(reg32)                                       \
                REG_FLD_GET(OVL_DATAPATH_CON_FLD_RDMA2_OUT_SEL, (reg32))
#define OVL_DATAPATH_CON_GET_RDMA1_OUT_SEL(reg32)                                       \
                REG_FLD_GET(OVL_DATAPATH_CON_FLD_RDMA1_OUT_SEL, (reg32))
#define OVL_DATAPATH_CON_GET_RDMA0_OUT_SEL(reg32)                                       \
                REG_FLD_GET(OVL_DATAPATH_CON_FLD_RDMA0_OUT_SEL, (reg32))
#define OVL_DATAPATH_CON_GET_PQ_OUT_SEL(reg32)                                          \
                REG_FLD_GET(OVL_DATAPATH_CON_FLD_PQ_OUT_SEL, (reg32))
#define OVL_DATAPATH_CON_GET_OVL_GAMMA_OUT(reg32)                                       \
                REG_FLD_GET(OVL_DATAPATH_CON_FLD_OVL_GAMMA_OUT, (reg32))
#define OVL_DATAPATH_CON_GET_ADOBE_LAYER(reg32)                                         \
                REG_FLD_GET(OVL_DATAPATH_CON_FLD_ADOBE_LAYER, (reg32))
#define OVL_DATAPATH_CON_GET_ADOBE_MODE(reg32)                                          \
                REG_FLD_GET(OVL_DATAPATH_CON_FLD_ADOBE_MODE, (reg32))
#define OVL_DATAPATH_CON_GET_L3_GPU_MODE(reg32)                                         \
                REG_FLD_GET(OVL_DATAPATH_CON_FLD_L3_GPU_MODE, (reg32))
#define OVL_DATAPATH_CON_GET_L2_GPU_MODE(reg32)                                         \
                REG_FLD_GET(OVL_DATAPATH_CON_FLD_L2_GPU_MODE, (reg32))
#define OVL_DATAPATH_CON_GET_L1_GPU_MODE(reg32)                                         \
                REG_FLD_GET(OVL_DATAPATH_CON_FLD_L1_GPU_MODE, (reg32))
#define OVL_DATAPATH_CON_GET_L0_GPU_MODE(reg32)                                         \
                REG_FLD_GET(OVL_DATAPATH_CON_FLD_L0_GPU_MODE, (reg32))
#define OVL_DATAPATH_CON_GET_OUTPUT_NO_RND(reg32)                                       \
                REG_FLD_GET(OVL_DATAPATH_CON_FLD_OUTPUT_NO_RND, (reg32))
#define OVL_DATAPATH_CON_GET_BGCLR_IN_SEL(reg32)                                        \
                REG_FLD_GET(OVL_DATAPATH_CON_FLD_BGCLR_IN_SEL, (reg32))
#define OVL_DATAPATH_CON_GET_OVL_RANDOM_BGCLR_EN(reg32)                                 \
                REG_FLD_GET(OVL_DATAPATH_CON_FLD_OVL_RANDOM_BGCLR_EN, (reg32))
#define OVL_DATAPATH_CON_GET_LAYER_SMI_ID_EN(reg32)                                     \
                REG_FLD_GET(OVL_DATAPATH_CON_FLD_LAYER_SMI_ID_EN, (reg32))

#define OVL_ROI_BGCLR_GET_ALPHA(reg32)            REG_FLD_GET(OVL_ROI_BGCLR_FLD_ALPHA, (reg32))
#define OVL_ROI_BGCLR_GET_RED(reg32)              REG_FLD_GET(OVL_ROI_BGCLR_FLD_RED, (reg32))
#define OVL_ROI_BGCLR_GET_GREEN(reg32)            REG_FLD_GET(OVL_ROI_BGCLR_FLD_GREEN, (reg32))
#define OVL_ROI_BGCLR_GET_BLUE(reg32)             REG_FLD_GET(OVL_ROI_BGCLR_FLD_BLUE, (reg32))

#define OVL_SRC_CON_GET_RELAY_MODE_EN(reg32)    REG_FLD_GET(OVL_SRC_CON_FLD_RELAY_MODE_EN, (reg32))
#define OVL_SRC_CON_GET_FORCE_RELAY_MODE(reg32)                                           \
                REG_FLD_GET(OVL_SRC_CON_FLD_FORCE_RELAY_MODE, (reg32))
#define OVL_SRC_CON_GET_LC_EN(reg32)            REG_FLD_GET(OVL_SRC_CON_FLD_LC_EN, (reg32))
#define OVL_SRC_CON_GET_L3_EN(reg32)            REG_FLD_GET(OVL_SRC_CON_FLD_L3_EN, (reg32))
#define OVL_SRC_CON_GET_L2_EN(reg32)            REG_FLD_GET(OVL_SRC_CON_FLD_L2_EN, (reg32))
#define OVL_SRC_CON_GET_L1_EN(reg32)            REG_FLD_GET(OVL_SRC_CON_FLD_L1_EN, (reg32))
#define OVL_SRC_CON_GET_L0_EN(reg32)            REG_FLD_GET(OVL_SRC_CON_FLD_L0_EN, (reg32))

#define OVL_L0_CON_GET_DSTKEY_EN(reg32)         REG_FLD_GET(OVL_L0_CON_FLD_DSTKEY_EN, (reg32))
#define OVL_L0_CON_GET_SRCKEY_EN(reg32)         REG_FLD_GET(OVL_L0_CON_FLD_SRCKEY_EN, (reg32))
#define OVL_L0_CON_GET_LAYER_SRC(reg32)         REG_FLD_GET(OVL_L0_CON_FLD_LAYER_SRC, (reg32))
#define OVL_L0_CON_GET_MTX_EN(reg32)            REG_FLD_GET(OVL_L0_CON_FLD_MTX_EN, (reg32))
#define OVL_L0_CON_GET_MTX_AUTO_DIS(reg32)      REG_FLD_GET(OVL_L0_CON_FLD_MTX_AUTO_DIS, (reg32))
#define OVL_L0_CON_GET_RGB_SWAP(reg32)          REG_FLD_GET(OVL_L0_CON_FLD_RGB_SWAP, (reg32))
#define OVL_L0_CON_GET_BYTE_SWAP(reg32)         REG_FLD_GET(OVL_L0_CON_FLD_BYTE_SWAP, (reg32))
#define OVL_L0_CON_GET_CLRFMT_MAN(reg32)        REG_FLD_GET(OVL_L0_CON_FLD_CLRFMT_MAN, (reg32))
#define OVL_L0_CON_GET_R_FIRST(reg32)           REG_FLD_GET(OVL_L0_CON_FLD_R_FIRST, (reg32))
#define OVL_L0_CON_GET_LANDSCAPE(reg32)         REG_FLD_GET(OVL_L0_CON_FLD_LANDSCAPE, (reg32))
#define OVL_L0_CON_GET_EN_3D(reg32)             REG_FLD_GET(OVL_L0_CON_FLD_EN_3D, (reg32))
#define OVL_L0_CON_GET_INT_MTX_SEL(reg32)       REG_FLD_GET(OVL_L0_CON_FLD_INT_MTX_SEL, (reg32))
#define OVL_L0_CON_GET_CLRFMT(reg32)            REG_FLD_GET(OVL_L0_CON_FLD_CLRFMT, (reg32))
#define OVL_L0_CON_GET_EXT_MTX_EN(reg32)        REG_FLD_GET(OVL_L0_CON_FLD_EXT_MTX_EN, (reg32))
#define OVL_L0_CON_GET_HORIZONTAL_FLIP_EN(reg32)                                        \
                REG_FLD_GET(OVL_L0_CON_FLD_HORIZONTAL_FLIP_EN, (reg32))
#define OVL_L0_CON_GET_VERTICAL_FLIP_EN(reg32)                                          \
                REG_FLD_GET(OVL_L0_CON_FLD_VERTICAL_FLIP_EN, (reg32))
#define OVL_L0_CON_GET_ALPHA_EN(reg32)          REG_FLD_GET(OVL_L0_CON_FLD_ALPHA_EN, (reg32))
#define OVL_L0_CON_GET_ALPHA(reg32)             REG_FLD_GET(OVL_L0_CON_FLD_ALPHA, (reg32))

#define OVL_L0_SRCKEY_GET_SRCKEY(reg32)         REG_FLD_GET(OVL_L0_SRCKEY_FLD_SRCKEY, (reg32))

#define OVL_L0_SRC_SIZE_GET_L0_SRC_H(reg32)     REG_FLD_GET(OVL_L0_SRC_SIZE_FLD_L0_SRC_H, (reg32))
#define OVL_L0_SRC_SIZE_GET_L0_SRC_W(reg32)     REG_FLD_GET(OVL_L0_SRC_SIZE_FLD_L0_SRC_W, (reg32))

#define OVL_L0_OFFSET_GET_L0_YOFF(reg32)        REG_FLD_GET(OVL_L0_OFFSET_FLD_L0_YOFF, (reg32))
#define OVL_L0_OFFSET_GET_L0_XOFF(reg32)        REG_FLD_GET(OVL_L0_OFFSET_FLD_L0_XOFF, (reg32))

#define OVL_L0_PITCH_GET_SURFL_EN(reg32)                                            \
                REG_FLD_GET(OVL_L0_PITCH_FLD_SURFL_EN, (reg32))
#define OVL_L0_PITCH_GET_L0_BLEND_RND_SHT(reg32)                                    \
                REG_FLD_GET(OVL_L0_PITCH_FLD_L0_BLEND_RND_SHT, (reg32))
#define OVL_L0_PITCH_GET_L0_SRGB_SEL_EXT2(reg32)                                    \
                REG_FLD_GET(OVL_L0_PITCH_FLD_L0_SRGB_SEL_EXT2, (reg32))
#define OVL_L0_PITCH_GET_L0_CONST_BLD(reg32)                                        \
                REG_FLD_GET(OVL_L0_PITCH_FLD_L0_CONST_BLD, (reg32))
#define OVL_L0_PITCH_GET_L0_DRGB_SEL_EXT(reg32)                                     \
                REG_FLD_GET(OVL_L0_PITCH_FLD_L0_DRGB_SEL_EXT, (reg32))
#define OVL_L0_PITCH_GET_L0_DA_SEL_EXT(reg32)                                       \
                REG_FLD_GET(OVL_L0_PITCH_FLD_L0_DA_SEL_EXT, (reg32))
#define OVL_L0_PITCH_GET_L0_SRGB_SEL_EXT(reg32)                                     \
                REG_FLD_GET(OVL_L0_PITCH_FLD_L0_SRGB_SEL_EXT, (reg32))
#define OVL_L0_PITCH_GET_L0_SA_SEL_EXT(reg32)                                       \
                REG_FLD_GET(OVL_L0_PITCH_FLD_L0_SA_SEL_EXT, (reg32))
#define OVL_L0_PITCH_GET_L0_DRGB_SEL(reg32)                                         \
                REG_FLD_GET(OVL_L0_PITCH_FLD_L0_DRGB_SEL, (reg32))
#define OVL_L0_PITCH_GET_L0_DA_SEL(reg32)                                           \
                REG_FLD_GET(OVL_L0_PITCH_FLD_L0_DA_SEL, (reg32))
#define OVL_L0_PITCH_GET_L0_SRGB_SEL(reg32)                                         \
                REG_FLD_GET(OVL_L0_PITCH_FLD_L0_SRGB_SEL, (reg32))
#define OVL_L0_PITCH_GET_L0_SA_SEL(reg32)                                           \
                REG_FLD_GET(OVL_L0_PITCH_FLD_L0_SA_SEL, (reg32))
#define OVL_L0_PITCH_GET_L0_SRC_PITCH(reg32)                                        \
                REG_FLD_GET(OVL_L0_PITCH_FLD_L0_SRC_PITCH, (reg32))

#define OVL_L0_TILE_GET_TILE_HORI_BLOCK_NUM(reg32)                                  \
                REG_FLD_GET(OVL_L0_TILE_FLD_TILE_HORI_BLOCK_NUM, (reg32))
#define OVL_L0_TILE_GET_TILE_EN(reg32)                                              \
                REG_FLD_GET(OVL_L0_TILE_FLD_TILE_EN, (reg32))
#define OVL_L0_TILE_GET_TILE_WIDTH_SEL(reg32)                                       \
                REG_FLD_GET(OVL_L0_TILE_FLD_TILE_WIDTH_SEL, (reg32))
#define OVL_L0_TILE_GET_TILE_HEIGHT(reg32)                                          \
                REG_FLD_GET(OVL_L0_TILE_FLD_TILE_HEIGHT, (reg32))

#define OVL_L0_CLIP_GET_L0_SRC_BOTTOM_CLIP(reg32)                                   \
                REG_FLD_GET(OVL_L0_CLIP_FLD_L0_SRC_BOTTOM_CLIP, (reg32))
#define OVL_L0_CLIP_GET_L0_SRC_TOP_CLIP(reg32)                                      \
                REG_FLD_GET(OVL_L0_CLIP_FLD_L0_SRC_TOP_CLIP, (reg32))
#define OVL_L0_CLIP_GET_L0_SRC_RIGHT_CLIP(reg32)                                    \
                REG_FLD_GET(OVL_L0_CLIP_FLD_L0_SRC_RIGHT_CLIP, (reg32))
#define OVL_L0_CLIP_GET_L0_SRC_LEFT_CLIP(reg32)                                     \
                REG_FLD_GET(OVL_L0_CLIP_FLD_L0_SRC_LEFT_CLIP, (reg32))

#define OVL_L1_CON_GET_DSTKEY_EN(reg32)         REG_FLD_GET(OVL_L1_CON_FLD_DSTKEY_EN, (reg32))
#define OVL_L1_CON_GET_SRCKEY_EN(reg32)         REG_FLD_GET(OVL_L1_CON_FLD_SRCKEY_EN, (reg32))
#define OVL_L1_CON_GET_LAYER_SRC(reg32)         REG_FLD_GET(OVL_L1_CON_FLD_LAYER_SRC, (reg32))
#define OVL_L1_CON_GET_MTX_EN(reg32)            REG_FLD_GET(OVL_L1_CON_FLD_MTX_EN, (reg32))
#define OVL_L1_CON_GET_MTX_AUTO_DIS(reg32)      REG_FLD_GET(OVL_L1_CON_FLD_MTX_AUTO_DIS, (reg32))
#define OVL_L1_CON_GET_RGB_SWAP(reg32)          REG_FLD_GET(OVL_L1_CON_FLD_RGB_SWAP, (reg32))
#define OVL_L1_CON_GET_BYTE_SWAP(reg32)         REG_FLD_GET(OVL_L1_CON_FLD_BYTE_SWAP, (reg32))
#define OVL_L1_CON_GET_CLRFMT_MAN(reg32)        REG_FLD_GET(OVL_L1_CON_FLD_CLRFMT_MAN, (reg32))
#define OVL_L1_CON_GET_R_FIRST(reg32)           REG_FLD_GET(OVL_L1_CON_FLD_R_FIRST, (reg32))
#define OVL_L1_CON_GET_LANDSCAPE(reg32)         REG_FLD_GET(OVL_L1_CON_FLD_LANDSCAPE, (reg32))
#define OVL_L1_CON_GET_EN_3D(reg32)             REG_FLD_GET(OVL_L1_CON_FLD_EN_3D, (reg32))
#define OVL_L1_CON_GET_INT_MTX_SEL(reg32)       REG_FLD_GET(OVL_L1_CON_FLD_INT_MTX_SEL, (reg32))
#define OVL_L1_CON_GET_CLRFMT(reg32)            REG_FLD_GET(OVL_L1_CON_FLD_CLRFMT, (reg32))
#define OVL_L1_CON_GET_EXT_MTX_EN(reg32)        REG_FLD_GET(OVL_L1_CON_FLD_EXT_MTX_EN, (reg32))
#define OVL_L1_CON_GET_HORIZONTAL_FLIP_EN(reg32)                              \
                REG_FLD_GET(OVL_L1_CON_FLD_HORIZONTAL_FLIP_EN, (reg32))
#define OVL_L1_CON_GET_VERTICAL_FLIP_EN(reg32)                                \
                REG_FLD_GET(OVL_L1_CON_FLD_VERTICAL_FLIP_EN, (reg32))
#define OVL_L1_CON_GET_ALPHA_EN(reg32)          REG_FLD_GET(OVL_L1_CON_FLD_ALPHA_EN, (reg32))
#define OVL_L1_CON_GET_ALPHA(reg32)             REG_FLD_GET(OVL_L1_CON_FLD_ALPHA, (reg32))

#define OVL_L1_SRCKEY_GET_SRCKEY(reg32)         REG_FLD_GET(OVL_L1_SRCKEY_FLD_SRCKEY, (reg32))

#define OVL_L1_SRC_SIZE_GET_L1_SRC_H(reg32)                                   \
                REG_FLD_GET(OVL_L1_SRC_SIZE_FLD_L1_SRC_H, (reg32))
#define OVL_L1_SRC_SIZE_GET_L1_SRC_W(reg32)                                   \
                REG_FLD_GET(OVL_L1_SRC_SIZE_FLD_L1_SRC_W, (reg32))

#define OVL_L1_OFFSET_GET_L1_YOFF(reg32)        REG_FLD_GET(OVL_L1_OFFSET_FLD_L1_YOFF, (reg32))
#define OVL_L1_OFFSET_GET_L1_XOFF(reg32)        REG_FLD_GET(OVL_L1_OFFSET_FLD_L1_XOFF, (reg32))

#define OVL_L1_PITCH_GET_SURFL_EN(reg32)                                       \
                REG_FLD_GET(OVL_L1_PITCH_FLD_SURFL_EN, (reg32))
#define OVL_L1_PITCH_GET_L1_BLEND_RND_SHT(reg32)                               \
                REG_FLD_GET(OVL_L1_PITCH_FLD_L1_BLEND_RND_SHT, (reg32))
#define OVL_L1_PITCH_GET_L1_SRGB_SEL_EXT2(reg32)                               \
                REG_FLD_GET(OVL_L1_PITCH_FLD_L1_SRGB_SEL_EXT2, (reg32))
#define OVL_L1_PITCH_GET_L1_CONST_BLD(reg32)                                   \
                REG_FLD_GET(OVL_L1_PITCH_FLD_L1_CONST_BLD, (reg32))
#define OVL_L1_PITCH_GET_L1_DRGB_SEL_EXT(reg32)                                \
                REG_FLD_GET(OVL_L1_PITCH_FLD_L1_DRGB_SEL_EXT, (reg32))
#define OVL_L1_PITCH_GET_L1_DA_SEL_EXT(reg32)                                  \
                REG_FLD_GET(OVL_L1_PITCH_FLD_L1_DA_SEL_EXT, (reg32))
#define OVL_L1_PITCH_GET_L1_SRGB_SEL_EXT(reg32)                                \
                REG_FLD_GET(OVL_L1_PITCH_FLD_L1_SRGB_SEL_EXT, (reg32))
#define OVL_L1_PITCH_GET_L1_SA_SEL_EXT(reg32)                                  \
                REG_FLD_GET(OVL_L1_PITCH_FLD_L1_SA_SEL_EXT, (reg32))
#define OVL_L1_PITCH_GET_L1_DRGB_SEL(reg32)                                    \
                REG_FLD_GET(OVL_L1_PITCH_FLD_L1_DRGB_SEL, (reg32))
#define OVL_L1_PITCH_GET_L1_DA_SEL(reg32)                                      \
                REG_FLD_GET(OVL_L1_PITCH_FLD_L1_DA_SEL, (reg32))
#define OVL_L1_PITCH_GET_L1_SRGB_SEL(reg32)                                    \
                REG_FLD_GET(OVL_L1_PITCH_FLD_L1_SRGB_SEL, (reg32))
#define OVL_L1_PITCH_GET_L1_SA_SEL(reg32)                                      \
                REG_FLD_GET(OVL_L1_PITCH_FLD_L1_SA_SEL, (reg32))
#define OVL_L1_PITCH_GET_L1_SRC_PITCH(reg32)                                   \
                REG_FLD_GET(OVL_L1_PITCH_FLD_L1_SRC_PITCH, (reg32))

#define OVL_L1_TILE_GET_TILE_HORI_BLOCK_NUM(reg32)                             \
                REG_FLD_GET(OVL_L1_TILE_FLD_TILE_HORI_BLOCK_NUM, (reg32))
#define OVL_L1_TILE_GET_TILE_EN(reg32)          REG_FLD_GET(OVL_L1_TILE_FLD_TILE_EN, (reg32))
#define OVL_L1_TILE_GET_TILE_WIDTH_SEL(reg32)   REG_FLD_GET(OVL_L1_TILE_FLD_TILE_WIDTH_SEL, (reg32))
#define OVL_L1_TILE_GET_TILE_HEIGHT(reg32)      REG_FLD_GET(OVL_L1_TILE_FLD_TILE_HEIGHT, (reg32))

#define OVL_L1_CLIP_GET_L1_SRC_BOTTOM_CLIP(reg32)                              \
                REG_FLD_GET(OVL_L1_CLIP_FLD_L1_SRC_BOTTOM_CLIP, (reg32))
#define OVL_L1_CLIP_GET_L1_SRC_TOP_CLIP(reg32)                                 \
                REG_FLD_GET(OVL_L1_CLIP_FLD_L1_SRC_TOP_CLIP, (reg32))
#define OVL_L1_CLIP_GET_L1_SRC_RIGHT_CLIP(reg32)                               \
                REG_FLD_GET(OVL_L1_CLIP_FLD_L1_SRC_RIGHT_CLIP, (reg32))
#define OVL_L1_CLIP_GET_L1_SRC_LEFT_CLIP(reg32)                                \
                REG_FLD_GET(OVL_L1_CLIP_FLD_L1_SRC_LEFT_CLIP, (reg32))

#define OVL_L2_CON_GET_DSTKEY_EN(reg32)         REG_FLD_GET(OVL_L2_CON_FLD_DSTKEY_EN, (reg32))
#define OVL_L2_CON_GET_SRCKEY_EN(reg32)         REG_FLD_GET(OVL_L2_CON_FLD_SRCKEY_EN, (reg32))
#define OVL_L2_CON_GET_LAYER_SRC(reg32)         REG_FLD_GET(OVL_L2_CON_FLD_LAYER_SRC, (reg32))
#define OVL_L2_CON_GET_MTX_EN(reg32)            REG_FLD_GET(OVL_L2_CON_FLD_MTX_EN, (reg32))
#define OVL_L2_CON_GET_MTX_AUTO_DIS(reg32)      REG_FLD_GET(OVL_L2_CON_FLD_MTX_AUTO_DIS, (reg32))
#define OVL_L2_CON_GET_RGB_SWAP(reg32)          REG_FLD_GET(OVL_L2_CON_FLD_RGB_SWAP, (reg32))
#define OVL_L2_CON_GET_BYTE_SWAP(reg32)         REG_FLD_GET(OVL_L2_CON_FLD_BYTE_SWAP, (reg32))
#define OVL_L2_CON_GET_CLRFMT_MAN(reg32)        REG_FLD_GET(OVL_L2_CON_FLD_CLRFMT_MAN, (reg32))
#define OVL_L2_CON_GET_R_FIRST(reg32)           REG_FLD_GET(OVL_L2_CON_FLD_R_FIRST, (reg32))
#define OVL_L2_CON_GET_LANDSCAPE(reg32)         REG_FLD_GET(OVL_L2_CON_FLD_LANDSCAPE, (reg32))
#define OVL_L2_CON_GET_EN_3D(reg32)             REG_FLD_GET(OVL_L2_CON_FLD_EN_3D, (reg32))
#define OVL_L2_CON_GET_INT_MTX_SEL(reg32)       REG_FLD_GET(OVL_L2_CON_FLD_INT_MTX_SEL, (reg32))
#define OVL_L2_CON_GET_CLRFMT(reg32)            REG_FLD_GET(OVL_L2_CON_FLD_CLRFMT, (reg32))
#define OVL_L2_CON_GET_EXT_MTX_EN(reg32)        REG_FLD_GET(OVL_L2_CON_FLD_EXT_MTX_EN, (reg32))
#define OVL_L2_CON_GET_HORIZONTAL_FLIP_EN(reg32)                               \
                REG_FLD_GET(OVL_L2_CON_FLD_HORIZONTAL_FLIP_EN, (reg32))
#define OVL_L2_CON_GET_VERTICAL_FLIP_EN(reg32)                                 \
                REG_FLD_GET(OVL_L2_CON_FLD_VERTICAL_FLIP_EN, (reg32))
#define OVL_L2_CON_GET_ALPHA_EN(reg32)          REG_FLD_GET(OVL_L2_CON_FLD_ALPHA_EN, (reg32))
#define OVL_L2_CON_GET_ALPHA(reg32)             REG_FLD_GET(OVL_L2_CON_FLD_ALPHA, (reg32))

#define OVL_L2_SRCKEY_GET_SRCKEY(reg32)         REG_FLD_GET(OVL_L2_SRCKEY_FLD_SRCKEY, (reg32))

#define OVL_L2_SRC_SIZE_GET_L2_SRC_H(reg32)     REG_FLD_GET(OVL_L2_SRC_SIZE_FLD_L2_SRC_H, (reg32))
#define OVL_L2_SRC_SIZE_GET_L2_SRC_W(reg32)     REG_FLD_GET(OVL_L2_SRC_SIZE_FLD_L2_SRC_W, (reg32))

#define OVL_L2_OFFSET_GET_L2_YOFF(reg32)        REG_FLD_GET(OVL_L2_OFFSET_FLD_L2_YOFF, (reg32))
#define OVL_L2_OFFSET_GET_L2_XOFF(reg32)        REG_FLD_GET(OVL_L2_OFFSET_FLD_L2_XOFF, (reg32))

#define OVL_L2_PITCH_GET_SURFL_EN(reg32)                                       \
                REG_FLD_GET(OVL_L2_PITCH_FLD_SURFL_EN, (reg32))
#define OVL_L2_PITCH_GET_L2_BLEND_RND_SHT(reg32)                               \
                REG_FLD_GET(OVL_L2_PITCH_FLD_L2_BLEND_RND_SHT, (reg32))
#define OVL_L2_PITCH_GET_L2_SRGB_SEL_EXT2(reg32)                               \
                REG_FLD_GET(OVL_L2_PITCH_FLD_L2_SRGB_SEL_EXT2, (reg32))
#define OVL_L2_PITCH_GET_L2_CONST_BLD(reg32)                                   \
                REG_FLD_GET(OVL_L2_PITCH_FLD_L2_CONST_BLD, (reg32))
#define OVL_L2_PITCH_GET_L2_DRGB_SEL_EXT(reg32)                                \
                REG_FLD_GET(OVL_L2_PITCH_FLD_L2_DRGB_SEL_EXT, (reg32))
#define OVL_L2_PITCH_GET_L2_DA_SEL_EXT(reg32)                                  \
                REG_FLD_GET(OVL_L2_PITCH_FLD_L2_DA_SEL_EXT, (reg32))
#define OVL_L2_PITCH_GET_L2_SRGB_SEL_EXT(reg32)                                \
                REG_FLD_GET(OVL_L2_PITCH_FLD_L2_SRGB_SEL_EXT, (reg32))
#define OVL_L2_PITCH_GET_L2_SA_SEL_EXT(reg32)                                  \
                REG_FLD_GET(OVL_L2_PITCH_FLD_L2_SA_SEL_EXT, (reg32))
#define OVL_L2_PITCH_GET_L2_DRGB_SEL(reg32)                                    \
                REG_FLD_GET(OVL_L2_PITCH_FLD_L2_DRGB_SEL, (reg32))
#define OVL_L2_PITCH_GET_L2_DA_SEL(reg32)                                      \
                REG_FLD_GET(OVL_L2_PITCH_FLD_L2_DA_SEL, (reg32))
#define OVL_L2_PITCH_GET_L2_SRGB_SEL(reg32)                                    \
                REG_FLD_GET(OVL_L2_PITCH_FLD_L2_SRGB_SEL, (reg32))
#define OVL_L2_PITCH_GET_L2_SA_SEL(reg32)                                      \
                REG_FLD_GET(OVL_L2_PITCH_FLD_L2_SA_SEL, (reg32))
#define OVL_L2_PITCH_GET_L2_SRC_PITCH(reg32)                                   \
                REG_FLD_GET(OVL_L2_PITCH_FLD_L2_SRC_PITCH, (reg32))

#define OVL_L2_TILE_GET_TILE_HORI_BLOCK_NUM(reg32)                             \
                REG_FLD_GET(OVL_L2_TILE_FLD_TILE_HORI_BLOCK_NUM, (reg32))
#define OVL_L2_TILE_GET_TILE_EN(reg32)                                         \
                REG_FLD_GET(OVL_L2_TILE_FLD_TILE_EN, (reg32))
#define OVL_L2_TILE_GET_TILE_WIDTH_SEL(reg32)                                  \
                REG_FLD_GET(OVL_L2_TILE_FLD_TILE_WIDTH_SEL, (reg32))
#define OVL_L2_TILE_GET_TILE_HEIGHT(reg32)                                     \
                REG_FLD_GET(OVL_L2_TILE_FLD_TILE_HEIGHT, (reg32))

#define OVL_L2_CLIP_GET_L2_SRC_BOTTOM_CLIP(reg32)                              \
                REG_FLD_GET(OVL_L2_CLIP_FLD_L2_SRC_BOTTOM_CLIP, (reg32))
#define OVL_L2_CLIP_GET_L2_SRC_TOP_CLIP(reg32)                                 \
                REG_FLD_GET(OVL_L2_CLIP_FLD_L2_SRC_TOP_CLIP, (reg32))
#define OVL_L2_CLIP_GET_L2_SRC_RIGHT_CLIP(reg32)                               \
                REG_FLD_GET(OVL_L2_CLIP_FLD_L2_SRC_RIGHT_CLIP, (reg32))
#define OVL_L2_CLIP_GET_L2_SRC_LEFT_CLIP(reg32)                                \
                REG_FLD_GET(OVL_L2_CLIP_FLD_L2_SRC_LEFT_CLIP, (reg32))

#define OVL_L3_CON_GET_DSTKEY_EN(reg32)         REG_FLD_GET(OVL_L3_CON_FLD_DSTKEY_EN, (reg32))
#define OVL_L3_CON_GET_SRCKEY_EN(reg32)         REG_FLD_GET(OVL_L3_CON_FLD_SRCKEY_EN, (reg32))
#define OVL_L3_CON_GET_LAYER_SRC(reg32)         REG_FLD_GET(OVL_L3_CON_FLD_LAYER_SRC, (reg32))
#define OVL_L3_CON_GET_MTX_EN(reg32)            REG_FLD_GET(OVL_L3_CON_FLD_MTX_EN, (reg32))
#define OVL_L3_CON_GET_MTX_AUTO_DIS(reg32)      REG_FLD_GET(OVL_L3_CON_FLD_MTX_AUTO_DIS, (reg32))
#define OVL_L3_CON_GET_RGB_SWAP(reg32)          REG_FLD_GET(OVL_L3_CON_FLD_RGB_SWAP, (reg32))
#define OVL_L3_CON_GET_BYTE_SWAP(reg32)         REG_FLD_GET(OVL_L3_CON_FLD_BYTE_SWAP, (reg32))
#define OVL_L3_CON_GET_CLRFMT_MAN(reg32)        REG_FLD_GET(OVL_L3_CON_FLD_CLRFMT_MAN, (reg32))
#define OVL_L3_CON_GET_R_FIRST(reg32)           REG_FLD_GET(OVL_L3_CON_FLD_R_FIRST, (reg32))
#define OVL_L3_CON_GET_LANDSCAPE(reg32)         REG_FLD_GET(OVL_L3_CON_FLD_LANDSCAPE, (reg32))
#define OVL_L3_CON_GET_EN_3D(reg32)             REG_FLD_GET(OVL_L3_CON_FLD_EN_3D, (reg32))
#define OVL_L3_CON_GET_INT_MTX_SEL(reg32)       REG_FLD_GET(OVL_L3_CON_FLD_INT_MTX_SEL, (reg32))
#define OVL_L3_CON_GET_CLRFMT(reg32)            REG_FLD_GET(OVL_L3_CON_FLD_CLRFMT, (reg32))
#define OVL_L3_CON_GET_EXT_MTX_EN(reg32)        REG_FLD_GET(OVL_L3_CON_FLD_EXT_MTX_EN, (reg32))
#define OVL_L3_CON_GET_HORIZONTAL_FLIP_EN(reg32)                               \
                REG_FLD_GET(OVL_L3_CON_FLD_HORIZONTAL_FLIP_EN, (reg32))
#define OVL_L3_CON_GET_VERTICAL_FLIP_EN(reg32)                                 \
                REG_FLD_GET(OVL_L3_CON_FLD_VERTICAL_FLIP_EN, (reg32))
#define OVL_L3_CON_GET_ALPHA_EN(reg32)          REG_FLD_GET(OVL_L3_CON_FLD_ALPHA_EN, (reg32))
#define OVL_L3_CON_GET_ALPHA(reg32)             REG_FLD_GET(OVL_L3_CON_FLD_ALPHA, (reg32))

#define OVL_L3_SRCKEY_GET_SRCKEY(reg32)         REG_FLD_GET(OVL_L3_SRCKEY_FLD_SRCKEY, (reg32))

#define OVL_L3_SRC_SIZE_GET_L3_SRC_H(reg32)     REG_FLD_GET(OVL_L3_SRC_SIZE_FLD_L3_SRC_H, (reg32))
#define OVL_L3_SRC_SIZE_GET_L3_SRC_W(reg32)     REG_FLD_GET(OVL_L3_SRC_SIZE_FLD_L3_SRC_W, (reg32))

#define OVL_L3_OFFSET_GET_L3_YOFF(reg32)        REG_FLD_GET(OVL_L3_OFFSET_FLD_L3_YOFF, (reg32))
#define OVL_L3_OFFSET_GET_L3_XOFF(reg32)        REG_FLD_GET(OVL_L3_OFFSET_FLD_L3_XOFF, (reg32))

#define OVL_L3_PITCH_GET_SURFL_EN(reg32)                                       \
                REG_FLD_GET(OVL_L3_PITCH_FLD_SURFL_EN, (reg32))
#define OVL_L3_PITCH_GET_L3_BLEND_RND_SHT(reg32)                               \
                REG_FLD_GET(OVL_L3_PITCH_FLD_L3_BLEND_RND_SHT, (reg32))
#define OVL_L3_PITCH_GET_L3_SRGB_SEL_EXT2(reg32)                               \
                REG_FLD_GET(OVL_L3_PITCH_FLD_L3_SRGB_SEL_EXT2, (reg32))
#define OVL_L3_PITCH_GET_L3_CONST_BLD(reg32)                                   \
                REG_FLD_GET(OVL_L3_PITCH_FLD_L3_CONST_BLD, (reg32))
#define OVL_L3_PITCH_GET_L3_DRGB_SEL_EXT(reg32)                                \
                REG_FLD_GET(OVL_L3_PITCH_FLD_L3_DRGB_SEL_EXT, (reg32))
#define OVL_L3_PITCH_GET_L3_DA_SEL_EXT(reg32)                                  \
                REG_FLD_GET(OVL_L3_PITCH_FLD_L3_DA_SEL_EXT, (reg32))
#define OVL_L3_PITCH_GET_L3_SRGB_SEL_EXT(reg32)                                \
                REG_FLD_GET(OVL_L3_PITCH_FLD_L3_SRGB_SEL_EXT, (reg32))
#define OVL_L3_PITCH_GET_L3_SA_SEL_EXT(reg32)                                  \
                REG_FLD_GET(OVL_L3_PITCH_FLD_L3_SA_SEL_EXT, (reg32))
#define OVL_L3_PITCH_GET_L3_DRGB_SEL(reg32)                                    \
                REG_FLD_GET(OVL_L3_PITCH_FLD_L3_DRGB_SEL, (reg32))
#define OVL_L3_PITCH_GET_L3_DA_SEL(reg32)                                      \
                REG_FLD_GET(OVL_L3_PITCH_FLD_L3_DA_SEL, (reg32))
#define OVL_L3_PITCH_GET_L3_SRGB_SEL(reg32)                                    \
                REG_FLD_GET(OVL_L3_PITCH_FLD_L3_SRGB_SEL, (reg32))
#define OVL_L3_PITCH_GET_L3_SA_SEL(reg32)                                      \
                REG_FLD_GET(OVL_L3_PITCH_FLD_L3_SA_SEL, (reg32))
#define OVL_L3_PITCH_GET_L3_SRC_PITCH(reg32)                                   \
                REG_FLD_GET(OVL_L3_PITCH_FLD_L3_SRC_PITCH, (reg32))

#define OVL_L3_TILE_GET_TILE_HORI_BLOCK_NUM(reg32)                             \
                REG_FLD_GET(OVL_L3_TILE_FLD_TILE_HORI_BLOCK_NUM, (reg32))
#define OVL_L3_TILE_GET_TILE_EN(reg32)                                         \
                REG_FLD_GET(OVL_L3_TILE_FLD_TILE_EN, (reg32))
#define OVL_L3_TILE_GET_TILE_WIDTH_SEL(reg32)                                  \
                REG_FLD_GET(OVL_L3_TILE_FLD_TILE_WIDTH_SEL, (reg32))
#define OVL_L3_TILE_GET_TILE_HEIGHT(reg32)                                     \
                REG_FLD_GET(OVL_L3_TILE_FLD_TILE_HEIGHT, (reg32))

#define OVL_L3_CLIP_GET_L3_SRC_BOTTOM_CLIP(reg32)                              \
                REG_FLD_GET(OVL_L3_CLIP_FLD_L3_SRC_BOTTOM_CLIP, (reg32))
#define OVL_L3_CLIP_GET_L3_SRC_TOP_CLIP(reg32)                                 \
                REG_FLD_GET(OVL_L3_CLIP_FLD_L3_SRC_TOP_CLIP, (reg32))
#define OVL_L3_CLIP_GET_L3_SRC_RIGHT_CLIP(reg32)                               \
                REG_FLD_GET(OVL_L3_CLIP_FLD_L3_SRC_RIGHT_CLIP, (reg32))
#define OVL_L3_CLIP_GET_L3_SRC_LEFT_CLIP(reg32)                                \
                REG_FLD_GET(OVL_L3_CLIP_FLD_L3_SRC_LEFT_CLIP, (reg32))

#define OVL_RDMA0_CTRL_GET_RDMA0_FIFO_USED_SIZE(reg32)                         \
                REG_FLD_GET(OVL_RDMA0_CTRL_FLD_RDMA0_FIFO_USED_SIZE, (reg32))
#define OVL_RDMA0_CTRL_GET_RDMA0_INTERLACE(reg32)                              \
                REG_FLD_GET(OVL_RDMA0_CTRL_FLD_RDMA0_INTERLACE, (reg32))
#define OVL_RDMA0_CTRL_GET_RDMA0_EN(reg32)                                     \
                REG_FLD_GET(OVL_RDMA0_CTRL_FLD_RDMA0_EN, (reg32))

#define OVL_RDMA0_MEM_GMC_SETTING1_GET_RDMA0_PRE_ULTRA_THRESHOLD_HIGH_OFS(reg32)    \
                REG_FLD_GET(OVL_RDMA0_MEM_GMC_SETTING1_FLD_RDMA0_PRE_ULTRA_THRESHOLD_HIGH_OFS,    \
                (reg32))
#define OVL_RDMA0_MEM_GMC_SETTING1_GET_RDMA0_ULTRA_THRESHOLD_HIGH_OFS(reg32)        \
                REG_FLD_GET(OVL_RDMA0_MEM_GMC_SETTING1_FLD_RDMA0_ULTRA_THRESHOLD_HIGH_OFS, (reg32))
#define OVL_RDMA0_MEM_GMC_SETTING1_GET_RDMA0_OSTD_PREULTRA_TH(reg32)                \
                REG_FLD_GET(OVL_RDMA0_MEM_GMC_SETTING1_FLD_RDMA0_OSTD_PREULTRA_TH, (reg32))
#define OVL_RDMA0_MEM_GMC_SETTING1_GET_RDMA0_OSTD_ULTRA_TH(reg32)                    \
                REG_FLD_GET(OVL_RDMA0_MEM_GMC_SETTING1_FLD_RDMA0_OSTD_ULTRA_TH, (reg32))

#define OVL_RDMA0_MEM_SLOW_CON_GET_RDMA0_SLOW_CNT(reg32)                             \
                REG_FLD_GET(OVL_RDMA0_MEM_SLOW_CON_FLD_RDMA0_SLOW_CNT, (reg32))
#define OVL_RDMA0_MEM_SLOW_CON_GET_RDMA0_SLOW_EN(reg32)                              \
                REG_FLD_GET(OVL_RDMA0_MEM_SLOW_CON_FLD_RDMA0_SLOW_EN, (reg32))

#define OVL_RDMA0_FIFO_CTRL_GET_RDMA0_FIFO_UND_EN(reg32)                             \
                REG_FLD_GET(OVL_RDMA0_FIFO_CTRL_FLD_RDMA0_FIFO_UND_EN, (reg32))
#define OVL_RDMA0_FIFO_CTRL_GET_RDMA0_FIFO_SIZE(reg32)                               \
                REG_FLD_GET(OVL_RDMA0_FIFO_CTRL_FLD_RDMA0_FIFO_SIZE, (reg32))
#define OVL_RDMA0_FIFO_CTRL_GET_RDMA0_FIFO_THRD(reg32)                               \
                REG_FLD_GET(OVL_RDMA0_FIFO_CTRL_FLD_RDMA0_FIFO_THRD, (reg32))

#define OVL_RDMA1_CTRL_GET_RDMA1_FIFO_USED_SIZE(reg32)                               \
                REG_FLD_GET(OVL_RDMA1_CTRL_FLD_RDMA1_FIFO_USED_SIZE, (reg32))
#define OVL_RDMA1_CTRL_GET_RDMA1_INTERLACE(reg32)                                    \
                REG_FLD_GET(OVL_RDMA1_CTRL_FLD_RDMA1_INTERLACE, (reg32))
#define OVL_RDMA1_CTRL_GET_RDMA1_EN(reg32)                                           \
                REG_FLD_GET(OVL_RDMA1_CTRL_FLD_RDMA1_EN, (reg32))

#define OVL_RDMA1_MEM_GMC_SETTING1_GET_RDMA1_PRE_ULTRA_THRESHOLD_HIGH_OFS(reg32)     \
                REG_FLD_GET(OVL_RDMA1_MEM_GMC_SETTING1_FLD_RDMA1_PRE_ULTRA_THRESHOLD_HIGH_OFS,    \
                (reg32))
#define OVL_RDMA1_MEM_GMC_SETTING1_GET_RDMA1_ULTRA_THRESHOLD_HIGH_OFS(reg32)          \
                REG_FLD_GET(OVL_RDMA1_MEM_GMC_SETTING1_FLD_RDMA1_ULTRA_THRESHOLD_HIGH_OFS, (reg32))
#define OVL_RDMA1_MEM_GMC_SETTING1_GET_RDMA1_OSTD_PREULTRA_TH(reg32)                  \
                REG_FLD_GET(OVL_RDMA1_MEM_GMC_SETTING1_FLD_RDMA1_OSTD_PREULTRA_TH, (reg32))
#define OVL_RDMA1_MEM_GMC_SETTING1_GET_RDMA1_OSTD_ULTRA_TH(reg32)                     \
                REG_FLD_GET(OVL_RDMA1_MEM_GMC_SETTING1_FLD_RDMA1_OSTD_ULTRA_TH, (reg32))

#define OVL_RDMA1_MEM_SLOW_CON_GET_RDMA1_SLOW_CNT(reg32)                              \
                REG_FLD_GET(OVL_RDMA1_MEM_SLOW_CON_FLD_RDMA1_SLOW_CNT, (reg32))
#define OVL_RDMA1_MEM_SLOW_CON_GET_RDMA1_SLOW_EN(reg32)                               \
                REG_FLD_GET(OVL_RDMA1_MEM_SLOW_CON_FLD_RDMA1_SLOW_EN, (reg32))

#define OVL_RDMA1_FIFO_CTRL_GET_RDMA1_FIFO_UND_EN(reg32)                              \
                REG_FLD_GET(OVL_RDMA1_FIFO_CTRL_FLD_RDMA1_FIFO_UND_EN, (reg32))
#define OVL_RDMA1_FIFO_CTRL_GET_RDMA1_FIFO_SIZE(reg32)                                \
                REG_FLD_GET(OVL_RDMA1_FIFO_CTRL_FLD_RDMA1_FIFO_SIZE, (reg32))
#define OVL_RDMA1_FIFO_CTRL_GET_RDMA1_FIFO_THRD(reg32)                                \
                REG_FLD_GET(OVL_RDMA1_FIFO_CTRL_FLD_RDMA1_FIFO_THRD, (reg32))

#define OVL_RDMA2_CTRL_GET_RDMA2_FIFO_USED_SIZE(reg32)                                \
                REG_FLD_GET(OVL_RDMA2_CTRL_FLD_RDMA2_FIFO_USED_SIZE, (reg32))
#define OVL_RDMA2_CTRL_GET_RDMA2_INTERLACE(reg32)                                     \
                REG_FLD_GET(OVL_RDMA2_CTRL_FLD_RDMA2_INTERLACE, (reg32))
#define OVL_RDMA2_CTRL_GET_RDMA2_EN(reg32)                                            \
                REG_FLD_GET(OVL_RDMA2_CTRL_FLD_RDMA2_EN, (reg32))

#define OVL_RDMA2_MEM_GMC_SETTING1_GET_RDMA2_PRE_ULTRA_THRESHOLD_HIGH_OFS(reg32)      \
                REG_FLD_GET(OVL_RDMA2_MEM_GMC_SETTING1_FLD_RDMA2_PRE_ULTRA_THRESHOLD_HIGH_OFS,    \
                (reg32))
#define OVL_RDMA2_MEM_GMC_SETTING1_GET_RDMA2_ULTRA_THRESHOLD_HIGH_OFS(reg32)          \
                REG_FLD_GET(OVL_RDMA2_MEM_GMC_SETTING1_FLD_RDMA2_ULTRA_THRESHOLD_HIGH_OFS, (reg32))
#define OVL_RDMA2_MEM_GMC_SETTING1_GET_RDMA2_OSTD_PREULTRA_TH(reg32)                   \
                REG_FLD_GET(OVL_RDMA2_MEM_GMC_SETTING1_FLD_RDMA2_OSTD_PREULTRA_TH, (reg32))
#define OVL_RDMA2_MEM_GMC_SETTING1_GET_RDMA2_OSTD_ULTRA_TH(reg32)                       \
                REG_FLD_GET(OVL_RDMA2_MEM_GMC_SETTING1_FLD_RDMA2_OSTD_ULTRA_TH, (reg32))

#define OVL_RDMA2_MEM_SLOW_CON_GET_RDMA2_SLOW_CNT(reg32)                                \
                REG_FLD_GET(OVL_RDMA2_MEM_SLOW_CON_FLD_RDMA2_SLOW_CNT, (reg32))
#define OVL_RDMA2_MEM_SLOW_CON_GET_RDMA2_SLOW_EN(reg32)                                 \
                REG_FLD_GET(OVL_RDMA2_MEM_SLOW_CON_FLD_RDMA2_SLOW_EN, (reg32))

#define OVL_RDMA2_FIFO_CTRL_GET_RDMA2_FIFO_UND_EN(reg32)                                \
                REG_FLD_GET(OVL_RDMA2_FIFO_CTRL_FLD_RDMA2_FIFO_UND_EN, (reg32))
#define OVL_RDMA2_FIFO_CTRL_GET_RDMA2_FIFO_SIZE(reg32)                                  \
                REG_FLD_GET(OVL_RDMA2_FIFO_CTRL_FLD_RDMA2_FIFO_SIZE, (reg32))
#define OVL_RDMA2_FIFO_CTRL_GET_RDMA2_FIFO_THRD(reg32)                                  \
                REG_FLD_GET(OVL_RDMA2_FIFO_CTRL_FLD_RDMA2_FIFO_THRD, (reg32))

#define OVL_RDMA3_CTRL_GET_RDMA3_FIFO_USED_SIZE(reg32)                                  \
                REG_FLD_GET(OVL_RDMA3_CTRL_FLD_RDMA3_FIFO_USED_SIZE, (reg32))
#define OVL_RDMA3_CTRL_GET_RDMA3_INTERLACE(reg32)                                       \
                REG_FLD_GET(OVL_RDMA3_CTRL_FLD_RDMA3_INTERLACE, (reg32))
#define OVL_RDMA3_CTRL_GET_RDMA3_EN(reg32)                                              \
                REG_FLD_GET(OVL_RDMA3_CTRL_FLD_RDMA3_EN, (reg32))

#define OVL_RDMA3_MEM_GMC_SETTING1_GET_RDMA3_PRE_ULTRA_THRESHOLD_HIGH_OFS(reg32)        \
                REG_FLD_GET(OVL_RDMA3_MEM_GMC_SETTING1_FLD_RDMA3_PRE_ULTRA_THRESHOLD_HIGH_OFS,    \
                (reg32))
#define OVL_RDMA3_MEM_GMC_SETTING1_GET_RDMA3_ULTRA_THRESHOLD_HIGH_OFS(reg32)            \
                REG_FLD_GET(OVL_RDMA3_MEM_GMC_SETTING1_FLD_RDMA3_ULTRA_THRESHOLD_HIGH_OFS, (reg32))
#define OVL_RDMA3_MEM_GMC_SETTING1_GET_RDMA3_OSTD_PREULTRA_TH(reg32)                    \
                REG_FLD_GET(OVL_RDMA3_MEM_GMC_SETTING1_FLD_RDMA3_OSTD_PREULTRA_TH, (reg32))
#define OVL_RDMA3_MEM_GMC_SETTING1_GET_RDMA3_OSTD_ULTRA_TH(reg32)                       \
                REG_FLD_GET(OVL_RDMA3_MEM_GMC_SETTING1_FLD_RDMA3_OSTD_ULTRA_TH, (reg32))

#define OVL_RDMA3_MEM_SLOW_CON_GET_RDMA3_SLOW_CNT(reg32)                                \
                REG_FLD_GET(OVL_RDMA3_MEM_SLOW_CON_FLD_RDMA3_SLOW_CNT, (reg32))
#define OVL_RDMA3_MEM_SLOW_CON_GET_RDMA3_SLOW_EN(reg32)                                 \
                REG_FLD_GET(OVL_RDMA3_MEM_SLOW_CON_FLD_RDMA3_SLOW_EN, (reg32))

#define OVL_RDMA3_FIFO_CTRL_GET_RDMA3_FIFO_UND_EN(reg32)                                \
                REG_FLD_GET(OVL_RDMA3_FIFO_CTRL_FLD_RDMA3_FIFO_UND_EN, (reg32))
#define OVL_RDMA3_FIFO_CTRL_GET_RDMA3_FIFO_SIZE(reg32)                                  \
                REG_FLD_GET(OVL_RDMA3_FIFO_CTRL_FLD_RDMA3_FIFO_SIZE, (reg32))
#define OVL_RDMA3_FIFO_CTRL_GET_RDMA3_FIFO_THRD(reg32)                                  \
                REG_FLD_GET(OVL_RDMA3_FIFO_CTRL_FLD_RDMA3_FIFO_THRD, (reg32))

#define OVL_L0_Y2R_PARA_R0_GET_C_CF_RMU(reg32)                                          \
                REG_FLD_GET(OVL_L0_Y2R_PARA_R0_FLD_C_CF_RMU, (reg32))
#define OVL_L0_Y2R_PARA_R0_GET_C_CF_RMY(reg32)                                          \
                REG_FLD_GET(OVL_L0_Y2R_PARA_R0_FLD_C_CF_RMY, (reg32))

#define OVL_L0_Y2R_PARA_R1_GET_C_CF_RMV(reg32)                                          \
                REG_FLD_GET(OVL_L0_Y2R_PARA_R1_FLD_C_CF_RMV, (reg32))

#define OVL_L0_Y2R_PARA_G0_GET_C_CF_GMU(reg32)                                          \
                REG_FLD_GET(OVL_L0_Y2R_PARA_G0_FLD_C_CF_GMU, (reg32))
#define OVL_L0_Y2R_PARA_G0_GET_C_CF_GMY(reg32)                                          \
                REG_FLD_GET(OVL_L0_Y2R_PARA_G0_FLD_C_CF_GMY, (reg32))

#define OVL_L0_Y2R_PARA_G1_GET_C_CF_GMV(reg32)                                          \
                REG_FLD_GET(OVL_L0_Y2R_PARA_G1_FLD_C_CF_GMV, (reg32))

#define OVL_L0_Y2R_PARA_B0_GET_C_CF_BMU(reg32)                                          \
                REG_FLD_GET(OVL_L0_Y2R_PARA_B0_FLD_C_CF_BMU, (reg32))
#define OVL_L0_Y2R_PARA_B0_GET_C_CF_BMY(reg32)                                          \
                REG_FLD_GET(OVL_L0_Y2R_PARA_B0_FLD_C_CF_BMY, (reg32))

#define OVL_L0_Y2R_PARA_B1_GET_C_CF_BMV(reg32)                                          \
                REG_FLD_GET(OVL_L0_Y2R_PARA_B1_FLD_C_CF_BMV, (reg32))

#define OVL_L0_Y2R_PARA_YUV_A_0_GET_C_CF_UA(reg32)                                      \
                REG_FLD_GET(OVL_L0_Y2R_PARA_YUV_A_0_FLD_C_CF_UA, (reg32))
#define OVL_L0_Y2R_PARA_YUV_A_0_GET_C_CF_YA(reg32)                                      \
                REG_FLD_GET(OVL_L0_Y2R_PARA_YUV_A_0_FLD_C_CF_YA, (reg32))

#define OVL_L0_Y2R_PARA_YUV_A_1_GET_C_CF_VA(reg32)                                      \
                REG_FLD_GET(OVL_L0_Y2R_PARA_YUV_A_1_FLD_C_CF_VA, (reg32))

#define OVL_L0_Y2R_PARA_RGB_A_0_GET_C_CF_GA(reg32)                                      \
                REG_FLD_GET(OVL_L0_Y2R_PARA_RGB_A_0_FLD_C_CF_GA, (reg32))
#define OVL_L0_Y2R_PARA_RGB_A_0_GET_C_CF_RA(reg32)                                      \
                REG_FLD_GET(OVL_L0_Y2R_PARA_RGB_A_0_FLD_C_CF_RA, (reg32))

#define OVL_L0_Y2R_PARA_RGB_A_1_GET_C_CF_BA(reg32)                                      \
                REG_FLD_GET(OVL_L0_Y2R_PARA_RGB_A_1_FLD_C_CF_BA, (reg32))

#define OVL_L1_Y2R_PARA_R0_GET_C_CF_RMU(reg32)                                          \
                REG_FLD_GET(OVL_L1_Y2R_PARA_R0_FLD_C_CF_RMU, (reg32))
#define OVL_L1_Y2R_PARA_R0_GET_C_CF_RMY(reg32)                                          \
                REG_FLD_GET(OVL_L1_Y2R_PARA_R0_FLD_C_CF_RMY, (reg32))

#define OVL_L1_Y2R_PARA_R1_GET_C_CF_RMV(reg32)                                          \
                REG_FLD_GET(OVL_L1_Y2R_PARA_R1_FLD_C_CF_RMV, (reg32))

#define OVL_L1_Y2R_PARA_G0_GET_C_CF_GMU(reg32)                                          \
                REG_FLD_GET(OVL_L1_Y2R_PARA_G0_FLD_C_CF_GMU, (reg32))
#define OVL_L1_Y2R_PARA_G0_GET_C_CF_GMY(reg32)                                          \
                REG_FLD_GET(OVL_L1_Y2R_PARA_G0_FLD_C_CF_GMY, (reg32))

#define OVL_L1_Y2R_PARA_G1_GET_C_CF_GMV(reg32)                                          \
                REG_FLD_GET(OVL_L1_Y2R_PARA_G1_FLD_C_CF_GMV, (reg32))

#define OVL_L1_Y2R_PARA_B0_GET_C_CF_BMU(reg32)                                          \
                REG_FLD_GET(OVL_L1_Y2R_PARA_B0_FLD_C_CF_BMU, (reg32))
#define OVL_L1_Y2R_PARA_B0_GET_C_CF_BMY(reg32)                                          \
                REG_FLD_GET(OVL_L1_Y2R_PARA_B0_FLD_C_CF_BMY, (reg32))

#define OVL_L1_Y2R_PARA_B1_GET_C_CF_BMV(reg32)                                          \
                REG_FLD_GET(OVL_L1_Y2R_PARA_B1_FLD_C_CF_BMV, (reg32))

#define OVL_L1_Y2R_PARA_YUV_A_0_GET_C_CF_UA(reg32)                                      \
                REG_FLD_GET(OVL_L1_Y2R_PARA_YUV_A_0_FLD_C_CF_UA, (reg32))
#define OVL_L1_Y2R_PARA_YUV_A_0_GET_C_CF_YA(reg32)                                      \
                REG_FLD_GET(OVL_L1_Y2R_PARA_YUV_A_0_FLD_C_CF_YA, (reg32))

#define OVL_L1_Y2R_PARA_YUV_A_1_GET_C_CF_VA(reg32)                                      \
                REG_FLD_GET(OVL_L1_Y2R_PARA_YUV_A_1_FLD_C_CF_VA, (reg32))

#define OVL_L1_Y2R_PARA_RGB_A_0_GET_C_CF_GA(reg32)                                      \
                REG_FLD_GET(OVL_L1_Y2R_PARA_RGB_A_0_FLD_C_CF_GA, (reg32))
#define OVL_L1_Y2R_PARA_RGB_A_0_GET_C_CF_RA(reg32)                                      \
                REG_FLD_GET(OVL_L1_Y2R_PARA_RGB_A_0_FLD_C_CF_RA, (reg32))

#define OVL_L1_Y2R_PARA_RGB_A_1_GET_C_CF_BA(reg32)                                      \
                REG_FLD_GET(OVL_L1_Y2R_PARA_RGB_A_1_FLD_C_CF_BA, (reg32))

#define OVL_L2_Y2R_PARA_R0_GET_C_CF_RMU(reg32)                                          \
                REG_FLD_GET(OVL_L2_Y2R_PARA_R0_FLD_C_CF_RMU, (reg32))
#define OVL_L2_Y2R_PARA_R0_GET_C_CF_RMY(reg32)                                          \
                REG_FLD_GET(OVL_L2_Y2R_PARA_R0_FLD_C_CF_RMY, (reg32))

#define OVL_L2_Y2R_PARA_R1_GET_C_CF_RMV(reg32)                                          \
                REG_FLD_GET(OVL_L2_Y2R_PARA_R1_FLD_C_CF_RMV, (reg32))

#define OVL_L2_Y2R_PARA_G0_GET_C_CF_GMU(reg32)                                          \
                REG_FLD_GET(OVL_L2_Y2R_PARA_G0_FLD_C_CF_GMU, (reg32))
#define OVL_L2_Y2R_PARA_G0_GET_C_CF_GMY(reg32)                                          \
                REG_FLD_GET(OVL_L2_Y2R_PARA_G0_FLD_C_CF_GMY, (reg32))

#define OVL_L2_Y2R_PARA_G1_GET_C_CF_GMV(reg32)                                          \
                REG_FLD_GET(OVL_L2_Y2R_PARA_G1_FLD_C_CF_GMV, (reg32))

#define OVL_L2_Y2R_PARA_B0_GET_C_CF_BMU(reg32)                                          \
                REG_FLD_GET(OVL_L2_Y2R_PARA_B0_FLD_C_CF_BMU, (reg32))
#define OVL_L2_Y2R_PARA_B0_GET_C_CF_BMY(reg32)                                          \
                REG_FLD_GET(OVL_L2_Y2R_PARA_B0_FLD_C_CF_BMY, (reg32))

#define OVL_L2_Y2R_PARA_B1_GET_C_CF_BMV(reg32)                                          \
                REG_FLD_GET(OVL_L2_Y2R_PARA_B1_FLD_C_CF_BMV, (reg32))

#define OVL_L2_Y2R_PARA_YUV_A_0_GET_C_CF_UA(reg32)                                      \
                REG_FLD_GET(OVL_L2_Y2R_PARA_YUV_A_0_FLD_C_CF_UA, (reg32))
#define OVL_L2_Y2R_PARA_YUV_A_0_GET_C_CF_YA(reg32)                                      \
                REG_FLD_GET(OVL_L2_Y2R_PARA_YUV_A_0_FLD_C_CF_YA, (reg32))

#define OVL_L2_Y2R_PARA_YUV_A_1_GET_C_CF_VA(reg32)                                      \
                REG_FLD_GET(OVL_L2_Y2R_PARA_YUV_A_1_FLD_C_CF_VA, (reg32))

#define OVL_L2_Y2R_PARA_RGB_A_0_GET_C_CF_GA(reg32)                                      \
                REG_FLD_GET(OVL_L2_Y2R_PARA_RGB_A_0_FLD_C_CF_GA, (reg32))
#define OVL_L2_Y2R_PARA_RGB_A_0_GET_C_CF_RA(reg32)                                      \
                REG_FLD_GET(OVL_L2_Y2R_PARA_RGB_A_0_FLD_C_CF_RA, (reg32))

#define OVL_L2_Y2R_PARA_RGB_A_1_GET_C_CF_BA(reg32)                                      \
                REG_FLD_GET(OVL_L2_Y2R_PARA_RGB_A_1_FLD_C_CF_BA, (reg32))

#define OVL_L3_Y2R_PARA_R0_GET_C_CF_RMU(reg32)                                          \
                REG_FLD_GET(OVL_L3_Y2R_PARA_R0_FLD_C_CF_RMU, (reg32))
#define OVL_L3_Y2R_PARA_R0_GET_C_CF_RMY(reg32)                                          \
                REG_FLD_GET(OVL_L3_Y2R_PARA_R0_FLD_C_CF_RMY, (reg32))

#define OVL_L3_Y2R_PARA_R1_GET_C_CF_RMV(reg32)                                          \
                REG_FLD_GET(OVL_L3_Y2R_PARA_R1_FLD_C_CF_RMV, (reg32))

#define OVL_L3_Y2R_PARA_G0_GET_C_CF_GMU(reg32)                                          \
                REG_FLD_GET(OVL_L3_Y2R_PARA_G0_FLD_C_CF_GMU, (reg32))
#define OVL_L3_Y2R_PARA_G0_GET_C_CF_GMY(reg32)                                          \
                REG_FLD_GET(OVL_L3_Y2R_PARA_G0_FLD_C_CF_GMY, (reg32))

#define OVL_L3_Y2R_PARA_G1_GET_C_CF_GMV(reg32)                                          \
                REG_FLD_GET(OVL_L3_Y2R_PARA_G1_FLD_C_CF_GMV, (reg32))

#define OVL_L3_Y2R_PARA_B0_GET_C_CF_BMU(reg32)                                          \
                REG_FLD_GET(OVL_L3_Y2R_PARA_B0_FLD_C_CF_BMU, (reg32))
#define OVL_L3_Y2R_PARA_B0_GET_C_CF_BMY(reg32)                                          \
                REG_FLD_GET(OVL_L3_Y2R_PARA_B0_FLD_C_CF_BMY, (reg32))

#define OVL_L3_Y2R_PARA_B1_GET_C_CF_BMV(reg32)                                          \
                REG_FLD_GET(OVL_L3_Y2R_PARA_B1_FLD_C_CF_BMV, (reg32))

#define OVL_L3_Y2R_PARA_YUV_A_0_GET_C_CF_UA(reg32)                                      \
                REG_FLD_GET(OVL_L3_Y2R_PARA_YUV_A_0_FLD_C_CF_UA, (reg32))
#define OVL_L3_Y2R_PARA_YUV_A_0_GET_C_CF_YA(reg32)                                      \
                REG_FLD_GET(OVL_L3_Y2R_PARA_YUV_A_0_FLD_C_CF_YA, (reg32))

#define OVL_L3_Y2R_PARA_YUV_A_1_GET_C_CF_VA(reg32)                                      \
                REG_FLD_GET(OVL_L3_Y2R_PARA_YUV_A_1_FLD_C_CF_VA, (reg32))

#define OVL_L3_Y2R_PARA_RGB_A_0_GET_C_CF_GA(reg32)                                      \
                REG_FLD_GET(OVL_L3_Y2R_PARA_RGB_A_0_FLD_C_CF_GA, (reg32))
#define OVL_L3_Y2R_PARA_RGB_A_0_GET_C_CF_RA(reg32)                                      \
                REG_FLD_GET(OVL_L3_Y2R_PARA_RGB_A_0_FLD_C_CF_RA, (reg32))

#define OVL_L3_Y2R_PARA_RGB_A_1_GET_C_CF_BA(reg32)                                      \
                REG_FLD_GET(OVL_L3_Y2R_PARA_RGB_A_1_FLD_C_CF_BA, (reg32))

#define OVL_DEBUG_MON_SEL_GET_DBG_MON_SEL(reg32)                                        \
                REG_FLD_GET(OVL_DEBUG_MON_SEL_FLD_DBG_MON_SEL, (reg32))

#define OVL_BLD_EXT_GET_EL2_MINUS_BLD(reg32)    REG_FLD_GET(OVL_BLD_EXT_FLD_EL2_MINUS_BLD, (reg32))
#define OVL_BLD_EXT_GET_EL1_MINUS_BLD(reg32)    REG_FLD_GET(OVL_BLD_EXT_FLD_EL1_MINUS_BLD, (reg32))
#define OVL_BLD_EXT_GET_EL0_MINUS_BLD(reg32)    REG_FLD_GET(OVL_BLD_EXT_FLD_EL0_MINUS_BLD, (reg32))
#define OVL_BLD_EXT_GET_LC_MINUS_BLD(reg32)     REG_FLD_GET(OVL_BLD_EXT_FLD_LC_MINUS_BLD, (reg32))
#define OVL_BLD_EXT_GET_L3_MINUS_BLD(reg32)     REG_FLD_GET(OVL_BLD_EXT_FLD_L3_MINUS_BLD, (reg32))
#define OVL_BLD_EXT_GET_L2_MINUS_BLD(reg32)     REG_FLD_GET(OVL_BLD_EXT_FLD_L2_MINUS_BLD, (reg32))
#define OVL_BLD_EXT_GET_L1_MINUS_BLD(reg32)     REG_FLD_GET(OVL_BLD_EXT_FLD_L1_MINUS_BLD, (reg32))
#define OVL_BLD_EXT_GET_L0_MINUS_BLD(reg32)     REG_FLD_GET(OVL_BLD_EXT_FLD_L0_MINUS_BLD, (reg32))

#define OVL_RDMA0_MEM_GMC_SETTING2_GET_RDMA0_FORCE_REQ_THRESHOLD(reg32)                 \
                REG_FLD_GET(OVL_RDMA0_MEM_GMC_SETTING2_FLD_RDMA0_FORCE_REQ_THRESHOLD, (reg32))
#define OVL_RDMA0_MEM_GMC_SETTING2_GET_RDMA0_REQ_THRESHOLD_ULTRA(reg32)                 \
                REG_FLD_GET(OVL_RDMA0_MEM_GMC_SETTING2_FLD_RDMA0_REQ_THRESHOLD_ULTRA, (reg32))
#define OVL_RDMA0_MEM_GMC_SETTING2_GET_RDMA0_REQ_THRESHOLD_PREULTRA(reg32)              \
                REG_FLD_GET(OVL_RDMA0_MEM_GMC_SETTING2_FLD_RDMA0_REQ_THRESHOLD_PREULTRA, (reg32))
#define OVL_RDMA0_MEM_GMC_SETTING2_GET_RDMA0_ISSUE_REQ_THRESHOLD_URG(reg32)             \
                REG_FLD_GET(OVL_RDMA0_MEM_GMC_SETTING2_FLD_RDMA0_ISSUE_REQ_THRESHOLD_URG, (reg32))
#define OVL_RDMA0_MEM_GMC_SETTING2_GET_RDMA0_ISSUE_REQ_THRESHOLD(reg32)                 \
                REG_FLD_GET(OVL_RDMA0_MEM_GMC_SETTING2_FLD_RDMA0_ISSUE_REQ_THRESHOLD, (reg32))

#define OVL_RDMA1_MEM_GMC_SETTING2_GET_RDMA1_FORCE_REQ_THRESHOLD(reg32)                 \
                REG_FLD_GET(OVL_RDMA1_MEM_GMC_SETTING2_FLD_RDMA1_FORCE_REQ_THRESHOLD, (reg32))
#define OVL_RDMA1_MEM_GMC_SETTING2_GET_RDMA1_REQ_THRESHOLD_ULTRA(reg32)                 \
                REG_FLD_GET(OVL_RDMA1_MEM_GMC_SETTING2_FLD_RDMA1_REQ_THRESHOLD_ULTRA, (reg32))
#define OVL_RDMA1_MEM_GMC_SETTING2_GET_RDMA1_REQ_THRESHOLD_PREULTRA(reg32)              \
                REG_FLD_GET(OVL_RDMA1_MEM_GMC_SETTING2_FLD_RDMA1_REQ_THRESHOLD_PREULTRA, (reg32))
#define OVL_RDMA1_MEM_GMC_SETTING2_GET_RDMA1_ISSUE_REQ_THRESHOLD_URG(reg32)             \
                REG_FLD_GET(OVL_RDMA1_MEM_GMC_SETTING2_FLD_RDMA1_ISSUE_REQ_THRESHOLD_URG, (reg32))
#define OVL_RDMA1_MEM_GMC_SETTING2_GET_RDMA1_ISSUE_REQ_THRESHOLD(reg32)                 \
                REG_FLD_GET(OVL_RDMA1_MEM_GMC_SETTING2_FLD_RDMA1_ISSUE_REQ_THRESHOLD, (reg32))

#define OVL_RDMA2_MEM_GMC_SETTING2_GET_RDMA2_FORCE_REQ_THRESHOLD(reg32)                 \
                REG_FLD_GET(OVL_RDMA2_MEM_GMC_SETTING2_FLD_RDMA2_FORCE_REQ_THRESHOLD, (reg32))
#define OVL_RDMA2_MEM_GMC_SETTING2_GET_RDMA2_REQ_THRESHOLD_ULTRA(reg32)                 \
                REG_FLD_GET(OVL_RDMA2_MEM_GMC_SETTING2_FLD_RDMA2_REQ_THRESHOLD_ULTRA, (reg32))
#define OVL_RDMA2_MEM_GMC_SETTING2_GET_RDMA2_REQ_THRESHOLD_PREULTRA(reg32)              \
                REG_FLD_GET(OVL_RDMA2_MEM_GMC_SETTING2_FLD_RDMA2_REQ_THRESHOLD_PREULTRA, (reg32))
#define OVL_RDMA2_MEM_GMC_SETTING2_GET_RDMA2_ISSUE_REQ_THRESHOLD_URG(reg32)             \
                REG_FLD_GET(OVL_RDMA2_MEM_GMC_SETTING2_FLD_RDMA2_ISSUE_REQ_THRESHOLD_URG, (reg32))
#define OVL_RDMA2_MEM_GMC_SETTING2_GET_RDMA2_ISSUE_REQ_THRESHOLD(reg32)                 \
                REG_FLD_GET(OVL_RDMA2_MEM_GMC_SETTING2_FLD_RDMA2_ISSUE_REQ_THRESHOLD, (reg32))

#define OVL_RDMA3_MEM_GMC_SETTING2_GET_RDMA3_FORCE_REQ_THRESHOLD(reg32)                 \
                REG_FLD_GET(OVL_RDMA3_MEM_GMC_SETTING2_FLD_RDMA3_FORCE_REQ_THRESHOLD, (reg32))
#define OVL_RDMA3_MEM_GMC_SETTING2_GET_RDMA3_REQ_THRESHOLD_ULTRA(reg32)                 \
                REG_FLD_GET(OVL_RDMA3_MEM_GMC_SETTING2_FLD_RDMA3_REQ_THRESHOLD_ULTRA, (reg32))
#define OVL_RDMA3_MEM_GMC_SETTING2_GET_RDMA3_REQ_THRESHOLD_PREULTRA(reg32)              \
                REG_FLD_GET(OVL_RDMA3_MEM_GMC_SETTING2_FLD_RDMA3_REQ_THRESHOLD_PREULTRA, (reg32))
#define OVL_RDMA3_MEM_GMC_SETTING2_GET_RDMA3_ISSUE_REQ_THRESHOLD_URG(reg32)             \
                REG_FLD_GET(OVL_RDMA3_MEM_GMC_SETTING2_FLD_RDMA3_ISSUE_REQ_THRESHOLD_URG, (reg32))
#define OVL_RDMA3_MEM_GMC_SETTING2_GET_RDMA3_ISSUE_REQ_THRESHOLD(reg32)                 \
                REG_FLD_GET(OVL_RDMA3_MEM_GMC_SETTING2_FLD_RDMA3_ISSUE_REQ_THRESHOLD, (reg32))

#define OVL_RDMA_BURST_CON0_GET_BURST_128B_BOUND(reg32)                                 \
                REG_FLD_GET(OVL_RDMA_BURST_CON0_FLD_BURST_128B_BOUND, (reg32))
#define OVL_RDMA_BURST_CON0_GET_BURST15A_32B(reg32)                                     \
                REG_FLD_GET(OVL_RDMA_BURST_CON0_FLD_BURST15A_32B, (reg32))
#define OVL_RDMA_BURST_CON0_GET_BURST14A_32B(reg32)                                     \
                REG_FLD_GET(OVL_RDMA_BURST_CON0_FLD_BURST14A_32B, (reg32))
#define OVL_RDMA_BURST_CON0_GET_BURST13A_32B(reg32)                                     \
                REG_FLD_GET(OVL_RDMA_BURST_CON0_FLD_BURST13A_32B, (reg32))
#define OVL_RDMA_BURST_CON0_GET_BURST12A_32B(reg32)                                     \
                REG_FLD_GET(OVL_RDMA_BURST_CON0_FLD_BURST12A_32B, (reg32))
#define OVL_RDMA_BURST_CON0_GET_BURST11A_32B(reg32)                                     \
                REG_FLD_GET(OVL_RDMA_BURST_CON0_FLD_BURST11A_32B, (reg32))
#define OVL_RDMA_BURST_CON0_GET_BURST10A_32B(reg32)                                     \
                REG_FLD_GET(OVL_RDMA_BURST_CON0_FLD_BURST10A_32B, (reg32))
#define OVL_RDMA_BURST_CON0_GET_BURST9A_32B(reg32)                                      \
                REG_FLD_GET(OVL_RDMA_BURST_CON0_FLD_BURST9A_32B, (reg32))

#define OVL_RDMA_BURST_CON1_GET_BURST15A_N32B(reg32)                                    \
                REG_FLD_GET(OVL_RDMA_BURST_CON1_FLD_BURST15A_N32B, (reg32))
#define OVL_RDMA_BURST_CON1_GET_BURST14A_N32B(reg32)                                    \
                REG_FLD_GET(OVL_RDMA_BURST_CON1_FLD_BURST14A_N32B, (reg32))
#define OVL_RDMA_BURST_CON1_GET_BURST13A_N32B(reg32)                                    \
                REG_FLD_GET(OVL_RDMA_BURST_CON1_FLD_BURST13A_N32B, (reg32))
#define OVL_RDMA_BURST_CON1_GET_BURST12A_N32B(reg32)                                    \
                REG_FLD_GET(OVL_RDMA_BURST_CON1_FLD_BURST12A_N32B, (reg32))
#define OVL_RDMA_BURST_CON1_GET_BURST11A_N32B(reg32)                                    \
                REG_FLD_GET(OVL_RDMA_BURST_CON1_FLD_BURST11A_N32B, (reg32))
#define OVL_RDMA_BURST_CON1_GET_BURST10A_N32B(reg32)                                    \
                REG_FLD_GET(OVL_RDMA_BURST_CON1_FLD_BURST10A_N32B, (reg32))
#define OVL_RDMA_BURST_CON1_GET_BURST9A_N32B(reg32)                                     \
                REG_FLD_GET(OVL_RDMA_BURST_CON1_FLD_BURST9A_N32B, (reg32))

#define OVL_RDMA_GREQ_NUM_GET_IOBUF_FLUSH_ULTRA(reg32)                                  \
                REG_FLD_GET(OVL_RDMA_GREQ_NUM_FLD_IOBUF_FLUSH_ULTRA, (reg32))
#define OVL_RDMA_GREQ_NUM_GET_IOBUF_FLUSH_PREULTRA(reg32)                               \
                REG_FLD_GET(OVL_RDMA_GREQ_NUM_FLD_IOBUF_FLUSH_PREULTRA, (reg32))
#define OVL_RDMA_GREQ_NUM_GET_GRP_BRK_STOP(reg32)                                       \
                REG_FLD_GET(OVL_RDMA_GREQ_NUM_FLD_GRP_BRK_STOP, (reg32))
#define OVL_RDMA_GREQ_NUM_GET_GRP_END_STOP(reg32)                                       \
                REG_FLD_GET(OVL_RDMA_GREQ_NUM_FLD_GRP_END_STOP, (reg32))
#define OVL_RDMA_GREQ_NUM_GET_GREQ_STOP_EN(reg32)                                       \
                REG_FLD_GET(OVL_RDMA_GREQ_NUM_FLD_GREQ_STOP_EN, (reg32))
#define OVL_RDMA_GREQ_NUM_GET_GREQ_DIS_CNT(reg32)                                       \
                REG_FLD_GET(OVL_RDMA_GREQ_NUM_FLD_GREQ_DIS_CNT, (reg32))
#define OVL_RDMA_GREQ_NUM_GET_OSTD_GREQ_NUM(reg32)                                      \
                REG_FLD_GET(OVL_RDMA_GREQ_NUM_FLD_OSTD_GREQ_NUM, (reg32))
#define OVL_RDMA_GREQ_NUM_GET_LAYER3_GREQ_NUM(reg32)                                    \
                REG_FLD_GET(OVL_RDMA_GREQ_NUM_FLD_LAYER3_GREQ_NUM, (reg32))
#define OVL_RDMA_GREQ_NUM_GET_LAYER2_GREQ_NUM(reg32)                                    \
                REG_FLD_GET(OVL_RDMA_GREQ_NUM_FLD_LAYER2_GREQ_NUM, (reg32))
#define OVL_RDMA_GREQ_NUM_GET_LAYER1_GREQ_NUM(reg32)                                    \
                REG_FLD_GET(OVL_RDMA_GREQ_NUM_FLD_LAYER1_GREQ_NUM, (reg32))
#define OVL_RDMA_GREQ_NUM_GET_LAYER0_GREQ_NUM(reg32)                                    \
                REG_FLD_GET(OVL_RDMA_GREQ_NUM_FLD_LAYER0_GREQ_NUM, (reg32))

#define OVL_RDMA_GREQ_URG_NUM_GET_GREQ_NUM_SHT(reg32)                                   \
                REG_FLD_GET(OVL_RDMA_GREQ_URG_NUM_FLD_GREQ_NUM_SHT, (reg32))
#define OVL_RDMA_GREQ_URG_NUM_GET_GREQ_NUM_SHT_VAL(reg32)                               \
                REG_FLD_GET(OVL_RDMA_GREQ_URG_NUM_FLD_GREQ_NUM_SHT_VAL, (reg32))
#define OVL_RDMA_GREQ_URG_NUM_GET_ARG_URG_BIAS(reg32)                                   \
                REG_FLD_GET(OVL_RDMA_GREQ_URG_NUM_FLD_ARG_URG_BIAS, (reg32))
#define OVL_RDMA_GREQ_URG_NUM_GET_ARG_GREQ_URG_TH(reg32)                                \
                REG_FLD_GET(OVL_RDMA_GREQ_URG_NUM_FLD_ARG_GREQ_URG_TH, (reg32))
#define OVL_RDMA_GREQ_URG_NUM_GET_LAYER3_GREQ_URG_NUM(reg32)                            \
                REG_FLD_GET(OVL_RDMA_GREQ_URG_NUM_FLD_LAYER3_GREQ_URG_NUM, (reg32))
#define OVL_RDMA_GREQ_URG_NUM_GET_LAYER2_GREQ_URG_NUM(reg32)                            \
                REG_FLD_GET(OVL_RDMA_GREQ_URG_NUM_FLD_LAYER2_GREQ_URG_NUM, (reg32))
#define OVL_RDMA_GREQ_URG_NUM_GET_LAYER1_GREQ_URG_NUM(reg32)                            \
                REG_FLD_GET(OVL_RDMA_GREQ_URG_NUM_FLD_LAYER1_GREQ_URG_NUM, (reg32))
#define OVL_RDMA_GREQ_URG_NUM_GET_LAYER0_GREQ_URG_NUM(reg32)                            \
                REG_FLD_GET(OVL_RDMA_GREQ_URG_NUM_FLD_LAYER0_GREQ_URG_NUM, (reg32))

#define OVL_DUMMY_REG_GET_OVERLAY_DUMMY(reg32)                                          \
                REG_FLD_GET(OVL_DUMMY_REG_FLD_OVERLAY_DUMMY, (reg32))

#define OVL_GDRDY_PRD_GET_GDRDY_PRD(reg32)                                              \
                REG_FLD_GET(OVL_GDRDY_PRD_FLD_GDRDY_PRD, (reg32))

#define OVL_RDMA_ULTRA_SRC_GET_ULTRA_RDMA_SRC(reg32)                                    \
                REG_FLD_GET(OVL_RDMA_ULTRA_SRC_FLD_ULTRA_RDMA_SRC, (reg32))
#define OVL_RDMA_ULTRA_SRC_GET_ULTRA_ROI_END_SRC(reg32)                                 \
                REG_FLD_GET(OVL_RDMA_ULTRA_SRC_FLD_ULTRA_ROI_END_SRC, (reg32))
#define OVL_RDMA_ULTRA_SRC_GET_ULTRA_SMI_SRC(reg32)                                     \
                REG_FLD_GET(OVL_RDMA_ULTRA_SRC_FLD_ULTRA_SMI_SRC, (reg32))
#define OVL_RDMA_ULTRA_SRC_GET_ULTRA_BUF_SRC(reg32)                                     \
                REG_FLD_GET(OVL_RDMA_ULTRA_SRC_FLD_ULTRA_BUF_SRC, (reg32))
#define OVL_RDMA_ULTRA_SRC_GET_PREULTRA_RDMA_SRC(reg32)                                 \
                REG_FLD_GET(OVL_RDMA_ULTRA_SRC_FLD_PREULTRA_RDMA_SRC, (reg32))
#define OVL_RDMA_ULTRA_SRC_GET_PREULTRA_ROI_END_SRC(reg32)                              \
                REG_FLD_GET(OVL_RDMA_ULTRA_SRC_FLD_PREULTRA_ROI_END_SRC, (reg32))
#define OVL_RDMA_ULTRA_SRC_GET_PREULTRA_SMI_SRC(reg32)                                  \
                REG_FLD_GET(OVL_RDMA_ULTRA_SRC_FLD_PREULTRA_SMI_SRC, (reg32))
#define OVL_RDMA_ULTRA_SRC_GET_PREULTRA_BUF_SRC(reg32)                                  \
                REG_FLD_GET(OVL_RDMA_ULTRA_SRC_FLD_PREULTRA_BUF_SRC, (reg32))

#define OVL_RDMA0_BUF_LOW_TH_GET_RDMA0_PREULTRA_LOW_TH(reg32)                           \
                REG_FLD_GET(OVL_RDMA0_BUF_LOW_TH_FLD_RDMA0_PREULTRA_LOW_TH, (reg32))
#define OVL_RDMA0_BUF_LOW_TH_GET_RDMA0_ULTRA_LOW_TH(reg32)                              \
                REG_FLD_GET(OVL_RDMA0_BUF_LOW_TH_FLD_RDMA0_ULTRA_LOW_TH, (reg32))

#define OVL_RDMA1_BUF_LOW_TH_GET_RDMA1_PREULTRA_LOW_TH(reg32)                           \
                REG_FLD_GET(OVL_RDMA1_BUF_LOW_TH_FLD_RDMA1_PREULTRA_LOW_TH, (reg32))
#define OVL_RDMA1_BUF_LOW_TH_GET_RDMA1_ULTRA_LOW_TH(reg32)                              \
                REG_FLD_GET(OVL_RDMA1_BUF_LOW_TH_FLD_RDMA1_ULTRA_LOW_TH, (reg32))

#define OVL_RDMA2_BUF_LOW_TH_GET_RDMA2_PREULTRA_LOW_TH(reg32)                           \
                REG_FLD_GET(OVL_RDMA2_BUF_LOW_TH_FLD_RDMA2_PREULTRA_LOW_TH, (reg32))
#define OVL_RDMA2_BUF_LOW_TH_GET_RDMA2_ULTRA_LOW_TH(reg32)                              \
                REG_FLD_GET(OVL_RDMA2_BUF_LOW_TH_FLD_RDMA2_ULTRA_LOW_TH, (reg32))

#define OVL_RDMA3_BUF_LOW_TH_GET_RDMA3_PREULTRA_LOW_TH(reg32)                           \
                REG_FLD_GET(OVL_RDMA3_BUF_LOW_TH_FLD_RDMA3_PREULTRA_LOW_TH, (reg32))
#define OVL_RDMA3_BUF_LOW_TH_GET_RDMA3_ULTRA_LOW_TH(reg32)                              \
                REG_FLD_GET(OVL_RDMA3_BUF_LOW_TH_FLD_RDMA3_ULTRA_LOW_TH, (reg32))

#define OVL_RDMA0_BUF_HIGH_TH_GET_RDMA0_PREULTRA_HIGH_DIS(reg32)                        \
                REG_FLD_GET(OVL_RDMA0_BUF_HIGH_TH_FLD_RDMA0_PREULTRA_HIGH_DIS, (reg32))
#define OVL_RDMA0_BUF_HIGH_TH_GET_RDMA0_PREULTRA_HIGH_TH(reg32)                         \
                REG_FLD_GET(OVL_RDMA0_BUF_HIGH_TH_FLD_RDMA0_PREULTRA_HIGH_TH, (reg32))

#define OVL_RDMA1_BUF_HIGH_TH_GET_RDMA1_PREULTRA_HIGH_DIS(reg32)                        \
                REG_FLD_GET(OVL_RDMA1_BUF_HIGH_TH_FLD_RDMA1_PREULTRA_HIGH_DIS, (reg32))
#define OVL_RDMA1_BUF_HIGH_TH_GET_RDMA1_PREULTRA_HIGH_TH(reg32)                         \
                REG_FLD_GET(OVL_RDMA1_BUF_HIGH_TH_FLD_RDMA1_PREULTRA_HIGH_TH, (reg32))

#define OVL_RDMA2_BUF_HIGH_TH_GET_RDMA2_PREULTRA_HIGH_DIS(reg32)                        \
                REG_FLD_GET(OVL_RDMA2_BUF_HIGH_TH_FLD_RDMA2_PREULTRA_HIGH_DIS, (reg32))
#define OVL_RDMA2_BUF_HIGH_TH_GET_RDMA2_PREULTRA_HIGH_TH(reg32)                         \
                REG_FLD_GET(OVL_RDMA2_BUF_HIGH_TH_FLD_RDMA2_PREULTRA_HIGH_TH, (reg32))

#define OVL_RDMA3_BUF_HIGH_TH_GET_RDMA3_PREULTRA_HIGH_DIS(reg32)                        \
                REG_FLD_GET(OVL_RDMA3_BUF_HIGH_TH_FLD_RDMA3_PREULTRA_HIGH_DIS, (reg32))
#define OVL_RDMA3_BUF_HIGH_TH_GET_RDMA3_PREULTRA_HIGH_TH(reg32)                         \
                REG_FLD_GET(OVL_RDMA3_BUF_HIGH_TH_FLD_RDMA3_PREULTRA_HIGH_TH, (reg32))

#define OVL_SMI_DBG_GET_SMI_FSM(reg32)                                                  \
                REG_FLD_GET(OVL_SMI_DBG_FLD_SMI_FSM, (reg32))

#define OVL_GREQ_LAYER_CNT_GET_LAYER3_GREQ_CNT(reg32)                                   \
                REG_FLD_GET(OVL_GREQ_LAYER_CNT_FLD_LAYER3_GREQ_CNT, (reg32))
#define OVL_GREQ_LAYER_CNT_GET_LAYER2_GREQ_CNT(reg32)                                   \
                REG_FLD_GET(OVL_GREQ_LAYER_CNT_FLD_LAYER2_GREQ_CNT, (reg32))
#define OVL_GREQ_LAYER_CNT_GET_LAYER1_GREQ_CNT(reg32)                                   \
                REG_FLD_GET(OVL_GREQ_LAYER_CNT_FLD_LAYER1_GREQ_CNT, (reg32))
#define OVL_GREQ_LAYER_CNT_GET_LAYER0_GREQ_CNT(reg32)                                   \
                REG_FLD_GET(OVL_GREQ_LAYER_CNT_FLD_LAYER0_GREQ_CNT, (reg32))

#define OVL_GDRDY_PRD_NUM_GET_GDRDY_PRD_NUM(reg32)                                      \
                REG_FLD_GET(OVL_GDRDY_PRD_NUM_FLD_GDRDY_PRD_NUM, (reg32))

#define OVL_FLOW_CTRL_DBG_GET_OVL_UPD_REG(reg32)                                        \
                REG_FLD_GET(OVL_FLOW_CTRL_DBG_FLD_OVL_UPD_REG, (reg32))
#define OVL_FLOW_CTRL_DBG_GET_REG_UPDATE(reg32)                                         \
                REG_FLD_GET(OVL_FLOW_CTRL_DBG_FLD_REG_UPDATE, (reg32))
#define OVL_FLOW_CTRL_DBG_GET_OVL_CLR(reg32)                                            \
                REG_FLD_GET(OVL_FLOW_CTRL_DBG_FLD_OVL_CLR, (reg32))
#define OVL_FLOW_CTRL_DBG_GET_OVL_START(reg32)                                          \
                REG_FLD_GET(OVL_FLOW_CTRL_DBG_FLD_OVL_START, (reg32))
#define OVL_FLOW_CTRL_DBG_GET_OVL_RUNNING(reg32)                                        \
                REG_FLD_GET(OVL_FLOW_CTRL_DBG_FLD_OVL_RUNNING, (reg32))
#define OVL_FLOW_CTRL_DBG_GET_FRAME_DONE(reg32)                                         \
                REG_FLD_GET(OVL_FLOW_CTRL_DBG_FLD_FRAME_DONE, (reg32))
#define OVL_FLOW_CTRL_DBG_GET_FRAME_UNDERRUN(reg32)                                     \
                REG_FLD_GET(OVL_FLOW_CTRL_DBG_FLD_FRAME_UNDERRUN, (reg32))
#define OVL_FLOW_CTRL_DBG_GET_FRAME_SWRST_DONE(reg32)                                   \
                REG_FLD_GET(OVL_FLOW_CTRL_DBG_FLD_FRAME_SWRST_DONE, (reg32))
#define OVL_FLOW_CTRL_DBG_GET_FRAME_HWRST_DONE(reg32)                                   \
                REG_FLD_GET(OVL_FLOW_CTRL_DBG_FLD_FRAME_HWRST_DONE, (reg32))
#define OVL_FLOW_CTRL_DBG_GET_TRIG(reg32)                                               \
                REG_FLD_GET(OVL_FLOW_CTRL_DBG_FLD_TRIG, (reg32))
#define OVL_FLOW_CTRL_DBG_GET_RST(reg32)                                                \
                REG_FLD_GET(OVL_FLOW_CTRL_DBG_FLD_RST, (reg32))
#define OVL_FLOW_CTRL_DBG_GET_RDMA0_IDLE(reg32)                                         \
                REG_FLD_GET(OVL_FLOW_CTRL_DBG_FLD_RDMA0_IDLE, (reg32))
#define OVL_FLOW_CTRL_DBG_GET_RDMA1_IDLE(reg32)                                         \
                REG_FLD_GET(OVL_FLOW_CTRL_DBG_FLD_RDMA1_IDLE, (reg32))
#define OVL_FLOW_CTRL_DBG_GET_RDMA2_IDLE(reg32)                                         \
                REG_FLD_GET(OVL_FLOW_CTRL_DBG_FLD_RDMA2_IDLE, (reg32))
#define OVL_FLOW_CTRL_DBG_GET_RDMA3_IDLE(reg32)                                         \
                REG_FLD_GET(OVL_FLOW_CTRL_DBG_FLD_RDMA3_IDLE, (reg32))
#define OVL_FLOW_CTRL_DBG_GET_OUT_IDLE(reg32)                                           \
                REG_FLD_GET(OVL_FLOW_CTRL_DBG_FLD_OUT_IDLE, (reg32))
#define OVL_FLOW_CTRL_DBG_GET_OVL_OUT_READY(reg32)                                      \
                REG_FLD_GET(OVL_FLOW_CTRL_DBG_FLD_OVL_OUT_READY, (reg32))
#define OVL_FLOW_CTRL_DBG_GET_OVL_OUT_VALID(reg32)                                      \
                REG_FLD_GET(OVL_FLOW_CTRL_DBG_FLD_OVL_OUT_VALID, (reg32))
#define OVL_FLOW_CTRL_DBG_GET_BLEND_IDLE(reg32)                                         \
                REG_FLD_GET(OVL_FLOW_CTRL_DBG_FLD_BLEND_IDLE, (reg32))
#define OVL_FLOW_CTRL_DBG_GET_ADDCON_IDLE(reg32)                                        \
                REG_FLD_GET(OVL_FLOW_CTRL_DBG_FLD_ADDCON_IDLE, (reg32))
#define OVL_FLOW_CTRL_DBG_GET_FSM_STATE(reg32)                                          \
                REG_FLD_GET(OVL_FLOW_CTRL_DBG_FLD_FSM_STATE, (reg32))

#define OVL_ADDCON_DBG_GET_L3_WIN_HIT(reg32)    REG_FLD_GET(OVL_ADDCON_DBG_FLD_L3_WIN_HIT, (reg32))
#define OVL_ADDCON_DBG_GET_L2_WIN_HIT(reg32)    REG_FLD_GET(OVL_ADDCON_DBG_FLD_L2_WIN_HIT, (reg32))
#define OVL_ADDCON_DBG_GET_ROI_Y(reg32)         REG_FLD_GET(OVL_ADDCON_DBG_FLD_ROI_Y, (reg32))
#define OVL_ADDCON_DBG_GET_L1_WIN_HIT(reg32)    REG_FLD_GET(OVL_ADDCON_DBG_FLD_L1_WIN_HIT, (reg32))
#define OVL_ADDCON_DBG_GET_L0_WIN_HIT(reg32)    REG_FLD_GET(OVL_ADDCON_DBG_FLD_L0_WIN_HIT, (reg32))
#define OVL_ADDCON_DBG_GET_ROI_X(reg32)         REG_FLD_GET(OVL_ADDCON_DBG_FLD_ROI_X, (reg32))

#define OVL_RDMA0_DBG_GET_SMI_GREQ(reg32)       REG_FLD_GET(OVL_RDMA0_DBG_FLD_SMI_GREQ, (reg32))
#define OVL_RDMA0_DBG_GET_RDMA0_SMI_BUSY(reg32)                                         \
                REG_FLD_GET(OVL_RDMA0_DBG_FLD_RDMA0_SMI_BUSY, (reg32))
#define OVL_RDMA0_DBG_GET_RDMA0_OUT_VALID(reg32)                                        \
                REG_FLD_GET(OVL_RDMA0_DBG_FLD_RDMA0_OUT_VALID, (reg32))
#define OVL_RDMA0_DBG_GET_RDMA0_OUT_READY(reg32)                                        \
                REG_FLD_GET(OVL_RDMA0_DBG_FLD_RDMA0_OUT_READY, (reg32))
#define OVL_RDMA0_DBG_GET_RDMA0_OUT_DATA(reg32)                                         \
                REG_FLD_GET(OVL_RDMA0_DBG_FLD_RDMA0_OUT_DATA, (reg32))
#define OVL_RDMA0_DBG_GET_RDMA0_LAYER_GREQ(reg32)                                       \
                REG_FLD_GET(OVL_RDMA0_DBG_FLD_RDMA0_LAYER_GREQ, (reg32))
#define OVL_RDMA0_DBG_GET_RDMA0_WRAM_RST_CS(reg32)                                      \
                REG_FLD_GET(OVL_RDMA0_DBG_FLD_RDMA0_WRAM_RST_CS, (reg32))

#define OVL_RDMA1_DBG_GET_SMI_GREQ(reg32)       REG_FLD_GET(OVL_RDMA1_DBG_FLD_SMI_GREQ, (reg32))
#define OVL_RDMA1_DBG_GET_RDMA1_SMI_BUSY(reg32)                                         \
                REG_FLD_GET(OVL_RDMA1_DBG_FLD_RDMA1_SMI_BUSY, (reg32))
#define OVL_RDMA1_DBG_GET_RDMA1_OUT_VALID(reg32)                                        \
                REG_FLD_GET(OVL_RDMA1_DBG_FLD_RDMA1_OUT_VALID, (reg32))
#define OVL_RDMA1_DBG_GET_RDMA1_OUT_READY(reg32)                                        \
                REG_FLD_GET(OVL_RDMA1_DBG_FLD_RDMA1_OUT_READY, (reg32))
#define OVL_RDMA1_DBG_GET_RDMA1_OUT_DATA(reg32)                                         \
                REG_FLD_GET(OVL_RDMA1_DBG_FLD_RDMA1_OUT_DATA, (reg32))
#define OVL_RDMA1_DBG_GET_RDMA1_LAYER_GREQ(reg32)                                       \
                REG_FLD_GET(OVL_RDMA1_DBG_FLD_RDMA1_LAYER_GREQ, (reg32))
#define OVL_RDMA1_DBG_GET_RDMA1_WRAM_RST_CS(reg32)                                      \
                REG_FLD_GET(OVL_RDMA1_DBG_FLD_RDMA1_WRAM_RST_CS, (reg32))

#define OVL_RDMA2_DBG_GET_SMI_GREQ(reg32)       REG_FLD_GET(OVL_RDMA2_DBG_FLD_SMI_GREQ, (reg32))
#define OVL_RDMA2_DBG_GET_RDMA2_SMI_BUSY(reg32)                                         \
                REG_FLD_GET(OVL_RDMA2_DBG_FLD_RDMA2_SMI_BUSY, (reg32))
#define OVL_RDMA2_DBG_GET_RDMA2_OUT_VALID(reg32)                                        \
                REG_FLD_GET(OVL_RDMA2_DBG_FLD_RDMA2_OUT_VALID, (reg32))
#define OVL_RDMA2_DBG_GET_RDMA2_OUT_READY(reg32)                                        \
                REG_FLD_GET(OVL_RDMA2_DBG_FLD_RDMA2_OUT_READY, (reg32))
#define OVL_RDMA2_DBG_GET_RDMA2_OUT_DATA(reg32)                                         \
                REG_FLD_GET(OVL_RDMA2_DBG_FLD_RDMA2_OUT_DATA, (reg32))
#define OVL_RDMA2_DBG_GET_RDMA2_LAYER_GREQ(reg32)                                       \
                REG_FLD_GET(OVL_RDMA2_DBG_FLD_RDMA2_LAYER_GREQ, (reg32))
#define OVL_RDMA2_DBG_GET_RDMA2_WRAM_RST_CS(reg32)                                      \
                REG_FLD_GET(OVL_RDMA2_DBG_FLD_RDMA2_WRAM_RST_CS, (reg32))

#define OVL_RDMA3_DBG_GET_SMI_GREQ(reg32)       REG_FLD_GET(OVL_RDMA3_DBG_FLD_SMI_GREQ, (reg32))
#define OVL_RDMA3_DBG_GET_RDMA3_SMI_BUSY(reg32)                                        \
                REG_FLD_GET(OVL_RDMA3_DBG_FLD_RDMA3_SMI_BUSY, (reg32))
#define OVL_RDMA3_DBG_GET_RDMA3_OUT_VALID(reg32)                                       \
                REG_FLD_GET(OVL_RDMA3_DBG_FLD_RDMA3_OUT_VALID, (reg32))
#define OVL_RDMA3_DBG_GET_RDMA3_OUT_READY(reg32)                                       \
                REG_FLD_GET(OVL_RDMA3_DBG_FLD_RDMA3_OUT_READY, (reg32))
#define OVL_RDMA3_DBG_GET_RDMA3_OUT_DATA(reg32)                                        \
                REG_FLD_GET(OVL_RDMA3_DBG_FLD_RDMA3_OUT_DATA, (reg32))
#define OVL_RDMA3_DBG_GET_RDMA3_LAYER_GREQ(reg32)                                      \
                REG_FLD_GET(OVL_RDMA3_DBG_FLD_RDMA3_LAYER_GREQ, (reg32))
#define OVL_RDMA3_DBG_GET_RDMA3_WRAM_RST_CS(reg32)                                     \
                REG_FLD_GET(OVL_RDMA3_DBG_FLD_RDMA3_WRAM_RST_CS, (reg32))

#define OVL_L0_CLR_GET_ALPHA(reg32)             REG_FLD_GET(OVL_L0_CLR_FLD_ALPHA, (reg32))
#define OVL_L0_CLR_GET_RED(reg32)               REG_FLD_GET(OVL_L0_CLR_FLD_RED, (reg32))
#define OVL_L0_CLR_GET_GREEN(reg32)             REG_FLD_GET(OVL_L0_CLR_FLD_GREEN, (reg32))
#define OVL_L0_CLR_GET_BLUE(reg32)              REG_FLD_GET(OVL_L0_CLR_FLD_BLUE, (reg32))

#define OVL_L1_CLR_GET_ALPHA(reg32)             REG_FLD_GET(OVL_L1_CLR_FLD_ALPHA, (reg32))
#define OVL_L1_CLR_GET_RED(reg32)               REG_FLD_GET(OVL_L1_CLR_FLD_RED, (reg32))
#define OVL_L1_CLR_GET_GREEN(reg32)             REG_FLD_GET(OVL_L1_CLR_FLD_GREEN, (reg32))
#define OVL_L1_CLR_GET_BLUE(reg32)              REG_FLD_GET(OVL_L1_CLR_FLD_BLUE, (reg32))

#define OVL_L2_CLR_GET_ALPHA(reg32)             REG_FLD_GET(OVL_L2_CLR_FLD_ALPHA, (reg32))
#define OVL_L2_CLR_GET_RED(reg32)               REG_FLD_GET(OVL_L2_CLR_FLD_RED, (reg32))
#define OVL_L2_CLR_GET_GREEN(reg32)             REG_FLD_GET(OVL_L2_CLR_FLD_GREEN, (reg32))
#define OVL_L2_CLR_GET_BLUE(reg32)              REG_FLD_GET(OVL_L2_CLR_FLD_BLUE, (reg32))

#define OVL_L3_CLR_GET_ALPHA(reg32)             REG_FLD_GET(OVL_L3_CLR_FLD_ALPHA, (reg32))
#define OVL_L3_CLR_GET_RED(reg32)               REG_FLD_GET(OVL_L3_CLR_FLD_RED, (reg32))
#define OVL_L3_CLR_GET_GREEN(reg32)             REG_FLD_GET(OVL_L3_CLR_FLD_GREEN, (reg32))
#define OVL_L3_CLR_GET_BLUE(reg32)              REG_FLD_GET(OVL_L3_CLR_FLD_BLUE, (reg32))

#define OVL_LC_CLR_GET_ALPHA(reg32)             REG_FLD_GET(OVL_LC_CLR_FLD_ALPHA, (reg32))
#define OVL_LC_CLR_GET_RED(reg32)               REG_FLD_GET(OVL_LC_CLR_FLD_RED, (reg32))
#define OVL_LC_CLR_GET_GREEN(reg32)             REG_FLD_GET(OVL_LC_CLR_FLD_GREEN, (reg32))
#define OVL_LC_CLR_GET_BLUE(reg32)              REG_FLD_GET(OVL_LC_CLR_FLD_BLUE, (reg32))

#define OVL_CRC_GET_CRC_RDY(reg32)              REG_FLD_GET(OVL_CRC_FLD_CRC_RDY, (reg32))
#define OVL_CRC_GET_CRC_OUT(reg32)              REG_FLD_GET(OVL_CRC_FLD_CRC_OUT, (reg32))

#define OVL_LC_CON_GET_DSTKEY_EN(reg32)         REG_FLD_GET(OVL_LC_CON_FLD_DSTKEY_EN, (reg32))
#define OVL_LC_CON_GET_SRCKEY_EN(reg32)         REG_FLD_GET(OVL_LC_CON_FLD_SRCKEY_EN, (reg32))
#define OVL_LC_CON_GET_LAYER_SRC(reg32)         REG_FLD_GET(OVL_LC_CON_FLD_LAYER_SRC, (reg32))
#define OVL_LC_CON_GET_R_FIRST(reg32)           REG_FLD_GET(OVL_LC_CON_FLD_R_FIRST, (reg32))
#define OVL_LC_CON_GET_LANDSCAPE(reg32)         REG_FLD_GET(OVL_LC_CON_FLD_LANDSCAPE, (reg32))
#define OVL_LC_CON_GET_EN_3D(reg32)             REG_FLD_GET(OVL_LC_CON_FLD_EN_3D, (reg32))
#define OVL_LC_CON_GET_ALPHA_EN(reg32)          REG_FLD_GET(OVL_LC_CON_FLD_ALPHA_EN, (reg32))
#define OVL_LC_CON_GET_ALPHA(reg32)             REG_FLD_GET(OVL_LC_CON_FLD_ALPHA, (reg32))

#define OVL_LC_SRCKEY_GET_SRCKEY(reg32)         REG_FLD_GET(OVL_LC_SRCKEY_FLD_SRCKEY, (reg32))

#define OVL_LC_SRC_SIZE_GET_LC_SRC_H(reg32)     REG_FLD_GET(OVL_LC_SRC_SIZE_FLD_LC_SRC_H, (reg32))
#define OVL_LC_SRC_SIZE_GET_LC_SRC_W(reg32)     REG_FLD_GET(OVL_LC_SRC_SIZE_FLD_LC_SRC_W, (reg32))

#define OVL_LC_OFFSET_GET_LC_YOFF(reg32)        REG_FLD_GET(OVL_LC_OFFSET_FLD_LC_YOFF, (reg32))
#define OVL_LC_OFFSET_GET_LC_XOFF(reg32)        REG_FLD_GET(OVL_LC_OFFSET_FLD_LC_XOFF, (reg32))

#define OVL_LC_SRC_SEL_GET_SURFL_EN(reg32)      REG_FLD_GET(OVL_LC_SRC_SEL_FLD_SURFL_EN, (reg32))
#define OVL_LC_SRC_SEL_GET_LC_BLEND_RND_SHT(reg32)                                      \
                REG_FLD_GET(OVL_LC_SRC_SEL_FLD_LC_BLEND_RND_SHT, (reg32))
#define OVL_LC_SRC_SEL_GET_LC_SRGB_SEL_EXT2(reg32)                                      \
                REG_FLD_GET(OVL_LC_SRC_SEL_FLD_LC_SRGB_SEL_EXT2, (reg32))
#define OVL_LC_SRC_SEL_GET_LC_CONST_BLD(reg32)                                          \
                REG_FLD_GET(OVL_LC_SRC_SEL_FLD_LC_CONST_BLD, (reg32))
#define OVL_LC_SRC_SEL_GET_LC_DRGB_SEL_EXT(reg32)                                       \
                REG_FLD_GET(OVL_LC_SRC_SEL_FLD_LC_DRGB_SEL_EXT, (reg32))
#define OVL_LC_SRC_SEL_GET_LC_DA_SEL_EXT(reg32)                                         \
                REG_FLD_GET(OVL_LC_SRC_SEL_FLD_LC_DA_SEL_EXT, (reg32))
#define OVL_LC_SRC_SEL_GET_LC_SRGB_SEL_EXT(reg32)                                       \
                REG_FLD_GET(OVL_LC_SRC_SEL_FLD_LC_SRGB_SEL_EXT, (reg32))
#define OVL_LC_SRC_SEL_GET_LC_SA_SEL_EXT(reg32)                                         \
                REG_FLD_GET(OVL_LC_SRC_SEL_FLD_LC_SA_SEL_EXT, (reg32))
#define OVL_LC_SRC_SEL_GET_LC_DRGB_SEL(reg32)                                           \
                REG_FLD_GET(OVL_LC_SRC_SEL_FLD_LC_DRGB_SEL, (reg32))
#define OVL_LC_SRC_SEL_GET_LC_DA_SEL(reg32)                                             \
                REG_FLD_GET(OVL_LC_SRC_SEL_FLD_LC_DA_SEL, (reg32))
#define OVL_LC_SRC_SEL_GET_LC_SRGB_SEL(reg32)                                           \
                REG_FLD_GET(OVL_LC_SRC_SEL_FLD_LC_SRGB_SEL, (reg32))
#define OVL_LC_SRC_SEL_GET_LC_SA_SEL(reg32)                                             \
                REG_FLD_GET(OVL_LC_SRC_SEL_FLD_LC_SA_SEL, (reg32))
#define OVL_LC_SRC_SEL_GET_CONST_LAYER_SEL(reg32)                                       \
                REG_FLD_GET(OVL_LC_SRC_SEL_FLD_CONST_LAYER_SEL, (reg32))

#define OVL_BANK_CON_GET_OVL_BANK_CON(reg32)                                            \
                REG_FLD_GET(OVL_BANK_CON_FLD_OVL_BANK_CON, (reg32))

#define OVL_FUNC_DCM0_GET_OVL_FUNC_DCM0(reg32)                                          \
                REG_FLD_GET(OVL_FUNC_DCM0_FLD_OVL_FUNC_DCM0, (reg32))

#define OVL_FUNC_DCM1_GET_OVL_FUNC_DCM1(reg32)                                          \
                REG_FLD_GET(OVL_FUNC_DCM1_FLD_OVL_FUNC_DCM1, (reg32))

#define OVL_DVFS_L0_ROI_GET_L0_DVFS_ROI_BB(reg32)                                       \
                REG_FLD_GET(OVL_DVFS_L0_ROI_FLD_L0_DVFS_ROI_BB, (reg32))
#define OVL_DVFS_L0_ROI_GET_L0_DVFS_ROI_TB(reg32)                                       \
                REG_FLD_GET(OVL_DVFS_L0_ROI_FLD_L0_DVFS_ROI_TB, (reg32))

#define OVL_DVFS_L1_ROI_GET_L1_DVFS_ROI_BB(reg32)                                       \
                REG_FLD_GET(OVL_DVFS_L1_ROI_FLD_L1_DVFS_ROI_BB, (reg32))
#define OVL_DVFS_L1_ROI_GET_L1_DVFS_ROI_TB(reg32)                                       \
                REG_FLD_GET(OVL_DVFS_L1_ROI_FLD_L1_DVFS_ROI_TB, (reg32))

#define OVL_DVFS_L2_ROI_GET_L2_DVFS_ROI_BB(reg32)                                       \
                REG_FLD_GET(OVL_DVFS_L2_ROI_FLD_L2_DVFS_ROI_BB, (reg32))
#define OVL_DVFS_L2_ROI_GET_L2_DVFS_ROI_TB(reg32)                                       \
                REG_FLD_GET(OVL_DVFS_L2_ROI_FLD_L2_DVFS_ROI_TB, (reg32))

#define OVL_DVFS_L3_ROI_GET_L3_DVFS_ROI_BB(reg32)                                       \
                REG_FLD_GET(OVL_DVFS_L3_ROI_FLD_L3_DVFS_ROI_BB, (reg32))
#define OVL_DVFS_L3_ROI_GET_L3_DVFS_ROI_TB(reg32)                                       \
                REG_FLD_GET(OVL_DVFS_L3_ROI_FLD_L3_DVFS_ROI_TB, (reg32))

#define OVL_DVFS_EL0_ROI_GET_EL0_DVFS_ROI_BB(reg32)                                     \
                REG_FLD_GET(OVL_DVFS_EL0_ROI_FLD_EL0_DVFS_ROI_BB, (reg32))
#define OVL_DVFS_EL0_ROI_GET_EL0_DVFS_ROI_TB(reg32)                                     \
                REG_FLD_GET(OVL_DVFS_EL0_ROI_FLD_EL0_DVFS_ROI_TB, (reg32))

#define OVL_DVFS_EL1_ROI_GET_EL1_DVFS_ROI_BB(reg32)                                     \
                REG_FLD_GET(OVL_DVFS_EL1_ROI_FLD_EL1_DVFS_ROI_BB, (reg32))
#define OVL_DVFS_EL1_ROI_GET_EL1_DVFS_ROI_TB(reg32)                                     \
                REG_FLD_GET(OVL_DVFS_EL1_ROI_FLD_EL1_DVFS_ROI_TB, (reg32))

#define OVL_DVFS_EL2_ROI_GET_EL2_DVFS_ROI_BB(reg32)                                     \
                REG_FLD_GET(OVL_DVFS_EL2_ROI_FLD_EL2_DVFS_ROI_BB, (reg32))
#define OVL_DVFS_EL2_ROI_GET_EL2_DVFS_ROI_TB(reg32)                                     \
                REG_FLD_GET(OVL_DVFS_EL2_ROI_FLD_EL2_DVFS_ROI_TB, (reg32))

#define OVL_DATAPATH_EXT_CON_GET_EL2_LAYER_SEL(reg32)                                   \
                REG_FLD_GET(OVL_DATAPATH_EXT_CON_FLD_EL2_LAYER_SEL, (reg32))
#define OVL_DATAPATH_EXT_CON_GET_EL1_LAYER_SEL(reg32)                                   \
                REG_FLD_GET(OVL_DATAPATH_EXT_CON_FLD_EL1_LAYER_SEL, (reg32))
#define OVL_DATAPATH_EXT_CON_GET_EL0_LAYER_SEL(reg32)                                   \
                REG_FLD_GET(OVL_DATAPATH_EXT_CON_FLD_EL0_LAYER_SEL, (reg32))
#define OVL_DATAPATH_EXT_CON_GET_EL2_GPU_MODE(reg32)                                    \
                REG_FLD_GET(OVL_DATAPATH_EXT_CON_FLD_EL2_GPU_MODE, (reg32))
#define OVL_DATAPATH_EXT_CON_GET_EL1_GPU_MODE(reg32)                                    \
                REG_FLD_GET(OVL_DATAPATH_EXT_CON_FLD_EL1_GPU_MODE, (reg32))
#define OVL_DATAPATH_EXT_CON_GET_EL0_GPU_MODE(reg32)                                    \
                REG_FLD_GET(OVL_DATAPATH_EXT_CON_FLD_EL0_GPU_MODE, (reg32))
#define OVL_DATAPATH_EXT_CON_GET_EL2_EN(reg32)                                          \
                REG_FLD_GET(OVL_DATAPATH_EXT_CON_FLD_EL2_EN, (reg32))
#define OVL_DATAPATH_EXT_CON_GET_EL1_EN(reg32)                                          \
                REG_FLD_GET(OVL_DATAPATH_EXT_CON_FLD_EL1_EN, (reg32))
#define OVL_DATAPATH_EXT_CON_GET_EL0_EN(reg32)                                          \
                REG_FLD_GET(OVL_DATAPATH_EXT_CON_FLD_EL0_EN, (reg32))

#define OVL_EL0_CON_GET_DSTKEY_EN(reg32)        REG_FLD_GET(OVL_EL0_CON_FLD_DSTKEY_EN, (reg32))
#define OVL_EL0_CON_GET_SRCKEY_EN(reg32)        REG_FLD_GET(OVL_EL0_CON_FLD_SRCKEY_EN, (reg32))
#define OVL_EL0_CON_GET_LAYER_SRC(reg32)        REG_FLD_GET(OVL_EL0_CON_FLD_LAYER_SRC, (reg32))
#define OVL_EL0_CON_GET_MTX_EN(reg32)           REG_FLD_GET(OVL_EL0_CON_FLD_MTX_EN, (reg32))
#define OVL_EL0_CON_GET_MTX_AUTO_DIS(reg32)     REG_FLD_GET(OVL_EL0_CON_FLD_MTX_AUTO_DIS, (reg32))
#define OVL_EL0_CON_GET_RGB_SWAP(reg32)         REG_FLD_GET(OVL_EL0_CON_FLD_RGB_SWAP, (reg32))
#define OVL_EL0_CON_GET_BYTE_SWAP(reg32)        REG_FLD_GET(OVL_EL0_CON_FLD_BYTE_SWAP, (reg32))
#define OVL_EL0_CON_GET_CLRFMT_MAN(reg32)       REG_FLD_GET(OVL_EL0_CON_FLD_CLRFMT_MAN, (reg32))
#define OVL_EL0_CON_GET_R_FIRST(reg32)          REG_FLD_GET(OVL_EL0_CON_FLD_R_FIRST, (reg32))
#define OVL_EL0_CON_GET_LANDSCAPE(reg32)        REG_FLD_GET(OVL_EL0_CON_FLD_LANDSCAPE, (reg32))
#define OVL_EL0_CON_GET_EN_3D(reg32)            REG_FLD_GET(OVL_EL0_CON_FLD_EN_3D, (reg32))
#define OVL_EL0_CON_GET_INT_MTX_SEL(reg32)      REG_FLD_GET(OVL_EL0_CON_FLD_INT_MTX_SEL, (reg32))
#define OVL_EL0_CON_GET_CLRFMT(reg32)           REG_FLD_GET(OVL_EL0_CON_FLD_CLRFMT, (reg32))
#define OVL_EL0_CON_GET_EXT_MTX_EN(reg32)       REG_FLD_GET(OVL_EL0_CON_FLD_EXT_MTX_EN, (reg32))
#define OVL_EL0_CON_GET_HORIZONTAL_FLIP_EN(reg32)                                       \
                REG_FLD_GET(OVL_EL0_CON_FLD_HORIZONTAL_FLIP_EN, (reg32))
#define OVL_EL0_CON_GET_VERTICAL_FLIP_EN(reg32)                                         \
                REG_FLD_GET(OVL_EL0_CON_FLD_VERTICAL_FLIP_EN, (reg32))
#define OVL_EL0_CON_GET_ALPHA_EN(reg32)         REG_FLD_GET(OVL_EL0_CON_FLD_ALPHA_EN, (reg32))
#define OVL_EL0_CON_GET_ALPHA(reg32)            REG_FLD_GET(OVL_EL0_CON_FLD_ALPHA, (reg32))

#define OVL_EL0_SRCKEY_GET_SRCKEY(reg32)        REG_FLD_GET(OVL_EL0_SRCKEY_FLD_SRCKEY, (reg32))

#define OVL_EL0_SRC_SIZE_GET_EL0_SRC_H(reg32)                                           \
                REG_FLD_GET(OVL_EL0_SRC_SIZE_FLD_EL0_SRC_H, (reg32))
#define OVL_EL0_SRC_SIZE_GET_EL0_SRC_W(reg32)                                           \
                REG_FLD_GET(OVL_EL0_SRC_SIZE_FLD_EL0_SRC_W, (reg32))

#define OVL_EL0_OFFSET_GET_EL0_YOFF(reg32)      REG_FLD_GET(OVL_EL0_OFFSET_FLD_EL0_YOFF, (reg32))
#define OVL_EL0_OFFSET_GET_EL0_XOFF(reg32)      REG_FLD_GET(OVL_EL0_OFFSET_FLD_EL0_XOFF, (reg32))

#define OVL_EL0_PITCH_GET_SURFL_EN(reg32)       REG_FLD_GET(OVL_EL0_PITCH_FLD_SURFL_EN, (reg32))
#define OVL_EL0_PITCH_GET_EL0_BLEND_RND_SHT(reg32)                                      \
                REG_FLD_GET(OVL_EL0_PITCH_FLD_EL0_BLEND_RND_SHT, (reg32))
#define OVL_EL0_PITCH_GET_EL0_SRGB_SEL_EXT2(reg32)                                      \
                REG_FLD_GET(OVL_EL0_PITCH_FLD_EL0_SRGB_SEL_EXT2, (reg32))
#define OVL_EL0_PITCH_GET_EL0_CONST_BLD(reg32)                                          \
                REG_FLD_GET(OVL_EL0_PITCH_FLD_EL0_CONST_BLD, (reg32))
#define OVL_EL0_PITCH_GET_EL0_DRGB_SEL_EXT(reg32)                                       \
                REG_FLD_GET(OVL_EL0_PITCH_FLD_EL0_DRGB_SEL_EXT, (reg32))
#define OVL_EL0_PITCH_GET_EL0_DA_SEL_EXT(reg32)                                         \
                REG_FLD_GET(OVL_EL0_PITCH_FLD_EL0_DA_SEL_EXT, (reg32))
#define OVL_EL0_PITCH_GET_EL0_SRGB_SEL_EXT(reg32)                                       \
                REG_FLD_GET(OVL_EL0_PITCH_FLD_EL0_SRGB_SEL_EXT, (reg32))
#define OVL_EL0_PITCH_GET_EL0_SA_SEL_EXT(reg32)                                         \
                REG_FLD_GET(OVL_EL0_PITCH_FLD_EL0_SA_SEL_EXT, (reg32))
#define OVL_EL0_PITCH_GET_EL0_DRGB_SEL(reg32)                                           \
                REG_FLD_GET(OVL_EL0_PITCH_FLD_EL0_DRGB_SEL, (reg32))
#define OVL_EL0_PITCH_GET_EL0_DA_SEL(reg32)                                             \
                REG_FLD_GET(OVL_EL0_PITCH_FLD_EL0_DA_SEL, (reg32))
#define OVL_EL0_PITCH_GET_EL0_SRGB_SEL(reg32)                                           \
                REG_FLD_GET(OVL_EL0_PITCH_FLD_EL0_SRGB_SEL, (reg32))
#define OVL_EL0_PITCH_GET_EL0_SA_SEL(reg32)                                             \
                REG_FLD_GET(OVL_EL0_PITCH_FLD_EL0_SA_SEL, (reg32))
#define OVL_EL0_PITCH_GET_EL0_SRC_PITCH(reg32)                                          \
                REG_FLD_GET(OVL_EL0_PITCH_FLD_EL0_SRC_PITCH, (reg32))

#define OVL_EL0_TILE_GET_TILE_HORI_BLOCK_NUM(reg32)                                     \
                REG_FLD_GET(OVL_EL0_TILE_FLD_TILE_HORI_BLOCK_NUM, (reg32))
#define OVL_EL0_TILE_GET_TILE_EN(reg32)                                                 \
                REG_FLD_GET(OVL_EL0_TILE_FLD_TILE_EN, (reg32))
#define OVL_EL0_TILE_GET_TILE_WIDTH_SEL(reg32)                                          \
                REG_FLD_GET(OVL_EL0_TILE_FLD_TILE_WIDTH_SEL, (reg32))
#define OVL_EL0_TILE_GET_TILE_HEIGHT(reg32)                                             \
                REG_FLD_GET(OVL_EL0_TILE_FLD_TILE_HEIGHT, (reg32))

#define OVL_EL0_CLIP_GET_EL0_SRC_BOTTOM_CLIP(reg32)                                     \
                REG_FLD_GET(OVL_EL0_CLIP_FLD_EL0_SRC_BOTTOM_CLIP, (reg32))
#define OVL_EL0_CLIP_GET_EL0_SRC_TOP_CLIP(reg32)                                        \
                REG_FLD_GET(OVL_EL0_CLIP_FLD_EL0_SRC_TOP_CLIP, (reg32))
#define OVL_EL0_CLIP_GET_EL0_SRC_RIGHT_CLIP(reg32)                                      \
                REG_FLD_GET(OVL_EL0_CLIP_FLD_EL0_SRC_RIGHT_CLIP, (reg32))
#define OVL_EL0_CLIP_GET_EL0_SRC_LEFT_CLIP(reg32)                                       \
                REG_FLD_GET(OVL_EL0_CLIP_FLD_EL0_SRC_LEFT_CLIP, (reg32))

#define OVL_EL1_CON_GET_DSTKEY_EN(reg32)        REG_FLD_GET(OVL_EL1_CON_FLD_DSTKEY_EN, (reg32))
#define OVL_EL1_CON_GET_SRCKEY_EN(reg32)        REG_FLD_GET(OVL_EL1_CON_FLD_SRCKEY_EN, (reg32))
#define OVL_EL1_CON_GET_LAYER_SRC(reg32)        REG_FLD_GET(OVL_EL1_CON_FLD_LAYER_SRC, (reg32))
#define OVL_EL1_CON_GET_MTX_EN(reg32)           REG_FLD_GET(OVL_EL1_CON_FLD_MTX_EN, (reg32))
#define OVL_EL1_CON_GET_MTX_AUTO_DIS(reg32)     REG_FLD_GET(OVL_EL1_CON_FLD_MTX_AUTO_DIS, (reg32))
#define OVL_EL1_CON_GET_RGB_SWAP(reg32)         REG_FLD_GET(OVL_EL1_CON_FLD_RGB_SWAP, (reg32))
#define OVL_EL1_CON_GET_BYTE_SWAP(reg32)        REG_FLD_GET(OVL_EL1_CON_FLD_BYTE_SWAP, (reg32))
#define OVL_EL1_CON_GET_CLRFMT_MAN(reg32)       REG_FLD_GET(OVL_EL1_CON_FLD_CLRFMT_MAN, (reg32))
#define OVL_EL1_CON_GET_R_FIRST(reg32)          REG_FLD_GET(OVL_EL1_CON_FLD_R_FIRST, (reg32))
#define OVL_EL1_CON_GET_LANDSCAPE(reg32)        REG_FLD_GET(OVL_EL1_CON_FLD_LANDSCAPE, (reg32))
#define OVL_EL1_CON_GET_EN_3D(reg32)            REG_FLD_GET(OVL_EL1_CON_FLD_EN_3D, (reg32))
#define OVL_EL1_CON_GET_INT_MTX_SEL(reg32)      REG_FLD_GET(OVL_EL1_CON_FLD_INT_MTX_SEL, (reg32))
#define OVL_EL1_CON_GET_CLRFMT(reg32)           REG_FLD_GET(OVL_EL1_CON_FLD_CLRFMT, (reg32))
#define OVL_EL1_CON_GET_EXT_MTX_EN(reg32)       REG_FLD_GET(OVL_EL1_CON_FLD_EXT_MTX_EN, (reg32))
#define OVL_EL1_CON_GET_HORIZONTAL_FLIP_EN(reg32)                                       \
                REG_FLD_GET(OVL_EL1_CON_FLD_HORIZONTAL_FLIP_EN, (reg32))
#define OVL_EL1_CON_GET_VERTICAL_FLIP_EN(reg32)                                         \
                REG_FLD_GET(OVL_EL1_CON_FLD_VERTICAL_FLIP_EN, (reg32))
#define OVL_EL1_CON_GET_ALPHA_EN(reg32)         REG_FLD_GET(OVL_EL1_CON_FLD_ALPHA_EN, (reg32))
#define OVL_EL1_CON_GET_ALPHA(reg32)            REG_FLD_GET(OVL_EL1_CON_FLD_ALPHA, (reg32))

#define OVL_EL1_SRCKEY_GET_SRCKEY(reg32)        REG_FLD_GET(OVL_EL1_SRCKEY_FLD_SRCKEY, (reg32))

#define OVL_EL1_SRC_SIZE_GET_EL1_SRC_H(reg32)                                           \
                REG_FLD_GET(OVL_EL1_SRC_SIZE_FLD_EL1_SRC_H, (reg32))
#define OVL_EL1_SRC_SIZE_GET_EL1_SRC_W(reg32)                                           \
                REG_FLD_GET(OVL_EL1_SRC_SIZE_FLD_EL1_SRC_W, (reg32))

#define OVL_EL1_OFFSET_GET_EL1_YOFF(reg32)      REG_FLD_GET(OVL_EL1_OFFSET_FLD_EL1_YOFF, (reg32))
#define OVL_EL1_OFFSET_GET_EL1_XOFF(reg32)      REG_FLD_GET(OVL_EL1_OFFSET_FLD_EL1_XOFF, (reg32))

#define OVL_EL1_PITCH_GET_SURFL_EN(reg32)       REG_FLD_GET(OVL_EL1_PITCH_FLD_SURFL_EN, (reg32))
#define OVL_EL1_PITCH_GET_EL1_BLEND_RND_SHT(reg32)                                      \
                REG_FLD_GET(OVL_EL1_PITCH_FLD_EL1_BLEND_RND_SHT, (reg32))
#define OVL_EL1_PITCH_GET_EL1_SRGB_SEL_EXT2(reg32)                                      \
                REG_FLD_GET(OVL_EL1_PITCH_FLD_EL1_SRGB_SEL_EXT2, (reg32))
#define OVL_EL1_PITCH_GET_EL1_CONST_BLD(reg32)                                          \
                REG_FLD_GET(OVL_EL1_PITCH_FLD_EL1_CONST_BLD, (reg32))
#define OVL_EL1_PITCH_GET_EL1_DRGB_SEL_EXT(reg32)                                       \
                REG_FLD_GET(OVL_EL1_PITCH_FLD_EL1_DRGB_SEL_EXT, (reg32))
#define OVL_EL1_PITCH_GET_EL1_DA_SEL_EXT(reg32)                                         \
                REG_FLD_GET(OVL_EL1_PITCH_FLD_EL1_DA_SEL_EXT, (reg32))
#define OVL_EL1_PITCH_GET_EL1_SRGB_SEL_EXT(reg32)                                       \
                REG_FLD_GET(OVL_EL1_PITCH_FLD_EL1_SRGB_SEL_EXT, (reg32))
#define OVL_EL1_PITCH_GET_EL1_SA_SEL_EXT(reg32)                                         \
                REG_FLD_GET(OVL_EL1_PITCH_FLD_EL1_SA_SEL_EXT, (reg32))
#define OVL_EL1_PITCH_GET_EL1_DRGB_SEL(reg32)                                           \
                REG_FLD_GET(OVL_EL1_PITCH_FLD_EL1_DRGB_SEL, (reg32))
#define OVL_EL1_PITCH_GET_EL1_DA_SEL(reg32)                                             \
                REG_FLD_GET(OVL_EL1_PITCH_FLD_EL1_DA_SEL, (reg32))
#define OVL_EL1_PITCH_GET_EL1_SRGB_SEL(reg32)                                           \
                REG_FLD_GET(OVL_EL1_PITCH_FLD_EL1_SRGB_SEL, (reg32))
#define OVL_EL1_PITCH_GET_EL1_SA_SEL(reg32)                                             \
                REG_FLD_GET(OVL_EL1_PITCH_FLD_EL1_SA_SEL, (reg32))
#define OVL_EL1_PITCH_GET_EL1_SRC_PITCH(reg32)                                          \
                REG_FLD_GET(OVL_EL1_PITCH_FLD_EL1_SRC_PITCH, (reg32))

#define OVL_EL1_TILE_GET_TILE_HORI_BLOCK_NUM(reg32)                                     \
                REG_FLD_GET(OVL_EL1_TILE_FLD_TILE_HORI_BLOCK_NUM, (reg32))
#define OVL_EL1_TILE_GET_TILE_EN(reg32)                                                 \
                REG_FLD_GET(OVL_EL1_TILE_FLD_TILE_EN, (reg32))
#define OVL_EL1_TILE_GET_TILE_WIDTH_SEL(reg32)                                          \
                REG_FLD_GET(OVL_EL1_TILE_FLD_TILE_WIDTH_SEL, (reg32))
#define OVL_EL1_TILE_GET_TILE_HEIGHT(reg32)                                             \
                REG_FLD_GET(OVL_EL1_TILE_FLD_TILE_HEIGHT, (reg32))

#define OVL_EL1_CLIP_GET_EL1_SRC_BOTTOM_CLIP(reg32)                                     \
                REG_FLD_GET(OVL_EL1_CLIP_FLD_EL1_SRC_BOTTOM_CLIP, (reg32))
#define OVL_EL1_CLIP_GET_EL1_SRC_TOP_CLIP(reg32)                                        \
                REG_FLD_GET(OVL_EL1_CLIP_FLD_EL1_SRC_TOP_CLIP, (reg32))
#define OVL_EL1_CLIP_GET_EL1_SRC_RIGHT_CLIP(reg32)                                      \
                REG_FLD_GET(OVL_EL1_CLIP_FLD_EL1_SRC_RIGHT_CLIP, (reg32))
#define OVL_EL1_CLIP_GET_EL1_SRC_LEFT_CLIP(reg32)                                       \
                REG_FLD_GET(OVL_EL1_CLIP_FLD_EL1_SRC_LEFT_CLIP, (reg32))

#define OVL_EL2_CON_GET_DSTKEY_EN(reg32)        REG_FLD_GET(OVL_EL2_CON_FLD_DSTKEY_EN, (reg32))
#define OVL_EL2_CON_GET_SRCKEY_EN(reg32)        REG_FLD_GET(OVL_EL2_CON_FLD_SRCKEY_EN, (reg32))
#define OVL_EL2_CON_GET_LAYER_SRC(reg32)        REG_FLD_GET(OVL_EL2_CON_FLD_LAYER_SRC, (reg32))
#define OVL_EL2_CON_GET_MTX_EN(reg32)           REG_FLD_GET(OVL_EL2_CON_FLD_MTX_EN, (reg32))
#define OVL_EL2_CON_GET_MTX_AUTO_DIS(reg32)     REG_FLD_GET(OVL_EL2_CON_FLD_MTX_AUTO_DIS, (reg32))
#define OVL_EL2_CON_GET_RGB_SWAP(reg32)         REG_FLD_GET(OVL_EL2_CON_FLD_RGB_SWAP, (reg32))
#define OVL_EL2_CON_GET_BYTE_SWAP(reg32)        REG_FLD_GET(OVL_EL2_CON_FLD_BYTE_SWAP, (reg32))
#define OVL_EL2_CON_GET_CLRFMT_MAN(reg32)       REG_FLD_GET(OVL_EL2_CON_FLD_CLRFMT_MAN, (reg32))
#define OVL_EL2_CON_GET_R_FIRST(reg32)          REG_FLD_GET(OVL_EL2_CON_FLD_R_FIRST, (reg32))
#define OVL_EL2_CON_GET_LANDSCAPE(reg32)        REG_FLD_GET(OVL_EL2_CON_FLD_LANDSCAPE, (reg32))
#define OVL_EL2_CON_GET_EN_3D(reg32)            REG_FLD_GET(OVL_EL2_CON_FLD_EN_3D, (reg32))
#define OVL_EL2_CON_GET_INT_MTX_SEL(reg32)      REG_FLD_GET(OVL_EL2_CON_FLD_INT_MTX_SEL, (reg32))
#define OVL_EL2_CON_GET_CLRFMT(reg32)           REG_FLD_GET(OVL_EL2_CON_FLD_CLRFMT, (reg32))
#define OVL_EL2_CON_GET_EXT_MTX_EN(reg32)       REG_FLD_GET(OVL_EL2_CON_FLD_EXT_MTX_EN, (reg32))
#define OVL_EL2_CON_GET_HORIZONTAL_FLIP_EN(reg32)                                       \
                REG_FLD_GET(OVL_EL2_CON_FLD_HORIZONTAL_FLIP_EN, (reg32))
#define OVL_EL2_CON_GET_VERTICAL_FLIP_EN(reg32)                                         \
                REG_FLD_GET(OVL_EL2_CON_FLD_VERTICAL_FLIP_EN, (reg32))
#define OVL_EL2_CON_GET_ALPHA_EN(reg32)         REG_FLD_GET(OVL_EL2_CON_FLD_ALPHA_EN, (reg32))
#define OVL_EL2_CON_GET_ALPHA(reg32)            REG_FLD_GET(OVL_EL2_CON_FLD_ALPHA, (reg32))

#define OVL_EL2_SRCKEY_GET_SRCKEY(reg32)        REG_FLD_GET(OVL_EL2_SRCKEY_FLD_SRCKEY, (reg32))

#define OVL_EL2_SRC_SIZE_GET_EL2_SRC_H(reg32)   REG_FLD_GET(OVL_EL2_SRC_SIZE_FLD_EL2_SRC_H, (reg32))
#define OVL_EL2_SRC_SIZE_GET_EL2_SRC_W(reg32)   REG_FLD_GET(OVL_EL2_SRC_SIZE_FLD_EL2_SRC_W, (reg32))

#define OVL_EL2_OFFSET_GET_EL2_YOFF(reg32)      REG_FLD_GET(OVL_EL2_OFFSET_FLD_EL2_YOFF, (reg32))
#define OVL_EL2_OFFSET_GET_EL2_XOFF(reg32)      REG_FLD_GET(OVL_EL2_OFFSET_FLD_EL2_XOFF, (reg32))

#define OVL_EL2_PITCH_GET_SURFL_EN(reg32)       REG_FLD_GET(OVL_EL2_PITCH_FLD_SURFL_EN, (reg32))
#define OVL_EL2_PITCH_GET_EL2_BLEND_RND_SHT(reg32)                                      \
                REG_FLD_GET(OVL_EL2_PITCH_FLD_EL2_BLEND_RND_SHT, (reg32))
#define OVL_EL2_PITCH_GET_EL2_SRGB_SEL_EXT2(reg32)                                      \
                REG_FLD_GET(OVL_EL2_PITCH_FLD_EL2_SRGB_SEL_EXT2, (reg32))
#define OVL_EL2_PITCH_GET_EL2_CONST_BLD(reg32)                                          \
                REG_FLD_GET(OVL_EL2_PITCH_FLD_EL2_CONST_BLD, (reg32))
#define OVL_EL2_PITCH_GET_EL2_DRGB_SEL_EXT(reg32)                                       \
                REG_FLD_GET(OVL_EL2_PITCH_FLD_EL2_DRGB_SEL_EXT, (reg32))
#define OVL_EL2_PITCH_GET_EL2_DA_SEL_EXT(reg32)                                         \
                REG_FLD_GET(OVL_EL2_PITCH_FLD_EL2_DA_SEL_EXT, (reg32))
#define OVL_EL2_PITCH_GET_EL2_SRGB_SEL_EXT(reg32)                                       \
                REG_FLD_GET(OVL_EL2_PITCH_FLD_EL2_SRGB_SEL_EXT, (reg32))
#define OVL_EL2_PITCH_GET_EL2_SA_SEL_EXT(reg32)                                         \
                REG_FLD_GET(OVL_EL2_PITCH_FLD_EL2_SA_SEL_EXT, (reg32))
#define OVL_EL2_PITCH_GET_EL2_DRGB_SEL(reg32)                                           \
                REG_FLD_GET(OVL_EL2_PITCH_FLD_EL2_DRGB_SEL, (reg32))
#define OVL_EL2_PITCH_GET_EL2_DA_SEL(reg32)                                             \
                REG_FLD_GET(OVL_EL2_PITCH_FLD_EL2_DA_SEL, (reg32))
#define OVL_EL2_PITCH_GET_EL2_SRGB_SEL(reg32)                                           \
                REG_FLD_GET(OVL_EL2_PITCH_FLD_EL2_SRGB_SEL, (reg32))
#define OVL_EL2_PITCH_GET_EL2_SA_SEL(reg32)                                             \
                REG_FLD_GET(OVL_EL2_PITCH_FLD_EL2_SA_SEL, (reg32))
#define OVL_EL2_PITCH_GET_EL2_SRC_PITCH(reg32)                                          \
                REG_FLD_GET(OVL_EL2_PITCH_FLD_EL2_SRC_PITCH, (reg32))

#define OVL_EL2_TILE_GET_TILE_HORI_BLOCK_NUM(reg32)                                     \
                REG_FLD_GET(OVL_EL2_TILE_FLD_TILE_HORI_BLOCK_NUM, (reg32))
#define OVL_EL2_TILE_GET_TILE_EN(reg32)                                                 \
                REG_FLD_GET(OVL_EL2_TILE_FLD_TILE_EN, (reg32))
#define OVL_EL2_TILE_GET_TILE_WIDTH_SEL(reg32)                                          \
                REG_FLD_GET(OVL_EL2_TILE_FLD_TILE_WIDTH_SEL, (reg32))
#define OVL_EL2_TILE_GET_TILE_HEIGHT(reg32)                                             \
                REG_FLD_GET(OVL_EL2_TILE_FLD_TILE_HEIGHT, (reg32))

#define OVL_EL2_CLIP_GET_EL2_SRC_BOTTOM_CLIP(reg32)                                     \
                REG_FLD_GET(OVL_EL2_CLIP_FLD_EL2_SRC_BOTTOM_CLIP, (reg32))
#define OVL_EL2_CLIP_GET_EL2_SRC_TOP_CLIP(reg32)                                        \
                REG_FLD_GET(OVL_EL2_CLIP_FLD_EL2_SRC_TOP_CLIP, (reg32))
#define OVL_EL2_CLIP_GET_EL2_SRC_RIGHT_CLIP(reg32)                                      \
                REG_FLD_GET(OVL_EL2_CLIP_FLD_EL2_SRC_RIGHT_CLIP, (reg32))
#define OVL_EL2_CLIP_GET_EL2_SRC_LEFT_CLIP(reg32)                                       \
                REG_FLD_GET(OVL_EL2_CLIP_FLD_EL2_SRC_LEFT_CLIP, (reg32))

#define OVL_EL0_CLR_GET_ALPHA(reg32)            REG_FLD_GET(OVL_EL0_CLR_FLD_ALPHA, (reg32))
#define OVL_EL0_CLR_GET_RED(reg32)              REG_FLD_GET(OVL_EL0_CLR_FLD_RED, (reg32))
#define OVL_EL0_CLR_GET_GREEN(reg32)            REG_FLD_GET(OVL_EL0_CLR_FLD_GREEN, (reg32))
#define OVL_EL0_CLR_GET_BLUE(reg32)             REG_FLD_GET(OVL_EL0_CLR_FLD_BLUE, (reg32))

#define OVL_EL1_CLR_GET_ALPHA(reg32)            REG_FLD_GET(OVL_EL1_CLR_FLD_ALPHA, (reg32))
#define OVL_EL1_CLR_GET_RED(reg32)              REG_FLD_GET(OVL_EL1_CLR_FLD_RED, (reg32))
#define OVL_EL1_CLR_GET_GREEN(reg32)            REG_FLD_GET(OVL_EL1_CLR_FLD_GREEN, (reg32))
#define OVL_EL1_CLR_GET_BLUE(reg32)             REG_FLD_GET(OVL_EL1_CLR_FLD_BLUE, (reg32))

#define OVL_EL2_CLR_GET_ALPHA(reg32)            REG_FLD_GET(OVL_EL2_CLR_FLD_ALPHA, (reg32))
#define OVL_EL2_CLR_GET_RED(reg32)              REG_FLD_GET(OVL_EL2_CLR_FLD_RED, (reg32))
#define OVL_EL2_CLR_GET_GREEN(reg32)            REG_FLD_GET(OVL_EL2_CLR_FLD_GREEN, (reg32))
#define OVL_EL2_CLR_GET_BLUE(reg32)             REG_FLD_GET(OVL_EL2_CLR_FLD_BLUE, (reg32))

#define OVL_SBCH_GET_L3_SBCH_CNST_EN(reg32)     REG_FLD_GET(OVL_SBCH_FLD_L3_SBCH_CNST_EN, (reg32))
#define OVL_SBCH_GET_L3_SBCH_TRANS_EN(reg32)    REG_FLD_GET(OVL_SBCH_FLD_L3_SBCH_TRANS_EN, (reg32))
#define OVL_SBCH_GET_L2_SBCH_CNST_EN(reg32)     REG_FLD_GET(OVL_SBCH_FLD_L2_SBCH_CNST_EN, (reg32))
#define OVL_SBCH_GET_L2_SBCH_TRANS_EN(reg32)    REG_FLD_GET(OVL_SBCH_FLD_L2_SBCH_TRANS_EN, (reg32))
#define OVL_SBCH_GET_L1_SBCH_CNST_EN(reg32)     REG_FLD_GET(OVL_SBCH_FLD_L1_SBCH_CNST_EN, (reg32))
#define OVL_SBCH_GET_L1_SBCH_TRANS_EN(reg32)    REG_FLD_GET(OVL_SBCH_FLD_L1_SBCH_TRANS_EN, (reg32))
#define OVL_SBCH_GET_L0_SBCH_CNST_EN(reg32)     REG_FLD_GET(OVL_SBCH_FLD_L0_SBCH_CNST_EN, (reg32))
#define OVL_SBCH_GET_L0_SBCH_TRANS_EN(reg32)    REG_FLD_GET(OVL_SBCH_FLD_L0_SBCH_TRANS_EN, (reg32))
#define OVL_SBCH_GET_L3_SBCH_UPDATE(reg32)      REG_FLD_GET(OVL_SBCH_FLD_L3_SBCH_UPDATE, (reg32))
#define OVL_SBCH_GET_L2_SBCH_UPDATE(reg32)      REG_FLD_GET(OVL_SBCH_FLD_L2_SBCH_UPDATE, (reg32))
#define OVL_SBCH_GET_L1_SBCH_UPDATE(reg32)      REG_FLD_GET(OVL_SBCH_FLD_L1_SBCH_UPDATE, (reg32))
#define OVL_SBCH_GET_L0_SBCH_UPDATE(reg32)      REG_FLD_GET(OVL_SBCH_FLD_L0_SBCH_UPDATE, (reg32))

#define OVL_SBCH_EXT_GET_EL2_SBCH_CNST_EN(reg32)                                        \
                REG_FLD_GET(OVL_SBCH_EXT_FLD_EL2_SBCH_CNST_EN, (reg32))
#define OVL_SBCH_EXT_GET_EL2_SBCH_TRANS_EN(reg32)                                       \
                REG_FLD_GET(OVL_SBCH_EXT_FLD_EL2_SBCH_TRANS_EN, (reg32))
#define OVL_SBCH_EXT_GET_EL1_SBCH_CNST_EN(reg32)                                        \
                REG_FLD_GET(OVL_SBCH_EXT_FLD_EL1_SBCH_CNST_EN, (reg32))
#define OVL_SBCH_EXT_GET_EL1_SBCH_TRANS_EN(reg32)                                       \
                REG_FLD_GET(OVL_SBCH_EXT_FLD_EL1_SBCH_TRANS_EN, (reg32))
#define OVL_SBCH_EXT_GET_EL0_SBCH_CNST_EN(reg32)                                        \
                REG_FLD_GET(OVL_SBCH_EXT_FLD_EL0_SBCH_CNST_EN, (reg32))
#define OVL_SBCH_EXT_GET_EL0_SBCH_TRANS_EN(reg32)                                       \
                REG_FLD_GET(OVL_SBCH_EXT_FLD_EL0_SBCH_TRANS_EN, (reg32))
#define OVL_SBCH_EXT_GET_EL2_SBCH_UPDATE(reg32)                                         \
                REG_FLD_GET(OVL_SBCH_EXT_FLD_EL2_SBCH_UPDATE, (reg32))
#define OVL_SBCH_EXT_GET_EL1_SBCH_UPDATE(reg32)                                         \
                REG_FLD_GET(OVL_SBCH_EXT_FLD_EL1_SBCH_UPDATE, (reg32))
#define OVL_SBCH_EXT_GET_EL0_SBCH_UPDATE(reg32)                                         \
                REG_FLD_GET(OVL_SBCH_EXT_FLD_EL0_SBCH_UPDATE, (reg32))

#define OVL_SBCH_CON_GET_EL2_SBCH_TRANS_INVALID(reg32)                                  \
                REG_FLD_GET(OVL_SBCH_CON_FLD_EL2_SBCH_TRANS_INVALID, (reg32))
#define OVL_SBCH_CON_GET_EL1_SBCH_TRANS_INVALID(reg32)                                  \
                REG_FLD_GET(OVL_SBCH_CON_FLD_EL1_SBCH_TRANS_INVALID, (reg32))
#define OVL_SBCH_CON_GET_EL0_SBCH_TRANS_INVALID(reg32)                                  \
                REG_FLD_GET(OVL_SBCH_CON_FLD_EL0_SBCH_TRANS_INVALID, (reg32))
#define OVL_SBCH_CON_GET_L3_SBCH_TRANS_INVALID(reg32)                                   \
                REG_FLD_GET(OVL_SBCH_CON_FLD_L3_SBCH_TRANS_INVALID, (reg32))
#define OVL_SBCH_CON_GET_L2_SBCH_TRANS_INVALID(reg32)                                   \
                REG_FLD_GET(OVL_SBCH_CON_FLD_L2_SBCH_TRANS_INVALID, (reg32))
#define OVL_SBCH_CON_GET_L1_SBCH_TRANS_INVALID(reg32)                                   \
                REG_FLD_GET(OVL_SBCH_CON_FLD_L1_SBCH_TRANS_INVALID, (reg32))
#define OVL_SBCH_CON_GET_L0_SBCH_TRANS_INVALID(reg32)                                   \
                REG_FLD_GET(OVL_SBCH_CON_FLD_L0_SBCH_TRANS_INVALID, (reg32))

#define OVL_L0_ADDR_GET_L0_ADDR(reg32)          REG_FLD_GET(OVL_L0_ADDR_FLD_L0_ADDR, (reg32))

#define OVL_L1_ADDR_GET_L1_ADDR(reg32)          REG_FLD_GET(OVL_L1_ADDR_FLD_L1_ADDR, (reg32))

#define OVL_L2_ADDR_GET_L2_ADDR(reg32)          REG_FLD_GET(OVL_L2_ADDR_FLD_L2_ADDR, (reg32))

#define OVL_L3_ADDR_GET_L3_ADDR(reg32)          REG_FLD_GET(OVL_L3_ADDR_FLD_L3_ADDR, (reg32))

#define OVL_EL0_ADDR_GET_EL0_ADDR(reg32)        REG_FLD_GET(OVL_EL0_ADDR_FLD_EL0_ADDR, (reg32))

#define OVL_EL1_ADDR_GET_EL1_ADDR(reg32)        REG_FLD_GET(OVL_EL1_ADDR_FLD_EL1_ADDR, (reg32))

#define OVL_EL2_ADDR_GET_EL2_ADDR(reg32)        REG_FLD_GET(OVL_EL2_ADDR_FLD_EL2_ADDR, (reg32))

#define OVL_SECURE_GET_L3_SECURE(reg32)         REG_FLD_GET(OVL_SECURE_FLD_L3_SECURE, (reg32))
#define OVL_SECURE_GET_L2_SECURE(reg32)         REG_FLD_GET(OVL_SECURE_FLD_L2_SECURE, (reg32))
#define OVL_SECURE_GET_L1_SECURE(reg32)         REG_FLD_GET(OVL_SECURE_FLD_L1_SECURE, (reg32))
#define OVL_SECURE_GET_L0_SECURE(reg32)         REG_FLD_GET(OVL_SECURE_FLD_L0_SECURE, (reg32))

#define OVL_STA_SET_RDMA3_IDLE(reg32, val)      REG_FLD_SET(OVL_STA_FLD_RDMA3_IDLE, (reg32), (val))
#define OVL_STA_SET_RDMA2_IDLE(reg32, val)      REG_FLD_SET(OVL_STA_FLD_RDMA2_IDLE, (reg32), (val))
#define OVL_STA_SET_RDMA1_IDLE(reg32, val)      REG_FLD_SET(OVL_STA_FLD_RDMA1_IDLE, (reg32), (val))
#define OVL_STA_SET_RDMA0_IDLE(reg32, val)      REG_FLD_SET(OVL_STA_FLD_RDMA0_IDLE, (reg32), (val))
#define OVL_STA_SET_OVL_RUN(reg32, val)         REG_FLD_SET(OVL_STA_FLD_OVL_RUN, (reg32), (val))

#define OVL_INTEN_SET_OVL_START_INTEN(reg32, val)                                       \
                REG_FLD_SET(OVL_INTEN_FLD_OVL_START_INTEN, (reg32), (val))
#define OVL_INTEN_SET_ABNORMAL_SOF_INTEN(reg32, val)                                    \
                REG_FLD_SET(OVL_INTEN_FLD_ABNORMAL_SOF_INTEN, (reg32), (val))
#define OVL_INTEN_SET_RDMA3_SMI_UNDERFLOW_INTEN(reg32, val)                             \
                REG_FLD_SET(OVL_INTEN_FLD_RDMA3_SMI_UNDERFLOW_INTEN, (reg32), (val))
#define OVL_INTEN_SET_RDMA2_SMI_UNDERFLOW_INTEN(reg32, val)                             \
                REG_FLD_SET(OVL_INTEN_FLD_RDMA2_SMI_UNDERFLOW_INTEN, (reg32), (val))
#define OVL_INTEN_SET_RDMA1_SMI_UNDERFLOW_INTEN(reg32, val)                             \
                REG_FLD_SET(OVL_INTEN_FLD_RDMA1_SMI_UNDERFLOW_INTEN, (reg32), (val))
#define OVL_INTEN_SET_RDMA0_SMI_UNDERFLOW_INTEN(reg32, val)                             \
                REG_FLD_SET(OVL_INTEN_FLD_RDMA0_SMI_UNDERFLOW_INTEN, (reg32), (val))
#define OVL_INTEN_SET_RDMA3_EOF_ABNORMAL_INTEN(reg32, val)                              \
                REG_FLD_SET(OVL_INTEN_FLD_RDMA3_EOF_ABNORMAL_INTEN, (reg32), (val))
#define OVL_INTEN_SET_RDMA2_EOF_ABNORMAL_INTEN(reg32, val)                              \
                REG_FLD_SET(OVL_INTEN_FLD_RDMA2_EOF_ABNORMAL_INTEN, (reg32), (val))
#define OVL_INTEN_SET_RDMA1_EOF_ABNORMAL_INTEN(reg32, val)                              \
                REG_FLD_SET(OVL_INTEN_FLD_RDMA1_EOF_ABNORMAL_INTEN, (reg32), (val))
#define OVL_INTEN_SET_RDMA0_EOF_ABNORMAL_INTEN(reg32, val)                              \
                REG_FLD_SET(OVL_INTEN_FLD_RDMA0_EOF_ABNORMAL_INTEN, (reg32), (val))
#define OVL_INTEN_SET_OVL_FME_HWRST_DONE_INTEN(reg32, val)                              \
                REG_FLD_SET(OVL_INTEN_FLD_OVL_FME_HWRST_DONE_INTEN, (reg32), (val))
#define OVL_INTEN_SET_OVL_FME_SWRST_DONE_INTEN(reg32, val)                              \
                REG_FLD_SET(OVL_INTEN_FLD_OVL_FME_SWRST_DONE_INTEN, (reg32), (val))
#define OVL_INTEN_SET_OVL_FME_UND_INTEN(reg32, val)                                     \
                REG_FLD_SET(OVL_INTEN_FLD_OVL_FME_UND_INTEN, (reg32), (val))
#define OVL_INTEN_SET_OVL_FME_CPL_INTEN(reg32, val)                                     \
                REG_FLD_SET(OVL_INTEN_FLD_OVL_FME_CPL_INTEN, (reg32), (val))
#define OVL_INTEN_SET_OVL_REG_CMT_INTEN(reg32, val)                                     \
                REG_FLD_SET(OVL_INTEN_FLD_OVL_REG_CMT_INTEN, (reg32), (val))

#define OVL_INTSTA_SET_OVL_START_INTSTA(reg32, val)                                     \
                REG_FLD_SET(OVL_INTSTA_FLD_OVL_START_INTSTA, (reg32), (val))
#define OVL_INTSTA_SET_ABNORMAL_SOF_INTSTA(reg32, val)                                  \
                REG_FLD_SET(OVL_INTSTA_FLD_ABNORMAL_SOF_INTSTA, (reg32), (val))
#define OVL_INTSTA_SET_RDMA3_SMI_UNDERFLOW_INTSTA(reg32, val)                           \
                REG_FLD_SET(OVL_INTSTA_FLD_RDMA3_SMI_UNDERFLOW_INTSTA, (reg32), (val))
#define OVL_INTSTA_SET_RDMA2_SMI_UNDERFLOW_INTSTA(reg32, val)                           \
                REG_FLD_SET(OVL_INTSTA_FLD_RDMA2_SMI_UNDERFLOW_INTSTA, (reg32), (val))
#define OVL_INTSTA_SET_RDMA1_SMI_UNDERFLOW_INTSTA(reg32, val)                           \
                REG_FLD_SET(OVL_INTSTA_FLD_RDMA1_SMI_UNDERFLOW_INTSTA, (reg32), (val))
#define OVL_INTSTA_SET_RDMA0_SMI_UNDERFLOW_INTSTA(reg32, val)                           \
                REG_FLD_SET(OVL_INTSTA_FLD_RDMA0_SMI_UNDERFLOW_INTSTA, (reg32), (val))
#define OVL_INTSTA_SET_RDMA3_EOF_ABNORMAL_INTSTA(reg32, val)                            \
                REG_FLD_SET(OVL_INTSTA_FLD_RDMA3_EOF_ABNORMAL_INTSTA, (reg32), (val))
#define OVL_INTSTA_SET_RDMA2_EOF_ABNORMAL_INTSTA(reg32, val)                            \
                REG_FLD_SET(OVL_INTSTA_FLD_RDMA2_EOF_ABNORMAL_INTSTA, (reg32), (val))
#define OVL_INTSTA_SET_RDMA1_EOF_ABNORMAL_INTSTA(reg32, val)                            \
                REG_FLD_SET(OVL_INTSTA_FLD_RDMA1_EOF_ABNORMAL_INTSTA, (reg32), (val))
#define OVL_INTSTA_SET_RDMA0_EOF_ABNORMAL_INTSTA(reg32, val)                            \
                REG_FLD_SET(OVL_INTSTA_FLD_RDMA0_EOF_ABNORMAL_INTSTA, (reg32), (val))
#define OVL_INTSTA_SET_OVL_FME_HWRST_DONE_INTSTA(reg32, val)                            \
                REG_FLD_SET(OVL_INTSTA_FLD_OVL_FME_HWRST_DONE_INTSTA, (reg32), (val))
#define OVL_INTSTA_SET_OVL_FME_SWRST_DONE_INTSTA(reg32, val)                            \
                REG_FLD_SET(OVL_INTSTA_FLD_OVL_FME_SWRST_DONE_INTSTA, (reg32), (val))
#define OVL_INTSTA_SET_OVL_FME_UND_INTSTA(reg32, val)                                   \
                REG_FLD_SET(OVL_INTSTA_FLD_OVL_FME_UND_INTSTA, (reg32), (val))
#define OVL_INTSTA_SET_OVL_FME_CPL_INTSTA(reg32, val)                                   \
                REG_FLD_SET(OVL_INTSTA_FLD_OVL_FME_CPL_INTSTA, (reg32), (val))
#define OVL_INTSTA_SET_OVL_REG_CMT_INTSTA(reg32, val)                                   \
                REG_FLD_SET(OVL_INTSTA_FLD_OVL_REG_CMT_INTSTA, (reg32), (val))

#define OVL_EN_SET_BLOCK_EXT_PREULTRA(reg32, val)                                       \
                REG_FLD_SET(OVL_EN_FLD_BLOCK_EXT_PREULTRA, (reg32), (val))
#define OVL_EN_SET_BLOCK_EXT_ULTRA(reg32, val)                                          \
                REG_FLD_SET(OVL_EN_FLD_BLOCK_EXT_ULTRA, (reg32), (val))
#define OVL_EN_SET_IGNORE_ABNORMAL_SOF(reg32, val)                                      \
                REG_FLD_SET(OVL_EN_FLD_IGNORE_ABNORMAL_SOF, (reg32), (val))
#define OVL_EN_SET_HF_FOVL_CK_ON(reg32, val)                                            \
                REG_FLD_SET(OVL_EN_FLD_HF_FOVL_CK_ON, (reg32), (val))
#define OVL_EN_SET_HG_FSMI_CK_ON(reg32, val)                                            \
                REG_FLD_SET(OVL_EN_FLD_HG_FSMI_CK_ON, (reg32), (val))
#define OVL_EN_SET_HG_FOVL_CK_ON(reg32, val)                                            \
                REG_FLD_SET(OVL_EN_FLD_HG_FOVL_CK_ON, (reg32), (val))
#define OVL_EN_SET_OVL_EN(reg32, val)                                                   \
                REG_FLD_SET(OVL_EN_FLD_OVL_EN, (reg32), (val))

#define OVL_TRIG_SET_CRC_CLR(reg32, val)        REG_FLD_SET(OVL_TRIG_FLD_CRC_CLR, (reg32), (val))
#define OVL_TRIG_SET_CRC_EN(reg32, val)         REG_FLD_SET(OVL_TRIG_FLD_CRC_EN, (reg32), (val))
#define OVL_TRIG_SET_OVL_SW_TRIG(reg32, val)                                            \
                REG_FLD_SET(OVL_TRIG_FLD_OVL_SW_TRIG, (reg32), (val))

#define OVL_RST_SET_OVL_SMI_IOBUF_HARD_RST(reg32, val)                                  \
                REG_FLD_SET(OVL_RST_FLD_OVL_SMI_IOBUF_HARD_RST, (reg32), (val))
#define OVL_RST_SET_OVL_SMI_IOBUF_RST(reg32, val)                                       \
                REG_FLD_SET(OVL_RST_FLD_OVL_SMI_IOBUF_RST, (reg32), (val))
#define OVL_RST_SET_OVL_SMI_HARD_RST(reg32, val)                                        \
                REG_FLD_SET(OVL_RST_FLD_OVL_SMI_HARD_RST, (reg32), (val))
#define OVL_RST_SET_OVL_SMI_RST(reg32, val)                                             \
                REG_FLD_SET(OVL_RST_FLD_OVL_SMI_RST, (reg32), (val))
#define OVL_RST_SET_OVL_RST(reg32, val)         REG_FLD_SET(OVL_RST_FLD_OVL_RST, (reg32), (val))

#define OVL_ROI_SIZE_SET_ROI_H(reg32, val)      REG_FLD_SET(OVL_ROI_SIZE_FLD_ROI_H, (reg32), (val))
#define OVL_ROI_SIZE_SET_ROI_W(reg32, val)      REG_FLD_SET(OVL_ROI_SIZE_FLD_ROI_W, (reg32), (val))

#define OVL_DATAPATH_CON_SET_WIDE_GAMUT_EN(reg32, val)                                  \
                REG_FLD_SET(OVL_DATAPATH_CON_FLD_WIDE_GAMUT_EN, (reg32), (val))
#define OVL_DATAPATH_CON_SET_OUTPUT_INTERLACE(reg32, val)                               \
                REG_FLD_SET(OVL_DATAPATH_CON_FLD_OUTPUT_INTERLACE, (reg32), (val))
#define OVL_DATAPATH_CON_SET_OUTPUT_CLAMP(reg32, val)                                   \
                REG_FLD_SET(OVL_DATAPATH_CON_FLD_OUTPUT_CLAMP, (reg32), (val))
#define OVL_DATAPATH_CON_SET_GCLAST_EN(reg32, val)                                      \
                REG_FLD_SET(OVL_DATAPATH_CON_FLD_GCLAST_EN, (reg32), (val))
#define OVL_DATAPATH_CON_SET_RDMA3_OUT_SEL(reg32, val)                                  \
                REG_FLD_SET(OVL_DATAPATH_CON_FLD_RDMA3_OUT_SEL, (reg32), (val))
#define OVL_DATAPATH_CON_SET_RDMA2_OUT_SEL(reg32, val)                                  \
                REG_FLD_SET(OVL_DATAPATH_CON_FLD_RDMA2_OUT_SEL, (reg32), (val))
#define OVL_DATAPATH_CON_SET_RDMA1_OUT_SEL(reg32, val)                                  \
                REG_FLD_SET(OVL_DATAPATH_CON_FLD_RDMA1_OUT_SEL, (reg32), (val))
#define OVL_DATAPATH_CON_SET_RDMA0_OUT_SEL(reg32, val)                                  \
                REG_FLD_SET(OVL_DATAPATH_CON_FLD_RDMA0_OUT_SEL, (reg32), (val))
#define OVL_DATAPATH_CON_SET_PQ_OUT_SEL(reg32, val)                                     \
                REG_FLD_SET(OVL_DATAPATH_CON_FLD_PQ_OUT_SEL, (reg32), (val))
#define OVL_DATAPATH_CON_SET_OVL_GAMMA_OUT(reg32, val)                                  \
                REG_FLD_SET(OVL_DATAPATH_CON_FLD_OVL_GAMMA_OUT, (reg32), (val))
#define OVL_DATAPATH_CON_SET_ADOBE_LAYER(reg32, val)                                    \
                REG_FLD_SET(OVL_DATAPATH_CON_FLD_ADOBE_LAYER, (reg32), (val))
#define OVL_DATAPATH_CON_SET_ADOBE_MODE(reg32, val)                                     \
                REG_FLD_SET(OVL_DATAPATH_CON_FLD_ADOBE_MODE, (reg32), (val))
#define OVL_DATAPATH_CON_SET_L3_GPU_MODE(reg32, val)                                    \
                REG_FLD_SET(OVL_DATAPATH_CON_FLD_L3_GPU_MODE, (reg32), (val))
#define OVL_DATAPATH_CON_SET_L2_GPU_MODE(reg32, val)                                    \
                REG_FLD_SET(OVL_DATAPATH_CON_FLD_L2_GPU_MODE, (reg32), (val))
#define OVL_DATAPATH_CON_SET_L1_GPU_MODE(reg32, val)                                    \
                REG_FLD_SET(OVL_DATAPATH_CON_FLD_L1_GPU_MODE, (reg32), (val))
#define OVL_DATAPATH_CON_SET_L0_GPU_MODE(reg32, val)                                    \
                REG_FLD_SET(OVL_DATAPATH_CON_FLD_L0_GPU_MODE, (reg32), (val))
#define OVL_DATAPATH_CON_SET_OUTPUT_NO_RND(reg32, val)                                  \
                REG_FLD_SET(OVL_DATAPATH_CON_FLD_OUTPUT_NO_RND, (reg32), (val))
#define OVL_DATAPATH_CON_SET_BGCLR_IN_SEL(reg32, val)                                   \
                REG_FLD_SET(OVL_DATAPATH_CON_FLD_BGCLR_IN_SEL, (reg32), (val))
#define OVL_DATAPATH_CON_SET_OVL_RANDOM_BGCLR_EN(reg32, val)                            \
                REG_FLD_SET(OVL_DATAPATH_CON_FLD_OVL_RANDOM_BGCLR_EN, (reg32), (val))
#define OVL_DATAPATH_CON_SET_LAYER_SMI_ID_EN(reg32, val)                                \
                REG_FLD_SET(OVL_DATAPATH_CON_FLD_LAYER_SMI_ID_EN, (reg32), (val))

#define OVL_ROI_BGCLR_SET_ALPHA(reg32, val)    REG_FLD_SET(OVL_ROI_BGCLR_FLD_ALPHA, (reg32), (val))
#define OVL_ROI_BGCLR_SET_RED(reg32, val)      REG_FLD_SET(OVL_ROI_BGCLR_FLD_RED, (reg32), (val))
#define OVL_ROI_BGCLR_SET_GREEN(reg32, val)    REG_FLD_SET(OVL_ROI_BGCLR_FLD_GREEN, (reg32), (val))
#define OVL_ROI_BGCLR_SET_BLUE(reg32, val)     REG_FLD_SET(OVL_ROI_BGCLR_FLD_BLUE, (reg32), (val))

#define OVL_SRC_CON_SET_RELAY_MODE_EN(reg32, val)                                       \
                REG_FLD_SET(OVL_SRC_CON_FLD_RELAY_MODE_EN, (reg32), (val))
#define OVL_SRC_CON_SET_FORCE_RELAY_MODE(reg32, val)                                    \
                REG_FLD_SET(OVL_SRC_CON_FLD_FORCE_RELAY_MODE, (reg32), (val))
#define OVL_SRC_CON_SET_LC_EN(reg32, val)      REG_FLD_SET(OVL_SRC_CON_FLD_LC_EN, (reg32), (val))
#define OVL_SRC_CON_SET_L3_EN(reg32, val)      REG_FLD_SET(OVL_SRC_CON_FLD_L3_EN, (reg32), (val))
#define OVL_SRC_CON_SET_L2_EN(reg32, val)      REG_FLD_SET(OVL_SRC_CON_FLD_L2_EN, (reg32), (val))
#define OVL_SRC_CON_SET_L1_EN(reg32, val)      REG_FLD_SET(OVL_SRC_CON_FLD_L1_EN, (reg32), (val))
#define OVL_SRC_CON_SET_L0_EN(reg32, val)      REG_FLD_SET(OVL_SRC_CON_FLD_L0_EN, (reg32), (val))

#define OVL_L0_CON_SET_DSTKEY_EN(reg32, val)                                            \
                REG_FLD_SET(OVL_L0_CON_FLD_DSTKEY_EN, (reg32), (val))
#define OVL_L0_CON_SET_SRCKEY_EN(reg32, val)                                            \
                REG_FLD_SET(OVL_L0_CON_FLD_SRCKEY_EN, (reg32), (val))
#define OVL_L0_CON_SET_LAYER_SRC(reg32, val)                                            \
                REG_FLD_SET(OVL_L0_CON_FLD_LAYER_SRC, (reg32), (val))
#define OVL_L0_CON_SET_MTX_EN(reg32, val)                                               \
                REG_FLD_SET(OVL_L0_CON_FLD_MTX_EN, (reg32), (val))
#define OVL_L0_CON_SET_MTX_AUTO_DIS(reg32, val)                                         \
                REG_FLD_SET(OVL_L0_CON_FLD_MTX_AUTO_DIS, (reg32), (val))
#define OVL_L0_CON_SET_RGB_SWAP(reg32, val)                                             \
                REG_FLD_SET(OVL_L0_CON_FLD_RGB_SWAP, (reg32), (val))
#define OVL_L0_CON_SET_BYTE_SWAP(reg32, val)                                            \
                REG_FLD_SET(OVL_L0_CON_FLD_BYTE_SWAP, (reg32), (val))
#define OVL_L0_CON_SET_CLRFMT_MAN(reg32, val)                                           \
                REG_FLD_SET(OVL_L0_CON_FLD_CLRFMT_MAN, (reg32), (val))
#define OVL_L0_CON_SET_R_FIRST(reg32, val)                                              \
                REG_FLD_SET(OVL_L0_CON_FLD_R_FIRST, (reg32), (val))
#define OVL_L0_CON_SET_LANDSCAPE(reg32, val)                                            \
                REG_FLD_SET(OVL_L0_CON_FLD_LANDSCAPE, (reg32), (val))
#define OVL_L0_CON_SET_EN_3D(reg32, val)                                                \
                REG_FLD_SET(OVL_L0_CON_FLD_EN_3D, (reg32), (val))
#define OVL_L0_CON_SET_INT_MTX_SEL(reg32, val)                                          \
                REG_FLD_SET(OVL_L0_CON_FLD_INT_MTX_SEL, (reg32), (val))
#define OVL_L0_CON_SET_CLRFMT(reg32, val)                                               \
                REG_FLD_SET(OVL_L0_CON_FLD_CLRFMT, (reg32), (val))
#define OVL_L0_CON_SET_EXT_MTX_EN(reg32, val)                                           \
                REG_FLD_SET(OVL_L0_CON_FLD_EXT_MTX_EN, (reg32), (val))
#define OVL_L0_CON_SET_HORIZONTAL_FLIP_EN(reg32, val)                                   \
                REG_FLD_SET(OVL_L0_CON_FLD_HORIZONTAL_FLIP_EN, (reg32), (val))
#define OVL_L0_CON_SET_VERTICAL_FLIP_EN(reg32, val)                                     \
                REG_FLD_SET(OVL_L0_CON_FLD_VERTICAL_FLIP_EN, (reg32), (val))
#define OVL_L0_CON_SET_ALPHA_EN(reg32, val)                                             \
                REG_FLD_SET(OVL_L0_CON_FLD_ALPHA_EN, (reg32), (val))
#define OVL_L0_CON_SET_ALPHA(reg32, val)                                                \
                REG_FLD_SET(OVL_L0_CON_FLD_ALPHA, (reg32), (val))

#define OVL_L0_SRCKEY_SET_SRCKEY(reg32, val)                                            \
                REG_FLD_SET(OVL_L0_SRCKEY_FLD_SRCKEY, (reg32), (val))

#define OVL_L0_SRC_SIZE_SET_L0_SRC_H(reg32, val)                                        \
                REG_FLD_SET(OVL_L0_SRC_SIZE_FLD_L0_SRC_H, (reg32), (val))
#define OVL_L0_SRC_SIZE_SET_L0_SRC_W(reg32, val)                                        \
                REG_FLD_SET(OVL_L0_SRC_SIZE_FLD_L0_SRC_W, (reg32), (val))

#define OVL_L0_OFFSET_SET_L0_YOFF(reg32, val)                                           \
                REG_FLD_SET(OVL_L0_OFFSET_FLD_L0_YOFF, (reg32), (val))
#define OVL_L0_OFFSET_SET_L0_XOFF(reg32, val)                                           \
                REG_FLD_SET(OVL_L0_OFFSET_FLD_L0_XOFF, (reg32), (val))

#define OVL_L0_PITCH_SET_SURFL_EN(reg32, val)                                           \
                REG_FLD_SET(OVL_L0_PITCH_FLD_SURFL_EN, (reg32), (val))
#define OVL_L0_PITCH_SET_L0_BLEND_RND_SHT(reg32, val)                                   \
                REG_FLD_SET(OVL_L0_PITCH_FLD_L0_BLEND_RND_SHT, (reg32), (val))
#define OVL_L0_PITCH_SET_L0_SRGB_SEL_EXT2(reg32, val)                                   \
                REG_FLD_SET(OVL_L0_PITCH_FLD_L0_SRGB_SEL_EXT2, (reg32), (val))
#define OVL_L0_PITCH_SET_L0_CONST_BLD(reg32, val)                                       \
                REG_FLD_SET(OVL_L0_PITCH_FLD_L0_CONST_BLD, (reg32), (val))
#define OVL_L0_PITCH_SET_L0_DRGB_SEL_EXT(reg32, val)                                    \
                REG_FLD_SET(OVL_L0_PITCH_FLD_L0_DRGB_SEL_EXT, (reg32), (val))
#define OVL_L0_PITCH_SET_L0_DA_SEL_EXT(reg32, val)                                      \
                REG_FLD_SET(OVL_L0_PITCH_FLD_L0_DA_SEL_EXT, (reg32), (val))
#define OVL_L0_PITCH_SET_L0_SRGB_SEL_EXT(reg32, val)                                    \
                REG_FLD_SET(OVL_L0_PITCH_FLD_L0_SRGB_SEL_EXT, (reg32), (val))
#define OVL_L0_PITCH_SET_L0_SA_SEL_EXT(reg32, val)                                      \
                REG_FLD_SET(OVL_L0_PITCH_FLD_L0_SA_SEL_EXT, (reg32), (val))
#define OVL_L0_PITCH_SET_L0_DRGB_SEL(reg32, val)                                        \
                REG_FLD_SET(OVL_L0_PITCH_FLD_L0_DRGB_SEL, (reg32), (val))
#define OVL_L0_PITCH_SET_L0_DA_SEL(reg32, val)                                          \
                REG_FLD_SET(OVL_L0_PITCH_FLD_L0_DA_SEL, (reg32), (val))
#define OVL_L0_PITCH_SET_L0_SRGB_SEL(reg32, val)                                        \
                REG_FLD_SET(OVL_L0_PITCH_FLD_L0_SRGB_SEL, (reg32), (val))
#define OVL_L0_PITCH_SET_L0_SA_SEL(reg32, val)                                          \
                REG_FLD_SET(OVL_L0_PITCH_FLD_L0_SA_SEL, (reg32), (val))
#define OVL_L0_PITCH_SET_L0_SRC_PITCH(reg32, val)                                       \
                REG_FLD_SET(OVL_L0_PITCH_FLD_L0_SRC_PITCH, (reg32), (val))

#define OVL_L0_TILE_SET_TILE_HORI_BLOCK_NUM(reg32, val)                                 \
                REG_FLD_SET(OVL_L0_TILE_FLD_TILE_HORI_BLOCK_NUM, (reg32), (val))
#define OVL_L0_TILE_SET_TILE_EN(reg32, val)                                             \
                REG_FLD_SET(OVL_L0_TILE_FLD_TILE_EN, (reg32), (val))
#define OVL_L0_TILE_SET_TILE_WIDTH_SEL(reg32, val)                                      \
                REG_FLD_SET(OVL_L0_TILE_FLD_TILE_WIDTH_SEL, (reg32), (val))
#define OVL_L0_TILE_SET_TILE_HEIGHT(reg32, val)                                         \
                REG_FLD_SET(OVL_L0_TILE_FLD_TILE_HEIGHT, (reg32), (val))

#define OVL_L0_CLIP_SET_L0_SRC_BOTTOM_CLIP(reg32, val)                                  \
                REG_FLD_SET(OVL_L0_CLIP_FLD_L0_SRC_BOTTOM_CLIP, (reg32), (val))
#define OVL_L0_CLIP_SET_L0_SRC_TOP_CLIP(reg32, val)                                     \
                REG_FLD_SET(OVL_L0_CLIP_FLD_L0_SRC_TOP_CLIP, (reg32), (val))
#define OVL_L0_CLIP_SET_L0_SRC_RIGHT_CLIP(reg32, val)                                   \
                REG_FLD_SET(OVL_L0_CLIP_FLD_L0_SRC_RIGHT_CLIP, (reg32), (val))
#define OVL_L0_CLIP_SET_L0_SRC_LEFT_CLIP(reg32, val)                                    \
                REG_FLD_SET(OVL_L0_CLIP_FLD_L0_SRC_LEFT_CLIP, (reg32), (val))

#define OVL_L1_CON_SET_DSTKEY_EN(reg32, val)                                            \
                REG_FLD_SET(OVL_L1_CON_FLD_DSTKEY_EN, (reg32), (val))
#define OVL_L1_CON_SET_SRCKEY_EN(reg32, val)                                            \
                REG_FLD_SET(OVL_L1_CON_FLD_SRCKEY_EN, (reg32), (val))
#define OVL_L1_CON_SET_LAYER_SRC(reg32, val)                                            \
                REG_FLD_SET(OVL_L1_CON_FLD_LAYER_SRC, (reg32), (val))
#define OVL_L1_CON_SET_MTX_EN(reg32, val)                                               \
                REG_FLD_SET(OVL_L1_CON_FLD_MTX_EN, (reg32), (val))
#define OVL_L1_CON_SET_MTX_AUTO_DIS(reg32, val)                                         \
                REG_FLD_SET(OVL_L1_CON_FLD_MTX_AUTO_DIS, (reg32), (val))
#define OVL_L1_CON_SET_RGB_SWAP(reg32, val)                                             \
                REG_FLD_SET(OVL_L1_CON_FLD_RGB_SWAP, (reg32), (val))
#define OVL_L1_CON_SET_BYTE_SWAP(reg32, val)                                            \
                REG_FLD_SET(OVL_L1_CON_FLD_BYTE_SWAP, (reg32), (val))
#define OVL_L1_CON_SET_CLRFMT_MAN(reg32, val)                                           \
                REG_FLD_SET(OVL_L1_CON_FLD_CLRFMT_MAN, (reg32), (val))
#define OVL_L1_CON_SET_R_FIRST(reg32, val)                                              \
                REG_FLD_SET(OVL_L1_CON_FLD_R_FIRST, (reg32), (val))
#define OVL_L1_CON_SET_LANDSCAPE(reg32, val)                                            \
                REG_FLD_SET(OVL_L1_CON_FLD_LANDSCAPE, (reg32), (val))
#define OVL_L1_CON_SET_EN_3D(reg32, val)                                                \
                REG_FLD_SET(OVL_L1_CON_FLD_EN_3D, (reg32), (val))
#define OVL_L1_CON_SET_INT_MTX_SEL(reg32, val)                                          \
                REG_FLD_SET(OVL_L1_CON_FLD_INT_MTX_SEL, (reg32), (val))
#define OVL_L1_CON_SET_CLRFMT(reg32, val)                                               \
                REG_FLD_SET(OVL_L1_CON_FLD_CLRFMT, (reg32), (val))
#define OVL_L1_CON_SET_EXT_MTX_EN(reg32, val)                                           \
                REG_FLD_SET(OVL_L1_CON_FLD_EXT_MTX_EN, (reg32), (val))
#define OVL_L1_CON_SET_HORIZONTAL_FLIP_EN(reg32, val)                                   \
                REG_FLD_SET(OVL_L1_CON_FLD_HORIZONTAL_FLIP_EN, (reg32), (val))
#define OVL_L1_CON_SET_VERTICAL_FLIP_EN(reg32, val)                                     \
                REG_FLD_SET(OVL_L1_CON_FLD_VERTICAL_FLIP_EN, (reg32), (val))
#define OVL_L1_CON_SET_ALPHA_EN(reg32, val)                                             \
                REG_FLD_SET(OVL_L1_CON_FLD_ALPHA_EN, (reg32), (val))
#define OVL_L1_CON_SET_ALPHA(reg32, val)                                                \
                REG_FLD_SET(OVL_L1_CON_FLD_ALPHA, (reg32), (val))

#define OVL_L1_SRCKEY_SET_SRCKEY(reg32, val)                                            \
                REG_FLD_SET(OVL_L1_SRCKEY_FLD_SRCKEY, (reg32), (val))

#define OVL_L1_SRC_SIZE_SET_L1_SRC_H(reg32, val)                                        \
                REG_FLD_SET(OVL_L1_SRC_SIZE_FLD_L1_SRC_H, (reg32), (val))
#define OVL_L1_SRC_SIZE_SET_L1_SRC_W(reg32, val)                                        \
                REG_FLD_SET(OVL_L1_SRC_SIZE_FLD_L1_SRC_W, (reg32), (val))

#define OVL_L1_OFFSET_SET_L1_YOFF(reg32, val)                                           \
                REG_FLD_SET(OVL_L1_OFFSET_FLD_L1_YOFF, (reg32), (val))
#define OVL_L1_OFFSET_SET_L1_XOFF(reg32, val)                                           \
                REG_FLD_SET(OVL_L1_OFFSET_FLD_L1_XOFF, (reg32), (val))

#define OVL_L1_PITCH_SET_SURFL_EN(reg32, val)                                           \
                REG_FLD_SET(OVL_L1_PITCH_FLD_SURFL_EN, (reg32), (val))
#define OVL_L1_PITCH_SET_L1_BLEND_RND_SHT(reg32, val)                                   \
                REG_FLD_SET(OVL_L1_PITCH_FLD_L1_BLEND_RND_SHT, (reg32), (val))
#define OVL_L1_PITCH_SET_L1_SRGB_SEL_EXT2(reg32, val)                                   \
                REG_FLD_SET(OVL_L1_PITCH_FLD_L1_SRGB_SEL_EXT2, (reg32), (val))
#define OVL_L1_PITCH_SET_L1_CONST_BLD(reg32, val)                                       \
                REG_FLD_SET(OVL_L1_PITCH_FLD_L1_CONST_BLD, (reg32), (val))
#define OVL_L1_PITCH_SET_L1_DRGB_SEL_EXT(reg32, val)                                    \
                REG_FLD_SET(OVL_L1_PITCH_FLD_L1_DRGB_SEL_EXT, (reg32), (val))
#define OVL_L1_PITCH_SET_L1_DA_SEL_EXT(reg32, val)                                      \
                REG_FLD_SET(OVL_L1_PITCH_FLD_L1_DA_SEL_EXT, (reg32), (val))
#define OVL_L1_PITCH_SET_L1_SRGB_SEL_EXT(reg32, val)                                    \
                REG_FLD_SET(OVL_L1_PITCH_FLD_L1_SRGB_SEL_EXT, (reg32), (val))
#define OVL_L1_PITCH_SET_L1_SA_SEL_EXT(reg32, val)                                      \
                REG_FLD_SET(OVL_L1_PITCH_FLD_L1_SA_SEL_EXT, (reg32), (val))
#define OVL_L1_PITCH_SET_L1_DRGB_SEL(reg32, val)                                        \
                REG_FLD_SET(OVL_L1_PITCH_FLD_L1_DRGB_SEL, (reg32), (val))
#define OVL_L1_PITCH_SET_L1_DA_SEL(reg32, val)                                          \
                REG_FLD_SET(OVL_L1_PITCH_FLD_L1_DA_SEL, (reg32), (val))
#define OVL_L1_PITCH_SET_L1_SRGB_SEL(reg32, val)                                        \
                REG_FLD_SET(OVL_L1_PITCH_FLD_L1_SRGB_SEL, (reg32), (val))
#define OVL_L1_PITCH_SET_L1_SA_SEL(reg32, val)                                          \
                REG_FLD_SET(OVL_L1_PITCH_FLD_L1_SA_SEL, (reg32), (val))
#define OVL_L1_PITCH_SET_L1_SRC_PITCH(reg32, val)                                       \
                REG_FLD_SET(OVL_L1_PITCH_FLD_L1_SRC_PITCH, (reg32), (val))

#define OVL_L1_TILE_SET_TILE_HORI_BLOCK_NUM(reg32, val)                                 \
                REG_FLD_SET(OVL_L1_TILE_FLD_TILE_HORI_BLOCK_NUM, (reg32), (val))
#define OVL_L1_TILE_SET_TILE_EN(reg32, val)                                             \
                REG_FLD_SET(OVL_L1_TILE_FLD_TILE_EN, (reg32), (val))
#define OVL_L1_TILE_SET_TILE_WIDTH_SEL(reg32, val)                                      \
                REG_FLD_SET(OVL_L1_TILE_FLD_TILE_WIDTH_SEL, (reg32), (val))
#define OVL_L1_TILE_SET_TILE_HEIGHT(reg32, val)                                         \
                REG_FLD_SET(OVL_L1_TILE_FLD_TILE_HEIGHT, (reg32), (val))

#define OVL_L1_CLIP_SET_L1_SRC_BOTTOM_CLIP(reg32, val)                                  \
                REG_FLD_SET(OVL_L1_CLIP_FLD_L1_SRC_BOTTOM_CLIP, (reg32), (val))
#define OVL_L1_CLIP_SET_L1_SRC_TOP_CLIP(reg32, val)                                     \
                REG_FLD_SET(OVL_L1_CLIP_FLD_L1_SRC_TOP_CLIP, (reg32), (val))
#define OVL_L1_CLIP_SET_L1_SRC_RIGHT_CLIP(reg32, val)                                   \
                REG_FLD_SET(OVL_L1_CLIP_FLD_L1_SRC_RIGHT_CLIP, (reg32), (val))
#define OVL_L1_CLIP_SET_L1_SRC_LEFT_CLIP(reg32, val)                                    \
                REG_FLD_SET(OVL_L1_CLIP_FLD_L1_SRC_LEFT_CLIP, (reg32), (val))

#define OVL_L2_CON_SET_DSTKEY_EN(reg32, val)                                            \
                REG_FLD_SET(OVL_L2_CON_FLD_DSTKEY_EN, (reg32), (val))
#define OVL_L2_CON_SET_SRCKEY_EN(reg32, val)                                            \
                REG_FLD_SET(OVL_L2_CON_FLD_SRCKEY_EN, (reg32), (val))
#define OVL_L2_CON_SET_LAYER_SRC(reg32, val)                                            \
                REG_FLD_SET(OVL_L2_CON_FLD_LAYER_SRC, (reg32), (val))
#define OVL_L2_CON_SET_MTX_EN(reg32, val)                                               \
                REG_FLD_SET(OVL_L2_CON_FLD_MTX_EN, (reg32), (val))
#define OVL_L2_CON_SET_MTX_AUTO_DIS(reg32, val)                                         \
                REG_FLD_SET(OVL_L2_CON_FLD_MTX_AUTO_DIS, (reg32), (val))
#define OVL_L2_CON_SET_RGB_SWAP(reg32, val)                                             \
                REG_FLD_SET(OVL_L2_CON_FLD_RGB_SWAP, (reg32), (val))
#define OVL_L2_CON_SET_BYTE_SWAP(reg32, val)                                            \
                REG_FLD_SET(OVL_L2_CON_FLD_BYTE_SWAP, (reg32), (val))
#define OVL_L2_CON_SET_CLRFMT_MAN(reg32, val)                                           \
                REG_FLD_SET(OVL_L2_CON_FLD_CLRFMT_MAN, (reg32), (val))
#define OVL_L2_CON_SET_R_FIRST(reg32, val)                                              \
                REG_FLD_SET(OVL_L2_CON_FLD_R_FIRST, (reg32), (val))
#define OVL_L2_CON_SET_LANDSCAPE(reg32, val)                                            \
                REG_FLD_SET(OVL_L2_CON_FLD_LANDSCAPE, (reg32), (val))
#define OVL_L2_CON_SET_EN_3D(reg32, val)                                                \
                REG_FLD_SET(OVL_L2_CON_FLD_EN_3D, (reg32), (val))
#define OVL_L2_CON_SET_INT_MTX_SEL(reg32, val)                                          \
                REG_FLD_SET(OVL_L2_CON_FLD_INT_MTX_SEL, (reg32), (val))
#define OVL_L2_CON_SET_CLRFMT(reg32, val)                                               \
                REG_FLD_SET(OVL_L2_CON_FLD_CLRFMT, (reg32), (val))
#define OVL_L2_CON_SET_EXT_MTX_EN(reg32, val)                                           \
                REG_FLD_SET(OVL_L2_CON_FLD_EXT_MTX_EN, (reg32), (val))
#define OVL_L2_CON_SET_HORIZONTAL_FLIP_EN(reg32, val)                                   \
                REG_FLD_SET(OVL_L2_CON_FLD_HORIZONTAL_FLIP_EN, (reg32), (val))
#define OVL_L2_CON_SET_VERTICAL_FLIP_EN(reg32, val)                                     \
                REG_FLD_SET(OVL_L2_CON_FLD_VERTICAL_FLIP_EN, (reg32), (val))
#define OVL_L2_CON_SET_ALPHA_EN(reg32, val)                                             \
                REG_FLD_SET(OVL_L2_CON_FLD_ALPHA_EN, (reg32), (val))
#define OVL_L2_CON_SET_ALPHA(reg32, val)                                                \
                REG_FLD_SET(OVL_L2_CON_FLD_ALPHA, (reg32), (val))

#define OVL_L2_SRCKEY_SET_SRCKEY(reg32, val)                                            \
                REG_FLD_SET(OVL_L2_SRCKEY_FLD_SRCKEY, (reg32), (val))

#define OVL_L2_SRC_SIZE_SET_L2_SRC_H(reg32, val)                                        \
                REG_FLD_SET(OVL_L2_SRC_SIZE_FLD_L2_SRC_H, (reg32), (val))
#define OVL_L2_SRC_SIZE_SET_L2_SRC_W(reg32, val)                                        \
                REG_FLD_SET(OVL_L2_SRC_SIZE_FLD_L2_SRC_W, (reg32), (val))

#define OVL_L2_OFFSET_SET_L2_YOFF(reg32, val)                                           \
                REG_FLD_SET(OVL_L2_OFFSET_FLD_L2_YOFF, (reg32), (val))
#define OVL_L2_OFFSET_SET_L2_XOFF(reg32, val)                                           \
                REG_FLD_SET(OVL_L2_OFFSET_FLD_L2_XOFF, (reg32), (val))

#define OVL_L2_PITCH_SET_SURFL_EN(reg32, val)                                           \
                REG_FLD_SET(OVL_L2_PITCH_FLD_SURFL_EN, (reg32), (val))
#define OVL_L2_PITCH_SET_L2_BLEND_RND_SHT(reg32, val)                                   \
                REG_FLD_SET(OVL_L2_PITCH_FLD_L2_BLEND_RND_SHT, (reg32), (val))
#define OVL_L2_PITCH_SET_L2_SRGB_SEL_EXT2(reg32, val)                                   \
                REG_FLD_SET(OVL_L2_PITCH_FLD_L2_SRGB_SEL_EXT2, (reg32), (val))
#define OVL_L2_PITCH_SET_L2_CONST_BLD(reg32, val)                                       \
                REG_FLD_SET(OVL_L2_PITCH_FLD_L2_CONST_BLD, (reg32), (val))
#define OVL_L2_PITCH_SET_L2_DRGB_SEL_EXT(reg32, val)                                    \
                REG_FLD_SET(OVL_L2_PITCH_FLD_L2_DRGB_SEL_EXT, (reg32), (val))
#define OVL_L2_PITCH_SET_L2_DA_SEL_EXT(reg32, val)                                      \
                REG_FLD_SET(OVL_L2_PITCH_FLD_L2_DA_SEL_EXT, (reg32), (val))
#define OVL_L2_PITCH_SET_L2_SRGB_SEL_EXT(reg32, val)                                    \
                REG_FLD_SET(OVL_L2_PITCH_FLD_L2_SRGB_SEL_EXT, (reg32), (val))
#define OVL_L2_PITCH_SET_L2_SA_SEL_EXT(reg32, val)                                      \
                REG_FLD_SET(OVL_L2_PITCH_FLD_L2_SA_SEL_EXT, (reg32), (val))
#define OVL_L2_PITCH_SET_L2_DRGB_SEL(reg32, val)                                        \
                REG_FLD_SET(OVL_L2_PITCH_FLD_L2_DRGB_SEL, (reg32), (val))
#define OVL_L2_PITCH_SET_L2_DA_SEL(reg32, val)                                          \
                REG_FLD_SET(OVL_L2_PITCH_FLD_L2_DA_SEL, (reg32), (val))
#define OVL_L2_PITCH_SET_L2_SRGB_SEL(reg32, val)                                        \
                REG_FLD_SET(OVL_L2_PITCH_FLD_L2_SRGB_SEL, (reg32), (val))
#define OVL_L2_PITCH_SET_L2_SA_SEL(reg32, val)                                          \
                REG_FLD_SET(OVL_L2_PITCH_FLD_L2_SA_SEL, (reg32), (val))
#define OVL_L2_PITCH_SET_L2_SRC_PITCH(reg32, val)                                       \
                REG_FLD_SET(OVL_L2_PITCH_FLD_L2_SRC_PITCH, (reg32), (val))

#define OVL_L2_TILE_SET_TILE_HORI_BLOCK_NUM(reg32, val)                                 \
                REG_FLD_SET(OVL_L2_TILE_FLD_TILE_HORI_BLOCK_NUM, (reg32), (val))
#define OVL_L2_TILE_SET_TILE_EN(reg32, val)                                             \
                REG_FLD_SET(OVL_L2_TILE_FLD_TILE_EN, (reg32), (val))
#define OVL_L2_TILE_SET_TILE_WIDTH_SEL(reg32, val)                                      \
                REG_FLD_SET(OVL_L2_TILE_FLD_TILE_WIDTH_SEL, (reg32), (val))
#define OVL_L2_TILE_SET_TILE_HEIGHT(reg32, val)                                         \
                REG_FLD_SET(OVL_L2_TILE_FLD_TILE_HEIGHT, (reg32), (val))

#define OVL_L2_CLIP_SET_L2_SRC_BOTTOM_CLIP(reg32, val)                                  \
                REG_FLD_SET(OVL_L2_CLIP_FLD_L2_SRC_BOTTOM_CLIP, (reg32), (val))
#define OVL_L2_CLIP_SET_L2_SRC_TOP_CLIP(reg32, val)                                     \
                REG_FLD_SET(OVL_L2_CLIP_FLD_L2_SRC_TOP_CLIP, (reg32), (val))
#define OVL_L2_CLIP_SET_L2_SRC_RIGHT_CLIP(reg32, val)                                   \
                REG_FLD_SET(OVL_L2_CLIP_FLD_L2_SRC_RIGHT_CLIP, (reg32), (val))
#define OVL_L2_CLIP_SET_L2_SRC_LEFT_CLIP(reg32, val)                                    \
                REG_FLD_SET(OVL_L2_CLIP_FLD_L2_SRC_LEFT_CLIP, (reg32), (val))

#define OVL_L3_CON_SET_DSTKEY_EN(reg32, val)                                            \
                REG_FLD_SET(OVL_L3_CON_FLD_DSTKEY_EN, (reg32), (val))
#define OVL_L3_CON_SET_SRCKEY_EN(reg32, val)                                            \
                REG_FLD_SET(OVL_L3_CON_FLD_SRCKEY_EN, (reg32), (val))
#define OVL_L3_CON_SET_LAYER_SRC(reg32, val)                                            \
                REG_FLD_SET(OVL_L3_CON_FLD_LAYER_SRC, (reg32), (val))
#define OVL_L3_CON_SET_MTX_EN(reg32, val)                                               \
                REG_FLD_SET(OVL_L3_CON_FLD_MTX_EN, (reg32), (val))
#define OVL_L3_CON_SET_MTX_AUTO_DIS(reg32, val)                                         \
                REG_FLD_SET(OVL_L3_CON_FLD_MTX_AUTO_DIS, (reg32), (val))
#define OVL_L3_CON_SET_RGB_SWAP(reg32, val)                                             \
                REG_FLD_SET(OVL_L3_CON_FLD_RGB_SWAP, (reg32), (val))
#define OVL_L3_CON_SET_BYTE_SWAP(reg32, val)                                            \
                REG_FLD_SET(OVL_L3_CON_FLD_BYTE_SWAP, (reg32), (val))
#define OVL_L3_CON_SET_CLRFMT_MAN(reg32, val)                                           \
                REG_FLD_SET(OVL_L3_CON_FLD_CLRFMT_MAN, (reg32), (val))
#define OVL_L3_CON_SET_R_FIRST(reg32, val)                                              \
                REG_FLD_SET(OVL_L3_CON_FLD_R_FIRST, (reg32), (val))
#define OVL_L3_CON_SET_LANDSCAPE(reg32, val)                                            \
                REG_FLD_SET(OVL_L3_CON_FLD_LANDSCAPE, (reg32), (val))
#define OVL_L3_CON_SET_EN_3D(reg32, val)                                                \
                REG_FLD_SET(OVL_L3_CON_FLD_EN_3D, (reg32), (val))
#define OVL_L3_CON_SET_INT_MTX_SEL(reg32, val)                                          \
                REG_FLD_SET(OVL_L3_CON_FLD_INT_MTX_SEL, (reg32), (val))
#define OVL_L3_CON_SET_CLRFMT(reg32, val)                                               \
                REG_FLD_SET(OVL_L3_CON_FLD_CLRFMT, (reg32), (val))
#define OVL_L3_CON_SET_EXT_MTX_EN(reg32, val)                                           \
                REG_FLD_SET(OVL_L3_CON_FLD_EXT_MTX_EN, (reg32), (val))
#define OVL_L3_CON_SET_HORIZONTAL_FLIP_EN(reg32, val)                                   \
                REG_FLD_SET(OVL_L3_CON_FLD_HORIZONTAL_FLIP_EN, (reg32), (val))
#define OVL_L3_CON_SET_VERTICAL_FLIP_EN(reg32, val)                                     \
                REG_FLD_SET(OVL_L3_CON_FLD_VERTICAL_FLIP_EN, (reg32), (val))
#define OVL_L3_CON_SET_ALPHA_EN(reg32, val)                                             \
                REG_FLD_SET(OVL_L3_CON_FLD_ALPHA_EN, (reg32), (val))
#define OVL_L3_CON_SET_ALPHA(reg32, val)                                                \
                REG_FLD_SET(OVL_L3_CON_FLD_ALPHA, (reg32), (val))

#define OVL_L3_SRCKEY_SET_SRCKEY(reg32, val)                                            \
                REG_FLD_SET(OVL_L3_SRCKEY_FLD_SRCKEY, (reg32), (val))

#define OVL_L3_SRC_SIZE_SET_L3_SRC_H(reg32, val)                                        \
                REG_FLD_SET(OVL_L3_SRC_SIZE_FLD_L3_SRC_H, (reg32), (val))
#define OVL_L3_SRC_SIZE_SET_L3_SRC_W(reg32, val)                                        \
                REG_FLD_SET(OVL_L3_SRC_SIZE_FLD_L3_SRC_W, (reg32), (val))

#define OVL_L3_OFFSET_SET_L3_YOFF(reg32, val)                                           \
                REG_FLD_SET(OVL_L3_OFFSET_FLD_L3_YOFF, (reg32), (val))
#define OVL_L3_OFFSET_SET_L3_XOFF(reg32, val)                                           \
                REG_FLD_SET(OVL_L3_OFFSET_FLD_L3_XOFF, (reg32), (val))

#define OVL_L3_PITCH_SET_SURFL_EN(reg32, val)                                           \
                REG_FLD_SET(OVL_L3_PITCH_FLD_SURFL_EN, (reg32), (val))
#define OVL_L3_PITCH_SET_L3_BLEND_RND_SHT(reg32, val)                                   \
                REG_FLD_SET(OVL_L3_PITCH_FLD_L3_BLEND_RND_SHT, (reg32), (val))
#define OVL_L3_PITCH_SET_L3_SRGB_SEL_EXT2(reg32, val)                                   \
                REG_FLD_SET(OVL_L3_PITCH_FLD_L3_SRGB_SEL_EXT2, (reg32), (val))
#define OVL_L3_PITCH_SET_L3_CONST_BLD(reg32, val)                                       \
                REG_FLD_SET(OVL_L3_PITCH_FLD_L3_CONST_BLD, (reg32), (val))
#define OVL_L3_PITCH_SET_L3_DRGB_SEL_EXT(reg32, val)                                    \
                REG_FLD_SET(OVL_L3_PITCH_FLD_L3_DRGB_SEL_EXT, (reg32), (val))
#define OVL_L3_PITCH_SET_L3_DA_SEL_EXT(reg32, val)                                      \
                REG_FLD_SET(OVL_L3_PITCH_FLD_L3_DA_SEL_EXT, (reg32), (val))
#define OVL_L3_PITCH_SET_L3_SRGB_SEL_EXT(reg32, val)                                    \
                REG_FLD_SET(OVL_L3_PITCH_FLD_L3_SRGB_SEL_EXT, (reg32), (val))
#define OVL_L3_PITCH_SET_L3_SA_SEL_EXT(reg32, val)                                      \
                REG_FLD_SET(OVL_L3_PITCH_FLD_L3_SA_SEL_EXT, (reg32), (val))
#define OVL_L3_PITCH_SET_L3_DRGB_SEL(reg32, val)                                        \
                REG_FLD_SET(OVL_L3_PITCH_FLD_L3_DRGB_SEL, (reg32), (val))
#define OVL_L3_PITCH_SET_L3_DA_SEL(reg32, val)                                          \
                REG_FLD_SET(OVL_L3_PITCH_FLD_L3_DA_SEL, (reg32), (val))
#define OVL_L3_PITCH_SET_L3_SRGB_SEL(reg32, val)                                        \
                REG_FLD_SET(OVL_L3_PITCH_FLD_L3_SRGB_SEL, (reg32), (val))
#define OVL_L3_PITCH_SET_L3_SA_SEL(reg32, val)                                          \
                REG_FLD_SET(OVL_L3_PITCH_FLD_L3_SA_SEL, (reg32), (val))
#define OVL_L3_PITCH_SET_L3_SRC_PITCH(reg32, val)                                       \
                REG_FLD_SET(OVL_L3_PITCH_FLD_L3_SRC_PITCH, (reg32), (val))

#define OVL_L3_TILE_SET_TILE_HORI_BLOCK_NUM(reg32, val)                                 \
                REG_FLD_SET(OVL_L3_TILE_FLD_TILE_HORI_BLOCK_NUM, (reg32), (val))
#define OVL_L3_TILE_SET_TILE_EN(reg32, val)                                             \
                REG_FLD_SET(OVL_L3_TILE_FLD_TILE_EN, (reg32), (val))
#define OVL_L3_TILE_SET_TILE_WIDTH_SEL(reg32, val)                                      \
                REG_FLD_SET(OVL_L3_TILE_FLD_TILE_WIDTH_SEL, (reg32), (val))
#define OVL_L3_TILE_SET_TILE_HEIGHT(reg32, val)                                         \
                REG_FLD_SET(OVL_L3_TILE_FLD_TILE_HEIGHT, (reg32), (val))

#define OVL_L3_CLIP_SET_L3_SRC_BOTTOM_CLIP(reg32, val)                                  \
                REG_FLD_SET(OVL_L3_CLIP_FLD_L3_SRC_BOTTOM_CLIP, (reg32), (val))
#define OVL_L3_CLIP_SET_L3_SRC_TOP_CLIP(reg32, val)                                     \
                REG_FLD_SET(OVL_L3_CLIP_FLD_L3_SRC_TOP_CLIP, (reg32), (val))
#define OVL_L3_CLIP_SET_L3_SRC_RIGHT_CLIP(reg32, val)                                   \
                REG_FLD_SET(OVL_L3_CLIP_FLD_L3_SRC_RIGHT_CLIP, (reg32), (val))
#define OVL_L3_CLIP_SET_L3_SRC_LEFT_CLIP(reg32, val)                                    \
                REG_FLD_SET(OVL_L3_CLIP_FLD_L3_SRC_LEFT_CLIP, (reg32), (val))

#define OVL_RDMA0_CTRL_SET_RDMA0_FIFO_USED_SIZE(reg32, val)                             \
                REG_FLD_SET(OVL_RDMA0_CTRL_FLD_RDMA0_FIFO_USED_SIZE, (reg32), (val))
#define OVL_RDMA0_CTRL_SET_RDMA0_INTERLACE(reg32, val)                                  \
                REG_FLD_SET(OVL_RDMA0_CTRL_FLD_RDMA0_INTERLACE, (reg32), (val))
#define OVL_RDMA0_CTRL_SET_RDMA0_EN(reg32, val)                                         \
                REG_FLD_SET(OVL_RDMA0_CTRL_FLD_RDMA0_EN, (reg32), (val))

#define OVL_RDMA0_MEM_GMC_SETTING1_SET_RDMA0_PRE_ULTRA_THRESHOLD_HIGH_OFS(reg32, val)   \
                REG_FLD_SET(OVL_RDMA0_MEM_GMC_SETTING1_FLD_RDMA0_PRE_ULTRA_THRESHOLD_HIGH_OFS,    \
                (reg32), (val))
#define OVL_RDMA0_MEM_GMC_SETTING1_SET_RDMA0_ULTRA_THRESHOLD_HIGH_OFS(reg32, val)       \
                REG_FLD_SET(OVL_RDMA0_MEM_GMC_SETTING1_FLD_RDMA0_ULTRA_THRESHOLD_HIGH_OFS,        \
                (reg32), (val))
#define OVL_RDMA0_MEM_GMC_SETTING1_SET_RDMA0_OSTD_PREULTRA_TH(reg32, val)               \
                REG_FLD_SET(OVL_RDMA0_MEM_GMC_SETTING1_FLD_RDMA0_OSTD_PREULTRA_TH, (reg32), (val))
#define OVL_RDMA0_MEM_GMC_SETTING1_SET_RDMA0_OSTD_ULTRA_TH(reg32, val)                  \
                REG_FLD_SET(OVL_RDMA0_MEM_GMC_SETTING1_FLD_RDMA0_OSTD_ULTRA_TH, (reg32), (val))

#define OVL_RDMA0_MEM_SLOW_CON_SET_RDMA0_SLOW_CNT(reg32, val)                           \
                                  \
                REG_FLD_SET(OVL_RDMA0_MEM_SLOW_CON_FLD_RDMA0_SLOW_CNT, (reg32), (val))
#define OVL_RDMA0_MEM_SLOW_CON_SET_RDMA0_SLOW_EN(reg32, val)                            \
                REG_FLD_SET(OVL_RDMA0_MEM_SLOW_CON_FLD_RDMA0_SLOW_EN, (reg32), (val))

#define OVL_RDMA0_FIFO_CTRL_SET_RDMA0_FIFO_UND_EN(reg32, val)                           \
                REG_FLD_SET(OVL_RDMA0_FIFO_CTRL_FLD_RDMA0_FIFO_UND_EN, (reg32), (val))
#define OVL_RDMA0_FIFO_CTRL_SET_RDMA0_FIFO_SIZE(reg32, val)                             \
                REG_FLD_SET(OVL_RDMA0_FIFO_CTRL_FLD_RDMA0_FIFO_SIZE, (reg32), (val))
#define OVL_RDMA0_FIFO_CTRL_SET_RDMA0_FIFO_THRD(reg32, val)                             \
                REG_FLD_SET(OVL_RDMA0_FIFO_CTRL_FLD_RDMA0_FIFO_THRD, (reg32), (val))

#define OVL_RDMA1_CTRL_SET_RDMA1_FIFO_USED_SIZE(reg32, val)                             \
                REG_FLD_SET(OVL_RDMA1_CTRL_FLD_RDMA1_FIFO_USED_SIZE, (reg32), (val))
#define OVL_RDMA1_CTRL_SET_RDMA1_INTERLACE(reg32, val)                                  \
                REG_FLD_SET(OVL_RDMA1_CTRL_FLD_RDMA1_INTERLACE, (reg32), (val))
#define OVL_RDMA1_CTRL_SET_RDMA1_EN(reg32, val)                                         \
                REG_FLD_SET(OVL_RDMA1_CTRL_FLD_RDMA1_EN, (reg32), (val))

#define OVL_RDMA1_MEM_GMC_SETTING1_SET_RDMA1_PRE_ULTRA_THRESHOLD_HIGH_OFS(reg32, val)   \
                REG_FLD_SET(OVL_RDMA1_MEM_GMC_SETTING1_FLD_RDMA1_PRE_ULTRA_THRESHOLD_HIGH_OFS,    \
                (reg32), (val))
#define OVL_RDMA1_MEM_GMC_SETTING1_SET_RDMA1_ULTRA_THRESHOLD_HIGH_OFS(reg32, val)       \
                REG_FLD_SET(OVL_RDMA1_MEM_GMC_SETTING1_FLD_RDMA1_ULTRA_THRESHOLD_HIGH_OFS,        \
                (reg32), (val))
#define OVL_RDMA1_MEM_GMC_SETTING1_SET_RDMA1_OSTD_PREULTRA_TH(reg32, val)               \
                REG_FLD_SET(OVL_RDMA1_MEM_GMC_SETTING1_FLD_RDMA1_OSTD_PREULTRA_TH, (reg32), (val))
#define OVL_RDMA1_MEM_GMC_SETTING1_SET_RDMA1_OSTD_ULTRA_TH(reg32, val)                  \
                REG_FLD_SET(OVL_RDMA1_MEM_GMC_SETTING1_FLD_RDMA1_OSTD_ULTRA_TH, (reg32), (val))

#define OVL_RDMA1_MEM_SLOW_CON_SET_RDMA1_SLOW_CNT(reg32, val)                           \
                REG_FLD_SET(OVL_RDMA1_MEM_SLOW_CON_FLD_RDMA1_SLOW_CNT, (reg32), (val))
#define OVL_RDMA1_MEM_SLOW_CON_SET_RDMA1_SLOW_EN(reg32, val)                            \
                REG_FLD_SET(OVL_RDMA1_MEM_SLOW_CON_FLD_RDMA1_SLOW_EN, (reg32), (val))

#define OVL_RDMA1_FIFO_CTRL_SET_RDMA1_FIFO_UND_EN(reg32, val)                           \
                REG_FLD_SET(OVL_RDMA1_FIFO_CTRL_FLD_RDMA1_FIFO_UND_EN, (reg32), (val))
#define OVL_RDMA1_FIFO_CTRL_SET_RDMA1_FIFO_SIZE(reg32, val)                             \
                REG_FLD_SET(OVL_RDMA1_FIFO_CTRL_FLD_RDMA1_FIFO_SIZE, (reg32), (val))
#define OVL_RDMA1_FIFO_CTRL_SET_RDMA1_FIFO_THRD(reg32, val)                             \
                REG_FLD_SET(OVL_RDMA1_FIFO_CTRL_FLD_RDMA1_FIFO_THRD, (reg32), (val))

#define OVL_RDMA2_CTRL_SET_RDMA2_FIFO_USED_SIZE(reg32, val)                             \
                REG_FLD_SET(OVL_RDMA2_CTRL_FLD_RDMA2_FIFO_USED_SIZE, (reg32), (val))
#define OVL_RDMA2_CTRL_SET_RDMA2_INTERLACE(reg32, val)                                  \
                REG_FLD_SET(OVL_RDMA2_CTRL_FLD_RDMA2_INTERLACE, (reg32), (val))
#define OVL_RDMA2_CTRL_SET_RDMA2_EN(reg32, val)                                         \
                REG_FLD_SET(OVL_RDMA2_CTRL_FLD_RDMA2_EN, (reg32), (val))

#define OVL_RDMA2_MEM_GMC_SETTING1_SET_RDMA2_PRE_ULTRA_THRESHOLD_HIGH_OFS(reg32, val)   \
                REG_FLD_SET(OVL_RDMA2_MEM_GMC_SETTING1_FLD_RDMA2_PRE_ULTRA_THRESHOLD_HIGH_OFS,    \
                (reg32), (val))
#define OVL_RDMA2_MEM_GMC_SETTING1_SET_RDMA2_ULTRA_THRESHOLD_HIGH_OFS(reg32, val)       \
                REG_FLD_SET(OVL_RDMA2_MEM_GMC_SETTING1_FLD_RDMA2_ULTRA_THRESHOLD_HIGH_OFS,        \
                (reg32), (val))
#define OVL_RDMA2_MEM_GMC_SETTING1_SET_RDMA2_OSTD_PREULTRA_TH(reg32, val)               \
                REG_FLD_SET(OVL_RDMA2_MEM_GMC_SETTING1_FLD_RDMA2_OSTD_PREULTRA_TH, (reg32), (val))
#define OVL_RDMA2_MEM_GMC_SETTING1_SET_RDMA2_OSTD_ULTRA_TH(reg32, val)                  \
                REG_FLD_SET(OVL_RDMA2_MEM_GMC_SETTING1_FLD_RDMA2_OSTD_ULTRA_TH, (reg32), (val))

#define OVL_RDMA2_MEM_SLOW_CON_SET_RDMA2_SLOW_CNT(reg32, val)                           \
                REG_FLD_SET(OVL_RDMA2_MEM_SLOW_CON_FLD_RDMA2_SLOW_CNT, (reg32), (val))
#define OVL_RDMA2_MEM_SLOW_CON_SET_RDMA2_SLOW_EN(reg32, val)                            \
                REG_FLD_SET(OVL_RDMA2_MEM_SLOW_CON_FLD_RDMA2_SLOW_EN, (reg32), (val))

#define OVL_RDMA2_FIFO_CTRL_SET_RDMA2_FIFO_UND_EN(reg32, val)                           \
                REG_FLD_SET(OVL_RDMA2_FIFO_CTRL_FLD_RDMA2_FIFO_UND_EN, (reg32), (val))
#define OVL_RDMA2_FIFO_CTRL_SET_RDMA2_FIFO_SIZE(reg32, val)                             \
                REG_FLD_SET(OVL_RDMA2_FIFO_CTRL_FLD_RDMA2_FIFO_SIZE, (reg32), (val))
#define OVL_RDMA2_FIFO_CTRL_SET_RDMA2_FIFO_THRD(reg32, val)                             \
                REG_FLD_SET(OVL_RDMA2_FIFO_CTRL_FLD_RDMA2_FIFO_THRD, (reg32), (val))

#define OVL_RDMA3_CTRL_SET_RDMA3_FIFO_USED_SIZE(reg32, val)                             \
                REG_FLD_SET(OVL_RDMA3_CTRL_FLD_RDMA3_FIFO_USED_SIZE, (reg32), (val))
#define OVL_RDMA3_CTRL_SET_RDMA3_INTERLACE(reg32, val)                                  \
                REG_FLD_SET(OVL_RDMA3_CTRL_FLD_RDMA3_INTERLACE, (reg32), (val))
#define OVL_RDMA3_CTRL_SET_RDMA3_EN(reg32, val)                                         \
                REG_FLD_SET(OVL_RDMA3_CTRL_FLD_RDMA3_EN, (reg32), (val))

#define OVL_RDMA3_MEM_GMC_SETTING1_SET_RDMA3_PRE_ULTRA_THRESHOLD_HIGH_OFS(reg32, val)   \
                REG_FLD_SET(OVL_RDMA3_MEM_GMC_SETTING1_FLD_RDMA3_PRE_ULTRA_THRESHOLD_HIGH_OFS,    \
                (reg32), (val))
#define OVL_RDMA3_MEM_GMC_SETTING1_SET_RDMA3_ULTRA_THRESHOLD_HIGH_OFS(reg32, val)       \
                REG_FLD_SET(OVL_RDMA3_MEM_GMC_SETTING1_FLD_RDMA3_ULTRA_THRESHOLD_HIGH_OFS,        \
                (reg32), (val))
#define OVL_RDMA3_MEM_GMC_SETTING1_SET_RDMA3_OSTD_PREULTRA_TH(reg32, val)               \
                REG_FLD_SET(OVL_RDMA3_MEM_GMC_SETTING1_FLD_RDMA3_OSTD_PREULTRA_TH, (reg32), (val))
#define OVL_RDMA3_MEM_GMC_SETTING1_SET_RDMA3_OSTD_ULTRA_TH(reg32, val)                  \
                REG_FLD_SET(OVL_RDMA3_MEM_GMC_SETTING1_FLD_RDMA3_OSTD_ULTRA_TH, (reg32), (val))

#define OVL_RDMA3_MEM_SLOW_CON_SET_RDMA3_SLOW_CNT(reg32, val)                           \
                REG_FLD_SET(OVL_RDMA3_MEM_SLOW_CON_FLD_RDMA3_SLOW_CNT, (reg32), (val))
#define OVL_RDMA3_MEM_SLOW_CON_SET_RDMA3_SLOW_EN(reg32, val)                            \
                REG_FLD_SET(OVL_RDMA3_MEM_SLOW_CON_FLD_RDMA3_SLOW_EN, (reg32), (val))

#define OVL_RDMA3_FIFO_CTRL_SET_RDMA3_FIFO_UND_EN(reg32, val)                           \
                REG_FLD_SET(OVL_RDMA3_FIFO_CTRL_FLD_RDMA3_FIFO_UND_EN, (reg32), (val))
#define OVL_RDMA3_FIFO_CTRL_SET_RDMA3_FIFO_SIZE(reg32, val)                             \
                REG_FLD_SET(OVL_RDMA3_FIFO_CTRL_FLD_RDMA3_FIFO_SIZE, (reg32), (val))
#define OVL_RDMA3_FIFO_CTRL_SET_RDMA3_FIFO_THRD(reg32, val)                             \
                REG_FLD_SET(OVL_RDMA3_FIFO_CTRL_FLD_RDMA3_FIFO_THRD, (reg32), (val))

#define OVL_L0_Y2R_PARA_R0_SET_C_CF_RMU(reg32, val)                                     \
                REG_FLD_SET(OVL_L0_Y2R_PARA_R0_FLD_C_CF_RMU, (reg32), (val))
#define OVL_L0_Y2R_PARA_R0_SET_C_CF_RMY(reg32, val)                                     \
                REG_FLD_SET(OVL_L0_Y2R_PARA_R0_FLD_C_CF_RMY, (reg32), (val))

#define OVL_L0_Y2R_PARA_R1_SET_C_CF_RMV(reg32, val)                                     \
                REG_FLD_SET(OVL_L0_Y2R_PARA_R1_FLD_C_CF_RMV, (reg32), (val))

#define OVL_L0_Y2R_PARA_G0_SET_C_CF_GMU(reg32, val)                                     \
                REG_FLD_SET(OVL_L0_Y2R_PARA_G0_FLD_C_CF_GMU, (reg32), (val))
#define OVL_L0_Y2R_PARA_G0_SET_C_CF_GMY(reg32, val)                                     \
                REG_FLD_SET(OVL_L0_Y2R_PARA_G0_FLD_C_CF_GMY, (reg32), (val))

#define OVL_L0_Y2R_PARA_G1_SET_C_CF_GMV(reg32, val)                                     \
                REG_FLD_SET(OVL_L0_Y2R_PARA_G1_FLD_C_CF_GMV, (reg32), (val))

#define OVL_L0_Y2R_PARA_B0_SET_C_CF_BMU(reg32, val)                                     \
                REG_FLD_SET(OVL_L0_Y2R_PARA_B0_FLD_C_CF_BMU, (reg32), (val))
#define OVL_L0_Y2R_PARA_B0_SET_C_CF_BMY(reg32, val)                                     \
                REG_FLD_SET(OVL_L0_Y2R_PARA_B0_FLD_C_CF_BMY, (reg32), (val))

#define OVL_L0_Y2R_PARA_B1_SET_C_CF_BMV(reg32, val)                                     \
                REG_FLD_SET(OVL_L0_Y2R_PARA_B1_FLD_C_CF_BMV, (reg32), (val))

#define OVL_L0_Y2R_PARA_YUV_A_0_SET_C_CF_UA(reg32, val)                                 \
                REG_FLD_SET(OVL_L0_Y2R_PARA_YUV_A_0_FLD_C_CF_UA, (reg32), (val))
#define OVL_L0_Y2R_PARA_YUV_A_0_SET_C_CF_YA(reg32, val)                                 \
                REG_FLD_SET(OVL_L0_Y2R_PARA_YUV_A_0_FLD_C_CF_YA, (reg32), (val))

#define OVL_L0_Y2R_PARA_YUV_A_1_SET_C_CF_VA(reg32, val)                                 \
                REG_FLD_SET(OVL_L0_Y2R_PARA_YUV_A_1_FLD_C_CF_VA, (reg32), (val))

#define OVL_L0_Y2R_PARA_RGB_A_0_SET_C_CF_GA(reg32, val)                                 \
                REG_FLD_SET(OVL_L0_Y2R_PARA_RGB_A_0_FLD_C_CF_GA, (reg32), (val))
#define OVL_L0_Y2R_PARA_RGB_A_0_SET_C_CF_RA(reg32, val)                                 \
                REG_FLD_SET(OVL_L0_Y2R_PARA_RGB_A_0_FLD_C_CF_RA, (reg32), (val))

#define OVL_L0_Y2R_PARA_RGB_A_1_SET_C_CF_BA(reg32, val)                                 \
                REG_FLD_SET(OVL_L0_Y2R_PARA_RGB_A_1_FLD_C_CF_BA, (reg32), (val))

#define OVL_L1_Y2R_PARA_R0_SET_C_CF_RMU(reg32, val)                                     \
                REG_FLD_SET(OVL_L1_Y2R_PARA_R0_FLD_C_CF_RMU, (reg32), (val))
#define OVL_L1_Y2R_PARA_R0_SET_C_CF_RMY(reg32, val)                                     \
                REG_FLD_SET(OVL_L1_Y2R_PARA_R0_FLD_C_CF_RMY, (reg32), (val))

#define OVL_L1_Y2R_PARA_R1_SET_C_CF_RMV(reg32, val)                                     \
                REG_FLD_SET(OVL_L1_Y2R_PARA_R1_FLD_C_CF_RMV, (reg32), (val))

#define OVL_L1_Y2R_PARA_G0_SET_C_CF_GMU(reg32, val)                                     \
                REG_FLD_SET(OVL_L1_Y2R_PARA_G0_FLD_C_CF_GMU, (reg32), (val))
#define OVL_L1_Y2R_PARA_G0_SET_C_CF_GMY(reg32, val)                                     \
                REG_FLD_SET(OVL_L1_Y2R_PARA_G0_FLD_C_CF_GMY, (reg32), (val))

#define OVL_L1_Y2R_PARA_G1_SET_C_CF_GMV(reg32, val)                                     \
                REG_FLD_SET(OVL_L1_Y2R_PARA_G1_FLD_C_CF_GMV, (reg32), (val))

#define OVL_L1_Y2R_PARA_B0_SET_C_CF_BMU(reg32, val)                                     \
                REG_FLD_SET(OVL_L1_Y2R_PARA_B0_FLD_C_CF_BMU, (reg32), (val))
#define OVL_L1_Y2R_PARA_B0_SET_C_CF_BMY(reg32, val)                                     \
                REG_FLD_SET(OVL_L1_Y2R_PARA_B0_FLD_C_CF_BMY, (reg32), (val))

#define OVL_L1_Y2R_PARA_B1_SET_C_CF_BMV(reg32, val)                                     \
                REG_FLD_SET(OVL_L1_Y2R_PARA_B1_FLD_C_CF_BMV, (reg32), (val))

#define OVL_L1_Y2R_PARA_YUV_A_0_SET_C_CF_UA(reg32, val)                                 \
                REG_FLD_SET(OVL_L1_Y2R_PARA_YUV_A_0_FLD_C_CF_UA, (reg32), (val))
#define OVL_L1_Y2R_PARA_YUV_A_0_SET_C_CF_YA(reg32, val)                                 \
                REG_FLD_SET(OVL_L1_Y2R_PARA_YUV_A_0_FLD_C_CF_YA, (reg32), (val))

#define OVL_L1_Y2R_PARA_YUV_A_1_SET_C_CF_VA(reg32, val)                                 \
                REG_FLD_SET(OVL_L1_Y2R_PARA_YUV_A_1_FLD_C_CF_VA, (reg32), (val))

#define OVL_L1_Y2R_PARA_RGB_A_0_SET_C_CF_GA(reg32, val)                                 \
                REG_FLD_SET(OVL_L1_Y2R_PARA_RGB_A_0_FLD_C_CF_GA, (reg32), (val))
#define OVL_L1_Y2R_PARA_RGB_A_0_SET_C_CF_RA(reg32, val)                                 \
                REG_FLD_SET(OVL_L1_Y2R_PARA_RGB_A_0_FLD_C_CF_RA, (reg32), (val))

#define OVL_L1_Y2R_PARA_RGB_A_1_SET_C_CF_BA(reg32, val)                                 \
                REG_FLD_SET(OVL_L1_Y2R_PARA_RGB_A_1_FLD_C_CF_BA, (reg32), (val))

#define OVL_L2_Y2R_PARA_R0_SET_C_CF_RMU(reg32, val)                                     \
                REG_FLD_SET(OVL_L2_Y2R_PARA_R0_FLD_C_CF_RMU, (reg32), (val))
#define OVL_L2_Y2R_PARA_R0_SET_C_CF_RMY(reg32, val)                                     \
                REG_FLD_SET(OVL_L2_Y2R_PARA_R0_FLD_C_CF_RMY, (reg32), (val))

#define OVL_L2_Y2R_PARA_R1_SET_C_CF_RMV(reg32, val)                                     \
                REG_FLD_SET(OVL_L2_Y2R_PARA_R1_FLD_C_CF_RMV, (reg32), (val))

#define OVL_L2_Y2R_PARA_G0_SET_C_CF_GMU(reg32, val)                                     \
                REG_FLD_SET(OVL_L2_Y2R_PARA_G0_FLD_C_CF_GMU, (reg32), (val))
#define OVL_L2_Y2R_PARA_G0_SET_C_CF_GMY(reg32, val)                                     \
                REG_FLD_SET(OVL_L2_Y2R_PARA_G0_FLD_C_CF_GMY, (reg32), (val))

#define OVL_L2_Y2R_PARA_G1_SET_C_CF_GMV(reg32, val)                                     \
                REG_FLD_SET(OVL_L2_Y2R_PARA_G1_FLD_C_CF_GMV, (reg32), (val))

#define OVL_L2_Y2R_PARA_B0_SET_C_CF_BMU(reg32, val)                                     \
                REG_FLD_SET(OVL_L2_Y2R_PARA_B0_FLD_C_CF_BMU, (reg32), (val))
#define OVL_L2_Y2R_PARA_B0_SET_C_CF_BMY(reg32, val)                                     \
                REG_FLD_SET(OVL_L2_Y2R_PARA_B0_FLD_C_CF_BMY, (reg32), (val))

#define OVL_L2_Y2R_PARA_B1_SET_C_CF_BMV(reg32, val)                                     \
                REG_FLD_SET(OVL_L2_Y2R_PARA_B1_FLD_C_CF_BMV, (reg32), (val))

#define OVL_L2_Y2R_PARA_YUV_A_0_SET_C_CF_UA(reg32, val)                                 \
                REG_FLD_SET(OVL_L2_Y2R_PARA_YUV_A_0_FLD_C_CF_UA, (reg32), (val))
#define OVL_L2_Y2R_PARA_YUV_A_0_SET_C_CF_YA(reg32, val)                                 \
                REG_FLD_SET(OVL_L2_Y2R_PARA_YUV_A_0_FLD_C_CF_YA, (reg32), (val))

#define OVL_L2_Y2R_PARA_YUV_A_1_SET_C_CF_VA(reg32, val)                                 \
                REG_FLD_SET(OVL_L2_Y2R_PARA_YUV_A_1_FLD_C_CF_VA, (reg32), (val))

#define OVL_L2_Y2R_PARA_RGB_A_0_SET_C_CF_GA(reg32, val)                                 \
                REG_FLD_SET(OVL_L2_Y2R_PARA_RGB_A_0_FLD_C_CF_GA, (reg32), (val))
#define OVL_L2_Y2R_PARA_RGB_A_0_SET_C_CF_RA(reg32, val)                                 \
                REG_FLD_SET(OVL_L2_Y2R_PARA_RGB_A_0_FLD_C_CF_RA, (reg32), (val))

#define OVL_L2_Y2R_PARA_RGB_A_1_SET_C_CF_BA(reg32, val)                                 \
                REG_FLD_SET(OVL_L2_Y2R_PARA_RGB_A_1_FLD_C_CF_BA, (reg32), (val))

#define OVL_L3_Y2R_PARA_R0_SET_C_CF_RMU(reg32, val)                                     \
                REG_FLD_SET(OVL_L3_Y2R_PARA_R0_FLD_C_CF_RMU, (reg32), (val))
#define OVL_L3_Y2R_PARA_R0_SET_C_CF_RMY(reg32, val)                                     \
                REG_FLD_SET(OVL_L3_Y2R_PARA_R0_FLD_C_CF_RMY, (reg32), (val))

#define OVL_L3_Y2R_PARA_R1_SET_C_CF_RMV(reg32, val)                                     \
                REG_FLD_SET(OVL_L3_Y2R_PARA_R1_FLD_C_CF_RMV, (reg32), (val))

#define OVL_L3_Y2R_PARA_G0_SET_C_CF_GMU(reg32, val)                                     \
                REG_FLD_SET(OVL_L3_Y2R_PARA_G0_FLD_C_CF_GMU, (reg32), (val))
#define OVL_L3_Y2R_PARA_G0_SET_C_CF_GMY(reg32, val)                                     \
                REG_FLD_SET(OVL_L3_Y2R_PARA_G0_FLD_C_CF_GMY, (reg32), (val))

#define OVL_L3_Y2R_PARA_G1_SET_C_CF_GMV(reg32, val)                                     \
                REG_FLD_SET(OVL_L3_Y2R_PARA_G1_FLD_C_CF_GMV, (reg32), (val))

#define OVL_L3_Y2R_PARA_B0_SET_C_CF_BMU(reg32, val)                                     \
                REG_FLD_SET(OVL_L3_Y2R_PARA_B0_FLD_C_CF_BMU, (reg32), (val))
#define OVL_L3_Y2R_PARA_B0_SET_C_CF_BMY(reg32, val)                                     \
                REG_FLD_SET(OVL_L3_Y2R_PARA_B0_FLD_C_CF_BMY, (reg32), (val))

#define OVL_L3_Y2R_PARA_B1_SET_C_CF_BMV(reg32, val)                                     \
                REG_FLD_SET(OVL_L3_Y2R_PARA_B1_FLD_C_CF_BMV, (reg32), (val))

#define OVL_L3_Y2R_PARA_YUV_A_0_SET_C_CF_UA(reg32, val)                                 \
                REG_FLD_SET(OVL_L3_Y2R_PARA_YUV_A_0_FLD_C_CF_UA, (reg32), (val))
#define OVL_L3_Y2R_PARA_YUV_A_0_SET_C_CF_YA(reg32, val)                                 \
                REG_FLD_SET(OVL_L3_Y2R_PARA_YUV_A_0_FLD_C_CF_YA, (reg32), (val))

#define OVL_L3_Y2R_PARA_YUV_A_1_SET_C_CF_VA(reg32, val)                                 \
                REG_FLD_SET(OVL_L3_Y2R_PARA_YUV_A_1_FLD_C_CF_VA, (reg32), (val))

#define OVL_L3_Y2R_PARA_RGB_A_0_SET_C_CF_GA(reg32, val)                                 \
                REG_FLD_SET(OVL_L3_Y2R_PARA_RGB_A_0_FLD_C_CF_GA, (reg32), (val))
#define OVL_L3_Y2R_PARA_RGB_A_0_SET_C_CF_RA(reg32, val)                                 \
                REG_FLD_SET(OVL_L3_Y2R_PARA_RGB_A_0_FLD_C_CF_RA, (reg32), (val))

#define OVL_L3_Y2R_PARA_RGB_A_1_SET_C_CF_BA(reg32, val)                                 \
                REG_FLD_SET(OVL_L3_Y2R_PARA_RGB_A_1_FLD_C_CF_BA, (reg32), (val))

#define OVL_DEBUG_MON_SEL_SET_DBG_MON_SEL(reg32, val)                                   \
                REG_FLD_SET(OVL_DEBUG_MON_SEL_FLD_DBG_MON_SEL, (reg32), (val))

#define OVL_BLD_EXT_SET_EL2_MINUS_BLD(reg32, val)                                       \
                REG_FLD_SET(OVL_BLD_EXT_FLD_EL2_MINUS_BLD, (reg32), (val))
#define OVL_BLD_EXT_SET_EL1_MINUS_BLD(reg32, val)                                       \
                REG_FLD_SET(OVL_BLD_EXT_FLD_EL1_MINUS_BLD, (reg32), (val))
#define OVL_BLD_EXT_SET_EL0_MINUS_BLD(reg32, val)                                       \
                REG_FLD_SET(OVL_BLD_EXT_FLD_EL0_MINUS_BLD, (reg32), (val))
#define OVL_BLD_EXT_SET_LC_MINUS_BLD(reg32, val)                                        \
                REG_FLD_SET(OVL_BLD_EXT_FLD_LC_MINUS_BLD, (reg32), (val))
#define OVL_BLD_EXT_SET_L3_MINUS_BLD(reg32, val)                                        \
                REG_FLD_SET(OVL_BLD_EXT_FLD_L3_MINUS_BLD, (reg32), (val))
#define OVL_BLD_EXT_SET_L2_MINUS_BLD(reg32, val)                                        \
                REG_FLD_SET(OVL_BLD_EXT_FLD_L2_MINUS_BLD, (reg32), (val))
#define OVL_BLD_EXT_SET_L1_MINUS_BLD(reg32, val)                                        \
                REG_FLD_SET(OVL_BLD_EXT_FLD_L1_MINUS_BLD, (reg32), (val))
#define OVL_BLD_EXT_SET_L0_MINUS_BLD(reg32, val)                                        \
                REG_FLD_SET(OVL_BLD_EXT_FLD_L0_MINUS_BLD, (reg32), (val))

#define OVL_RDMA0_MEM_GMC_SETTING2_SET_RDMA0_FORCE_REQ_THRESHOLD(reg32, val)            \
                REG_FLD_SET(OVL_RDMA0_MEM_GMC_SETTING2_FLD_RDMA0_FORCE_REQ_THRESHOLD,   \
                (reg32), (val))
#define OVL_RDMA0_MEM_GMC_SETTING2_SET_RDMA0_REQ_THRESHOLD_ULTRA(reg32, val)            \
                REG_FLD_SET(OVL_RDMA0_MEM_GMC_SETTING2_FLD_RDMA0_REQ_THRESHOLD_ULTRA,   \
                (reg32), (val))
#define OVL_RDMA0_MEM_GMC_SETTING2_SET_RDMA0_REQ_THRESHOLD_PREULTRA(reg32, val)         \
                REG_FLD_SET(OVL_RDMA0_MEM_GMC_SETTING2_FLD_RDMA0_REQ_THRESHOLD_PREULTRA,  \
                (reg32), (val))
#define OVL_RDMA0_MEM_GMC_SETTING2_SET_RDMA0_ISSUE_REQ_THRESHOLD_URG(reg32, val)        \
                REG_FLD_SET(OVL_RDMA0_MEM_GMC_SETTING2_FLD_RDMA0_ISSUE_REQ_THRESHOLD_URG, \
                (reg32), (val))
#define OVL_RDMA0_MEM_GMC_SETTING2_SET_RDMA0_ISSUE_REQ_THRESHOLD(reg32, val)            \
                REG_FLD_SET(OVL_RDMA0_MEM_GMC_SETTING2_FLD_RDMA0_ISSUE_REQ_THRESHOLD,   \
                (reg32), (val))

#define OVL_RDMA1_MEM_GMC_SETTING2_SET_RDMA1_FORCE_REQ_THRESHOLD(reg32, val)            \
                REG_FLD_SET(OVL_RDMA1_MEM_GMC_SETTING2_FLD_RDMA1_FORCE_REQ_THRESHOLD,   \
                (reg32), (val))
#define OVL_RDMA1_MEM_GMC_SETTING2_SET_RDMA1_REQ_THRESHOLD_ULTRA(reg32, val)            \
                REG_FLD_SET(OVL_RDMA1_MEM_GMC_SETTING2_FLD_RDMA1_REQ_THRESHOLD_ULTRA,   \
                (reg32), (val))
#define OVL_RDMA1_MEM_GMC_SETTING2_SET_RDMA1_REQ_THRESHOLD_PREULTRA(reg32, val)         \
                REG_FLD_SET(OVL_RDMA1_MEM_GMC_SETTING2_FLD_RDMA1_REQ_THRESHOLD_PREULTRA,  \
                (reg32), (val))
#define OVL_RDMA1_MEM_GMC_SETTING2_SET_RDMA1_ISSUE_REQ_THRESHOLD_URG(reg32, val)        \
                REG_FLD_SET(OVL_RDMA1_MEM_GMC_SETTING2_FLD_RDMA1_ISSUE_REQ_THRESHOLD_URG,  \
                (reg32), (val))
#define OVL_RDMA1_MEM_GMC_SETTING2_SET_RDMA1_ISSUE_REQ_THRESHOLD(reg32, val)            \
                REG_FLD_SET(OVL_RDMA1_MEM_GMC_SETTING2_FLD_RDMA1_ISSUE_REQ_THRESHOLD,   \
                (reg32), (val))

#define OVL_RDMA2_MEM_GMC_SETTING2_SET_RDMA2_FORCE_REQ_THRESHOLD(reg32, val)            \
                REG_FLD_SET(OVL_RDMA2_MEM_GMC_SETTING2_FLD_RDMA2_FORCE_REQ_THRESHOLD,   \
                (reg32), (val))
#define OVL_RDMA2_MEM_GMC_SETTING2_SET_RDMA2_REQ_THRESHOLD_ULTRA(reg32, val)            \
                REG_FLD_SET(OVL_RDMA2_MEM_GMC_SETTING2_FLD_RDMA2_REQ_THRESHOLD_ULTRA,   \
                (reg32), (val))
#define OVL_RDMA2_MEM_GMC_SETTING2_SET_RDMA2_REQ_THRESHOLD_PREULTRA(reg32, val)         \
                REG_FLD_SET(OVL_RDMA2_MEM_GMC_SETTING2_FLD_RDMA2_REQ_THRESHOLD_PREULTRA,  \
                (reg32), (val))
#define OVL_RDMA2_MEM_GMC_SETTING2_SET_RDMA2_ISSUE_REQ_THRESHOLD_URG(reg32, val)        \
                REG_FLD_SET(OVL_RDMA2_MEM_GMC_SETTING2_FLD_RDMA2_ISSUE_REQ_THRESHOLD_URG,  \
                (reg32), (val))
#define OVL_RDMA2_MEM_GMC_SETTING2_SET_RDMA2_ISSUE_REQ_THRESHOLD(reg32, val)            \
                REG_FLD_SET(OVL_RDMA2_MEM_GMC_SETTING2_FLD_RDMA2_ISSUE_REQ_THRESHOLD,   \
                (reg32), (val))

#define OVL_RDMA3_MEM_GMC_SETTING2_SET_RDMA3_FORCE_REQ_THRESHOLD(reg32, val)            \
                REG_FLD_SET(OVL_RDMA3_MEM_GMC_SETTING2_FLD_RDMA3_FORCE_REQ_THRESHOLD,   \
                (reg32), (val))
#define OVL_RDMA3_MEM_GMC_SETTING2_SET_RDMA3_REQ_THRESHOLD_ULTRA(reg32, val)            \
                REG_FLD_SET(OVL_RDMA3_MEM_GMC_SETTING2_FLD_RDMA3_REQ_THRESHOLD_ULTRA,   \
                (reg32), (val))
#define OVL_RDMA3_MEM_GMC_SETTING2_SET_RDMA3_REQ_THRESHOLD_PREULTRA(reg32, val)         \
                REG_FLD_SET(OVL_RDMA3_MEM_GMC_SETTING2_FLD_RDMA3_REQ_THRESHOLD_PREULTRA,  \
                (reg32), (val))
#define OVL_RDMA3_MEM_GMC_SETTING2_SET_RDMA3_ISSUE_REQ_THRESHOLD_URG(reg32, val)        \
                REG_FLD_SET(OVL_RDMA3_MEM_GMC_SETTING2_FLD_RDMA3_ISSUE_REQ_THRESHOLD_URG,  \
                (reg32), (val))
#define OVL_RDMA3_MEM_GMC_SETTING2_SET_RDMA3_ISSUE_REQ_THRESHOLD(reg32, val)            \
                REG_FLD_SET(OVL_RDMA3_MEM_GMC_SETTING2_FLD_RDMA3_ISSUE_REQ_THRESHOLD,   \
                (reg32), (val))

#define OVL_RDMA_BURST_CON0_SET_BURST_128B_BOUND(reg32, val)                            \
                REG_FLD_SET(OVL_RDMA_BURST_CON0_FLD_BURST_128B_BOUND, (reg32), (val))
#define OVL_RDMA_BURST_CON0_SET_BURST15A_32B(reg32, val)                                \
                REG_FLD_SET(OVL_RDMA_BURST_CON0_FLD_BURST15A_32B, (reg32), (val))
#define OVL_RDMA_BURST_CON0_SET_BURST14A_32B(reg32, val)                                \
                REG_FLD_SET(OVL_RDMA_BURST_CON0_FLD_BURST14A_32B, (reg32), (val))
#define OVL_RDMA_BURST_CON0_SET_BURST13A_32B(reg32, val)                                \
                REG_FLD_SET(OVL_RDMA_BURST_CON0_FLD_BURST13A_32B, (reg32), (val))
#define OVL_RDMA_BURST_CON0_SET_BURST12A_32B(reg32, val)                                \
                REG_FLD_SET(OVL_RDMA_BURST_CON0_FLD_BURST12A_32B, (reg32), (val))
#define OVL_RDMA_BURST_CON0_SET_BURST11A_32B(reg32, val)                                \
                REG_FLD_SET(OVL_RDMA_BURST_CON0_FLD_BURST11A_32B, (reg32), (val))
#define OVL_RDMA_BURST_CON0_SET_BURST10A_32B(reg32, val)                                \
                REG_FLD_SET(OVL_RDMA_BURST_CON0_FLD_BURST10A_32B, (reg32), (val))
#define OVL_RDMA_BURST_CON0_SET_BURST9A_32B(reg32, val)                                 \
                REG_FLD_SET(OVL_RDMA_BURST_CON0_FLD_BURST9A_32B, (reg32), (val))

#define OVL_RDMA_BURST_CON1_SET_BURST15A_N32B(reg32, val)                               \
                REG_FLD_SET(OVL_RDMA_BURST_CON1_FLD_BURST15A_N32B, (reg32), (val))
#define OVL_RDMA_BURST_CON1_SET_BURST14A_N32B(reg32, val)                               \
                REG_FLD_SET(OVL_RDMA_BURST_CON1_FLD_BURST14A_N32B, (reg32), (val))
#define OVL_RDMA_BURST_CON1_SET_BURST13A_N32B(reg32, val)                               \
                REG_FLD_SET(OVL_RDMA_BURST_CON1_FLD_BURST13A_N32B, (reg32), (val))
#define OVL_RDMA_BURST_CON1_SET_BURST12A_N32B(reg32, val)                               \
                REG_FLD_SET(OVL_RDMA_BURST_CON1_FLD_BURST12A_N32B, (reg32), (val))
#define OVL_RDMA_BURST_CON1_SET_BURST11A_N32B(reg32, val)                               \
                REG_FLD_SET(OVL_RDMA_BURST_CON1_FLD_BURST11A_N32B, (reg32), (val))
#define OVL_RDMA_BURST_CON1_SET_BURST10A_N32B(reg32, val)                               \
                REG_FLD_SET(OVL_RDMA_BURST_CON1_FLD_BURST10A_N32B, (reg32), (val))
#define OVL_RDMA_BURST_CON1_SET_BURST9A_N32B(reg32, val)                                \
                REG_FLD_SET(OVL_RDMA_BURST_CON1_FLD_BURST9A_N32B, (reg32), (val))

#define OVL_RDMA_GREQ_NUM_SET_IOBUF_FLUSH_ULTRA(reg32, val)                             \
                REG_FLD_SET(OVL_RDMA_GREQ_NUM_FLD_IOBUF_FLUSH_ULTRA, (reg32), (val))
#define OVL_RDMA_GREQ_NUM_SET_IOBUF_FLUSH_PREULTRA(reg32, val)                          \
                REG_FLD_SET(OVL_RDMA_GREQ_NUM_FLD_IOBUF_FLUSH_PREULTRA, (reg32), (val))
#define OVL_RDMA_GREQ_NUM_SET_GRP_BRK_STOP(reg32, val)                                  \
                REG_FLD_SET(OVL_RDMA_GREQ_NUM_FLD_GRP_BRK_STOP, (reg32), (val))
#define OVL_RDMA_GREQ_NUM_SET_GRP_END_STOP(reg32, val)                                  \
                REG_FLD_SET(OVL_RDMA_GREQ_NUM_FLD_GRP_END_STOP, (reg32), (val))
#define OVL_RDMA_GREQ_NUM_SET_GREQ_STOP_EN(reg32, val)                                  \
                REG_FLD_SET(OVL_RDMA_GREQ_NUM_FLD_GREQ_STOP_EN, (reg32), (val))
#define OVL_RDMA_GREQ_NUM_SET_GREQ_DIS_CNT(reg32, val)                                  \
                REG_FLD_SET(OVL_RDMA_GREQ_NUM_FLD_GREQ_DIS_CNT, (reg32), (val))
#define OVL_RDMA_GREQ_NUM_SET_OSTD_GREQ_NUM(reg32, val)                                 \
                REG_FLD_SET(OVL_RDMA_GREQ_NUM_FLD_OSTD_GREQ_NUM, (reg32), (val))
#define OVL_RDMA_GREQ_NUM_SET_LAYER3_GREQ_NUM(reg32, val)                               \
                REG_FLD_SET(OVL_RDMA_GREQ_NUM_FLD_LAYER3_GREQ_NUM, (reg32), (val))
#define OVL_RDMA_GREQ_NUM_SET_LAYER2_GREQ_NUM(reg32, val)                               \
                REG_FLD_SET(OVL_RDMA_GREQ_NUM_FLD_LAYER2_GREQ_NUM, (reg32), (val))
#define OVL_RDMA_GREQ_NUM_SET_LAYER1_GREQ_NUM(reg32, val)                               \
                REG_FLD_SET(OVL_RDMA_GREQ_NUM_FLD_LAYER1_GREQ_NUM, (reg32), (val))
#define OVL_RDMA_GREQ_NUM_SET_LAYER0_GREQ_NUM(reg32, val)                               \
                REG_FLD_SET(OVL_RDMA_GREQ_NUM_FLD_LAYER0_GREQ_NUM, (reg32), (val))

#define OVL_RDMA_GREQ_URG_NUM_SET_GREQ_NUM_SHT(reg32, val)                              \
                REG_FLD_SET(OVL_RDMA_GREQ_URG_NUM_FLD_GREQ_NUM_SHT, (reg32), (val))
#define OVL_RDMA_GREQ_URG_NUM_SET_GREQ_NUM_SHT_VAL(reg32, val)                          \
                REG_FLD_SET(OVL_RDMA_GREQ_URG_NUM_FLD_GREQ_NUM_SHT_VAL, (reg32), (val))
#define OVL_RDMA_GREQ_URG_NUM_SET_ARG_URG_BIAS(reg32, val)                              \
                REG_FLD_SET(OVL_RDMA_GREQ_URG_NUM_FLD_ARG_URG_BIAS, (reg32), (val))
#define OVL_RDMA_GREQ_URG_NUM_SET_ARG_GREQ_URG_TH(reg32, val)                           \
                REG_FLD_SET(OVL_RDMA_GREQ_URG_NUM_FLD_ARG_GREQ_URG_TH, (reg32), (val))
#define OVL_RDMA_GREQ_URG_NUM_SET_LAYER3_GREQ_URG_NUM(reg32, val)                       \
                REG_FLD_SET(OVL_RDMA_GREQ_URG_NUM_FLD_LAYER3_GREQ_URG_NUM, (reg32), (val))
#define OVL_RDMA_GREQ_URG_NUM_SET_LAYER2_GREQ_URG_NUM(reg32, val)                       \
                REG_FLD_SET(OVL_RDMA_GREQ_URG_NUM_FLD_LAYER2_GREQ_URG_NUM, (reg32), (val))
#define OVL_RDMA_GREQ_URG_NUM_SET_LAYER1_GREQ_URG_NUM(reg32, val)                       \
                REG_FLD_SET(OVL_RDMA_GREQ_URG_NUM_FLD_LAYER1_GREQ_URG_NUM, (reg32), (val))
#define OVL_RDMA_GREQ_URG_NUM_SET_LAYER0_GREQ_URG_NUM(reg32, val)                       \
                REG_FLD_SET(OVL_RDMA_GREQ_URG_NUM_FLD_LAYER0_GREQ_URG_NUM, (reg32), (val))

#define OVL_DUMMY_REG_SET_OVERLAY_DUMMY(reg32, val)                                     \
                REG_FLD_SET(OVL_DUMMY_REG_FLD_OVERLAY_DUMMY, (reg32), (val))

#define OVL_GDRDY_PRD_SET_GDRDY_PRD(reg32, val)                                         \
                REG_FLD_SET(OVL_GDRDY_PRD_FLD_GDRDY_PRD, (reg32), (val))

#define OVL_RDMA_ULTRA_SRC_SET_ULTRA_RDMA_SRC(reg32, val)                               \
                                  \
                REG_FLD_SET(OVL_RDMA_ULTRA_SRC_FLD_ULTRA_RDMA_SRC, (reg32), (val))
#define OVL_RDMA_ULTRA_SRC_SET_ULTRA_ROI_END_SRC(reg32, val)                            \
                                  \
                REG_FLD_SET(OVL_RDMA_ULTRA_SRC_FLD_ULTRA_ROI_END_SRC, (reg32), (val))
#define OVL_RDMA_ULTRA_SRC_SET_ULTRA_SMI_SRC(reg32, val)                                \
                REG_FLD_SET(OVL_RDMA_ULTRA_SRC_FLD_ULTRA_SMI_SRC, (reg32), (val))
#define OVL_RDMA_ULTRA_SRC_SET_ULTRA_BUF_SRC(reg32, val)                                \
                REG_FLD_SET(OVL_RDMA_ULTRA_SRC_FLD_ULTRA_BUF_SRC, (reg32), (val))
#define OVL_RDMA_ULTRA_SRC_SET_PREULTRA_RDMA_SRC(reg32, val)                            \
                REG_FLD_SET(OVL_RDMA_ULTRA_SRC_FLD_PREULTRA_RDMA_SRC, (reg32), (val))
#define OVL_RDMA_ULTRA_SRC_SET_PREULTRA_ROI_END_SRC(reg32, val)                         \
                REG_FLD_SET(OVL_RDMA_ULTRA_SRC_FLD_PREULTRA_ROI_END_SRC, (reg32), (val))
#define OVL_RDMA_ULTRA_SRC_SET_PREULTRA_SMI_SRC(reg32, val)                             \
                REG_FLD_SET(OVL_RDMA_ULTRA_SRC_FLD_PREULTRA_SMI_SRC, (reg32), (val))
#define OVL_RDMA_ULTRA_SRC_SET_PREULTRA_BUF_SRC(reg32, val)                             \
                REG_FLD_SET(OVL_RDMA_ULTRA_SRC_FLD_PREULTRA_BUF_SRC, (reg32), (val))

#define OVL_RDMA0_BUF_LOW_TH_SET_RDMA0_PREULTRA_LOW_TH(reg32, val)                      \
                REG_FLD_SET(OVL_RDMA0_BUF_LOW_TH_FLD_RDMA0_PREULTRA_LOW_TH, (reg32), (val))
#define OVL_RDMA0_BUF_LOW_TH_SET_RDMA0_ULTRA_LOW_TH(reg32, val)                         \
                REG_FLD_SET(OVL_RDMA0_BUF_LOW_TH_FLD_RDMA0_ULTRA_LOW_TH, (reg32), (val))

#define OVL_RDMA1_BUF_LOW_TH_SET_RDMA1_PREULTRA_LOW_TH(reg32, val)                      \
                REG_FLD_SET(OVL_RDMA1_BUF_LOW_TH_FLD_RDMA1_PREULTRA_LOW_TH, (reg32), (val))
#define OVL_RDMA1_BUF_LOW_TH_SET_RDMA1_ULTRA_LOW_TH(reg32, val)                         \
                REG_FLD_SET(OVL_RDMA1_BUF_LOW_TH_FLD_RDMA1_ULTRA_LOW_TH, (reg32), (val))

#define OVL_RDMA2_BUF_LOW_TH_SET_RDMA2_PREULTRA_LOW_TH(reg32, val)                      \
                REG_FLD_SET(OVL_RDMA2_BUF_LOW_TH_FLD_RDMA2_PREULTRA_LOW_TH, (reg32), (val))
#define OVL_RDMA2_BUF_LOW_TH_SET_RDMA2_ULTRA_LOW_TH(reg32, val)                         \
                REG_FLD_SET(OVL_RDMA2_BUF_LOW_TH_FLD_RDMA2_ULTRA_LOW_TH, (reg32), (val))

#define OVL_RDMA3_BUF_LOW_TH_SET_RDMA3_PREULTRA_LOW_TH(reg32, val)                      \
                REG_FLD_SET(OVL_RDMA3_BUF_LOW_TH_FLD_RDMA3_PREULTRA_LOW_TH, (reg32), (val))
#define OVL_RDMA3_BUF_LOW_TH_SET_RDMA3_ULTRA_LOW_TH(reg32, val)                         \
                REG_FLD_SET(OVL_RDMA3_BUF_LOW_TH_FLD_RDMA3_ULTRA_LOW_TH, (reg32), (val))

#define OVL_RDMA0_BUF_HIGH_TH_SET_RDMA0_PREULTRA_HIGH_DIS(reg32, val)                   \
                REG_FLD_SET(OVL_RDMA0_BUF_HIGH_TH_FLD_RDMA0_PREULTRA_HIGH_DIS, (reg32), (val))
#define OVL_RDMA0_BUF_HIGH_TH_SET_RDMA0_PREULTRA_HIGH_TH(reg32, val)                    \
                REG_FLD_SET(OVL_RDMA0_BUF_HIGH_TH_FLD_RDMA0_PREULTRA_HIGH_TH, (reg32), (val))

#define OVL_RDMA1_BUF_HIGH_TH_SET_RDMA1_PREULTRA_HIGH_DIS(reg32, val)                   \
                REG_FLD_SET(OVL_RDMA1_BUF_HIGH_TH_FLD_RDMA1_PREULTRA_HIGH_DIS, (reg32), (val))
#define OVL_RDMA1_BUF_HIGH_TH_SET_RDMA1_PREULTRA_HIGH_TH(reg32, val)                    \
                REG_FLD_SET(OVL_RDMA1_BUF_HIGH_TH_FLD_RDMA1_PREULTRA_HIGH_TH, (reg32), (val))

#define OVL_RDMA2_BUF_HIGH_TH_SET_RDMA2_PREULTRA_HIGH_DIS(reg32, val)                   \
                REG_FLD_SET(OVL_RDMA2_BUF_HIGH_TH_FLD_RDMA2_PREULTRA_HIGH_DIS, (reg32), (val))
#define OVL_RDMA2_BUF_HIGH_TH_SET_RDMA2_PREULTRA_HIGH_TH(reg32, val)                    \
                REG_FLD_SET(OVL_RDMA2_BUF_HIGH_TH_FLD_RDMA2_PREULTRA_HIGH_TH, (reg32), (val))

#define OVL_RDMA3_BUF_HIGH_TH_SET_RDMA3_PREULTRA_HIGH_DIS(reg32, val)                   \
                REG_FLD_SET(OVL_RDMA3_BUF_HIGH_TH_FLD_RDMA3_PREULTRA_HIGH_DIS, (reg32), (val))
#define OVL_RDMA3_BUF_HIGH_TH_SET_RDMA3_PREULTRA_HIGH_TH(reg32, val)                    \
                REG_FLD_SET(OVL_RDMA3_BUF_HIGH_TH_FLD_RDMA3_PREULTRA_HIGH_TH, (reg32), (val))

#define OVL_SMI_DBG_SET_SMI_FSM(reg32, val)                                             \
                REG_FLD_SET(OVL_SMI_DBG_FLD_SMI_FSM, (reg32), (val))

#define OVL_GREQ_LAYER_CNT_SET_LAYER3_GREQ_CNT(reg32, val)                              \
                REG_FLD_SET(OVL_GREQ_LAYER_CNT_FLD_LAYER3_GREQ_CNT, (reg32), (val))
#define OVL_GREQ_LAYER_CNT_SET_LAYER2_GREQ_CNT(reg32, val)                              \
                REG_FLD_SET(OVL_GREQ_LAYER_CNT_FLD_LAYER2_GREQ_CNT, (reg32), (val))
#define OVL_GREQ_LAYER_CNT_SET_LAYER1_GREQ_CNT(reg32, val)                              \
                REG_FLD_SET(OVL_GREQ_LAYER_CNT_FLD_LAYER1_GREQ_CNT, (reg32), (val))
#define OVL_GREQ_LAYER_CNT_SET_LAYER0_GREQ_CNT(reg32, val)                              \
                REG_FLD_SET(OVL_GREQ_LAYER_CNT_FLD_LAYER0_GREQ_CNT, (reg32), (val))

#define OVL_GDRDY_PRD_NUM_SET_GDRDY_PRD_NUM(reg32, val)                                 \
                REG_FLD_SET(OVL_GDRDY_PRD_NUM_FLD_GDRDY_PRD_NUM, (reg32), (val))

#define OVL_FLOW_CTRL_DBG_SET_OVL_UPD_REG(reg32, val)                                   \
                REG_FLD_SET(OVL_FLOW_CTRL_DBG_FLD_OVL_UPD_REG, (reg32), (val))
#define OVL_FLOW_CTRL_DBG_SET_REG_UPDATE(reg32, val)                                    \
                REG_FLD_SET(OVL_FLOW_CTRL_DBG_FLD_REG_UPDATE, (reg32), (val))
#define OVL_FLOW_CTRL_DBG_SET_OVL_CLR(reg32, val)                                       \
                REG_FLD_SET(OVL_FLOW_CTRL_DBG_FLD_OVL_CLR, (reg32), (val))
#define OVL_FLOW_CTRL_DBG_SET_OVL_START(reg32, val)                                     \
                REG_FLD_SET(OVL_FLOW_CTRL_DBG_FLD_OVL_START, (reg32), (val))
#define OVL_FLOW_CTRL_DBG_SET_OVL_RUNNING(reg32, val)                                   \
                REG_FLD_SET(OVL_FLOW_CTRL_DBG_FLD_OVL_RUNNING, (reg32), (val))
#define OVL_FLOW_CTRL_DBG_SET_FRAME_DONE(reg32, val)                                    \
                REG_FLD_SET(OVL_FLOW_CTRL_DBG_FLD_FRAME_DONE, (reg32), (val))
#define OVL_FLOW_CTRL_DBG_SET_FRAME_UNDERRUN(reg32, val)                                \
                REG_FLD_SET(OVL_FLOW_CTRL_DBG_FLD_FRAME_UNDERRUN, (reg32), (val))
#define OVL_FLOW_CTRL_DBG_SET_FRAME_SWRST_DONE(reg32, val)                              \
                REG_FLD_SET(OVL_FLOW_CTRL_DBG_FLD_FRAME_SWRST_DONE, (reg32), (val))
#define OVL_FLOW_CTRL_DBG_SET_FRAME_HWRST_DONE(reg32, val)                              \
                REG_FLD_SET(OVL_FLOW_CTRL_DBG_FLD_FRAME_HWRST_DONE, (reg32), (val))
#define OVL_FLOW_CTRL_DBG_SET_TRIG(reg32, val)                                          \
                REG_FLD_SET(OVL_FLOW_CTRL_DBG_FLD_TRIG, (reg32), (val))
#define OVL_FLOW_CTRL_DBG_SET_RST(reg32, val)                                           \
                REG_FLD_SET(OVL_FLOW_CTRL_DBG_FLD_RST, (reg32), (val))
#define OVL_FLOW_CTRL_DBG_SET_RDMA0_IDLE(reg32, val)                                    \
                REG_FLD_SET(OVL_FLOW_CTRL_DBG_FLD_RDMA0_IDLE, (reg32), (val))
#define OVL_FLOW_CTRL_DBG_SET_RDMA1_IDLE(reg32, val)                                    \
                REG_FLD_SET(OVL_FLOW_CTRL_DBG_FLD_RDMA1_IDLE, (reg32), (val))
#define OVL_FLOW_CTRL_DBG_SET_RDMA2_IDLE(reg32, val)                                    \
                REG_FLD_SET(OVL_FLOW_CTRL_DBG_FLD_RDMA2_IDLE, (reg32), (val))
#define OVL_FLOW_CTRL_DBG_SET_RDMA3_IDLE(reg32, val)                                    \
                REG_FLD_SET(OVL_FLOW_CTRL_DBG_FLD_RDMA3_IDLE, (reg32), (val))
#define OVL_FLOW_CTRL_DBG_SET_OUT_IDLE(reg32, val)                                      \
                REG_FLD_SET(OVL_FLOW_CTRL_DBG_FLD_OUT_IDLE, (reg32), (val))
#define OVL_FLOW_CTRL_DBG_SET_OVL_OUT_READY(reg32, val)                                 \
                REG_FLD_SET(OVL_FLOW_CTRL_DBG_FLD_OVL_OUT_READY, (reg32), (val))
#define OVL_FLOW_CTRL_DBG_SET_OVL_OUT_VALID(reg32, val)                                 \
                REG_FLD_SET(OVL_FLOW_CTRL_DBG_FLD_OVL_OUT_VALID, (reg32), (val))
#define OVL_FLOW_CTRL_DBG_SET_BLEND_IDLE(reg32, val)                                    \
                REG_FLD_SET(OVL_FLOW_CTRL_DBG_FLD_BLEND_IDLE, (reg32), (val))
#define OVL_FLOW_CTRL_DBG_SET_ADDCON_IDLE(reg32, val)                                   \
                REG_FLD_SET(OVL_FLOW_CTRL_DBG_FLD_ADDCON_IDLE, (reg32), (val))
#define OVL_FLOW_CTRL_DBG_SET_FSM_STATE(reg32, val)                                     \
                REG_FLD_SET(OVL_FLOW_CTRL_DBG_FLD_FSM_STATE, (reg32), (val))

#define OVL_ADDCON_DBG_SET_L3_WIN_HIT(reg32, val)                                       \
                REG_FLD_SET(OVL_ADDCON_DBG_FLD_L3_WIN_HIT, (reg32), (val))
#define OVL_ADDCON_DBG_SET_L2_WIN_HIT(reg32, val)                                       \
                REG_FLD_SET(OVL_ADDCON_DBG_FLD_L2_WIN_HIT, (reg32), (val))
#define OVL_ADDCON_DBG_SET_ROI_Y(reg32, val)                                            \
                REG_FLD_SET(OVL_ADDCON_DBG_FLD_ROI_Y, (reg32), (val))
#define OVL_ADDCON_DBG_SET_L1_WIN_HIT(reg32, val)                                       \
                REG_FLD_SET(OVL_ADDCON_DBG_FLD_L1_WIN_HIT, (reg32), (val))
#define OVL_ADDCON_DBG_SET_L0_WIN_HIT(reg32, val)                                       \
                REG_FLD_SET(OVL_ADDCON_DBG_FLD_L0_WIN_HIT, (reg32), (val))
#define OVL_ADDCON_DBG_SET_ROI_X(reg32, val)                                            \
                REG_FLD_SET(OVL_ADDCON_DBG_FLD_ROI_X, (reg32), (val))

#define OVL_RDMA0_DBG_SET_SMI_GREQ(reg32, val)                                          \
                REG_FLD_SET(OVL_RDMA0_DBG_FLD_SMI_GREQ, (reg32), (val))
#define OVL_RDMA0_DBG_SET_RDMA0_SMI_BUSY(reg32, val)                                    \
                REG_FLD_SET(OVL_RDMA0_DBG_FLD_RDMA0_SMI_BUSY, (reg32), (val))
#define OVL_RDMA0_DBG_SET_RDMA0_OUT_VALID(reg32, val)                                   \
                REG_FLD_SET(OVL_RDMA0_DBG_FLD_RDMA0_OUT_VALID, (reg32), (val))
#define OVL_RDMA0_DBG_SET_RDMA0_OUT_READY(reg32, val)                                   \
                REG_FLD_SET(OVL_RDMA0_DBG_FLD_RDMA0_OUT_READY, (reg32), (val))
#define OVL_RDMA0_DBG_SET_RDMA0_OUT_DATA(reg32, val)                                    \
                REG_FLD_SET(OVL_RDMA0_DBG_FLD_RDMA0_OUT_DATA, (reg32), (val))
#define OVL_RDMA0_DBG_SET_RDMA0_LAYER_GREQ(reg32, val)                                  \
                REG_FLD_SET(OVL_RDMA0_DBG_FLD_RDMA0_LAYER_GREQ, (reg32), (val))
#define OVL_RDMA0_DBG_SET_RDMA0_WRAM_RST_CS(reg32, val)                                 \
                REG_FLD_SET(OVL_RDMA0_DBG_FLD_RDMA0_WRAM_RST_CS, (reg32), (val))

#define OVL_RDMA1_DBG_SET_SMI_GREQ(reg32, val)                                          \
                REG_FLD_SET(OVL_RDMA1_DBG_FLD_SMI_GREQ, (reg32), (val))
#define OVL_RDMA1_DBG_SET_RDMA1_SMI_BUSY(reg32, val)                                    \
                REG_FLD_SET(OVL_RDMA1_DBG_FLD_RDMA1_SMI_BUSY, (reg32), (val))
#define OVL_RDMA1_DBG_SET_RDMA1_OUT_VALID(reg32, val)                                   \
                REG_FLD_SET(OVL_RDMA1_DBG_FLD_RDMA1_OUT_VALID, (reg32), (val))
#define OVL_RDMA1_DBG_SET_RDMA1_OUT_READY(reg32, val)                                   \
                REG_FLD_SET(OVL_RDMA1_DBG_FLD_RDMA1_OUT_READY, (reg32), (val))
#define OVL_RDMA1_DBG_SET_RDMA1_OUT_DATA(reg32, val)                                    \
                REG_FLD_SET(OVL_RDMA1_DBG_FLD_RDMA1_OUT_DATA, (reg32), (val))
#define OVL_RDMA1_DBG_SET_RDMA1_LAYER_GREQ(reg32, val)                                  \
                REG_FLD_SET(OVL_RDMA1_DBG_FLD_RDMA1_LAYER_GREQ, (reg32), (val))
#define OVL_RDMA1_DBG_SET_RDMA1_WRAM_RST_CS(reg32, val)                                 \
                REG_FLD_SET(OVL_RDMA1_DBG_FLD_RDMA1_WRAM_RST_CS, (reg32), (val))

#define OVL_RDMA2_DBG_SET_SMI_GREQ(reg32, val)                                          \
                REG_FLD_SET(OVL_RDMA2_DBG_FLD_SMI_GREQ, (reg32), (val))
#define OVL_RDMA2_DBG_SET_RDMA2_SMI_BUSY(reg32, val)                                    \
                REG_FLD_SET(OVL_RDMA2_DBG_FLD_RDMA2_SMI_BUSY, (reg32), (val))
#define OVL_RDMA2_DBG_SET_RDMA2_OUT_VALID(reg32, val)                                   \
                REG_FLD_SET(OVL_RDMA2_DBG_FLD_RDMA2_OUT_VALID, (reg32), (val))
#define OVL_RDMA2_DBG_SET_RDMA2_OUT_READY(reg32, val)                                   \
                REG_FLD_SET(OVL_RDMA2_DBG_FLD_RDMA2_OUT_READY, (reg32), (val))
#define OVL_RDMA2_DBG_SET_RDMA2_OUT_DATA(reg32, val)                                    \
                REG_FLD_SET(OVL_RDMA2_DBG_FLD_RDMA2_OUT_DATA, (reg32), (val))
#define OVL_RDMA2_DBG_SET_RDMA2_LAYER_GREQ(reg32, val)                                  \
                REG_FLD_SET(OVL_RDMA2_DBG_FLD_RDMA2_LAYER_GREQ, (reg32), (val))
#define OVL_RDMA2_DBG_SET_RDMA2_WRAM_RST_CS(reg32, val)                                 \
                REG_FLD_SET(OVL_RDMA2_DBG_FLD_RDMA2_WRAM_RST_CS, (reg32), (val))

#define OVL_RDMA3_DBG_SET_SMI_GREQ(reg32, val)                                          \
                REG_FLD_SET(OVL_RDMA3_DBG_FLD_SMI_GREQ, (reg32), (val))
#define OVL_RDMA3_DBG_SET_RDMA3_SMI_BUSY(reg32, val)                                    \
                REG_FLD_SET(OVL_RDMA3_DBG_FLD_RDMA3_SMI_BUSY, (reg32), (val))
#define OVL_RDMA3_DBG_SET_RDMA3_OUT_VALID(reg32, val)                                   \
                REG_FLD_SET(OVL_RDMA3_DBG_FLD_RDMA3_OUT_VALID, (reg32), (val))
#define OVL_RDMA3_DBG_SET_RDMA3_OUT_READY(reg32, val)                                   \
                REG_FLD_SET(OVL_RDMA3_DBG_FLD_RDMA3_OUT_READY, (reg32), (val))
#define OVL_RDMA3_DBG_SET_RDMA3_OUT_DATA(reg32, val)                                    \
                REG_FLD_SET(OVL_RDMA3_DBG_FLD_RDMA3_OUT_DATA, (reg32), (val))
#define OVL_RDMA3_DBG_SET_RDMA3_LAYER_GREQ(reg32, val)                                  \
                REG_FLD_SET(OVL_RDMA3_DBG_FLD_RDMA3_LAYER_GREQ, (reg32), (val))
#define OVL_RDMA3_DBG_SET_RDMA3_WRAM_RST_CS(reg32, val)                                 \
                REG_FLD_SET(OVL_RDMA3_DBG_FLD_RDMA3_WRAM_RST_CS, (reg32), (val))

#define OVL_L0_CLR_SET_ALPHA(reg32, val)        REG_FLD_SET(OVL_L0_CLR_FLD_ALPHA, (reg32), (val))
#define OVL_L0_CLR_SET_RED(reg32, val)          REG_FLD_SET(OVL_L0_CLR_FLD_RED, (reg32), (val))
#define OVL_L0_CLR_SET_GREEN(reg32, val)        REG_FLD_SET(OVL_L0_CLR_FLD_GREEN, (reg32), (val))
#define OVL_L0_CLR_SET_BLUE(reg32, val)         REG_FLD_SET(OVL_L0_CLR_FLD_BLUE, (reg32), (val))

#define OVL_L1_CLR_SET_ALPHA(reg32, val)        REG_FLD_SET(OVL_L1_CLR_FLD_ALPHA, (reg32), (val))
#define OVL_L1_CLR_SET_RED(reg32, val)          REG_FLD_SET(OVL_L1_CLR_FLD_RED, (reg32), (val))
#define OVL_L1_CLR_SET_GREEN(reg32, val)        REG_FLD_SET(OVL_L1_CLR_FLD_GREEN, (reg32), (val))
#define OVL_L1_CLR_SET_BLUE(reg32, val)         REG_FLD_SET(OVL_L1_CLR_FLD_BLUE, (reg32), (val))

#define OVL_L2_CLR_SET_ALPHA(reg32, val)        REG_FLD_SET(OVL_L2_CLR_FLD_ALPHA, (reg32), (val))
#define OVL_L2_CLR_SET_RED(reg32, val)          REG_FLD_SET(OVL_L2_CLR_FLD_RED, (reg32), (val))
#define OVL_L2_CLR_SET_GREEN(reg32, val)        REG_FLD_SET(OVL_L2_CLR_FLD_GREEN, (reg32), (val))
#define OVL_L2_CLR_SET_BLUE(reg32, val)         REG_FLD_SET(OVL_L2_CLR_FLD_BLUE, (reg32), (val))

#define OVL_L3_CLR_SET_ALPHA(reg32, val)        REG_FLD_SET(OVL_L3_CLR_FLD_ALPHA, (reg32), (val))
#define OVL_L3_CLR_SET_RED(reg32, val)          REG_FLD_SET(OVL_L3_CLR_FLD_RED, (reg32), (val))
#define OVL_L3_CLR_SET_GREEN(reg32, val)        REG_FLD_SET(OVL_L3_CLR_FLD_GREEN, (reg32), (val))
#define OVL_L3_CLR_SET_BLUE(reg32, val)         REG_FLD_SET(OVL_L3_CLR_FLD_BLUE, (reg32), (val))

#define OVL_LC_CLR_SET_ALPHA(reg32, val)        REG_FLD_SET(OVL_LC_CLR_FLD_ALPHA, (reg32), (val))
#define OVL_LC_CLR_SET_RED(reg32, val)          REG_FLD_SET(OVL_LC_CLR_FLD_RED, (reg32), (val))
#define OVL_LC_CLR_SET_GREEN(reg32, val)        REG_FLD_SET(OVL_LC_CLR_FLD_GREEN, (reg32), (val))
#define OVL_LC_CLR_SET_BLUE(reg32, val)         REG_FLD_SET(OVL_LC_CLR_FLD_BLUE, (reg32), (val))

#define OVL_CRC_SET_CRC_RDY(reg32, val)         REG_FLD_SET(OVL_CRC_FLD_CRC_RDY, (reg32), (val))
#define OVL_CRC_SET_CRC_OUT(reg32, val)         REG_FLD_SET(OVL_CRC_FLD_CRC_OUT, (reg32), (val))

#define OVL_LC_CON_SET_DSTKEY_EN(reg32, val)                                            \
                REG_FLD_SET(OVL_LC_CON_FLD_DSTKEY_EN, (reg32), (val))
#define OVL_LC_CON_SET_SRCKEY_EN(reg32, val)                                            \
                REG_FLD_SET(OVL_LC_CON_FLD_SRCKEY_EN, (reg32), (val))
#define OVL_LC_CON_SET_LAYER_SRC(reg32, val)                                            \
                REG_FLD_SET(OVL_LC_CON_FLD_LAYER_SRC, (reg32), (val))
#define OVL_LC_CON_SET_R_FIRST(reg32, val)      REG_FLD_SET(OVL_LC_CON_FLD_R_FIRST, (reg32), (val))
#define OVL_LC_CON_SET_LANDSCAPE(reg32, val)                                            \
                REG_FLD_SET(OVL_LC_CON_FLD_LANDSCAPE, (reg32), (val))
#define OVL_LC_CON_SET_EN_3D(reg32, val)        REG_FLD_SET(OVL_LC_CON_FLD_EN_3D, (reg32), (val))
#define OVL_LC_CON_SET_ALPHA_EN(reg32, val)     REG_FLD_SET(OVL_LC_CON_FLD_ALPHA_EN, (reg32), (val))
#define OVL_LC_CON_SET_ALPHA(reg32, val)        REG_FLD_SET(OVL_LC_CON_FLD_ALPHA, (reg32), (val))

#define OVL_LC_SRCKEY_SET_SRCKEY(reg32, val)                                            \
                REG_FLD_SET(OVL_LC_SRCKEY_FLD_SRCKEY, (reg32), (val))

#define OVL_LC_SRC_SIZE_SET_LC_SRC_H(reg32, val)                                        \
                REG_FLD_SET(OVL_LC_SRC_SIZE_FLD_LC_SRC_H, (reg32), (val))
#define OVL_LC_SRC_SIZE_SET_LC_SRC_W(reg32, val)                                        \
                REG_FLD_SET(OVL_LC_SRC_SIZE_FLD_LC_SRC_W, (reg32), (val))

#define OVL_LC_OFFSET_SET_LC_YOFF(reg32, val)                                           \
                REG_FLD_SET(OVL_LC_OFFSET_FLD_LC_YOFF, (reg32), (val))
#define OVL_LC_OFFSET_SET_LC_XOFF(reg32, val)                                           \
                REG_FLD_SET(OVL_LC_OFFSET_FLD_LC_XOFF, (reg32), (val))

#define OVL_LC_SRC_SEL_SET_SURFL_EN(reg32, val)                                         \
                REG_FLD_SET(OVL_LC_SRC_SEL_FLD_SURFL_EN, (reg32), (val))
#define OVL_LC_SRC_SEL_SET_LC_BLEND_RND_SHT(reg32, val)                                 \
                REG_FLD_SET(OVL_LC_SRC_SEL_FLD_LC_BLEND_RND_SHT, (reg32), (val))
#define OVL_LC_SRC_SEL_SET_LC_SRGB_SEL_EXT2(reg32, val)                                 \
                REG_FLD_SET(OVL_LC_SRC_SEL_FLD_LC_SRGB_SEL_EXT2, (reg32), (val))
#define OVL_LC_SRC_SEL_SET_LC_CONST_BLD(reg32, val)                                     \
                REG_FLD_SET(OVL_LC_SRC_SEL_FLD_LC_CONST_BLD, (reg32), (val))
#define OVL_LC_SRC_SEL_SET_LC_DRGB_SEL_EXT(reg32, val)                                  \
                REG_FLD_SET(OVL_LC_SRC_SEL_FLD_LC_DRGB_SEL_EXT, (reg32), (val))
#define OVL_LC_SRC_SEL_SET_LC_DA_SEL_EXT(reg32, val)                                    \
                REG_FLD_SET(OVL_LC_SRC_SEL_FLD_LC_DA_SEL_EXT, (reg32), (val))
#define OVL_LC_SRC_SEL_SET_LC_SRGB_SEL_EXT(reg32, val)                                  \
                REG_FLD_SET(OVL_LC_SRC_SEL_FLD_LC_SRGB_SEL_EXT, (reg32), (val))
#define OVL_LC_SRC_SEL_SET_LC_SA_SEL_EXT(reg32, val)                                    \
                REG_FLD_SET(OVL_LC_SRC_SEL_FLD_LC_SA_SEL_EXT, (reg32), (val))
#define OVL_LC_SRC_SEL_SET_LC_DRGB_SEL(reg32, val)                                      \
                REG_FLD_SET(OVL_LC_SRC_SEL_FLD_LC_DRGB_SEL, (reg32), (val))
#define OVL_LC_SRC_SEL_SET_LC_DA_SEL(reg32, val)                                        \
                REG_FLD_SET(OVL_LC_SRC_SEL_FLD_LC_DA_SEL, (reg32), (val))
#define OVL_LC_SRC_SEL_SET_LC_SRGB_SEL(reg32, val)                                      \
                REG_FLD_SET(OVL_LC_SRC_SEL_FLD_LC_SRGB_SEL, (reg32), (val))
#define OVL_LC_SRC_SEL_SET_LC_SA_SEL(reg32, val)                                        \
                REG_FLD_SET(OVL_LC_SRC_SEL_FLD_LC_SA_SEL, (reg32), (val))
#define OVL_LC_SRC_SEL_SET_CONST_LAYER_SEL(reg32, val)                                  \
                REG_FLD_SET(OVL_LC_SRC_SEL_FLD_CONST_LAYER_SEL, (reg32), (val))

#define OVL_BANK_CON_SET_OVL_BANK_CON(reg32, val)                                       \
                REG_FLD_SET(OVL_BANK_CON_FLD_OVL_BANK_CON, (reg32), (val))

#define OVL_FUNC_DCM0_SET_OVL_FUNC_DCM0(reg32, val)                                     \
                REG_FLD_SET(OVL_FUNC_DCM0_FLD_OVL_FUNC_DCM0, (reg32), (val))

#define OVL_FUNC_DCM1_SET_OVL_FUNC_DCM1(reg32, val)                                     \
                REG_FLD_SET(OVL_FUNC_DCM1_FLD_OVL_FUNC_DCM1, (reg32), (val))

#define OVL_DVFS_L0_ROI_SET_L0_DVFS_ROI_BB(reg32, val)                                  \
                REG_FLD_SET(OVL_DVFS_L0_ROI_FLD_L0_DVFS_ROI_BB, (reg32), (val))
#define OVL_DVFS_L0_ROI_SET_L0_DVFS_ROI_TB(reg32, val)                                  \
                REG_FLD_SET(OVL_DVFS_L0_ROI_FLD_L0_DVFS_ROI_TB, (reg32), (val))

#define OVL_DVFS_L1_ROI_SET_L1_DVFS_ROI_BB(reg32, val)                                  \
                REG_FLD_SET(OVL_DVFS_L1_ROI_FLD_L1_DVFS_ROI_BB, (reg32), (val))
#define OVL_DVFS_L1_ROI_SET_L1_DVFS_ROI_TB(reg32, val)                                  \
                REG_FLD_SET(OVL_DVFS_L1_ROI_FLD_L1_DVFS_ROI_TB, (reg32), (val))

#define OVL_DVFS_L2_ROI_SET_L2_DVFS_ROI_BB(reg32, val)                                  \
                REG_FLD_SET(OVL_DVFS_L2_ROI_FLD_L2_DVFS_ROI_BB, (reg32), (val))
#define OVL_DVFS_L2_ROI_SET_L2_DVFS_ROI_TB(reg32, val)                                  \
                REG_FLD_SET(OVL_DVFS_L2_ROI_FLD_L2_DVFS_ROI_TB, (reg32), (val))

#define OVL_DVFS_L3_ROI_SET_L3_DVFS_ROI_BB(reg32, val)                                  \
                REG_FLD_SET(OVL_DVFS_L3_ROI_FLD_L3_DVFS_ROI_BB, (reg32), (val))
#define OVL_DVFS_L3_ROI_SET_L3_DVFS_ROI_TB(reg32, val)                                  \
                REG_FLD_SET(OVL_DVFS_L3_ROI_FLD_L3_DVFS_ROI_TB, (reg32), (val))

#define OVL_DVFS_EL0_ROI_SET_EL0_DVFS_ROI_BB(reg32, val)                                \
                REG_FLD_SET(OVL_DVFS_EL0_ROI_FLD_EL0_DVFS_ROI_BB, (reg32), (val))
#define OVL_DVFS_EL0_ROI_SET_EL0_DVFS_ROI_TB(reg32, val)                                \
                REG_FLD_SET(OVL_DVFS_EL0_ROI_FLD_EL0_DVFS_ROI_TB, (reg32), (val))

#define OVL_DVFS_EL1_ROI_SET_EL1_DVFS_ROI_BB(reg32, val)                                \
                REG_FLD_SET(OVL_DVFS_EL1_ROI_FLD_EL1_DVFS_ROI_BB, (reg32), (val))
#define OVL_DVFS_EL1_ROI_SET_EL1_DVFS_ROI_TB(reg32, val)                                \
                REG_FLD_SET(OVL_DVFS_EL1_ROI_FLD_EL1_DVFS_ROI_TB, (reg32), (val))

#define OVL_DVFS_EL2_ROI_SET_EL2_DVFS_ROI_BB(reg32, val)                                \
                REG_FLD_SET(OVL_DVFS_EL2_ROI_FLD_EL2_DVFS_ROI_BB, (reg32), (val))
#define OVL_DVFS_EL2_ROI_SET_EL2_DVFS_ROI_TB(reg32, val)                                \
                REG_FLD_SET(OVL_DVFS_EL2_ROI_FLD_EL2_DVFS_ROI_TB, (reg32), (val))

#define OVL_DATAPATH_EXT_CON_SET_EL2_LAYER_SEL(reg32, val)                              \
                REG_FLD_SET(OVL_DATAPATH_EXT_CON_FLD_EL2_LAYER_SEL, (reg32), (val))
#define OVL_DATAPATH_EXT_CON_SET_EL1_LAYER_SEL(reg32, val)                              \
                REG_FLD_SET(OVL_DATAPATH_EXT_CON_FLD_EL1_LAYER_SEL, (reg32), (val))
#define OVL_DATAPATH_EXT_CON_SET_EL0_LAYER_SEL(reg32, val)                              \
                REG_FLD_SET(OVL_DATAPATH_EXT_CON_FLD_EL0_LAYER_SEL, (reg32), (val))
#define OVL_DATAPATH_EXT_CON_SET_EL2_GPU_MODE(reg32, val)                               \
                REG_FLD_SET(OVL_DATAPATH_EXT_CON_FLD_EL2_GPU_MODE, (reg32), (val))
#define OVL_DATAPATH_EXT_CON_SET_EL1_GPU_MODE(reg32, val)                               \
                REG_FLD_SET(OVL_DATAPATH_EXT_CON_FLD_EL1_GPU_MODE, (reg32), (val))
#define OVL_DATAPATH_EXT_CON_SET_EL0_GPU_MODE(reg32, val)                               \
                REG_FLD_SET(OVL_DATAPATH_EXT_CON_FLD_EL0_GPU_MODE, (reg32), (val))
#define OVL_DATAPATH_EXT_CON_SET_EL2_EN(reg32, val)                                     \
                REG_FLD_SET(OVL_DATAPATH_EXT_CON_FLD_EL2_EN, (reg32), (val))
#define OVL_DATAPATH_EXT_CON_SET_EL1_EN(reg32, val)                                     \
                REG_FLD_SET(OVL_DATAPATH_EXT_CON_FLD_EL1_EN, (reg32), (val))
#define OVL_DATAPATH_EXT_CON_SET_EL0_EN(reg32, val)                                     \
                REG_FLD_SET(OVL_DATAPATH_EXT_CON_FLD_EL0_EN, (reg32), (val))

#define OVL_EL0_CON_SET_DSTKEY_EN(reg32, val)                                           \
                REG_FLD_SET(OVL_EL0_CON_FLD_DSTKEY_EN, (reg32), (val))
#define OVL_EL0_CON_SET_SRCKEY_EN(reg32, val)                                           \
                REG_FLD_SET(OVL_EL0_CON_FLD_SRCKEY_EN, (reg32), (val))
#define OVL_EL0_CON_SET_LAYER_SRC(reg32, val)                                           \
                REG_FLD_SET(OVL_EL0_CON_FLD_LAYER_SRC, (reg32), (val))
#define OVL_EL0_CON_SET_MTX_EN(reg32, val)                                              \
                REG_FLD_SET(OVL_EL0_CON_FLD_MTX_EN, (reg32), (val))
#define OVL_EL0_CON_SET_MTX_AUTO_DIS(reg32, val)                                        \
                REG_FLD_SET(OVL_EL0_CON_FLD_MTX_AUTO_DIS, (reg32), (val))
#define OVL_EL0_CON_SET_RGB_SWAP(reg32, val)                                            \
                REG_FLD_SET(OVL_EL0_CON_FLD_RGB_SWAP, (reg32), (val))
#define OVL_EL0_CON_SET_BYTE_SWAP(reg32, val)                                           \
                REG_FLD_SET(OVL_EL0_CON_FLD_BYTE_SWAP, (reg32), (val))
#define OVL_EL0_CON_SET_CLRFMT_MAN(reg32, val)                                          \
                REG_FLD_SET(OVL_EL0_CON_FLD_CLRFMT_MAN, (reg32), (val))
#define OVL_EL0_CON_SET_R_FIRST(reg32, val)                                             \
                REG_FLD_SET(OVL_EL0_CON_FLD_R_FIRST, (reg32), (val))
#define OVL_EL0_CON_SET_LANDSCAPE(reg32, val)                                           \
                REG_FLD_SET(OVL_EL0_CON_FLD_LANDSCAPE, (reg32), (val))
#define OVL_EL0_CON_SET_EN_3D(reg32, val)                                               \
                REG_FLD_SET(OVL_EL0_CON_FLD_EN_3D, (reg32), (val))
#define OVL_EL0_CON_SET_INT_MTX_SEL(reg32, val)                                         \
                REG_FLD_SET(OVL_EL0_CON_FLD_INT_MTX_SEL, (reg32), (val))
#define OVL_EL0_CON_SET_CLRFMT(reg32, val)                                              \
                REG_FLD_SET(OVL_EL0_CON_FLD_CLRFMT, (reg32), (val))
#define OVL_EL0_CON_SET_EXT_MTX_EN(reg32, val)                                          \
                REG_FLD_SET(OVL_EL0_CON_FLD_EXT_MTX_EN, (reg32), (val))
#define OVL_EL0_CON_SET_HORIZONTAL_FLIP_EN(reg32, val)                                  \
                REG_FLD_SET(OVL_EL0_CON_FLD_HORIZONTAL_FLIP_EN, (reg32), (val))
#define OVL_EL0_CON_SET_VERTICAL_FLIP_EN(reg32, val)                                    \
                REG_FLD_SET(OVL_EL0_CON_FLD_VERTICAL_FLIP_EN, (reg32), (val))
#define OVL_EL0_CON_SET_ALPHA_EN(reg32, val)                                            \
                REG_FLD_SET(OVL_EL0_CON_FLD_ALPHA_EN, (reg32), (val))
#define OVL_EL0_CON_SET_ALPHA(reg32, val)                                               \
                REG_FLD_SET(OVL_EL0_CON_FLD_ALPHA, (reg32), (val))

#define OVL_EL0_SRCKEY_SET_SRCKEY(reg32, val)                                           \
                REG_FLD_SET(OVL_EL0_SRCKEY_FLD_SRCKEY, (reg32), (val))

#define OVL_EL0_SRC_SIZE_SET_EL0_SRC_H(reg32, val)                                      \
                REG_FLD_SET(OVL_EL0_SRC_SIZE_FLD_EL0_SRC_H, (reg32), (val))
#define OVL_EL0_SRC_SIZE_SET_EL0_SRC_W(reg32, val)                                      \
                REG_FLD_SET(OVL_EL0_SRC_SIZE_FLD_EL0_SRC_W, (reg32), (val))

#define OVL_EL0_OFFSET_SET_EL0_YOFF(reg32, val)                                         \
                REG_FLD_SET(OVL_EL0_OFFSET_FLD_EL0_YOFF, (reg32), (val))
#define OVL_EL0_OFFSET_SET_EL0_XOFF(reg32, val)                                         \
                REG_FLD_SET(OVL_EL0_OFFSET_FLD_EL0_XOFF, (reg32), (val))

#define OVL_EL0_PITCH_SET_SURFL_EN(reg32, val)                                          \
                REG_FLD_SET(OVL_EL0_PITCH_FLD_SURFL_EN, (reg32), (val))
#define OVL_EL0_PITCH_SET_EL0_BLEND_RND_SHT(reg32, val)                                 \
                REG_FLD_SET(OVL_EL0_PITCH_FLD_EL0_BLEND_RND_SHT, (reg32), (val))
#define OVL_EL0_PITCH_SET_EL0_SRGB_SEL_EXT2(reg32, val)                                 \
                REG_FLD_SET(OVL_EL0_PITCH_FLD_EL0_SRGB_SEL_EXT2, (reg32), (val))
#define OVL_EL0_PITCH_SET_EL0_CONST_BLD(reg32, val)                                     \
                REG_FLD_SET(OVL_EL0_PITCH_FLD_EL0_CONST_BLD, (reg32), (val))
#define OVL_EL0_PITCH_SET_EL0_DRGB_SEL_EXT(reg32, val)                                  \
                REG_FLD_SET(OVL_EL0_PITCH_FLD_EL0_DRGB_SEL_EXT, (reg32), (val))
#define OVL_EL0_PITCH_SET_EL0_DA_SEL_EXT(reg32, val)                                    \
                REG_FLD_SET(OVL_EL0_PITCH_FLD_EL0_DA_SEL_EXT, (reg32), (val))
#define OVL_EL0_PITCH_SET_EL0_SRGB_SEL_EXT(reg32, val)                                  \
                REG_FLD_SET(OVL_EL0_PITCH_FLD_EL0_SRGB_SEL_EXT, (reg32), (val))
#define OVL_EL0_PITCH_SET_EL0_SA_SEL_EXT(reg32, val)                                    \
                REG_FLD_SET(OVL_EL0_PITCH_FLD_EL0_SA_SEL_EXT, (reg32), (val))
#define OVL_EL0_PITCH_SET_EL0_DRGB_SEL(reg32, val)                                      \
                REG_FLD_SET(OVL_EL0_PITCH_FLD_EL0_DRGB_SEL, (reg32), (val))
#define OVL_EL0_PITCH_SET_EL0_DA_SEL(reg32, val)                                        \
                REG_FLD_SET(OVL_EL0_PITCH_FLD_EL0_DA_SEL, (reg32), (val))
#define OVL_EL0_PITCH_SET_EL0_SRGB_SEL(reg32, val)                                      \
                REG_FLD_SET(OVL_EL0_PITCH_FLD_EL0_SRGB_SEL, (reg32), (val))
#define OVL_EL0_PITCH_SET_EL0_SA_SEL(reg32, val)                                        \
                REG_FLD_SET(OVL_EL0_PITCH_FLD_EL0_SA_SEL, (reg32), (val))
#define OVL_EL0_PITCH_SET_EL0_SRC_PITCH(reg32, val)                                     \
                REG_FLD_SET(OVL_EL0_PITCH_FLD_EL0_SRC_PITCH, (reg32), (val))

#define OVL_EL0_TILE_SET_TILE_HORI_BLOCK_NUM(reg32, val)                                \
                REG_FLD_SET(OVL_EL0_TILE_FLD_TILE_HORI_BLOCK_NUM, (reg32), (val))
#define OVL_EL0_TILE_SET_TILE_EN(reg32, val)                                            \
                REG_FLD_SET(OVL_EL0_TILE_FLD_TILE_EN, (reg32), (val))
#define OVL_EL0_TILE_SET_TILE_WIDTH_SEL(reg32, val)                                     \
                REG_FLD_SET(OVL_EL0_TILE_FLD_TILE_WIDTH_SEL, (reg32), (val))
#define OVL_EL0_TILE_SET_TILE_HEIGHT(reg32, val)                                        \
                REG_FLD_SET(OVL_EL0_TILE_FLD_TILE_HEIGHT, (reg32), (val))

#define OVL_EL0_CLIP_SET_EL0_SRC_BOTTOM_CLIP(reg32, val)                                \
                REG_FLD_SET(OVL_EL0_CLIP_FLD_EL0_SRC_BOTTOM_CLIP, (reg32), (val))
#define OVL_EL0_CLIP_SET_EL0_SRC_TOP_CLIP(reg32, val)                                   \
                REG_FLD_SET(OVL_EL0_CLIP_FLD_EL0_SRC_TOP_CLIP, (reg32), (val))
#define OVL_EL0_CLIP_SET_EL0_SRC_RIGHT_CLIP(reg32, val)                                 \
                REG_FLD_SET(OVL_EL0_CLIP_FLD_EL0_SRC_RIGHT_CLIP, (reg32), (val))
#define OVL_EL0_CLIP_SET_EL0_SRC_LEFT_CLIP(reg32, val)                                  \
                REG_FLD_SET(OVL_EL0_CLIP_FLD_EL0_SRC_LEFT_CLIP, (reg32), (val))

#define OVL_EL1_CON_SET_DSTKEY_EN(reg32, val)                                           \
                REG_FLD_SET(OVL_EL1_CON_FLD_DSTKEY_EN, (reg32), (val))
#define OVL_EL1_CON_SET_SRCKEY_EN(reg32, val)                                           \
                REG_FLD_SET(OVL_EL1_CON_FLD_SRCKEY_EN, (reg32), (val))
#define OVL_EL1_CON_SET_LAYER_SRC(reg32, val)                                           \
                REG_FLD_SET(OVL_EL1_CON_FLD_LAYER_SRC, (reg32), (val))
#define OVL_EL1_CON_SET_MTX_EN(reg32, val)                                              \
                REG_FLD_SET(OVL_EL1_CON_FLD_MTX_EN, (reg32), (val))
#define OVL_EL1_CON_SET_MTX_AUTO_DIS(reg32, val)                                        \
                REG_FLD_SET(OVL_EL1_CON_FLD_MTX_AUTO_DIS, (reg32), (val))
#define OVL_EL1_CON_SET_RGB_SWAP(reg32, val)                                            \
                REG_FLD_SET(OVL_EL1_CON_FLD_RGB_SWAP, (reg32), (val))
#define OVL_EL1_CON_SET_BYTE_SWAP(reg32, val)                                           \
                REG_FLD_SET(OVL_EL1_CON_FLD_BYTE_SWAP, (reg32), (val))
#define OVL_EL1_CON_SET_CLRFMT_MAN(reg32, val)                                          \
                REG_FLD_SET(OVL_EL1_CON_FLD_CLRFMT_MAN, (reg32), (val))
#define OVL_EL1_CON_SET_R_FIRST(reg32, val)                                             \
                REG_FLD_SET(OVL_EL1_CON_FLD_R_FIRST, (reg32), (val))
#define OVL_EL1_CON_SET_LANDSCAPE(reg32, val)                                           \
                REG_FLD_SET(OVL_EL1_CON_FLD_LANDSCAPE, (reg32), (val))
#define OVL_EL1_CON_SET_EN_3D(reg32, val)                                               \
                REG_FLD_SET(OVL_EL1_CON_FLD_EN_3D, (reg32), (val))
#define OVL_EL1_CON_SET_INT_MTX_SEL(reg32, val)                                         \
                REG_FLD_SET(OVL_EL1_CON_FLD_INT_MTX_SEL, (reg32), (val))
#define OVL_EL1_CON_SET_CLRFMT(reg32, val)                                              \
                REG_FLD_SET(OVL_EL1_CON_FLD_CLRFMT, (reg32), (val))
#define OVL_EL1_CON_SET_EXT_MTX_EN(reg32, val)                                          \
                REG_FLD_SET(OVL_EL1_CON_FLD_EXT_MTX_EN, (reg32), (val))
#define OVL_EL1_CON_SET_HORIZONTAL_FLIP_EN(reg32, val)                                  \
                REG_FLD_SET(OVL_EL1_CON_FLD_HORIZONTAL_FLIP_EN, (reg32), (val))
#define OVL_EL1_CON_SET_VERTICAL_FLIP_EN(reg32, val)                                    \
                REG_FLD_SET(OVL_EL1_CON_FLD_VERTICAL_FLIP_EN, (reg32), (val))
#define OVL_EL1_CON_SET_ALPHA_EN(reg32, val)                                            \
                REG_FLD_SET(OVL_EL1_CON_FLD_ALPHA_EN, (reg32), (val))
#define OVL_EL1_CON_SET_ALPHA(reg32, val)                                               \
                REG_FLD_SET(OVL_EL1_CON_FLD_ALPHA, (reg32), (val))

#define OVL_EL1_SRCKEY_SET_SRCKEY(reg32, val)                                           \
                REG_FLD_SET(OVL_EL1_SRCKEY_FLD_SRCKEY, (reg32), (val))

#define OVL_EL1_SRC_SIZE_SET_EL1_SRC_H(reg32, val)                                      \
                REG_FLD_SET(OVL_EL1_SRC_SIZE_FLD_EL1_SRC_H, (reg32), (val))
#define OVL_EL1_SRC_SIZE_SET_EL1_SRC_W(reg32, val)                                      \
                REG_FLD_SET(OVL_EL1_SRC_SIZE_FLD_EL1_SRC_W, (reg32), (val))

#define OVL_EL1_OFFSET_SET_EL1_YOFF(reg32, val)                                         \
                REG_FLD_SET(OVL_EL1_OFFSET_FLD_EL1_YOFF, (reg32), (val))
#define OVL_EL1_OFFSET_SET_EL1_XOFF(reg32, val)                                         \
                REG_FLD_SET(OVL_EL1_OFFSET_FLD_EL1_XOFF, (reg32), (val))

#define OVL_EL1_PITCH_SET_SURFL_EN(reg32, val)                                          \
                REG_FLD_SET(OVL_EL1_PITCH_FLD_SURFL_EN, (reg32), (val))
#define OVL_EL1_PITCH_SET_EL1_BLEND_RND_SHT(reg32, val)                                 \
                REG_FLD_SET(OVL_EL1_PITCH_FLD_EL1_BLEND_RND_SHT, (reg32), (val))
#define OVL_EL1_PITCH_SET_EL1_SRGB_SEL_EXT2(reg32, val)                                 \
                REG_FLD_SET(OVL_EL1_PITCH_FLD_EL1_SRGB_SEL_EXT2, (reg32), (val))
#define OVL_EL1_PITCH_SET_EL1_CONST_BLD(reg32, val)                                     \
                REG_FLD_SET(OVL_EL1_PITCH_FLD_EL1_CONST_BLD, (reg32), (val))
#define OVL_EL1_PITCH_SET_EL1_DRGB_SEL_EXT(reg32, val)                                  \
                REG_FLD_SET(OVL_EL1_PITCH_FLD_EL1_DRGB_SEL_EXT, (reg32), (val))
#define OVL_EL1_PITCH_SET_EL1_DA_SEL_EXT(reg32, val)                                    \
                REG_FLD_SET(OVL_EL1_PITCH_FLD_EL1_DA_SEL_EXT, (reg32), (val))
#define OVL_EL1_PITCH_SET_EL1_SRGB_SEL_EXT(reg32, val)                                  \
                REG_FLD_SET(OVL_EL1_PITCH_FLD_EL1_SRGB_SEL_EXT, (reg32), (val))
#define OVL_EL1_PITCH_SET_EL1_SA_SEL_EXT(reg32, val)                                    \
                REG_FLD_SET(OVL_EL1_PITCH_FLD_EL1_SA_SEL_EXT, (reg32), (val))
#define OVL_EL1_PITCH_SET_EL1_DRGB_SEL(reg32, val)                                      \
                REG_FLD_SET(OVL_EL1_PITCH_FLD_EL1_DRGB_SEL, (reg32), (val))
#define OVL_EL1_PITCH_SET_EL1_DA_SEL(reg32, val)                                        \
                REG_FLD_SET(OVL_EL1_PITCH_FLD_EL1_DA_SEL, (reg32), (val))
#define OVL_EL1_PITCH_SET_EL1_SRGB_SEL(reg32, val)                                      \
                REG_FLD_SET(OVL_EL1_PITCH_FLD_EL1_SRGB_SEL, (reg32), (val))
#define OVL_EL1_PITCH_SET_EL1_SA_SEL(reg32, val)                                        \
                REG_FLD_SET(OVL_EL1_PITCH_FLD_EL1_SA_SEL, (reg32), (val))
#define OVL_EL1_PITCH_SET_EL1_SRC_PITCH(reg32, val)                                     \
                REG_FLD_SET(OVL_EL1_PITCH_FLD_EL1_SRC_PITCH, (reg32), (val))

#define OVL_EL1_TILE_SET_TILE_HORI_BLOCK_NUM(reg32, val)                                \
                REG_FLD_SET(OVL_EL1_TILE_FLD_TILE_HORI_BLOCK_NUM, (reg32), (val))
#define OVL_EL1_TILE_SET_TILE_EN(reg32, val)                                            \
                REG_FLD_SET(OVL_EL1_TILE_FLD_TILE_EN, (reg32), (val))
#define OVL_EL1_TILE_SET_TILE_WIDTH_SEL(reg32, val)                                     \
                REG_FLD_SET(OVL_EL1_TILE_FLD_TILE_WIDTH_SEL, (reg32), (val))
#define OVL_EL1_TILE_SET_TILE_HEIGHT(reg32, val)                                        \
                REG_FLD_SET(OVL_EL1_TILE_FLD_TILE_HEIGHT, (reg32), (val))

#define OVL_EL1_CLIP_SET_EL1_SRC_BOTTOM_CLIP(reg32, val)                                \
                REG_FLD_SET(OVL_EL1_CLIP_FLD_EL1_SRC_BOTTOM_CLIP, (reg32), (val))
#define OVL_EL1_CLIP_SET_EL1_SRC_TOP_CLIP(reg32, val)                                   \
                REG_FLD_SET(OVL_EL1_CLIP_FLD_EL1_SRC_TOP_CLIP, (reg32), (val))
#define OVL_EL1_CLIP_SET_EL1_SRC_RIGHT_CLIP(reg32, val)                                 \
                REG_FLD_SET(OVL_EL1_CLIP_FLD_EL1_SRC_RIGHT_CLIP, (reg32), (val))
#define OVL_EL1_CLIP_SET_EL1_SRC_LEFT_CLIP(reg32, val)                                  \
                REG_FLD_SET(OVL_EL1_CLIP_FLD_EL1_SRC_LEFT_CLIP, (reg32), (val))

#define OVL_EL2_CON_SET_DSTKEY_EN(reg32, val)                                           \
                REG_FLD_SET(OVL_EL2_CON_FLD_DSTKEY_EN, (reg32), (val))
#define OVL_EL2_CON_SET_SRCKEY_EN(reg32, val)                                           \
                REG_FLD_SET(OVL_EL2_CON_FLD_SRCKEY_EN, (reg32), (val))
#define OVL_EL2_CON_SET_LAYER_SRC(reg32, val)                                           \
                REG_FLD_SET(OVL_EL2_CON_FLD_LAYER_SRC, (reg32), (val))
#define OVL_EL2_CON_SET_MTX_EN(reg32, val)                                              \
                REG_FLD_SET(OVL_EL2_CON_FLD_MTX_EN, (reg32), (val))
#define OVL_EL2_CON_SET_MTX_AUTO_DIS(reg32, val)                                        \
                REG_FLD_SET(OVL_EL2_CON_FLD_MTX_AUTO_DIS, (reg32), (val))
#define OVL_EL2_CON_SET_RGB_SWAP(reg32, val)                                            \
                REG_FLD_SET(OVL_EL2_CON_FLD_RGB_SWAP, (reg32), (val))
#define OVL_EL2_CON_SET_BYTE_SWAP(reg32, val)                                           \
                REG_FLD_SET(OVL_EL2_CON_FLD_BYTE_SWAP, (reg32), (val))
#define OVL_EL2_CON_SET_CLRFMT_MAN(reg32, val)                                          \
                REG_FLD_SET(OVL_EL2_CON_FLD_CLRFMT_MAN, (reg32), (val))
#define OVL_EL2_CON_SET_R_FIRST(reg32, val)                                             \
                REG_FLD_SET(OVL_EL2_CON_FLD_R_FIRST, (reg32), (val))
#define OVL_EL2_CON_SET_LANDSCAPE(reg32, val)                                           \
                REG_FLD_SET(OVL_EL2_CON_FLD_LANDSCAPE, (reg32), (val))
#define OVL_EL2_CON_SET_EN_3D(reg32, val)                                               \
                REG_FLD_SET(OVL_EL2_CON_FLD_EN_3D, (reg32), (val))
#define OVL_EL2_CON_SET_INT_MTX_SEL(reg32, val)                                         \
                REG_FLD_SET(OVL_EL2_CON_FLD_INT_MTX_SEL, (reg32), (val))
#define OVL_EL2_CON_SET_CLRFMT(reg32, val)                                              \
                REG_FLD_SET(OVL_EL2_CON_FLD_CLRFMT, (reg32), (val))
#define OVL_EL2_CON_SET_EXT_MTX_EN(reg32, val)                                          \
                REG_FLD_SET(OVL_EL2_CON_FLD_EXT_MTX_EN, (reg32), (val))
#define OVL_EL2_CON_SET_HORIZONTAL_FLIP_EN(reg32, val)                                  \
                REG_FLD_SET(OVL_EL2_CON_FLD_HORIZONTAL_FLIP_EN, (reg32), (val))
#define OVL_EL2_CON_SET_VERTICAL_FLIP_EN(reg32, val)                                    \
                REG_FLD_SET(OVL_EL2_CON_FLD_VERTICAL_FLIP_EN, (reg32), (val))
#define OVL_EL2_CON_SET_ALPHA_EN(reg32, val)                                            \
                REG_FLD_SET(OVL_EL2_CON_FLD_ALPHA_EN, (reg32), (val))
#define OVL_EL2_CON_SET_ALPHA(reg32, val)                                               \
                REG_FLD_SET(OVL_EL2_CON_FLD_ALPHA, (reg32), (val))

#define OVL_EL2_SRCKEY_SET_SRCKEY(reg32, val)                                           \
                REG_FLD_SET(OVL_EL2_SRCKEY_FLD_SRCKEY, (reg32), (val))

#define OVL_EL2_SRC_SIZE_SET_EL2_SRC_H(reg32, val)                                      \
                REG_FLD_SET(OVL_EL2_SRC_SIZE_FLD_EL2_SRC_H, (reg32), (val))
#define OVL_EL2_SRC_SIZE_SET_EL2_SRC_W(reg32, val)                                      \
                REG_FLD_SET(OVL_EL2_SRC_SIZE_FLD_EL2_SRC_W, (reg32), (val))

#define OVL_EL2_OFFSET_SET_EL2_YOFF(reg32, val)                                         \
                REG_FLD_SET(OVL_EL2_OFFSET_FLD_EL2_YOFF, (reg32), (val))
#define OVL_EL2_OFFSET_SET_EL2_XOFF(reg32, val)                                         \
                REG_FLD_SET(OVL_EL2_OFFSET_FLD_EL2_XOFF, (reg32), (val))

#define OVL_EL2_PITCH_SET_SURFL_EN(reg32, val)                                          \
                REG_FLD_SET(OVL_EL2_PITCH_FLD_SURFL_EN, (reg32), (val))
#define OVL_EL2_PITCH_SET_EL2_BLEND_RND_SHT(reg32, val)                                 \
                REG_FLD_SET(OVL_EL2_PITCH_FLD_EL2_BLEND_RND_SHT, (reg32), (val))
#define OVL_EL2_PITCH_SET_EL2_SRGB_SEL_EXT2(reg32, val)                                 \
                REG_FLD_SET(OVL_EL2_PITCH_FLD_EL2_SRGB_SEL_EXT2, (reg32), (val))
#define OVL_EL2_PITCH_SET_EL2_CONST_BLD(reg32, val)                                     \
                REG_FLD_SET(OVL_EL2_PITCH_FLD_EL2_CONST_BLD, (reg32), (val))
#define OVL_EL2_PITCH_SET_EL2_DRGB_SEL_EXT(reg32, val)                                  \
                REG_FLD_SET(OVL_EL2_PITCH_FLD_EL2_DRGB_SEL_EXT, (reg32), (val))
#define OVL_EL2_PITCH_SET_EL2_DA_SEL_EXT(reg32, val)                                    \
                REG_FLD_SET(OVL_EL2_PITCH_FLD_EL2_DA_SEL_EXT, (reg32), (val))
#define OVL_EL2_PITCH_SET_EL2_SRGB_SEL_EXT(reg32, val)                                  \
                REG_FLD_SET(OVL_EL2_PITCH_FLD_EL2_SRGB_SEL_EXT, (reg32), (val))
#define OVL_EL2_PITCH_SET_EL2_SA_SEL_EXT(reg32, val)                                    \
                REG_FLD_SET(OVL_EL2_PITCH_FLD_EL2_SA_SEL_EXT, (reg32), (val))
#define OVL_EL2_PITCH_SET_EL2_DRGB_SEL(reg32, val)                                      \
                REG_FLD_SET(OVL_EL2_PITCH_FLD_EL2_DRGB_SEL, (reg32), (val))
#define OVL_EL2_PITCH_SET_EL2_DA_SEL(reg32, val)                                        \
                REG_FLD_SET(OVL_EL2_PITCH_FLD_EL2_DA_SEL, (reg32), (val))
#define OVL_EL2_PITCH_SET_EL2_SRGB_SEL(reg32, val)                                      \
                REG_FLD_SET(OVL_EL2_PITCH_FLD_EL2_SRGB_SEL, (reg32), (val))
#define OVL_EL2_PITCH_SET_EL2_SA_SEL(reg32, val)                                        \
                REG_FLD_SET(OVL_EL2_PITCH_FLD_EL2_SA_SEL, (reg32), (val))
#define OVL_EL2_PITCH_SET_EL2_SRC_PITCH(reg32, val)                                     \
                REG_FLD_SET(OVL_EL2_PITCH_FLD_EL2_SRC_PITCH, (reg32), (val))

#define OVL_EL2_TILE_SET_TILE_HORI_BLOCK_NUM(reg32, val)                                \
                REG_FLD_SET(OVL_EL2_TILE_FLD_TILE_HORI_BLOCK_NUM, (reg32), (val))
#define OVL_EL2_TILE_SET_TILE_EN(reg32, val)                                            \
                REG_FLD_SET(OVL_EL2_TILE_FLD_TILE_EN, (reg32), (val))
#define OVL_EL2_TILE_SET_TILE_WIDTH_SEL(reg32, val)                                     \
                REG_FLD_SET(OVL_EL2_TILE_FLD_TILE_WIDTH_SEL, (reg32), (val))
#define OVL_EL2_TILE_SET_TILE_HEIGHT(reg32, val)                                        \
                REG_FLD_SET(OVL_EL2_TILE_FLD_TILE_HEIGHT, (reg32), (val))

#define OVL_EL2_CLIP_SET_EL2_SRC_BOTTOM_CLIP(reg32, val)                                \
                REG_FLD_SET(OVL_EL2_CLIP_FLD_EL2_SRC_BOTTOM_CLIP, (reg32), (val))
#define OVL_EL2_CLIP_SET_EL2_SRC_TOP_CLIP(reg32, val)                                   \
                REG_FLD_SET(OVL_EL2_CLIP_FLD_EL2_SRC_TOP_CLIP, (reg32), (val))
#define OVL_EL2_CLIP_SET_EL2_SRC_RIGHT_CLIP(reg32, val)                                 \
                REG_FLD_SET(OVL_EL2_CLIP_FLD_EL2_SRC_RIGHT_CLIP, (reg32), (val))
#define OVL_EL2_CLIP_SET_EL2_SRC_LEFT_CLIP(reg32, val)                                  \
                REG_FLD_SET(OVL_EL2_CLIP_FLD_EL2_SRC_LEFT_CLIP, (reg32), (val))

#define OVL_EL0_CLR_SET_ALPHA(reg32, val)       REG_FLD_SET(OVL_EL0_CLR_FLD_ALPHA, (reg32), (val))
#define OVL_EL0_CLR_SET_RED(reg32, val)         REG_FLD_SET(OVL_EL0_CLR_FLD_RED, (reg32), (val))
#define OVL_EL0_CLR_SET_GREEN(reg32, val)       REG_FLD_SET(OVL_EL0_CLR_FLD_GREEN, (reg32), (val))
#define OVL_EL0_CLR_SET_BLUE(reg32, val)        REG_FLD_SET(OVL_EL0_CLR_FLD_BLUE, (reg32), (val))

#define OVL_EL1_CLR_SET_ALPHA(reg32, val)       REG_FLD_SET(OVL_EL1_CLR_FLD_ALPHA, (reg32), (val))
#define OVL_EL1_CLR_SET_RED(reg32, val)         REG_FLD_SET(OVL_EL1_CLR_FLD_RED, (reg32), (val))
#define OVL_EL1_CLR_SET_GREEN(reg32, val)       REG_FLD_SET(OVL_EL1_CLR_FLD_GREEN, (reg32), (val))
#define OVL_EL1_CLR_SET_BLUE(reg32, val)        REG_FLD_SET(OVL_EL1_CLR_FLD_BLUE, (reg32), (val))

#define OVL_EL2_CLR_SET_ALPHA(reg32, val)       REG_FLD_SET(OVL_EL2_CLR_FLD_ALPHA, (reg32), (val))
#define OVL_EL2_CLR_SET_RED(reg32, val)         REG_FLD_SET(OVL_EL2_CLR_FLD_RED, (reg32), (val))
#define OVL_EL2_CLR_SET_GREEN(reg32, val)       REG_FLD_SET(OVL_EL2_CLR_FLD_GREEN, (reg32), (val))
#define OVL_EL2_CLR_SET_BLUE(reg32, val)        REG_FLD_SET(OVL_EL2_CLR_FLD_BLUE, (reg32), (val))

#define OVL_SBCH_SET_L3_SBCH_CNST_EN(reg32, val)                                        \
                REG_FLD_SET(OVL_SBCH_FLD_L3_SBCH_CNST_EN, (reg32), (val))
#define OVL_SBCH_SET_L3_SBCH_TRANS_EN(reg32, val)                                       \
                REG_FLD_SET(OVL_SBCH_FLD_L3_SBCH_TRANS_EN, (reg32), (val))
#define OVL_SBCH_SET_L2_SBCH_CNST_EN(reg32, val)                                        \
                REG_FLD_SET(OVL_SBCH_FLD_L2_SBCH_CNST_EN, (reg32), (val))
#define OVL_SBCH_SET_L2_SBCH_TRANS_EN(reg32, val)                                       \
                REG_FLD_SET(OVL_SBCH_FLD_L2_SBCH_TRANS_EN, (reg32), (val))
#define OVL_SBCH_SET_L1_SBCH_CNST_EN(reg32, val)                                        \
                REG_FLD_SET(OVL_SBCH_FLD_L1_SBCH_CNST_EN, (reg32), (val))
#define OVL_SBCH_SET_L1_SBCH_TRANS_EN(reg32, val)                                       \
                REG_FLD_SET(OVL_SBCH_FLD_L1_SBCH_TRANS_EN, (reg32), (val))
#define OVL_SBCH_SET_L0_SBCH_CNST_EN(reg32, val)                                        \
                REG_FLD_SET(OVL_SBCH_FLD_L0_SBCH_CNST_EN, (reg32), (val))
#define OVL_SBCH_SET_L0_SBCH_TRANS_EN(reg32, val)                                       \
                REG_FLD_SET(OVL_SBCH_FLD_L0_SBCH_TRANS_EN, (reg32), (val))
#define OVL_SBCH_SET_L3_SBCH_UPDATE(reg32, val)                                         \
                REG_FLD_SET(OVL_SBCH_FLD_L3_SBCH_UPDATE, (reg32), (val))
#define OVL_SBCH_SET_L2_SBCH_UPDATE(reg32, val)                                         \
                REG_FLD_SET(OVL_SBCH_FLD_L2_SBCH_UPDATE, (reg32), (val))
#define OVL_SBCH_SET_L1_SBCH_UPDATE(reg32, val)                                         \
                REG_FLD_SET(OVL_SBCH_FLD_L1_SBCH_UPDATE, (reg32), (val))
#define OVL_SBCH_SET_L0_SBCH_UPDATE(reg32, val)                                         \
                REG_FLD_SET(OVL_SBCH_FLD_L0_SBCH_UPDATE, (reg32), (val))

#define OVL_SBCH_EXT_SET_EL2_SBCH_CNST_EN(reg32, val)                                   \
                REG_FLD_SET(OVL_SBCH_EXT_FLD_EL2_SBCH_CNST_EN, (reg32), (val))
#define OVL_SBCH_EXT_SET_EL2_SBCH_TRANS_EN(reg32, val)                                  \
                REG_FLD_SET(OVL_SBCH_EXT_FLD_EL2_SBCH_TRANS_EN, (reg32), (val))
#define OVL_SBCH_EXT_SET_EL1_SBCH_CNST_EN(reg32, val)                                   \
                REG_FLD_SET(OVL_SBCH_EXT_FLD_EL1_SBCH_CNST_EN, (reg32), (val))
#define OVL_SBCH_EXT_SET_EL1_SBCH_TRANS_EN(reg32, val)                                  \
                REG_FLD_SET(OVL_SBCH_EXT_FLD_EL1_SBCH_TRANS_EN, (reg32), (val))
#define OVL_SBCH_EXT_SET_EL0_SBCH_CNST_EN(reg32, val)                                   \
                REG_FLD_SET(OVL_SBCH_EXT_FLD_EL0_SBCH_CNST_EN, (reg32), (val))
#define OVL_SBCH_EXT_SET_EL0_SBCH_TRANS_EN(reg32, val)                                  \
                REG_FLD_SET(OVL_SBCH_EXT_FLD_EL0_SBCH_TRANS_EN, (reg32), (val))
#define OVL_SBCH_EXT_SET_EL2_SBCH_UPDATE(reg32, val)                                    \
                REG_FLD_SET(OVL_SBCH_EXT_FLD_EL2_SBCH_UPDATE, (reg32), (val))
#define OVL_SBCH_EXT_SET_EL1_SBCH_UPDATE(reg32, val)                                    \
                REG_FLD_SET(OVL_SBCH_EXT_FLD_EL1_SBCH_UPDATE, (reg32), (val))
#define OVL_SBCH_EXT_SET_EL0_SBCH_UPDATE(reg32, val)                                    \
                REG_FLD_SET(OVL_SBCH_EXT_FLD_EL0_SBCH_UPDATE, (reg32), (val))

#define OVL_SBCH_CON_SET_EL2_SBCH_TRANS_INVALID(reg32, val)                             \
                REG_FLD_SET(OVL_SBCH_CON_FLD_EL2_SBCH_TRANS_INVALID, (reg32), (val))
#define OVL_SBCH_CON_SET_EL1_SBCH_TRANS_INVALID(reg32, val)                             \
                REG_FLD_SET(OVL_SBCH_CON_FLD_EL1_SBCH_TRANS_INVALID, (reg32), (val))
#define OVL_SBCH_CON_SET_EL0_SBCH_TRANS_INVALID(reg32, val)                             \
                REG_FLD_SET(OVL_SBCH_CON_FLD_EL0_SBCH_TRANS_INVALID, (reg32), (val))
#define OVL_SBCH_CON_SET_L3_SBCH_TRANS_INVALID(reg32, val)                              \
                REG_FLD_SET(OVL_SBCH_CON_FLD_L3_SBCH_TRANS_INVALID, (reg32), (val))
#define OVL_SBCH_CON_SET_L2_SBCH_TRANS_INVALID(reg32, val)                              \
                REG_FLD_SET(OVL_SBCH_CON_FLD_L2_SBCH_TRANS_INVALID, (reg32), (val))
#define OVL_SBCH_CON_SET_L1_SBCH_TRANS_INVALID(reg32, val)                              \
                REG_FLD_SET(OVL_SBCH_CON_FLD_L1_SBCH_TRANS_INVALID, (reg32), (val))
#define OVL_SBCH_CON_SET_L0_SBCH_TRANS_INVALID(reg32, val)                              \
                REG_FLD_SET(OVL_SBCH_CON_FLD_L0_SBCH_TRANS_INVALID, (reg32), (val))

#define OVL_L0_ADDR_SET_L0_ADDR(reg32, val)    REG_FLD_SET(OVL_L0_ADDR_FLD_L0_ADDR, (reg32), (val))
#define OVL_L1_ADDR_SET_L1_ADDR(reg32, val)    REG_FLD_SET(OVL_L1_ADDR_FLD_L1_ADDR, (reg32), (val))
#define OVL_L2_ADDR_SET_L2_ADDR(reg32, val)    REG_FLD_SET(OVL_L2_ADDR_FLD_L2_ADDR, (reg32), (val))
#define OVL_L3_ADDR_SET_L3_ADDR(reg32, val)    REG_FLD_SET(OVL_L3_ADDR_FLD_L3_ADDR, (reg32), (val))

#define OVL_EL0_ADDR_SET_EL0_ADDR(reg32, val)                                            \
                REG_FLD_SET(OVL_EL0_ADDR_FLD_EL0_ADDR, (reg32), (val))
#define OVL_EL1_ADDR_SET_EL1_ADDR(reg32, val)                                            \
                REG_FLD_SET(OVL_EL1_ADDR_FLD_EL1_ADDR, (reg32), (val))
#define OVL_EL2_ADDR_SET_EL2_ADDR(reg32, val)                                            \
                REG_FLD_SET(OVL_EL2_ADDR_FLD_EL2_ADDR, (reg32), (val))

#define OVL_SECURE_SET_L3_SECURE(reg32, val)  REG_FLD_SET(OVL_SECURE_FLD_L3_SECURE, (reg32), (val))
#define OVL_SECURE_SET_L2_SECURE(reg32, val)  REG_FLD_SET(OVL_SECURE_FLD_L2_SECURE, (reg32), (val))
#define OVL_SECURE_SET_L1_SECURE(reg32, val)  REG_FLD_SET(OVL_SECURE_FLD_L1_SECURE, (reg32), (val))
#define OVL_SECURE_SET_L0_SECURE(reg32, val)  REG_FLD_SET(OVL_SECURE_FLD_L0_SECURE, (reg32), (val))

#define OVL_STA_VAL_RDMA3_IDLE(val)             REG_FLD_VAL(OVL_STA_FLD_RDMA3_IDLE, (val))
#define OVL_STA_VAL_RDMA2_IDLE(val)             REG_FLD_VAL(OVL_STA_FLD_RDMA2_IDLE, (val))
#define OVL_STA_VAL_RDMA1_IDLE(val)             REG_FLD_VAL(OVL_STA_FLD_RDMA1_IDLE, (val))
#define OVL_STA_VAL_RDMA0_IDLE(val)             REG_FLD_VAL(OVL_STA_FLD_RDMA0_IDLE, (val))
#define OVL_STA_VAL_OVL_RUN(val)                REG_FLD_VAL(OVL_STA_FLD_OVL_RUN, (val))

#define OVL_INTEN_VAL_OVL_START_INTEN(val)                                               \
                REG_FLD_VAL(OVL_INTEN_FLD_OVL_START_INTEN, (val))
#define OVL_INTEN_VAL_ABNORMAL_SOF_INTEN(val)                                            \
                REG_FLD_VAL(OVL_INTEN_FLD_ABNORMAL_SOF_INTEN, (val))
#define OVL_INTEN_VAL_RDMA3_SMI_UNDERFLOW_INTEN(val)                                     \
                REG_FLD_VAL(OVL_INTEN_FLD_RDMA3_SMI_UNDERFLOW_INTEN, (val))
#define OVL_INTEN_VAL_RDMA2_SMI_UNDERFLOW_INTEN(val)                                     \
                REG_FLD_VAL(OVL_INTEN_FLD_RDMA2_SMI_UNDERFLOW_INTEN, (val))
#define OVL_INTEN_VAL_RDMA1_SMI_UNDERFLOW_INTEN(val)                                     \
                REG_FLD_VAL(OVL_INTEN_FLD_RDMA1_SMI_UNDERFLOW_INTEN, (val))
#define OVL_INTEN_VAL_RDMA0_SMI_UNDERFLOW_INTEN(val)                                     \
                REG_FLD_VAL(OVL_INTEN_FLD_RDMA0_SMI_UNDERFLOW_INTEN, (val))
#define OVL_INTEN_VAL_RDMA3_EOF_ABNORMAL_INTEN(val)                                      \
                REG_FLD_VAL(OVL_INTEN_FLD_RDMA3_EOF_ABNORMAL_INTEN, (val))
#define OVL_INTEN_VAL_RDMA2_EOF_ABNORMAL_INTEN(val)                                      \
                REG_FLD_VAL(OVL_INTEN_FLD_RDMA2_EOF_ABNORMAL_INTEN, (val))
#define OVL_INTEN_VAL_RDMA1_EOF_ABNORMAL_INTEN(val)                                      \
                REG_FLD_VAL(OVL_INTEN_FLD_RDMA1_EOF_ABNORMAL_INTEN, (val))
#define OVL_INTEN_VAL_RDMA0_EOF_ABNORMAL_INTEN(val)                                      \
                REG_FLD_VAL(OVL_INTEN_FLD_RDMA0_EOF_ABNORMAL_INTEN, (val))
#define OVL_INTEN_VAL_OVL_FME_HWRST_DONE_INTEN(val)                                      \
                REG_FLD_VAL(OVL_INTEN_FLD_OVL_FME_HWRST_DONE_INTEN, (val))
#define OVL_INTEN_VAL_OVL_FME_SWRST_DONE_INTEN(val)                                      \
                REG_FLD_VAL(OVL_INTEN_FLD_OVL_FME_SWRST_DONE_INTEN, (val))
#define OVL_INTEN_VAL_OVL_FME_UND_INTEN(val)                                             \
                REG_FLD_VAL(OVL_INTEN_FLD_OVL_FME_UND_INTEN, (val))
#define OVL_INTEN_VAL_OVL_FME_CPL_INTEN(val)                                             \
                REG_FLD_VAL(OVL_INTEN_FLD_OVL_FME_CPL_INTEN, (val))
#define OVL_INTEN_VAL_OVL_REG_CMT_INTEN(val)                                             \
                REG_FLD_VAL(OVL_INTEN_FLD_OVL_REG_CMT_INTEN, (val))

#define OVL_INTSTA_VAL_OVL_START_INTSTA(val)                                             \
                REG_FLD_VAL(OVL_INTSTA_FLD_OVL_START_INTSTA, (val))
#define OVL_INTSTA_VAL_ABNORMAL_SOF_INTSTA(val)                                          \
                REG_FLD_VAL(OVL_INTSTA_FLD_ABNORMAL_SOF_INTSTA, (val))
#define OVL_INTSTA_VAL_RDMA3_SMI_UNDERFLOW_INTSTA(val)                                   \
                REG_FLD_VAL(OVL_INTSTA_FLD_RDMA3_SMI_UNDERFLOW_INTSTA, (val))
#define OVL_INTSTA_VAL_RDMA2_SMI_UNDERFLOW_INTSTA(val)                                   \
                REG_FLD_VAL(OVL_INTSTA_FLD_RDMA2_SMI_UNDERFLOW_INTSTA, (val))
#define OVL_INTSTA_VAL_RDMA1_SMI_UNDERFLOW_INTSTA(val)                                   \
                REG_FLD_VAL(OVL_INTSTA_FLD_RDMA1_SMI_UNDERFLOW_INTSTA, (val))
#define OVL_INTSTA_VAL_RDMA0_SMI_UNDERFLOW_INTSTA(val)                                   \
                REG_FLD_VAL(OVL_INTSTA_FLD_RDMA0_SMI_UNDERFLOW_INTSTA, (val))
#define OVL_INTSTA_VAL_RDMA3_EOF_ABNORMAL_INTSTA(val)                                    \
                REG_FLD_VAL(OVL_INTSTA_FLD_RDMA3_EOF_ABNORMAL_INTSTA, (val))
#define OVL_INTSTA_VAL_RDMA2_EOF_ABNORMAL_INTSTA(val)                                    \
                REG_FLD_VAL(OVL_INTSTA_FLD_RDMA2_EOF_ABNORMAL_INTSTA, (val))
#define OVL_INTSTA_VAL_RDMA1_EOF_ABNORMAL_INTSTA(val)                                    \
                REG_FLD_VAL(OVL_INTSTA_FLD_RDMA1_EOF_ABNORMAL_INTSTA, (val))
#define OVL_INTSTA_VAL_RDMA0_EOF_ABNORMAL_INTSTA(val)                                    \
                REG_FLD_VAL(OVL_INTSTA_FLD_RDMA0_EOF_ABNORMAL_INTSTA, (val))
#define OVL_INTSTA_VAL_OVL_FME_HWRST_DONE_INTSTA(val)                                    \
                REG_FLD_VAL(OVL_INTSTA_FLD_OVL_FME_HWRST_DONE_INTSTA, (val))
#define OVL_INTSTA_VAL_OVL_FME_SWRST_DONE_INTSTA(val)                                    \
                REG_FLD_VAL(OVL_INTSTA_FLD_OVL_FME_SWRST_DONE_INTSTA, (val))
#define OVL_INTSTA_VAL_OVL_FME_UND_INTSTA(val)                                           \
                REG_FLD_VAL(OVL_INTSTA_FLD_OVL_FME_UND_INTSTA, (val))
#define OVL_INTSTA_VAL_OVL_FME_CPL_INTSTA(val)                                           \
                REG_FLD_VAL(OVL_INTSTA_FLD_OVL_FME_CPL_INTSTA, (val))
#define OVL_INTSTA_VAL_OVL_REG_CMT_INTSTA(val)                                           \
                REG_FLD_VAL(OVL_INTSTA_FLD_OVL_REG_CMT_INTSTA, (val))

#define OVL_EN_VAL_BLOCK_EXT_PREULTRA(val)      REG_FLD_VAL(OVL_EN_FLD_BLOCK_EXT_PREULTRA, (val))
#define OVL_EN_VAL_BLOCK_EXT_ULTRA(val)         REG_FLD_VAL(OVL_EN_FLD_BLOCK_EXT_ULTRA, (val))
#define OVL_EN_VAL_IGNORE_ABNORMAL_SOF(val)     REG_FLD_VAL(OVL_EN_FLD_IGNORE_ABNORMAL_SOF, (val))
#define OVL_EN_VAL_HF_FOVL_CK_ON(val)           REG_FLD_VAL(OVL_EN_FLD_HF_FOVL_CK_ON, (val))
#define OVL_EN_VAL_HG_FSMI_CK_ON(val)           REG_FLD_VAL(OVL_EN_FLD_HG_FSMI_CK_ON, (val))
#define OVL_EN_VAL_HG_FOVL_CK_ON(val)           REG_FLD_VAL(OVL_EN_FLD_HG_FOVL_CK_ON, (val))
#define OVL_EN_VAL_OVL_EN(val)                  REG_FLD_VAL(OVL_EN_FLD_OVL_EN, (val))

#define OVL_TRIG_VAL_CRC_CLR(val)               REG_FLD_VAL(OVL_TRIG_FLD_CRC_CLR, (val))
#define OVL_TRIG_VAL_CRC_EN(val)                REG_FLD_VAL(OVL_TRIG_FLD_CRC_EN, (val))
#define OVL_TRIG_VAL_OVL_SW_TRIG(val)           REG_FLD_VAL(OVL_TRIG_FLD_OVL_SW_TRIG, (val))

#define OVL_RST_VAL_OVL_SMI_IOBUF_HARD_RST(val)                                         \
                REG_FLD_VAL(OVL_RST_FLD_OVL_SMI_IOBUF_HARD_RST, (val))
#define OVL_RST_VAL_OVL_SMI_IOBUF_RST(val)      REG_FLD_VAL(OVL_RST_FLD_OVL_SMI_IOBUF_RST, (val))
#define OVL_RST_VAL_OVL_SMI_HARD_RST(val)       REG_FLD_VAL(OVL_RST_FLD_OVL_SMI_HARD_RST, (val))
#define OVL_RST_VAL_OVL_SMI_RST(val)            REG_FLD_VAL(OVL_RST_FLD_OVL_SMI_RST, (val))
#define OVL_RST_VAL_OVL_RST(val)                REG_FLD_VAL(OVL_RST_FLD_OVL_RST, (val))

#define OVL_ROI_SIZE_VAL_ROI_H(val)             REG_FLD_VAL(OVL_ROI_SIZE_FLD_ROI_H, (val))
#define OVL_ROI_SIZE_VAL_ROI_W(val)             REG_FLD_VAL(OVL_ROI_SIZE_FLD_ROI_W, (val))

#define OVL_DATAPATH_CON_VAL_WIDE_GAMUT_EN(val)                                         \
                REG_FLD_VAL(OVL_DATAPATH_CON_FLD_WIDE_GAMUT_EN, (val))
#define OVL_DATAPATH_CON_VAL_OUTPUT_INTERLACE(val)                                      \
                REG_FLD_VAL(OVL_DATAPATH_CON_FLD_OUTPUT_INTERLACE, (val))
#define OVL_DATAPATH_CON_VAL_OUTPUT_CLAMP(val)                                          \
                REG_FLD_VAL(OVL_DATAPATH_CON_FLD_OUTPUT_CLAMP, (val))
#define OVL_DATAPATH_CON_VAL_GCLAST_EN(val)                                             \
                REG_FLD_VAL(OVL_DATAPATH_CON_FLD_GCLAST_EN, (val))
#define OVL_DATAPATH_CON_VAL_RDMA3_OUT_SEL(val)                                         \
                REG_FLD_VAL(OVL_DATAPATH_CON_FLD_RDMA3_OUT_SEL, (val))
#define OVL_DATAPATH_CON_VAL_RDMA2_OUT_SEL(val)                                         \
                REG_FLD_VAL(OVL_DATAPATH_CON_FLD_RDMA2_OUT_SEL, (val))
#define OVL_DATAPATH_CON_VAL_RDMA1_OUT_SEL(val)                                         \
                REG_FLD_VAL(OVL_DATAPATH_CON_FLD_RDMA1_OUT_SEL, (val))
#define OVL_DATAPATH_CON_VAL_RDMA0_OUT_SEL(val)                                         \
                REG_FLD_VAL(OVL_DATAPATH_CON_FLD_RDMA0_OUT_SEL, (val))
#define OVL_DATAPATH_CON_VAL_PQ_OUT_SEL(val)                                            \
                REG_FLD_VAL(OVL_DATAPATH_CON_FLD_PQ_OUT_SEL, (val))
#define OVL_DATAPATH_CON_VAL_OVL_GAMMA_OUT(val)                                         \
                REG_FLD_VAL(OVL_DATAPATH_CON_FLD_OVL_GAMMA_OUT, (val))
#define OVL_DATAPATH_CON_VAL_ADOBE_LAYER(val)                                           \
                REG_FLD_VAL(OVL_DATAPATH_CON_FLD_ADOBE_LAYER, (val))
#define OVL_DATAPATH_CON_VAL_ADOBE_MODE(val)                                            \
                REG_FLD_VAL(OVL_DATAPATH_CON_FLD_ADOBE_MODE, (val))
#define OVL_DATAPATH_CON_VAL_L3_GPU_MODE(val)                                           \
                REG_FLD_VAL(OVL_DATAPATH_CON_FLD_L3_GPU_MODE, (val))
#define OVL_DATAPATH_CON_VAL_L2_GPU_MODE(val)                                           \
                REG_FLD_VAL(OVL_DATAPATH_CON_FLD_L2_GPU_MODE, (val))
#define OVL_DATAPATH_CON_VAL_L1_GPU_MODE(val)                                           \
                REG_FLD_VAL(OVL_DATAPATH_CON_FLD_L1_GPU_MODE, (val))
#define OVL_DATAPATH_CON_VAL_L0_GPU_MODE(val)                                           \
                REG_FLD_VAL(OVL_DATAPATH_CON_FLD_L0_GPU_MODE, (val))
#define OVL_DATAPATH_CON_VAL_OUTPUT_NO_RND(val)                                         \
                REG_FLD_VAL(OVL_DATAPATH_CON_FLD_OUTPUT_NO_RND, (val))
#define OVL_DATAPATH_CON_VAL_BGCLR_IN_SEL(val)                                          \
                REG_FLD_VAL(OVL_DATAPATH_CON_FLD_BGCLR_IN_SEL, (val))
#define OVL_DATAPATH_CON_VAL_OVL_RANDOM_BGCLR_EN(val)                                   \
                REG_FLD_VAL(OVL_DATAPATH_CON_FLD_OVL_RANDOM_BGCLR_EN, (val))
#define OVL_DATAPATH_CON_VAL_LAYER_SMI_ID_EN(val)                                       \
                REG_FLD_VAL(OVL_DATAPATH_CON_FLD_LAYER_SMI_ID_EN, (val))

#define OVL_ROI_BGCLR_VAL_ALPHA(val)            REG_FLD_VAL(OVL_ROI_BGCLR_FLD_ALPHA, (val))
#define OVL_ROI_BGCLR_VAL_RED(val)              REG_FLD_VAL(OVL_ROI_BGCLR_FLD_RED, (val))
#define OVL_ROI_BGCLR_VAL_GREEN(val)            REG_FLD_VAL(OVL_ROI_BGCLR_FLD_GREEN, (val))
#define OVL_ROI_BGCLR_VAL_BLUE(val)             REG_FLD_VAL(OVL_ROI_BGCLR_FLD_BLUE, (val))

#define OVL_SRC_CON_VAL_RELAY_MODE_EN(val)      REG_FLD_VAL(OVL_SRC_CON_FLD_RELAY_MODE_EN, (val))
#define OVL_SRC_CON_VAL_FORCE_RELAY_MODE(val)                                           \
                REG_FLD_VAL(OVL_SRC_CON_FLD_FORCE_RELAY_MODE, (val))
#define OVL_SRC_CON_VAL_LC_EN(val)              REG_FLD_VAL(OVL_SRC_CON_FLD_LC_EN, (val))
#define OVL_SRC_CON_VAL_L3_EN(val)              REG_FLD_VAL(OVL_SRC_CON_FLD_L3_EN, (val))
#define OVL_SRC_CON_VAL_L2_EN(val)              REG_FLD_VAL(OVL_SRC_CON_FLD_L2_EN, (val))
#define OVL_SRC_CON_VAL_L1_EN(val)              REG_FLD_VAL(OVL_SRC_CON_FLD_L1_EN, (val))
#define OVL_SRC_CON_VAL_L0_EN(val)              REG_FLD_VAL(OVL_SRC_CON_FLD_L0_EN, (val))

#define OVL_L0_CON_VAL_DSTKEY_EN(val)           REG_FLD_VAL(OVL_L0_CON_FLD_DSTKEY_EN, (val))
#define OVL_L0_CON_VAL_SRCKEY_EN(val)           REG_FLD_VAL(OVL_L0_CON_FLD_SRCKEY_EN, (val))
#define OVL_L0_CON_VAL_LAYER_SRC(val)           REG_FLD_VAL(OVL_L0_CON_FLD_LAYER_SRC, (val))
#define OVL_L0_CON_VAL_MTX_EN(val)              REG_FLD_VAL(OVL_L0_CON_FLD_MTX_EN, (val))
#define OVL_L0_CON_VAL_MTX_AUTO_DIS(val)        REG_FLD_VAL(OVL_L0_CON_FLD_MTX_AUTO_DIS, (val))
#define OVL_L0_CON_VAL_RGB_SWAP(val)            REG_FLD_VAL(OVL_L0_CON_FLD_RGB_SWAP, (val))
#define OVL_L0_CON_VAL_BYTE_SWAP(val)           REG_FLD_VAL(OVL_L0_CON_FLD_BYTE_SWAP, (val))
#define OVL_L0_CON_VAL_CLRFMT_MAN(val)          REG_FLD_VAL(OVL_L0_CON_FLD_CLRFMT_MAN, (val))
#define OVL_L0_CON_VAL_R_FIRST(val)             REG_FLD_VAL(OVL_L0_CON_FLD_R_FIRST, (val))
#define OVL_L0_CON_VAL_LANDSCAPE(val)           REG_FLD_VAL(OVL_L0_CON_FLD_LANDSCAPE, (val))
#define OVL_L0_CON_VAL_EN_3D(val)               REG_FLD_VAL(OVL_L0_CON_FLD_EN_3D, (val))
#define OVL_L0_CON_VAL_INT_MTX_SEL(val)         REG_FLD_VAL(OVL_L0_CON_FLD_INT_MTX_SEL, (val))
#define OVL_L0_CON_VAL_CLRFMT(val)              REG_FLD_VAL(OVL_L0_CON_FLD_CLRFMT, (val))
#define OVL_L0_CON_VAL_EXT_MTX_EN(val)          REG_FLD_VAL(OVL_L0_CON_FLD_EXT_MTX_EN, (val))
#define OVL_L0_CON_VAL_HORIZONTAL_FLIP_EN(val)                                          \
                REG_FLD_VAL(OVL_L0_CON_FLD_HORIZONTAL_FLIP_EN, (val))
#define OVL_L0_CON_VAL_VERTICAL_FLIP_EN(val)    REG_FLD_VAL(OVL_L0_CON_FLD_VERTICAL_FLIP_EN, (val))
#define OVL_L0_CON_VAL_ALPHA_EN(val)            REG_FLD_VAL(OVL_L0_CON_FLD_ALPHA_EN, (val))
#define OVL_L0_CON_VAL_ALPHA(val)               REG_FLD_VAL(OVL_L0_CON_FLD_ALPHA, (val))

#define OVL_L0_SRCKEY_VAL_SRCKEY(val)           REG_FLD_VAL(OVL_L0_SRCKEY_FLD_SRCKEY, (val))

#define OVL_L0_SRC_SIZE_VAL_L0_SRC_H(val)       REG_FLD_VAL(OVL_L0_SRC_SIZE_FLD_L0_SRC_H, (val))
#define OVL_L0_SRC_SIZE_VAL_L0_SRC_W(val)       REG_FLD_VAL(OVL_L0_SRC_SIZE_FLD_L0_SRC_W, (val))

#define OVL_L0_OFFSET_VAL_L0_YOFF(val)          REG_FLD_VAL(OVL_L0_OFFSET_FLD_L0_YOFF, (val))
#define OVL_L0_OFFSET_VAL_L0_XOFF(val)          REG_FLD_VAL(OVL_L0_OFFSET_FLD_L0_XOFF, (val))

#define OVL_L0_PITCH_VAL_SURFL_EN(val)          REG_FLD_VAL(OVL_L0_PITCH_FLD_SURFL_EN, (val))
#define OVL_L0_PITCH_VAL_L0_BLEND_RND_SHT(val)                                          \
                REG_FLD_VAL(OVL_L0_PITCH_FLD_L0_BLEND_RND_SHT, (val))
#define OVL_L0_PITCH_VAL_L0_SRGB_SEL_EXT2(val)                                          \
                REG_FLD_VAL(OVL_L0_PITCH_FLD_L0_SRGB_SEL_EXT2, (val))
#define OVL_L0_PITCH_VAL_L0_CONST_BLD(val)      REG_FLD_VAL(OVL_L0_PITCH_FLD_L0_CONST_BLD, (val))
#define OVL_L0_PITCH_VAL_L0_DRGB_SEL_EXT(val)                                           \
               REG_FLD_VAL(OVL_L0_PITCH_FLD_L0_DRGB_SEL_EXT, (val))
#define OVL_L0_PITCH_VAL_L0_DA_SEL_EXT(val)     REG_FLD_VAL(OVL_L0_PITCH_FLD_L0_DA_SEL_EXT, (val))
#define OVL_L0_PITCH_VAL_L0_SRGB_SEL_EXT(val)                                           \
               REG_FLD_VAL(OVL_L0_PITCH_FLD_L0_SRGB_SEL_EXT, (val))
#define OVL_L0_PITCH_VAL_L0_SA_SEL_EXT(val)     REG_FLD_VAL(OVL_L0_PITCH_FLD_L0_SA_SEL_EXT, (val))
#define OVL_L0_PITCH_VAL_L0_DRGB_SEL(val)       REG_FLD_VAL(OVL_L0_PITCH_FLD_L0_DRGB_SEL, (val))
#define OVL_L0_PITCH_VAL_L0_DA_SEL(val)         REG_FLD_VAL(OVL_L0_PITCH_FLD_L0_DA_SEL, (val))
#define OVL_L0_PITCH_VAL_L0_SRGB_SEL(val)       REG_FLD_VAL(OVL_L0_PITCH_FLD_L0_SRGB_SEL, (val))
#define OVL_L0_PITCH_VAL_L0_SA_SEL(val)         REG_FLD_VAL(OVL_L0_PITCH_FLD_L0_SA_SEL, (val))
#define OVL_L0_PITCH_VAL_L0_SRC_PITCH(val)      REG_FLD_VAL(OVL_L0_PITCH_FLD_L0_SRC_PITCH, (val))

#define OVL_L0_TILE_VAL_TILE_HORI_BLOCK_NUM(val)                                        \
                REG_FLD_VAL(OVL_L0_TILE_FLD_TILE_HORI_BLOCK_NUM, (val))
#define OVL_L0_TILE_VAL_TILE_EN(val)            REG_FLD_VAL(OVL_L0_TILE_FLD_TILE_EN, (val))
#define OVL_L0_TILE_VAL_TILE_WIDTH_SEL(val)     REG_FLD_VAL(OVL_L0_TILE_FLD_TILE_WIDTH_SEL, (val))
#define OVL_L0_TILE_VAL_TILE_HEIGHT(val)        REG_FLD_VAL(OVL_L0_TILE_FLD_TILE_HEIGHT, (val))

#define OVL_L0_CLIP_VAL_L0_SRC_BOTTOM_CLIP(val)                                        \
                REG_FLD_VAL(OVL_L0_CLIP_FLD_L0_SRC_BOTTOM_CLIP, (val))
#define OVL_L0_CLIP_VAL_L0_SRC_TOP_CLIP(val)    REG_FLD_VAL(OVL_L0_CLIP_FLD_L0_SRC_TOP_CLIP, (val))
#define OVL_L0_CLIP_VAL_L0_SRC_RIGHT_CLIP(val)                                          \
               REG_FLD_VAL(OVL_L0_CLIP_FLD_L0_SRC_RIGHT_CLIP, (val))
#define OVL_L0_CLIP_VAL_L0_SRC_LEFT_CLIP(val)   REG_FLD_VAL(OVL_L0_CLIP_FLD_L0_SRC_LEFT_CLIP, (val))

#define OVL_L1_CON_VAL_DSTKEY_EN(val)           REG_FLD_VAL(OVL_L1_CON_FLD_DSTKEY_EN, (val))
#define OVL_L1_CON_VAL_SRCKEY_EN(val)           REG_FLD_VAL(OVL_L1_CON_FLD_SRCKEY_EN, (val))
#define OVL_L1_CON_VAL_LAYER_SRC(val)           REG_FLD_VAL(OVL_L1_CON_FLD_LAYER_SRC, (val))
#define OVL_L1_CON_VAL_MTX_EN(val)              REG_FLD_VAL(OVL_L1_CON_FLD_MTX_EN, (val))
#define OVL_L1_CON_VAL_MTX_AUTO_DIS(val)        REG_FLD_VAL(OVL_L1_CON_FLD_MTX_AUTO_DIS, (val))
#define OVL_L1_CON_VAL_RGB_SWAP(val)            REG_FLD_VAL(OVL_L1_CON_FLD_RGB_SWAP, (val))
#define OVL_L1_CON_VAL_BYTE_SWAP(val)           REG_FLD_VAL(OVL_L1_CON_FLD_BYTE_SWAP, (val))
#define OVL_L1_CON_VAL_CLRFMT_MAN(val)          REG_FLD_VAL(OVL_L1_CON_FLD_CLRFMT_MAN, (val))
#define OVL_L1_CON_VAL_R_FIRST(val)             REG_FLD_VAL(OVL_L1_CON_FLD_R_FIRST, (val))
#define OVL_L1_CON_VAL_LANDSCAPE(val)           REG_FLD_VAL(OVL_L1_CON_FLD_LANDSCAPE, (val))
#define OVL_L1_CON_VAL_EN_3D(val)               REG_FLD_VAL(OVL_L1_CON_FLD_EN_3D, (val))
#define OVL_L1_CON_VAL_INT_MTX_SEL(val)         REG_FLD_VAL(OVL_L1_CON_FLD_INT_MTX_SEL, (val))
#define OVL_L1_CON_VAL_CLRFMT(val)              REG_FLD_VAL(OVL_L1_CON_FLD_CLRFMT, (val))
#define OVL_L1_CON_VAL_EXT_MTX_EN(val)          REG_FLD_VAL(OVL_L1_CON_FLD_EXT_MTX_EN, (val))
#define OVL_L1_CON_VAL_HORIZONTAL_FLIP_EN(val)                                          \
                REG_FLD_VAL(OVL_L1_CON_FLD_HORIZONTAL_FLIP_EN, (val))
#define OVL_L1_CON_VAL_VERTICAL_FLIP_EN(val)    REG_FLD_VAL(OVL_L1_CON_FLD_VERTICAL_FLIP_EN, (val))
#define OVL_L1_CON_VAL_ALPHA_EN(val)            REG_FLD_VAL(OVL_L1_CON_FLD_ALPHA_EN, (val))
#define OVL_L1_CON_VAL_ALPHA(val)               REG_FLD_VAL(OVL_L1_CON_FLD_ALPHA, (val))

#define OVL_L1_SRCKEY_VAL_SRCKEY(val)           REG_FLD_VAL(OVL_L1_SRCKEY_FLD_SRCKEY, (val))

#define OVL_L1_SRC_SIZE_VAL_L1_SRC_H(val)       REG_FLD_VAL(OVL_L1_SRC_SIZE_FLD_L1_SRC_H, (val))
#define OVL_L1_SRC_SIZE_VAL_L1_SRC_W(val)       REG_FLD_VAL(OVL_L1_SRC_SIZE_FLD_L1_SRC_W, (val))

#define OVL_L1_OFFSET_VAL_L1_YOFF(val)          REG_FLD_VAL(OVL_L1_OFFSET_FLD_L1_YOFF, (val))
#define OVL_L1_OFFSET_VAL_L1_XOFF(val)          REG_FLD_VAL(OVL_L1_OFFSET_FLD_L1_XOFF, (val))

#define OVL_L1_PITCH_VAL_SURFL_EN(val)          REG_FLD_VAL(OVL_L1_PITCH_FLD_SURFL_EN, (val))
#define OVL_L1_PITCH_VAL_L1_BLEND_RND_SHT(val)                                          \
                REG_FLD_VAL(OVL_L1_PITCH_FLD_L1_BLEND_RND_SHT, (val))
#define OVL_L1_PITCH_VAL_L1_SRGB_SEL_EXT2(val)                                          \
                REG_FLD_VAL(OVL_L1_PITCH_FLD_L1_SRGB_SEL_EXT2, (val))
#define OVL_L1_PITCH_VAL_L1_CONST_BLD(val)      REG_FLD_VAL(OVL_L1_PITCH_FLD_L1_CONST_BLD, (val))
#define OVL_L1_PITCH_VAL_L1_DRGB_SEL_EXT(val)                                           \
                REG_FLD_VAL(OVL_L1_PITCH_FLD_L1_DRGB_SEL_EXT, (val))
#define OVL_L1_PITCH_VAL_L1_DA_SEL_EXT(val)     REG_FLD_VAL(OVL_L1_PITCH_FLD_L1_DA_SEL_EXT, (val))
#define OVL_L1_PITCH_VAL_L1_SRGB_SEL_EXT(val)                                           \
                REG_FLD_VAL(OVL_L1_PITCH_FLD_L1_SRGB_SEL_EXT, (val))
#define OVL_L1_PITCH_VAL_L1_SA_SEL_EXT(val)     REG_FLD_VAL(OVL_L1_PITCH_FLD_L1_SA_SEL_EXT, (val))
#define OVL_L1_PITCH_VAL_L1_DRGB_SEL(val)       REG_FLD_VAL(OVL_L1_PITCH_FLD_L1_DRGB_SEL, (val))
#define OVL_L1_PITCH_VAL_L1_DA_SEL(val)         REG_FLD_VAL(OVL_L1_PITCH_FLD_L1_DA_SEL, (val))
#define OVL_L1_PITCH_VAL_L1_SRGB_SEL(val)       REG_FLD_VAL(OVL_L1_PITCH_FLD_L1_SRGB_SEL, (val))
#define OVL_L1_PITCH_VAL_L1_SA_SEL(val)         REG_FLD_VAL(OVL_L1_PITCH_FLD_L1_SA_SEL, (val))
#define OVL_L1_PITCH_VAL_L1_SRC_PITCH(val)      REG_FLD_VAL(OVL_L1_PITCH_FLD_L1_SRC_PITCH, (val))

#define OVL_L1_TILE_VAL_TILE_HORI_BLOCK_NUM(val)                                        \
                REG_FLD_VAL(OVL_L1_TILE_FLD_TILE_HORI_BLOCK_NUM, (val))
#define OVL_L1_TILE_VAL_TILE_EN(val)            REG_FLD_VAL(OVL_L1_TILE_FLD_TILE_EN, (val))
#define OVL_L1_TILE_VAL_TILE_WIDTH_SEL(val)     REG_FLD_VAL(OVL_L1_TILE_FLD_TILE_WIDTH_SEL, (val))
#define OVL_L1_TILE_VAL_TILE_HEIGHT(val)        REG_FLD_VAL(OVL_L1_TILE_FLD_TILE_HEIGHT, (val))

#define OVL_L1_CLIP_VAL_L1_SRC_BOTTOM_CLIP(val)                                         \
                REG_FLD_VAL(OVL_L1_CLIP_FLD_L1_SRC_BOTTOM_CLIP, (val))
#define OVL_L1_CLIP_VAL_L1_SRC_TOP_CLIP(val)                                            \
                REG_FLD_VAL(OVL_L1_CLIP_FLD_L1_SRC_TOP_CLIP, (val))
#define OVL_L1_CLIP_VAL_L1_SRC_RIGHT_CLIP(val)                                          \
                REG_FLD_VAL(OVL_L1_CLIP_FLD_L1_SRC_RIGHT_CLIP, (val))
#define OVL_L1_CLIP_VAL_L1_SRC_LEFT_CLIP(val)                                           \
                REG_FLD_VAL(OVL_L1_CLIP_FLD_L1_SRC_LEFT_CLIP, (val))

#define OVL_L2_CON_VAL_DSTKEY_EN(val)           REG_FLD_VAL(OVL_L2_CON_FLD_DSTKEY_EN, (val))
#define OVL_L2_CON_VAL_SRCKEY_EN(val)           REG_FLD_VAL(OVL_L2_CON_FLD_SRCKEY_EN, (val))
#define OVL_L2_CON_VAL_LAYER_SRC(val)           REG_FLD_VAL(OVL_L2_CON_FLD_LAYER_SRC, (val))
#define OVL_L2_CON_VAL_MTX_EN(val)              REG_FLD_VAL(OVL_L2_CON_FLD_MTX_EN, (val))
#define OVL_L2_CON_VAL_MTX_AUTO_DIS(val)        REG_FLD_VAL(OVL_L2_CON_FLD_MTX_AUTO_DIS, (val))
#define OVL_L2_CON_VAL_RGB_SWAP(val)            REG_FLD_VAL(OVL_L2_CON_FLD_RGB_SWAP, (val))
#define OVL_L2_CON_VAL_BYTE_SWAP(val)           REG_FLD_VAL(OVL_L2_CON_FLD_BYTE_SWAP, (val))
#define OVL_L2_CON_VAL_CLRFMT_MAN(val)          REG_FLD_VAL(OVL_L2_CON_FLD_CLRFMT_MAN, (val))
#define OVL_L2_CON_VAL_R_FIRST(val)             REG_FLD_VAL(OVL_L2_CON_FLD_R_FIRST, (val))
#define OVL_L2_CON_VAL_LANDSCAPE(val)           REG_FLD_VAL(OVL_L2_CON_FLD_LANDSCAPE, (val))
#define OVL_L2_CON_VAL_EN_3D(val)               REG_FLD_VAL(OVL_L2_CON_FLD_EN_3D, (val))
#define OVL_L2_CON_VAL_INT_MTX_SEL(val)         REG_FLD_VAL(OVL_L2_CON_FLD_INT_MTX_SEL, (val))
#define OVL_L2_CON_VAL_CLRFMT(val)              REG_FLD_VAL(OVL_L2_CON_FLD_CLRFMT, (val))
#define OVL_L2_CON_VAL_EXT_MTX_EN(val)          REG_FLD_VAL(OVL_L2_CON_FLD_EXT_MTX_EN, (val))
#define OVL_L2_CON_VAL_HORIZONTAL_FLIP_EN(val)                                          \
                REG_FLD_VAL(OVL_L2_CON_FLD_HORIZONTAL_FLIP_EN, (val))
#define OVL_L2_CON_VAL_VERTICAL_FLIP_EN(val)    REG_FLD_VAL(OVL_L2_CON_FLD_VERTICAL_FLIP_EN, (val))
#define OVL_L2_CON_VAL_ALPHA_EN(val)            REG_FLD_VAL(OVL_L2_CON_FLD_ALPHA_EN, (val))
#define OVL_L2_CON_VAL_ALPHA(val)               REG_FLD_VAL(OVL_L2_CON_FLD_ALPHA, (val))

#define OVL_L2_SRCKEY_VAL_SRCKEY(val)           REG_FLD_VAL(OVL_L2_SRCKEY_FLD_SRCKEY, (val))

#define OVL_L2_SRC_SIZE_VAL_L2_SRC_H(val)       REG_FLD_VAL(OVL_L2_SRC_SIZE_FLD_L2_SRC_H, (val))
#define OVL_L2_SRC_SIZE_VAL_L2_SRC_W(val)       REG_FLD_VAL(OVL_L2_SRC_SIZE_FLD_L2_SRC_W, (val))

#define OVL_L2_OFFSET_VAL_L2_YOFF(val)          REG_FLD_VAL(OVL_L2_OFFSET_FLD_L2_YOFF, (val))
#define OVL_L2_OFFSET_VAL_L2_XOFF(val)          REG_FLD_VAL(OVL_L2_OFFSET_FLD_L2_XOFF, (val))

#define OVL_L2_PITCH_VAL_SURFL_EN(val)          REG_FLD_VAL(OVL_L2_PITCH_FLD_SURFL_EN, (val))
#define OVL_L2_PITCH_VAL_L2_BLEND_RND_SHT(val)                                          \
                REG_FLD_VAL(OVL_L2_PITCH_FLD_L2_BLEND_RND_SHT, (val))
#define OVL_L2_PITCH_VAL_L2_SRGB_SEL_EXT2(val)                                          \
                REG_FLD_VAL(OVL_L2_PITCH_FLD_L2_SRGB_SEL_EXT2, (val))
#define OVL_L2_PITCH_VAL_L2_CONST_BLD(val)      REG_FLD_VAL(OVL_L2_PITCH_FLD_L2_CONST_BLD, (val))
#define OVL_L2_PITCH_VAL_L2_DRGB_SEL_EXT(val)                                           \
                REG_FLD_VAL(OVL_L2_PITCH_FLD_L2_DRGB_SEL_EXT, (val))
#define OVL_L2_PITCH_VAL_L2_DA_SEL_EXT(val)     REG_FLD_VAL(OVL_L2_PITCH_FLD_L2_DA_SEL_EXT, (val))
#define OVL_L2_PITCH_VAL_L2_SRGB_SEL_EXT(val)                                           \
                REG_FLD_VAL(OVL_L2_PITCH_FLD_L2_SRGB_SEL_EXT, (val))
#define OVL_L2_PITCH_VAL_L2_SA_SEL_EXT(val)     REG_FLD_VAL(OVL_L2_PITCH_FLD_L2_SA_SEL_EXT, (val))
#define OVL_L2_PITCH_VAL_L2_DRGB_SEL(val)       REG_FLD_VAL(OVL_L2_PITCH_FLD_L2_DRGB_SEL, (val))
#define OVL_L2_PITCH_VAL_L2_DA_SEL(val)         REG_FLD_VAL(OVL_L2_PITCH_FLD_L2_DA_SEL, (val))
#define OVL_L2_PITCH_VAL_L2_SRGB_SEL(val)       REG_FLD_VAL(OVL_L2_PITCH_FLD_L2_SRGB_SEL, (val))
#define OVL_L2_PITCH_VAL_L2_SA_SEL(val)         REG_FLD_VAL(OVL_L2_PITCH_FLD_L2_SA_SEL, (val))
#define OVL_L2_PITCH_VAL_L2_SRC_PITCH(val)      REG_FLD_VAL(OVL_L2_PITCH_FLD_L2_SRC_PITCH, (val))

#define OVL_L2_TILE_VAL_TILE_HORI_BLOCK_NUM(val)                                        \
                REG_FLD_VAL(OVL_L2_TILE_FLD_TILE_HORI_BLOCK_NUM, (val))
#define OVL_L2_TILE_VAL_TILE_EN(val)            REG_FLD_VAL(OVL_L2_TILE_FLD_TILE_EN, (val))
#define OVL_L2_TILE_VAL_TILE_WIDTH_SEL(val)     REG_FLD_VAL(OVL_L2_TILE_FLD_TILE_WIDTH_SEL, (val))
#define OVL_L2_TILE_VAL_TILE_HEIGHT(val)        REG_FLD_VAL(OVL_L2_TILE_FLD_TILE_HEIGHT, (val))

#define OVL_L2_CLIP_VAL_L2_SRC_BOTTOM_CLIP(val)                                         \
                REG_FLD_VAL(OVL_L2_CLIP_FLD_L2_SRC_BOTTOM_CLIP, (val))
#define OVL_L2_CLIP_VAL_L2_SRC_TOP_CLIP(val)    REG_FLD_VAL(OVL_L2_CLIP_FLD_L2_SRC_TOP_CLIP, (val))
#define OVL_L2_CLIP_VAL_L2_SRC_RIGHT_CLIP(val)                                          \
                REG_FLD_VAL(OVL_L2_CLIP_FLD_L2_SRC_RIGHT_CLIP, (val))
#define OVL_L2_CLIP_VAL_L2_SRC_LEFT_CLIP(val)                                           \
                REG_FLD_VAL(OVL_L2_CLIP_FLD_L2_SRC_LEFT_CLIP, (val))

#define OVL_L3_CON_VAL_DSTKEY_EN(val)           REG_FLD_VAL(OVL_L3_CON_FLD_DSTKEY_EN, (val))
#define OVL_L3_CON_VAL_SRCKEY_EN(val)           REG_FLD_VAL(OVL_L3_CON_FLD_SRCKEY_EN, (val))
#define OVL_L3_CON_VAL_LAYER_SRC(val)           REG_FLD_VAL(OVL_L3_CON_FLD_LAYER_SRC, (val))
#define OVL_L3_CON_VAL_MTX_EN(val)              REG_FLD_VAL(OVL_L3_CON_FLD_MTX_EN, (val))
#define OVL_L3_CON_VAL_MTX_AUTO_DIS(val)        REG_FLD_VAL(OVL_L3_CON_FLD_MTX_AUTO_DIS, (val))
#define OVL_L3_CON_VAL_RGB_SWAP(val)            REG_FLD_VAL(OVL_L3_CON_FLD_RGB_SWAP, (val))
#define OVL_L3_CON_VAL_BYTE_SWAP(val)           REG_FLD_VAL(OVL_L3_CON_FLD_BYTE_SWAP, (val))
#define OVL_L3_CON_VAL_CLRFMT_MAN(val)          REG_FLD_VAL(OVL_L3_CON_FLD_CLRFMT_MAN, (val))
#define OVL_L3_CON_VAL_R_FIRST(val)             REG_FLD_VAL(OVL_L3_CON_FLD_R_FIRST, (val))
#define OVL_L3_CON_VAL_LANDSCAPE(val)           REG_FLD_VAL(OVL_L3_CON_FLD_LANDSCAPE, (val))
#define OVL_L3_CON_VAL_EN_3D(val)               REG_FLD_VAL(OVL_L3_CON_FLD_EN_3D, (val))
#define OVL_L3_CON_VAL_INT_MTX_SEL(val)         REG_FLD_VAL(OVL_L3_CON_FLD_INT_MTX_SEL, (val))
#define OVL_L3_CON_VAL_CLRFMT(val)              REG_FLD_VAL(OVL_L3_CON_FLD_CLRFMT, (val))
#define OVL_L3_CON_VAL_EXT_MTX_EN(val)          REG_FLD_VAL(OVL_L3_CON_FLD_EXT_MTX_EN, (val))
#define OVL_L3_CON_VAL_HORIZONTAL_FLIP_EN(val)                                          \
                REG_FLD_VAL(OVL_L3_CON_FLD_HORIZONTAL_FLIP_EN, (val))
#define OVL_L3_CON_VAL_VERTICAL_FLIP_EN(val)    REG_FLD_VAL(OVL_L3_CON_FLD_VERTICAL_FLIP_EN, (val))
#define OVL_L3_CON_VAL_ALPHA_EN(val)            REG_FLD_VAL(OVL_L3_CON_FLD_ALPHA_EN, (val))
#define OVL_L3_CON_VAL_ALPHA(val)               REG_FLD_VAL(OVL_L3_CON_FLD_ALPHA, (val))

#define OVL_L3_SRCKEY_VAL_SRCKEY(val)           REG_FLD_VAL(OVL_L3_SRCKEY_FLD_SRCKEY, (val))

#define OVL_L3_SRC_SIZE_VAL_L3_SRC_H(val)       REG_FLD_VAL(OVL_L3_SRC_SIZE_FLD_L3_SRC_H, (val))
#define OVL_L3_SRC_SIZE_VAL_L3_SRC_W(val)       REG_FLD_VAL(OVL_L3_SRC_SIZE_FLD_L3_SRC_W, (val))

#define OVL_L3_OFFSET_VAL_L3_YOFF(val)          REG_FLD_VAL(OVL_L3_OFFSET_FLD_L3_YOFF, (val))
#define OVL_L3_OFFSET_VAL_L3_XOFF(val)          REG_FLD_VAL(OVL_L3_OFFSET_FLD_L3_XOFF, (val))

#define OVL_L3_PITCH_VAL_SURFL_EN(val)          REG_FLD_VAL(OVL_L3_PITCH_FLD_SURFL_EN, (val))
#define OVL_L3_PITCH_VAL_L3_BLEND_RND_SHT(val)                                           \
                REG_FLD_VAL(OVL_L3_PITCH_FLD_L3_BLEND_RND_SHT, (val))
#define OVL_L3_PITCH_VAL_L3_SRGB_SEL_EXT2(val)                                           \
                REG_FLD_VAL(OVL_L3_PITCH_FLD_L3_SRGB_SEL_EXT2, (val))
#define OVL_L3_PITCH_VAL_L3_CONST_BLD(val)      REG_FLD_VAL(OVL_L3_PITCH_FLD_L3_CONST_BLD, (val))
#define OVL_L3_PITCH_VAL_L3_DRGB_SEL_EXT(val)                                            \
                REG_FLD_VAL(OVL_L3_PITCH_FLD_L3_DRGB_SEL_EXT, (val))
#define OVL_L3_PITCH_VAL_L3_DA_SEL_EXT(val)     REG_FLD_VAL(OVL_L3_PITCH_FLD_L3_DA_SEL_EXT, (val))
#define OVL_L3_PITCH_VAL_L3_SRGB_SEL_EXT(val)                                            \
                REG_FLD_VAL(OVL_L3_PITCH_FLD_L3_SRGB_SEL_EXT, (val))
#define OVL_L3_PITCH_VAL_L3_SA_SEL_EXT(val)     REG_FLD_VAL(OVL_L3_PITCH_FLD_L3_SA_SEL_EXT, (val))
#define OVL_L3_PITCH_VAL_L3_DRGB_SEL(val)       REG_FLD_VAL(OVL_L3_PITCH_FLD_L3_DRGB_SEL, (val))
#define OVL_L3_PITCH_VAL_L3_DA_SEL(val)         REG_FLD_VAL(OVL_L3_PITCH_FLD_L3_DA_SEL, (val))
#define OVL_L3_PITCH_VAL_L3_SRGB_SEL(val)       REG_FLD_VAL(OVL_L3_PITCH_FLD_L3_SRGB_SEL, (val))
#define OVL_L3_PITCH_VAL_L3_SA_SEL(val)         REG_FLD_VAL(OVL_L3_PITCH_FLD_L3_SA_SEL, (val))
#define OVL_L3_PITCH_VAL_L3_SRC_PITCH(val)      REG_FLD_VAL(OVL_L3_PITCH_FLD_L3_SRC_PITCH, (val))

#define OVL_L3_TILE_VAL_TILE_HORI_BLOCK_NUM(val)                                        \
                REG_FLD_VAL(OVL_L3_TILE_FLD_TILE_HORI_BLOCK_NUM, (val))
#define OVL_L3_TILE_VAL_TILE_EN(val)            REG_FLD_VAL(OVL_L3_TILE_FLD_TILE_EN, (val))
#define OVL_L3_TILE_VAL_TILE_WIDTH_SEL(val)     REG_FLD_VAL(OVL_L3_TILE_FLD_TILE_WIDTH_SEL, (val))
#define OVL_L3_TILE_VAL_TILE_HEIGHT(val)        REG_FLD_VAL(OVL_L3_TILE_FLD_TILE_HEIGHT, (val))

#define OVL_L3_CLIP_VAL_L3_SRC_BOTTOM_CLIP(val)                                         \
                REG_FLD_VAL(OVL_L3_CLIP_FLD_L3_SRC_BOTTOM_CLIP, (val))
#define OVL_L3_CLIP_VAL_L3_SRC_TOP_CLIP(val)                                            \
                REG_FLD_VAL(OVL_L3_CLIP_FLD_L3_SRC_TOP_CLIP, (val))
#define OVL_L3_CLIP_VAL_L3_SRC_RIGHT_CLIP(val)                                          \
                REG_FLD_VAL(OVL_L3_CLIP_FLD_L3_SRC_RIGHT_CLIP, (val))
#define OVL_L3_CLIP_VAL_L3_SRC_LEFT_CLIP(val)                                           \
                REG_FLD_VAL(OVL_L3_CLIP_FLD_L3_SRC_LEFT_CLIP, (val))

#define OVL_RDMA0_CTRL_VAL_RDMA0_FIFO_USED_SIZE(val)                                    \
                REG_FLD_VAL(OVL_RDMA0_CTRL_FLD_RDMA0_FIFO_USED_SIZE, (val))
#define OVL_RDMA0_CTRL_VAL_RDMA0_INTERLACE(val)                                         \
                REG_FLD_VAL(OVL_RDMA0_CTRL_FLD_RDMA0_INTERLACE, (val))
#define OVL_RDMA0_CTRL_VAL_RDMA0_EN(val)                                                \
                REG_FLD_VAL(OVL_RDMA0_CTRL_FLD_RDMA0_EN, (val))

#define OVL_RDMA0_MEM_GMC_SETTING1_VAL_RDMA0_PRE_ULTRA_THRESHOLD_HIGH_OFS(val)          \
                REG_FLD_VAL(OVL_RDMA0_MEM_GMC_SETTING1_FLD_RDMA0_PRE_ULTRA_THRESHOLD_HIGH_OFS,  \
                (val))
#define OVL_RDMA0_MEM_GMC_SETTING1_VAL_RDMA0_ULTRA_THRESHOLD_HIGH_OFS(val)              \
                REG_FLD_VAL(OVL_RDMA0_MEM_GMC_SETTING1_FLD_RDMA0_ULTRA_THRESHOLD_HIGH_OFS, (val))
#define OVL_RDMA0_MEM_GMC_SETTING1_VAL_RDMA0_OSTD_PREULTRA_TH(val)                      \
                REG_FLD_VAL(OVL_RDMA0_MEM_GMC_SETTING1_FLD_RDMA0_OSTD_PREULTRA_TH, (val))
#define OVL_RDMA0_MEM_GMC_SETTING1_VAL_RDMA0_OSTD_ULTRA_TH(val)                         \
                REG_FLD_VAL(OVL_RDMA0_MEM_GMC_SETTING1_FLD_RDMA0_OSTD_ULTRA_TH, (val))

#define OVL_RDMA0_MEM_SLOW_CON_VAL_RDMA0_SLOW_CNT(val)                                  \
                REG_FLD_VAL(OVL_RDMA0_MEM_SLOW_CON_FLD_RDMA0_SLOW_CNT, (val))
#define OVL_RDMA0_MEM_SLOW_CON_VAL_RDMA0_SLOW_EN(val)                                   \
                REG_FLD_VAL(OVL_RDMA0_MEM_SLOW_CON_FLD_RDMA0_SLOW_EN, (val))

#define OVL_RDMA0_FIFO_CTRL_VAL_RDMA0_FIFO_UND_EN(val)                                  \
                REG_FLD_VAL(OVL_RDMA0_FIFO_CTRL_FLD_RDMA0_FIFO_UND_EN, (val))
#define OVL_RDMA0_FIFO_CTRL_VAL_RDMA0_FIFO_SIZE(val)                                    \
                REG_FLD_VAL(OVL_RDMA0_FIFO_CTRL_FLD_RDMA0_FIFO_SIZE, (val))
#define OVL_RDMA0_FIFO_CTRL_VAL_RDMA0_FIFO_THRD(val)                                    \
                REG_FLD_VAL(OVL_RDMA0_FIFO_CTRL_FLD_RDMA0_FIFO_THRD, (val))

#define OVL_RDMA1_CTRL_VAL_RDMA1_FIFO_USED_SIZE(val)                                    \
                REG_FLD_VAL(OVL_RDMA1_CTRL_FLD_RDMA1_FIFO_USED_SIZE, (val))
#define OVL_RDMA1_CTRL_VAL_RDMA1_INTERLACE(val)                                         \
                REG_FLD_VAL(OVL_RDMA1_CTRL_FLD_RDMA1_INTERLACE, (val))
#define OVL_RDMA1_CTRL_VAL_RDMA1_EN(val)                                                \
                REG_FLD_VAL(OVL_RDMA1_CTRL_FLD_RDMA1_EN, (val))

#define OVL_RDMA1_MEM_GMC_SETTING1_VAL_RDMA1_PRE_ULTRA_THRESHOLD_HIGH_OFS(val)          \
                REG_FLD_VAL(OVL_RDMA1_MEM_GMC_SETTING1_FLD_RDMA1_PRE_ULTRA_THRESHOLD_HIGH_OFS,  \
                (val))
#define OVL_RDMA1_MEM_GMC_SETTING1_VAL_RDMA1_ULTRA_THRESHOLD_HIGH_OFS(val)              \
                REG_FLD_VAL(OVL_RDMA1_MEM_GMC_SETTING1_FLD_RDMA1_ULTRA_THRESHOLD_HIGH_OFS, (val))
#define OVL_RDMA1_MEM_GMC_SETTING1_VAL_RDMA1_OSTD_PREULTRA_TH(val)                      \
                REG_FLD_VAL(OVL_RDMA1_MEM_GMC_SETTING1_FLD_RDMA1_OSTD_PREULTRA_TH, (val))
#define OVL_RDMA1_MEM_GMC_SETTING1_VAL_RDMA1_OSTD_ULTRA_TH(val)                         \
                REG_FLD_VAL(OVL_RDMA1_MEM_GMC_SETTING1_FLD_RDMA1_OSTD_ULTRA_TH, (val))

#define OVL_RDMA1_MEM_SLOW_CON_VAL_RDMA1_SLOW_CNT(val)                                  \
                REG_FLD_VAL(OVL_RDMA1_MEM_SLOW_CON_FLD_RDMA1_SLOW_CNT, (val))
#define OVL_RDMA1_MEM_SLOW_CON_VAL_RDMA1_SLOW_EN(val)                                   \
                REG_FLD_VAL(OVL_RDMA1_MEM_SLOW_CON_FLD_RDMA1_SLOW_EN, (val))

#define OVL_RDMA1_FIFO_CTRL_VAL_RDMA1_FIFO_UND_EN(val)                                  \
                REG_FLD_VAL(OVL_RDMA1_FIFO_CTRL_FLD_RDMA1_FIFO_UND_EN, (val))
#define OVL_RDMA1_FIFO_CTRL_VAL_RDMA1_FIFO_SIZE(val)                                    \
                REG_FLD_VAL(OVL_RDMA1_FIFO_CTRL_FLD_RDMA1_FIFO_SIZE, (val))
#define OVL_RDMA1_FIFO_CTRL_VAL_RDMA1_FIFO_THRD(val)                                    \
                REG_FLD_VAL(OVL_RDMA1_FIFO_CTRL_FLD_RDMA1_FIFO_THRD, (val))

#define OVL_RDMA2_CTRL_VAL_RDMA2_FIFO_USED_SIZE(val)                                    \
                REG_FLD_VAL(OVL_RDMA2_CTRL_FLD_RDMA2_FIFO_USED_SIZE, (val))
#define OVL_RDMA2_CTRL_VAL_RDMA2_INTERLACE(val)                                         \
                REG_FLD_VAL(OVL_RDMA2_CTRL_FLD_RDMA2_INTERLACE, (val))
#define OVL_RDMA2_CTRL_VAL_RDMA2_EN(val)                                                \
                REG_FLD_VAL(OVL_RDMA2_CTRL_FLD_RDMA2_EN, (val))

#define OVL_RDMA2_MEM_GMC_SETTING1_VAL_RDMA2_PRE_ULTRA_THRESHOLD_HIGH_OFS(val)          \
                REG_FLD_VAL(OVL_RDMA2_MEM_GMC_SETTING1_FLD_RDMA2_PRE_ULTRA_THRESHOLD_HIGH_OFS,  \
                (val))
#define OVL_RDMA2_MEM_GMC_SETTING1_VAL_RDMA2_ULTRA_THRESHOLD_HIGH_OFS(val)              \
                REG_FLD_VAL(OVL_RDMA2_MEM_GMC_SETTING1_FLD_RDMA2_ULTRA_THRESHOLD_HIGH_OFS, (val))
#define OVL_RDMA2_MEM_GMC_SETTING1_VAL_RDMA2_OSTD_PREULTRA_TH(val)                      \
                REG_FLD_VAL(OVL_RDMA2_MEM_GMC_SETTING1_FLD_RDMA2_OSTD_PREULTRA_TH, (val))
#define OVL_RDMA2_MEM_GMC_SETTING1_VAL_RDMA2_OSTD_ULTRA_TH(val)                         \
                REG_FLD_VAL(OVL_RDMA2_MEM_GMC_SETTING1_FLD_RDMA2_OSTD_ULTRA_TH, (val))

#define OVL_RDMA2_MEM_SLOW_CON_VAL_RDMA2_SLOW_CNT(val)                                  \
                REG_FLD_VAL(OVL_RDMA2_MEM_SLOW_CON_FLD_RDMA2_SLOW_CNT, (val))
#define OVL_RDMA2_MEM_SLOW_CON_VAL_RDMA2_SLOW_EN(val)                                   \
                REG_FLD_VAL(OVL_RDMA2_MEM_SLOW_CON_FLD_RDMA2_SLOW_EN, (val))

#define OVL_RDMA2_FIFO_CTRL_VAL_RDMA2_FIFO_UND_EN(val)                                  \
                REG_FLD_VAL(OVL_RDMA2_FIFO_CTRL_FLD_RDMA2_FIFO_UND_EN, (val))
#define OVL_RDMA2_FIFO_CTRL_VAL_RDMA2_FIFO_SIZE(val)                                    \
                REG_FLD_VAL(OVL_RDMA2_FIFO_CTRL_FLD_RDMA2_FIFO_SIZE, (val))
#define OVL_RDMA2_FIFO_CTRL_VAL_RDMA2_FIFO_THRD(val)                                    \
                REG_FLD_VAL(OVL_RDMA2_FIFO_CTRL_FLD_RDMA2_FIFO_THRD, (val))

#define OVL_RDMA3_CTRL_VAL_RDMA3_FIFO_USED_SIZE(val)                                    \
                REG_FLD_VAL(OVL_RDMA3_CTRL_FLD_RDMA3_FIFO_USED_SIZE, (val))
#define OVL_RDMA3_CTRL_VAL_RDMA3_INTERLACE(val)                                         \
                REG_FLD_VAL(OVL_RDMA3_CTRL_FLD_RDMA3_INTERLACE, (val))
#define OVL_RDMA3_CTRL_VAL_RDMA3_EN(val)                                                \
                REG_FLD_VAL(OVL_RDMA3_CTRL_FLD_RDMA3_EN, (val))

#define OVL_RDMA3_MEM_GMC_SETTING1_VAL_RDMA3_PRE_ULTRA_THRESHOLD_HIGH_OFS(val)          \
                REG_FLD_VAL(OVL_RDMA3_MEM_GMC_SETTING1_FLD_RDMA3_PRE_ULTRA_THRESHOLD_HIGH_OFS,  \
                (val))
#define OVL_RDMA3_MEM_GMC_SETTING1_VAL_RDMA3_ULTRA_THRESHOLD_HIGH_OFS(val)              \
                REG_FLD_VAL(OVL_RDMA3_MEM_GMC_SETTING1_FLD_RDMA3_ULTRA_THRESHOLD_HIGH_OFS, (val))
#define OVL_RDMA3_MEM_GMC_SETTING1_VAL_RDMA3_OSTD_PREULTRA_TH(val)                      \
                REG_FLD_VAL(OVL_RDMA3_MEM_GMC_SETTING1_FLD_RDMA3_OSTD_PREULTRA_TH, (val))
#define OVL_RDMA3_MEM_GMC_SETTING1_VAL_RDMA3_OSTD_ULTRA_TH(val)                         \
                REG_FLD_VAL(OVL_RDMA3_MEM_GMC_SETTING1_FLD_RDMA3_OSTD_ULTRA_TH, (val))

#define OVL_RDMA3_MEM_SLOW_CON_VAL_RDMA3_SLOW_CNT(val)                                  \
                REG_FLD_VAL(OVL_RDMA3_MEM_SLOW_CON_FLD_RDMA3_SLOW_CNT, (val))
#define OVL_RDMA3_MEM_SLOW_CON_VAL_RDMA3_SLOW_EN(val)                                   \
                REG_FLD_VAL(OVL_RDMA3_MEM_SLOW_CON_FLD_RDMA3_SLOW_EN, (val))

#define OVL_RDMA3_FIFO_CTRL_VAL_RDMA3_FIFO_UND_EN(val)                                  \
                REG_FLD_VAL(OVL_RDMA3_FIFO_CTRL_FLD_RDMA3_FIFO_UND_EN, (val))
#define OVL_RDMA3_FIFO_CTRL_VAL_RDMA3_FIFO_SIZE(val)                                    \
                REG_FLD_VAL(OVL_RDMA3_FIFO_CTRL_FLD_RDMA3_FIFO_SIZE, (val))
#define OVL_RDMA3_FIFO_CTRL_VAL_RDMA3_FIFO_THRD(val)                                    \
                REG_FLD_VAL(OVL_RDMA3_FIFO_CTRL_FLD_RDMA3_FIFO_THRD, (val))

#define OVL_L0_Y2R_PARA_R0_VAL_C_CF_RMU(val)                                            \
                REG_FLD_VAL(OVL_L0_Y2R_PARA_R0_FLD_C_CF_RMU, (val))
#define OVL_L0_Y2R_PARA_R0_VAL_C_CF_RMY(val)                                            \
                REG_FLD_VAL(OVL_L0_Y2R_PARA_R0_FLD_C_CF_RMY, (val))

#define OVL_L0_Y2R_PARA_R1_VAL_C_CF_RMV(val)                                            \
                REG_FLD_VAL(OVL_L0_Y2R_PARA_R1_FLD_C_CF_RMV, (val))

#define OVL_L0_Y2R_PARA_G0_VAL_C_CF_GMU(val)                                            \
                REG_FLD_VAL(OVL_L0_Y2R_PARA_G0_FLD_C_CF_GMU, (val))
#define OVL_L0_Y2R_PARA_G0_VAL_C_CF_GMY(val)                                            \
                REG_FLD_VAL(OVL_L0_Y2R_PARA_G0_FLD_C_CF_GMY, (val))

#define OVL_L0_Y2R_PARA_G1_VAL_C_CF_GMV(val)                                            \
                REG_FLD_VAL(OVL_L0_Y2R_PARA_G1_FLD_C_CF_GMV, (val))

#define OVL_L0_Y2R_PARA_B0_VAL_C_CF_BMU(val)                                            \
                REG_FLD_VAL(OVL_L0_Y2R_PARA_B0_FLD_C_CF_BMU, (val))
#define OVL_L0_Y2R_PARA_B0_VAL_C_CF_BMY(val)                                            \
                REG_FLD_VAL(OVL_L0_Y2R_PARA_B0_FLD_C_CF_BMY, (val))

#define OVL_L0_Y2R_PARA_B1_VAL_C_CF_BMV(val)                                            \
                REG_FLD_VAL(OVL_L0_Y2R_PARA_B1_FLD_C_CF_BMV, (val))

#define OVL_L0_Y2R_PARA_YUV_A_0_VAL_C_CF_UA(val)                                        \
                REG_FLD_VAL(OVL_L0_Y2R_PARA_YUV_A_0_FLD_C_CF_UA, (val))
#define OVL_L0_Y2R_PARA_YUV_A_0_VAL_C_CF_YA(val)                                        \
                REG_FLD_VAL(OVL_L0_Y2R_PARA_YUV_A_0_FLD_C_CF_YA, (val))

#define OVL_L0_Y2R_PARA_YUV_A_1_VAL_C_CF_VA(val)                                        \
                REG_FLD_VAL(OVL_L0_Y2R_PARA_YUV_A_1_FLD_C_CF_VA, (val))

#define OVL_L0_Y2R_PARA_RGB_A_0_VAL_C_CF_GA(val)                                        \
                REG_FLD_VAL(OVL_L0_Y2R_PARA_RGB_A_0_FLD_C_CF_GA, (val))
#define OVL_L0_Y2R_PARA_RGB_A_0_VAL_C_CF_RA(val)                                        \
                REG_FLD_VAL(OVL_L0_Y2R_PARA_RGB_A_0_FLD_C_CF_RA, (val))

#define OVL_L0_Y2R_PARA_RGB_A_1_VAL_C_CF_BA(val)                                        \
                REG_FLD_VAL(OVL_L0_Y2R_PARA_RGB_A_1_FLD_C_CF_BA, (val))

#define OVL_L1_Y2R_PARA_R0_VAL_C_CF_RMU(val)    REG_FLD_VAL(OVL_L1_Y2R_PARA_R0_FLD_C_CF_RMU, (val))
#define OVL_L1_Y2R_PARA_R0_VAL_C_CF_RMY(val)    REG_FLD_VAL(OVL_L1_Y2R_PARA_R0_FLD_C_CF_RMY, (val))

#define OVL_L1_Y2R_PARA_R1_VAL_C_CF_RMV(val)    REG_FLD_VAL(OVL_L1_Y2R_PARA_R1_FLD_C_CF_RMV, (val))

#define OVL_L1_Y2R_PARA_G0_VAL_C_CF_GMU(val)    REG_FLD_VAL(OVL_L1_Y2R_PARA_G0_FLD_C_CF_GMU, (val))
#define OVL_L1_Y2R_PARA_G0_VAL_C_CF_GMY(val)    REG_FLD_VAL(OVL_L1_Y2R_PARA_G0_FLD_C_CF_GMY, (val))

#define OVL_L1_Y2R_PARA_G1_VAL_C_CF_GMV(val)    REG_FLD_VAL(OVL_L1_Y2R_PARA_G1_FLD_C_CF_GMV, (val))

#define OVL_L1_Y2R_PARA_B0_VAL_C_CF_BMU(val)    REG_FLD_VAL(OVL_L1_Y2R_PARA_B0_FLD_C_CF_BMU, (val))
#define OVL_L1_Y2R_PARA_B0_VAL_C_CF_BMY(val)    REG_FLD_VAL(OVL_L1_Y2R_PARA_B0_FLD_C_CF_BMY, (val))

#define OVL_L1_Y2R_PARA_B1_VAL_C_CF_BMV(val)    REG_FLD_VAL(OVL_L1_Y2R_PARA_B1_FLD_C_CF_BMV, (val))

#define OVL_L1_Y2R_PARA_YUV_A_0_VAL_C_CF_UA(val)                                        \
                REG_FLD_VAL(OVL_L1_Y2R_PARA_YUV_A_0_FLD_C_CF_UA, (val))
#define OVL_L1_Y2R_PARA_YUV_A_0_VAL_C_CF_YA(val)                                        \
                REG_FLD_VAL(OVL_L1_Y2R_PARA_YUV_A_0_FLD_C_CF_YA, (val))

#define OVL_L1_Y2R_PARA_YUV_A_1_VAL_C_CF_VA(val)                                        \
                REG_FLD_VAL(OVL_L1_Y2R_PARA_YUV_A_1_FLD_C_CF_VA, (val))

#define OVL_L1_Y2R_PARA_RGB_A_0_VAL_C_CF_GA(val)                                        \
                REG_FLD_VAL(OVL_L1_Y2R_PARA_RGB_A_0_FLD_C_CF_GA, (val))
#define OVL_L1_Y2R_PARA_RGB_A_0_VAL_C_CF_RA(val)                                        \
                REG_FLD_VAL(OVL_L1_Y2R_PARA_RGB_A_0_FLD_C_CF_RA, (val))

#define OVL_L1_Y2R_PARA_RGB_A_1_VAL_C_CF_BA(val)                                        \
                REG_FLD_VAL(OVL_L1_Y2R_PARA_RGB_A_1_FLD_C_CF_BA, (val))

#define OVL_L2_Y2R_PARA_R0_VAL_C_CF_RMU(val)    REG_FLD_VAL(OVL_L2_Y2R_PARA_R0_FLD_C_CF_RMU, (val))
#define OVL_L2_Y2R_PARA_R0_VAL_C_CF_RMY(val)    REG_FLD_VAL(OVL_L2_Y2R_PARA_R0_FLD_C_CF_RMY, (val))

#define OVL_L2_Y2R_PARA_R1_VAL_C_CF_RMV(val)    REG_FLD_VAL(OVL_L2_Y2R_PARA_R1_FLD_C_CF_RMV, (val))

#define OVL_L2_Y2R_PARA_G0_VAL_C_CF_GMU(val)    REG_FLD_VAL(OVL_L2_Y2R_PARA_G0_FLD_C_CF_GMU, (val))
#define OVL_L2_Y2R_PARA_G0_VAL_C_CF_GMY(val)    REG_FLD_VAL(OVL_L2_Y2R_PARA_G0_FLD_C_CF_GMY, (val))

#define OVL_L2_Y2R_PARA_G1_VAL_C_CF_GMV(val)    REG_FLD_VAL(OVL_L2_Y2R_PARA_G1_FLD_C_CF_GMV, (val))

#define OVL_L2_Y2R_PARA_B0_VAL_C_CF_BMU(val)    REG_FLD_VAL(OVL_L2_Y2R_PARA_B0_FLD_C_CF_BMU, (val))
#define OVL_L2_Y2R_PARA_B0_VAL_C_CF_BMY(val)    REG_FLD_VAL(OVL_L2_Y2R_PARA_B0_FLD_C_CF_BMY, (val))

#define OVL_L2_Y2R_PARA_B1_VAL_C_CF_BMV(val)    REG_FLD_VAL(OVL_L2_Y2R_PARA_B1_FLD_C_CF_BMV, (val))

#define OVL_L2_Y2R_PARA_YUV_A_0_VAL_C_CF_UA(val)                                        \
                REG_FLD_VAL(OVL_L2_Y2R_PARA_YUV_A_0_FLD_C_CF_UA, (val))
#define OVL_L2_Y2R_PARA_YUV_A_0_VAL_C_CF_YA(val)                                        \
                REG_FLD_VAL(OVL_L2_Y2R_PARA_YUV_A_0_FLD_C_CF_YA, (val))

#define OVL_L2_Y2R_PARA_YUV_A_1_VAL_C_CF_VA(val)                                        \
                REG_FLD_VAL(OVL_L2_Y2R_PARA_YUV_A_1_FLD_C_CF_VA, (val))

#define OVL_L2_Y2R_PARA_RGB_A_0_VAL_C_CF_GA(val)                                        \
                REG_FLD_VAL(OVL_L2_Y2R_PARA_RGB_A_0_FLD_C_CF_GA, (val))
#define OVL_L2_Y2R_PARA_RGB_A_0_VAL_C_CF_RA(val)                                        \
                REG_FLD_VAL(OVL_L2_Y2R_PARA_RGB_A_0_FLD_C_CF_RA, (val))

#define OVL_L2_Y2R_PARA_RGB_A_1_VAL_C_CF_BA(val)                                        \
                REG_FLD_VAL(OVL_L2_Y2R_PARA_RGB_A_1_FLD_C_CF_BA, (val))

#define OVL_L3_Y2R_PARA_R0_VAL_C_CF_RMU(val)    REG_FLD_VAL(OVL_L3_Y2R_PARA_R0_FLD_C_CF_RMU, (val))
#define OVL_L3_Y2R_PARA_R0_VAL_C_CF_RMY(val)    REG_FLD_VAL(OVL_L3_Y2R_PARA_R0_FLD_C_CF_RMY, (val))

#define OVL_L3_Y2R_PARA_R1_VAL_C_CF_RMV(val)    REG_FLD_VAL(OVL_L3_Y2R_PARA_R1_FLD_C_CF_RMV, (val))

#define OVL_L3_Y2R_PARA_G0_VAL_C_CF_GMU(val)    REG_FLD_VAL(OVL_L3_Y2R_PARA_G0_FLD_C_CF_GMU, (val))
#define OVL_L3_Y2R_PARA_G0_VAL_C_CF_GMY(val)    REG_FLD_VAL(OVL_L3_Y2R_PARA_G0_FLD_C_CF_GMY, (val))

#define OVL_L3_Y2R_PARA_G1_VAL_C_CF_GMV(val)    REG_FLD_VAL(OVL_L3_Y2R_PARA_G1_FLD_C_CF_GMV, (val))

#define OVL_L3_Y2R_PARA_B0_VAL_C_CF_BMU(val)    REG_FLD_VAL(OVL_L3_Y2R_PARA_B0_FLD_C_CF_BMU, (val))
#define OVL_L3_Y2R_PARA_B0_VAL_C_CF_BMY(val)    REG_FLD_VAL(OVL_L3_Y2R_PARA_B0_FLD_C_CF_BMY, (val))

#define OVL_L3_Y2R_PARA_B1_VAL_C_CF_BMV(val)    REG_FLD_VAL(OVL_L3_Y2R_PARA_B1_FLD_C_CF_BMV, (val))

#define OVL_L3_Y2R_PARA_YUV_A_0_VAL_C_CF_UA(val)                                        \
                REG_FLD_VAL(OVL_L3_Y2R_PARA_YUV_A_0_FLD_C_CF_UA, (val))
#define OVL_L3_Y2R_PARA_YUV_A_0_VAL_C_CF_YA(val)                                        \
                REG_FLD_VAL(OVL_L3_Y2R_PARA_YUV_A_0_FLD_C_CF_YA, (val))

#define OVL_L3_Y2R_PARA_YUV_A_1_VAL_C_CF_VA(val)                                        \
                REG_FLD_VAL(OVL_L3_Y2R_PARA_YUV_A_1_FLD_C_CF_VA, (val))

#define OVL_L3_Y2R_PARA_RGB_A_0_VAL_C_CF_GA(val)                                        \
                REG_FLD_VAL(OVL_L3_Y2R_PARA_RGB_A_0_FLD_C_CF_GA, (val))
#define OVL_L3_Y2R_PARA_RGB_A_0_VAL_C_CF_RA(val)                                        \
                REG_FLD_VAL(OVL_L3_Y2R_PARA_RGB_A_0_FLD_C_CF_RA, (val))

#define OVL_L3_Y2R_PARA_RGB_A_1_VAL_C_CF_BA(val)                                        \
                REG_FLD_VAL(OVL_L3_Y2R_PARA_RGB_A_1_FLD_C_CF_BA, (val))

#define OVL_DEBUG_MON_SEL_VAL_DBG_MON_SEL(val)                                          \
                REG_FLD_VAL(OVL_DEBUG_MON_SEL_FLD_DBG_MON_SEL, (val))

#define OVL_BLD_EXT_VAL_EL2_MINUS_BLD(val)      REG_FLD_VAL(OVL_BLD_EXT_FLD_EL2_MINUS_BLD, (val))
#define OVL_BLD_EXT_VAL_EL1_MINUS_BLD(val)      REG_FLD_VAL(OVL_BLD_EXT_FLD_EL1_MINUS_BLD, (val))
#define OVL_BLD_EXT_VAL_EL0_MINUS_BLD(val)      REG_FLD_VAL(OVL_BLD_EXT_FLD_EL0_MINUS_BLD, (val))
#define OVL_BLD_EXT_VAL_LC_MINUS_BLD(val)       REG_FLD_VAL(OVL_BLD_EXT_FLD_LC_MINUS_BLD, (val))
#define OVL_BLD_EXT_VAL_L3_MINUS_BLD(val)       REG_FLD_VAL(OVL_BLD_EXT_FLD_L3_MINUS_BLD, (val))
#define OVL_BLD_EXT_VAL_L2_MINUS_BLD(val)       REG_FLD_VAL(OVL_BLD_EXT_FLD_L2_MINUS_BLD, (val))
#define OVL_BLD_EXT_VAL_L1_MINUS_BLD(val)       REG_FLD_VAL(OVL_BLD_EXT_FLD_L1_MINUS_BLD, (val))
#define OVL_BLD_EXT_VAL_L0_MINUS_BLD(val)       REG_FLD_VAL(OVL_BLD_EXT_FLD_L0_MINUS_BLD, (val))

#define OVL_RDMA0_MEM_GMC_SETTING2_VAL_RDMA0_FORCE_REQ_THRESHOLD(val)                   \
                REG_FLD_VAL(OVL_RDMA0_MEM_GMC_SETTING2_FLD_RDMA0_FORCE_REQ_THRESHOLD, (val))
#define OVL_RDMA0_MEM_GMC_SETTING2_VAL_RDMA0_REQ_THRESHOLD_ULTRA(val)                   \
                REG_FLD_VAL(OVL_RDMA0_MEM_GMC_SETTING2_FLD_RDMA0_REQ_THRESHOLD_ULTRA, (val))
#define OVL_RDMA0_MEM_GMC_SETTING2_VAL_RDMA0_REQ_THRESHOLD_PREULTRA(val)                \
                REG_FLD_VAL(OVL_RDMA0_MEM_GMC_SETTING2_FLD_RDMA0_REQ_THRESHOLD_PREULTRA, (val))
#define OVL_RDMA0_MEM_GMC_SETTING2_VAL_RDMA0_ISSUE_REQ_THRESHOLD_URG(val)               \
                REG_FLD_VAL(OVL_RDMA0_MEM_GMC_SETTING2_FLD_RDMA0_ISSUE_REQ_THRESHOLD_URG, (val))
#define OVL_RDMA0_MEM_GMC_SETTING2_VAL_RDMA0_ISSUE_REQ_THRESHOLD(val)                   \
                REG_FLD_VAL(OVL_RDMA0_MEM_GMC_SETTING2_FLD_RDMA0_ISSUE_REQ_THRESHOLD, (val))

#define OVL_RDMA1_MEM_GMC_SETTING2_VAL_RDMA1_FORCE_REQ_THRESHOLD(val)                   \
                REG_FLD_VAL(OVL_RDMA1_MEM_GMC_SETTING2_FLD_RDMA1_FORCE_REQ_THRESHOLD, (val))
#define OVL_RDMA1_MEM_GMC_SETTING2_VAL_RDMA1_REQ_THRESHOLD_ULTRA(val)                   \
                REG_FLD_VAL(OVL_RDMA1_MEM_GMC_SETTING2_FLD_RDMA1_REQ_THRESHOLD_ULTRA, (val))
#define OVL_RDMA1_MEM_GMC_SETTING2_VAL_RDMA1_REQ_THRESHOLD_PREULTRA(val)                \
                REG_FLD_VAL(OVL_RDMA1_MEM_GMC_SETTING2_FLD_RDMA1_REQ_THRESHOLD_PREULTRA, (val))
#define OVL_RDMA1_MEM_GMC_SETTING2_VAL_RDMA1_ISSUE_REQ_THRESHOLD_URG(val)               \
                REG_FLD_VAL(OVL_RDMA1_MEM_GMC_SETTING2_FLD_RDMA1_ISSUE_REQ_THRESHOLD_URG, (val))
#define OVL_RDMA1_MEM_GMC_SETTING2_VAL_RDMA1_ISSUE_REQ_THRESHOLD(val)                   \
                REG_FLD_VAL(OVL_RDMA1_MEM_GMC_SETTING2_FLD_RDMA1_ISSUE_REQ_THRESHOLD, (val))

#define OVL_RDMA2_MEM_GMC_SETTING2_VAL_RDMA2_FORCE_REQ_THRESHOLD(val)                   \
                REG_FLD_VAL(OVL_RDMA2_MEM_GMC_SETTING2_FLD_RDMA2_FORCE_REQ_THRESHOLD, (val))
#define OVL_RDMA2_MEM_GMC_SETTING2_VAL_RDMA2_REQ_THRESHOLD_ULTRA(val)                   \
                REG_FLD_VAL(OVL_RDMA2_MEM_GMC_SETTING2_FLD_RDMA2_REQ_THRESHOLD_ULTRA, (val))
#define OVL_RDMA2_MEM_GMC_SETTING2_VAL_RDMA2_REQ_THRESHOLD_PREULTRA(val)                \
                REG_FLD_VAL(OVL_RDMA2_MEM_GMC_SETTING2_FLD_RDMA2_REQ_THRESHOLD_PREULTRA, (val))
#define OVL_RDMA2_MEM_GMC_SETTING2_VAL_RDMA2_ISSUE_REQ_THRESHOLD_URG(val)               \
                REG_FLD_VAL(OVL_RDMA2_MEM_GMC_SETTING2_FLD_RDMA2_ISSUE_REQ_THRESHOLD_URG, (val))
#define OVL_RDMA2_MEM_GMC_SETTING2_VAL_RDMA2_ISSUE_REQ_THRESHOLD(val)                   \
                REG_FLD_VAL(OVL_RDMA2_MEM_GMC_SETTING2_FLD_RDMA2_ISSUE_REQ_THRESHOLD, (val))

#define OVL_RDMA3_MEM_GMC_SETTING2_VAL_RDMA3_FORCE_REQ_THRESHOLD(val)                   \
                REG_FLD_VAL(OVL_RDMA3_MEM_GMC_SETTING2_FLD_RDMA3_FORCE_REQ_THRESHOLD, (val))
#define OVL_RDMA3_MEM_GMC_SETTING2_VAL_RDMA3_REQ_THRESHOLD_ULTRA(val)                   \
                REG_FLD_VAL(OVL_RDMA3_MEM_GMC_SETTING2_FLD_RDMA3_REQ_THRESHOLD_ULTRA, (val))
#define OVL_RDMA3_MEM_GMC_SETTING2_VAL_RDMA3_REQ_THRESHOLD_PREULTRA(val)                \
                REG_FLD_VAL(OVL_RDMA3_MEM_GMC_SETTING2_FLD_RDMA3_REQ_THRESHOLD_PREULTRA, (val))
#define OVL_RDMA3_MEM_GMC_SETTING2_VAL_RDMA3_ISSUE_REQ_THRESHOLD_URG(val)               \
                REG_FLD_VAL(OVL_RDMA3_MEM_GMC_SETTING2_FLD_RDMA3_ISSUE_REQ_THRESHOLD_URG, (val))
#define OVL_RDMA3_MEM_GMC_SETTING2_VAL_RDMA3_ISSUE_REQ_THRESHOLD(val)                   \
                REG_FLD_VAL(OVL_RDMA3_MEM_GMC_SETTING2_FLD_RDMA3_ISSUE_REQ_THRESHOLD, (val))

#define OVL_RDMA_BURST_CON0_VAL_BURST_128B_BOUND(val)                                   \
                REG_FLD_VAL(OVL_RDMA_BURST_CON0_FLD_BURST_128B_BOUND, (val))
#define OVL_RDMA_BURST_CON0_VAL_BURST15A_32B(val)                                       \
                REG_FLD_VAL(OVL_RDMA_BURST_CON0_FLD_BURST15A_32B, (val))
#define OVL_RDMA_BURST_CON0_VAL_BURST14A_32B(val)                                       \
                REG_FLD_VAL(OVL_RDMA_BURST_CON0_FLD_BURST14A_32B, (val))
#define OVL_RDMA_BURST_CON0_VAL_BURST13A_32B(val)                                       \
                REG_FLD_VAL(OVL_RDMA_BURST_CON0_FLD_BURST13A_32B, (val))
#define OVL_RDMA_BURST_CON0_VAL_BURST12A_32B(val)                                       \
                REG_FLD_VAL(OVL_RDMA_BURST_CON0_FLD_BURST12A_32B, (val))
#define OVL_RDMA_BURST_CON0_VAL_BURST11A_32B(val)                                       \
                REG_FLD_VAL(OVL_RDMA_BURST_CON0_FLD_BURST11A_32B, (val))
#define OVL_RDMA_BURST_CON0_VAL_BURST10A_32B(val)                                       \
                REG_FLD_VAL(OVL_RDMA_BURST_CON0_FLD_BURST10A_32B, (val))
#define OVL_RDMA_BURST_CON0_VAL_BURST9A_32B(val)                                        \
                REG_FLD_VAL(OVL_RDMA_BURST_CON0_FLD_BURST9A_32B, (val))

#define OVL_RDMA_BURST_CON1_VAL_BURST15A_N32B(val)                                      \
                REG_FLD_VAL(OVL_RDMA_BURST_CON1_FLD_BURST15A_N32B, (val))
#define OVL_RDMA_BURST_CON1_VAL_BURST14A_N32B(val)                                      \
                REG_FLD_VAL(OVL_RDMA_BURST_CON1_FLD_BURST14A_N32B, (val))
#define OVL_RDMA_BURST_CON1_VAL_BURST13A_N32B(val)                                      \
                REG_FLD_VAL(OVL_RDMA_BURST_CON1_FLD_BURST13A_N32B, (val))
#define OVL_RDMA_BURST_CON1_VAL_BURST12A_N32B(val)                                      \
                REG_FLD_VAL(OVL_RDMA_BURST_CON1_FLD_BURST12A_N32B, (val))
#define OVL_RDMA_BURST_CON1_VAL_BURST11A_N32B(val)                                      \
                REG_FLD_VAL(OVL_RDMA_BURST_CON1_FLD_BURST11A_N32B, (val))
#define OVL_RDMA_BURST_CON1_VAL_BURST10A_N32B(val)                                      \
                REG_FLD_VAL(OVL_RDMA_BURST_CON1_FLD_BURST10A_N32B, (val))
#define OVL_RDMA_BURST_CON1_VAL_BURST9A_N32B(val)                                       \
                REG_FLD_VAL(OVL_RDMA_BURST_CON1_FLD_BURST9A_N32B, (val))

#define OVL_RDMA_GREQ_NUM_VAL_IOBUF_FLUSH_ULTRA(val)                                    \
                REG_FLD_VAL(OVL_RDMA_GREQ_NUM_FLD_IOBUF_FLUSH_ULTRA, (val))
#define OVL_RDMA_GREQ_NUM_VAL_IOBUF_FLUSH_PREULTRA(val)                                 \
                REG_FLD_VAL(OVL_RDMA_GREQ_NUM_FLD_IOBUF_FLUSH_PREULTRA, (val))
#define OVL_RDMA_GREQ_NUM_VAL_GRP_BRK_STOP(val)                                         \
                REG_FLD_VAL(OVL_RDMA_GREQ_NUM_FLD_GRP_BRK_STOP, (val))
#define OVL_RDMA_GREQ_NUM_VAL_GRP_END_STOP(val)                                         \
                REG_FLD_VAL(OVL_RDMA_GREQ_NUM_FLD_GRP_END_STOP, (val))
#define OVL_RDMA_GREQ_NUM_VAL_GREQ_STOP_EN(val)                                         \
                REG_FLD_VAL(OVL_RDMA_GREQ_NUM_FLD_GREQ_STOP_EN, (val))
#define OVL_RDMA_GREQ_NUM_VAL_GREQ_DIS_CNT(val)                                         \
                REG_FLD_VAL(OVL_RDMA_GREQ_NUM_FLD_GREQ_DIS_CNT, (val))
#define OVL_RDMA_GREQ_NUM_VAL_OSTD_GREQ_NUM(val)                                        \
                REG_FLD_VAL(OVL_RDMA_GREQ_NUM_FLD_OSTD_GREQ_NUM, (val))
#define OVL_RDMA_GREQ_NUM_VAL_LAYER3_GREQ_NUM(val)                                      \
                REG_FLD_VAL(OVL_RDMA_GREQ_NUM_FLD_LAYER3_GREQ_NUM, (val))
#define OVL_RDMA_GREQ_NUM_VAL_LAYER2_GREQ_NUM(val)                                      \
                REG_FLD_VAL(OVL_RDMA_GREQ_NUM_FLD_LAYER2_GREQ_NUM, (val))
#define OVL_RDMA_GREQ_NUM_VAL_LAYER1_GREQ_NUM(val)                                      \
                REG_FLD_VAL(OVL_RDMA_GREQ_NUM_FLD_LAYER1_GREQ_NUM, (val))
#define OVL_RDMA_GREQ_NUM_VAL_LAYER0_GREQ_NUM(val)                                      \
                REG_FLD_VAL(OVL_RDMA_GREQ_NUM_FLD_LAYER0_GREQ_NUM, (val))

#define OVL_RDMA_GREQ_URG_NUM_VAL_GREQ_NUM_SHT(val)                                     \
                REG_FLD_VAL(OVL_RDMA_GREQ_URG_NUM_FLD_GREQ_NUM_SHT, (val))
#define OVL_RDMA_GREQ_URG_NUM_VAL_GREQ_NUM_SHT_VAL(val)                                 \
                REG_FLD_VAL(OVL_RDMA_GREQ_URG_NUM_FLD_GREQ_NUM_SHT_VAL, (val))
#define OVL_RDMA_GREQ_URG_NUM_VAL_ARG_URG_BIAS(val)                                     \
                REG_FLD_VAL(OVL_RDMA_GREQ_URG_NUM_FLD_ARG_URG_BIAS, (val))
#define OVL_RDMA_GREQ_URG_NUM_VAL_ARG_GREQ_URG_TH(val)                                  \
                REG_FLD_VAL(OVL_RDMA_GREQ_URG_NUM_FLD_ARG_GREQ_URG_TH, (val))
#define OVL_RDMA_GREQ_URG_NUM_VAL_LAYER3_GREQ_URG_NUM(val)                              \
                REG_FLD_VAL(OVL_RDMA_GREQ_URG_NUM_FLD_LAYER3_GREQ_URG_NUM, (val))
#define OVL_RDMA_GREQ_URG_NUM_VAL_LAYER2_GREQ_URG_NUM(val)                              \
                REG_FLD_VAL(OVL_RDMA_GREQ_URG_NUM_FLD_LAYER2_GREQ_URG_NUM, (val))
#define OVL_RDMA_GREQ_URG_NUM_VAL_LAYER1_GREQ_URG_NUM(val)                              \
                REG_FLD_VAL(OVL_RDMA_GREQ_URG_NUM_FLD_LAYER1_GREQ_URG_NUM, (val))
#define OVL_RDMA_GREQ_URG_NUM_VAL_LAYER0_GREQ_URG_NUM(val)                              \
                REG_FLD_VAL(OVL_RDMA_GREQ_URG_NUM_FLD_LAYER0_GREQ_URG_NUM, (val))

#define OVL_DUMMY_REG_VAL_OVERLAY_DUMMY(val)                                            \
                REG_FLD_VAL(OVL_DUMMY_REG_FLD_OVERLAY_DUMMY, (val))

#define OVL_GDRDY_PRD_VAL_GDRDY_PRD(val)                                                \
                REG_FLD_VAL(OVL_GDRDY_PRD_FLD_GDRDY_PRD, (val))

#define OVL_RDMA_ULTRA_SRC_VAL_ULTRA_RDMA_SRC(val)                                      \
                REG_FLD_VAL(OVL_RDMA_ULTRA_SRC_FLD_ULTRA_RDMA_SRC, (val))
#define OVL_RDMA_ULTRA_SRC_VAL_ULTRA_ROI_END_SRC(val)                                   \
                REG_FLD_VAL(OVL_RDMA_ULTRA_SRC_FLD_ULTRA_ROI_END_SRC, (val))
#define OVL_RDMA_ULTRA_SRC_VAL_ULTRA_SMI_SRC(val)                                       \
                REG_FLD_VAL(OVL_RDMA_ULTRA_SRC_FLD_ULTRA_SMI_SRC, (val))
#define OVL_RDMA_ULTRA_SRC_VAL_ULTRA_BUF_SRC(val)                                       \
                REG_FLD_VAL(OVL_RDMA_ULTRA_SRC_FLD_ULTRA_BUF_SRC, (val))
#define OVL_RDMA_ULTRA_SRC_VAL_PREULTRA_RDMA_SRC(val)                                   \
                REG_FLD_VAL(OVL_RDMA_ULTRA_SRC_FLD_PREULTRA_RDMA_SRC, (val))
#define OVL_RDMA_ULTRA_SRC_VAL_PREULTRA_ROI_END_SRC(val)                                \
                REG_FLD_VAL(OVL_RDMA_ULTRA_SRC_FLD_PREULTRA_ROI_END_SRC, (val))
#define OVL_RDMA_ULTRA_SRC_VAL_PREULTRA_SMI_SRC(val)                                    \
                REG_FLD_VAL(OVL_RDMA_ULTRA_SRC_FLD_PREULTRA_SMI_SRC, (val))
#define OVL_RDMA_ULTRA_SRC_VAL_PREULTRA_BUF_SRC(val)                                    \
                REG_FLD_VAL(OVL_RDMA_ULTRA_SRC_FLD_PREULTRA_BUF_SRC, (val))

#define OVL_RDMA0_BUF_LOW_TH_VAL_RDMA0_PREULTRA_LOW_TH(val)                             \
                REG_FLD_VAL(OVL_RDMA0_BUF_LOW_TH_FLD_RDMA0_PREULTRA_LOW_TH, (val))
#define OVL_RDMA0_BUF_LOW_TH_VAL_RDMA0_ULTRA_LOW_TH(val)                                \
                REG_FLD_VAL(OVL_RDMA0_BUF_LOW_TH_FLD_RDMA0_ULTRA_LOW_TH, (val))

#define OVL_RDMA1_BUF_LOW_TH_VAL_RDMA1_PREULTRA_LOW_TH(val)                             \
                REG_FLD_VAL(OVL_RDMA1_BUF_LOW_TH_FLD_RDMA1_PREULTRA_LOW_TH, (val))
#define OVL_RDMA1_BUF_LOW_TH_VAL_RDMA1_ULTRA_LOW_TH(val)                                \
                REG_FLD_VAL(OVL_RDMA1_BUF_LOW_TH_FLD_RDMA1_ULTRA_LOW_TH, (val))

#define OVL_RDMA2_BUF_LOW_TH_VAL_RDMA2_PREULTRA_LOW_TH(val)                             \
                REG_FLD_VAL(OVL_RDMA2_BUF_LOW_TH_FLD_RDMA2_PREULTRA_LOW_TH, (val))
#define OVL_RDMA2_BUF_LOW_TH_VAL_RDMA2_ULTRA_LOW_TH(val)                                \
                REG_FLD_VAL(OVL_RDMA2_BUF_LOW_TH_FLD_RDMA2_ULTRA_LOW_TH, (val))

#define OVL_RDMA3_BUF_LOW_TH_VAL_RDMA3_PREULTRA_LOW_TH(val)                             \
                REG_FLD_VAL(OVL_RDMA3_BUF_LOW_TH_FLD_RDMA3_PREULTRA_LOW_TH, (val))
#define OVL_RDMA3_BUF_LOW_TH_VAL_RDMA3_ULTRA_LOW_TH(val)                                \
                REG_FLD_VAL(OVL_RDMA3_BUF_LOW_TH_FLD_RDMA3_ULTRA_LOW_TH, (val))

#define OVL_RDMA0_BUF_HIGH_TH_VAL_RDMA0_PREULTRA_HIGH_DIS(val)                          \
                REG_FLD_VAL(OVL_RDMA0_BUF_HIGH_TH_FLD_RDMA0_PREULTRA_HIGH_DIS, (val))
#define OVL_RDMA0_BUF_HIGH_TH_VAL_RDMA0_PREULTRA_HIGH_TH(val)                           \
                REG_FLD_VAL(OVL_RDMA0_BUF_HIGH_TH_FLD_RDMA0_PREULTRA_HIGH_TH, (val))

#define OVL_RDMA1_BUF_HIGH_TH_VAL_RDMA1_PREULTRA_HIGH_DIS(val)                          \
                REG_FLD_VAL(OVL_RDMA1_BUF_HIGH_TH_FLD_RDMA1_PREULTRA_HIGH_DIS, (val))
#define OVL_RDMA1_BUF_HIGH_TH_VAL_RDMA1_PREULTRA_HIGH_TH(val)                           \
                REG_FLD_VAL(OVL_RDMA1_BUF_HIGH_TH_FLD_RDMA1_PREULTRA_HIGH_TH, (val))

#define OVL_RDMA2_BUF_HIGH_TH_VAL_RDMA2_PREULTRA_HIGH_DIS(val)                          \
                REG_FLD_VAL(OVL_RDMA2_BUF_HIGH_TH_FLD_RDMA2_PREULTRA_HIGH_DIS, (val))
#define OVL_RDMA2_BUF_HIGH_TH_VAL_RDMA2_PREULTRA_HIGH_TH(val)                           \
                REG_FLD_VAL(OVL_RDMA2_BUF_HIGH_TH_FLD_RDMA2_PREULTRA_HIGH_TH, (val))

#define OVL_RDMA3_BUF_HIGH_TH_VAL_RDMA3_PREULTRA_HIGH_DIS(val)                          \
                REG_FLD_VAL(OVL_RDMA3_BUF_HIGH_TH_FLD_RDMA3_PREULTRA_HIGH_DIS, (val))
#define OVL_RDMA3_BUF_HIGH_TH_VAL_RDMA3_PREULTRA_HIGH_TH(val)                           \
                REG_FLD_VAL(OVL_RDMA3_BUF_HIGH_TH_FLD_RDMA3_PREULTRA_HIGH_TH, (val))

#define OVL_SMI_DBG_VAL_SMI_FSM(val)            REG_FLD_VAL(OVL_SMI_DBG_FLD_SMI_FSM, (val))

#define OVL_GREQ_LAYER_CNT_VAL_LAYER3_GREQ_CNT(val)                                     \
                REG_FLD_VAL(OVL_GREQ_LAYER_CNT_FLD_LAYER3_GREQ_CNT, (val))
#define OVL_GREQ_LAYER_CNT_VAL_LAYER2_GREQ_CNT(val)                                     \
                REG_FLD_VAL(OVL_GREQ_LAYER_CNT_FLD_LAYER2_GREQ_CNT, (val))
#define OVL_GREQ_LAYER_CNT_VAL_LAYER1_GREQ_CNT(val)                                     \
                REG_FLD_VAL(OVL_GREQ_LAYER_CNT_FLD_LAYER1_GREQ_CNT, (val))
#define OVL_GREQ_LAYER_CNT_VAL_LAYER0_GREQ_CNT(val)                                     \
                REG_FLD_VAL(OVL_GREQ_LAYER_CNT_FLD_LAYER0_GREQ_CNT, (val))

#define OVL_GDRDY_PRD_NUM_VAL_GDRDY_PRD_NUM(val)                                        \
                REG_FLD_VAL(OVL_GDRDY_PRD_NUM_FLD_GDRDY_PRD_NUM, (val))

#define OVL_FLOW_CTRL_DBG_VAL_OVL_UPD_REG(val)                                          \
                REG_FLD_VAL(OVL_FLOW_CTRL_DBG_FLD_OVL_UPD_REG, (val))
#define OVL_FLOW_CTRL_DBG_VAL_REG_UPDATE(val)                                           \
                REG_FLD_VAL(OVL_FLOW_CTRL_DBG_FLD_REG_UPDATE, (val))
#define OVL_FLOW_CTRL_DBG_VAL_OVL_CLR(val)                                              \
                REG_FLD_VAL(OVL_FLOW_CTRL_DBG_FLD_OVL_CLR, (val))
#define OVL_FLOW_CTRL_DBG_VAL_OVL_START(val)                                            \
                REG_FLD_VAL(OVL_FLOW_CTRL_DBG_FLD_OVL_START, (val))
#define OVL_FLOW_CTRL_DBG_VAL_OVL_RUNNING(val)                                          \
                REG_FLD_VAL(OVL_FLOW_CTRL_DBG_FLD_OVL_RUNNING, (val))
#define OVL_FLOW_CTRL_DBG_VAL_FRAME_DONE(val)                                           \
                REG_FLD_VAL(OVL_FLOW_CTRL_DBG_FLD_FRAME_DONE, (val))
#define OVL_FLOW_CTRL_DBG_VAL_FRAME_UNDERRUN(val)                                       \
                REG_FLD_VAL(OVL_FLOW_CTRL_DBG_FLD_FRAME_UNDERRUN, (val))
#define OVL_FLOW_CTRL_DBG_VAL_FRAME_SWRST_DONE(val)                                     \
                REG_FLD_VAL(OVL_FLOW_CTRL_DBG_FLD_FRAME_SWRST_DONE, (val))
#define OVL_FLOW_CTRL_DBG_VAL_FRAME_HWRST_DONE(val)                                     \
                REG_FLD_VAL(OVL_FLOW_CTRL_DBG_FLD_FRAME_HWRST_DONE, (val))
#define OVL_FLOW_CTRL_DBG_VAL_TRIG(val)                                                 \
                REG_FLD_VAL(OVL_FLOW_CTRL_DBG_FLD_TRIG, (val))
#define OVL_FLOW_CTRL_DBG_VAL_RST(val)                                                  \
                REG_FLD_VAL(OVL_FLOW_CTRL_DBG_FLD_RST, (val))
#define OVL_FLOW_CTRL_DBG_VAL_RDMA0_IDLE(val)                                           \
                REG_FLD_VAL(OVL_FLOW_CTRL_DBG_FLD_RDMA0_IDLE, (val))
#define OVL_FLOW_CTRL_DBG_VAL_RDMA1_IDLE(val)                                           \
                REG_FLD_VAL(OVL_FLOW_CTRL_DBG_FLD_RDMA1_IDLE, (val))
#define OVL_FLOW_CTRL_DBG_VAL_RDMA2_IDLE(val)                                           \
                REG_FLD_VAL(OVL_FLOW_CTRL_DBG_FLD_RDMA2_IDLE, (val))
#define OVL_FLOW_CTRL_DBG_VAL_RDMA3_IDLE(val)                                           \
                REG_FLD_VAL(OVL_FLOW_CTRL_DBG_FLD_RDMA3_IDLE, (val))
#define OVL_FLOW_CTRL_DBG_VAL_OUT_IDLE(val)                                             \
                REG_FLD_VAL(OVL_FLOW_CTRL_DBG_FLD_OUT_IDLE, (val))
#define OVL_FLOW_CTRL_DBG_VAL_OVL_OUT_READY(val)                                        \
                REG_FLD_VAL(OVL_FLOW_CTRL_DBG_FLD_OVL_OUT_READY, (val))
#define OVL_FLOW_CTRL_DBG_VAL_OVL_OUT_VALID(val)                                        \
                REG_FLD_VAL(OVL_FLOW_CTRL_DBG_FLD_OVL_OUT_VALID, (val))
#define OVL_FLOW_CTRL_DBG_VAL_BLEND_IDLE(val)                                           \
                REG_FLD_VAL(OVL_FLOW_CTRL_DBG_FLD_BLEND_IDLE, (val))
#define OVL_FLOW_CTRL_DBG_VAL_ADDCON_IDLE(val)                                          \
                REG_FLD_VAL(OVL_FLOW_CTRL_DBG_FLD_ADDCON_IDLE, (val))
#define OVL_FLOW_CTRL_DBG_VAL_FSM_STATE(val)                                            \
                REG_FLD_VAL(OVL_FLOW_CTRL_DBG_FLD_FSM_STATE, (val))

#define OVL_ADDCON_DBG_VAL_L3_WIN_HIT(val)                                              \
                REG_FLD_VAL(OVL_ADDCON_DBG_FLD_L3_WIN_HIT, (val))
#define OVL_ADDCON_DBG_VAL_L2_WIN_HIT(val)                                              \
                REG_FLD_VAL(OVL_ADDCON_DBG_FLD_L2_WIN_HIT, (val))
#define OVL_ADDCON_DBG_VAL_ROI_Y(val)                                                   \
                REG_FLD_VAL(OVL_ADDCON_DBG_FLD_ROI_Y, (val))
#define OVL_ADDCON_DBG_VAL_L1_WIN_HIT(val)                                              \
                REG_FLD_VAL(OVL_ADDCON_DBG_FLD_L1_WIN_HIT, (val))
#define OVL_ADDCON_DBG_VAL_L0_WIN_HIT(val)                                              \
                REG_FLD_VAL(OVL_ADDCON_DBG_FLD_L0_WIN_HIT, (val))
#define OVL_ADDCON_DBG_VAL_ROI_X(val)                                                   \
                REG_FLD_VAL(OVL_ADDCON_DBG_FLD_ROI_X, (val))

#define OVL_RDMA0_DBG_VAL_SMI_GREQ(val)                                                 \
                REG_FLD_VAL(OVL_RDMA0_DBG_FLD_SMI_GREQ, (val))
#define OVL_RDMA0_DBG_VAL_RDMA0_SMI_BUSY(val)                                           \
                REG_FLD_VAL(OVL_RDMA0_DBG_FLD_RDMA0_SMI_BUSY, (val))
#define OVL_RDMA0_DBG_VAL_RDMA0_OUT_VALID(val)                                          \
                REG_FLD_VAL(OVL_RDMA0_DBG_FLD_RDMA0_OUT_VALID, (val))
#define OVL_RDMA0_DBG_VAL_RDMA0_OUT_READY(val)                                          \
                REG_FLD_VAL(OVL_RDMA0_DBG_FLD_RDMA0_OUT_READY, (val))
#define OVL_RDMA0_DBG_VAL_RDMA0_OUT_DATA(val)                                           \
                REG_FLD_VAL(OVL_RDMA0_DBG_FLD_RDMA0_OUT_DATA, (val))
#define OVL_RDMA0_DBG_VAL_RDMA0_LAYER_GREQ(val)                                         \
EG_FLD_VAL(OVL_RDMA0_DBG_FLD_RDMA0_LAYER_GREQ, (val))
#define OVL_RDMA0_DBG_VAL_RDMA0_WRAM_RST_CS(val)                                        \
                REG_FLD_VAL(OVL_RDMA0_DBG_FLD_RDMA0_WRAM_RST_CS, (val))

#define OVL_RDMA1_DBG_VAL_SMI_GREQ(val)                                                 \
                REG_FLD_VAL(OVL_RDMA1_DBG_FLD_SMI_GREQ, (val))
#define OVL_RDMA1_DBG_VAL_RDMA1_SMI_BUSY(val)                                           \
                REG_FLD_VAL(OVL_RDMA1_DBG_FLD_RDMA1_SMI_BUSY, (val))
#define OVL_RDMA1_DBG_VAL_RDMA1_OUT_VALID(val)                                          \
                REG_FLD_VAL(OVL_RDMA1_DBG_FLD_RDMA1_OUT_VALID, (val))
#define OVL_RDMA1_DBG_VAL_RDMA1_OUT_READY(val)                                          \
                REG_FLD_VAL(OVL_RDMA1_DBG_FLD_RDMA1_OUT_READY, (val))
#define OVL_RDMA1_DBG_VAL_RDMA1_OUT_DATA(val)                                           \
                REG_FLD_VAL(OVL_RDMA1_DBG_FLD_RDMA1_OUT_DATA, (val))
#define OVL_RDMA1_DBG_VAL_RDMA1_LAYER_GREQ(val)                                         \
                REG_FLD_VAL(OVL_RDMA1_DBG_FLD_RDMA1_LAYER_GREQ, (val))
#define OVL_RDMA1_DBG_VAL_RDMA1_WRAM_RST_CS(val)                                        \
                REG_FLD_VAL(OVL_RDMA1_DBG_FLD_RDMA1_WRAM_RST_CS, (val))

#define OVL_RDMA2_DBG_VAL_SMI_GREQ(val)                                                 \
                REG_FLD_VAL(OVL_RDMA2_DBG_FLD_SMI_GREQ, (val))
#define OVL_RDMA2_DBG_VAL_RDMA2_SMI_BUSY(val)                                           \
                REG_FLD_VAL(OVL_RDMA2_DBG_FLD_RDMA2_SMI_BUSY, (val))
#define OVL_RDMA2_DBG_VAL_RDMA2_OUT_VALID(val)                                          \
                REG_FLD_VAL(OVL_RDMA2_DBG_FLD_RDMA2_OUT_VALID, (val))
#define OVL_RDMA2_DBG_VAL_RDMA2_OUT_READY(val)                                          \
                     REG_FLD_VAL(OVL_RDMA2_DBG_FLD_RDMA2_OUT_READY, (val))
#define OVL_RDMA2_DBG_VAL_RDMA2_OUT_DATA(val)                                           \
                REG_FLD_VAL(OVL_RDMA2_DBG_FLD_RDMA2_OUT_DATA, (val))
#define OVL_RDMA2_DBG_VAL_RDMA2_LAYER_GREQ(val)                                         \
                REG_FLD_VAL(OVL_RDMA2_DBG_FLD_RDMA2_LAYER_GREQ, (val))
#define OVL_RDMA2_DBG_VAL_RDMA2_WRAM_RST_CS(val)                                        \
                REG_FLD_VAL(OVL_RDMA2_DBG_FLD_RDMA2_WRAM_RST_CS, (val))

#define OVL_RDMA3_DBG_VAL_SMI_GREQ(val)                                                 \
                REG_FLD_VAL(OVL_RDMA3_DBG_FLD_SMI_GREQ, (val))
#define OVL_RDMA3_DBG_VAL_RDMA3_SMI_BUSY(val)                                           \
                REG_FLD_VAL(OVL_RDMA3_DBG_FLD_RDMA3_SMI_BUSY, (val))
#define OVL_RDMA3_DBG_VAL_RDMA3_OUT_VALID(val)                                          \
                REG_FLD_VAL(OVL_RDMA3_DBG_FLD_RDMA3_OUT_VALID, (val))
#define OVL_RDMA3_DBG_VAL_RDMA3_OUT_READY(val)                                          \
                REG_FLD_VAL(OVL_RDMA3_DBG_FLD_RDMA3_OUT_READY, (val))
#define OVL_RDMA3_DBG_VAL_RDMA3_OUT_DATA(val)                                           \
                REG_FLD_VAL(OVL_RDMA3_DBG_FLD_RDMA3_OUT_DATA, (val))
#define OVL_RDMA3_DBG_VAL_RDMA3_LAYER_GREQ(val)                                         \
                REG_FLD_VAL(OVL_RDMA3_DBG_FLD_RDMA3_LAYER_GREQ, (val))
#define OVL_RDMA3_DBG_VAL_RDMA3_WRAM_RST_CS(val)                                        \
                REG_FLD_VAL(OVL_RDMA3_DBG_FLD_RDMA3_WRAM_RST_CS, (val))

#define OVL_L0_CLR_VAL_ALPHA(val)               REG_FLD_VAL(OVL_L0_CLR_FLD_ALPHA, (val))
#define OVL_L0_CLR_VAL_RED(val)                 REG_FLD_VAL(OVL_L0_CLR_FLD_RED, (val))
#define OVL_L0_CLR_VAL_GREEN(val)               REG_FLD_VAL(OVL_L0_CLR_FLD_GREEN, (val))
#define OVL_L0_CLR_VAL_BLUE(val)                REG_FLD_VAL(OVL_L0_CLR_FLD_BLUE, (val))

#define OVL_L1_CLR_VAL_ALPHA(val)               REG_FLD_VAL(OVL_L1_CLR_FLD_ALPHA, (val))
#define OVL_L1_CLR_VAL_RED(val)                 REG_FLD_VAL(OVL_L1_CLR_FLD_RED, (val))
#define OVL_L1_CLR_VAL_GREEN(val)               REG_FLD_VAL(OVL_L1_CLR_FLD_GREEN, (val))
#define OVL_L1_CLR_VAL_BLUE(val)                REG_FLD_VAL(OVL_L1_CLR_FLD_BLUE, (val))

#define OVL_L2_CLR_VAL_ALPHA(val)               REG_FLD_VAL(OVL_L2_CLR_FLD_ALPHA, (val))
#define OVL_L2_CLR_VAL_RED(val)                 REG_FLD_VAL(OVL_L2_CLR_FLD_RED, (val))
#define OVL_L2_CLR_VAL_GREEN(val)               REG_FLD_VAL(OVL_L2_CLR_FLD_GREEN, (val))
#define OVL_L2_CLR_VAL_BLUE(val)                REG_FLD_VAL(OVL_L2_CLR_FLD_BLUE, (val))

#define OVL_L3_CLR_VAL_ALPHA(val)               REG_FLD_VAL(OVL_L3_CLR_FLD_ALPHA, (val))
#define OVL_L3_CLR_VAL_RED(val)                 REG_FLD_VAL(OVL_L3_CLR_FLD_RED, (val))
#define OVL_L3_CLR_VAL_GREEN(val)               REG_FLD_VAL(OVL_L3_CLR_FLD_GREEN, (val))
#define OVL_L3_CLR_VAL_BLUE(val)                REG_FLD_VAL(OVL_L3_CLR_FLD_BLUE, (val))

#define OVL_LC_CLR_VAL_ALPHA(val)               REG_FLD_VAL(OVL_LC_CLR_FLD_ALPHA, (val))
#define OVL_LC_CLR_VAL_RED(val)                 REG_FLD_VAL(OVL_LC_CLR_FLD_RED, (val))
#define OVL_LC_CLR_VAL_GREEN(val)               REG_FLD_VAL(OVL_LC_CLR_FLD_GREEN, (val))
#define OVL_LC_CLR_VAL_BLUE(val)                REG_FLD_VAL(OVL_LC_CLR_FLD_BLUE, (val))

#define OVL_CRC_VAL_CRC_RDY(val)                REG_FLD_VAL(OVL_CRC_FLD_CRC_RDY, (val))
#define OVL_CRC_VAL_CRC_OUT(val)                REG_FLD_VAL(OVL_CRC_FLD_CRC_OUT, (val))

#define OVL_LC_CON_VAL_DSTKEY_EN(val)           REG_FLD_VAL(OVL_LC_CON_FLD_DSTKEY_EN, (val))
#define OVL_LC_CON_VAL_SRCKEY_EN(val)           REG_FLD_VAL(OVL_LC_CON_FLD_SRCKEY_EN, (val))
#define OVL_LC_CON_VAL_LAYER_SRC(val)           REG_FLD_VAL(OVL_LC_CON_FLD_LAYER_SRC, (val))
#define OVL_LC_CON_VAL_R_FIRST(val)             REG_FLD_VAL(OVL_LC_CON_FLD_R_FIRST, (val))
#define OVL_LC_CON_VAL_LANDSCAPE(val)           REG_FLD_VAL(OVL_LC_CON_FLD_LANDSCAPE, (val))
#define OVL_LC_CON_VAL_EN_3D(val)               REG_FLD_VAL(OVL_LC_CON_FLD_EN_3D, (val))
#define OVL_LC_CON_VAL_ALPHA_EN(val)            REG_FLD_VAL(OVL_LC_CON_FLD_ALPHA_EN, (val))
#define OVL_LC_CON_VAL_ALPHA(val)               REG_FLD_VAL(OVL_LC_CON_FLD_ALPHA, (val))

#define OVL_LC_SRCKEY_VAL_SRCKEY(val)           REG_FLD_VAL(OVL_LC_SRCKEY_FLD_SRCKEY, (val))

#define OVL_LC_SRC_SIZE_VAL_LC_SRC_H(val)       REG_FLD_VAL(OVL_LC_SRC_SIZE_FLD_LC_SRC_H, (val))
#define OVL_LC_SRC_SIZE_VAL_LC_SRC_W(val)       REG_FLD_VAL(OVL_LC_SRC_SIZE_FLD_LC_SRC_W, (val))

#define OVL_LC_OFFSET_VAL_LC_YOFF(val)          REG_FLD_VAL(OVL_LC_OFFSET_FLD_LC_YOFF, (val))
#define OVL_LC_OFFSET_VAL_LC_XOFF(val)          REG_FLD_VAL(OVL_LC_OFFSET_FLD_LC_XOFF, (val))

#define OVL_LC_SRC_SEL_VAL_SURFL_EN(val)                                                \
                REG_FLD_VAL(OVL_LC_SRC_SEL_FLD_SURFL_EN, (val))
#define OVL_LC_SRC_SEL_VAL_LC_BLEND_RND_SHT(val)                                        \
                REG_FLD_VAL(OVL_LC_SRC_SEL_FLD_LC_BLEND_RND_SHT, (val))
#define OVL_LC_SRC_SEL_VAL_LC_SRGB_SEL_EXT2(val)                                        \
                REG_FLD_VAL(OVL_LC_SRC_SEL_FLD_LC_SRGB_SEL_EXT2, (val))
#define OVL_LC_SRC_SEL_VAL_LC_CONST_BLD(val)                                            \
                REG_FLD_VAL(OVL_LC_SRC_SEL_FLD_LC_CONST_BLD, (val))
#define OVL_LC_SRC_SEL_VAL_LC_DRGB_SEL_EXT(val)                                         \
                REG_FLD_VAL(OVL_LC_SRC_SEL_FLD_LC_DRGB_SEL_EXT, (val))
#define OVL_LC_SRC_SEL_VAL_LC_DA_SEL_EXT(val)                                           \
                REG_FLD_VAL(OVL_LC_SRC_SEL_FLD_LC_DA_SEL_EXT, (val))
#define OVL_LC_SRC_SEL_VAL_LC_SRGB_SEL_EXT(val)                                         \
                REG_FLD_VAL(OVL_LC_SRC_SEL_FLD_LC_SRGB_SEL_EXT, (val))
#define OVL_LC_SRC_SEL_VAL_LC_SA_SEL_EXT(val)                                           \
                REG_FLD_VAL(OVL_LC_SRC_SEL_FLD_LC_SA_SEL_EXT, (val))
#define OVL_LC_SRC_SEL_VAL_LC_DRGB_SEL(val)                                             \
                REG_FLD_VAL(OVL_LC_SRC_SEL_FLD_LC_DRGB_SEL, (val))
#define OVL_LC_SRC_SEL_VAL_LC_DA_SEL(val)                                               \
                REG_FLD_VAL(OVL_LC_SRC_SEL_FLD_LC_DA_SEL, (val))
#define OVL_LC_SRC_SEL_VAL_LC_SRGB_SEL(val)                                             \
                REG_FLD_VAL(OVL_LC_SRC_SEL_FLD_LC_SRGB_SEL, (val))
#define OVL_LC_SRC_SEL_VAL_LC_SA_SEL(val)                                               \
                REG_FLD_VAL(OVL_LC_SRC_SEL_FLD_LC_SA_SEL, (val))
#define OVL_LC_SRC_SEL_VAL_CONST_LAYER_SEL(val)                                         \
                REG_FLD_VAL(OVL_LC_SRC_SEL_FLD_CONST_LAYER_SEL, (val))

#define OVL_BANK_CON_VAL_OVL_BANK_CON(val)      REG_FLD_VAL(OVL_BANK_CON_FLD_OVL_BANK_CON, (val))

#define OVL_FUNC_DCM0_VAL_OVL_FUNC_DCM0(val)    REG_FLD_VAL(OVL_FUNC_DCM0_FLD_OVL_FUNC_DCM0, (val))
#define OVL_FUNC_DCM1_VAL_OVL_FUNC_DCM1(val)    REG_FLD_VAL(OVL_FUNC_DCM1_FLD_OVL_FUNC_DCM1, (val))

#define OVL_DVFS_L0_ROI_VAL_L0_DVFS_ROI_BB(val)                                         \
                REG_FLD_VAL(OVL_DVFS_L0_ROI_FLD_L0_DVFS_ROI_BB, (val))
#define OVL_DVFS_L0_ROI_VAL_L0_DVFS_ROI_TB(val)                                         \
                REG_FLD_VAL(OVL_DVFS_L0_ROI_FLD_L0_DVFS_ROI_TB, (val))

#define OVL_DVFS_L1_ROI_VAL_L1_DVFS_ROI_BB(val)                                         \
                REG_FLD_VAL(OVL_DVFS_L1_ROI_FLD_L1_DVFS_ROI_BB, (val))
#define OVL_DVFS_L1_ROI_VAL_L1_DVFS_ROI_TB(val)                                         \
                REG_FLD_VAL(OVL_DVFS_L1_ROI_FLD_L1_DVFS_ROI_TB, (val))

#define OVL_DVFS_L2_ROI_VAL_L2_DVFS_ROI_BB(val)                                         \
                REG_FLD_VAL(OVL_DVFS_L2_ROI_FLD_L2_DVFS_ROI_BB, (val))
#define OVL_DVFS_L2_ROI_VAL_L2_DVFS_ROI_TB(val)                                         \
                REG_FLD_VAL(OVL_DVFS_L2_ROI_FLD_L2_DVFS_ROI_TB, (val))

#define OVL_DVFS_L3_ROI_VAL_L3_DVFS_ROI_BB(val)                                         \
                REG_FLD_VAL(OVL_DVFS_L3_ROI_FLD_L3_DVFS_ROI_BB, (val))
#define OVL_DVFS_L3_ROI_VAL_L3_DVFS_ROI_TB(val)                                         \
                REG_FLD_VAL(OVL_DVFS_L3_ROI_FLD_L3_DVFS_ROI_TB, (val))

#define OVL_DVFS_EL0_ROI_VAL_EL0_DVFS_ROI_BB(val)                                       \
                     REG_FLD_VAL(OVL_DVFS_EL0_ROI_FLD_EL0_DVFS_ROI_BB, (val))
#define OVL_DVFS_EL0_ROI_VAL_EL0_DVFS_ROI_TB(val)                                       \
                REG_FLD_VAL(OVL_DVFS_EL0_ROI_FLD_EL0_DVFS_ROI_TB, (val))

#define OVL_DVFS_EL1_ROI_VAL_EL1_DVFS_ROI_BB(val)                                       \
                REG_FLD_VAL(OVL_DVFS_EL1_ROI_FLD_EL1_DVFS_ROI_BB, (val))
#define OVL_DVFS_EL1_ROI_VAL_EL1_DVFS_ROI_TB(val)                                       \
                REG_FLD_VAL(OVL_DVFS_EL1_ROI_FLD_EL1_DVFS_ROI_TB, (val))

#define OVL_DVFS_EL2_ROI_VAL_EL2_DVFS_ROI_BB(val)                                       \
                     REG_FLD_VAL(OVL_DVFS_EL2_ROI_FLD_EL2_DVFS_ROI_BB, (val))
#define OVL_DVFS_EL2_ROI_VAL_EL2_DVFS_ROI_TB(val)                                       \
                REG_FLD_VAL(OVL_DVFS_EL2_ROI_FLD_EL2_DVFS_ROI_TB, (val))

#define OVL_DATAPATH_EXT_CON_VAL_EL2_LAYER_SEL(val)                                     \
                REG_FLD_VAL(OVL_DATAPATH_EXT_CON_FLD_EL2_LAYER_SEL, (val))
#define OVL_DATAPATH_EXT_CON_VAL_EL1_LAYER_SEL(val)                                     \
                REG_FLD_VAL(OVL_DATAPATH_EXT_CON_FLD_EL1_LAYER_SEL, (val))
#define OVL_DATAPATH_EXT_CON_VAL_EL0_LAYER_SEL(val)                                     \
                REG_FLD_VAL(OVL_DATAPATH_EXT_CON_FLD_EL0_LAYER_SEL, (val))
#define OVL_DATAPATH_EXT_CON_VAL_EL2_GPU_MODE(val)                                      \
                REG_FLD_VAL(OVL_DATAPATH_EXT_CON_FLD_EL2_GPU_MODE, (val))
#define OVL_DATAPATH_EXT_CON_VAL_EL1_GPU_MODE(val)                                      \
                REG_FLD_VAL(OVL_DATAPATH_EXT_CON_FLD_EL1_GPU_MODE, (val))
#define OVL_DATAPATH_EXT_CON_VAL_EL0_GPU_MODE(val)                                      \
                REG_FLD_VAL(OVL_DATAPATH_EXT_CON_FLD_EL0_GPU_MODE, (val))
#define OVL_DATAPATH_EXT_CON_VAL_EL2_EN(val)    REG_FLD_VAL(OVL_DATAPATH_EXT_CON_FLD_EL2_EN, (val))
#define OVL_DATAPATH_EXT_CON_VAL_EL1_EN(val)    REG_FLD_VAL(OVL_DATAPATH_EXT_CON_FLD_EL1_EN, (val))
#define OVL_DATAPATH_EXT_CON_VAL_EL0_EN(val)    REG_FLD_VAL(OVL_DATAPATH_EXT_CON_FLD_EL0_EN, (val))

#define OVL_EL0_CON_VAL_DSTKEY_EN(val)           REG_FLD_VAL(OVL_EL0_CON_FLD_DSTKEY_EN, (val))
#define OVL_EL0_CON_VAL_SRCKEY_EN(val)           REG_FLD_VAL(OVL_EL0_CON_FLD_SRCKEY_EN, (val))
#define OVL_EL0_CON_VAL_LAYER_SRC(val)           REG_FLD_VAL(OVL_EL0_CON_FLD_LAYER_SRC, (val))
#define OVL_EL0_CON_VAL_MTX_EN(val)              REG_FLD_VAL(OVL_EL0_CON_FLD_MTX_EN, (val))
#define OVL_EL0_CON_VAL_MTX_AUTO_DIS(val)        REG_FLD_VAL(OVL_EL0_CON_FLD_MTX_AUTO_DIS, (val))
#define OVL_EL0_CON_VAL_RGB_SWAP(val)            REG_FLD_VAL(OVL_EL0_CON_FLD_RGB_SWAP, (val))
#define OVL_EL0_CON_VAL_BYTE_SWAP(val)           REG_FLD_VAL(OVL_EL0_CON_FLD_BYTE_SWAP, (val))
#define OVL_EL0_CON_VAL_CLRFMT_MAN(val)          REG_FLD_VAL(OVL_EL0_CON_FLD_CLRFMT_MAN, (val))
#define OVL_EL0_CON_VAL_R_FIRST(val)             REG_FLD_VAL(OVL_EL0_CON_FLD_R_FIRST, (val))
#define OVL_EL0_CON_VAL_LANDSCAPE(val)           REG_FLD_VAL(OVL_EL0_CON_FLD_LANDSCAPE, (val))
#define OVL_EL0_CON_VAL_EN_3D(val)               REG_FLD_VAL(OVL_EL0_CON_FLD_EN_3D, (val))
#define OVL_EL0_CON_VAL_INT_MTX_SEL(val)         REG_FLD_VAL(OVL_EL0_CON_FLD_INT_MTX_SEL, (val))
#define OVL_EL0_CON_VAL_CLRFMT(val)              REG_FLD_VAL(OVL_EL0_CON_FLD_CLRFMT, (val))
#define OVL_EL0_CON_VAL_EXT_MTX_EN(val)          REG_FLD_VAL(OVL_EL0_CON_FLD_EXT_MTX_EN, (val))
#define OVL_EL0_CON_VAL_HORIZONTAL_FLIP_EN(val)                                         \
                REG_FLD_VAL(OVL_EL0_CON_FLD_HORIZONTAL_FLIP_EN, (val))
#define OVL_EL0_CON_VAL_VERTICAL_FLIP_EN(val)                                           \
                REG_FLD_VAL(OVL_EL0_CON_FLD_VERTICAL_FLIP_EN, (val))
#define OVL_EL0_CON_VAL_ALPHA_EN(val)            REG_FLD_VAL(OVL_EL0_CON_FLD_ALPHA_EN, (val))
#define OVL_EL0_CON_VAL_ALPHA(val)               REG_FLD_VAL(OVL_EL0_CON_FLD_ALPHA, (val))

#define OVL_EL0_SRCKEY_VAL_SRCKEY(val)          REG_FLD_VAL(OVL_EL0_SRCKEY_FLD_SRCKEY, (val))

#define OVL_EL0_SRC_SIZE_VAL_EL0_SRC_H(val)     REG_FLD_VAL(OVL_EL0_SRC_SIZE_FLD_EL0_SRC_H, (val))
#define OVL_EL0_SRC_SIZE_VAL_EL0_SRC_W(val)     REG_FLD_VAL(OVL_EL0_SRC_SIZE_FLD_EL0_SRC_W, (val))

#define OVL_EL0_OFFSET_VAL_EL0_YOFF(val)        REG_FLD_VAL(OVL_EL0_OFFSET_FLD_EL0_YOFF, (val))
#define OVL_EL0_OFFSET_VAL_EL0_XOFF(val)        REG_FLD_VAL(OVL_EL0_OFFSET_FLD_EL0_XOFF, (val))

#define OVL_EL0_PITCH_VAL_SURFL_EN(val)         REG_FLD_VAL(OVL_EL0_PITCH_FLD_SURFL_EN, (val))
#define OVL_EL0_PITCH_VAL_EL0_BLEND_RND_SHT(val)                                        \
                REG_FLD_VAL(OVL_EL0_PITCH_FLD_EL0_BLEND_RND_SHT, (val))
#define OVL_EL0_PITCH_VAL_EL0_SRGB_SEL_EXT2(val)                                        \
                REG_FLD_VAL(OVL_EL0_PITCH_FLD_EL0_SRGB_SEL_EXT2, (val))
#define OVL_EL0_PITCH_VAL_EL0_CONST_BLD(val)    REG_FLD_VAL(OVL_EL0_PITCH_FLD_EL0_CONST_BLD, (val))
#define OVL_EL0_PITCH_VAL_EL0_DRGB_SEL_EXT(val)                                         \
                REG_FLD_VAL(OVL_EL0_PITCH_FLD_EL0_DRGB_SEL_EXT, (val))
#define OVL_EL0_PITCH_VAL_EL0_DA_SEL_EXT(val)                                           \
                REG_FLD_VAL(OVL_EL0_PITCH_FLD_EL0_DA_SEL_EXT, (val))
#define OVL_EL0_PITCH_VAL_EL0_SRGB_SEL_EXT(val)                                         \
                REG_FLD_VAL(OVL_EL0_PITCH_FLD_EL0_SRGB_SEL_EXT, (val))
#define OVL_EL0_PITCH_VAL_EL0_SA_SEL_EXT(val)                                           \
                REG_FLD_VAL(OVL_EL0_PITCH_FLD_EL0_SA_SEL_EXT, (val))
#define OVL_EL0_PITCH_VAL_EL0_DRGB_SEL(val)     REG_FLD_VAL(OVL_EL0_PITCH_FLD_EL0_DRGB_SEL, (val))
#define OVL_EL0_PITCH_VAL_EL0_DA_SEL(val)       REG_FLD_VAL(OVL_EL0_PITCH_FLD_EL0_DA_SEL, (val))
#define OVL_EL0_PITCH_VAL_EL0_SRGB_SEL(val)     REG_FLD_VAL(OVL_EL0_PITCH_FLD_EL0_SRGB_SEL, (val))
#define OVL_EL0_PITCH_VAL_EL0_SA_SEL(val)       REG_FLD_VAL(OVL_EL0_PITCH_FLD_EL0_SA_SEL, (val))
#define OVL_EL0_PITCH_VAL_EL0_SRC_PITCH(val)    REG_FLD_VAL(OVL_EL0_PITCH_FLD_EL0_SRC_PITCH, (val))

#define OVL_EL0_TILE_VAL_TILE_HORI_BLOCK_NUM(val)                                      \
                REG_FLD_VAL(OVL_EL0_TILE_FLD_TILE_HORI_BLOCK_NUM, (val))
#define OVL_EL0_TILE_VAL_TILE_EN(val)           REG_FLD_VAL(OVL_EL0_TILE_FLD_TILE_EN, (val))
#define OVL_EL0_TILE_VAL_TILE_WIDTH_SEL(val)    REG_FLD_VAL(OVL_EL0_TILE_FLD_TILE_WIDTH_SEL, (val))
#define OVL_EL0_TILE_VAL_TILE_HEIGHT(val)       REG_FLD_VAL(OVL_EL0_TILE_FLD_TILE_HEIGHT, (val))

#define OVL_EL0_CLIP_VAL_EL0_SRC_BOTTOM_CLIP(val)                                      \
                REG_FLD_VAL(OVL_EL0_CLIP_FLD_EL0_SRC_BOTTOM_CLIP, (val))
#define OVL_EL0_CLIP_VAL_EL0_SRC_TOP_CLIP(val)                                         \
                REG_FLD_VAL(OVL_EL0_CLIP_FLD_EL0_SRC_TOP_CLIP, (val))
#define OVL_EL0_CLIP_VAL_EL0_SRC_RIGHT_CLIP(val)                                       \
                REG_FLD_VAL(OVL_EL0_CLIP_FLD_EL0_SRC_RIGHT_CLIP, (val))
#define OVL_EL0_CLIP_VAL_EL0_SRC_LEFT_CLIP(val)                                        \
                REG_FLD_VAL(OVL_EL0_CLIP_FLD_EL0_SRC_LEFT_CLIP, (val))

#define OVL_EL1_CON_VAL_DSTKEY_EN(val)          REG_FLD_VAL(OVL_EL1_CON_FLD_DSTKEY_EN, (val))
#define OVL_EL1_CON_VAL_SRCKEY_EN(val)          REG_FLD_VAL(OVL_EL1_CON_FLD_SRCKEY_EN, (val))
#define OVL_EL1_CON_VAL_LAYER_SRC(val)          REG_FLD_VAL(OVL_EL1_CON_FLD_LAYER_SRC, (val))
#define OVL_EL1_CON_VAL_MTX_EN(val)             REG_FLD_VAL(OVL_EL1_CON_FLD_MTX_EN, (val))
#define OVL_EL1_CON_VAL_MTX_AUTO_DIS(val)       REG_FLD_VAL(OVL_EL1_CON_FLD_MTX_AUTO_DIS, (val))
#define OVL_EL1_CON_VAL_RGB_SWAP(val)           REG_FLD_VAL(OVL_EL1_CON_FLD_RGB_SWAP, (val))
#define OVL_EL1_CON_VAL_BYTE_SWAP(val)          REG_FLD_VAL(OVL_EL1_CON_FLD_BYTE_SWAP, (val))
#define OVL_EL1_CON_VAL_CLRFMT_MAN(val)         REG_FLD_VAL(OVL_EL1_CON_FLD_CLRFMT_MAN, (val))
#define OVL_EL1_CON_VAL_R_FIRST(val)            REG_FLD_VAL(OVL_EL1_CON_FLD_R_FIRST, (val))
#define OVL_EL1_CON_VAL_LANDSCAPE(val)          REG_FLD_VAL(OVL_EL1_CON_FLD_LANDSCAPE, (val))
#define OVL_EL1_CON_VAL_EN_3D(val)              REG_FLD_VAL(OVL_EL1_CON_FLD_EN_3D, (val))
#define OVL_EL1_CON_VAL_INT_MTX_SEL(val)        REG_FLD_VAL(OVL_EL1_CON_FLD_INT_MTX_SEL, (val))
#define OVL_EL1_CON_VAL_CLRFMT(val)             REG_FLD_VAL(OVL_EL1_CON_FLD_CLRFMT, (val))
#define OVL_EL1_CON_VAL_EXT_MTX_EN(val)         REG_FLD_VAL(OVL_EL1_CON_FLD_EXT_MTX_EN, (val))
#define OVL_EL1_CON_VAL_HORIZONTAL_FLIP_EN(val)                                         \
                REG_FLD_VAL(OVL_EL1_CON_FLD_HORIZONTAL_FLIP_EN, (val))
#define OVL_EL1_CON_VAL_VERTICAL_FLIP_EN(val)   REG_FLD_VAL(OVL_EL1_CON_FLD_VERTICAL_FLIP_EN, (val))
#define OVL_EL1_CON_VAL_ALPHA_EN(val)           REG_FLD_VAL(OVL_EL1_CON_FLD_ALPHA_EN, (val))
#define OVL_EL1_CON_VAL_ALPHA(val)              REG_FLD_VAL(OVL_EL1_CON_FLD_ALPHA, (val))

#define OVL_EL1_SRCKEY_VAL_SRCKEY(val)          REG_FLD_VAL(OVL_EL1_SRCKEY_FLD_SRCKEY, (val))

#define OVL_EL1_SRC_SIZE_VAL_EL1_SRC_H(val)     REG_FLD_VAL(OVL_EL1_SRC_SIZE_FLD_EL1_SRC_H, (val))
#define OVL_EL1_SRC_SIZE_VAL_EL1_SRC_W(val)     REG_FLD_VAL(OVL_EL1_SRC_SIZE_FLD_EL1_SRC_W, (val))

#define OVL_EL1_OFFSET_VAL_EL1_YOFF(val)        REG_FLD_VAL(OVL_EL1_OFFSET_FLD_EL1_YOFF, (val))
#define OVL_EL1_OFFSET_VAL_EL1_XOFF(val)        REG_FLD_VAL(OVL_EL1_OFFSET_FLD_EL1_XOFF, (val))

#define OVL_EL1_PITCH_VAL_SURFL_EN(val)                                                 \
                REG_FLD_VAL(OVL_EL1_PITCH_FLD_SURFL_EN, (val))
#define OVL_EL1_PITCH_VAL_EL1_BLEND_RND_SHT(val)                                        \
                REG_FLD_VAL(OVL_EL1_PITCH_FLD_EL1_BLEND_RND_SHT, (val))
#define OVL_EL1_PITCH_VAL_EL1_SRGB_SEL_EXT2(val)                                        \
                REG_FLD_VAL(OVL_EL1_PITCH_FLD_EL1_SRGB_SEL_EXT2, (val))
#define OVL_EL1_PITCH_VAL_EL1_CONST_BLD(val)                                            \
                REG_FLD_VAL(OVL_EL1_PITCH_FLD_EL1_CONST_BLD, (val))
#define OVL_EL1_PITCH_VAL_EL1_DRGB_SEL_EXT(val)                                         \
                REG_FLD_VAL(OVL_EL1_PITCH_FLD_EL1_DRGB_SEL_EXT, (val))
#define OVL_EL1_PITCH_VAL_EL1_DA_SEL_EXT(val)                                           \
                REG_FLD_VAL(OVL_EL1_PITCH_FLD_EL1_DA_SEL_EXT, (val))
#define OVL_EL1_PITCH_VAL_EL1_SRGB_SEL_EXT(val)                                         \
                REG_FLD_VAL(OVL_EL1_PITCH_FLD_EL1_SRGB_SEL_EXT, (val))
#define OVL_EL1_PITCH_VAL_EL1_SA_SEL_EXT(val)                                           \
                REG_FLD_VAL(OVL_EL1_PITCH_FLD_EL1_SA_SEL_EXT, (val))
#define OVL_EL1_PITCH_VAL_EL1_DRGB_SEL(val)                                             \
                REG_FLD_VAL(OVL_EL1_PITCH_FLD_EL1_DRGB_SEL, (val))
#define OVL_EL1_PITCH_VAL_EL1_DA_SEL(val)                                               \
                REG_FLD_VAL(OVL_EL1_PITCH_FLD_EL1_DA_SEL, (val))
#define OVL_EL1_PITCH_VAL_EL1_SRGB_SEL(val)                                             \
                REG_FLD_VAL(OVL_EL1_PITCH_FLD_EL1_SRGB_SEL, (val))
#define OVL_EL1_PITCH_VAL_EL1_SA_SEL(val)                                               \
                REG_FLD_VAL(OVL_EL1_PITCH_FLD_EL1_SA_SEL, (val))
#define OVL_EL1_PITCH_VAL_EL1_SRC_PITCH(val)                                            \
                REG_FLD_VAL(OVL_EL1_PITCH_FLD_EL1_SRC_PITCH, (val))

#define OVL_EL1_TILE_VAL_TILE_HORI_BLOCK_NUM(val)                                       \
                REG_FLD_VAL(OVL_EL1_TILE_FLD_TILE_HORI_BLOCK_NUM, (val))
#define OVL_EL1_TILE_VAL_TILE_EN(val)           REG_FLD_VAL(OVL_EL1_TILE_FLD_TILE_EN, (val))
#define OVL_EL1_TILE_VAL_TILE_WIDTH_SEL(val)    REG_FLD_VAL(OVL_EL1_TILE_FLD_TILE_WIDTH_SEL, (val))
#define OVL_EL1_TILE_VAL_TILE_HEIGHT(val)       REG_FLD_VAL(OVL_EL1_TILE_FLD_TILE_HEIGHT, (val))

#define OVL_EL1_CLIP_VAL_EL1_SRC_BOTTOM_CLIP(val)                                       \
                REG_FLD_VAL(OVL_EL1_CLIP_FLD_EL1_SRC_BOTTOM_CLIP, (val))
#define OVL_EL1_CLIP_VAL_EL1_SRC_TOP_CLIP(val)                                          \
                REG_FLD_VAL(OVL_EL1_CLIP_FLD_EL1_SRC_TOP_CLIP, (val))
#define OVL_EL1_CLIP_VAL_EL1_SRC_RIGHT_CLIP(val)                                        \
                REG_FLD_VAL(OVL_EL1_CLIP_FLD_EL1_SRC_RIGHT_CLIP, (val))
#define OVL_EL1_CLIP_VAL_EL1_SRC_LEFT_CLIP(val)                                         \
                REG_FLD_VAL(OVL_EL1_CLIP_FLD_EL1_SRC_LEFT_CLIP, (val))

#define OVL_EL2_CON_VAL_DSTKEY_EN(val)             REG_FLD_VAL(OVL_EL2_CON_FLD_DSTKEY_EN, (val))
#define OVL_EL2_CON_VAL_SRCKEY_EN(val)             REG_FLD_VAL(OVL_EL2_CON_FLD_SRCKEY_EN, (val))
#define OVL_EL2_CON_VAL_LAYER_SRC(val)             REG_FLD_VAL(OVL_EL2_CON_FLD_LAYER_SRC, (val))
#define OVL_EL2_CON_VAL_MTX_EN(val)                REG_FLD_VAL(OVL_EL2_CON_FLD_MTX_EN, (val))
#define OVL_EL2_CON_VAL_MTX_AUTO_DIS(val)          REG_FLD_VAL(OVL_EL2_CON_FLD_MTX_AUTO_DIS, (val))
#define OVL_EL2_CON_VAL_RGB_SWAP(val)              REG_FLD_VAL(OVL_EL2_CON_FLD_RGB_SWAP, (val))
#define OVL_EL2_CON_VAL_BYTE_SWAP(val)             REG_FLD_VAL(OVL_EL2_CON_FLD_BYTE_SWAP, (val))
#define OVL_EL2_CON_VAL_CLRFMT_MAN(val)            REG_FLD_VAL(OVL_EL2_CON_FLD_CLRFMT_MAN, (val))
#define OVL_EL2_CON_VAL_R_FIRST(val)               REG_FLD_VAL(OVL_EL2_CON_FLD_R_FIRST, (val))
#define OVL_EL2_CON_VAL_LANDSCAPE(val)             REG_FLD_VAL(OVL_EL2_CON_FLD_LANDSCAPE, (val))
#define OVL_EL2_CON_VAL_EN_3D(val)                 REG_FLD_VAL(OVL_EL2_CON_FLD_EN_3D, (val))
#define OVL_EL2_CON_VAL_INT_MTX_SEL(val)           REG_FLD_VAL(OVL_EL2_CON_FLD_INT_MTX_SEL, (val))
#define OVL_EL2_CON_VAL_CLRFMT(val)                REG_FLD_VAL(OVL_EL2_CON_FLD_CLRFMT, (val))
#define OVL_EL2_CON_VAL_EXT_MTX_EN(val)            REG_FLD_VAL(OVL_EL2_CON_FLD_EXT_MTX_EN, (val))
#define OVL_EL2_CON_VAL_HORIZONTAL_FLIP_EN(val)                                         \
                REG_FLD_VAL(OVL_EL2_CON_FLD_HORIZONTAL_FLIP_EN, (val))
#define OVL_EL2_CON_VAL_VERTICAL_FLIP_EN(val)                                           \
                REG_FLD_VAL(OVL_EL2_CON_FLD_VERTICAL_FLIP_EN, (val))
#define OVL_EL2_CON_VAL_ALPHA_EN(val)              REG_FLD_VAL(OVL_EL2_CON_FLD_ALPHA_EN, (val))
#define OVL_EL2_CON_VAL_ALPHA(val)                 REG_FLD_VAL(OVL_EL2_CON_FLD_ALPHA, (val))

#define OVL_EL2_SRCKEY_VAL_SRCKEY(val)           REG_FLD_VAL(OVL_EL2_SRCKEY_FLD_SRCKEY, (val))

#define OVL_EL2_SRC_SIZE_VAL_EL2_SRC_H(val)      REG_FLD_VAL(OVL_EL2_SRC_SIZE_FLD_EL2_SRC_H, (val))
#define OVL_EL2_SRC_SIZE_VAL_EL2_SRC_W(val)      REG_FLD_VAL(OVL_EL2_SRC_SIZE_FLD_EL2_SRC_W, (val))

#define OVL_EL2_OFFSET_VAL_EL2_YOFF(val)         REG_FLD_VAL(OVL_EL2_OFFSET_FLD_EL2_YOFF, (val))
#define OVL_EL2_OFFSET_VAL_EL2_XOFF(val)         REG_FLD_VAL(OVL_EL2_OFFSET_FLD_EL2_XOFF, (val))

#define OVL_EL2_PITCH_VAL_SURFL_EN(val)                                                 \
                REG_FLD_VAL(OVL_EL2_PITCH_FLD_SURFL_EN, (val))
#define OVL_EL2_PITCH_VAL_EL2_BLEND_RND_SHT(val)                                        \
                REG_FLD_VAL(OVL_EL2_PITCH_FLD_EL2_BLEND_RND_SHT, (val))
#define OVL_EL2_PITCH_VAL_EL2_SRGB_SEL_EXT2(val)                                        \
                REG_FLD_VAL(OVL_EL2_PITCH_FLD_EL2_SRGB_SEL_EXT2, (val))
#define OVL_EL2_PITCH_VAL_EL2_CONST_BLD(val)                                            \
                REG_FLD_VAL(OVL_EL2_PITCH_FLD_EL2_CONST_BLD, (val))
#define OVL_EL2_PITCH_VAL_EL2_DRGB_SEL_EXT(val)                                         \
                REG_FLD_VAL(OVL_EL2_PITCH_FLD_EL2_DRGB_SEL_EXT, (val))
#define OVL_EL2_PITCH_VAL_EL2_DA_SEL_EXT(val)                                           \
                REG_FLD_VAL(OVL_EL2_PITCH_FLD_EL2_DA_SEL_EXT, (val))
#define OVL_EL2_PITCH_VAL_EL2_SRGB_SEL_EXT(val)                                         \
                REG_FLD_VAL(OVL_EL2_PITCH_FLD_EL2_SRGB_SEL_EXT, (val))
#define OVL_EL2_PITCH_VAL_EL2_SA_SEL_EXT(val)                                           \
                REG_FLD_VAL(OVL_EL2_PITCH_FLD_EL2_SA_SEL_EXT, (val))
#define OVL_EL2_PITCH_VAL_EL2_DRGB_SEL(val)                                             \
                REG_FLD_VAL(OVL_EL2_PITCH_FLD_EL2_DRGB_SEL, (val))
#define OVL_EL2_PITCH_VAL_EL2_DA_SEL(val)                                               \
                REG_FLD_VAL(OVL_EL2_PITCH_FLD_EL2_DA_SEL, (val))
#define OVL_EL2_PITCH_VAL_EL2_SRGB_SEL(val)                                             \
                REG_FLD_VAL(OVL_EL2_PITCH_FLD_EL2_SRGB_SEL, (val))
#define OVL_EL2_PITCH_VAL_EL2_SA_SEL(val)                                               \
                REG_FLD_VAL(OVL_EL2_PITCH_FLD_EL2_SA_SEL, (val))
#define OVL_EL2_PITCH_VAL_EL2_SRC_PITCH(val)                                            \
                REG_FLD_VAL(OVL_EL2_PITCH_FLD_EL2_SRC_PITCH, (val))

#define OVL_EL2_TILE_VAL_TILE_HORI_BLOCK_NUM(val)                                       \
                REG_FLD_VAL(OVL_EL2_TILE_FLD_TILE_HORI_BLOCK_NUM, (val))
#define OVL_EL2_TILE_VAL_TILE_EN(val)                                                   \
                REG_FLD_VAL(OVL_EL2_TILE_FLD_TILE_EN, (val))
#define OVL_EL2_TILE_VAL_TILE_WIDTH_SEL(val)                                            \
                REG_FLD_VAL(OVL_EL2_TILE_FLD_TILE_WIDTH_SEL, (val))
#define OVL_EL2_TILE_VAL_TILE_HEIGHT(val)                                               \
                REG_FLD_VAL(OVL_EL2_TILE_FLD_TILE_HEIGHT, (val))

#define OVL_EL2_CLIP_VAL_EL2_SRC_BOTTOM_CLIP(val)                                       \
                REG_FLD_VAL(OVL_EL2_CLIP_FLD_EL2_SRC_BOTTOM_CLIP, (val))
#define OVL_EL2_CLIP_VAL_EL2_SRC_TOP_CLIP(val)                                          \
                REG_FLD_VAL(OVL_EL2_CLIP_FLD_EL2_SRC_TOP_CLIP, (val))
#define OVL_EL2_CLIP_VAL_EL2_SRC_RIGHT_CLIP(val)                                        \
                REG_FLD_VAL(OVL_EL2_CLIP_FLD_EL2_SRC_RIGHT_CLIP, (val))
#define OVL_EL2_CLIP_VAL_EL2_SRC_LEFT_CLIP(val)                                         \
                REG_FLD_VAL(OVL_EL2_CLIP_FLD_EL2_SRC_LEFT_CLIP, (val))

#define OVL_EL0_CLR_VAL_ALPHA(val)             REG_FLD_VAL(OVL_EL0_CLR_FLD_ALPHA, (val))
#define OVL_EL0_CLR_VAL_RED(val)               REG_FLD_VAL(OVL_EL0_CLR_FLD_RED, (val))
#define OVL_EL0_CLR_VAL_GREEN(val)             REG_FLD_VAL(OVL_EL0_CLR_FLD_GREEN, (val))
#define OVL_EL0_CLR_VAL_BLUE(val)              REG_FLD_VAL(OVL_EL0_CLR_FLD_BLUE, (val))

#define OVL_EL1_CLR_VAL_ALPHA(val)             REG_FLD_VAL(OVL_EL1_CLR_FLD_ALPHA, (val))
#define OVL_EL1_CLR_VAL_RED(val)               REG_FLD_VAL(OVL_EL1_CLR_FLD_RED, (val))
#define OVL_EL1_CLR_VAL_GREEN(val)             REG_FLD_VAL(OVL_EL1_CLR_FLD_GREEN, (val))
#define OVL_EL1_CLR_VAL_BLUE(val)              REG_FLD_VAL(OVL_EL1_CLR_FLD_BLUE, (val))

#define OVL_EL2_CLR_VAL_ALPHA(val)             REG_FLD_VAL(OVL_EL2_CLR_FLD_ALPHA, (val))
#define OVL_EL2_CLR_VAL_RED(val)               REG_FLD_VAL(OVL_EL2_CLR_FLD_RED, (val))
#define OVL_EL2_CLR_VAL_GREEN(val)             REG_FLD_VAL(OVL_EL2_CLR_FLD_GREEN, (val))
#define OVL_EL2_CLR_VAL_BLUE(val)              REG_FLD_VAL(OVL_EL2_CLR_FLD_BLUE, (val))

#define OVL_SBCH_VAL_L3_SBCH_CNST_EN(val)      REG_FLD_VAL(OVL_SBCH_FLD_L3_SBCH_CNST_EN, (val))
#define OVL_SBCH_VAL_L3_SBCH_TRANS_EN(val)     REG_FLD_VAL(OVL_SBCH_FLD_L3_SBCH_TRANS_EN, (val))
#define OVL_SBCH_VAL_L2_SBCH_CNST_EN(val)      REG_FLD_VAL(OVL_SBCH_FLD_L2_SBCH_CNST_EN, (val))
#define OVL_SBCH_VAL_L2_SBCH_TRANS_EN(val)     REG_FLD_VAL(OVL_SBCH_FLD_L2_SBCH_TRANS_EN, (val))
#define OVL_SBCH_VAL_L1_SBCH_CNST_EN(val)      REG_FLD_VAL(OVL_SBCH_FLD_L1_SBCH_CNST_EN, (val))
#define OVL_SBCH_VAL_L1_SBCH_TRANS_EN(val)     REG_FLD_VAL(OVL_SBCH_FLD_L1_SBCH_TRANS_EN, (val))
#define OVL_SBCH_VAL_L0_SBCH_CNST_EN(val)      REG_FLD_VAL(OVL_SBCH_FLD_L0_SBCH_CNST_EN, (val))
#define OVL_SBCH_VAL_L0_SBCH_TRANS_EN(val)     REG_FLD_VAL(OVL_SBCH_FLD_L0_SBCH_TRANS_EN, (val))
#define OVL_SBCH_VAL_L3_SBCH_UPDATE(val)       REG_FLD_VAL(OVL_SBCH_FLD_L3_SBCH_UPDATE, (val))
#define OVL_SBCH_VAL_L2_SBCH_UPDATE(val)       REG_FLD_VAL(OVL_SBCH_FLD_L2_SBCH_UPDATE, (val))
#define OVL_SBCH_VAL_L1_SBCH_UPDATE(val)       REG_FLD_VAL(OVL_SBCH_FLD_L1_SBCH_UPDATE, (val))
#define OVL_SBCH_VAL_L0_SBCH_UPDATE(val)       REG_FLD_VAL(OVL_SBCH_FLD_L0_SBCH_UPDATE, (val))

#define OVL_SBCH_EXT_VAL_EL2_SBCH_CNST_EN(val)                                          \
                REG_FLD_VAL(OVL_SBCH_EXT_FLD_EL2_SBCH_CNST_EN, (val))
#define OVL_SBCH_EXT_VAL_EL2_SBCH_TRANS_EN(val)                                         \
                REG_FLD_VAL(OVL_SBCH_EXT_FLD_EL2_SBCH_TRANS_EN, (val))
#define OVL_SBCH_EXT_VAL_EL1_SBCH_CNST_EN(val)                                          \
                REG_FLD_VAL(OVL_SBCH_EXT_FLD_EL1_SBCH_CNST_EN, (val))
#define OVL_SBCH_EXT_VAL_EL1_SBCH_TRANS_EN(val)                                         \
                REG_FLD_VAL(OVL_SBCH_EXT_FLD_EL1_SBCH_TRANS_EN, (val))
#define OVL_SBCH_EXT_VAL_EL0_SBCH_CNST_EN(val)                                          \
                REG_FLD_VAL(OVL_SBCH_EXT_FLD_EL0_SBCH_CNST_EN, (val))
#define OVL_SBCH_EXT_VAL_EL0_SBCH_TRANS_EN(val)                                         \
                REG_FLD_VAL(OVL_SBCH_EXT_FLD_EL0_SBCH_TRANS_EN, (val))
#define OVL_SBCH_EXT_VAL_EL2_SBCH_UPDATE(val)                                           \
                REG_FLD_VAL(OVL_SBCH_EXT_FLD_EL2_SBCH_UPDATE, (val))
#define OVL_SBCH_EXT_VAL_EL1_SBCH_UPDATE(val)                                           \
                REG_FLD_VAL(OVL_SBCH_EXT_FLD_EL1_SBCH_UPDATE, (val))
#define OVL_SBCH_EXT_VAL_EL0_SBCH_UPDATE(val)                                           \
                REG_FLD_VAL(OVL_SBCH_EXT_FLD_EL0_SBCH_UPDATE, (val))

#define OVL_SBCH_CON_VAL_EL2_SBCH_TRANS_INVALID(val)                                    \
                REG_FLD_VAL(OVL_SBCH_CON_FLD_EL2_SBCH_TRANS_INVALID, (val))
#define OVL_SBCH_CON_VAL_EL1_SBCH_TRANS_INVALID(val)                                    \
                REG_FLD_VAL(OVL_SBCH_CON_FLD_EL1_SBCH_TRANS_INVALID, (val))
#define OVL_SBCH_CON_VAL_EL0_SBCH_TRANS_INVALID(val)                                    \
                REG_FLD_VAL(OVL_SBCH_CON_FLD_EL0_SBCH_TRANS_INVALID, (val))
#define OVL_SBCH_CON_VAL_L3_SBCH_TRANS_INVALID(val)                                     \
                REG_FLD_VAL(OVL_SBCH_CON_FLD_L3_SBCH_TRANS_INVALID, (val))
#define OVL_SBCH_CON_VAL_L2_SBCH_TRANS_INVALID(val)                                     \
                REG_FLD_VAL(OVL_SBCH_CON_FLD_L2_SBCH_TRANS_INVALID, (val))
#define OVL_SBCH_CON_VAL_L1_SBCH_TRANS_INVALID(val)                                     \
                REG_FLD_VAL(OVL_SBCH_CON_FLD_L1_SBCH_TRANS_INVALID, (val))
#define OVL_SBCH_CON_VAL_L0_SBCH_TRANS_INVALID(val)                                     \
                REG_FLD_VAL(OVL_SBCH_CON_FLD_L0_SBCH_TRANS_INVALID, (val))

#define OVL_L0_ADDR_VAL_L0_ADDR(val)           REG_FLD_VAL(OVL_L0_ADDR_FLD_L0_ADDR, (val))
#define OVL_L1_ADDR_VAL_L1_ADDR(val)           REG_FLD_VAL(OVL_L1_ADDR_FLD_L1_ADDR, (val))
#define OVL_L2_ADDR_VAL_L2_ADDR(val)           REG_FLD_VAL(OVL_L2_ADDR_FLD_L2_ADDR, (val))
#define OVL_L3_ADDR_VAL_L3_ADDR(val)           REG_FLD_VAL(OVL_L3_ADDR_FLD_L3_ADDR, (val))

#define OVL_EL0_ADDR_VAL_EL0_ADDR(val)         REG_FLD_VAL(OVL_EL0_ADDR_FLD_EL0_ADDR, (val))
#define OVL_EL1_ADDR_VAL_EL1_ADDR(val)         REG_FLD_VAL(OVL_EL1_ADDR_FLD_EL1_ADDR, (val))
#define OVL_EL2_ADDR_VAL_EL2_ADDR(val)         REG_FLD_VAL(OVL_EL2_ADDR_FLD_EL2_ADDR, (val))

#define OVL_SECURE_VAL_L3_SECURE(val)          REG_FLD_VAL(OVL_SECURE_FLD_L3_SECURE, (val))
#define OVL_SECURE_VAL_L2_SECURE(val)          REG_FLD_VAL(OVL_SECURE_FLD_L2_SECURE, (val))
#define OVL_SECURE_VAL_L1_SECURE(val)          REG_FLD_VAL(OVL_SECURE_FLD_L1_SECURE, (val))
#define OVL_SECURE_VAL_L0_SECURE(val)          REG_FLD_VAL(OVL_SECURE_FLD_L0_SECURE, (val))

#ifdef __cplusplus
}
#endif
