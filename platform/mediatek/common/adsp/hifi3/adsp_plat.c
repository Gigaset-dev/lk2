/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include "adsp_plat.h"
#include <arch/ops.h>
#include <assert.h>
#ifdef WITH_PLATFORM_MEDIATEK_COMMON_EMI
#include <emi_mpu.h>
#endif
#include <err.h>
#include <init_mtk.h>
#include <libfdt.h>
#include <load_image.h>
#include <mblock.h>
#include <mtcmos.h>
#include <platform_mtk.h>
#include <platform/adsp_reg.h>
#include <platform/reg.h>
#include <set_fdt.h>
#include <smc.h>
#include <smc_id_table.h>
#include <stdlib.h>
#include <string.h>
#include <trace.h>

#define LOCAL_TRACE (0)

#define adsp_smc_send(_opid, _val1, _val2)                   \
({                                                           \
    struct __smccc_res res;                            \
    __smc_conduit(MTK_SIP_BL_ADSP_CONTROL,           \
                  _opid, _val1, _val2, 0, 0, 0, 0, &res);\
    res.a0;                                              \
})

static void adsp_init(unsigned int level)
{
    adsp_set_devapc_domain();
    /* set default status: disabled */
    adsp_set_status(0);
}

MT_LK_INIT_HOOK_BL33(adsp_init, adsp_init, LK_INIT_LEVEL_PLATFORM);

void *adsp_dram_alloc(void **phy, unsigned int *size)
{
    unsigned int cid, idx;
    unsigned int dram_size = 0;
    struct adsp_imgs_t *img;
    uint64_t adsp_dram_lower_bound, adsp_dram_img_max;
    uint64_t ccci_base = 0;
    const struct reserved_t *mblock_reserved;

    /* for loop to get whole size */
    for (cid = 0; cid < adsp_get_core_num(); cid++) {
        if (adsp_core_exist(cid) == 0)
            assert(0);

        for (idx = 0; idx < 3; idx++) {
            img = adsp_get_img_info(cid, idx);
            dram_size += img->size;
        }
    }
    dram_size = ROUNDUP(dram_size, RESERVED_MEM_ALIGN);

    *size = dram_size;

    if (need_relocate()) {
        adsp_dram_lower_bound = ADSP_DRAM_LOWER_BOUND_H;
        adsp_dram_img_max = ADSP_DRAM_IMG_ADDR_H_MAX;
    } else {
        adsp_dram_lower_bound = ADSP_DRAM_LOWER_BOUND_L;
        adsp_dram_img_max = ADSP_DRAM_IMG_ADDR_L_MAX;
    }

    mblock_reserved = mblock_query_reserved_by_name(CCCI_SHAREDC_RSV_NAME, 0);
    if (mblock_reserved && mblock_reserved->start) {
        ccci_base = mblock_reserved->start;
        adsp_dram_img_max = MIN(adsp_dram_img_max, ccci_base);
    } else {
        LTRACEF("mblock (%s) is not reserved. adsp_dram_img_max  use default %llx\n",
                CCCI_SHAREDC_RSV_NAME, adsp_dram_img_max);
    }

    *phy = (void *) (u64) mblock_alloc_range(dram_size, ADSP_DRAM_ADDR_ALIGN,
           adsp_dram_lower_bound, adsp_dram_img_max, 0, 0, ADSP_IMG_RSV_NAME);

    return *phy;
}

static void adsp_sw_reset(void)
{
    setbits32(ADSP_CFGREG_SW_RSTN, ADSP_SW_RSTN);
    udelay(1);
    clrbits32(ADSP_CFGREG_SW_RSTN, ADSP_SW_RSTN);
}

void adsp_update_sram_imgs_base(void *base)
{
    unsigned int cid;
    unsigned int SRAM_IDX = 2;
    struct adsp_imgs_t *img;

    for (cid = 0; cid < adsp_get_core_num(); cid++) {
        img = adsp_get_img_info(cid, SRAM_IDX);
        img->base = base;
        adsp_set_img_info(cid, SRAM_IDX, img);
        base += img->size;
    }
}

