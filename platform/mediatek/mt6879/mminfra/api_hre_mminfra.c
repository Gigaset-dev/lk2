/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <debug.h>
#include <platform/reg.h>

#include "api_hre_mminfra.h"

#define mm_read(addr)        readl(addr)
#define mm_write(addr, val)  writel(val, addr)
#define udelay(usec)         spin(usec)

void mminfra_hre_bkrs(unsigned int select)
{
    switch (select) {
    case INIT_HRE_BK_CONFIG:
    case INIT_HRE_RS_CONFIG:
    //initial bk/rs settings

    /* TINFO = "[SSPM][MMINFRA] Set Backup/Restore Settings" */
        //rg_hre_clk_dcm_ctrl ([31]-> force hre active)
        mm_write(MMINFRA_HRE_ADDR_BASE + 0x10c, 0x1FF0A);
        //rg_wdt_tmr_ctrl
        //(wdt occur in clock period x 2^(WDT_UNIT+1) x (WDT_PRD-1 ~ WDT_PRD))
        mm_write(MMINFRA_HRE_ADDR_BASE + 0x0EC, 0x37);
        mm_write(MMINFRA_HRE_ADDR_BASE + 0x0E8, 0xFFFFFFFF); //rg_wdt_src_en
        mm_write(MMINFRA_HRE_ADDR_BASE + 0x110, 0x0FFFFFFF); //rg_int_src_en
#ifndef HRE_DEBUG_MODE_EN
        mm_write(MMINFRA_HRE_ADDR_BASE + 0x11C, 0x1); //rg_hre_debug_lock
#endif
        //rg_hre_ctrl ([19:18]-> PSECUR_B, [17:16]-> PID, [15:12]-> PDOMAIN(debug path),
        //[11:8]-> PDOMAIN(hre dma), [5]-> finish_en)
        mm_write(MMINFRA_HRE_ADDR_BASE + 0x000, 0xC);
        mm_write(MMINFRA_HRE_ADDR_BASE + 0x004, 0x1e800000);  //rg_ip_addr_base
        mm_write(MMINFRA_HRE_ADDR_BASE + 0x008, 0x0);         //rg_ip_addr_min
        mm_write(MMINFRA_HRE_ADDR_BASE + 0x00C, 0x30000);     //rg_ip_addr_max

        mm_write(MMINFRA_HRE_ADDR_BASE + 0x010, 0x1e800800);  //rg_bk0_addr_ip
        mm_write(MMINFRA_HRE_ADDR_BASE + 0x014, 0x1e8008f0);  //rg_bk1_addr_ip
        mm_write(MMINFRA_HRE_ADDR_BASE + 0x018, 0x1e820f00);  //rg_bk2_addr_ip
        mm_write(MMINFRA_HRE_ADDR_BASE + 0x01C, 0x1e820800);  //rg_bk3_addr_ip
        mm_write(MMINFRA_HRE_ADDR_BASE + 0x020, 0x1e820000);  //rg_bk4_addr_ip
        mm_write(MMINFRA_HRE_ADDR_BASE + 0x024, 0x1e821000);  //rg_bk5_addr_ip
        mm_write(MMINFRA_HRE_ADDR_BASE + 0x028, 0x1e820700);  //rg_bk6_addr_ip
        mm_write(MMINFRA_HRE_ADDR_BASE + 0x02C, 0x1e821700);  //rg_bk7_addr_ip
        mm_write(MMINFRA_HRE_ADDR_BASE + 0x030, 0x1e801100);  //rg_bk8_addr_ip
        mm_write(MMINFRA_HRE_ADDR_BASE + 0x034, 0x1e801220);  //rg_bk9_addr_ip
        mm_write(MMINFRA_HRE_ADDR_BASE + 0x038, 0x1e800280);  //rg_bk10_addr_ip
        mm_write(MMINFRA_HRE_ADDR_BASE + 0x03C, 0x1e800a28);  //rg_bk11_addr_ip
        // mm_write(MMINFRA_HRE_ADDR_BASE + 0x040, 0x1e80f100);  //rg_bk12_addr_ip
        // mm_write(MMINFRA_HRE_ADDR_BASE + 0x044, 0x1e80f220);  //rg_bk13_addr_ip
        // mm_write(MMINFRA_HRE_ADDR_BASE + 0x048, 0x1e800280);  //rg_bk14_addr_ip
        // mm_write(MMINFRA_HRE_ADDR_BASE + 0x04C, 0x0);         //rg_bk15_addr_ip
        // mm_write(MMINFRA_HRE_ADDR_BASE + 0x050, 0x0);         //rg_bk16_addr_ip
        // mm_write(MMINFRA_HRE_ADDR_BASE + 0x054, 0x0);         //rg_bk17_addr_ip
        // mm_write(MMINFRA_HRE_ADDR_BASE + 0x058, 0x0);         //rg_bk18_addr_ip
        // mm_write(MMINFRA_HRE_ADDR_BASE + 0x05C, 0x0);         //rg_bk19_addr_ip
        mm_write(MMINFRA_HRE_ADDR_BASE + 0x060, 0x1e9005b8);  //rg_bk0_addr_sram
        mm_write(MMINFRA_HRE_ADDR_BASE + 0x064, 0x1e900694);  //rg_bk1_addr_sram
        mm_write(MMINFRA_HRE_ADDR_BASE + 0x068, 0x1e900698);  //rg_bk2_addr_sram
        mm_write(MMINFRA_HRE_ADDR_BASE + 0x06C, 0x1e90069c);  //rg_bk3_addr_sram
        mm_write(MMINFRA_HRE_ADDR_BASE + 0x070, 0x1e9006cc);  //rg_bk4_addr_sram
        mm_write(MMINFRA_HRE_ADDR_BASE + 0x074, 0x1e9008cc);  //rg_bk5_addr_sram
        mm_write(MMINFRA_HRE_ADDR_BASE + 0x078, 0x1e900ab0);  //rg_bk6_addr_sram
        mm_write(MMINFRA_HRE_ADDR_BASE + 0x07C, 0x1e900ad0);  //rg_bk7_addr_sram
        mm_write(MMINFRA_HRE_ADDR_BASE + 0x080, 0x1e900ae8);  //rg_bk8_addr_sram
        mm_write(MMINFRA_HRE_ADDR_BASE + 0x084, 0x1e900b0c);  //rg_bk9_addr_sram
        mm_write(MMINFRA_HRE_ADDR_BASE + 0x088, 0x1e900bf0);  //rg_bk10_addr_sram
        mm_write(MMINFRA_HRE_ADDR_BASE + 0x08C, 0x1e900bf4);  //rg_bk11_addr_sram
        // mm_write(MMINFRA_HRE_ADDR_BASE + 0x090, 0x1e900d00);  //rg_bk12_addr_sram
        // mm_write(MMINFRA_HRE_ADDR_BASE + 0x094, 0x1e900d24);  //rg_bk13_addr_sram
        // mm_write(MMINFRA_HRE_ADDR_BASE + 0x098, 0x1e900e08);  //rg_bk14_addr_sram
        // mm_write(MMINFRA_HRE_ADDR_BASE + 0x09C, 0x0);         //rg_bk15_addr_sram
        // mm_write(MMINFRA_HRE_ADDR_BASE + 0x0A0, 0x0);         //rg_bk16_addr_sram
        // mm_write(MMINFRA_HRE_ADDR_BASE + 0x0A4, 0x0);         //rg_bk17_addr_sram
        // mm_write(MMINFRA_HRE_ADDR_BASE + 0x0A8, 0x0);         //rg_bk18_addr_sram
        // mm_write(MMINFRA_HRE_ADDR_BASE + 0x0AC, 0x0);         //rg_bk19_addr_sram
        mm_write(MMINFRA_HRE_ADDR_BASE + 0x0B0, 0x0C010137);  //rg_count0
        mm_write(MMINFRA_HRE_ADDR_BASE + 0x0B4, 0x06087980);  //rg_count1
        mm_write(MMINFRA_HRE_ADDR_BASE + 0x0B8, 0x14013909);  //rg_count2
        // mm_write(MMINFRA_HRE_ADDR_BASE + 0x0BC, 0x13909);     //rg_count3
        // mm_write(MMINFRA_HRE_ADDR_BASE + 0x0C0, 0x0);         //rg_count4
        mm_write(MMINFRA_HRE_ADDR_BASE + 0x0C4, 0xFFF);      //rg_hre_seq_en
        mm_write(MMINFRA_HRE_ADDR_BASE + 0x0C8, 0x0);         //rg_hre_addr_mode0
        mm_write(MMINFRA_HRE_ADDR_BASE + 0x0CC, 0x0);         //rg_hre_addr_mode1

        break;
    case SAVE_HRE_BK_CONFIG:
        //save bk settings in SRAM

        /* TINFO = "[SSPM][MMINFRA] Save Backup Settings in SRAM" */
        mm_write(MMINFRA_HRE_ADDR_BASE + 0x0D0, 0x1);         //rg_save_backup_en set
        /* TINFO = "[SSPM][MMINFRA] Wait for HRE DMA Idle" */
        udelay(10);
        while (mm_read(MMINFRA_HRE_ADDR_BASE + 0x1EC) != 0x0) //rg_dma_state, 0 : idle
            ;
        mm_write(MMINFRA_HRE_ADDR_BASE + 0x0D0, 0x0);         //rg_save_backup_en unset
        break;
    case SAVE_HRE_RS_CONFIG:
        //save rs settings in SRAM
        /* TINFO = "[SSPM][MMINFRA] Save Restore Settings in SRAM" */
        mm_write(MMINFRA_HRE_ADDR_BASE + 0x0D4, 0x1);         //rg_save_restore_en set
        udelay(10);
        /* TINFO = "[SSPM][MMINFRA] Wait for HRE DMA Idle" */
        while (mm_read(MMINFRA_HRE_ADDR_BASE + 0x1EC) != 0x0)  //rg_dma_state, 0 : idle
            ;
        mm_write(MMINFRA_HRE_ADDR_BASE + 0x0D4, 0x0);         //rg_save_restore_en unset
        break;
    default:
        break;
    }
}
