/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <compiler.h>
#include <malloc.h>
#include <string.h>

#include "ccci_ld_md_chk_hdr.h"
#include "ccci_ld_md_log.h"
#include "ccci_ld_md_mem.h"
#include "ccci_ld_md_tag_dt.h"

#include "ccci_ld_md_compatible.h" /* Will remove when kernel legacy code phase out */

#define LOCAL_TRACE         0

#define HDR_INF_BUF_SIZE    80

static int check_header_dump(void *header_addr, int ver)
{
    struct md_check_header_v1 *v1_hdr;
    struct md_check_header_v3 *v3_hdr;
    struct md_check_header_v6 *v6_hdr;
    int i;

    if (!header_addr)
        return -1;

    switch (ver) {
    case 1:
        /* parsing md check header */
        v1_hdr = (struct md_check_header_v1 *)header_addr;
        ALWAYS_LOG("===== Dump v1 md header =====\n");
        ALWAYS_LOG("MD IMG  - Magic          : %s\n", v1_hdr->common.check_header);
        ALWAYS_LOG("MD IMG  - header_verno   : 0x%08X\n", v1_hdr->common.header_verno);
        ALWAYS_LOG("MD IMG  - product_ver    : 0x%08X\n", v1_hdr->common.product_ver);
        ALWAYS_LOG("MD IMG  - image_type     : 0x%08X\n", v1_hdr->common.image_type);
        ALWAYS_LOG("MD IMG  - mem_size       : 0x%08X\n", v1_hdr->common.mem_size);
        ALWAYS_LOG("MD IMG  - md_img_size    : 0x%08X\n", v1_hdr->common.md_img_size);
        ALWAYS_LOG("MD IMG  - ap_md_smem_size: 0x%08X\n", v1_hdr->ap_md_smem_size);
        ALWAYS_LOG("=============================\n");
        break;

    case 3: /* V3 */
        /* parsing md check header */
        v3_hdr = (struct md_check_header_v3 *)header_addr;
        ALWAYS_LOG("===== Dump v3 md header =====\n");
        ALWAYS_LOG("MD IMG  - Magic          : %s\n", v3_hdr->common.check_header);
        ALWAYS_LOG("MD IMG  - header_verno   : 0x%08X\n", v3_hdr->common.header_verno);
        ALWAYS_LOG("MD IMG  - product_ver    : 0x%08X\n", v3_hdr->common.product_ver);
        ALWAYS_LOG("MD IMG  - image_type     : 0x%08X\n", v3_hdr->common.image_type);
        ALWAYS_LOG("MD IMG  - mem_size       : 0x%08X\n", v3_hdr->common.mem_size);
        ALWAYS_LOG("MD IMG  - md_img_size    : 0x%08X\n", v3_hdr->common.md_img_size);
        ALWAYS_LOG("MD IMG  - dsp offset     : 0x%08X\n", v3_hdr->dsp_img_offset);
        ALWAYS_LOG("MD IMG  - dsp size       : 0x%08X\n", v3_hdr->dsp_img_size);
        ALWAYS_LOG("MD IMG  - rpc_sec_mem_addr : 0x%08X\n", v3_hdr->rpc_sec_mem_addr);
        ALWAYS_LOG("=============================\n");
        break;

    case 6:
        /* parsing md check header */
        v6_hdr = (struct md_check_header_v6 *)header_addr;
        ALWAYS_LOG("===== Dump v6 md header =====\n");
        ALWAYS_LOG("MD IMG  - Magic          : %s\n", v6_hdr->common.check_header);
        ALWAYS_LOG("MD IMG  - header_verno   : 0x%08X\n", v6_hdr->common.header_verno);
        ALWAYS_LOG("MD IMG  - product_ver    : 0x%08X\n", v6_hdr->common.product_ver);
        ALWAYS_LOG("MD IMG  - image_type     : 0x%08X\n", v6_hdr->common.image_type);
        ALWAYS_LOG("MD IMG  - mem_size       : 0x%08X\n", v6_hdr->common.mem_size);
        ALWAYS_LOG("MD IMG  - md_img_size    : 0x%08X\n", v6_hdr->common.md_img_size);
        ALWAYS_LOG("MD IMG  - dsp offset     : 0x%08X\n", v6_hdr->dsp_img_offset);
        ALWAYS_LOG("MD IMG  - dsp size       : 0x%08X\n", v6_hdr->dsp_img_size);
        ALWAYS_LOG("MD IMG  - rpc_sec_mem_addr : 0x%08X\n", v6_hdr->rpc_sec_mem_addr);
        ALWAYS_LOG("MD IMG  - armv7 offset   : 0x%08X\n", v6_hdr->arm7_img_offset);
        ALWAYS_LOG("MD IMG  - armv7 size     : 0x%08X\n", v6_hdr->arm7_img_size);
        ALWAYS_LOG("MD IMG  - region num     : 0x%08X\n", v6_hdr->region_num);
        ALWAYS_LOG("MD IMG  - ap_md_smem_size: 0x%08X\n", v6_hdr->ap_md_smem_size);
        ALWAYS_LOG("MD IMG  - md_md_smem_size: 0x%08X\n", v6_hdr->md_to_md_smem_size);
        ALWAYS_LOG("MD IMG  - amms_pos_size  : 0x%08X\n", v6_hdr->amms_pos_size);

        for (i = 0; i < 8; i++) {
            ALWAYS_LOG("MD IMG  - region[%d] off : 0x%08X\n", i,
                                       v6_hdr->region_info[i].region_offset);
            ALWAYS_LOG("MD IMG  - region[%d] size: 0x%08X\n", i,
                                         v6_hdr->region_info[i].region_size);
        }
        for (i = 0; i < 4; i++)
            ALWAYS_LOG("MD IMG  - domain_attr[%d] : 0x%08X\n", i, v6_hdr->domain_attr[i]);
        for (i = 0; i < MAX_PADDING_NUM_V6_HDR; i++) {
            ALWAYS_LOG("MD IMG  - padding info[%d] offset: 0x%08X\n", i,
                       v6_hdr->padding_blk[i].start_offset);
            ALWAYS_LOG("MD IMG  - padding info[%d] size: 0x%08X\n", i,
                       v6_hdr->padding_blk[i].length);
        }
        ALWAYS_LOG("MD IMG  - drdi offset    : 0x%08X\n", v6_hdr->drdi_offset);
        ALWAYS_LOG("MD IMG  - drdi size      : 0x%08X\n", v6_hdr->drdi_size);
        ALWAYS_LOG("Runtime - drdi rt offset : 0x%08X\n",
                                                v6_hdr->drdi_rt_offset);
        ALWAYS_LOG("Runtime - drdi rt size   : 0x%08X\n", v6_hdr->drdi_rt_size);
        ALWAYS_LOG("Runtime - amms pos size  : 0x%08X\n",
                                                         v6_hdr->amms_pos_size);
        ALWAYS_LOG("Runtime - consys  size   : 0x%08X\n", v6_hdr->consys_size);
        ALWAYS_LOG("Runtime - udc support    : 0x%08X\n", v6_hdr->udc_support);
        ALWAYS_LOG("Runtime - cp debug flag  : 0x%08X\n", v6_hdr->cp_debug_flag);
        ALWAYS_LOG("Runtime - nv cache size  : 0x%08X\n", v6_hdr->nv_cache_shm_size);
        ALWAYS_LOG("Runtime - drdi_version   : 0x%08X\n", v6_hdr->drdi_version);
        ALWAYS_LOG("=============================\n");
        break;

    default:
        break;
    }

    return 0;
}

