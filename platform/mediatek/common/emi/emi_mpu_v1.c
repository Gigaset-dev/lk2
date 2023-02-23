/*
 * Copyright (c) 2021-2022 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <assert.h>
#include <debug.h>
#include <emi_mpu.h>
#include <platform/emi.h>
#include <platform_mtk.h>
#include <reg.h>
#include <smc.h>
#include <smc_id_table.h>
#include <sys/types.h>

#include "emi_util.h"

#define emi_mpu_smc_set(start, end, apc, res) \
    __smc_conduit(MTK_SIP_EMIMPU_CONTROL, EMIMPU_SET, start, end, apc, 0, 0, 0, &res)
#define emi_mpu_smc_clear(region, res) \
    __smc_conduit(MTK_SIP_EMIMPU_CONTROL, EMIMPU_CLEAR, region, 0, 0, 0, 0, 0, &res)
#define emi_mpu_smc_check_region_info(region, res) \
    __smc_conduit(MTK_SIP_EMIMPU_CONTROL, EMIMPU_READ, EMIMPU_CHECK_REGION_INFO,\
                region, 0, 0, 0, 0, &res)
#define emi_mpu_smc_check_ns_cpu(res) \
    __smc_conduit(MTK_SIP_EMIMPU_CONTROL, EMIMPU_READ, EMIMPU_CHECK_NS_CPU, 0, 0, 0, 0, 0, &res)

static struct unmap_region unmap_region_info[EMI_MPU_REGION_NUM];

/* for gz_unmap2.c */
paddr_t check_violation_for_unmap(void)
{
    ulong temp_addr;

    if (readl(EMI_MPU_MPUS)) {
            temp_addr = (((ulong)readl(EMI_MPU_MPUT_2ND) & (ulong)0xF << (ulong)32) |
                                 readl(EMI_MPU_MPUT));
            EMI_DBG("temp_addr: 0x%lx\n", temp_addr);
            return (paddr_t)(temp_addr + EMI_DRAM_BASE);
    } else
        return 0;
}

/* for gz_unmap2.c */
unsigned int check_ns_cpu_perm(void)
{
    unsigned long long region, val;
    struct __smccc_res res;
    unsigned int i, count = 0;

    emi_mpu_smc_check_ns_cpu(res);
    region = res.a0;

    for (i = 0; i < EMI_MPU_REGION_NUM; i++) {
        val = (region >> i) & 0x1;
        if (val) {
            unmap_region_info[count].region = i;

            emi_mpu_smc_check_region_info(i, res);
            unmap_region_info[count].enable = res.a0;
            unmap_region_info[count].start = res.a1;
            unmap_region_info[count].end = res.a2;
            count++;
        }
    }

    return count;
}

/* for gz_unmap2.c */
struct unmap_region *get_region_info(void)
{
    return &unmap_region_info;
}

/* for gz_unmap2.c */
void clear_violation(void)
{
    writel(0x0, EMI_MPU_MPUS);
    writel(0x0, EMI_MPU_MPUT);
    writel(0x0, EMI_MPU_MPUT_2ND);
}

int emi_mpu_set_protection(struct emi_region_info_t *region_info)
{
    unsigned int start, end, bootstage;
    unsigned int dgroup, region;
    struct __smccc_res res;
    int i;

#ifdef DISABLE_EMI_MPU
    EMI_DBG("EMI MPU v1 not enable\n");
    return 0;
#endif

    if (region_info->region >= EMI_MPU_REGION_NUM) {
        EMI_DBG("EMI MPU can not support region %u\n",
                region_info->region);
        return -1;
    }

    /* For SMPU and legacy mpu different end address definition */
    if (0xFFFF == (region_info->end & 0xFFFF)) {
        region_info->end += 1;
    }

    if ((region_info->start & 0xFFFF) | (region_info->end & 0xFFFF)) {
        EMI_DBG("LK %s start:0x%llx end=0x%llx is not 64kB alignment!\n",
                __func__, region_info->start, region_info->end);
        ASSERT(0);
    }
    EMI_DBG("LK %s start:0x%llx end=0x%llx region=%u\n", __func__,
            region_info->start, region_info->end, region_info->region);

    start = (unsigned int)(region_info->start >> EMI_MPU_ALIGN_BITS) |
            (region_info->region << 24);
    region = (start >> 24) & 0xFF;

    bootstage = get_boot_stage();

    if (bootstage == BOOT_STAGE_BL2_EXT || bootstage == BOOT_STAGE_AEE) {
        for (i = EMI_MPU_DGROUP_NUM - 1; i >= 0; i--) {
            end = (unsigned int)(region_info->end >> EMI_MPU_ALIGN_BITS) |
                    (i << 24);

            dgroup = (end >> 24) & 0xFF;
            start &= 0x00FFFFFF;
            end &= 0x00FFFFFF;

            if (i == (EMI_MPU_DGROUP_NUM - 1)) {
                if ((start < DRAM_OFFSET) || (end < start)) {
                    EMI_DBG("BL2_EXT: start:0x%x end:0x%x, error!\n", start, end);
                    return -1;
                } else {
                    start -= DRAM_OFFSET;
                }
                if ((region >= EMI_MPU_REGION_NUM) || (dgroup > EMI_MPU_DGROUP_NUM)) {
                    EMI_DBG("BL2_EXT: region:%u dgroup:%u, error!\n", region, dgroup);
                    return -1;
                }
            }

            end -= DRAM_OFFSET;
            /* For SMPU and legacy mpu different end address definition */
            end -= 0x1;
            EMI_DBG("BL2_EXT: %s i:%d start:0x%x end=0x%x region=%u ",
                      __func__, i, start, end, region_info->region);
            EMI_DBG("apc%d=0x%x\n", i, region_info->apc[i]);
            writel(start, EMI_MPU_SA(region));
            writel(end, EMI_MPU_EA(region));
            writel(region_info->apc[i], EMI_MPU_APC(region, dgroup));
        }
        return 0;
    } else if (bootstage == BOOT_STAGE_BL33) {
        for (i = EMI_MPU_DGROUP_NUM - 1; i >= 0; i--) {
            end = (unsigned int)(region_info->end >> EMI_MPU_ALIGN_BITS) |
                    (i << 24);
            EMI_DBG("BL33:  %s i:%d (region)start:0x%x (dgroup)end=0x%x region=%u\n", __func__,
                    i, start, (end - 1), region_info->region);
            emi_mpu_smc_set(start, end, region_info->apc[i], res);
            EMI_DBG("apc%d=0x%x, result=%d\n", i, region_info->apc[i], (int)res.a0);
        }
        return (int)res.a0;
    } else {
        EMI_DBG("boot stage error!\n");
        return -1;
    }
}

int emi_mpu_clear_protection(struct emi_region_info_t *region_info)
{
    struct __smccc_res res;

#ifdef DISABLE_EMI_MPU
    EMI_DBG("EMI MPU v1 not enable\n");
    return 0;
#endif

    if (region_info->region >= EMI_MPU_REGION_NUM) {
        EMI_DBG("EMI MPU can not support region:%u\n",
                region_info->region);
        return -1;
    }

    emi_mpu_smc_clear(region_info->region, res);

    return 0;
}
