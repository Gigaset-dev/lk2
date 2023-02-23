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
#pragma once

#include <compiler.h>
#include <stdint.h>

#define ELFMAG      "\177ELF"
#define SELFMAG     4

#define CORE_STR "CORE"

#ifndef ELF_CORE_EFLAGS
#define ELF_CORE_EFLAGS 0
#endif

#define EI_NIDENT   16

#define EI_CLASS    4
#define EI_DATA     5
#define EI_VERSION  6
#define EI_OSABI    7
#define EI_PAD      8

#define EM_ARM 40
#define EM_AARCH64 183

#define ET_CORE 4

#define PT_NULL 0
#define PT_LOAD 1
#define PT_NOTE 4

#define ELFCLASS32 1
#define ELFCLASS64 2

#define NT_PRSTATUS 1
#define NT_PRFPREG 2
#define NT_PRPSINFO 3
#define NT_AUXV 6
#define NT_ARM_PAC_MASK 0x406
/* MRDUMP support note type */
#define NT_MRDUMP_MACHDESC 0xAEE0
#define NT_MRDUMP_CBLOCK 0xBEEF

#define AT_HWCAP 16

#define PF_R 0x4
#define PF_W 0x2
#define PF_X 0x1

#define ELFOSABI_NONE 0

#define EV_CURRENT 1

#define ELFDATA2LSB 1

#define ELF_PRARGSZ 80

#define MRDUMP_TYPE_FULL_MEMORY 0
#define MRDUMP_TYPE_KERNEL_1 1
#define MRDUMP_TYPE_KERNEL_2 2

#define MRDUMP_TYPE_MASK 0x3

#define NT_IPANIC_MISC 4095

typedef uint32_t Elf32_Addr;
typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Off;
typedef int32_t Elf32_Sword;
typedef uint32_t Elf32_Word;

typedef uint64_t Elf64_Addr;
typedef uint16_t Elf64_Half;
typedef uint64_t Elf64_Off;
typedef uint32_t Elf64_Word;
typedef uint64_t Elf64_Xword;

struct elf_siginfo {
    int si_signo;
    int si_code;
    int si_errno;
};

struct __PACKED elf_mrdump_machdesc {
    uint32_t flags;
    uint32_t nr_cpus;

    uint64_t phys_offset;
    uint64_t total_memory;

    uint64_t page_offset;
    uint64_t tcr_el1_t1sz;

    uint64_t kimage_vaddr;

    uint64_t modules_start;
    uint64_t modules_end;

    uint64_t vmalloc_start;
    uint64_t vmalloc_end;

    uint64_t master_page_table;

    uint64_t dfdmem_pa;
};

#define ELF_ARM_NGREGS 18
typedef uint32_t elf_arm_gregset_t[ELF_ARM_NGREGS];

struct elf32_hdr {
    unsigned char e_ident[EI_NIDENT];
    Elf32_Half e_type;
    Elf32_Half e_machine;
    Elf32_Word e_version;
    Elf32_Addr e_entry;
    Elf32_Off e_phoff;
    Elf32_Off e_shoff;
    Elf32_Word e_flags;
    Elf32_Half e_ehsize;
    Elf32_Half e_phentsize;
    Elf32_Half e_phnum;
    Elf32_Half e_shentsize;
    Elf32_Half e_shnum;
    Elf32_Half e_shstrndx;
};

struct elf32_phdr {
    Elf32_Word p_type;
    Elf32_Off p_offset;
    Elf32_Addr p_vaddr;
    Elf32_Addr p_paddr;
    Elf32_Word p_filesz;
    Elf32_Word p_memsz;
    Elf32_Word p_flags;
    Elf32_Word p_align;
};

struct elf32_note {
    Elf32_Word   n_namesz;       /* Name size */
    Elf32_Word   n_descsz;       /* Content size */
    Elf32_Word   n_type;         /* Content type */
};

struct elf32_timeval {
    int32_t tv_sec;
    int32_t tv_usec;
};

struct elf32_arm_prstatus {
    struct elf_siginfo pr_info;
    short pr_cursig;
    unsigned long pr_sigpend;
    unsigned long pr_sighold;

    int32_t pr_pid;
    int32_t pr_ppid;
    int32_t pr_pgrp;

    int32_t pr_sid;
    struct elf32_timeval pr_utime;
    struct elf32_timeval pr_stime;
    struct elf32_timeval pr_cutime;
    struct elf32_timeval pr_cstime;

    elf_arm_gregset_t pr_reg;

    int pr_fpvalid;
};

struct elf32_prpsinfo {
    char pr_state;
    char pr_sname;
    char pr_zomb;
    char pr_nice;
    unsigned long pr_flag;

    uint16_t pr_uid;
    uint16_t pr_gid;

