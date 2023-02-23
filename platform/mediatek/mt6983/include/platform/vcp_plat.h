/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <platform/addressmap.h>


/*platform dependent address*/
#define VCP_EMI_REGION                  30

#define VCP_LOADER_OFFSET               0x0
#define VCP_LOADER_SIZE                 0x2000      // 8K
#define VCP_FW_OFFSET                   0x2000
#define VCP_DRAM_IMG_OFFSET             VCP_DRAM_OFFSET // fix @ 2M for loader and firmware
#define VCP_DRAM_IMG_SIZE               0x200000    // 2.0MB dram image

#define VCP_DRAM_ALIGN                  0x00100000  // due to l1c align = 1MB
#define VCP_DRAM_SIZE                   0x800000    // 8.0MB total reserved dram size

#define DRAM_ADDR_MAX                   0xC0000000  // max address can VCP remap
#define DRAM_4GB_MAX                    0xFFFFFFFF
#define DRAM_4GB_OFFSET                 0x40000000

#define VCP_SRAM_BASE                   VCP_ADDR_BASE
#define VCP_CFG_BASE                    (VCP_ADDR_BASE + 0x200000)

/*core 0/1 */
#define R_CORE0_SW_RSTN_CLR             (VCP_CFG_BASE + 0x30000)
#define R_CORE0_SW_RSTN_SET             (VCP_CFG_BASE + 0x30004)

#define R_CORE1_SW_RSTN_CLR             (VCP_CFG_BASE + 0x40000)
#define R_CORE1_SW_RSTN_SET             (VCP_CFG_BASE + 0x40004)

#define R_CORE0_DBG_CTRL                (VCP_CFG_BASE + 0x30010)
#define R_CORE1_DBG_CTRL                (VCP_CFG_BASE + 0x40010)
        #define M_CORE_TBUF_DBG_SEL     (7 << 4)
        #define S_CORE_TBUF_DBG_SEL     (4)

#define R_CORE0_WDT_IRQ                 (VCP_CFG_BASE + 0x30030)
#define R_CORE1_WDT_IRQ                 (VCP_CFG_BASE + 0x40030)
        #define B_WDT_IRQ               (1 << 0)

#define R_CORE0_WDT_CFG                 (VCP_CFG_BASE + 0x30034)
#define R_CORE1_WDT_CFG                 (VCP_CFG_BASE + 0x40034)
        #define V_INSTANT_WDT           0x80000000

#define R_CORE0_STATUS                  (VCP_CFG_BASE + 0x30070)
        #define B_CORE_GATED            (1 << 0)
        #define B_CORE_HALT             (1 << 1)
#define R_CORE0_MON_PC                  (VCP_CFG_BASE + 0x30080)
#define R_CORE0_MON_LR                  (VCP_CFG_BASE + 0x30084)
#define R_CORE0_MON_SP                  (VCP_CFG_BASE + 0x30088)
#define R_CORE0_TBUF_WPTR               (VCP_CFG_BASE + 0x3008c)

#define R_CORE0_MON_PC_LATCH            (VCP_CFG_BASE + 0x300d0)
#define R_CORE0_MON_LR_LATCH            (VCP_CFG_BASE + 0x300d4)
#define R_CORE0_MON_SP_LATCH            (VCP_CFG_BASE + 0x300d8)

#define R_CORE1_STATUS                  (VCP_CFG_BASE + 0x40070)
#define R_CORE1_MON_PC                  (VCP_CFG_BASE + 0x40080)
#define R_CORE1_MON_LR                  (VCP_CFG_BASE + 0x40084)
#define R_CORE1_MON_SP                  (VCP_CFG_BASE + 0x40088)
#define R_CORE1_TBUF_WPTR               (VCP_CFG_BASE + 0x4008c)

#define R_CORE1_MON_PC_LATCH            (VCP_CFG_BASE + 0x400d0)
#define R_CORE1_MON_LR_LATCH            (VCP_CFG_BASE + 0x400d4)
#define R_CORE1_MON_SP_LATCH            (VCP_CFG_BASE + 0x400d8)

