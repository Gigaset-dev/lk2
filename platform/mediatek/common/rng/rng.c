/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <debug.h>
#include <err.h>
#include <platform_mtk.h>
#include <smc.h>
#include <smc_id_table.h>

uint32_t get_rnd(uint32_t *rnd)
{
    struct __smccc_res res;

    if (rnd == NULL) {
        dprintf(CRITICAL, "%s invaild parameter!\n", __func__);

        return ERR_INVALID_ARGS;
    }

    if (get_boot_stage() == BOOT_STAGE_BL33) {
        __smc_conduit(MTK_SIP_LK_GET_RND, 0, 0, 0, 0, 0, 0, 0, &res);

        if (res.a0) {
            dprintf(CRITICAL, "%s failed!\n", __func__);
        } else {
            dprintf(INFO, "%s success!\n", __func__);
            *rnd = res.a1;
        }

        return res.a0;
    }

    return ERR_NOT_SUPPORTED;
}
