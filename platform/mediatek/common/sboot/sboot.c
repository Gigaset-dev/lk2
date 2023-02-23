/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <bootctrl.h>
#include <compiler.h>
#include <custom.h>
#include <debug.h>
#include <err.h>
#include <errno.h>
#include <key_derive.h>
#include <lib/bio.h>
#include <lib/pl_boottags.h>
#include <lock_state.h>
#include <platform_mtk.h>
#include <sboot.h>
#include <sec_policy.h>
#include <stdint.h>
#include <string.h>
#include <verified_boot.h>
#include <verified_boot_common.h>
#include <verified_boot_error.h>

#define EN_SUBGROUP     (1)
#define DIS_SUBGROUP    (0)
#define DA_VER          (0) /* Set da otp ver in lk */

#define BOOT_TAG_SOC_ID (0x88610021)
static struct boot_tag_soc_id {
    uint8_t id[SOC_ID_LEN];
} socid_info;

#define BOOT_TAG_MEID_INFO (0x8861002B)
static struct boot_tag_meid_info {
    uint8_t meid[ME_IDENTITY_LEN];
} meid_info;

static void pl_boottags_socid_hook(struct boot_tag *tag)
{
    struct boot_tag_soc_id *p = (struct boot_tag_soc_id *)&tag->data;

    for (int i = 0; i < SOC_ID_LEN; i++)
        socid_info.id[i] = p->id[i];
}
PL_BOOTTAGS_INIT_HOOK(pl_boottags_socid_hook,
                      BOOT_TAG_SOC_ID,
                      pl_boottags_socid_hook);

static void pl_boottags_meid_hook(struct boot_tag *tag)
{
    struct boot_tag_meid_info *p = (struct boot_tag_meid_info *)&tag->data;

    for (int i = 0; i < ME_IDENTITY_LEN; i++)
        meid_info.meid[i] = p->meid[i];
}
PL_BOOTTAGS_INIT_HOOK(pl_boottags_meid_hook,
                      BOOT_TAG_MEID_INFO,
                      pl_boottags_meid_hook);

int sec_get_socid_from_pl_boottags(uint8_t socid_arr[], uint32_t size)
{
    if (socid_arr == NULL)
        return ERR_VB_INVALID_ADDR;

    memcpy(socid_arr, socid_info.id, size);
    return 0;
}

int sec_get_meid_from_pl_boottags(uint8_t meid_arr[], uint32_t size)
{
    if (meid_arr == NULL)
        return ERR_VB_INVALID_ADDR;

    memcpy(meid_arr, meid_info.meid, size);
    return 0;
}

__WEAK uint8_t *sec_get_socid_addr(void)
{
    dprintf(CRITICAL, "[SEC] Weak Function %s At Line %d", __func__, __LINE__);
    return NULL;
}

__WEAK int sec_get_vfy_policy(const char *part_name)
{
    dprintf(CRITICAL, "[SEC] Weak Function %s At Line %d", __func__, __LINE__);
    return 0;
}

__WEAK int sec_cert_parser_and_vfy(const char *part_name,
       const char *img_name,
       uint8_t *p_hdr,
       uint32_t hdr_sz,
       off_t start_offset,
       uint32_t oem_opt)
{
    dprintf(CRITICAL, "[SEC] Weak Function %s At Line %d", __func__, __LINE__);
    return 0;
}

__WEAK int sec_hdr_auth(uint8_t *p_hdr, uint32_t hdr_size)
{
    dprintf(CRITICAL, "[SEC] Weak Function %s At Line %d", __func__, __LINE__);
    return 0;
}

__WEAK int sec_img_auth(uint8_t *buf, uint32_t buf_sz)
{
    dprintf(CRITICAL, "[SEC] Weak Function %s At Line %d", __func__, __LINE__);
    return 0;
}

__WEAK int sec_rollback_check(unsigned int group, const char *img_name)
{
    dprintf(CRITICAL, "[SEC] Weak Function %s At Line %d", __func__, __LINE__);
    return 0;
}

__WEAK uint32_t *sec_fill_atag_masp_data(uint32_t *ptr)
{
    dprintf(CRITICAL, "[SEC] Weak Function %s At Line %d", __func__, __LINE__);
    return ptr;
}

__WEAK int sec_set_device_lock(int do_lock)
{
    dprintf(CRITICAL, "[SEC] Weak Function %s At Line %d", __func__, __LINE__);
    return 0;
}

