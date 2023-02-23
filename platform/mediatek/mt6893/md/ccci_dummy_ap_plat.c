/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <debug.h>
#include <mtcmos.h>
#include <platform/addressmap.h>
#include <platform/reg.h>

#include "ccci_ld_md_log.h"
#include "ccci_ld_md_tag_dt.h"

#define LOCAL_TRACE 0

//============================================================================
// The following codes need porting for each new chip !!!!!
//============================================================================
#define ccci_write32(a, v)    writel(v, a)
#define ccci_read32(a)        readl(a)

//------- register part ---------------------------------------
#define MD1_BUS_PROTECT_SET    (INFRACFG_AO_BASE + 0x00001B84)
#define MD1_BUS_PROTECT_CLR    (INFRACFG_AO_BASE + 0x00001B88)
#define MD1_BUS_PROTECT_STA    (INFRACFG_AO_BASE + 0x00001B90)
#define MD1_BUS_PROTECT1_SET   (INFRACFG_AO_BASE + 0x000012A0)
#define MD1_BUS_PROTECT1_CLR   (INFRACFG_AO_BASE + 0x000012A4)
#define MD1_BUS_PROTECT1_STA   (INFRACFG_AO_BASE + 0x00001228)
#define MD1_BUS_PROTECT2_SET   (INFRACFG_AO_BASE + 0x00001BB8)
#define MD1_BUS_PROTECT2_CLR   (INFRACFG_AO_BASE + 0x00001BBC)
#define MD1_BUS_PROTECT2_STA   (INFRACFG_AO_BASE + 0x00001BC4)
#define PROTECTION_BITMASK     ((0x1 << 10)|(0x1 << 21)|(0x1 << 29)) // bit 10,21,29
#define PROTECTION1_BITMASK    (0x1 << 7) // bit 7
#define PROTECTION2_BITMASK    ((0x1 << 6)|(0x1 << 7)) // bit 6,7

#define TOPRGU_BASE            (RGU_BASE + 0x00007000)
#define TOP_RGU_WDT_MODE       (0x0)
#define TOP_RGU_WDT_SWRST      (0x14)
#define TOP_RGU_WDT_SWSYSRST   (0x18)
#define TOP_RGU_WDT_NONRST_REG (0x20)
#define TOP_RGU_LATCH_CONTROL  (0x44)
#define MD1_SYS                (1 << 7)
#define UNLOCK_KEY             (0x88000000)

#define CCIF0_BASE_DATA        (CCIF0_AP_BASE + 0x00000100)

//------- code part ---------------------------------------
void ccci_md_bus_protection_en(void)
{
    /* enable protection for MD1 */
    ALWAYS_LOG("enable protection for md\n");
    ccci_write32(MD1_BUS_PROTECT_SET, PROTECTION_BITMASK);
    ccci_write32(MD1_BUS_PROTECT1_SET, PROTECTION1_BITMASK);
    ccci_write32(MD1_BUS_PROTECT2_SET, PROTECTION2_BITMASK);
    /* poll protection ready */
    ALWAYS_LOG("wait protection ....\n");
    while ((ccci_read32(MD1_BUS_PROTECT_STA) & PROTECTION_BITMASK) != PROTECTION_BITMASK)
        ALWAYS_LOG("0x%x\n", ccci_read32(MD1_BUS_PROTECT_STA));

    ALWAYS_LOG("wait protection1 ....\n");
    while ((ccci_read32(MD1_BUS_PROTECT1_STA) & PROTECTION1_BITMASK) != PROTECTION1_BITMASK)
        ALWAYS_LOG("0x%x\n", ccci_read32(MD1_BUS_PROTECT1_STA));

    ALWAYS_LOG("wait protection2 ....\n");
    while ((ccci_read32(MD1_BUS_PROTECT2_STA)&PROTECTION2_BITMASK) != PROTECTION2_BITMASK)
        ALWAYS_LOG("0x%x\n", ccci_read32(MD1_BUS_PROTECT2_STA));

    ALWAYS_LOG("protection enable done\n");
}

void ccci_md_bus_protection_disable(void)
{
    /* enable protection for MD1 */
    ALWAYS_LOG("disable protection for md\n");
    ccci_write32(MD1_BUS_PROTECT_CLR, PROTECTION_BITMASK);
    ccci_write32(MD1_BUS_PROTECT1_CLR, PROTECTION1_BITMASK);
    ccci_write32(MD1_BUS_PROTECT2_CLR, PROTECTION2_BITMASK);
    /* poll protection ready */
    ALWAYS_LOG("wait protection disable....\n");
    while ((ccci_read32(MD1_BUS_PROTECT_STA) & PROTECTION_BITMASK) != 0x00000000)
        ALWAYS_LOG("0x%x\n", ccci_read32(MD1_BUS_PROTECT_STA));

    ALWAYS_LOG("wait protection1 disable....\n");
    while ((ccci_read32(MD1_BUS_PROTECT1_STA)&PROTECTION1_BITMASK) != 0x00000000)
        ALWAYS_LOG("0x%x\n", ccci_read32(MD1_BUS_PROTECT1_STA));

    ALWAYS_LOG("wait protection2 disable....\n");
    while ((ccci_read32(MD1_BUS_PROTECT2_STA)&PROTECTION2_BITMASK) != 0x00000000)
        ALWAYS_LOG("0x%x\n", ccci_read32(MD1_BUS_PROTECT2_STA));

    ALWAYS_LOG("protection disable done\n");
}


unsigned int ccci_get_ap_rgu_wdt_non_rst_reg_val(void)
{
    return ccci_read32(TOP_RGU_WDT_NONRST_REG);
}

void ccci_set_ap_rgu_wdt_non_rst_reg_val(unsigned int new_val)
{
    ccci_write32(TOP_RGU_WDT_NONRST_REG, new_val);
}

void ccci_using_ap_rgu_reset_md(void)
{
    ccci_write32(TOP_RGU_WDT_SWSYSRST,
            (ccci_read32(TOP_RGU_WDT_SWSYSRST) | UNLOCK_KEY) | MD1_SYS);
    ALWAYS_LOG("Power off MD......\n");
    spin(5000);
    ALWAYS_LOG("Power on MD......\n");
    ccci_write32(TOP_RGU_WDT_SWSYSRST,
            (ccci_read32(TOP_RGU_WDT_SWSYSRST) | UNLOCK_KEY) & (~MD1_SYS));
}

void ccci_md_special_init(void)
{
    unsigned long long base = 0ULL;
    unsigned int val;

    /* Update MD bank0 base to CCIF0 for MD USB */
    if (ccci_find_tag_inf("md_bank0_base", (char *)&base, sizeof(base)) == (int)sizeof(base)) {
        ALWAYS_LOG("MD bank0 base addr:0x%llX\n", base);
        val = (unsigned int)base;
        ccci_write32(CCIF0_BASE_DATA, val >> 24);
    } else
        ALWAYS_LOG("MD bank0 base addr:0x%llX (fail)\n", base);
}
