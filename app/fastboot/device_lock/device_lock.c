/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <custom.h>
#include <debug.h>
#include <err.h>
#include <fastboot.h>
#include <fastboot_entry.h>
#include <fastboot_oem_cmd.h>
#include <lib/bio.h>
#include <lock_state.h>
#include <mtk_key.h>
#if (WITH_PLATFORM_MEDIATEK_COMMON_VIDEO)
#include <platform/video.h>
#endif
#include <sboot.h>
#include <string.h>
#include <sys/types.h>
#include <verified_boot_error.h>

#define udelay(x)               spin(x)
#define mdelay(x)               udelay((x) * 1000)

#define OP_TIMEOUT_MS           (5000)
#define OP_INFO_DISP_MS         (3000)

#define UNLOCK_KEY_SIZE         (32)
#define SERIAL_NUMBER_SIZE      (16)

#define DEFAULT_ALLOW_UNLOCK    (1)

#define METADATA_PARTITION      "metadata"
#define USERDATA_PARTITION      "userdata"
#define METADATA_PARTITION_ALT  "md_udc"
#define CACHE_PARTITION         "cache"

unsigned char unlock_key_str[UNLOCK_KEY_SIZE] = {0};

/* Generate key, store in global buffer and display to terminal */
__WEAK int cmd_oem_get_key(const char *arg, void *data, unsigned int sz)
{
    /* FIXME: Add default implementation here */
    fastboot_info("Nothing has been done");
    fastboot_okay("");
    return NO_ERROR;
}

/* Get encrypted key string from terminal, and store in global buffer */
__WEAK int cmd_oem_set_enckey(const char *arg, void *data, unsigned int sz)
{
    /* FIXME: Add default implementation here */
    fastboot_info("Nothing has been done");
    fastboot_okay("");
    return NO_ERROR;
}

/* Decrypt the string from terminal and compare with stored key string */
__WEAK int check_key(void)
{
    /* FIXME: Add default implementation here */
    fastboot_info("Please Implement check key function");
    return ERR_VB_FB_VFY_KEY_FAIL;
}

static void show_fastboot_mode(void)
{
#if (WITH_PLATFORM_MEDIATEK_COMMON_VIDEO)
    video_clear_screen();
    video_set_cursor(video_get_rows() / 2, 0);
    video_printf(" => FASTBOOT mode...\n");
#endif
}

static void show_unlock_warranty(void)
{
#if (WITH_PLATFORM_MEDIATEK_COMMON_VIDEO)
    video_clear_screen();
    video_set_cursor(video_get_rows() / 2, 0);

    video_printf("Unlock bootloader?\n\n");
    video_printf("If you unlock the bootloader, you will be able to\n");
    video_printf("install custom operating system software on this phone.\n\n");
    video_printf("A custom OS is not subject to the same testing as the original OS, and can\n");
    video_printf("cause your phone and installed applications to stop working properly.\n\n");
    video_printf("To prevent unauthorized access to your personal data,\n");
    video_printf("unlocking the bootloader will also delete all personal\n");
    video_printf("data from your phone(a \"factory data reset\").\n\n");
    video_printf("Press the Volume UP/Down buttons to select Yes or No.\n\n");
    video_printf("Yes (Volume UP or Volume Down): Unlock(may void warranty).\n\n");
    video_printf("No  (Auto exit after 5 seconds): Do not unlock bootloader.\n\n");
#endif
}

static void show_lock_warranty(void)
{
#if (WITH_PLATFORM_MEDIATEK_COMMON_VIDEO)
    video_clear_screen();
    video_set_cursor(video_get_rows() / 2, 0);

    video_printf("Lock bootloader?\n\n");
    video_printf("If you lock the bootloader,you will need to install official operating\n");
    video_printf("system software on this phone.\n\n");
    video_printf("To prevent unauthorized access to your personal data,\n");
    video_printf("locking the bootloader will also delete all personal\n");
    video_printf("data from your phone(a \"factory data reset\").\n\n");
    video_printf("Press the Volume UP/Down buttons to select Yes or No.\n\n");
    video_printf("Yes (Volume UP or Volume Down): Lock bootloader.\n\n");
    video_printf("No  (Auto exit after 5 seconds): Do not lock bootloader.\n\n");
#endif
}

