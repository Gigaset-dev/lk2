/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#define CCCI_SECURE_RET_VALUE_LK 0xfffffffffffffff1

enum CCCI_SECURE_REQ_ID {
    MD_DBGSYS_REG_DUMP = 0,
    MD_BANK0_HW_REMAP,
    MD_BANK1_HW_REMAP,
    MD_BANK4_HW_REMAP,
    MD_SIB_HW_REMAP,
    MD_CLOCK_REQUEST,
    MD_POWER_CONFIG,
    MD_FLIGHT_MODE_SET,
    MD_HW_REMAP_LOCKED, /* 8 */
};

enum MD_POWER_CONFIG_ID {
    MD_KERNEL_BOOT_UP,
    MD_LK_BOOT_UP,
    MD_CHECK_FLAG,
    MD_CHECK_DONE,
    MD_BOOT_STATUS,
    MD_LK_BOOT_UP_FOR_UNGATE,
    MD_LK_BYPASS_BROM,
    MD_LK_GET_BOOT_UP_STATUS,
    MD_LK_BOOT_PLAT,
};

int ccci_apply_md_bank0_1_hw_remap(unsigned long long addr);
int ccci_apply_md_bank4_remap_by_slot(unsigned long long addr, int slot);
int ccci_apply_md_bank4_remap(unsigned long long addr);
int ccci_apply_md_sib_remapping(unsigned long long addr);
void ccci_set_hw_locked(void);

int ccci_let_md_go(void);
