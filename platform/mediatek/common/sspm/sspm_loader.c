/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <arch/ops.h>
#include <err.h>
#include <kernel/vm.h>
#include <libfdt.h>
#include <load_image.h>
#include <mblock.h>
#include <platform/addressmap.h>
#include <platform/mboot_params.h>
#include <platform/mrdump_params.h>
#include <platform_mtk.h>
#include <reg.h>
#include <set_fdt.h>
#include <sspm_plat.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <trace.h>
#ifdef SSPM_PLATFORM_EMIMPU_ENABLE
#include <emi_mpu.h>
#endif

#define LOCAL_TRACE 0
#define WA_MIRROR_OFFSET 0x400000000ULL

#ifdef LEGACY_EMIMPU
#define SSPM_EMI_ALIGN SSPM_MEM_ALIGN // Enable 64K alignment for legacy mpu
#else
#define SSPM_EMI_ALIGN PAGE_SIZE
#endif

struct ptimg_hdr_t {
    unsigned int magic;     /* magic */
    unsigned int hdr_size;  /* data size */
    unsigned int img_size;  /* img size */
    unsigned int align;     /* align */
    unsigned int id;        /* image id */
    unsigned int addr;      /* memory addr */
};

struct sspm_info_t {
    unsigned int sspm_dm_ofs; /* for sspm data memory */
    unsigned int sspm_dm_sz;
    unsigned int rd_ofs;      /* for sspm ramdump */
    unsigned int rd_sz;
    unsigned int xfile_ofs;  /* for LKdump */
    unsigned int xfile_sz;
};

static void *sspm_memcpy(void *dest, const void *src, int count)
{
    unsigned int *tmp = dest;
    const unsigned int *s = src;

    while (count > 0) {
        *tmp++ = *s++;
        count -= 4;
    }

    return dest;
}

#ifdef SSPM_PLATFORM_EMIMPU_ENABLE
static void sspm_emi_protect_enable(void *addr, bool is_abnormal)
{
    u32 type;
    status_t ret = NO_ERROR;
    /*
     * setup EMI MPU
     * domain 0: AP
     * domain 8: SSPM
     */
    struct emi_region_info_t region_info;

    region_info.start = (unsigned long long)(unsigned int)addr;
    region_info.end = (unsigned long long)(unsigned int)
        (addr + SSPM_MEM_SIZE);
    region_info.region = SSPM_MPU_REGION_ROM;

    /* If ABNORMAL BOOT, SSPM_MPU_REGION_ROM should be accessible for LKDump */
    type = (is_abnormal) ? SEC_RW_NSEC_R : FORBIDDEN;

    SET_ACCESS_PERMISSION(region_info.apc, UNLOCK,
                    FORBIDDEN, FORBIDDEN, FORBIDDEN, FORBIDDEN,
                    FORBIDDEN, FORBIDDEN, FORBIDDEN, SEC_RW,
                    FORBIDDEN, FORBIDDEN, FORBIDDEN, FORBIDDEN,
                    FORBIDDEN, FORBIDDEN, FORBIDDEN, type);
    ret = emi_mpu_set_protection(&region_info);

    dprintf(CRITICAL, "sspm enable emimpu ret=%d\n", ret);
}

static void sspm_share_memory_emi_protect(void *addr, unsigned int size)
{
    u32 type;
    status_t ret = NO_ERROR;
    /*
     * setup EMI MPU
     * domain 0: AP
     * domain 8: SSPM
     */
    struct emi_region_info_t region_info;

    region_info.start = (unsigned long long)(unsigned int)addr;
    region_info.end = (unsigned long long)(unsigned int)(addr + size);
    region_info.region = SSPM_MPU_REGION_SHARE;

    SET_ACCESS_PERMISSION(region_info.apc, UNLOCK,
                    FORBIDDEN, FORBIDDEN, FORBIDDEN, FORBIDDEN,
                    FORBIDDEN, FORBIDDEN, FORBIDDEN, SEC_RW,
                    FORBIDDEN, FORBIDDEN, FORBIDDEN, FORBIDDEN,
                    FORBIDDEN, FORBIDDEN, FORBIDDEN, NO_PROTECTION);
    ret = emi_mpu_set_protection(&region_info);

    dprintf(CRITICAL, "sspm enable share emimpu ret=%d\n", ret);
}
#endif

