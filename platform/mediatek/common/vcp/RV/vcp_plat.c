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
#include <arch/ops.h>                   /* for arch_clean_invalidate_cache_range() */
#include <assert.h>
#include <debug.h>
#ifdef WITH_PLATFORM_MEDIATEK_COMMON_EMI
#include <emi_mpu.h>
#endif
#include <err.h>
#include <load_image.h>
#include <mblock.h>
#include <mtcmos.h>
#include <platform/vcp_plat.h>
#include <platform_mtk.h>
#include <reg.h>
#include <smc.h>
#include <smc_id_table.h>
#include <stdlib.h>
#include <string.h>
#include <trace.h>
#include <vcp.h>

#define LOCAL_TRACE (0)
/*************************************************************************/
int verify_load_vcp_image(const char *part_name, void *addr_phy, void *addr)
{
    int vcp_dram_use_size = 0;
    int img_size, ld_size, fw_size;
    int vcp_sram_size = vcp_get_sram_size();
    int vcp_resv_dram_size = vcp_get_resv_dram_size();
    int vcp_core_nums = vcp_get_core_num();
    int vcp_dram_img_size;
    /* vir addr for lk load image from flash */
    void *img_ptr, *vcp_dram_img_ptr, *vcp_dram_backup_ptr;
    /* phy addr for vcp_reion_info use */
    void *ld_ptr_phy = NULL;
    void *fw_ptr_phy = NULL;
    void *vcp_dram_img_ptr_phy = NULL;
    void *vcp_dram_backup_ptr_phy = NULL;
    struct __smccc_res res;
    int sram_load_status;

    struct vcp_region_info_st *vcp_region_info;
    uint32_t reg_temp;

    /* step 0: set core0/1 RSTN */
    writel(0x1, R_CORE0_SW_RSTN_SET);
    writel(0x1, R_CORE1_SW_RSTN_SET);

    // step 1: enable sram, enable 1 block per time
    // No sram PD register in MMup, power on with mtcmos

    /* Clean SRAM. */
    memset((void *) VCP_SRAM_BASE, 0, vcp_sram_size);

    // Layout of VCP reserved DRAM memory.
    /************************
     * +------------------+ *
     * |  VCP Loader      | *
     * +------------------+ *
     * |  VCP Firmware    | *
     * +------------------+ *
     ************************
     *    VCP DRAM          *
     ************************
     *    VCP DRAM backup   *
     ************************/

    // step 2: load/verify firmware
    img_ptr  = addr;
    ld_ptr_phy  = addr_phy;
    ld_size = VCP_LOADER_SIZE;
    img_size = load_partition(part_name, IMG_NAME_VCP_A, img_ptr, vcp_sram_size);
    if (img_size <= 0) {
        LTRACEF("[VCP] load_vcp_image fail %s\n", IMG_NAME_VCP_A);
        return -1;
    }

    fw_ptr_phy  = addr_phy + VCP_FW_OFFSET;
    fw_size = img_size;

    // step 3: load/verify vcp_dram_img code (optional)
    vcp_dram_img_ptr  = addr + VCP_DRAM_IMG_OFFSET;
    vcp_dram_img_ptr_phy = addr_phy + VCP_DRAM_IMG_OFFSET;
    vcp_dram_img_size = load_partition(part_name, DRM_NAME_VCP_A, vcp_dram_img_ptr,
                                       VCP_DRAM_IMG_SIZE);

    if (vcp_dram_img_size > 0) {
        LTRACEF("[VCP] load_vcp_image %s 0x%x bytes (max size = 0x%x).\n",
                        DRM_NAME_VCP_A, vcp_dram_img_size, VCP_DRAM_IMG_SIZE);

            /*
             * assert if size of feature placed in dram plus size of overlay exceeds
             * larger than VCP_DRAM_IMG_SIZE.
             */
        if (vcp_dram_img_size > VCP_DRAM_IMG_SIZE)
            assert(0);

            // step 3.1 backup vcp_dram_img code
        vcp_dram_backup_ptr = vcp_dram_img_ptr +
            ROUNDUP(vcp_dram_img_size, 1024) * vcp_core_nums;
        vcp_dram_backup_ptr_phy = vcp_dram_img_ptr_phy +
            ROUNDUP(vcp_dram_img_size, 1024) * vcp_core_nums;

        /* assert if img total size is larger than reserved dram size */
        if ((vcp_dram_backup_ptr + vcp_dram_img_size) >
            (img_ptr + vcp_resv_dram_size))
                panic("[VCP] backup image overflow %p > %p.\n",
                    (vcp_dram_backup_ptr + vcp_dram_img_size),
                    (img_ptr + vcp_resv_dram_size));

        memcpy(vcp_dram_backup_ptr, vcp_dram_img_ptr, vcp_dram_img_size);
    } else {
        LTRACEF("[VCP] Not support- vcp_dram_img\n");
        vcp_dram_img_size = 0;
    }

    /* clean cache before smc */
    arch_clean_invalidate_cache_range((addr_t)addr, vcp_resv_dram_size);

    /* set img dram address and load sram in TF-A*/
    __smc_conduit(MTK_SIP_BL_TINYSYS_VCP_CONTROL,
            MTK_TINYSYS_VCP_BL_OP_SECURE_INIT_VCP,
            vcp_resv_dram_size, ld_ptr_phy, ld_size, fw_size,
            vcp_dram_img_ptr_phy, vcp_dram_img_size, &res);
    sram_load_status = res.a0;

    LTRACEF("ap_loader_start 0x%p\n",
            ld_ptr_phy);
    LTRACEF("ap_loader_size 0x%x\n",
            ld_size);
    LTRACEF("ap_firmware_start 0x%p\n",
            fw_ptr_phy);
    LTRACEF("ap_firmware_size 0x%x\n",
            fw_size);
    LTRACEF("ap_dram_start 0x%p\n",
            vcp_dram_img_ptr_phy);
    LTRACEF("ap_dram_size 0x%x\n",
            vcp_dram_img_size);
    LTRACEF("ap_dram_backup_start 0x%p\n",
            vcp_dram_backup_ptr_phy);
    LTRACEF("sram_load_status %d\n",
            sram_load_status);
    DSB;

    vcp_set_devapc_domain();
    vcp_dram_use_size = (sram_load_status < 0) ? 0 : (img_size + vcp_dram_img_size);
    return vcp_dram_use_size;
}

