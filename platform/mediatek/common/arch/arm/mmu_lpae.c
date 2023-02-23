/*
 * Copyright (c) 2019 MediaTek Inc.
 * Copyright (c) 2008-2016 Travis Geiselbrecht
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
#include <arch.h>
#include <arch/arm.h>
#include <arch/arm/mmu.h>
#include <arch/mmu.h>
#include <arch/ops.h>
#include <debug.h>
#include <err.h>
#include <kernel/vm.h>
#include <lib/console.h>
#include <platform/mmu_lpae.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <trace.h>

#define LOCAL_TRACE 0

#if ARM_WITH_MMU

#define IS_SUPERSECTION_ALIGNED(x) IS_ALIGNED(x, SUPERSECTION_SIZE)

#define MMU_MEMORY_L1_DESCRIPTOR_SUPERSECTION_MASK   ((0x3 << 0) | (0x1 << 18))
#define MMU_MEMORY_L1_SUPERSECTION_BA_SHIFT          24 /* BA address L mapping to [31:24] */
#define MMU_MEMORY_L1_SUPERSECTION_EBAL_SHIFT        20 /* extend BA address L mapping to [35:32] */
#define MMU_MEMORY_L1_SUPERSECTION_EBAH_SHIFT        5  /* extend BA address H mapping to [39:36] */
#define MMU_MEMORY_L1_SUPERSECTION_BA(x) \
    ((((x) >> 24) & 0xff) << MMU_MEMORY_L1_SUPERSECTION_BA_SHIFT)
#define MMU_MEMORY_L1_SUPERSECTION_EBAL(x) \
    ((((x) >> 32) & 0xf) << MMU_MEMORY_L1_SUPERSECTION_EBAL_SHIFT)
#define MMU_MEMORY_L1_SUPERSECTION_EBAH(x) \
    ((((x) >> 36) & 0xf) << MMU_MEMORY_L1_SUPERSECTION_EBAH_SHIFT)

static inline bool is_valid_vaddr(arch_aspace_t *aspace, vaddr_t vaddr)
{
    return (vaddr >= aspace->base && vaddr <= aspace->base + aspace->size - 1);
}

static void arm_mmu_map_supersection(arch_aspace_t *aspace, uint64_t paddr,
                                    addr_t vaddr, uint flags)
{
    int index;

    LTRACEF("aspace %p tt %p pa 0x%llx va 0x%lx flags 0x%x\n",
            aspace, aspace->tt_virt, paddr, vaddr, flags);

    DEBUG_ASSERT(aspace);
    DEBUG_ASSERT(aspace->tt_virt);
    DEBUG_ASSERT((flags & MMU_MEMORY_L1_DESCRIPTOR_SUPERSECTION_MASK)
                == MMU_MEMORY_L1_DESCRIPTOR_SUPERSECTION);

    /* Get the index into the translation table */
    index = vaddr / SECTION_SIZE;

    /* Set the entry value, repeating 16 times:
     * ((0x2 << 0) | (0x1 << 18)): Supersection entry
     * l1desc<8:5>:l1desc<23:20>:l1desc<31:24>: paddr
     * flags: TEX, CB and AP bit settings provided by the caller.
     */
    for (uint i = 0; i < (SUPERSECTION_SIZE / SECTION_SIZE); i++) {
        aspace->tt_virt[index + i] = MMU_MEMORY_L1_SUPERSECTION_BA(paddr) |
                                    MMU_MEMORY_L1_SUPERSECTION_EBAL(paddr) |
                                    MMU_MEMORY_L1_SUPERSECTION_EBAH(paddr) |
                                    MMU_MEMORY_L1_DESCRIPTOR_SUPERSECTION | flags;
    }
}

