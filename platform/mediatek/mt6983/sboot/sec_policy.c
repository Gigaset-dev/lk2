/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <debug.h>
#include <lock_state.h>
#include <sboot.h>
#include <sec_policy.h>
#include <string.h>
#include <verified_boot.h>
#include <verified_boot_state.h>

#define VFY_EN                  (0x1)
#define DL_ALLOW                (0x2)

#define VFY_DIS_DL_FORBIDDEN    (0x0)
#define VFY_DIS_DL_ALLOW        DL_ALLOW
#define VFY_EN_DL_FORBIDDEN     VFY_EN
#define VFY_EN_DL_ALLOW         (VFY_EN|DL_ALLOW)

#define SHA256_HASH_SZ (32)
unsigned char g_binding_hash[SHA256_HASH_SZ] = {0};

struct policy_part_map g_policy_map[] = {
    {"default",
    0,
    0,
    0,
    VFY_DIS_DL_FORBIDDEN,
    VFY_DIS_DL_ALLOW,
    VFY_EN_DL_FORBIDDEN,
    VFY_DIS_DL_ALLOW},
    {"preloader",
    0,
    0,
    0,
    VFY_DIS_DL_FORBIDDEN,
    VFY_DIS_DL_ALLOW,
    VFY_EN_DL_FORBIDDEN,
    VFY_DIS_DL_ALLOW},
    {"lk",
    "lk_a",
    "lk_b",
    0,
    VFY_DIS_DL_FORBIDDEN,
    VFY_DIS_DL_ALLOW,
    VFY_EN_DL_FORBIDDEN,
    VFY_DIS_DL_ALLOW},
    {"logo",
    "logo_a",
    "logo_b",
    0,
    VFY_DIS_DL_FORBIDDEN,
    VFY_DIS_DL_ALLOW,
    VFY_EN_DL_FORBIDDEN,
    VFY_DIS_DL_ALLOW},
    {"boot",
    "boot_a",
    "boot_b",
    0,
    VFY_DIS_DL_FORBIDDEN,
    VFY_DIS_DL_ALLOW,
    VFY_EN_DL_FORBIDDEN,
    VFY_DIS_DL_ALLOW},
    {"recovery",
    "recovery_a",
    "recovery_b",
    0,
    VFY_DIS_DL_FORBIDDEN,
    VFY_DIS_DL_ALLOW,
    VFY_EN_DL_FORBIDDEN,
    VFY_DIS_DL_ALLOW},
    {"system",
    0,
    0,
    0,
    VFY_DIS_DL_FORBIDDEN,
    VFY_DIS_DL_ALLOW,
    VFY_EN_DL_FORBIDDEN,
    VFY_DIS_DL_ALLOW},
    {"tee",
    "tee_a",
    "tee_b",
    0,
    VFY_DIS_DL_FORBIDDEN,
    VFY_DIS_DL_ALLOW,
    VFY_EN_DL_FORBIDDEN,
    VFY_DIS_DL_ALLOW},
    {"tee1",
    "tee1_a",
    "tee1_b",
    0,
    VFY_DIS_DL_FORBIDDEN,
    VFY_DIS_DL_ALLOW,
    VFY_EN_DL_FORBIDDEN,
    VFY_DIS_DL_ALLOW},
    {"tee2",
    "tee2_a",
    "tee2_b",
    0,
    VFY_DIS_DL_FORBIDDEN,
    VFY_DIS_DL_ALLOW,
    VFY_EN_DL_FORBIDDEN,
    VFY_DIS_DL_ALLOW},
    {"oemkeystore",
    0,
    0,
    0,
    VFY_DIS_DL_FORBIDDEN,
    VFY_DIS_DL_ALLOW,
    VFY_EN_DL_FORBIDDEN,
    VFY_DIS_DL_ALLOW},
    {"keystore",
    0,
    0,
    0,
    VFY_DIS_DL_FORBIDDEN,
    VFY_DIS_DL_ALLOW,
    VFY_EN_DL_FORBIDDEN,
    VFY_DIS_DL_ALLOW},
    {"userdata",
    0,
    0,
    0,
    VFY_DIS_DL_FORBIDDEN,
    VFY_DIS_DL_ALLOW,
    VFY_EN_DL_FORBIDDEN,
    VFY_DIS_DL_ALLOW},
    {"md1img",
    0,
    0,
    0,
    VFY_DIS_DL_FORBIDDEN,
    VFY_DIS_DL_ALLOW,
    VFY_EN_DL_FORBIDDEN,
    VFY_DIS_DL_ALLOW},
    {"md1dsp",
    0,
    0,
    0,
    VFY_DIS_DL_FORBIDDEN,
    VFY_DIS_DL_ALLOW,
    VFY_EN_DL_FORBIDDEN,
    VFY_DIS_DL_ALLOW},
    {"md1arm7",
    0,
    0,
    0,
    VFY_DIS_DL_FORBIDDEN,
    VFY_DIS_DL_ALLOW,
    VFY_EN_DL_FORBIDDEN,
    VFY_DIS_DL_ALLOW},
    {"md3img",
    0,
    0,
    0,
    VFY_DIS_DL_FORBIDDEN,
    VFY_DIS_DL_ALLOW,
    VFY_EN_DL_FORBIDDEN,
    VFY_DIS_DL_ALLOW},
    {"scp1",
    0,
    0,
    0,
    VFY_DIS_DL_FORBIDDEN,
    VFY_DIS_DL_ALLOW,
    VFY_EN_DL_FORBIDDEN,
    VFY_DIS_DL_ALLOW},
    {"scp2",
    0,
    0,
    0,
    VFY_DIS_DL_FORBIDDEN,
    VFY_DIS_DL_ALLOW,
    VFY_EN_DL_FORBIDDEN,
    VFY_DIS_DL_ALLOW},
    {"spmfw",
    0,
    0,
    0,
    VFY_DIS_DL_FORBIDDEN,
    VFY_DIS_DL_ALLOW,
    VFY_EN_DL_FORBIDDEN,
    VFY_DIS_DL_ALLOW},
    {"mcupmfw",
    0,
    0,
    0,
    VFY_DIS_DL_FORBIDDEN,
    VFY_DIS_DL_ALLOW,
    VFY_EN_DL_FORBIDDEN,
    VFY_DIS_DL_ALLOW},
    {"dtbo",
    0,
    0,
    0,
    VFY_DIS_DL_FORBIDDEN,
    VFY_DIS_DL_ALLOW,
    VFY_EN_DL_FORBIDDEN,
    VFY_DIS_DL_ALLOW},
    /*End of array*/
    {"NULL", "NULL", "NULL", "NULL", 0, 0, 0, 0},
};

