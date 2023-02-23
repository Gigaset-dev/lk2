/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once
/*
 * Compatible function
 * used for preloader/lk/kernel environment
 */
#include <nandx.h>

#include "nandx_errno.h"
#include "nandx_os.h"

#ifndef min_t
#define min_t(type, x, y) ({                    \
    type __min1 = (x);                      \
    type __min2 = (y);                      \
    __min1 < __min2 ? __min1 : __min2; })

#define max_t(type, x, y) ({                    \
    type __max1 = (x);                      \
    type __max2 = (y);                      \
    __max1 > __max2 ? __max1 : __max2; })
#endif

#ifndef GENMASK
#define GENMASK(h, l) \
    (((~0UL) << (l)) & (~0UL >> ((sizeof(unsigned long) * 8) - 1 - (h))))
#endif

#ifndef __weak
#define __weak __attribute__((__weak__))
#endif

#ifndef __packed
#define __packed __attribute__((__packed__))
#endif

#ifndef KB
#define KB(x)   ((x) << 10)
#define MB(x)   (KB(x) << 10)
#define GB(x)   (MB(x) << 10)
#endif

#ifndef offsetof
#define offsetof(type, member) ((size_t)&((type *)0)->member)
#endif

#ifndef NULL
#define NULL (void *)0
#endif
static inline u32 nandx_popcount(u32 x)
{
    x = (x & 0x55555555) + ((x >> 1) & 0x55555555);
    x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
    x = (x & 0x0F0F0F0F) + ((x >> 4) & 0x0F0F0F0F);
    x = (x & 0x00FF00FF) + ((x >> 8) & 0x00FF00FF);
    x = (x & 0x0000FFFF) + ((x >> 16) & 0x0000FFFF);

    return x;
}

#ifndef zero_popcount
#define zero_popcount(x) (32 - nandx_popcount(x))
#endif

#define reminder(x, y) \
    ({ \
        u64 __temp = (x); \
        do_div(__temp, (y)); \
    })

#ifndef round_up
#define round_up(x, y)          ((((x) - 1) | ((y) - 1)) + 1)
#define round_down(x, y)        ((x) & ~((y) - 1))
#endif

#ifndef readx_poll_timeout_atomic
#define readx_poll_timeout_atomic(op, addr, val, cond, delay_us, timeout_us) \
    ({ \
        u64 end = get_current_time_us() + timeout_us; \
        for (;;) { \
            u64 now = get_current_time_us(); \
            (val) = op(addr); \
            if (cond) \
                break; \
            if (now > end) { \
                (val) = op(addr); \
                break; \
            } \
        } \
        (cond) ? 0 : -ETIMEDOUT; \
    })

#define readl_poll_timeout_atomic(addr, val, cond, delay_us, timeout_us) \
    readx_poll_timeout_atomic(readl, addr, val, cond, delay_us, timeout_us)
#define readw_poll_timeout_atomic(addr, val, cond, delay_us, timeout_us) \
    readx_poll_timeout_atomic(readw, addr, val, cond, delay_us, timeout_us)
#define readb_poll_timeout_atomic(addr, val, cond, delay_us, timeout_us) \
    readx_poll_timeout_atomic(readb, addr, val, cond, delay_us, timeout_us)
#endif

#ifndef container_of
#define container_of(ptr, type, member) \
    ({const __typeof__(((type *)0)->member) * __mptr = (ptr); \
        (type *)((char *)__mptr - offsetof(type, member)); })
#endif

static inline u32 nandx_cpu_to_be32(u32 val)
{
    u32 temp = 1;
    u8 *p_temp = (u8 *)&temp;

    if (*p_temp)
        return ((val & 0xff) << 24) | ((val & 0xff00) << 8) |
               ((val >> 8) & 0xff00) | ((val >> 24) & 0xff);

    return val;
}

