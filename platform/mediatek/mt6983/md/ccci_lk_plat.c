/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <platform/boot_mode.h>
#include <string.h>

#ifdef WITH_PLATFORM_MEDIATEK_COMMON_EMI
#include <emi_mpu.h>
#endif

#include "ccci_ld_md_mpu.h"
#include "ccci_ld_md_log.h"
#include "ccci_ld_md_smem.h"
#include "ccci_ld_md_smem_uid.h"

#define LOCAL_TRACE 0

static struct smem_region_lk s_nc_ref_tbl[] = {
    {{SMEM_USER_RAW_DFD,          0, 0,          0x1000000,
                              SMEM_ATTR_MD_NC | SMEM_ATTR_NO_MAP, 0}, NULL},
    {{SMEM_USER_RAW_UDC_DATA,     0, 0,          0x1000,    SMEM_ATTR_MD_NC, 0},
                                                                          ccci_get_udc_data_size},
    {{SMEM_USER_MD_WIFI_PROXY,    0, 64 * 1024,          0,               0, 0}, NULL},
    {{SMEM_USER_RAW_AMMS_POS,     0, 0,          0x10000,   SMEM_ATTR_MD_NC, 0}, NULL},
    {{SMEM_USER_RAW_MDCCCI_DBG,   0, 2 * 1024,   0,         SMEM_ATTR_MD_NC, 0}, NULL},
    {{SMEM_USER_RAW_MDSS_DBG,     0, 24 * 1024,  0,         SMEM_ATTR_MD_NC, 0}, NULL},
    {{SMEM_USER_RAW_RESERVED,     0, 32 * 1024,  0,         SMEM_ATTR_MD_NC, 0}, NULL},
    {{SMEM_USER_RAW_RUNTIME_DATA, 0, 4 * 1024,   0,         SMEM_ATTR_MD_NC, 0}, NULL},
    {{SMEM_USER_RAW_FORCE_ASSERT, 0, 1 * 1024,   0,         SMEM_ATTR_MD_NC, 0}, NULL},
    {{SMEM_USER_LOW_POWER,        0, 512,        0,         SMEM_ATTR_MD_NC, 0}, NULL},
    {{SMEM_USER_RAW_DBM,          0, 512,        0,         SMEM_ATTR_MD_NC, 0}, NULL},
    {{SMEM_USER_CCISM_SCP,        0, 32 * 1024,  0,         SMEM_ATTR_MD_NC, 0}, NULL},
    {{SMEM_USER_RAW_CCB_CTRL,     0, 4 * 1024,   0,         SMEM_ATTR_MD_NC, 0}, NULL},
    {{SMEM_USER_RAW_NETD,         0, 8 * 1024,   0,         SMEM_ATTR_MD_NC, 0}, NULL},
    {{SMEM_USER_RAW_USB,          0, 4 * 1024,   0,         SMEM_ATTR_MD_NC, 0}, NULL},
    {{SMEM_USER_RAW_AUDIO,        0, 52 * 1024,  0,         SMEM_ATTR_MD_NC, 0}, NULL},
    {{SMEM_USER_CCISM_MCU,        0, 721 * 1024, 0,         SMEM_ATTR_MD_NC, 0}, NULL},
    {{SMEM_USER_CCISM_MCU_EXP,    0, 121 * 1024, 0,         SMEM_ATTR_MD_NC, 0}, NULL},
    {{SMEM_USER_RESERVED,         0,  18 * 1024, 0,
                              SMEM_ATTR_MD_NC | SMEM_ATTR_PADDING, 0}, NULL},
    {{SMEM_USER_MD_DRDI,          0, 0,          0,         SMEM_ATTR_MD_NC, 0},
                                                                          ccci_get_drdi_copy_size},
};

