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
#include "lcm_common.h"

#define PIN_ENCRYPT 0x80000000
#define DEVICE_TREE_SUPPORT 1

int lcm_util_get_i2c_lcd_bias(int *id, int *addr, void *fdt);
int lcm_util_get_pin(const char *pin_name, void *fdt);

#if defined(MTK_LCM_DEVICE_TREE_SUPPORT)
LCM_STATUS lcm_util_set_data(const LCM_UTIL_FUNCS *lcm_util, char type, LCM_DATA_T1 *t1);
LCM_STATUS lcm_util_set_write_cmd_v1(const LCM_UTIL_FUNCS *lcm_util, LCM_DATA_T5 *t5,
                                     unsigned char force_update);
LCM_STATUS lcm_util_set_write_cmd_v2(const LCM_UTIL_FUNCS *lcm_util, LCM_DATA_T3 *t3,
                                     unsigned char force_update);
LCM_STATUS lcm_util_set_write_cmd_v23(const LCM_UTIL_FUNCS *lcm_util, void *handle,
                                      LCM_DATA_T3 *t3, unsigned char force_update);
LCM_STATUS lcm_util_set_read_cmd_v2(const LCM_UTIL_FUNCS *lcm_util, LCM_DATA_T4 *t4,
                                    unsigned int *compare);
#endif

