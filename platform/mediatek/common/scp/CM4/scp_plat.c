/*
 * Copyright (c) 2019 MediaTek Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#if ARCH_ARM
#include <arch/arm.h>
#endif
#if ARCH_ARM64
#include <arch/arm64.h>
#endif
#include <arch/ops.h>                   /* for arch_sync_cache_range() */
#include <debug.h>
#include <err.h>
#include <load_image.h>
#include <mblock.h>
#include <scp.h>
#include <scp_plat.h>
#include "scp_plat_priv.h"
#include <string.h>
#include <trace.h>

#define LOCAL_TRACE (0)

/*************************************************************************/
int verify_load_scp_image(const char *part_name, void *addr_phy, void *addr)
{
    int scp_dram_use_size = 0;
    void *ld_ptr, *fw_ptr;
    int ld_size, fw_size;
    void *l1c_ptr, *l1c_backup_ptr = NULL;
    int l1c_size;
    int scp_sram_size;
    /* phy addr for scp_reion_info use */
    void *ld_ptr_phy, *fw_ptr_phy, *l1c_ptr_phy, *l1c_backup_ptr_phy;
    struct scp_region_info_st *scp_region_info;
    uint32_t reg_temp;
    const char *prompt = "SCP";

    // step 0: enable sram, enable 1 block per time
    for (reg_temp = 0xffffffff; reg_temp != 0;) {
        reg_temp = reg_temp >> 1;
        writel(reg_temp, SCP_SRAM_PWR_REG);
    }

    /*
     * l1c sram, 64K / tail sram, +32k
     * L1_SRAM_PD / d_l1c_SRAM_PD / d_l1c_tag_SRAM_PD / p_l1c_SRAM_PD / p_l1c_tag_SRAM_PD
     */
    writel(0, CLK_CTRL_L1_SRAM_PD);
    /* TCM_TAIL_SRAM_PD */
    writel(0, CLK_CTRL_TCM_TAIL_SRAM_PD);


    // step 1: load/verify loader
    ld_ptr = addr;
    ld_ptr_phy  = addr_phy;
    ld_size = load_partition(part_name, LOADER_NAME_SCP_A, ld_ptr, SCP_FW_OFFSET);
    if (ld_size <= 0) {
        LTRACEF("%s: load_scp_image fail %s\n", prompt, LOADER_NAME_SCP_A);
        return ld_size;
    }

    // step 2: load/verify firmware
    fw_ptr = addr + SCP_FW_OFFSET;
    fw_ptr_phy  = addr_phy + SCP_FW_OFFSET;
    fw_size = load_partition(part_name, FIRMWARE_NAME_SCP_A, fw_ptr, SCP_L1C_OFFSET - SCP_FW_OFFSET);
    if (fw_size <= 0) {
        LTRACEF("%s: load_scp_image fail %s\n", prompt, FIRMWARE_NAME_SCP_A);
        return fw_size;
    }
    scp_dram_use_size = SCP_FW_OFFSET + fw_size;

    // step 3: load/verify l1c code (optional)
    l1c_ptr = addr + SCP_L1C_OFFSET;
    l1c_ptr_phy = addr_phy + SCP_L1C_OFFSET;
    l1c_size = load_partition(part_name, L1C_NAME_SCP_A, l1c_ptr, SCP_DRAM_SIZE - SCP_L1C_OFFSET);

    if (l1c_size > 0) {
        LTRACEF("%s: load_scp_image %s %d bytes\n", prompt, L1C_NAME_SCP_A, l1c_size);
        // step 3.1 backup l1c code
        l1c_backup_ptr = addr + SCP_L1C_BACKUP_OFFSET;
        l1c_backup_ptr_phy = addr_phy + SCP_L1C_BACKUP_OFFSET;
        memcpy(l1c_backup_ptr, l1c_ptr, l1c_size);
        /* Disable scp bus wt buffer */
        writel(readl(SCP_SYS_CTRL)|(0x1 << AHB_SLICE_POST_WT), SCP_SYS_CTRL);
#if CFG_SCP_ULTRA_SUPPORT
        writel((0x2 << AXI_AWULTRA) | (0x2 << AXI_ARULTRA), BUS_QOS);
#endif
        /* Read the l1c setting from image in dram */
        scp_region_info = (struct scp_region_info_st *)(ld_ptr + SCP_LOADER_SIZE);
        scp_l1c_init(scp_region_info->Il1c_con, scp_region_info->Dl1c_con);
        scp_dram_use_size = SCP_L1C_BACKUP_OFFSET + l1c_size;
    } else {
        writel(readl(SCP_SYS_CTRL)|(0x3 << BYPASS_P_L1C), SCP_SYS_CTRL);
        /* scp clk gates: l1c controller */
        writel((1 << L1C_I_CTRL_CG) | (1 << L1C_D_CTRL_CG), SCP_CLR_CLK_CG);
        LTRACEF("%s: no l1c support\n", prompt);
    }

  //   SCP DRAM layout
  /************************
   *     SCP loader       *
   ************************
   *     SCP Firmware     *
   ************************
   *     SCP L1C          *
   ************************
   *     SCP DRAM backup  *
   ************************/

    // step 6: copy loader to sram
    memcpy((void *) SCP_SRAM_BASE, ld_ptr, ld_size);

    // step 7: save firmware/cache info to sram
    scp_region_info = (struct scp_region_info_st *)(SCP_SRAM_BASE + SCP_LOADER_SIZE);
    scp_region_info->ap_loader_start = (uint32_t)ld_ptr_phy;
    scp_region_info->ap_loader_size = (uint32_t)ld_size;
    scp_region_info->ap_firmware_start = (uint32_t)fw_ptr_phy;
    scp_sram_size = scp_get_sram_size();

    LTRACEF("%s: sram size=%u bytes\n", prompt, scp_sram_size);
    if (fw_size > (scp_sram_size - SCP_FW_OFFSET)) {
        scp_region_info->ap_firmware_size =
            (scp_sram_size - SCP_FW_OFFSET
             - L1C_SIZE(scp_region_info->Il1c_con)
             - L1C_SIZE(scp_region_info->Dl1c_con));
        LTRACEF("%s: (overlay)firmware size %d bytes\n", prompt,
            scp_region_info->ap_firmware_size);
    } else {
        scp_region_info->ap_firmware_size = (uint32_t)fw_size;
    }

    scp_region_info->ap_dram_start = (uint32_t)l1c_ptr_phy;
    scp_region_info->ap_dram_size = (uint32_t)l1c_size;
    scp_region_info->ap_dram_backup_start = (uint32_t)l1c_backup_ptr_phy;
    //scp_region_info->scp_log_thru_ap_uart = get_scp_log_thru_ap_uart();
    scp_region_info->Il1c_con = (uint32_t)L1C_SEL(L1C_IL1C)->L1C_CON;
    scp_region_info->Dl1c_con = (uint32_t)L1C_SEL(L1C_DL1C)->L1C_CON;
    scp_region_info->scpctl = scp_get_scpctl();
    LTRACEF("scp_region_info->ap_loader_start 0x%x\n", scp_region_info->ap_loader_start);
    LTRACEF("scp_region_info->ap_loader_size 0x%x\n", scp_region_info->ap_loader_size);
    LTRACEF("scp_region_info->ap_firmware_start 0x%x\n",
        scp_region_info->ap_firmware_start);
    LTRACEF("scp_region_info->ap_firmware_size 0x%x\n", scp_region_info->ap_firmware_size);
    LTRACEF("scp_region_info->ap_dram_start 0x%x\n", scp_region_info->ap_dram_start);
    LTRACEF("scp_region_info->ap_dram_size 0x%x\n", scp_region_info->ap_dram_size);
    LTRACEF("scp_region_info->ap_dram_backup_start 0x%x\n",
        scp_region_info->ap_dram_backup_start);
    LTRACEF("scp_region_info->scp_log_thru_ap_uart 0x%x\n",
        scp_region_info->scp_log_thru_ap_uart);
    LTRACEF("scp_region_info->Il1c_con 0x%x\n", scp_region_info->Il1c_con);
    LTRACEF("scp_region_info->Dl1c_con 0x%x\n", scp_region_info->Dl1c_con);
    LTRACEF("scp_region_info->scpctl 0x%x\n", scp_region_info->scpctl);
    DSB;

    writel(readl(SCP_JTAG_REG) | SCP_JTAG_EN, SCP_JTAG_REG);
    DSB;

    return scp_dram_use_size;
}

