/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#if ARCH_ARM
#include <arch/arm.h>
#endif
#if ARCH_ARM64
#include <arch/arm64.h>
#endif
#include <assert.h>
#include <debug.h>
#include <err.h>
#include <load_image.h>
#include <mblock.h>
#include <platform_mtk.h>
#include <reg.h>
#include <scp.h>
#include <scp_plat.h>
#include "scp_plat_priv.h"
#include <smc.h>
#include <smc_id_table.h>
#include <stdlib.h>
#include <string.h>
#include <trace.h>
#ifdef SCP_ENABLE_EMI_PROTECTION
#include <emi_mpu.h>
#endif

#define LOCAL_TRACE (0)
/*************************************************************************/
int verify_load_scp_image(const char *part_name, void *addr_phy, void *addr)
{
    int scp_dram_use_size = 0;
    int img_size, ld_size, fw_size;
    int scp_sram_size = scp_get_sram_size();
    int scp_dram_img_size;
    /* vir addr for lk load image from flash */
    void *img_ptr, *scp_dram_img_ptr, *scp_dram_backup_ptr;
    /* phy addr for scp_reion_info use */
    void *ld_ptr_phy = NULL;
    void *fw_ptr_phy = NULL;
    void *scp_dram_img_ptr_phy = NULL;
    void *scp_dram_backup_ptr_phy = NULL;

    struct scp_region_info_st *scp_region_info;
    uint32_t reg_temp;

    /* step 0: set core0/1 RSTN */
    writel(0x1, R_CORE0_SW_RSTN_SET);
    writel(0x1, R_CORE1_SW_RSTN_SET);

    // step 1: enable sram, enable 1 block per time
    for (reg_temp = 0xffffffff; reg_temp != 0;) {
        reg_temp = reg_temp >> 1;
        writel(reg_temp, R_L2TCM_SRAM_PD0);
    }

    for (reg_temp = 0xffffffff; reg_temp != 0;) {
        reg_temp = reg_temp >> 1;
        writel(reg_temp, R_L2TCM_SRAM_PD1);
    }

    for (reg_temp = 0xffffffff; reg_temp != 0;) {
        reg_temp = reg_temp >> 1;
        writel(reg_temp, R_L2TCM_SRAM_PD2);
    }

    for (reg_temp = 0xffffffff; reg_temp != 0;) {
        reg_temp = reg_temp >> 1;
        writel(reg_temp, R_L2TCM_SRAM_PD3);
    }

    for (reg_temp = 0xffffffff; reg_temp != 0;) {
        reg_temp = reg_temp >> 1;
        writel(reg_temp, R_CPU0_SRAM_PD);
    }

    for (reg_temp = 0xffffffff; reg_temp != 0;) {
        reg_temp = reg_temp >> 1;
        writel(reg_temp, R_CPU1_SRAM_PD);
    }

    for (reg_temp = 0xffffffff; reg_temp != 0;) {
        reg_temp = reg_temp >> 1;
        writel(reg_temp, R_L1TCM_SRAM_PDN);
    }

    /* Clean SRAM. */
    memset((void *) SCP_SRAM_BASE, 0, scp_sram_size);

    // Layout of SCP reserved DRAM memory.
    /************************
     * +------------------+ *
     * |  SCP Loader      | *
     * +------------------+ *
     * |  SCP Firmware    | *
     * +------------------+ *
     ************************
     *    SCP DRAM          *
     ************************
     *    SCP DRAM backup   *
     ************************/

    // step 2: load/verify firmware
    img_ptr  = addr;
    ld_ptr_phy  = addr_phy;
    ld_size = SCP_LOADER_SIZE;
    img_size = load_partition(part_name, IMG_NAME_SCP_A, img_ptr, SCP_MEM_MAX);
    if (img_size <= 0) {
        LTRACEF("[SCP] load_scp_image fail %s\n", IMG_NAME_SCP_A);
        return -1;
    }

    fw_ptr_phy  = addr_phy + SCP_FW_OFFSET;
    fw_size = img_size;

    // step 3: load/verify scp_dram_img code (optional)
    scp_dram_img_ptr  = addr + SCP_DRAM_IMG_OFFSET;
    scp_dram_img_ptr_phy = addr_phy + SCP_DRAM_IMG_OFFSET;
    scp_dram_img_size = load_partition(part_name, DRM_NAME_SCP_A, scp_dram_img_ptr,
                                       SCP_DRAM_IMG_SIZE);

    if (scp_dram_img_size > 0) {
        LTRACEF("[SCP] load_scp_image %s 0x%x bytes (max size = 0x%x).\n",
                        DRM_NAME_SCP_A, scp_dram_img_size, SCP_DRAM_IMG_SIZE);

            /*
             * assert if size of feature placed in dram plus size of overlay exceeds
             * larger than SCP_DRAM_IMG_SIZE.
             */
        if (scp_dram_img_size > SCP_DRAM_IMG_SIZE)
            assert(0);

            // step 3.1 backup scp_dram_img code
        scp_dram_backup_ptr = scp_dram_img_ptr +
            ROUNDUP(scp_dram_img_size, 1024) * SCP_CORE_NUMS;
        scp_dram_backup_ptr_phy = scp_dram_img_ptr_phy +
            ROUNDUP(scp_dram_img_size, 1024) * SCP_CORE_NUMS;
        memcpy(scp_dram_backup_ptr, scp_dram_img_ptr, scp_dram_img_size);
    } else {
        LTRACEF("[SCP] Not support- scp_dram_img\n");
    }

    // step 4: copy loader to sram
    memcpy((void *) SCP_SRAM_BASE, img_ptr, ld_size);

    // step 5: save firmware/cache info to sram
    scp_region_info = (struct scp_region_info_st *)(SCP_SRAM_BASE + 0x4);
    /* check region_info size to avoid scp boot fail */
    if (scp_region_info->struct_size != sizeof(struct scp_region_info_st)) {
        LTRACEF("[SCP] scp_region_info size is not sync: 0x%x vs 0x%lx\n",
                scp_region_info->struct_size, sizeof(struct scp_region_info_st));
        LTRACEF("[SCP] please check scp_region_info struct in LK/kernel/scp side\n");
        assert(0);
    }

    if (need_relocate()) {
    /* embed remap address */
        scp_region_info->ap_loader_start   = (uint32_t)ld_ptr_phy | SCP_DRAM_RESV_EMBED;
    } else {
        scp_region_info->ap_loader_start   = (uint32_t)ld_ptr_phy;
    }
    scp_region_info->ap_loader_size    = (uint32_t)ld_size;
    scp_region_info->ap_firmware_start = (uint32_t)fw_ptr_phy;

    LTRACEF("[SCP]sram size=0x%x bytes\n", scp_sram_size);
    if (fw_size > scp_sram_size) {
        scp_region_info->ap_firmware_size = scp_sram_size;
        LTRACEF("[SCP](overlay)firmware size 0x%x bytes\n",
                scp_region_info->ap_firmware_size);
    } else {
        scp_region_info->ap_firmware_size = (uint32_t)fw_size;
    }

    scp_region_info->ap_dram_start = (uint32_t)scp_dram_img_ptr_phy;
    scp_region_info->ap_dram_size  = (uint32_t)scp_dram_img_size;
    scp_region_info->ap_dram_backup_start = (uint32_t)scp_dram_backup_ptr_phy;
    scp_region_info->scpctl = scp_get_scpctl();

    LTRACEF("scp_region_info->ap_loader_start 0x%x\n",
            scp_region_info->ap_loader_start);
    LTRACEF("scp_region_info->ap_loader_size 0x%x\n",
            scp_region_info->ap_loader_size);
    LTRACEF("scp_region_info->ap_firmware_start 0x%x\n",
            scp_region_info->ap_firmware_start);
    LTRACEF("scp_region_info->ap_firmware_size 0x%x\n",
            scp_region_info->ap_firmware_size);
    LTRACEF("scp_region_info->ap_dram_start 0x%x\n",
            scp_region_info->ap_dram_start);
    LTRACEF("scp_region_info->ap_dram_size 0x%x\n",
            scp_region_info->ap_dram_size);
    LTRACEF("scp_region_info->ap_dram_backup_start 0x%x\n",
            scp_region_info->ap_dram_backup_start);
    LTRACEF("scp_region_info->scp_log_thru_ap_uart 0x%x\n",
            scp_region_info->scp_log_thru_ap_uart);
    LTRACEF("scp_region_info->scpctl 0x%x\n",
            scp_region_info->scpctl);

#ifdef MTK_TINYSYS_SCP_SECURE_DUMP
    {
    struct __smccc_res res;

    /* set dram address to ATF*/
    __smc_conduit(MTK_SIP_BL_TINYSYS_SCP_CONTROL,
            MTK_TINYSYS_SCP_BL_OP_DRAM,
            scp_dram_img_ptr_phy, scp_dram_img_size,
            0, 0, 0, 0, &res);
    LTRACEF("[SCP] region_info dram=%p, 0x%x\n",
                scp_dram_img_ptr_phy, scp_dram_img_size);

    /* set scp region to ATF */
    __smc_conduit(MTK_SIP_BL_TINYSYS_SCP_CONTROL,
            MTK_TINYSYS_SCP_BL_OP_REGION_INFO,
            0, 0, 0, 0, 0, 0, &res);
    }
#endif /* secure dump */

    DSB;

    scp_set_devapc_domain();
#ifdef MTK_TINYSYS_SCP_SECURE_DUMP
    scp_set_devapc_regbank();
    scp_set_devapc_l2tcm();
#endif
    scp_dram_use_size = img_size + scp_dram_img_size;
    return scp_dram_use_size;
}

