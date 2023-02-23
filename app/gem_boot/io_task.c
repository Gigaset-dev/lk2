/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <app/boot_mode.h>
#include <assert.h>
#include <debug.h>
#include <err.h>
#include <kernel/event.h>
#include <profiling.h>
#include <platform/boot_mode.h>
#include <platform_mtk.h>
#include <sys/types.h>
#include <trace.h>

#include "io_task.h"

#define LOCAL_TRACE 1

static event_t iotask_end_event = EVENT_INITIAL_VALUE(iotask_end_event, false, 0);

static status_t app_load_scp(void *fdt)
{
    status_t ret = NO_ERROR;
    PROFILING_START("load scp");
    ret = platform_load_scp(fdt);
    PROFILING_END();
    return ret;
}

static status_t app_load_spm(void *fdt)
{
    status_t ret = NO_ERROR;
    PROFILING_START("load spm");
    ret = platform_load_spm(fdt);
    PROFILING_END();
    return ret;
}

static status_t app_load_modem(void *fdt, u8 b_mode)
{
    status_t ret = NO_ERROR;
    PROFILING_START("load modem image");
    ret = platform_load_modem(fdt, b_mode);
    PROFILING_END();

    return ret;
}

static status_t app_load_medmcu(void *fdt)
{
    status_t ret = NO_ERROR;
    PROFILING_START("load medmcu");
    ret = platform_load_medmcu(fdt);
    PROFILING_END();
    return ret;
}

static status_t app_load_dpm(void)
{
    status_t ret = NO_ERROR;
    PROFILING_START("load dpm");
    ret = platform_load_dpm();
    PROFILING_END();
    return ret;
}

static status_t app_load_rootfs_sig(void)
{
    status_t ret = NO_ERROR;
    PROFILING_START("load rootfs_sig");
    ret = platform_load_rootfs_sig();
    PROFILING_END();
    return ret;

}
void io_task_start(void *fdt)
{
    u8 b_mode = get_boot_mode();

    LTRACEF("%s fdt=0x%p, mode=%d\n", __func__, fdt, b_mode);
    ASSERT(app_load_scp(fdt) == NO_ERROR);
    ASSERT(app_load_spm(fdt) == NO_ERROR);
    ASSERT(app_load_modem(fdt, b_mode) == NO_ERROR);
    ASSERT(app_load_medmcu(fdt) == NO_ERROR);
    ASSERT(app_load_dpm() == NO_ERROR);
    ASSERT(app_load_rootfs_sig() == NO_ERROR);

    /*
     * Notify gem_boot that io_task is done.
     * DO NOT add any firmware loading function after this.
     */
    dprintf(INFO, "Wake up gem_boot\n");
    event_signal(&iotask_end_event, true);
}

void wait_for_io_task(void)
{
    dprintf(INFO, "Wait for io_task\n");
    event_wait(&iotask_end_event);
    event_unsignal(&iotask_end_event);
    dprintf(INFO, "io_task is done\n");
}
