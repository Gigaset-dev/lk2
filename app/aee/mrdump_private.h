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
#include <kernel/vm.h>
#include <lib/bio.h>
#include <platform/mrdump.h>
#include <platform/mrdump_params.h>
#include <stdint.h>

#define MRDUMP_LINUX_KERNEL_MEMORY "linux-kernel-memory"

/*
 * for ext4 and f2fs, InfoLBA (header), version 2
 * v1: support allocate size > 4G
 * v2: support timestamp
 */
#define MRDUMP_PAF_VERSION 0x0002

#define MRDUMP_PAF_FILE_CHECK    2
#define MRDUMP_PAF_INFO_LBA      4
#define MRDUMP_PAF_ADDR_LBA      8
#define MRDUMP_PAF_ALLOCSIZE    12
#define MRDUMP_PAF_COREDUMPSIZE 20
#define MRDUMP_PAF_TIMESTAMP    28
#define MRDUMP_PAF_CRC32        36
#define MRDUMP_LBA_DATAONLY     MRDUMP_PAF_CRC32
#define MRDUMP_PAF_TOTAL_SIZE   40

#define KZIP_ENTRY_MAX 64
#define LOCALHEADERMAGIC 0x04034b50UL
#define CENTRALHEADERMAGIC 0x02014b50UL
#define ZIP64ENDOFCENTRALDIRMAGIC 0x06064b50UL
#define ZIP64ENDOFCENTRALDIRLOCATORMAGIC 0x07064b50UL
#define ENDOFCENTRALDIRMAGIC 0x06054b50UL

#define MRDUMP_CORE_HEADER_SIZE (2*4096)
#define MRDUMP_EXPDB_DRAM_KLOG_OFFSET 3129344

enum MRDUMP_BDATA_STATE {
    BDATA_STATE_CHECK_PASS,
    BDATA_STATE_FILE_ACCESS_ERROR,
    BDATA_STATE_PARTITION_ACCESS_ERROR,
    BDATA_STATE_BLOCK_HEADER_ERROR,
    BDATA_STATE_NO_MEM_ERROR,
};

struct kzip_entry {
    char *filename;
    int level;
    uint64_t localheader_offset;
    uint64_t comp_size;
    uint64_t uncomp_size;
    uint32_t crc32;
};

struct kzip_file {
    uint64_t reported_size;
    uint64_t wdk_kick_size;
    uint64_t current_size;

    uint32_t entries_num;
    struct kzip_entry zentries[KZIP_ENTRY_MAX];
    void *handle;

    int (*write_cb)(void *handle, void *buf, int size);
};

#define MEM_NO_MAP 0
#define MEM_DO_MAP 1
#define EXPDB_FILE 2
struct kzip_addlist {
    uint64_t address;
    uint64_t size;
    uint32_t type;
    uint64_t memmap;
    uint64_t pageflags;
    uint32_t struct_page_size;
};

#define MRDUMP_CPU_MAX 12

typedef uint32_t arm32_gregset_t[18];
typedef uint64_t arm64_gregset_t[34];

struct arm32_ctrl_regs {
    uint32_t sctlr;
    uint64_t ttbcr;
    uint64_t ttbr0;
    uint64_t ttbr1;
};

struct arm64_ctrl_regs {
    uint64_t sctlr_el1;
    uint64_t sctlr_el2;
    uint64_t sctlr_el3;

    uint64_t tcr_el1;
    uint64_t tcr_el2;
    uint64_t tcr_el3;

    uint64_t ttbr0_el1;
    uint64_t ttbr0_el2;
    uint64_t ttbr0_el3;

    uint64_t ttbr1_el1;

    uint64_t sp_el[4];
};

struct mrdump_arm32_reg {
    arm32_gregset_t arm32_regs;
    struct arm32_ctrl_regs arm32_creg;
};

struct mrdump_arm64_reg {
    arm64_gregset_t arm64_regs;
    struct arm64_ctrl_regs arm64_creg;
};

struct mrdump_crash_record {
    int reboot_mode;

    char msg[128];

    uint32_t fault_cpu;

    union {
        struct mrdump_arm32_reg arm32_reg;
        struct mrdump_arm64_reg arm64_reg;
    } cpu_regs[0];
};

