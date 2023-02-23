/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <debug.h>
#include <err.h>
#include <init_mtk.h>
#include <trace.h>

#include "apupw.h"

#define LOCAL_TRACE 0

void apusys_plat_init(uint level)
{
    LTRACE_ENTRY;

    apusys_power_init();

    LTRACE_EXIT;
}
MT_LK_INIT_HOOK_BL33(apusys_plat_init, apusys_plat_init,
                     LK_INIT_LEVEL_PLATFORM);
