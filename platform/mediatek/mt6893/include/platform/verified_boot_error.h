/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#define ERR_MOD_SHIFT           20
#define ERR_LV1_SHIFT           12
#define MAKE_ERR_MOD_LV1_BASE(mod, lv1_err) ((mod << ERR_MOD_SHIFT) | \
                          (lv1_err << ERR_LV1_SHIFT))

#define ERR_MOD_SECURITY_ID     0x00000001

#define MOD_ID                  ERR_MOD_SECURITY_ID

#define ERR_VB_LV1_GENERAL      0x0000
#define ERR_VB_LV1_AUTH         0x0001
#define ERR_VB_LV1_CRYPTO       0x0002
#define ERR_VB_LV1_SECCFG       0x0003
#define ERR_VB_LV1_ANTIROLLBACK 0x0004
#define ERR_VB_LV1_REGION       0x0005
#define ERR_VB_LV1_TRNG         0x0006
#define ERR_VB_LV1_VBSTATE      0x0007
#define ERR_VB_LV1_AVB          0x0008
#define ERR_VB_LV1_PL           0x0009

/* MOD: offset: 20
 * MOD_LV1: offset: 12
 * MOD_LV2: offset: 0
 * for MOD = 0x01, MOD_LV1 = 0x02, MOD_LV2 = 0x03, error code
 * would be 0x0010_2003
 */

/* 0x0010_0000 */
#define ERR_VB_GENERAL_BASE      (MAKE_ERR_MOD_LV1_BASE(MOD_ID, ERR_VB_LV1_GENERAL))
/* 0x0010_1000 */
#define ERR_VB_AUTH_BASE         (MAKE_ERR_MOD_LV1_BASE(MOD_ID, ERR_VB_LV1_AUTH))
/* 0x0010_2000 */
#define ERR_VB_CRYPTO_BASE       (MAKE_ERR_MOD_LV1_BASE(MOD_ID, ERR_VB_LV1_CRYPTO))
/* 0x0010_3000 */
#define ERR_VB_SECCFG_BASE       (MAKE_ERR_MOD_LV1_BASE(MOD_ID, ERR_VB_LV1_SECCFG))
/* 0x0010_4000 */
#define ERR_VB_ANTIROLLBACK_BASE (MAKE_ERR_MOD_LV1_BASE(MOD_ID, ERR_VB_LV1_ANTIROLLBACK))
/* 0x0010_5000 */
#define ERR_VB_REGION_BASE       (MAKE_ERR_MOD_LV1_BASE(MOD_ID, ERR_VB_LV1_REGION))
/* 0x0010_6000 */
#define ERR_VB_TRNG_BASE         (MAKE_ERR_MOD_LV1_BASE(MOD_ID, ERR_VB_LV1_TRNG))
/* 0x0010_7000 */
#define ERR_VB_STATE_BASE        (MAKE_ERR_MOD_LV1_BASE(MOD_ID, ERR_VB_LV1_VBSTATE))
/* 0x0010_8000 */
#define ERR_AVB_BASE             (MAKE_ERR_MOD_LV1_BASE(MOD_ID, ERR_VB_LV1_AVB))
/* 0x0010_9000 */
#define ERR_PL_INFO_BASE         (MAKE_ERR_MOD_LV1_BASE(MOD_ID, ERR_VB_LV1_PL))

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

