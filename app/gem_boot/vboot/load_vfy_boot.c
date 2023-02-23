/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <app/boot_info.h>
#include <app/load_vfy_boot.h>
#include <assert.h>
#include <bootimg.h>
#include <errno.h>
#include <lib/bio.h>
#include <lib/kcmdline.h>
#include <malloc.h>
#include <platform_mtk.h>
#include <sboot.h>
#include <string.h>
#include <verified_boot_error.h>

#define CMDLINE_ROOT_RAM    ""

int load_vfy_boot(uint32_t bootimg_type)
{
    #define MAX_GPT_NAME_SIZE (73)
    char part_name[16] = {0};
    vaddr_t boot_load_addr = 0;
    bdev_t *bdev = NULL;
    int err = 0;
    int len = 0;
    struct bootimg_hdr *header = NULL;
    uint32_t n = 0, m = 0, o = 0, p = 0, q = 0;
    uint32_t bootimg_size = 0;
    bool img_auth_required = true;
    const char *suffix = get_suffix();
    char revised_part_name[MAX_GPT_NAME_SIZE] = {0};

    strncpy(part_name, "boot", sizeof(part_name) - 1);
    snprintf(revised_part_name, sizeof(revised_part_name) - 1, "%s%s", part_name, suffix);

    bdev = bio_open_by_label(revised_part_name);
    if (!bdev) {
        /* fallback partition name without A/B suffix */
        if (strlen(suffix) > 0) {
            memset(revised_part_name, 0, sizeof(revised_part_name));
            strncpy(revised_part_name, part_name, strlen(part_name));
            bdev = bio_open_by_label(revised_part_name);
        }

        if (!bdev) {
            dprintf(CRITICAL, "fail to open %s\n", revised_part_name);
            err = ERR_VB_INVALID_DEVICE;
            goto exit;
        }
    }

    img_auth_required = get_vfy_policy_adapter(revised_part_name);
    dprintf(CRITICAL, "img_auth_required = %d\n", img_auth_required);
    header = (struct bootimg_hdr *)calloc(1, sizeof(struct bootimg_hdr));
    if (!header) {
        dprintf(CRITICAL, "fail to malloc header\n");
        return ERR_VB_BUF_ADDR_INVALID;
    }

    bdev = bio_open_by_label(revised_part_name);
    if (!bdev) {
        dprintf(CRITICAL, "fail to open %s\n", revised_part_name);
        err = ERR_VB_INVALID_DEVICE;
        goto exit;
    }

    len = bio_read(bdev, header, 0, sizeof(struct bootimg_hdr));
    if (len < (ssize_t)sizeof(struct bootimg_hdr)) {
        dprintf(CRITICAL, "fail to read bootimg header, size: 0x%x\n", len);
        err = ERR_VB_INVALID_IO;
        goto exit;
    }

    if (img_auth_required) {
        uint8_t *complete_header = malloc(header->page_sz);

        if (!complete_header) {
            dprintf(CRITICAL, "fail to malloc complete_header\n");
            err = ERR_VB_BUF_ADDR_INVALID;
            goto exit;
        }

        len = bio_read(bdev, complete_header, 0, header->page_sz);
        if (len < (ssize_t)header->page_sz) {
            dprintf(CRITICAL, "fail to read complete bootimg header, size: 0x%x\n", len);
            err = ERR_VB_INVALID_IO;
            free(complete_header);
            goto exit;
        }

        /* For boot image, image name is not used for image lookup.
         * Hence we use partition name here, which is not referenced
         * at all.
         */
        err = sec_cert_parser_and_vfy_adapter(revised_part_name,
                                              revised_part_name,
                                              (uint8_t *)header,
                                              sizeof(struct bootimg_hdr),
                                              0x0,
                                              0x0);
        if (err != 0) {
            dprintf(CRITICAL, "%s cert chain vfy fail. (0x%x)\n",
                    revised_part_name, err);
            free(complete_header);
        }

        ASSERT(err == 0);

        err = sec_rollback_check_adapter();
        if (err != 0)
            dprintf(CRITICAL, "[SEC] ARB Check Fail (0x%x)\n", err);
        ASSERT(err == 0);

        err = sec_hdr_auth_adapter((uint8_t *)complete_header, header->page_sz);
        if (err != 0) {
            dprintf(CRITICAL, "[SEC] Header Auth Fail (0x%x)\n", err);
            free(complete_header);
        }

        ASSERT(err == 0);

        if (complete_header)
            free(complete_header);
    }

    ASSERT(header->page_sz != 0);
    n = (header->kernel_sz + header->page_sz - 1) / header->page_sz;
    m = (header->ramdisk_sz + header->page_sz - 1) / header->page_sz;
    o = (header->second_sz + header->page_sz - 1) / header->page_sz;
    p = (header->recovery_dtbo_size + header->page_sz - 1) / header->page_sz;
    q = (header->dtb_size + header->page_sz - 1) / header->page_sz;
    bootimg_size = (n + m + o + p + q + 1) * header->page_sz;

    boot_load_addr = (vaddr_t)malloc(bootimg_size);
    if (!boot_load_addr) {
        dprintf(CRITICAL, "fail to malloc boot_load_addr\n");
        err = ERR_VB_BUF_ADDR_INVALID;
        goto exit;
    }

    len = bio_read(bdev, (void *)boot_load_addr, 0, bootimg_size);
    if (len < (ssize_t)bootimg_size) {
        dprintf(CRITICAL, "fail to read bootimg, size: 0x%x\n", len);
        err = ERR_VB_INVALID_IO;
        goto exit;
    }

    if (img_auth_required) {
        err = sec_img_auth_adapter(boot_load_addr + header->page_sz,
                                   bootimg_size - header->page_sz);
        if (err != 0)
            dprintf(CRITICAL, "[SEC] Image Auth Fail (0x%x)\n", err);
        ASSERT(err == 0);
    }
    load_bootinfo_bootimg_hdr((struct bootimg_hdr *)boot_load_addr);
    parse_boot_opt();
    kcmdline_append(CMDLINE_ROOT_RAM);
    set_bootimg_loaded(boot_load_addr);

exit:
    free(header);

    if (bdev)
        bio_close(bdev);

    /* Do not free boot_load_addr here. gem_boot will do the job. */

    return err;
}