static void compatible_for_legacy(void *header_addr)
{
    struct md_check_header_comm *hdr = NULL;
    int size;

    hdr = (struct md_check_header_comm *)header_addr;

    /* MD check header */
    switch (hdr->header_verno) {
    case 1:
        size = sizeof(struct md_check_header_v1);
        break;
    case 3:
        size = sizeof(struct md_check_header_v3);
        break;
    case 6:
        size = sizeof(struct md_check_header_v6);
        break;
    default:
        CCCI_TRACE_LOG("Ivalid MD_check header version:%d\n", hdr->header_verno);
        return;
    }

    if (ccci_insert_tag_inf("md1_chk", (char *)hdr, size)) {
        ALWAYS_LOG("add tag md_chk_hdr fail\n");
        return;
    }

    if (ccci_insert_tag_inf("md1img", (char *)&hdr->md_img_size, sizeof(int))) {
        ALWAYS_LOG("add tag md1img fail\n");
        return;
    }
}

static int add_md_inf_to_tag(void *header_addr)
{
    struct md_check_header_comm *hdr = NULL;
    char *hdr_inf_buf;
    int ret = 0;

    hdr = (struct md_check_header_comm *)header_addr;
    hdr_inf_buf = malloc(HDR_INF_BUF_SIZE);
    if (!hdr_inf_buf) {
        CCCI_TRACE_LOG("Alloc hdr_inf_buf memory fail\n");
        return -1;
    }

    /* MD chip platform */
    memset(hdr_inf_buf, 0, HDR_INF_BUF_SIZE);
    memcpy(hdr_inf_buf, hdr->platform, sizeof(hdr->platform));
    if (ccci_insert_tag_inf("md_platform", hdr_inf_buf,
                            sizeof(hdr->platform)) < 0) {
        CCCI_TRACE_LOG("add tag MD_PLATFORM fail\n");
        ret = -1;
        goto _exit;
    }

    /* MD build time */
    memset(hdr_inf_buf, 0, HDR_INF_BUF_SIZE);
    memcpy(hdr_inf_buf, hdr->build_time, sizeof(hdr->build_time));
    if (ccci_insert_tag_inf("md_build_time", hdr_inf_buf,
                            sizeof(hdr->build_time)) < 0) {
        CCCI_TRACE_LOG("add tag MD_BUILD_TIME fail\n");
        ret = -1;
        goto _exit;
    }

    /* MD build version */
    memset(hdr_inf_buf, 0, HDR_INF_BUF_SIZE);
    memcpy(hdr_inf_buf, hdr->build_ver, sizeof(hdr->build_ver));
    if (ccci_insert_tag_inf("md_build_ver", hdr_inf_buf,
                            sizeof(hdr->build_ver)) < 0) {
        CCCI_TRACE_LOG("add tag MD_BUILD_VER fail\n");
        ret = -1;
        goto _exit;
    }

    /* MD type: capabiltiy */
    if (ccci_insert_tag_inf("md_capability", (char *)&hdr->image_type,
                            sizeof(int)) < 0) {
        CCCI_TRACE_LOG("add tag MD_BUILD_VER fail\n");
        ret = -1;
    }

    compatible_for_legacy(header_addr);

_exit:

    free(hdr_inf_buf);

    return ret;
}

