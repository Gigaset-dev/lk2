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

#if defined(MTK_LCM_DEVICE_TREE_SUPPORT)
/* LCM_FUNC */
#define LCM_FUNC_GPIO   1
#define LCM_FUNC_I2C    2
#define LCM_FUNC_UTIL   3
#define LCM_FUNC_CMD    4

/* LCM_GPIO_TYPE */
#define LCM_GPIO_MODE   1
#define LCM_GPIO_DIR    2
#define LCM_GPIO_OUT    3

/* LCM_GPIO_MODE_DATA */
#define LCM_GPIO_MODE_00    0
#define LCM_GPIO_MODE_01    1
#define LCM_GPIO_MODE_02    2
#define LCM_GPIO_MODE_03    3
#define LCM_GPIO_MODE_04    4
#define LCM_GPIO_MODE_05    5
#define LCM_GPIO_MODE_06    6
#define LCM_GPIO_MODE_07    7
#define MAX_LCM_GPIO_MODE    8

/* LCM_GPIO_DIR_DATA */
#define LCM_GPIO_DIR_IN 0
#define LCM_GPIO_DIR_OUT    1

/* LCM_GPIO_OUT_DATA */
#define LCM_GPIO_OUT_ZERO   0
#define LCM_GPIO_OUT_ONE    1

/* LCM_I2C_TYPE */
#define LCM_I2C_WRITE   1

/* LCM_UTIL_TYPE */
#define LCM_UTIL_RESET  1
#define LCM_UTIL_MDELAY 2
#define LCM_UTIL_UDELAY 3
#define LCM_UTIL_WRITE_CMD_V1   4
#define LCM_UTIL_WRITE_CMD_V2   5
#define LCM_UTIL_READ_CMD_V1    6
#define LCM_UTIL_READ_CMD_V2    7
#define LCM_UTIL_WRITE_CMD_V21  8
#define LCM_UTIL_WRITE_CMD_V22  9
#define LCM_UTIL_WRITE_CMD_V23  10

/* LCM_UTIL_RESET_DATA */
#define LCM_UTIL_RESET_LOW  0
#define LCM_UTIL_RESET_HIGH 1
#endif

