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

#include "ccci_ld_md_log.h"
#include "ccci_ld_md_mem.h"

#define LOCAL_TRACE 0

struct md_mem_blk_node {
    struct md_mem_blk desc;
    int next;
};

#define BLK_SIZE    (sizeof(struct md_mem_blk_node))

static struct md_mem_blk *s_md_mem_tbl;
static struct md_mem_blk_node *s_md_mem_tbl_list;

static int s_md_mem_tbl_list_free_head;
static int s_md_mem_tbl_list_head;
static int s_md_mem_tbl_list_num;

static int s_1st_md_mem_node_rdy, s_md_mem_layout_init_done;
static int s_final_md_mem_blk_node_num;

static int md_mem_blk_inf_dump_helper(struct md_mem_blk *desc)
{
    char inf_str[22];
    char attr_str[20];
    unsigned int inf_flag, attr_flag;

    if (!desc) {
        CCCI_TRACE_LOG("desc is NULL\r\n");
        return -1;
    }

    attr_flag = desc->attr_flag;
    inf_flag = desc->inf_flag;
    memset((unsigned char *)inf_str, '-', sizeof(inf_str) - 1);
    inf_str[sizeof(inf_str) - 1] = 0;
    memset((unsigned char *)attr_str, '-', sizeof(attr_str) - 1);
    attr_str[sizeof(attr_str) - 1] = 0;

    // Make INFO string
    if (inf_flag & INF_MD_REGION_0) {
        inf_str[0] = 'M';
        inf_str[1] = '0';
    }
    if (inf_flag & INF_MD_REGION_1) {
        inf_str[2] = 'M';
        inf_str[3] = '1';
    }
    if (inf_flag & INF_MD_REGION_2) {
        inf_str[4] = 'M';
        inf_str[5] = '2';
    }
    if (inf_flag & INF_MD_REGION_3) {
        inf_str[6] = 'M';
        inf_str[7] = '3';
    }
    if (inf_flag & INF_MD_REGION_4) {
        inf_str[8] = 'M';
        inf_str[9] = '4';
    }
    if (inf_flag & INF_MD_REGION_5) {
        inf_str[10] = 'M';
        inf_str[11] = '5';
    }
    if (inf_flag & INF_MD_REGION_6) {
        inf_str[12] = 'M';
        inf_str[13] = '6';
    }
    if (inf_flag & INF_MD_REGION_7) {
        inf_str[14] = 'M';
        inf_str[15] = '7';
    }
    if (inf_flag & INF_DSP_REGION_0) {
        inf_str[16] = 'D';
        inf_str[17] = '0';
    }
    if (inf_flag & INF_DSP_REGION_1) {
        inf_str[18] = 'D';
        inf_str[19] = '1';
    }

    // Make ATTR String
    if (attr_flag & ATTR_WHOLE) {
        attr_str[0] = 'W';
        attr_str[1] = '-';
    }
    if (attr_flag & ATTR_DSP_RESERVED) {
        attr_str[2] = 'D';
        attr_str[3] = 'r';
    }
    if (attr_flag & ATTR_PADDING_MEMORY) {
        attr_str[4] = 'P';
        attr_str[5] = 'd';
    }
    if (attr_flag & ATTR_PADDING_AT_MIDDLE) {
        attr_str[6] = 'P';
        attr_str[7] = 'm';
    }
    if (attr_flag & ATTR_PADDING_RETRIEVE) {
        attr_str[8] = 'P';
        attr_str[9] = 'r';
    }
    if (attr_flag & ATTR_DRDI_SRC) {
        attr_str[10] = 'D';
        attr_str[11] = 's';
    }
    if (attr_flag & ATTR_DRDI_RT_DES) {
        attr_str[12] = 'D';
        attr_str[13] = 'd';
    }
    if (attr_flag & ATTR_RAMDISK) {
        attr_str[14] = 'N';
        attr_str[15] = 'v';
    }
    if (attr_flag & ATTR_OPTIONAL) {
        attr_str[16] = 'O';
        attr_str[17] = 'p';
    }

    ALWAYS_LOG("| 0x%016llx | 0x%08x | 0x%08x | %s | %s |\r\n",
            desc->ap_phy, desc->offset, desc->size, inf_str, attr_str);

    return 0;
}

static void dump_tmp_layout(void)
{
    int i;
    int curr;

    if (!s_md_mem_tbl_list)
        return;

    curr = s_md_mem_tbl_list_head;

    for (i = 0; i < s_md_mem_tbl_list_num; i++) {
        if (curr < 0)
            break;
        if (curr >= s_md_mem_tbl_list_num)
            break;
        md_mem_blk_inf_dump_helper(&s_md_mem_tbl_list[curr].desc);
        curr = s_md_mem_tbl_list[curr].next;
    }
}

