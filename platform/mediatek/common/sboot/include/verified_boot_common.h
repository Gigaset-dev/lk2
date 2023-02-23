/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#define HWUID_SIZE               (16)
#define MAX_PARTITION_NAME_LEN   (32)
#define PUBK_LEN                 (256)
#define RECOVERY_ROLLBACK_INDEX  (1)
#define RPMB_KEY_SIZE            (32)
#define SOC_ID_LEN               (32)
#define ME_IDENTITY_LEN          (16)
#define INVAILD_OTP_VALUE        (0xFFFFFFFF)

#define FAIL(code) (((int)(code)) != 0)

/* MISC group sub grouping
 * SUB_TINYSYS   SUB_TEE      SUB_LK       SUB_DA
 * 00000000   |  00000000  |  00000000  |  00000000
 */
#define SUB_GROUP_DA             (0)
#define SUB_GROUP_LK             (1)
#define SUB_GROUP_TEE            (2)
#define SUB_GROUP_TINYSYS        (3)
#define SUB_GROUP_MAX            (4)

struct sub_group_table {
    const char *image_name;
    uint8_t    sub_group;
};

enum OTPGroupType {
    SECURE_GROUP = 0,
    NON_SECURE_GROUP,
    AVB_GROUP,
    RECOVERY_GROUP,
    MISC_GROUP
};

struct otp_group_info {
    enum OTPGroupType group;
    uint32_t unit;
    uint32_t start_idx;
    uint32_t end_idx;
    uint32_t otp_ver;
    uint32_t otp_max_ver;
    uint32_t min_ver;
};

enum flash_dev_type_e {
    FLASH_DEV_EMMC = 0,
    FLASH_DEV_UFS,
    FLASH_DEV_NAND,
    FLASH_DEV_COMBO,
    FLASH_DEV_NOR,
};

struct data_buffer_t {
    char *data;
    uint32_t length;
};
