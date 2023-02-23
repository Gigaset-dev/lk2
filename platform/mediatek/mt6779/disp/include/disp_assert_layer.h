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

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    DAL_STATUS_OK                = 0,

    DAL_STATUS_NOT_READY         = -1,
    DAL_STATUS_INVALID_ARGUMENT  = -2,
    DAL_STATUS_LOCK_FAIL         = -3,
    DAL_STATUS_LCD_IN_SUSPEND    = -4,
    DAL_STATUS_FATAL_ERROR       = -10,
}
DAL_STATUS;


typedef enum {
    DAL_COLOR_BLACK     = 0x000000,
    DAL_COLOR_WHITE     = 0xFFFFFF,
    DAL_COLOR_RED       = 0xFF0000,
    DAL_COLOR_GREEN     = 0x00FF00,
    DAL_COLOR_BLUE      = 0x0000FF,
    DAL_COLOR_TURQUOISE = (DAL_COLOR_GREEN | DAL_COLOR_BLUE),
    DAL_COLOR_YELLOW    = (DAL_COLOR_RED | DAL_COLOR_GREEN),
    DAL_COLOR_PINK      = (DAL_COLOR_RED | DAL_COLOR_BLUE),
} DAL_COLOR;


/* Display Assertion Layer API */

unsigned int DAL_GetLayerSize(void);

DAL_STATUS DAL_Init(unsigned int layerVA, unsigned int layerPA);
DAL_STATUS DAL_SetColor(unsigned int fgColor, unsigned int bgColor);
DAL_STATUS DAL_Clean(void);
DAL_STATUS DAL_Printf(const char *fmt, ...);
DAL_STATUS DAL_OnDispPowerOn(void);
DAL_STATUS DAL_LowMemoryOn(void);
DAL_STATUS DAL_LowMemoryOff(void);
#ifdef __cplusplus
}
#endif

