/*
 * Copyright (c) 2019 MediaTek Inc.
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
#include <debug.h>
#include <kernel/vm.h>
#include <mblock.h>
#include <platform/mboot_expdb.h>
#include <platform/mrdump_params.h>
#include <reg.h>
#include <sspm_plat.h>
#include <stdio.h>
#include <string.h>
#include <trace.h>

#define LOCAL_TRACE 0

struct sspm_tbuf_st {
    u32 tbuf_wptr;
    u32 tbuf_l[16];
    u32 tbuf_h[16];
};

static struct sspm_tbuf_st sspm_tbuf;

void save_tbuf(void)
{
    static struct sspm_tbuf;
    unsigned int tbufl, tbufh, r, i, j;

    sspm_tbuf.tbuf_wptr = readl(SSPM_TBUF_WPTR);
    r = sspm_tbuf.tbuf_wptr;
    for (i = 0, j = r; i < 16; ++i) {
        j = (j-1) & 0xF;
        writel(j, SSPM_DBG_SEL);
        sspm_tbuf.tbuf_l[i] = readl(SSPM_TBUFL);
        sspm_tbuf.tbuf_h[i] = readl(SSPM_TBUFH);
    }
}


static void save_sspm_xfile(CALLBACK dev_write)
{
    paddr_t sspm_addr_pa = 0;
    u64 reserved_size;

    int ret;
    void *sspm_addr_va = NULL;
    unsigned int ofs = 0;
    unsigned int len = 0;

    ret = mrdump_get_reserved_info("SSPM-reserved", &sspm_addr_pa, &reserved_size);
    if (ret < 0) {
        dprintf(CRITICAL, "sspm err: get reserved in fail\n");
        return;
    }

    if (sspm_addr_pa == 0) {
        dprintf(CRITICAL, "sspm mblock allocation failed\n");
        return;
    }
    ret = vmm_alloc_physical(vmm_get_kernel_aspace(),
                        "vm-SSPM-reserved",
                        SSPM_MEM_SIZE,
                        &sspm_addr_va,
                        PAGE_SIZE_SHIFT,
                        (paddr_t)sspm_addr_pa,
                        0,
                        ARCH_MMU_FLAG_CACHED);

    if (ret < 0) {
        dprintf(CRITICAL, "sspm vmm_alloc_physical failed, err: %d\n", ret);
        vmm_free_region(vmm_get_kernel_aspace(), (vaddr_t)sspm_addr_va);
        return;
    }
    ofs = readl(sspm_addr_va + SSPM_XFILE_ADDR_OFS);
    len = readl(sspm_addr_va + SSPM_XFILE_SIZE_OFS);
    LTRACEF("xfile_ofs: 0x%x, len:0x%x\n", ofs, len);

    if (len > 0) {
        if (!dev_write(vaddr_to_paddr(sspm_addr_va + ofs), (unsigned long)len))
            dprintf(CRITICAL, "[SSPM] AEE EXPDB xifle fail\n");
    } else {
        dprintf(CRITICAL, "[SSPM] AEE EXPDB without xfile\n");
    }
}
AEE_EXPDB_INIT_HOOK(SYS_SSPM_XFILE, AEE_LKDUMP_SSPM_XFILE_SZ, save_sspm_xfile);

#if SSPM_FULLY_COREDUMP
static void save_sspm_last_log(CALLBACK dev_write)
{
    unsigned long addr;
    unsigned int len;

    addr = SSPM_SRAM_BASE + readl(SSPM_SRAM_BASE + SSPM_LOG_OFFSET);
    len = SSPM_LOG_SZ;

    LTRACEF("[SSPM]log buf@0x%lx, len is %d bytes\n", addr, len);

    if (!dev_write(vaddr_to_paddr(addr), (unsigned long)len))
        dprintf(CRITICAL, "[SSPM]last log dump fail\n");
}
#else
static void save_sspm_last_log(CALLBACK dev_write)
{
    unsigned int addr = readl(SSPM_LASTK_ADDR);
    unsigned int len = readl(SSPM_LASTK_SZ);
    int retry = SSPM_COREDUP_RETRY;

    LTRACEF("[SSPM] buf@0x%x, len is %d bytes\n", addr, len);

    do {
        addr = readl(SSPM_LASTK_ADDR);
        len = readl(SSPM_LASTK_SZ);
        LTRACEF("[SSPM]log buf@0x%x, len is %d bytes\n", addr, len);
        if (len) {
            if (!dev_write((uint64_t)addr, (unsigned long)len))
                dprintf(CRITICAL, "[SSPM]last log dump fail\n");

            return;
        } else {
            spin(100); //delay 100us
        }
    } while (--retry);

    dprintf(INFO, "[SSPM] AEE EXPDB without last_log\n");
}
#endif
AEE_EXPDB_INIT_HOOK(SYS_SSPM_LAST_LOG, AEE_LKDUMP_SSPM_LAST_LOG_SZ, save_sspm_last_log);


static void save_sspm_data(CALLBACK dev_write)
{
    char *buf = NULL;
    unsigned int tbufl, tbufh, r, i, j;
    int length = 0;
    char dispatch;
    unsigned int ahb_status, axi_status, ahb_addr_m0, ahb_addr_m1, ahb_addr_m2;

    buf = malloc(AEE_LKDUMP_SSPM_DATA_SZ);
    if (!buf) {
        dprintf(CRITICAL, "[SSPM] Can't alloc buffer\n");
        return;
    }

    ahb_status  = readl(SSPM_AHB_STATUS);
    axi_status  = readl(SSPM_AXI_STATUS);
    ahb_addr_m0 = readl(SSPM_AHB_M0_ADDR);
    ahb_addr_m1 = readl(SSPM_AHB_M1_ADDR);
    ahb_addr_m2 = readl(SSPM_AHB_M2_ADDR);

#define CHK_PENDING(n)  ((ahb_status & (1 << (n))) == 0)
#define CHK_MASTER(n)   ((ahb_status & (1 << (n))) != 0)

    if (CHK_PENDING(17)) {
        /* M0 trans is pending*/
        if (((ahb_addr_m0 >= SSPM_SYSREG_START) && (ahb_addr_m0 < SSPM_SYSREG_END))
            || (ahb_addr_m0 >= SSPM_DRAM_REGION))
            dispatch = 'P'; /* Platform owner */
        else
            dispatch = 'S'; /* SSPM owner */
    } else if ((CHK_PENDING(18) && CHK_MASTER(3)) || CHK_PENDING(19)) {
        /* M1 or M2 trans is pending*/
        dispatch = 'P'; /* Platform owner */
    } else {
        dispatch = 'L'; /* To see SSPM_LAST_LOG */
    }

    memset(buf, 0, AEE_LKDUMP_SSPM_DATA_SZ);
    length += snprintf(buf + length, (AEE_LKDUMP_SSPM_DATA_SZ-1) - length,
                       "AHB_STATUS: 0x%08x\n"
                       "AXI_STATUS: 0x%08x\n"
                       "AHB_M0_ADDR: 0x%x\n"
                       "AHB_M1_ADDR: 0x%x\n"
                       "AHB_M2_ADDR: 0x%x\n"
                       "LastSP: 0x%x\n"
                       "LastLR: 0x%x\n"
                       "LastPC: 0x%x\n"
                       "Dispatch: %c\n",
                       ahb_status, axi_status, ahb_addr_m0, ahb_addr_m1, ahb_addr_m2,
                       readl(SSPM_SP), readl(SSPM_LR), readl(SSPM_PC), dispatch);

