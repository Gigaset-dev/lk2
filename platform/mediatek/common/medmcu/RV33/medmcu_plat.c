/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <arch/arm.h>
#include <arch/ops.h>                   /* for arch_sync_cache_range() */
#include <debug.h>
#include <err.h>
#include <load_image.h>
#include <mblock.h>
#include <medmcu.h>
#include <medmcu_plat.h>
#include <mtcmos.h>
#include <platform/addressmap.h>
#include <platform/debug.h>
#include <platform/reg.h>
#include <string.h>
#include <stdlib.h>
#include <trace.h>

#include "medmcu_plat_priv.h"
#if ENABLE_MEDMCU_EMI_PROTECTION
#include <emi_mpu.h>
#endif

#define LOCAL_TRACE (1)

void dump_med_netsys_power_clk_regs(void)
{
    unsigned int netsys_power_val, netsys_clk_val;
    unsigned int netsys_clk_500M_val, netsys_clk_wed_val;

    dprintf(CRITICAL, "%s: IO_BASE=0x%08x\n", __func__, IO_BASE);
    netsys_power_val = read32(SPM_BASE + 0x16C);
    netsys_clk_val = read32(IO_BASE + 0xA0);
    netsys_clk_500M_val = read32(IO_BASE + 0xC0);
    netsys_clk_wed_val = read32(IO_BASE + 0xD0);
    dprintf(CRITICAL, "%s:REG_0x%08x=0x%08x, REG_0x%08x=0x%08x\n",
        __func__, SPM_BASE - KERNEL_ASPACE_BASE + 0x16c, netsys_power_val,
        IO_BASE_PHY + 0xA0, netsys_clk_val);
    dprintf(CRITICAL, "%s:REG_0x%08x=0x%08x, REG_0x%08x=0x%08x\n",
        __func__, IO_BASE_PHY + 0xC0, netsys_clk_500M_val,
        IO_BASE_PHY + 0xD0, netsys_clk_wed_val);
}

#ifndef mdelay
#define mdelay(x)       spin((x) * 1000)
#endif

