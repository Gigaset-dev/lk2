/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#define MAX_MD_NUM      4

enum {
    main_img = 1,
    dsp_img,
    ramdisk_img,
    drdi_img,
};

enum AMMS_VER {
    AMMS_V1_2 = 0,
    AMMS_V3 = 3,
};

struct download_info {
    int     img_type;   /* Main image, or plug-in image */
    const char  *partition_name;
    const char  *image_name;
    int     ref_flag;
};

#define LD_IMG_NO_VERIFY    (1<<0)
#define LK_IMG_OPTINAL      (1<<1)
#define DUMMY_AP_FLAG       (LD_IMG_NO_VERIFY | LK_IMG_OPTINAL)
#define DUMMY_AP_FLAG_MAIN  LD_IMG_NO_VERIFY

/* --- wrapper api --- */
unsigned long long ccci_resv_named_memory(const char name[], unsigned int size,
                                          unsigned char **va_o);
int ccci_retrieve_mem(unsigned long long pa_addr, int size);
/* Need custom implement */
unsigned int customized_get_rf_id(void);
unsigned int customized_get_model_id(void);

/* --- ap-md share */
int cal_ap_md_smem_layout(void *fdt, u8 b_mode);

/* --- impelement by request --- */
void ccci_md_chk_hdr_notify_platform(void *hdr);
void ccci_md_rorw_base_notify_platform(unsigned long long pa);
void ccci_md_nc_smem_info_notify_platform(unsigned long long pa, unsigned int total);
void ccci_md_c_smem_info_notify_platform(unsigned long long pa, unsigned int total);
void ccci_platform_post_process(void);
const struct download_info *ccci_get_tbl_cust(void);
const struct download_info *ccci_get_tbl_from_platform(void);
const struct download_info *ccci_get_tbl_for_test(void);
unsigned int ccci_get_max_md_resv_size_from_platform(void);
int ccci_get_max_region_num_from_platform(void);
void ccci_post_process(void *fdt);
void ccci_test_md_power_on(void);
int ccci_is_load_md_img_only(unsigned char b_mode);
