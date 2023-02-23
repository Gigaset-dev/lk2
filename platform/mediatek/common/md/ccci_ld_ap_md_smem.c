/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <assert.h>
#include <debug.h>
#include <kernel/vm.h>
#include <libfdt.h>
#include <lib/pl_boottags.h>
#include <malloc.h>
#include <platform/boot_mode.h>
#include <platform/reg.h>
#include <set_fdt.h>
#include <sys/types.h>
#include <string.h>
#include <sysenv.h>

#include "ccci_ld_md_sec.h"
#include "ccci_ld_md_core.h"
#include "ccci_ld_md_log.h"
#include "ccci_ld_md_mpu.h"
#include "ccci_ld_md_smem.h"
#include "ccci_ld_md_smem_uid.h"
#include "ccci_ld_md_tag_dt.h"
#include "ccci_lk_plat.h"

#define LOCAL_TRACE 0

struct rt_smem_region {
    unsigned long long ap_phy;
    unsigned long long ap_vir;
    struct smem_region inf;
};

static void smem_layout_dump(const char name[], struct rt_smem_region *rt_tbl, u32 num)
{
    u32 i;

    ALWAYS_LOG("Dump %s datails -------------\n", name);
    for (i = 0; i < num; i++)
        ALWAYS_LOG("[%d]0x%llx:0x%08x:0x%08x--0x%x<0x%x>\n", rt_tbl[i].inf.id,
                   rt_tbl[i].ap_phy, rt_tbl[i].inf.ap_offset,
                   rt_tbl[i].inf.md_offset, rt_tbl[i].inf.size,
                   rt_tbl[i].inf.flags);
}

static int md_smem_remapping_helper(unsigned long long addr, u32 size, u32 slot)
{
    u32 i, j;

    if (!size)
        return 0;

    if (addr && (addr != ALIGN(addr, 32 * 1024 * 1024))) {
        CCCI_TRACE_LOG("addr[0x%llx] is not 32M align\n", addr);
        return -1;
    }

    if (slot >= 8) {
        CCCI_TRACE_LOG("Invalid slot:%d\n", slot);
        return -1;
    }

    /* Using fix setting */
    j = 0;
    ALWAYS_LOG("Map 0x%x to 0x%llx for MD\n", 0x40000000 + 0x2000000 * slot, addr);
    for (i = slot; i < 8; i++) {
        ccci_apply_md_bank4_remap_by_slot(addr + 0x2000000 * (unsigned long long)j, i);
        j++;
        if ((0x2000000 * j) > size)
            return 0;
    }
    if ((0x2000000 * j) < size) {
        CCCI_TRACE_LOG("Size abnormal[0x%x] for slot:%d\n", size, slot);
        return -1;
    }
    return 0;
}

/*=============================================================================
 * Compatible code for share memory legacy
 *=============================================================================
 */
struct nc_smem_node_s {
    u32 ap_offset;
    u32 md_offset;
    u32 size;
    u32 id;
};

static struct nc_smem_node_s nc_smem_info_ext[] = {
    {0, 0, 0, SMEM_USER_RAW_DFD},
    {0, 0, 0, SMEM_USER_RAW_UDC_DATA},
    {0, 0, 0, SMEM_USER_MD_WIFI_PROXY},
#ifndef CCCI_NO_NEED_CHANGE_MDSS
    {0, 0, 24*1024, SMEM_USER_RAW_MDSS_DBG},
    {0, 0, 32*1024, SMEM_USER_RAW_RESERVED},
#endif
    {0, 0, 0, SMEM_USER_MD_DATA},
};

static void update_nc_smem_info_ext(struct rt_smem_region *item)
{
    u32 i;
    int done = 0;

    for (i = 0; i < countof(nc_smem_info_ext); i++) {
        if ((u32)item->inf.id == nc_smem_info_ext[i].id) {
            nc_smem_info_ext[i].ap_offset = item->inf.ap_offset;
            nc_smem_info_ext[i].md_offset = item->inf.md_offset;
            nc_smem_info_ext[i].size = item->inf.size;
            done = 1;
            break;
        }
    }

    if (done)
        ALWAYS_LOG("NC-smem ID[%d] done. ap_offset:0x%X(size:0x%X)\n", item->inf.id,
                   nc_smem_info_ext[i].ap_offset, nc_smem_info_ext[i].size);
    else
        CCCI_TRACE_LOG("NC-smem ID[%d] not found @ nc_smem_info_ext\n", item->inf.id);
}

struct smem_layout_s {
    unsigned long long base_addr;
    u32 ap_md1_smem_offset;
    u32 ap_md1_smem_size;
    u32 ap_md3_smem_offset;
    u32 ap_md3_smem_size;
    u32 md1_md3_smem_offset;
    u32 md1_md3_smem_size;
    u32 total_smem_size;
};

static struct smem_layout_s smem_info;