static int adsp_image_checker(void *addr)
{
    unsigned int cid, img_id, idx;
    struct adsp_imgs_t *img;
    unsigned int *paddr, *img_base_vir;

    paddr = (uint32_t *)addr;
    for (cid = 0; cid < adsp_get_core_num(); cid++) {
        for (img_id = 0; img_id < 2; img_id++) {
            img = adsp_get_img_info(cid, img_id);
            img_base_vir = (uint32_t *)ADDR_PHY_TO_VIR(img->base);

            for (idx = 0; idx < img->size / sizeof(uint32_t); idx++) {
                if (*(img_base_vir + idx) != *(paddr + idx)) {
                    dprintf(CRITICAL, "%s idx:%d check fail dram: %x sram:%x\n",
                            __func__, idx,
                            *(paddr + idx), *(img_base_vir + idx));
                    return ERR_NOT_VALID;
                }
            }
            paddr += img->size / sizeof(uint32_t);
        }
    }

    return 0;
}

int verify_load_adsp_img(const char *part_name, void *addr_phy, void *addr)
{
    unsigned int cid, img_id;
    unsigned int core_num = adsp_get_core_num();
    /* virtual address for lk load image from flash */
    void *img_ptr = addr;
    unsigned int img_size = 0;
    struct adsp_imgs_t *img;
#ifdef ADSP_LOADER_SUPPORT
    unsigned int sram_img_off;
    struct __smccc_res res;
#endif

    /* load coreX sram first */
    for (cid = 0; cid < core_num; cid++) {
        if (!adsp_core_exist(cid))
            continue;
        img = adsp_get_img_info(cid, 2);  // img: hifi3_x_sram
        img_size = load_partition(part_name, img->name, img_ptr, img->size);
        if (img_size <= 0 || img_size > img->size) {
            LTRACEF("%s load adsp image %s fail\n", LOG_TAG, img->name);
            return ERR_BAD_LEN;
        }
        LTRACEF("%s load %s done. addr=%p, size=%x\n",
           LOG_TAG, img->name, img_ptr, img->size);

        img_ptr += img->size;
    }
#ifdef ADSP_LOADER_SUPPORT
    sram_img_off = (unsigned int)(img_ptr - addr);
#endif
    /* enable adsp clock & release CPU to access hifi3 */
    adsp_power_on();
    adsp_sw_reset();

    for (cid = 0; cid < core_num; cid++) {
        if (!adsp_core_exist(cid))
            continue;

        for (img_id = 0; img_id < 2; img_id++) {
            void *img_base_vir;

            img = adsp_get_img_info(cid, img_id);
            img_base_vir = ADDR_PHY_TO_VIR(img->base);
            img_size = load_partition(part_name, img->name, img_ptr, img->size);
            if (img_size <= 0 || img_size > img->size) {
                LTRACEF("%s load adsp image %s fail\n", LOG_TAG, img->name);
                return ERR_BAD_LEN;
            }
#ifndef ADSP_LOADER_SUPPORT
            memcpy(img_base_vir, img_ptr, img_size);
#endif
            LTRACEF("%s load %s done. addr=%p, size=%x(%x)\n",
                    LOG_TAG, img->name, img_ptr, img->size, img_size);
            /* increase img_ptr if tcm images need to back up in DRAM */
            img_ptr += img->size;
        }
    }

#ifdef ADSP_LOADER_SUPPORT
    arch_clean_invalidate_cache_range((addr_t)addr, (size_t)(img_ptr-addr));
    __smc_conduit(MTK_SIP_BL_ADSP_CONTROL,
                  MTK_ADSP_BL_OP_LOAD_IMG,
                  addr_phy, sram_img_off, 0, 0, 0, 0, &res);
    if (res.a0)
        dprintf(CRITICAL, "%s loader timeout. status:%lx\n", LOG_TAG, res.a0);

    if (adsp_image_checker(addr + sram_img_off) != 0)
        return ERR_NOT_VALID;

    __smc_conduit(MTK_SIP_BL_ADSP_CONTROL,
                  MTK_ADSP_BL_OP_CFG_BOOTUP, 0, 0, 0, 0, 0, 0, &res);

#endif
    adsp_power_off();

    return 0;
}

