/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include "apusys_rv.h"
#include <arch/mmu.h>
#include <arch/ops.h>
#include <assert.h>
#include <debug.h>
#include <err.h>
#include <errno.h>
#include <kernel/vm.h>
#include <libfdt.h>
#include <load_image.h>
#include <mblock.h>
#include <partition_utils.h>
#include <platform/apusys_rv_plat.h>
#include <platform_mtk.h>
#include <string.h>
#include <trace.h>

/* for MPU protection */
#if ENABLE_APUSYS_EMI_PROTECTION
#include <emi_mpu.h>
#endif

/* for APUSYS DEVACP init hook at BL33 */
#include <init_mtk.h>
#include <smc.h>
#include <smc_id_table.h>

#define LOCAL_TRACE 0

static int apusys_rv_prop_off(void *fdt)
{
    int off = 0, len = 0, namestroff = 0;
    const struct fdt_property *prop;

    off = fdt_path_offset(fdt, "/apusys_rv");
    if (off < 0) {
        off = fdt_path_offset(fdt, "/soc/apusys_rv");
        if (off < 0) {
            dprintf(CRITICAL, "%s: apusys_rv not found\n", __func__);
            return -ERR_NOT_FOUND;
        }
    }

    prop = fdt_get_property(fdt, off, "status", &len);
    if (prop) {
        namestroff = fdt32_to_cpu(prop->nameoff);
        dprintf(CRITICAL, "%s: %s=%s\n", __func__, fdt_string(fdt, namestroff), (char *)prop->data);

        if (strncmp((char *)prop->data, "okay", strlen("okay")))
            return -ERR_NOT_READY;
    } else {
        dprintf(CRITICAL, "%s: prop is NULL\n", __func__);
        return -ERR_NOT_FOUND;
    }

    return NO_ERROR;
}

static int apusys_rv_get_buf_sz(void *fdt, const char *name)
{
    int off = 0, len = 0, namestroff = 0, ret = 0;
    const struct fdt_property *prop;

    off = fdt_path_offset(fdt, "/apusys_rv");
    if (off < 0) {
        off = fdt_path_offset(fdt, "/soc/apusys_rv");
        if (off < 0) {
            dprintf(CRITICAL, "%s: apusys_rv not found\n", __func__);
            return -ERR_NOT_FOUND;
        }
    }

    prop = fdt_get_property(fdt, off, name, &len);
    if (prop) {
        namestroff = fdt32_to_cpu(prop->nameoff);
        ret = fdt32_to_cpu(*(unsigned int *) prop->data);
        LTRACEF("%s: %s=<0x%x>\n", __func__, fdt_string(fdt, namestroff), ret);
    } else {
        dprintf(CRITICAL, "%s: prop is NULL\n", __func__);
        return -ERR_NOT_FOUND;
    }

    return ret;
}

static off_t get_apusys_rv_part_size(const char *part_name)
{
    int i;
    bdev_t *dev;
    off_t ret = 0;
    int name_list_cnt = 0;
    char *name_list[NUM_PARTITIONS] = {NULL};
    const char *label;

    partition_get_name_list((char **)&name_list, &name_list_cnt);

    label = NULL;
    for (i = 0; i < name_list_cnt; i++) {
        if (!strncmp(name_list[i], part_name, strlen(part_name))) {
            label = name_list[i];
            break;
        }
    }

    if (!label) {
        dprintf(CRITICAL, "%s: unable to locate %s\n", __func__, part_name);
        return -ERR_NOT_FOUND;
    }
    dev = bio_open_by_label(label);
    if (dev) {
        ret = dev->total_size;
        LTRACEF("%s: part total_size size = 0x%llx\n", __func__, ret);
        bio_close(dev);
    } else {
        dprintf(CRITICAL, "%s: unable to get part size of %s\n", __func__, part_name);
        return -ERR_NOT_FOUND;
    }

    return ret;
}

