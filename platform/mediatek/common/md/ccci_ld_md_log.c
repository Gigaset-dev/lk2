/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <err.h>
#include <malloc.h>
#include <stdarg.h>
#include <trace.h>

#include "ccci_ld_md_log.h"
#include "ccci_ld_md_tag_dt.h"

#define LOCAL_TRACE 0

#define TRACE_LOG_BUF_SIZE (1024 * 16)

static char *s_trace_buf;
static int s_trace_size;

int ccci_lk_trace(const char *fmt, ...)
{
    int ret;

    if (!s_trace_buf)
        return -1;

    va_list ap;

    va_start(ap, fmt);

    ret = vsnprintf(&s_trace_buf[s_trace_size], TRACE_LOG_BUF_SIZE - s_trace_size,
            fmt, ap);
    va_end(ap);

    if ((ret < 0) || (ret >= (TRACE_LOG_BUF_SIZE - s_trace_size))) {
        LTRACEF_LEVEL(ALWAYS, "[CCCI] %s ret abnormal:%d at line:%d", __func__, ret, __LINE__);
        return -1;
    }

    if (ret > 0) {
        LTRACEF_LEVEL(ALWAYS, "[CCCI] %s", &s_trace_buf[s_trace_size]);
        s_trace_size += ret;
    }

    return ret;
}

int ccci_lk_add_trace_to_kernel(void)
{
    if (s_trace_buf && s_trace_size) {
        LTRACEF_LEVEL(ALWAYS, "[CCCI] Current trace log size: %d\n", s_trace_size);
        return ccci_insert_tag_inf("err_trace", s_trace_buf, s_trace_size);
    }
    return 0;
}

int ccci_lk_trace_init(void)
{
    s_trace_buf = malloc(TRACE_LOG_BUF_SIZE);
    if (s_trace_buf)
        return 0;

    LTRACEF_LEVEL(ALWAYS, "[CCCI] %s alloc memory fail", __func__);
    return -1;
}

void ccci_lk_trace_de_init(void)
{
    if (s_trace_buf) {
        free(s_trace_buf);
        s_trace_buf = NULL;
    }
}