void vcp_emi_protect_enable(unsigned long long addr, unsigned int size)
{
    /* setup EMI MPU */
#ifdef WITH_PLATFORM_MEDIATEK_COMMON_EMI
    struct emi_region_info_t region_info;

    region_info.start = addr;
    region_info.end = addr + size;
    region_info.region = VCP_EMI_REGION;
    emi_mpu_set_protection(&region_info);
#endif
}

void vcp_power_off(void)
{
    /*  TODO: check unset CG and mux */

   /*
    * SW_RSTN = 0x0
    * SW_RSTN_DUAL = 0x0
    * CLK_SW_SEL = 0x1 //switch to 32Khz
    */
    writel(0x1, R_CORE0_SW_RSTN_SET);
    writel(0x1, R_CORE1_SW_RSTN_SET);
    writel(0x1, R_VCP_CLK_SEL);    /*turn off vcp clock*/
    // disable vcp power by spm
    spm_mtcmos_ctrl_mm_proc_dormant_disable();
    spm_mtcmos_ctrl_mm_proc_shutdown_disable();

    LTRACEF("DISABLE VCP\n");
}
void vcp_power_on(void)
{
    /* enable vcp power by spm */
    spm_mtcmos_ctrl_mm_proc_shutdown_enable();
    spm_mtcmos_ctrl_mm_proc_dormant_enable();
    /*  TODO: set CG and mux */
}



void *vcp_dram_alloc(void **phy, unsigned int size)
{
    if (need_relocate())
        *phy = (void *) mblock_alloc_range(size, VCP_DRAM_ALIGN,
            VCP_DRAM_BOUND_LOWER, VCP_DRAM_BOUND_UPPER, 0, 0, VCP_DRAM_RESV_NAME);
    else
        *phy = (void *) mblock_alloc(size, VCP_DRAM_ALIGN,
            DRAM_ADDR_MAX, 0, 0, VCP_DRAM_RESV_NAME);

    return *phy;
}
void vcp_dram_free(void *phy)
{
    mblock_free(phy);
}

void vcp_set_devapc_domain(void)
{
    /* devapc domain setting, MMup = 0x110 */
    writel(DOMAIN_VCP, R_SEC_DOMAIN);
}