void scp_emi_protect_enable(void *dram_addr)
{
    /*
     * setup EMI MPU
     * domain 0: AP
     * domain 3: SCP
     */
#ifdef SCP_ENABLE_EMI_PROTECTION
    struct emi_region_info_t region_info;

    region_info.start = (uint64_t)dram_addr;
    region_info.end = (uint64_t)(dram_addr + SCP_DRAM_SIZE);
    region_info.region = SCP_EMI_REGION;

    /* reserved this for mpu version 1 */
    SET_ACCESS_PERMISSION(region_info.apc, UNLOCK,
                          FORBIDDEN, FORBIDDEN, FORBIDDEN, FORBIDDEN,
                          FORBIDDEN, FORBIDDEN, FORBIDDEN, FORBIDDEN,
                          FORBIDDEN, FORBIDDEN, FORBIDDEN, FORBIDDEN,
                          NO_PROTECTION, FORBIDDEN, FORBIDDEN, NO_PROTECTION);

    emi_mpu_set_protection(&region_info);
#endif
}

void disable_scp_hw(void)
{
   /*
    * SW_RSTN = 0x0
    * SW_RSTN_DUAL = 0x0
    * CLK_SW_SEL = 0x1 //switch to 32Khz
    * CLK_ENABLE = 0x0 //disable any clk request
    * SRAM_PDN = 0xffffff
    */
    writel(0x1, R_CORE0_SW_RSTN_SET);
    writel(0x1, R_CORE1_SW_RSTN_SET);
    writel(0x1, R_SCP_CLK_SEL);    /*turn off scp clock*/
    writel(0x0, R_SCP_CLK_EN);
    writel(0xFFFFFFFF, R_L1TCM_SRAM_PDN); /*turn off scp sram*/
    writel(0xFFFFFFFF, R_L2TCM_SRAM_PD0);
    writel(0xFFFFFFFF, R_L2TCM_SRAM_PD1);
    writel(0xFFFFFFFF, R_L2TCM_SRAM_PD2);
    writel(0xFFFFFFFF, R_CPU0_SRAM_PD);
    writel(0xFFFFFFFF, R_CPU1_SRAM_PD);
    writel(0x3, R_TCM_TAIL_SRAM_PD);

    LTRACEF("DISABLE SCP\n");
}

