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
#include "gpueb.h"
#include <kernel/vm.h>
#include <load_image.h>
#include <mblock.h>
#include <platform/mboot_params.h>
#include <platform/reg.h>
#include <platform/wait.h>
#include <platform_mtk.h>
#include <platform/mrdump_params.h>
#include <string.h>
#include <trace.h>
#include <libfdt.h>
#include <smc.h>
#include <smc_id_table.h>

#define LOCAL_TRACE         0
#define BOOT_TIMEOUT_US     1000000
#define PAGE_SHIFT          12

#define udelay(x)           spin(x)

#ifdef WITH_PLATFORM_MEDIATEK_COMMON_EMI
#include <emi_mpu.h>
#endif

static void *gpueb_memcpy(void *dst, const void *src, int count)
{
    unsigned int *tmp = dst;
    const unsigned int *s = src;

    LTRACEF("%s: src(0x%p): %x, dst(0x%p): %x\n",
        MOD, s, *s, tmp, *tmp);

    while (count > 0) {
#if GPUEB_FPGA_PLATFORM
        if (count % 0x2000 == sizeof(unsigned int))
            LTRACEF("%s: src(0x%p): %x, dst(0x%p): %x\n",
                MOD, s, *s, tmp, *tmp);
#endif

        *tmp++ = *s++;
        count -= sizeof(unsigned int);

#if GPUEB_FPGA_PLATFORM
        udelay(500);
#endif
    }

    return dst;
}

void *gpueb_clear(void *dest, const unsigned int clear, int count)
{
    unsigned int *tmp = dest;

    LTRACEF("%s: dest[0x%x], claer(%d)\n",
        MOD, (unsigned int)tmp, clear);
    while (count > 0) {
        *tmp++ = clear;
        count -= sizeof(unsigned int);
#if GPUEB_FPGA_PLATFORM
        udelay(500);
#endif
    }
    return dest;
}

static void gpueb_update_fdt(void *fdt, paddr_t gpueb_mem_pa, unsigned int gpueb_mem_sz)
{
    int offset;
    status_t ret = NO_ERROR;

    offset = fdt_path_offset(fdt, "/soc/gpueb");
    /* if gpueb node not found, skip fdt update */
    if (offset < 0)
        return;

    ret = fdt_setprop_u64(fdt, offset, "gpueb_mem_addr", gpueb_mem_pa);
    if (ret)
        LTRACEF("%s: fail to fdt_setprop_u64: gpueb_mem_addr(0x%lx) (%d)\n",
            MOD, gpueb_mem_pa, ret);

    ret = fdt_setprop_u64(fdt, offset, "gpueb_mem_size", gpueb_mem_sz);
    if (ret)
        LTRACEF("%s: fail to fdt_setprop_u64: gpueb_mem_size(0x%x) (%d)\n",
            MOD, gpueb_mem_sz, ret);

    LTRACEF("%s: gpueb_mem_pa: 0x%lx, gpueb_mem_sz: 0x%x\n", MOD, gpueb_mem_pa, gpueb_mem_sz);
}

