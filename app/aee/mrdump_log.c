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

#include <assert.h>
#include <debug.h>
#include <malloc.h>
#ifdef WITH_PLATFORM_MEDIATEK_COMMON_VIDEO
#include <platform/video.h>
#endif
#include <printf.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <trace.h>

#include "aee.h"
#include "mrdump_private.h"

static unsigned int log_size;

void voprintf(char type, const char *msg, va_list ap)
{
    char msgbuf[128], *p;
    int err = 0;

    p = msgbuf;
    if (msg[0] == '\r') {
        *p++ = msg[0];
        msg++;
    }

    *p++ = type;
    *p++ = ':';
    err = vsnprintf(p, sizeof(msgbuf) - (p - msgbuf), msg, ap);
    ASSERT(err > 0);

    switch (type) {
    case 'I':
    case 'W':
    case 'E':
#ifdef WITH_PLATFORM_MEDIATEK_COMMON_VIDEO
        video_printf("%s", msgbuf);
#endif
        break;
    }

    dprintf(CRITICAL, "[%s] %s", MRDUMP_GO_DUMP, msgbuf);
}

void voprintf_verbose(const char *msg, ...)
{
    va_list ap;

    va_start(ap, msg);
    voprintf('V', msg, ap);
    va_end(ap);
}

void voprintf_debug(const char *msg, ...)
{
    va_list ap;

    va_start(ap, msg);
    voprintf('D', msg, ap);
    va_end(ap);
}

void voprintf_info(const char *msg, ...)
{
    va_list ap;

    va_start(ap, msg);
    voprintf('I', msg, ap);
    va_end(ap);
}

void voprintf_warning(const char *msg, ...)
{
    va_list ap;

    va_start(ap, msg);
    voprintf('W', msg, ap);
    va_end(ap);
}

void voprintf_error(const char *msg, ...)
{
    va_list ap;

    va_start(ap, msg);
    voprintf('E', msg, ap);
    va_end(ap);
}

void vo_show_progress(int sizeM)
{
#ifdef WITH_PLATFORM_MEDIATEK_COMMON_VIDEO
    video_set_cursor((video_get_rows() / 4) * 3, (video_get_cols() - 22) / 2);
    video_printf("=====================\n");
    video_set_cursor((video_get_rows() / 4) * 3 + 1, (video_get_cols() - 22) / 2);
    video_printf(">>> Written %4dM <<<\n", sizeM);
    video_set_cursor((video_get_rows() / 4) * 3 + 2, (video_get_cols() - 22) / 2);
    video_printf("=====================\n");
    video_set_cursor(video_get_rows() - 1, 0);
#endif
    dprintf(CRITICAL, "... Written %dM\n", sizeM);
}
