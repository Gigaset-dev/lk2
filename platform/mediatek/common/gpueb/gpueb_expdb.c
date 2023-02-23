/*
 * copyright (c) 2021 mediatek inc.
 *
 * use of this source code is governed by a mit-style
 * license that can be found in the license file or at
 * https://opensource.org/licenses/mit
 */
#include <arch/ops.h>
#include <debug.h>
#include <err.h>
#include <kernel/vm.h>
#include <platform/mboot_expdb.h>
#include <platform/mrdump_params.h>
#include <platform/reg.h>
#include <stdio.h>
#include <string.h>
#include <trace.h>
#include "gpueb.h"

#define LOCAL_TRACE        0

#define gpueb_ee_size      0x40080
#define EXTRA_BUF          0x10000

struct gpueb_reg_save_st {
    uint32_t addr;
    uint32_t size;
};

struct gpueb_reg_save_st gpueb_reg_save_list[2];

static void *gpueb_expcpy(void *dst, const void *src, int count)
{
    unsigned int *tmp = dst;
    const unsigned int *s = src;

    while (count > 0) {
        *tmp++ = *s++;
        count -= sizeof(unsigned int);
    }

    return dst;
}

static void save_dram_gpueb_last_log(CALLBACK dev_write)
{
    paddr_t gpu_shared_dram_pa = 0;
    paddr_t gpueb_sram_on_dram_pa = 0;
    vaddr_t gpu_shared_dram_va = 0;
    vaddr_t gpueb_sram_on_dram_va = 0;
    u64 reserved_size = 0;
    paddr_t log_addr = 0;
    int len = 0;
    int ret = 0;

    if (!(read32(SPM_XPU_PWR_STATUS)&SPM_XPU_PWR_STATUS_MFG0)) {
        dprintf(CRITICAL, "%s: GPUEB(MFG0) NOT power-on!!!\n", EXPMOD);
        return;
    }

    /* dump gpu shared dram data */
    ret = mrdump_get_reserved_info("me_GPUEB_SHARED", (u64 *)&gpu_shared_dram_pa, &reserved_size);
    if (ret < 0) {
        dprintf(CRITICAL, "%s: get reserved shared dram fail\n", EXPMOD);
        return;
    }
    LTRACEF("%s: gpu_shared_dram_pa: 0x%lx, size: 0x%llx\n",
        EXPMOD, gpu_shared_dram_pa, reserved_size);

    ret = vmm_alloc_physical(vmm_get_kernel_aspace(),
            "vm-GPU-shared-reserved",
            reserved_size,
            (void *)&gpu_shared_dram_va,
            PAGE_SIZE_SHIFT,
            gpu_shared_dram_pa,
            0,
            ARCH_MMU_FLAG_CACHED);
    if (ret != NO_ERROR) {
        dprintf(CRITICAL, "%s: vmm failed for gpu shared dram, err: %d\n", EXPMOD, ret);
        return;
    }
    LTRACEF("%s: vmm_alloc_physical: PA[0x%lx], VA[0x%lx]\n",
        EXPMOD, gpu_shared_dram_pa, gpu_shared_dram_va);

    arch_clean_invalidate_cache_range((addr_t)gpu_shared_dram_va, (size_t)reserved_size);

    gpueb_sram_on_dram_pa = gpu_shared_dram_pa + GPUEB_DATA_DRAM_OFS;
    gpueb_sram_on_dram_va = gpu_shared_dram_va + GPUEB_DATA_DRAM_OFS;
    LTRACEF_LEVEL(ALWAYS, "%s: shared dram va: 0x%lx, sram at dram va: 0x%lx\n",
        EXPMOD, gpu_shared_dram_va, gpueb_sram_on_dram_va);

    /* in case the gpueb sram data is invalid */
    if ((unsigned int)(*(unsigned int *)(gpueb_sram_on_dram_va + (PLT_RD_MAGIC - GPUEB_SRAM_BASE)))
            != INITIAL_MAGIC) {
        dprintf(CRITICAL, "%s: invalid pclrsp = 0x%x, lastlog = 0x%x\n",
                EXPMOD,
                *(unsigned int *)(gpueb_sram_on_dram_va + (GPUEB_LAST_PCLRSP - GPUEB_SRAM_BASE)),
                *(unsigned int *)(gpueb_sram_on_dram_va + (GPUEB_LASTK_ADDR - GPUEB_SRAM_BASE)));
        goto err_vmm_alloc;
    }

    if ((unsigned int)(*(unsigned int *)(gpueb_sram_on_dram_va +
            (GPUEB_LASTK_ADDR - GPUEB_SRAM_BASE))) == INITIAL_MAGIC) {
        dprintf(CRITICAL, "%s: gpueb disable logger, no lastk\n", EXPMOD);
        goto err_vmm_alloc;
    }

    //get logger address
    log_addr = gpueb_sram_on_dram_pa +
        *(unsigned int *)(gpueb_sram_on_dram_va + (GPUEB_LASTK_ADDR - GPUEB_SRAM_BASE));
    len = (int)(*(unsigned int *)(gpueb_sram_on_dram_va + (GPUEB_LASTK_SZ - GPUEB_SRAM_BASE)));
    LTRACEF("%s: last log: Addr[0x%lx], size = [0x%x]\n",
        EXPMOD, log_addr, len);

    if (len <= 0) {
        dprintf(CRITICAL, "%s: invalid last log size = %d\n", EXPMOD, len);
        goto err_vmm_alloc;
    }

    if ((log_addr + len)
            > (gpueb_sram_on_dram_pa + GPUEB_SRAM_SIZE - RESERVED_SRAM_SIZE)) {
        dprintf(CRITICAL, "%s: invalid last log addr(dram: 0x%lx)\n", EXPMOD, log_addr);
        dprintf(CRITICAL, "%s: end log addr(0x%lx) > sram range(0x%lx)\n",
            EXPMOD, (log_addr + len),
            (gpueb_sram_on_dram_pa + GPUEB_SRAM_SIZE - RESERVED_SRAM_SIZE));
        goto err_vmm_alloc;
    }

    dev_write(log_addr, len);

err_vmm_alloc:
    vmm_free_region(vmm_get_kernel_aspace(), gpu_shared_dram_va);
}

