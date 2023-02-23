/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <compiler.h>
#include <debug.h>
#include "sec_policy.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <trace.h>

#define LOCAL_TRACE 0

#define VFY_EN                  0x1
#define DL_ALLOW                0x2

#define VFY_DIS_DL_FORBIDDEN    0x0
#define VFY_DIS_DL_ALLOW        DL_ALLOW
#define VFY_EN_DL_FORBIDDEN     VFY_EN
#define VFY_EN_DL_ALLOW         (VFY_EN|DL_ALLOW)

/* bypass auth check: return false, need auth check: return true */
bool get_vfy_policy(const char *part_name)
{
        /* dummy: skip verify */
        return false;
}

/* forbid dl: return false, allow dl: return true */
bool get_dl_policy(const char *part_name)
{
        /* dummy: dl is allowed */
        return true;
}