#if GPUEB_XFILE_SUPPORT
status_t gpueb_reserve_xfile(paddr_t gpueb_shared_pa, addr_t gpueb_xfile, unsigned int gpueb_xsize)
{
    vaddr_t gpueb_xfile_on_dram_va = 0;
    vaddr_t gpu_shared_dram_va = 0;
    status_t ret = NO_ERROR;

    LTRACEF("%s [GPUEB] XFILE Complete setting: gpueb_xfile(0x%lx), gpueb_xsize(%x)\n",
        MOD, gpueb_xfile, gpueb_xsize);

    if (gpueb_xsize == 0 || gpueb_xsize > (unsigned int) AEE_LKDUMP_GPUEB_XFILE_SZ) {
        dprintf(CRITICAL, "%s: xfile invalid input: gpueb_xfile(0x%lx), gpueb_xsize(%x)\n",
            MOD, gpueb_xfile, gpueb_xsize);
        return ERR_GENERIC;
    }

    ret = vmm_alloc_physical(vmm_get_kernel_aspace(),
            "vm-GPU-shared-reserved",
            GPUEB_SHARED_MEM_SIZE,
            (void *)&gpu_shared_dram_va,
            PAGE_SIZE_SHIFT,
            gpueb_shared_pa,
            0,
            ARCH_MMU_FLAG_CACHED);
    if (ret != NO_ERROR) {
        dprintf(CRITICAL, "%s: vmm failed for gpu shared dram, err: %d\n", MOD, ret);
        return ret;
    }
    LTRACEF("%s: vmm_alloc_physical: PA[0x%lx], VA[0x%lx]\n",
        MOD, gpueb_shared_pa, gpu_shared_dram_va);

    arch_clean_invalidate_cache_range((addr_t)gpu_shared_dram_va, GPUEB_SHARED_MEM_SIZE);

    gpueb_xfile_on_dram_va = gpu_shared_dram_va + GPUEB_XFILE_DRAM_OFS;
    LTRACEF("%s: gpueb_xfile_on_dram_pa: 0x%lx, gpueb_xfile_on_dram_va: 0x%lx\n",
        MOD, gpueb_shared_pa + GPUEB_XFILE_DRAM_OFS, gpueb_xfile_on_dram_va);

    /* Copy whole xfile (with header image) into DRAM */
    LTRACEF("%s [GPUEB] Preparing Copy XFILE(0x%lx) to reserved DRAM(0x%lx): Size=%d\n",
        MOD, gpueb_xfile, gpueb_xfile_on_dram_va, gpueb_xsize);
    gpueb_memcpy((void *)gpueb_xfile_on_dram_va, (void *)gpueb_xfile, (int)gpueb_xsize);
    LTRACEF("%s [GPUEB] Copy XFILE(0x%lx) to reserved DRAM(0x%lx): Size=%x\n",
        MOD, gpueb_xfile, gpueb_xfile_on_dram_va, gpueb_xsize);

    vmm_free_region(vmm_get_kernel_aspace(), gpueb_xfile_on_dram_va);

    return NO_ERROR;
}
#endif

