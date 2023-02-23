/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <arch/mmu.h>
#include <assert.h>
#include <err.h>
#include <kernel/vm.h>
#include <lib/pl_boottags.h>
#include <load_image.h>
#include <mblock.h>
#include <platform.h>
#include <platform/memory_layout.h>
#include <platform/sec_devinfo.h>
#include <platform_mtk.h>
#include <string.h>
#include "tee_loader.h"

#define MOD "[BL32]"

/* Signature size */
#define TEE_HEADER_SIG_SIZE (0x240)
#define TEE_ARG_SIZE        (0x100)

#define TEE_MAP_FAILED      ((void *) ERR_GENERIC)

static uint8_t meid[ME_IDENTITY_LEN];

static void tee_meid_hook(struct boot_tag *tag)
{
    uint8_t *p;

    if (tag == NULL) {
        dprintf(CRITICAL, "%s Invalid parameter\n", MOD);
        return;
    }

    p = (uint8_t *)&tag->data;

    for (int i = 0; i < ME_IDENTITY_LEN; i++)
        meid[i] = p[i];
}

static void *tee_mmap(char *name, void *addr, size_t length)
{
    status_t ret = NO_ERROR;
    void *addr_va = (void *)(KERNEL_ASPACE_BASE + addr);

    if (addr == 0) {
        dprintf(CRITICAL, "%s Wrong address\n", MOD);
        return ERR_INVALID_ARGS;
    }

    if (length == 0) {
        dprintf(CRITICAL, "%s Wrong size\n", MOD);
        return ERR_INVALID_ARGS;
    }

    ret = vmm_alloc_physical(vmm_get_kernel_aspace(),
                        name,
                        length,
                        &addr_va,
                        PAGE_SIZE_SHIFT,
                        (paddr_t)addr,
                        VMM_FLAG_VALLOC_SPECIFIC,
                        ARCH_MMU_FLAG_CACHED);

    if (ret != NO_ERROR) {
        dprintf(CRITICAL, "%s vmm_alloc_physical failed, err: %d\n", MOD, ret);
        return TEE_MAP_FAILED;
    }

    return addr_va;
}

static int tee_munmap(void *addr)
{
    status_t ret = NO_ERROR;

    if (addr == 0) {
        dprintf(CRITICAL, "%s Wrong address\n", MOD);
        return ERR_INVALID_ARGS;
    }

    ret = vmm_free_region(vmm_get_kernel_aspace(), (vaddr_t)addr);

    if (ret != NO_ERROR) {
        dprintf(CRITICAL, "%s vmm_free_region failed, err: %d\n", MOD, ret);
        return ret;
    }

    return ret;
}

static status_t tee_set_atf_arg_addr(u64 bl_param_phy, u64 atf_arg_phy)
{
    status_t ret = NO_ERROR;
    struct mtk_bl_param_t *p_mtk_bl_param;

    p_mtk_bl_param = tee_mmap(VM_BL_NAME, (void *)bl_param_phy, PAGE_SIZE);
    if (p_mtk_bl_param == TEE_MAP_FAILED || p_mtk_bl_param == ERR_INVALID_ARGS) {
        dprintf(CRITICAL, "%s Map BL parameter failed\n", MOD);
        return ERR_GENERIC;
    }

    p_mtk_bl_param->atf_arg_addr = atf_arg_phy;

    ret = tee_munmap((void *)p_mtk_bl_param);
    if (ret != NO_ERROR) {
        dprintf(CRITICAL, "%s Umap BL parameter failed\n", MOD);
        return ERR_GENERIC;
    }

    return ret;
}

