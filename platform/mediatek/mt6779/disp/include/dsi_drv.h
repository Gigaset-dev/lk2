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

/* #include <platform/mt_typedefs.h> */
#include <stdlib.h>

#include "lcm_drv.h"

#ifdef __cplusplus
extern "C" {
#endif

// ---------------------------------------------------------------------------

#define DSI_CHECK_RET(expr)             \
    do {                                \
        DSI_STATUS ret = (expr);        \
        ASSERT(DSI_STATUS_OK == ret);   \
    } while (0)

// ---------------------------------------------------------------------------

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

// ESD recovery method for video mode LCM
#define     METHOD_NONCONTINUOUS_CLK            (0x1)
#define     METHOD_BUS_TURN_AROUND              (0x2)

// State of DSI engine
#define     DSI_VDO_VSA_VS_STATE                (0x008)
#define     DSI_VDO_VSA_HS_STATE                (0x010)
#define     DSI_VDO_VSA_VE_STATE                (0x020)
#define     DSI_VDO_VBP_STATE                   (0x040)
#define     DSI_VDO_VACT_STATE                  (0x080)
#define     DSI_VDO_VFP_STATE                   (0x100)

// ---------------------------------------------------------------------------

typedef enum {
    DSI_STATUS_OK = 0,

    DSI_STATUS_ERROR,
}
DSI_STATUS;


typedef enum {
    SHORT_PACKET_RW = 0,
    FB_WRITE        = 1,
    LONG_PACKET_W   = 2,
    FB_READ         = 3,
} DSI_INS_TYPE;


typedef enum {
    DISABLE_BTA = 0,
    ENABLE_BTA  = 1,
} DSI_CMDQ_BTA;


typedef enum {
    LOW_POWER   = 0,
    HIGH_SPEED  = 1,
} DSI_CMDQ_HS;


typedef enum {
    CL_8BITS    = 0,
    CL_16BITS   = 1,
} DSI_CMDQ_CL;


typedef enum {
    DISABLE_TE  = 0,
    ENABLE_TE   = 1,
} DSI_CMDQ_TE;


typedef enum {
    DISABLE_RPT = 0,
    ENABLE_RPT  = 1,
} DSI_CMDQ_RPT;


typedef struct {
    unsigned        type    : 2;
    unsigned        BTA     : 1;
    unsigned        HS      : 1;
    unsigned        CL      : 1;
    unsigned        TE      : 1;
    unsigned        Rsv     : 1;
    unsigned        RPT     : 1;
} DSI_CMDQ_CONFG, *PDSI_CMDQ_CONFIG;


typedef struct {
    unsigned CONFG          : 8;
    unsigned Data_ID        : 8;
    unsigned Data0          : 8;
    unsigned Data1          : 8;
} DSI_T0_INS, *PDSI_T0_INS;

typedef struct {
    unsigned CONFG          : 8;
    unsigned Data_ID        : 8;
    unsigned mem_start0     : 8;
    unsigned mem_start1     : 8;
} DSI_T1_INS, *PDSI_T1_INS;

typedef struct {
    unsigned CONFG          : 8;
    unsigned Data_ID        : 8;
    unsigned WC16           : 16;
    unsigned int *pdata;
} DSI_T2_INS, *PDSI_T2_INS;

typedef struct {
    unsigned CONFG          : 8;
    unsigned Data_ID        : 8;
    unsigned mem_start0     : 8;
    unsigned mem_start1     : 8;
} DSI_T3_INS, *PDSI_T3_INS;

typedef struct {
    u8 TXDIV0;
    u8 TXDIV1;
    u32  SDM_PCW;
    u8 SSC_PH_INIT;
    u16  SSC_PRD;
    u16  SSC_DELTA1;
    u16  SSC_DELTA;
} DSI_PLL_CONFIG;

DSI_STATUS DSI_Init(BOOL isDsiPoweredOn);
DSI_STATUS DSI_Deinit(void);

DSI_STATUS DSI_PowerOn(void);
DSI_STATUS DSI_PowerOff(void);

DSI_STATUS DSI_WaitForNotBusy(void);

DSI_STATUS DSI_StartTransfer(BOOL needStartDSI);
unsigned int DSI_Detect_CLK_Glitch(void);
DSI_STATUS DSI_Reset(void);
DSI_STATUS DSI_LP_Reset(void);
DSI_STATUS DSI_SetMode(unsigned int mode);

DSI_STATUS DSI_EnableInterrupt(DISP_INTERRUPT_EVENTS eventID);
DSI_STATUS DSI_SetInterruptCallback(void (*pCB)(DISP_INTERRUPT_EVENTS eventID));

DSI_STATUS DSI_handle_TE(void);


DSI_STATUS DSI_Write_T0_INS(DSI_T0_INS *t0);
DSI_STATUS DSI_Write_T1_INS(DSI_T1_INS *t1);
DSI_STATUS DSI_Write_T2_INS(DSI_T2_INS *t2);
DSI_STATUS DSI_Write_T3_INS(DSI_T3_INS *t3);

DSI_STATUS DSI_TXRX_Control(BOOL cksm_en,
                            BOOL ecc_en,
                            unsigned char lane_num,
                            unsigned char vc_num,
                            BOOL null_packet_en,
                            BOOL err_correction_en,
                            BOOL dis_eotp_en,
                            BOOL hstx_cklp_en,
                            unsigned int  max_return_size);
//#ifndef MT65XX_NEW_DISP
#if 0
DSI_STATUS DSI_PS_Control(unsigned int ps_type, unsigned int ps_wc);
#else
DSI_STATUS DSI_PS_Control(unsigned int ps_type, unsigned int vact_line, unsigned int ps_wc);
#endif

DSI_STATUS DSI_ConfigVDOTiming(unsigned int dsi_vsa_nl,
                               unsigned int dsi_vbp_nl,
                               unsigned int dsi_vfp_nl,
                               unsigned int dsi_vact_nl,
                               unsigned int dsi_line_nb,
                               unsigned int dsi_hsa_nb,
                               unsigned int dsi_hbp_nb,
                               unsigned int dsi_hfp_nb,
                               unsigned int dsi_rgb_nb,
                               unsigned int dsi_hsa_wc,
                               unsigned int dsi_hbp_wc,
                               unsigned int dsi_hfp_wc);


//void init_mipi_pll(void);
void DSI_Config_VDO_Timing(LCM_PARAMS *lcm_params);

//#ifndef MT65XX_NEW_DISP
void DSI_PHY_clk_setting(LCM_PARAMS *lcm_params);
//void DSI_PHY_clk_setting(unsigned int div1, unsigned int div2, unsigned int lane_no);
//#endif
void DSI_PHY_clk_switch(BOOL on);

void DSI_PHY_TIMCONFIG(LCM_PARAMS *lcm_params);

void DSI_clk_ULP_mode(BOOL enter);
void DSI_clk_HS_mode(BOOL enter);
BOOL DSI_clk_HS_state(void);
void DSI_lane0_ULP_mode(BOOL enter);

BOOL DSI_esd_check(void);
void DSI_handle_esd_recovery(void);
void DSI_set_int_TE(BOOL enable, unsigned int period);
BOOL DSI_handle_int_TE(void);
void DSI_set_noncont_clk(BOOL enable, unsigned int period);
void DSI_handle_noncont_clk(void);
void DSI_set_cmdq_V3(LCM_setting_table_V3 *para_tbl, unsigned int size, unsigned char force_update);
void DSI_set_cmdq_V2(unsigned cmd, unsigned char count, unsigned char *para_list, unsigned char force_update);
void DSI_set_cmdq(unsigned int *pdata, unsigned int queue_size, unsigned char force_update);
void DSI_write_lcm_cmd(unsigned int cmd);
void DSI_write_lcm_regs(unsigned int addr, unsigned int *para, unsigned int nums);
u32 DSI_read_lcm_reg(void);
u32 DSI_dcs_read_lcm_reg(unsigned char);
u32 DSI_dcs_read_lcm_reg_v2(u8 cmd, u8 *buffer, u8 buffer_size);
DSI_STATUS DSI_write_lcm_fb(unsigned int addr, BOOL long_length);
DSI_STATUS DSI_read_lcm_fb(void);

DSI_STATUS DSI_enable_MIPI_txio(BOOL en);
BOOL Need_Wait_ULPS(void);

DSI_STATUS Wait_ULPS_Mode(void);
DSI_STATUS Wait_WakeUp(void);

//DSI_STATUS DSI_DumpRegisters(void);
//enum DSI_STATUS DSI_DumpRegisters(DISP_MODULE_ENUM module, int level);
//enum DSI_STATUS DSI_DumpRegisters(DISP_MODULE_ENUM module, void* cmdq, int level)

DSI_STATUS DSI_FMDesense_Query(void);
DSI_STATUS DSI_FM_Desense(unsigned long freq);
DSI_STATUS DSI_Reset_CLK(void);
DSI_STATUS DSI_Get_Default_CLK(unsigned int *clk);
DSI_STATUS DSI_Get_Current_CLK(unsigned int *clk);
DSI_STATUS DSI_Change_CLK(unsigned int clk);

void DSI_WaitBtaTE(void);
DSI_STATUS DSI_WaitVsync(void);
#ifdef __cplusplus
}
#endif

