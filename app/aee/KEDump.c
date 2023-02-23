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
#include <arch/mmu.h>
#include <arch/ops.h>
#include <assert.h>
#include <debug.h>
#include <err.h>
#include <errno.h>
#include <kernel/vm.h>
#include <lib/bio.h>
#include <lib/cksum.h>
#include <lib/zlib.h>
#include <lk/init.h>
#include <platform/addressmap.h>
#include <platform/aee_common.h>
#include <platform/log_store_lk.h>
#include <platform/mboot_expdb.h>
#include <platform/mboot_params.h>
#include <platform/mrdump_expdb.h>
#include <platform/wdt.h>
#include <platform_mtk.h>
#if !IS_64BIT
#include <platform/mmu_lpae.h>
#endif
#include <platform/mrdump.h>
#include <rtc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "KEHeader.h"
#include "mrdump_elf.h"
#include "mrdump_private.h"

/**************************/
/* ----------------   */
/* MBOOT_PARAMS_DRAM_ADDR  */
/* (1M align)         */
/* +MBOOT_PARAMS_DRAM_SIZE */
/*            */
/* ----------------   */
/* +0xe0000       */
/*            */
/* ----------------   */
/* KE_RESERVED_MEM_ADDR   */
/*            */
/* ----------------   */
/* RAMDISK_LOAD_ADDR      */
/**************************/

static void *mrdump_mini_header_vaddr;

struct elfhdr {
    void *start;
    unsigned int e_machine;
    unsigned int e_phoff;
    unsigned int e_phnum;
};

static struct kedump_crc kc;

static unsigned int last_dump_step;

#define elf_note    elf32_note
#define PHDR_PTR(ehdr, phdr, mem)       \
    (ehdr->e_machine == EM_ARM ? \
        ((struct elf32_phdr *)phdr)->mem : ((struct elf64_phdr *)phdr)->mem)

#define PHDR_TYPE(ehdr, phdr) PHDR_PTR(ehdr, phdr, p_type)
#define PHDR_VADDR(ehdr, phdr) PHDR_PTR(ehdr, phdr, p_vaddr)
#define PHDR_ADDR(ehdr, phdr) PHDR_PTR(ehdr, phdr, p_paddr)
#define PHDR_SIZE(ehdr, phdr) PHDR_PTR(ehdr, phdr, p_filesz)
#define PHDR_MEMSZ(ehdr, phdr) PHDR_PTR(ehdr, phdr, p_memsz)
#define PHDR_FLAGS(ehdr, phdr) PHDR_PTR(ehdr, phdr, p_flags)
#define PHDR_ALIGN(ehdr, phdr) PHDR_PTR(ehdr, phdr, p_align)
#define PHDR_OFF(ehdr, phdr) PHDR_PTR(ehdr, phdr, p_offset)
#define PHDR_INDEX(ehdr, i)         \
    (ehdr->e_machine == EM_ARM ? \
        ehdr->start + ehdr->e_phoff + sizeof(struct elf32_phdr) * i : \
        ehdr->start + ehdr->e_phoff + sizeof(struct elf64_phdr) * i)

static struct mrdump_control_block *g_mcb;

struct ipanic_header panic_header;
static unsigned long long header_off;

