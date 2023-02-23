/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
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
#define LOG_TAG "unknown"
#endif

#define DISP_LOG_V(fmt, args...) \
do { \
    if (DDP_INFO <= LK_DEBUGLEVEL) \
        LTRACEF("[LK_DDP/"LOG_TAG"]"fmt, ##args); \
   } while (0)
#define DISP_LOG_D(fmt, args...) \
do { \
    if (DDP_INFO <= LK_DEBUGLEVEL) \
        LTRACEF("[LK_DDP/"LOG_TAG"]"fmt, ##args); \
   } while (0)
#define DISP_LOG_I(fmt, args...) \
do { \
    if (DDP_INFO <= LK_DEBUGLEVEL) \
        LTRACEF("[LK_DDP/"LOG_TAG"]"fmt, ##args); \
   } while (0)
#define DISP_LOG_W(fmt, args...) \
do { \
    if (DDP_INFO <= LK_DEBUGLEVEL) \
        LTRACEF("[LK_DDP/"LOG_TAG"]"fmt, ##args); \
   } while (0)
#define DISP_LOG_E(fmt, args...) \
do { \
    if (DDP_CRITICAL <= LK_DEBUGLEVEL) \
        LTRACEF("[LK_DDP/"LOG_TAG"]error:"fmt, ##args); \
   } while (0)
#define DISP_LOG_F(fmt, args...) \
do { \
    if (DDP_INFO <= LK_DEBUGLEVEL) \
        LTRACEF("[LK_DDP/"LOG_TAG"]error:"fmt, ##args); \
   } while (0)

#define DDPMSG(string, args...) \
 do { \
    if (DDP_INFO <= LK_DEBUGLEVEL) \
        LTRACEF("[LK_DDP/"LOG_TAG"]"string, ##args); \
   } while (0) // default on, important msg, not err
#define DDPERR(string, args...) \
 do { \
    if (DDP_CRITICAL <= LK_DEBUGLEVEL) \
        LTRACEF("[LK_DDP/"LOG_TAG"]error:"string, ##args); \
   } while (0)  //default on, err msg
#define DDPDBG(string, args...) \
 do { \
    if (DDP_INFO <= LK_DEBUGLEVEL) \
        LTRACEF("[LK_DDP/"LOG_TAG"]"string, ##args); \
   } while (0)  // default on, important msg, not err

#define DDPDUMP(string, args...) \
do { \
    if (DDP_CRITICAL <= LK_DEBUGLEVEL) \
        LTRACEF("[LK_DDP/"LOG_TAG"]"string, ##args); \
   } while (0)  // default on, important msg, not err
