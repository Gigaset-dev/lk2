/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

//#include <linux/types.h>
#include "ddp_hal.h"
//#include "mt_typedefs.h"
#include <stdlib.h>
#include <stdbool.h>
#include <lcm_drv.h>
#include "ddp_data_type.h"
#include "ddp_reg.h"
#include "disp_event.h"
#include "ddp_ovl.h"
struct OVL_CONFIG_STRUCT {
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
};

struct RDMA_CONFIG_STRUCT {
    unsigned int idx;            // instance index
    enum DpColorFormat  inputFormat;
    unsigned int address;
    unsigned int pitch;
    unsigned int width;
    unsigned int height;
};

struct WDMA_CONFIG_STRUCT {
    unsigned int srcWidth;
    unsigned int srcHeight;     // input
    unsigned int clipX;
    unsigned int clipY;
    unsigned int clipWidth;
    unsigned int clipHeight;    // clip
    enum DpColorFormat  outputFormat;
    unsigned int dstAddress;
    unsigned int dstWidth;     // output
    unsigned int useSpecifiedAlpha;
    unsigned char alpha;
};

struct disp_ddp_path_config {
    // for ovl
    unsigned int ovl_dirty;
    unsigned int rdma_dirty;
    unsigned int wdma_dirty;
    unsigned int dst_dirty;
    int ovl_layer_scanned;
    struct OVL_CONFIG_STRUCT  ovl_config[TOTAL_OVL_LAYER_NUM];
    struct RDMA_CONFIG_STRUCT rdma_config;
    struct WDMA_CONFIG_STRUCT wdma_config;
    LCM_DSI_PARAMS dsi_config;
    LCM_DPI_PARAMS dpi_config;
    unsigned int lcm_bpp;
    unsigned int dst_w;
    unsigned int dst_h;
    LCM_ROUND_CORNER round_corner_params;
    bool is_dual;
};

typedef int (*ddp_module_notify)(enum DISP_MODULE_ENUM, enum DISP_PATH_EVENT);

struct DDP_MODULE_DRIVER {
    enum DISP_MODULE_ENUM    module;
    int (*init)(enum DISP_MODULE_ENUM module, void *handle);
    int (*deinit)(enum DISP_MODULE_ENUM module, void *handle);
    int (*config)(enum DISP_MODULE_ENUM module, struct disp_ddp_path_config *config, void *handle);
    int (*start)(enum DISP_MODULE_ENUM module, void *handle);
    int (*trigger)(enum DISP_MODULE_ENUM module, void *cmdq_handle);
    int (*stop)(enum DISP_MODULE_ENUM module, void *handle);
    int (*reset)(enum DISP_MODULE_ENUM module, void *handle);
    int (*power_on)(enum DISP_MODULE_ENUM module, void *handle);
    int (*power_off)(enum DISP_MODULE_ENUM module, void *handle);
    int (*is_idle)(enum DISP_MODULE_ENUM module);
    int (*is_busy)(enum DISP_MODULE_ENUM module);
    int (*dump_info)(enum DISP_MODULE_ENUM module, int level);
    int (*bypass)(enum DISP_MODULE_ENUM module, int bypass);
    int (*build_cmdq)(enum DISP_MODULE_ENUM module, void *cmdq_handle, enum CMDQ_STATE state);
    int (*set_lcm_utils)(enum DISP_MODULE_ENUM module, LCM_DRIVER *lcm_drv, void *fdt);
    int (*set_listener)(enum DISP_MODULE_ENUM module, ddp_module_notify notify);
    int (*polling_irq)(enum DISP_MODULE_ENUM module, int bit, int timeout);
    int (*connect)(enum DISP_MODULE_ENUM module, enum DISP_MODULE_ENUM prev,
                    enum DISP_MODULE_ENUM next, int connect, void *handle);
};

struct ddp_module {
    enum DISP_MODULE_ENUM module_id;
    const char *module_name;
    unsigned int can_connect; /* module can be connect if 1 */
    struct DDP_MODULE_DRIVER *module_driver;
};

unsigned int is_ddp_module(enum DISP_MODULE_ENUM module);
const char *ddp_get_module_name(enum DISP_MODULE_ENUM module);
void ddp_init_modules_driver(void);
int ddp_enable_module_clock(enum DISP_MODULE_ENUM module);
int ddp_disable_module_clock(enum DISP_MODULE_ENUM module);


extern struct DDP_MODULE_DRIVER ddp_driver_dsi0;
/* extern struct DDP_MODULE_DRIVER ddp_driver_dsi1; */
/* extern struct DDP_MODULE_DRIVER ddp_driver_dsidual; */
extern struct DDP_MODULE_DRIVER ddp_driver_ovl;
extern struct DDP_MODULE_DRIVER ddp_driver_rdma;
extern struct DDP_MODULE_DRIVER ddp_driver_color;
extern struct DDP_MODULE_DRIVER ddp_driver_ccorr;
extern struct DDP_MODULE_DRIVER ddp_driver_aal;
extern struct DDP_MODULE_DRIVER ddp_driver_gamma;
extern struct DDP_MODULE_DRIVER ddp_driver_postmask;
extern struct DDP_MODULE_DRIVER ddp_driver_dither;
/* extern struct DDP_MODULE_DRIVER ddp_driver_split; */
extern struct DDP_MODULE_DRIVER ddp_driver_pwm;
extern struct DDP_MODULE_DRIVER ddp_driver_dsc;

