/*
 * Copyright (c) 2021 MediaTek Inc.
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
#include <arch/ops.h>
#include <assert.h>
#include <err.h>
#include <kernel/vm.h>
#include <libfdt.h>
#include <load_image.h>
#include <mblock.h>
#include <platform/lpm.h>
#include <smc.h>
#include <smc_id_table.h>
#include <string.h>
#include <trace.h>

#include "spm_common.h"

#define LOCAL_TRACE 0

#define SPM_MEM_SIZE   0x00010000  // 64K
#define SPM_MEM_ALIGN  0x00010000  // 64K (minimal size for EMI MPU)
#define SPM_MEM_LIMIT  0x78000000
#define SPM_ARGS_SPMFW_INIT 1


static uint64_t spmfw_base_addr_pa;
static void *spmfw_base_addr_va;

static bool is_spmfw_magic(int magic)
{
    bool ret = 0;

    switch (magic) {
    case SPM_FW_MAGIC_1:
    case SPM_FW_MAGIC_2:
        ret = 1;
        break;
    default:
        break;
    }

    return ret;
}
static void __get_spmfw_version(void *spm_addr, char *dst, int dst_len)
{
    vaddr_t *addr;
    int offset = 0;
    int ret;
    struct spm_fw_header *header;

    addr = spm_addr;
    header = (struct spm_fw_header *) addr;
    while (is_spmfw_magic(header->magic)) {
        unsigned short firmware_size = 0;
        char *ptr;

        ptr = (((char *) header) + sizeof(*header));
        memcpy(&firmware_size, ptr + offset, 2);

        offset += 16 + (firmware_size * 4);
        offset += sizeof(struct pcm_desc) - offsetof(struct pcm_desc, pmem_words);

        ret = snprintf(dst, dst_len, "%s", ptr + offset);
        if (ret < 0)
                dprintf(CRITICAL, "copy spmfw version string fail\n");
        else
                dprintf(CRITICAL, "spmfw version: %s\n", dst);
        header = (struct spm_fw_header *) (((char *) header) + sizeof(*header) + header->size);
        offset = 0;
    }

}
static void append_spmfw_version(void *spm_addr, void *fdt)
{
    const char *name = "spmfw_version";
    int nodeoffset;
    char version[PCM_FIRMWARE_VERSION_SIZE];

    if (!fdt) {
        dprintf(CRITICAL, "illegal fdt address. LINE: %d\n", __LINE__);
        return;
    }

    nodeoffset = fdt_node_offset_by_compatible(fdt, -1, "mediatek,sleep");
    if (nodeoffset < 0) {
        dprintf(CRITICAL, "/sleep not found. LINE: %d\n", __LINE__);
        return; /* return 1, if fail */
    }

    __get_spmfw_version(spm_addr, version, PCM_FIRMWARE_VERSION_SIZE);

    if (fdt_appendprop_string(fdt, nodeoffset, name, version) != 0)
        return;
}

static int __load_spmfw_to_dram(void)
{

    char part_name[16] = "spmfw";
    int ret;

    spmfw_base_addr_pa = mblock_alloc(SPM_MEM_SIZE, SPM_MEM_ALIGN, SPM_MEM_LIMIT,
        0, 0, "SPM-reserved");

    if (!spmfw_base_addr_pa) {
        dprintf(CRITICAL, "mblock allocation failed\n");
        ret = ERR_NO_MEMORY;
        goto err_mblock_alloc;
    }
    LTRACEF("[spmfw] addr=0x%llx size=0x%x\n", spmfw_base_addr_pa, SPM_MEM_SIZE);

    ret = vmm_alloc_physical(vmm_get_kernel_aspace(),
                        "vm-SPM-reserved",
                        SPM_MEM_SIZE,
                        &spmfw_base_addr_va,
                        PAGE_SIZE_SHIFT,
                        (paddr_t)spmfw_base_addr_pa,
                        0,
                        ARCH_MMU_FLAG_CACHED);

    LTRACEF("[spmfw] after vmm addr=0x%llx size=0x%x\n", spmfw_base_addr_pa, SPM_MEM_SIZE);

    if (ret != NO_ERROR) {
        dprintf(CRITICAL, "[spmfw] vmm_alloc_physical failed\n");
        goto err_vmm_alloc;
    }

    ret = load_partition(part_name, "spmfw", spmfw_base_addr_va, SPM_MEM_SIZE);
    if (ret < 0) {
        dprintf(CRITICAL, "[spmfw] load spmfw fail, ret = %d\n", ret);
        goto err_load_partition;
    }

    /* set spmfw range as invalid for flush to DRAM */
    arch_clean_invalidate_cache_range((addr_t)spmfw_base_addr_va, (size_t)SPM_MEM_SIZE);

    ret = NO_ERROR;
    return ret;

err_load_partition:
    vmm_free_region(vmm_get_kernel_aspace(), (vaddr_t)spmfw_base_addr_va);
err_vmm_alloc:
    mblock_free(spmfw_base_addr_pa);
err_mblock_alloc:
    return ret;
}
static int __free_spmfw_from_dram(void)
{
    vmm_free_region(vmm_get_kernel_aspace(), (vaddr_t)spmfw_base_addr_va);
    mblock_free(spmfw_base_addr_pa);
    return NO_ERROR;
}
status_t platform_load_spm(void *fdt)
{
    int ret;
    struct __smccc_res smccc_res;

    ret = __load_spmfw_to_dram();
    if (ret < 0)
        return ret;

    /* append spmfw version name to DTS */
    append_spmfw_version(spmfw_base_addr_va, fdt);

    /* notify atf to load spmfw  */
    LTRACEF("[spmfw] before smc call\n");
    memset(&smccc_res, 0, sizeof(smccc_res));
    LOAD_SPMFW_SMC(spmfw_base_addr_pa, SPM_MEM_SIZE, &smccc_res);
    LTRACEF("[spmfw] after smc call\n");

    ret = __free_spmfw_from_dram();

    return ret;
}

int aee_get_spmfw_version(char *dst, int dst_len)
{
    int ret;
    void *va;
    char part_name[16] = "spmfw";

    ret = 0;
    va = malloc(SPM_MEM_SIZE);
    if (!va) {
        dprintf(CRITICAL, "[spmfw][aee] malloc fail\n");
        return -1;
    }

    ret = load_partition(part_name, "spmfw", va, SPM_MEM_SIZE);
    if (ret < 0) {
        dprintf(CRITICAL, "[spmfw][aee] load spmfw fail, ret = %d\n", ret);
        goto err_aee_load_partition;
    }

    /* set spmfw range as invalid for flush to DRAM */
    arch_clean_invalidate_cache_range((addr_t)va, (size_t)SPM_MEM_SIZE);
    /* append spmfw version name to DTS */
    __get_spmfw_version(va, dst, dst_len);

err_aee_load_partition:
    free(va);
    return ret;
}

