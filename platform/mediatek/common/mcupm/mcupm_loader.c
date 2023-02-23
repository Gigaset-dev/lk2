/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <arch/mmu.h>
#include <arch/ops.h>
#include <debug.h>
#include <err.h>
#include <kernel/vm.h>
#include <load_image.h>
#include <mblock.h>
#include "mcupm.h"
#include <platform/addressmap.h>
#include <platform/mboot_params.h>
#include <platform/mrdump_params.h>
#include <platform_mtk.h>
#include <platform/reg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <trace.h>
#ifdef WITH_PLATFORM_MEDIATEK_COMMON_EMI
#include <emi_mpu.h>
#endif

#define LOCAL_TRACE 0

static void *mcupm_memcpy(void *dest, const void *src, int count)
{
    unsigned int *tmp = dest;
    const unsigned int *s = src;

    while (count > 0) {
        *tmp++ = *s++;
        count -= 4;
    }

    return dest;
}
#ifdef MCUPM_PLATFORM_EMIMPU_ENABLE
#ifdef WITH_PLATFORM_MEDIATEK_COMMON_EMI
static void mcupm_emi_protect_enable(void *addr, bool is_abnormal)
{
    u32 type;
    status_t ret = NO_ERROR;
    /*
     * setup EMI MPU
     * domain 0: AP
     * domain 14: MCUPM
     */
    struct emi_region_info_t region_info;

    region_info.start = (unsigned long long)(unsigned int)addr;
    region_info.end = (unsigned long long)(unsigned int)
        (addr + MCUPM_MEM_SIZE);
    region_info.region = MCUPM_MPU_REGION;
    /* If ABNORMAL BOOT, MCUPM_MPU_REGION should be accessible for LKDump */
    type = (is_abnormal) ? NO_PROTECTION : FORBIDDEN;

    SET_ACCESS_PERMISSION(region_info.apc, UNLOCK,
                    FORBIDDEN, NO_PROTECTION, FORBIDDEN, FORBIDDEN,
                    FORBIDDEN, FORBIDDEN, FORBIDDEN, FORBIDDEN,
                    FORBIDDEN, FORBIDDEN, FORBIDDEN, FORBIDDEN,
                    FORBIDDEN, FORBIDDEN, FORBIDDEN, type);
    ret = emi_mpu_set_protection(&region_info);

    dprintf(CRITICAL, "mcupm enable emimpu ret=%d type=%X\n", ret, type);
}
#endif
#endif

