/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <debug.h>
#include <init_mtk.h>
#include <libfdt.h>
#include <platform/reg.h>
#include <pmif_common.h>
#include <trace.h>

#define LOCAL_TRACE 0

static unsigned int pmic_all_rgn_en_lk;

void pmif_mpu_init(void *fdt)
{
    int offset, len;
    unsigned int *data;

    if (!fdt) {
        dprintf(CRITICAL, "[PMIF] fdt is NULL\n");
        return;
    }

    offset = fdt_path_offset(fdt, PMIF_MPU_DT_NODE_NAME);
    if (offset < 0) {
        dprintf(CRITICAL, "[PMIF] Failed to find " PMIF_MPU_DT_NODE_NAME " in dtb\n");
        return;
    }

    data = (unsigned int *)fdt_getprop(fdt, offset, PMIF_MPU_DT_PROP_NAME, &len);
    if (len <= 0 || !data) {
        dprintf(CRITICAL, "[PMIF] Failed to find property " PMIF_MPU_DT_PROP_NAME "\n");
        return;
    }

    pmic_all_rgn_en_lk = fdt32_to_cpu(*data);
}

void pmif_enable_mpu(void)
{
    u32 rgn_en = read32(&mtk_pmif->pmic_all_rgn_en);

    LTRACEF("PMIF MPU setting: 0x%x\n", pmic_all_rgn_en_lk);
    if (pmic_all_rgn_en_lk) {
        rgn_en |= pmic_all_rgn_en_lk;
        write32(&mtk_pmif->pmic_all_rgn_en, rgn_en);
    }
}
