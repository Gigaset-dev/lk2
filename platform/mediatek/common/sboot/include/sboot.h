/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <stdbool.h>
#include <sys/types.h>
#include <verified_boot_common.h>

/* anti-rollback */
int sec_rollback_check_adapter(const char *img_name);

/* get verify policy */
int sec_get_vfy_policy_adapter(const char *part_name);

/* verify cert of image */
int sec_cert_parser_and_vfy_adapter(const char *part_name,
       const char *img_name,
       uint8_t *p_hdr,
       uint32_t hdr_sz,
       off_t start_offset,
       uint32_t oem_opt);

/* verify image header */
int sec_hdr_auth_adapter(uint8_t *p_hdr, uint32_t hdr_size);

/* verify image body */
int sec_img_auth_adapter(uint8_t *buf, uint32_t buf_sz);

/* fill secure lib masp data */
uint32_t *sec_fill_atag_masp_data_adapter(uint32_t *ptr);

/* set lock state */
int sec_set_device_lock_adapter(int do_lock);

/* get lock state */
int sec_query_device_lock_adapter(int *lock_state);

/* get public key */
int sec_get_pubk_adapter(uint8_t *pubk, uint32_t pubk_size);

/* sha256 function */
int sec_sha256_adapter(const uint8_t *in, uintptr_t inlen, uint8_t *hash);
uint32_t sec_sha256_tfa_init_adapter(void);
uint32_t sec_sha256_tfa_process_adapter(const uint8_t *in, uint64_t len);
uint32_t sec_sha256_tfa_done_adapter(uint8_t *hash);

/* update otp version */
int sec_otp_ver_update_adapter(uint32_t boot_mode);

/* sync sml data */
int sec_sync_sml_data_adapter(void);

/* is protect2 ready */
int sec_is_protect2_ready_for_wp_adapter(void);

/* Get Software Version Number For AVB Image Group */
int sec_get_avb_otp_ver_adapter(uint32_t group, uint32_t *out_ver);

/* Get SOCID For dconfig (boot_para) */
int sec_get_socid_cert_adapter(uint8_t *socid, uint32_t id_len);

/* Check If Boot In Factory Mode */
int sec_get_factory_mode_adapter(int *is_factory_mode);

/* Get SN From Platform */
int sec_get_serial_number_adapter(uint8_t *unlock_key,
                                  uint32_t unlock_key_len,
                                  uint8_t *cal_serial_number,
                                  uint32_t cal_serial_len);

/* Derive Per-Device Key From Platform */
int sec_key_derive_adapter(uint32_t key_type,
                           uint8_t *output_key,
                           uint32_t output_key_size);

/* Get Download Policy From Secure Library */
int sec_get_dl_policy_adapter(const char *part_name);

/* Lock State Custom Wrapper */
int sec_set_lock_state_adapter(uint32_t lock_state);
int sec_get_lock_state_adapter(uint32_t *lock_state);

/* Security efuse api wrapper */
int sec_efuse_sbc_enabled_adapter(void);

/* Fastboot Security */
int sec_create_loader_hb_adapter(enum flash_dev_type_e fd, const struct data_buffer_t *pl,
                         struct data_buffer_t *hb);
int sec_erase_header_block_for_cert_adapter(struct data_buffer_t *hb);
int sec_get_hb_size_adapter(enum flash_dev_type_e fd, const struct data_buffer_t *pl,
                    uint32_t *hb_len);
int sec_get_updated_cert_hb_adapter(enum flash_dev_type_e fd, struct data_buffer_t *hb,
                            const struct data_buffer_t *cert,
                            uint32_t *cert_offset);
int sec_pl_bin_already_has_hb_adapter(const struct data_buffer_t *pl);
uint32_t sec_get_cert_offset_adapter(struct data_buffer_t *hb);
uint32_t sec_skip_pl_bin_hb_size_adapter(const char *pl);
int sec_dev_read_wrapper(char *part_name, uint64_t offset, uint8_t *data, uint32_t size);
int sec_dev_write_wrapper(char *part_name, uint64_t offset, uint8_t *data, uint32_t size);
int sec_get_socid_from_pl_boottags(uint8_t socid_arr[], uint32_t size);
int sec_get_meid_from_pl_boottags(uint8_t meid_arr[], uint32_t size);

int sec_pl_auth_adpater(int8_t *pl_addr, uint32_t pl_len);
int sec_set_cust_pubk_adpater(uint8_t *pubk, uint32_t pubk_size);
int sec_get_cust_pubk_adpater(uint8_t *pubk, uint32_t pubk_size);
int sec_clear_cust_pubk_adpater(void);
uint32_t sec_img_auth_buf_adapter(uint8_t *img_buf, uint32_t img_buf_sz,
                           uint8_t *img_name, uint32_t oem_opt);
void sec_otp_set_da_ver_adpter(void);