#define R_CORE0_TBUF_DATA31_0           (VCP_CFG_BASE + 0x300e0)
#define R_CORE0_TBUF_DATA63_32          (VCP_CFG_BASE + 0x300e4)
#define R_CORE0_TBUF_DATA95_64          (VCP_CFG_BASE + 0x300e8)
#define R_CORE0_TBUF_DATA127_96         (VCP_CFG_BASE + 0x300ec)

#define R_CORE1_TBUF_DATA31_0           (VCP_CFG_BASE + 0x400e0)
#define R_CORE1_TBUF_DATA63_32          (VCP_CFG_BASE + 0x400e4)
#define R_CORE1_TBUF_DATA95_64          (VCP_CFG_BASE + 0x400e8)
#define R_CORE1_TBUF_DATA127_96         (VCP_CFG_BASE + 0x400ec)

#define R_VCP_CLK_SEL                   (VCP_CFG_BASE + 0x21000)

#define R_CORE0_CACHE_RAM               (VCP_CFG_BASE + 0x60000)
#define R_CORE1_CACHE_RAM               (VCP_CFG_BASE + 0x80000)

#define R_SEC_CTRL                      (VCP_CFG_BASE + 0xa5000)
        #define B_CORE0_CACHE_DBG_EN    (1 << 28)
        #define B_CORE1_CACHE_DBG_EN    (1 << 29)

#define VCP_DOMAIN_ID                   6
#define R_SEC_DOMAIN                    (VCP_CFG_BASE + 0xA5080)
        #define S_CORE0_DOMAIN          (0)
        #define S_CORE1_DOMAIN          (4)
        #define S_DMA0_DOMAIN           (12)
        #define S_DMA1_DOMAIN           (16)
        #define S_SPI0_DOMAIN           (20)
        #define S_SPI1_DOMAIN           (24)
        #define S_SPI2_DOMAIN           (27)
        #define DOMAIN_VCP              ((VCP_DOMAIN_ID << S_SPI2_DOMAIN) | \
                                        (VCP_DOMAIN_ID << S_SPI1_DOMAIN) | \
                                        (VCP_DOMAIN_ID << S_SPI0_DOMAIN) | \
                                        (VCP_DOMAIN_ID << S_SPI0_DOMAIN) | \
                                        (VCP_DOMAIN_ID << S_DMA1_DOMAIN) | \
                                        (VCP_DOMAIN_ID << S_DMA0_DOMAIN) | \
                                        (VCP_DOMAIN_ID << S_CORE1_DOMAIN) | \
                                        (VCP_DOMAIN_ID << S_CORE0_DOMAIN))

/*image name definition*/

#define IMG_NAME_VCP_A     "tinysys-vcp-RV55_A"
#define DRM_NAME_VCP_A     "tinysys-vcp-RV55_A_dram"


/* image name change need modify build system and security flow, so use define to dispatch */


/****************************************************************************
 * 1. New entries must be appended to the end of the structure.
 * 2. Do NOT use conditional option such as #ifdef inside the structure.
 ****************************************************************************/
struct vcp_region_info_st {
    uint32_t ap_loader_start;
    uint32_t ap_loader_size;
    uint32_t ap_firmware_start;
    uint32_t ap_firmware_size;
    uint32_t ap_dram_start;
    uint32_t ap_dram_size;
    uint32_t ap_dram_backup_start;
    /*        This is the size of the structure.
     *        It can act as a version number if entries can only be
     *        added to (not deleted from) the structure.
     *        It should be the first entry of the structure, but for
     *        compatibility reason, it is appended here.
     */
    uint32_t struct_size;
    uint32_t vcp_log_thru_ap_uart;
    uint32_t TaskContext_ptr;
    uint32_t vcpctl;
    uint32_t regdump_start;
    uint32_t regdump_size;
    uint32_t ap_params_start;
};
