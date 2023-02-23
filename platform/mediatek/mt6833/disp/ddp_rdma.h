/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include "ddp_info.h"

#define RDMA_INSTANCES  1
#define RDMA_MAX_WIDTH  4095
#define RDMA_MAX_HEIGHT 4095

enum RDMA_OUTPUT_FORMAT {
    RDMA_OUTPUT_FORMAT_ARGB   = 0,
    RDMA_OUTPUT_FORMAT_YUV444 = 1,
};

enum RDMA_MODE {
    RDMA_MODE_DIRECT_LINK = 0,
    RDMA_MODE_MEMORY      = 1,
};

struct rdma_color_matrix {
    u32 C00;
    u32 C01;
    u32 C02;
    u32 C10;
    u32 C11;
    u32 C12;
    u32 C20;
    u32 C21;
    u32 C22;
};

struct rdma_color_pre {
    u32 ADD0;
    u32 ADD1;
    u32 ADD2;
};

struct rdma_color_post {
    u32 ADD0;
    u32 ADD1;
    u32 ADD2;
};

// start module
int RDMAStart(enum DISP_MODULE_ENUM module, void *handle);

// stop module
int RDMAStop(enum DISP_MODULE_ENUM module, void *handle);

// reset module
int RDMAReset(enum DISP_MODULE_ENUM module, void *handle);

// configu module
int RDMAConfig(enum DISP_MODULE_ENUM module,
               enum RDMA_MODE mode,
               unsigned int address,
               enum DpColorFormat  inFormat,
               unsigned int pitch,
               unsigned int width,
               unsigned int height,
               unsigned int ufoe_enable,
               unsigned int dsi_mode,
               void *handle); // ourput setting

int RDMAWait(enum DISP_MODULE_ENUM module, void *handle);

void RDMASetTargetLine(enum DISP_MODULE_ENUM module, unsigned int line, void *handle);

void RDMADump(enum DISP_MODULE_ENUM module);
void rdma_dump_analysis(enum DISP_MODULE_ENUM module);

void rdma_enable_color_transform(enum DISP_MODULE_ENUM module);
void rdma_disable_color_transform(enum DISP_MODULE_ENUM module);
void rdma_set_color_matrix(enum DISP_MODULE_ENUM module,
                           struct rdma_color_matrix *matrix,
                           struct rdma_color_pre *pre,
                           struct rdma_color_post *post);