int arch_mmu_map_large(arch_aspace_t *aspace, addr_t vaddr, uint64_t paddr, uint count, uint flags)
{
    LTRACEF("vaddr 0x%lx paddr 0x%llx count %u flags 0x%x\n", vaddr, paddr, count, flags);

    DEBUG_ASSERT(aspace);
    DEBUG_ASSERT(aspace->tt_virt);

    DEBUG_ASSERT(is_valid_vaddr(aspace, vaddr));
    if (!is_valid_vaddr(aspace, vaddr))
        return ERR_OUT_OF_RANGE;

#if !WITH_ARCH_MMU_PICK_SPOT
    if (flags & ARCH_MMU_FLAG_NS) {
        /* WITH_ARCH_MMU_PICK_SPOT is required to support NS memory */
        panic("NS mem is not supported\n");
    }
#endif

    /* count, paddr and vaddr must be supersection aligned */
    DEBUG_ASSERT(paddr >= 0x100000000);
    DEBUG_ASSERT(IS_SUPERSECTION_ALIGNED(vaddr));
    DEBUG_ASSERT(IS_SUPERSECTION_ALIGNED(paddr));
    DEBUG_ASSERT(IS_ALIGNED(count, SUPERSECTION_SIZE / PAGE_SIZE));
    if (!IS_SUPERSECTION_ALIGNED(vaddr) || !IS_SUPERSECTION_ALIGNED(paddr) ||
        !IS_ALIGNED(count, SUPERSECTION_SIZE / PAGE_SIZE))
        return ERR_INVALID_ARGS;

    if (count == 0)
        return NO_ERROR;

    /* see what kind of mapping we can use */
    int mapped = 0;

    while (count > 0) {
        if (IS_SUPERSECTION_ALIGNED(vaddr) && IS_SUPERSECTION_ALIGNED(paddr)
            && count >= SUPERSECTION_SIZE / PAGE_SIZE) {
            /* we can use a supersection */

            /* compute the arch flags for L1 supersections */
            uint arch_flags = mmu_flags_to_l1_arch_flags(flags) |
                              MMU_MEMORY_L1_DESCRIPTOR_SUPERSECTION;

            /* map it */
            arm_mmu_map_supersection(aspace, paddr, vaddr, arch_flags);
            count -= SUPERSECTION_SIZE / PAGE_SIZE;
            mapped += SUPERSECTION_SIZE / PAGE_SIZE;
            vaddr += SUPERSECTION_SIZE;
            paddr += SUPERSECTION_SIZE;
        }
    }

done:
    DSB;
    return mapped;
}

static int cmd_mmu(int argc, const cmd_args *argv)
{
    if (argc < 2) {
notenoughargs:
        printf("not enough arguments\n");
usage:
        printf("usage:\n");
        printf("%s map_large <phys_H> <phys_L> <virt> <count> <flags>\n", argv[0].str);
        return ERR_GENERIC;
    }

    if (!strcmp(argv[1].str, "map_large")) {
        if (argc < 7)
            goto notenoughargs;

        vmm_aspace_t *aspace = vaddr_to_aspace((void *)argv[4].u);

        if (!aspace) {
            printf("ERROR: outside of any address space\n");
            return -1;
        }

        uint64_t paddr = argv[2].u;

        paddr = (paddr << 32) | argv[3].u;
        int err = arch_mmu_map_large(&aspace->arch_aspace, argv[4].u, paddr, argv[5].u, argv[6].u);

        if (err == (int)argv[5].u)
            printf("arch_mmu_map_large OK\n");
        else
            printf("arch_mmu_map_large fail returns %d\n", err);

        // write out
        printf("writing first pass...");
        for (uint i = 0; i < (argv[5].u / 128); i++)
            *(unsigned long *)(argv[4].u + 0x80000*i) = i;
        printf("done\n");

        // verify
        printf("verifying...");
        for (uint i = 0; i < (argv[5].u / 128); i++) {
            if (*(unsigned long *)(argv[4].u + 0x80000*i) != i)
                printf("error at %lx\n", (argv[4].u + 0x80000*i));
        }
        printf("done\n");
    } else {
        printf("unknown command\n");
        goto usage;
    }

    return NO_ERROR;
}

STATIC_COMMAND_START
STATIC_COMMAND("mmu", "mmu commands", &cmd_mmu)
STATIC_COMMAND_END(mmu);

#endif // ARM_WITH_MMU