status_t platform_load_mcupm(void)
{
    paddr_t mcupm_addr_pa = 0;
    void *mcupm_addr_va = NULL;
    status_t ret = NO_ERROR;
    int ret_val;
    size_t reserved_size;
    char part_name[8] = "mcupm";
    unsigned char *img, *mcupm_ldimg, *mcupm_img, *mcupm_pmimg, *mcupm_ximg;
    unsigned int mcupm_ldsize, mcupm_imgsize, mcupm_pmsize, mcupm_xsize;
    struct ptimg_hdr_t *hdr;
    vaddr_t tmp_addr;
    struct region_info_t *region_info = NULL;
    struct mcupm_info_t *info = NULL;

    if (mboot_params_current_abnormal_boot() != 1) {
    /* normal boot */
        mcupm_addr_pa = mblock_alloc(MCUPM_MEM_SIZE, MCUPM_MEM_ALIGN, MCUPM_MEM_LIMIT,
            0, 0, "MCUPM-reserved");
        LTRACEF("MCUPM: malloc start addr=0x%lx, size=0x%lx\n", mcupm_addr_pa, reserved_size);
    } else {
    /* abnormal boot */
        ret_val = mrdump_get_reserved_info("MCUPM-reserved", &mcupm_addr_pa, &reserved_size);
        if (ret_val < 0) {
            dprintf(CRITICAL, "MCUPM err: get reserved in fail\n");
            return ERR_NO_MEMORY;
        }
#ifdef MCUPM_PLATFORM_EMIMPU_ENABLE
#ifdef WITH_PLATFORM_MEDIATEK_COMMON_EMI
        mcupm_emi_protect_enable(mcupm_addr_pa, true);
#endif
#endif
        LTRACEF("MCUPM: abnormal start addr=0x%lx, size=0x%lx\n", mcupm_addr_pa, reserved_size);
    }

    //set vmm
    ret = vmm_alloc_physical(vmm_get_kernel_aspace(),
            "vm-MCUPM-reserved",
            MCUPM_MEM_SIZE,
            &mcupm_addr_va,
            PAGE_SIZE_SHIFT,
            (paddr_t)mcupm_addr_pa,
            0,
            ARCH_MMU_FLAG_CACHED);

    if (ret != NO_ERROR) {
        dprintf(CRITICAL, "MCUPM vmm_alloc_physical failed, err: %d\n", ret);
        goto err_vmm_alloc;
    }
    LTRACEF("%s MCUPM vmm_alloc_physical success\n", MOD);

#ifndef LEGACY_MCUPM_LOADER
    //Unlock SPM POWERON_CONFIG_EN
    write32(POWERON_CONFIG_EN, SPM_REGWR_CFG_KEY | BCLK_CG_EN_LSB);

    //Reinit CPUEB_PWR_CON
    // (1) CPUEB MTCMOS
    write32(CPUEB_PWR_CON, read32(CPUEB_PWR_CON) | CPUEB_PWR_ON_LSB);
    write32(CPUEB_PWR_CON, read32(CPUEB_PWR_CON) | CPUEB_PWR_ON_2ND_LSB);
    while ((read32(CPUEB_PWR_CON) & (SC_CPUEB_PWR_ACK_LSB | SC_CPUEB_PWR_ACK_2ND_LSB))
        != (SC_CPUEB_PWR_ACK_LSB | SC_CPUEB_PWR_ACK_2ND_LSB))
        ;
    write32(CPUEB_PWR_CON, read32(CPUEB_PWR_CON) & ~(CPUEB_PWR_CLK_DIS_LSB));
    write32(CPUEB_PWR_CON, read32(CPUEB_PWR_CON) & ~(CPUEB_PWR_ISO_LSB));
    write32(CPUEB_PWR_CON, read32(CPUEB_PWR_CON) | CPUEB_PWR_RST_B_LSB);
    LTRACEF("%s MCUPM MTCMOS Power on success\n", MOD);

    // (2) SRAM leave DORMANT mode
    write32(CPUEB_PWR_CON, read32(CPUEB_PWR_CON) | CPUEB_SRAM_SLEEP_B_LSB);
    while (!(read32(CPUEB_PWR_CON) & SC_CPUEB_SRAM_SLEEP_B_ACK_LSB))
    ;
    udelay(1);
    write32(CPUEB_PWR_CON, read32(CPUEB_PWR_CON) | CPUEB_SRAM_ISOINT_B_LSB);
    write32(CPUEB_PWR_CON, read32(CPUEB_PWR_CON) & ~(CPUEB_SRAM_CKISO_LSB));
    LTRACEF("%s MCUPM wakeup sram success\n", MOD);

    //Power on MCUPM sram
    write32(CPUEB_PWR_CON, read32(CPUEB_PWR_CON) & ~(0x1 << 8));
    do {
    } while ((read32(CPUEB_PWR_CON) & 0x1000) == 0x1000);
    LTRACEF("%s Power on MCUPM sram success\n", MOD);
#endif

    //initial mcupm info
    info = mcupm_addr_va;
    tmp_addr = (vaddr_t)mcupm_addr_va + ROUNDUP(sizeof(*info), 4);

    //load image from flash
    ret = load_partition(part_name, "tinysys-mcupm-RV33_A", (void *)tmp_addr, MCUPM_MEM_SIZE);
    if (ret < 0) {
        dprintf(CRITICAL, "mcupm load from partition failed, ret: %d\n", ret);
        goto err_load_partition;
    }

    LTRACEF("%s [load_partition MCUPM] Addr: 0x%lx, Size(%x)\n",
        MOD, tmp_addr, MCUPM_MEM_SIZE);
    LTRACEF("%s sync cache pa=%lx va=%p size=0x%x ret=%d\n",
        MOD, mcupm_addr_pa, mcupm_addr_va, MCUPM_MEM_SIZE, ret);

    ret = NO_ERROR;

    //load image to TCM
    mcupm_ldimg = mcupm_img = NULL;
    mcupm_ldsize = mcupm_imgsize = 0;

    mcupm_ximg = NULL;
    mcupm_xsize = 0;
    mcupm_pmsize = 0;
    hdr = (struct ptimg_hdr_t *) tmp_addr;

    while (hdr->magic == PT_MAGIC) {
        img = ((char *) hdr) + hdr->hdr_size;

        switch (hdr->id) {
        case PT_ID_MCUPM_PM:
            mcupm_pmimg = img;
            mcupm_pmsize = hdr->img_size;
            tmp_addr = mcupm_pmimg;
            break;
        case PT_ID_MCUPM_XFILE:
            mcupm_ximg = img;
            mcupm_xsize = hdr->img_size;
            break;
        default:
            dprintf(CRITICAL, "%s Warning: Ignore unknown MCUPM image %d\n", MOD, hdr->id);
            break;
        }
        img += ROUNDUP(hdr->img_size, hdr->align);
        hdr = (struct ptimg_hdr_t *) img;
    }

    //set xfile info for LK dump
    memset(info, 0, ROUNDUP(sizeof(*info), 4));
    if (mcupm_xsize > 0) {
        info->xfile_addr = (unsigned int)mcupm_ximg - sizeof(struct ptimg_hdr_t);
        info->xfile_sz = sizeof(struct ptimg_hdr_t) + mcupm_xsize + sizeof(unsigned int);
    }

    mcupm_ldimg = tmp_addr;
    mcupm_ldsize = (unsigned int)MCUPM_LOADER_SIZE;
    mcupm_img = tmp_addr + (unsigned int)MCUPM_IMG_OFS;
    mcupm_imgsize = (unsigned int)MCUPM_TCM_SZ - (unsigned int)MCUPM_IMG_OFS;

    write32(MCUPM_SW_RSTN, 0x0);
    mcupm_memcpy((unsigned char *) MCUPM_SRAM_BASE, mcupm_ldimg, mcupm_ldsize);

    //set region info
    region_info = (struct region_info_t *) (MCUPM_SRAM_BASE + 0x4);
    region_info->loader_start = vaddr_to_paddr(mcupm_ldimg);
    region_info->loader_size = mcupm_ldsize;
    region_info->fw_start = vaddr_to_paddr(mcupm_img);
    region_info->fw_size = mcupm_imgsize - RESERVED_SRAM_SIZE;
    region_info->rdmp_start = vaddr_to_paddr(tmp_addr + (unsigned int)MCUPM_RDMP_OFS);
    region_info->rdmp_size = MCUPM_TCM_SZ;

    /* set fw range as invalid for flush to DRAM */
    arch_clean_invalidate_cache_range((addr_t) mcupm_addr_va, (size_t) MCUPM_MEM_SIZE);

    LTRACEF("region info start = %x, size = %x, fw start = %x, size = %x, rdmp_start = %x\n",
                region_info->loader_start,
                region_info->loader_size,
                region_info->fw_start,
                region_info->fw_size,
                region_info->rdmp_start);

#if MCUPM_DBG_MODE
    /* MCUPM JTAG GPIO pinmux: */
    DRV_SetReg32(0x10005350, DRV_Reg32(0x10005350)|0x33300000);
    DRV_SetReg32(0x10005360, DRV_Reg32(0x10005360)|0x00000033);
    DRV_SetReg32(MCUPM_CFGREG_DBG_CON, DRV_Reg32(MCUPM_CFGREG_DBG_CON)|0x100);
#endif

    write32(MCUPM_INFO, (unsigned int) mcupm_addr_pa);
    // If not normal boot, notify mcupm to backup
    write32(MCUPM_ABNORMAL_BOOT, mboot_params_current_abnormal_boot());
    LTRACEF("%s MCUPM start with %s boot\n", MOD,
               read32(MCUPM_ABNORMAL_BOOT) ? "abnormal" : "normal");
    dprintf(INFO, "%s MCUPM start with %s boot\n", MOD,
               read32(MCUPM_ABNORMAL_BOOT) ? "abnormal" : "normal");

    write32(MCUPM_DM_ADDR, (unsigned int) region_info->rdmp_start);
    write32(MCUPM_DM_SZ, (unsigned int) region_info->rdmp_size);
    write32(MCUPM_WARM_BOOT, 0x0);
    write32(MCUPM_DEV_STATUS, START_MAGIC);

    LTRACEF("mcupm: abnormal_boot = 0x%x\n", mboot_params_current_abnormal_boot());

#define MCUPM_CFGREG_SW_RSTN_SW_RSTN  (0x1 << 0)
#define MCUPM_CFGREG_SW_RSTN_DMA_BUSY_MASK  (0x1 << 1)
    write32(MCUPM_SW_RSTN, MCUPM_CFGREG_SW_RSTN_SW_RSTN | MCUPM_CFGREG_SW_RSTN_DMA_BUSY_MASK);

    LTRACEF("mcupm: MCUPM_SRAM_GPR0: 0x%lx, 0x%x\n", MCUPM_INFO, read32(MCUPM_INFO));
    LTRACEF("mcupm: MCUPM_SRAM_GPR1: 0x%lx, 0x%x\n", MCUPM_ABNORMAL_BOOT,
                  read32(MCUPM_ABNORMAL_BOOT));
#ifdef MCUPM_PLATFORM_EMIMPU_ENABLE
#ifdef WITH_PLATFORM_MEDIATEK_COMMON_EMI
    if (mboot_params_current_abnormal_boot() != 1)
        mcupm_emi_protect_enable(mcupm_addr_pa, false);
#endif
#endif
    LTRACEF("%s MCUPM part. load & reset finished\n", MOD);

    return NO_ERROR;

err_load_partition:
    vmm_free_region(vmm_get_kernel_aspace(), (vaddr_t)mcupm_addr_va);

err_vmm_alloc:
    mblock_free(mcupm_addr_pa);
    return ret;
}