static void save_gpueb_last_log(CALLBACK dev_write)
{
    vaddr_t log_addr = 0;
    int len = 0;

    if (!(read32(SPM_XPU_PWR_STATUS)&SPM_XPU_PWR_STATUS_MFG0)) {
        dprintf(CRITICAL, "%s: GPUEB(MFG0) NOT power-on!!!\n", EXPMOD);
        return;
    }

    /* in case the gpueb sram data is invalid */
    if (read32(PLT_RD_MAGIC) != INITIAL_MAGIC) {
        dprintf(CRITICAL, "%s: invalid pclrsp = 0x%x, lastlog = 0x%x\n",
                EXPMOD,
                read32(GPUEB_LAST_PCLRSP),
                read32(GPUEB_LASTK_ADDR));
        return;
    }

    if (read32(GPUEB_LASTK_ADDR) == INITIAL_MAGIC) {
        dprintf(CRITICAL, "%s: gpueb disable logger, no lastk\n", EXPMOD);
        return;
    }

    //get logger address
    log_addr = (vaddr_t)(read32(GPUEB_LASTK_ADDR) + GPUEB_SRAM_BASE);
    len = read32(GPUEB_LASTK_SZ);

    if (len <= 0) {
        dprintf(CRITICAL, "%s: invalid last log size = %d\n", EXPMOD, len);
        return;
    }

    if ((log_addr + len)
            > ((vaddr_t)GPUEB_SRAM_BASE + GPUEB_SRAM_SIZE - RESERVED_SRAM_SIZE)) {
        dprintf(CRITICAL, "%s: invalid last log addr(0x%lx)\n", EXPMOD, log_addr);
        dprintf(CRITICAL, "%s: end log addr(0x%lx) > sram range(0x%lx)\n",
            EXPMOD, (log_addr + len),
            (GPUEB_SRAM_BASE + GPUEB_SRAM_SIZE - RESERVED_SRAM_SIZE));
        return;
    }

    write32(GPUEB_CFGREG_SW_RSTN, 0x0);
    LTRACEF("%s: init GPUEB_CFGREG_SW_RSTN = 0x%x\n",
        EXPMOD, read32(GPUEB_CFGREG_SW_RSTN));

    dev_write(vaddr_to_paddr((void *)log_addr), len);
}
#if GPUEB_AEE_LK_EXP
#if GPUEB_SRAM_RESET_BY_HWGRST
AEE_EXPDB_INIT_HOOK(SYS_GPUEB_LAST_LOG, AEE_LKDUMP_GPUEB_LAST_LOG_SZ, save_dram_gpueb_last_log);
#else
AEE_EXPDB_INIT_HOOK(SYS_GPUEB_LAST_LOG, AEE_LKDUMP_GPUEB_LAST_LOG_SZ, save_gpueb_last_log);
#endif
#endif

