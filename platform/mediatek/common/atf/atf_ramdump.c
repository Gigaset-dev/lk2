/*
 * Copyright (c) 2021 MediaTek Inc. All Rights Reserved.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <arch/arm/mmu.h>
#include <arch/mmu.h>
#include <arch/ops.h>
#include <debug.h>
#include <err.h>
#include <init_mtk.h>
#include <kernel/vm.h>
#include <libfdt.h>
#include <lk/init.h>
#include <mblock.h>
#include <platform/mboot_expdb.h>
#include <platform/mrdump_params.h>
#include <smc.h>
#include <smc_id_table.h>
#include <sys/types.h>

#define ATF_CRASH_MAGIC_NO                    0xdead1abf
#define ATF_LAST_LOG_MAGIC_NO                 0x41544641
#define ATF_DUMP_DONE_MAGIC_NO                0xd07ed07e
#define ATF_SMC_UNK                           0xffffffff

#define ATF_RAMDUMP_BUFFER_SIZE               (0x80000) /* 512KB */
#define ATF_RAMDUMP_BUFFER_LIMIT_ADDR         (0xC0000000)
#define ATF_RAMDUMP_BUFFER_ADDR_ALIGNMENT     (0x10000)

/* each 8 bytes, support 16 types */
#define ATF_OP_ID_GET_FOOTPRINT_BUF_INFO      (0x0)
#define MAX_FOOTPRINT_TYPE_STRING_SIZE        8
#define MAX_SUPPORTED_FOOTPRINT_TYPE          16
#define MAX_CORE_COUNT                        32
#define FOOTPRINT_MAGIC_NUM                   0xCA7FF007

#define ATF_RET_SUCCESS 0
#define ATF_RET_UNKNOWN -1

#define INFO_LOG(fmt, args...)    dprintf(INFO, fmt, ##args)
#define ALWAYS_LOG(fmt, args...)    dprintf(ALWAYS, fmt, ##args)

struct footprint_header {
    uint32_t size;
    uint32_t magic;
};

struct footprint_package {
    struct footprint_header header;
    /* 4 bytes alignment */
    uint8_t name_string[MAX_FOOTPRINT_TYPE_STRING_SIZE];    /* 8 */
    uint32_t footprint[MAX_CORE_COUNT];                    /* 16*/
};

static u64 atf_ramdump_addr;
static u64 atf_ramdump_size;

static u64 atf_log_buf_addr;
static u64 atf_log_buf_size;
static u64 atf_crash_flag_addr;

static u64 atf_footprint_addr;
static u64 atf_footprint_buf_size;

static void *atf_log_buf_addr_va;
static void *footprint_map_addr_va;
static void *atf_ramdump_addr_va;
static void *atf_crash_flag_addr_va;

static void get_atf_ramdump_memory(void)
{
    uint32_t address_hi, address_lo, size_hi, size_lo;
    u64 limit_addr = (u64)ATF_RAMDUMP_BUFFER_LIMIT_ADDR;
    u64 align_addr = (u64)ATF_RAMDUMP_BUFFER_ADDR_ALIGNMENT;
    struct __smccc_res res;

#if LK_AS_BL33
    atf_ramdump_size = ATF_RAMDUMP_BUFFER_SIZE;
    atf_ramdump_addr = mblock_alloc((u64) atf_ramdump_size, (u64)align_addr, limit_addr, 0, 0,
                        "atf_ramdump");
#elif LK_AS_AEE
    mrdump_get_reserved_info("atf_ramdump", &atf_ramdump_addr, &atf_ramdump_size);
#endif

    if (atf_ramdump_addr) {
        address_hi = (atf_ramdump_addr >> 32) & 0xFFFFFFFF;
        address_lo = atf_ramdump_addr & 0xFFFFFFFF;
        size_hi = 0;
        size_lo = ATF_RAMDUMP_BUFFER_SIZE & 0xFFFFFFFF;
        __smc_conduit(MTK_SIP_RAM_DUMP_ADDR, address_hi, address_lo, size_hi, size_lo,
                      0, 0, 0, &res);

        ALWAYS_LOG("ATF: ram dump address hi:0x%x, address lo:0x%x, size hi:%u, size lo:%u\n",
                    address_hi, address_lo, size_hi, size_lo);
    } else {
        ALWAYS_LOG("ATF: Can't reserve atf_ramdump memory, atf_ramdump_addr=0x%llx!\n",
                    atf_ramdump_addr);
    }
}

