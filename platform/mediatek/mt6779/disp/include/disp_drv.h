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

#ifdef __cplusplus
extern "C" {
#endif

// ---------------------------------------------------------------------------

#define DISP_CHECK_RET(expr)                                                \
    do {                                                                    \
        DISP_STATUS ret = (expr);                                           \
        if (DISP_STATUS_OK != ret) {                                        \
            DISP_LOG_PRINT(ANDROID_LOG_ERROR, "COMMON", "[ERROR][mtkfb] DISP API return error code: 0x%x\n"      \
                   "  file : %s, line : %d\n"                               \
                   "  expr : %s\n", ret, __FILE__, __LINE__, #expr);        \
        }                                                                   \
    } while (0)


// ---------------------------------------------------------------------------

typedef enum {
    DISP_STATUS_OK = 0,

    DISP_STATUS_NOT_IMPLEMENTED,
    DISP_STATUS_ALREADY_SET,
    DISP_STATUS_ERROR,
}
DISP_STATUS;


typedef enum {
    DISP_STATE_IDLE = 0,
    DISP_STATE_BUSY,
} DISP_STATE;


#define MAKE_PANEL_COLOR_FORMAT(R, G, B) ((R << 16) | (G << 8) | B)
#define PANEL_COLOR_FORMAT_TO_BPP(x) ((x&0xff) + ((x>>8)&0xff) + ((x>>16)&0xff))

typedef enum {
    PANEL_COLOR_FORMAT_RGB332 = MAKE_PANEL_COLOR_FORMAT(3, 3, 2),
    PANEL_COLOR_FORMAT_RGB444 = MAKE_PANEL_COLOR_FORMAT(4, 4, 4),
    PANEL_COLOR_FORMAT_RGB565 = MAKE_PANEL_COLOR_FORMAT(5, 6, 5),
    PANEL_COLOR_FORMAT_RGB666 = MAKE_PANEL_COLOR_FORMAT(6, 6, 6),
    PANEL_COLOR_FORMAT_RGB888 = MAKE_PANEL_COLOR_FORMAT(8, 8, 8),
} PANEL_COLOR_FORMAT;

typedef enum {
    DISP_LCD_INTERRUPT_EVENTS_START     = 0x01,
    DISP_LCD_TRANSFER_COMPLETE_INT      = 0x01,
    DISP_LCD_REG_COMPLETE_INT           = 0x02,
    DISP_LCD_CDMQ_COMPLETE_INT          = 0x03,
    DISP_LCD_HTT_INT                    = 0x04,
    DISP_LCD_SYNC_INT                   = 0x05,
    DISP_LCD_INTERRUPT_EVENTS_END       = 0x05,


    DISP_DSI_INTERRUPT_EVENTS_START     = 0x11,
    DISP_DSI_READ_RDY_INT               = 0x11,
    DISP_DSI_CMD_DONE_INT               = 0x12,
    DISP_DSI_INTERRUPT_EVENTS_END       = 0x12,

    DISP_DPI_INTERRUPT_EVENTS_START     = 0x21,
    DISP_DPI_FIFO_EMPTY_INT             = 0x21,
    DISP_DPI_FIFO_FULL_INT              = 0x22,
    DISP_DPI_OUT_EMPTY_INT              = 0x23,
    DISP_DPI_CNT_OVERFLOW_INT           = 0x24,
    DISP_DPI_LINE_ERR_INT               = 0x25,
    DISP_DPI_VSYNC_INT                  = 0x26,
    DISP_DPI_INTERRUPT_EVENTS_END       = 0x26,
} DISP_INTERRUPT_EVENTS;

#define DISP_LCD_INTERRUPT_EVENTS_NUMBER (DISP_LCD_INTERRUPT_EVENTS_END - DISP_LCD_INTERRUPT_EVENTS_START + 1)
#define DISP_DSI_INTERRUPT_EVENTS_NUMBER (DISP_DSI_INTERRUPT_EVENTS_END - DISP_DSI_INTERRUPT_EVENTS_START + 1)
#define DISP_DPI_INTERRUPT_EVENTS_NUMBER (DISP_DPI_INTERRUPT_EVENTS_END - DISP_DPI_INTERRUPT_EVENTS_START + 1)

typedef void (*DISP_INTERRUPT_CALLBACK_PTR)(void *params);

typedef struct {
    DISP_INTERRUPT_CALLBACK_PTR pFunc;
    void *pParam;
} DISP_INTERRUPT_CALLBACK_STRUCT;

// ---------------------------------------------------------------------------
// Public Functions
// ---------------------------------------------------------------------------

DISP_STATUS DISP_Init(u32 fbVA, u32 fbPA, bool isLcmInited);
DISP_STATUS DISP_Deinit(void);
DISP_STATUS DISP_PowerEnable(bool enable);
DISP_STATUS DISP_PanelEnable(bool enable);
DISP_STATUS DISP_LCDPowerEnable(bool enable);   ///only used to power on LCD for memory out
DISP_STATUS DISP_SetFrameBufferAddr(u32 fbPhysAddr);
DISP_STATUS DISP_EnterOverlayMode(void);
DISP_STATUS DISP_LeaveOverlayMode(void);
DISP_STATUS DISP_EnableDirectLinkMode(u32 layer);
DISP_STATUS DISP_DisableDirectLinkMode(u32 layer);
DISP_STATUS DISP_UpdateScreen(u32 x, u32 y, u32 width, u32 height);
DISP_STATUS DISP_SetInterruptCallback(DISP_INTERRUPT_EVENTS eventID, DISP_INTERRUPT_CALLBACK_STRUCT *pCBStruct);
DISP_STATUS DISP_WaitForLCDNotBusy(void);


///TODO: implement it and replace LCD_LayerXXX for mtkfb.c
typedef enum {
    DISP_SET_LAYER_ENABLE = 1,          ///type: bool
    DISP_GET_LAYER_ENABLE,
    DISP_SET_LAYER_ADDRESS,             ///type: u32
    DISP_GET_LAYER_ADDRESS,
    DISP_SET_LAYER_FORMAT,              ///type:
    DISP_GET_LAYER_FORMAT,
    DISP_SET_LAYER_ALPHA_BLENDING,
    DISP_GET_LAYER_ALPHA_BLENDING,
    DISP_SET_LAYER_SIZE,
    DISP_GET_LAYER_SIZE,
    DISP_SET_LAYER_PITCH,
    DISP_GET_LAYER_PITCH,
    DISP_SET_LAYER_OFFSET,
    DISP_GET_LAYER_OFFSET,
    DISP_SET_LAYER_ROTATION,
    DISP_GET_LAYER_ROTATION,
    DISP_SET_LAYER_SOURCE_KEY,
    DISP_GET_LAYER_SOURCE_KEY,
    DISP_SET_LAYER_3D,
    DISP_GET_LAYER_3D,
    DISP_SET_LAYER_DITHER_EN,
    DISP_GET_LAYER_DITHER_EN,
    DISP_SET_LAYER_DITHER_CONFIG,
    DISP_GET_LAYER_DITHER_CONFIG,
} DISP_LAYER_CONTROL_CODE_ENUM;
DISP_STATUS DISP_LayerControl(DISP_LAYER_CONTROL_CODE_ENUM code, u32 layer_id, void *param, u32 *param_len);

DISP_STATUS DISP_ConfigDither(int lrs, int lgs, int lbs, int dbr, int dbg, int dbb);


// Retrieve Information
u32 DISP_GetScreenWidth(void);
u32 DISP_GetScreenHeight(void);
u32 DISP_GetScreenBpp(void);
u32 DISP_GetPages(void);
DISP_STATUS DISP_SetScreenBpp(u32);   ///config how many bits for each pixel of framebuffer
DISP_STATUS DISP_SetPages(u32);         ///config how many framebuffer will be used
///above information is used to determine the vRAM size

bool   DISP_IsDirectLinkMode(void);
bool   DISP_IsInOverlayMode(void);
u32 DISP_GetFBRamSize(void);         ///get FB buffer size
u32 DISP_GetVRamSize(void);          /// get total RAM size (FB+working buffer+DAL buffer)
PANEL_COLOR_FORMAT DISP_GetPanelColorFormat(void);
u32 DISP_GetPanelBPP(void);
bool DISP_IsLcmFound(void);
bool DISP_IsImmediateUpdate(void);
DISP_STATUS DISP_ConfigImmediateUpdate(bool enable);

DISP_STATUS DISP_SetBacklight(u32 level);
DISP_STATUS DISP_SetPWM(u32 divider);
DISP_STATUS DISP_GetPWM(u32 divider, unsigned int *freq);
DISP_STATUS DISP_SetBacklight_mode(u32 mode);

DISP_STATUS DISP_Set3DPWM(bool enable, bool landscape);

// FM De-sense
DISP_STATUS DISP_FMDesense_Query(void);
DISP_STATUS DISP_FM_Desense(unsigned long);
DISP_STATUS DISP_Reset_Update(void);
DISP_STATUS DISP_Get_Default_UpdateSpeed(unsigned int *speed);
DISP_STATUS DISP_Get_Current_UpdateSpeed(unsigned int *speed);
DISP_STATUS DISP_Change_Update(unsigned int);
///////////////

DISP_STATUS DISP_InitM4U(void);
DISP_STATUS DISP_ConfigAssertLayerMva(void);
DISP_STATUS DISP_AllocUILayerMva(unsigned int pa, unsigned int *mva, unsigned int size);
DISP_STATUS DISP_AllocOverlayMva(unsigned int va, unsigned int *mva, unsigned int size);
DISP_STATUS DISP_DeallocMva(unsigned int va, unsigned int mva, unsigned int size);
DISP_STATUS DISP_DumpM4U(void);

// ---------------------------------------------------------------------------
// Private Functions
// ---------------------------------------------------------------------------

typedef struct {
    DISP_STATUS (*init)(u32 fbVA, u32 fbPA, bool isLcmInited);
    DISP_STATUS (*enable_power)(bool enable);
    DISP_STATUS (*update_screen)(void);

    u32 (*get_working_buffer_size)(void);
    u32 (*get_working_buffer_bpp)(void);
    PANEL_COLOR_FORMAT (*get_panel_color_format)(void);
    void (*init_te_control)(void);
    u32 (*get_dithering_bpp)(void);

    DISP_STATUS (*capture_framebuffer)(unsigned int pvbuf, unsigned int bpp);

    void (*esd_reset)(void);
    bool (*esd_check)(void);
} DISP_DRIVER;


const DISP_DRIVER *DISP_GetDriverDBI(void);
const DISP_DRIVER *DISP_GetDriverDPI(void);
const DISP_DRIVER *DISP_GetDriverDSI(void);


bool DISP_SelectDevice(const char *lcm_name);
bool DISP_DetectDevice(void);
bool DISP_SelectDeviceBoot(const char *lcm_name);
u32 DISP_GetVRamSizeBoot(char *cmdline);
DISP_STATUS DISP_Capture_Framebuffer(unsigned int pvbuf, unsigned int bpp);
bool DISP_IsContextInited(void);

DISP_STATUS DISP_Capture_Videobuffer(unsigned int pvbuf, unsigned int bpp, unsigned int video_rotation);
u32 DISP_GetOutputBPPforDithering(void);
bool DISP_IsLCDBusy(void);
DISP_STATUS DISP_ChangeLCDWriteCycle(void);
DISP_STATUS DISP_M4U_On(bool enable);
const char *DISP_GetLCMId(void);

bool DISP_EsdRecoverCapbility(void);
bool DISP_EsdCheck(void);
bool DISP_EsdRecover(void);
void DISP_WaitVSYNC(void);
void DISP_InitVSYNC(unsigned int vsync_interval);
DISP_STATUS DISP_PauseVsync(bool enable);
void DISP_Change_LCM_Resolution(unsigned int width, unsigned int height);

unsigned long DISP_GetLCMIndex(void);
// ---------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