#if SSPM_TBUF_SUPPORT
    r = sspm_tbuf.tbuf_wptr;
    length += snprintf(buf + length, (AEE_LKDUMP_SSPM_DATA_SZ-1) - length,
                       "\nTBUF_WPTR=%u\n", r);

    for (i = 0, j = r; i < 16; ++i) {
        j = (j-1) & 0xF;
        tbufl = sspm_tbuf.tbuf_l[i];
        tbufh = sspm_tbuf.tbuf_h[i];
        length += snprintf(buf + length, (AEE_LKDUMP_SSPM_DATA_SZ-1) - length,
                           "%u: TBUF[%u] T=0x%x F=0x%x\n", i, j, tbufh, tbufl);
    }
#endif

    LTRACEF("buf@0x%x, len is %d bytes\n", *buf, length);
    if (!dev_write(vaddr_to_paddr(buf), (unsigned long)length))
        dprintf(CRITICAL, "[SSPM] AEE EXPDB data fail\n");

    free(buf);
}
AEE_EXPDB_INIT_HOOK(SYS_SSPM_DATA, AEE_LKDUMP_SSPM_DATA_SZ, save_sspm_data);


#if SSPM_FULLY_COREDUMP
static void save_sspm_coredump(CALLBACK dev_write)
{
    unsigned long addr = SSPM_SRAM_BASE;
    unsigned int len = SSPM_TCM_SZ;

    LTRACEF("dm buf@0x%lx, len is %d bytes\n", addr, len);
    if (!dev_write(vaddr_to_paddr(addr), (unsigned long)len))
        dprintf(CRITICAL, "[SSPM] AEE EXPDB Coredump(DM) fail\n");

    addr = SSPM_SRAM_BASE + readl(SSPM_SRAM_BASE + SSPM_CTX_OFFSET);
    len = SSPM_CTX_SZ;

    if (readl(addr + SSPM_RSP_OFFSET) == 0)
        writel(readl(SSPM_SP), (addr + SSPM_RSP_OFFSET));

    if (readl(addr + SSPM_RPC_OFFSET) == 0)
        writel(readl(SSPM_PC), (addr + SSPM_RPC_OFFSET));

    if (readl(addr + SSPM_RLR_OFFSET) == 0)
        writel(readl(SSPM_LR), (addr + SSPM_RLR_OFFSET));

    arch_clean_invalidate_cache_range((addr_t)addr, SSPM_CTX_SZ);

    LTRACEF("[SSPM]rm buf@0x%lx, len is %d bytes\n", addr, len);

    if (!dev_write(vaddr_to_paddr(addr), (unsigned long)len))
        dprintf(CRITICAL, "[SSPM] AEE EXPDB Coredump(RM) fail\n");

}

