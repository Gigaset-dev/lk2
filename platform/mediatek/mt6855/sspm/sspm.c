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


void sspm_wdt_set(void)
{
    dprintf(CRITICAL, "Setup sspm WDT mode\n");
}

void sspm_pinmux_set(void)
{
#if SSPM_DBG_MODE
    //Setup GPIO pinmux SSPM JTAG
    writel(0x00044444, 0x10005310);
    //Setup GPIO pinmux for SSPM UART
    writel(0x50000000, 0x10005360);
    writel(0x00000005, 0x10005370);
#endif
}

void sspm_enable_sram(void)
{
    /* enable spm 0x1C001000 = 0x0b160001*/
    writel(0x0b160001, IO_BASE+0x0C001000);
    /* enable sspm sram 0x1C001F20 bit 16 set 0 to enable*/
    writel(0x00000012, IO_BASE+0x0C001F20);
}