static int apusys_rv_load_fw(struct apusys_secure_info_t *sec_info, const char *part_name,
    off_t sec_mem_size, void *sec_mem_addr_va)
{
    status_t ret = NO_ERROR;
    unsigned int apusys_pmsize, apusys_xsize;
    char *apusys_pmimg, *apusys_ximg;
    vaddr_t tmp_addr;
    char *img;
    struct ptimg_hdr_t *hdr;

    /* initialize apusys sec_info */
    tmp_addr = sec_info->up_code_buf_ofs + sec_info->up_code_buf_sz +
        (vaddr_t)sec_mem_addr_va + ROUNDUP(sizeof(*sec_info), APUSYS_FW_ALIGN);
    /* load image from flash */
    ret = load_partition(part_name, IMG_NAME_APUSYS_A, (void *)tmp_addr, sec_mem_size);
    if (ret < 0) {
        dprintf(CRITICAL, "apusys load from partition failed, ret: %d\n", ret);
        return ret;
    }

    /* separate ptimg */
    apusys_pmimg = apusys_ximg = NULL;
    apusys_pmsize = apusys_xsize = 0;
    hdr = (struct ptimg_hdr_t *) tmp_addr;

    while (hdr->magic == PT_MAGIC) {
        img = ((char *) hdr) + hdr->hdr_size;
        LTRACEF("Rhdr->hdr_size= 0x%x\n", hdr->hdr_size);

        switch (hdr->id) {
        case PT_ID_APUSYS_FW:
            LTRACEF("PT_ID_APUSYS_FW\n");
            apusys_pmimg = img;
            apusys_pmsize = hdr->img_size;
            sec_info->up_fw_ofs = (unsigned int) img - (unsigned int) sec_mem_addr_va;
            sec_info->up_fw_sz = apusys_pmsize;
            LTRACEF("up_fw_ofs = 0x%x, up_fw_sz = 0x%x\n",
                sec_info->up_fw_ofs, sec_info->up_fw_sz);
            break;
        case PT_ID_APUSYS_XFILE:
            LTRACEF("PT_ID_APUSYS_XFILE\n");
            apusys_ximg = img;
            apusys_xsize = hdr->img_size;
            sec_info->up_xfile_ofs = (unsigned int) hdr - (unsigned int) sec_mem_addr_va;
            sec_info->up_xfile_sz = hdr->hdr_size + apusys_xsize;
            LTRACEF("up_xfile_ofs = 0x%x, up_xfile_sz = 0x%x\n",
                sec_info->up_xfile_ofs, sec_info->up_xfile_sz);
            break;
        case PT_ID_MDLA_FW_BOOT:
            LTRACEF("PT_ID_MDLA_FW_BOOT\n");
            sec_info->mdla_fw_boot_ofs = (unsigned int) img - (unsigned int) sec_mem_addr_va;
            sec_info->mdla_fw_boot_sz = hdr->img_size;
            LTRACEF("mdla_fw_boot_ofs = 0x%x, mdla_fw_boot_sz = 0x%x\n",
                sec_info->mdla_fw_boot_ofs, sec_info->mdla_fw_boot_sz);
            break;
        case PT_ID_MDLA_FW_MAIN:
            LTRACEF("PT_ID_MDLA_FW_MAIN\n");
            sec_info->mdla_fw_main_ofs = (unsigned int) img - (unsigned int) sec_mem_addr_va;
            sec_info->mdla_fw_main_sz = hdr->img_size;
            LTRACEF("mdla_fw_main_ofs = 0x%x, mdla_fw_main_sz = 0x%x\n",
                sec_info->mdla_fw_main_ofs, sec_info->mdla_fw_main_sz);
            break;
        case PT_ID_MDLA_XFILE:
            LTRACEF("PT_ID_MDLA_XFILE\n");
            sec_info->mdla_xfile_ofs = (unsigned int) hdr - (unsigned int) sec_mem_addr_va;
            sec_info->mdla_xfile_sz = hdr->hdr_size + hdr->img_size;
            LTRACEF("mdla_xfile_ofs = 0x%x, mdla_xfile_sz = 0x%x\n",
                sec_info->mdla_xfile_ofs, sec_info->mdla_xfile_sz);
            break;
        case PT_ID_MVPU_FW:
            LTRACEF("PT_ID_MVPU_FW\n");
            sec_info->mvpu_fw_ofs = (unsigned int) img - (unsigned int) sec_mem_addr_va;
            sec_info->mvpu_fw_sz = hdr->img_size;
            LTRACEF("mvpu_fw_ofs = 0x%x, mvpu_fw_sz = 0x%x\n",
                sec_info->mvpu_fw_ofs, sec_info->mvpu_fw_sz);
            break;
        case PT_ID_MVPU_XFILE:
            LTRACEF("PT_ID_MVPU_XFILE\n");
            sec_info->mvpu_xfile_ofs = (unsigned int) hdr - (unsigned int) sec_mem_addr_va;
            sec_info->mvpu_xfile_sz = hdr->hdr_size + hdr->img_size;
            LTRACEF("mvpu_xfile_ofs = 0x%x, mvpu_xfile_sz = 0x%x\n",
                sec_info->mvpu_xfile_ofs, sec_info->mvpu_xfile_sz);
            break;
        case PT_ID_MVPU_SEC_FW:
            LTRACEF("PT_ID_MVPU_SEC_FW\n");
            sec_info->mvpu_sec_fw_ofs = (unsigned int) img - (unsigned int) sec_mem_addr_va;
            sec_info->mvpu_sec_fw_sz = hdr->img_size;
            LTRACEF("mvpu_sec_fw_ofs = 0x%x, mvpu_sec_fw_sz = 0x%x\n",
                sec_info->mvpu_sec_fw_ofs, sec_info->mvpu_sec_fw_sz);
            break;
        case PT_ID_MVPU_SEC_XFILE:
            LTRACEF("PT_ID_MVPU_SEC_XFILE\n");
            sec_info->mvpu_sec_xfile_ofs = (unsigned int) hdr - (unsigned int) sec_mem_addr_va;
            sec_info->mvpu_sec_xfile_sz = hdr->hdr_size + hdr->img_size;
            LTRACEF("mvpu_sec_xfile_ofs = 0x%x, mvpu_sec_xfile_sz = 0x%x\n",
                sec_info->mvpu_sec_xfile_ofs, sec_info->mvpu_sec_xfile_sz);
            break;
        default:
            dprintf(CRITICAL, "Warning: Ignore unknown APUSYS image_%d\n", hdr->id);
            break;
        }

        LTRACEF("hdr->img_size = 0x%x, ROUNDUP(hdr->img_size, hdr->align) = 0x%x\n",
            hdr->img_size, ROUNDUP(hdr->img_size, hdr->align));
        img += ROUNDUP(hdr->img_size, hdr->align);
        hdr = (struct ptimg_hdr_t *) img;
    }

    if (!apusys_pmimg || !apusys_ximg) {
        dprintf(CRITICAL, "APUSYS partition missing - PM:0x%x, XM:0x%x (@0x%x)\n",
            (u32)apusys_pmimg, (u32)apusys_ximg, (u32)tmp_addr);
        return -ERR_NOT_FOUND;
    }

    dprintf(CRITICAL,
        "%s APUSYS part load finished: PM:0x%x(up_fw_ofs = 0x%x), XM:0x%x(up_xfile_ofs = 0x%x)\n",
        __func__, (u32)apusys_pmimg, sec_info->up_fw_ofs, (u32)apusys_ximg, sec_info->up_xfile_ofs);

    return NO_ERROR;
}