/*
 * this function need gpueb to keeping awaken
 * gpueb_crash_dump: dump gpueb tcm info.
 * @param memorydump:   gpueb dump struct
 * @param gpueb_core_id:  core id
 * @return:             gpueb dump size
 */

static int gpueb_crash_dump(void *crash_buffer)
{
    unsigned int gpueb_dump_size = 0;
    int ret = 0;
    struct MemoryDumpGPUEB *pmemorydump;
    unsigned int regdump_offset = 0;
    unsigned int tbuffer_offset = 0;
    char *regdump_base = NULL;
    paddr_t gpu_shared_dram_pa = 0;
    vaddr_t gpu_shared_dram_va = 0;
#if GPUEB_SRAM_RESET_BY_HWGRST
    vaddr_t gpueb_sram_on_dram_va = 0;
#endif
    u64 reserved_size = 0;

    /* region_info will be null when recovery boot fail, skip coredump */
    if (read32(GPUEB_INFO) == 0) {
        dprintf(CRITICAL, "%s: GPUEB_INFO = 0, skip coredump\n", EXPMOD);
#if !(IGNORE_SRAM_VALID_CHECK)
        return ret;
#endif
    }

    pmemorydump = (void *)crash_buffer;

    LTRACEF("%s: MFG PWR status: 0x%x\n",
        EXPMOD, read32(SPM_XPU_PWR_STATUS));

    write32(GPUEB_CFGREG_SW_RSTN, 0x0);
    LTRACEF("%s: init GPUEB_CFGREG_SW_RSTN = 0x%x\n",
        EXPMOD, read32(GPUEB_CFGREG_SW_RSTN));

#if !(GPUEB_SRAM_RESET_BY_HWGRST)
    /* dump full SRAM data */
    gpueb_expcpy((void *)&(pmemorydump->l2tcm), (void *)GPUEB_SRAM_BASE, GPUEB_SRAM_SIZE);
    LTRACEF("%s: Copy SRAM data from GPUEB SRAM[0x%lx]\n",
        EXPMOD, GPUEB_SRAM_BASE);

    /* dump trace buffer */
    tbuffer_offset = read32(GPUEB_LAST_TBUF);
    LTRACEF("%s: tbuffer offset(0x%x), size(0x%x)\n",
        EXPMOD, tbuffer_offset, GPUEB_TBUF_SIZE);
    if (tbuffer_offset > (GPUEB_SRAM_SIZE - RESERVED_SRAM_SIZE - GPUEB_TBUF_SIZE))
        dprintf(CRITICAL, "%s: tbuffer offset(0x%x) invalid\n", EXPMOD, tbuffer_offset);
    else
        gpueb_expcpy(pmemorydump->tbuf,
            (void *)(tbuffer_offset + GPUEB_SRAM_BASE), GPUEB_TBUF_SIZE);
#endif

    /* dump gpu shared dram data */
    ret = mrdump_get_reserved_info("me_GPUEB_SHARED", (u64 *)&gpu_shared_dram_pa, &reserved_size);
    if (ret < 0) {
        dprintf(CRITICAL, "%s: get reserved shared dram fail\n", EXPMOD);
        goto dumpreg;
    }
    LTRACEF("%s: gpu_shared_dram_pa: 0x%lx, size: 0x%llx\n",
        EXPMOD, gpu_shared_dram_pa, reserved_size);

    ret = vmm_alloc_physical(vmm_get_kernel_aspace(),
            "vm-GPU-shared-reserved",
            reserved_size,
            (void *)&gpu_shared_dram_va,
            PAGE_SIZE_SHIFT,
            gpu_shared_dram_pa,
            0,
            ARCH_MMU_FLAG_CACHED);
    if (ret != NO_ERROR) {
        dprintf(CRITICAL, "%s: vmm failed for gpu shared dram, err: %d\n",
            EXPMOD, ret);
        goto dumpreg;
    }
    LTRACEF("%s: vmm_alloc_physical: PA[0x%lx], VA[0x%lx]\n",
        EXPMOD, gpu_shared_dram_pa, gpu_shared_dram_va);

    arch_clean_invalidate_cache_range((addr_t)gpu_shared_dram_va, (size_t)reserved_size);

#if GPUEB_SRAM_RESET_BY_HWGRST
    /* dump SRAM data from shared dram */
    gpueb_sram_on_dram_va = gpu_shared_dram_va + GPUEB_DATA_DRAM_OFS;
    LTRACEF_LEVEL(ALWAYS, "%s: shared dram va: 0x%lx, sram at dram va: 0x%lx\n",
        EXPMOD, gpu_shared_dram_va, gpueb_sram_on_dram_va);

    gpueb_expcpy((void *)&(pmemorydump->l2tcm), (void *)gpueb_sram_on_dram_va, GPUEB_SRAM_SIZE);
    LTRACEF("%s: Copy SRAM data from GPU shared dram[0x%lx]\n",
        EXPMOD, gpueb_sram_on_dram_va);

    LTRACEF_LEVEL(ALWAYS, "%s: GPUEB footprint: 0x%x\n", EXPMOD,
        *(int *)(&(pmemorydump->l2tcm[GPUEB_INIT_FOOTPRINT-GPUEB_SRAM_BASE])));

    /* dump trace buffer from shared dram */
    tbuffer_offset = (unsigned int)(*(unsigned int *)(gpueb_sram_on_dram_va +
        (GPUEB_LAST_TBUF - GPUEB_SRAM_BASE)));
    LTRACEF("%s: tbuffer offset(0x%x), size(0x%x)\n",
        EXPMOD, tbuffer_offset, GPUEB_TBUF_SIZE);
    if (tbuffer_offset > (GPUEB_SRAM_SIZE - RESERVED_SRAM_SIZE - GPUEB_TBUF_SIZE))
        dprintf(CRITICAL, "%s: tbuffer offset(0x%x) invalid\n", EXPMOD, tbuffer_offset);
    else
        gpueb_expcpy(pmemorydump->tbuf,
            (void *)(tbuffer_offset + gpueb_sram_on_dram_va), GPUEB_TBUF_SIZE);
#endif

    vmm_free_region(vmm_get_kernel_aspace(), gpu_shared_dram_va);

dumpreg:
#if GPUEB_PERI_DUMP_SUPPORT
    /* dump CFGREG/INTC/MBOX register */
    //CFGREG 0x200(0x200)
    memcpy((pmemorydump->regdump+regdump_offset),
           (void *)GPUEB_CFGREG_BASE, GPUEB_CFGREG_DUMP_SIZE);
    regdump_offset += GPUEB_CFGREG_DUMP_SIZE;
    regdump_offset = ROUNDDOWN(regdump_offset, 16);
    LTRACEF("%s: CFGREG register dump done\n", EXPMOD);

    //CKCTRL 0x10(0x10)
    memcpy((pmemorydump->regdump+regdump_offset),
           (void *)GPUEB_CKCTRL_BASE, GPUEB_CKCTRL_DUMP_SIZE);
    regdump_offset += GPUEB_CKCTRL_DUMP_SIZE;
    regdump_offset = ROUNDDOWN(regdump_offset, 16);
    LTRACEF("%s: CKCTRL register dump done\n", EXPMOD);

    //DMA 0x80(0x78)
    memcpy((pmemorydump->regdump+regdump_offset), (void *)GPUEB_DMA0_BASE, GPUEB_DMA0_DUMP_SIZE);
    regdump_offset += GPUEB_DMA0_DUMP_SIZE;
    memcpy((pmemorydump->regdump+regdump_offset), (void *)GPUEB_DMA1_BASE, GPUEB_DMA1_DUMP_SIZE);
    regdump_offset += GPUEB_DMA1_DUMP_SIZE;
    regdump_offset = ROUNDDOWN(regdump_offset, 16);
    LTRACEF("%s: DMA register dump done\n", EXPMOD);

    //INTC 0x60(0x60)
    //gpueb_regcpy((pmemorydump->regdump+regdump_offset), GPUEB_INTC_BASE, GPUEB_INTC_DUMP_SIZE);
    memcpy((pmemorydump->regdump+regdump_offset), (void *)GPUEB_INTC_BASE, GPUEB_INTC_DUMP_SIZE);
    regdump_offset += GPUEB_INTC_DUMP_SIZE;
    regdump_offset = ROUNDDOWN(regdump_offset, 16);
    LTRACEF("%s: INTC register dump done\n", EXPMOD);

    //MBOX 0x20(0x18)
    regdump_base = (pmemorydump->regdump+regdump_offset);
    *regdump_base = read32(GPUEB_MBOX_IPI_GPUEB_STA);
    regdump_offset += sizeof(unsigned int);

    regdump_base = (pmemorydump->regdump+regdump_offset);
    *regdump_base = read32(GPUEB_MBOX_IPI_GPUEB_SET);
    regdump_offset += sizeof(unsigned int);

    regdump_base = (pmemorydump->regdump+regdump_offset);
    *regdump_base = read32(GPUEB_MBOX_IPI_GPUEB_CLR);
    regdump_offset += sizeof(unsigned int);

    regdump_base = (pmemorydump->regdump+regdump_offset);
    *regdump_base = read32(GPUEB_MBOX_SW_INT_STA);
    regdump_offset += sizeof(unsigned int);

    regdump_base = (pmemorydump->regdump+regdump_offset);
    *regdump_base = read32(GPUEB_MBOX_SW_INT_GPUEB_SET);
    regdump_offset += sizeof(unsigned int);

    regdump_base = (pmemorydump->regdump+regdump_offset);
    *regdump_base = read32(GPUEB_MBOX_SW_INT_GPUEB_CLR);
    regdump_offset += sizeof(unsigned int);

    regdump_offset = ROUNDDOWN(regdump_offset, 16);
    LTRACEF("%s: MBOX register dump done\n", EXPMOD);

    //PMIC_INTF 0x20(0x14)
    regdump_base = (pmemorydump->regdump+regdump_offset);
    *regdump_base = read32(GPUEB_PMIC_SW_INTF);
    regdump_offset += sizeof(unsigned int);

    regdump_base = (pmemorydump->regdump+regdump_offset);
    *regdump_base = read32(GPUEB_PMIC_CMD_DAT);
    regdump_offset += sizeof(unsigned int);

    regdump_base = (pmemorydump->regdump+regdump_offset);
    *regdump_base = read32(GPUEB_PMIC_CMD_STATE);
    regdump_offset += sizeof(unsigned int);

    regdump_base = (pmemorydump->regdump+regdump_offset);
    *regdump_base = read32(GPUEB_PMIC_READ_CMD_DAT);
    regdump_offset += sizeof(unsigned int);

    regdump_base = (pmemorydump->regdump+regdump_offset);
    *regdump_base = read32(GPUEB_PMIC_READ_CMD_DAT_BACK);
    regdump_offset += sizeof(unsigned int);

    regdump_offset = ROUNDDOWN(regdump_offset, 16);
    LTRACEF("%s: PMIC_INTF register dump done\n", EXPMOD);

    //RPC 0x30(0x2C)
    regdump_base = (pmemorydump->regdump+regdump_offset);
    *regdump_base = read32(MFG_RPC_GPUEB_TURBO);
    regdump_offset += sizeof(unsigned int);

    regdump_base = (pmemorydump->regdump+regdump_offset);
    *regdump_base = read32(MFG_RPC_GPUEB_CFG);
    regdump_offset += sizeof(unsigned int);

    regdump_base = (pmemorydump->regdump+regdump_offset);
    *regdump_base = read32(MFG_RPC_AO_CLK_CFG);
    regdump_offset += sizeof(unsigned int);

    regdump_base = (pmemorydump->regdump+regdump_offset);
    *regdump_base = read32(MFG_RPC_INTF_PWR_RDY_REG);
    regdump_offset += sizeof(unsigned int);

    regdump_base = (pmemorydump->regdump+regdump_offset);
    *regdump_base = read32(MFG_GPU_EB_MBIST_TCM_DELSEL);
    regdump_offset += sizeof(unsigned int);

    regdump_base = (pmemorydump->regdump+regdump_offset);
    *regdump_base = read32(MFG_GPU_EB_MBIST_BTB_DELSEL);
    regdump_offset += sizeof(unsigned int);

    regdump_base = (pmemorydump->regdump+regdump_offset);
    *regdump_base = read32(MFG_GPU_EB_SPM_RPC_SLP_PROT_EN_SET);
    regdump_offset += sizeof(unsigned int);

    regdump_base = (pmemorydump->regdump+regdump_offset);
    *regdump_base = read32(MFG_GPU_EB_SPM_RPC_SLP_PROT_EN_CLR);
    regdump_offset += sizeof(unsigned int);

    regdump_base = (pmemorydump->regdump+regdump_offset);
    *regdump_base = read32(MFG_GPU_EB_SPM_RPC_SLP_PROT_EN_STA);
    regdump_offset += sizeof(unsigned int);

    regdump_base = (pmemorydump->regdump+regdump_offset);
    *regdump_base = read32(MFG_GPU_EB_CLONED_SC_MFG1_PWR_CON);
    regdump_offset += sizeof(unsigned int);

    regdump_base = (pmemorydump->regdump+regdump_offset);
    *regdump_base = read32(MFG_PTP_BRISKET_PWR_CON);
    regdump_offset += sizeof(unsigned int);

    regdump_offset = ROUNDDOWN(regdump_offset, 16);
    LTRACEF("%s: RPC register dump done\n", EXPMOD);

    //SPM_INTF 0x20(0x18)
    memcpy((pmemorydump->regdump+regdump_offset), (void *)GPUEB_SPM_INTF, GPUEB_SPM_INTF_DUMP_SIZE);
    regdump_offset += GPUEB_SPM_INTF_DUMP_SIZE;
    regdump_offset = ROUNDDOWN(regdump_offset, 16);
    LTRACEF("%s: SPM_INTF register dump done\n", EXPMOD);
#endif

    gpueb_dump_size = MDUMP_L2TCM_SIZE + MDUMP_REGDUMP_SIZE + MDUMP_TBUF_SIZE;

    return gpueb_dump_size;
}

