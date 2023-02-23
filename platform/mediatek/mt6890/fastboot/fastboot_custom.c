/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <debug.h>
#include <platform/mrdump_expdb.h>
#include <platform/mrdump_params.h>
#include <platform_mtk.h>
#include <stdlib.h>
#include <string.h>
#include <trace.h>

#define LOCAL_TRACE 0

int get_fastboot_idle_timeout(void)
{
    char timeout_param[8];

    if (!mrdump_get_env(MRDUMP_FB_TIMEOUT, timeout_param,
            sizeof(timeout_param))) {
        if (strlen(timeout_param) > 0)
            return (atoi(timeout_param) * 1000);
    }
    return 0;
}