/* AUTH */
#define ERR_VB_CERT_OBJ_ID_MISMATCH       (ERR_VB_AUTH_BASE + 0x000)
#define ERR_VB_CERT_OBJ_LEN_MISMATCH      (ERR_VB_AUTH_BASE + 0x001)
#define ERR_VB_CERT_TRAVERSE_MODE_UNKNOWN (ERR_VB_AUTH_BASE + 0x002)
#define ERR_VB_CERT_INVALID_OID_IDX       (ERR_VB_AUTH_BASE + 0x003)
#define ERR_VB_CERT_OID_NOT_FOUND         (ERR_VB_AUTH_BASE + 0x004)
#define ERR_VB_CERT_OBJ_NOT_FOUND         (ERR_VB_AUTH_BASE + 0x005)
#define ERR_VB_CERT_E_KEY_LEN_MISMATCH    (ERR_VB_AUTH_BASE + 0x006)
#define ERR_VB_CERT_E_KEY_MISMATCH        (ERR_VB_AUTH_BASE + 0x007)
#define ERR_VB_CERT_N_KEY_LEN_MISMATCH    (ERR_VB_AUTH_BASE + 0x008)
#define ERR_VB_CERT_N_KEY_MISMATCH        (ERR_VB_AUTH_BASE + 0x009)
#define ERR_VB_INVALID_OID_LEN            (ERR_VB_AUTH_BASE + 0x00A)
#define ERR_VB_INVALID_OID_BUF_LEN        (ERR_VB_AUTH_BASE + 0x00B)
#define ERR_VB_INVALID_OID_DER_SZ         (ERR_VB_AUTH_BASE + 0x00C)
#define ERR_VB_UNEXPECTED_IMG_TYPE        (ERR_VB_AUTH_BASE + 0x00D)
#define ERR_VB_IMG_INVALID_TYPE           (ERR_VB_AUTH_BASE + 0x00E)
#define ERR_VB_IMG_NOT_FOUND              (ERR_VB_AUTH_BASE + 0x00F)
#define ERR_VB_IMG_HDR_HASH_VFY_FAIL      (ERR_VB_AUTH_BASE + 0x010)
#define ERR_VB_IMG_HASH_VFY_FAIL          (ERR_VB_AUTH_BASE + 0x011)
#define ERR_VB_INVALID_IMG_HDR            (ERR_VB_AUTH_BASE + 0x012)
#define ERR_VB_INVALID_SIG_CONTENT        (ERR_VB_AUTH_BASE + 0x013)
#define ERR_VB_INVALID_SOCID_LEN          (ERR_VB_AUTH_BASE + 0x014)
#define ERR_VB_SW_ID_MISMATCH             (ERR_VB_AUTH_BASE + 0x015)
#define ERR_VB_INVALID_SIG_LEN            (ERR_VB_AUTH_BASE + 0x016)
#define ERR_VB_PADDING_LEN_INVALID        (ERR_VB_AUTH_BASE + 0x017)
#define ERR_VB_UNSUPPORTED_SIG_TYPE       (ERR_VB_AUTH_BASE + 0x018)
#define ERR_VB_INVALID_CERT_SZ            (ERR_VB_AUTH_BASE + 0x019)
#define ERR_VB_INVALID_HASH_SZ            (ERR_VB_AUTH_BASE + 0x01A)
#define ERR_VB_INVALID_SIG_SZ             (ERR_VB_AUTH_BASE + 0x01B)
#define ERR_VB_PUBK_NOT_INITIALIZED       (ERR_VB_AUTH_BASE + 0x01C)
#define ERR_VB_PUBK_AUTH_FAIL             (ERR_VB_AUTH_BASE + 0x01D)
#define ERR_VB_INVALID_PUBK_SZ            (ERR_VB_AUTH_BASE + 0x01E)
#define ERR_VB_DA_VFY_FAIL                (ERR_VB_AUTH_BASE + 0x01F)
#define ERR_VB_DA_KEY_INIT_FAIL           (ERR_VB_AUTH_BASE + 0x020)
#define ERR_VB_DA_RELOCATE_LEN_NOT_ENOUGH (ERR_VB_AUTH_BASE + 0x021)

/* CRYPTO */
#define ERR_VB_CRYPTO_HACC_MODE_INVALID     (ERR_VB_CRYPTO_BASE + 0x000)
#define ERR_VB_CRYPTO_HACC_KEY_LEN_INVALID  (ERR_VB_CRYPTO_BASE + 0x001)
#define ERR_VB_CRYPTO_HACC_DATA_UNALIGNED   (ERR_VB_CRYPTO_BASE + 0x002)
#define ERR_VB_CRYPTO_HACC_SEED_LEN_INVALID (ERR_VB_CRYPTO_BASE + 0x003)
#define ERR_VB_CRYPTO_HASH_FAIL             (ERR_VB_CRYPTO_BASE + 0x004)
#define ERR_VB_CRYPTO_RSA_PSS_CHK_FAIL      (ERR_VB_CRYPTO_BASE + 0x005)
#define ERR_VB_CRYPTO_RSA_SIG_CHK_FAIL      (ERR_VB_CRYPTO_BASE + 0x006)
#define ERR_VB_CRYPTO_SHA256_OP_FAIL        (ERR_VB_CRYPTO_BASE + 0x007)

