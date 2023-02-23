/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <arch/ops.h>
#include <assert.h>
#include <compiler.h>
#include <err.h>
#include <init_mtk.h>
#include <kernel/vm.h>
#include <mblock.h>
#include <platform/gz_api.h>
#include <platform_mtk.h>
#include <string.h>
#include <trace.h>

#define LOCAL_TRACE 0

#define SZ_2MB (2UL << 20)
#define SZ_34MB (34UL << 20)

#ifdef WITH_PLATFORM_MEDIATEK_COMMON_EMI
#include <emi_mpu.h>
#define REGION_ID_PGTABLES 13
#endif

#define MBLOCK_RELOC_LOWER 0x240000000
#define MBLOCK_RELOC_LIMIT 0x340000000

static void clean_pgd(u64 pgd_pa, size_t size)
{
    status_t ret;
    void *vaddr = NULL;

    ret = vmm_alloc_physical(vmm_get_kernel_aspace(),
                        "prot-pgd",
                        size,
                        &vaddr,
                        PAGE_SIZE_SHIFT,
                        (paddr_t)pgd_pa,
                        0,
                        ARCH_MMU_FLAG_CACHED);
    TRACEF("map pgd_pa=0x%llx size=%lx ret=%d\n", pgd_pa, size, ret);
    ASSERT(ret == NO_ERROR);

    memset(vaddr, 0x0, size);
    arch_clean_cache_range((addr_t)vaddr, size);
    TRACEF("clean pgd done\n");

    if (vaddr)
        vmm_free_region(vmm_get_kernel_aspace(), (vaddr_t)vaddr);
}

static void enable_mpu(u64 pgd_pa, size_t size)
{
#ifdef WITH_PLATFORM_MEDIATEK_COMMON_EMI
    struct emi_region_info_t region_info;
    status_t ret;

    region_info.start = pgd_pa;
    region_info.end = (pgd_pa + size);
    region_info.region = REGION_ID_PGTABLES;
    ret = emi_mpu_set_protection(&region_info);
    TRACEF("emi_mpu_set_protection ret=%d\n", ret);
#else
    TRACEF("common emi not defined\n");
#endif
}

static void mtk_smmu_bl2_ext_init(uint level)
{
    u64 prot_pgd = 0;
    u64 size = SZ_34MB;
    u64 align = SZ_2MB;
    u64 limit = 0x80000000;

    if (is_el2_enabled()) {
        if (need_relocate()) {
            prot_pgd = (u64)mblock_alloc_range(size, align,
                                               MBLOCK_RELOC_LOWER,
                                               MBLOCK_RELOC_LIMIT,
                                               0, 0, "platform_mtksmmu_protpgd");
            LTRACEF("mblock_alloc: relocate prot_pgd=%llx\n", prot_pgd);
        } else {
            prot_pgd = (u32)mblock_alloc(size, align, limit, 0, 0, "platform_mtksmmu_protpgd");
            LTRACEF("mblock_alloc: prot_pgd=%llx\n", prot_pgd);
        }
        ASSERT(prot_pgd);

        /* Clean first 2MB (which consist of prot pgd and hw pgd) */
        clean_pgd(prot_pgd, SZ_2MB);

        enable_mpu(prot_pgd, size);
    }
}
MT_LK_INIT_HOOK_BL2_EXT(mtk_smmu_bl2_ext_init, mtk_smmu_bl2_ext_init, LK_INIT_LEVEL_PLATFORM);
