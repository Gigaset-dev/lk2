/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

/*****************************************************************************/
/* Memory --- size, alignment, limit settings                */
/*****************************************************************************/

/****************************************/
/* Memory --- AP-MD share               */
/****************************************/

/* size setting */
 /* SIB */
#define CCCI_SMEM_SIB_MAX_SIZE  (0x60000000)
 /* ap-md share non-cacheable*/
#ifdef MTK_DFD_ENABLE_CACHE_DUMP
#define CCCI_SMEM_SIZE_DFD (8*1024*1024)
#else
#define CCCI_SMEM_SIZE_DFD (448*1024)
#endif

#define CCCI_SMEM_SIZE_UDC_NONCACHE (25536*1024)
#define CCCI_SMEM_SIZE_UDC_CACHE (512*1024)
#define CCCI_SMEM_WIFI_MD_PROXY (32*1024)

#define AP_MD1_SMEM_SIZE    0x100000

 /* ap-md share cacheable */
#define MAX_SMEM_SIZE       0x10000000 //history: 6M-->256M-->64M-->256M
#define CACHABLE_SMEM_MAX_SIZE     (128*1024*1024)
/*offset = MAX_SMEM_SIZE - CACHABLE_SMEM_MAX_SIZE */
#define CCCI_SMEM_SIZE_CCB_DHL (2*1024*1024)
#define CCCI_SMEM_SIZE_RAW_DHL (62*1024*1024)
#define CCB_DATA_BUF_SIZE (CCCI_SMEM_SIZE_CCB_DHL + CCCI_SMEM_SIZE_RAW_DHL)

#define CCB_DATA_BUF_DEFAULT_GEAR  12 /* NOTE: This value may be different at different platform */
#define CCB_GEAR_SUPPORT_LIST "1(2,20);2(2,10);3(0,0);4(2,30);11(2,2);12(2,62)"

#define SMEM_MD_CONSYS_SIZE        (43*1024*1024)
#define SMEM_MD_NVRAM_CACHE_SIZE    (18*1024*1024) /* LID */
#define SMEM_MD_USIP_SIZE        (384*1024) /* Audio */

/* alignment and limit setting */
 /* Share memory cacheable for MD */
#define SMEM_CACHE_ALIGNMENT    0x2000000L
#define SMEM_CACHE_LIMIT    0x90000000LL

 /* Share memory non-cacheable for MD */
#define SMEM_NONCACHE_ALIGNMENT    0x2000000L
#define SMEM_NONCACHE_LIMIT    0x90000000LL

 /* RO + RW memory non-cacheable for MD */
#define RO_RW_MEM_ALIGNMENT    0x2000000L
#ifdef DUMMY_AP_MODE
#define RO_RW_MEM_LIMIT        0xA0000000LL
#else
#define RO_RW_MEM_LIMIT        0xC0000000LL
#endif

 /* SIB memory */
#define SIB_SMEM_ALIGNMENT    0x8000000L
#define SIB_SMEM_LIMIT        0x200000000LL

#define NC_SMEM_ALIGN   0x10000
#define C_SMEM_ALIGN    0x10000

#define SMEM_MTEE_SLOT  5

#define SMEM_NC_OFFSET  0
#define SMEM_NC_MAX     (32 * 1024 * 1024 * 5)

#define SMEM_C_OFFSET   (32 * 1024 * 1024 * 4)
#define SMEM_C_MAX      (32 * 1024 * 1024 * 4)

/* IRQ OFFSET */
#define GIC_OFFSET_NUM  (32)

/* APIs for Share memory */
void *ccci_get_nc_smem_ref_tbl(int *num);
void *ccci_get_c_smem_ref_tbl(int *num);

/* APIs for MPU */
void ccci_plat_apply_mpu_setting(void);
int ccci_plat_get_padding_mpu_region(int buf[], int num);
int ccci_plat_update_md_mpu_setting(int id, unsigned long long start,
                                    unsigned int size, int mpu_id);