static void *get_check_header(char *base, unsigned int img_size, unsigned int *header_ver)
{
    unsigned int ver = 0;
    unsigned int size;
    void *header_addr = NULL;
    char *tail_addr = NULL;
    struct md_check_header_comm *common_hdr;
    char name[16];

    if (img_size & 0x3) {
        CCCI_TRACE_LOG("Check header size addr should 4 bytes align[0x%x]\n", img_size);
        return NULL;
    }

    tail_addr = (char *)&base[img_size];
    ALWAYS_LOG("tail_addr[%p]\n", tail_addr);
    if (!tail_addr) {
        CCCI_TRACE_LOG("tail_addr is NULL\n");
        return NULL;
    }

    size = *((unsigned int *)(tail_addr - 4));
    ALWAYS_LOG("Check headr v1(%u), v3(%u), v6(%u) curr(%u)\n",
               (unsigned int)sizeof(struct md_check_header_v1),
               (unsigned int)sizeof(struct md_check_header_v3),
               (unsigned int)sizeof(struct md_check_header_v6), size);

    if (size == sizeof(struct md_check_header_v1)) {
        header_addr = tail_addr - sizeof(struct md_check_header_v1);
        ver = 1;
    } else if (size == sizeof(struct md_check_header_v3)) {
        header_addr = tail_addr - sizeof(struct md_check_header_v3);
        ver = 3;
    } else if (size == sizeof(struct md_check_header_v6)) {
        header_addr = tail_addr - sizeof(struct md_check_header_v6);
        ver = 6;
    } else {
        CCCI_TRACE_LOG("Header version detect fail[size:%d]\n", size);
        return NULL;
    }

    if (img_size < size) {
        CCCI_TRACE_LOG("MD image size(%u) < check header size(%u)\n", img_size, size);
        return NULL;
    }

    common_hdr = (struct md_check_header_comm *)header_addr;
    if (!common_hdr) {
        CCCI_TRACE_LOG("check_header pointer is NULL\n");
        return NULL;
    }

    if (strncmp((const char *)common_hdr->check_header, "CHECK_HEADER", 12)) {
        memcpy((void *)name, (void *)common_hdr->check_header, 12);
        name[12] = 0;
        CCCI_TRACE_LOG("Invalid md check header str[%s]\n", name);
        return NULL;
    }

    if (common_hdr->header_verno != ver) {
        CCCI_TRACE_LOG("Version abnormal(%u<@hdr>:%u<by size>)\n",
                common_hdr->header_verno, ver);
        return NULL;
    }

    /* Dump header info */
    check_header_dump(common_hdr, ver);

    if (header_ver)
        *header_ver = ver;

    return header_addr;
}