static int nc_smem_legacy_compatible(u32 total, struct rt_smem_region rt_tbl[], u32 num)
{
    u32 i;

    smem_info.ap_md1_smem_size = total;
    smem_info.total_smem_size = total;

    ALWAYS_LOG("smem_info.ap_md1_smem_offset: %x\n", smem_info.ap_md1_smem_offset);
    ALWAYS_LOG("smem_info.ap_md1_smem_size: %x\n", smem_info.ap_md1_smem_size);
    ALWAYS_LOG("smem_info.ap_md3_smem_offset: %x\n", smem_info.ap_md3_smem_offset);
    ALWAYS_LOG("smem_info.ap_md3_smem_size: %x\n", smem_info.ap_md3_smem_size);
    ALWAYS_LOG("smem_info.md1_md3_smem_offset: %x\n", smem_info.md1_md3_smem_offset);
    ALWAYS_LOG("smem_info.md1_md3_smem_size: %x\n", smem_info.md1_md3_smem_size);
    ALWAYS_LOG("smem_info.total_smem_size: %x\n", smem_info.total_smem_size);

    smem_info.base_addr = rt_tbl[0].ap_phy;
    /* 3. information to TAG for kernel */
    for (i = 0; i < num; i++) {
        if ((rt_tbl[i].inf.id == SMEM_USER_RAW_DFD)
                || (rt_tbl[i].inf.id == SMEM_USER_RAW_UDC_DATA)
                || (rt_tbl[i].inf.id == SMEM_USER_MD_WIFI_PROXY)
                || (rt_tbl[i].inf.id == SMEM_USER_MD_DATA))
            update_nc_smem_info_ext(&rt_tbl[i]);
        else if (rt_tbl[i].inf.id == SMEM_USER_RAW_PHY_CAP) {
            /* 2M for MD MPU include SIB header */
            if (rt_tbl[i].inf.size > 2 * 1024 * 1024) {
                smem_info.ap_md1_smem_size = total - (rt_tbl[i].inf.size - 2 * 1024 * 1024);
                ALWAYS_LOG("adjust smem_info.ap_md1_smem_size: %x\n", smem_info.ap_md1_smem_size);
            }
        }
    }
    if (ccci_insert_tag_inf("nc_smem_info_ext",
                (char *)nc_smem_info_ext, sizeof(nc_smem_info_ext)) < 0) {
        CCCI_TRACE_LOG("insert nc_smem_info_ext fail\n");
        return -1;
    }
    num = countof(nc_smem_info_ext);
    if (ccci_insert_tag_inf("nc_smem_info_ext_num",
                (char *)&num, sizeof(int)) < 0)
        CCCI_TRACE_LOG("insert nc_smem_info_ext_num fail\n");

    CCCI_TRACE_LOG("smem_info.base_addr: %x\n", (u32)smem_info.base_addr);
    CCCI_TRACE_LOG("smem_info.total_smem_size: %x\n", smem_info.total_smem_size);

    if (ccci_insert_tag_inf("smem_layout", (char *)&smem_info, sizeof(struct smem_layout_s)) < 0) {
        CCCI_TRACE_LOG("insert smem_layout fail\n");
        return -1;
    }

    return 0;
}

struct csmem_item_s {
    unsigned long long csmem_buffer_addr;
    u32 md_offset;
    u32 csmem_buffer_size;
    u32 item_cnt;
};
static struct csmem_item_s csmem_info;

static struct csmem_item_s csmem_layout[] = {
{0, 0, SMEM_MD_CONSYS_SIZE,      SMEM_USER_RAW_MD_CONSYS},
{0, 0, SMEM_MD_NVRAM_CACHE_SIZE, SMEM_USER_MD_NVRAM_CACHE},
{0, 0, (22*1024*1024),           SMEM_USER_CCB_START}, /* ccb */
{0, 0, 0,                        SMEM_USER_RAW_UDC_DESCTAB},
{0, 0, SMEM_MD_USIP_SIZE,        SMEM_USER_RAW_USIP}, /* audio */
{0, 0, 0,                        SMEM_USER_MD_CDMR},
};

static void update_csmem_layout(struct rt_smem_region *item, u32 offset)
{
    u32 i;
    int done = 0;

    for (i = 0; i < countof(csmem_layout); i++) {
        if ((u32)item->inf.id == csmem_layout[i].item_cnt) {
            csmem_layout[i].csmem_buffer_addr = item->ap_phy;
            csmem_layout[i].md_offset = item->inf.md_offset - offset;
            csmem_layout[i].csmem_buffer_size = item->inf.size;
            done = 1;
            break;
        }
    }

    if (done)
        ALWAYS_LOG("C-smem ID[%d] done. ap_phy:0x%llX(size:0x%X)\n", item->inf.id,
                   csmem_layout[i].csmem_buffer_addr, csmem_layout[i].csmem_buffer_size);
    else
        CCCI_TRACE_LOG("C-smem ID[%d] not found @ csmem_layout\n", item->inf.id);
}

struct ccb_layout_t {
    unsigned long long ccb_data_buffer_addr;
    u32 ccb_data_buffer_size;
} ccb_layout_t;
static struct ccb_layout_t ccb_info;

