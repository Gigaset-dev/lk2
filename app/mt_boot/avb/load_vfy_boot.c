/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <app/boot_info.h>
#include <app/dm_verity_status.h>
#include <app/load_vfy_boot.h>
#include <app/vboot_state.h>
#include <assert.h>
#include <avb_slot_verify.h>
#include <avb_hal.h>
#include <avb_hal_cmdline.h>
#include <avb_hal_error.h>
#include <avb_ops.h>
#include <avb_persist.h>
#include <avb_user_verification.h>
#include <bootctrl.h>
#include <bootimg.h>
#include <err.h>
#include <errno.h>
#include <libfdt.h>
#include <lib/bio.h>
#include <lib/kcmdline.h>
#include <lock_state.h>
#include <malloc.h>
#include <platform/wdt.h>
#include <RoT.h>
#include <sboot.h>
#include <set_fdt.h>
#include <string.h>
#include <verified_boot.h>
#include <verified_boot_error.h>

#define CMDLINE_ROOT_RAM            "root=/dev/ram"
#define CMDLINE_ROOT_SYSTEM_COMMON  "skip_initramfs ro rootwait init=/init"

#define VENDOR_BOOT_NAME            "vendor_boot"

/* Support V4 Format */
static const char * const boot_partitions[]     = {"boot", NULL, NULL};
/* Support Both V4 And V2 Format */
static const char * const recovery_partitions[] = {"recovery", NULL, NULL};

/* Please keep avb_cmdline_attrs and avb_dt_attrs sync since we
 * use them for cmdline attrs to dt attrs conversion
 */
static const char * const avb_cmdline_attrs[] = {
    "androidboot.vbmeta.hash_alg",
    "androidboot.vbmeta.size",
    "androidboot.vbmeta.digest",
};

static const char * const avb_dt_attrs[] = {
    "vbmeta.hash_alg",
    "vbmeta.size",
    "vbmeta.digest",
};

static int32_t build_up_vendor_boot(const char **req_parts, uint32_t part_cnt)
{
    bdev_t *bdev;
    uint32_t i;
    int32_t ret = 0;
    const char *suffix = get_suffix();
    char *part_name;

    i = strlen(VENDOR_BOOT_NAME) + strlen(suffix) + 1;
    part_name = malloc(i);
    if (!part_name)
        ASSERT(0);

    if (snprintf(part_name, i, "%s%s", (char *)VENDOR_BOOT_NAME, suffix) > 0)
        part_name[i - 1] = '\0';

    bdev = bio_open_by_label(part_name);
    if (!bdev) {
        dprintf(CRITICAL, "partition vendor boot not exists\n");
        if (part_name)
            free(part_name);
        return ERR_VB_INVALID_DEVICE;
    }

    /* add vendor_boot in request partitions */
    for (i = 0 ; i < part_cnt ; i++) {
        if (req_parts[i] == NULL) {
            req_parts[i] = (char *)VENDOR_BOOT_NAME;
            dprintf(CRITICAL, "vendor_boot exists\n");
            break;
        }
    }
    if (i >= part_cnt) {
        dprintf(CRITICAL, "Not enough space to add vendor_boot to request partitions\n");
        ASSERT(0);
    }

    if (part_name)
        free(part_name);

    bio_close(bdev);
    return ret;
}

static uint32_t boot_vfy_policy(uint32_t bootimg_type)
{

    uint32_t img_auth_required = 1;

    switch (bootimg_type) {
    case BOOTIMG_TYPE_RECOVERY:
        img_auth_required = sec_get_vfy_policy_adapter(recovery_partitions[0]);
        break;
    case BOOTIMG_TYPE_BOOT:
        img_auth_required = sec_get_vfy_policy_adapter(boot_partitions[0]);
        break;
    default:
        dprintf(CRITICAL, "[AVB] invalid boot image type\n");
    }

    dprintf(CRITICAL, "[AVB] img_auth_required = %d\n", img_auth_required);

    return img_auth_required;
}

static uint32_t avb_cmdline_postprocessing(char *cmdline)
{
    uint32_t ret = 0;
    uint32_t i = 0;

    /* Extract avb parameters from cmdline */
    for (i = 0; i < countof(avb_cmdline_attrs); i++) {
        ret = avb_extract_from_cmdline(cmdline,
                                       strlen(cmdline),
                                       avb_cmdline_attrs[i]);
        if (ret) {
            dprintf(CRITICAL, "[AVB] Extract %s fails\n", avb_cmdline_attrs[i]);
            return ret;
        }
    }

    return ret;
}


