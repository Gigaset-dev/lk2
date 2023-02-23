/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

/******************************************************************************
 ** Sub section:
 **   modem/dsp check header
 ******************************************************************************/
struct md_check_header_comm {
    unsigned char check_header[12];  /* magic number is "CHECK_HEADER"*/
    unsigned int  header_verno;   /* header structure version number */
    unsigned int  product_ver;     /* 0x0:invalid; 0x1:debug version; 0x2:release version */
    unsigned int  image_type;       /* 0x0:invalid; 0x1:2G modem; 0x2: 3G modem */
    unsigned char platform[16];   /* MT6573_S01 or MT6573_S02 */
    unsigned char build_time[64];   /* build time string */
    unsigned char build_ver[64];     /* project version, ex:11A_MD.W11.28 */

    unsigned char bind_sys_id;     /* bind to md sys id, MD SYS1: 1, MD SYS2: 2 */
    unsigned char ext_attr;       /* no shrink: 0, shrink: 1*/
    unsigned char reserved[2];     /* for reserved */

    unsigned int  mem_size;       /* md ROM/RAM image size requested by md */
    unsigned int  md_img_size;     /* md image size, exclude head size*/
} __PACKED;

struct md_check_header_v1 {
    struct md_check_header_comm common; /* common part */
    unsigned int  ap_md_smem_size;     /* share memory size */
    unsigned int  size;           /* the size of this structure */
};

struct md_check_header_v3 {
    struct md_check_header_comm common; /* common part */
    unsigned int  rpc_sec_mem_addr;  /* RPC secure memory address */

    unsigned int  dsp_img_offset;
    unsigned int  dsp_img_size;
    unsigned char reserved2[88];

    unsigned int  size;           /* the size of this structure */
};

struct md_regin_info_s {
    unsigned int region_offset;
    unsigned int region_size;
};

struct free_padding_block_s {
    unsigned int start_offset;
    unsigned int length;
};

#define MAX_PADDING_NUM_V6_HDR  8

struct md_check_header_v6 {
    struct md_check_header_comm common; /* common part */
    unsigned int  rpc_sec_mem_addr;  /* RPC secure memory address */

    unsigned int  dsp_img_offset;
    unsigned int  dsp_img_size;

    unsigned int  region_num;    /* total region number */
    struct md_regin_info_s region_info[8];    /* max support 8 regions */
    /* max support 4 domain settings, each region has 4 control bits*/
    unsigned int  domain_attr[4];

    unsigned int  arm7_img_offset;
    unsigned int  arm7_img_size;

    struct free_padding_block_s padding_blk[MAX_PADDING_NUM_V6_HDR]; /* should be 8 */

    unsigned int  ap_md_smem_size;
    unsigned int  md_to_md_smem_size;

    unsigned int ramdisk_offset;
    unsigned int ramdisk_size;

    unsigned int drdi_offset;    /* For load sub image */
    unsigned int drdi_size;        /* For load sub image */
    unsigned int drdi_rt_offset;    /* For runtime position */
    unsigned int drdi_rt_size;    /* For runtime position */

    unsigned int amms_pos_size;     /*For amms pos*/
    unsigned int consys_size;
    unsigned int udc_support;    /* 1:support 0:not support */
    unsigned int cp_debug_flag;
    unsigned int nv_cache_shm_size;
    unsigned int drdi_version; /* 3:is new arch*/

    unsigned char reserved_1[104];

    unsigned int  size; /* the size of this structure */
};
