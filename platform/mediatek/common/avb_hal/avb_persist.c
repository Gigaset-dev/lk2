/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <avb_hal.h>
#include <avb_hal_error.h>
#include <avb_ops.h>
#include <avb_persist.h>
#include <avb_slot_verify.h>
#include <avb_util.h>
#include <debug.h>
#include <string.h>

struct PersistDescriptor *g_persist_des;
uint64_t g_persist_offset;
uint32_t g_init_persist_area;
uint32_t g_pre_alloc_sz;
uint32_t g_update_persist;
uint32_t g_persist_value_total_sz;

static bool save_to_add(uint32_t ori, uint32_t value_to_add, uint32_t boundary)
{
    if ((ori + value_to_add) > boundary || (ori + value_to_add) < ori)
        return 0;

    return 1;
}

struct PersistDescriptor *get_persist_desc(void)
{
    return g_persist_des;
}

void set_update_persist_status(void)
{
    g_update_persist = 1;
}

bool get_update_persist_status(void)
{
    return g_update_persist;
}

bool get_init_persist_status(void)
{
    if (g_init_persist_area)
        return 1;
    else
        return 0;
}

static void avb_persist_descriptor_to_host_byte_order(struct PersistDescriptorHdr *buf,
                                                      struct PersistDescriptorHdr *des)
{
    if (buf == NULL || des == NULL)
        return;

    avb_memcpy(des, buf, PERSISTDESCHDR_SIZE);

    des->tag = avb_be32toh(des->tag);
    des->key_sz = avb_be32toh(des->key_sz);
    des->value_sz = avb_be32toh(des->value_sz);
}

static void avb_persist_descriptor_to_slave_byte_order(struct PersistDescriptorHdr *buf,
                                                       struct PersistDescriptorHdr *des)
{
    if (buf == NULL || des == NULL)
        return;

    avb_memcpy(des, buf, PERSISTDESCHDR_SIZE);

    des->tag = avb_htobe32(des->tag);
    des->key_sz = avb_htobe32(des->key_sz);
    des->value_sz = avb_htobe32(des->value_sz);
}

static void avb_persist_header_to_host_byte_order(union PersistHdr *buf, union PersistHdr *h)
{
    if (buf == NULL || h == NULL)
        return;

    avb_memcpy(h, buf, sizeof(h->info));
    h->info.magic = avb_be32toh(h->info.magic);
    h->info.major_ver = avb_be32toh(h->info.major_ver);
    h->info.minor_ver = avb_be32toh(h->info.minor_ver);
}

static uint32_t persist_value_validate(union PersistHdr *buf, uint32_t *exist)
{
    uint32_t ret = 0;
    union PersistHdr h;

    if (buf == NULL || exist == NULL) {
        ret = ERR_VB_INVALID_ADDR;
        goto end;
    }

    avb_persist_header_to_host_byte_order(buf, &h);
    *exist = 0;

    if (h.info.magic == PERSIST_MAGIC)
        *exist = 1;

end:
    return ret;
}

static uint32_t create_persist_area(AvbOps *ops,
                                    const char *part_name,
                                    uint64_t abs_offset,
                                    char *buf)
{
    uint32_t ret = 0;
    union PersistHdr *h = NULL;
    AvbIOResult io_res;

    if (part_name == NULL || buf == NULL) {
        ret = ERR_VB_INVALID_ADDR;
        goto end;
    }

    avb_memset(buf, 0x0, PERSISTHDR_SIZE);

    h = (union PersistHdr *)buf;

    h->info.magic = avb_htobe32(PERSIST_MAGIC);
    h->info.major_ver = avb_htobe32(PERSIST_MAJOR_VER);
    h->info.minor_ver = avb_htobe32(PERSIST_MINOR_VER);

    io_res = ops->write_to_partition(ops,
                                     part_name,
                                     abs_offset,
                                     PERSISTHDR_SIZE,
                                     h);
    if (io_res != AVB_IO_RESULT_OK) {
        dprintf(ALWAYS, "[AVB] Fail to write init persist value to %s.\n", part_name);
        ret = (uint32_t)io_res;
        goto end;
    }

end:
    return ret;
}

