/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <lcm_drv.h>

enum DISP_PRIMARY_PATH_MODE {
    DIRECT_LINK_MODE,
    DECOUPLE_MODE,
    SINGLE_LAYER_MODE,
    DEBUG_RDMA1_DSI0_MODE
};


// ---------------------------------------------------------------------------
#define DISP_CHECK_RET(expr)                                                \
    do {                                                                    \
        enum DISP_STATUS ret = (expr);                                           \
        if (ret != DISP_STATUS_OK)                                        \
            DISP_LOG_PRINT(ANDROID_LOG_ERROR, "COMMON",                     \
                   "[ERROR][mtkfb] DISP API return error code: 0x%x\n"      \
                   "  file : %s, line : %d\n"                               \
                   "  expr : %s\n", ret, __FILE__, __LINE__, #expr);        \
    } while (0)


// ---------------------------------------------------------------------------

#define ASSERT_LAYER    (DDP_OVL_LAYER_MUN-1)
extern unsigned int FB_LAYER;    // default LCD layer
#define DISP_DEFAULT_UI_LAYER_ID (DDP_OVL_LAYER_MUN-1)
#define DISP_CHANGED_UI_LAYER_ID (DDP_OVL_LAYER_MUN-2)

struct DISP_LAYER_INFO {
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
};

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

enum DISP_OP_STATE {
    DISP_OP_PRE = 0,
    DISP_OP_NORMAL,
    DISP_OP_POST,
};

enum DISPLAY_HAL_IOCTL {
    DISPLAY_HAL_IOCTL_SET_CMDQ = 0xff00,
    DISPLAY_HAL_IOCTL_ENABLE_CMDQ,
    DISPLAY_HAL_IOCTL_DUMP,
    DISPLAY_HAL_IOCTL_PATTERN,
};

struct disp_input_config {
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
};

int primary_display_init(void *fdt);
int primary_display_sw_init(bool isLCMConnected, void *fdt, char *plcm_name);
int primary_display_lcm_probe(char *plcm_name, void *fdt);
int primary_display_get_panel_index(void);
int primary_display_get_is_cphy(void);
int primary_display_config(unsigned int pa, unsigned int mva);
int primary_display_suspend(void);
int primary_display_resume(void);

int primary_display_get_width(void);
int primary_display_get_align_width(void);
int primary_display_get_height(void);
int primary_display_get_align_height(void);
int primary_display_get_bpp(void);

int primary_display_set_overlay_layer(struct disp_input_config *input);
int primary_display_is_alive(void);
int primary_display_is_sleepd(void);
int priamry_display_wait_for_vsync(void);
int primary_display_config_input(struct disp_input_config *input);
int primary_display_trigger(int blocking);
void primary_display_diagnose(void);
const char *primary_display_get_lcm_name(void);

u32 DISP_GetVRamSize(void);
u32 DISP_GetFBRamSize(void);
u32 DISP_GetPages(void);
u32 DISP_GetScreenBpp(void);
u32 DISP_GetScreenWidth(void);
u32 DISP_GetScreenHeight(void);
int disp_hal_allocate_framebuffer(unsigned int pa_start, unsigned int pa_end,
                                    unsigned int *va, unsigned int *mva);
int primary_display_is_video_mode(void);
int primary_display_get_vsync_interval(void);
int primary_display_setbacklight(unsigned int level);
int primary_display_is_lcm_connected(void);
#if defined(MTK_ROUND_CORNER_SUPPORT) && !defined(DISP_HW_RC)
LCM_ROUND_CORNER *primary_display_get_corner_params(void);
#endif

extern void video_printf(const char *fmt, ...);
extern void disp_path_clock_on(const char *str);