static uint32_t boot_post_processing(AvbOps *ops, uint32_t bootimg_type,
                                     AvbSlotVerifyData *slot_data)
{
    uint32_t ret = 0;
    uint32_t i = 0;
    const char *suffix = NULL;
    bool out_verification_enabled = true;
    vaddr_t load_addr = 0, boot_load_addr = 0, vendorboot_load_addr = 0;
    const char *part_name = get_bootimg_partition_name(bootimg_type);

    if (part_name == NULL)
        return AVB_IO_RESULT_ERROR_IO;

    if (slot_data == NULL)
        return AVB_IO_RESULT_ERROR_IO;

    ret = collect_rot_info(slot_data);
    if (ret)
        return AVB_IO_RESULT_ERROR_IO;

    /* Set boot_load_addr and vendorboot_load_addr to slot data respectively */
    for (i = 0; i < slot_data->num_loaded_partitions; i++) {
        if (slot_data->loaded_partitions[i].partition_name) {
            load_addr = (vaddr_t)slot_data->loaded_partitions[i].data;
            if (!strcmp(part_name,
                    slot_data->loaded_partitions[i].partition_name)) {
                load_bootinfo_from_hdr((void *)load_addr);
                boot_load_addr = load_addr;
            } else if (!strcmp((char *)VENDOR_BOOT_NAME,
                    slot_data->loaded_partitions[i].partition_name)) {
                load_bootinfo_from_hdr((void *)load_addr);
                vendorboot_load_addr = load_addr;
            }
        }
    }

    if (!boot_load_addr)
        return AVB_SLOT_VERIFY_RESULT_ERROR_INVALID_ARGUMENT;

    kcmdline_append(CMDLINE_ROOT_RAM);

    suffix = get_suffix();
    if (suffix == NULL) {
        dprintf(CRITICAL, "[AVB] Fail to get suffix\n");
        ret = AVB_SLOT_VERIFY_RESULT_ERROR_INVALID_ARGUMENT;
        goto end;
    }
    if (!avb_user_verification_get(ops, suffix, &out_verification_enabled)) {
        dprintf(CRITICAL, "[AVB] Fail To Get Verification Disabled Bit\n");
        out_verification_enabled = true;
    }

    if (out_verification_enabled) {
        ret = avb_cmdline_postprocessing(slot_data->cmdline);
        if (ret)
            goto end;
    }

    dprintf(INFO, "[AVB] cmdline = %s\n", slot_data->cmdline);
    kcmdline_append(slot_data->cmdline);

    set_bootimg_loaded(boot_load_addr, vendorboot_load_addr);

    ret = write_persist_value(ops, PERSIST_PART_NAME);

end:
    return ret;
}

static void handle_vboot_state(uint32_t bootimg_type)
{
    print_boot_state();
    switch (bootimg_type) {
    case BOOTIMG_TYPE_RECOVERY:
        show_warning(recovery_partitions[0]);
        break;
    case BOOTIMG_TYPE_BOOT:
        show_warning(boot_partitions[0]);
        break;
    default:
        dprintf(CRITICAL, "[AVB] invalid boot image type\n");
    }
    set_boot_state_to_cmdline();
    set_rot_boot_state(get_boot_state());
}

