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

typedef struct _rdma_color_matrix {
    u32 C00;
    u32 C01;
    u32 C02;
    u32 C10;
    u32 C11;
    u32 C12;
    u32 C20;
    u32 C21;
    u32 C22;
} rdma_color_matrix;

typedef struct _rdma_color_pre {
    u32 ADD0;
    u32 ADD1;
    u32 ADD2;
} rdma_color_pre;

typedef struct _rdma_color_post {
    u32 ADD0;
    u32 ADD1;
    u32 ADD2;
} rdma_color_post;

// start module
int RDMAStart(DISP_MODULE_ENUM module, void *handle);

// stop module
int RDMAStop(DISP_MODULE_ENUM module, void *handle);

// reset module
int RDMAReset(DISP_MODULE_ENUM module, void *handle);

// configu module
int RDMAConfig(DISP_MODULE_ENUM module,
               enum RDMA_MODE mode,
               unsigned address,
               DpColorFormat inFormat,
               unsigned pitch,
               unsigned width,
               unsigned height,
               unsigned ufoe_enable,
               unsigned dsi_mode,
               void *handle);  // ourput setting

int RDMAWait(DISP_MODULE_ENUM module, void *handle);

void RDMASetTargetLine(DISP_MODULE_ENUM module, unsigned int line, void *handle);

void RDMADump(DISP_MODULE_ENUM module);

void rdma_enable_color_transform(DISP_MODULE_ENUM module);
void rdma_disable_color_transform(DISP_MODULE_ENUM module);
void rdma_set_color_matrix(DISP_MODULE_ENUM module,
                           rdma_color_matrix *matrix,
                           rdma_color_pre *pre,
                           rdma_color_post *post);
void rdma_dump_analysis(DISP_MODULE_ENUM module);

