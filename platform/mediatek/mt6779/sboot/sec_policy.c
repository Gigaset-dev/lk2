/*
 * Copyright (c) 2019 MediaTek Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <compiler.h>
#include <debug.h>
#include <lock_state.h>
#include <sec_policy.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <trace.h>
#include <vb_state.h>

#define LOCAL_TRACE 0

#define VFY_EN                  0x1
#define DL_ALLOW                0x2

#define VFY_DIS_DL_FORBIDDEN    0x0
#define VFY_DIS_DL_ALLOW        DL_ALLOW
#define VFY_EN_DL_FORBIDDEN     VFY_EN
#define VFY_EN_DL_ALLOW         (VFY_EN|DL_ALLOW)


struct policy_part_map g_policy_map[] = {
    /* part_name1,   part_name2,  part_name3, part_name4,     sb_dis_lock,        sb_dis_unlock,       sb_en_lock,         sb_en_unlock */
    {"default",          "",         "",          "",      VFY_DIS_DL_FORBIDDEN,  VFY_DIS_DL_ALLOW,  VFY_EN_DL_FORBIDDEN,  VFY_DIS_DL_ALLOW},
    {"logo",             "",         "",          "",      VFY_DIS_DL_FORBIDDEN,  VFY_DIS_DL_ALLOW,  VFY_EN_DL_FORBIDDEN,  VFY_DIS_DL_ALLOW},
    {"dtbo",             "",         "",          "",      VFY_DIS_DL_FORBIDDEN,  VFY_DIS_DL_ALLOW,  VFY_EN_DL_FORBIDDEN,  VFY_DIS_DL_ALLOW},
    {"spmfw",            "",         "",          "",      VFY_DIS_DL_FORBIDDEN,  VFY_DIS_DL_ALLOW,  VFY_EN_DL_FORBIDDEN,  VFY_DIS_DL_ALLOW},
    {"mcupmfw",          "",         "",          "",      VFY_DIS_DL_FORBIDDEN,  VFY_DIS_DL_ALLOW,  VFY_EN_DL_FORBIDDEN,  VFY_DIS_DL_ALLOW},
    {"boot",             "",         "",          "",      VFY_DIS_DL_FORBIDDEN,  VFY_DIS_DL_ALLOW,  VFY_EN_DL_FORBIDDEN,  VFY_DIS_DL_ALLOW},
    {"recovery",         "",         "",          "",      VFY_DIS_DL_FORBIDDEN,  VFY_DIS_DL_ALLOW,  VFY_EN_DL_FORBIDDEN,  VFY_DIS_DL_ALLOW},
    {"vbmeta",           "",         "",          "",      VFY_DIS_DL_FORBIDDEN,  VFY_DIS_DL_ALLOW,  VFY_EN_DL_FORBIDDEN,  VFY_DIS_DL_ALLOW},
    {"vbmeta_system",    "",         "",          "",      VFY_DIS_DL_FORBIDDEN,  VFY_DIS_DL_ALLOW,  VFY_EN_DL_FORBIDDEN,  VFY_DIS_DL_ALLOW},
    {"vbmeta_vendor",    "",         "",          "",      VFY_DIS_DL_FORBIDDEN,  VFY_DIS_DL_ALLOW,  VFY_EN_DL_FORBIDDEN,  VFY_DIS_DL_ALLOW},
    {"super",            "",         "",          "",      VFY_DIS_DL_FORBIDDEN,  VFY_DIS_DL_ALLOW,  VFY_EN_DL_FORBIDDEN,  VFY_DIS_DL_ALLOW},
    {"system",           "",         "",          "",      VFY_DIS_DL_FORBIDDEN,  VFY_DIS_DL_ALLOW,  VFY_EN_DL_FORBIDDEN,  VFY_DIS_DL_ALLOW},
    {"vendor",           "",         "",          "",      VFY_DIS_DL_FORBIDDEN,  VFY_DIS_DL_ALLOW,  VFY_EN_DL_FORBIDDEN,  VFY_DIS_DL_ALLOW},
    {"product",          "",         "",          "",      VFY_DIS_DL_FORBIDDEN,  VFY_DIS_DL_ALLOW,  VFY_EN_DL_FORBIDDEN,  VFY_DIS_DL_ALLOW},
    /* End of array */
    {"NULL", "NULL", "NULL", "NULL", 0, 0, 0, 0},
};

