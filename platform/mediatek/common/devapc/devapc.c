/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <debug.h>
#include <devapc.h>
#include <platform_mtk.h>
#include <smc.h>
#include <smc_id_table.h>

void devapc_protection_enable(enum devapc_type type)
{
    struct __smccc_res res;

    if (type >= DEVAPC_NUM) {
        dprintf(CRITICAL, "%s invaild parameter!\n", __func__);
        return;
    }

    if (get_boot_stage() == BOOT_STAGE_BL33) {
        __smc_conduit(MTK_SIP_LK_DAPC_INIT_AARCH32, type, 0, 0, 0, 0, 0, 0, &res);

        if (res.a0)
            dprintf(CRITICAL, "%s failed!\n", __func__);
        else
            dprintf(INFO, "%s success!\n", __func__);
    }
}
