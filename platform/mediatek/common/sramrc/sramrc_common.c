/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <init_mtk.h>
#include <smc.h>
#include <smc_id_table.h>
#include <string.h>
#include <trace.h>

#include "sramrc.h"

#define LOCAL_TRACE 0

status_t sramrc_init(void *fdt)
{
    struct __smccc_res smccc_res;

    /* notify atf to init sramrc  */
    memset(&smccc_res, 0, sizeof(smccc_res));

    __smc_conduit(MTK_SIP_BL_SSC_CONTROL,
                  SRAMRC_INIT, 0, 0, 0,
                  0, 0, 0, &smccc_res);
    LTRACEF("[sramrc] init sramrc done\n");
    return 0;
}
MT_LK_INIT_HOOK_BL33(sramrc_init, sramrc_init, LK_INIT_LEVEL_PLATFORM);