static void save_atf_log(CALLBACK dev_write)
{
    uint32_t datasize = 0;
    int ret;

    /* ATF log is located in DRAM, we must allocate it first */
    ret = vmm_alloc_physical(vmm_get_kernel_aspace(),
            "vm-atf-log",
            ROUNDUP(atf_log_buf_size, PAGE_SIZE),
            &atf_log_buf_addr_va,
            PAGE_SIZE_SHIFT,
            ROUNDDOWN((paddr_t)atf_log_buf_addr, PAGE_SIZE),
            0,
            ARCH_MMU_FLAG_CACHED);

    if (ret != NO_ERROR)
        dprintf(CRITICAL, "ATF: vmm_alloc_physical failed\n");

    datasize = dev_write(vaddr_to_paddr(atf_log_buf_addr_va), atf_log_buf_size);

    /* Clear ATF crash flag */
    writel(ATF_DUMP_DONE_MAGIC_NO, atf_crash_flag_addr_va);
    arch_clean_cache_range(ROUNDDOWN((paddr_t)atf_crash_flag_addr_va, PAGE_SIZE), PAGE_SIZE);

    if (!datasize)
        dprintf(CRITICAL, "ATF: Log dump FAIL\n");
    else
        dprintf(INFO, "ATF: dev_w:%u, log_buf pa:0x%llx va:0x%p, size:%llx, crash_flag:0x%x\n",
                datasize, atf_log_buf_addr, atf_log_buf_addr_va,
                atf_log_buf_size, readl(atf_crash_flag_addr_va));
}
AEE_EXPDB_INIT_HOOK(SYS_ATF_LOG, 0x40000, save_atf_log);

