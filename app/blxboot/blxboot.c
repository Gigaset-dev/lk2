/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <app.h>
#include <compiler.h>
#include <fastboot_entry.h>
#include <kernel/event.h>
#include <platform/mtk_key.h>
#include <trace.h>

#define LOCAL_TRACE 0

static event_t boot_linux_event = EVENT_INITIAL_VALUE(boot_linux_event, false, 0);

void notify_boot_linux(void)
{
    LTRACEF("Notify boot linux.\n");
    event_signal(&boot_linux_event, false);
}

static void wait_for_boot_linux(void)
{
    LTRACEF("Wait for boot linux event...\n");
    event_wait(&boot_linux_event);
    event_unsignal(&boot_linux_event);
    LTRACEF("Ready to boot linux...\n");
}

static void blxboot_init(const struct app_descriptor *app)
{
    if (detect_key(DOWNLOAD_KEY)) {
        LTRACEF("Download key pressed, enter fastboot mode.\n");
        notify_enter_fastboot();
    } else {
        notify_boot_linux();
    }
}

static void blxboot_entry(const struct app_descriptor *app, void *args)
{
    wait_for_boot_linux();
    LTRACEF("blxboot starts.\n");
}

APP_START(blxboot)
.init = blxboot_init,
.entry = blxboot_entry,
 APP_END