__WEAK int sec_query_device_lock(int *lock_state)
{
    dprintf(CRITICAL, "[SEC] Weak Function %s At Line %d", __func__, __LINE__);
    return 0;
}

__WEAK int sec_get_pubk(uint8_t *pubk, uint32_t pubk_size)
{
    dprintf(CRITICAL, "[SEC] Weak Function %s At Line %d", __func__, __LINE__);
    return 0;
}

__WEAK int sec_sha256(const uint8_t *in, uintptr_t inlen, uint8_t *hash)
{
    dprintf(CRITICAL, "[SEC] Weak Function %s At Line %d", __func__, __LINE__);
    return 0;
}

__WEAK uint32_t sec_sha256_tfa_init(void)
{
    dprintf(CRITICAL, "[SEC] Weak Function %s At Line %d", __func__, __LINE__);
    return 0;
}

__WEAK uint32_t sec_sha256_tfa_process(const uint8_t *in, uint64_t len)
{
    dprintf(CRITICAL, "[SEC] Weak Function %s At Line %d", __func__, __LINE__);
    return 0;
}

__WEAK uint32_t sec_sha256_tfa_done(uint8_t *hash)
{
    dprintf(CRITICAL, "[SEC] Weak Function %s At Line %d", __func__, __LINE__);
    return 0;
}

__WEAK int sec_otp_ver_update(uint32_t boot_mode, uint32_t enable_subgroup)
{
    dprintf(CRITICAL, "[SEC] Weak Function %s At Line %d", __func__, __LINE__);
    return 0;
}

__WEAK int sec_is_protect2_ready_for_wp(void)
{
    dprintf(CRITICAL, "[SEC] Weak Function %s At Line %d", __func__, __LINE__);
    return 0;
}

__WEAK int sec_sync_sml_data(void)
{
    dprintf(CRITICAL, "[SEC] Weak Function %s At Line %d", __func__, __LINE__);
    return 0;
}

__WEAK int sec_get_otp_ver(uint32_t group, uint32_t *out_ver)
{
    dprintf(CRITICAL, "[SEC] Weak Function %s At Line %d", __func__, __LINE__);
    return 0;
}

__WEAK int sec_get_socid_cert(uint8_t *socid, uint32_t id_len)
{
    dprintf(CRITICAL, "[SEC] Weak Function %s At Line %d", __func__, __LINE__);
    return 0;
}

__WEAK int sec_is_in_factory(int *is_factory_mode)
{
    dprintf(CRITICAL, "[SEC] Weak Function %s At Line %d", __func__, __LINE__);
    return 0;
}

__WEAK int sec_get_serial_number_from_unlock_key(uint8_t *unlock_key,
                                                 uint32_t unlock_key_len,
                                                 uint8_t *cal_serial_number,
                                                 uint32_t cal_serial_len)
{
    dprintf(CRITICAL, "[SEC] Weak Function %s At Line %d", __func__, __LINE__);
    return 0;
}

__WEAK unsigned int sec_key_derive(uint32_t key_type,
                                   uint8_t *output_key,
                                   uint32_t output_key_size)
{
    dprintf(CRITICAL, "[SEC] Weak Function %s At Line %d", __func__, __LINE__);
    return 0;
}

__WEAK uint32_t sec_skip_pl_bin_hb_size(const char *pl)
{
    dprintf(CRITICAL, "[SEC] Weak Function %s At Line %d", __func__, __LINE__);
    return 0;
}

__WEAK int sec_pl_bin_already_has_hb(const struct data_buffer_t *pl)
{
    dprintf(CRITICAL, "[SEC] Weak Function %s At Line %d", __func__, __LINE__);
    return 0;
}

__WEAK int sec_get_hb_size(enum flash_dev_type_e fd, const struct data_buffer_t *pl,
                    uint32_t *hb_len)
{
    dprintf(CRITICAL, "[SEC] Weak Function %s At Line %d", __func__, __LINE__);
    return 0;
}

__WEAK int sec_create_loader_hb(enum flash_dev_type_e fd, const struct data_buffer_t *pl,
                         struct data_buffer_t *hb)
{
    dprintf(CRITICAL, "[SEC] Weak Function %s At Line %d", __func__, __LINE__);
    return 0;
}