void *scp_dram_alloc(void **phy, unsigned int *size)
{
    struct __smccc_res res;

    *size = SCP_DRAM_SIZE;
    if (need_relocate()) {
        *phy = (void *) mblock_alloc_range(SCP_DRAM_SIZE, SCP_DRAM_ALIGN,
            SCP_DRAM_RESV_START, SCP_DRAM_RESV_END, 0, 0, "SCP-reserved");
    } else {
        *phy = (void *) mblock_alloc(SCP_DRAM_SIZE, SCP_DRAM_ALIGN,
            DRAM_ADDR_MAX, 0, 0, "SCP-reserved");
    }

#ifdef MTK_TINYSYS_SCP_SECURE_DUMP
    /* set reserved address of SCP-ROM*/
    __smc_conduit(MTK_SIP_BL_TINYSYS_SCP_CONTROL,
        MTK_TINYSYS_SCP_BL_OP_RESVDRAM,
        (uint64_t)*phy, (uint64_t)*size,
        0, 0, 0, 0, &res);
#endif /* secure dump */

    return *phy;
}

void scp_set_devapc_domain(void)
{
    /* devapc domain setting, Core0 = 1, Core1 = 1, ADSP = 0 */
    writel(DOMAIN_SCP, R_SEC_DOMAIN);
    writel(DOMAIN_SCP_1, R_SEC_DOMAIN_1);
    writel(DOMAIN_ADSP, R_ADSP_DOMAIN);
}