static int ass_func_for_v6_normal_img(void *hdr)
{
    struct md_check_header_v6 *chk_hdr = NULL;
    unsigned int offset, i, size, flag;

    chk_hdr = (struct md_check_header_v6 *)hdr;

    /* Update modem whole memory */
    if (ccci_insert_mem_node(0, chk_hdr->common.mem_size, 0, ATTR_WHOLE) < 0) {
        CCCI_TRACE_LOG("update whole info fail\n");
        return -1;
    }

    /* Update modem memory region */
    for (i = 0; i < chk_hdr->region_num; i++) {
        offset = chk_hdr->region_info[i].region_offset;
        size =  chk_hdr->region_info[i].region_size;
        if (!size)
            continue;
        flag = ccci_layout_md_region_to_flag(i);
        if (!flag) {
            CCCI_TRACE_LOG("region number abnormal:%d\n", i);
            return -1;
        }
        if (ccci_insert_mem_node(offset, size, flag, 0) < 0) {
            CCCI_TRACE_LOG("update md region:%d fail\n", i);
            return -1;
        }
    }

    /* Update dsp reserved memory region */
    offset = chk_hdr->dsp_img_offset;
    size =  chk_hdr->dsp_img_size;
    if (!offset && !size)
        CCCI_TRACE_LOG("DSP size & offset are 0\n");
    else if (!size) {
        CCCI_TRACE_LOG("DSP size info abnormal:0\n");
        return -1;
    } else if (!offset) {
        CCCI_TRACE_LOG("DSP offset info abnormal:0\n");
        return -1;
    } else if (ccci_insert_mem_node(offset, size, 0, ATTR_DSP_RESERVED) < 0) {
        CCCI_TRACE_LOG("update DSP reserved fail\n");
        return -1;
    }

    /* Update modem padding memory region */
    for (i = 0; i < MAX_PADDING_NUM_V6_HDR; i++) {
        offset = chk_hdr->padding_blk[i].start_offset;
        size =  chk_hdr->padding_blk[i].length;
        if (!size)
            continue;
        if (ccci_insert_mem_node(offset, size, 0, ATTR_PADDING_MEMORY) < 0) {
            CCCI_TRACE_LOG("update md padding region:%d fail\n", i);
            return -1;
        }
    }

    /* Update drdi src region */
    offset = chk_hdr->drdi_offset;
    size =  chk_hdr->drdi_size;
    if (size) {
        if (ccci_insert_mem_node(offset, size, 0, ATTR_DRDI_SRC) < 0) {
            CCCI_TRACE_LOG("update DRDI SRC fail\n");
            return -1;
        }
    }

    /* Update drdi rt region */
    offset = chk_hdr->drdi_rt_offset;
    size =  chk_hdr->drdi_rt_size;
    if (size) {
        if (ccci_insert_mem_node(offset, size, 0, ATTR_DRDI_RT_DES) < 0) {
            CCCI_TRACE_LOG("update DRDI RT fail\n");
            return -1;
        }
    }

    /* Update ramdisk region */
    offset = chk_hdr->ramdisk_offset;
    size =  chk_hdr->ramdisk_size;
    if (size) {
        if (ccci_insert_mem_node(offset, size, 0, ATTR_RAMDISK) < 0) {
            CCCI_TRACE_LOG("update Ramdisk fail\n");
            return -1;
        }
    }

    /* Update udc en */
    flag = chk_hdr->udc_support;
    if (ccci_insert_tag_inf("udc_en", (char *)&flag, sizeof(flag)) < 0) {
        CCCI_TRACE_LOG("add udc_en tag fail\n");
        return -1;
    }

    /* Update consys_size size */
    flag = chk_hdr->consys_size;
    if (ccci_insert_tag_inf("consys_size", (char *)&flag, sizeof(flag)) < 0) {
        CCCI_TRACE_LOG("add consys_size tag fail\n");
        return -1;
    }

    /* Update nv_cache_shm_size size */
    flag = chk_hdr->nv_cache_shm_size;
    if (ccci_insert_tag_inf("nv_cache_shm_size", (char *)&flag, sizeof(flag)) < 0) {
        CCCI_TRACE_LOG("add nv_cache_shm_size tag fail\n");
        return -1;
    }

    /* Update drdi_version */
    flag = chk_hdr->drdi_version;
    if (ccci_insert_tag_inf("drdi_version", (char *)&flag, sizeof(flag)) < 0) {
        CCCI_TRACE_LOG("add drdi_version tag fail\n");
        return -1;
    }

    if (add_md_inf_to_tag(chk_hdr) < 0)
        return -1;

    ccci_update_load_inf_tbl_compatible(chk_hdr->common.image_type, chk_hdr->common.md_img_size,
                                        chk_hdr->common.mem_size);

    return 0;
}

/* --- Assistant function for DSP check header v3 --- */
static int ass_func_for_dsp_normal_img(void *hdr, unsigned int load_offset)
{
    struct md_check_header_v3 *chk_hdr = NULL;
    unsigned int offset, size;

    chk_hdr = (struct md_check_header_v3 *)hdr;

    offset = 0;
    size = chk_hdr->common.md_img_size;
    if (ccci_insert_mem_node(load_offset + offset, size, INF_DSP_REGION_0, 0)
                             < 0) {
        CCCI_TRACE_LOG("Update DSP RO fail\n");
        return -1;
    }
    offset = chk_hdr->common.md_img_size;
    size = chk_hdr->common.mem_size - offset;
    if (ccci_insert_mem_node(load_offset + offset, size, INF_DSP_REGION_1, 0)
                             < 0) {
        CCCI_TRACE_LOG("Update DSP RW fail\n");
        return -1;
    }

    return 0;
}

/* ========================================================================== */
/* Export to external functions                                               */
/* ========================================================================== */

int ccci_main_img_parsing(unsigned char *base, int size, char **hdr_ptr)
{
    void *hdr = NULL;
    unsigned int ver = 0;

    hdr = get_check_header(base, size, &ver);
    if (!hdr)
        return -1;

    if (hdr_ptr)
        *hdr_ptr = (char *)hdr;

    switch (ver) {
    case 6:
        return ass_func_for_v6_normal_img(hdr);
    default:
        CCCI_TRACE_LOG("not support header version:%u\n", ver);
        break;
    }

    return -1;
}

int ccci_dsp_img_parsing(unsigned char *base, int size, unsigned int offset)
{
    void *hdr = NULL;
    unsigned int ver = 0;

    hdr = get_check_header(base, size, &ver);
    if (!hdr)
        return -1;

    switch (ver) {
    case 3:
        return ass_func_for_dsp_normal_img(hdr, offset);
    default:
        CCCI_TRACE_LOG("not support header version:%u\n", ver);
        break;
    }

    return -1;
}

int ccci_get_md_version(char buf[], int size)
{
    int ret;

    ret = ccci_find_tag_inf("md_build_ver", buf, size);
    if (ret)
        return strlen(buf) + 1;
    return ret;
}
