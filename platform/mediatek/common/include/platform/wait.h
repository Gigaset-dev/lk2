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

#include <platform.h>
#include <platform/round.h>

/*
 * wait until cond gets true or timeout.
 *
 * cond : C expression to wait
 * timeout : usecs
 *
 * Returns:
 * 0 : if cond = false after timeout elapsed.
 * 1 : if cond = true after timeout elapsed,
 * or the remain usecs if cond = true before timeout elapsed.
 */
#define wait_us(cond, timeout)                                 \
({                                                             \
    lk_bigtime_t __ret;                                        \
    if (cond) {                                                \
        __ret = timeout;                                       \
    } else {                                                   \
        lk_bigtime_t __end = current_time_hires() + timeout;   \
                                                               \
        for (;;) {                                             \
            lk_bigtime_t __now = current_time_hires();         \
                                                               \
            if (cond) {                                        \
                __ret = (__end > __now) ? (__end - __now) : 1; \
                break;                                         \
            }                                                  \
                                                               \
            if (__end <= __now) {                              \
                __ret = 0;                                     \
                break;                                         \
            }                                                  \
        }                                                      \
    }                                                          \
    __ret;                                                     \
})

#define wait_ms(cond, timeout)                                 \
    DIV_ROUND_UP(wait_us(cond, (timeout) * 1000), 1000)
