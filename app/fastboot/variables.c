/*
 * Copyright (c) 2009, Google Inc.
 * Copyright (c) 2021, MediaTek Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#include <debug.h>
#include <fastboot.h>
#include <lib/bio.h>
#include <lib/version.h>
#include <malloc.h>
#include <partition_utils.h>
#include <string.h>
#include <trace.h>

#define LOCAL_TRACE 0

__WEAK char *get_lk_version(void)
{
    static char buffer[64];
    size_t buflen = sizeof(buffer);
    int i;

    i = snprintf((char *)buffer, buflen, "%s_%s",
                     version.project, version.buildid);

    if (i > 0)
        LTRACEF("lk_version:%s\n", buffer);

    return (char *)buffer;
}

/* List major partitions for fastboot query.
 * Target platform can implement its own images list.
 */
__WEAK const char **get_fastboot_image_list(void)
{
    /* partition name without a/b suffix */
    static const char * const images[] = {
       "lk",
       "boot",
       "vendor_boot",
       "dtbo",
       "md1img",
       "super",
       "system",
       "vbmeta",
       "vbmeta_system",
       "vbmeta_vendor",
       "userdata",
       NULL
    };

    return (const char **)images;
}

/* Publish partition-type, partition-size, has-slot
 * 1. Check if the partition exist and if a/b slot
 * 2. Publish has-slot:<part_name>:<yes/no>
 * 3. Publish partition-size:<part_name>:<size>
 *    (if it is a/b slot, also publish for slot b.)
 * 4. Publish partition-type:<part_name>:<type>
      (if it is a/b slot, also publish for slot b.)
 */
void publish_partition_info(void)
{
    #define PART_NAME_LEN 64
    #define PART_TYPE_CNT 3  /* 3 patition types */
    #define PART_TYPE_LEN 5  /* raw, ext4, or f2fs */
    static char partition_type[PART_TYPE_CNT][PART_TYPE_LEN] = {"raw", "ext4", "f2fs"};
    char part_type[] = "partition-type:";
    char part_size[] = "partition-size:";
    char has_slot[]  = "has-slot:";

    bdev_t *dev = NULL;
    int n, is_ab, size_len, part_type_idx = 0;
    char *part_size_ptr, *part_type_ptr, *has_slot_ptr, *get_size;
    const char **name_list = get_fastboot_image_list();
    char part_name[PART_NAME_LEN] = {0};

    while (*name_list != NULL) {
        if (strlen(*name_list) == 0) {
            name_list++;
            continue;
        }

        /* add suffix _a for partition name */
        n = snprintf(part_name, PART_NAME_LEN, "%s_a", *name_list);
        if ((n < 0) || (n >= PART_NAME_LEN))
            LTRACEF("snprintf error.\n");
        size_len = strlen(part_name) + strlen(has_slot) + 1;
        has_slot_ptr = malloc(size_len);
        if (!has_slot_ptr)
            goto error;
        n = snprintf(has_slot_ptr, size_len, "%s%s", has_slot, *name_list);
        if ((n < 0) || (n >= size_len))
            LTRACEF("snprintf error.\n");

        /* 1. Check if the partition exist and if a/b slot */
        /* 2. Publish has-slot:<part_name>:<yes/no> */
        dev = bio_open_by_label(part_name);
        if (dev) {
            /* this partition has a/b slot */
            is_ab = 1;
            fastboot_publish((const char *) has_slot_ptr, "yes");
        } else {
            /* fallback to the partition name w/o suffix */
            n = snprintf(part_name, PART_NAME_LEN, "%s", *name_list);
            if ((n < 0) || (n >= PART_NAME_LEN))
                LTRACEF("snprintf error.\n");

            dev = bio_open_by_label(part_name);
            if (!dev) {
                /* partition not exists, check next one */
                name_list++;
                free(has_slot_ptr);
                continue;
            }
            /* this partition is not a/b slot */
            is_ab = 0;
            fastboot_publish((const char *) has_slot_ptr, "no");
        }
        free(has_slot_ptr);

        /* 3. Publish partition-size:<part_name>:<size> */
        /* generate patition-size:<part_name> */
        size_len = strlen(part_name) + strlen(part_size) + 1;
        part_size_ptr = malloc(size_len);
        if (!part_size_ptr)
            goto error;
        n = snprintf(part_size_ptr, size_len, "%s%s", part_size, part_name);
        if ((n < 0) || (n >= size_len))
            LTRACEF("snprintf error.\n");

        get_size = malloc(size_len);
        if (!get_size) {
            free(part_size_ptr);
            goto error;
        }
        n = snprintf(get_size, size_len, "%llx", dev->total_size);
        if ((n < 0) || (n >= size_len))
            LTRACEF("snprintf error.\n");
        else
            fastboot_publish((const char *) part_size_ptr, get_size);
        free(part_size_ptr);

        if (is_ab) {
            size_len = strlen(part_name) + strlen(part_size) + 1;
            part_size_ptr = malloc(size_len);
            if (!part_size_ptr) {
                free(get_size);
                goto error;
            }
            n = snprintf(part_size_ptr, size_len, "%s%s_b", part_size, *name_list);
            if ((n < 0) || (n >= size_len))
                LTRACEF("snprintf error.\n");
            else
                fastboot_publish((const char *) part_size_ptr, get_size);
            free(part_size_ptr);
        }
        free(get_size);

        /* 4. Publish partition-type:<part_name>:<type> */
        /* generate partition-type:<part_name> */
        size_len = strlen(part_name) + strlen(part_type) + 1;
        part_type_ptr = malloc(size_len);
        if (!part_type_ptr)
            goto error;
        n = snprintf(part_type_ptr, size_len, "%s%s", part_type, part_name);
        if ((n < 0) || (n >= size_len))
            LTRACEF("snprintf error.\n");

        partition_get_type(part_name, &part_type_idx);
        if (part_type_idx < PART_TYPE_CNT) {
            fastboot_publish((const char *) part_type_ptr, partition_type[part_type_idx]);
            free(part_type_ptr);
            part_type_ptr = NULL;
            if (is_ab) { /* publish slot b */
                size_len = strlen(part_name) + strlen(part_type) + 1;
                part_type_ptr = malloc(size_len);
                if (!part_type_ptr)
                    goto error;
                n = snprintf(part_type_ptr, size_len, "%s%s_b", part_type, *name_list);
                if ((n < 0) || (n >= size_len))
                    LTRACEF("snprintf error.\n");
                else
                    fastboot_publish((const char *) part_type_ptr, partition_type[part_type_idx]);
            }
        }
        if (part_type_ptr)
            free(part_type_ptr);

        name_list++;
    }
    LTRACEF("Publish partition done.\n");
    goto done;
error:
    dprintf(CRITICAL, "Publish partition error.\n");
done:
    if (dev)
        bio_close(dev);
}