static void save_gpueb_coredump(CALLBACK dev_write)
{
    int memory_dump_size = 0;
    unsigned char *output = malloc(gpueb_ee_size + EXTRA_BUF);  // extra buffer 64 kb

    if (!output) {
        dprintf(CRITICAL, "%s: memory allocat for output failed\n", EXPMOD);
        return;
    }

    if (!(read32(SPM_XPU_PWR_STATUS)&SPM_XPU_PWR_STATUS_MFG0)) {
        dprintf(CRITICAL, "%s: GPUEB(MFG0) NOT power-on!!!\n", EXPMOD);
        free(output);
        return;
    }

#if !(GPUEB_SRAM_RESET_BY_HWGRST)
    /* in case the gpueb address is invalid */
    if (read32(PLT_RD_MAGIC) != INITIAL_MAGIC) {
        dprintf(CRITICAL, "%s: invalid pclrsp = 0x%x, lastlog = 0x%x\n",
                EXPMOD,
                read32(GPUEB_LAST_PCLRSP),
                read32(GPUEB_LASTK_ADDR));
#if !(IGNORE_SRAM_VALID_CHECK)
        free(output);
        return;
#endif
    }
#endif

    memset(output, 0, gpueb_ee_size + EXTRA_BUF);
    memory_dump_size = gpueb_crash_dump(output);

    if (memory_dump_size > gpueb_ee_size)
        dprintf(CRITICAL, "%s: memory_dump_size err %d\n", EXPMOD, memory_dump_size);
    else
        dev_write(vaddr_to_paddr(output), memory_dump_size);

    free(output);

    return;
}
#if GPUEB_AEE_LK_EXP
AEE_EXPDB_INIT_HOOK(SYS_GPUEB_COREDUMP, AEE_LKDUMP_GPUEB_COREDUMP_SZ, save_gpueb_coredump);
#endif

