/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <assert.h>
#include <debug.h>
#include <err.h>
#include <init_mtk.h>
#include <mblock.h>
#include <platform_mtk.h>
#include <smc.h>
#include <smc_id_table.h>

enum cmdq_bl_smc_request {
    CMDQ_BL_IOVA_MAP,
};

static void cmdq_smc(void)
{
    struct __smccc_res res;

    __smc_conduit(MTK_SIP_BL_CMDQ_CONTROL, CMDQ_BL_IOVA_MAP, 0, 0, 0, 0, 0, 0, &res);
    dprintf(ALWAYS, "[cmdq] %s: a0:%lx\n", __func__, res.a0);
}
MT_LK_INIT_HOOK_BL33(cmdq_smc, cmdq_smc, LK_INIT_LEVEL_APPS - 1);

static void cmdq_init(uint level)
{
    u64 block;
    const u64 size = 0x100000, align = 0x100000, lower = 0x240000000, limit = 0x340000000;

    if (need_relocate())
        block = mblock_alloc_range(size, align, lower, limit, 0, 0, "me_cmdq_reserved");
    else
        block = mblock_alloc(size, align, limit, 0, 0, "me_cmdq_reserved");

    dprintf(ALWAYS, "[cmdq] %s: block:%llx size:%llx lower:%llx limit:%llx\n",
        __func__, block, size, lower, limit);
    ASSERT(block);
}
MT_LK_INIT_HOOK_BL2_EXT(cmdq_init, cmdq_init, LK_INIT_LEVEL_PLATFORM);