static void save_atf_footprint(CALLBACK dev_write)
{
    uint32_t datasize = 0;
    uint32_t i = 0;
    uint64_t footprint_map_addr;
    uint64_t footprint_map_size;
    uint64_t addr_offset, size_offset;
    uint32_t local_strlen = 0;
    int ret;

    char *ptr_footprint_buf_start = 0;
    /* for pointer of footprint buffer address */
    char *ptr_footprint_buf_pointer = 0;
    struct footprint_package *ptr_fp_pkg, *ptr_next_fp_pkg;

    /* allocate temp buffer to parcking footprint data */
    ptr_footprint_buf_start = malloc(atf_footprint_buf_size);

    if (atf_footprint_addr > 0 && ptr_footprint_buf_start != NULL) {
        //ptr_fp_pkg = (struct footprint_package *)atf_footprint_addr;

        /* round down */
        addr_offset = (uint64_t)(atf_footprint_addr & (PAGE_SIZE - 1));
        if (addr_offset > 0)
            footprint_map_addr = (atf_footprint_addr - addr_offset); /* round down */
        else
            footprint_map_addr = atf_footprint_addr;

        /* round up */
        size_offset = (atf_footprint_buf_size & (PAGE_SIZE - 1));
        if (size_offset > 0)
            footprint_map_size = (atf_footprint_buf_size - size_offset) + PAGE_SIZE;
        else
            footprint_map_size = atf_footprint_buf_size;

        ret = vmm_alloc_physical(vmm_get_kernel_aspace(),
            "vm-atf-footprint",
            footprint_map_size,
            &footprint_map_addr_va,
            PAGE_SIZE_SHIFT,
            (paddr_t)footprint_map_addr,
            0,
            ARCH_MMU_FLAG_CACHED);

        if (ret != NO_ERROR)
            dprintf(CRITICAL, "ATF: vmm_alloc_physical failed\n");

        /* locate working buffer by VA */
        ptr_fp_pkg = (struct footprint_package *)(footprint_map_addr_va + addr_offset);

        /* clean whole buffer */
        memset((void *)ptr_footprint_buf_start,
                0x0,
                atf_footprint_buf_size);

        /* traverse footprint_package*/
        ptr_footprint_buf_pointer = ptr_footprint_buf_start;
        while (ptr_fp_pkg->header.magic == FOOTPRINT_MAGIC_NUM) {
            /* cast to void * for byte count */
            ptr_next_fp_pkg = (struct footprint_package *)
                ((void *)ptr_fp_pkg + ptr_fp_pkg->header.size);

            /* format the value to buffer */
            local_strlen = strlen(&ptr_fp_pkg->name_string[0]);
            if (local_strlen > 0 &&
                local_strlen < MAX_FOOTPRINT_TYPE_STRING_SIZE) {
                ret = snprintf(ptr_footprint_buf_pointer, atf_footprint_buf_size > local_strlen ?
                    local_strlen : atf_footprint_buf_size, &ptr_fp_pkg->name_string[0]);
                if (ret < 0)
                    dprintf(CRITICAL, "ATF: fp buffer name copy failed\n");
                /* string and = */
                ptr_footprint_buf_pointer += strlen(ptr_footprint_buf_pointer);
                /* traverse footprint value*/
                i = 0;
                while (&ptr_fp_pkg->footprint[i] < ptr_next_fp_pkg) {
                    /* print comma */
                    if (&ptr_fp_pkg->footprint[i+1] < ptr_next_fp_pkg) {
                        ret = sprintf(ptr_footprint_buf_pointer, "[%d]=0x%8x,",
                                i, ptr_fp_pkg->footprint[i]);
                        if (ret < 0)
                            dprintf(CRITICAL, "ATF: fp pkg copy [%d], failed\n", i);
                    } else {
                        ret = sprintf(ptr_footprint_buf_pointer, "[%d]=0x%8x\n",
                                i, ptr_fp_pkg->footprint[i]);
                        if (ret < 0)
                            dprintf(CRITICAL, "ATF: fp pkg copy [%d] failed\n", i);
                    }
                    /* int and ,*/
                    ptr_footprint_buf_pointer += strlen(ptr_footprint_buf_pointer);
                    i++;
                }
            } /* if (0< local_strlen < MAX_FOOTPRINT_TYPE_STRING_SIZE) */

            /* point to next footprint_package*/
            ptr_fp_pkg = ptr_next_fp_pkg;
        } /* while (ptr_fp_pkg->header.magic == FOOTPRINT_MAGIC_NUM) */

        /* flush and invalidate memory */
        arch_sync_cache_range(ptr_footprint_buf_start, atf_footprint_buf_size);

        /* scratch whole memory and write it into db file */
        datasize = dev_write(vaddr_to_paddr(ptr_footprint_buf_start), atf_footprint_buf_size);

        if (!datasize)
            dprintf(CRITICAL, "ATF: footprint FAIL\n");
        else
            dprintf(CRITICAL, "ATF: dev_write:%u, atf fp addr pa:0x%llx va:0x%p size:0x%llx\n",
                datasize, atf_footprint_addr, footprint_map_addr_va, atf_footprint_buf_size);
    } /* End of if (atf_footprint_addr > 0) { */
    /* free memory */
    if (ptr_footprint_buf_start)
        free(ptr_footprint_buf_start);
}
AEE_EXPDB_INIT_HOOK(SYS_ATF_FOOTPRINT, 0x800, save_atf_footprint);

static void save_atf_rdump(CALLBACK dev_write)
{
    uint32_t datasize = 0;
    int ret;

    ret = vmm_alloc_physical(vmm_get_kernel_aspace(),
            "vm-atf-rdump",
            ROUNDUP(atf_ramdump_size, PAGE_SIZE),
            &atf_ramdump_addr_va,
            PAGE_SIZE_SHIFT,
            ROUNDDOWN((paddr_t)atf_ramdump_addr, PAGE_SIZE),
            0,
            ARCH_MMU_FLAG_CACHED);

    if (ret != NO_ERROR)
        dprintf(CRITICAL, "ATF: vmm_alloc_physical failed\n");

    datasize = dev_write(vaddr_to_paddr(atf_ramdump_addr_va), atf_ramdump_size);

    if (!datasize)
        dprintf(CRITICAL, "ATF: RDUMP FAIL\n");
    else
        dprintf(CRITICAL, "ATF: dev_write:%u, atf ramdump addr pa:0x%llx va:0x%p, size:0x%llx\n",
            datasize, atf_ramdump_addr, atf_ramdump_addr_va, atf_ramdump_size);
}
AEE_EXPDB_INIT_HOOK(SYS_ATF_RDUMP, 0x80000, save_atf_rdump);