static void add_c_smem_to_tag_inf(void)
{
    u32 md1_bank4_cache_offset;

    if (ccci_insert_tag_inf("md1_bank4_cache_info", (char *)&csmem_info,
        sizeof(csmem_info)) < 0)
        CCCI_TRACE_LOG("insert md1_smem_cahce_info fail\n");
    if (ccci_insert_tag_inf("md1_bank4_cache_layout", (char *)&csmem_layout,
        sizeof(csmem_layout)) < 0)
        CCCI_TRACE_LOG("insert md1_smem_cahce_layout fail\n");

    /* md view: bank4 offset */
    md1_bank4_cache_offset = MAX_SMEM_SIZE - CACHABLE_SMEM_MAX_SIZE;
    if (ccci_insert_tag_inf("md1_smem_cahce_offset", (char *)&md1_bank4_cache_offset,
        sizeof(md1_bank4_cache_offset)) < 0)
        CCCI_TRACE_LOG("insert md1_smem_cahce_offset fail\n");

    if (ccci_insert_tag_inf("ccb_info", (char *)&ccb_info, sizeof(ccb_layout_t)) < 0)
        CCCI_TRACE_LOG("insert ccb_info fail\n");
}

static void c_smem_legacy_compatible(struct rt_smem_region rt_tbl[], u32 num,
                                     u32 offset, u32 total)
{
    unsigned int i;

    for (i = 0; i < num; i++) {
        switch (rt_tbl[i].inf.id) {
        case SMEM_USER_RAW_MD_CONSYS:
            /* consys buffer size: from MD image */
            update_csmem_layout(&rt_tbl[i], offset);
            break;
        case SMEM_USER_CCB_START:
            update_csmem_layout(&rt_tbl[i], offset);
            ccb_info.ccb_data_buffer_addr = rt_tbl[i].ap_phy;
            ccb_info.ccb_data_buffer_size = rt_tbl[i].inf.size;
            break;
        case SMEM_USER_RAW_UDC_DESCTAB:
            update_csmem_layout(&rt_tbl[i], offset);
            break;
        case SMEM_USER_MD_NVRAM_CACHE:
            /* NVRAM Cache buffer size: from MD image */
            update_csmem_layout(&rt_tbl[i], offset);
            break;
        case SMEM_USER_RAW_USIP:
            update_csmem_layout(&rt_tbl[i], offset);
            break;
        case SMEM_USER_MD_CDMR:
            update_csmem_layout(&rt_tbl[i], offset);
            break;
        default:
            break;
        }
    }
    csmem_info.csmem_buffer_addr = rt_tbl[0].ap_phy;
    csmem_info.md_offset = offset;
    csmem_info.csmem_buffer_size = total;
    csmem_info.item_cnt = sizeof(csmem_layout)/sizeof(struct csmem_item_s);

    CCCI_TRACE_LOG("cache_buffer_addr: %x\n", (u32)csmem_info.csmem_buffer_addr);
    CCCI_TRACE_LOG("cache_buffer_size: %x\n", csmem_info.csmem_buffer_size);
    CCCI_TRACE_LOG("cache_buffer_itm_cnt: %x\n", (u32)csmem_info.item_cnt);

    add_c_smem_to_tag_inf();
}

/*=============================================================================
 * Export to external for share memory dynamic update
 *=============================================================================
 */
void ccci_get_dfd_size_from_dts(void *kernel_fdt, u8 boot_mode, struct smem_region_lk *region)
{
    int nodeoffset;
    u32 *data = NULL;
    int len = 0;
    int dfd_enabled;
    u32 dfd_size = CCCI_SMEM_SIZE_DFD;
    const char *node_1 = "mediatek,dfd_cache";
    const char *node_2 = "mediatek,enabled";

    if (!region) {
        CCCI_TRACE_LOG("[%s] region NULL\n", __func__);
        return;
    }

    if (kernel_fdt == NULL) {
        CCCI_TRACE_LOG("[%s] error: kernel fdt is NULL!\n", __func__);
        goto _Exit;
    }

    nodeoffset = fdt_node_offset_by_compatible(kernel_fdt, -1, node_1);
    if (nodeoffset < 0) {
        CCCI_TRACE_LOG("[%s] error: '%s' not found!\n", __func__, node_1);
        goto _Exit;
    }

    data = (u32 *)fdt_getprop(kernel_fdt,
            nodeoffset, node_2, &len);
    if (data == NULL) {
        CCCI_TRACE_LOG("[%s] error: get '%s' fail.\n", __func__, node_2);
        goto _Exit;
    }

    dfd_enabled =  fdt32_to_cpu(*(u32 *)data);

    if (dfd_enabled) /* Fixme: change implement when DFD ready */
        ALWAYS_LOG("DFD en at dts");
    else
        ALWAYS_LOG("DFD dis at dts");

    ALWAYS_LOG("[%s] DFD dfd_enabled = %d, len = %d, dfd_size = %u\n",
            __func__, dfd_enabled, len, dfd_size);

_Exit:
    region->sm.size = dfd_size;
}