static void get_unlock_permission(unsigned int *is_allow_unlock)
{
#if (MTK_SECURITY_SW_SUPPORT)
    int len = 0;
    bdev_t *dev_frp = 0;
    int flag_size = sizeof(unsigned int);
    unsigned long long unlock_allowed_flag_offset = 0;

    dev_frp = bio_open_by_label("frp");
    if (!dev_frp) {
        dprintf(CRITICAL, "No frp partition exists\n");
        *is_allow_unlock = 1;
        return;
    }

    unlock_allowed_flag_offset = dev_frp->total_size - flag_size;
    dprintf(CRITICAL, "frp paritition size: 0x%llx\n", dev_frp->total_size);
    dprintf(CRITICAL, "unlock_allowed_flag_offset: 0x%llx\n", unlock_allowed_flag_offset);

    len = bio_read(dev_frp, is_allow_unlock, unlock_allowed_flag_offset, flag_size);
    if (len != flag_size) {
        dprintf(CRITICAL, "Failed to get unlock allow flag\n");
        *is_allow_unlock = 0;
    }
    bio_close(dev_frp);
#else
    *is_allow_unlock = DEFAULT_ALLOW_UNLOCK;
#endif
}

static int check_oem_key(void)
{
    int ret = 0;
    char *serial_number = 0;
    char cal_serial_number[SERIAL_NUMBER_SIZE + 1] = {0};

    serial_number = get_serialno();
    sec_get_serial_number_adapter(unlock_key_str,
                                  UNLOCK_KEY_SIZE,
                                  (unsigned char *)cal_serial_number,
                                  SERIAL_NUMBER_SIZE);

    ret = memcmp(serial_number, cal_serial_number, SERIAL_NUMBER_SIZE);
    if (ret) {
        dprintf(CRITICAL, "Serial number mismatch\n");
        dprintf(CRITICAL, "Serial number: %s\n", serial_number);
        dprintf(CRITICAL, "Calculated serial number: %s\n", cal_serial_number);
        ret = ERR_VB_FB_UNLOCK_WRONG_KEY_CODE;
    }

    return ret;
}

static int wipe_userdata(void)
{
    int ret = 0;
    off_t erased = 0;
    bdev_t *bdev = NULL;

    bdev = bio_open_by_label(USERDATA_PARTITION);
    if (bdev == NULL) {
        dprintf(CRITICAL, "Partition %s, does not exist\n", USERDATA_PARTITION);
        ret = ERR_VB_FB_PARTITION_NOT_FOUND;
        goto error;
    }

    erased = bio_erase(bdev, 0x0, (size_t)bdev->total_size);
    if (erased != bdev->total_size) {
        dprintf(CRITICAL, "Partition %s, erase failed! erased: %lld, total_size: %lld\n",
                USERDATA_PARTITION, erased, bdev->total_size);
        ret = ERR_VB_FB_PARTITION_FORMAT_FAIL;
        goto error;
    }
    bio_close(bdev);

    bdev = bio_open_by_label(METADATA_PARTITION);
    if (bdev == NULL) {
        dprintf(CRITICAL, "Partition %s, does not exist\n", METADATA_PARTITION);
    } else {
        erased = bio_erase(bdev, 0x0, (size_t)bdev->total_size);
        if (erased != bdev->total_size) {
            dprintf(CRITICAL, "Partition %s, erase failed! erased: %lld, total_size: %lld\n",
                    METADATA_PARTITION, erased, bdev->total_size);
            ret = ERR_VB_FB_PARTITION_FORMAT_FAIL;
            goto error;
        }
        bio_close(bdev);
    }

    bdev = bio_open_by_label(METADATA_PARTITION_ALT);
    if (bdev == NULL) {
        dprintf(CRITICAL, "Partition %s, does not exist\n", METADATA_PARTITION_ALT);
    } else {
        erased = bio_erase(bdev, 0x0, (size_t)bdev->total_size);
        if (erased != bdev->total_size) {
            dprintf(CRITICAL, "Partition %s, erase failed! erased: %lld, total_size: %lld\n",
                    METADATA_PARTITION_ALT, erased, bdev->total_size);
            ret = ERR_VB_FB_PARTITION_FORMAT_FAIL;
            goto error;
        }
        bio_close(bdev);
    }

    bdev = bio_open_by_label(CACHE_PARTITION);
    if (bdev == NULL) {
        dprintf(CRITICAL, "Partition %s, does not exist\n", CACHE_PARTITION);
    } else {
        erased = bio_erase(bdev, 0x0, (size_t)bdev->total_size);
        if (erased != bdev->total_size) {
            dprintf(CRITICAL, "Partition %s, erase failed! erased: %lld, total_size: %lld\n",
                    CACHE_PARTITION, erased, bdev->total_size);
            ret = ERR_VB_FB_PARTITION_FORMAT_FAIL;
        }
    }

error:
    if (bdev)
        bio_close(bdev);

    if (ret)
        dprintf(CRITICAL, "[SEC] Fastboot wip user data fail (0x%x)\n", ret);

    return ret;
}

