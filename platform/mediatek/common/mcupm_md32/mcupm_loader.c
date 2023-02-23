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
#include <platform/reg.h>
#include <string.h>
#include <trace.h>

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

status_t platform_load_mcupm(void)
{
    u64 mcupm_addr_pa;
    void *mcupm_addr_va;
    status_t ret = NO_ERROR;
    char part_name[8] = "mcupm";
    unsigned char *img, *mcupm_ldimg, *mcupm_img, *mcupm_pmimg, *mcupm_ximg;
    unsigned int mcupm_ldsize, mcupm_imgsize, mcupm_pmsize, mcupm_xsize;
    unsigned char *mcupm_dmimg;
    unsigned int mcupm_dmsize;
    struct ptimg_hdr_t *hdr;
    vaddr_t tmp_addr;
    struct mcupm_info_t *info = NULL;

    //allocate mblock
    mcupm_addr_pa = mblock_alloc(MCUPM_MEM_SIZE, MCUPM_MEM_ALIGN, MCUPM_MEM_LIMIT,
            0, 0, "MCUPM-reserved");

    if (!mcupm_addr_pa) {
        dprintf(CRITICAL, "MCUPM mblock allocation failed\n");
        return ERR_NO_MEMORY;
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

    //initial mcupm info
    info = mcupm_addr_va;
    tmp_addr = (vaddr_t)mcupm_addr_va + ROUNDUP(sizeof(*info), 4);

    //load image from flash
    ret = load_partition(part_name, "tinysys-mcupm", (void *)tmp_addr, MCUPM_MEM_SIZE);
    if (ret < 0) {
        dprintf(CRITICAL, "mcupm load from partition failed, ret: %d\n", ret);
        goto err_load_partition;
    }

    ret = NO_ERROR;

    //load image to TCM
    mcupm_dmimg = NULL;
    mcupm_dmsize = 0;
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
	case PT_ID_MCUPM_DM:
		mcupm_dmimg = img;
		mcupm_dmsize = hdr->img_size;
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

    dprintf("mcupm_pmimg: 0x%x(%d bytes), mcupm_dmimg: 0x%x(%d bytes), mcupm_ximg: 0x%x(%d bytes), CFG_MCUPM_MEMADDR: 0x%x\n",
	    (u32)mcupm_pmimg, mcupm_pmsize, (u32)mcupm_dmimg, mcupm_dmsize, (u32)mcupm_ximg, mcupm_xsize, CFG_MCUPM_MEMADDR);

    //set xfile info for LK dump
    memset(info, 0, ROUNDUP(sizeof(*info), 4));
    write32(MCUPM_SW_RSTN, 0x0);

    if (mcupm_pmsize <= SRAM_32KB_SIZE) {
	    mcupm_memcpy((unsigned char *) CFG_MCUPM_MEMADDR, mcupm_pmimg, mcupm_pmsize);
    } else {mcupm_memcpy((unsigned char *) CFG_MCUPM_MEMADDR, mcupm_pmimg, SRAM_32KB_SIZE);
	    mcupm_memcpy((unsigned char *) SRAM_32KB_AP_BASE, mcupm_pmimg + SRAM_32KB_SIZE, mcupm_pmsize - SRAM_32KB_SIZE);
    }

    info->mcupm_dm_ofs = (unsigned int)mcupm_dmimg - (unsigned int)info;
    info->mcupm_dm_sz = mcupm_dmsize;
    info->rd_ofs = ROUNDUP(MCUPM_IMG_SZ, MCUPM_MEM_ALIGN);
    info->rd_sz  = MCUPM_TCM_SZ - mcupm_pmsize;
    info->xfile_addr = (unsigned int)mcupm_ximg - sizeof(struct ptimg_hdr_t);
    info->xfile_sz = sizeof(struct ptimg_hdr_t) + mcupm_xsize + sizeof(unsigned int); // with postfix 'END'

    arch_clean_invalidate_cache_range((addr_t) mcupm_addr_va, (size_t) MCUPM_MEM_SIZE);

    LTRACEF("mcupm_info @0x%x: dm offset 0x%x (%d bytes), rd offset 0x%x(%d bytes), xfile addr 0x%x(%d bytes)\n",
	    (u32)info, info->mcupm_dm_ofs, info->mcupm_dm_sz, info->rd_ofs, info->rd_sz, info->xfile_addr, info->xfile_sz);

    if ( (mcupm_pmsize + mcupm_dmsize + mcupm_xsize + 3 * sizeof(struct ptimg_hdr_t)) > MCUPM_IMG_SZ) {
	LTRACEF("%s Warning: MCUPM IMG is over than %dK, the coredump will overwrite it!\n", MOD, MCUPM_IMG_SZ/1024);
    }

#if MCUPM_DBG_MODE
    /* MCUPM JTAG GPIO pinmux: */
    DRV_SetReg32(0x10005350, DRV_Reg32(0x10005350)|0x33300000);
    DRV_SetReg32(0x10005360, DRV_Reg32(0x10005360)|0x00000033);
    DRV_SetReg32(MCUPM_CFGREG_DBG_CON, DRV_Reg32(MCUPM_CFGREG_DBG_CON)|0x100);
#endif

    write32(MCUPM_INFO, (unsigned int) mcupm_addr_pa);
    write32(MCUPM_ABNORMAL_BOOT, 0);

#define MCUPM_CFGREG_SW_RSTN_SW_RSTN  (0x1 << 0)
#define MCUPM_CFGREG_SW_RSTN_DMA_BUSY_MASK  (0x1 << 1)
    write32(MCUPM_SW_RSTN, MCUPM_CFGREG_SW_RSTN_SW_RSTN | MCUPM_CFGREG_SW_RSTN_DMA_BUSY_MASK);

    LTRACEF("mcupm: MCUPM_SRAM_GPR0: 0x%x, 0x%x\n", MCUPM_INFO, read32(MCUPM_INFO));
    LTRACEF("mcupm: MCUPM_SRAM_GPR1: 0x%x, 0x%x\n", MCUPM_ABNORMAL_BOOT,
                  read32(MCUPM_ABNORMAL_BOOT));

    LTRACEF("%s MCUPM part. load & reset finished\n", MOD);

err_load_partition:
    vmm_free_region(vmm_get_kernel_aspace(), (vaddr_t)mcupm_addr_va);

err_vmm_alloc:
    mblock_free(mcupm_addr_pa);
    return ret;
}
