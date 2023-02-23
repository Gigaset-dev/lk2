/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <debug.h>
#include <emi_mpu.h>
#include <platform/emi.h>
#include <platform_mtk.h>
#include <reg.h>
#include <smc.h>
#include <smc_id_table.h>
#include <sys/types.h>

#include "emi_util.h"

#define emi_mpu_smc_set(start, end, region, res) \
    __smc_conduit(MTK_SIP_EMIMPU_CONTROL, EMIMPU_SET, start, end, region, 0, 0, 0, &res)
#define emi_kp_smc_set(start, end, region, res) \
    __smc_conduit(MTK_SIP_EMIMPU_CONTROL, KP_SET, start, end, region, 0, 0, 0, &res)
#define emi_mpu_smc_clear(region, res) \
    __smc_conduit(MTK_SIP_EMIMPU_CONTROL, EMIMPU_CLEAR, region, 0, 0, 0, 0, 0, &res)
#define emi_mpu_smc_check_region_info(region, res) \
    __smc_conduit(MTK_SIP_EMIMPU_CONTROL, EMIMPU_READ, EMIMPU_CHECK_REGION_INFO,\
                region, 0, 0, 0, 0, &res)
#define emi_mpu_smc_check_ns_cpu(res) \
    __smc_conduit(MTK_SIP_EMIMPU_CONTROL, EMIMPU_READ, EMIMPU_CHECK_NS_CPU, 0, 0, 0, 0, 0, &res)
#define emi_mpu_smc_set_md_perm(region, perm, res) \
    __smc_conduit(MTK_SIP_EMIMPU_CONTROL, AID_TABLE_SET, region, perm, 0, 0, 0, 0, &res)

static struct unmap_region unmap_region_info[EMI_MPU_REGION_NUM];

int emi_kp_set_protection(unsigned int start, unsigned int end, unsigned int region)
{
    struct __smccc_res res;

    if (region >= EMI_KP_REGION_NUM) {
        EMI_DBG("kernel protection can not support region %u\n", region);
        return -1;
    }

    emi_kp_smc_set(start, end, region, res);

    return (int)res.a0;
}

paddr_t check_violation_for_unmap(void)
{
    ulong temp_addr;

    if (readl(SMPU_READ_AID)) {
            temp_addr = (((ulong)readl(SMPU_READ_ADDR_MSB) << (ulong)32) |
                (readl(SMPU_READ_ADDR_H) << 16) | readl(SMPU_READ_ADDR_L));
            return (paddr_t)(temp_addr + EMI_DRAM_BASE);
#ifdef SUB_EMI_SMPU
    } else  if (readl(SUB_SMPU_READ_AID)) {
            temp_addr = (((ulong)readl(SUB_SMPU_READ_ADDR_MSB) << (ulong)32) |
                (readl(SUB_SMPU_READ_ADDR_H) << 16) | readl(SUB_SMPU_READ_ADDR_L));
            return (paddr_t)(temp_addr + EMI_DRAM_BASE);
#endif
    } else
        return 0;
}

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
            unmap_region_info[count].region = i + 1;

            emi_mpu_smc_check_region_info(i + 1, res);
            unmap_region_info[count].enable = res.a0;
            unmap_region_info[count].start = res.a1;
            unmap_region_info[count].end = res.a2;

            count++;
        }
    }

    return count;
}

int set_md_region(unsigned int region, unsigned int perm)
{
    struct __smccc_res res;

    emi_mpu_smc_set_md_perm(region, perm, res);

    return (int)res.a0;
}

struct unmap_region *get_region_info(void)
{
    return &unmap_region_info;
}

void clear_violation(void)
{
    unsigned int i;

    writel(0x1, SMPU_READ_CLEAR);
    writel(0x0, SMPU_READ_CLEAR);
    writel(0x80000000, EMI_MPU_CLEAR);
    writel(0x40000000, EMI_MPU_HP_CLEAR);
    for (i = 0; i < 16; i++) {
        writel(0x3, (EMI_MPU_CLEAR_CONTENT + 0x4*i));
        writel(0xffffffff, (EMI_MPU_DOMAIN_ABORT + 0x4*i));
    }

#ifdef SUB_EMI_SMPU
    writel(0x1, SUB_SMPU_READ_CLEAR);
    writel(0x0, SUB_SMPU_READ_CLEAR);
    writel(0x80000000, SUB_EMI_MPU_CLEAR);
    writel(0x40000000, SUB_EMI_MPU_HP_CLEAR);
    for (i = 0; i < 16; i++) {
        writel(0x3, (SUB_EMI_MPU_CLEAR_CONTENT + 0x4*i));
        writel(0xffffffff, (SUB_EMI_MPU_DOMAIN_ABORT + 0x4*i));
    }
#endif
}