__WEAK int sec_get_updated_cert_hb(enum flash_dev_type_e fd, struct data_buffer_t *hb,
                            const struct data_buffer_t *cert,
                            uint32_t *cert_offset)
{
    dprintf(CRITICAL, "[SEC] Weak Function %s At Line %d", __func__, __LINE__);
    return 0;
}

__WEAK int sec_erase_header_block_for_cert(struct data_buffer_t *hb)
{
    dprintf(CRITICAL, "[SEC] Weak Function %s At Line %d", __func__, __LINE__);
    return 0;
}

__WEAK uint32_t sec_get_cert_offset(struct data_buffer_t *hb)
{
    dprintf(CRITICAL, "[SEC] Weak Function %s At Line %d", __func__, __LINE__);
    return 0;
}

__WEAK int sec_cfg_get_lock_state(uint32_t *lock_state)
{
    dprintf(CRITICAL, "[SEC] Weak Function %s At Line %d", __func__, __LINE__);
    return 0;
}

__WEAK int sec_cfg_set_lock_state(uint32_t lock_state)
{
    dprintf(CRITICAL, "[SEC] Weak Function %s At Line %d", __func__, __LINE__);
    return 0;
}

__WEAK int sec_pl_auth(int8_t *pl_addr, uint32_t pl_len)
{
    dprintf(CRITICAL, "[SEC] Weak Function %s At Line %d", __func__, __LINE__);
    return 0;
}

__WEAK int sec_set_cust_pubk(uint8_t *pubk, uint32_t pubk_size)
{
    dprintf(CRITICAL, "[SEC] Weak Function %s At Line %d", __func__, __LINE__);
    return 0;
}

__WEAK int sec_get_cust_pubk(uint8_t *pubk, uint32_t pubk_size)
{
    dprintf(CRITICAL, "[SEC] Weak Function %s At Line %d", __func__, __LINE__);
    return 0;
}

__WEAK int sec_clear_cust_pubk(void)
{
    dprintf(CRITICAL, "[SEC] Weak Function %s At Line %d", __func__, __LINE__);
    return 0;
}

__WEAK uint32_t sec_img_auth_buf(uint8_t *img_buf, uint32_t img_buf_sz,
                                           uint8_t *img_name, uint32_t oem_opt)
{
    dprintf(CRITICAL, "[SEC] Weak Function %s At Line %d", __func__, __LINE__);
    return 0;
}

/* anti-rollback */
int sec_rollback_check_adapter(const char *img_name)
{
    int ret = 0;
#if (MTK_SECURITY_SW_SUPPORT && MTK_SECURITY_ANTI_ROLLBACK)
    enum BOOT_STAGE boot_stage = 0;

    boot_stage = get_boot_stage();

    if (boot_stage == BOOT_STAGE_BL2_EXT)
#if (MTK_SECURITY_ENABLE_SUB_GROUP)
        ret = sec_rollback_check(MISC_GROUP, img_name);
#else
        ret = sec_rollback_check(SECURE_GROUP, img_name);
#endif
    else if (boot_stage == BOOT_STAGE_BL33)
        ret = sec_rollback_check(NON_SECURE_GROUP, NULL);
#endif
    return ret;
}

/* get verify policy */
int sec_get_vfy_policy_adapter(const char *part_name)
{
#if (MTK_SECURITY_SW_SUPPORT)
    return sec_get_vfy_policy(part_name);
#else
    return 0;
#endif
}

/* verify cert of image */
int sec_cert_parser_and_vfy_adapter(const char *part_name,
       const char *img_name,
       uint8_t *p_hdr,
       uint32_t hdr_sz,
       off_t start_offset,
       uint32_t oem_opt)
{
#if (MTK_SECURITY_SW_SUPPORT)
    return sec_cert_parser_and_vfy(part_name, img_name,
                                 p_hdr, hdr_sz,
                                 start_offset, oem_opt);
#else
    return 0;
#endif
}

/* verify image header */
int sec_hdr_auth_adapter(uint8_t *p_hdr, uint32_t hdr_size)
{
#if (MTK_SECURITY_SW_SUPPORT)
    return sec_hdr_auth(p_hdr, hdr_size);
#else
    return 0;
#endif
}

/* verify image body */
int sec_img_auth_adapter(uint8_t *buf, uint32_t buf_sz)
{
#if (MTK_SECURITY_SW_SUPPORT)
    return sec_img_auth(buf, buf_sz);
#else
    return 0;
#endif
}

