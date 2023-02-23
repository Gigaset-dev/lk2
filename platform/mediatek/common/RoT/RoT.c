/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <avb_ops.h>
#include <avb_property_descriptor.h>
#include <avb_slot_verify.h>
#include <debug.h>
#include <err.h>
#include <lock_state.h>
#include <RoT.h>
#include <sboot.h>
#include <smc.h>
#include <smc_id_table.h>
#include <stdint.h>
#include <string.h>
#include <verified_boot_state.h>

#define AVB_ROT_DEBUG                       (0)
#define SHA256_LENGTH                       (32)
#define SHA384_LENGTH                       (48)

#define OS_VERSION_KEY_IDX                  (0)
#define OS_PATCHLEVEL_KEY_IDX               (1)
#define VENDOR_PATCHLEVEL_KEY_IDX           (2)
#define BOOT_PATCHLEVEL_KEY_IDX             (3)
#define NUM_AVB_VERSIONS                    (4)

static uint32_t rot_boot_state;

struct vb_version {
    uint32_t os_version;
    uint32_t os_patchlevel;
    uint32_t vendor_patchlevel;
    uint32_t boot_patchlevel;
};

static struct vb_version g_vb_version;
static uint8_t g_vbmeta_digest_sha256[SHA256_LENGTH] = {0};

static const char *avb_version_key[NUM_AVB_VERSIONS] = {
    "com.android.build.system.os_version",
    "com.android.build.system.security_patch",
    "com.android.build.vendor.security_patch",
    "com.android.build.boot.security_patch",
};

static const char *avb_version_str[NUM_AVB_VERSIONS] = {NULL};

static uint32_t version_str_to_uint(uint32_t ver_type, const char *str,
                                    uint32_t str_sz)
{
#define MAX_OS_VER_NUM (3)
    uint32_t val = 0;
    uint32_t i = 0;
    uint32_t idx = 0;
    uint32_t xx[MAX_OS_VER_NUM] = {0};
    uint32_t yyyy = 0;
    uint32_t mm = 0;
    uint32_t dd = 0;

    /* return 0 for invalid version string */
    if (str == NULL || str_sz == 0)
        return 0;

    switch (ver_type) {
    case OS_VERSION_KEY_IDX:
        /* assume os_version format is
         * xx[0] or
         * xx[0].xx[1] or
         * xx[0].xx[1].xx[2]
         */
        if (str_sz == 1 && (str[0] >= 'M' && str[0] <= 'Z')) {
            xx[0] = str[0] - 'M' + 6;
        } else {
              for (i = 0; i < str_sz; i++) {
                  if (str[i] == '.') {
                      idx++;
                      if (idx == MAX_OS_VER_NUM)
                          break;
                  } else if (str[i] < '0' || str[i] > '9') {
                        dprintf(INFO, "[%s] invalid os_version\n", __func__);
                        return 0;
                    }
              xx[idx] = xx[idx] * 10 + (str[i] - '0');
              }
          }

        /* version overflow */
        for (idx = 0; idx < MAX_OS_VER_NUM; idx++)
            if (xx[idx] >= 100)
                xx[idx] = 99;

        val =  10000 * xx[0] + 100 * xx[1] + xx[2];
        break;
    case VENDOR_PATCHLEVEL_KEY_IDX:
    case BOOT_PATCHLEVEL_KEY_IDX:
    case OS_PATCHLEVEL_KEY_IDX:
        /* assume its format is YYYY-MM or YYYY-MM-DD */
        if (str_sz != 7 && str_sz != 10) {
            val = 0;
            break;
        }
        /* str_sz = 7 or 10 */
        for (i = 0; i < 4; i++) {
            if (str[i] < '0' && str[i] > '9')
                return 0;

            yyyy = yyyy * 10 + (uint32_t)(str[i] - '0');
        }

        val = yyyy;

        if (str[4] != '-')
            return 0;

        for (i = 5; i < 7; i++) {
            if (str[i] < '0' && str[i] > '9')
                return 0;

            mm = mm * 10 + (uint32_t)(str[i] - '0');
        }

        val = val * 100 + mm;

        if (str_sz == 10) {
            if (str[7] != '-')
                return 0;

            for (i = 8; i < 10; i++) {
                if (str[i] < '0' && str[i] > '9')
                    return 0;

                dd = dd * 10 + (uint32_t)(str[i] - '0');
            }

            val = val * 100 + dd;
        }

        break;
    default:
        return 0;
    }

    if (ver_type == OS_PATCHLEVEL_KEY_IDX && val >= 1000000) {
    /* DD is ignored for os_patchlevel */
        val /= 100;
    }

    return val;
}

