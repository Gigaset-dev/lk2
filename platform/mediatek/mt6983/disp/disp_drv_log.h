/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <debug.h>
#include <assert.h>
#include <trace.h>

#define DISP_INFO INFO
#define DISP_ALWAYS ALWAYS
#define DISP_CRITICAL CRITICAL

#define LOCAL_TRACE 1

#define DISP_LOG_PRINT(level, sub_module, fmt, arg...) \
do { \
    if (DISP_INFO <= LK_DEBUGLEVEL) \
        LTRACEF("[DISP]"fmt, ##arg); \
   } while (0)
#define LOG_PRINT(level, module, fmt, arg...) \
do { \
    if (DISP_INFO <= LK_DEBUGLEVEL) \
        LTRACEF("[DISP]"fmt, ##arg); \
   } while (0)
#define DISPMSG(string, args...) \
do { \
    if (DISP_ALWAYS <= LK_DEBUGLEVEL) \
        LTRACEF("[DISP]"string, ##args); \
    } while (0) // default off, important msg, not err
#define DISPERR(string, args...) \
do { \
    if (DISP_CRITICAL <= LK_DEBUGLEVEL) \
        LTRACEF("[DISP]"string, ##args); \
   } while (0)   //default off, err msg
#define DISPDBG(string, args...) \
do { \
    if (DISP_INFO <= LK_DEBUGLEVEL) \
        LTRACEF("[DISP]"string, ##args); \
   } while (0)  // default off, dbg msg, not err
#define DISPCHECK(string, args...) \
do { \
    if (DISP_INFO <= LK_DEBUGLEVEL) \
        LTRACEF("[DISPCHECK]"string, ##args); \
   } while (0)
#define DISPFUNC() \
do { \
    if (DISP_ALWAYS <= LK_DEBUGLEVEL) \
        LTRACEF("[DISP]func|%s\n", __func__); \
   } while (0)