/*************************************************************************/
int verify_load_medmcu_image(const char *part_name, void *addr_phy, void *addr)
{
    int medmcu_dram_use_size = 0;
    void *img_ptr;
    int img_size, ld_size, fw_size;
    void *medhw_ptr, *medhw_ptr_phy;
    int medhw_size;
    int medmcu_sram_size;
    struct medmcu_region_info_st *medmcu_region_info;
    uint32_t reg_temp;
    const char *prompt = "MEDMCU";
    int ret = 0;

    /* netsys on */
    ret = spm_mtcmos_ctrl_netsys(STA_POWER_ON);
    LTRACEF("netsys on: %d and delay 1ms for MED HW design\n", ret);

    mdelay(1);

    dump_med_netsys_power_clk_regs();

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

    /* set CG */
    writel(0xffffffff, R_SET_CLK_CG);
    /* set 26M/32K clk */
    writel(0x0, MED_INFRA_CLK);

    /* clean sram */
    memset((void *) MEDMCU_SRAM_BASE, 0, MEDMCU_SRAM_SIZE);

  //   MEDMCU DRAM layout
  /*************************
   * ********************  *
   * *  MEDMCU loader   *  *
   * ********************  *
   * *  MEDMCU Firmware *  *
   * ********************  *
   *************************
   *    MEDHW              *
   *************************/
    // step 2: load/verify loader
    img_ptr = addr;
    img_size = load_partition(part_name, IMG_NAME_MEDMCU_A, img_ptr,
                        MEDMCU_MEDHW_OFFSET - MEDMCU_FW_OFFSET);
    if (img_size <= 0) {
        LTRACEF("%s: load_medmcu_image fail %s\n", prompt, IMG_NAME_MEDMCU_A);
        return img_size;
    }

    ld_size = MEDMCU_LOADER_SIZE;
    fw_size = img_size;

    medmcu_region_info =
            (struct medmcu_region_info_st *)(MEDMCU_SRAM_BASE + 0x4);

    // step 3: copy loader to sram
    memcpy((void *) MEDMCU_SRAM_BASE, img_ptr, ld_size);

    // step 4: save firmware/cache info to sram
    medmcu_region_info->ap_loader_start = (uint32_t)addr_phy;
    medmcu_region_info->ap_loader_size = (uint32_t)ld_size;
    medmcu_region_info->ap_firmware_start =
            (uint32_t)addr_phy + MEDMCU_FW_OFFSET;
    medmcu_sram_size = medmcu_get_sram_size();

    LTRACEF("%s: sram size=%u bytes\n", prompt, medmcu_sram_size);
    if (fw_size > medmcu_sram_size) {
        medmcu_region_info->ap_firmware_size = medmcu_sram_size;
        LTRACEF("%s: (overlay)firmware size %d bytes\n", prompt,
            medmcu_region_info->ap_firmware_size);
    } else {
        medmcu_region_info->ap_firmware_size = (uint32_t)fw_size;
    }

    // step 5: load/verify medhw
    //GC_0
    medhw_ptr = addr + MEDMCU_MEDHW_OFFSET;
    medhw_ptr_phy = addr_phy + MEDMCU_MEDHW_OFFSET;
    medhw_size = load_partition(part_name, MEDHW_NAME_GC0_PM, medhw_ptr, MEDHW_GC0_PM_SIZE);
    if (medhw_size > 0)
        LTRACEF("%s: load_medhw_gc0_pm_image %s %d bytes\n",
                 prompt, MEDHW_NAME_GC0_PM, medhw_size);
    else
        LTRACEF("%s: no medhw_gc0_pm binary\n", prompt);
    medmcu_region_info->ap_medhw_gc0_pm_start = (uint32_t)medhw_ptr_phy;
    medmcu_region_info->ap_medhw_gc0_pm_size = (uint32_t)medhw_size;

    medhw_ptr += ROUNDUP(medhw_size, 4);
    medhw_ptr_phy += ROUNDUP(medhw_size, 4);
    medhw_size = load_partition(part_name, MEDHW_NAME_GC0_DM, medhw_ptr, MEDHW_GC0_DM_SIZE);
    if (medhw_size > 0)
        LTRACEF("%s: load_medhw_gc0_dm_image %s %d bytes\n",
                 prompt, MEDHW_NAME_GC0_DM, medhw_size);
    else
        LTRACEF("%s: no medhw_gc0_dm binary\n", prompt);
    medmcu_region_info->ap_medhw_gc0_dm_start = (uint32_t)medhw_ptr_phy;
    medmcu_region_info->ap_medhw_gc0_dm_size = (uint32_t)medhw_size;

    //GC_1
    medhw_ptr += ROUNDUP(medhw_size, 4);
    medhw_ptr_phy += ROUNDUP(medhw_size, 4);
    medhw_size = load_partition(part_name, MEDHW_NAME_GC1_PM, medhw_ptr, MEDHW_GC1_PM_SIZE);
    if (medhw_size > 0)
        LTRACEF("%s: load_medhw_gc1_pm_image %s %d bytes\n",
                 prompt, MEDHW_NAME_GC1_PM, medhw_size);
    else
        LTRACEF("%s: no medhw_gc1_pm binary\n", prompt);
    medmcu_region_info->ap_medhw_gc1_pm_start = (uint32_t)medhw_ptr_phy;
    medmcu_region_info->ap_medhw_gc1_pm_size = (uint32_t)medhw_size;

    medhw_ptr += ROUNDUP(medhw_size, 4);
    medhw_ptr_phy += ROUNDUP(medhw_size, 4);
    medhw_size = load_partition(part_name, MEDHW_NAME_GC1_DM, medhw_ptr, MEDHW_GC1_DM_SIZE);
    if (medhw_size > 0)
        LTRACEF("%s: load_medhw_gc1_dm_image %s %d bytes\n",
                 prompt, MEDHW_NAME_GC1_DM, medhw_size);
    else
        LTRACEF("%s: no medhw_gc1_dm binary\n", prompt);
    medmcu_region_info->ap_medhw_gc1_dm_start = (uint32_t)medhw_ptr_phy;
    medmcu_region_info->ap_medhw_gc1_dm_size = (uint32_t)medhw_size;

    //GC_2
    medhw_ptr += ROUNDUP(medhw_size, 4);
    medhw_ptr_phy += ROUNDUP(medhw_size, 4);
    medhw_size = load_partition(part_name, MEDHW_NAME_GC2_PM, medhw_ptr, MEDHW_GC2_PM_SIZE);
    if (medhw_size > 0)
        LTRACEF("%s: load_medhw_gc2_pm_image %s %d bytes\n",
                 prompt, MEDHW_NAME_GC2_PM, medhw_size);
    else
        LTRACEF("%s: no medhw_gc2_pm binary\n", prompt);
    medmcu_region_info->ap_medhw_gc2_pm_start = (uint32_t)medhw_ptr_phy;
    medmcu_region_info->ap_medhw_gc2_pm_size = (uint32_t)medhw_size;

    medhw_ptr += ROUNDUP(medhw_size, 4);
    medhw_ptr_phy += ROUNDUP(medhw_size, 4);
    medhw_size = load_partition(part_name, MEDHW_NAME_GC2_DM, medhw_ptr, MEDHW_GC2_DM_SIZE);
    if (medhw_size > 0)
        LTRACEF("%s: load_medhw_gc2_dm_image %s %d bytes\n",
                 prompt, MEDHW_NAME_GC2_DM, medhw_size);
    else
        LTRACEF("%s: no medhw_gc2_dm binary\n", prompt);
    medmcu_region_info->ap_medhw_gc2_dm_start = (uint32_t)medhw_ptr_phy;
    medmcu_region_info->ap_medhw_gc2_dm_size = (uint32_t)medhw_size;


    medmcu_region_info->medmcuctl = medmcu_get_medmcuctl();
    LTRACEF("medmcu_region_info->ap_loader_start 0x%x\n",
             medmcu_region_info->ap_loader_start);
    LTRACEF("medmcu_region_info->ap_loader_size 0x%x\n",
             medmcu_region_info->ap_loader_size);
    LTRACEF("medmcu_region_info->ap_firmware_start 0x%x\n",
             medmcu_region_info->ap_firmware_start);
    LTRACEF("medmcu_region_info->ap_firmware_size 0x%x\n",
             medmcu_region_info->ap_firmware_size);
    LTRACEF("medmcu_region_info->ap_medhw_gc0_pm_start 0x%x\n",
             medmcu_region_info->ap_medhw_gc0_pm_start);
    LTRACEF("medmcu_region_info->ap_medhw_gc0_pm_size 0x%x\n",
             medmcu_region_info->ap_medhw_gc0_pm_size);
    LTRACEF("medmcu_region_info->ap_medhw_gc0_dm_start 0x%x\n",
             medmcu_region_info->ap_medhw_gc0_dm_start);
    LTRACEF("medmcu_region_info->ap_medhw_gc0_dm_size 0x%x\n",
             medmcu_region_info->ap_medhw_gc0_dm_size);
    LTRACEF("medmcu_region_info->ap_medhw_gc1_pm_start 0x%x\n",
             medmcu_region_info->ap_medhw_gc1_pm_start);
    LTRACEF("medmcu_region_info->ap_medhw_gc1_pm_size 0x%x\n",
             medmcu_region_info->ap_medhw_gc1_pm_size);
    LTRACEF("medmcu_region_info->ap_medhw_gc1_dm_start 0x%x\n",
             medmcu_region_info->ap_medhw_gc1_dm_start);
    LTRACEF("medmcu_region_info->ap_medhw_gc1_dm_size 0x%x\n",
             medmcu_region_info->ap_medhw_gc1_dm_size);
    LTRACEF("medmcu_region_info->ap_medhw_gc2_pm_start 0x%x\n",
             medmcu_region_info->ap_medhw_gc2_pm_start);
    LTRACEF("medmcu_region_info->ap_medhw_gc2_pm_size 0x%x\n",
             medmcu_region_info->ap_medhw_gc2_pm_size);
    LTRACEF("medmcu_region_info->ap_medhw_gc2_dm_start 0x%x\n",
             medmcu_region_info->ap_medhw_gc2_dm_start);
    LTRACEF("medmcu_region_info->ap_medhw_gc2_dm_size 0x%x\n",
             medmcu_region_info->ap_medhw_gc2_dm_size);
    LTRACEF("medmcu_region_info->medmcuctl 0x%x\n",
             medmcu_region_info->medmcuctl);

    DSB;

    medmcu_dram_use_size = img_size + medhw_size;

    return medmcu_dram_use_size;
}