static int unlock_process(void)
{
    int ret = 0;
    int lock_state = 0;

    if (sec_efuse_sbc_enabled_adapter()) {
        ret = check_key();
        if (ret) {
            dprintf(INFO, "[SEC] Unlock key verify fail (0x%x)\n", ret);
            fastboot_fail("Unlock key verify failed");
            return ret;
        }
    }

    ret = wipe_userdata();
    if (ret) {
        dprintf(CRITICAL, "Failed to wipe userdata partition, error: 0x%x\n", ret);
        fastboot_fail("wipe userdata failed");
        return ret;
    }

    /* Lock State Demo Code
     * use sec_set_device_lock_adapter to set lock state, it will
     * call custom_set_lock_state function.
     * init mtk_param_t struct and call set_param before calling
     * sec_set_device_lock_adapter.
     * if custom_set_lock_state not need param, no need do any thing
     * here.
     */
    struct mtk_param_t mtk_param = { "Test Code", sizeof("Test Code") };

    set_param(&mtk_param);

    ret = sec_set_device_lock_adapter(DEVICE_STATE_UNLOCKED);
    if (ret) {
        dprintf(CRITICAL, "Failed to set lock state, error: 0x%x\n", ret);
        fastboot_fail("Failed to set lock state");
        return ret;
    }

    ret = sec_query_device_lock_adapter(&lock_state);
    if (!ret)
        dprintf(INFO, "Current lock state = %d\n", lock_state);
    fastboot_okay("");

    return ret;
}

static int lock_process(void)
{
    int ret = 0;
    int lock_state = 0;
    int is_factory_mode = 0;

    ret = sec_get_factory_mode_adapter(&is_factory_mode);
    if (ret) {
        dprintf(CRITICAL, "Get factory mode failed, error: 0x%x\n", ret);
        return ret;
    }

    if (!is_factory_mode) {
        ret = wipe_userdata();
        if (ret) {
            dprintf(CRITICAL, "Wipe userdata partition failed, error: 0x%x\n", ret);
            fastboot_fail("wipe userdata failed");
            return ret;
        }
    }

    /* Lock State Demo Code
     * use sec_set_device_lock_adapter to set lock state, it will
     * call custom_set_lock_state function.
     * init mtk_param_t struct and call set_param before calling
     * sec_set_device_lock_adapter.
     * if custom_set_lock_state not need param, no need do anything
     * here.
     */
    struct mtk_param_t mtk_param = { "Test Code", sizeof("Test Code") };

    set_param(&mtk_param);

    ret = sec_set_device_lock_adapter(DEVICE_STATE_LOCKED);
    if (ret) {
        dprintf(CRITICAL, "Failed to set lock state, error: 0x%x\n", ret);
        fastboot_fail("Failed to set lock state");
        return ret;
    }

    ret = sec_query_device_lock_adapter(&lock_state);
    if (!ret)
        dprintf(INFO, "Current lock state = %d\n", lock_state);
    fastboot_okay("");

    return ret;
}

void update_secure_status_var(void)
{
    static const char str_buf[2][4] = {"no", "yes"};
    int is_locked = 0;
    int ret = 0;

    ret = sec_query_device_lock_adapter(&is_locked);
    if (ret) {
        fastboot_fail("Failed to get secure status!");
        return;
    }

    if (!strcmp(fastboot_get_var("secure"), ""))
        fastboot_publish("secure", str_buf[is_locked]);
    else
        fastboot_update_var("secure", str_buf[is_locked]);


    if (!strcmp(fastboot_get_var("unlocked"), ""))
        fastboot_publish("unlocked", str_buf[is_locked ? 0 : 1]);
    else
        fastboot_update_var("unlocked", str_buf[is_locked ? 0 : 1]);
}

