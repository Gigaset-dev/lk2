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

#include <assert.h>
#include <debug.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <trace.h>

#include "aee.h"
#include "mrdump_elf.h"
#include "mrdump_private.h"

#define LOCAL_TRACE 0

#define mrdump_elf_setup_elfhdr(elfp, machid, elfhdr_t, elfphdr_t, phdrcount)    \
do { \
    elfp->e_type = ET_CORE;                        \
    elfp->e_machine = machid;                    \
    elfp->e_version = EV_CURRENT;                    \
    elfp->e_entry = 0;                        \
    elfp->e_phoff = sizeof(elfhdr_t);                \
    elfp->e_shoff = 0;                        \
    elfp->e_flags = ELF_CORE_EFLAGS;                \
    elfp->e_ehsize = sizeof(elfhdr_t);                \
    elfp->e_phentsize = sizeof(elfphdr_t);                \
    elfp->e_phnum = phdrcount;                    \
    elfp->e_shentsize = 0;                        \
    elfp->e_shnum = 0;                        \
    elfp->e_shstrndx = 0;                        \
} while (0)

static inline void mrdump_elf_setup_eident(unsigned char e_ident[EI_NIDENT], unsigned char elfclasz)
{
    memcpy(e_ident, ELFMAG, SELFMAG);
    e_ident[EI_CLASS] = elfclasz;
    e_ident[EI_DATA] = ELFDATA2LSB;
    e_ident[EI_VERSION] = EV_CURRENT;
    e_ident[EI_OSABI] = ELFOSABI_NONE;
    memset(e_ident+EI_PAD, 0, EI_NIDENT-EI_PAD);
}

/* An ELF note in memory */
struct memelfnote {
    const char *name;
    int type;
    unsigned int datasz;
    void *data;
};

static int notesize(struct memelfnote *en)
{
    int sz;

    sz = sizeof(struct elf32_note);
    sz += ROUNDUP((strlen(en->name) + 1), 4);
    sz += ROUNDUP(en->datasz, 4);

    return sz;
}

static uint8_t *storenote(struct memelfnote *men, uint8_t *bufp)
{
    struct elf32_note en;

    en.n_namesz = strlen(men->name) + 1;
    en.n_descsz = men->datasz;
    en.n_type = men->type;

    memcpy(bufp, &en, sizeof(en));
    bufp += sizeof(en);

    memcpy(bufp, men->name, en.n_namesz);
    bufp += en.n_namesz;

    bufp = (uint8_t *) ROUNDUP((unsigned long)bufp, 4);
    memcpy(bufp, men->data, men->datasz);
    bufp += men->datasz;

    bufp = (uint8_t *) ROUNDUP((unsigned long)bufp, 4);
    return bufp;
}

static uint8_t *mrdump_core32_write_cpu_note(const struct mrdump_control_block *mrdump_cb, int cpu,
        struct elf32_phdr *nhdr, uint8_t *bufp)
{
    struct memelfnote notes;
    struct elf32_arm_prstatus prstatus;
    char cpustr[16];
    int err = 0;

    memset(&prstatus, 0, sizeof(struct elf32_arm_prstatus));

    err = snprintf(cpustr, sizeof(cpustr), "CPU%d", cpu);
    ASSERT(err > 0);

    /* set up the process status */
    notes.name = cpustr;
    notes.type = NT_PRSTATUS;
    notes.datasz = sizeof(struct elf32_arm_prstatus);
    notes.data = &prstatus;

    prstatus.pr_pid = cpu + 1;
    memcpy(&prstatus.pr_reg,
        (unsigned long *)&mrdump_cb->crash_record.cpu_regs[cpu].arm32_reg.arm32_regs,
        sizeof(elf_arm_gregset_t));

    nhdr->p_filesz += notesize(&notes);
    return storenote(&notes, bufp);
}

static uint8_t *mrdump_core32_write_machdesc(const struct mrdump_control_block *mrdump_cb,
        struct elf32_phdr *nhdr, uint8_t *bufp)
{
    struct memelfnote notes;
    struct elf_mrdump_machdesc machdesc;
    const struct mrdump_machdesc *kparams = &mrdump_cb->machdesc;

    memset(&machdesc, 0, sizeof(struct elf_mrdump_machdesc));

