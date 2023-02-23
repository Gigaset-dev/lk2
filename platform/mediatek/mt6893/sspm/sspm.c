/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <err.h>
#include <load_image.h>
#include <platform/addressmap.h>
#include <platform/mboot_params.h>
#include <platform_mtk.h>
#include <reg.h>
#include <stdlib.h>
#include <string.h>
#include <sspm_plat.h>
#include <sys/types.h>


void sspm_emi_protect_enable(void *addr, int first_boot)
{
#if ENABLE_MPU
    u32 type;

    /*
     * setup EMI MPU
     * domain 0: AP
     * domain 8: SSPM
     */

    struct emi_region_info_t region_info;

    region_info.start = (unsigned long long)(unsigned int)addr;
    region_info.end = (unsigned long long)(unsigned int)(addr)
        + ROUNDUP(SSPM_MEM_SIZE, SSPM_MEM_ALIGN) - 1;
    region_info.region = SSPM_MPU_REGION_ID;

    if (first_boot > 0) {
        SET_ACCESS_PERMISSION(region_info.apc, UNLOCK,
                        FORBIDDEN, FORBIDDEN, FORBIDDEN, FORBIDDEN,
                        FORBIDDEN, FORBIDDEN, FORBIDDEN, SEC_RW,
                        FORBIDDEN, FORBIDDEN, FORBIDDEN, FORBIDDEN,
                        FORBIDDEN, FORBIDDEN, FORBIDDEN, NO_PROTECTION);
        emi_mpu_set_protection(&region_info);
    } else {
        if (mboot_params_current_abnormal_boot()) {
            /* If ABNORMAL BOOT, SSPM_MPU_REGION_ID should be accessible for LKDump */
            type = SEC_RW_NSEC_R;
        } else {
            type = FORBIDDEN;
        }
        SET_ACCESS_PERMISSION(region_info.apc, UNLOCK,
                        FORBIDDEN, FORBIDDEN, FORBIDDEN, FORBIDDEN,
                        FORBIDDEN, FORBIDDEN, FORBIDDEN, SEC_RW,
                        FORBIDDEN, FORBIDDEN, FORBIDDEN, FORBIDDEN,
                        FORBIDDEN, FORBIDDEN, FORBIDDEN, type);
        emi_mpu_set_protection(&region_info);
    }

    dprintf(CRITICAL, "sspm enable emi\n");
#endif

}


void sspm_wdt_set(void)
{
    dprintf(CRITICAL, "Setup sspm WDT mode\n");
}


void sspm_share_memory_emi_protect(void *addr, unsigned int size)
{
    /* reserved */


}



void sspm_pinmux_set(void)
{
#if SSPM_DBG_MODE
    //Setup GPIO pinmux SSPM JTAG
    writel(0x66066000, 0x10005360);
    writel(0x00000006, 0x10005370);
    //Setup GPIO pinmux for SSPM UART0
    writel(0x44000000, 0x100053B0);
#endif
}

void sspm_enable_sram(void)
{
    /* reserved */
}

