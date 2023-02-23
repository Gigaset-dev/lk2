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
#include <app/boot_mode.h>
#include <assert.h>
#include <debug.h>
#include <err.h>
#include <kernel/event.h>
#include <platform/boot_mode.h>
#include <platform_mtk.h>
#include <profiling.h>
#include <sys/types.h>
#include <trace.h>

#include "io_task.h"

#define LOCAL_TRACE 1

static event_t iotask_end_event = EVENT_INITIAL_VALUE(iotask_end_event, false, 0);

static status_t app_load_scp(void *fdt, u8 b_mode)
{
    status_t ret = NO_ERROR;

    PROFILING_START("load scp");
    if (b_mode != RECOVERY_BOOT) {
        ret = platform_load_scp(fdt);
    }
    PROFILING_END();

    return ret;
}

static status_t app_load_ccu(void *fdt, u8 b_mode)
{
    status_t ret = NO_ERROR;

    PROFILING_START("load ccu");
    if (b_mode != RECOVERY_BOOT)
        ret = platform_load_ccu(fdt);
    PROFILING_END();

    return ret;
}

static status_t app_load_spm(void *fdt, u8 b_mode)
{
    status_t ret = NO_ERROR;

    PROFILING_START("load spm");
    if (b_mode != RECOVERY_BOOT) {
        ret =  platform_load_spm(fdt);
    }
    PROFILING_END();

    return ret;
}

static status_t app_load_apu(void *fdt, u8 b_mode)
{
    status_t ret = NO_ERROR;

    PROFILING_START("load apu");
    if (b_mode != RECOVERY_BOOT) {
        ret = platform_load_apu(fdt);
    }
    PROFILING_END();

    return ret;
}

static status_t app_load_apusys_rv(void *fdt, u8 b_mode)
{
    status_t ret = NO_ERROR;

    PROFILING_START("load apusys_rv");
    if (b_mode != RECOVERY_BOOT)
        ret = platform_load_apusys_rv(fdt);
    PROFILING_END();

    return ret;
}

static status_t app_load_dpm(u8 b_mode)
{
    status_t ret = NO_ERROR;

    PROFILING_START("load dpm");
    if (b_mode != RECOVERY_BOOT) {
        ret = platform_load_dpm();
    }
    PROFILING_END();

    return ret;
}

static status_t app_load_modem(void *fdt, u8 b_mode)
{
    status_t ret = NO_ERROR;

    PROFILING_START("load modem");
    ret = platform_load_modem(fdt, b_mode);
    PROFILING_END();

    return ret;
}

static status_t app_load_adsp(void *fdt, u8 b_mode)
{
    status_t ret = NO_ERROR;

    PROFILING_START("load adsp");
    if (b_mode != RECOVERY_BOOT) {
        ret = platform_load_adsp(fdt);
        if (ret == ERR_OFFLINE)
            ret = NO_ERROR;
    }
    PROFILING_END();

    return ret;
}

static status_t app_load_gpueb(void *fdt, u8 b_mode)
{
    status_t ret = NO_ERROR;

    PROFILING_START("load gpueb");
    if (b_mode != RECOVERY_BOOT)
        ret = platform_load_gpueb(fdt);
    PROFILING_END();

    return ret;
}

static status_t app_load_vcp(void *fdt, u8 b_mode)
{
    status_t ret = NO_ERROR;

    PROFILING_START("load vcp");
    if (b_mode != RECOVERY_BOOT)
        ret = platform_load_vcp(fdt);

    PROFILING_END();

    return ret;
}

void io_task_start(void *fdt)
{
    u8 b_mode = get_boot_mode();

    LTRACEF("%s fdt=0x%p, mode=%d\n", __func__, fdt, b_mode);
    ASSERT(app_load_scp(fdt, b_mode) == NO_ERROR);
    ASSERT(app_load_ccu(fdt, b_mode) == NO_ERROR);
    ASSERT(app_load_spm(fdt, b_mode) == NO_ERROR);
    ASSERT(app_load_apu(fdt, b_mode) == NO_ERROR);
    ASSERT(app_load_apusys_rv(fdt, b_mode) == NO_ERROR);
    ASSERT(app_load_dpm(b_mode) == NO_ERROR);
    ASSERT(app_load_modem(fdt, b_mode) == NO_ERROR);
    ASSERT(app_load_adsp(fdt, b_mode) == NO_ERROR);
    ASSERT(app_load_gpueb(fdt, b_mode) == NO_ERROR);
    ASSERT(app_load_vcp(fdt, b_mode) == NO_ERROR);

    /*
     * Notify mt_boot that io_task is done.
     * DO NOT add any firmware loading function after this.
     */
    dprintf(INFO, "Wake up mt_boot\n");
    event_signal(&iotask_end_event, true);
}

void wait_for_io_task(void)
{
    dprintf(INFO, "Wait for io_task\n");
    event_wait(&iotask_end_event);
    event_unsignal(&iotask_end_event);
    dprintf(INFO, "io_task is done\n");
}
