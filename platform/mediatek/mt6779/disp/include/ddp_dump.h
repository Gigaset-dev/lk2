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
#include "ddp_path.h"

typedef enum {
    DDP_SIGNAL_OVL0__OVL0_MOUT        = 1,
    DDP_SIGNAL_OVL0_MOUT1__WDMA0_SEL0 = 0,
    DDP_SIGNAL_COLOR0_SEL__COLOR0     = 3,
    DDP_SIGNAL_COLOR0__COLOR0_SOUT    = 2,
    DDP_SIGNAL_COLOR0_SOUT1__MERGE0   = 23,
    DDP_SIGNAL_COLOR0_SOUT0__AAL_SEL0 = 22,
    DDP_SIGNAL_MERGE0__AAL_SEL1       = 18,
    DDP_SIGNAL_AAL_SEL__AAL0          = 24,
    DDP_SIGNAL_AAL0__OD               = 10,
    DDP_SIGNAL_PATH0_SOUT0__UFOE_SEL0 = 26,
    DDP_SIGNAL_PATH0_SOUT1__SPLIT0    = 25,
    DDP_SIGNAL_SPLIT0__PATH1_SEL2     = 29,
    DDP_SIGNAL_SPLIT0__UFOE_SEL1      = 28,
    DDP_SIGNAL_UFOE_SEL__UFOE0        = 27,
    DDP_SIGNAL_UFOE0__UFOE_MOUT       = 11,

    DDP_SIGNAL_OVL1__OVL1_MOUT        = 5,
    DDP_SIGNAL_OVL1_MOUT1__WDMA1_SEL0 = 4,
    DDP_SIGNAL_OVL1_MOUT0__COLOR1_SEL1 = 7,
    DDP_SIGNAL_COLOR1_SOUT0__GAMMA0   = 21,
    DDP_SIGNAL_COLOR1__COLOR1_SOUT    = 20,
    DDP_SIGNAL_COLOR1_SOUT1__MERGE0   = 19,
    DDP_SIGNAL_COLOR1_SEL__COLOR1     = 6,
    DDP_SIGNAL_GAMMA0__GAMMA_MOUT     = 31,
    DDP_SIGNAL_PATH1_SEL__PATH1_SOUT  = 30,
    DDP_SIGNAL_PATH1_SOUT0__DSI0_SEL2 = 9,
    DDP_SIGNAL_RDMA2__RDMA2_SOUT      = 17,
    DDP_SIGNAL_RDMA2_SOUT1__DPI_SEL2  = 16,
    DDP_SIGNAL_RDMA2_SOUT0__DSI1_SEL2 = 15,
    DDP_SIGNAL_SPLIT1__DSI1_SEL0      = 13,
    DDP_SIGNAL_DSI0_SEL__DSI0         = 12,
    DDP_SIGNAL_DSI1_SEL__DSI1         = 8,
    DDP_SIGNAL_DPI_SEL__DPI0          = 14
} DISP_ENGINE_SIGNAL0;

typedef enum {
    DDP_SIGNAL_OVL0_MOUT0__COLOR0_SEL1 = 7,
    DDP_SIGNAL_RDMA0__RDMA0_SOUT       = 6,
    DDP_SIGNAL_RDMA0_SOUT0__PATH0_SEL0 = 11,
    DDP_SIGNAL_RDMA0_SOUT1__COLOR0_SEL0 = 8,
    DDP_SIGNAL_OD__OD_MOUT             = 23,
    DDP_SIGNAL_OD_MOUT2__WDMA0_SEL1    = 14,
    DDP_SIGNAL_OD_MOUT0__RDMA0         = 10,
    DDP_SIGNAL_OD_MOUT1__PATH0_SEL1    = 9,
    DDP_SIGNAL_PATH0_SEL__PATH0_SOUT   = 12,
    DDP_SIGNAL_UFOE_MOUT2__DPI_SEL0    = 3,
    DDP_SIGNAL_UFOE_MOUT1__SPLIT1      = 2,
    DDP_SIGNAL_UFOE_MOUT0__DSI0_SEL0   = 1,
    DDP_SIGNAL_UFOE_MOUT3__WDMA0_SEL2  = 22,
    DDP_SIGNAL_WDMA0_SEL__WDMA0        = 13,

    DDP_SIGNAL_RDMA1_SOUT0__PATH1_SEL0 = 21,
    DDP_SIGNAL_RDMA1_SOUT1__COLOR1_SEL0 = 16,
    DDP_SIGNAL_RDMA1__RDMA1_SOUT       = 15,
    DDP_SIGNAL_GAMMA_MOUT2__WDMA1_SEL1 = 17,
    DDP_SIGNAL_GAMMA_MOUT1__PATH1_SEL1 = 20,
    DDP_SIGNAL_GAMMA_MOUT0__RDMA1      = 19,
    DDP_SIGNAL_PATH1_SOUT1__DSI1_SEL1  = 5,
    DDP_SIGNAL_PATH1_SOUT2__DPI_SEL1   = 4,
    DDP_SIGNAL_SPLIT1__DSI0_SEL1       = 0,
    DDP_SIGNAL_WDMA1_SEL__WDMA1        = 18
} DISP_ENGINE_SIGNAL1;

const char *ddp_get_fmt_name(DISP_MODULE_ENUM module, unsigned int fmt);
int ddp_dump_analysis(DISP_MODULE_ENUM module);
int ddp_dump_reg(DISP_MODULE_ENUM module);
int ddp_check_signal(DDP_SCENARIO_ENUM scenario);

