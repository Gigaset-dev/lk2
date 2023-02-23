/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <platform/addressmap.h>

/* ToDo: the following argument should be parameterized */
/*platform dependent address*/
#define MEDMCU_DRAM_SIZE               0x00040000  // 256KB
#define MEDMCU_DRAM_ALIGN              0x00100000  // due to l1c align = 1MB
#define MEDMCU_DRAM_START              0x5D400000

#define DRAM_ADDR_MAX                  0x9FFFFFFF  // max address can MEDMCU remap
#define DRAM_4GB_MAX                   0xFFFFFFFF
#define DRAM_4GB_OFFSET                0x40000000
#define MEDMCU_EMI_REGION              6
#define ENABLE_MEDMCU_EMI_PROTECTION  (1)

#define MED_INFRA_CLK                 (MED_INFRA_BASE + 0x80014) //26M, 32K clk

#define MEDMCU_SRAM_BASE               MEDMCU_ADDR_BASE
#define MEDMCU_SRAM_SIZE               0x20000
#define MEDMCU_CFG_BASE               (MEDMCU_ADDR_BASE + 0x200000)

/*core 0/1 */
#define R_CORE0_SW_RSTN_CLR           (MEDMCU_CFG_BASE + 0x30000)
#define R_CORE0_SW_RSTN_SET           (MEDMCU_CFG_BASE + 0x30004)

#define R_CORE1_SW_RSTN_CLR           (MEDMCU_CFG_BASE + 0x40000)
#define R_CORE1_SW_RSTN_SET           (MEDMCU_CFG_BASE + 0x40004)

#define R_CORE0_DBG_CTRL              (MEDMCU_CFG_BASE + 0x30010)
#define R_CORE1_DBG_CTRL              (MEDMCU_CFG_BASE + 0x40010)
#define M_CORE_TBUF_DBG_SEL           (7 << 4)
#define S_CORE_TBUF_DBG_SEL           (4)
#define R_CORE0_WDT_IRQ               (MEDMCU_CFG_BASE + 0x30030)
#define R_CORE1_WDT_IRQ               (MEDMCU_CFG_BASE + 0x40030)
#define B_WDT_IRQ                     (1 << 0)

#define R_CORE0_WDT_CFG               (MEDMCU_CFG_BASE + 0x30034)
#define R_CORE1_WDT_CFG               (MEDMCU_CFG_BASE + 0x40034)
#define V_INSTANT_WDT                 0x80000000

#define R_CORE0_STATUS                (MEDMCU_CFG_BASE + 0x30070)
#define B_CORE_GATED                  (1 << 0)
#define B_CORE_HALT                   (1 << 1)
#define R_CORE0_MON_PC                (MEDMCU_CFG_BASE + 0x30080)
#define R_CORE0_MON_LR                (MEDMCU_CFG_BASE + 0x30084)
#define R_CORE0_MON_SP                (MEDMCU_CFG_BASE + 0x30088)
#define R_CORE0_TBUF_WPTR             (MEDMCU_CFG_BASE + 0x3008c)

#define R_CORE0_MON_PC_LATCH          (MEDMCU_CFG_BASE + 0x300d0)
#define R_CORE0_MON_LR_LATCH          (MEDMCU_CFG_BASE + 0x300d4)
#define R_CORE0_MON_SP_LATCH          (MEDMCU_CFG_BASE + 0x300d8)

#define R_CORE1_STATUS                (MEDMCU_CFG_BASE + 0x40070)
#define R_CORE1_MON_PC                (MEDMCU_CFG_BASE + 0x40080)
#define R_CORE1_MON_LR                (MEDMCU_CFG_BASE + 0x40084)
#define R_CORE1_MON_SP                (MEDMCU_CFG_BASE + 0x40088)
#define R_CORE1_TBUF_WPTR             (MEDMCU_CFG_BASE + 0x4008c)

#define R_CORE1_MON_PC_LATCH          (MEDMCU_CFG_BASE + 0x400d0)
#define R_CORE1_MON_LR_LATCH          (MEDMCU_CFG_BASE + 0x400d4)
#define R_CORE1_MON_SP_LATCH          (MEDMCU_CFG_BASE + 0x400d8)

#define R_CORE0_TBUF_DATA31_0         (MEDMCU_CFG_BASE + 0x300e0)
#define R_CORE0_TBUF_DATA63_32        (MEDMCU_CFG_BASE + 0x300e4)
#define R_CORE0_TBUF_DATA95_64        (MEDMCU_CFG_BASE + 0x300e8)
#define R_CORE0_TBUF_DATA127_96       (MEDMCU_CFG_BASE + 0x300ec)

#define R_CORE1_TBUF_DATA31_0         (MEDMCU_CFG_BASE + 0x400e0)
#define R_CORE1_TBUF_DATA63_32        (MEDMCU_CFG_BASE + 0x400e4)
#define R_CORE1_TBUF_DATA95_64        (MEDMCU_CFG_BASE + 0x400e8)
#define R_CORE1_TBUF_DATA127_96       (MEDMCU_CFG_BASE + 0x400ec)