status_t platform_load_gpueb(void *fdt)
{
    paddr_t gpueb_fw_addr_pa = 0, gpueb_shared_addr_pa = 0, gpumpu_addr_pa = 0;
    vaddr_t gpueb_fw_addr_va = 0;
    status_t ret = NO_ERROR;
    char part_name[8] = "gpueb";
    addr_t gpueb_img;
    unsigned int gpueb_imgsize;
    struct __smccc_res res;
#if GPUEB_XFILE_SUPPORT
    unsigned char *img;
    addr_t gpueb_xfile;
    unsigned int gpueb_xsize;
    struct ptimg_hdr_t *hdr = NULL;
#endif
    addr_t tmp_addr = 0;
    struct gpueb_info_t *info = NULL;
#ifdef WITH_PLATFORM_MEDIATEK_COMMON_EMI
    struct emi_region_info_t region_info;
#endif

    LTRACEF("%s: start\n", MOD);

    if (!(read32(SPM_XPU_PWR_STATUS)&SPM_XPU_PWR_STATUS_MFG0)) {
        dprintf(CRITICAL, "%s: GPUEB(MFG0) NOT power-on!!!\n", MOD);
        return ERR_OFFLINE;
    }

#if NEED_MFG1_PWR_CHECK
    if (read32(SPM_MFG1_PWR_CON) == MFG1_POWERED_ON) {
        dprintf(CRITICAL, "%s: MFG1 already powered on!!!\n", MOD);
        return ERR_ALREADY_EXISTS;
    }
#endif

    /* Allocate mblock for PA
     * Note GPR is 32bits, we can't alloc with address than 4GB
     */
    gpueb_fw_addr_pa = mblock_alloc(GPUEB_FW_MEM_SIZE,
        GPUEB_MEM_ALIGN, GPUEB_MEM_LIMIT, 0, 0, "me_GPUEB_FW");
    if (!gpueb_fw_addr_pa) {
        dprintf(CRITICAL, "%s: GPUEB-FW mblock allocation failed\n", MOD);
        return ERR_NO_MEMORY;
    }
    LTRACEF("%s: [GPUEB-FW][mblock alloc] PA: 0x%lx, Size(%x) Align(%x)\n",
        MOD, gpueb_fw_addr_pa, GPUEB_FW_MEM_SIZE, GPUEB_MEM_ALIGN);

    /* Reserve memory for communication between GPUEB and APMCU in kernel */
    /* need to workaround SMPU due to insufficient size of SPMU */
    if (need_relocate())
        gpueb_shared_addr_pa = mblock_alloc_range(GPUEB_SHARED_MEM_SIZE, GPUEB_MEM_ALIGN,
            GPUEB_MEM_LOWER_BOUND, GPUEB_MEM_UPPER_BOUND, 0, 0, "me_GPUEB_SHARED");
    /* no need to workaround */
    else
        gpueb_shared_addr_pa = mblock_alloc(GPUEB_SHARED_MEM_SIZE, GPUEB_MEM_ALIGN,
            GPUEB_MEM_LIMIT, 0, 0, "me_GPUEB_SHARED");
    if (!gpueb_shared_addr_pa) {
        dprintf(CRITICAL, "%s: GPUEB-SHARED mblock allocation failed\n", MOD);
        return ERR_NO_MEMORY;
    }

#ifdef WITH_PLATFORM_MEDIATEK_COMMON_EMI
    region_info.start = gpueb_shared_addr_pa;
    region_info.end = (gpueb_shared_addr_pa + GPUEB_SHARED_MEM_SIZE);
    region_info.region = REGION_ID_GPU_EB_RESERVE;
    ret = emi_mpu_set_protection(&region_info);
    LTRACEF("%s: [GPUEB-SHARED][mblock alloc] PA: 0x%lx, Size(%x) Align(%x) Region-%d(%d)\n",
        MOD, gpueb_shared_addr_pa, GPUEB_SHARED_MEM_SIZE, GPUEB_MEM_ALIGN,
        REGION_ID_GPU_EB_RESERVE, ret);
#else
    LTRACEF("%s: [GPUEB-SHARED][mblock alloc] PA: 0x%llx, Size(%x) Align(%x)\n",
        MOD, gpueb_shared_addr_pa, GPUEB_SHARED_MEM_SIZE, GPUEB_MEM_ALIGN);
#endif
    gpueb_update_fdt(fdt, gpueb_shared_addr_pa, GPUEB_SHARED_MEM_SIZE);

    /* Reserve memory for GPUMPU & Secure GPU memory heap
     * Shared the region is dirty design due to lack of MPU regions
     * Number of MPU regions would increased in 2022 projs
     */
    /* need to workaround SMPU due to insufficient size of SPMU */
    if (need_relocate())
        gpumpu_addr_pa = mblock_alloc_range(GPUMPU_MEM_SIZE + GPU_FW_PRV_HEAP,
            GPUEB_MEM_ALIGN, GPUEB_MEM_LOWER_BOUND, GPUEB_MEM_UPPER_BOUND,
            0, 0, "me_GPUmputab_PMA");
    /* no need to workaround */
    else
        gpumpu_addr_pa = mblock_alloc(GPUMPU_MEM_SIZE + GPU_FW_PRV_HEAP,
            GPUEB_MEM_ALIGN, GPUEB_MEM_LIMIT, 0, 0, "me_GPUmputab_PMA");
    if (!gpumpu_addr_pa) {
        dprintf(CRITICAL, "%s: GPUMPU mblock allocation failed\n", MOD);
        return ERR_NO_MEMORY;
    }
#ifdef WITH_PLATFORM_MEDIATEK_COMMON_EMI
    region_info.start = gpumpu_addr_pa;
    region_info.end = (gpumpu_addr_pa + GPUMPU_MEM_SIZE + GPU_FW_PRV_HEAP);
    region_info.region = REGION_ID_GPU_MPU_TAB;
    ret = emi_mpu_set_protection(&region_info);
    LTRACEF("%s: [GPUMPU][mblock alloc] PA: 0x%lx, Size(%x) Align(%x) Region-%d(%d)\n",
        MOD, gpumpu_addr_pa, GPUMPU_MEM_SIZE + GPU_FW_PRV_HEAP, GPUEB_MEM_ALIGN,
        REGION_ID_GPU_MPU_TAB, ret);
#else
    LTRACEF("%s: [GPUMPU][mblock alloc] PA: 0x%lx, Size(%x) Align(%x)\n",
        MOD, gpumpu_addr_pa, GPUMPU_MEM_SIZE + GPU_FW_PRV_HEAP, GPUEB_MEM_ALIGN);
#endif

    //Set vmm for VA
    ret = vmm_alloc_physical(vmm_get_kernel_aspace(),
            "vm-GPUEB-reserved",
            GPUEB_FW_MEM_SIZE,
            (void *)&gpueb_fw_addr_va,
            PAGE_SIZE_SHIFT,
            gpueb_fw_addr_pa,
            0,
            ARCH_MMU_FLAG_CACHED);

    if (ret != NO_ERROR) {
        dprintf(CRITICAL, "%s: GPUEB vmm_alloc_physical failed, err: %d\n", MOD, ret);
        goto err_vmm_alloc;
    }

    LTRACEF("%s: [GPUEB-FW][vmm_alloc_physical] PA: 0x%lx, VA: 0x%lx, Size(%x) page(%x)\n",
        MOD, gpueb_fw_addr_pa, gpueb_fw_addr_va, GPUEB_FW_MEM_SIZE, PAGE_SIZE_SHIFT);

    //Initial GPUEB info
    info = (struct gpueb_info_t *)gpueb_fw_addr_va;
    tmp_addr = gpueb_fw_addr_va + ROUNDUP(sizeof(*info), 4);
    //load image from flash
    ret = load_partition(part_name, "tinysys-gpueb-RV33_A", (void *)tmp_addr, GPUEB_FW_MEM_SIZE);
    if (ret < 0) {
        dprintf(CRITICAL, "%s: GPUEB load from partition failed, ret: %d\n", MOD, ret);
        goto err_load_partition;
    }

    LTRACEF("%s: [load_partition GPUEB] Addr: 0x%lx, Size(%x)\n",
        MOD, tmp_addr, GPUEB_FW_MEM_SIZE);

    arch_clean_invalidate_cache_range((addr_t)gpueb_fw_addr_va, GPUEB_FW_MEM_SIZE);
    LTRACEF("%s: sync cache PA: 0x%lx, VA: 0x%lx, size(0x%x)\n",
        MOD, gpueb_fw_addr_pa, gpueb_fw_addr_va, GPUEB_FW_MEM_SIZE);

    ret = NO_ERROR;

    //load image to GPUEB SRAM
    gpueb_img = 0;
    gpueb_imgsize = 0;

#if GPUEB_XFILE_SUPPORT
    gpueb_xfile = 0;
    gpueb_xsize = 0;

    hdr = (struct ptimg_hdr_t *) tmp_addr;

    while (hdr->magic == PT_MAGIC) {
        LTRACEF("%s hdr: magic(0x%x), hdr_size(0x%x), img_size(0x%x)\n",
            MOD, hdr->magic, hdr->hdr_size, hdr->img_size);
        LTRACEF("%s hdr: align(0x%x), id(0x%x), addr(0x%x)\n",
            MOD, hdr->align, hdr->id, hdr->addr);

        img = ((unsigned char *) hdr) + hdr->hdr_size;

        switch (hdr->id) {
        case PT_ID_GPUEB_FW:
            /* GPUEB FW no need include header */
            gpueb_img = (addr_t) img;
            gpueb_imgsize = hdr->img_size;
            break;
        case PT_ID_GPUEB_XFILE:
            /* Reserved gpueb xfile should include header */
            gpueb_xfile = (addr_t) (img - hdr->hdr_size);
            gpueb_xsize = hdr->hdr_size + hdr->img_size;
            break;
        default:
            dprintf(CRITICAL, "%s: warning: Ignore unknown GPUEB image %d\n", MOD, hdr->id);
            break;
        }
        img += ROUNDUP(hdr->img_size, hdr->align);
        hdr = (struct ptimg_hdr_t *) img;
    }

    LTRACEF("%s: gpueb img: 0x%lx(%d bytes), gpueb xfile: 0x%lx(%d bytes)\n",
        MOD, gpueb_img, gpueb_imgsize, gpueb_xfile, gpueb_xsize);

    if (!gpueb_img || !gpueb_xfile) {
        dprintf(CRITICAL, "%s: partition missing - img:0x%lx, xfile:0x%lx (@0x%lx)\n",
            MOD, gpueb_img, gpueb_xfile, tmp_addr);
        goto err_load_partition;
    }

    if (gpueb_imgsize == 0 || gpueb_imgsize > GPUEB_SRAM_SIZE) {
        dprintf(CRITICAL, "%s: invalid gpueb_imgsize (%d bytes)\n", MOD, gpueb_imgsize);
        goto err_load_partition;
    }

    if (gpueb_reserve_xfile(gpueb_shared_addr_pa, gpueb_xfile, gpueb_xsize) < 0)
        dprintf(CRITICAL, "%s: Warning: GPUEB reserve xfile fail\n", MOD);

#else
    dprintf(CRITICAL, "%s: info: XFILE not supported.\n", MOD);
    gpueb_img = tmp_addr;
    gpueb_imgsize = (unsigned int)GPUEB_COPY_SIZE;
#endif

    LTRACEF("%s: Image Complete setting: img(0x%lx), size(0x%x)\n",
        MOD, gpueb_img, gpueb_imgsize);

    write32(GPUEB_CFGREG_SW_RSTN, 0x0);
    LTRACEF("%s: init GPUEB_CFGREG_SW_RSTN\n", MOD);

    gpueb_clear((void *)GPUEB_SRAM_BASE, 0x0, (int)GPUEB_SRAM_SIZE);

#if GPUEB_SYARAM_DATA_SUPPORT
    gpueb_clear((void *)GPUEB_RSV_SYSRAM_BASE, 0x0, (int)GPUEB_SYSRAM_DATA_SZ);
#endif

    /* Copy whole image(no mtk header image) into SRAM */
    LTRACEF("%s: Preparing Copy ld & image(0x%lx) to GPUEB SRAM(0x%lx): Size=%d\n",
        MOD, gpueb_img, GPUEB_SRAM_BASE, gpueb_imgsize);
    gpueb_memcpy((void *)GPUEB_SRAM_BASE, (void *)gpueb_img, (int)gpueb_imgsize);
    LTRACEF("%s: Copy ld & image(0x%lx) to GPUEB SRAM(0x%lx): Size=%x\n",
        MOD, gpueb_img, GPUEB_SRAM_BASE, gpueb_imgsize);

#if GPUEB_DBG_MODE
    /* GPUEB JTAG GPIO pinmux: */
    DRV_SetReg32(0x10005350, DRV_Reg32(0x10005350)|0x33300000);
    DRV_SetReg32(0x10005360, DRV_Reg32(0x10005360)|0x00000033);
    DRV_SetReg32(GPUEB_CFGREG_DBG_CON, DRV_Reg32(GPUEB_CFGREG_DBG_CON)|0x100);
#endif

    write32(GPUEB_INFO, (unsigned int)gpueb_fw_addr_pa);
    // If not normal boot, notify GPUEB to backup
    write32(GPUEB_ABNORMAL_BOOT, 0x0);
    LTRACEF("%s: GPUEB start with %s boot\n",
        MOD, (read32(GPUEB_ABNORMAL_BOOT) ? "abnormal" : "normal"));
    //write32(GPUEB_DM_ADDR, (unsigned int)region_info->rdmp_start);
    //write32(GPUEB_DM_SZ, (unsigned int)region_info->rdmp_size);
    write32(GPUEB_WARM_BOOT, 0x0);
    LTRACEF("%s: ram_console_is_abnormal_boot = 0x%x\n", MOD, 0x0);

    /* Pass gpueb_shared_addr_pa to TF-A */
    __smc_conduit(MTK_SIP_BL_TINYSYS_GPUEB_CONTROL,
            GPUEB_SMC_BL_OP_SHAREDRAM,
            gpueb_shared_addr_pa, 0, 0, 0,
            0, 0, &res);
    if ((int) res.a0 < 0) {
        dprintf(CRITICAL, "%s: [GPUEB-SHARED] Invalid DRAM addr(0x%llx)\n",
            MOD, gpueb_shared_addr_pa);
    } else {
        dprintf(CRITICAL, "%s: [GPUEB-SHARED] Pass DRAM addr(0x%llx) by SMC\n",
            MOD, gpueb_shared_addr_pa);
    }

    /* GPUMPU set info */
    gpumpu_addr_pa = gpumpu_addr_pa >> PAGE_SHIFT;
    write32(GPUMPU_RSV_ADDR, (unsigned int)gpumpu_addr_pa);
    LTRACEF("%s: GPUMPU_RSV_ADDR(0x%016lx): 0x%x\n", MOD, GPUMPU_RSV_ADDR, read32(GPUMPU_RSV_ADDR));

    //Release reset to trigger boot-up
    write32(GPUEB_CFGREG_SW_RSTN,
        GPUEB_CFGREG_SW_RSTN_SW_RSTN | GPUEB_CFGREG_SW_RSTN_DMA_BUSY_MASK);

    LTRACEF("%s: GPUEB_SRAM_GPR0(0x%08lx): 0x%x\n", MOD, GPUEB_INFO, read32(GPUEB_INFO));
    LTRACEF("%s: GPUEB_SRAM_GPR1(0x%08lx): 0x%x\n",
        MOD, GPUEB_ABNORMAL_BOOT, read32(GPUEB_ABNORMAL_BOOT));

    LTRACEF("%s: GPUEB part. load & reset finished\n", MOD);

    // Check Magic number for polling GPUEB complete
    if (wait_us((read32(GPUEB_INIT_FOOTPRINT) == GPUEB_BOOT_UP_COMPLETE), BOOT_TIMEOUT_US) == 0) {
        dprintf(CRITICAL, "%s: GPUEB boot-up timed out!!!\n", MOD);
        dprintf(CRITICAL, "%s: GPUEB_PLL = 0x%x\n", MOD, read32(GPUEB_PLL));
        dprintf(CRITICAL, "%s: GPUEB_MUX = 0x%x\n", MOD, read32(GPUEB_MUX));
        dprintf(CRITICAL, "%s: GPUEB_INIT_FOOTPRINT = 0x%x\n", MOD, read32(GPUEB_INIT_FOOTPRINT));
        dprintf(CRITICAL, "%s: GPUFREQ_FOOTPRINT_GPR = 0x%x\n", MOD, read32(GPUFREQ_FOOTPRINT_GPR));
        dprintf(CRITICAL, "%s: MFG1_PWR_CON = 0x%x\n", MOD, read32(SPM_MFG1_PWR_CON));
        return ERR_TIMED_OUT;
    }

    LTRACEF("%s: GPUEB boot-up complete\n", MOD);

err_load_partition:
    vmm_free_region(vmm_get_kernel_aspace(), gpueb_fw_addr_va);

err_vmm_alloc:
    mblock_free(gpueb_fw_addr_pa);
    return ret;
}
