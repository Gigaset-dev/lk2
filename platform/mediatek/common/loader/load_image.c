/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <assert.h>
#include <debug.h>
#include <dt_table.h>
#include <errno.h>
#include <lib/bio.h>
#include <libfdt.h>
#include <load_image.h>
#include <malloc.h>
#include <mkimg.h>
#include <platform.h>
#include <platform_mtk.h>
#include <sboot.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

__WEAK void revise_part_name(const char *part_name, char *revised_part_name,
       int revised_part_name_size)
{
    int err;

    err = snprintf(revised_part_name, revised_part_name_size - 1, "%s", part_name);
    if (err < 0) {
        dprintf(CRITICAL, "Fail to revise part name (%d)\n", err);
    }
}

static int load_mkimg_part_info(bdev_t *bdev, const char *img_name,
                        union mkimg_hdr *p_hdr, off_t *img_start_addr)
{
    int len = 0;
    off_t offset = 0;

    /* read partition header */
    while (offset < bdev->total_size) {
        /* check partition magic */
        if (p_hdr->info.magic != MKIMG_MAGIC) {
            dprintf(CRITICAL, "partition magic error\n");
            return -EINVAL;
        }

        p_hdr->info.name[MKIMG_NAME_SZ - 1] = '\0'; //append end char
        dprintf(INFO, "=========================================\n");
        dprintf(INFO, "magic number : 0x%x\n", p_hdr->info.magic);
        dprintf(INFO, "name         : %s\n", p_hdr->info.name);
        dprintf(INFO, "size         : %d\n", p_hdr->info.dsz);
        dprintf(INFO, "=========================================\n");

        /* check image name */
        if (!strncasecmp(p_hdr->info.name, img_name, sizeof(p_hdr->info.name))) {
            dprintf(CRITICAL, "%s image name found\n", img_name);
            *img_start_addr = offset + sizeof(union mkimg_hdr);
            return sizeof(union mkimg_hdr);
        }

        /* find next image */
        offset += sizeof(union mkimg_hdr) + ROUNDUP(p_hdr->info.dsz, p_hdr->info.align_sz);

        len = bio_read(bdev, p_hdr, offset, sizeof(union mkimg_hdr));
        if (len < (ssize_t)sizeof(union mkimg_hdr)) {
            dprintf(CRITICAL, "failed to read mkimg hdr @%llu (%d)\n", offset, len);
            return -EIO;
        }
    }
    return -EINVAL;
}

int load_partition(const char *part_name, const char *img_name, void *buffer, size_t buffer_len)
{
    #define MAX_GPT_NAME_SIZE 72
    int err = 0;
    int len = 0;
    lk_time_t begin;
    bdev_t *bdev = NULL;
    off_t start_addr = 0;
    off_t part_offset = 0;
    union mkimg_hdr *p_hdr;
    size_t total_len;
    uint32_t header_size = 0;
    bool img_auth_required = true;
    char revised_part_name[MAX_GPT_NAME_SIZE];

    begin = current_time();

    p_hdr = (union mkimg_hdr *)memalign(CACHE_LINE, sizeof(union mkimg_hdr));
    if (!p_hdr)
        return -ENOMEM;

    revise_part_name(part_name, revised_part_name, MAX_GPT_NAME_SIZE);
    bdev = bio_open_by_label(revised_part_name);
    if (!bdev) {
        dprintf(CRITICAL, "Partition [%s], [%s] is not exist.\n", part_name, revised_part_name);
        err = -ENODEV;
        goto exit;
    }

    len = bio_read(bdev, (void *)p_hdr, 0, sizeof(union mkimg_hdr));
    if (len < (ssize_t)sizeof(union mkimg_hdr)) {
        dprintf(CRITICAL, "failed to read hdr (%d)\n", len);
        err = -EIO;
        goto exit;
    }

    if (fdt_magic(p_hdr) == DT_TABLE_MAGIC) {
        total_len = fdt_totalsize(p_hdr);
        dprintf(CRITICAL, "DT hdr detected (%zu)\n", total_len);
        header_size = sizeof(struct dt_table_header);
    } else if (p_hdr->info.magic == MKIMG_MAGIC) {
        err = load_mkimg_part_info(bdev, img_name, p_hdr, &start_addr);
        if (err < 0)
            goto exit;
        total_len = p_hdr->info.dsz;
        dprintf(CRITICAL, "mkimg hdr detected (%zu)\n", total_len);
        header_size = sizeof(union mkimg_hdr);
        part_offset = start_addr - header_size;
    } else {
        dprintf(CRITICAL, "non of known hdr detected\n");
        err = -EINVAL;
        goto exit;
    }

    if ((buffer_len > 0) && (buffer_len < total_len)) {
        dprintf(CRITICAL, "fatal: buffer_len < total_len\n");
        ASSERT(0);
    }

    img_auth_required = sec_get_vfy_policy_adapter(revised_part_name);
    dprintf(CRITICAL, "img_auth_required = %d\n", img_auth_required);

    if (img_auth_required) {
        err = sec_cert_parser_and_vfy_adapter(revised_part_name,
                                          img_name,
                                          (uint8_t *) p_hdr,
                                          header_size,
                                          part_offset,
                                          0x0);
        if (err != 0) {
            dprintf(CRITICAL, "[SEC] %s Cert Chain Verify Fail (0x%x)\n", img_name, err);
            ASSERT(0);
        }

        err = sec_rollback_check_adapter(img_name);
        if (err != 0) {
            dprintf(CRITICAL, "[SEC] ARB Check Fail (0x%x)\n", err);
            ASSERT(0);
        }

        err = sec_hdr_auth_adapter((uint8_t *)p_hdr, header_size);
        if (err != 0) {
            dprintf(CRITICAL, "[SEC] %s Header Auth Fail (0x%x)\n", img_name, err);
            ASSERT(0);
        }
    }

    len = bio_read(bdev, buffer, start_addr, total_len);
    if (len < (ssize_t)total_len) {
        dprintf(CRITICAL, "failed to read @%llu (%d)\n", start_addr, len);
        err = -EIO;
        goto exit;
    }

    if (img_auth_required) {
        // dtbo got 2 layers of headers (dt_table and fdt headers), so
        // we should get rid of the outer one before auth
        if (fdt_magic(p_hdr) == DT_TABLE_MAGIC) {
            buffer += header_size;
            total_len -= header_size;
        }

        err = sec_img_auth_adapter(buffer, total_len);
        if (err != 0) {
            dprintf(CRITICAL, "[SEC] %s Image Auth Fail (0x%x)\n", img_name, err);
            ASSERT(0);
        }
    }

    dprintf(CRITICAL,
            "Load '%s' partition to 0x%p (%d bytes in %d ms)\n",
            revised_part_name,
            buffer,
            len,
            current_time() - begin);

exit:

    if (bdev)
        bio_close(bdev);

    if (p_hdr)
        free(p_hdr);

    if (err < 0)
        return err;
    return len;
}