struct  __PACKED mrdump_ksyms_param {
    char     tag[4];
    uint32_t flag;
    uint32_t crc;
    uint64_t start_addr;
    uint32_t size;
    uint32_t addresses_off;
    uint32_t num_syms_off;
    uint32_t names_off;
    uint32_t markers_off;
    uint32_t token_table_off;
    uint32_t token_index_off;
};

struct mrdump_machdesc {
    uint32_t nr_cpus;

    uint64_t page_offset;
    uint64_t tcr_el1_t1sz;

    uint64_t kimage_vaddr;
    uint64_t dram_start;
    uint64_t dram_end;
    uint64_t kimage_stext;
    uint64_t kimage_etext;
    uint64_t kimage_stext_real;
    uint64_t kimage_voffset;
    uint64_t kernel_pac_mask;
    uint64_t unused1;

    uint64_t vmalloc_start;
    uint64_t vmalloc_end;

    uint64_t modules_start;
    uint64_t modules_end;

    uint64_t phys_offset;
    uint64_t master_page_table;

    uint64_t memmap;
    uint64_t pageflags;
    uint32_t struct_page_size;

    uint64_t dfdmem_pa;  // Reserved for DFD 3.0+

    struct mrdump_ksyms_param kallsyms;
};

struct mrdump_control_block {
    char sig[8];

    struct mrdump_machdesc machdesc;
    uint32_t machdesc_crc;

    uint32_t unused0;
    uint32_t output_fs_lbaooo;

    struct mrdump_crash_record crash_record;
};

struct kzip_file *kzip_open(void *handle, int (*write_cb)(void *handle, void *p, int size));
bool kzip_add_file(struct kzip_file *zf, const struct kzip_addlist *addlist, const char *zfilename);
bool kzip_close(struct kzip_file *zf);

struct mrdump_dev {
    const char *name;
    void *handle;
    bdev_t *bdev;

    bool (*read)(struct mrdump_dev *dev, uint64_t sector_addr, uint8_t *pdBuf, int32_t blockLen);
    bool (*write)(struct mrdump_dev *dev, uint64_t sector_addr, uint8_t *pdBuf, int32_t blockLen);
    uint64_t (*get_size)(struct mrdump_dev *dev);
};

struct mrdump_dev *mrdump_dev_emmc_ext4(void);
struct mrdump_dev *mrdump_dev_partition(const char *partname);

int mrdump_null_output(const struct mrdump_control_block *kparams,
    const struct kzip_addlist *memlist);
int mrdump_ext4_output(const struct mrdump_control_block *mrdump_cb,
    const struct kzip_addlist *memlist, const struct kzip_addlist *memlist_cmmd,
    struct mrdump_dev *mrdump_dev);
int mrdump_partition_output(const struct mrdump_control_block *mrdump_cb,
    const struct kzip_addlist *memlist, const struct kzip_addlist *memlist_cmmd,
    struct mrdump_dev *mrdump_dev);
int mrdump_usb_output(const struct mrdump_control_block *mrdump_cb,
    const struct kzip_addlist *memlist, const struct kzip_addlist *memlist_cmm);
#ifdef WITH_PLATFORM_MEDIATEK_COMMON_PCIE
int mrdump_pcie_output(const struct mrdump_control_block *mrdump_cb,
    const struct kzip_addlist *memlist, const struct kzip_addlist *memlist_cmm);
#endif

void mrdump_core_header_init(const struct mrdump_control_block *kparams,
    const struct kzip_addlist *memlist);

struct kzip_addlist *mrdump_memlist_fill_cmm(const struct mrdump_control_block *mrdump_cblock);
void mrdump_memlist_free_cmm(const struct kzip_addlist *memlist);

struct mrdump_dev *mrdump_dev_blkpart(const char *partname);
void mrdump_read_expdb(void *data, int len, u64 offset_src);

int aee_debug_kinfo_map(void);
void aee_debug_kinfo_unmap(void);
struct mrdump_control_block *aee_mrdump_get_info(void);

int mrdump_detection(void);
void mrdump_run(void);
uint64_t get_mpt(void);