#else
static void save_sspm_coredump(CALLBACK dev_write)
{
    unsigned int addr;
    unsigned int len;
    int retry = SSPM_COREDUP_RETRY;

    if (!readl(SSPM_BACKUP)) {
        dprintf(CRITICAL, "[SSPM] Can't find coredump\n");
        return;
    }

    do {
        addr = readl(SSPM_DM_ADDR);
        len = readl(SSPM_DM_SZ);
        LTRACEF("dm buf@0x%x, len is %d bytes\n", addr, len);
        if (len) {
            if (!dev_write((uint64_t)addr, (unsigned long)len))
                dprintf(CRITICAL, "[SSPM] AEE EXPDB Coredump(DM) fail\n");

            break;
        } else {
            spin(100); //delay 100us
        }
    } while (--retry);

    addr = readl(SSPM_RM_ADDR);
    len = readl(SSPM_RM_SZ);
    LTRACEF("[SSPM]rm buf@0x%x, len is %d bytes\n", addr, len);
    if (!dev_write((uint64_t)addr, (unsigned long)len))
        dprintf(CRITICAL, "[SSPM] AEE EXPDB Coredump(RM) fail\n");

}
#endif
AEE_EXPDB_INIT_HOOK(SYS_SSPM_COREDUMP, AEE_LKDUMP_SSPM_COREDUMP_SZ, save_sspm_coredump);

