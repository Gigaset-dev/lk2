/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <debug.h>
#include <mcdi.h>
#include <platform/addressmap.h>
#include <platform/mboot_expdb.h>
#include <reg.h>
#include <smc.h>
#include <string.h>
#include <trace.h>

/* Please check adressmap.h that sram had been map or not */
#define MCDI_SRAM_OFFSET                (0x1B000)
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
#define LPM_MCDI_AEE_CPC_LATCH_2         (0x16C)
#define LPM_MCDI_AEE_CPC_LATCH_3         (0x170)
#define LPM_MCDI_AEE_CPC_LATCH_4         (0x174)
#define LPM_MCDI_AEE_CPC_LATCH_5         (0x178)

/* CPC Trace Data*/
#define CPC_TRACE_SIZE                 (0x20)
#define CPC_TRACE_ID_NUM               (10)
#define CPC_TRACE_SRAM(id)             (MCDI_SRAM_BASE_PHY + 0x10 + (id) * CPC_TRACE_SIZE)
#define CPC_MCUSYS_TRACE_SEL           (MCUCFG_BASE_PHY + 0xAB14)
#define CPC_MCUSYS_TRACE_DATA          (MCUCFG_BASE_PHY + 0xAB20)

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
                    MCDI_MCUSYS_READ(0xAB8C));
    MCDI_SRAM_WRITE(LPM_MCDI_AEE_CPC_LATCH_5,
                    MCDI_MCUSYS_READ(0xAB90));
}

static void save_mcdi_data(CALLBACK dev_write)
{
    latch_mcusys_info();

    if (!dev_write(MCDI_DUMP_BASE, MCDI_DUMP_SIZE))
        dprintf(CRITICAL, "%s: mcdi aee dump failed\n", __func__);
}
AEE_EXPDB_INIT_HOOK(SYS_MCDI_DATA, MCDI_DUMP_SIZE, save_mcdi_data);

/* CPC_MCUSYS_TRACE_SEL is Secure_W, called in platform_elX_init el3 */
void dump_cpc_data(void)
{
    unsigned int id;

    for (id = 0; id < CPC_TRACE_ID_NUM; id++) {
        writel(id, CPC_MCUSYS_TRACE_SEL);
        memcpy((void *)CPC_TRACE_SRAM(id),
               (const void *)CPC_MCUSYS_TRACE_DATA,
               CPC_TRACE_SIZE);
    }
}
