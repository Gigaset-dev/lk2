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

#include "lcm_drv.h"

#ifdef __cplusplus
extern "C" {
#endif

// ---------------------------------------------------------------------------

#define DSI_CHECK_RET(expr)             \
    do {                                \
        enum DSI_STATUS ret = (expr);        \
        ASSERT(ret == DSI_STATUS_OK);   \
    } while (0)

/* --------------------------------------------------------------------------- */

#define     DSI_DCS_SHORT_PACKET_ID_0           0x05
#define     DSI_DCS_SHORT_PACKET_ID_1           0x15
#define     DSI_DCS_LONG_PACKET_ID              0x39
#define     DSI_DCS_READ_PACKET_ID              0x06

#define     DSI_GERNERIC_SHORT_PACKET_ID_1      0x13
#define     DSI_GERNERIC_SHORT_PACKET_ID_2      0x23
#define     DSI_GERNERIC_LONG_PACKET_ID         0x29
#define     DSI_GERNERIC_READ_LONG_PACKET_ID    0x14


#define     DSI_WMEM_CONTI                      (0x3C)
#define     DSI_RMEM_CONTI                      (0x3E)

/* ESD recovery method for video mode LCM */
#define     METHOD_NONCONTINUOUS_CLK            (0x1)
#define     METHOD_BUS_TURN_AROUND              (0x2)

/* State of DSI engine */
#define     DSI_VDO_VSA_VS_STATE                (0x008)
#define     DSI_VDO_VSA_HS_STATE                (0x010)
#define     DSI_VDO_VSA_VE_STATE                (0x020)
#define     DSI_VDO_VBP_STATE                   (0x040)
#define     DSI_VDO_VACT_STATE                  (0x080)
#define     DSI_VDO_VFP_STATE                   (0x100)

/* --------------------------------------------------------------------------- */
enum DSI_STATUS {
    DSI_STATUS_OK = 0,

    DSI_STATUS_ERROR,
};


enum DSI_INS_TYPE {
    SHORT_PACKET_RW = 0,
    FB_WRITE        = 1,
    LONG_PACKET_W   = 2,
    FB_READ         = 3,
};


enum DSI_CMDQ_BTA {
    DISABLE_BTA = 0,
    ENABLE_BTA  = 1,
};


enum DSI_CMDQ_HS {
    LOW_POWER   = 0,
    HIGH_SPEED  = 1,
};


enum DSI_CMDQ_CL {
    CL_8BITS    = 0,
    CL_16BITS   = 1,
};


enum DSI_CMDQ_TE {
    DISABLE_TE  = 0,
    ENABLE_TE   = 1,
};


enum DSI_CMDQ_RPT {
    DISABLE_RPT = 0,
    ENABLE_RPT  = 1,
};


struct DSI_CMDQ_CONFG {
    unsigned        type    : 2;
    unsigned        BTA     : 1;
    unsigned        HS      : 1;
    unsigned        CL      : 1;
    unsigned        TE      : 1;
    unsigned        Rsv     : 1;
    unsigned        RPT     : 1;
};


struct DSI_T0_INS {
    unsigned CONFG          : 8;
    unsigned Data_ID        : 8;
    unsigned Data0          : 8;
    unsigned Data1          : 8;
};

struct DSI_T1_INS {
    unsigned CONFG          : 8;
    unsigned Data_ID        : 8;
    unsigned mem_start0     : 8;
    unsigned mem_start1     : 8;
};

struct DSI_T2_INS {
    unsigned CONFG          : 8;
    unsigned Data_ID        : 8;
    unsigned WC16           : 16;
    unsigned int *pdata;
};

struct DSI_T3_INS {
    unsigned CONFG          : 8;
    unsigned Data_ID        : 8;
    unsigned mem_start0     : 8;
    unsigned mem_start1     : 8;
};

struct DSI_PLL_CONFIG {
    u8 TXDIV0;
    u8 TXDIV1;
    u32  SDM_PCW;
    u8 SSC_PH_INIT;
    u16  SSC_PRD;
    u16  SSC_DELTA1;
    u16  SSC_DELTA;
};

enum DSI_INTERFACE_ID {
    DSI_INTERFACE_0 = 0,
    DSI_INTERFACE_1,
    DSI_INTERFACE_DUAL,
    DSI_INTERFACE_NUM,
};


void dsi_analysis(DISP_MODULE_ENUM module);
void DSI_PHY_clk_setting(DISP_MODULE_ENUM module, void *cmdq, LCM_DSI_PARAMS *dsi_params);
enum DSI_STATUS DSI_DumpRegisters(DISP_MODULE_ENUM module, int level);
//enum DSI_STATUS DSI_DumpRegisters(DISP_MODULE_ENUM module, void* cmdq, int level);

#ifdef __cplusplus
}
#endif