/* fill masp secure lib data */
uint32_t *sec_fill_atag_masp_data_adapter(uint32_t *ptr)
{
#if (MTK_SECURITY_SW_SUPPORT)
    return sec_fill_atag_masp_data(ptr);
#else
    return ptr;
#endif
}

/* set lock state */
int sec_set_device_lock_adapter(int do_lock)
{
#if (MTK_SECURITY_SW_SUPPORT)
    return sec_set_device_lock(do_lock);
#else
    return 0;
#endif
}

/* get lock state */
int sec_query_device_lock_adapter(int *lock_state)
{
#if (MTK_SECURITY_SW_SUPPORT)
    return sec_query_device_lock(lock_state);
#else
    *lock_state = DEVICE_STATE_UNLOCKED;
    return 0;
#endif
}

/* get public key */
int sec_get_pubk_adapter(uint8_t *pubk, uint32_t pubk_size)
{
#if (MTK_SECURITY_SW_SUPPORT)
    return sec_get_pubk(pubk, pubk_size);
#else
    return 0;
#endif
}

/* sha256 */
int sec_sha256_adapter(const uint8_t *in, uintptr_t inlen, uint8_t *hash)
{
#if (MTK_SECURITY_SW_SUPPORT)
    return sec_sha256(in, inlen, hash);
#else
    return 0;
#endif
}

uint32_t sec_sha256_tfa_init_adapter(void)
{
#if (MTK_SECURITY_SW_SUPPORT)
    return sec_sha256_tfa_init();
#else
    return 0;
#endif
}

uint32_t sec_sha256_tfa_process_adapter(const uint8_t *in, uint64_t len)
{
#if (MTK_SECURITY_SW_SUPPORT)
    return sec_sha256_tfa_process(in, len);
#else
    return 0;
#endif
}

uint32_t sec_sha256_tfa_done_adapter(uint8_t *hash)
{
#if (MTK_SECURITY_SW_SUPPORT)
    return sec_sha256_tfa_done(hash);
#else
    return 0;
#endif
}

/* Get Software Version Number For AVB Image Group */
int sec_get_avb_otp_ver_adapter(uint32_t group, uint32_t *out_ver)
{
#if (MTK_SECURITY_SW_SUPPORT && MTK_SECURITY_ANTI_ROLLBACK)
    if (group != AVB_GROUP && group != RECOVERY_GROUP) {
        dprintf(CRITICAL, "Read otp version: Not avb or recovery group!\n");
        return ERR_VB_INVALID_GROUP_IDX;
    }

    return sec_get_otp_ver(group, out_ver);
#else
    return 0;
#endif
}

/* Get SOCID For dconfig (boot_para) */
int sec_get_socid_cert_adapter(uint8_t *socid, uint32_t id_len)
{
#if (MTK_SECURITY_SW_SUPPORT)
    return sec_get_socid_cert(socid, id_len);
#else
    return 0;
#endif
}

/* Check If protect2 Is Ready For WP */
int sec_is_protect2_ready_for_wp_adapter(void)
{
#if (MTK_SECURITY_SW_SUPPORT && MTK_POWER_ON_WRITE_PROTECT)
    return sec_is_protect2_ready_for_wp();
#else
    /* Relax protect2 Partition WP */
    return 1;
#endif
}

/* sync sml data */
int sec_sync_sml_data_adapter(void)
{
#if (MTK_SECURITY_SW_SUPPORT && MTK_POWER_ON_WRITE_PROTECT)
    return sec_sync_sml_data();
#else
    return 0;
#endif
}

/* Check If Boot In Factory Mode */
int sec_get_factory_mode_adapter(int *is_factory_mode)
{
#if (MTK_SECURITY_SW_SUPPORT)
    return sec_is_in_factory(is_factory_mode);
#else
    return 0;
#endif
}

/* Get SN From Platform */
int sec_get_serial_number_adapter(uint8_t *unlock_key,
                                  uint32_t unlock_key_len,
                                  uint8_t *cal_serial_number,
                                  uint32_t cal_serial_len)
{
#if (MTK_SECURITY_SW_SUPPORT)
    return sec_get_serial_number_from_unlock_key(unlock_key,
                                                 unlock_key_len,
                                                 cal_serial_number,
                                                 cal_serial_len);
#else
    return 0;
#endif
}

