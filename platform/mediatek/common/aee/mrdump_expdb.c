/*
 * Copyright (c) 2019 MediaTek Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <errno.h>
#include <lk/init.h>
#include <lib/bio.h>
#include <lib/pl_boottags.h>
#include <malloc.h>
#include <platform.h>
#include <platform_halt.h>
#include <platform_mtk.h>
#include <platform/mboot_params.h>
#include <platform/mrdump_expdb.h>
#include <platform/wdt.h>
#include <string.h>
#include <trace.h>

#define LOCAL_TRACE 0

#define BOOT_TAG_AEE_EXP_INFO 0x8861001D

struct boot_tag_aee_expception_info {
    u8 exp_type;
    u32 wdt_status;
};
static struct boot_tag_aee_expception_info aee_exp_info;

int mrdump_get_env(const char *name, char *out, int out_len)
{
    bdev_t *bdev = NULL;
    struct mrdump_expdb_config *mrdump_config;

    mrdump_config = calloc(1, sizeof(struct mrdump_expdb_config));
    if (mrdump_config == NULL) {
        LTRACEF_LEVEL(ALWAYS, "%s malloc for mrdump_config fail\n", __func__);
        return -ENOMEM;
    }

    bdev = bio_open_by_label(AEE_IPANIC_LABEL);
    if (bdev == NULL) {
        LTRACEF_LEVEL(ALWAYS, "%s partition does not exist\n",
                      AEE_IPANIC_LABEL);
        free(mrdump_config);
        return -ENODEV;
    } else {
        off_t part_total_size = bdev->total_size;
        off_t offset;
        int32_t bio_out_len;

        offset = part_total_size - MRDUMP_CONFIG_EXPDB_OFFSET;
        bio_out_len = bio_read(bdev, mrdump_config, offset, sizeof(struct mrdump_expdb_config));
        if (!bio_out_len || bio_out_len != sizeof(struct mrdump_expdb_config)) {
            LTRACEF_LEVEL(ALWAYS, "%s: bio_read fail\n", __func__);
            bio_close(bdev);
            free(mrdump_config);
            return -EIO;
        }

        if (mrdump_config->sig != MRDUMP_CONFIG_SIG) {
            strlcpy(out, "", out_len);
            LTRACEF_LEVEL(ALWAYS, "unknown mrdump config signature\n");
            bio_close(bdev);
            free(mrdump_config);
            return -1;
        }

        if (!strncmp(name, MRDUMP_OUTPUT, sizeof(MRDUMP_OUTPUT))) {
            strlcpy(out, mrdump_config->output_dev, out_len);
        } else if (!strncmp(name, MRDUMP_ALLOCATE_SIZE, 20)) {
            strlcpy(out, mrdump_config->allocate_size, out_len);
        } else if (!strncmp(name, MRDUMP_MEM_SIZE, sizeof(MRDUMP_MEM_SIZE))) {
            strlcpy(out, mrdump_config->mem_size, out_len);
        } else if (!strncmp(name, MRDUMP_LBAOOO, sizeof(MRDUMP_LBAOOO))) {
            strlcpy(out, mrdump_config->lbaooo, out_len);
        } else if (!strncmp(name, MRDUMP_DUMP_TYPE, sizeof(MRDUMP_DUMP_TYPE))) {
            strlcpy(out, mrdump_config->dump_type, out_len);
        } else if (!strncmp(name, MRDUMP_FB_TIMEOUT, sizeof(MRDUMP_FB_TIMEOUT))) {
            strlcpy(out, mrdump_config->fb_timeout, out_len);
        } else {
            LTRACEF_LEVEL(ALWAYS, "unknown mrdump config\n");
            bio_close(bdev);
            free(mrdump_config);
            return -1;
        }
    }
    LTRACEF_LEVEL(ALWAYS, "%s: %s\n", name, out);
    bio_close(bdev);
    free(mrdump_config);
    return 0;
}

int mrdump_set_env(const char *name, const char *value)
{
    bdev_t *bdev = NULL;
    struct mrdump_expdb_config *mrdump_config;
    int32_t out_len;
    off_t part_total_size;
    off_t offset;

    mrdump_config = malloc(sizeof(struct mrdump_expdb_config));
    if (mrdump_config == NULL) {
        LTRACEF_LEVEL(ALWAYS, "%s malloc for mrdump_config fail\n", __func__);
        return -ENOMEM;
    }

    memset(mrdump_config, 0, sizeof(struct mrdump_expdb_config));
    bdev = bio_open_by_label(AEE_IPANIC_LABEL);
    if (bdev == NULL) {
        LTRACEF_LEVEL(ALWAYS, "%s partition does not exist\n", AEE_IPANIC_LABEL);
        free(mrdump_config);
        return -ENODEV;
    }

    part_total_size = bdev->total_size;
    offset = part_total_size - MRDUMP_CONFIG_EXPDB_OFFSET;
    out_len = bio_read(bdev, mrdump_config, offset, sizeof(struct mrdump_expdb_config));
    if (!out_len || out_len != sizeof(struct mrdump_expdb_config)) {
        LTRACEF_LEVEL(ALWAYS, "%s: bio_read fail\n", __func__);
        bio_close(bdev);
        free(mrdump_config);
        return -EIO;
    }

    if (mrdump_config->sig != MRDUMP_CONFIG_SIG) {
        memset(mrdump_config, 0, sizeof(struct mrdump_expdb_config));
        mrdump_config->sig = MRDUMP_CONFIG_SIG;
    }

    if (!strncmp(name, MRDUMP_OUTPUT, sizeof(MRDUMP_OUTPUT))) {
        strlcpy(mrdump_config->output_dev, value, sizeof(mrdump_config->output_dev));
    } else if (!strncmp(name, MRDUMP_ALLOCATE_SIZE, sizeof(MRDUMP_ALLOCATE_SIZE))) {
        strlcpy(mrdump_config->allocate_size, value, sizeof(mrdump_config->allocate_size));
    } else if (!strncmp(name, MRDUMP_MEM_SIZE, sizeof(MRDUMP_MEM_SIZE))) {
        strlcpy(mrdump_config->mem_size, value, sizeof(mrdump_config->mem_size));
    } else if (!strncmp(name, MRDUMP_LBAOOO, sizeof(MRDUMP_LBAOOO))) {
        strlcpy(mrdump_config->lbaooo, value, sizeof(mrdump_config->lbaooo));
    } else if (!strncmp(name, MRDUMP_DUMP_TYPE, sizeof(MRDUMP_DUMP_TYPE))) {
        strlcpy(mrdump_config->dump_type, value, sizeof(mrdump_config->dump_type));
    } else if (!strncmp(name, MRDUMP_FB_TIMEOUT, sizeof(MRDUMP_FB_TIMEOUT))) {
        strlcpy(mrdump_config->fb_timeout, value, sizeof(mrdump_config->fb_timeout));
    } else {
        LTRACEF_LEVEL(ALWAYS, "unknown mrdump config\n");
        bio_close(bdev);
        free(mrdump_config);
        return -1;
    }
    LTRACEF_LEVEL(ALWAYS, "%s update to %s\n", name, value);

    out_len = bio_write(bdev, mrdump_config, offset, sizeof(struct mrdump_expdb_config));
    if (out_len < 0 || out_len != sizeof(struct mrdump_expdb_config)) {
        LTRACEF_LEVEL(1, "%s: bio_write fail\n", __func__);
        bio_close(bdev);
        free(mrdump_config);
        return -EIO;
    }
    bio_close(bdev);
    free(mrdump_config);
    return 0;
}

unsigned char aee_get_exp_type(void)
{
    return aee_exp_info.exp_type;
}

unsigned int aee_get_wdt_status(void)
{
    return aee_exp_info.wdt_status;
}

static status_t reboot_reason_shutdown_callback(void *data,
    platform_halt_action suggested_action, platform_halt_reason reason)
{
    /* save reboot reason to RGU bit 4-11 */
    if (reason == HALT_REASON_SW_PANIC &&
        mtk_wdt_get_aee_rsv() == 0) {
#if LK_AS_BL33
        mtk_wdt_set_aee_rsv(AEE_EXP_TYPE_BL33_CRASH);
#elif LK_AS_BL2_EXT
        mtk_wdt_set_aee_rsv(AEE_EXP_TYPE_BL2_EXT_CRASH);
#elif LK_AS_AEE
        mtk_wdt_set_aee_rsv(AEE_EXP_TYPE_AEE_LK_CRASH);
#endif
    }
    return 0;
}


static void aee_exp_info_boottag_hook(struct boot_tag *tag)
{
     memcpy(&aee_exp_info, &tag->data, sizeof(aee_exp_info));

}
PL_BOOTTAGS_INIT_HOOK(aee_exp_info_tag, BOOT_TAG_AEE_EXP_INFO,
                                    aee_exp_info_boottag_hook);

static void register_reboot_reason(unsigned int level)
{
    register_platform_halt_callback("reboot_reason_shutdown_callback",
                                    &reboot_reason_shutdown_callback, NULL);
}

LK_INIT_HOOK(register_reboot_reason, &register_reboot_reason, LK_INIT_LEVEL_ARCH - 1);