void medmcu_emi_protect_enable(void *addr)
{
    /*
     * setup EMI MPU
     * domain 0: AP : NO_PROTECTION
     * domain 3: MEDMCU : NO_PROTECTION
     */
#if ENABLE_MEDMCU_EMI_PROTECTION
    struct emi_region_info_t region_info;

    region_info.start = (unsigned long long)(unsigned int)addr;
    region_info.end = (unsigned long long)(unsigned int)(addr + MEDMCU_DRAM_SIZE - 1);
    region_info.region = MEDMCU_EMI_REGION;

    SET_ACCESS_PERMISSION(region_info.apc, UNLOCK,
                        FORBIDDEN, FORBIDDEN, FORBIDDEN, FORBIDDEN,
                        FORBIDDEN, FORBIDDEN, FORBIDDEN, FORBIDDEN,
                        FORBIDDEN, FORBIDDEN, FORBIDDEN, FORBIDDEN,
                        NO_PROTECTION, FORBIDDEN, FORBIDDEN, NO_PROTECTION);
    emi_mpu_set_protection(&region_info);
#endif
}

void disable_medmcu_hw(void)
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
    writel(0x1, R_MEDMCU_CLK_SEL);    /*turn off scp clock*/
    writel(0x0, R_MEDMCU_CLK_EN);
    writel(0xffffffff, R_L1TCM_SRAM_PDN); /*turn off scp sram*/
    writel(0xffffffff, R_L2TCM_SRAM_PD0);
    writel(0xffffffff, R_L2TCM_SRAM_PD1);
    writel(0xffffffff, R_L2TCM_SRAM_PD2);
    writel(0xffffffff, R_CPU0_SRAM_PD);
    writel(0xffffffff, R_CPU1_SRAM_PD);
    writel(0x3, R_TCM_TAIL_SRAM_PD);

    LTRACEF("DISABLE MEDMCU\n");
}

void *medmcu_dram_alloc(void **phy, unsigned int *size)
{
    *size = MEDMCU_DRAM_SIZE;
    *phy = (void *) (unsigned int) mblock_alloc(MEDMCU_DRAM_SIZE, MEDMCU_DRAM_ALIGN,
        DRAM_ADDR_MAX, MEDMCU_DRAM_START, 0, "lk_medmcu_reserved");

    return *phy;
}