uint32_t collect_rot_info(AvbSlotVerifyData *slot_data)
{
    uint32_t ret = 0;
    uint32_t i = 0;
    uint32_t j = 0;
    size_t out_val_size = 0;

    for (i = 0; i < slot_data->num_vbmeta_images; i++) {
        for (j = 0; j < NUM_AVB_VERSIONS; j++) {
            const char *val;

            if (avb_version_str[j])
                continue;

            val = avb_property_lookup(slot_data->vbmeta_images[i].vbmeta_data,
                      slot_data->vbmeta_images[i].vbmeta_size,
                      avb_version_key[j],
                      0,
                      &out_val_size);

            if (out_val_size) {
                dprintf(INFO, "%s is found in idx 0x%x, val = %s\n", avb_version_key[j], i, val);
                avb_version_str[j] = val;
            }
            out_val_size = 0;
        }
    }

    /* if os_version is not found, set it to "10" */
    if (avb_version_str[OS_VERSION_KEY_IDX] == NULL)
        avb_version_str[OS_VERSION_KEY_IDX] = "10";

    if (avb_version_str[OS_VERSION_KEY_IDX]) {
        g_vb_version.os_version = version_str_to_uint(OS_VERSION_KEY_IDX,
            avb_version_str[OS_VERSION_KEY_IDX],
            strlen(avb_version_str[OS_VERSION_KEY_IDX]));
    }

    if (avb_version_str[OS_PATCHLEVEL_KEY_IDX]) {
        g_vb_version.os_patchlevel = version_str_to_uint(OS_PATCHLEVEL_KEY_IDX,
            avb_version_str[OS_PATCHLEVEL_KEY_IDX],
            strlen(avb_version_str[OS_PATCHLEVEL_KEY_IDX]));
    }

    if (avb_version_str[VENDOR_PATCHLEVEL_KEY_IDX]) {
        g_vb_version.vendor_patchlevel = version_str_to_uint(VENDOR_PATCHLEVEL_KEY_IDX,
            avb_version_str[VENDOR_PATCHLEVEL_KEY_IDX],
            strlen(avb_version_str[VENDOR_PATCHLEVEL_KEY_IDX]));
    }

    if (avb_version_str[BOOT_PATCHLEVEL_KEY_IDX]) {
        g_vb_version.boot_patchlevel = version_str_to_uint(BOOT_PATCHLEVEL_KEY_IDX,
            avb_version_str[BOOT_PATCHLEVEL_KEY_IDX],
            strlen(avb_version_str[BOOT_PATCHLEVEL_KEY_IDX]));
    }

    avb_slot_verify_data_calculate_vbmeta_digest(
        slot_data, AVB_DIGEST_TYPE_SHA256, g_vbmeta_digest_sha256);

#if (AVB_ROT_DEBUG)
    for (i = 0; i < NUM_AVB_VERSIONS; i++)
        dprintf(ALWAYS, "avb version str[%s] = %s\n", avb_version_key[i],
            avb_version_str[i] == NULL ? "NOT FOUND" : avb_version_str[i]);

    dprintf(ALWAYS, "os_version = %u\n", g_vb_version.os_version);
    dprintf(ALWAYS, "os_patchlevel = %u\n", g_vb_version.os_patchlevel);
    dprintf(ALWAYS, "vendor_patchlevel = %u\n", g_vb_version.vendor_patchlevel);
    dprintf(ALWAYS, "boot_patchlevel = %u\n", g_vb_version.boot_patchlevel);
    dprintf(ALWAYS, "vbmeta_digest =\n");
    for (i = 0; i < AVB_SHA256_DIGEST_SIZE; i++)
        dprintf(INFO, "0x%x\n", g_vbmeta_digest_sha256[i]);
#endif /* AVB_ROT_DEBUG */

    return ret;
}

void send_root_of_trust_info(void)
{
    uint8_t public_key[PUBK_LEN] = {0};
    uint8_t public_key_hash[SHA256_LENGTH] = {0};
    int device_lock_state = DEVICE_STATE_LOCKED;
    uint32_t smc_ret = 0;
    struct __smccc_res res;

    dprintf(ALWAYS, "sending root of trust info...\n");

    /* read public key and generate SHA256(pubk) */
    if (sec_get_pubk_adapter(public_key, PUBK_LEN))
        dprintf(ALWAYS, "fail to get public key info\n");

    if (sec_sha256_adapter(public_key, PUBK_LEN, public_key_hash))
        dprintf(ALWAYS, "fail to calculate public key hash\n");

    /* query device lock */
    if (sec_query_device_lock_adapter(&device_lock_state))
        dprintf(ALWAYS, "fail to get device lock state\n");

    /* send to ARM-TF via SMC call */
    uint64_t *p_hash = (uint64_t *)public_key_hash;
    uint64_t *p_vbmeta_digest = (uint64_t *)g_vbmeta_digest_sha256;

    /* send info. and the sequence : pubk(32B), device state(4B), boot state(4B), os version(4B) */
    /* Keymaster Requirments: Device Lock, Verified Boot Key Effective;
     *                        Device Unlock, Verified Boot Key Remains 0
     */
    if (device_lock_state == DEVICE_STATE_UNLOCKED)
        __smc_conduit(MTK_SIP_LK_ROOT_OF_TRUST, 0, 0, 0, 0, 0, 0, 0, &res);
    else
        __smc_conduit(MTK_SIP_LK_ROOT_OF_TRUST, *(p_hash), *(p_hash + 1),
        *(p_hash + 2), *(p_hash + 3), 0, 0, 0, &res);

    __smc_conduit(MTK_SIP_LK_ROOT_OF_TRUST, device_lock_state, rot_boot_state,
    g_vb_version.os_version, g_vb_version.os_patchlevel, 0, 0, 0, &res);
    __smc_conduit(MTK_SIP_LK_ROOT_OF_TRUST, *(p_vbmeta_digest), *(p_vbmeta_digest + 1),
    *(p_vbmeta_digest + 2), *(p_vbmeta_digest + 3), 0, 0, 0, &res);
    __smc_conduit(MTK_SIP_LK_ROOT_OF_TRUST, g_vb_version.vendor_patchlevel,
    g_vb_version.boot_patchlevel, 0, 0, 0, 0, 0, &res);

    if (res.a0)
        dprintf(INFO, "fail to send root of trust info. : 0x%lx\n", res.a0);

    /* to test if SMC call is locked after previous 4 calls */
    __smc_conduit(MTK_SIP_LK_ROOT_OF_TRUST, 0, 0, 0, 0, 0, 0, 0, &res);
    if (!res.a0)
        dprintf(INFO, "Warning! root of trust smc call is not locked : 0x%lx\n", res.a0);

}

void set_rot_boot_state(uint32_t state)
{
    rot_boot_state = state;
}