static void save_gpu_eb_data_dump(CALLBACK dev_write)
{
    unsigned int *gpueb_sysram_addr = (unsigned int *)GPUEB_RSV_SYSRAM_BASE;
    int sysram_dump_size = AEE_LKDUMP_GPUEB_SYSRAM_DATA_SZ;

    paddr_t gpu_shared_dram_pa = 0;
    u64 reserved_size = 0;
    paddr_t gpufreq_data_addr = 0;
    int gpufreq_dump_size = AEE_LKDUMP_GPUFREQ_DATA_SZ;
    int ret = 0;

#if GPUEB_SYARAM_DATA_SUPPORT
    /* dump gpueb sysram data */
    dev_write(vaddr_to_paddr(gpueb_sysram_addr), sysram_dump_size);
#else
    dprintf(CRITICAL, "%s: gpueb sysram data not suppot\n", EXPMOD);
#endif

    /* dump gpu shared dram data */
    ret = mrdump_get_reserved_info("me_GPUEB_SHARED", (u64 *)&gpu_shared_dram_pa, &reserved_size);
    if (ret < 0) {
        dprintf(CRITICAL, "%s: get reserved shared dram fail\n", EXPMOD);
        return;
    }
    LTRACEF("%s: gpu_shared_dram_pa: 0x%lx, size: 0x%llx\n",
        EXPMOD, gpu_shared_dram_pa, reserved_size);

    gpufreq_data_addr = gpu_shared_dram_pa;

    dev_write(gpufreq_data_addr, gpufreq_dump_size);

}
#if GPU_EB_EXT_DATA_DUMP_SUPPORT
AEE_EXPDB_INIT_HOOK(SYS_GPU_EB_EXT_DUMP, AEE_LKDUMP_GPU_EB_EXT_DATA_SZ, save_gpu_eb_data_dump);
#endif