void ccci_get_udc_data_size(void *fdt, u8 boot_mode, struct smem_region_lk *region)
{
    int en = 0;

    if (!region) {
        CCCI_TRACE_LOG("[%s] region NULL\n", __func__);
        return;
    }

    if (ccci_find_tag_inf("udc_en", (char *)&en, sizeof(int)) != sizeof(int))
        CCCI_TRACE_LOG("[%s] error: 'udc_en' not found!\n", __func__);

    if (en)
        region->sm.size = CCCI_SMEM_SIZE_UDC_NONCACHE;
    else
        region->sm.size = 0;
}

void ccci_get_drdi_copy_size(void *fdt, u8 boot_mode, struct smem_region_lk *region)
{
    int drdi_version = 0;

    if (!region) {
        CCCI_TRACE_LOG("[%s] region NULL\n", __func__);
        return;
    }

    if (ccci_find_tag_inf("drdi_version", (char *)&drdi_version, sizeof(int)) != sizeof(int))
        CCCI_TRACE_LOG("[%s] error: 'drdi_version' not found!\n", __func__);

    if (drdi_version == AMMS_V3)
        region->sm.size = 64*1024;
    else
        region->sm.size = 0;

}

void ccci_get_udc_desc_size(void *fdt, u8 boot_mode, struct smem_region_lk *region)
{
    int en = 0;

    if (!region) {
        CCCI_TRACE_LOG("[%s] region NULL\n", __func__);
        return;
    }

    if (ccci_find_tag_inf("udc_en", (char *)&en, sizeof(int)) != sizeof(int))
        CCCI_TRACE_LOG("[%s] error: 'udc_en' not found!\n", __func__);

    if (en)
        region->sm.size = CCCI_SMEM_SIZE_UDC_CACHE;
    else
        region->sm.size = 0;
}

void ccci_get_consys_size(void *fdt, u8 boot_mode, struct smem_region_lk *region)
{
    int size = 0;

    if (!region) {
        CCCI_TRACE_LOG("[%s] region NULL\n", __func__);
        return;
    }

    if (ccci_find_tag_inf("consys_size", (char *)&size, sizeof(int)) !=
                           sizeof(int))
        CCCI_TRACE_LOG("[%s] error: 'consys_size' not found!\n", __func__);

    ALWAYS_LOG("Get WIFI proxy smem size:0x%x\n", size);
    region->sm.size = size;
}

void ccci_get_nv_cache_size(void *fdt, u8 boot_mode, struct smem_region_lk *region)
{
    int size = 0;

    if (!region) {
        CCCI_TRACE_LOG("[%s] region NULL\n", __func__);
        return;
    }
    if (ccci_find_tag_inf("nv_cache_shm_size", (char *)&size, sizeof(int)) != sizeof(int))
        CCCI_TRACE_LOG("[%s] error: 'nv_cache_shm_size' not found!\n",
                        __func__);

    ALWAYS_LOG("Get nv cache smem size:0x%x\n", size);
    if (size)
        region->sm.size = size;
}

static int s_md_ccb_gear_id;
static u32 get_ccb_gear_val(u8 boot_mode)
{
    u32 md1_ccb_cap_gear = 0;
    char *md1_ccb_gear_str;

    if (boot_mode == META_BOOT) {
        ALWAYS_LOG("meta mode[md1_ccb_cap_gear]%d\n", CCB_DATA_BUF_DEFAULT_GEAR);
        return CCB_DATA_BUF_DEFAULT_GEAR;
    }
    md1_ccb_gear_str = get_env("md1_ccb_cap_gear");
    if (md1_ccb_gear_str == NULL)
        md1_ccb_cap_gear = 0;
    else
        md1_ccb_cap_gear = atoi(md1_ccb_gear_str);

    if (!md1_ccb_cap_gear) {
#ifdef MTK_DYNAMIC_CCB_BUFFER_GEAR_ID
        md1_ccb_cap_gear = MTK_DYNAMIC_CCB_BUFFER_GEAR_ID;
#else
        md1_ccb_cap_gear = CCB_DATA_BUF_DEFAULT_GEAR;
#endif
    }

    s_md_ccb_gear_id = md1_ccb_cap_gear;
    return md1_ccb_cap_gear;
}

void ccci_get_ccb_size(void *fdt, u8 boot_mode, struct smem_region_lk *region)
{
    switch (get_ccb_gear_val(boot_mode)) {
    case 1:
        region->sm.size = 22 * 1024 * 1024;
        return;
    case 2:
        region->sm.size = 12 * 1024 * 1024;
        return;
    case 3:
        region->sm.size = 0;
        return;
    case 4:
        region->sm.size = 32 * 1024 * 1024;
        return;
    case 6:
        region->sm.size = 52 * 1024 * 1024;
        return;
    case 11:
        region->sm.size = 4 * 1024 * 1024;
        return;
    case 12:
        region->sm.size = 64 * 1024 * 1024;
        return;
    case 13:
        region->sm.size = 96 * 1024 * 1024;
        return;
    default:
        region->sm.size = 0;
        s_md_ccb_gear_id = 3;
        ALWAYS_LOG("un-support gear id, using 3\n");
        return;
    }
}

