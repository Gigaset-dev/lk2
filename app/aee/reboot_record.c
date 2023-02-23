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
#include <arch/ops.h>
#include <debug.h>
#include <kernel/vm.h>
#include <platform/aee_common.h>
#include <platform/reg.h>
#include <string.h>
#include <trace.h>

#include "aee.h"
#include "mrdump_private.h"

#define LOCAL_TRACE 0

static void *aee_debug_kinfo_va;

int aee_debug_kinfo_map(void)
{
    const struct reserved_t *reserved = NULL;
    status_t err;

    reserved = mblock_query_reserved_by_name("aee_debug_kinfo", 0);
    if (reserved) {
        err = vmm_alloc_physical(vmm_get_kernel_aspace(),
                    "aee_debug_kinfo", reserved->size,
                    &aee_debug_kinfo_va, 0,
                    reserved->start, 0, ARCH_MMU_FLAG_CACHED);
        if (err) {
            voprintf_error("vmm_alloc_physical for memmap fail %d\n", err);
            return -1;
        }
    } else {
        voprintf_error("query aee_debug_kinfo fail\n");
        return -1;
    }

    return 0;
}

void aee_debug_kinfo_unmap(void)
{
    if (aee_debug_kinfo_va) {
        vmm_free_region(vmm_get_kernel_aspace(), (vaddr_t)aee_debug_kinfo_va);
        aee_debug_kinfo_va = NULL;
    }
}

struct mrdump_control_block *mrdump_cb_addr(void)
{
    if (aee_debug_kinfo_va)
        return aee_debug_kinfo_va + MRDUMP_CB_DRAM_OFF;
    else
        return NULL;
}

int mrdump_cb_size(void)
{
    return MRDUMP_CB_SIZE;
}

uint64_t ram_console_dram_addr(void)
{
    if (aee_debug_kinfo_va)
        return aee_debug_kinfo_va + RAM_CONSOLE_DRAM_OFF;
    else
        return NULL;
}

struct mrdump_control_block *aee_mrdump_get_info(void)
{
    struct mrdump_control_block *bufp = NULL;

    bufp = mrdump_cb_addr();
    if (bufp == NULL)
        return NULL;

    if (memcmp(bufp->sig, MRDUMP_GO_DUMP, 8) == 0) {
        return bufp;
    } else {
        bufp->sig[7] = 0;
        dprintf(CRITICAL, "wrong mcb sig %s\n", bufp->sig);
        return NULL;
    }
}

struct mrdump_control_block *aee_mrdump_get_params(void)
{
    struct mrdump_control_block *bufp = mrdump_cb_addr();

    if (bufp == NULL) {
        voprintf_debug("mrdump_cb is NULL\n");
        return NULL;
    }
    if (memcmp(bufp->sig, MRDUMP_GO_DUMP, 8) == 0) {
        bufp->sig[0] = 'X';
        aee_mrdump_flush_cblock(bufp);
        voprintf_debug("Boot record found at %p[%02x%02x]\n", bufp, bufp->sig[0], bufp->sig[1]);
        return bufp;
    } else {
        voprintf_debug("No Boot record found\n");
        return NULL;
    }
}

void aee_mrdump_flush_cblock(struct mrdump_control_block *bufp)
{
    if (bufp != NULL)
        arch_clean_cache_range((addr_t)bufp, sizeof(struct mrdump_control_block));
}