static void save_gpueb_xfile(CALLBACK dev_write)
{
    paddr_t gpu_shared_dram_pa = 0;
    paddr_t gpueb_xfile_on_dram_pa = 0;
    vaddr_t gpu_shared_dram_va = 0;
    vaddr_t gpueb_xfile_on_dram_va = 0;
    paddr_t gpueb_xfile_pa = 0;
    unsigned int gpueb_xsize = 0;
    u64 reserved_size = 0;
    int ret = 0;
    struct ptimg_hdr_t *hdr = NULL;

    /* dump gpu shared dram data */
    ret = mrdump_get_reserved_info("me_GPUEB_SHARED", (u64 *)&gpu_shared_dram_pa, &reserved_size);
    if (ret < 0) {
        dprintf(CRITICAL, "%s: get reserved shared dram fail\n", EXPMOD);
        return;
    }
    LTRACEF("%s: gpu_shared_dram_pa: 0x%lx, size: 0x%llx\n",
        EXPMOD, gpu_shared_dram_pa, reserved_size);

    ret = vmm_alloc_physical(vmm_get_kernel_aspace(),
            "vm-GPU-shared-reserved",
            reserved_size,
            (void *)&gpu_shared_dram_va,
            PAGE_SIZE_SHIFT,
            gpu_shared_dram_pa,
            0,
            ARCH_MMU_FLAG_CACHED);
    if (ret != NO_ERROR) {
        dprintf(CRITICAL, "%s: vmm failed for gpu shared dram, err: %d\n", EXPMOD, ret);
        return;
    }
    LTRACEF("%s: vmm_alloc_physical: PA[0x%lx], VA[0x%lx]\n",
        EXPMOD, gpu_shared_dram_pa, gpu_shared_dram_va);

    arch_clean_invalidate_cache_range((addr_t)gpu_shared_dram_va, (size_t)reserved_size);

    gpueb_xfile_on_dram_pa = gpu_shared_dram_pa + GPUEB_XFILE_DRAM_OFS;
    gpueb_xfile_on_dram_va = gpu_shared_dram_va + GPUEB_XFILE_DRAM_OFS;
    LTRACEF("%s: gpueb_xfile_on_dram_pa: 0x%lx, gpueb_xfile_on_dram_va: 0x%lx\n",
        EXPMOD, gpueb_xfile_on_dram_pa, gpueb_xfile_on_dram_va);

    hdr = (struct ptimg_hdr_t *) gpueb_xfile_on_dram_va;

    if (hdr->magic == PT_MAGIC) {
        /* SYS_GPUEB_XFILE should include header not only xfile for parsing*/
        gpueb_xfile_pa = gpueb_xfile_on_dram_pa;
        gpueb_xsize = hdr->hdr_size + hdr->img_size;
    } else {
        dprintf(CRITICAL, "%s: invalid xfile hdr->magic(0x%x) not equal to PT_MAGIC(0x%lx)\n",
            EXPMOD, hdr->magic, (unsigned long) PT_MAGIC);
        goto err_vmm_alloc;
    }

    LTRACEF("%s parse xfile header done: gpueb_xfile_pa(0x%lx), gpueb_xsize(%x)\n",
        EXPMOD, gpueb_xfile_pa, gpueb_xsize);

    if (gpueb_xsize == 0 || gpueb_xsize > AEE_LKDUMP_GPUEB_XFILE_SZ) {
        dprintf(CRITICAL, "%s: invalid xfile size(0x%x), AEE_LKDUMP_GPUEB_XFILE_SZ(0x%lx)\n",
            EXPMOD, gpueb_xsize, (unsigned long) AEE_LKDUMP_GPUEB_XFILE_SZ);
        goto err_vmm_alloc;
    } else {
        dev_write(gpueb_xfile_pa, gpueb_xsize);
    }

err_vmm_alloc:
    vmm_free_region(vmm_get_kernel_aspace(), gpu_shared_dram_va);
}
#if GPUEB_XFILE_SUPPORT
AEE_EXPDB_INIT_HOOK(SYS_GPUEB_XFILE, AEE_LKDUMP_GPUEB_XFILE_SZ, save_gpueb_xfile);
#endif
