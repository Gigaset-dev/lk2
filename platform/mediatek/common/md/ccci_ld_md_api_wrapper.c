/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <debug.h>
#include <err.h>
#include <kernel/vm.h>
#include <mblock.h>
#include <platform_mtk.h>
#include <string.h>

#include "ccci_ld_md_args.h"
#include "ccci_ld_md_core.h"
#include "ccci_ld_md_log.h"
#include "ccci_lk_plat.h"

#define LOCAL_TRACE 0

/*
 * ccci_resv_named_memory - reserve a memory using mblock API
 *
 * @name: assign a dedicated name for this memory area
 * @size: size of the memory want to be reserved
 */

unsigned long long ccci_resv_named_memory(const char name[], unsigned int size,
                                          unsigned char **va_o)
{
    unsigned long long limit = 0LL, low_bound = 0LL, pa;
    unsigned long align = 0;
    unsigned int mapping = 0;
    void *va = NULL;
    int ret;

    if (ccci_get_mem_limit_align_map(name, &low_bound, &limit, &align, &mapping) < 0) {
        CCCI_TRACE_LOG("get limit and align fail for %s\n", name);
        return 0ULL;
    }
    /* mapping == 1: mapped memory->1; == 0: no-map->0 */
    pa = mblock_alloc_range(size, align, low_bound, limit, 0ULL, mapping, name);
    if (!pa) {
        CCCI_TRACE_LOG("alloc sz=0x%x, ali=0x%lx, low=0x%llx, lim=0x%llx, name=%s fail\n",
                    size, align, low_bound, limit, name);
        return 0ULL;
    }

    if (va_o) {
        ret = vmm_alloc_physical(vmm_get_kernel_aspace(), name, size, &va,
                                 PAGE_SIZE_SHIFT, (paddr_t)pa, 0, ARCH_MMU_FLAG_CACHED);
        if (ret != NO_ERROR) {
            CCCI_TRACE_LOG("map va for pa fail,ret = %d\n", ret);
            mblock_free_partial(pa, size);
            va_o = NULL;

            return 0ULL;
        }
        *va_o = (unsigned char *)va;
    }

    CCCI_TRACE_LOG("mblock_alloc:sz=0x%x, ali=0x%lx, low=0x%llx, lim=0x%llx, exp=0\n",
                size, align, low_bound, limit);
    CCCI_TRACE_LOG("mblock_alloc result=>name=%s,0x%llx,%p\n", name, pa, va);

    return pa;
}

int ccci_retrieve_mem(unsigned long long pa_addr, int size)
{
    CCCI_TRACE_LOG("0x%xB retrieved by AP from:0x%llx\n", (int)size, pa_addr);

    return mblock_free_partial(pa_addr, size);
}

/* Weak function list that should be implement by platform code if need */
__WEAK void ccci_md_chk_hdr_notify_platform(void *hdr)
{
}

__WEAK void ccci_md_rorw_base_notify_platform(unsigned long long pa)
{
}

__WEAK void ccci_md_nc_smem_info_notify_platform(unsigned long long pa, unsigned int total)
{
}

__WEAK void ccci_md_c_smem_info_notify_platform(unsigned long long pa, unsigned int total)
{
}

__WEAK void ccci_platform_post_process(void)
{
}

__WEAK const struct download_info *ccci_get_tbl_cust(void)
{
    return NULL;
}

__WEAK const struct download_info *ccci_get_tbl_from_platform(void)
{
    return NULL;
}

__WEAK const struct download_info *ccci_get_tbl_for_test(void)
{
    return NULL;
}

__WEAK unsigned int ccci_get_max_md_resv_size_from_platform(void)
{
    return 0;
}

__WEAK int ccci_get_max_region_num_from_platform(void)
{
    return -1;
}

__WEAK void ccci_post_process(void *fdt)
{
}

__WEAK void ccci_test_md_power_on(void)
{
}

__WEAK int ccci_is_load_md_img_only(unsigned char b_mode)
{
    return 0;
}

/* Need custom implement */
__WEAK unsigned int customized_get_rf_id(void)
{
    return 0x0F0F0F0F;
}

__WEAK unsigned int customized_get_model_id(void)
{
    return 0x0F0F0F0F;
}

