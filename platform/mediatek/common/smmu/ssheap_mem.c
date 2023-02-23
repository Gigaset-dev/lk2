/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <assert.h>
#include <compiler.h>
#include <err.h>
#include <libfdt.h>
#include <platform_mtk.h>
#include <set_fdt.h>
#include <trace.h>

#define LOCAL_TRACE 0

#define SVP_FEATURES_DT_UNAME "mediatek,mtk_sec_video_path_support"
#define SVP_PAGE_BASED_SIZE_STR "svp-page-based-size"
#define PROT_PAGE_BASED_SIZE_STR "prot-page-based-size"
#define WFD_PAGE_BASED_SIZE_STR "wfd-page-based-size"

#define SSHEAP_RELOC_LOWER 0x240000000
#define SSHEAP_RELOC_LIMIT 0x440000000

static bool is_svp_enabled(void *fdt)
{
    int offset;

    offset = fdt_node_offset_by_compatible(fdt, -1, SVP_FEATURES_DT_UNAME);
    if (offset < 0)
        return false;

    return true;
}

static status_t query_size(void *fdt, int offset, const char *prop_name, u64 *out_val)
{
    const fdt32_t *data_ptr;
    int len;

    data_ptr = fdt_getprop(fdt, offset, prop_name, &len);
    if (!data_ptr) {
        TRACEF("%s not found\n", SVP_PAGE_BASED_SIZE_STR);
        return ERR_NOT_FOUND;
    }

    *out_val = ((u64)fdt32_to_cpu(data_ptr[0]) << 32) + fdt32_to_cpu(data_ptr[1]);
    TRACEF("prop_name:%s out_val=%llx\n", prop_name, *out_val);

    return NO_ERROR;
}

static status_t query_ssmr_page_based_size(void *fdt, u64 *svp_pb_size,
                                           u64 *prot_pb_size,
                                           u64 *wfd_pb_size)
{
    int offset;
    status_t ret;

    offset = fdt_path_offset(fdt, "/memory-ssmr-features");
    if (offset < 0) {
        TRACEF("can't find memory-ssmr-features\n");
        return ERR_NOT_FOUND;
    }

    ret = query_size(fdt, offset, SVP_PAGE_BASED_SIZE_STR, svp_pb_size);
    if (ret)
        return ERR_NOT_FOUND;

    ret = query_size(fdt, offset, PROT_PAGE_BASED_SIZE_STR, prot_pb_size);
    if (ret)
        return ERR_NOT_FOUND;

    ret = query_size(fdt, offset, WFD_PAGE_BASED_SIZE_STR, wfd_pb_size);
    if (ret)
        return ERR_NOT_FOUND;

    return NO_ERROR;
}

void apply_alloc_range(void *fdt, int node_offset)
{
    u32 node_alloc_ranges[4];
    u64 alloc_start, alloc_size;
    int ret;

    alloc_start = SSHEAP_RELOC_LOWER;
    alloc_size =  SSHEAP_RELOC_LIMIT;
    node_alloc_ranges[0] = (u32)cpu_to_fdt32(alloc_start >> 32);
    node_alloc_ranges[1] = (u32)cpu_to_fdt32(alloc_start);
    node_alloc_ranges[2] = (u32)cpu_to_fdt32(alloc_size >> 32);
    node_alloc_ranges[3] = (u32)cpu_to_fdt32(alloc_size);
    ret = fdt_setprop(fdt, node_offset, "alloc-ranges",  node_alloc_ranges,
                sizeof(u32) * 4);
    if (ret) {
        TRACEF("set alloc-ranges failed. ret=%d\n", ret);
        return;
    }
    TRACEF("ssheap alloc-range: %llx %llx\n", alloc_start, alloc_size);
}

static void update_ssheap_cma_size(void *fdt, u64 size)
{
    int offset, node_offset;
    u32 node_size[2];
    int ret;

    /* Get offset of ssheap cma in the reserved-memory */
    offset = fdt_path_offset(fdt, "/reserved-memory");
    if (offset < 0) {
        TRACEF("reserved-memory not found\n");
        return;
    }
    node_offset = fdt_subnode_offset(fdt, offset, "ssheap-reserved-cma_memory");
    if (node_offset < 0) {
        TRACEF("ssheap-reserved-cma_memory not found\n");
        return;
    }

    /* Update size */
    node_size[0] = (u32)cpu_to_fdt32(size >> 32);
    node_size[1] = (u32)cpu_to_fdt32(size);
    ret = fdt_setprop(fdt, node_offset, "size",  node_size,
                sizeof(u32) * 2);
    if (ret) {
        TRACEF("set ssheap-reserved-cma_memory size failed ret=%d\n", ret);
        return;
    }
    LTRACEF("ssheap cma updated to size %llx\n", size);

    /* Delete alloc-ranges */
    ret = fdt_delprop(fdt, node_offset, "alloc-ranges");
    if (ret) {
        TRACEF("alloc-ranges not found ret=%d\n", ret);
    }
    LTRACEF("alloc-ranges has been removed\n");

    if (need_relocate()) {
        TRACEF("relocate ssheap\n");
        apply_alloc_range(fdt, node_offset);
    }
}

static void memory_ssheap_fixup(void *fdt)
{
    u64 svp_pg_size = 0;
    u64 prot_pg_size = 0;
    u64 wfd_pg_size = 0;
    u64 final_size = 0;
    status_t ret;

    ret = query_ssmr_page_based_size(fdt, &svp_pg_size, &prot_pg_size, &wfd_pg_size);
    if (ret) {
        TRACEF("query ssmr page based failed! ret=%d\n", ret);
        return;
    }

    if (is_svp_enabled(fdt)) {
        final_size = svp_pg_size + prot_pg_size + wfd_pg_size;
        TRACEF("has svp enabled - final_size=%llx [%llx %llx %llx]\n",
               final_size, svp_pg_size, prot_pg_size, wfd_pg_size);
    } else {
        final_size = (prot_pg_size) ? prot_pg_size : (4UL << 20);
        TRACEF("no svp - final_size=%llx [%llx]\n", final_size, prot_pg_size);
    }

    update_ssheap_cma_size(fdt, final_size);
}

SET_FDT_INIT_HOOK(memory_ssheap_fixup, memory_ssheap_fixup);
