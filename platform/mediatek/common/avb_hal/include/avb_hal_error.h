/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#define STATUS_OK               0x0

#define ERR_MOD_SHIFT           20
#define ERR_LV1_SHIFT           12
#define MAKE_ERR_MOD_LV1_BASE(mod, lv1_err) \
                                ((mod << ERR_MOD_SHIFT) | (lv1_err << ERR_LV1_SHIFT))

#define ERR_MOD_SECURITY_ID     0x00000001
#define MOD_ID                  ERR_MOD_SECURITY_ID

#define ERR_VB_LV1_GENERAL      0x0000
#define ERR_VB_LV1_AVB          0x0008

/* MOD: offset: 20
 * MOD_LV1: offset: 12
 * MOD_LV2: offset: 0
 * for MOD = 0x01, MOD_LV1 = 0x02, MOD_LV2 = 0x03, error code
 * would be 0x0010_2003
 */
#define ERR_VB_GENERAL_BASE     (MAKE_ERR_MOD_LV1_BASE(MOD_ID, ERR_VB_LV1_GENERAL))
#define ERR_AVB_BASE            (MAKE_ERR_MOD_LV1_BASE(MOD_ID, ERR_VB_LV1_AVB))

/* GENERAL */
#define ERR_VB_BUF_ADDR_INVALID           (ERR_VB_GENERAL_BASE + 0x000)
#define ERR_VB_BUF_OVERFLOW               (ERR_VB_GENERAL_BASE + 0x001)
#define ERR_VB_INVALID_IMG_SZ             (ERR_VB_GENERAL_BASE + 0x002)
#define ERR_VB_INVALID_IMG_ALIGN_SZ       (ERR_VB_GENERAL_BASE + 0x003)
#define ERR_VB_INVALID_PAGE_SZ            (ERR_VB_GENERAL_BASE + 0x004)
#define ERR_VB_INVALID_STRING             (ERR_VB_GENERAL_BASE + 0x005)
#define ERR_VB_INVALID_ADDR               (ERR_VB_GENERAL_BASE + 0x006)
#define ERR_VB_SEC_BUF_ALLOCATE_FAIL      (ERR_VB_GENERAL_BASE + 0x007)
#define ERR_VB_STORAGE_READ_FAIL          (ERR_VB_GENERAL_BASE + 0x008)
#define ERR_VB_INVALID_BUF_SZ             (ERR_VB_GENERAL_BASE + 0x009)

/* AVB */
#define ERR_AVB_CMDLINE_ENTRY_NOT_FOUND   (ERR_AVB_BASE + 0x000)
#define ERR_AVB_INVALID_KEY_BUF           (ERR_AVB_BASE + 0x001)
#define ERR_AVB_INVALID_OUTPUT_BUF        (ERR_AVB_BASE + 0x002)
#define ERR_AVB_INVALID_VALUE_SZ          (ERR_AVB_BASE + 0x003)
#define ERR_AVB_INVALID_INPUT_BUF         (ERR_AVB_BASE + 0x004)
#define ERR_AVB_INVALID_CMDLINE_FMT       (ERR_AVB_BASE + 0x005)
#define ERR_AVB_CMDLINE_OVERFLOW          (ERR_AVB_BASE + 0x006)
#define ERR_AVB_UNLOCK_DEVICE_FAILED      (ERR_AVB_BASE + 0x007)
#define ERR_AVB_SET_VERIFICATION_FAILED   (ERR_AVB_BASE + 0x008)
#define ERR_AVB_SET_VERITY_FAILED         (ERR_AVB_BASE + 0x009)
#define ERR_AVB_VERIFICAITON_FAILED       (ERR_AVB_BASE + 0x00A)
#define ERR_AVB_NO_SUCH_PARTITION         (ERR_AVB_BASE + 0x00B)
#define ERR_AVB_INVALID_OFFSET            (ERR_AVB_BASE + 0x00C)
#define ERR_AVB_UNINITIAL_PERSIST_VALUE   (ERR_AVB_BASE + 0x00D)