    notes.name = "MRDUMP01";
    notes.type = NT_MRDUMP_MACHDESC;
    notes.datasz = sizeof(struct elf_mrdump_machdesc);
    notes.data = &machdesc;

    machdesc.flags = MRDUMP_TYPE_FULL_MEMORY;
    machdesc.nr_cpus = kparams->nr_cpus;
    machdesc.phys_offset = kparams->phys_offset;
    machdesc.page_offset = kparams->page_offset;
    machdesc.tcr_el1_t1sz = kparams->tcr_el1_t1sz;
    machdesc.kimage_vaddr = kparams->kimage_vaddr;
    machdesc.modules_start = kparams->modules_start;
    machdesc.modules_end = kparams->modules_end;
    machdesc.vmalloc_start = kparams->vmalloc_start;
    machdesc.vmalloc_end = kparams->vmalloc_end;
    machdesc.master_page_table = kparams->master_page_table;

    nhdr->p_filesz += notesize(&notes);
    return storenote(&notes, bufp);
}

static uint8_t *mrdump_core32_write_cb(const struct mrdump_control_block *mrdump_cb,
        struct elf32_phdr *nhdr, uint8_t *bufp)
{
    struct memelfnote notes;
    uint64_t mrdump_cb_paddr = (uint64_t)(uintptr_t)mrdump_cb;

    notes.name = MRDUMP_GO_DUMP;
    notes.type = NT_MRDUMP_CBLOCK;
    notes.datasz = sizeof(mrdump_cb_paddr);
    notes.data = &mrdump_cb_paddr;

    nhdr->p_filesz += notesize(&notes);
    return storenote(&notes, bufp);
}

static void mrdump_core32_header_init(const struct mrdump_control_block *mrdump_cb,
        const struct kzip_addlist *memlist)
{
    struct elf32_phdr *nhdr, *phdr;
    struct elf32_hdr *elf;
    off_t offset = 0;
    const struct mrdump_machdesc *kparams = &mrdump_cb->machdesc;

    uint8_t *bufp = (uint8_t *)(uintptr_t)memlist[0].address;

    elf = (struct elf32_hdr *) bufp;
    bufp += sizeof(struct elf32_hdr);
    offset += sizeof(struct elf32_hdr);

    nhdr = (struct elf32_phdr *) bufp;
    bufp += sizeof(struct elf32_phdr);
    offset += sizeof(struct elf32_phdr);
    memset(nhdr, 0, sizeof(struct elf32_phdr));
    nhdr->p_type = PT_NOTE;

    unsigned long long foffset = MRDUMP_CORE_HEADER_SIZE;
    int i;

    for (i = 1; memlist[i].address != 0; i++) {
        phdr = (struct elf32_phdr *) bufp;
        bufp += sizeof(struct elf32_phdr);
        offset += sizeof(struct elf32_phdr);

        phdr->p_type    = PT_LOAD;
        phdr->p_flags   = PF_R|PF_W|PF_X;
        phdr->p_offset  = foffset;
        phdr->p_vaddr   = 0;
        phdr->p_paddr   = memlist[i].address;
        phdr->p_filesz  = memlist[i].size;
        phdr->p_memsz   = memlist[i].size;
        phdr->p_align   = 0;

        foffset += phdr->p_filesz;
    }

    mrdump_elf_setup_eident(elf->e_ident, ELFCLASS32);
    mrdump_elf_setup_elfhdr(elf, EM_ARM, struct elf32_hdr, struct elf32_phdr, i);

    nhdr->p_offset = offset;

    /* NT_PRPSINFO */
    struct elf32_prpsinfo prpsinfo;
    struct memelfnote notes;
    /* set up the process info */
    notes.name = CORE_STR;
    notes.type = NT_PRPSINFO;
    notes.datasz = sizeof(struct elf32_prpsinfo);
    notes.data = &prpsinfo;

    memset(&prpsinfo, 0, sizeof(struct elf32_prpsinfo));
    prpsinfo.pr_state = 0;
    prpsinfo.pr_sname = 'R';
    prpsinfo.pr_zomb = 0;
    prpsinfo.pr_gid = prpsinfo.pr_uid = mrdump_cb->crash_record.fault_cpu + 1;
    strlcpy(prpsinfo.pr_fname, "vmlinux", sizeof(prpsinfo.pr_fname));
    strlcpy(prpsinfo.pr_psargs, "vmlinux", ELF_PRARGSZ);

    nhdr->p_filesz += notesize(&notes);
    bufp = storenote(&notes, bufp);

    bufp = mrdump_core32_write_machdesc(mrdump_cb, nhdr, bufp);

    /* Store pre-cpu backtrace */
    for (unsigned int cpu = 0; cpu < kparams->nr_cpus; cpu++)
        bufp = mrdump_core32_write_cpu_note(mrdump_cb, cpu, nhdr, bufp);

    /* store mrdump control block */
    bufp = mrdump_core32_write_cb(mrdump_cb, nhdr, bufp);
}

