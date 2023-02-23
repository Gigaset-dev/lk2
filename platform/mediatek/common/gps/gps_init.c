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
#include <sys/types.h>
#include <platform_mtk.h>

#ifdef WITH_PLATFORM_MEDIATEK_COMMON_EMI
#include <emi_mpu.h>
#define REGION_ID_GPS 47
#endif

#define GPS_EMI_MIRROR_OFFSET 0x400000000ULL
static unsigned int gps_get_fdt_prop(void *fdt, int offset, const char *name)
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

void gps_emi_init(void *fdt)
{
    status_t ret = NO_ERROR;
    paddr_t mblock_pa = 0;
    u64 mirror = 0;
    off_t mblock_sz = 0;
    int offset;
    unsigned int mver, mblock, msize, malign, mmax;
#ifdef WITH_PLATFORM_MEDIATEK_COMMON_EMI
    struct emi_region_info_t region_info;
#endif

    if (!fdt) {
        dprintf(CRITICAL, "%s fdt is NULL\n", __func__);
        return;
    }

    offset = fdt_path_offset(fdt, "/soc/gps");
    if (offset < 0) {
        offset = fdt_path_offset(fdt, "/gps");
        if (offset < 0) {
            dprintf(CRITICAL, "%s gps offset is not found\n", __func__);
            return;
        }
    }

    mver = gps_get_fdt_prop(fdt, offset, "emi-connac-ver");
    msize = gps_get_fdt_prop(fdt, offset, "emi-size");
    malign = gps_get_fdt_prop(fdt, offset, "emi-alignment");
    mmax = gps_get_fdt_prop(fdt, offset, "emi-max-addr");

    dprintf(INFO, "%s, mver=%d, msize=0x%x, malign=0x%x, mmax=0x%x, emi-connac-ver %d",
        __func__, mver, msize, malign, mmax, mver);

    if (msize == 0 || malign == 0 || mmax == 0) {
        dprintf(CRITICAL, "%s fail to get value from dts\n", __func__);
        return;
    }

    if (need_relocate()) {
        mblock_pa = mblock_alloc(msize, malign, mmax, 0, 0, "consys_gps_emi");
        mirror = mblock_alloc(msize, malign, MBLOCK_NO_LIMIT,
            (u64)((u64)mblock_pa + GPS_EMI_MIRROR_OFFSET), 0, "consys_gps_emi");
    } else {
        mblock_pa = mblock_alloc(msize, malign, mmax, 0, 0, "consys_gps_emi");
    }
    if (!mblock_pa) {
        dprintf(CRITICAL, "%s mblock allocation failed", __func__);
        return;
    }

    mblock = cpu_to_fdt32(mblock_pa);
    ret = fdt_setprop(fdt, offset, "emi-addr", &mblock, sizeof(mblock));
    if (ret < 0)
        dprintf(CRITICAL, "%s setprop emi-addr fail!! ret = %d\n", __func__, ret);

    dprintf(INFO, "%s, start=0x%x, end=0x%x, mirror=0x%llx\n",
        __func__, (unsigned int)mblock_pa, (unsigned int)(mblock_pa + msize), mirror);

#ifdef WITH_PLATFORM_MEDIATEK_COMMON_EMI
    region_info.start = mblock_pa;
    region_info.end = (mblock_pa + msize);
    region_info.region = REGION_ID_GPS; //TODO: assign connsys aid
    ret = emi_mpu_set_protection(&region_info);
    dprintf(INFO, "%s, emi_mpu_set_protection ret=%d\n", __func__, ret);
#endif
}

void gps_init(void *fdt)
{
    gps_emi_init(fdt);
}

SET_FDT_INIT_HOOK(gps_init, gps_init);