static void cmd_flashing_unlock(const char *arg, void *data, unsigned int sz)
{
    int ret = 0;
    unsigned int is_unlock_allowed = 0;
    unsigned int count = 0;

    show_unlock_warranty();

    while (count < OP_TIMEOUT_MS) {
        if (detect_key(PMIC_RST_KEY) || detect_key(MENU_OK_KEY)) { // VOL_UP or VOL_DOWN key
#if (WITH_PLATFORM_MEDIATEK_COMMON_VIDEO)
            video_printf("Start unlock flow\n");
#endif

            get_unlock_permission(&is_unlock_allowed);
            dprintf(CRITICAL, "is_unlock_allowed = 0x%x\n", is_unlock_allowed);
            if (!is_unlock_allowed) {
                fastboot_fail("Prohibit unlock operation\n");
                ret = -1;
                goto unlock_out;
            }

            fastboot_info("Start unlock flow\n");
            ret = unlock_process();
            if (!ret) {
                update_secure_status_var();
                fastboot_okay("");
            } else {
                fastboot_fail("Failed to unlock the device\n");
            }
            goto unlock_out;
        }

        mdelay(10);
        count += 10;
    }

#if (WITH_PLATFORM_MEDIATEK_COMMON_VIDEO)
    video_printf("Timeout. Will return to fastboot in 3s\n");
#endif
    fastboot_okay("");
    mdelay(OP_INFO_DISP_MS);
    show_fastboot_mode();
    return;

unlock_out:
#if (WITH_PLATFORM_MEDIATEK_COMMON_VIDEO)
    if (ret)
        video_printf("Failed to unlock the device! Will return to fastboot in 3s\n");
    else
        video_printf("Unlock the device successfully! Will return to fastboot in 3s\n");
#endif
    mdelay(OP_INFO_DISP_MS);
    show_fastboot_mode();
}

static void cmd_flashing_lock(const char *arg, void *data, unsigned int sz)
{
    int ret = 0;
    int is_factory_mode = 0;
    unsigned int count = 0;

    ret = sec_get_factory_mode_adapter(&is_factory_mode);
    if (ret) {
        fastboot_fail("Failed to get current mode\n");
        goto lock_out;
    }

    show_lock_warranty();
    while (count < OP_TIMEOUT_MS) {
        if (detect_key(PMIC_RST_KEY) || detect_key(MENU_OK_KEY) || is_factory_mode) {
            fastboot_info("Start lock flow\n");
            ret = lock_process();

            if (!ret) {
                update_secure_status_var();
                fastboot_okay("");
            } else {
                fastboot_fail("Failed to lock the device\n");
            }
            goto lock_out;
        }

        mdelay(10);
        count += 10;
    }

#if (WITH_PLATFORM_MEDIATEK_COMMON_VIDEO)
    video_printf("Timeout. Will return to fastboot in 3s\n");
#endif
    fastboot_okay("");
    mdelay(OP_INFO_DISP_MS);
    show_fastboot_mode();
    return;

lock_out:
#if (WITH_PLATFORM_MEDIATEK_COMMON_VIDEO)
    if (ret)
        video_printf("Failed to lock the device! Will return to fastboot in 3s\n");
    else
        video_printf("Lock the device successfully! Will return to fastboot in 3s\n");
#endif
    mdelay(OP_INFO_DISP_MS);
    show_fastboot_mode();
}

static void cmd_flashing_get_unlock_ability(const char *arg, void *data, unsigned int sz)
{
    unsigned int is_unlock_allowed = 0;

    get_unlock_permission(&is_unlock_allowed);
    dprintf(CRITICAL, "is_unlock_allowed = 0x%x\n", is_unlock_allowed);
    if (is_unlock_allowed)
        fastboot_info("unlock_ability is true\n");
    else
        fastboot_info("unlock_ability is false\n");
    fastboot_okay("");
}

FASTBOOT_OEM_CMD_START(flashing_unlock)
    .cmd_str = "flashing unlock",
    .cmd_handler = cmd_flashing_unlock,
    .allowed_when_security_on = true,
    .forbidden_when_lock_on = false,
FASTBOOT_OEM_CMD_END

FASTBOOT_OEM_CMD_START(flashing_lock)
    .cmd_str = "flashing lock",
    .cmd_handler = cmd_flashing_lock,
    .allowed_when_security_on = true,
    .forbidden_when_lock_on = false,
FASTBOOT_OEM_CMD_END

FASTBOOT_OEM_CMD_START(flashing_get_unlock_ability)
    .cmd_str = "flashing get_unlock_ability",
    .cmd_handler = cmd_flashing_get_unlock_ability,
    .allowed_when_security_on = true,
    .forbidden_when_lock_on = false,
FASTBOOT_OEM_CMD_END

FASTBOOT_OEM_CMD_START(oem_set_enckey)
    .cmd_str = "oem set_enckey",
    .cmd_handler = cmd_oem_set_enckey,
    .allowed_when_security_on = true,
    .forbidden_when_lock_on = false,
FASTBOOT_OEM_CMD_END

FASTBOOT_OEM_CMD_START(oem_get_key)
    .cmd_str = "oem get_key",
    .cmd_handler = cmd_oem_get_key,
    .allowed_when_security_on = true,
    .forbidden_when_lock_on = false,
FASTBOOT_OEM_CMD_END