static uint32_t parse_persist_value(char *buf, uint32_t boundary)
{
    uint32_t ret = 0;

#if (MTK_SECURITY_SW_SUPPORT)
    struct PersistDescriptorHdr *pDes = NULL, info;
    uint32_t persist_des_idx = 0;
    uint32_t curr_locate = 0;

    if (buf == NULL) {
        ret = ERR_VB_INVALID_ADDR;
        goto end;
    }

    if (!save_to_add(curr_locate, PERSISTHDR_SIZE +
                    PERSISTDESCHDR_SIZE, boundary))
        goto end;

    pDes = (struct PersistDescriptorHdr *)(buf + PERSISTHDR_SIZE);
    avb_persist_descriptor_to_host_byte_order(pDes, &info);
    curr_locate += PERSISTHDR_SIZE + PERSISTDESCHDR_SIZE;

    while (info.tag == PERSIST_TAG_MAGIC) {
        g_persist_des[persist_des_idx].hdr.tag = info.tag;
        g_persist_des[persist_des_idx].hdr.key_sz = info.key_sz;
        g_persist_des[persist_des_idx].hdr.value_sz = info.value_sz;

        g_persist_des[persist_des_idx].key = avb_malloc(info.key_sz);
        if (g_persist_des[persist_des_idx].key == NULL) {
            ret = ERR_VB_INVALID_ADDR;
            goto end;
        }

        if (!save_to_add(curr_locate, info.key_sz, boundary)) {
            ret = ERR_VB_BUF_OVERFLOW;
            goto end;
        }

        avb_memcpy(g_persist_des[persist_des_idx].key, (buf + curr_locate), info.key_sz);
        curr_locate += info.key_sz;

        g_persist_des[persist_des_idx].value = avb_malloc(info.value_sz);
        if (g_persist_des[persist_des_idx].value == NULL) {
            ret = ERR_VB_INVALID_ADDR;
            goto end;
        }

        if (!save_to_add(curr_locate, info.value_sz, boundary)) {
            ret = ERR_VB_BUF_OVERFLOW;
            goto end;
        }

        avb_memcpy(g_persist_des[persist_des_idx].value, (buf + curr_locate), info.value_sz);
        curr_locate += info.value_sz;

        if (!save_to_add(curr_locate, PERSISTDESCHDR_SIZE, boundary))
            goto end;

        pDes = (struct PersistDescriptorHdr *)(buf + curr_locate);
        avb_persist_descriptor_to_host_byte_order(pDes, &info);
        curr_locate += PERSISTDESCHDR_SIZE;
        persist_des_idx++;
    }

    g_persist_value_total_sz = curr_locate - PERSISTHDR_SIZE;

end:
#endif

    return ret;
}