static void kedump_print_time(void)
{
    struct rtc_time tm;

    rtc_get_time(&tm);
    LOG("kedump: current time: [%d/%d/%d %d:%d:%d]\n",
        tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
}

static bool kedump_is_mcb_valid(struct mrdump_control_block *mcb)
{
    if (mcb == NULL) {
        LOG("%s: mrdump_cb is NULL\n", __func__);
        return false;
    }

    if (!memcmp(mcb->sig, MRDUMP_GO_DUMP, 8) ||
            !memcmp(mcb->sig, "XRDUMP", 6)) {
        return true;
    } else {
        return false;
    }
}

static uint64_t _get_mpt(struct mrdump_control_block *mcb)
{
    struct mrdump_machdesc *mmp;

    if (mcb == NULL) {
        LOG("%s: mrdump_cb is NULL\n", __func__);
        return 0;
    }

    if (!kedump_is_mcb_valid(mcb)) {
        LOG("mrdump_cb: unexpected sig error:0x%llx in %s\n", *(uint64_t *)mcb, __func__);
        return 0;
    }
    mmp = &mcb->machdesc;
    return mmp->master_page_table;
}

uint64_t get_mpt(void)
{
    return _get_mpt(g_mcb);
}


static void get_vmalloc_range(struct mrdump_control_block *mcb,
        uint64_t *vmalloc_start, uint64_t *vmalloc_end)
{
    struct mrdump_machdesc *mmp;

    if (mcb == NULL || vmalloc_start == NULL || vmalloc_end == NULL)
        return;

    if (kedump_is_mcb_valid(mcb)) {
        mmp = &mcb->machdesc;
        *vmalloc_start = mmp->vmalloc_start;
        *vmalloc_end = mmp->vmalloc_end;
    } else {
        *vmalloc_start = 0;
        *vmalloc_end = 0;
    }
}

static uint64_t dram_start;
static uint64_t dram_end;

static void kedump_get_dram_range(void)
{
    dram_start = mblock_get_memory_start();
    dram_end = dram_start + mblock_get_memory_size();
    LOG("kedump dram range 0x%llx - 0x%llx\n", dram_start, dram_end);
}

static bool is_dram_address(uint64_t addr)
{
    if (!addr)
        return false;

    if (!dram_start || !dram_end) {
        LOG("kedump invalid dram range 0x%llx - 0x%llx\n", dram_start, dram_end);
        return false;
    }

    return (addr >= dram_start && addr < dram_end);
}

static int is_arm_32bit(struct mrdump_control_block *mcb, uint64_t vaddr)
{
    bool isret = (vaddr <= 0xffffffffUL) ? true : false;
    struct mrdump_machdesc *mmp;

    if (mcb == NULL)
        return isret;

    if (kedump_is_mcb_valid(mcb)) {
        mmp = &mcb->machdesc;
        return (mmp->page_offset > 0 && mmp->page_offset <= 0xffffffffUL);
    } else {
        return isret;
    }
}

static struct elfhdr kehdr;
static struct elfhdr *ehdr;
static struct elfhdr *kedump_elf_hdr(void)
{
    char *ei;

    if (ehdr)
        return ehdr;

    kehdr.start = mrdump_mini_header_vaddr;
    LOG("kedump: KEHeader %p\n", kehdr.start);
    if (kehdr.start) {
        ei = (char *)kehdr.start; //elf_hdr.e_ident
        LOG("kedump: read header 0x%p[0x%x%x%x%x]\n", ei, ei[0], ei[1], ei[2], ei[3]);
        /* valid elf header */
        if (ei[0] == 0x7f && ei[1] == 'E' && ei[2] == 'L' && ei[3] == 'F') {
            kehdr.e_machine = ((struct elf32_hdr *)(kehdr.start))->e_machine;
            if (kehdr.e_machine == EM_ARM) {
                kehdr.e_phnum = ((struct elf32_hdr *)(kehdr.start))->e_phnum;
                kehdr.e_phoff = ((struct elf32_hdr *)(kehdr.start))->e_phoff;
                ehdr = &kehdr;
            } else if (kehdr.e_machine == EM_AARCH64) {
                kehdr.e_phnum = ((struct elf64_hdr *)(kehdr.start))->e_phnum;
                kehdr.e_phoff = ((struct elf64_hdr *)(kehdr.start))->e_phoff;
                ehdr = &kehdr;
            }
        }
        if (ehdr == NULL)
            LOG("kedump: invalid header[0x%x%x%x%x]\n", ei[0], ei[1], ei[2], ei[3]);
    }
    LOG("kedump: mach[0x%x], phnum[0x%x], phoff[0x%x]\n",
            kehdr.e_machine, kehdr.e_phnum, kehdr.e_phoff);
    return ehdr;
}

uint64_t kedump_mem_read(uint64_t paddr, unsigned long sz, void *buf)
{
    uint64_t size_read = 0;
    uint64_t start;
    void *vaddr = NULL;
    vaddr_t memsrc;
    unsigned long rest = sz;
    unsigned long buf_offset = 0;
    int map_ok;

    if (!is_dram_address(paddr) || !is_dram_address(paddr + sz - 1)) {
        LOG("kedump: illegal pa:0x%llx(sz:0x%lx)\n", paddr, sz);
        return 0;
    }

    start = ROUNDDOWN((uint64_t)paddr, (uint64_t)AEE_MAP_SIZE);

    /* minirdump: minirdump will dump memory in DRAM, we must allocate it first */
#if !IS_64BIT
    if (paddr >= SZ_4G) {
        /* we can only map 16M at one time for 32bit & pa>4G case */
        map_ok = vmm_alloc_physical(vmm_get_kernel_aspace(),
                "ke_mem_read", AEE_MAP_SIZE,
                &vaddr, AEE_MAP_ALIGN,
                0, VMM_FLAG_NO_MAP_PA, ARCH_MMU_FLAG_CACHED);
        if (map_ok != NO_ERROR) {
            LOG("vmm_alloc_physical map beyond 4G fail\n");
            return map_ok;
        }
        vmm_aspace_t *aspace = vaddr_to_aspace(vaddr);

        map_ok = arch_mmu_map_large(&aspace->arch_aspace, (vaddr_t)vaddr,
                                    start, AEE_MAP_SIZE / PAGE_SIZE,
                                    ARCH_MMU_FLAG_CACHED);
        if (map_ok < 0) {
            LOG("arch_mmu_map_large fail @0x%llx\n", paddr);
            vmm_free_region(vmm_get_kernel_aspace(), (vaddr_t)vaddr);
            return map_ok;
        }
    } else
#endif
    {
        map_ok = vmm_alloc_physical(vmm_get_kernel_aspace(),
                "ke_mem_read", AEE_MAP_SIZE,
                &vaddr, AEE_MAP_ALIGN,
                (paddr_t)start, 0, ARCH_MMU_FLAG_CACHED);
        if (map_ok != NO_ERROR) {
            LOG("kedump: map error\n");
            return map_ok;
        }
    }

    memsrc = (vaddr_t)(vaddr + (uint32_t)(paddr - start));

    while (rest > 0) {
        unsigned long read_sz;

        if (rest <= PAGE_SIZE)
            read_sz = rest;
        else
            read_sz = PAGE_SIZE;

        memcpy(buf + buf_offset, (void *)(memsrc + (sz - rest)), read_sz);

        size_read += read_sz;
        buf_offset += read_sz;
        rest -= read_sz;
    }
    vmm_free_region(vmm_get_kernel_aspace(), (vaddr_t)vaddr);
    return size_read;
}

/**************** mini rdump part ****************/
static void fill_prstatus32(struct elf32_arm_prstatus *prstatus, void *regs,
              u32 p, unsigned long pid)
{
    if (!prstatus)
        return;

    *(struct pt_regs_32 *)(&prstatus->pr_reg) = *(struct pt_regs_32 *)regs;
    prstatus->pr_pid = pid;
    prstatus->pr_ppid = g_mcb->machdesc.nr_cpus;
    if (p)
        prstatus->pr_sigpend = p;
}

static void fill_prstatus64(struct elf_arm64_prstatus64 *prstatus, void *regs,
              u64 p, unsigned long pid)
{
    if (!prstatus)
        return;

    *(struct pt_regs_64 *)(&prstatus->pr_reg) = *(struct pt_regs_64 *)regs;
    prstatus->pr_pid = pid;
    prstatus->pr_ppid = g_mcb->machdesc.nr_cpus;
    if (p)
        prstatus->pr_sigpend = p;
}

static void fill_note(struct elf_note *note, const char *name,
        int type, unsigned int sz, unsigned int namesz)
{
    char *n_name;

    if (!note)
        return;

    n_name = (char *)note + sizeof(struct elf_note);
    note->n_namesz = namesz;
    note->n_type = type;
    note->n_descsz = sz;
    snprintf(n_name, namesz, "%s", name);
}

static void fill_note_S(struct elf_note *note, const char *name, int type,
        unsigned int sz)
{
    fill_note(note, name, type, sz, NOTE_NAME_SHORT);
}

/*
 * get the address of prstatus in mrdump_mini_elf_header
 *
 * struct mrdump_mini_elf_header {
 *    struct elfhdr            ehdr;
 *    struct elf_phdr            phdrs[MRDUMP_MINI_NR_SECTION];
 *    struct mrdump_mini_elf_psinfo    psinfo;
 *    struct mrdump_mini_elf_prstatus    prstatus[AEE_MTK_CPU_NUMS];
 *    struct mrdump_mini_elf_note    misc[MRDUMP_MINI_NR_MISC];
 * };
 */
static void *get_mini_prstatus_addr(void)
{
    void *phdr;
    struct elf_note *note;
    uint32_t sz;

    phdr = PHDR_INDEX(ehdr, 0);
    note = (struct elf_note *)(ehdr->start + PHDR_OFF(ehdr, phdr)); /* mini psinfo */
    sz = sizeof(struct elf_note) + note->n_namesz + note->n_descsz; /* size of psinfo */

    return (void *)(ehdr->start + PHDR_OFF(ehdr, phdr) + sz);
}

/*
 * struct mrdump_mini_elf_prstatus {
 *    struct elf_note        note;
 *    char            name[NOTE_NAME_SHORT];
 *    struct elf_prstatus    data;
 * };
 */
static void *get_prstatus_idx_note_addr(int idx)
{
    void *pmini_prstatus = get_mini_prstatus_addr();
    struct elf_note *note;
    uint32_t sz;

    if (idx < 0)
        return NULL;

    note = (struct elf_note *)pmini_prstatus;
    sz = sizeof(struct elf_note) + note->n_namesz + note->n_descsz; /* size of prstatus[0] */

    return (void *)(pmini_prstatus + sz * idx); /* target: &prstatus[cpu].data */
}

static char *get_prstatus_idx_name_addr(int idx)
{
    if (idx < 0)
        return NULL;

    return (char *)(get_prstatus_idx_note_addr(idx) +
            sizeof(struct elf_note));
}

static void *get_prstatus_idx_data_addr(int idx)
{
    void *pmini_prstatus = get_mini_prstatus_addr();
    struct elf_note *note;
    uint32_t sz;
    uint32_t prstatus_off;

    if (idx < 0)
        return NULL;

    note = (struct elf_note *)pmini_prstatus;
    sz = sizeof(struct elf_note) + note->n_namesz + note->n_descsz; /* size of prstatus[0] */
    prstatus_off = sizeof(struct elf_note) + note->n_namesz; /* data offset in prstatus[0] */

    return (void *)(pmini_prstatus + sz * idx + prstatus_off); /* target: &prstatus[cpu].data */
}

static uint32_t get_prstatus_data_size(void)
{
    void *pmini_prstatus = get_mini_prstatus_addr();
    struct elf_note *note = (struct elf_note *)pmini_prstatus;

    return note->n_descsz;
}

static int minir_cpu_regs(uint32_t cpu, uint32_t idx)
{
    char name[NOTE_NAME_SHORT];
    void *pprstatus;
    void *regs;
    u64 tsk = 0;
    u32 pid = cpu + 100;
    u32 sp32 = 0;
    int err = 0;

    if (cpu >= AEE_MTK_CPU_NUMS) {
        LOG("aee invalid cpu %d\n", cpu);
        return -1;
    }

    if (!ehdr) {
        LOG("aee invalid ehdr\n");
        return -1;
    }
    err = snprintf(name, NOTE_NAME_SHORT - 1, "core%d", cpu);
    ASSERT(err > 0);

    pprstatus = get_prstatus_idx_data_addr(idx);
    if (ehdr->e_machine == EM_ARM) {
        regs = (void *)&(g_mcb->crash_record.cpu_regs[cpu].arm32_reg.arm32_regs);
        sp32 = g_mcb->crash_record.cpu_regs[cpu].arm32_reg.arm32_regs[13];
        if (sp32 > STACK_SIZE_32)
            tsk = ROUNDDOWN(sp32, STACK_SIZE_32) + TASK_OFF_IN_TI_32;
        fill_prstatus32((struct elf32_arm_prstatus *)pprstatus, regs,
                (u32)tsk, pid);
    } else {
        regs = (void *)&(g_mcb->crash_record.cpu_regs[cpu].arm64_reg.arm64_regs);
        tsk = g_mcb->crash_record.cpu_regs[cpu].arm64_reg.arm64_creg.sp_el[0];
        fill_prstatus64((struct elf_arm64_prstatus64 *)pprstatus, regs,
                tsk, pid);
    }
    fill_note_S((struct elf_note *)get_prstatus_idx_note_addr(idx), name,
            NT_PRSTATUS, get_prstatus_data_size());
    return 0;
}

static void fill_elf_load_phdr32(struct elf32_phdr *phdr, int sz, uint32_t vaddr)
{
    if (!phdr)
        return;

    phdr->p_type = PT_LOAD;
    phdr->p_vaddr = vaddr;
    phdr->p_paddr = 0;
    phdr->p_filesz = sz;
    phdr->p_memsz = 0;
    phdr->p_flags = 0;
    phdr->p_align = 0;
}

static void fill_elf_load_phdr64(struct elf64_phdr *phdr, int sz, uint64_t vaddr)
{
    if (!phdr)
        return;

    phdr->p_type = PT_LOAD;
    phdr->p_vaddr = vaddr;
    phdr->p_paddr = 0;
    phdr->p_filesz = sz;
    phdr->p_memsz = 0;
    phdr->p_flags = 0;
    phdr->p_align = 0;
}

static void fill_elf_note_phdr32(struct elf32_phdr *phdr, int sz, uint32_t offset)
{
    if (!phdr)
        return;

    phdr->p_type = PT_NOTE;
    phdr->p_flags = 0;
    phdr->p_offset = offset;
    phdr->p_vaddr = 0;
    phdr->p_paddr = 0;
    phdr->p_filesz = sz;
    phdr->p_memsz = 0;
    phdr->p_align = 0;
}

static void fill_elf_note_phdr64(struct elf64_phdr *phdr, int sz, uint64_t offset)
{
    if (!phdr)
        return;

    phdr->p_type = PT_NOTE;
    phdr->p_flags = 0;
    phdr->p_offset = offset;
    phdr->p_vaddr = 0;
    phdr->p_paddr = 0;
    phdr->p_filesz = sz;
    phdr->p_memsz = 0;
    phdr->p_align = 0;
}

static void mrdump_mini_add_entry_ext32(uint32_t start, uint32_t end)
{
    uint32_t laddr, haddr;
    struct elf32_phdr *phdr;
    int i;

    for (i = 0; i < MRDUMP_MINI_NR_SECTION; i++) {
        phdr = PHDR_INDEX(ehdr, i);
        if (phdr->p_type == PT_NULL)
            break;
        if (phdr->p_type != PT_LOAD)
            continue;
        laddr = phdr->p_vaddr;
        haddr = laddr + phdr->p_filesz;
        if (start >= laddr && end <= haddr)
            return;
        if (start >= haddr || end <= laddr)
            continue;
        if (laddr < start)
            start = laddr;

        if (haddr > end)
            end = haddr;
        break;
    }
    if (i < MRDUMP_MINI_NR_SECTION)
        fill_elf_load_phdr32(phdr, end - start, start);
    else
        LOG("mrdump: MINI_NR_SECTION overflow!\n");
}


static uint64_t mrdump_mini_untagged_addr(uint64_t vaddr)
{
    return vaddr | (0xffUL << 56);
}

static void mrdump_mini_add_entry_ext64(uint64_t start, uint64_t end)
{
    uint64_t laddr, haddr;
    struct elf64_phdr *phdr;
    int i;

    for (i = 0; i < MRDUMP_MINI_NR_SECTION; i++) {
        phdr = PHDR_INDEX(ehdr, i);
        if (phdr->p_type == PT_NULL)
            break;
        if (phdr->p_type != PT_LOAD)
            continue;
        laddr = phdr->p_vaddr;
        haddr = laddr + phdr->p_filesz;
        if (start >= laddr && end <= haddr)
            return;
        if (start >= haddr || end <= laddr)
            continue;
        if (laddr < start)
            start = laddr;

        if (haddr > end)
            end = haddr;
        break;
    }

    if (i < MRDUMP_MINI_NR_SECTION)
        fill_elf_load_phdr64(phdr, end - start, start);
    else
        LOG("mrdump: MINI_NR_SECTION overflow!\n");
}

static bool mrdump_mini_is_vaddr_in_dram(uint64_t vaddr)
{
    if (ehdr->e_machine == EM_ARM) {
        if (is_dram_address(v2p_32(vaddr)))
            return true;
        else
            return false;
    } else if (ehdr->e_machine == EM_AARCH64) {
        if (is_dram_address(v2p_64(vaddr)))
            return true;
        else
            return false;
    }

    LOG("mrdump: unknown machine!\n");
    return false;
}

static uint64_t mrdump_mini_get_start_addr(uint64_t addr, unsigned long size)
{
    uint64_t start = ROUNDDOWN(addr, PAGE_SIZE);
    uint64_t min_start = start - ROUNDUP(size / 2, PAGE_SIZE);

    if (ehdr->e_machine == EM_AARCH64 &&
        g_mcb->machdesc.vmalloc_start > min_start &&
        g_mcb->machdesc.vmalloc_start < addr) {
        min_start = g_mcb->machdesc.vmalloc_start;
    }

    while (start > min_start) {
        if (start - PAGE_SIZE < min_start)
            break;

        if (!mrdump_mini_is_vaddr_in_dram(start - PAGE_SIZE))
            break;

        start -= PAGE_SIZE;
    }

    return start;
}

static uint64_t mrdump_mini_get_end_addr(uint64_t addr, unsigned long size)
{
    uint64_t end_max = ROUNDUP(addr + size / 2, PAGE_SIZE);
    uint64_t end = ROUNDUP(addr, PAGE_SIZE);

    /* the end address is not counted: [start, end) */
    while (end < end_max) {
        if (!mrdump_mini_is_vaddr_in_dram(end))
            break;

        if (end + PAGE_SIZE > end_max)
            break;

        end += PAGE_SIZE;
    }

    return end;
}

static void mrdump_mini_add_entry(uint64_t addr, unsigned long size)
{
    uint64_t start, end;

    if (ehdr->e_machine == EM_AARCH64)
        addr = mrdump_mini_untagged_addr(addr);

    if (!mrdump_mini_is_vaddr_in_dram(addr))
        return;

    start = mrdump_mini_get_start_addr(addr, size);
    end = mrdump_mini_get_end_addr(addr, size);

    if (ehdr->e_machine == EM_ARM)
        mrdump_mini_add_entry_ext32((uint32_t)start, (uint32_t)end);
    else
        mrdump_mini_add_entry_ext64(start, end);
}

static void mrdump_mini_walk_stack_32(u32 sp)
{
    u32 top = ROUNDUP(sp, STACK_SIZE_32);
    u32 *p;
    void *stack_buf;
    u32 paddr;

    if (top < sp) {
        LOG("invalid sp 0x%x\n", sp);
        return;
    }

    paddr = v2p_32(sp);
    if (!paddr)
        return;

    stack_buf = malloc(STACK_SIZE_32);
    if (!stack_buf)
        return;
    memset(stack_buf, 0, STACK_SIZE_32);
    kedump_mem_read(paddr, top - sp, stack_buf);
    for (p = (u32 *)stack_buf; (u32)p < (u32)(stack_buf + top - sp); p++)
        mrdump_mini_add_entry(*p, MRDUMP_MINI_SECTION_SIZE);
    free(stack_buf);
}

static void mrdump_mini_walk_stack_64(u64 sp)
{
    u64 top;
    u64 *p;
    void *stack_buf;
    u64 paddr;

    top = ROUNDUP(sp, STACK_SIZE_64);

    paddr = v2p_64(sp);
    if (!paddr) {
        LOG("convert sp failed 0x%llx\n", sp);
        return;
    }

    stack_buf = malloc(STACK_SIZE_64);
    if (!stack_buf)
        return;
    memset(stack_buf, 0, STACK_SIZE_64);
    kedump_mem_read(paddr, top - sp, stack_buf);
    for (p = (u64 *)stack_buf; (u32)p < (u32)(stack_buf + top - sp); p++)
        mrdump_mini_add_entry(*p, MRDUMP_MINI_SECTION_SIZE);
    free(stack_buf);
}

static void mrdump_mini_add_loads32(void)
{
    int i, id;
    struct elf32_arm_prstatus *prstatus;
    unsigned int exp_type = 0;
    u32 loops = g_mcb->machdesc.nr_cpus;

    mboot_params_get_exp_type(&exp_type);

    if (!ehdr)
        return;

    if (loops > AEE_MTK_CPU_NUMS)
        loops = AEE_MTK_CPU_NUMS;
    for (id = 0; id < loops; id++) {
        if (!strncmp(get_prstatus_idx_name_addr(id), "NA", 2))
            continue;
        prstatus = get_prstatus_idx_data_addr(id);

        for (i = 0; i < ELF_ARM_NGREGS; i++)
            mrdump_mini_add_entry(prstatus->pr_reg[i],
                    MRDUMP_MINI_SECTION_SIZE);
        mrdump_mini_add_entry(prstatus->pr_sigpend, MRDUMP_MINI_SECTION_SIZE);
        if (id == 0 && exp_type == AEE_EXP_TYPE_KE)
            mrdump_mini_walk_stack_32(prstatus->pr_reg[13]);
    }
}

static void mrdump_mini_add_loads64(void)
{
    int i, id;
    struct elf_arm64_prstatus64 *prstatus;
    unsigned int exp_type = 0;
    u32 loops = g_mcb->machdesc.nr_cpus;

    if (!ehdr)
        return;

    mboot_params_get_exp_type(&exp_type);

    if (loops > AEE_MTK_CPU_NUMS)
        loops = AEE_MTK_CPU_NUMS;
    for (id = 0; id < loops; id++) {
        if (!strncmp(get_prstatus_idx_name_addr(id), "NA", 2))
            continue;
        prstatus = get_prstatus_idx_data_addr(id);

        for (i = 0; i < ELF_ARM64_NGREGS; i++)
            mrdump_mini_add_entry(prstatus->pr_reg[i],
                    MRDUMP_MINI_SECTION_SIZE);
        mrdump_mini_add_entry(prstatus->pr_sigpend, MRDUMP_MINI_SECTION_SIZE);
        if (id == 0 && exp_type == AEE_EXP_TYPE_KE)
            mrdump_mini_walk_stack_64(prstatus->pr_reg[31]);
    }
}

static void mrdump_mini_add_loads(void)
{
    if (!ehdr) {
        LOG("aee invalid ehdr\n");
        return;
    }

    if (ehdr->e_machine == EM_ARM)
        mrdump_mini_add_loads32();
    else
        mrdump_mini_add_loads64();
}

static void mrdump_mini_add_misc_load(void *m_data)
{
    uint64_t start;
    uint64_t end;

    if (ehdr->e_machine == EM_ARM) {
        start = ((struct mrdump_mini_misc_data32 *)m_data)->vaddr;
        end = start + ((struct mrdump_mini_misc_data32 *)m_data)->size;
        mrdump_mini_add_entry_ext32((uint32_t)start, (uint32_t)end);
    } else {
        start = ((struct mrdump_mini_misc_data64 *)m_data)->vaddr;
        start = mrdump_mini_untagged_addr(start);
        end = start + ((struct mrdump_mini_misc_data64 *)m_data)->size;
        end = mrdump_mini_untagged_addr(end);
        mrdump_mini_add_entry_ext64(start, end);
    }
}

static void mrdump_mini_add_misc_loads(void)
{
    struct elfhdr *ehdr;
    unsigned long sz_misc;
    void *phdr_misc, *phdr_pspr;
    struct elf_note *misc, *miscs;
    char *m_name;
    void *m_data;
    unsigned int i;

    ehdr = kedump_elf_hdr();
    if (!ehdr)
        return;

    phdr_pspr = PHDR_INDEX(ehdr, 0);
    phdr_misc = PHDR_INDEX(ehdr, 1);
    if (PHDR_OFF(ehdr, phdr_misc) != PHDR_OFF(ehdr, phdr_pspr) + PHDR_SIZE(ehdr, phdr_pspr)) {
        LOG("first 2 ehdr invalid\n");
        return;
    }

    miscs = (struct elf_note *)(ehdr->start + PHDR_OFF(ehdr, phdr_misc));
    sz_misc = sizeof(struct elf_note) + miscs->n_namesz + miscs->n_descsz;
    for (i = 0; i < (PHDR_SIZE(ehdr, phdr_misc)) / sz_misc; i++) {
        misc = (struct elf_note *)((void *)miscs + sz_misc * i);
        m_name = (char *)misc + sizeof(struct elf_note);
        if (m_name[0] == 'N' && m_name[1] == 'A' && m_name[2] == '\0')
            break;
        if (misc->n_type != NT_IPANIC_MISC) {
            LOG("misc invalid type: 0x%x\n", misc->n_type);
            break;
        }
        m_data = (void *)misc + sizeof(struct elf_note) + misc->n_namesz;
        if (strcmp(m_name, MRDUMP_MINI_MISC_LOAD) == 0)
            mrdump_mini_add_misc_load(m_data);
    }
}


static struct mrdump_mini_auxv auxv;
static struct mrdump_mini_pac_mask pac;

static void init_pac_note(void)
{
    /* init pac */
    pac.note.n_namesz = PAC_NOTE_NAME_SZ;
    pac.note.n_descsz = sizeof(struct elf_arm64_pac_mask);
    pac.note.n_type = NT_ARM_PAC_MASK;
    strlcpy(&pac.name, PAC_NOTE_NAME, PAC_NOTE_NAME_SZ);
    pac.pac_mask64.pauth_dmask = g_mcb->machdesc.kernel_pac_mask;
    pac.pac_mask64.pauth_cmask = g_mcb->machdesc.kernel_pac_mask;

    /* init auxv */
    auxv.note.n_namesz = AUXV_NOTE_NAME_SZ;
    auxv.note.n_descsz = sizeof(struct elf_arm64_auxv);
    auxv.note.n_type = NT_AUXV;
    strlcpy(&auxv.name, AUXV_NOTE_NAME, AUXV_NOTE_NAME_SZ);
    auxv.auxv64.saved_auxv[0] = AT_HWCAP;
    auxv.auxv64.saved_auxv[1] = 0xffffffff;
}

static void mrdump_mini_gen_elf_header(void *elfhd, uint32_t sz_header)
{

    void *phdr;
    void *ptr_elf = elfhd;
    uint32_t sz_hdr, sz_psinfo, sz_prstatus = 0, sz_misc, sz_tmp;
    struct elf_note *note;
    int id;
    uint32_t nr_cpus = g_mcb->machdesc.nr_cpus;

    if (nr_cpus > AEE_MTK_CPU_NUMS)
        nr_cpus = AEE_MTK_CPU_NUMS;

    /* copy ehdr and phdrs array of mrdump_mini_elf_header to elfhd */
    phdr = PHDR_INDEX(ehdr, 0);
    sz_hdr = PHDR_OFF(ehdr, phdr); /* sizeof ehdr + sizeof phdrs array*/
    memcpy(ptr_elf, ehdr->start, sz_hdr);
    ptr_elf += sz_hdr;

    /*copy psinfo of mrdump_mini_elf_header to elfhd */
    note = (struct elf_note *)(ehdr->start + sz_hdr); /* mini psinfo */
    sz_psinfo = sizeof(struct elf_note) + note->n_namesz + note->n_descsz;
    memcpy(ptr_elf, (void *)note, sz_psinfo);
    ptr_elf += sz_psinfo;

    /* make prstatus */
    if (g_mcb->machdesc.kernel_pac_mask)
        init_pac_note();
    note = (struct elf_note *)get_mini_prstatus_addr();
    sz_tmp = sizeof(struct elf_note) + note->n_namesz + note->n_descsz; /* size of prstatus[0] */
    for (id = 0; id < nr_cpus; id++) {
        if (!strncmp(get_prstatus_idx_name_addr(id), "NA", 2))
            continue;
        memcpy(ptr_elf, get_prstatus_idx_note_addr(id), sz_tmp);
        ptr_elf += sz_tmp;
        sz_prstatus += sz_tmp;
        if (g_mcb->machdesc.kernel_pac_mask) {
            memcpy(ptr_elf, &pac, sizeof(pac));
            ptr_elf += sizeof(pac);
            sz_prstatus += sizeof(pac);
        }
    }
    if (g_mcb->machdesc.kernel_pac_mask) {
        memcpy(ptr_elf, &auxv, sizeof(auxv));
        ptr_elf += sizeof(auxv);
        sz_prstatus += sizeof(auxv);
    }

    /* update phdrs[0] of elfhd */
    if (ehdr->e_machine == EM_ARM)
        fill_elf_note_phdr32((struct elf32_phdr *)(elfhd + ehdr->e_phoff),
                             sz_psinfo + sz_prstatus, sz_hdr);
    else
        fill_elf_note_phdr64((struct elf64_phdr *)(elfhd + ehdr->e_phoff),
                             sz_psinfo + sz_prstatus, sz_hdr);

    /* copy misc of mrdump_mini_elf_header to elfhd */
    phdr = PHDR_INDEX(ehdr, 1);
    sz_tmp = PHDR_SIZE(ehdr, phdr); /* sizeof misc array of mrdump_mini_elf_header */
    note = (struct elf_note *)(ehdr->start + PHDR_OFF(ehdr, phdr));
    sz_misc = sizeof(struct elf_note) + NOTE_NAME_LONG;
    sz_misc += ehdr->e_machine == EM_ARM ?
            sizeof(struct mrdump_mini_misc_data32) : sizeof(struct mrdump_mini_misc_data64);
    sz_misc = sz_misc * MRDUMP_MINI_NR_MISC; /* sizeof misc array of mrdump_mini_elf_header too*/
    if (sz_tmp != sz_misc)
        LOG("kedump: WARN: invalid dram data [%d/%d] found in misc pt_note\n", sz_tmp, sz_misc);
    memcpy(ptr_elf, (void *)note, sz_misc);
    ptr_elf += sz_misc;

    /* update phdrs[1] of elfhd */
    if (ehdr->e_machine == EM_ARM)
        fill_elf_note_phdr32(
                (struct elf32_phdr *)(elfhd + ehdr->e_phoff + sizeof(struct elf32_phdr)),
                sz_misc,
                sz_hdr + sz_psinfo + sz_prstatus);
    else
        fill_elf_note_phdr64(
                (struct elf64_phdr *)(elfhd + ehdr->e_phoff + sizeof(struct elf64_phdr)),
                sz_misc,
                sz_hdr + sz_psinfo + sz_prstatus);
}

static void mrdump_mini_load_header(void)
{
    uint32_t cpu;
    uint32_t crash_cpu = g_mcb->crash_record.fault_cpu;
    uint32_t nr_cpus = g_mcb->machdesc.nr_cpus;

    mrdump_mini_add_misc_loads();

    if (nr_cpus > AEE_MTK_CPU_NUMS) {
        LOG("kedump: invalid control block nr_cpus<%d>\n", nr_cpus);
        nr_cpus = AEE_MTK_CPU_NUMS;
    }
    if (crash_cpu >= nr_cpus)
        crash_cpu = 0;

    /* put regs of crash cpu at first slot of mini prstatus */
    minir_cpu_regs(crash_cpu, 0);

    /* append other cpus */
    for (cpu = 0; cpu < crash_cpu; cpu++)
        minir_cpu_regs(cpu, cpu + 1);

    for (cpu = crash_cpu + 1; cpu < nr_cpus; cpu++)
        minir_cpu_regs(cpu, cpu);

    /* fill mini header with PT_LOADs */
    mrdump_mini_add_loads();
}
/**************** mini rdump end ****************/

/* the min offset reserved for the header's size. */
static unsigned long kedump_mrdump_reserved_header_size(
        unsigned int header_sz, unsigned long long expdb_off)
{
    unsigned long long tmp_off;

    if (is_expdb_nand()) {
        /* to get next block start offset */
        tmp_off = ALIGN(expdb_off, panic_header.blksize);

        /* if remaining space not enough for mrump header, extend one block */
        if (header_sz > (tmp_off - expdb_off))
            tmp_off += panic_header.blksize;

        /* make sure the PTLOAD start at beginning of a NAND block */
        return (unsigned long)(tmp_off - expdb_off);
    }

    return ALIGN(header_sz, TRUNK_SZ);
}

static unsigned int kedump_mrdump_header_size(struct elfhdr *ehdr)
{
    void *phdr = PHDR_INDEX(ehdr, 1);

    return PHDR_OFF(ehdr, phdr) + PHDR_SIZE(ehdr, phdr);
}


static unsigned long long kedump_dev_write_vmalloc_range(
        uint64_t vaddr, uint64_t paddr, unsigned long size, uint64_t exp_off)
{
    bool flag;
    unsigned long long lcheckaddr;
    unsigned long long lnew, hnew, hnewtmp;
    unsigned long long pcheckaddr;
    int checkloop = 0;
    unsigned long long size_wrote = 0, size_done = 0;

    /* vaddr & vadd + size are PAGE_SIZE alignment */
    lnew = vaddr;
    hnew = vaddr + size;
    flag = true;

    if (!paddr)
          paddr = v2p_64(lnew);

    while (flag) {
        for (checkloop = 1; checkloop < (int)((hnew - lnew) / PAGE_SIZE); checkloop++) {
            lcheckaddr = lnew + checkloop * (unsigned long long)PAGE_SIZE;
            pcheckaddr = v2p_64(lcheckaddr);
            /* NOTE:
             * convert result should not be 0
             * if the result is 0 it should be the case invalid pfn pa address is recorded in kernel
             */
            if (pcheckaddr == 0 || !is_dram_address(pcheckaddr)) {
                if (pcheckaddr != 0)
                    LOG("kedump: non-dram address:0x%llx\n", pcheckaddr);
                LOG("kedump: expected pa:0x%llx (va:0x%llx)\n",
                        paddr + checkloop * (unsigned long long)PAGE_SIZE, lcheckaddr);
                continue;
            }
            if (pcheckaddr != (paddr + checkloop * (unsigned long long)PAGE_SIZE)) {
                flag = false;
                LOG("kedump: non-cont 0x%llx found(va:0x%llx, pa:0x%llx)\n",
                        paddr + checkloop * (unsigned long long)PAGE_SIZE,
                        lcheckaddr, pcheckaddr);
                break;
            }
        }

        hnewtmp = flag ? hnew : lcheckaddr;
        expdb_flush_trunk_buf();
        expdb_set_offset(exp_off + size_done);
        if (dram_end && paddr > dram_end)
            paddr = 0;
        size_wrote += expdb_write((uint64_t)paddr, (hnewtmp - lnew));
        size_done += hnewtmp - lnew;

        if (!flag) {
            /* found non-adjacent PA */
            lnew = lcheckaddr;
            paddr = pcheckaddr;
            flag = true; /* continue the while loop to dump next non-adjacent block */
        } else {
            flag = false;
        }
    }

    return size_wrote;
}

static unsigned long long kedump_dev_write_va_32(
        uint64_t vaddr, unsigned long size)
{
    unsigned long long paddr;

    paddr = v2p_32(vaddr);
    if (paddr)
        return expdb_write((uint64_t)paddr, size);

    LOG("kedump: 32va to pa failed 0x%llx -> 0x%llx\n", vaddr, paddr);
    return 0;
}

static void kedump_add2hdr(unsigned int size, unsigned int datasize,
                           const char *name);
static unsigned int kedump_mini_rdump(struct elfhdr *ehdr)
{
    void *phdr, *elf_header;
    unsigned long long addr;
    unsigned long long vaddr;
    unsigned long long vmalloc_start, vmalloc_end;
    unsigned long size;
    unsigned int i;
    unsigned int total = 0;
    unsigned long long offset; /* start of mini rdump in expdb */
    unsigned long elfoff; /* mini rdump used size */
    unsigned int sz_header;
    unsigned long long next_file_offset;
    unsigned int loop = ehdr->e_phnum;

    expdb_flush_trunk_buf(); /* start from a new "page" of expdb */
    offset = expdb_get_offset();
    sz_header = kedump_mrdump_header_size(ehdr);
    elfoff = kedump_mrdump_reserved_header_size(sz_header, offset);
    sz_header = elfoff;

    elf_header = malloc(sz_header);
    if (elf_header == NULL) {
        LOG("kedump: malloc failed at %s\n", __func__);
        return 0;
    }
    memset(elf_header, 0x0, sz_header);

    mrdump_mini_load_header();

    if (loop > MRDUMP_MINI_NR_SECTION) {
        LOG("kedump: invalid e_phnum %d\n", loop);
        loop = MRDUMP_MINI_NR_SECTION;
    }

    for (i = 0; i < loop; i++) {
        phdr = PHDR_INDEX(ehdr, i);
        if (PHDR_SIZE(ehdr, phdr) != 0 || PHDR_TYPE(ehdr, phdr) != 0)
            LOG("kedump: %d-PT[%d] %llx@%llx -> %llx(%llx)\n",
                    i, PHDR_TYPE(ehdr, phdr), (uint64_t)PHDR_SIZE(ehdr, phdr),
                    (uint64_t)PHDR_VADDR(ehdr, phdr), (uint64_t)elfoff,
                    (uint64_t)PHDR_OFF(ehdr, phdr));
        if (PHDR_TYPE(ehdr, phdr) != PT_LOAD)
            continue;
        if (PHDR_MEMSZ(ehdr, phdr) != 0 ||
            PHDR_FLAGS(ehdr, phdr) != 0 ||
            PHDR_ALIGN(ehdr, phdr) != 0) {
            LOG("memory check fail: memsz-0x%llx, flags-0x%x, align-0x%llx\n",
                    PHDR_MEMSZ(ehdr, phdr),
                    PHDR_FLAGS(ehdr, phdr),
                    PHDR_ALIGN(ehdr, phdr));
            break; /* stop dumping minirdump */
        }
        addr = PHDR_ADDR(ehdr, phdr);
        vaddr = PHDR_VADDR(ehdr, phdr);

        size = PHDR_SIZE(ehdr, phdr);
        if (size == 0 || elfoff == 0)
            LOG("kedump: dump addr 0x%llx, size 0x%lx\n", addr, size);
        if (ehdr->e_machine == EM_ARM)
            ((struct elf32_phdr *)phdr)->p_offset = elfoff;
        else
            ((struct elf64_phdr *)phdr)->p_offset = elfoff;
        if (size != 0 && elfoff != 0) {
            /*
             * 1.|start...end|...|vmalloc_start...vmalloc_end|
             *
             * 2.|vmalloc_start...vmalloc_end|...|start...end|
             *
             * 3.|vmalloc_start...|start...end|...vmalloc_end|
             *
             * 4.|vmalloc_start...|start...vmalloc_end|...end|
             *
             * 5.|start...|vmalloc_start...end|...vmalloc_end|
             *
             * 6.|start...|vmalloc_start...vmalloc_end|...end|
             */
            get_vmalloc_range(g_mcb, &vmalloc_start, &vmalloc_end);
            if (!is_arm_32bit(g_mcb, vaddr)
                    && (vaddr >= vmalloc_start)
                    && (vaddr + size <= vmalloc_end)) {
                total += kedump_dev_write_vmalloc_range(vaddr, addr, size, offset + elfoff);
            } else if (is_arm_32bit(g_mcb, vaddr)) {
                /*
                 * WARN: be carefull with the writing offset of expdb!!!
                 *
                 * Due to NAND storage is not allowed to write as convenient as EMMC storage,
                 * you have to set offset to a new page of NAND storage for each writing.
                 * We don't want the differences confuse us when we write to expdb, so we
                 * designed the expdb_write(), which take care of the offset itself, to make
                 * it easier for us to write files to expdb.
                 *
                 * However, once you change the offset yourself, which is necessary for
                 * SYS_MINI_RDUMP, you could messed up the writing of expdb_write().
                 * So,please set the offset carefully before or after you do expdb_write().
                 */
                expdb_flush_trunk_buf();
                expdb_set_offset(offset + elfoff);
                if (addr)
                    total += expdb_write((uint64_t)addr, size);
                else
                    total += kedump_dev_write_va_32(vaddr, size);
            } else {
                if ((vaddr + size <= vmalloc_start) || (vaddr >= vmalloc_end)) {
                    dprintf(INFO, "kedump: normal no-overlapp case: %d\n",
                            (vaddr + size <= vmalloc_start) ? 1 : 2);
                } else if ((vaddr <= vmalloc_start) && (vaddr + size >= vmalloc_end)) {
                    LOG("kedump: never overlapp case: 6\n");
                } else if ((vaddr + size <= vmalloc_end) || (vaddr >= vmalloc_start)) {
                    /* not expected cases */
                    LOG("kedump: should not be overlapp case: %d\n",
                            (vaddr + size <= vmalloc_end) ? 5 : 4);
                }
                total += kedump_dev_write_vmalloc_range(vaddr, addr, size, offset + elfoff);
            }
        }
        elfoff += ROUNDUP(size, TRUNK_SZ); /* next LOAD start from new page */
    }
    expdb_set_offset(offset + elfoff);

    /* Add file info to panic header ASAP */
    kedump_add2hdr(elfoff, elfoff, "SYS_MINI_RDUMP");

    /* Now we write the mini header to expdb */
    expdb_flush_trunk_buf();
    next_file_offset = expdb_get_offset();
    expdb_set_offset(offset);
    /* generate elf header for SYS_MINI_RDUMP file */
    mrdump_mini_gen_elf_header(elf_header, sz_header);
    total += expdb_write((uint64_t)vaddr_to_paddr(elf_header), sz_header);
    expdb_flush_trunk_buf();
    expdb_set_offset(next_file_offset);
    LOG("kedump: mini_rdump 0x%x/0x%lx @0x%llx\n", total, elfoff, offset);
    /* show curent time */
    kedump_print_time();
    free(elf_header);
    return elfoff;
}

static unsigned int kedump_misc(unsigned long long paddr, unsigned int start,
        unsigned int size)
{
    unsigned int total;

    dprintf(INFO, "kedump: misc data %x@%llx+%x\n", size, paddr, start);
    if (start >= size)
        start = start % size;
    total = expdb_write((uint64_t)(paddr + start), size - start);
    if (start)
        total += expdb_write((uint64_t)paddr, start);
    return total;
}

static unsigned int kedump_misc32(struct mrdump_mini_misc_data32 *data)
{
    unsigned int addr = data->paddr;
    unsigned int start = 0;
    unsigned int size = data->size;

    if (data->start != 0)
        kedump_mem_read(data->start, sizeof(unsigned int), &start);
    else
        start = 0;
    return kedump_misc((uint64_t)((uint32_t)addr), start, size);
}

static unsigned int kedump_misc64(struct mrdump_mini_misc_data64 *data)
{
    unsigned long long addr = (unsigned long long)data->paddr;
    unsigned int start = 0;
    unsigned int size = (unsigned int)data->size;

    if (data->start != 0)
        kedump_mem_read(data->start, sizeof(unsigned int), &start);
    else
        start = 0;
    return kedump_misc(addr, start, size);
}

static void kedump_add2hdr(unsigned int size, unsigned int datasize, const char *name)
{
    struct ipanic_data_header *pdata;
    uint64_t expdb_offset;
    uint64_t file_offset;
    int i;

    for (i = 0; i < IPANIC_NR_SECTIONS; i++) {
        pdata = &panic_header.data_hdr[i];
        if (pdata->valid == 0)
            break;
    }
    expdb_offset = expdb_get_offset();
    file_offset = expdb_offset - datasize;
    LOG("kedump add: %s[%d] %x/%x@%llx\n", name, i, datasize, size, file_offset);
    if (i < IPANIC_NR_SECTIONS) {
        pdata->offset = file_offset;
        pdata->total = size;
        pdata->used = datasize;
        strlcpy((char *)pdata->name, name, sizeof(pdata->name));
        pdata->valid = 1;
    }
    if (is_expdb_nand()) {
        /* NAND: don't write to expdb, just update the panic_header */
        header_off += sizeof(struct ipanic_data_header);
    } else {
        /* Other: write to expdb ASAP  */
        expdb_update_panic_header(header_off, (void *)pdata,
                                  sizeof(struct ipanic_data_header));
        header_off += sizeof(struct ipanic_data_header);
    }
}

static ssize_t length_limit;
static ssize_t length_plat_debug;
static ssize_t length_data_written;
/**
 * plat_dump_write - write debug info into expdb partition
 * @paddr: The physical start address of debug info memory.
 * @sz: The expected size written into expdb.
 *
 * @return: The actual size written into expdb.
 */
static bool plat_dump_write(uint64_t paddr, unsigned long sz)
{
    if (length_data_written >= length_limit)
        return false;

    if (((u32)length_data_written + sz) > (u32)length_limit)
        sz = (unsigned long)(length_limit - length_data_written);

    ssize_t datasize = expdb_write(paddr, sz);

    if (datasize > 0) {
        length_plat_debug += sz;
        length_data_written += datasize;

        return true;
    }

    return false;
}

enum {
    MBOOT_EXPDB_DFD = 0,
    MBOOT_EXPDB_LAST_CPU_BUS,
    MBOOT_EXPDB_VIP_LIST_NUM
};

extern const struct mboot_expdb_init __mboot_expdb[];
extern const struct mboot_expdb_init __mboot_expdb_end[];
static struct mboot_expdb_init *vip_list[MBOOT_EXPDB_VIP_LIST_NUM];

static void kedump_platform_debug(void)
{
    int plat_step = 100;

    for (const struct mboot_expdb_init *ptr = __mboot_expdb; ptr != __mboot_expdb_end; ptr++) {
        if (!strcmp(ptr->name, "DFD.dfd"))
            vip_list[MBOOT_EXPDB_DFD] = ptr;
        else if (!strcmp(ptr->name, "SYS_LAST_CPU_BUS"))
            vip_list[MBOOT_EXPDB_LAST_CPU_BUS] = ptr;
    }

    /* dump files in vip_list firstly */
    for (int i = 0; i < MBOOT_EXPDB_VIP_LIST_NUM; i++) {
        struct mboot_expdb_init *ptr = vip_list[i];

        if (!ptr)
            continue;

        length_limit = ptr->size;
        length_plat_debug = 0;
        length_data_written = 0;
        plat_step++;

        mboot_params_set_dump_step(plat_step);
        LOG("kedump: start %s\n", ptr->name);
        ptr->hook(plat_dump_write);

        if (length_data_written > 0)
            kedump_add2hdr(length_plat_debug, length_data_written, ptr->name);
    }

    /* dump other files */
    for (const struct mboot_expdb_init *ptr = __mboot_expdb; ptr != __mboot_expdb_end; ptr++) {
        bool skip_flag = false;

        for (int i = 0; i < MBOOT_EXPDB_VIP_LIST_NUM; i++) {
            if (ptr == vip_list[i]) {
                skip_flag = true;
                break;
            }
        }

        if (skip_flag)
            continue;

        length_limit = ptr->size;
        length_plat_debug = 0;
        length_data_written = 0;
        plat_step++;

        mboot_params_set_dump_step(plat_step);
        LOG("kedump: start %s\n", ptr->name);
        ptr->hook(plat_dump_write);

        if (length_data_written > 0)
            kedump_add2hdr(length_plat_debug, length_data_written, ptr->name);
    }
}

static int kedump_kernel_info(void)
{
    struct elfhdr *ehdr;
    unsigned long sz_misc, i, loop;
    void *phdr_misc, *phdr_pspr;
    struct elf_note *misc, *miscs;
    char *m_name;
    void *m_data;
    char name[32];
    unsigned int size, datasize;

    ehdr = kedump_elf_hdr();
    if (ehdr == 0)
        return -1;
    mboot_params_set_dump_step(AEE_LKDUMP_MINI_RDUMP);

    phdr_pspr = PHDR_INDEX(ehdr, 0);
    phdr_misc = PHDR_INDEX(ehdr, 1);
    if (PHDR_OFF(ehdr, phdr_misc) !=
            PHDR_OFF(ehdr, phdr_pspr) + PHDR_SIZE(ehdr, phdr_pspr)) {
        LOG("first 2 ehdr invalid\n");
        return -1;
    }
    if (g_mcb)
        kedump_mini_rdump(ehdr);
    else
        LOG("mrdump skip mini rdump - invalid mcb\n");

    miscs = (struct elf_note *)(ehdr->start + PHDR_OFF(ehdr, phdr_misc));
    LOG("kedump: misc[%p] %llx@%llx\n",
            phdr_misc, PHDR_SIZE(ehdr, phdr_misc), PHDR_OFF(ehdr, phdr_misc));
    sz_misc = sizeof(struct elf_note) + miscs->n_namesz + miscs->n_descsz;
    LOG("kedump: miscs[%p], size %lx\n", miscs, sz_misc);
    loop = (PHDR_SIZE(ehdr, phdr_misc)) / sz_misc;
    if (loop > MRDUMP_MINI_NR_MISC) {
        loop = MRDUMP_MINI_NR_MISC;
        LOG("kedump: invalid misc size\n");
    }
    /* update log store header data */
    logstore_flush_header_cache();
    for (i = 0; i < loop; i++) {
        char klog_first[16];
        unsigned int start_tmp = 0;

        memset(klog_first, 0x0, sizeof(klog_first));
        misc = (struct elf_note *)((void *)miscs + sz_misc * i);
        m_name = (char *)misc + sizeof(struct elf_note);
        if (m_name[0] == 'N' && m_name[1] == 'A' && m_name[2] == '\0')
            break;
        if (misc->n_type != NT_IPANIC_MISC) {
            LOG("misc invalid type: 0x%x\n", misc->n_type);
            break;
        }
        if (strcmp(m_name, MRDUMP_MINI_MISC_LOAD) == 0)
            continue;
        m_data = (void *)misc + sizeof(struct elf_note) + misc->n_namesz;
        if (misc->n_descsz == sizeof(struct mrdump_mini_misc_data32)) {
            if (strcmp(m_name, "_KERNEL_LOG_") == 0) {
                if (((struct mrdump_mini_misc_data32 *)m_data)->start != 0)
                    kedump_mem_read(((struct mrdump_mini_misc_data32 *)m_data)->start,
                            sizeof(unsigned int), &start_tmp);
                else
                    start_tmp = 0;
                sprintf(klog_first, "_%u", start_tmp);
                ((struct mrdump_mini_misc_data32 *)m_data)->start = 0;
            }
            datasize = kedump_misc32((struct mrdump_mini_misc_data32 *)m_data);
            size = ((struct mrdump_mini_misc_data32 *)m_data)->size;
        } else {
            if (strcmp(m_name, "_KERNEL_LOG_") == 0) {
                if (((struct mrdump_mini_misc_data64 *)m_data)->start != 0)
                    kedump_mem_read(((struct mrdump_mini_misc_data64 *)m_data)->start,
                            sizeof(unsigned int), &start_tmp);
                else
                    start_tmp = 0;
                sprintf(klog_first, "_%u", start_tmp);
                ((struct mrdump_mini_misc_data64 *)m_data)->start = 0;
            }
            datasize = kedump_misc64((struct mrdump_mini_misc_data64 *)m_data);
            size = ((struct mrdump_mini_misc_data64 *)m_data)->size;
        }
        /* [SYS_]MISC[_RAW] */
        if (m_name[0] == '_')
            strlcpy(name, "SYS", sizeof(name));
        else
            name[0] = 0;
        strlcat(name, m_name, sizeof(name));
        if (m_name[strlen(m_name)-1] == '_')
            strlcat(name,  "RAW", sizeof(name));
        if (klog_first[0] != 0)
            strlcat(name, klog_first, sizeof(name));
        kedump_add2hdr(size, datasize, name);
    }
    return 0;
}

static int kedump_mboot_params(void)
{
    paddr_t addr_misc = 0;
    unsigned int sz_misc = 0;
    unsigned int datasize;
    void *vaddr = NULL;

    /* mboot_params raw log */
    mboot_params_set_dump_step(AEE_LKDUMP_RAMCONSOLE_RAW);
    mboot_params_addr_size(&addr_misc, &sz_misc);
    if (addr_misc && sz_misc) {
        datasize = kedump_misc(vaddr_to_paddr((void *)addr_misc), 0, sz_misc);
        kc.mboot_params_crc = calculate_crc32((void *)addr_misc, sz_misc);
        kedump_add2hdr(sz_misc, datasize, "SYS_RAMCONSOLE_RAW");
    }

    /* pstore raw log*/
    mboot_params_set_dump_step(AEE_LKDUMP_PSTORE_RAW);
    addr_misc = 0;
    sz_misc = 0;
    pstore_addr_size(&addr_misc, &sz_misc);

    if (addr_misc && sz_misc) {
        datasize = kedump_misc(addr_misc, 0, sz_misc);
        {
            uint32_t start = ROUNDDOWN((uint32_t)addr_misc, PAGE_SIZE);
            uint32_t secsize = ROUNDUP((uint32_t)sz_misc, PAGE_SIZE);

            /* minirdump: minirdump will dump memory in DRAM, we must allocate it first */
            int map_ok = vmm_alloc_physical(vmm_get_kernel_aspace(),
                    "pstore_raw", secsize,
                    &vaddr, PAGE_SIZE_SHIFT,
                    (paddr_t)start, 0, ARCH_MMU_FLAG_CACHED);

            if (map_ok != NO_ERROR)
                LOG("kedump: map error for pstore\n");
            vaddr += (uint32_t)(addr_misc - start);
        }

        kc.pstore_crc = calculate_crc32((void *)vaddr, sz_misc);
        kedump_add2hdr(sz_misc, datasize, "SYS_PSTORE_RAW");
        vmm_free_region(vmm_get_kernel_aspace(), (vaddr_t)vaddr);
    }

    /* save crc data*/
    mboot_params_set_dump_step(AEE_LKDUMP_KEDUMP_CRC);
    datasize = expdb_write((uint64_t)vaddr_to_paddr((void *)(&kc)),
                                    sizeof(struct kedump_crc));
    kedump_add2hdr(sizeof(struct kedump_crc), datasize, "KEDUMP_CRC");
    return 0;
}

static int kedump_to_expdb(void)
{
    unsigned long long offset;
    unsigned int datasize;
    addr_t zaee_addr;
    unsigned int zaee_size;

    last_dump_step = mboot_params_get_dump_step();
    if (last_dump_step != AEE_LKDUMP_CLEAR) {
        LOG("kedump: last lk dump is not finished at step %x\n", last_dump_step);
    }

    //write header firstly
    panic_header.magic = AEE_IPANIC_MAGIC;
    panic_header.version = AEE_IPANIC_PHDR_VERSION;
    panic_header.size = sizeof(panic_header);
    panic_header.blksize = expdb_get_block_size();
    panic_header.partsize = expdb_get_part_size();
    panic_header.exp_type = aee_get_exp_type();
    panic_header.wdt_status = aee_get_wdt_status();
    dprintf(CRITICAL, "panic_header.exp_type = 0x%x\n", panic_header.exp_type);
    dprintf(CRITICAL, "panic_header.wdt_status = 0x%x\n", panic_header.wdt_status);
    header_off = sizeof(panic_header) - sizeof(struct ipanic_data_header) * IPANIC_NR_SECTIONS;
    /*
     * NAND: erase before use for each block, header will be writen in the end
     * Other: update the header info to expdb ASAP
     */
    if (is_expdb_nand())
        expdb_erase_db_part(); /* erase once for all */
    else
        expdb_update_panic_header(0, (void *)(&panic_header), header_off);

    LOG("kedump: block size:0x%x\n", panic_header.blksize);
    /* reserve space in expdb for panic header */
    offset = ALIGN(sizeof(panic_header), panic_header.blksize);
    expdb_set_offset(offset); // set the start point

    logstore_flush_header_cache(); // update log store data in case of aee lk watchdog timeout
    kedump_mboot_params();
    kedump_platform_debug();
    kedump_print_time();
    logstore_flush_header_cache(); //dtto
    kedump_kernel_info();

    /* save KEdump flow logs */
    zaee_logbuf_addr_size(&zaee_addr, &zaee_size);
    datasize = expdb_write((uint64_t)vaddr_to_paddr((void *)zaee_addr), zaee_size);
    kedump_add2hdr(zaee_size, datasize, "ZAEE_LOG");
    if (is_expdb_nand()) {
        expdb_flush_trunk_buf();
        expdb_update_panic_header(0, (void *)(&panic_header), sizeof(panic_header));
    }

    mboot_params_set_dump_step(AEE_LKDUMP_CLEAR);
    return 0;
}

static int kedump_avail(void)
{
    void *mini_va = mrdump_mini_header_vaddr;

    if (((char *)mini_va)[0] == 0x81 &&
            ((char *)mini_va)[1] == 'E' &&
            ((char *)mini_va)[2] == 'L' &&
            ((char *)mini_va)[3] == 'F') {
        LOG("kedump: already dumped in lk\n");
        vmm_free_region(vmm_get_kernel_aspace(), (vaddr_t)mrdump_mini_header_vaddr);
        return -1;
    }

    return 0;
}

int kedump_get_data_info(int index, char **name, u32 *offset, u32 *size)
{
    struct ipanic_header *iheader;
    struct ipanic_data_header *pdata;
    bdev_t *dev_expdb;
    int ret = 0;
    int len = 0;

    iheader = (struct ipanic_header *)malloc(sizeof(struct ipanic_header));
    if (iheader == NULL) {
        LOG("kedump: malloc failed at %s\n", __func__);
        return 0;
    }

    memset(iheader, 0x0, sizeof(struct ipanic_header));
    dev_expdb = bio_open_by_label(AEE_IPANIC_LABEL);
    if (!dev_expdb) {
        LOG("kedump: Partition [%s] is not exist.\n", AEE_IPANIC_LABEL);
        ret = -ENODEV;
        goto exit;
    }

    len = bio_read(dev_expdb, (void *)iheader, 0, sizeof(struct ipanic_header));
    if (len < (ssize_t)sizeof(struct ipanic_header)) {
        LOG("kedump: failed to read iheader (%d)\n", len);
        ret = -EIO;
        goto exit;
    }

    if (iheader->magic != AEE_IPANIC_MAGIC || iheader->version < AEE_IPANIC_PHDR_VERSION) {
        ret = -1;
        goto exit;
    }

    if (index < 0 || index >= IPANIC_NR_SECTIONS) {
        LOG("kedump: invalid index number:%d\n", index);
        ret = -2;
        goto exit;
    }

    if (name == NULL || offset == NULL || size == NULL) {
        LOG("kedump: invalid argument number:%d\n", index);
        ret = -3;
        goto exit;
    }

    pdata = &iheader->data_hdr[index];
    if (pdata->valid == 0) {
        ret = -4;
        goto exit;

    }
    *name = (char *)pdata->name;
    *offset = pdata->offset;
    *size = pdata->used;

exit:
    if (dev_expdb)
        bio_close(dev_expdb);
    if (iheader)
        free(iheader);
    return ret;
}

static int kedump_done(void)
{
    void *flag = mrdump_mini_header_vaddr;

    ((char *)flag)[0] = 0x81;
    ((char *)flag)[1] = 'E';
    ((char *)flag)[2] = 'L';
    ((char *)flag)[3] = 'F';
    arch_clean_cache_range((addr_t)flag, sizeof(struct elfhdr));
    vmm_free_region(vmm_get_kernel_aspace(), (vaddr_t)mrdump_mini_header_vaddr);
    return 0;
}

/* in case that platform didn't support smart_reset_check() */
__WEAK const char *smart_reset_check(void)
{
    return NULL;
}

/* in case that platform didn't support mtk_wdt_get_last_stage() */
__WEAK const char *mtk_wdt_get_last_stage(void)
{
    return NULL;
}

#define SHOW_ARGS(p, a, b, c, d, e) \
    LOG(#a":0x%llx, "#b":0x%llx,"#c":0x%llx, "#d":0x%llx\n", (p)->a, (p)->b, (p)->c, (p)->d, (p)->e)
static void show_info(struct mrdump_control_block *mcb)
{
    struct mrdump_machdesc *mmp;

    if (mcb == NULL) {
        LOG("%s: mrdump_cb is NULL\n", __func__);
        return;
    }

    if (kedump_is_mcb_valid(mcb)) {
        mmp = &mcb->machdesc;
        SHOW_ARGS(mmp, vmalloc_start, vmalloc_end, master_page_table, tcr_el1_t1sz,
            kernel_pac_mask);
    } else {
        LOG("mrdump_cb: sig error:0x%llx in %s\n", *(uint64_t *)mcb, __func__);
        return;
    }
}

static void ke_reserved_mem_map(void)
{
    addr_t addr;
    unsigned int size;
    status_t err;

    mrdump_mini_header_addr_size(&addr, &size);

    /* KEDump need to use header in DRAZM, we must allocate it first */
    err = vmm_alloc_physical(vmm_get_kernel_aspace(),
            "ke_mem_addr", size,
            &mrdump_mini_header_vaddr, PAGE_SIZE_SHIFT,
            (paddr_t)addr, 0, ARCH_MMU_FLAG_CACHED);
    if (err != NO_ERROR)
        panic("mini_start vmm_alloc_physical failed\n");
}

/* Dump KE information to expdb */
void kedump_mini(void)
{
    const char *status;

    status = smart_reset_check();
    if (status != NULL)
        LOG("%s\n", status);

    status = mtk_wdt_get_last_stage();
    if (status != NULL)
        LOG("%s\n", status);

    mtk_wdt_restart_timer();
    LOG("kedump mini start\n");
    kedump_print_time();
    if (!aee_ddr_reserve_enable())
        LOG("kedump: ddr reserve mode disabled\n");
    else
        LOG("kedump: ddr reserve mode enabled\n");
    if (!aee_ddr_reserve_success())
        LOG("kedump: ddr reserve mode failed\n");

    LOG("kedump: boot_reason(%d)\n", platform_get_boot_reason());
    if (is_expdb_valid()) {
        /* only recored the first exception */
        LOG("kedump: skip due to expdb has data\n");
        return;
    }
    ke_reserved_mem_map();
    if (kedump_avail())
        return;

    g_mcb = aee_mrdump_get_info();
    show_info(g_mcb);
    kedump_get_dram_range();

    kedump_to_expdb();
    kedump_done();
    LOG("kedump mini done\n");
    return;
}
