/*
 * Copyright (c) 2021-2022 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <string.h>
#include <sys/types.h>
#include "verified_boot_common.h"
#include <verified_boot_error.h>
#include <verified_boot.h>

struct sub_group_table sub_table[] = {
    {"da",                   SUB_GROUP_DA},
    {"lk",                   SUB_GROUP_LK},
    {"pi_img",               SUB_GROUP_LK},
    {"logo",                 SUB_GROUP_LK},
    {"atf",                  SUB_GROUP_TEE},
    {"tee",                  SUB_GROUP_TEE},
    {"tinysys-sspm",         SUB_GROUP_TINYSYS},
    {"tinysys-mcupm",        SUB_GROUP_TINYSYS},
    {"tinysys-mcupm-RV33_A", SUB_GROUP_TINYSYS}
};

uint8_t get_sub_group(const char *img_name)
{
    uint32_t sub_group_num = countof(sub_table);

    if (img_name)
        for (int i = 0; i < sub_group_num; i++)
            if (strcmp(img_name, sub_table[i].image_name) == 0)
                return sub_table[i].sub_group;

    return SUB_GROUP_MAX;
}

struct otp_group_info otp_group_list[] = {
/* {GROUP, unit, 0, 0, 0, 0, 0},
 * The maximum version for each GROUP is (unit * 32 - 1)
 * Sum of all units should be smaller than or equal to 8.
 * otp group list support customization, but MISC_GROUP
 * unit only support value 1 currently.
 */
    {SECURE_GROUP,     1, 0, 0, 0, 0, 0}, /* DO NOT modify secure group info */
    {NON_SECURE_GROUP, 2, 0, 0, 0, 0, 0},
    {AVB_GROUP,        2, 0, 0, 0, 0, 0},
    {RECOVERY_GROUP,   2, 0, 0, 0, 0, 0},
    {MISC_GROUP,       1, 0, 0, 0, 0, 0}
};

uint32_t get_otp_group_num(void)
{
    return countof(otp_group_list);
}