/* Derive Per-Device Key From Platform */
int sec_key_derive_adapter(uint32_t key_type,
                           uint8_t *output_key,
                           uint32_t output_key_size)
{
#if (MTK_SECURITY_SW_SUPPORT)
    return sec_key_derive(key_type, output_key, output_key_size);
#else
    return 0;
#endif
}

/* Get Current Custom Lock State */
int sec_get_lock_state_adapter(uint32_t *lock_state)
{
#if (MTK_SECURITY_SW_SUPPORT)
#if (LOCK_STATE_SECCFG_CUSTOM)
    uint32_t ret = NO_ERROR;
    uint32_t lock_state1 = LKS_LOCK;
    uint32_t lock_state2 = LKS_LOCK;

    if (sec_cfg_get_lock_state(&lock_state1)) {
        struct res_state_t state = { ERR_VB_LKS_SECCFG_FAIL, lock_state1, lock_state2 };

        return custom_error_process(&state);
    }

    if (custom_get_lock_state(&lock_state2)) {
        struct res_state_t state = { ERR_VB_LKS_CUSTOM_FAIL, lock_state1, lock_state2 };

        return custom_error_process(&state);
    } else if (lock_state1 == lock_state2) {
        *lock_state = lock_state1;
        /* Get lock state ok */
        return NO_ERROR;
    } else {
        *lock_state = LKS_LOCK;
        struct res_state_t state = { ERR_VB_LKS_NOT_EQUAL, lock_state1, lock_state2 };

        return custom_error_process(&state);
    }
#elif (LOCK_STATE_CUSTOM)
    /* Use custom implementation */
    return custom_get_lock_state(lock_state);
#elif (LOCK_STATE_RPMB)
    /* No Support Now */
#else
    /* Default */
    return sec_cfg_get_lock_state(lock_state);
#endif
#else //MTK_SECURITY_SW_SUPPORT
    *lock_state = LKS_UNLOCK;
    return 0;
#endif
}

/* Set Current Custom Lock State To Lock */
int sec_set_lock_state_adapter(uint32_t lock_state)
{
#if (MTK_SECURITY_SW_SUPPORT)
#if (LOCK_STATE_SECCFG_CUSTOM)
    if (sec_cfg_set_lock_state(lock_state)) {
        dprintf(CRITICAL, "Seccfg set lock state failed!\n");
        return ERR_VB_LKS_SECCFG_FAIL;
    }

    if (custom_set_lock_state(lock_state)) {
        dprintf(CRITICAL, "Custom set lock state failed!\n");
        return ERR_VB_LKS_CUSTOM_FAIL;
    } else {
        /* Set lock state ok */
        return NO_ERROR;
    }
#elif (LOCK_STATE_CUSTOM)
    /* Use custom implementation */
    return custom_set_lock_state(lock_state);
#elif (LOCK_STATE_RPMB)
    /* No Support Now */
#else
    /* Default */
    return sec_cfg_set_lock_state(lock_state);
#endif
#else //MTK_SECURITY_SW_SUPPORT
    return 0;
#endif
}

/* Security eFuse API */
int sec_efuse_sbc_enabled_adapter(void)
{
#if (MTK_SECURITY_SW_SUPPORT)
    return efuse_sbc_enabled();
#else
    return 0;
#endif
}
/* Get Size To Skip Preloader Header Block */
uint32_t sec_skip_pl_bin_hb_size_adapter(const char *pl)
{
    return sec_skip_pl_bin_hb_size(pl);
}

/* Check Preloader For Headerblock Existence */
int sec_pl_bin_already_has_hb_adapter(const struct data_buffer_t *pl)
{
    return sec_pl_bin_already_has_hb(pl);
}

/* Get Preloader Headerblock Size */
int sec_get_hb_size_adapter(enum flash_dev_type_e fd, const struct data_buffer_t *pl,
                    uint32_t *hb_len)
{
    return sec_get_hb_size(fd, pl, hb_len);
}

/* Create Preloader Headerblock */
int sec_create_loader_hb_adapter(enum flash_dev_type_e fd,
                                 const struct data_buffer_t *pl,
                                 struct data_buffer_t *hb)
{
    return sec_create_loader_hb(fd, pl, hb);
}

/* Update Certificate Headerblock */
int sec_get_updated_cert_hb_adapter(enum flash_dev_type_e fd,
                                    struct data_buffer_t *hb,
                                    const struct data_buffer_t *cert,
                                    uint32_t *cert_offset)
{
    return sec_get_updated_cert_hb(fd, hb, cert, cert_offset);
}

