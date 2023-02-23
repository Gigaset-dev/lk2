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

#include <debug.h>
#include <stdarg.h>
#include <stdint.h>
#include <sys/types.h>

enum AEE_MODE {
    AEE_MODE_MTK_ENG = 1,
    AEE_MODE_MTK_USER,
    AEE_MODE_CUSTOMER_ENG,
    AEE_MODE_CUSTOMER_USER
};

enum AEE_REBOOT_MODE {
    AEE_REBOOT_MODE_NORMAL = 0,
    AEE_REBOOT_MODE_KERNEL_OOPS,
    AEE_REBOOT_MODE_KERNEL_PANIC,
    AEE_REBOOT_MODE_NESTED_EXCEPTION,
    AEE_REBOOT_MODE_WDT,
    AEE_REBOOT_MODE_EXCEPTION_KDUMP,
    AEE_REBOOT_MODE_MRDUMP_KEY,
    AEE_REBOOT_MODE_GZ_KE,
    AEE_REBOOT_MODE_GZ_WDT,
    AEE_REBOOT_MODE_HANG_DETECT,
};

#define IPANIC_OOPS_HEADER_PROCESS_NAME_LENGTH 256
#define IPANIC_OOPS_HEADER_BACKTRACE_LENGTH 3840

struct ipanic_oops_header {
    char process_path[IPANIC_OOPS_HEADER_PROCESS_NAME_LENGTH];
    char backtrace[IPANIC_OOPS_HEADER_BACKTRACE_LENGTH];
};

const char *mrdump_mode2string(uint8_t mode);

struct mrdump_control_block *aee_mrdump_get_params(void);
struct mrdump_control_block *mrdump_cb_addr(void);
int mrdump_cb_size(void);

void aee_mrdump_flush_cblock(struct mrdump_control_block *bufp);

void voprintf(char type, const char *msg, va_list ap);
void voprintf_verbose(const char *msg, ...);
void voprintf_debug(const char *msg, ...);
void voprintf_info(const char *msg, ...);
void voprintf_warning(const char *msg, ...);
void voprintf_error(const char *msg, ...);
void vo_show_progress(int size_mb);

void mrdump_status_none(const char *fmt, ...);
void mrdump_status_ok(const char *fmt, ...);
void mrdump_status_error(const char *fmt, ...);

struct aee_timer {
    unsigned int acc_ms;
    unsigned int start_ms;
};

/**
 * for sleep and delay (mdelay and udelay)
 */
#define udelay(x)   spin(x)
#define mdelay(x)   udelay((x) * 1000)