static int notesize64(struct memelfnote *en)
{
    int sz;

    sz = sizeof(struct elf64_note);
    sz += ROUNDUP((strlen(en->name) + 1), 4);
    sz += ROUNDUP(en->datasz, 4);

    return sz;
}

static uint8_t *storenote64(struct memelfnote *men, uint8_t *bufp)
{
    struct elf64_note en;

    en.n_namesz = strlen(men->name) + 1;
    en.n_descsz = men->datasz;
    en.n_type = men->type;

    memcpy(bufp, &en, sizeof(en));
    bufp += sizeof(en);

    memcpy(bufp, men->name, en.n_namesz);
    bufp += en.n_namesz;

    bufp = (uint8_t *) ROUNDUP((unsigned long)bufp, 4);
    memcpy(bufp, men->data, men->datasz);
    bufp += men->datasz;

    bufp = (uint8_t *) ROUNDUP((unsigned long)bufp, 4);
    return bufp;
}

static uint8_t *mrdump_core64_write_cpu_note(const struct mrdump_control_block *mrdump_cb, int cpu,
        struct elf64_phdr *nhdr, uint8_t *bufp)
{
    struct memelfnote notes;
    struct elf_arm64_prstatus64 prstatus;
    char cpustr[16];
    int err;

    memset(&prstatus, 0, sizeof(struct elf_arm64_prstatus64));

    err = snprintf(cpustr, sizeof(cpustr), "CPU%d", cpu);
    ASSERT(err > 0);

    /* set up the process status */
    notes.name = cpustr;
    notes.type = NT_PRSTATUS;
    notes.datasz = sizeof(struct elf_arm64_prstatus64);
    notes.data = &prstatus;

    prstatus.pr_pid = cpu + 1;
    memcpy(&prstatus.pr_reg, &mrdump_cb->crash_record.cpu_regs[cpu].arm64_reg.arm64_regs,
        sizeof(elf_arm64_gregset_t));

    nhdr->p_filesz += notesize64(&notes);
    return storenote64(&notes, bufp);
}

static uint8_t *mrdump_core64_write_machdesc(const struct mrdump_control_block *mrdump_cb,
        struct elf64_phdr *nhdr, uint8_t *bufp)
{
    struct memelfnote notes;
    struct elf_mrdump_machdesc machdesc;
    const struct mrdump_machdesc *kparams = &mrdump_cb->machdesc;

    memset(&machdesc, 0, sizeof(struct elf_mrdump_machdesc));

    notes.name = "MRDUMP01";
    notes.type = NT_MRDUMP_MACHDESC;
    notes.datasz = sizeof(struct elf_mrdump_machdesc);
    notes.data = &machdesc;

    machdesc.flags = MRDUMP_TYPE_FULL_MEMORY;
    machdesc.nr_cpus = kparams->nr_cpus;
    machdesc.phys_offset = kparams->phys_offset;
    machdesc.page_offset = kparams->page_offset;
    machdesc.tcr_el1_t1sz = kparams->tcr_el1_t1sz;
    machdesc.kimage_vaddr = kparams->kimage_vaddr;
    machdesc.modules_start = kparams->modules_start;
    machdesc.modules_end = kparams->modules_end;
    machdesc.vmalloc_start = kparams->vmalloc_start;
    machdesc.vmalloc_end = kparams->vmalloc_end;
    machdesc.master_page_table = kparams->master_page_table;

    nhdr->p_filesz += notesize64(&notes);
    return storenote64(&notes, bufp);
}