void adsp_emi_protect_enable(const unsigned int id, void *addr, unsigned int size)
{
    /* setup EMI MPU
     * domain 0: AP
     * domain 10: ADSP
     */
#ifdef WITH_PLATFORM_MEDIATEK_COMMON_EMI
    struct emi_region_info_t region_info;

    region_info.start = (unsigned long long)addr;
    region_info.end = (unsigned long long)(addr + size);
    region_info.region = id;
    SET_ACCESS_PERMISSION(region_info.apc, UNLOCK,
        FORBIDDEN, FORBIDDEN, FORBIDDEN, FORBIDDEN,
        FORBIDDEN, NO_PROTECTION, FORBIDDEN, FORBIDDEN,
        FORBIDDEN, FORBIDDEN, FORBIDDEN, FORBIDDEN,
        FORBIDDEN, FORBIDDEN, FORBIDDEN, SEC_R_NSEC_R);
    emi_mpu_set_protection(&region_info);
#else
    LTRACEF("(bypass)%s id=%d, addr=%p\n", __func__, id, addr);
#endif
}

static int calc_and_update_audio_rsv(void *fdt)
{

    int nodeoffset = 0, property;
    unsigned int sum_mem = 0;
    unsigned int afe_enable = 0;
    fdt32_t rsv_size;

    nodeoffset = fdt_node_offset_by_compatible(fdt, -1, SND_ADSP_DT_NODE);
    if (nodeoffset < 0) {
        LTRACEF("%s %s is not found. Use default setting in dts.\n",
            LOG_TAG, SND_ADSP_DT_NODE);
        return -1;
    }

    fdt_for_each_property_offset(property, fdt, nodeoffset) {
        const char *name;
        unsigned int *prop;
        int prop_len;
        unsigned int mem, enable;

        prop = (unsigned int *)fdt_getprop_by_offset(fdt, property, &name, &prop_len);
        if (prop_len < 0) {
            LTRACEF("%s get %s property fail prop_len=%d\n", LOG_TAG, name, prop_len);
            assert(0);
        }

        if (prop_len == 20) { /* audio feature: 5*sizeof(unsigned int) */
            enable = fdt32_to_cpu(*prop);
            mem = fdt32_to_cpu(*(prop + 4));
            if (enable) {
                sum_mem += mem;
                afe_enable = 1;
            }
        } else if (prop_len == 8) { /* afe mem: 2*sizeof(unsigned int)*/
            enable = fdt32_to_cpu(*prop);
            mem = fdt32_to_cpu(*(prop + 1));
            if (afe_enable & enable)
                sum_mem += mem;
        } else { /* bypass */
            continue;
        }
    }

    /* write audio reserved memory back to adsp_common node */
    nodeoffset = fdt_node_offset_by_compatible(fdt, -1, ADSP_DT_NODE_COMMON);
    if (nodeoffset < 0) {
        LTRACEF("%s failed to find %s in dtb\n", LOG_TAG, ADSP_DT_NODE_COMMON);
        assert(0);
    }
    rsv_size = cpu_to_fdt32(sum_mem);
    fdt_setprop_inplace(fdt, nodeoffset, ADSP_RSV_AUDIO_PROPERTY,
                        &rsv_size, sizeof(rsv_size));

    return 0;
}

/* sum reserved memory from adsp_common node,
 * and update value to reserved-adsp node in dts
 */