static int alloc_md_mem_blk_list_mem(int num)
{
    int i;

    s_md_mem_tbl_list = (struct md_mem_blk_node *)malloc(BLK_SIZE * num);
    if (!s_md_mem_tbl_list) {
        CCCI_TRACE_LOG("Alloc fail\r\n");
        return -1;
    }

    s_md_mem_tbl_list_num = num;
    memset((unsigned char *)s_md_mem_tbl_list, 0, BLK_SIZE * num);
    for (i = 0; i < num; i++)
        s_md_mem_tbl_list[i].next = i + 1;

    //Update node next to -1 to mark as last one
    s_md_mem_tbl_list[num-1].next = -1;
    s_md_mem_tbl_list_free_head = 0;

    return 0;
}

static int get_free_md_mem_blk_node(void)
{
    int free_idx = s_md_mem_tbl_list_free_head;

    if (!s_md_mem_tbl_list) {
        CCCI_TRACE_LOG("MD mem blk node NULL\r\n");
        return -1;
    }
    if (s_md_mem_tbl_list_free_head < 0) {
        CCCI_TRACE_LOG("MD mem blk node empty\r\n");
        return -1;
    }
    if (free_idx >= s_md_mem_tbl_list_num) {
        CCCI_TRACE_LOG("Invalid free idx:%d, max:%d\r\n", free_idx,
                    s_md_mem_tbl_list_num - 1);
        return -1;
    }

    s_md_mem_tbl_list_free_head = s_md_mem_tbl_list[free_idx].next;
    return free_idx;
}

/* This function only support 3 cases
 *   case 0: |======|======| ==> |++====|======|
 *   case 1: |======|======| ==> |==++==|======|
 *   case 2: |======|======| ==> |====++|======|
 *   case 4: |======|======| ==> |=====+|+=====| NOT suppose case
 */

//case 0: |======|======| ==> |++====|======|
static int insert_case_0(int insert_id, unsigned int offset, unsigned int size,
                         unsigned int inf_flag, unsigned int attr_flag)
{
    int new_node_id = get_free_md_mem_blk_node();

    if (new_node_id < 0) {
        CCCI_TRACE_LOG("Get free node fail\r\n");
        return -1;
    }

    memcpy((unsigned char *)&s_md_mem_tbl_list[new_node_id],
           (unsigned char *)&s_md_mem_tbl_list[insert_id], BLK_SIZE);

    // Update list link
    s_md_mem_tbl_list[new_node_id].next = s_md_mem_tbl_list[insert_id].next;
    s_md_mem_tbl_list[insert_id].next = new_node_id;

    // Update node value
    s_md_mem_tbl_list[insert_id].desc.size = size;
    s_md_mem_tbl_list[insert_id].desc.inf_flag |= inf_flag;
    s_md_mem_tbl_list[insert_id].desc.attr_flag |= attr_flag;

    s_md_mem_tbl_list[new_node_id].desc.offset = offset + size;
    s_md_mem_tbl_list[new_node_id].desc.size =
                               s_md_mem_tbl_list[new_node_id].desc.size - size;

    return 1;
}

//case 1: |======|======| ==> |==++==|======|   ATTR_PADDING_AT_MIDDLE
static int insert_case_1(int insert_id, unsigned int offset, unsigned int size,
                         unsigned int inf_flag, unsigned int attr_flag)
{
    int new_node_id1, new_node_id2 = get_free_md_mem_blk_node();

    new_node_id1 = get_free_md_mem_blk_node();
    if (new_node_id1 < 0) {
        CCCI_TRACE_LOG("Get free node fail - 0\r\n");
        return -1;
    }
    new_node_id2 = get_free_md_mem_blk_node();
    if (new_node_id2 < 0) {
        CCCI_TRACE_LOG("Get free node fail - 1\r\n");
        return -1;
    }

    memcpy((unsigned char *)&s_md_mem_tbl_list[new_node_id1],
           (unsigned char *)&s_md_mem_tbl_list[insert_id], BLK_SIZE);
    memcpy((unsigned char *)&s_md_mem_tbl_list[new_node_id2],
           (unsigned char *)&s_md_mem_tbl_list[insert_id], BLK_SIZE);

    // Update list link
    s_md_mem_tbl_list[new_node_id2].next = s_md_mem_tbl_list[insert_id].next;
    s_md_mem_tbl_list[insert_id].next = new_node_id1;
    s_md_mem_tbl_list[new_node_id1].next = new_node_id2;

    // Update node value
    s_md_mem_tbl_list[insert_id].desc.size =
                             offset - s_md_mem_tbl_list[insert_id].desc.offset;

    s_md_mem_tbl_list[new_node_id1].desc.offset = offset;
    s_md_mem_tbl_list[new_node_id1].desc.size = size;
    s_md_mem_tbl_list[new_node_id1].desc.inf_flag |= inf_flag;
    s_md_mem_tbl_list[new_node_id1].desc.attr_flag |= attr_flag;
    if (attr_flag & ATTR_PADDING_MEMORY)
        s_md_mem_tbl_list[new_node_id1].desc.attr_flag |=
                                                        ATTR_PADDING_AT_MIDDLE;

    s_md_mem_tbl_list[new_node_id2].desc.offset = offset + size;
    s_md_mem_tbl_list[new_node_id2].desc.size -=
                                 s_md_mem_tbl_list[insert_id].desc.size + size;

    return 2;
}

