/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <avb_ops.h>
#include <avb_persist.h>
#include <avbkey.h>
#include <avbkey_recovery.h>
#include <debug.h>
#include <efi.h>
#include <err.h>
#include <lib/bio.h>
#include <libavb.h>
#include <lock_state.h>
#include <partition_utils.h>
#include <sboot.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <verified_boot_common.h>

uint8_t g_avb_key[AVB_PUBK_SZ] = {AVB_PUBK};
uint8_t g_avb_recovery_key[AVB_RECOVERY_PUBK_SZ] = {AVB_RECOVERY_PUBK};

static int64_t get_total_size(const char *partition)
{
    bdev_t *bdev;
    int64_t total_size;

    bdev = bio_open_by_label(partition);
    if (!bdev)
        return -1;

    total_size = bdev->total_size;
    bio_close(bdev);

    return total_size;
}

AvbIOResult avb_hal_get_abs_offset(const char *partition,
                                   int64_t offset,
                                   uint64_t *abs_offset)
{
    int64_t total_size;

    if (partition == NULL || abs_offset == NULL)
        return AVB_IO_RESULT_ERROR_IO;

    total_size = get_total_size(partition);
    if (total_size < 0)
        return AVB_IO_RESULT_ERROR_NO_SUCH_PARTITION;

    if (offset < 0) {
        if (offset + total_size < 0)
            return AVB_IO_RESULT_ERROR_RANGE_OUTSIDE_PARTITION;
        offset += total_size;
    }

    *abs_offset = offset;

    return AVB_IO_RESULT_OK;
}

static AvbIOResult avb_hal_read_from_partition(AvbOps *ops,
                                        const char *partition,
                                        int64_t offset,
                                        size_t num_bytes,
                                        void *buffer,
                                        size_t *out_num_read)
{
    uint64_t abs_offset;
    ssize_t read_len;
    bdev_t *bdev;
    AvbIOResult ret;

    if (partition == NULL || buffer == NULL || out_num_read == NULL)
        return AVB_IO_RESULT_ERROR_IO;

    ret = avb_hal_get_abs_offset(partition, offset, &abs_offset);
    if (ret != AVB_IO_RESULT_OK)
        return ret;

    bdev = bio_open_by_label(partition);
    if (!bdev)
        return AVB_IO_RESULT_ERROR_NO_SUCH_PARTITION;

    read_len = bio_read(bdev, buffer, abs_offset, num_bytes);

    bio_close(bdev);

    if (read_len < (ssize_t)num_bytes)
        return AVB_IO_RESULT_ERROR_IO;

    *out_num_read = (size_t)read_len;

    return AVB_IO_RESULT_OK;
}

static AvbIOResult avb_hal_write_to_partition(AvbOps *ops,
                                       const char *partition,
                                       int64_t offset,
                                       size_t num_bytes,
                                       const void *buffer)
{
    uint64_t abs_offset;
    ssize_t write_len;
    bdev_t *bdev;
    AvbIOResult ret;

    if (partition == NULL || buffer == NULL)
        return AVB_IO_RESULT_ERROR_IO;

    ret = avb_hal_get_abs_offset(partition, offset, &abs_offset);
    if (ret != AVB_IO_RESULT_OK)
        return ret;

    bdev = bio_open_by_label(partition);
    if (!bdev)
        return AVB_IO_RESULT_ERROR_NO_SUCH_PARTITION;

    write_len = bio_write(bdev, buffer, abs_offset, num_bytes);

    bio_close(bdev);

    if (write_len < (ssize_t)num_bytes)
        return AVB_IO_RESULT_ERROR_IO;

    return AVB_IO_RESULT_OK;
}

static AvbIOResult avb_hal_verify_public_key(AvbOps *ops,
                                      const uint8_t *public_key_data,
                                      size_t public_key_length,
                                      const uint8_t *public_key_metadata,
                                      size_t public_key_metadata_length,
                                      bool *out_is_trusted)
{
    AvbIOResult ret = AVB_IO_RESULT_OK;
    uint8_t *pubk;
    uint32_t i;
    uint32_t pubk_sz;

    if (out_is_trusted == NULL || public_key_data == NULL)
        return AVB_IO_RESULT_ERROR_IO;

    pubk = (uint8_t *)(public_key_data + sizeof(AvbRSAPublicKeyHeader));
    pubk_sz = avb_htobe32(
              ((AvbRSAPublicKeyHeader *)public_key_data)->key_num_bits) / 8;

    *out_is_trusted = false;
    if (pubk_sz != AVB_PUBK_SZ) {
        dprintf(CRITICAL, "[AVB] invalid pubk size\n");
        goto end;
    }

    if (memcmp((void *)g_avb_key, (void *)pubk, pubk_sz) == 0)
        *out_is_trusted = true;
    else {
        dprintf(CRITICAL, "[AVB] lk_avb_key =\n");
        for (i = 0; i < AVB_PUBK_SZ; i++) {
            dprintf(CRITICAL, "0x%x ", g_avb_key[i]);
            if (((i + 1) % 16) == 0)
                dprintf(CRITICAL, "\n");
        }
        dprintf(CRITICAL, "[AVB] vbmeta_avb_key =\n");
        for (i = 0; i < AVB_PUBK_SZ; i++) {
            dprintf(CRITICAL, "0x%x ", pubk[i]);
            if (((i + 1) % 16) == 0)
                dprintf(CRITICAL, "\n");
        }
    }

end:

    return ret;
}

