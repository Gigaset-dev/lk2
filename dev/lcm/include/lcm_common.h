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

#if defined(MTK_LCM_DEVICE_TREE_SUPPORT)
typedef enum {
    LCM_STATUS_OK = 0,
    LCM_STATUS_ERROR,
} LCM_STATUS;


void lcm_common_parse_dts(const LCM_DTS *DTS, unsigned char force_update);
void lcm_common_set_util_funcs(const LCM_UTIL_FUNCS *util);
void lcm_common_get_params(LCM_PARAMS *params);
void lcm_common_init(void);
void lcm_common_suspend(void);
void lcm_common_resume(void);
void lcm_common_update(unsigned int x, unsigned int y, unsigned int width, unsigned int height);
void lcm_common_setbacklight(unsigned int level);
void lcm_common_setbacklight_cmdq(void *handle, unsigned int level);
unsigned int lcm_common_compare_id(void);
unsigned int lcm_common_ata_check(unsigned char *buffer);
#endif