#ifdef MTK_TINYSYS_SCP_SECURE_DUMP
void scp_set_devapc_regbank(void)
{

    struct __smccc_res res;

    __smc_conduit(MTK_SIP_BL_TINYSYS_SCP_CONTROL,
        MTK_TINYSYS_SCP_BL_OP_DEVAPC,
        0, 0, 0, 0, 0, 0, &res);

}

void scp_set_devapc_l2tcm(void)
{

    struct __smccc_res res;

    __smc_conduit(MTK_SIP_BL_TINYSYS_SCP_CONTROL,
        MTK_TINYSYS_SCP_BL_OP_DEVAPC_L2TCM,
        0, 0, 0, 0, 0, 0, &res);

}

void scp_sharedram_alloc(void)
{
    struct __smccc_res res;
    void *sharedram_addr;

    if (need_relocate()) {
        sharedram_addr = (void *)mblock_alloc_range(SCP_SHAREDRAM_SIZE,
                      SCP_SHAREDRAM_ALIGN,
                      SCP_DRAM_RESV_START, SCP_DRAM_RESV_END,
                      0, 0, "reserve-memory-scp_share");
    } else {
        sharedram_addr = (void *)(uint64_t)mblock_alloc(SCP_SHAREDRAM_SIZE,
                                                SCP_SHAREDRAM_ALIGN, SCP_SHAREDRAM_ADDR_MAX,
                                                0, 0, "reserve-memory-scp_share");
    }

    LTRACEF("[SCP] sharedram=%p, 0x%x\n", sharedram_addr, SCP_SHAREDRAM_SIZE);
    if (sharedram_addr == ((void *) 0)) {
        LTRACEF_LEVEL(CRITICAL, "[SCP] mblock_reserve fail\n");
        assert(0);
    }

    /* set reserved address of SCP-SHARE*/
    __smc_conduit(MTK_SIP_BL_TINYSYS_SCP_CONTROL,
        MTK_TINYSYS_SCP_BL_OP_SHAREDRAM,
        (uint64_t)sharedram_addr, (uint64_t)SCP_SHAREDRAM_SIZE,
        0, 0, 0, 0, &res);

    /* use emi mpu to protect SCP-SHARE */
#if defined(SCP_ENABLE_EMI_PROTECTION) && defined(SCP_SHARE_EMI_REGION)
    {
    struct emi_region_info_t region_info;

    region_info.start = (uint64_t)sharedram_addr;
    region_info.end = (uint64_t)(sharedram_addr + SCP_SHAREDRAM_SIZE);
    region_info.region = SCP_SHARE_EMI_REGION;

    /* reserved this for mpu version 1 */
    SET_ACCESS_PERMISSION(region_info.apc, UNLOCK,
            FORBIDDEN, FORBIDDEN, FORBIDDEN, FORBIDDEN,
            FORBIDDEN, FORBIDDEN, FORBIDDEN, FORBIDDEN,
            FORBIDDEN, FORBIDDEN, FORBIDDEN, FORBIDDEN,
            NO_PROTECTION, FORBIDDEN, FORBIDDEN, NO_PROTECTION);

    emi_mpu_set_protection(&region_info);
    }
#endif
}
#endif /* secure dump */