status_t platform_load_sspm(void)
{
    paddr_t sspm_addr_pa = 0;
    paddr_t sspm_mirror_addr_pa = 0;
    void *sspm_addr_va = NULL;
    vaddr_t load_addr = 0;
    char part_name[16] = "sspm";
    status_t ret = NO_ERROR;
    int ret_val;
    u64 reserved_size;
    bool boot_type; // 1 is for abnormal, 0 is for normal

    void *img = NULL, *pmimg = NULL, *dmimg = NULL, *cpmimg = NULL, *ximg = NULL;
    unsigned int pmsize = 0, dmsize = 0, cpmsize = 0, xsize = 0;
    struct ptimg_hdr_t *hdr;
    struct sspm_info_t *info;


    boot_type = mboot_params_current_abnormal_boot();

    if (boot_type != 1) {
        /* normal boot*/
        sspm_addr_pa = mblock_alloc(SSPM_MEM_SIZE, SSPM_EMI_ALIGN, SSPM_DRAM_ADDR_MAX,
        0, 0, "SSPM-reserved");
        dprintf(CRITICAL, "sspm: malloc start addr=0x%llx, size=0x%llx\n",
        sspm_addr_pa, SSPM_MEM_SIZE);

        if (need_relocate()) {
            sspm_mirror_addr_pa = mblock_alloc(SSPM_MEM_SIZE, SSPM_EMI_ALIGN, MBLOCK_NO_LIMIT,
            sspm_addr_pa + WA_MIRROR_OFFSET, 0, "SSPM-reserved");
            dprintf(CRITICAL, "sspm: mirror malloc start addr=0x%llx, size=0x%llx\n",
            sspm_mirror_addr_pa, SSPM_MEM_SIZE);
        }
    } else {
        /* abnormal boot*/
        ret_val = mrdump_get_reserved_info("SSPM-reserved", &sspm_addr_pa, &reserved_size);
        if (ret_val < 0) {
            dprintf(CRITICAL, "sspm err: get reserved in fail\n");
            return ERR_NO_MEMORY;
        }
#if SSPM_TBUF_SUPPORT
        save_tbuf();
#endif
        LTRACEF("sspm: abnormal start addr=0x%llx, size=0x%llx\n",
        sspm_addr_pa, reserved_size);
#ifdef LEGACY_EMIMPU
#ifdef SSPM_PLATFORM_EMIMPU_ENABLE
        /* Setup EMI MPU for abnormal reboot */
        sspm_emi_protect_enable((void *)sspm_addr_pa, boot_type);
#endif
#endif
#if SSPM_FULLY_COREDUMP
       return NO_ERROR;
#endif
    }

    if (sspm_addr_pa == 0) {
        dprintf(CRITICAL, "sspm mblock allocation failed\n");
        return ERR_NO_MEMORY;
    }

    ret = vmm_alloc_physical(vmm_get_kernel_aspace(),
                        "vm-SSPM-reserved",
                        SSPM_MEM_SIZE,
                        &sspm_addr_va,
                        PAGE_SIZE_SHIFT,
                        (paddr_t)sspm_addr_pa,
                        0,
                        ARCH_MMU_FLAG_CACHED);

    if (ret != NO_ERROR) {
        dprintf(CRITICAL, "sspm vmm_alloc_physical failed, err: %d\n", ret);
        goto err_vmm_alloc;
    }

    /* enable sram*/
    sspm_enable_sram();

    info = sspm_addr_va;

    load_addr = (vaddr_t)sspm_addr_va + ROUNDUP(sizeof(*info), 4);

    /* Setup EMI MPU first time*/

    /*load sspm img*/
    ret = load_partition(part_name, "tinysys-sspm", (void *)load_addr, SSPM_MEM_SIZE);
    if (ret < 0) {
        dprintf(CRITICAL, "sspm load failed, ret: %d\n", ret);
        goto err_load_partition;
    }
    /* separate ptimg */
    hdr = (struct ptimg_hdr_t *) load_addr;

    while (hdr->magic == PT_MAGIC) {
        img = ((char *) hdr) + hdr->hdr_size;

        switch (hdr->id) {
        case PT_ID_SSPM_PM:
            pmimg = img;
            pmsize = hdr->img_size;
            break;
        case PT_ID_SSPM_DM:
            dmimg = img;
            dmsize = hdr->img_size;
            break;
        case PT_ID_SSPM_CPM:
            cpmimg = img;
            cpmsize = hdr->img_size;
            break;
        case PT_ID_SSPM_XFILE:
            ximg = img;
            xsize = hdr->img_size;
            break;
        default:
            dprintf(CRITICAL, "sspm warning: Ignore unknown sspm image_%d\n", hdr->id);
            break;
        }

        img += ROUNDUP(hdr->img_size, hdr->align);
        hdr = (struct ptimg_hdr_t *) img;
    }
    LTRACEF("sspm pmimg: 0x%x(%d bytes), dmimg: 0x%x(%d bytes), ximg: 0x%x(%d bytes)\n",
        (u32)pmimg, pmsize, (u32)dmimg, dmsize, (u32)ximg, xsize);

    if (!pmimg || !dmimg || !ximg) {
        dprintf(CRITICAL, "sspm partition missing - PM:0x%x, DM:0x%x (@0x%x)\n",
        (u32)pmimg, (u32)dmimg, (u32)load_addr);

        goto err_load_partition;
    }

    if (cpmsize > SSPM_CPM_SZ) {
        dprintf(CRITICAL, "sspm .cpm (%d bytes) is oversize\n", cpmsize);
        goto err_load_partition;
    }

    /* load PM */
    sspm_memcpy((unsigned char *) SSPM_SRAM_BASE, pmimg, pmsize);

    /* Setup sspm_info  */
    memset(info, 0, ROUNDUP(sizeof(*info), 4));

    info->sspm_dm_ofs = (unsigned int)dmimg - (unsigned int)sspm_addr_va;
    info->sspm_dm_sz = dmsize;

    info->rd_ofs = ROUNDUP(SSPM_IMG_SZ, SSPM_MEM_ALIGN);
    info->rd_sz  = SSPM_TCM_SZ - pmsize;

    info->xfile_ofs = (unsigned int)ximg - sizeof(struct ptimg_hdr_t) - (unsigned int)sspm_addr_va;
    /* with postfix 'END'*/
    info->xfile_sz = sizeof(struct ptimg_hdr_t) + xsize + sizeof(unsigned int);
    LTRACEF("sspm_info @0x%x:dm@0x%x(%d bytes) rd@0x%x(%d bytes) xfile@0x%x(%d bytes)\n",
        (u32)info, info->sspm_dm_ofs, info->sspm_dm_sz,
        info->rd_ofs, info->rd_sz, info->xfile_ofs, info->xfile_sz);

    if ((pmsize + dmsize + xsize + 3 * sizeof(struct ptimg_hdr_t)) > SSPM_IMG_SZ) {
        dprintf(CRITICAL, "sspm error: IMG is over than %dK, the coredump will overwrite\n",
            SSPM_IMG_SZ/1024);
        goto err_load_partition;
    }

    /* re-allocate CPM */
    if (cpmimg != NULL) {
        load_addr = ROUNDUP(((int)sspm_addr_va + SSPM_IMG_SZ + SSPM_TCM_SZ), SSPM_DDR_ALIGN)
            + (SSPM_TCM_SZ - SSPM_DDR_SIZE);
        memcpy((void *)load_addr, (void *)cpmimg, cpmsize);
        writel(load_addr >> 18, SSPM_DDR_RMAP0);

        dprintf(CRITICAL, "Re-allocate CPM to 0x%x; DDR_RMAP is 0x%x\n",
            (u32)load_addr, readl(SSPM_DDR_RMAP0));
    }

    /* Setup SSPM WDT mode */
    sspm_wdt_set();

    /* Setup GPIO pinmux SSPM JTAG*/
    sspm_pinmux_set();

    /* Setup SSPM mode (normal or abnormal boot)*/
    /* If not normal boot, write 1 to gpr1 notify sspm abnormal boot*/
    LTRACEF("Setup sspm mode, is abnormal=%d\n", boot_type);
    writel(sspm_addr_pa, SSPM_CFGREG_GPR0);
    writel(boot_type, SSPM_CFGREG_GPR1);

    /*clean dcache & icache before set up EMI MPU*/
    LTRACEF("sspm sync cache pa=%lx va=%p size=0x%x\n", sspm_addr_pa, sspm_addr_va, SSPM_MEM_SIZE);
    arch_clean_invalidate_cache_range((addr_t)sspm_addr_va, SSPM_MEM_SIZE);

#ifdef SSPM_PLATFORM_EMIMPU_ENABLE
    /* Setup EMI MPU */
    sspm_emi_protect_enable((void *)sspm_addr_pa, boot_type);
#endif

    LTRACEF("sspm start to run\n");
    writel(VALUE_SW_RSTN, SSPM_SW_RSTN);

    return NO_ERROR;

err_load_partition:
    vmm_free_region(vmm_get_kernel_aspace(), (vaddr_t)sspm_addr_va);

err_vmm_alloc:
    if (boot_type != 1)
        mblock_free(sspm_addr_pa);

    return ret;
}