static int apusys_rv_initialize_aee_coredump_buf(
    struct apusys_aee_coredump_info_t *aee_coredump_info,
    struct apusys_secure_info_t *sec_info, void *aee_coredump_mem_addr_va,
    void *sec_mem_addr_va, unsigned int regdump_buf_sz)
{
    aee_coredump_info = (struct apusys_aee_coredump_info_t *)aee_coredump_mem_addr_va;

    aee_coredump_info->up_coredump_ofs = sizeof(*aee_coredump_info);
    aee_coredump_info->up_coredump_sz = sec_info->up_coredump_sz;
    aee_coredump_info->regdump_ofs = aee_coredump_info->up_coredump_ofs +
        aee_coredump_info->up_coredump_sz;
    aee_coredump_info->regdump_sz = regdump_buf_sz;
    aee_coredump_info->mdla_coredump_ofs = aee_coredump_info->regdump_ofs +
        aee_coredump_info->regdump_sz;
    aee_coredump_info->mdla_coredump_sz = sec_info->mdla_coredump_sz;
    aee_coredump_info->mvpu_coredump_ofs = aee_coredump_info->mdla_coredump_ofs +
        aee_coredump_info->mdla_coredump_sz;
    aee_coredump_info->mvpu_coredump_sz = sec_info->mvpu_coredump_sz;
    aee_coredump_info->mvpu_sec_coredump_ofs = aee_coredump_info->mvpu_coredump_ofs +
        aee_coredump_info->mvpu_coredump_sz;
    aee_coredump_info->mvpu_sec_coredump_sz = sec_info->mvpu_sec_coredump_sz;

    aee_coredump_info->up_xfile_ofs = aee_coredump_info->mvpu_sec_coredump_ofs +
        aee_coredump_info->mvpu_sec_coredump_sz;
    aee_coredump_info->up_xfile_sz = sec_info->up_xfile_sz;
    aee_coredump_info->mdla_xfile_ofs = aee_coredump_info->up_xfile_ofs +
        aee_coredump_info->up_xfile_sz;
    aee_coredump_info->mdla_xfile_sz = sec_info->mdla_xfile_sz;
    aee_coredump_info->mvpu_xfile_ofs = aee_coredump_info->mdla_xfile_ofs +
        aee_coredump_info->mdla_xfile_sz;
    aee_coredump_info->mvpu_xfile_sz = sec_info->mvpu_xfile_sz;
    aee_coredump_info->mvpu_sec_xfile_ofs = aee_coredump_info->mvpu_xfile_ofs +
        aee_coredump_info->mvpu_xfile_sz;
    aee_coredump_info->mvpu_sec_xfile_sz = sec_info->mvpu_sec_xfile_sz;

    /* copy uP xfile to aee_coredump buffer */
    memcpy((void *) (aee_coredump_mem_addr_va + aee_coredump_info->up_xfile_ofs),
        (void *) (sec_mem_addr_va + sec_info->up_xfile_ofs), sec_info->up_xfile_sz);

    /* copy mdla xfile to aee_coredump buffer */
    memcpy((void *) (aee_coredump_mem_addr_va + aee_coredump_info->mdla_xfile_ofs),
        (void *) (sec_mem_addr_va + sec_info->mdla_xfile_ofs), sec_info->mdla_xfile_sz);

    /* copy mvpu xfile to aee_coredump buffer */
    memcpy((void *) (aee_coredump_mem_addr_va + aee_coredump_info->mvpu_xfile_ofs),
        (void *) (sec_mem_addr_va + sec_info->mvpu_xfile_ofs), sec_info->mvpu_xfile_sz);

    /* copy mvpu_sec xfile to aee_coredump buffer */
    memcpy((void *) (aee_coredump_mem_addr_va + aee_coredump_info->mvpu_sec_xfile_ofs),
        (void *) (sec_mem_addr_va + sec_info->mvpu_sec_xfile_ofs), sec_info->mvpu_sec_xfile_sz);

    return NO_ERROR;
}