/*=============================================================================
 * Share memory calculate
 *=============================================================================
 */
static struct rt_smem_region *s_nc_smem_tbl, *s_c_smem_tbl;
static u32 s_nc_smem_tbl_num, s_c_smem_tbl_num;

static int smem_calc_and_update(void *fdt, u8 boot_mode, struct smem_region_lk *ref_tbl,
                                int ref_num, u32 block_align, u32 *total)
{
    u32 align_offset, offset;
    int num, i;

    /* Update runtime setting and calc alignment */
    offset = 0;
    num = 0;
    for (i = 0; i < ref_num; i++) {
        if (ref_tbl[i].rt_cb)
            ref_tbl[i].rt_cb(fdt, boot_mode, &ref_tbl[i]);

        if (!ref_tbl[i].sm.align)
            align_offset = offset;
        else
            align_offset = ALIGN(offset, ref_tbl[i].sm.align);
        ALWAYS_LOG("curr_offset:0x%x<%d> -- align offset:0x%x\n", offset, i, align_offset);
        if (align_offset != offset) {
            num++;
            ALWAYS_LOG("align adjust for %d size:0x%x\n", i, align_offset - offset);
            offset = align_offset;
        }
        ref_tbl[i].sm.ap_offset = offset;
        ref_tbl[i].sm.md_offset = offset;
        offset += ref_tbl[i].sm.size;
    }
    align_offset = ALIGN(offset, block_align);

    if (total)
        *total = align_offset;

    return ref_num + num;
}

static struct rt_smem_region *smem_resv_and_rt_tbl_update(const char name[],
                                                          struct smem_region_lk *ref_tbl,
                                                          int tbl_num,
                                                          u32 bank_offset, u32 resv_size)
{
    unsigned long long smem_addr = 0ULL;
    u32 size, offset;
    int i, j;
    struct rt_smem_region *rt_tbl = NULL;

    /* Alloc runtime nc share memory info table */
    size = sizeof(struct rt_smem_region) * tbl_num;
    rt_tbl = malloc(size);
    if (!rt_tbl) {
        CCCI_TRACE_LOG("Alloc rt_tbl memory fail\n");
        return NULL;
    }

    memset(rt_tbl, 0, size);

    /* Allocate share memory */
    smem_addr = ccci_resv_named_memory(name, resv_size, NULL);
    if (!smem_addr) {
        CCCI_TRACE_LOG("allocate MD NC share memory fail\n");
        free(rt_tbl);
        return NULL;
    }

    /* Init runtime share memory info table */
    i = 0;
    offset = 0;
    for (j = 0; j < tbl_num; j++) {
        ALWAYS_LOG("curr offset:0x%x -- ref:0x%x[%d]\n", offset, ref_tbl[i].sm.ap_offset, i);
        rt_tbl[j].inf.id = ref_tbl[i].sm.id;
        rt_tbl[j].ap_phy = smem_addr + offset;
        rt_tbl[j].inf.ap_offset = offset;
        rt_tbl[j].inf.md_offset = offset + bank_offset;

        if (offset != ref_tbl[i].sm.ap_offset) {
            rt_tbl[j].inf.flags = SMEM_ATTR_PADDING;
            rt_tbl[j].inf.size = ref_tbl[i].sm.ap_offset - offset;
            ALWAYS_LOG("Add pading smem size:0x%x\n", rt_tbl[j].inf.size);
        } else {
            rt_tbl[j].inf.flags = ref_tbl[i].sm.flags;
            rt_tbl[j].inf.size = ref_tbl[i].sm.size;
            ALWAYS_LOG("Add normal smem size:0x%x\n", rt_tbl[j].inf.size);
            i++;
        }

        ALWAYS_LOG("==>[%d]%llx-%x-%x\n", ref_tbl[i].sm.id, smem_addr + offset,
                    offset, rt_tbl[j].inf.size);
        offset += rt_tbl[j].inf.size;
    }
    smem_layout_dump(name, rt_tbl, tbl_num);

    return rt_tbl;
}

static int config_dedicated_mpu(struct rt_smem_region *rt_tbl, u32 num)
{
    u32 i;
    int ret;

    for (i = 0; i < num; i++) {
        if (!(rt_tbl[i].inf.flags & SMEM_ATTR_DEDICATED_MPU))
            continue;
        if (rt_tbl[i].inf.id == SMEM_USER_RAW_MD_CONSYS) {
            ret = ccci_plat_update_md_mpu_setting(MD_CONSYS,
                                                  rt_tbl[i].ap_phy, rt_tbl[i].inf.size, -1);
            if (ret < 0)
                return -1;
        }
    }

    return 0;
}

