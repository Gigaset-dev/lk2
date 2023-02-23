/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <assert.h>
#include <debug.h>
#include <err.h>
#include <libfdt.h>
#include <mblock.h>
#include <set_fdt.h>

#ifdef WITH_PLATFORM_MEDIATEK_COMMON_EMI
#include <emi_mpu.h>
#define REGION_ID_CONNINFRA 46
#endif

#define CONNSYS_WIFI_MBLOCK_NAME "shared-dma-pool_wifi-reserve-memory_dma"

struct connsys_dt_dram_info {
    unsigned int start_hi;
    unsigned int start_lo;
    unsigned int size_hi;
    unsigned int size_lo;
};

struct connsys_dt_size_info {
    unsigned int size_hi;
    unsigned int size_lo;
};

static unsigned int consys_get_fdt_prop(void *fdt, int offset, const char *name)
{
    const struct fdt_property *prop;
    unsigned int *data;
    unsigned int value;
    int len;

    prop = fdt_get_property(fdt, offset, name, &len);
    if (!prop) {
        dprintf(CRITICAL, "[%s][%s] is not found\n", __func__, name);
        return 0;
    }
    data = (uint32_t *)prop->data;
    value = fdt32_to_cpu(*data);

    return value;
}

static void consys_init_conninfra(void *fdt)
{
    status_t ret = NO_ERROR;
    paddr_t mblock_pa = 0;
    int offset;
    fdt64_t mblock;
    unsigned int msize, malign, mmax;
    const char *prop_size = "emi-size";
    const char *prop_alignment = "emi-alignment";
    const char *prop_max_addr = "emi-max-addr";
#ifdef WITH_PLATFORM_MEDIATEK_COMMON_EMI
    struct emi_region_info_t region_info;
#endif

    if (!fdt) {
        dprintf(CRITICAL, "%s fdt is NULL\n", __func__);
        return;
    }

    offset = fdt_path_offset(fdt, "/soc/consys");
    if (offset < 0) {
        offset = fdt_path_offset(fdt, "/consys");
        if (offset < 0) {
            dprintf(CRITICAL, "%s consys offset is not found\n", __func__);
            return;
        }
    }

    msize = consys_get_fdt_prop(fdt, offset, prop_size);
    malign = consys_get_fdt_prop(fdt, offset, prop_alignment);
    mmax = consys_get_fdt_prop(fdt, offset, prop_max_addr);

    if (msize == 0 || malign == 0 || mmax == 0) {
        dprintf(CRITICAL, "[%s] fail to get value from dts\n", __func__);
        return;
    }

    mblock_pa = mblock_alloc(msize, malign, mmax, 0, 0, "consys_emi_reserved");
    if (!mblock_pa) {
        dprintf(CRITICAL, "[%s] mblock allocation failed", __func__);
        return;
    }

    mblock = cpu_to_fdt64(mblock_pa);
    ret = fdt_setprop(fdt, offset, "emi-addr", &mblock, sizeof(mblock));
    if (ret < 0)
        dprintf(CRITICAL, "[%s] setprop emi-addr fail!! ret = %d\n", __func__, ret);

#ifdef WITH_PLATFORM_MEDIATEK_COMMON_EMI
    region_info.start = mblock_pa;
    region_info.end = (mblock_pa + msize);
    region_info.region = REGION_ID_CONNINFRA;
    emi_mpu_set_protection(&region_info);
#endif
}

static void consys_init_wifi(void *fdt)
{
    status_t ret = NO_ERROR;
    paddr_t mblock_pa = 0;
    int offset;
    unsigned int mblock, msize, malign, mmax;
    struct connsys_dt_dram_info rsv_mem_reg_property = {0};
    struct connsys_dt_size_info rsv_mem_size_property = {0};
#ifdef WITH_PLATFORM_MEDIATEK_COMMON_EMI
    struct emi_region_info_t region_info;
#endif

    if (!fdt) {
        dprintf(CRITICAL, "%s fdt is NULL\n", __func__);
        return;
    }

    offset = fdt_path_offset(fdt, "/soc/wifi");
    if (offset < 0) {
        offset = fdt_path_offset(fdt, "/wifi");
        if (offset < 0) {
            dprintf(CRITICAL, "%s consys offset is not found\n", __func__);
            return;
        }
    }

    msize = consys_get_fdt_prop(fdt, offset, "emi-size");
    malign = consys_get_fdt_prop(fdt, offset, "emi-alignment");
    mmax = consys_get_fdt_prop(fdt, offset, "emi-max-addr");

    if (msize == 0 || malign == 0 || mmax == 0) {
        dprintf(CRITICAL, "fail to get value from dts\n", __func__);
        return;
    }


    /*
     *   Create reserve-memory use mblock API
     *
     *   wifi_mem: wifi-reserve-memory {
     *      compatible = "shared-dma-pool";
     *      no-map;
     *      size = <0 0xf20000>;
     *      alignment = <0 0x1000000>;
     *      alloc-ranges = <0 0x40000000 0 0x80000000>;
     *   };
     *
     */

    /* Generate reserve-memory with compatible = "shared-dma-pool" */
    /* and add subnode to fdt */
    mblock_pa = mblock_alloc(msize, malign, mmax, 0, 0,
        CONNSYS_WIFI_MBLOCK_NAME);
    if (!mblock_pa) {
        dprintf(CRITICAL, "%s mblock allocation failed\n", __func__);
        return;
    }

    if (fdt_path_offset(fdt, "/soc/wifi") >= 0)
        ret = mblock_pre_alloc_phandle("/soc/wifi", CONNSYS_WIFI_MBLOCK_NAME);
    else
        ret = mblock_pre_alloc_phandle("/wifi", CONNSYS_WIFI_MBLOCK_NAME);
    if (ret)
        dprintf(CRITICAL, "%s pre alloc phandle failed!! ret = %d\n", __func__, ret);

#ifdef WITH_PLATFORM_MEDIATEK_COMMON_EMI
    region_info.start = mblock_pa;
    region_info.end = (mblock_pa + msize);
    region_info.region = 44; //TODO: assign Wi-Fi PDMA SR#
    emi_mpu_set_protection(&region_info);
#endif
}

static void consys_init(void *fdt)
{
    consys_init_conninfra(fdt);
    consys_init_wifi(fdt);
}


SET_FDT_INIT_HOOK(consys_init, consys_init);
