/*
 * Copyright (c) 2022 MediaTek Inc.
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
#define CCIF0_BASE_DATA        (CCIF0_AP_BASE + 0x00000100)

//------- code part ---------------------------------------
unsigned int ccci_get_ap_rgu_wdt_non_rst_reg_val(void)
{
    return 0; /* Porting for MD WDT */
}

void ccci_set_ap_rgu_wdt_non_rst_reg_val(unsigned int new_val)
{
    /* Porting for MD WDT */
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