void atf_log_init(void)
{
    uint32_t smc_id = 0;
    uint32_t op_id = 0;
    struct __smccc_res res;
    u64 atf_footprint_hi_addr = 0;
    u64 atf_footprint_low_addr = 0;
    int ret;

    /* reserve ramdump memory and pass to ATF*/
    get_atf_ramdump_memory();

    /* get log buffer and footprint buffer from ATF */
    smc_id = MTK_SIP_LK_DUMP_ATF_LOG_INFO;
    __smc_conduit(smc_id, 0, 0, 0, 0, 0, 0, 0, &res);
    atf_log_buf_addr = res.a0;
    atf_log_buf_size = res.a1;
    atf_crash_flag_addr = res.a2;
    dprintf(CRITICAL, "ATF: log_buf_addr = 0x%llx, size = 0x%llx, crash_flag_addr = 0x%llx\n",
                atf_log_buf_addr, atf_log_buf_size, atf_crash_flag_addr);

    smc_id = MTK_SIP_BL_ATF_CONTROL_AARCH32;
    op_id = ATF_OP_ID_GET_FOOTPRINT_BUF_INFO;
    __smc_conduit(smc_id, 0, 0, 0, 0, 0, 0, 0, &res);
    atf_footprint_hi_addr = res.a0;
    atf_footprint_low_addr = res.a1;
    atf_footprint_buf_size = res.a2;
    dprintf(CRITICAL, "ATF: footprint hi_addr = 0x%llx, low_addr = 0x%llx, buf_size = 0x%llx\n",
            atf_footprint_hi_addr, atf_footprint_low_addr, atf_footprint_buf_size);

    /* atf_footprint_hi_addr may return -1  */
    if (atf_footprint_hi_addr == ATF_SMC_UNK) {
        dprintf(CRITICAL, "atf_footprint_hi_addr=0x%llx(ATF_SMC_UNK)\n", atf_footprint_hi_addr);
        atf_footprint_addr = 0;
        atf_footprint_buf_size = 0;
    } else {    /* Get footprint addr */
        atf_footprint_addr = (atf_footprint_hi_addr << 32) | atf_footprint_low_addr;
    }

    if ((atf_log_buf_addr >> 32) == ATF_SMC_UNK) {
        dprintf(CRITICAL, "LK Dump: ATF LOG INIT not supported\n");
    } else {
        ret = vmm_alloc_physical(vmm_get_kernel_aspace(),
            "vm-atf-crashflag",
            PAGE_SIZE,
            &atf_crash_flag_addr_va,
            PAGE_SIZE_SHIFT,
            ROUNDDOWN((paddr_t)atf_crash_flag_addr, PAGE_SIZE),
            0,
            ARCH_MMU_FLAG_CACHED);

        if (ret != NO_ERROR)
            dprintf(CRITICAL, "ATF: vmm_alloc_physical failed\n");

        /* round down */
        atf_crash_flag_addr_va += (atf_crash_flag_addr & (PAGE_SIZE - 1));

        /* backward compatible for legacy chips */
        if (readl(atf_crash_flag_addr_va) == ATF_CRASH_MAGIC_NO)
            dprintf(CRITICAL, "ATF: CRASH BUFF\n");
        else if (readl(atf_crash_flag_addr_va) == ATF_LAST_LOG_MAGIC_NO)
            dprintf(CRITICAL, "ATF: LAST BUFF\n");
        else
            dprintf(CRITICAL, "ATF: RAW BUFF\n");
    }
}
MT_LK_INIT_HOOK_AEE(atf_log, atf_log_init, LK_INIT_LEVEL_PLATFORM);
MT_LK_INIT_HOOK_BL33(atf_log, atf_log_init, LK_INIT_LEVEL_PLATFORM);