static AvbIOResult avb_hal_read_rollback_index(AvbOps *ops,
                                        size_t rollback_index_location,
                                        uint64_t *out_rollback_index)
{
    AvbIOResult ret = AVB_IO_RESULT_OK;
    uint32_t version = 0;

    if (out_rollback_index == NULL)
        return AVB_IO_RESULT_ERROR_IO;

    *out_rollback_index = 0;

    ret = sec_get_avb_otp_ver_adapter(AVB_GROUP, &version);
    if (ret)
        return AVB_IO_RESULT_ERROR_IO;

    if (rollback_index_location == RECOVERY_ROLLBACK_INDEX) {
        ret = sec_get_avb_otp_ver_adapter(RECOVERY_GROUP, &version);
        if (ret)
            return AVB_IO_RESULT_ERROR_IO;
    }

    *out_rollback_index = (uint64_t)version;

    return ret;
}

static AvbIOResult avb_hal_write_rollback_index(AvbOps *ops,
        size_t rollback_index_location,
        uint64_t rollback_index)
{
    AvbIOResult ret = AVB_IO_RESULT_OK;

    /* otp update process is not implemented here. */

    return ret;
}

static AvbIOResult avb_hal_read_is_device_unlocked(AvbOps *ops, bool *out_is_unlocked)
{
    AvbIOResult ret = AVB_IO_RESULT_OK;
    int lock_state = DEVICE_STATE_LOCKED;

    if (out_is_unlocked == NULL)
        return AVB_IO_RESULT_ERROR_IO;

    *out_is_unlocked = false;
    ret = sec_query_device_lock_adapter(&lock_state);
    if (ret != AVB_IO_RESULT_OK)
        return AVB_IO_RESULT_ERROR_IO;

    if (lock_state == DEVICE_STATE_UNLOCKED)
        *out_is_unlocked = true;

    return AVB_IO_RESULT_OK;
}

static AvbIOResult avb_hal_get_unique_guid_for_partition(AvbOps *ops,
        const char *partition,
        char *guid_buf,
        size_t guid_buf_size)
{
    char *uuid = NULL;

    if (partition == NULL || guid_buf == NULL)
        return AVB_IO_RESULT_ERROR_IO;

    if (guid_buf_size < UNIQUE_PARTITION_GUID_SIZE)
        return AVB_IO_RESULT_ERROR_IO;

    if (partition_get_uuid_by_name(partition, &uuid) != NO_ERROR)
        return AVB_IO_RESULT_ERROR_NO_SUCH_PARTITION;

    if (uuid == NULL)
        return AVB_IO_RESULT_ERROR_IO;

    /* Clear GUID Buffer To Avoid Random Value */
    assert(guid_buf_size == PART_META_INFO_UUIDLEN);
    memset(guid_buf, 0, guid_buf_size);
    memcpy(guid_buf, uuid, PART_META_INFO_UUIDLEN);

    return AVB_IO_RESULT_OK;
}

AvbIOResult avb_hal_get_size_of_partition(AvbOps *ops,
        const char *partition,
        uint64_t *out_size_num_bytes)
{
    int64_t total_size;

    if (partition == NULL || out_size_num_bytes == NULL)
        return AVB_IO_RESULT_ERROR_IO;

    total_size = get_total_size(partition);
    if (total_size < 0)
        return AVB_IO_RESULT_ERROR_NO_SUCH_PARTITION;

    *out_size_num_bytes = (uint64_t)total_size;

    return AVB_IO_RESULT_OK;
}

