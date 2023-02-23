/*
 * Copyright (c) 2021 MediaTek Inc. All Rights Reserved.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <arch/mmu.h>
#include <arch/ops.h>
#include <debug.h>
#include <err.h>
#include <init_mtk.h>
#include <kernel/vm.h>
#include <libfdt.h>
#include <lk/init.h>
#include <mblock.h>
#include <platform/mboot_expdb.h>
#include <platform/mrdump_params.h>
#include <sys/types.h>

#define TFA_DEBUG_BUF_SIZE                    0x200000
#define TFA_DEBUG_BUF_NAME                   "atf-log-reserved"

static void save_tfa_debug_buf(CALLBACK dev_write)
{
    uint32_t datasize = 0;
    int vmm_ret;
    uint64_t debug_buf_start = 0;
    uint64_t debug_buf_size = 0;
    void *debug_buf_va = NULL;

    /* Get tfa debug buffer */
    const struct reserved_t *debug_buf_p;

    debug_buf_p = mblock_query_reserved_by_name(TFA_DEBUG_BUF_NAME, 0);
    if (debug_buf_p == NULL) {
        dprintf(CRITICAL, "TFA: Could not find %s from mblock\n", TFA_DEBUG_BUF_NAME);
        return;
    }
    debug_buf_start = debug_buf_p->start;
    debug_buf_size = debug_buf_p->size;

    if ((debug_buf_start == 0) || (debug_buf_size == 0)) {
        dprintf(CRITICAL, "TFA: Fail to allocate debug buffer\n");
        return;
    }
    dprintf(INFO, "TFA: debug_buf_start:0x%llx, debug_buf_size:0x%llx\n",
        debug_buf_start, debug_buf_size);

    /* Memory map tfa debug buffer */
    vmm_ret = vmm_alloc_physical(vmm_get_kernel_aspace(),
            "vm-tfa-debug",
            ROUNDUP(debug_buf_size, PAGE_SIZE),
            &debug_buf_va,
            PAGE_SIZE_SHIFT,
            ROUNDDOWN((paddr_t)debug_buf_start, PAGE_SIZE),
            0,
            ARCH_MMU_FLAG_CACHED);

    if (vmm_ret != NO_ERROR || debug_buf_va == NULL) {
        dprintf(CRITICAL, "TFA: vmm_alloc_physical failed\n");
        return;
    }

    datasize = dev_write(vaddr_to_paddr(debug_buf_va), debug_buf_size);

    if (!datasize) {
        dprintf(CRITICAL, "TFA: Log dump FAIL\n");
        return;
    } else {
        dprintf(INFO, "TFA: dev_w:%u, debug_buf_va:0x%p, size:%llx\n",
                datasize, debug_buf_va, debug_buf_size);
        return;
    }
}
AEE_EXPDB_INIT_HOOK(TFA_DEBUG_BUF, TFA_DEBUG_BUF_SIZE, save_tfa_debug_buf);