static int update_adsp_rsv_node(void *fdt)
{
    int nodeoffset_common, nodeoffset_share, property;
    int ret;
    unsigned int sum_mem = 0;
    fdt64_t rsv_size, rsv_info[2];
    uint64_t mem_shared_max, mem_shared_phy, mem_shared_size;
    uint64_t image_base = 0, adsp_dram_shared_max;
    const struct reserved_t *mblock_reserved;

    if (fdt == NULL)
        panic("kernel fdt is NULL!\n");

    nodeoffset_common = fdt_node_offset_by_compatible(fdt, -1, ADSP_DT_NODE_COMMON);
    if (nodeoffset_common < 0) {
        LTRACEF("%s failed to find %s in dtb\n", LOG_TAG, ADSP_DT_NODE_COMMON);
        assert(0);
    }

    fdt_for_each_property_offset(property, fdt, nodeoffset_common) {
        const char *name;
        unsigned int *prop;
        int prop_len;

        prop = (unsigned int *)fdt_getprop_by_offset(fdt, property, &name, &prop_len);
        if (prop_len < 0) {
            LTRACEF("%s get %s property fail prop_len=%d\n", LOG_TAG, name, prop_len);
            assert(0);
        }
        if (strstr(name, ADSP_RSV_PROPERTY_PREFIX) != NULL)
            sum_mem += ROUNDUP(fdt32_to_cpu(*prop), ADSP_MPU_MEM_ALIGN);
    }
    sum_mem = ROUNDUP(sum_mem, RESERVED_MEM_ALIGN);

    nodeoffset_share = fdt_node_offset_by_compatible(fdt, -1, ADSP_DT_NODE_MEM_SHARED);
    /* Reserved shared memory in Kernel if ADSP_DT_NODE_MEM_SHARED is found in dts.
     * Otherwise, reserved in LK, and update (phys_addr, size) into ADSP_DT_NODE_COMMON.
     */
    if (nodeoffset_share >= 0) {
        LTRACEF("%s %s is found in dtb, update size and return\n",
                LOG_TAG, ADSP_DT_NODE_MEM_SHARED);
        rsv_size = cpu_to_fdt64((u64)sum_mem);
        fdt_setprop_inplace(fdt, nodeoffset_share, "size", &rsv_size, sizeof(rsv_size));
        return 0;
    }

    /* malloc */
    if (need_relocate())
        adsp_dram_shared_max = ADSP_DRAM_SHARED_ADDR_H_MAX;
    else
        adsp_dram_shared_max = ADSP_DRAM_SHARED_ADDR_L_MAX;

    mblock_reserved = mblock_query_reserved_by_name(ADSP_IMG_RSV_NAME, 0);
    if (mblock_reserved && mblock_reserved->start) {
        image_base = mblock_reserved->start;
    } else {
        LTRACEF("mblock(%s) is not reserved.\n", ADSP_IMG_RSV_NAME);
        return 0;
    }

    mem_shared_max = MIN(image_base + ADSP_DRAM_REMAP_LIMIT, adsp_dram_shared_max);
    mem_shared_size = (u64)ROUNDUP(sum_mem, RESERVED_MEM_ALIGN);
    mem_shared_phy = mblock_alloc_range(mem_shared_size, ADSP_DRAM_ADDR_ALIGN,
                     image_base, mem_shared_max, 0, 0, ADSP_SHARED_RSV_NAME);

    if (mem_shared_phy != 0) {
        if (adsp_get_status()) {
            /* write shared mem info into adsp mpu region via smc */
            ret = adsp_smc_send(MTK_ADSP_BL_OP_CFG_MPU, mem_shared_phy, mem_shared_size);
            if (ret < 0) {
                LTRACEF("%s adsp mpu fail.\n", LOG_TAG);
                return ret;
            }
        }
        adsp_emi_protect_enable(MPU_REGION_ID_ADSP_SHARED_MEM,
                                (void *)mem_shared_phy, mem_shared_size);
        rsv_info[0] = cpu_to_fdt64(mem_shared_phy);
        rsv_info[1] = cpu_to_fdt64(mem_shared_size);
        ret = fdt_setprop(fdt, nodeoffset_common, "shared_memory", &rsv_info, sizeof(rsv_info));
        if (ret < 0) {
            LTRACEF("%s set \"shared_memory\" property fail.\n", LOG_TAG);
            return ret;
        }
    } else {
        LTRACEF("%s mblock_alloc_range %s fail\n", LOG_TAG, ADSP_SHARED_RSV_NAME);
        return -ERR_NO_MEMORY;
    }
    return 0;
}