static int config_smem_common_mpu(struct rt_smem_region *rt_tbl, u32 num, u32 flag, u32 size)
{
    if (!num) {
        CCCI_TRACE_LOG("rt smem table num is 0\n");
        return -1;
    }

    if (size > 0x10000000) {// 256MB
        CCCI_TRACE_LOG("size > 256MB(0x%X)\n", size);
        return -1;
    }

    return ccci_plat_update_md_mpu_setting(flag, rt_tbl[0].ap_phy, size, -1);
}

static int smem_noncacheable_init(void *fdt, u8 b_mode, u32 offset, u32 max, u32 *o_size)
{
    int ret, num;
    struct rt_smem_region *rt_tbl = NULL;
    struct smem_region_lk *ref_tbl = NULL;
    u32 total = 0;

    num = 0;
    ref_tbl = (struct smem_region_lk *)ccci_get_nc_smem_ref_tbl(&num);

    if (num <= 0) {
        CCCI_TRACE_LOG("ccci_get_nc_smem_ref_tbl: table item num: %d\n", num);
        return -1;
    }

    ret = smem_calc_and_update(fdt, b_mode, ref_tbl, num, NC_SMEM_ALIGN, &total);
    if (ret <= 0) {
        CCCI_TRACE_LOG("MD non-cache memory calc abnormal:%d\n", ret);
        return -1;
    }
    if (total > max) {
        CCCI_TRACE_LOG("MD non-cache memory size abnormal:0x%x > max:0x%x\n", total, max);
        return -1;
    }

    num = ret;
    rt_tbl = smem_resv_and_rt_tbl_update("ap_md_nc_smem", ref_tbl, num, offset, total);
    if (!rt_tbl)
        return -1;

    /* insert share memory layout to lk info */
    if (ccci_insert_tag_inf("nc_smem_layout", (char *)rt_tbl,
                             sizeof(struct rt_smem_region) * num) < 0) {
        CCCI_TRACE_LOG("insert nc_smem_layout fail\n");
        free(rt_tbl);
        return -1;
    }
    if (ccci_insert_tag_inf("nc_smem_layout_num", (char *)&num, sizeof(int)) < 0) {
        CCCI_TRACE_LOG("insert nc_smem_layout_num fail\n");
        free(rt_tbl);
        return -1;
    }

    ALWAYS_LOG("NC share memory base_phy: 0x%llx, size:%u\n",
                rt_tbl[0].ap_phy, total);

    s_nc_smem_tbl = rt_tbl;
    s_nc_smem_tbl_num = num;
    if (o_size)
        *o_size = total;

    ccci_md_nc_smem_info_notify_platform(rt_tbl[0].ap_phy, total);

    if (md_smem_remapping_helper(rt_tbl[0].ap_phy, 256 * 1024 * 1024, 0) < 0)
        return -1;

    /* Fix me, remove this function when legacy share memory structure phase out */
    if (nc_smem_legacy_compatible(total, rt_tbl, num) < 0)
        return -1;

    if (config_smem_common_mpu(rt_tbl, num, MD_NC_SMEM, total) < 0)
        return -1;

    if (config_dedicated_mpu(rt_tbl, num) < 0)
        return -1;

    return 0;
}

static int smem_cacheable_init(void *fdt, u8 b_mode, u32 offset, u32 max, u32 *o_size)
{
    int ret, num;
    struct rt_smem_region *rt_tbl = NULL;
    struct smem_region_lk *ref_tbl = NULL;
    u32 total = 0;

    num = 0;
    ref_tbl = (struct smem_region_lk *)ccci_get_c_smem_ref_tbl(&num);

    if (num <= 0) {
        CCCI_TRACE_LOG("ccci_get_c_smem_ref_tbl: table item num: %d\n", num);
        return -1;
    }

    ret = smem_calc_and_update(fdt, b_mode, ref_tbl, num, C_SMEM_ALIGN, &total);
    if (ret <= 0) {
        CCCI_TRACE_LOG("MD cache memory calc abnormal:%d\n", ret);
        return -1;
    }
    if (total > max) {
        CCCI_TRACE_LOG("MD cache memory size abnormal:0x%x > max:0x%x\n", total, max);
        return -1;
    }

    num = ret;
    rt_tbl = smem_resv_and_rt_tbl_update("ap_md_c_smem", ref_tbl, num, offset, total);
    if (!rt_tbl)
        return -1;

    /* insert share memory layout to lk info */
    if (ccci_insert_tag_inf("c_smem_layout", (char *)rt_tbl,
        sizeof(struct rt_smem_region) * num) < 0) {
        CCCI_TRACE_LOG("insert c_smem_layout fail\n");
        free(rt_tbl);
        return -1;
    }
    if (ccci_insert_tag_inf("c_smem_layout_num", (char *)&num, sizeof(int)) < 0) {
        CCCI_TRACE_LOG("insert c_smem_layout_num fail\n");
        free(rt_tbl);
        return -1;
    }
    if (ccci_insert_tag_inf("ccb_gear_id", (char *)&s_md_ccb_gear_id, sizeof(u32)) < 0)
        CCCI_TRACE_LOG("insert ccb_gear_id fail\n");

    ALWAYS_LOG("C share memory base_phy: 0x%llx, size:%u\n",
                rt_tbl[0].ap_phy, total);

    s_c_smem_tbl_num = num;
    s_c_smem_tbl = rt_tbl;
    if (o_size)
        *o_size = total;

    ccci_md_c_smem_info_notify_platform(rt_tbl[0].ap_phy, total);

    if (md_smem_remapping_helper(rt_tbl[0].ap_phy, max, offset / (32 * 1024 * 1024)) < 0)
        return -1;

    /* Fix me, remove this function when legacy share memory structure phase out */
    c_smem_legacy_compatible(rt_tbl, num, offset, total);

    if (config_smem_common_mpu(rt_tbl, num, MD_C_SMEM, total) < 0)
        return -1;

    if (config_dedicated_mpu(rt_tbl, num) < 0)
        return -1;

    return 0;
}

