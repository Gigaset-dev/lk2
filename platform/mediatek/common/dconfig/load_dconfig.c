/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <dconfig.h>
#include <debug.h>
#include <errno.h>
#include <lib/bio.h>
#include <malloc.h>
#include <mkimg.h>
#include <platform.h>
#include <sboot.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <trace.h>
#include <verified_boot.h>

#include "dconfig_private.h"

#define LOCAL_TRACE 0

static int search_mkimg(bdev_t *bdev, const char *img_name,
                        union mkimg_hdr *p_hdr, off_t *img_start_addr)
{
    int len = 0;
    off_t offset = 0;

    len = bio_read(bdev, (void *)p_hdr, offset, sizeof(union mkimg_hdr));
    if (len < (ssize_t)sizeof(union mkimg_hdr)) {
        dprintf(CRITICAL, "failed to read hdr (%d)\n", len);
        return -EIO;
    }

    /* read partition header */
    while (offset < bdev->total_size) {
        /* check partition magic */
        if (p_hdr->info.magic != MKIMG_MAGIC) {
            dprintf(CRITICAL, "partition doesn't contain dconfig image\n");
            return -EINVAL;
        }

        p_hdr->info.name[MKIMG_NAME_SZ - 1] = '\0'; //append end char

        /* check image name */
        if (!strncasecmp(p_hdr->info.name, img_name, sizeof(p_hdr->info.name))) {
            LTRACEF_LEVEL(2, "%s image name found\n", img_name);
            *img_start_addr = offset + sizeof(union mkimg_hdr);
            return 0;
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

int load_dconfig_partition(const char *img_name, void *buffer, size_t buffer_size,
                           uint8_t socid[SOCID_LEN])
{
    int err = 0;
    int len = 0;
    lk_time_t begin;
    bdev_t *bdev = NULL;
    off_t start_addr = 0;
    off_t part_offset = 0;
    union mkimg_hdr *p_hdr;
    size_t total_len;
    uint32_t header_size = 0, socid_ret;
    bool img_auth_required;

    LTRACEF_LEVEL(2, "Load dconfig img (%s)\n", img_name);
    begin = current_time();

    p_hdr = (union mkimg_hdr *)malloc(sizeof(union mkimg_hdr));
    if (!p_hdr)
        return -ENOMEM;

    bdev = bio_open_by_label(DCONFIG_PART);
    if (!bdev) {
        dprintf(CRITICAL, "Partition [%s] is not exist.\n", DCONFIG_PART);
        err = -ENODEV;
        goto exit;
    }

    err = search_mkimg(bdev, img_name, p_hdr, &start_addr);
    if (err < 0)
        goto exit;
    total_len = p_hdr->info.dsz;
    header_size = sizeof(union mkimg_hdr);
    part_offset = start_addr - header_size;

    img_auth_required = sec_get_vfy_policy_adapter(DCONFIG_PART);
    LTRACEF_LEVEL(2, "img_auth_required = %d\n", img_auth_required);

    if (img_auth_required) {
        err = sec_cert_parser_and_vfy_adapter(DCONFIG_PART,
                                  img_name,
                                  (uint8_t *) p_hdr,
                                  header_size,
                                  part_offset,
                                  GET_SOCID_FROM_CERT2);
        if (err != 0) {
            dprintf(CRITICAL, "%s cert chain vfy fail. (0x%x)\n", img_name, err);
            err = -EIO;
            goto exit;
        }

        err = sec_hdr_auth_adapter((uint8_t *)p_hdr, header_size);
        if (err != 0) {
            dprintf(CRITICAL, "%s header auth fail. (0x%x)\n", img_name, err);
            err = -EIO;
            goto exit;
        }

        if (total_len > buffer_size) {
            err = -EINVAL;
            goto exit;
        }
    }

    len = bio_read(bdev, buffer, start_addr, total_len);
    if (len < (ssize_t)total_len) {
        dprintf(CRITICAL, "failed to read load_partition @%llu (%d)\n", start_addr, len);
        err = -EIO;
        goto exit;
    }

    if (img_auth_required) {
        err = sec_img_auth_adapter(buffer, total_len);
        if (err != 0) {
            dprintf(CRITICAL, "image auth fail. (0x%x)\n", err);
            err = -EIO;
            goto exit;
        }

        socid_ret = sec_get_socid_cert_adapter(socid, SOCID_LEN);
        if (socid_ret != 0) {
            dprintf(CRITICAL, "No cert socid found (0x%x)\n", socid_ret);
            memset(socid, 0, SOCID_LEN);
            err = -EIO;
            goto exit;
        }
    }

    LTRACEF_LEVEL(2, "Load '%s' (%d bytes in %d ms)\n", img_name, len,
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

