/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <arch/ops.h>
#include <assert.h>
#include <err.h>
#include <kernel/vm.h>
#include <load_image.h>
#include <mblock.h>
#include <platform/dpm.h>
#include <string.h>
#include <trace.h>

#define LOCAL_TRACE  0

__WEAK void wake_dpm_sram_up(void)
{
    LTRACEF("[dpm] weak waking up dpm SRAM\n");
    return;
}

status_t platform_load_dpm(void)
{
    void *pm_ptr, *dm_ptr, *dpm_dram_addr_va;
    int pm_size, dm_size, ret;
    u64 dpm_dram_addr_pa;
    unsigned int i = 0;

    dpm_dram_addr_pa = mblock_alloc(DPM_DRAM_SIZE, PAGE_SIZE,
                                MBLOCK_NO_LIMIT, 0, 0, "DPM-reserved");

    LTRACEF("[DPM] %s: dpm_dram_addr=%llu\n", __func__, dpm_dram_addr_pa);
    if (!dpm_dram_addr_pa) {
        dprintf(CRITICAL, "[DPM] mblock_reserve fail\n");
        return ERR_NO_MEMORY;
    }

    ret = vmm_alloc_physical(vmm_get_kernel_aspace(),
                        "vm-DPM-reserved",
                        DPM_DRAM_SIZE,
                        &dpm_dram_addr_va,
                        PAGE_SIZE_SHIFT,
                        (paddr_t)dpm_dram_addr_pa,
                        0,
                        ARCH_MMU_FLAG_CACHED);
    if (ret != NO_ERROR) {
        dprintf(CRITICAL, "[DPM] vmm_alloc_physical failed\n");
        goto err_vmm_alloc;
    }

    wake_dpm_sram_up();

    // step 1: load pm
    pm_ptr = dpm_dram_addr_va;
    pm_size = ret = load_partition(DPM_IMG, DPM_PM, pm_ptr, DPM_DM_OFFSET);
    if (ret < 0) {
        dprintf(CRITICAL, "[DPM] load_DPM_image fail %s\n", DPM_PM);
        goto err_load_partition;
    }

    // step 2: load dm
    dm_ptr = dpm_dram_addr_va + DPM_DM_OFFSET;
    dm_size = ret = load_partition(DPM_IMG, DPM_DM, dm_ptr, DPM_DRAM_SIZE - DPM_DM_OFFSET);
    if (dm_size < 0) {
        dprintf(CRITICAL, "[DPM] load_DPM_image fail %s\n", DPM_DM);
        goto err_load_partition;
    }
    for (i = 0; i < DPM_NUM; i++) {
        //config DPM SRAM layout
        LTRACEF("[DPM] %s: pm_ptr=%p pm_size=0x%x dm_ptr=%p dm_size=0x%x\n",
                     __func__, pm_ptr, pm_size, dm_ptr, dm_size);
        if (pm_size > 0x8000) { //pm > 32K
             //bigger than sram size
            return -1;
        } else if (pm_size > 0x4000) { //pm > 16K
            //Default Value
        } else if (pm_size > 0x2000) { //pm > 8K
            writel(readl((DPM_CFG_CH0 + i * DPM_CH_OFFSET)) | 0x10000000,
                         (DPM_CFG_CH0 + i * DPM_CH_OFFSET));
        } else { //pm < 8K
            writel(readl((DPM_CFG_CH0 + i * DPM_CH_OFFSET)) | 0x20000000,
                         (DPM_CFG_CH0 + i * DPM_CH_OFFSET));
        }
        //   DPM DRAM layout
        /*
         ************************
         *     PM: 24|16|8      *
         ************************
         *     DM               *
         ************************
         */
        // step 3: copy PM to DPM_SRAM
        memcpy((void *) (DPM_PM1_SRAM_BASE + i * DPM_CH_OFFSET), pm_ptr, pm_size);
        memcpy((void *) (DPM_DM1_SRAM_BASE + i * DPM_CH_OFFSET), dm_ptr, dm_size);

        // step 4: write bootargs
        writel(0x00000000, (DPM_BARGS_CH0_REG0 + i * DPM_CH_OFFSET));
        writel(0x00000000, (DPM_BARGS_CH0_REG1 + i * DPM_CH_OFFSET));
    }

    DSB;

    for (i = 0; i < DPM_NUM; i++) {
        //step 5: Release Reset
        writel(readl((DPM_CFG_CH0 + i * DPM_CH_OFFSET)) | 0x1, (DPM_CFG_CH0 + i * DPM_CH_OFFSET));
    }
    ret = NO_ERROR;
err_load_partition:
    vmm_free_region(vmm_get_kernel_aspace(), (vaddr_t)dpm_dram_addr_va);
err_vmm_alloc:
    mblock_free(dpm_dram_addr_pa);
    return ret;
}