AvbIOResult avb_hal_read_persistent_value(AvbOps *ops,
                                          const char *name,
                                          size_t buffer_sz,
                                          uint8_t *out_buffer,
                                          size_t *out_num_bytes_read)
{
    AvbIOResult ret = AVB_IO_RESULT_OK;
    uint32_t persist_des_idx = 0;
    uint32_t find = 0;
    struct PersistDescriptor *g_persist_des = 0;

    if (name == NULL || out_buffer == NULL || out_num_bytes_read == NULL) {
        ret = AVB_IO_RESULT_ERROR_IO;
        goto end;
    }

    if (!get_init_persist_status()) {
        ret = AVB_IO_RESULT_ERROR_IO;
        goto end;
    }

    g_persist_des = get_persist_desc();
    if (!g_persist_des) {
        ret = AVB_IO_RESULT_ERROR_IO;
        goto end;
    }

    while (persist_des_idx < MAX_PERSIST_TAG_SZ &&
            g_persist_des[persist_des_idx].hdr.tag == PERSIST_TAG_MAGIC) {
        if (g_persist_des[persist_des_idx].hdr.key_sz == strlen(name)) {
            if (!avb_memcmp(g_persist_des[persist_des_idx].key, name, strlen(name))) {
                if (g_persist_des[persist_des_idx].hdr.value_sz > buffer_sz) {
                    ret = AVB_IO_RESULT_ERROR_INSUFFICIENT_SPACE;
                    goto end;
                }

                if (g_persist_des[persist_des_idx].value == NULL) {
                    ret = AVB_IO_RESULT_ERROR_IO;
                    goto end;
                }

                avb_memcpy(out_buffer,
                           g_persist_des[persist_des_idx].value,
                           g_persist_des[persist_des_idx].hdr.value_sz);
                *out_num_bytes_read = g_persist_des[persist_des_idx].hdr.value_sz;
                find = 1;
                break;
            }
        }

        persist_des_idx++;
    }

    if (find == 0) {
        ret = AVB_IO_RESULT_ERROR_NO_SUCH_VALUE;
        goto end;
    }

end:
    return ret;
}

AvbIOResult avb_hal_write_persistent_value(AvbOps *ops,
                                           const char *name,
                                           size_t value_sz,
                                           const uint8_t *value)
{
    AvbIOResult ret = AVB_IO_RESULT_OK;
    uint32_t persist_des_idx = 0;
    uint32_t empty_tag_idx = MAX_PERSIST_TAG_SZ;
    uint32_t find = 0;
    uint8_t *new_buf = NULL;
    struct PersistDescriptor *g_persist_des = 0;

    if (name == NULL || value == NULL) {
        ret = AVB_IO_RESULT_ERROR_IO;
        goto end;
    }

    if (!get_init_persist_status()) {
        ret = AVB_IO_RESULT_ERROR_IO;
        goto end;
    }

    g_persist_des = get_persist_desc();
    if (!g_persist_des) {
        ret = AVB_IO_RESULT_ERROR_IO;
        goto end;
    }

    while (persist_des_idx < MAX_PERSIST_TAG_SZ) {
        if (g_persist_des[persist_des_idx].hdr.tag == PERSIST_TAG_MAGIC &&
            g_persist_des[persist_des_idx].hdr.key_sz == strlen(name)) {

            if (!avb_memcmp(g_persist_des[persist_des_idx].key, name, strlen(name))) {
                if (g_persist_des[persist_des_idx].hdr.value_sz >= value_sz) {
                    if (value_sz == 0) {
                        /* delete this item */
                        g_persist_des[persist_des_idx].hdr.tag = 0x0;
                    }
                    avb_memset(g_persist_des[persist_des_idx].value,
                               0x0,
                               g_persist_des[persist_des_idx].hdr.value_sz);

                    if (g_persist_des[persist_des_idx].value == NULL) {
                        ret = AVB_IO_RESULT_ERROR_IO;
                        goto end;
                    }

                    avb_memcpy(g_persist_des[persist_des_idx].value, value, value_sz);
                    g_persist_des[persist_des_idx].hdr.value_sz = value_sz;

                } else {
                    new_buf = avb_malloc(value_sz);
                    if (new_buf == NULL) {
                        ret = AVB_IO_RESULT_ERROR_OOM;
                        goto end;
                    }
                    avb_memcpy(new_buf, value, value_sz);
                    g_persist_des[persist_des_idx].value = new_buf;
                }
                find = 1;
                break;
            }

        } else if (g_persist_des[persist_des_idx].hdr.tag != PERSIST_TAG_MAGIC &&
                   empty_tag_idx == MAX_PERSIST_TAG_SZ) {
            empty_tag_idx = persist_des_idx;
        }

        persist_des_idx++;
    }

    /* create a new item */
    if (find == 0) {
        if (empty_tag_idx >= MAX_PERSIST_TAG_SZ) {
            ret = AVB_IO_RESULT_ERROR_OOM;
            goto end;
        }
        dprintf(CRITICAL, "[AVB] create a new persist item...\n");
        g_persist_des[empty_tag_idx].hdr.tag = PERSIST_TAG_MAGIC;
        g_persist_des[empty_tag_idx].hdr.key_sz = strlen(name);
        g_persist_des[empty_tag_idx].hdr.value_sz = value_sz;
        new_buf = avb_malloc(strlen(name));
        if (new_buf == NULL) {
            ret = AVB_IO_RESULT_ERROR_OOM;
            goto end;
        }
        avb_memcpy(new_buf, name, strlen(name));
        g_persist_des[empty_tag_idx].key = new_buf;

        new_buf = avb_malloc(value_sz);
        if (new_buf == NULL) {
            ret = AVB_IO_RESULT_ERROR_OOM;
            goto end;
        }
        avb_memcpy(new_buf, value, value_sz);
        g_persist_des[empty_tag_idx].value = new_buf;
    }

    set_update_persist_status();

end:
    return ret;
}

