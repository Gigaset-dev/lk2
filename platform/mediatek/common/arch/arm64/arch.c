/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <arch.h>
#include <arch/ops.h>
#include <compiler.h>
#include <debug.h>
#if WITH_KERNEL_VM
#include <kernel/vm.h>
#endif
#include <platform.h>
#include <stdlib.h>
#include <target.h>
#include <trace.h>

#define LOCAL_TRACE 0

void arch_chain_load(void *entry, ulong arg0, ulong arg1, ulong arg2, ulong arg3)
{
    LTRACEF("entry %p, args 0x%lx 0x%lx 0x%lx 0x%lx\n", entry, arg0, arg1, arg2, arg3);

    arch_disable_ints();

    /* give target and platform a chance to put hardware into a suitable
     * state for chain loading.
     */
    target_quiesce();
    platform_quiesce();

    paddr_t entry_pa;
    paddr_t loader_pa;

#if WITH_KERNEL_VM
    uint64_t sctlr_el1;

    entry_pa = vaddr_to_paddr(entry);
    if (entry_pa == (paddr_t)NULL)
        panic("error translating entry physical address\n");

    LTRACEF("entry pa 0x%lx\n", entry_pa);

    loader_pa = vaddr_to_paddr((void *)&arm64_chain_load);
    if (loader_pa == (paddr_t)NULL)
        panic("error translating loader physical address\n");

    LTRACEF("loader pa 0x%lx\n", loader_pa);

    /* Disable WXN before ttbr0 trampoline switch */
    sctlr_el1 = ARM64_READ_SYSREG(sctlr_el1);
    sctlr_el1 &= ~(1 << 19);
    ARM64_WRITE_SYSREG(sctlr_el1, sctlr_el1);

    /* TTBR0_EL1 already contains the physical address mapping */
    ARM64_WRITE_SYSREG(tcr_el1, (uint64_t)MMU_TCR_FLAGS_IDENT);
#else
    entry_pa = (paddr_t)entry;
    loader_pa = (paddr_t)&arm64_chain_load;
#endif

    LTRACEF("disabling instruction/data cache\n");
    arch_disable_cache(UCACHE);

    /* put the booting cpu back into close to a default state */
    arch_quiesce();

    LTRACEF("branching to physical address of loader\n");

    /* branch to the physical address version of the chain loader routine */
    void (*loader)(paddr_t entry, ulong, ulong, ulong, ulong) __NO_RETURN = (void *)loader_pa;

    loader(entry_pa, arg0, arg1, arg2, arg3);
}
