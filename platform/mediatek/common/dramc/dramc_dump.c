/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <debug.h>
#include <platform/dramc.h>
#include <platform/mboot_expdb.h>
#include <platform/mboot_params.h>

static int plat_dram_debug_get(void **data, int *len)
{
    sram_plat_dbg_info_addr_size((addr_t *)data, (unsigned int *)len);
    return 0;
}

static void save_dram_data(CALLBACK dev_write)
{
    char *buf = NULL;
    int len = 0;
    bool result = false;

    if (!plat_dram_debug_get((void **)&buf, &len)) {
        result = dev_write((uint64_t)buf, len);
        if (!result)
            dprintf(ALWAYS, "%s: failed to set dram data.\n", __func__);
    }
}
AEE_EXPDB_INIT_HOOK(SYS_LAST_DRAM, DRAMC_BUF_LENGTH, save_dram_data);
