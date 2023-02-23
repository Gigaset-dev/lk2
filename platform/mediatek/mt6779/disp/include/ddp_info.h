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

#pragma once

//#include <linux/types.h>
#include "ddp_hal.h"
/* #include "mt_typedefs.h" */
#include <stdlib.h>
#include "ddp_data_type.h"
#include "lcm_drv.h"
#include "ddp_reg.h"
#include "disp_event.h"
#include "ddp_ovl.h"
typedef struct _OVL_CONFIG_STRUCT {
    unsigned int ovl_index;
    unsigned int layer;
    unsigned int layer_en;
    enum OVL_LAYER_SOURCE source;
    unsigned int fmt;
    unsigned int addr;
    unsigned int vaddr;
    unsigned int src_x;
    unsigned int src_y;
    unsigned int src_w;
    unsigned int src_h;
    unsigned int src_pitch;
    unsigned int dst_x;
    unsigned int dst_y;
    unsigned int dst_w;
    unsigned int dst_h;                  // clip region
    unsigned int keyEn;
    unsigned int key;
    unsigned int aen;
    unsigned char alpha;

    unsigned int isTdshp;
    unsigned int isDirty;

    unsigned int buff_idx;
    unsigned int identity;
    unsigned int connected_type;
    unsigned int security;
} OVL_CONFIG_STRUCT;

typedef struct _RDMA_CONFIG_STRUCT {
    unsigned idx;            // instance index
    DpColorFormat inputFormat;
    unsigned address;
    unsigned pitch;
    unsigned width;
    unsigned height;
} RDMA_CONFIG_STRUCT;

typedef struct _WDMA_CONFIG_STRUCT {
    unsigned srcWidth;
    unsigned srcHeight;     // input
    unsigned clipX;
    unsigned clipY;
    unsigned clipWidth;
    unsigned clipHeight;    // clip
    DpColorFormat outputFormat;
    unsigned dstAddress;
    unsigned dstWidth;     // output
    unsigned int useSpecifiedAlpha;
    unsigned char alpha;
} WDMA_CONFIG_STRUCT;

typedef struct {
    // for ovl
    unsigned int ovl_dirty;
    unsigned int rdma_dirty;
    unsigned int wdma_dirty;
    unsigned int dst_dirty;
    int ovl_layer_scanned;
    OVL_CONFIG_STRUCT  ovl_config[TOTAL_OVL_LAYER_NUM];
    RDMA_CONFIG_STRUCT rdma_config;
    WDMA_CONFIG_STRUCT wdma_config;
    LCM_DSI_PARAMS dsi_config;
    LCM_DPI_PARAMS dpi_config;
    unsigned int lcm_bpp;
    unsigned int dst_w;
    unsigned int dst_h;
    LCM_ROUND_CORNER round_corner_params;
} disp_ddp_path_config;

typedef int (*ddp_module_notify)(DISP_MODULE_ENUM, DISP_PATH_EVENT);

typedef struct {
    DISP_MODULE_ENUM    module;
    int (*init)(DISP_MODULE_ENUM module, void *handle);
    int (*deinit)(DISP_MODULE_ENUM module, void *handle);
    int (*config)(DISP_MODULE_ENUM module, disp_ddp_path_config *config, void *handle);
    int (*start)(DISP_MODULE_ENUM module, void *handle);
    int (*trigger)(DISP_MODULE_ENUM module, void *cmdq_handle);
    int (*stop)(DISP_MODULE_ENUM module, void *handle);
    int (*reset)(DISP_MODULE_ENUM module, void *handle);
    int (*power_on)(DISP_MODULE_ENUM module, void *handle);
    int (*power_off)(DISP_MODULE_ENUM module, void *handle);
    int (*is_idle)(DISP_MODULE_ENUM module);
    int (*is_busy)(DISP_MODULE_ENUM module);
    int (*dump_info)(DISP_MODULE_ENUM module, int level);
    int (*bypass)(DISP_MODULE_ENUM module, int bypass);
    int (*build_cmdq)(DISP_MODULE_ENUM module, void *cmdq_handle, CMDQ_STATE state);
    int (*set_lcm_utils)(DISP_MODULE_ENUM module, LCM_DRIVER *lcm_drv, void *fdt);
    int (*set_listener)(DISP_MODULE_ENUM module, ddp_module_notify notify);
    int (*polling_irq)(DISP_MODULE_ENUM module, int bit, int timeout);
    int (*connect)(DISP_MODULE_ENUM module, DISP_MODULE_ENUM prev, DISP_MODULE_ENUM next, int connect, void *handle);
} DDP_MODULE_DRIVER;

const char *ddp_get_module_name(DISP_MODULE_ENUM module);
//int   ddp_get_module_max_irq_bit(DISP_MODULE_ENUM module);
int ddp_enable_module_clock(DISP_MODULE_ENUM module);

int ddp_disable_module_clock(DISP_MODULE_ENUM module);