    int32_t pr_pid;
    int32_t pr_ppid;
    int32_t pr_pgrp;
    int32_t pr_sid;

    char pr_fname[16];
    char pr_psargs[ELF_PRARGSZ];
};

#define ELF_ARM64_NGREGS 34
typedef uint64_t elf_arm64_gregset_t[ELF_ARM64_NGREGS];

struct elf64_hdr {
    unsigned char e_ident[EI_NIDENT];
    Elf64_Half e_type;
    Elf64_Half e_machine;
    Elf64_Word e_version;
    Elf64_Addr e_entry;
    Elf64_Off e_phoff;
    Elf64_Off e_shoff;
    Elf64_Word e_flags;
    Elf64_Half e_ehsize;
    Elf64_Half e_phentsize;
    Elf64_Half e_phnum;
    Elf64_Half e_shentsize;
    Elf64_Half e_shnum;
    Elf64_Half e_shstrndx;
};

struct elf64_phdr {
    Elf64_Word p_type;
    Elf64_Word p_flags;
    Elf64_Off p_offset;
    Elf64_Addr p_vaddr;
    Elf64_Addr p_paddr;
    Elf64_Xword p_filesz;
    Elf64_Xword p_memsz;
    Elf64_Xword p_align;
};

struct elf64_note {
    Elf64_Word    n_namesz;   /* Name size */
    Elf64_Word    n_descsz;   /* Content size */
    Elf64_Word    n_type;     /* Content type */
};

struct elf_timeval64 {
    int64_t tv_sec;
    int64_t tv_usec;
};

struct elf_arm64_prstatus64 {
    struct elf_siginfo pr_info;
    short pr_cursig;
    uint64_t pr_sigpend;
    uint64_t pr_sighold;

    int32_t pr_pid;
    int32_t pr_ppid;
    int32_t pr_pgrp;

    int32_t pr_sid;
    struct elf_timeval64 pr_utime;
    struct elf_timeval64 pr_stime;
    struct elf_timeval64 pr_cutime;
    struct elf_timeval64 pr_cstime;

    elf_arm64_gregset_t pr_reg;

    int pr_fpvalid;
};

struct elf_prpsinfo64 {
    char pr_state;
    char pr_sname;
    char pr_zomb;
    char pr_nice;
    uint64_t pr_flag;

    uint32_t pr_uid;
    uint32_t pr_gid;

    int32_t pr_pid;
    int32_t pr_ppid;
    int32_t pr_pgrp;
    int32_t pr_sid;

    char pr_fname[16];
    char pr_psargs[ELF_PRARGSZ];
};


/******************************** mini rdump ********************************/
#define MRDUMP_MINI_NR_SECTION 60
#define MRDUMP_MINI_SECTION_SIZE (32 * 1024)
#define MRDUMP_MINI_NR_MISC 40
#define MRDUMP_MINI_MISC_LOAD "load"
#define AEE_MTK_CPU_NUMS 16
#define NOTE_NAME_LONG 20
#define NOTE_NAME_SHORT 12
#define STACK_SIZE_32 (PAGE_SIZE * 2)
#define STACK_SIZE_64 (PAGE_SIZE * 4)

/*
 * struct thread_info { // kernel-4.14~5.15
 *     unsigned long          flags;//offset = 0
 *     int                    preempt_count; //offset = 4
 *     mm_segment_t           addr_limit; //offset = 8
 *     struct task_struct     *task; //offset= 12
 *     ...
 * }
 */
 #define TASK_OFF_IN_TI_32 12

struct pt_regs_32 {
    uint32_t uregs[ELF_ARM_NGREGS];
};

struct pt_regs_64 {
    uint64_t uregs[ELF_ARM64_NGREGS];
};

struct mrdump_mini_misc_data32 {
    unsigned int vaddr;
    unsigned int paddr;
    unsigned int start;
    unsigned int size;
};

struct mrdump_mini_misc_data64 {
    unsigned long long vaddr;
    unsigned long long paddr;
    unsigned long long start;
    unsigned long long size;
};

struct elf_arm64_pac_mask {
    uint64_t pauth_dmask;
    uint64_t pauth_cmask;
};

#define PAC_NOTE_NAME "LINUX"
#define PAC_NOTE_NAME_SZ 6

struct mrdump_mini_pac_mask {
    struct elf64_note note;
    char name[8];
    struct elf_arm64_pac_mask pac_mask64;
} __PACKED;

struct elf_arm64_auxv {
    uint64_t saved_auxv[4];
};

#define AUXV_NOTE_NAME "CORE"
#define AUXV_NOTE_NAME_SZ 5

struct mrdump_mini_auxv {
    struct elf64_note note;
    char name[8];
    struct elf_arm64_auxv auxv64;
} __PACKED;
/****************************** mini rdump end *******************************/