static struct smem_region_lk s_c_ref_tbl[] = {
    {{SMEM_USER_RAW_MD_CONSYS,   0, 0x2B00000,  0x10000,
           SMEM_ATTR_NO_MAP | SMEM_ATTR_DEDICATED_MPU | SMEM_ATTR_MD_C, 0}, ccci_get_consys_size},
    {{SMEM_USER_MD_NVRAM_CACHE,  0, 0x300000,  64,       SMEM_ATTR_MD_C, 0},
                                                                          ccci_get_nv_cache_size},
    {{SMEM_USER_CCB_START,       0, 0x1600000,  0x100000, SMEM_ATTR_MD_C, 0},
                                                             ccci_get_ccb_size},
    {{SMEM_USER_RAW_UDC_DESCTAB, 0, 0x1000,     0x10000,  SMEM_ATTR_MD_C, 0},
                                                        ccci_get_udc_desc_size},
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

/* Fix me, remove the following defination when MPU driver ready */
#define NO_PROTECTION 0
#define SEC_R_NSEC_R  1
#define FORBIDDEN     3

/* ========================================================================= */
/* MPU relate functions                                                      */
/* ========================================================================= */
#define MPU_FLAG_UPDATED            (1 << 0)
#define MPU_FLAG_PADDING            (1 << 1)
#define MPU_FLAG_SKIP               (1 << 7)

struct md_mpu_attr {
    unsigned long long addr;
    unsigned int size;
    unsigned int blk_flag;
    int          blk_id;
    int          mpu_id;
};

/* Please porting this table for different platform. */
static struct md_mpu_attr mpu_att_tbl[] = {
    /* addr | size | blk_flag | blk_id | mpu_id */
    {0ULL, 0, 0,  MD_MCU_RO_HW_RO, 32},
    {0ULL, 0, 0,  MD_DSP_RO,       33},
    {0ULL, 0, 0,  MD_DSP_RW,       34},
    {0ULL, 0, 0,  MD_DRDI,         35},
    {0ULL, 0, 0,  MD_MCU_RW_HW_RW, 36},
    {0ULL, 0, 0,  MD_MCU_RW_HW_RO, 37},
    {0ULL, 0, 0,  MD_MCU_RO_HW_RW, 38},
    {0ULL, 0, 0,  MD_TRACE_TOP,    39},
    {0ULL, 0, MPU_FLAG_PADDING, 0, 40},
    {0ULL, 0, 0,  MD_CONSYS,       41},
    {0ULL, 0, 0,  MD_NC_SMEM,      42},
    {0ULL, 0, 0,  MD_C_SMEM,       43},
};

/* ========================================================================== */
/* Export to external functions                                               */
/* ========================================================================== */
void ccci_plat_apply_mpu_setting(void)
{
    unsigned int i;
#ifdef WITH_PLATFORM_MEDIATEK_COMMON_EMI
    struct emi_region_info_t region_info[15] = {0};
#endif

    for (i = 0; i < countof(mpu_att_tbl); i++) {
        if (!(mpu_att_tbl[i].blk_flag & MPU_FLAG_UPDATED))
            continue;

        if (mpu_att_tbl[i].blk_flag & MPU_FLAG_SKIP) {
            ALWAYS_LOG("skip config region[%02u]\n", mpu_att_tbl[i].mpu_id);
            continue;
        }

#ifdef WITH_PLATFORM_MEDIATEK_COMMON_EMI
        region_info[i].start = (u64)mpu_att_tbl[i].addr;
        region_info[i].end = (u64)(mpu_att_tbl[i].addr + mpu_att_tbl[i].size);
        region_info[i].region = mpu_att_tbl[i].mpu_id;

        if (mpu_att_tbl[i].blk_flag & MPU_FLAG_PADDING)
            set_md_region(mpu_att_tbl[i].mpu_id, mpu_att_tbl[i].blk_id);
        emi_mpu_set_protection(&region_info[i]);
        ALWAYS_LOG("MPU Region start[0x%llX] size[0x%x] end[0x%llX] region[%02u]\n",
                    region_info[i].start, mpu_att_tbl[i].size, region_info[i].end,
                    region_info[i].region);
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
    int i, j;
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
    for (j = 0; j < (int)countof(mpu_att_tbl); j++) {
        if (mpu_att_tbl[j].mpu_id == mpu_id) {
            match = 1;
            break;
        }
    }
    if (!match) {
        ALWAYS_LOG("MPU: Find padding blk MPU id(%d) fail\n", mpu_id);
        return -1;
    }

    mpu_att_tbl[j].addr = start;
    mpu_att_tbl[j].size = size;
    mpu_att_tbl[j].blk_id = mpu_att_tbl[i].blk_id;
    mpu_att_tbl[j].blk_flag |= MPU_FLAG_UPDATED | MPU_FLAG_PADDING;

    return 0;
}

int ccci_is_load_md_img_only(unsigned char b_mode)
{
    if ((b_mode == RECOVERY_BOOT) || (b_mode == KERNEL_POWER_OFF_CHARGING_BOOT))
        return 1;
    return 0;
}
