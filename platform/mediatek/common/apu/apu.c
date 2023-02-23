/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <err.h>
#include <debug.h>
#include "vpu.h"
#include "mvpu.h"

// MVPU:
/* basic */
#define MODULE_NAME "[APU] "

/* utility */
#define LOG_CRI(format, args...)  dprintf(CRITICAL, MODULE_NAME format, ##args)
#define LOG_INF(format, args...)  dprintf(INFO, MODULE_NAME format, ##args)

status_t platform_load_apu(void *fdt)
{
    status_t ret;
    if (!fdt)
        return ERR_NOT_VALID;

    /* load vpu image */
    if (vpu_prop_off(fdt, 0) < 0) {
        LOG_INF("VPU is not supported on this platform\n");
    } else {
        ret = apu_load_vpu(fdt);
        if (ret != NO_ERROR)
            goto err_ret;
    }

    if (mvpu_prop_off(fdt) < 0) {
        LOG_INF("MVPU is not supported on this platform\n");
    } else {
        ret = apu_load_mvpu(fdt);
        if (ret != NO_ERROR)
            goto err_ret;
    }

err_ret:
    return ret;
}

