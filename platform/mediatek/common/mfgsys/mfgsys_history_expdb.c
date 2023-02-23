/*
 * copyright (c) 2021 mediatek inc.
 *
 * use of this source code is governed by a mit-style
 * license that can be found in the license file or at
 * https://opensource.org/licenses/mit
 */

#include <arch/ops.h>
#include <debug.h>
#include <err.h>
#include <kernel/vm.h> /* for vaddr_to_paddr */
#include <platform/mboot_expdb.h> /* for AEE_EXPDB_INIT_HOOK */
#include <platform/mrdump_params.h>
#include <platform/reg.h> /* for read32 */
#include <stdio.h>
#include <string.h> /* for memcpy */
#include <trace.h>
#include "mfgsys_history.h"

#define LOCAL_TRACE        0

static void save_gpu_mfgsys_history(CALLBACK dev_write)
{
    unsigned int *mfgsys_sysram_addr = (unsigned int *)MFGSYS_HISTORY_SYSRAM_BASE;
    int sysram_dump_size = AEE_LKDUMP_MFGSYS_HISTORY_DATA_SZ;

    dprintf(CRITICAL, "%s: mfgsys history sysram data suppot\n", EXPMOD);
    /* dump gpueb sysram data */
    if (!dev_write(vaddr_to_paddr(mfgsys_sysram_addr), sysram_dump_size))
        dprintf(CRITICAL, "%s: MFGSYS history dump fail\n", EXPMOD);

}
#if MFGSYS_SYSRAM_HISTORY_SUPPORT
AEE_EXPDB_INIT_HOOK(SYS_GPU_MFGSYS_HISTORY_LOG,
    AEE_LKDUMP_MFGSYS_HISTORY_DATA_SZ, save_gpu_mfgsys_history);
#endif