static status_t tee_set_atf_arg(paddr_t bl32_addr)
{
    status_t ret = NO_ERROR;
    struct reserved_t *atf_para_pa;
    void *addr_arg;
    struct atf_arg *atf_arg;
    u32 i;

    if (bl32_addr == 0)
        return ERR_INVALID_ARGS;

    /* Get TEE Parameters from SecMem of mblock */
    atf_para_pa = mblock_query_reserved_by_name(SECMEM_RESERVED_NAME, 0);
    if (atf_para_pa == NULL)
        return ERR_GENERIC;

    /* Pass atf_arg_addr to TF-A */
    ret = tee_set_atf_arg_addr(lk_boot_args[0], atf_para_pa->start);
    if (ret != NO_ERROR) {
        dprintf(CRITICAL, "%s Pass ATF argument failed\n", MOD);
        return ERR_GENERIC;
    }

    /* Map ATF argument */
    addr_arg = tee_mmap(VM_TEE_NAME, (void *)atf_para_pa->start, atf_para_pa->size);
    if (addr_arg == TEE_MAP_FAILED || addr_arg == ERR_INVALID_ARGS) {
        dprintf(CRITICAL, "%s Map ATF argument failed\n", MOD);
        return ERR_GENERIC;
    }

    /* Set related ATF argument */
    atf_arg = (struct atf_arg *)addr_arg;

    memset((void *)atf_arg, 0, TEE_ARG_SIZE);
    atf_arg->tee_support = 0x1;
    atf_arg->tee_entry = bl32_addr;
    atf_arg->tee_boot_arg_addr = (u64)(atf_para_pa->start + TEE_ARG_SIZE);
    memcpy((void *)atf_arg->hwuid, (void *)meid, sizeof(atf_arg->hwuid));
    memset((void *)meid, 0, sizeof(meid));
    for (i = 0; i < HRID_SIZE; i++)
        atf_arg->HRID[i] = get_devinfo_with_index(HRID_INDEX + i);
    for (i = HRID_SIZE; i < (sizeof(atf_arg->HRID) / sizeof(u32)); i++)
        atf_arg->HRID[i] = 0;

    /* Free ATF argument */
    ret = tee_munmap((void *)addr_arg);
    if (ret != NO_ERROR) {
        dprintf(CRITICAL, "%s Umap ATF argument failed\n", MOD);
        return ERR_GENERIC;
    }

    return ret;
}

static status_t tee_set_tee_arg(paddr_t bl32_addr, size_t bl32_size)
{
    status_t ret = NO_ERROR;
    struct reserved_t *tee_para;
    void *addr_arg;

    if (bl32_addr == 0)
        return ERR_INVALID_ARGS;

    if (bl32_size == 0)
        return ERR_INVALID_ARGS;

    if (bl32_size > TEE_MEM_MAX_SIZE)
        return ERR_INVALID_ARGS;

    /* Get TEE Parameters from SecMem of mblock */
    tee_para = mblock_query_reserved_by_name(SECMEM_RESERVED_NAME, 0);
    if (tee_para == NULL)
        return ERR_GENERIC;

    /* Map TEE argument */
    addr_arg = tee_mmap(VM_TEE_NAME, (void *)tee_para->start, tee_para->size);
    if (addr_arg == TEE_MAP_FAILED || addr_arg == ERR_INVALID_ARGS) {
        dprintf(CRITICAL, "%s Map TEE argument failed\n", MOD);
        return ERR_GENERIC;
    }

    /* Set related TEE argument for TEEs */
    addr_arg += TEE_ARG_SIZE;
    memset((void *)addr_arg, 0, TEE_ARG_SIZE);
    if (tee_vendor_module.set_boot_param)
        tee_vendor_module.set_boot_param(addr_arg, bl32_addr, bl32_size,
            mblock_get_memory_start(), mblock_get_memory_size());

    /* Free TEE argument */
    ret = tee_munmap((void *)addr_arg);
    if (ret != NO_ERROR) {
        dprintf(CRITICAL, "%s Umap TEE argument failed\n", MOD);
        return ERR_GENERIC;
    }

    return ret;
}