static void sspm_set_fdt(void *fdt)
{
    int nodeoffset;
    unsigned int *fdtptr = NULL;
    paddr_t res_rstart = 0;
    paddr_t mirror_res_rstart = 0;
    unsigned int res_rsize = 0;
    unsigned int tmp;
    int len = 0;

    nodeoffset = fdt_node_offset_by_compatible(fdt, -1, "mediatek,sspm");
    if (nodeoffset < 0)
        assert(0);

    fdtptr = (unsigned int *)fdt_getprop(fdt, nodeoffset, "sspm_res_ram_size", &len);
    if (fdtptr == NULL) {
        LTRACEF("sspm set_fdt skip if res_ram_size is not set\n");
        return;
    }

    res_rsize = fdt32_to_cpu(*(unsigned int *)fdtptr);

    res_rstart = mblock_alloc(res_rsize, SSPM_EMI_ALIGN, SSPM_DRAM_ADDR_MAX,
        0, 0, "sspm_ap-shared");
    dprintf(CRITICAL, "sspm: malloc for ap-shared start addr=0x%llx, size=0x%lx\n",
        res_rstart, res_rsize);

    if (need_relocate()) {
        mirror_res_rstart = mblock_alloc(res_rsize, SSPM_EMI_ALIGN, MBLOCK_NO_LIMIT,
            res_rstart + WA_MIRROR_OFFSET, 0, "sspm_ap-shared");
        dprintf(CRITICAL, "sspm: mirror malloc for ap-shared start addr=0x%llx, size=0x%lx\n",
            mirror_res_rstart, res_rsize);
    }

    if (res_rstart == 0) {
        dprintf(CRITICAL, "sspm_ap-shared allocation failed\n");
        assert(0);
    }
#ifdef SSPM_PLATFORM_EMIMPU_ENABLE
    /* enable emi mpu */
    sspm_share_memory_emi_protect((void *)res_rstart, res_rsize);
#endif
    tmp = cpu_to_fdt32((unsigned int)res_rstart);
    if (fdt_setprop(fdt, nodeoffset, "sspm_res_ram_start", &tmp, sizeof(tmp)) < 0) {
        dprintf(CRITICAL, "sspm set sspm_res_ram_start fails\n");
        assert(0);
    }
}
SET_FDT_INIT_HOOK(sspm_set_fdt, sspm_set_fdt);
