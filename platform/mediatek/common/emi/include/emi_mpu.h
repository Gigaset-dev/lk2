/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#define NO_PROTECTION      0
#define SEC_RW             1
#define SEC_RW_NSEC_R      2
#define SEC_RW_NSEC_W      3
#define SEC_R_NSEC_R       4
#define FORBIDDEN          5
#define SEC_R_NSEC_RW      6

#define LOCK               1
#define UNLOCK             0

#define EMIMPU_SET          0
#define EMIMPU_CLEAR        1
#define EMIMPU_READ         2
#define EMIMPU_SLVERR       3
#define EMIDBG_DUMP         4
#define EMIDBG_MSG          5
#define AID_TABLE_SET       6
#define KP_SET              7

#define EMIMPU_READ_SA           0
#define EMIMPU_READ_EA           1
#define EMIMPU_READ_APC          2
#define EMIMPU_READ_ENABLE       3
#define EMIMPU_READ_AID          4
#define EMIMPU_CHECK_NS_CPU      5
#define EMIMPU_CHECK_REGION_INFO 6

#if (MPU_INFO_VER == 1)
#define EMI_MPU_ALIGN_BITS      16
#define EMI_MPU_REGION_NUM      32
#elif (MPU_INFO_VER == 2)
#define EMI_MPU_ALIGN_BITS      12
#define EMI_MPU_REGION_NUM      48
#define EMI_KP_REGION_NUM       8
#endif

#define EMI_DRAM_BASE           (0x40000000)
#define DRAM_OFFSET             (EMI_DRAM_BASE >> EMI_MPU_ALIGN_BITS)
#define EMI_MPU_DOMAIN_NUM      16
#define EMI_MPU_DGROUP_NUM      (EMI_MPU_DOMAIN_NUM / 8)

#if (EMI_MPU_DGROUP_NUM == 1)
#define SET_ACCESS_PERMISSION(apc_ary, lock, d7, d6, d5, d4, d3, d2, d1, d0) \
{ \
    apc_ary[0] = \
        (((unsigned int)  d7) << 21) | (((unsigned int)  d6) << 18) | \
        (((unsigned int)  d5) << 15) | (((unsigned int)  d4) << 12) | \
        (((unsigned int)  d3) <<  9) | (((unsigned int)  d2) <<  6) | \
        (((unsigned int)  d1) <<  3) |  ((unsigned int)  d0) | ((unsigned int) lock << 31); \
}
#elif (EMI_MPU_DGROUP_NUM == 2)
#define SET_ACCESS_PERMISSION(apc_ary, lock, d15, d14, d13, d12, d11, d10,     \
                                       d9, d8, d7, d6, d5, d4, d3, d2, d1, d0) \
{ \
    apc_ary[1] = \
        (((unsigned int) d15) << 21) | (((unsigned int) d14) << 18) | \
        (((unsigned int) d13) << 15) | (((unsigned int) d12) << 12) | \
        (((unsigned int) d11) <<  9) | (((unsigned int) d10) <<  6) | \
        (((unsigned int)  d9) <<  3) |  ((unsigned int)  d8); \
    apc_ary[0] = \
        (((unsigned int)  d7) << 21) | (((unsigned int)  d6) << 18) | \
        (((unsigned int)  d5) << 15) | (((unsigned int)  d4) << 12) | \
        (((unsigned int)  d3) <<  9) | (((unsigned int)  d2) <<  6) | \
        (((unsigned int)  d1) <<  3) |  ((unsigned int)  d0) | ((unsigned int) lock << 31); \
}
#endif

struct emi_region_info_t {
    unsigned long long start;
    unsigned long long end;
    unsigned int region;
    unsigned int apc[EMI_MPU_DGROUP_NUM];
};

struct unmap_region {
    unsigned int region;
    unsigned int enable;
    unsigned long long start;
    unsigned long long end;
};

int emi_mpu_set_protection(struct emi_region_info_t *region_info);
int emi_mpu_clear_protection(struct emi_region_info_t *region_info);
int emi_kp_set_protection(unsigned int start, unsigned int end, unsigned int region);
paddr_t check_violation_for_unmap(void);
void clear_violation(void);
unsigned int check_ns_cpu_perm(void);
int set_md_region(unsigned int region, unsigned int perm);
struct unmap_region *get_region_info(void);