uint32_t write_persist_value(AvbOps *ops, const char *part_name)
{
    uint32_t ret = 0;

#if (MTK_SECURITY_SW_SUPPORT)
    uint32_t persist_des_idx = 0;
    uint64_t curr_offset = 0;
    uint64_t part_sz = 0;
    uint64_t total_write_sz = 0;
    uint64_t redundant_sz = 0;
    uint8_t *tmp_buf = NULL;
    AvbIOResult io_res;
    struct PersistDescriptorHdr info;

    if (!get_init_persist_status()) {
        ret = ERR_AVB_UNINITIAL_PERSIST_VALUE;
        goto end;
    }

    if (!get_update_persist_status())
        goto end;

    /* get persist storage size in specific "partition" */
    avb_hal_get_size_of_partition(ops, part_name, &part_sz);
    if (part_sz <= 0) {
        ret = ERR_AVB_NO_SUCH_PARTITION;
        goto end;
    }

    if (!save_to_add(curr_offset, g_persist_offset +
                        PERSISTHDR_SIZE, part_sz)) {
        ret = ERR_VB_BUF_OVERFLOW;
        goto end;
    }

    curr_offset += (g_persist_offset + PERSISTHDR_SIZE);

    while (persist_des_idx < MAX_PERSIST_TAG_SZ) {
        if (g_persist_des[persist_des_idx].hdr.tag != PERSIST_TAG_MAGIC) {
            persist_des_idx++;
            continue;
        }

        avb_persist_descriptor_to_slave_byte_order(
                                &g_persist_des[persist_des_idx].hdr, &info);
        io_res = ops->write_to_partition(ops,
                                 part_name,
                                 curr_offset,
                                 PERSISTDESCHDR_SIZE,
                                 &info);
        if (io_res != AVB_IO_RESULT_OK) {
            dprintf(ALWAYS, "Error writing to partition %s.\n", part_name);
            ret = (uint32_t)io_res;
            goto end;
        }

        curr_offset += PERSISTDESCHDR_SIZE;

        if (!save_to_add(curr_offset,
                            g_persist_des[persist_des_idx].hdr.key_sz, part_sz)) {
            ret = ERR_VB_BUF_OVERFLOW;
            goto end;
        }

        io_res = ops->write_to_partition(ops,
                                 part_name,
                                 curr_offset,
                                 g_persist_des[persist_des_idx].hdr.key_sz,
                                 g_persist_des[persist_des_idx].key);

        if (io_res != AVB_IO_RESULT_OK) {
            dprintf(ALWAYS, "Error writing to partition %s.\n", part_name);
            ret = (uint32_t)io_res;
            goto end;
        }

        curr_offset += g_persist_des[persist_des_idx].hdr.key_sz;

        if (!save_to_add(curr_offset,
                            g_persist_des[persist_des_idx].hdr.value_sz,
                            part_sz)) {
            ret = ERR_VB_BUF_OVERFLOW;
            goto end;
        }

        io_res = ops->write_to_partition(ops,
                                 part_name,
                                 curr_offset,
                                 g_persist_des[persist_des_idx].hdr.value_sz,
                                 g_persist_des[persist_des_idx].value);
        if (io_res != AVB_IO_RESULT_OK) {
            dprintf(ALWAYS, "Error writing to partition %s.\n", part_name);
            ret = (uint32_t)io_res;
            goto end;
        }

        curr_offset += g_persist_des[persist_des_idx].hdr.value_sz;
        persist_des_idx++;
    }

    total_write_sz = curr_offset - g_persist_offset - PERSISTHDR_SIZE;
    if (total_write_sz < g_persist_value_total_sz) {
        redundant_sz = g_persist_value_total_sz - total_write_sz;
        tmp_buf = avb_malloc(redundant_sz);
        if (tmp_buf == NULL) {
            ret = AVB_IO_RESULT_ERROR_OOM;
            goto end;
        }

        avb_memset(tmp_buf, 0x0, redundant_sz);

        io_res = ops->write_to_partition(ops,
                                         part_name,
                                         curr_offset,
                                         redundant_sz,
                                         tmp_buf);
        if (io_res != AVB_IO_RESULT_OK) {
            dprintf(ALWAYS, "Error writing to partition %s.\n", part_name);
            ret = (uint32_t)io_res;
            goto end;
        }
    }

    dprintf(ALWAYS, "Write persist value to %s.\n", part_name);

end:
    if (tmp_buf != NULL)
        avb_free(tmp_buf);
#endif

    return ret;
}