#define MBLOCK_RSV_SIZE 4096
#define MBLOCK_BUF_SIZE 512
static uint8_t *mrdump_core64_write_mblock_reserved_info(struct elf64_phdr *nhdr, uint8_t *bufp)
{
    struct memelfnote notes;
    char name[MBLOCK_RESERVED_NAME_SIZE];
    u64 start, size;
    int err, i;
    char *mblock_reserved_info, *buff;

    mblock_reserved_info = malloc(MBLOCK_RSV_SIZE);
    if (!mblock_reserved_info) {
        dprintf(CRITICAL, "%s: malloc fail\n", __func__);
        return bufp;
    }

    buff = malloc(MBLOCK_BUF_SIZE);
    if (!buff) {
        dprintf(CRITICAL, "%s: malloc fail\n", __func__);
        free(mblock_reserved_info);
        return bufp;
    }

    memset(mblock_reserved_info, 0, MBLOCK_RSV_SIZE);
    memset(buff, 0, MBLOCK_BUF_SIZE);

    notes.name = "MBLOCK_RSV_INFO";
    notes.type = 0;
    notes.datasz = 0;
    notes.data = mblock_reserved_info;

    strncat(mblock_reserved_info, "[",
        MBLOCK_RSV_SIZE - strlen(mblock_reserved_info) - 1);
    for (i = 0; i < MBLOCK_RESERVED_NUM_MAX; i++) {
        if (!mrdump_get_reserved_info_by_index(i, name, &start, &size)) {
            memset(buff, 0, MBLOCK_BUF_SIZE);
            snprintf(buff, MBLOCK_BUF_SIZE,
                "{\"name\":\"%s\", \"start\":\"0x%llx\", \"size\":\"0x%llx\"},",
                name, start, size);
            strncat(mblock_reserved_info, buff,
                MBLOCK_RSV_SIZE - strlen(mblock_reserved_info) - 1);
        }
    }

    /* reserved a NULL object {} as a end of array object for JSON */
    strncat(mblock_reserved_info, "{\"name\":\"\", \"start\":\"\", \"size\":\"\"}]",
                        MBLOCK_RSV_SIZE - strlen(mblock_reserved_info) - 1);
    notes.datasz = strlen(mblock_reserved_info);

    nhdr->p_filesz += notesize64(&notes);
    return storenote64(&notes, bufp);
}

static uint8_t *mrdump_core64_write_vmcoreinfo(const struct mrdump_control_block *mrdump_cb,
        struct elf64_phdr *nhdr, uint8_t *bufp)
{
    struct memelfnote notes;
    const struct mrdump_machdesc *kparams = &mrdump_cb->machdesc;
    char vmcoreinfo[512];
    int err;

    memset(vmcoreinfo, 0, sizeof(vmcoreinfo));

    notes.name = "VMCOREINFO";
    notes.type = 0;
    notes.datasz = 0;
    notes.data = &vmcoreinfo;
    err = snprintf(vmcoreinfo, sizeof(vmcoreinfo),
         "PAGESIZE=4096\n"
         "NUMBER(kimage_voffset)=0x%llx\n"
         "SYMBOL(_stext)=0x%llx\n"
         "NUMBER(VA_BITS)=%d\n"
         "NUMBER(PHYS_OFFSET)=0x%llx\n"
         "NUMBER(TCR_EL1_T1SZ)=%llu\n"
         "NUMBER(KERNELPACMASK)=0x%llx\n",
         (uint64_t)kparams->kimage_voffset,
         (uint64_t)kparams->kimage_stext_real,
         39,
         (uint64_t)kparams->phys_offset,
         (uint64_t)kparams->tcr_el1_t1sz,
         (uint64_t)kparams->kernel_pac_mask);
    ASSERT(err > 0);
    notes.datasz = strlen(vmcoreinfo);

    nhdr->p_filesz += notesize64(&notes);
    return storenote64(&notes, bufp);
}

static uint8_t *mrdump_core64_write_cb(const struct mrdump_control_block *mrdump_cb,
        struct elf64_phdr *nhdr, uint8_t *bufp)
{
    struct memelfnote notes;
    uint64_t mrdump_cb_paddr = (uint64_t)(uintptr_t)mrdump_cb;