/* parse and calculate adsp reserved memory from dts */
static void _parse_adsp_rsv_mem(void *fdt)
{
    int ret = 0;
    /* audio feature from dts.
     * return -1: use default setting in dts without update.
     */
    ret = calc_and_update_audio_rsv(fdt);
    if (ret == 0)
        update_adsp_rsv_node(fdt);
}

void platform_fdt_adsp_params(void *fdt)
{
    int commonoffset, nodeoffset, ret;
    char cdata[10] = {0};
    char compat_name[32];
    unsigned int cid = 0;
    unsigned long long system[2];
    int n = 0, core_status = 0;
    struct adsp_imgs_t *img;

    /* update adsp common/ coreX status according to load status.
     */
    commonoffset = fdt_node_offset_by_compatible(fdt, -1, ADSP_DT_NODE_COMMON);
    if (commonoffset < 0) {
        LTRACEF("%s failed to find %s in dtb\n", LOG_TAG, ADSP_DT_NODE_COMMON);
        assert(0);
    }
    if (snprintf(cdata, sizeof(cdata), "%s", adsp_get_status()?"okay" : "disabled") < 0)
        assert(0);

    LTRACEF("%s set adsp status = %s\n", LOG_TAG, cdata);
    ret = fdt_setprop_string(fdt, commonoffset, "status", cdata);
    if (ret < 0) {
        LTRACEF("%s failed to set status ret=%d\n", LOG_TAG, ret);
        assert(0);
    }

    for (cid = 0; cid < adsp_get_core_num(); cid++) {
        memset(compat_name, 0, sizeof(compat_name));
        n = snprintf(compat_name, sizeof(compat_name), "%s_%d",
            ADSP_DT_NODE_CORE, cid);
        if (n < 0 || n > (int)sizeof(compat_name))
            assert(0);
        nodeoffset = fdt_node_offset_by_compatible(fdt, -1, compat_name);
        if (nodeoffset < 0) {
            LTRACEF("%s failed to find %s in dtb\n", LOG_TAG, compat_name);
            assert(0);
        }

        core_status = adsp_core_exist(cid);
        if (core_status == 0) {
            if (snprintf(cdata, sizeof(cdata), "%s", "disabled") < 0)
                assert(0);

            ret = fdt_setprop_string(fdt, nodeoffset, "status", cdata);
            if (ret < 0) {
                LTRACEF("%s failed to set core%d status ret=%d\n", LOG_TAG, cid, ret);
                assert(0);
            }
            continue;
        }

        /* pass parameter to kernel */
        img = adsp_get_img_info(cid, 2);
        system[0] = cpu_to_fdt64((uint64_t)img->base);
        system[1] = cpu_to_fdt64((uint64_t)img->size);
        ret = fdt_setprop(fdt, nodeoffset, "system", &system, sizeof(system));
        if (ret < 0) {
            LTRACEF("%s failed to set core%d system to dts ret=%d\n", LOG_TAG, cid, ret);
            assert(0);
        }
    }

    /* parse and calculate memory need for audio resv from fdt */
    _parse_adsp_rsv_mem(fdt);

    return;
}

SET_FDT_INIT_HOOK(platform_fdt_adsp_params, platform_fdt_adsp_params);