static unsigned int get_policy_entry_idx(const char *part_name)
{
    unsigned int i = 0;
    unsigned int num_of_entries = sizeof(g_policy_map)/sizeof(struct policy_part_map);

    if (!part_name) {
        dprintf(CRITICAL, "[SEC_POLICY] part_name invalid, use default policy\n");
        return 0;
    }
    while (i < num_of_entries) {
        if (g_policy_map[i].part_name1 &&
            (strcmp(part_name, g_policy_map[i].part_name1) == 0))
            break;
        else if (g_policy_map[i].part_name2 &&
            (strcmp(part_name, g_policy_map[i].part_name2) == 0))
            break;
        else if (g_policy_map[i].part_name3 &&
            (strcmp(part_name, g_policy_map[i].part_name3) == 0))
            break;
        else if (g_policy_map[i].part_name4 &&
            (strcmp(part_name, g_policy_map[i].part_name4) == 0))
            break;
        i++;
    }

    if (i == num_of_entries)
        i = 0;

    return i;
}

static unsigned int get_sec_state(uint32_t *sboot_state, uint32_t *lock_state)
{
    int ret = 0;

    ret = get_sboot_state(sboot_state);
    if (ret != 0)
        goto end;

    ret = sec_get_lock_state_adapter(lock_state);
    if (ret != 0)
        goto end;

end:
    return ret;
}

static unsigned int get_sec_policy(unsigned int policy_entry_idx)
{
    uint32_t sboot_state = 0;
    uint32_t lock_state = 0;
    unsigned char sec_policy = 0;
    unsigned int ret = 0;

    ret = get_sec_state(&sboot_state, &lock_state);
    if (ret) /* security policy not available */
        return 0;

    if (sboot_state == 0 && lock_state == LKS_UNLOCK)
        sec_policy = g_policy_map[policy_entry_idx].sec_sbcdis_unlock_policy;
    else if (sboot_state == 0 && lock_state != LKS_UNLOCK)
        sec_policy = g_policy_map[policy_entry_idx].sec_sbcdis_lock_policy;
    else if (sboot_state == 1 && lock_state == LKS_UNLOCK)
        sec_policy = g_policy_map[policy_entry_idx].sec_sbcen_unlock_policy;
    else if (sboot_state == 1 && lock_state != LKS_UNLOCK)
        sec_policy = g_policy_map[policy_entry_idx].sec_sbcen_lock_policy;

    return sec_policy;
}

/* bypass auth check: return 0, need auth check: return 1 */
bool sec_get_vfy_policy(const char *part_name)
{
    unsigned int sec_policy = 0;
    unsigned int policy_entry_idx = 0;

    if (part_name == NULL)
        return true;

    policy_entry_idx = get_policy_entry_idx(part_name);
    sec_policy = get_sec_policy(policy_entry_idx);
    if (sec_policy & VFY_EN) {
        /* need verify */
        return true;
    } else {
        /* skip verify */
        return false;
    }
}

/* bypass dl check: return 0, need dl check: return 1 */
bool sec_get_dl_policy(const char *part_name)
{
    unsigned int sec_policy = 0;
    unsigned int policy_entry_idx = 0;

    if (part_name == NULL)
        return false;

    policy_entry_idx = get_policy_entry_idx(part_name);
    sec_policy = get_sec_policy(policy_entry_idx);

    if (sec_policy & DL_ALLOW) {
        /* dl is permitted */
        return true;
    } else {
        /* dl is forbidden */
        return false;
    }
}

