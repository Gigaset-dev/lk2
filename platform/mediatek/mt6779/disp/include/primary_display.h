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

#include "ddp_hal.h"
#include "ddp_manager.h"
//#include "disp_drv_platform.h"
#include "lcm_drv.h"

typedef enum {
    DIRECT_LINK_MODE,
    DECOUPLE_MODE,
    SINGLE_LAYER_MODE,
    DEBUG_RDMA1_DSI0_MODE
} DISP_PRIMARY_PATH_MODE;


// ---------------------------------------------------------------------------

#define DISP_RET_0_IF_NULL(x)           \
    do {                                 \
        if ((x) == NULL) { \
            DISPERR("%s is NULL, return 0\n", #x); \
            return 0; \
        } \
    } while (0)


#define DISP_RET_VOID_IF_NULL(x)            \
    do {                                 \
        if ((x) == NULL) { \
            DISPERR("%s is NULL, return 0\n", #x); \
            return; \
        } \
    } while (0)

#define DISP_RET_VOID_IF_0(x)           \
    do {                                 \
        if ((x) == 0) { \
            DISPERR("%s is NULL, return 0\n", #x); \
            return; \
        } \
    } while (0)

#define DISP_RET_0_IF_0(x)          \
        do {                                 \
            if ((x) == 0) { \
                DISPERR("%s is NULL, return 0\n", #x); \
                return 0; \
            } \
        } while (0)

#define DISP_RET_NULL_IF_0(x)           \
    do {                                 \
        if ((x) == 0) { \
            DISPERR("%s is NULL, return 0\n", #x); \
            return NULL; \
        } \
    } while (0)


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

#define ASSERT_LAYER    (DDP_OVL_LAYER_MUN-1)
extern unsigned int FB_LAYER;    // default LCD layer
#define DISP_DEFAULT_UI_LAYER_ID (DDP_OVL_LAYER_MUN-1)
#define DISP_CHANGED_UI_LAYER_ID (DDP_OVL_LAYER_MUN-2)

typedef struct {
    unsigned int id;
    unsigned int curr_en;
    unsigned int next_en;
    unsigned int hw_en;
    int curr_idx;
    int next_idx;
    int hw_idx;
    int curr_identity;
    int next_identity;
    int hw_identity;
    int curr_conn_type;
    int next_conn_type;
    int hw_conn_type;
} DISP_LAYER_INFO;

typedef enum {
    DISP_STATUS_OK = 0,

    DISP_STATUS_NOT_IMPLEMENTED,
    DISP_STATUS_ALREADY_SET,
    DISP_STATUS_ERROR,
} DISP_STATUS;


typedef enum {
    DISP_STATE_IDLE = 0,
    DISP_STATE_BUSY,
} DISP_STATE;

typedef enum {
    DISP_OP_PRE = 0,
    DISP_OP_NORMAL,
    DISP_OP_POST,
} DISP_OP_STATE;



typedef enum {
    DISPLAY_HAL_IOCTL_SET_CMDQ = 0xff00,
    DISPLAY_HAL_IOCTL_ENABLE_CMDQ,
    DISPLAY_HAL_IOCTL_DUMP,
    DISPLAY_HAL_IOCTL_PATTERN,
} DISPLAY_HAL_IOCTL;

typedef struct {
    unsigned int layer;
    unsigned int layer_en;
    unsigned int fmt;
    unsigned int addr;
    unsigned int addr_sub_u;
    unsigned int addr_sub_v;
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
} disp_input_config;

int primary_display_init(char *lcm_name, void *fdt);
int primary_display_config(unsigned int pa, unsigned int mva);
int primary_display_suspend(void);
int primary_display_resume(void);


int primary_display_get_width(void);
int primary_display_get_height(void);
int primary_display_get_bpp(void);
int primary_display_get_pages(void);

int primary_display_set_overlay_layer(disp_input_config *input);
int primary_display_is_alive (void);
int primary_display_is_sleepd(void);
int priamry_display_wait_for_vsync(void);
int primary_display_config_input(disp_input_config *input);
int primary_display_trigger(int blocking);
int primary_display_diagnose(void);
const char *primary_display_get_lcm_name (void);

int primary_display_get_info(void *dispif_info);
int primary_display_capture_framebuffer(unsigned int *pbuf);
u32 DISP_GetVRamSize(void);
u32 DISP_GetFBRamSize(void);
u32 DISP_GetPages(void);
u32 DISP_GetScreenBpp(void);
u32 DISP_GetScreenWidth(void);
u32 DISP_GetScreenHeight(void);
u32 DISP_GetActiveHeight(void);
u32 DISP_GetActiveWidth(void);
int disp_hal_allocate_framebuffer(unsigned int pa_start, unsigned int pa_end, unsigned int *va, unsigned int *mva);
int primary_display_is_video_mode (void);
int primary_display_get_vsync_interval(void);
int primary_display_setbacklight(unsigned int level);
int primary_display_is_lcm_connected(void);
#if defined(MTK_ROUND_CORNER_SUPPORT) && !defined(DISP_HW_RC)
LCM_ROUND_CORNER *primary_display_get_corner_params(void);
#endif