status_t platform_load_tee(void)
{
    paddr_t bl32_addr_pa, bl32_addr_start, bl32_addr_pa_mirror;
    void *bl32_addr_va;
    size_t bl32_res_size = TEE_DRAM_SIZE;
    size_t bl32_size;
    vaddr_t load_addr = 0;
    char part_name[16] = "tee";
    status_t ret = NO_ERROR;

    /* Default Size: BL32_SIZE = 50MB */
    if (bl32_res_size == 0) {
        dprintf(CRITICAL, "%s BL32 Default Size 0x%zx\n",
            MOD, TEE_MEM_DEF_SIZE);
        bl32_res_size = TEE_MEM_DEF_SIZE;
    }

    /* Max Size: TEE_MEM_MAX_SIZE = 2GB */
    if (bl32_res_size > TEE_MEM_MAX_SIZE) {
        dprintf(CRITICAL, "%s BL32 Size is Over 0x%zx, 0x%zx\n",
            MOD, TEE_MEM_MAX_SIZE, bl32_res_size);
        bl32_res_size = TEE_MEM_MAX_SIZE;
    }

    bl32_addr_pa = mblock_alloc(bl32_res_size, PAGE_SIZE,
        MBLOCK_NO_LIMIT, (paddr_t)BL32_BASE_PHY, MBLOCK_NO_MAP, TEE_RESERVED_NAME);

    if (need_relocate())
        bl32_addr_pa_mirror = mblock_alloc(bl32_res_size, PAGE_SIZE,
            MBLOCK_NO_LIMIT, (paddr_t)BL32_BASE_PHY + TEE_MEM_MIRROR_OFFSET,
            MBLOCK_NO_MAP, TEE_RESERVED_NAME);

    if (bl32_addr_pa == 0) {
        dprintf(CRITICAL, "%s mblock allocation failed\n", MOD);
        return ERR_NO_MEMORY;
    }

    bl32_addr_start = bl32_addr_pa;
    bl32_addr_pa = (bl32_addr_start - PAGE_SIZE);
    bl32_addr_va = (void *)(bl32_addr_start + KERNEL_ASPACE_BASE - PAGE_SIZE);
    bl32_size = (bl32_res_size + PAGE_SIZE);
    /* mmap the allocated memory */
    ret = vmm_alloc_physical(vmm_get_kernel_aspace(),
                        "vm-BL32-reserved",
                        bl32_size,
                        &bl32_addr_va,
                        PAGE_SIZE_SHIFT,
                        (paddr_t)bl32_addr_pa,
                        VMM_FLAG_VALLOC_SPECIFIC,
                        ARCH_MMU_FLAG_CACHED);

    if (ret != NO_ERROR) {
        dprintf(CRITICAL, "%s vmm_alloc_physical failed, err: %d\n", MOD, ret);
        goto err_vmm_alloc;
    }

    /* load image to align BL32_BASE_PHY excepting signature */
    load_addr = (vaddr_t)(bl32_addr_va + PAGE_SIZE - TEE_HEADER_SIG_SIZE);
    ret = load_partition(part_name, "tee", (void *)load_addr, bl32_res_size);
    if (ret < 0) {
        dprintf(CRITICAL, "%s load failed, ret: %d\n", MOD, ret);
        goto err_load_partition;
    }

    /* Set related ATF arguments */
    dprintf(INFO, "%s Set related ATF arguments\n", MOD);
    ret = tee_set_atf_arg(bl32_addr_start);
    if (ret != NO_ERROR) {
        dprintf(CRITICAL, "%s Set ATF argument failed, ret: %d\n", MOD, ret);
        goto err_load_partition;
    }

    /* Set related TEE arguments */
    dprintf(INFO, "%s Set related TEE arguments\n", MOD);
    ret = tee_set_tee_arg(bl32_addr_start, bl32_res_size);
    if (ret != NO_ERROR) {
        dprintf(CRITICAL, "%s Set TEE argument failed, ret: %d\n", MOD, ret);
        goto err_load_partition;
    }

    vmm_free_region(vmm_get_kernel_aspace(), (vaddr_t)bl32_addr_va);

    return NO_ERROR;

err_load_partition:
    vmm_free_region(vmm_get_kernel_aspace(), (vaddr_t)bl32_addr_va);

err_vmm_alloc:
    mblock_free(bl32_addr_start);
    if (need_relocate())
        mblock_free(bl32_addr_pa_mirror);

    return ret;
}

PL_BOOTTAGS_INIT_HOOK(tee_meid_hook, BOOT_TAG_MEID_INFO, tee_meid_hook);
PL_BOOTTAGS_TO_BE_UPDATED(tee_meid_hook, BOOT_TAG_MEID_INFO, &meid);
