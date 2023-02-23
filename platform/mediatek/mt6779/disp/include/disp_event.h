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

typedef enum {
    DISP_PATH_EVENT_FRAME_START = 0,
    DISP_PATH_EVENT_FRAME_DONE,
    DISP_PATH_EVENT_FRAME_REG_UPDATE,
    DISP_PATH_EVENT_FRAME_TARGET_LINE,
    DISP_PATH_EVENT_FRAME_COMPLETE,
    DISP_PATH_EVENT_FRAME_STOP,
    DISP_PATH_EVENT_IF_CMD_DONE,
    DISP_PATH_EVENT_IF_VSYNC,
    DISP_PATH_EVENT_AAL_TRIGGER,
    DISP_PATH_EVENT_COLOR_TRIGGER,
    DISP_PATH_EVENT_NUM,
    DISP_PATH_EVENT_NONE = 0xff,
} DISP_PATH_EVENT;