//case 2: |======|======| ==> |====++|======|
static int insert_case_2(int insert_id, unsigned int offset, unsigned int size,
                         unsigned int inf_flag, unsigned int attr_flag)
{
    int new_node_id = get_free_md_mem_blk_node();

    if (new_node_id < 0) {
        CCCI_TRACE_LOG("Get free node fail\r\n");
        return -1;
    }

    memcpy((unsigned char *)&s_md_mem_tbl_list[new_node_id],
           (unsigned char *)&s_md_mem_tbl_list[insert_id], BLK_SIZE);

    // Update list link
    s_md_mem_tbl_list[new_node_id].next = s_md_mem_tbl_list[insert_id].next;
    s_md_mem_tbl_list[insert_id].next = new_node_id;

    // Update node value
    s_md_mem_tbl_list[new_node_id].desc.size = size;
    s_md_mem_tbl_list[new_node_id].desc.offset = offset;
    s_md_mem_tbl_list[new_node_id].desc.inf_flag |= inf_flag;
    s_md_mem_tbl_list[new_node_id].desc.attr_flag |= attr_flag;

    s_md_mem_tbl_list[insert_id].desc.size =
                                 s_md_mem_tbl_list[insert_id].desc.size - size;

    return 1;
}

static const unsigned int md_region_flag[] = {
    INF_MD_REGION_0, INF_MD_REGION_1, INF_MD_REGION_2, INF_MD_REGION_3,
    INF_MD_REGION_4, INF_MD_REGION_5, INF_MD_REGION_6, INF_MD_REGION_7,
};

static int alloc_mem_blk_list(int num)
{
    if (num < 2) {
        CCCI_TRACE_LOG("Num is too less:%d\r\n", num);
        return -1;
    }

    return alloc_md_mem_blk_list_mem(num);
}

static struct md_mem_blk *alloc_mem_blk_table(int num)
{
    struct md_mem_blk *ptr = NULL;

    ptr = (struct md_mem_blk *)malloc(sizeof(struct md_mem_blk) * num);
    if (!ptr) {
        CCCI_TRACE_LOG("Alloc memory fail\r\n");
        return NULL;
    }
    memset(ptr, 0, sizeof(struct md_mem_blk) * num);
    return ptr;
}

/* ========================================================================= */
/* Export to external functions                                              */
/* ========================================================================= */
int ccci_mem_layout_init(int num)
{
    struct md_mem_blk *ptr = NULL;

    if (alloc_mem_blk_list(num) < 0)
        return -1;

    ptr = alloc_mem_blk_table(num);
    if (!ptr) {
        free(s_md_mem_tbl_list);
        return -1;
    }
    s_md_mem_tbl = ptr;

    s_md_mem_layout_init_done = 1;

    return 0;
}

void ccci_mem_layout_de_init(void)
{
    if (s_md_mem_tbl_list) {
        free(s_md_mem_tbl_list);
        s_md_mem_tbl_list = NULL;
    }

    if (s_md_mem_tbl) {
        free(s_md_mem_tbl);
        s_md_mem_tbl = NULL;
    }

    s_md_mem_tbl_list_free_head = 0;
    s_md_mem_tbl_list_head = 0;
    s_md_mem_tbl_list_num = 0;
    s_1st_md_mem_node_rdy = 0;
    s_final_md_mem_blk_node_num = 0;
    s_md_mem_layout_init_done = 0;
}

struct md_mem_blk *ccci_mem_layout_tbl_get(void)
{
    if (!s_md_mem_layout_init_done) {
        CCCI_TRACE_LOG("MD memory layout obj not init\r\n");
        return NULL;
    }

