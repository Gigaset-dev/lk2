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

typedef enum {
    EXT_DIRECT_LINK_MODE,
    EXT_DECOUPLE_MODE,
    EXT_SINGLE_LAYER_MODE,
    EXT_DEBUG_RDMA1_DSI1_MODE
} DISP_EXTERNAL_PATH_MODE;



int external_display_init(char *lcm_name);
int external_display_config(unsigned int pa, unsigned int mva);
int external_display_suspend(void);
int external_display_resume(void);


int external_display_get_width(void);
int external_display_get_height(void);
int external_display_get_bpp(void);
int external_display_get_pages(void);

int external_display_set_overlay_layer(disp_input_config *input);
int external_display_is_alive(void);
int external_display_is_sleepd(void);
int external_display_wait_for_vsync(void);
int external_display_config_input(disp_input_config *input);
int external_display_trigger(int blocking);
int external_display_diagnose(void);
const char *external_display_get_lcm_name(void);

int external_display_get_info(void *dispif_info);
int external_display_capture_framebuffer(unsigned int *pbuf);
u32 EXT_DISP_GetVRamSize(void);
u32 EXT_DISP_GetFBRamSize(void);
u32 EXT_DISP_GetPages(void);
u32 EXT_DISP_GetScreenBpp(void);
u32 EXT_DISP_GetScreenWidth(void);
u32 EXT_DISP_GetScreenHeight(void);
u32 EXT_DISP_GetActiveHeight(void);
u32 EXT_DISP_GetActiveWidth(void);
int disp_hal_allocate_framebuffer(unsigned int pa_start, unsigned int pa_end, unsigned int *va, unsigned int *mva);
int external_display_is_video_mode(void);
int external_display_get_vsync_interval(void);
int external_display_setbacklight(unsigned int level);

