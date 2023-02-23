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

#include "disp_drv.h"
#include "lcm_drv.h"
#ifdef __cplusplus
extern "C" {
#endif

// ---------------------------------------------------------------------------

#define DPI_CHECK_RET(expr)             \
    do {                                \
        DPI_STATUS ret = (expr);        \
        ASSERT(DPI_STATUS_OK == ret);   \
    } while (0)

// ---------------------------------------------------------------------------

typedef enum {
    DPI_STATUS_OK = 0,

    DPI_STATUS_ERROR,
}
DPI_STATUS;


typedef enum {
    DPI_FB_FORMAT_RGB565 = 0,
    DPI_FB_FORMAT_RGB888 = 1,
    DPI_FB_FORMAT_XRGB888 = 1,
    DPI_FB_FORMAT_RGBX888 = 1,
    DPI_FB_FORMAT_NUM,
} DPI_FB_FORMAT;


typedef enum {
    DPI_RGB_ORDER_RGB = 0,
    DPI_RGB_ORDER_BGR = 1,
} DPI_RGB_ORDER;


typedef enum {
    DPI_FB_0   = 0,
    DPI_FB_1   = 1,
    DPI_FB_2   = 2,
    DPI_FB_NUM,
} DPI_FB_ID;


typedef enum {
    DPI_POLARITY_RISING  = 0,
    DPI_POLARITY_FALLING = 1
} DPI_POLARITY;

// ---------------------------------------------------------------------------

DPI_STATUS DPI_Init(BOOL isDpiPoweredOn);
DPI_STATUS DPI_Deinit(void);

DPI_STATUS DPI_Init_PLL(unsigned int mipi_pll_clk_ref, unsigned int mipi_pll_clk_div1, unsigned int mipi_pll_clk_div2);
DPI_STATUS DPI_Set_DrivingCurrent(LCM_PARAMS *lcm_params);

DPI_STATUS DPI_PowerOn(void);
DPI_STATUS DPI_PowerOff(void);

DPI_STATUS DPI_EnableClk(void);
DPI_STATUS DPI_DisableClk(void);

DPI_STATUS DPI_EnableSeqOutput(BOOL enable);
DPI_STATUS DPI_SetRGBOrder(DPI_RGB_ORDER input, DPI_RGB_ORDER output);

DPI_STATUS DPI_ConfigPixelClk(DPI_POLARITY polarity, u32 divisor, u32 duty);
DPI_STATUS DPI_ConfigDataEnable(DPI_POLARITY polarity);
DPI_STATUS DPI_ConfigVsync(DPI_POLARITY polarity,
                           u32 pulseWidth, u32 backPorch, u32 frontPorch);
DPI_STATUS DPI_ConfigHsync(DPI_POLARITY polarity,
                           u32 pulseWidth, u32 backPorch, u32 frontPorch);

DPI_STATUS DPI_FBSyncFlipWithLCD(BOOL enable);
DPI_STATUS DPI_SetDSIMode(BOOL enable);
BOOL       DPI_IsDSIMode(void);
DPI_STATUS DPI_FBSetFormat(DPI_FB_FORMAT format);
DPI_FB_FORMAT DPI_FBGetFormat(void);
DPI_STATUS DPI_FBSetSize(u32 width, u32 height);
DPI_STATUS DPI_FBEnable(DPI_FB_ID id, BOOL enable);
DPI_STATUS DPI_FBSetAddress(DPI_FB_ID id, u32 address);
DPI_STATUS DPI_FBSetPitch(DPI_FB_ID id, u32 pitchInByte);

DPI_STATUS DPI_SetFifoThreshold(u32 low, u32 high);

// Debug
DPI_STATUS DPI_DumpRegisters(void);

DPI_STATUS DPI_Capture_Framebuffer(unsigned int pvbuf, unsigned int bpp);

//FM De-sense
DPI_STATUS DPI_FMDesense_Query(void);
DPI_STATUS DPI_FM_Desense(unsigned long freq);
DPI_STATUS DPI_Get_Default_CLK(unsigned int *clk);
DPI_STATUS DPI_Get_Current_CLK(unsigned int *clk);
DPI_STATUS DPI_Change_CLK(unsigned int clk);
DPI_STATUS DPI_Reset_CLK(void);

void DPI_mipi_switch(BOOL on);
void DPI_DisableIrq(void);
void DPI_EnableIrq(void);
DPI_STATUS DPI_FreeIRQ(void);

DPI_STATUS DPI_EnableInterrupt(DISP_INTERRUPT_EVENTS eventID);
DPI_STATUS DPI_SetInterruptCallback(void (*pCB)(DISP_INTERRUPT_EVENTS eventID));
void DPI_WaitVSYNC(void);
void DPI_InitVSYNC(unsigned int vsync_interval);
void DPI_PauseVSYNC(BOOL enable);

DPI_STATUS DPI_ConfigLVDS(LCM_PARAMS *lcm_params);
DPI_STATUS DPI_WaitVsync(void);
// ---------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