    return s_md_mem_tbl;
}

int ccci_create_1st_mem_node(unsigned int size)
{
    int id;

    if (!s_md_mem_layout_init_done) {
        CCCI_TRACE_LOG("MD memory layout obj not init\r\n");
        return -1;
    }

    id = get_free_md_mem_blk_node();
    if (id < 0)
        return -1;

    s_md_mem_tbl_list[id].desc.offset = 0;
    s_md_mem_tbl_list[id].desc.size = size;
    s_md_mem_tbl_list[id].desc.inf_flag = 0;
    s_md_mem_tbl_list[id].desc.attr_flag = 0;
    s_md_mem_tbl_list[id].next = -1;
    s_md_mem_tbl_list_head = id;
    s_1st_md_mem_node_rdy = 1;
    s_final_md_mem_blk_node_num = 1;

    return 0;
}

int ccci_insert_mem_node(unsigned int offset, unsigned int size,
                         unsigned int inf_flag, unsigned int attr_flag)
{
    int ins_node_id, ret, matched = 0, i;
    unsigned int curr_offset, curr_size, curr_end;

    if (!s_md_mem_layout_init_done) {
        CCCI_TRACE_LOG("MD memory layout obj not init\r\n");
        return -1;
    }

    if (!size) {
        CCCI_TRACE_LOG("Args: size is invalid:0\r\n");
        return -1;
    }

    if (!s_1st_md_mem_node_rdy) {
        CCCI_TRACE_LOG("MD mem tbl 1st reserved not ready\r\n");
        return -1;
    }

    // Step 1, find insert position
    ins_node_id = s_md_mem_tbl_list_head;
    if (ins_node_id < 0) {
        CCCI_TRACE_LOG("MD mem tbl 1st node id invalid:%d\r\n", ins_node_id);
        return -1;
    }

    for (i = 0; i < s_md_mem_tbl_list_num; i++) {
        curr_offset = s_md_mem_tbl_list[ins_node_id].desc.offset;
        curr_size = s_md_mem_tbl_list[ins_node_id].desc.size;
        curr_end = curr_offset + curr_size - 1;
        if ((curr_offset <= offset) && (offset <= curr_end)) {
            matched = 1;
            break;
        }
        ins_node_id = s_md_mem_tbl_list[ins_node_id].next;
        if (ins_node_id < 0) {
            CCCI_TRACE_LOG("Enter md mem node end_0, not found\r\n");
            dump_tmp_layout();
            return -1;
        }
        if (ins_node_id > s_md_mem_tbl_list_num) {
            CCCI_TRACE_LOG("MD mem node id invalid:%d for insert\r\n", ins_node_id);
            dump_tmp_layout();
            return -1;
        }
    }
    if (!matched) {
        CCCI_TRACE_LOG("Enter md mem node end_1, not found\r\n");
        dump_tmp_layout();
        return -1;
    }

    // Step 2, check if "offset + size" is valid
    if ((offset + size - 1) > curr_end) {
        CCCI_TRACE_LOG("Invalid args: offset:%u, size:%u\r\n", offset, size);
        dump_tmp_layout();
        return -1;
    }

    if ((offset == curr_offset) && ((offset + size - 1) == curr_end)) {
        if (((~s_md_mem_tbl_list[ins_node_id].desc.inf_flag) & inf_flag) ||
            ((~s_md_mem_tbl_list[ins_node_id].desc.attr_flag) & attr_flag)) {
            ALWAYS_LOG("Update attr & inf for exist block\n");
            s_md_mem_tbl_list[ins_node_id].desc.inf_flag |= inf_flag;
            s_md_mem_tbl_list[ins_node_id].desc.attr_flag |= attr_flag;

            return 0;
        } else {
            CCCI_TRACE_LOG("Insert dupliacte md mem blk(0x%08x:0x%08x)\r\n",
                            offset, size);
            dump_tmp_layout();
            return -1;
        }
    }

    // Step 3, insert operation
    if (offset == curr_offset) {// case 0: |======|======| ==> |++====|======|
        ret = insert_case_0(ins_node_id, offset, size, inf_flag, attr_flag);
        if (ret > 0)
            s_final_md_mem_blk_node_num += ret;
        return ret;
    }
    if ((offset + size - 1) == curr_end) {
        // case 2: |======|======| ==> |====++|======|
        ret = insert_case_2(ins_node_id, offset, size, inf_flag, attr_flag);
        if (ret > 0)
            s_final_md_mem_blk_node_num += ret;
        return ret;
    }
    //case 1: |======|======| ==> |==++==|======|
    ret = insert_case_1(ins_node_id, offset, size, inf_flag, attr_flag);
    if (ret > 0)
        s_final_md_mem_blk_node_num += ret;
    return ret;
}