int get_policy_entry_idx(const char *part_name)
{
    uint32_t i = 0;
    uint32_t num_of_entries = countof(g_policy_map);

    while (i < num_of_entries) {
        if (strcmp(part_name, g_policy_map[i].part_name1) == 0)
            break;
        else if (strcmp(part_name, g_policy_map[i].part_name2) == 0)
            break;
        else if (strcmp(part_name, g_policy_map[i].part_name3) == 0)
            break;
        else if (strcmp(part_name, g_policy_map[i].part_name4) == 0)
            break;
        i++;
    }

    if (i == num_of_entries) {
        LTRACEF("[SEC_POLICY] reached the end, use default policy.\n");
        i = 0;
    }

    return i;
}

static int get_sec_state(uint32_t *sboot_state, uint32_t *lock_state)
{
    int ret = 0;

    ret = get_sboot_state(sboot_state);
    if (ret != 0)
        goto end;

    ret = get_lock_state(lock_state);
    if (ret != 0)
        goto end;

    dprintf(CRITICAL, "[SEC_POLICY] sboot_state = 0x%x\n", *sboot_state);
    dprintf(CRITICAL, "[SEC_POLICY] lock_state = 0x%x\n", *lock_state);

end:
    return 0;
}

static uint32_t get_sec_policy(uint32_t policy_entry_idx)
{
    uint32_t ret = 0;
    uint32_t sboot_state = 0;
    uint32_t lock_state = 0;
    uint32_t sec_policy = 0;

    ret = get_sec_state(&sboot_state, &lock_state);
    if (ret) /* security policy not available */
        return VFY_EN_DL_FORBIDDEN;

    if (sboot_state == 0 && lock_state == DEVICE_STATE_UNLOCKED)
        sec_policy = g_policy_map[policy_entry_idx].sec_sbcdis_unlock_policy;
    else if (sboot_state == 0 && lock_state != DEVICE_STATE_UNLOCKED)
        sec_policy = g_policy_map[policy_entry_idx].sec_sbcdis_lock_policy;
    else if (sboot_state == 1 && lock_state == DEVICE_STATE_UNLOCKED)
        sec_policy = g_policy_map[policy_entry_idx].sec_sbcen_unlock_policy;
    else if (sboot_state == 1 && lock_state != DEVICE_STATE_UNLOCKED)
        sec_policy = g_policy_map[policy_entry_idx].sec_sbcen_lock_policy;

    return sec_policy;
}

/* bypass auth check: return false, need auth check: return true */
bool get_vfy_policy(const char *part_name)
{
    uint32_t sec_policy = 0;
    uint32_t policy_entry_idx = 0;

    if (part_name == NULL)
        return true;

    policy_entry_idx = get_policy_entry_idx(part_name);
    sec_policy = get_sec_policy(policy_entry_idx);

    if (sec_policy & VFY_EN)
        /* need verify */
        return true;
    else
        /* skip verify */
        return false;
}

/* forbid dl: return false, allow dl: return true */
bool get_dl_policy(const char *part_name)
{
    uint32_t sec_policy = 0;
    uint32_t policy_entry_idx = 0;

    if (part_name == NULL)
        return false;

    policy_entry_idx = get_policy_entry_idx(part_name);
    sec_policy = get_sec_policy(policy_entry_idx);

    if (sec_policy & DL_ALLOW)
        /* dl is permitted */
        return true;
    else
        /* dl is forbidden */
        return false;
}
