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
#include "mcupm.h"
#include <platform/mboot_expdb.h>
#include <platform/reg.h>
#include <stdio.h>
#include <string.h>
#include <trace.h>

#define mcupm_ee_size 0x40080
#define EXTRA_BUF     0x10000

struct mcupm_reg_save_st {
    uint32_t addr;
    uint32_t size;
};

struct mcupm_reg_save_st mcupm_reg_save_list[2];

#if (MCUPM_Core_Dump == 1)
static void save_mcupm_last_log(CALLBACK dev_write)
{
    unsigned int *log_addr = NULL;
    unsigned int src_w_pos, src_r_pos;
    int len;

    /* in case the mcupm address is invalid */
    if (read32(PLT_RD_MAGIC) != INITIAL_MAGIC) {
        dprintf(CRITICAL, "[mcupm] invalid pclrsp = 0x%llx, lastlog = 0x%llx\n",
                (uint64_t)read32(GPR_BASE_ADDR(21)),
                (uint64_t)read32(GPR_BASE_ADDR(8)));
        return;
    }

    if (read32(MCUPM_LASTK_ADDR) == INITIAL_MAGIC) {
        dprintf(CRITICAL, "[mcupm] mcupm disable logger\n");
        return;
    }

    //get logger address
    log_addr = (unsigned int *)(read32(MCUPM_LASTK_ADDR) + MCUPM_SRAM_BASE);
    len = read32(MCUPM_LASTK_SZ);

    if (len <= 0) {
        dprintf(CRITICAL, "[mcupm] invalid last log size = %x\n", len);
        return;
    }

    src_w_pos = *(unsigned int **)(MCUPM_LASTK_W_POS);
    src_r_pos = *(unsigned int **)(MCUPM_LASTK_R_POS);

    src_w_pos = read32(src_w_pos + MCUPM_SRAM_BASE);
    src_r_pos = read32(src_r_pos + MCUPM_SRAM_BASE);

    if (src_w_pos > src_r_pos)
        dev_write(vaddr_to_paddr(log_addr + src_r_pos), src_w_pos - src_r_pos);
    else {
        dev_write(vaddr_to_paddr(log_addr + src_r_pos), len - src_r_pos);
        dev_write(vaddr_to_paddr(log_addr), src_w_pos);
    }
}
AEE_EXPDB_INIT_HOOK(SYS_MCUPM_LAST_LOG, AEE_LKDUMP_MCUPM_LAST_LOG_SZ, save_mcupm_last_log);
#endif

static void mcupm_do_regdump(uint32_t *out, uint32_t *out_end)
{
    int i = 0;
    void *from;
    uint32_t *buf = out;
    int size_limit = countof(mcupm_reg_save_list);
    uint32_t *update = NULL;
    int totalsize = 0;

    //initialize reg save list
    mcupm_reg_save_list[0].addr = MCUPM_SW_RSTN;
    mcupm_reg_save_list[0].size = 0x1000;
    mcupm_reg_save_list[1].addr = MCUPM_SW_RSTN + 0x2000;
    mcupm_reg_save_list[1].size = 0x100;

    for (i = 0; i < size_limit; i++) {
        if (((uint32_t)buf + mcupm_reg_save_list[i].size
                    + sizeof(struct mcupm_reg_save_st)) > (uint32_t)out_end) {
            dprintf(CRITICAL, "[mcupm] %s overflow\n", __func__);
            break;
        }
        *buf = mcupm_reg_save_list[i].addr;
        buf++;
        *buf = mcupm_reg_save_list[i].size;
        buf++;
        totalsize += 2;
        from = (void *)mcupm_reg_save_list[i].addr;
        memcpy(buf, from, mcupm_reg_save_list[i].size);
        buf += (mcupm_reg_save_list[i].size / sizeof(uint32_t));
        totalsize += (mcupm_reg_save_list[i].size / sizeof(uint32_t));
    }

    buf = buf - totalsize + 2;
    update = (unsigned int *)(read32(MCUPM_LAST_PCLRSP) + MCUPM_SRAM_BASE);
    buf = buf + (CPUEB_CFGREG_DBG_MON_PC/sizeof(uint32_t));

    write32(buf, read32(update));
    write32(buf+1, read32(update + 1));
    write32(buf+2, read32(update + 2));
}

/*
 * this function need mcupm to keeping awaken
 * mcupm_crash_dump: dump mcupm tcm info.
 * @param memorydump:   mcupm dump struct
 * @param mcupm_core_id:  core id
 * @return:             mcupm dump size
 */

