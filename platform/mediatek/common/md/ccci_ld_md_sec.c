/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <smc.h>
#include <smc_id_table.h>

#include "ccci_ld_md_log.h"
#include "ccci_ld_md_sec.h"

#define LOCAL_TRACE 0

int ccci_apply_md_bank0_1_hw_remap(unsigned long long addr)
{
    struct __smccc_res res;
    unsigned long long md_img_start_addr = addr;

    __smc_conduit(MTK_SIP_BL_CCCI_CONTROL, MD_BANK0_HW_REMAP,
                  (u32)md_img_start_addr, (u32)(md_img_start_addr >> 32),
                  0, 0, 0, 0, &res);

    if (res.a0 == (size_t)CCCI_SECURE_RET_VALUE_LK) {
        CCCI_TRACE_LOG("[%s] error: MD_BANK0_HW_REMAP fail: 0x%lX\n", __func__,
                    res.a0);
        return -1;

    } else {
        ALWAYS_LOG("BANK0_MAP0 value:0x%lX\n", res.a1);
        ALWAYS_LOG("BANK0_MAP1 value:0x%lX\n", res.a2);
        ALWAYS_LOG("BANK0_MAP2 value:0x%lX\n", res.a3);
    }

    /* md_img_start_addr = addr + 0x10000000; *//* 256M~512M ATF will add  */
    __smc_conduit(MTK_SIP_BL_CCCI_CONTROL, MD_BANK1_HW_REMAP,
                  (u32)md_img_start_addr, (u32)(md_img_start_addr >> 32),
                  0, 0, 0, 0, &res);

    if (res.a0 == (size_t)CCCI_SECURE_RET_VALUE_LK) {
        CCCI_TRACE_LOG("[%s] error: MD_BANK1_HW_REMAP fail: 0x%lX\n", __func__,
                    res.a0);
        return -1;

    } else {
        ALWAYS_LOG("BANK1_MAP0 value:0x%lX\n", res.a0);
        ALWAYS_LOG("BANK1_MAP1 value:0x%lX\n", res.a1);
        ALWAYS_LOG("BANK1_MAP2 value:0x%lX\n", res.a2);
        ALWAYS_LOG("BANK1_MAP3 value:0x%lX\n", res.a3);
    }

    return 0;
}

int ccci_apply_md_bank4_remap_by_slot(unsigned long long addr, int slot)
{
    struct __smccc_res res;

    ALWAYS_LOG("[%s] addr: 0x%llX\n", __func__, addr);

    __smc_conduit(MTK_SIP_BL_CCCI_CONTROL, MD_BANK4_HW_REMAP,
                  (u32)addr, (u32)(addr >> 32),
                  (u32)slot, 0, 0, 0, &res);

    if (res.a0 == (size_t)CCCI_SECURE_RET_VALUE_LK) {
        CCCI_TRACE_LOG("[%s] error: MD_BANK4_HW_REMAP fail: 0x%lX\n", __func__,
                    res.a0);
        return -1;
    } else
        ALWAYS_LOG("BANK4_MAP(%d) value:0x%lX\n", slot, res.a1);

    return 0;

}

int ccci_apply_md_sib_remapping(unsigned long long addr)
{
    struct __smccc_res res;

    ALWAYS_LOG("[%s] addr: 0x%llX\n", __func__, addr);

    __smc_conduit(MTK_SIP_BL_CCCI_CONTROL, MD_SIB_HW_REMAP,
        (u32)addr, (u32)(addr >> 32), 0, 0, 0, 0, &res);

    if (res.a0 == (size_t)CCCI_SECURE_RET_VALUE_LK) {
        ALWAYS_LOG("[%s] error: MD_SIB_HW_REMAP fail: 0x%lX\n", __func__, res.a0);
        return -1;

    } else {
        ALWAYS_LOG("SIBLOG_BANK_MAP0 value:0x%lX\n", res.a1);
        ALWAYS_LOG("SIBLOG_BANK_MAP1 value:0x%lX\n", res.a2);
        ALWAYS_LOG("SIBLOG_BANK_MAP2 value:0x%lX\n", res.a3);
    }

    return 0;
}

void ccci_set_hw_locked(void)
{
    struct __smccc_res res;

    ALWAYS_LOG("start\n");

    __smc_conduit(MTK_SIP_BL_CCCI_CONTROL, MD_HW_REMAP_LOCKED,
        0, 0, 0, 0, 0, 0, &res);

    if (res.a0 == (size_t)CCCI_SECURE_RET_VALUE_LK)
        ALWAYS_LOG("error: MD_HW_REMAP_LOCKED fail: 0x%lX\n", res.a0);
    else
        ALWAYS_LOG("call MD_HW_REMAP_LOCKED succ.\n");
}

int ccci_let_md_go(void)
{
    struct __smccc_res res;

    ALWAYS_LOG("Trigger MD run\n");
    __smc_conduit(MTK_SIP_BL_CCCI_CONTROL, MD_POWER_CONFIG, MD_LK_BOOT_UP, 0, 0, 0, 0, 0, &res);
    ALWAYS_LOG("md_power_value=0x%lx, HW code=0x%lx, SW_version=0x%lx\n", res.a0, res.a1, res.a2);

    if (res.a0)
        return -1;
    return 0;
}