/* SECCFG */
#define ERR_VB_SECCFG_NOT_FOUND           (ERR_VB_SECCFG_BASE + 0x000)
#define ERR_VB_SECCFG_STATUS_INVALID      (ERR_VB_SECCFG_BASE + 0x001)
#define ERR_VB_SECCFG_ERASE_FAIL          (ERR_VB_SECCFG_BASE + 0x002)
#define ERR_VB_SECCFG_WRITE_FAIL          (ERR_VB_SECCFG_BASE + 0x003)
#define ERR_VB_SECCFG_BUF_OVERFLOW        (ERR_VB_SECCFG_BASE + 0x004)
#define ERR_VB_SECCFG_INVALID_MAGIC       (ERR_VB_SECCFG_BASE + 0x005)

/* ANTI ROLLBACK */
#define ERR_VB_CERT_IMG_VER_NOT_SYNC      (ERR_VB_ANTIROLLBACK_BASE + 0x000)
#define ERR_VB_IMG_VER_ROLLBACK           (ERR_VB_ANTIROLLBACK_BASE + 0x001)
#define ERR_VB_IMG_VER_OVERFLOW           (ERR_VB_ANTIROLLBACK_BASE + 0x002)
#define ERR_VB_UNKNOWN_NV_CNT_GROUP       (ERR_VB_ANTIROLLBACK_BASE + 0x003)
#define ERR_VB_GET_NV_CNT_FAIL            (ERR_VB_ANTIROLLBACK_BASE + 0x004)
#define ERR_VB_WRITE_NV_CNT_FAIL          (ERR_VB_ANTIROLLBACK_BASE + 0x005)
#define ERR_VB_SW_VER_MISMATCH            (ERR_VB_ANTIROLLBACK_BASE + 0x006)
#define ERR_VB_INVALID_IMG_OTP_UNIT       (ERR_VB_ANTIROLLBACK_BASE + 0x007)
#define ERR_VB_INVALID_OTP_INDEX          (ERR_VB_ANTIROLLBACK_BASE + 0x008)
#define ERR_VB_INVALID_GROUP_IDX          (ERR_VB_ANTIROLLBACK_BASE + 0x009)
#define ERR_VB_INVALID_GROUP_NUM          (ERR_VB_ANTIROLLBACK_BASE + 0x00A)
#define ERR_VB_SEC_VER_BUF_INIT_FAIL      (ERR_VB_ANTIROLLBACK_BASE + 0x00B)
#define ERR_VB_SEC_OTP_TABLE_NOT_INIT     (ERR_VB_ANTIROLLBACK_BASE + 0x00C)
#define ERR_VB_IMG_VER_UNDERFLOW          (ERR_VB_ANTIROLLBACK_BASE + 0x00D)
#define ERR_VB_CERT_INVALID_VER_ID        (ERR_VB_ANTIROLLBACK_BASE + 0x00E)

/* REGION CHECK */
#define ERR_VB_REGION_INCLUDE             (ERR_VB_REGION_BASE + 0x000)
#define ERR_VB_REGION_OVERLAP             (ERR_VB_REGION_BASE + 0x001)
#define ERR_VB_REGION_OVERFLOW            (ERR_VB_REGION_BASE + 0x002)
#define ERR_VB_DA_ADDR_INVALID            (ERR_VB_REGION_BASE + 0x003)
#define ERR_VB_DA_LEN_INVALID             (ERR_VB_REGION_BASE + 0x004)
#define ERR_VB_INVALID_LOAD_ADDR          (ERR_VB_REGION_BASE + 0x005)
#define ERR_VB_INVALID_VER                (ERR_VB_REGION_BASE + 0x006)

/* TRNG */
#define ERR_VB_TRNG_WRITE_DATA_FAIL       (ERR_VB_TRNG_BASE + 0x000)

/* VBOOT STATE */
#define ERR_VB_STATE_PRINT_FAIL           (ERR_VB_STATE_BASE + 0x000)
#define ERR_VB_STATE_SHOW_WARNING_FAIL    (ERR_VB_STATE_BASE + 0x001)
#define ERR_VB_STATE_SET_CMDLINE_FAIL     (ERR_VB_STATE_BASE + 0x002)

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

/* PL */
#define ERR_VB_ROM_INFO_NOT_FOUND         (ERR_PL_INFO_BASE + 0x000)
#define ERR_VB_GFH_NOT_FOUND              (ERR_PL_INFO_BASE + 0x001)