int load_vfy_boot(uint32_t bootimg_type)
{
    int ret = NO_ERROR;
    AvbSlotVerifyResult avb_ret = AVB_SLOT_VERIFY_RESULT_OK;
    AvbSlotVerifyData *slot_data = NULL;
    AvbSlotVerifyFlags avb_flag = AVB_SLOT_VERIFY_FLAGS_NONE;
    AvbHashtreeErrorMode hashtree_error_mode = AVB_HASHTREE_ERROR_MODE_RESTART_AND_INVALIDATE;
    char * const*requested_partitions = NULL;
    uint32_t dm_status = 0;
    AvbOps *ops = 0;
    uint32_t lock_state = LKS_DEFAULT;

    ops = avb_hal_get_operations();
    if (!ops) {
        dprintf(CRITICAL, "[AVB] Bad AVB Operation Pointer\n");
        return ERR_VB_INVALID_ADDR;
    }

    /* Boot verify security policy */
    if (boot_vfy_policy(bootimg_type) == 0)
        avb_flag = AVB_SLOT_VERIFY_FLAGS_ALLOW_VERIFICATION_ERROR;

#if (MTK_SECURITY_SW_SUPPORT)
    if (avb_flag & AVB_SLOT_VERIFY_FLAGS_ALLOW_VERIFICATION_ERROR)
        set_boot_state(BOOT_STATE_ORANGE);
    else
        set_boot_state(BOOT_STATE_RED);
#else
    /* Disable AVB verification in two steps */
    /* Step-1: Mark verify off flag On VBMETA */
    if (avb_user_verification_set(ops, get_suffix(), 0) != true) {
        dprintf(CRITICAL, "[AVB] Fail to disable avb verification\n");
        ret = AVB_SLOT_VERIFY_RESULT_ERROR_INVALID_ARGUMENT;
        goto end;
    }

    /* Step-2: Allow error during AVB slot verify */
    avb_flag |= AVB_SLOT_VERIFY_FLAGS_ALLOW_VERIFICATION_ERROR;
    dprintf(CRITICAL, "[AVB] AVB verification is disabled\n");
#endif

    if (bootimg_type == BOOTIMG_TYPE_BOOT) {
        requested_partitions = malloc(sizeof(boot_partitions));
        if (requested_partitions) {
            memcpy((void *)requested_partitions, boot_partitions, sizeof(boot_partitions));
            build_up_vendor_boot((const char **)requested_partitions, countof(boot_partitions));
        }
    } else if (bootimg_type == BOOTIMG_TYPE_RECOVERY) {
        requested_partitions = malloc(sizeof(recovery_partitions));
        if (requested_partitions)
            memcpy((void *)requested_partitions, recovery_partitions, sizeof(recovery_partitions));

        /* Note recovery image signature is not contained in VBMETA */
        avb_flag |= AVB_SLOT_VERIFY_FLAGS_NO_VBMETA_PARTITION;
    }

    if (!requested_partitions) {
        avb_ret = AVB_SLOT_VERIFY_RESULT_ERROR_OOM;
        goto end;
    }

    get_dm_verity_status(&dm_status);
    if (dm_status)
        avb_flag |= AVB_SLOT_VERIFY_FLAGS_RESTART_CAUSED_BY_HASHTREE_CORRUPTION;
    get_hash_tree_error_mode((uint32_t *)&hashtree_error_mode);

    ret = init_persist_value(ops, PERSIST_PART_NAME, PERSIST_VALUE_OFFSET);
    if (ret) {
        dprintf(CRITICAL, "init_persist_value ret = 0x%x\n", ret);
        goto end;
    }

    switch (bootimg_type) {
    case BOOTIMG_TYPE_BOOT:
    case BOOTIMG_TYPE_RECOVERY:
        avb_ret = avb_slot_verify(ops,
                                  (const char *const *)requested_partitions,
                                  get_suffix(),
                                  avb_flag,
                                  hashtree_error_mode,
                                  &slot_data);

        if (avb_flag & AVB_SLOT_VERIFY_FLAGS_ALLOW_VERIFICATION_ERROR)
            avb_ret = AVB_SLOT_VERIFY_RESULT_OK;

        if (avb_ret != AVB_SLOT_VERIFY_RESULT_OK)
            goto end;

        break;
    default:
        avb_ret = AVB_SLOT_VERIFY_RESULT_ERROR_INVALID_ARGUMENT;
        goto end;
    }

    if (avb_ret == AVB_SLOT_VERIFY_RESULT_OK) {
        ret = boot_post_processing(ops, bootimg_type, slot_data);
        if (ret)
            goto end;

#if (MTK_SECURITY_SW_SUPPORT)
        if (!(avb_flag & AVB_SLOT_VERIFY_FLAGS_ALLOW_VERIFICATION_ERROR))
            set_boot_state(BOOT_STATE_GREEN);
#endif
    }

end:
    if (requested_partitions)
        free((void *)requested_partitions);

    if (slot_data != NULL)
        hashtree_error_mode = slot_data->resolved_hashtree_error_mode;

    dprintf(CRITICAL, "[AVB] avb_ret = %d\n", avb_ret);

    /* If device state is "unlocked", override
     * boot state with "ORANGE".
     */
#if (MTK_SECURITY_SW_SUPPORT)
    ret = sec_get_lock_state_adapter(&lock_state);
    if (ret == 0 && lock_state == LKS_UNLOCK)
        set_boot_state(BOOT_STATE_ORANGE);
#endif

    handle_vboot_state(bootimg_type);
    send_root_of_trust_info();
    dm_verity_handler((uint32_t)hashtree_error_mode);

    return ret;
}

void set_fdt_avb_cmdline(void *fdt)
{
#if (MTK_SECURITY_SW_SUPPORT)
    int nodeoffset;
    char fdt_value[MAX_VALUE_SZ + 1] = {0};
    int fdt_ret = 0;
    int i = 0;

    /* Create /firmware/android if it does not exist  */
    nodeoffset = fdt_path_offset(fdt, "/firmware/android");
    if (nodeoffset < 0) {
        nodeoffset = fdt_path_offset(fdt, "/firmware");
        if (nodeoffset < 0) {
            /* Add subnode "firmware" in root */
            nodeoffset = fdt_add_subnode(fdt, 0, "firmware");
            if (nodeoffset < 0) {
                dprintf(INFO, "Warning: can't add firmware node in device tree\n");
                return;
            }
        }

        /* Add subnode "android" in "/firmware" */
        nodeoffset = fdt_add_subnode(fdt, nodeoffset, "android");
        if (nodeoffset < 0) {
            dprintf(INFO, "Warning: can't add firmware/android node in device tree\n");
            return;
        }
    }

    /* Set cmdline attributes to device tree */
    for (i = 0; i < countof(avb_dt_attrs); i++) {
        fdt_ret = avb_get_cmdline(avb_cmdline_attrs[i], fdt_value, MAX_VALUE_SZ);
        if (fdt_ret == STATUS_OK) {
            fdt_ret = fdt_setprop_string(fdt, nodeoffset, avb_dt_attrs[i], fdt_value);
            if (fdt_ret) {
                dprintf(INFO, "Error: can't set property in device tree\n");
                return;
            }
        }
    }
#endif
}

SET_FDT_INIT_HOOK(set_fdt_avb_cmdline, set_fdt_avb_cmdline);