#define R_MEDMCU_CLK_SEL              (MEDMCU_CFG_BASE + 0x21000)
#define R_MEDMCU_CLK_EN               (MEDMCU_CFG_BASE + 0x21004)
#define R_L1TCM_SRAM_PDN              (MEDMCU_CFG_BASE + 0x2102C)
#define R_SET_CLK_CG                  (MEDMCU_CFG_BASE + 0x21030)
#define R_CPU0_SRAM_PD                (MEDMCU_CFG_BASE + 0x21080)
#define R_CPU1_SRAM_PD                (MEDMCU_CFG_BASE + 0x21084)
#define R_TCM_TAIL_SRAM_PD            (MEDMCU_CFG_BASE + 0x21094)
#define R_CLK_CTRL_GENERAL_CTRL       (MEDMCU_CFG_BASE + 0x2109C)

#define R_L2TCM_SRAM_PD0              (MEDMCU_CFG_BASE + 0x210C0)
#define R_L2TCM_SRAM_PD1              (MEDMCU_CFG_BASE + 0x210C4)
#define R_L2TCM_SRAM_PD2              (MEDMCU_CFG_BASE + 0x210C8)

#define R_CORE0_CACHE_RAM             (MEDMCU_CFG_BASE + 0x60000)
#define R_CORE1_CACHE_RAM             (MEDMCU_CFG_BASE + 0x80000)

#define R_SEC_CTRL                    (MEDMCU_CFG_BASE + 0xa5000)
#define B_CORE0_CACHE_DBG_EN          (1 << 28)
#define B_CORE1_CACHE_DBG_EN          (1 << 29)

#define R_SEC_DOMAIN                  (MEDMCU_CFG_BASE + 0xA5080)
#define S_CORE0_DOMAIN                (0)
#define S_CORE1_DOMAIN                (4)
#define S_DMA0_DOMAIN                 (12)
#define S_DMA1_DOMAIN                 (16)
#define S_SPI0_DOMAIN                 (20)
#define S_SPI1_DOMAIN                 (24)
#define S_SPI2_DOMAIN                 (27)
#define DOMAIN_MEDMCU                 ((1 << S_SPI2_DOMAIN) | \
                                      (1 << S_SPI1_DOMAIN) | \
                                      (1 << S_SPI0_DOMAIN) | \
                                      (1 << S_SPI0_DOMAIN) | \
                                      (1 << S_DMA1_DOMAIN) | \
                                      (1 << S_DMA0_DOMAIN) | \
                                      (1 << S_CORE1_DOMAIN) | \
                                      (1 << S_CORE0_DOMAIN))

#define R_ADSP_DOMAIN                 (MEDMCU_CFG_BASE + 0xA5088)
#define S_ADSP_DOMAIN                 (0)
#define DOMAIN_ADSP                   (0 << S_ADSP_DOMAIN)

#define MEDMCU_LOADER_OFFSET           0x0
#define MEDMCU_LOADER_SIZE             0x2000 //8K
#define MEDMCU_FW_OFFSET               0x2000
#define MEDMCU_MEDHW_OFFSET            MEDMCU_SRAM_SIZE
#define MEDMCU_MEDHW_SIZE              0x9000 //medhw size? 24+12 KB
#define MEDHW_GC0_PM_SIZE              0x2000
#define MEDHW_GC0_DM_SIZE              0x1000
#define MEDHW_GC1_PM_SIZE              0x2000
#define MEDHW_GC1_DM_SIZE              0x1000
#define MEDHW_GC2_PM_SIZE              0x1000
#define MEDHW_GC2_DM_SIZE              0x1000

/*image name definition*/
#define IMG_NAME_MEDMCU_A              "tinysys-medmcu-RV33_A"
#define MEDHW_NAME_GC0_PM              "medhw-gc0-pm"
#define MEDHW_NAME_GC0_DM              "medhw-gc0-dm"
#define MEDHW_NAME_GC1_PM              "medhw-gc1-pm"
#define MEDHW_NAME_GC1_DM              "medhw-gc1-dm"
#define MEDHW_NAME_GC2_PM              "medhw-gc2-pm"
#define MEDHW_NAME_GC2_DM              "medhw-gc2-dm"

/******************************************************************************
 * 1. New entries must be appended to the end of the structure.
 * 2. Do NOT use conditional option such as #ifdef inside the structure.
 ******************************************************************************/
struct medmcu_region_info_st {
    uint32_t ap_loader_start;
    uint32_t ap_loader_size;
    uint32_t ap_firmware_start;
    uint32_t ap_firmware_size;
    uint32_t ap_medhw_gc0_pm_start;
    uint32_t ap_medhw_gc0_pm_size;
    uint32_t ap_medhw_gc0_dm_start;
    uint32_t ap_medhw_gc0_dm_size;
    uint32_t ap_medhw_gc1_pm_start;
    uint32_t ap_medhw_gc1_pm_size;
    uint32_t ap_medhw_gc1_dm_start;
    uint32_t ap_medhw_gc1_dm_size;
    uint32_t ap_medhw_gc2_pm_start;
    uint32_t ap_medhw_gc2_pm_size;
    uint32_t ap_medhw_gc2_dm_start;
    uint32_t ap_medhw_gc2_dm_size;
    /* This is the size of the structure.
     * It can act as a version number if entries can only be
     * added to (not deleted from) the structure.
     * It should be the first entry of the structure, but for
     * compatibility reason, it is appended here.
     */
    uint32_t struct_size;
    uint32_t medmcu_log_thru_ap_uart;
    uint32_t TaskContext_ptr;
    uint32_t medmcuctl;
    uint32_t regdump_start;
    uint32_t regdump_size;
};