/*=============================================================================
 * Special case: SIB (PHY Cap) (Dedicate HW remap)
 *=============================================================================
 */
struct sib_item_s {
    unsigned long long md1_sib_addr;
    u32 md1_sib_size;
};

static void smem_sib_init(void)
{
#ifndef CCCI_SMEM_SIB_AT_BANK4
    u32 sib_size;
    char *md1_sib_str;
    struct sib_item_s sib_info;
    unsigned long long sib_buf = 0ULL;

    memset(&sib_info, 0, sizeof(struct sib_item_s));
    /* 1. size calculation */
    /* SIB size: from sys_env setting by user, unit: M */
    md1_sib_str = get_env("md1_phy_cap_gear");
    if (md1_sib_str == NULL) {
        sib_size = 0;
        ALWAYS_LOG("[sib_size] 0 for env[sib_size] NULL\n");
    } else {
        sib_size = atoi(md1_sib_str);
        ALWAYS_LOG("env[sib_size]%x.\n", sib_size);
    }
    sib_info.md1_sib_size = sib_size * 1024 * 1024;
    /* max size: by platform */
    if (sib_info.md1_sib_size > CCCI_SMEM_SIB_MAX_SIZE)
        sib_info.md1_sib_size = CCCI_SMEM_SIB_MAX_SIZE;
    /* 2. allocate memory */
    if (sib_info.md1_sib_size) {
        sib_buf = ccci_resv_named_memory("md1_sib_mem", sib_info.md1_sib_size, NULL);
        if (!sib_buf) {
            CCCI_TRACE_LOG("allocate MD phy capture memory fail\n");
            return;
        }
        sib_info.md1_sib_addr = sib_buf;
        ccci_apply_md_sib_remapping(sib_buf);
        ALWAYS_LOG("md1_sib_addr = 0x%llx\n", sib_info.md1_sib_addr);
    } else {
        ALWAYS_LOG("MD phy capture memory size is 0\n");
        return;
    }
    /* 3. information to TAG for kernel */
    if (ccci_insert_tag_inf("md1_sib_info", (char *)&sib_info,
                            sizeof(sib_info)) < 0)
        CCCI_TRACE_LOG("insert md1_sib_addr fail\n");

    /* configure mpu */
    ccci_plat_update_md_mpu_setting(MD_TRACE_TOP, sib_buf, sib_info.md1_sib_size, -1);
#endif
}

/*=============================================================================
 * Special case: GZ MTEE(HW remap) (Occupy MD Bank4)
 *=============================================================================
 */
#define BOOT_TAG_GZ_PARAM     (0x88610024)
#define ME_IDENTITY_LEN       (16)
#define RPMB_KEY_SIZE         (32)

struct boot_tag_gz_param {
    u8  hwuid[ME_IDENTITY_LEN];
    u64 modemMteeShareMemPA;
    u32 modemMteeShareMemSize;
    u8  rpmb_key[RPMB_KEY_SIZE];
    u32 rpmb_base;
    u32 rpmb_size;
};

struct boot_tag_gz_param gz_param;

static void pl_boottags_gz_para_hook(struct boot_tag *tag)
{
    memcpy(&gz_param, &tag->data, sizeof(struct boot_tag_gz_param));
    ALWAYS_LOG("mtee.base_addr: 0x%x, 0x%x\n",
            (u32)gz_param.modemMteeShareMemPA, gz_param.modemMteeShareMemSize);
}

PL_BOOTTAGS_INIT_HOOK(pl_boottags_gz_para_hook,
                      BOOT_TAG_GZ_PARAM,
                      pl_boottags_gz_para_hook);

