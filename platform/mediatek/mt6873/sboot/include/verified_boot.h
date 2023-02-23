/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <stdint.h>
#include <sys/types.h>

/* cert chain related operation */
#define SEC_FEATURE_MASK_ANTIROLLBACK   (0x00000001)
#define BYPASS_CERT_CHAIN_VERIFY        (0x00000002)
#define GET_SOCID_FROM_CERT2            (0x00000004)

/* max version for avb group */
#define AVB_OTP_MAX_UNIT_NUM            (8)
#define AVB_OTP_SECURE_GROUP_UNIT_NUM   (1)
#define RECOVERY_ROLLBACK_INDEX         (1)

/* avb persist value.
 * If PERSIST_VALUE_OFFSET is not specified, offset is the half partition
 * size of seccfg by default.
 */
#define PERSIST_PART_NAME               "seccfg"
#define PERSIST_VALUE_OFFSET            (0x200)
#define PERSIST_UNSPECIFIED_OFFSET      (0xFFFFFFFFFFFFFFFF)

#define SHA256_HASH_SZ                  (32)
#define RSA2048_KEY_SZ                  (256)
#define RSA2048_SIG_SZ                  (256)
#define SOCID_LEN                       (32)

/* secure boot flow */
int sec_lk_vb_init(void);
int sec_func_init(uint64_t pl_start_addr);
int seclib_set_oemkey(uint8_t *pubk, uint32_t size);
int sec_cert_parser_and_vfy(const char *part_name,
                        const char *img_name,
                        uint8_t *p_hdr,
                        uint32_t hdr_sz,
                        off_t start_offset,
                        uint32_t oem_opt);
int sec_hdr_auth(uint8_t *p_hdr, uint32_t hdr_size);
int sec_img_auth(uint8_t *buf, uint32_t buf_sz);
void sec_show_reminder_warning(void);
int sec_pl_auth(uint8_t *pl_addr, uint32_t pl_len);
int sec_get_socid_cert(uint8_t *socid, uint32_t id_len);

