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

#define DDP_INFO INFO
#define DDP_CRITICAL CRITICAL
#define LOCAL_TRACE 0

#ifndef LOG_TAG
#define LOG_TAG "unknow"
#endif

#define DISP_LOG_V(fmt, args...)   do { if (DDP_INFO <= LK_DEBUGLEVEL) { LTRACEF("[LK_DDP/"LOG_TAG"]"fmt, ##args); } } while (0)
#define DISP_LOG_D(fmt, args...)   do { if (DDP_INFO <= LK_DEBUGLEVEL) { LTRACEF("[LK_DDP/"LOG_TAG"]"fmt, ##args); } } while (0)
#define DISP_LOG_I(fmt, args...)   do { if (DDP_INFO <= LK_DEBUGLEVEL) { LTRACEF("[LK_DDP/"LOG_TAG"]"fmt, ##args); } } while (0)
#define DISP_LOG_W(fmt, args...)   do { if (DDP_INFO <= LK_DEBUGLEVEL) { LTRACEF("[LK_DDP/"LOG_TAG"]"fmt, ##args); } } while (0)
#define DISP_LOG_E(fmt, args...)   do { if (DDP_CRITICAL <= LK_DEBUGLEVEL) { LTRACEF("[LK_DDP/"LOG_TAG"]error:"fmt, ##args); } } while (0)
#define DISP_LOG_F(fmt, args...)   do { if (DDP_INFO <= LK_DEBUGLEVEL) { LTRACEF("[LK_DDP/"LOG_TAG"]error:"fmt, ##args); } } while (0)

#define DDPMSG(string, args...)    do { if (DDP_INFO <= LK_DEBUGLEVEL) { LTRACEF("[LK_DDP/"LOG_TAG"]"string, ##args); } } while (0) // default on, important msg, not err
#define DDPERR(string, args...)    do { if (DDP_CRITICAL <= LK_DEBUGLEVEL) { LTRACEF("[LK_DDP/"LOG_TAG"]error:"string, ##args); } } while (0)  //default on, err msg
#define DDPDBG(string, args...)    do { if (DDP_INFO <= LK_DEBUGLEVEL) { LTRACEF("[LK_DDP/"LOG_TAG"]"string, ##args); } } while (0)  // default on, important msg, not err

#define DDPDUMP(string, args...)   do { if (DDP_CRITICAL <= LK_DEBUGLEVEL) { LTRACEF("[LK_DDP/"LOG_TAG"]"string, ##args); } } while (0)  // default on, important msg, not err


