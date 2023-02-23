/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <lcm_drv.h>

#define MAX_LCM_NUMBER  2


struct disp_lcm_handle {
    LCM_PARAMS          *params;
    LCM_DRIVER          *drv;
    LCM_INTERFACE_ID    lcm_if_id;
    int                 module;
    int                 is_inited;
    int                 is_connected;
};

struct disp_lcm_handle *disp_lcm_probe(char *plcm_name, LCM_INTERFACE_ID lcm_id, void *fdt);
int disp_lcm_init(struct disp_lcm_handle *plcm, void *fdt);
LCM_PARAMS *disp_lcm_get_params(struct disp_lcm_handle *plcm);
LCM_INTERFACE_ID disp_lcm_get_interface_id(struct disp_lcm_handle *plcm);
int disp_lcm_update(struct disp_lcm_handle *plcm, int x, int y, int w, int h, int force);
int disp_lcm_esd_check(struct disp_lcm_handle *plcm);
int disp_lcm_esd_recover(struct disp_lcm_handle *plcm);
int disp_lcm_suspend(struct disp_lcm_handle *plcm);
int disp_lcm_resume(struct disp_lcm_handle *plcm);
int disp_lcm_set_backlight(struct disp_lcm_handle *plcm, int level);
int disp_lcm_read_fb(struct disp_lcm_handle *plcm);
int disp_lcm_ioctl(struct disp_lcm_handle *plcm, LCM_IOCTL ioctl, unsigned int arg);
int disp_lcm_is_video_mode(struct disp_lcm_handle *plcm);
int disp_lcm_is_inited(struct disp_lcm_handle *plcm);
const char *disp_lcm_get_name(struct disp_lcm_handle *plcm);
int disp_lcm_width(struct disp_lcm_handle *plcm);
int disp_lcm_height(struct disp_lcm_handle *plcm);
int disp_lcm_x(struct disp_lcm_handle *plcm);
int disp_lcm_y(struct disp_lcm_handle *plcm);

extern void *g_fdt;
extern unsigned char lcm_name_list[][128];
#if defined(MTK_LCM_DEVICE_TREE_SUPPORT)
extern LCM_DRIVER lcm_common_drv;
#endif

// these 2 variables are defined in mt65xx_lcm_list.c
extern LCM_DRIVER *lcm_driver_list[];
extern unsigned int lcm_count;