AvbIOResult avb_hal_validate_public_key_for_partition(AvbOps *ops,
                                                      const char *partition,
                                                      const uint8_t *public_key_data,
                                                      size_t public_key_length,
                                                      const uint8_t *public_key_metadata,
                                                      size_t public_key_metadata_length,
                                                      bool *out_is_trusted,
                                                      uint32_t *out_rollback_index_location)
{
    AvbRSAPublicKeyHeader *key_hdr;
    uint8_t *pubk;
    uint32_t i = 0;
    uint32_t pubk_sz = 0;
    AvbIOResult ret = AVB_IO_RESULT_OK;

    if (ops == NULL) {
        return AVB_IO_RESULT_ERROR_IO;
    }

    if (partition == NULL) {
        return AVB_IO_RESULT_ERROR_IO;
    }

    if (out_is_trusted == NULL) {
        return AVB_IO_RESULT_ERROR_IO;
    }

    if (public_key_data == NULL) {
        return AVB_IO_RESULT_ERROR_IO;
    }

    if (out_rollback_index_location == NULL) {
        return AVB_IO_RESULT_ERROR_IO;
    }

    /* We only support recovery partition now. */
    if (0 != memcmp((void *)partition, (void *)"recovery", strlen("recovery")))
        return AVB_IO_RESULT_ERROR_NO_SUCH_PARTITION;

    /* We only support recovery partition now and this value
     * must be the same as the value set in device/mediatek/common/
     * device.mk for recovery. */
    *out_rollback_index_location = RECOVERY_ROLLBACK_INDEX;

    key_hdr = (AvbRSAPublicKeyHeader *)public_key_data;
    pubk = (uint8_t *)(public_key_data + sizeof(AvbRSAPublicKeyHeader));
    pubk_sz = avb_htobe32(key_hdr->key_num_bits) / 8;

    *out_is_trusted = false;
    if (pubk_sz != AVB_PUBK_SZ) {
        dprintf(CRITICAL, "[AVB] %s: invalid pubk size\n", partition);
        goto end;
    }

    if (0 == memcmp((void *)g_avb_recovery_key, (void *)pubk, pubk_sz)) {
        *out_is_trusted = true;
    } else {
        dprintf(CRITICAL, "[AVB] recovery pubk hash in lk partition =\n");
        for (i = 0; i < AVB_PUBK_SZ; i++) {
            dprintf(CRITICAL, "0x%x ", g_avb_recovery_key[i]);
            if (((i + 1) % 16) == 0)
                dprintf(CRITICAL, "\n");
        }
        dprintf(CRITICAL, "[AVB] pubk hash in %s partition =\n", partition);
        for (i = 0; i < AVB_PUBK_SZ; i++) {
            dprintf(CRITICAL, "0x%x ", pubk[i]);
            if (((i + 1) % 16) == 0)
                dprintf(CRITICAL, "\n");
        }
    }

end:
    return ret;

}

AvbOps ops = {
    .user_data = NULL,
    .ab_ops = NULL,
    .atx_ops = NULL,
    .read_from_partition = avb_hal_read_from_partition,
    .get_preloaded_partition = NULL,
    .write_to_partition = avb_hal_write_to_partition,
    .validate_vbmeta_public_key = avb_hal_verify_public_key,
    .validate_public_key_for_partition = avb_hal_validate_public_key_for_partition,
    .read_rollback_index = avb_hal_read_rollback_index,
    .write_rollback_index = avb_hal_write_rollback_index,
    .read_is_device_unlocked = avb_hal_read_is_device_unlocked,
    .get_unique_guid_for_partition = avb_hal_get_unique_guid_for_partition,
    .get_size_of_partition = avb_hal_get_size_of_partition,
    .read_persistent_value = avb_hal_read_persistent_value,
    .write_persistent_value = avb_hal_write_persistent_value
};

AvbOps *avb_hal_get_operations(void)
{
    return &ops;
}