    notes.name = MRDUMP_GO_DUMP;
    notes.type = NT_MRDUMP_CBLOCK;
    notes.datasz = sizeof(mrdump_cb_paddr);
    notes.data = &mrdump_cb_paddr;

    nhdr->p_filesz += notesize64(&notes);
    return storenote64(&notes, bufp);
}

static void mrdump_core64_header_init(const struct mrdump_control_block *mrdump_cb,
        const struct kzip_addlist *memlist)
{
    struct elf64_phdr *nhdr, *phdr;
    struct elf64_hdr *elf;
    off_t offset = 0;
    const struct mrdump_machdesc *kparams = &mrdump_cb->machdesc;

    uint8_t *bufp = (uint8_t *)(uintptr_t)memlist[0].address;

    elf = (struct elf64_hdr *) bufp;
    bufp += sizeof(struct elf64_hdr);
    offset += sizeof(struct elf64_hdr);

    nhdr = (struct elf64_phdr *) bufp;
    bufp += sizeof(struct elf64_phdr);
    offset += sizeof(struct elf64_phdr);
    memset(nhdr, 0, sizeof(struct elf64_phdr));
    nhdr->p_type = PT_NOTE;

    unsigned long long foffset = MRDUMP_CORE_HEADER_SIZE;
    int i;

    for (i = 1; memlist[i].address != 0; i++) {
        phdr = (struct elf64_phdr *) bufp;
        bufp += sizeof(struct elf64_phdr);
        offset += sizeof(struct elf64_phdr);

        phdr->p_type    = PT_LOAD;
        phdr->p_flags   = PF_R|PF_W|PF_X;
        phdr->p_offset  = foffset;
        phdr->p_vaddr   = 0;
        phdr->p_paddr   = memlist[i].address;
        phdr->p_filesz  = memlist[i].size;
        phdr->p_memsz   = memlist[i].size;
        phdr->p_align   = 0;

        foffset += phdr->p_filesz;
    }

    mrdump_elf_setup_eident(elf->e_ident, ELFCLASS64);
    mrdump_elf_setup_elfhdr(elf, EM_AARCH64, struct elf64_hdr, struct elf64_phdr, i);

    nhdr->p_offset = offset;

    /* NT_PRPSINFO */
    struct elf_prpsinfo64 prpsinfo;
    struct memelfnote notes;
    /* set up the process info */
    notes.name = CORE_STR;
    notes.type = NT_PRPSINFO;
    notes.datasz = sizeof(struct elf_prpsinfo64);
    notes.data = &prpsinfo;

    memset(&prpsinfo, 0, sizeof(struct elf_prpsinfo64));
    prpsinfo.pr_state = 0;
    prpsinfo.pr_sname = 'R';
    prpsinfo.pr_zomb = 0;
    prpsinfo.pr_gid = prpsinfo.pr_uid = mrdump_cb->crash_record.fault_cpu + 1;
    strlcpy(prpsinfo.pr_fname, "vmlinux", sizeof(prpsinfo.pr_fname));
    strlcpy(prpsinfo.pr_psargs, "vmlinux", ELF_PRARGSZ);

    nhdr->p_filesz += notesize64(&notes);
    bufp = storenote64(&notes, bufp);

    bufp = mrdump_core64_write_machdesc(mrdump_cb, nhdr, bufp);
    bufp = mrdump_core64_write_vmcoreinfo(mrdump_cb, nhdr, bufp);
    bufp = mrdump_core64_write_mblock_reserved_info(nhdr, bufp);

    /* Store pre-cpu backtrace */
    for (unsigned int cpu = 0; cpu < kparams->nr_cpus; cpu++)
        bufp = mrdump_core64_write_cpu_note(mrdump_cb, cpu, nhdr, bufp);

    /* store mrdump control block */
    bufp = mrdump_core64_write_cb(mrdump_cb, nhdr, bufp);
}

void mrdump_core_header_init(const struct mrdump_control_block *kparams,
        const struct kzip_addlist *memlist)
{
    if (kparams->machdesc.page_offset <= 0xffffffffULL) {
        voprintf_info("32b kernel detected\n");
        mrdump_core32_header_init(kparams, memlist);
    } else {
        voprintf_info("64b kernel detected\n");
        mrdump_core64_header_init(kparams, memlist);
    }
}