struct md_mem_blk *ccci_get_1st_sub_mem_blk(unsigned int inf_flags,
                                            unsigned int attr_flags)
{
    int i = 0;
    int curr;

    if (s_final_md_mem_blk_node_num == 0) {
        ALWAYS_LOG("No need to generate for num:0\r\n");
        return NULL;
    }

    curr = s_md_mem_tbl_list_head;
    for (i = 0; i < s_final_md_mem_blk_node_num; i++) {
        if (curr >= s_md_mem_tbl_list_num) {
            CCCI_TRACE_LOG("Invalid curr id:%d\r\n", curr);
            return NULL;
        }
        if (curr == -1)
            break;
        if (s_md_mem_tbl_list[curr].desc.inf_flag & inf_flags)
            return &s_md_mem_tbl_list[curr].desc;
        if (s_md_mem_tbl_list[curr].desc.attr_flag & attr_flags)
            return &s_md_mem_tbl_list[curr].desc;
        curr = s_md_mem_tbl_list[curr].next;
    }
    return NULL;
}

int ccci_update_base_to_mem_tbl(unsigned long long pa_base)
{
    int i = 0;
    int curr;
    unsigned long long start_addr = pa_base;

    if (s_final_md_mem_blk_node_num == 0) {
        ALWAYS_LOG("No need to generate for num:0\r\n");
        return -1;
    }

    curr = s_md_mem_tbl_list_head;
    for (i = 0; i < s_final_md_mem_blk_node_num; i++) {
        if (curr >= s_md_mem_tbl_list_num) {
            CCCI_TRACE_LOG("Invalid curr id:%d\r\n", curr);
            return -1;
        }
        if (curr == -1)
            break;
        s_md_mem_tbl[i].offset = s_md_mem_tbl_list[curr].desc.offset;
        s_md_mem_tbl[i].size = s_md_mem_tbl_list[curr].desc.size;
        s_md_mem_tbl[i].attr_flag = s_md_mem_tbl_list[curr].desc.attr_flag;
        s_md_mem_tbl[i].inf_flag = s_md_mem_tbl_list[curr].desc.inf_flag;
        s_md_mem_tbl[i].ap_phy = start_addr + s_md_mem_tbl[i].offset;

        curr = s_md_mem_tbl_list[curr].next;
    }
    if (i != s_final_md_mem_blk_node_num) {
        CCCI_TRACE_LOG("MD mem tbl num miss-match:%d--%d\r\n", i,
                    s_final_md_mem_blk_node_num);
        return -1;
    }
    return i;
}

void ccci_mem_blk_inf_dump(void)
{
    int i;

    if (!s_final_md_mem_blk_node_num) {
        CCCI_TRACE_LOG("Table number is zero\r\n");
        return;
    }

    ALWAYS_LOG("|------ AP Addr -----|-- MD Addr -|--- Size ---|------ INF Flags ------|---- Attr Flags -----|\r\n");
    for (i = 0; i < s_final_md_mem_blk_node_num; i++)
        md_mem_blk_inf_dump_helper(&s_md_mem_tbl[i]);

    ALWAYS_LOG("------- Dump md memory layout end -----------------\r\n");
}

void ccci_mem_blk_tmp_list_dump(void)
{
    int i = 0;
    int curr;

    if (s_final_md_mem_blk_node_num == 0) {
        CCCI_TRACE_LOG("Temp list is NULL\r\n");
        return;
    }

    curr = s_md_mem_tbl_list_head;
    ALWAYS_LOG("|------ AP Addr -----|-- MD Addr -|--- Size ---|------ INF Flags ------|---- Attr Flags -----|\r\n");
    for (i = 0; i < s_final_md_mem_blk_node_num; i++) {
        if (curr >= s_md_mem_tbl_list_num) {
            CCCI_TRACE_LOG("Invalid curr id:%d\r\n", curr);
            return;
        }
        if (curr == -1)
            break;
        md_mem_blk_inf_dump_helper(&s_md_mem_tbl_list[curr].desc);
        curr = s_md_mem_tbl_list[curr].next;
    }
    ALWAYS_LOG("------- Dump md memory layout temp list end -----------------\r\n");
}

unsigned int ccci_layout_md_region_to_flag(unsigned int id)
{
    if (id >= countof(md_region_flag))
        return 0;
    return md_region_flag[id];
}
