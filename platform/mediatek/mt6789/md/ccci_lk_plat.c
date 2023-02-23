/*
 * Copyright (c) 2022 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <platform/boot_mode.h>
#include <stdlib.h>
#include <string.h>
#include <sysenv.h>

#ifdef WITH_PLATFORM_MEDIATEK_COMMON_EMI
#include <emi_mpu.h>
#endif

#include "ccci_ld_md_mpu.h"
#include "ccci_ld_md_log.h"
#include "ccci_ld_md_tag_dt.h"
#include "ccci_ld_md_smem.h"
#include "ccci_ld_md_smem_uid.h"

#define LOCAL_TRACE 0

#define SIB_SIZE_C_L0 (0*1024*1024)
#define SIB_SIZE_C_L1 (32*1024*1024)
#define SIB_SIZE_C_L2 (64*1024*1024)
#define SIB_SIZE_C_L3 (128*1024*1024)
#define SIB_SIZE_C_L4 (222*1024*1024)

#define SIB_SIZE_MAX (113*1024*1024) //Note: SIB at Bank4 doest use CCCI_SMEM_SIB_MAX_SIZE

/* check list during bringup, open it when emimpu ready */
#define CCCI_PLATFORM_ALL_READY

static void get_sib_size(void *fdt, u8 boot_mode, struct smem_region_lk *region)
{
    unsigned int sib_gear = 0;
    unsigned int sib_size = 0;
    char *env_str = NULL;

    if (!region) {
        CCCI_TRACE_LOG("[%s] region NULL\n", __func__);
        return;
    }

    env_str = get_env("md1_phy_cap_gear");
    if (!env_str)
        return;

    sib_gear = atoi(env_str);
    ALWAYS_LOG("env[md1_phy_cap_gear]%d.\n", sib_gear);

    switch (sib_gear) {
    case 0:
        sib_size = SIB_SIZE_C_L0;
        break;
    case 1:
        sib_size = SIB_SIZE_C_L1;
        break;
    case 2:
        sib_size = SIB_SIZE_C_L2;
        break;
    case 3:
        sib_size = SIB_SIZE_C_L3;
        break;
    case 4:
        sib_size = SIB_SIZE_C_L4;
        break;
    default:
        sib_size = SIB_SIZE_C_L0;
        break;
    }

    /* sib max size must < unused size */
    if (sib_size > SIB_SIZE_MAX)
        sib_size = SIB_SIZE_MAX;

    region->sm.size = sib_size;
    if (ccci_insert_tag_inf("md1_phy_cap", (char *)&sib_size, sizeof(unsigned int)) < 0)
        ALWAYS_LOG("insert md1_phy_cap fail\n");
}

static void get_dpma_md_data_size(void *fdt, u8 boot_mode, struct smem_region_lk *region)
{
    char *env_str = NULL;
    unsigned int i;

    if (!region) {
        CCCI_TRACE_LOG("[%s] region NULL\n", __func__);
        return;
    }

    // For SMEM_USER_MD_DATA
    env_str = get_env("md_data_en");
    if (env_str == NULL) {
        ALWAYS_LOG("[%s] using default\n", __func__);
        return;
    }

    if (!strncmp(env_str, "disable", strlen("disable"))) {
        ALWAYS_LOG("[%s] get disable setting for md_data_en\n", __func__);
        region->sm.size = 0;
    }
}

