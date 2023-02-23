/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <arch/arm64.h>
#include <platform/plat_cs.h>
#include <reg.h>

void disable_cache_share(void)
{
    /*
     * L3 Cache Share Disable Flow:
     * 1) Configured the "L3_SHARE_EN" to 0 for disabling the L3 Share function.
     * 2) Executed DSB instruction.
     * 3) Configured the "CLUSTERPWRCTLR[5:4]" register to "11"
     * 4) Polling the "CLUSTERPWRSTAT[5:4]" register is equal to "11"
     * 5) Configured the "L3_SHARE_PRE_EN" to 0 for releasing the SRAM power fixed.
     * 7) Configured the "L3_SHARE_FULLnHALF" to 0 to default value
     * 6) Executed DSB instruction.
     *
     */

    unsigned long v = 0;
    /* clean l3_share_en */
    writel(readl(MP0_CLUSTER_CFG0) & ~(1 << L3_SHARE_EN), MP0_CLUSTER_CFG0);
    DSB;

    __asm__ volatile ("mrs %0, S3_0_C15_C3_5" : "=r" (v));
    v |= (CLUST_DIS_VAL << CLUST_DIS_SHIFT);
    __asm__ volatile ("msr S3_0_C15_C3_5, %0" : : "r" (v));
    DSB;

PWR_LOOP:
    __asm__ volatile ("mrs %0, S3_0_C15_C3_7" : "=r" (v));
    if (((v >> CLUST_DIS_SHIFT) & CLUST_DIS_VAL) != CLUST_DIS_VAL)
        goto PWR_LOOP;

    writel(readl(MP0_CLUSTER_CFG0) & ~(1 << L3_SHARE_PRE_EN), MP0_CLUSTER_CFG0);
    writel(readl(MP0_CLUSTER_CFG0) & ~(1 << L3_SHARE_FULLnHALF), MP0_CLUSTER_CFG0);
    DSB;
}