void scp_emi_protect_enable(void *addr)
{
    /*
     * setup EMI MPU
     * domain 0: AP
     * domain 3: SCP
     */
#if ENABLE_SCP_EMI_PROTECTION
    struct emi_region_info_t region_info;

    region_info.start = (unsigned long long)(unsigned int)addr;
    region_info.end = (unsigned long long)(unsigned int)(addr + SCP_DRAM_SIZE - 1);
    region_info.region = SCP_EMI_REGION;

    SET_ACCESS_PERMISSION(region_info.apc, UNLOCK,
                        FORBIDDEN, FORBIDDEN, FORBIDDEN, FORBIDDEN,
                        FORBIDDEN, FORBIDDEN, FORBIDDEN, FORBIDDEN,
                        FORBIDDEN, FORBIDDEN, FORBIDDEN, FORBIDDEN,
                        NO_PROTECTION, FORBIDDEN, FORBIDDEN, SEC_R_NSEC_R);
    emi_mpu_set_protection(&region_info);
#endif
}

void disable_scp_hw(void)
{
    /* turn off SCP related module
     * PDEF_SCP_SRAM_PDN = #0xffffff
     * PDEF_SCP_CLK_SEL = #0x1
     * PDEF_SCP_CLK_EN = #0x0
     * PDEF_SCP_CFGREG_SW_RSTN = #0x0
     */
    writel(0x0, SCP_CFGREG_SW_RSTN);
    writel(0xFFFFFFFF, SCP_SRAM_PDN);
    writel(0x1, SCP_CLK_SEL);
    writel(0x0, SCP_CLK_EN);
    writel(0xFFFFFFFF, CLK_CTRL_L1_SRAM_PD);
    writel(0xFFFFFFFF, CLK_CTRL_TCM_TAIL_SRAM_PD);

    LTRACEF("DISABLE SCP\n");
}

void *scp_dram_alloc(void **phy, unsigned int *size)
{
    *size = SCP_DRAM_SIZE;
    *phy = (void *) (unsigned int) mblock_alloc(SCP_DRAM_SIZE, SCP_DRAM_ALIGN,
        DRAM_ADDR_MAX, 0, 0, "SCP-reserved");

    return *phy;
}

