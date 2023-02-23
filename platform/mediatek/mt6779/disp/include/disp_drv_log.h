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

//#include "xlog.h"

#include <debug.h>
#include <assert.h>
#include <trace.h>

#ifdef USER_BUILD
#define DISP_INFO INFO
#define DISP_ALWAYS ALWAYS
#define DISP_CRITICAL CRITICAL
#else
#define DISP_INFO INFO
#define DISP_ALWAYS ALWAYS
#define DISP_CRITICAL CRITICAL
#endif

#define LOCAL_TRACE 0

#define DISP_LOG_PRINT(level, sub_module, fmt, arg...) do { if (DISP_INFO <= LK_DEBUGLEVEL) { LTRACEF("[DISP]"fmt, ##arg); } } while (0)
#define LOG_PRINT(level, module, fmt, arg...) do { if (DISP_INFO <= LK_DEBUGLEVEL) { LTRACEF("[DISP]"fmt, ##arg); } } while (0)
#define DISPMSG(string, args...)    do { if (DISP_ALWAYS <= LK_DEBUGLEVEL) { LTRACEF("[DISP]"string, ##args); } } while (0) // default off, important msg, not err
#define DISPERR(string, args...)    do { if (DISP_CRITICAL <= LK_DEBUGLEVEL) { LTRACEF("[DISP]"string, ##args); } } while (0)   //default off, err msg
#define DISPDBG(string, args...)    do { if (DISP_INFO <= LK_DEBUGLEVEL) { LTRACEF("[DISP]"string, ##args); } } while (0)  // default off, dbg msg, not err
#define DISPCHECK(string, args...)  do { if (DISP_INFO <= LK_DEBUGLEVEL) { LTRACEF("[DISPCHECK]"string, ##args); } } while (0)
#define DISPFUNC()  do { if (DISP_ALWAYS <= LK_DEBUGLEVEL) { LTRACEF("[DISP]func|%s\n", __func__); } } while (0)