uint32_t init_persist_value(AvbOps *ops, const char *part_name, uint64_t offset)
{
    uint32_t ret = 0;

#if (MTK_SECURITY_SW_SUPPORT)
    uint64_t part_sz = 0;
    size_t read_len = 0;
    uint64_t abs_offset = 0;
    union PersistHdr *buf = NULL;
    AvbIOResult io_res;
    uint32_t persist_exist = 0;

    if (part_name == NULL) {
        ret = ERR_VB_INVALID_ADDR;
        goto end;
    }

    /* get persist storage offset in specific "partition" */
    avb_hal_get_size_of_partition(ops, part_name, &part_sz);
    if (part_sz <= 0) {
        ret = ERR_AVB_NO_SUCH_PARTITION;
        goto end;
    }

    /* Check offset value. If offset is uninitial, we assume that persist
     * value is stored at the offset is half of assigned partition size.
     */
    g_persist_offset = offset;
    if (g_persist_offset == PERSIST_UNSPECIFIED_OFFSET) {
        dprintf(ALWAYS, "[AVB] Persist offset is half of partition \"%s\" size\n",
                        part_name);
        g_persist_offset = ((part_sz / 2) + (PERSIST_ALIGN_SZ - 1)) /
                            PERSIST_ALIGN_SZ * PERSIST_ALIGN_SZ;
    }

    if (g_persist_offset >= part_sz ||
        (g_persist_offset + PERSISTHDR_SIZE + PERSISTDESC_SIZE) > part_sz ||
        (g_persist_offset + PERSISTHDR_SIZE + PERSISTDESC_SIZE) <= g_persist_offset) {
        dprintf(ALWAYS, "[AVB] persist_offset = 0x%llx is invalid\n", g_persist_offset);
        ret = ERR_AVB_INVALID_OFFSET;
        goto end;
    }

    ret = (uint32_t)avb_hal_get_abs_offset(part_name, g_persist_offset, &abs_offset);
    if (ret != AVB_IO_RESULT_OK)
        goto end;

    /* Read persist area. If it does not exist, create a new one. */
    if ((part_sz - abs_offset) > DEFAULT_PERSIST_PREALLOC_BUF_SZ)
        g_pre_alloc_sz = DEFAULT_PERSIST_PREALLOC_BUF_SZ;
    else
        g_pre_alloc_sz = part_sz - abs_offset;

    buf = avb_malloc(g_pre_alloc_sz);
    if (buf == NULL) {
        ret = ERR_VB_INVALID_ADDR;
        goto end;
    }

    io_res = ops->read_from_partition(ops,
                                    part_name,
                                    abs_offset,
                                    g_pre_alloc_sz,
                                    buf,
                                    &read_len);
    if (io_res != AVB_IO_RESULT_OK) {
        dprintf(ALWAYS, "Error loading persist value from %s.\n", part_name);
        ret = (uint32_t)io_res;
        goto end;
    }

    ret = persist_value_validate(buf, &persist_exist);
    if (ret)
        goto end;

    g_persist_des = avb_malloc(PERSISTDESC_SIZE * MAX_PERSIST_TAG_SZ);
    if (g_persist_des == NULL) {
        dprintf(ALWAYS, "g_persist_des NULL\n");
        ret = ERR_VB_INVALID_ADDR;
        goto end;
    }

    avb_memset(g_persist_des, 0x0, PERSISTDESC_SIZE * MAX_PERSIST_TAG_SZ);

    if (!persist_exist) {
        ret = create_persist_area(ops, part_name, abs_offset, (char *)buf);
        if (ret)
            goto end;
    } else {
        ret = parse_persist_value((char *)buf, g_pre_alloc_sz);
        if (ret) {
            dprintf(ALWAYS, "Parse persist value fail. ret = 0x%x\n", ret);
            avb_memset(g_persist_des, 0x0, PERSISTDESC_SIZE * MAX_PERSIST_TAG_SZ);
            ret = 0;
        }
    }

    g_init_persist_area = 1;

end:
    dprintf(ALWAYS, "[AVB] \"%s\" partition size = 0x%llx\n", part_name, part_sz);
    dprintf(ALWAYS, "[AVB] persist offset = 0x%llx\n", g_persist_offset);
    dprintf(ALWAYS, "[AVB] persist allocate size = 0x%x\n", g_pre_alloc_sz);
    dprintf(ALWAYS, "[AVB] persist area init end (%d).\n", g_init_persist_area);
    dprintf(ALWAYS, "[AVB] persist ret = 0x%x.\n", ret);

    if (buf != NULL)
        avb_free(buf);

#endif
    return ret;
}