int emi_mpu_set_protection(struct emi_region_info_t *region_info)
{
    unsigned long long start, end, region, bootstage;
    unsigned long long region_set, region_bit, val;
    struct __smccc_res res;

    if ((!region_info->region) || (region_info->region >= EMI_MPU_REGION_NUM)) {
        EMI_DBG("EMI MPU can not support region %u\n",
                region_info->region);
        return -1;
    }

    start = (unsigned long long)(region_info->start >> EMI_MPU_ALIGN_BITS);
    end = (unsigned long long)(region_info->end >> EMI_MPU_ALIGN_BITS);
    region = region_info->region;

    region_set = region/MPU_REGION_NUMBER_PER_SET;
    region_bit = region%MPU_REGION_NUMBER_PER_SET;

    EMI_DBG("LK %s start:%llx end=%llx region=%u ", __func__,
            region_info->start, region_info->end, region_info->region);

    bootstage = get_boot_stage();

    if (bootstage == BOOT_STAGE_BL2_EXT) {
        start &= 0x00FFFFFF;
        end &= 0x00FFFFFF;

        if ((start >= DRAM_OFFSET) && (end >= start)) {
            start -= DRAM_OFFSET;
            end -= DRAM_OFFSET;
        } else {
            return -1;
        }

        if (region_bit == 0) {
            writel(start & ADDR_MASK,
                    SECURE_REGION_SA0((region_set-1), (MPU_REGION_NUMBER_PER_SET-1)));
            writel(start >> ADDR_BIT,
                    SECURE_REGION_SA1((region_set-1), (MPU_REGION_NUMBER_PER_SET-1)));
            writel(end & ADDR_MASK,
                    SECURE_REGION_EA0((region_set-1), (MPU_REGION_NUMBER_PER_SET-1)));
            writel((end >> ADDR_BIT) | REGION_ENABLE,
                    SECURE_REGION_EA1((region_set-1), (MPU_REGION_NUMBER_PER_SET-1)));
        } else {
            writel(start & ADDR_MASK, SECURE_REGION_SA0(region_set, (region_bit-1)));
            writel(start >> ADDR_BIT, SECURE_REGION_SA1(region_set, (region_bit-1)));
            writel(end & ADDR_MASK, SECURE_REGION_EA0(region_set, (region_bit-1)));
            writel((end >> ADDR_BIT) | REGION_ENABLE,
                    SECURE_REGION_EA1(region_set, (region_bit-1)));
        }

#ifdef SUB_EMI_SMPU
        if (region_bit == 0) {
            writel(start & ADDR_MASK,
                    SUB_SECURE_REGION_SA0((region_set-1), (MPU_REGION_NUMBER_PER_SET-1)));
            writel(start >> ADDR_BIT,
                    SUB_SECURE_REGION_SA1((region_set-1), (MPU_REGION_NUMBER_PER_SET-1)));
            writel(end & ADDR_MASK,
                    SUB_SECURE_REGION_EA0((region_set-1), (MPU_REGION_NUMBER_PER_SET-1)));
            writel((end >> ADDR_BIT) | REGION_ENABLE,
                    SUB_SECURE_REGION_EA1((region_set-1), (MPU_REGION_NUMBER_PER_SET-1)));
        } else {
            writel(start & ADDR_MASK, SUB_SECURE_REGION_SA0(region_set, (region_bit-1)));
            writel(start >> ADDR_BIT, SUB_SECURE_REGION_SA1(region_set, (region_bit-1)));
            writel(end & ADDR_MASK, SUB_SECURE_REGION_EA0(region_set, (region_bit-1)));
            writel((end >> ADDR_BIT) | REGION_ENABLE,
                    SUB_SECURE_REGION_EA1(region_set, (region_bit-1)));
        }
#endif
        /* enable region detection */
        if (region_bit == 0) {
            val = readl(SECURE_REGION_ENABLE((region_set-1)));
            writel(val | (1<<(MPU_REGION_NUMBER_PER_SET-1)),
                    SECURE_REGION_ENABLE((region_set-1)));
        } else {
            val = readl(SECURE_REGION_ENABLE(region_set));
            writel(val | (1<<(region_bit-1)), SECURE_REGION_ENABLE(region_set));
        }

#ifdef SUB_EMI_SMPU
        if (region_bit == 0) {
            val = readl(SUB_SECURE_REGION_ENABLE((region_set-1)));
            writel(val | (1<<(region_bit-1)), SUB_SECURE_REGION_ENABLE((region_set-1)));
        } else {
            val = readl(SUB_SECURE_REGION_ENABLE(region_set));
            writel(val | (1<<(region_bit-1)), SUB_SECURE_REGION_ENABLE(region_set));
        }
#endif
        return 0;
    } else if (bootstage == BOOT_STAGE_BL33) {
        emi_mpu_smc_set(start, end, region_info->region, res);
        EMI_DBG("result=%d\n", (int)res.a0);
        return (int)res.a0;
    } else {
        EMI_DBG("boot stage error!\n");
        return -1;
    }

    return 0;
}

int emi_mpu_clear_protection(struct emi_region_info_t *region_info)
{
    struct __smccc_res res;

    if (region_info->region >= EMI_MPU_REGION_NUM) {
        EMI_DBG("EMI MPU can not support region:%u\n",
                region_info->region);
        return -1;
    }

    emi_mpu_smc_clear(region_info->region, res);

    return 0;
}