static struct smem_region_lk s_nc_ref_tbl[] = {
    {{SMEM_USER_RAW_DFD,          0, 0,          0x800000,
                              SMEM_ATTR_MD_NC | SMEM_ATTR_NO_MAP, 0}, ccci_get_dfd_size_from_dts},
    //64K size for MD CCCI
    {{SMEM_USER_RAW_MDCCCI_DBG,   0,  2 * 1024,   0,         SMEM_ATTR_MD_NC, 0}, NULL},
    {{SMEM_USER_RAW_MDSS_DBG,     0, 10 * 1024,  0,         SMEM_ATTR_MD_NC, 0}, NULL},
    {{SMEM_USER_RAW_RESERVED,     0, 46 * 1024,  0,         SMEM_ATTR_MD_NC, 0}, NULL},
    {{SMEM_USER_RAW_RUNTIME_DATA, 0,  4 * 1024,   0,         SMEM_ATTR_MD_NC, 0}, NULL},
    {{SMEM_USER_RAW_FORCE_ASSERT, 0,  1 * 1024,   0,         SMEM_ATTR_MD_NC, 0}, NULL},
    {{SMEM_USER_RAW_RESERVED,    0, 848,        0,         SMEM_ATTR_MD_NC, 0}, NULL},
    {{SMEM_USER_RAW_DBM,          0, 176,        0,         SMEM_ATTR_MD_NC, 0}, NULL},
    //960K size for AP-MD CCCI
    {{SMEM_USER_CCISM_SCP,        0,  32 * 1024,  0,         SMEM_ATTR_MD_NC, 0}, NULL},
    {{SMEM_USER_RAW_CCB_CTRL,     0,   4 * 1024,   0,         SMEM_ATTR_MD_NC, 0}, NULL},
    {{SMEM_USER_RAW_NETD,         0,   4 * 1024,   0,         SMEM_ATTR_MD_NC, 0}, NULL},
    {{SMEM_USER_RAW_USB,          0,   4 * 1024,   0,         SMEM_ATTR_MD_NC, 0}, NULL},
    {{SMEM_USER_RAW_AUDIO,        0,  52 * 1024,  0,         SMEM_ATTR_MD_NC, 0}, NULL},
    {{SMEM_USER_CCISM_MCU,        0, 721 * 1024, 0,         SMEM_ATTR_MD_NC, 0}, NULL},
    {{SMEM_USER_CCISM_MCU_EXP,    0, 121 * 1024, 0,         SMEM_ATTR_MD_NC, 0}, NULL},
    {{SMEM_USER_RESERVED,         0,  22 * 1024,        0,         SMEM_ATTR_MD_NC, 0}, NULL},
    //64B size for AMMS DRDI
    {{SMEM_USER_MD_DRDI,          0, 0,          0,         SMEM_ATTR_MD_NC, 0},
                                                                         ccci_get_drdi_copy_size},
    {{SMEM_USER_MD_DATA,         0,  5 * 1024 * 1024,  8,  SMEM_ATTR_MD_NC, 0},
                                                                           get_dpma_md_data_size},
    {{SMEM_USER_RAW_PHY_CAP,     0,  0, 16,  SMEM_ATTR_NO_MAP | SMEM_ATTR_MD_NC, 0}, get_sib_size},
};

static struct smem_region_lk s_c_ref_tbl[] = {
    {{SMEM_USER_CCB_START,       0, 0x1600000,  0x100000, SMEM_ATTR_MD_C, 0},
                                                             ccci_get_ccb_size},
    {{SMEM_USER_RAW_MD_CONSYS,   0, 0x2B00000,  0x10000,
           SMEM_ATTR_NO_MAP | SMEM_ATTR_DEDICATED_MPU | SMEM_ATTR_MD_C, 0}, ccci_get_consys_size},
    {{SMEM_USER_RAW_USIP,        0, 384 * 1024, 128,      SMEM_ATTR_MD_C, 0}, NULL},
};

void *ccci_get_nc_smem_ref_tbl(int *num)
{
    if (num)
        *num = (int)countof(s_nc_ref_tbl);

    return s_nc_ref_tbl;
}

void *ccci_get_c_smem_ref_tbl(int *num)
{
    if (num)
        *num = (int)countof(s_c_ref_tbl);

    return s_c_ref_tbl;
}


/* ========================================================================= */
/* MPU relate functions                                                      */
/* ========================================================================= */

#define MPU_FLAG_UPDATED            (1 << 0)
#define MPU_FLAG_PADDING            (1 << 1)
#define MPU_FLAG_SKIP               (1 << 7)

/* ========================================================================= */
/* MPU relate functions                                                      */
/* ========================================================================= */

#define MPU_MDOMAIN_ID_TOTAL_NUM    16
#define MPU_STR_BUF_SIZE            64


struct md_mpu_attr {
    unsigned long long addr;
    unsigned int size;
    unsigned int blk_flag;
    int          blk_id;
    int          mpu_id;
    char         attr[MPU_MDOMAIN_ID_TOTAL_NUM];
};