/* Erase Preader Headerblock For Certificate */
int sec_erase_header_block_for_cert_adapter(struct data_buffer_t *hb)
{
    return sec_erase_header_block_for_cert(hb);
}

/* Get Preloader Certificate Offset */
uint32_t sec_get_cert_offset_adapter(struct data_buffer_t *hb)
{
    return sec_get_cert_offset(hb);
}

/* Storage Read Adapter On Top Of BIO APIs */
int sec_dev_read_wrapper(char *part_name, uint64_t offset, uint8_t *data, uint32_t size)
{
    int ret = 0;
    bdev_t *bdev = NULL;

    bdev = bio_open_by_label(part_name);
    if (!bdev) {
        dprintf(CRITICAL, "failed to open %s\n", part_name);
        return -EINVAL;
    }
    ret = bio_read(bdev, data, offset, size);
    bio_close(bdev);
    if (ret < 0)
        return ret;
    return 0;
}

/* Storage Write Adapter On Top Of BIO APIs */
int sec_dev_write_wrapper(char *part_name, uint64_t offset, uint8_t *data, uint32_t size)
{
    int ret = 0;
    bdev_t *bdev = NULL;

    bdev = bio_open_by_label(part_name);
    if (!bdev) {
        dprintf(CRITICAL, "failed to open %s\n", part_name);
        return -EINVAL;
    }
    ret = bio_write(bdev, data, offset, size);
    bio_close(bdev);
    if (ret < 0)
        return ret;
    return 0;
}

/* otp ver update */
int sec_otp_ver_update_adapter(uint32_t boot_mode)
{
    int slot = 0;
    int ret = 0;

    if (is_ab_enable()) {
        slot = check_suffix_with_slot(get_suffix());
        if (get_bctrl_mdata(slot, BOOTCTRL_SUCCESS)) {
            dprintf(CRITICAL, "[SEC] Update version, boot successfully on slot %d\n", slot);
#if (MTK_SECURITY_SW_SUPPORT && MTK_SECURITY_ANTI_ROLLBACK)
#if (MTK_SECURITY_ENABLE_SUB_GROUP)
            ret = sec_otp_ver_update(boot_mode, EN_SUBGROUP);
#else
            ret = sec_otp_ver_update(boot_mode, DIS_SUBGROUP);
#endif
#else
            ret = 0;
#endif
        } else {
            dprintf(CRITICAL, "[SEC] Bypass version update due to boot failure on slot %d\n", slot);
        }
    } else {
        dprintf(CRITICAL, "[SEC] Update version, boot successfully\n");
#if (MTK_SECURITY_SW_SUPPORT && MTK_SECURITY_ANTI_ROLLBACK)
#if (MTK_SECURITY_ENABLE_SUB_GROUP)
        ret = sec_otp_ver_update(boot_mode, EN_SUBGROUP);
#else
        ret = sec_otp_ver_update(boot_mode, DIS_SUBGROUP);
#endif
#else
        ret = 0;
#endif
    }

    return ret;
}

/* Preloader Image Authentication For Fastboot Security */
int sec_pl_auth_adpater(int8_t *pl_addr, uint32_t pl_len)
{
    return sec_pl_auth(pl_addr, pl_len);
}

uint32_t sec_img_auth_buf_adapter(uint8_t *img_buf, uint32_t img_buf_sz,
                                           uint8_t *img_name, uint32_t oem_opt)
{
    return sec_img_auth_buf(img_buf, img_buf_sz, img_name, oem_opt);
}

/* Cust Key Set, Get And Clear For AVB Yellow State */
int sec_set_cust_pubk_adpater(uint8_t *pubk, uint32_t pubk_size)
{
    return sec_set_cust_pubk(pubk, pubk_size);
}

int sec_get_cust_pubk_adpater(uint8_t *pubk, uint32_t pubk_size)
{
    return sec_get_cust_pubk(pubk, pubk_size);
}

int sec_clear_cust_pubk_adpater(void)
{
    return sec_clear_cust_pubk();
}

void sec_otp_set_da_ver_adpter(void)
{
#if (MTK_SECURITY_ANTI_ROLLBACK && MTK_SECURITY_UPDATE_DA_ARB)
    sec_otp_set_da_ver(DA_VER);
#endif
}