static int apusys_rv_sec_mem_emi_protect_en(uint64_t pa, off_t size)
{
#if ENABLE_APUSYS_EMI_PROTECTION
    /*
     * setup EMI MPU
     * domain 0: APMCU
     * domain 5: APUSYS
     */
    struct emi_region_info_t region_info;

    region_info.start = (unsigned long long) pa;
    region_info.end = (unsigned long long) (pa + size);
    region_info.region = APUSYS_SEC_FW_EMI_REGION;

    SET_ACCESS_PERMISSION(region_info.apc, UNLOCK,
                          FORBIDDEN, FORBIDDEN, FORBIDDEN, FORBIDDEN,
                          FORBIDDEN, FORBIDDEN, FORBIDDEN, FORBIDDEN,
                          FORBIDDEN, FORBIDDEN, SEC_RW, FORBIDDEN,
                          FORBIDDEN, FORBIDDEN, FORBIDDEN, SEC_RW);

    return emi_mpu_set_protection(&region_info);
#else
    return 0;
#endif
}

status_t platform_load_apusys_rv(void *fdt)
{
    status_t ret = NO_ERROR;
    u64 sec_mem_addr_pa, aee_coredump_mem_addr_pa;
    void *sec_mem_addr_va, *aee_coredump_mem_addr_va;
    const char part_name[10] = "apusys";
    struct apusys_secure_info_t *sec_info = NULL;
    struct apusys_aee_coredump_info_t *aee_coredump_info = NULL;
    unsigned int coredump_buf_sz = 0, xfile_buf_sz = 0;
    unsigned int regdump_buf_sz = 0;
    off_t sec_mem_size = 0, aee_coredump_mem_size = 0;
    off_t apusys_part_size = 0;
    struct __smccc_res smccc_res;

    ret = apusys_rv_prop_off(fdt);
    if (ret == -ERR_NOT_FOUND) {
        dprintf(CRITICAL, "%s: failed to find apusys_rv in dtb\n", __func__);
        ASSERT(0);
    } else if (ret == -ERR_NOT_READY) {
        dprintf(CRITICAL, "%s: apusys_rv is not ready yet\n", __func__);
        return NO_ERROR;
    }

    if (ENABLE_APUSYS_SECURE_BOOT) {
        __smc_conduit(MTK_SIP_BL_APUSYS_CONTROL, MTK_APUSYS_SMC_OP_APUSYS_RV_INIT,
                0, 0, 0, 0, 0, 0, &smccc_res);
        ret = (int) smccc_res.a0;
        if (ret < 0) {
            dprintf(CRITICAL, "%s: smc call MTK_APUSYS_SMC_OP_APUSYS_RV_INIT fail(%d)\n",
                __func__, ret);
            return ret;
        }
    }

    if (ENABLE_APUSYS_MBOX_MPU) {
        __smc_conduit(MTK_SIP_BL_APUSYS_CONTROL, MTK_APUSYS_SMC_OP_APUSYS_RV_SETUP_MBOX_MPU,
                0, 0, 0, 0, 0, 0, &smccc_res);
        ret = (int) smccc_res.a0;
        if (ret < 0) {
            dprintf(CRITICAL, "%s: smc call MTK_APUSYS_SMC_OP_APUSYS_RV_SETUP_MBOX_MPU fail(%d)\n",
                __func__, ret);
            return ret;
        }
    }

    sec_info = (struct apusys_secure_info_t *) malloc(sizeof(struct apusys_secure_info_t));
    if (!sec_info)
        return -ENOMEM;
    memset((void *) sec_info, 0x0, sizeof(struct apusys_secure_info_t));

    apusys_part_size = get_apusys_rv_part_size(part_name);
    if (apusys_part_size < 0)
        goto err_apusys_rv_get_buf_sz;

    /* fill in dts property sec_info */
    sec_info->up_code_buf_ofs = 0;
    ret = apusys_rv_get_buf_sz(fdt, "up_code_buf_sz");
    if (ret < 0)
        goto err_apusys_rv_get_buf_sz;
    sec_info->up_code_buf_sz = ret;

    sec_info->up_coredump_ofs = sec_info->up_code_buf_ofs +
        sec_info->up_code_buf_sz + apusys_part_size;
    ret = apusys_rv_get_buf_sz(fdt, "up_coredump_buf_sz");
    if (ret < 0)
        goto err_apusys_rv_get_buf_sz;
    sec_info->up_coredump_sz = ret;

    sec_info->mdla_coredump_ofs = sec_info->up_coredump_ofs + sec_info->up_coredump_sz;
    ret = apusys_rv_get_buf_sz(fdt, "mdla_coredump_buf_sz");
    if (ret < 0)
        goto err_apusys_rv_get_buf_sz;
    sec_info->mdla_coredump_sz = ret;

    sec_info->mvpu_coredump_ofs = sec_info->mdla_coredump_ofs + sec_info->mdla_coredump_sz;
    ret = apusys_rv_get_buf_sz(fdt, "mvpu_coredump_buf_sz");
    if (ret < 0)
        goto err_apusys_rv_get_buf_sz;
    sec_info->mvpu_coredump_sz = ret;

    sec_info->mvpu_sec_coredump_ofs = sec_info->mvpu_coredump_ofs + sec_info->mvpu_coredump_sz;
    ret = apusys_rv_get_buf_sz(fdt, "mvpu_sec_coredump_buf_sz");
    if (ret < 0)
        goto err_apusys_rv_get_buf_sz;
    sec_info->mvpu_sec_coredump_sz = ret;

    coredump_buf_sz = sec_info->up_coredump_sz + sec_info->mdla_coredump_sz +
        sec_info->mvpu_coredump_sz + sec_info->mvpu_sec_coredump_sz;

    sec_mem_size = ROUNDUP(apusys_part_size + sec_info->up_code_buf_sz + coredump_buf_sz,
        APUSYS_SEC_MEM_ALIGN);
    sec_info->total_sz = sec_mem_size;

    /* allocate apusys-rv_secure */
    if (need_relocate())
        sec_mem_addr_pa = mblock_alloc_range(sec_mem_size, APUSYS_SEC_MEM_ALIGN,
            APUSYS_MEM_LOWER_BOUND, APUSYS_MEM_UPPER_BOUND, 0, 0, "apu_apusys-rv_secure");
    else
        sec_mem_addr_pa = mblock_alloc(sec_mem_size, APUSYS_SEC_MEM_ALIGN, APUSYS_MEM_LIMIT,
            0, 0, "apu_apusys-rv_secure");

    if (!sec_mem_addr_pa) {
        dprintf(CRITICAL, "apu_apusys-rv_secure mblock allocation failed\n");
        ret = -ERR_NO_MEMORY;
        goto err_apusys_rv_get_buf_sz;
    }

    /* set vmm */
    ret = vmm_alloc_physical(vmm_get_kernel_aspace(),
            "vm-apu_apusys-rv_secure",
            sec_mem_size,
            &sec_mem_addr_va,
            PAGE_SIZE_SHIFT,
            (paddr_t)sec_mem_addr_pa,
            0,
            ARCH_MMU_FLAG_CACHED);

    if (ret != NO_ERROR) {
        dprintf(CRITICAL, "APUSYS vmm_alloc_physical vm-apu_apusys-rv_secure failed, err: %d\n",
            ret);
        goto err_vmm_apu_apusys_rv_secure_alloc;
    }

    memset(sec_mem_addr_va, 0, sec_mem_size);

    /* load apusys FWs to rv_secure mem */
    ret = apusys_rv_load_fw(sec_info, part_name, sec_mem_size, sec_mem_addr_va);
    if (ret != NO_ERROR) {
        dprintf(CRITICAL, "%s: apusys_rv_load_fw fail(%d)\n", __func__, ret);
        goto err_load_partition;
    }

    /* copy uP firmware to code buffer */
    memcpy((void *) (sec_mem_addr_va + sec_info->up_code_buf_ofs),
        (void *) (sec_mem_addr_va + sec_info->up_fw_ofs), sec_info->up_fw_sz);

    /* initialize apusys sec_info */
    memcpy((void *)(sec_info->up_code_buf_ofs + sec_info->up_code_buf_sz + sec_mem_addr_va),
        (void *)sec_info, sizeof(*sec_info));

    /* flush to DRAM */
    arch_clean_invalidate_cache_range((addr_t)sec_mem_addr_va, sec_mem_size);

    if (ENABLE_APUSYS_SECURE_BOOT) {
        __smc_conduit(MTK_SIP_BL_APUSYS_CONTROL, MTK_APUSYS_SMC_OP_APUSYS_RV_SETUP_SECURE_MEM,
                sec_mem_addr_pa, sec_mem_size, 0, 0, 0, 0, &smccc_res);
        ret = (int) smccc_res.a0;
        if (ret < 0) {
            dprintf(CRITICAL,
                "%s: smc call MTK_APUSYS_SMC_OP_APUSYS_RV_SETUP_SECURE_MEM fail(%d)\n",
                __func__, ret);
            goto err_smc_setup_secure_mem;
        }
    }

    xfile_buf_sz = sec_info->up_xfile_sz + sec_info->mdla_xfile_sz +
        sec_info->mvpu_xfile_sz + sec_info->mvpu_sec_xfile_sz;
    LTRACEF("%s: xfile_buf_sz = 0x%x, sizeof(*aee_coredump_info) = 0x%x, coredump_buf_sz = 0x%x\n",
        __func__, xfile_buf_sz, (u32)sizeof(*aee_coredump_info), coredump_buf_sz);

    ret = apusys_rv_get_buf_sz(fdt, "regdump_buf_sz");
    if (ret < 0) {
        dprintf(CRITICAL, "%s: get regdump_buf_sz fail\n", __func__);
        goto err_apusys_rv_get_regdump_buf_sz;
    }
    regdump_buf_sz = ret;

    aee_coredump_mem_size = sizeof(*aee_coredump_info) + coredump_buf_sz +
        xfile_buf_sz + regdump_buf_sz;
    aee_coredump_mem_size = ROUNDUP(aee_coredump_mem_size, APUSYS_AEE_COREDUMP_MEM_ALIGN);

    /* allocate apusys-rv_aee-coredump */
    if (need_relocate())
        aee_coredump_mem_addr_pa = mblock_alloc_range(aee_coredump_mem_size,
            APUSYS_AEE_COREDUMP_MEM_ALIGN, APUSYS_MEM_LOWER_BOUND, APUSYS_MEM_UPPER_BOUND,
            0, 0, "apu_apusys-rv_aee-coredump");
    else
        aee_coredump_mem_addr_pa = mblock_alloc(aee_coredump_mem_size,
            APUSYS_AEE_COREDUMP_MEM_ALIGN, APUSYS_MEM_LIMIT, 0, 0,
            "apu_apusys-rv_aee-coredump");

    if (!aee_coredump_mem_addr_pa) {
        dprintf(CRITICAL, "apu_apusys-rv_aee-coredump mblock allocation failed\n");
        ret = -ERR_NO_MEMORY;
        goto err_mblock_apu_apusys_rv_aee_coredump_alloc;
    }

    /* set vmm */
    ret = vmm_alloc_physical(vmm_get_kernel_aspace(),
            "vm-apu_apusys-rv_aee-coredump",
            aee_coredump_mem_size,
            &aee_coredump_mem_addr_va,
            PAGE_SIZE_SHIFT,
            (paddr_t)aee_coredump_mem_addr_pa,
            0,
            ARCH_MMU_FLAG_CACHED);

    if (ret != NO_ERROR) {
        dprintf(CRITICAL,
            "APUSYS vmm_alloc_physical vm-apu_apusys-rv_aee-coredump failed, err: %d\n", ret);
        goto err_vmm_apu_apusys_rv_aee_coredump_alloc;
    }

    memset(aee_coredump_mem_addr_va, 0, aee_coredump_mem_size);

    ret = apusys_rv_initialize_aee_coredump_buf(aee_coredump_info, sec_info,
        aee_coredump_mem_addr_va, sec_mem_addr_va, regdump_buf_sz);
    if (ret != NO_ERROR) {
        dprintf(CRITICAL, "%s: apusys_rv_initialize_aee_coredump_buf fail(%d)\n", __func__, ret);
        goto err_apusys_rv_initialize_aee_coredump_buf;
    }

    /* flush to DRAM */
    arch_clean_invalidate_cache_range((addr_t)aee_coredump_mem_addr_va, aee_coredump_mem_size);

    if (ENABLE_APUSYS_SECURE_BOOT) {
        __smc_conduit(MTK_SIP_BL_APUSYS_CONTROL,
            MTK_APUSYS_SMC_OP_APUSYS_RV_SETUP_AEE_COREDUMP_MEM,
                aee_coredump_mem_addr_pa, aee_coredump_mem_size, 0, 0, 0, 0, &smccc_res);
        ret = (int) smccc_res.a0;
        if (ret < 0) {
            dprintf(CRITICAL,
                "%s: smc call MTK_APUSYS_SMC_OP_APUSYS_RV_SETUP_AEE_COREDUMP_MEM fail(%d)\n",
                __func__, ret);
            goto err_smc_setup_aee_coredump_mem;
        }
    }

    ret = apusys_rv_sec_mem_emi_protect_en(sec_mem_addr_pa, sec_mem_size);
    if (ret) {
        dprintf(CRITICAL, "%s: apusys_rv_sec_mem_emi_protect_en fail(%d)\n",
            __func__, ret);
        goto err_apusys_rv_sec_mem_emi_protect_en;
    }

    if (sec_info)
        free(sec_info);

    return ret;

err_apusys_rv_sec_mem_emi_protect_en:
err_smc_setup_aee_coredump_mem:
err_apusys_rv_initialize_aee_coredump_buf:
    vmm_free_region(vmm_get_kernel_aspace(), (vaddr_t)aee_coredump_mem_addr_va);

err_vmm_apu_apusys_rv_aee_coredump_alloc:
    mblock_free(aee_coredump_mem_addr_pa);

err_mblock_apu_apusys_rv_aee_coredump_alloc:
err_apusys_rv_get_regdump_buf_sz:
err_smc_setup_secure_mem:
err_load_partition:
    vmm_free_region(vmm_get_kernel_aspace(), (vaddr_t)sec_mem_addr_va);

err_vmm_apu_apusys_rv_secure_alloc:
    mblock_free(sec_mem_addr_pa);

err_apusys_rv_get_buf_sz:
    if (sec_info)
        free(sec_info);

    return ret;
}

void apusys_devapc_init(uint level)
{
    struct __smccc_res smccc_res;

    __smc_conduit(MTK_SIP_BL_APUSYS_CONTROL, MTK_APUSYS_SMC_OP_DEVAPC_INIT,
            0, 0, 0, 0, 0, 0, &smccc_res);
}

MT_LK_INIT_HOOK_BL33(apusys_devapc_init, apusys_devapc_init, LK_INIT_LEVEL_APPS - 1);

void apusys_security_ctrl_init(uint level)
{
    struct __smccc_res smccc_res;

    __smc_conduit(MTK_SIP_BL_APUSYS_CONTROL, MTK_APUSYS_SMC_OP_SECURITY_CTRL_INIT,
            0, 0, 0, 0, 0, 0, &smccc_res);
}

MT_LK_INIT_HOOK_BL33(apusys_security_ctrl_init, apusys_security_ctrl_init, LK_INIT_LEVEL_APPS - 1);
