/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

/*****************************************************************************
 ** Common section:
 **   Share memory layout for kernel
 *****************************************************************************/
struct smem_region {
    int          id;
    unsigned int ap_offset;
    unsigned int size;
    unsigned int align;
    unsigned int flags;
    unsigned int md_offset;
};

struct smem_region_lk {
    struct smem_region sm;
    void (*rt_cb)(void *fdt, u8 boot_mode, struct smem_region_lk *region);
};

void ccci_get_dfd_size_from_dts(void *kernel_fdt, u8 boot_mode, struct smem_region_lk *region);
void ccci_get_udc_data_size(void *fdt, u8 boot_mode, struct smem_region_lk *region);
void ccci_get_udc_desc_size(void *fdt, u8 boot_mode, struct smem_region_lk *region);
void ccci_get_consys_size(void *fdt, u8 boot_mode, struct smem_region_lk *region);
void ccci_get_nv_cache_size(void *fdt, u8 boot_mode, struct smem_region_lk *region);
void ccci_get_drdi_copy_size(void *fdt, u8 boot_mode, struct smem_region_lk *region);

void ccci_get_ccb_size(void *fdt, u8 boot_mode, struct smem_region_lk *region);


#define SMEM_ATTR_MD_NC           (1 << 0)
#define SMEM_ATTR_MD_C            (1 << 1)
#define SMEM_ATTR_PADDING         (1 << 2)
#define SMEM_ATTR_NO_MAP          (1 << 3)
#define SMEM_ATTR_NO_INIT         (1 << 4)
#define SMEM_ATTR_CCCI_USE        (1 << 5)
#define SMEM_ATTR_DEDICATED_MPU   (1 << 6)
