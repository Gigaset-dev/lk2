/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

//#include <platform/mt_typedefs.h>
#include <stdlib.h>

#include "disp_drv_log.h"

#ifdef __cplusplus
extern "C" {
#endif

// ---------------------------------------------------------------------------

#define DISP_CHECK_RET(expr)                                                \
    do {                                                                    \
        enum DISP_STATUS ret = (expr);                                           \
        if (ret != DISP_STATUS_OK) {                                        \
            DISP_LOG_PRINT(ANDROID_LOG_ERROR, "COMMON",                     \
                   "[ERROR][mtkfb] DISP API return error code: 0x%x\n"      \
                   "  file : %s, line : %d\n"                               \
                   "  expr : %s\n", ret, __FILE__, __LINE__, #expr);        \
        }                                                                   \
    } while (0)


// ---------------------------------------------------------------------------

enum DISP_STATUS {
    DISP_STATUS_OK = 0,

    DISP_STATUS_NOT_IMPLEMENTED,
    DISP_STATUS_ALREADY_SET,
    DISP_STATUS_ERROR,
};


enum DISP_STATE {
    DISP_STATE_IDLE = 0,
    DISP_STATE_BUSY,
};


#define MAKE_PANEL_COLOR_FORMAT(R, G, B) ((R << 16) | (G << 8) | B)
#define PANEL_COLOR_FORMAT_TO_BPP(x) ((x&0xff) + ((x>>8)&0xff) + ((x>>16)&0xff))

enum PANEL_COLOR_FORMAT {
    PANEL_COLOR_FORMAT_RGB332 = MAKE_PANEL_COLOR_FORMAT(3, 3, 2),
    PANEL_COLOR_FORMAT_RGB444 = MAKE_PANEL_COLOR_FORMAT(4, 4, 4),
    PANEL_COLOR_FORMAT_RGB565 = MAKE_PANEL_COLOR_FORMAT(5, 6, 5),
    PANEL_COLOR_FORMAT_RGB666 = MAKE_PANEL_COLOR_FORMAT(6, 6, 6),
    PANEL_COLOR_FORMAT_RGB888 = MAKE_PANEL_COLOR_FORMAT(8, 8, 8),
};

enum DISP_INTERRUPT_EVENTS {
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
};

#define DISP_LCD_INTERRUPT_EVENTS_NUMBER                        \
        (DISP_LCD_INTERRUPT_EVENTS_END - DISP_LCD_INTERRUPT_EVENTS_START + 1)
#define DISP_DSI_INTERRUPT_EVENTS_NUMBER                        \
        (DISP_DSI_INTERRUPT_EVENTS_END - DISP_DSI_INTERRUPT_EVENTS_START + 1)
#define DISP_DPI_INTERRUPT_EVENTS_NUMBER                        \
        (DISP_DPI_INTERRUPT_EVENTS_END - DISP_DPI_INTERRUPT_EVENTS_START + 1)

typedef void (*DISP_INTERRUPT_CALLBACK_PTR)(void *params);

struct DISP_INTERRUPT_CALLBACK_STRUCT {
    DISP_INTERRUPT_CALLBACK_PTR pFunc;
    void *pParam;
};

// ---------------------------------------------------------------------------
// Public Functions
// ---------------------------------------------------------------------------

enum DISP_STATUS DISP_Init(u32 fbVA, u32 fbPA, bool isLcmInited);
enum DISP_STATUS DISP_Deinit(void);
enum DISP_STATUS DISP_PowerEnable(bool enable);
enum DISP_STATUS DISP_PanelEnable(bool enable);
enum DISP_STATUS DISP_LCDPowerEnable(bool enable);   ///only used to power on LCD for memory out
enum DISP_STATUS DISP_SetFrameBufferAddr(u32 fbPhysAddr);
enum DISP_STATUS DISP_EnterOverlayMode(void);
enum DISP_STATUS DISP_LeaveOverlayMode(void);
enum DISP_STATUS DISP_EnableDirectLinkMode(u32 layer);
enum DISP_STATUS DISP_DisableDirectLinkMode(u32 layer);
enum DISP_STATUS DISP_UpdateScreen(u32 x, u32 y, u32 width, u32 height);
enum DISP_STATUS DISP_SetInterruptCallback(enum DISP_INTERRUPT_EVENTS eventID,
                                        struct DISP_INTERRUPT_CALLBACK_STRUCT *pCBStruct);
enum DISP_STATUS DISP_WaitForLCDNotBusy(void);


///TODO: implement it and replace LCD_LayerXXX for mtkfb.c
enum DISP_LAYER_CONTROL_CODE_ENUM {
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
};
enum DISP_STATUS DISP_LayerControl(enum DISP_LAYER_CONTROL_CODE_ENUM code, u32 layer_id,
                                void *param, u32 *param_len);

enum DISP_STATUS DISP_ConfigDither(int lrs, int lgs, int lbs, int dbr, int dbg, int dbb);


// Retrieve Information
u32 DISP_GetScreenWidth(void);
u32 DISP_GetScreenHeight(void);
u32 DISP_GetScreenBpp(void);
u32 DISP_GetPages(void);
///above information is used to determine the vRAM size

bool   DISP_IsDirectLinkMode(void);
bool   DISP_IsInOverlayMode(void);
u32 DISP_GetFBRamSize(void);         ///get FB buffer size
u32 DISP_GetVRamSize(void);          /// get total RAM size (FB+working buffer+DAL buffer)
enum PANEL_COLOR_FORMAT DISP_GetPanelColorFormat(void);
u32 DISP_GetPanelBPP(void);
bool DISP_IsLcmFound(void);
bool DISP_IsImmediateUpdate(void);
enum DISP_STATUS DISP_ConfigImmediateUpdate(bool enable);

enum DISP_STATUS DISP_SetBacklight(u32 level);
enum DISP_STATUS DISP_SetPWM(u32 divider);
enum DISP_STATUS DISP_GetPWM(u32 divider, unsigned int *freq);
enum DISP_STATUS DISP_SetBacklight_mode(u32 mode);

enum DISP_STATUS DISP_Set3DPWM(bool enable, bool landscape);

// FM De-sense
enum DISP_STATUS DISP_FMDesense_Query(void);
enum DISP_STATUS DISP_Reset_Update(void);
enum DISP_STATUS DISP_Get_Default_UpdateSpeed(unsigned int *speed);
enum DISP_STATUS DISP_Get_Current_UpdateSpeed(unsigned int *speed);
///////////////

enum DISP_STATUS DISP_InitM4U(void);
enum DISP_STATUS DISP_ConfigAssertLayerMva(void);
enum DISP_STATUS DISP_AllocUILayerMva(unsigned int pa, unsigned int *mva, unsigned int size);
enum DISP_STATUS DISP_AllocOverlayMva(unsigned int va, unsigned int *mva, unsigned int size);
enum DISP_STATUS DISP_DeallocMva(unsigned int va, unsigned int mva, unsigned int size);
enum DISP_STATUS DISP_DumpM4U(void);

// ---------------------------------------------------------------------------
// Private Functions
// ---------------------------------------------------------------------------

struct DISP_DRIVER {
    enum DISP_STATUS (*init)(u32 fbVA, u32 fbPA, bool isLcmInited);
    enum DISP_STATUS (*enable_power)(bool enable);
    enum DISP_STATUS (*update_screen)(void);

    u32 (*get_working_buffer_size)(void);
    u32 (*get_working_buffer_bpp)(void);
    enum PANEL_COLOR_FORMAT (*get_panel_color_format)(void);
    void (*init_te_control)(void);
    u32 (*get_dithering_bpp)(void);

    enum DISP_STATUS (*capture_framebuffer)(unsigned int pvbuf, unsigned int bpp);

    void (*esd_reset)(void);
    bool (*esd_check)(void);
};


const struct DISP_DRIVER *DISP_GetDriverDBI(void);
const struct DISP_DRIVER *DISP_GetDriverDPI(void);
const struct DISP_DRIVER *DISP_GetDriverDSI(void);


bool DISP_SelectDevice(const char *lcm_name);
bool DISP_DetectDevice(void);
bool DISP_SelectDeviceBoot(const char *lcm_name);
u32 DISP_GetVRamSizeBoot(char *cmdline);
enum DISP_STATUS DISP_Capture_Framebuffer(unsigned int pvbuf, unsigned int bpp);
bool DISP_IsContextInited(void);

enum DISP_STATUS DISP_Capture_Videobuffer(unsigned int pvbuf, unsigned int bpp,
                                        unsigned int video_rotation);
u32 DISP_GetOutputBPPforDithering(void);
bool DISP_IsLCDBusy(void);
enum DISP_STATUS DISP_ChangeLCDWriteCycle(void);
enum DISP_STATUS DISP_M4U_On(bool enable);
const char *DISP_GetLCMId(void);

bool DISP_EsdRecoverCapbility(void);
bool DISP_EsdCheck(void);
bool DISP_EsdRecover(void);
void DISP_WaitVSYNC(void);
void DISP_InitVSYNC(unsigned int vsync_interval);
enum DISP_STATUS DISP_PauseVsync(bool enable);
void DISP_Change_LCM_Resolution(unsigned int width, unsigned int height);

unsigned long DISP_GetLCMIndex(void);
// ---------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif
