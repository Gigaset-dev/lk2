/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <debug.h>
#include <libfdt.h>
#include <kernel/vm.h>
#include <malloc.h>
#include <set_fdt.h>
#include <sys/types.h>
#include <string.h>

#include "ccci_ld_md_core.h"
#include "ccci_ld_md_log.h"

#define LOCAL_TRACE 0

/******************************************************************************
 ** Sub module: ccci_lk_info (LK to Kernel arguments and information)
 **  Using share memory and device tree
 **  ccci_lk_info structure is stored at device tree
 **  other more detail parameters are stored at share memory
 ******************************************************************************/
struct ccci_lk_info_s {
    unsigned long long base_addr;
    unsigned int       size;
    int                lk_info_err_no;
    int                version;
    int                tag_num;
    unsigned int       ld_flag;
    int                ld_md_errno[MAX_MD_NUM];
};

#define TAG_MAX_SIZE    (0x10000)
#define CCCI_TAG_NAME_LEN   (64)
#define CCCI_LK_INFO_VER    (3) /* 1/2- ap-md share memory re-use, _ccci_lk_info/_v2 */

struct ccci_tag_s {
    char tag_name[CCCI_TAG_NAME_LEN];
    unsigned int data_offset;
    unsigned int data_size;
    unsigned int next_tag_offset;
};

static struct ccci_lk_info_s s_lk_inf;

int ccci_lk_tag_info_init(void)
{
    int i;
    unsigned char *mem_addr;
    unsigned long long pa;

    pa = ccci_resv_named_memory("ccci_tag_mem", TAG_MAX_SIZE, &mem_addr);
    if (!pa) {
        CCCI_TRACE_LOG("allocate tag memory fail\n");
        return -1;
    }

    s_lk_inf.base_addr = (u64)mem_addr;
    s_lk_inf.size = 0;
    s_lk_inf.tag_num = 0;
    s_lk_inf.version = CCCI_LK_INFO_VER;
    s_lk_inf.ld_flag = 0;
    s_lk_inf.lk_info_err_no = 0;
    for (i = 0; i < MAX_MD_NUM; i++)
        s_lk_inf.ld_md_errno[i] = 0;
    return 0;
}

void ccci_update_common_err_info(int error)
{
    s_lk_inf.lk_info_err_no = error;
}

void ccci_update_err_by_md(int md_id, int error)
{
    s_lk_inf.ld_md_errno[md_id] = error;
}

void ccci_update_md_load_bitmap(int md_id)
{
    s_lk_inf.ld_flag |= (1<<md_id);
}

/*
 * insert_ccci_tag_inf - instert TAG information to memory.
 *
 * @name: assign a dedicated name for this data pattern
 * @data: the data to be stored
 * @size: size of data
 */

int ccci_insert_tag_inf(const char *name, char *data, unsigned int size)
{
    int i, total_size;
    unsigned int curr_offset = s_lk_inf.size;
    struct ccci_tag_s *tag = (struct ccci_tag_s *)((unsigned long)
                                             (s_lk_inf.base_addr + curr_offset));
    char *buf = (char *)((unsigned long)
    (s_lk_inf.base_addr + curr_offset + sizeof(struct ccci_tag_s)));

    if (size == 0) {
        CCCI_TRACE_LOG("tag info size is 0\n");
        return 0;
    }

    if (!name || !data) {
        CCCI_TRACE_LOG("name or data is NULL\n");
        return 0;
    }
    /* make sure 8 bytes align */
    total_size = ROUNDUP(curr_offset + size + sizeof(struct ccci_tag_s), 8);

    if (total_size >= TAG_MAX_SIZE) {
        CCCI_TRACE_LOG("not enought memory to insert(%d)\n", TAG_MAX_SIZE - total_size);
        return -1;
    }

    /* Copy name */
    for (i = 0; i < CCCI_TAG_NAME_LEN-1; i++) {
        if (name[i] == 0)
            break;

        tag->tag_name[i] = name[i];
    }
    tag->tag_name[i] = 0;

    /* Set offset */
    tag->data_offset = curr_offset + sizeof(struct ccci_tag_s);
    /* Set data size */
    tag->data_size = size;
    /* Set next offset */
    tag->next_tag_offset = total_size;
    /* Copy data */
    memcpy(buf, data, size);

    /* update control structure */
    s_lk_inf.size = total_size;
    s_lk_inf.tag_num++;

    LTRACEF("tag insert(%d), [name]:%s [4 bytes]:[%x][%x][%x][%x] [size]:%d\n",
             s_lk_inf.tag_num, name, data[0], data[1], data[2], data[3], size);

    return 0;
}

int ccci_find_tag_inf(const char *name, char *buf, unsigned int size)
{
    int i;
    int cpy_size;
    char *curr;
    struct ccci_tag_s tag;

    if (buf == NULL)
        return -1;

    if (!s_lk_inf.base_addr)
        return -2;

    curr = (char *)(unsigned long)s_lk_inf.base_addr;
    for (i = 0; i < s_lk_inf.tag_num; i++) {
        /* 1. Copy tag */
        memcpy(&tag, curr, sizeof(struct ccci_tag_s));

        /* 2. compare tag value. */
        /*
         * In "ccci_insert_tag_inf", it has been ensured
         * that the end of "tag_name" is '\0'
         */
        tag.tag_name[CCCI_TAG_NAME_LEN - 1] = '\0';
        if (strcmp(tag.tag_name, name) != 0) {
            curr = (char *)(s_lk_inf.base_addr + tag.next_tag_offset);
            continue;
        }
        /* found it */
        cpy_size = size > tag.data_size?tag.data_size:size;
        memcpy(buf, (void *)(s_lk_inf.base_addr + tag.data_offset), cpy_size);

        return cpy_size;
    }

    return -3;
}

void ccci_lk_info_ctl_dump(void)
{
    ALWAYS_LOG("lk info.base_addr: 0x%x\n", (unsigned int)s_lk_inf.base_addr);
    ALWAYS_LOG("lk info.base_addr_phy: 0x%x\n",
               (unsigned int)vaddr_to_paddr((void *)((unsigned long)s_lk_inf.base_addr)));

    ALWAYS_LOG("lk info.size:      0x%x\n", s_lk_inf.size);
    ALWAYS_LOG("lk info.tag_num:   0x%x\n", s_lk_inf.tag_num);
}


/******************************************************************************
 * NOTE: Customers need to implement their own customized_get_rf_id()
 *       function. If customized_get_rf_id() is not implemented, the
 *       value 0x0F0F0F0F will be used as the default. Each customer can
 *       maintain their own implementation of customized_get_rf_id()
 *       in a separate source file as long as it can be linked.
 ******************************************************************************/
static unsigned int get_md_rf_id(void)
{
    unsigned int prop_val = 0x0F0F0F0F;

    if (customized_get_rf_id)
        prop_val = customized_get_rf_id();

    return prop_val;
}


/******************************************************************************
 * NOTE: Customers need to implement their own customized_get_model_id()
 *       function. If customized_get_model_id() is not implemented, the
 *       value 0x0F0F0F0F will be used as the default. Each customer can
 *       maintain their own implementation of customized_get_model_id()
 *       in a separate source file as long as it can be linked.
 ******************************************************************************/
static unsigned int get_md_model_id(void)
{
    unsigned int prop_val = 0x0F0F0F0F;

    if (customized_get_model_id)
        prop_val = customized_get_model_id();

    return prop_val;
}

static void create_md_attr_dt_node(void *fdt)
{
    unsigned int prop_val = 0x0F0F0F0F; /*  no property set */
    int err, offset;
    char compatible[] = "mediatek,md_attr_node";

    offset = fdt_add_subnode(fdt, 0, "md_attr_node");
    if (offset < 0)
        return;

    err = fdt_setprop(fdt, offset, "compatible", compatible, sizeof(compatible));
    if (err)
        return;

    /* For RF ID ++++++++++ */
    prop_val = get_md_rf_id();
    err = fdt_setprop_u32(fdt, offset, "mediatek,md_drdi_rf_set_idx", prop_val);
    if (err)
        return;
    /* For RF ID ---------- */

    /* For model ID ++++++++ */
    prop_val = get_md_model_id();
    err = fdt_setprop_u32(fdt, offset, "mediatek,md_product_name_model_id", prop_val);
    if (err)
        return;
    /* For model ID -------- */

}

/*
 ** This function will using globle variable: s_lk_inf;
 ** and a weak function will be called: md_reserve_mem_size_fixup
 **/
void ccci_update_md_arg_info_to_dt(void *fdt)
{
    int node_offset = 0;
    char *ptr = (char *)&s_lk_inf;
    int ret = 0;

    node_offset = fdt_path_offset(fdt, "/mddriver");
    if (node_offset < 0) {
        ALWAYS_LOG("[ccci]/mddriver not found, try /soc/mddriver\n");
        node_offset = fdt_path_offset(fdt, "/soc/mddriver");
        if (node_offset < 0) {
            CCCI_TRACE_LOG("/soc/mddriver not found!\n");
            return;
        }
    }
    s_lk_inf.base_addr = vaddr_to_paddr((void *)((addr_t)s_lk_inf.base_addr));

    ret = fdt_setprop(fdt, node_offset,
        "ccci,modem_info_v2", ptr, sizeof(struct ccci_lk_info_s));
    if (ret != 0)
        CCCI_TRACE_LOG("create modem arguments info DT Fail\n");
    else
        ALWAYS_LOG("create modem arguments info FDT OK\n");

    if ((s_lk_inf.ld_flag & (1<<0)) == 0) {
        ret = fdt_setprop(fdt, node_offset,
        "status", "disabled", strlen("disabled"));
        if (ret != 0)
            CCCI_TRACE_LOG("create modem status info DT Fail\n");
        else
            ALWAYS_LOG("create modem status info FDT OK\n");
    } else
        ALWAYS_LOG("md1 enabled\n");

    create_md_attr_dt_node(fdt);

}

SET_FDT_INIT_HOOK(ccci_update_md_arg_info_to_dt, ccci_update_md_arg_info_to_dt);