/* Please porting this table for different platform. */
static struct md_mpu_attr mpu_att_tbl[] = {
/* addr | size | blk_flag | blk_id | mpu_id | attr  */

/* No| D0(AP)   | D1(MD1) | D2(CONN) | D3(SCP)  | D4(MM)
 *   | D5(Rsv ) | D6(MFG) | D7(MDHW)
 *   | D8(SSPM) | D9(SPM) |D10(ADSP) | D11-15(Rsv)
 */
    {0ULL, 0, 0,  MD_MCU_RO_HW_RO, 10, {FORBIDDEN,  SEC_R_NSEC_R,
          [2 ... 6] = FORBIDDEN, SEC_R_NSEC_R, [8 ... 15] = FORBIDDEN} },
    {0ULL, 0, 0,  MD_DSP_RO, 11, {FORBIDDEN, SEC_R_NSEC_R,
          [2 ... 6] = FORBIDDEN, SEC_R_NSEC_R, [8 ... 15] = FORBIDDEN} },
    {0ULL, 0, 0,  MD_DSP_RW, 12, {FORBIDDEN, NO_PROTECTION,
          [2 ... 6] = FORBIDDEN, NO_PROTECTION, [8 ... 15] = FORBIDDEN} },
    {0ULL, 0, 0,  MD_DRDI, 13, {FORBIDDEN, NO_PROTECTION,
          [2 ... 6] = FORBIDDEN, SEC_R_NSEC_R, [8 ... 15] = FORBIDDEN} },
    {0ULL, 0, 0,  MD_MCU_RW_HW_RW, 14, {FORBIDDEN, NO_PROTECTION,
          [2 ... 6] = FORBIDDEN, NO_PROTECTION, [8 ... 15] = FORBIDDEN} },
    {0ULL, 0, 0,  MD_MCU_RW_HW_RO, 15, {FORBIDDEN, NO_PROTECTION,
          [2 ... 6] = FORBIDDEN, SEC_R_NSEC_R, [8 ... 15] = FORBIDDEN} },
    {0ULL, 0, 0,  MD_MCU_RO_HW_RW, 16, {FORBIDDEN, SEC_R_NSEC_R,
          [2 ... 6] = FORBIDDEN, NO_PROTECTION, [8 ... 15] = FORBIDDEN} },
    {0ULL, 0, 0,  MD_TRACE_TOP, 17, {SEC_R_NSEC_R, FORBIDDEN,
          [2 ... 6] = FORBIDDEN, NO_PROTECTION, [8 ... 15] = FORBIDDEN} },
    {0ULL, 0, MPU_FLAG_PADDING, 0, 22, {[0 ... 15] = NO_PROTECTION} },
    {0ULL, 0, 0,  MD_CONSYS, 23, {SEC_R_NSEC_R,  NO_PROTECTION, NO_PROTECTION,
          [3 ... 6] = FORBIDDEN, NO_PROTECTION, [8 ... 15] = FORBIDDEN} },
    {0ULL, 0, 0,  MD_C_SMEM, 24, {NO_PROTECTION, NO_PROTECTION,
          [2 ... 6] = FORBIDDEN, NO_PROTECTION, FORBIDDEN, FORBIDDEN,
                                 NO_PROTECTION, [11 ... 15] = FORBIDDEN} },
    {0ULL, 0, 0,  MD_NC_SMEM, 25, {NO_PROTECTION, NO_PROTECTION, FORBIDDEN,
                                   NO_PROTECTION,  [4 ... 6] = FORBIDDEN,
                                   NO_PROTECTION, [8 ... 15] = FORBIDDEN} },
};