static int mcupm_crash_dump(void *crash_buffer)
{
    unsigned int mcupm_dump_size;
    int ret = 0;
    struct MemoryDumpMCUPM *pmemorydump;
    uint32_t dram_start = 0;
    uint32_t dram_size = 0;
    void *dram_addr = NULL;
    int addr_diff = 0;


    /* region_info will be null when recovery boot fail, skip coredump */
    if (read32(GPR_BASE_ADDR(0)) == NULL) {
        dprintf(CRITICAL, "[mcupm] mcupm_region_info = 0, skip coredump\n");
        return 0;
    }

    pmemorydump = (void *)crash_buffer;

    //set mmu
    ret = vmm_alloc_physical(vmm_get_kernel_aspace(), "mcupm-reserved",
            MDUMP_DRAM_SIZE, &dram_addr, PAGE_SIZE_SHIFT,
            ROUNDDOWN((paddr_t)read32(MCUPM_DM_ADDR),
                PAGE_SIZE), 0, ARCH_MMU_FLAG_UNCACHED);

    if (ret != NO_ERROR) {
        dprintf(CRITICAL, "set mmu failed err = %d, dram_addr (0x%p) expected (0x%x)\n",
                ret, dram_addr, read32(MCUPM_DM_ADDR));
        return 0;
    }

    addr_diff = read32(MCUPM_DM_ADDR) - vaddr_to_paddr(dram_addr);
    memcpy((void *)&(pmemorydump->l2tcm), dram_addr + addr_diff, read32(MCUPM_DM_SZ));

    /* dump sys registers */
    //mcupm_do_regdump((void *)&(pmemorydump->regdump), (void *)&(pmemorydump->tbuf));

    /* dump trace buffer */
    //memcpy(pmemorydump->tbuf, read32(MCUPM_LAST_TBUF) + MCUPM_SRAM_BASE, MCUPM_TBUF_SIZE);

    /* set fw range as invalid for flush to DRAM */
    arch_clean_invalidate_cache_range((addr_t) dram_addr, (size_t) MDUMP_DRAM_SIZE);

    /* vmm free */
    vmm_free_region(vmm_get_kernel_aspace(), (vaddr_t)dram_addr);

    mcupm_dump_size = MDUMP_L2TCM_SIZE + MDUMP_REGDUMP_SIZE + MDUMP_TBUF_SIZE;

    return mcupm_dump_size;
}

static unsigned int save_mcupm_coredump(CALLBACK dev_write)
{
    int memory_dump_size;
    unsigned char *output = malloc(mcupm_ee_size + EXTRA_BUF);  // extra buffer 64 kb

    if (!output) {
        dprintf(CRITICAL, "[mcupm] memory allocat for output failed\n");
        return 0;
    }

    /* in case the mcupm address is invalid */
#ifdef LEGACY_MCUPM_LOADER
    if (read32(PLT_RD_MAGIC) != INITIAL_MAGIC || read32(MCUPM_DEV_STATUS) != COREDUMP_MAGIC) {
        dprintf(CRITICAL, "[mcupm] invalid sta=0x%llx plt=0x%llx pclrsp=0x%llx, lastlog=0x%llx\n",
               (uint64_t)read32(MCUPM_DEV_STATUS),
               (uint64_t)read32(PLT_RD_MAGIC),
#else
    if (read32(PLT_RD_MAGIC) != INITIAL_MAGIC) {
        dprintf(CRITICAL, "[mcupm] invalid plt=0x%llx pclrsp = 0x%llx, lastlog = 0x%llx\n",
               (uint64_t)read32(PLT_RD_MAGIC),
#endif
                (uint64_t)read32(GPR_BASE_ADDR(21)),
                (uint64_t)read32(GPR_BASE_ADDR(8)));
        free(output);
        return 0;
    }

    memset(output, 0, mcupm_ee_size + EXTRA_BUF);
    memory_dump_size = mcupm_crash_dump(output);

    if (memory_dump_size > mcupm_ee_size) {
        dprintf(CRITICAL, "[mcupm] memory_dump_size err %d\n", memory_dump_size);
        memory_dump_size = 0;
    } else
        dev_write(vaddr_to_paddr(output), memory_dump_size);

    free(output);

    return memory_dump_size;
}
AEE_EXPDB_INIT_HOOK(SYS_MCUPM_COREDUMP, AEE_LKDUMP_MCUPM_COREDUMP_SZ, save_mcupm_coredump);

