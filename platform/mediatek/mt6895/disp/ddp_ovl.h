/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include "ddp_data_type.h"
#include "ddp_hal.h"


#define OVL_MAX_WIDTH  (4095)
#define OVL_MAX_HEIGHT (4095)

#define TOTAL_OVL_LAYER_NUM    (6)
#define OVL_NUM                (6)

// start overlay module
int OVLStart(enum DISP_MODULE_ENUM module, void *handle);

// stop overlay module
int OVLStop(enum DISP_MODULE_ENUM module, void *handle);

// reset overlay module
int OVLReset(enum DISP_MODULE_ENUM module, void *handle);

// set region of interest
int OVLROI(enum DISP_MODULE_ENUM module,
            unsigned int bgW,
            unsigned int bgH,
            unsigned int bgColor,
            void *handle);

// switch layer on/off
int OVLLayerSwitch(enum DISP_MODULE_ENUM module,
                   unsigned int layer,
                   unsigned int en,
                   void *handle);

// configure layer property
int OVLLayerConfig(enum DISP_MODULE_ENUM module,
                   unsigned int layer,
                   unsigned int source,
                   enum DpColorFormat  format,
                   unsigned int addr,
                   unsigned int src_x,
                   unsigned int src_y,
                   unsigned int src_pitch,
                   unsigned int dst_x,
                   unsigned int dst_y,
                   unsigned int dst_w,
                   unsigned int dst_h,
                   unsigned int keyEn,
                   unsigned int key,
                   unsigned int aen,
                   unsigned char alpha,
                   void *handle);

int OVL3DConfig(enum DISP_MODULE_ENUM module,
                unsigned int layer_id,
                unsigned int en_3d,
                unsigned int landscape,
                unsigned int r_first,
                void *handle);

void OVLDump(enum DISP_MODULE_ENUM module);
void ovl_dump_analysis(enum DISP_MODULE_ENUM module);
