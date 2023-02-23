/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <debug.h>
#include <platform/addressmap.h>
#include <platform/mboot_expdb.h>
#include <platform/lpm.h>
#include <reg.h>
#include <smc.h>
#include <trace.h>

/* Please check adressmap.h that sram had been map or not */
#define MCDI_SRAM_OFFSET                (0x1b000)
#define MCDI_SRAM_BASE                  (SRAM_BASE + MCDI_SRAM_OFFSET) /* 0x0011b000 */
#define MCDI_SRAM_SIZE                  0x800 /* 2K bytes */
#define MCDI_SRAM_BASE_PHY              (SRAM_BASE_PHY + MCDI_SRAM_OFFSET)

#define MCDI_DUMP_BASE                  (MCDI_SRAM_BASE_PHY)
#define MCDI_DUMP_SIZE                  (MCDI_SRAM_SIZE)

#define MCDI_SRAM_READ(off)             readl(MCDI_SRAM_BASE + off)
#define MCDI_SRAM_WRITE(off, val)       writel(val, (MCDI_SRAM_BASE + off))

#define MCDI_MCUSYS_READ(offset)        readl(MCUCFG_BASE + offset)

#define LPM_MCDI_AEE_SPMC_LATCH_1        (0x140)
#define LPM_MCDI_AEE_SPMC_LATCH_2        (0x144)
#define LPM_MCDI_AEE_PWR_STAT_CPU0       (0x148)
#define LPM_MCDI_AEE_PWR_STAT_CPU1       (0x14C)
#define LPM_MCDI_AEE_PWR_STAT_CPU2       (0x150)
#define LPM_MCDI_AEE_PWR_STAT_CPU3       (0x154)
#define LPM_MCDI_AEE_PWR_STAT_CPU4       (0x158)
#define LPM_MCDI_AEE_PWR_STAT_CPU5       (0x15C)
#define LPM_MCDI_AEE_PWR_STAT_CPU6       (0x160)
#define LPM_MCDI_AEE_PWR_STAT_CPU7       (0x164)
#define LPM_MCDI_AEE_CPC_LATCH_1         (0x168)
#define LPM_MCDI_AEE_CPC_LATCH_2         (0x16c)
#define LPM_MCDI_AEE_CPC_LATCH_3         (0x170)
#define LPM_MCDI_AEE_CPC_LATCH_4         (0x174)


/* mcdi smc call cmd */
#define CPC_SMC_EVENT_DUMP_TRACE_DATA 0

enum {
    MCUSYS_STATUS,
    CPC_COMMAND
};

static void latch_mcusys_info(void)
{
    /* spmc info */
    MCDI_SRAM_WRITE(LPM_MCDI_AEE_SPMC_LATCH_1,
                    MCDI_MCUSYS_READ(0xA748) & 0x03FF0000);
    MCDI_SRAM_WRITE(LPM_MCDI_AEE_SPMC_LATCH_2,
                    MCDI_MCUSYS_READ(0xA718) & 0x00FFFFFF);

    /* cpu 0 ~ 7 spmc pwr status */
    MCDI_SRAM_WRITE(LPM_MCDI_AEE_PWR_STAT_CPU0,
                    MCDI_MCUSYS_READ(0x0214) & 0x000003FF);
    MCDI_SRAM_WRITE(LPM_MCDI_AEE_PWR_STAT_CPU1,
                    MCDI_MCUSYS_READ(0x0A14) & 0x000003FF);
    MCDI_SRAM_WRITE(LPM_MCDI_AEE_PWR_STAT_CPU2,
                    MCDI_MCUSYS_READ(0x1214) & 0x000003FF);
    MCDI_SRAM_WRITE(LPM_MCDI_AEE_PWR_STAT_CPU3,
                    MCDI_MCUSYS_READ(0x1A14) & 0x000003FF);
    MCDI_SRAM_WRITE(LPM_MCDI_AEE_PWR_STAT_CPU4,
                    MCDI_MCUSYS_READ(0x2214) & 0x000003FF);
    MCDI_SRAM_WRITE(LPM_MCDI_AEE_PWR_STAT_CPU5,
                    MCDI_MCUSYS_READ(0x2A14) & 0x000003FF);
    MCDI_SRAM_WRITE(LPM_MCDI_AEE_PWR_STAT_CPU6,
                    MCDI_MCUSYS_READ(0x3214) & 0x000003FF);
    MCDI_SRAM_WRITE(LPM_MCDI_AEE_PWR_STAT_CPU7,
                    MCDI_MCUSYS_READ(0x3A14) & 0x000003FF);

    /* cpc latch info, please comfirm with CODA and DE */
    MCDI_SRAM_WRITE(LPM_MCDI_AEE_CPC_LATCH_1,
                    MCDI_MCUSYS_READ(0xAB80));
    MCDI_SRAM_WRITE(LPM_MCDI_AEE_CPC_LATCH_2,
                    MCDI_MCUSYS_READ(0xAB84));
    MCDI_SRAM_WRITE(LPM_MCDI_AEE_CPC_LATCH_3,
                    MCDI_MCUSYS_READ(0xAB88));
    MCDI_SRAM_WRITE(LPM_MCDI_AEE_CPC_LATCH_4,
                    MCDI_MCUSYS_READ(0xAB8c));
}

static void save_mcdi_data(CALLBACK dev_write)
{
     struct __smccc_res smccc_mcdi;

     latch_mcusys_info();

     /* notify atf mcdi driver dump */
     MT_LPM_SMC_CPU_PM(CPC_COMMAND, CPC_SMC_EVENT_DUMP_TRACE_DATA,
                       0, 0, &smccc_mcdi);

     if (!dev_write(MCDI_DUMP_BASE, MCDI_DUMP_SIZE))
         dprintf(CRITICAL, "%s: mcdi aee dump failed\n", __func__);
}
AEE_EXPDB_INIT_HOOK(SYS_MCDI_DATA, MCDI_DUMP_SIZE, save_mcdi_data);