static void smem_others_init(void)
{
    u32 mtee_support = 1;

    if (gz_param.modemMteeShareMemPA && gz_param.modemMteeShareMemSize) {
        ALWAYS_LOG("mtee.base_addr: 0x%x, 0x%x\n",
            (u32)gz_param.modemMteeShareMemPA, gz_param.modemMteeShareMemSize);
        if (ccci_insert_tag_inf("mtee_support", (char *)&mtee_support,
                sizeof(mtee_support)) < 0) {
            CCCI_TRACE_LOG("insert mtee_support fail\n");
            return;
        }

        md_smem_remapping_helper(gz_param.modemMteeShareMemPA, gz_param.modemMteeShareMemSize,
                                 SMEM_MTEE_SLOT);
    }
}

/*=============================================================================
 * Main section, share memory calcaulte
 *=============================================================================
 */
int cal_ap_md_smem_layout(void *fdt, u8 b_mode)
{
    int ret = 0;

    /* SIB: Alloc large size memory, at 1st */
    smem_sib_init();

    /* Layout overview
     * Share memory -- MD Non-cache
     * Share memory -- MD cache
     * Share memory -- other misc (High slot)
     */
    ret = smem_noncacheable_init(fdt, b_mode, SMEM_NC_OFFSET, SMEM_NC_MAX, NULL);
    if (ret < 0)
        return -1;
    ret = smem_cacheable_init(fdt, b_mode, SMEM_C_OFFSET, SMEM_C_MAX, NULL);
    if (ret < 0)
        return -1;

    smem_others_init();

    return 0;
}

/*=============================================================================
 * For user that query setting from DT
 *=============================================================================
 */
static void ccci_update_ap_md_smem_info_to_dt(void *fdt)
{
    int node_offset = 0;
    int ret = 0;
    u32 i;

    node_offset = fdt_path_offset(fdt, "/mddriver");
    if (node_offset < 0) {
        CCCI_TRACE_LOG("/mddriver not found, try /soc/mddriver\n");
        node_offset = fdt_path_offset(fdt, "/soc/mddriver");
        if (node_offset < 0) {
            CCCI_TRACE_LOG("/soc/mddriver not found!\n");
            return;
        }
    }

    ret = fdt_setprop_string(fdt, node_offset, "md1_ccb_gear_list",
                             CCB_GEAR_SUPPORT_LIST);
    if (ret != 0)
        CCCI_TRACE_LOG("create ccb arguments info DT Fail\n");
    else
        ALWAYS_LOG("create ccb arguments info FDT OK\n");

    ret = fdt_setprop_u32(fdt, node_offset, "md1_ccb_cap_gear", s_md_ccb_gear_id);
    if (ret != 0)
        CCCI_TRACE_LOG("create ccb gear info DT Fail\n");
    else
        ALWAYS_LOG("create ccb gear info FDT OK\n");

    for (i = 0; i < s_nc_smem_tbl_num; i++) {
        switch (s_nc_smem_tbl[i].inf.id) {
        case SMEM_USER_LOW_POWER:
            ALWAYS_LOG("low power base = 0x%llx(offset = 0x%x), size:%u\n",
                        s_nc_smem_tbl[i].ap_phy, s_nc_smem_tbl[i].inf.ap_offset,
                        s_nc_smem_tbl[i].inf.size);
            if (fdt_setprop_u64(fdt, node_offset, "md_low_power_addr",
                                s_nc_smem_tbl[i].ap_phy))
                ALWAYS_LOG("create md_low_power_mem info DT fail\n");
            else
                ALWAYS_LOG("create md_low_power_mem info DT OK\n");
            break;

        default:
            break;
        }
    }
}

SET_FDT_INIT_HOOK(ccci_update_ap_md_smem_info_to_dt, ccci_update_ap_md_smem_info_to_dt);

/*=============================================================================
 * Export to LK user query AP MD share memory info.
 *=============================================================================
 */
int ccci_get_smem_info(int id, unsigned long long *ap_addr,
                       u32 *md_addr, u32 *size)
{
    u32 i;

    if (!s_nc_smem_tbl) {
        CCCI_TRACE_LOG("[ccci]%s share memory(nc) not initialized done\n", __func__);
        return -1;
    }
    if (!s_c_smem_tbl) {
        CCCI_TRACE_LOG("[ccci]%s share memory(c) not initialized done\n", __func__);
        return -1;
    }

    for (i = 0; i < s_nc_smem_tbl_num; i++) {
        if (s_nc_smem_tbl[i].inf.id == id) {
            if (ap_addr)
                *ap_addr = s_nc_smem_tbl[i].ap_phy;
            if (md_addr)
                *md_addr = s_nc_smem_tbl[i].inf.md_offset + 0x40000000;
            if (size)
                *size = s_nc_smem_tbl[i].inf.size;
            return 0;
        }
    }

    for (i = 0; i < s_c_smem_tbl_num; i++) {
        if (s_c_smem_tbl[i].inf.id == id) {
            if (ap_addr)
                *ap_addr = s_c_smem_tbl[i].ap_phy;
            if (md_addr)
                *md_addr = s_c_smem_tbl[i].inf.md_offset + 0x40000000;
            if (size)
                *size = s_c_smem_tbl[i].inf.size;
            return 0;
        }
    }

    return -1;
}