/* ========================================================================== */
/* Export to external functions                                               */
/* ========================================================================== */
void ccci_plat_apply_mpu_setting(void)
{
    unsigned int i;
#ifdef WITH_PLATFORM_MEDIATEK_COMMON_EMI
    struct emi_region_info_t region_info = {0};
#endif

    for (i = 0; i < countof(mpu_att_tbl); i++) {
        if (!(mpu_att_tbl[i].blk_flag & MPU_FLAG_UPDATED))
            continue;

        if (mpu_att_tbl[i].blk_flag & MPU_FLAG_SKIP) {
            ALWAYS_LOG("skip config region[%02u]\n", mpu_att_tbl[i].mpu_id);
            continue;
        }

#ifdef WITH_PLATFORM_MEDIATEK_COMMON_EMI
        region_info.start = (u64)mpu_att_tbl[i].addr;
        region_info.end = (u64)(mpu_att_tbl[i].addr + mpu_att_tbl[i].size);
        region_info.region = mpu_att_tbl[i].mpu_id;
        SET_ACCESS_PERMISSION(region_info.apc, UNLOCK,
           mpu_att_tbl[i].attr[15], mpu_att_tbl[i].attr[14], mpu_att_tbl[i].attr[13],
           mpu_att_tbl[i].attr[12], mpu_att_tbl[i].attr[11], mpu_att_tbl[i].attr[10],
           mpu_att_tbl[i].attr[9], mpu_att_tbl[i].attr[8], mpu_att_tbl[i].attr[7],
           mpu_att_tbl[i].attr[6], mpu_att_tbl[i].attr[5], mpu_att_tbl[i].attr[4],
           mpu_att_tbl[i].attr[3], mpu_att_tbl[i].attr[2], mpu_att_tbl[i].attr[1],
           mpu_att_tbl[i].attr[0]
                           );
        emi_mpu_set_protection(&region_info);
        ALWAYS_LOG("MPU Region start[0x%llX] size[0x%x] end[0x%llX] region[%02u]\n",
                    region_info.start, mpu_att_tbl[i].size, region_info.end,
                    region_info.region);
#endif
    }
}

int ccci_plat_get_padding_mpu_region(int buf[], int num)
{
    int i, j = 0;

    for (i = 0; i < (int)countof(mpu_att_tbl); i++) {
        if (j >= num)
            return j;
        if (mpu_att_tbl[i].blk_flag & MPU_FLAG_PADDING) {
            buf[j] = mpu_att_tbl[i].mpu_id;
            j++;
        }
    }

    return j;
}

int ccci_plat_update_md_mpu_setting(int id, unsigned long long start,
                                    unsigned int size, int mpu_id)
{
#ifndef CCCI_PLATFORM_ALL_READY
    return 0;
#endif

    int i, padding_id, k;
    int match = 0;

    for (i = 0; i < (int)countof(mpu_att_tbl); i++) {
        if (mpu_att_tbl[i].blk_id == id) {
            match = 1;
            break;
        }
    }

    if (!match) {
        ALWAYS_LOG("MPU: Find blk id(%d) fail\n", id);
        return -1;
    }
    if (mpu_att_tbl[i].blk_flag & MPU_FLAG_PADDING) {
        ALWAYS_LOG("MPU: blk id(%d) is padding region, un-expected!\n", id);
        return -1;
    }

    if (mpu_id < 0) {
        /* Update base and size is enough */
        mpu_att_tbl[i].addr = start;
        mpu_att_tbl[i].size = size;
        mpu_att_tbl[i].blk_flag |= MPU_FLAG_UPDATED;
        return 0;
    }

    /* Find relate region and copy */
    match = 0;
    for (padding_id = 0; padding_id < (int)countof(mpu_att_tbl); padding_id++) {
        if (mpu_att_tbl[padding_id].mpu_id == mpu_id) {
            match = 1;
            break;
        }
    }
    if (!match) {
        ALWAYS_LOG("MPU: Find padding blk MPU id(%d) fail\n", mpu_id);
        return -1;
    }

    mpu_att_tbl[padding_id].addr = start;
    mpu_att_tbl[padding_id].size = size;
    mpu_att_tbl[padding_id].blk_id = mpu_att_tbl[i].blk_id;
    mpu_att_tbl[padding_id].blk_flag |= MPU_FLAG_UPDATED | MPU_FLAG_PADDING;

    for (k = 0; k < MPU_MDOMAIN_ID_TOTAL_NUM; k++)
        mpu_att_tbl[padding_id].attr[k] = mpu_att_tbl[i].attr[k];

    return 0;
}

int ccci_is_load_md_img_only(unsigned char b_mode)
{
    if ((b_mode == RECOVERY_BOOT) || (b_mode == KERNEL_POWER_OFF_CHARGING_BOOT))
        return 1;
    return 0;
}
