/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <app/dm_verity_status.h>
#include <avb_crypto.h>
#include <avb_hal_error.h>
#include <avb_hal.h>
#include <avb_ops.h>
#include <avb_persist.h>
#include <avb_slot_verify.h>
#include <debug.h>
#include <fastboot.h>
#include <fastboot_oem_cmd.h>
#include <platform/addressmap.h>
#include <reg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <verified_boot.h>

#define DM_VERITY_ERROR_WAIT_TIME   (5000)

#define BOOT_MISC2                  (SECURITY_AO_BASE + 0x088)
#define BOOT_MISC2_DM_VERITY_ERR    (0x1 << 0)

__WEAK uint32_t cust_get_dm_verity_state(void)
{
    return 0;
}

__WEAK uint32_t cust_set_dm_verity_state(uint32_t state)
{
    return 0;
}

__WEAK uint32_t sec_cfg_get_dm_verity_state(uint32_t *dm_verity_state)
{
    return 0;
}

__WEAK uint32_t sec_cfg_set_dm_verity_state(uint32_t dm_verity_state)
{
    return 0;
}

static int get_dm_verity_state(void)
{
#if (MTK_SECURITY_SW_SUPPORT)
    int ret = 0;
    int dm_state = DM_VERITY_STATUS_OK;

    ret = sec_cfg_get_dm_verity_state(&dm_state);
    if (ret)
        dm_state = DM_VERITY_GENERAL_ERROR;

    return dm_state;
#else
    return 0;
#endif
}

static int set_dm_verity_state(uint32_t state)
{
#if (MTK_SECURITY_SW_SUPPORT)
    return sec_cfg_set_dm_verity_state(state);
#else
    return 0;
#endif
}

int get_dm_verity_status(uint32_t *status)
{
    uint32_t dm_state_boot_misc2 = DM_VERITY_STATUS_OK;
    uint32_t dm_state = DM_VERITY_STATUS_OK;
    int ret = STATUS_OK;
#if (MTK_SECURITY_SW_SUPPORT)
    uint32_t reg_val = 0;
#endif

    if (status == NULL)
        return ERR_VB_INVALID_ADDR;

#if (MTK_SECURITY_SW_SUPPORT)
    if (readl(BOOT_MISC2) & BOOT_MISC2_DM_VERITY_ERR)
        dm_state_boot_misc2 = DM_VERITY_GENERAL_ERROR;
#endif

    dm_state = get_dm_verity_state();

    if (dm_state_boot_misc2 != DM_VERITY_STATUS_OK) {
        ret = set_dm_verity_state(DM_VERITY_GENERAL_ERROR);
        if (ret) {
            dprintf(CRITICAL, "[%s] set_dm_verity_state fails(0x%x)\n", __func__, ret);
            return ret;
        }

#if (MTK_SECURITY_SW_SUPPORT)
        reg_val = readl(BOOT_MISC2);
        reg_val &= ~BOOT_MISC2_DM_VERITY_ERR;
        writel(reg_val, BOOT_MISC2);
#endif
        *status = DM_VERITY_GENERAL_ERROR;
    } else if (dm_state != DM_VERITY_STATUS_OK) {
        *status = DM_VERITY_GENERAL_ERROR;
    } else {
        *status = DM_VERITY_STATUS_OK;
    }

    return ret;
}

int clear_dm_verity_status(void)
{
    int ret = STATUS_OK;
    AvbOps *ops = 0;
    uint8_t digest_sha256[AVB_SHA256_DIGEST_SIZE] = {0};
#if (MTK_SECURITY_SW_SUPPORT)
    uint32_t reg_val = 0;

    reg_val = readl(BOOT_MISC2);
    reg_val &= ~BOOT_MISC2_DM_VERITY_ERR;
    writel(reg_val, BOOT_MISC2);
#endif

    ret = set_dm_verity_state(DM_VERITY_STATUS_OK);
    if (ret) {
        dprintf(CRITICAL, "[%s] set_dm_verity_state fails(0x%x)\n", __func__, ret);
        return ret;
    }

    ops = avb_hal_get_operations();
    if (!ops) {
        dprintf(CRITICAL, "[%s] Bad AVB Operation Pointer\n", __func__);
        return ERR_VB_INVALID_ADDR;
    }

    ret = init_persist_value(ops, PERSIST_PART_NAME, PERSIST_VALUE_OFFSET);
    if (ret != AVB_IO_RESULT_OK) {
        dprintf(CRITICAL, "[%s] Failed to initialize %s!\n", __func__, PERSIST_PART_NAME);
        return ret;
    }

    memset(digest_sha256, 0, AVB_SHA256_DIGEST_SIZE);
    ret = ops->write_persistent_value(ops,
                                      AVB_NPV_MANAGED_VERITY_MODE,
                                      AVB_SHA256_DIGEST_SIZE,
                                      digest_sha256);
    if (ret != AVB_IO_RESULT_OK) {
        dprintf(CRITICAL, "[%s] Failed to write digest_sha256!\n", __func__);
        return ret;
    }

    ret = write_persist_value(ops, PERSIST_PART_NAME);
    if (ret != AVB_IO_RESULT_OK) {
        dprintf(CRITICAL,
                "[%s] Failed to update digest_sha256 to %s!\n",
                __func__,
                PERSIST_PART_NAME);
    } else {
        dprintf(CRITICAL,
                "[%s] Update digest_sha256 to %s successfully!\n",
                __func__,
                PERSIST_PART_NAME);
    }

    return ret;
}

static void cmd_clear_dm_verity_status(const char *arg, void *data, unsigned int sz)
{
    dprintf(ALWAYS, "[FASTBOOT] Start reset dm-verity status.\n");

    clear_dm_verity_status();
    fastboot_okay("");
}

FASTBOOT_OEM_CMD_START(cmd_clear_dm_verity_status)
    .cmd_str = "oem cdms",
    .cmd_handler = cmd_clear_dm_verity_status,
    .allowed_when_security_on = true,
    .forbidden_when_lock_on = false,
FASTBOOT_OEM_CMD_END
