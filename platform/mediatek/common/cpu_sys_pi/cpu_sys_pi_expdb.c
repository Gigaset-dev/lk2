/*
 * copyright (c) 2021 mediatek inc.
 *
 * use of this source code is governed by a mit-style
 * license that can be found in the license file or at
 * https://opensource.org/licenses/mit
 */
#include <kernel/vm.h> /* for vaddr_to_paddr */
#include <platform/cpu_sys_pi_expdb.h> /* for GPR_BASE_ADDR */
#include <platform/mboot_expdb.h> /* for AEE_EXPDB_INIT_HOOK */
#include <platform/reg.h> /* for read32 */
#include <string.h> /* for memcpy */

#define SYS_PI_DB_SIZE 0x80000


enum {
    kMCUPM_EEM_LOG_DB = 1,
    kMCUPM_PICACHU_LOG_DB,
    kMCUPM_SESv7_LOG_DB,
    kMCUPM_DVCS_LOG_DB,
    kMCUPM_KRAKEN_LOG_DB,
};

struct header {
    unsigned int magic;
    unsigned int item_size;
};

struct item {
    unsigned int idx;
    unsigned int data_offset;
    unsigned int data_size;
};


static unsigned int _dump_misc(unsigned char *addr_va)
{
    struct header header;
    struct item item;
    struct item *item_ptr;
    unsigned int offset = 0;
    unsigned int data_offset = 0;
    unsigned int i;
    unsigned int data_size;

    header.magic = 0xA5A5BEEF;
    header.item_size = 0;
    offset += sizeof(struct header);
    memset(&item, 0x0, sizeof(struct item));

#ifdef MCUPM_EEM_LOG_SIZE
    data_size = MCUPM_EEM_LOG_SIZE;
    if (data_size < SYS_PI_DB_SIZE)  {
        item.idx = kMCUPM_EEM_LOG_DB;
        item.data_size = MCUPM_EEM_LOG_SIZE; //2.5 KB
        memcpy(addr_va + offset, &item, sizeof(struct item));
        offset += sizeof(struct item);
        header.item_size++;
    }
#endif

#ifdef MCUPM_PICACHU_LOG_SIZE
    data_size = MCUPM_PICACHU_LOG_SIZE;
    if (data_size < SYS_PI_DB_SIZE)  {
        item.idx = kMCUPM_PICACHU_LOG_DB;
        item.data_size = MCUPM_PICACHU_LOG_SIZE; //3.5 KB
        memcpy(addr_va + offset, &item, sizeof(struct item));
        offset += sizeof(struct item);
        header.item_size++;
    }
#endif

#ifdef MCUPM_SES_LOG_SIZE
    data_size = MCUPM_SES_LOG_SIZE;
    if (data_size < SYS_PI_DB_SIZE)  {
        item.idx = kMCUPM_SESv7_LOG_DB;
        item.data_size = data_size; //2 KB
        memcpy(addr_va + offset, &item, sizeof(struct item));
        offset += sizeof(struct item);
        header.item_size++;
    }
#endif

#ifdef MCUPM_DVCS_LOG_SIZE
    data_size = MCUPM_DVCS_LOG_SIZE;
    if (data_size < SYS_PI_DB_SIZE)  {
        item.idx = kMCUPM_DVCS_LOG_DB;
        item.data_size = data_size; //2 KB
        memcpy(addr_va + offset, &item, sizeof(struct item));
        offset += sizeof(struct item);
        header.item_size++;
    }
#endif

#ifdef MCUPM_KRAKEN_LOG_SIZE
    data_size = MCUPM_KRAKEN_LOG_SIZE;
    if (data_size < SYS_PI_DB_SIZE)  {
        item.idx = kMCUPM_KRAKEN_LOG_DB;
        item.data_size = MCUPM_KRAKEN_LOG_SIZE;
        memcpy(addr_va + offset, &item, sizeof(struct item));
        offset += sizeof(struct item);
        header.item_size++;
    }
#endif

    memcpy(addr_va, &header, sizeof(struct header));
    data_offset = offset;
    for (i = 0; i < header.item_size; i++) {
        item_ptr = (struct item *)(addr_va + sizeof(struct header) + i * sizeof(struct item));
        item_ptr->data_offset = data_offset;
        data_offset += item_ptr->data_size;
    }

    return offset;
}

static void save_cpu_sys_pi_db(CALLBACK dev_write)
{
    unsigned int memory_dump_size = 0;
    unsigned char *addr_va = NULL;
    unsigned long addr = 0;
    unsigned int size = 0;

    addr_va = malloc(SYS_PI_DB_SIZE);
    if (addr_va == NULL) {
        dprintf(CRITICAL, "[CPU_SYS_PI] malloc failed\n");
        return;
    }
    memset(addr_va, 0x0, SYS_PI_DB_SIZE);
    memory_dump_size = _dump_misc(addr_va);

#ifdef MCUPM_EEM_LOG_SIZE
    addr = MCUPM_EEM_LOG_ADDR;
    size = MCUPM_EEM_LOG_SIZE;
    if (memory_dump_size + size > SYS_PI_DB_SIZE) {
        dprintf(CRITICAL, "[CPU_SYS_PI] MCUPM_EEM_LOG_SIZE=%d\n", size);
        goto out;
    }
    memcpy(addr_va + memory_dump_size, (void *)addr, size);
    memory_dump_size = memory_dump_size + size;
#endif

#ifdef MCUPM_PICACHU_LOG_SIZE
    addr = MCUPM_PICACHU_LOG_ADDR;
    size = MCUPM_PICACHU_LOG_SIZE;
    if (memory_dump_size + size > SYS_PI_DB_SIZE) {
        dprintf(CRITICAL, "[CPU_SYS_PI] MCUPM_PICACHU_LOG_SIZE size=%d\n", size);
        goto out;
    }
    memcpy(addr_va + memory_dump_size, (void *)addr, size);
    memory_dump_size = memory_dump_size + size;
#endif

#ifdef MCUPM_SES_LOG_SIZE
    addr = MCUPM_SES_LOG_ADDR;
    size = MCUPM_SES_LOG_SIZE;

    write32(MCUPM_SW_RSTN, read32(MCUPM_SW_RSTN) & 0xFFFFFFF7);
    if (memory_dump_size + size > SYS_PI_DB_SIZE) {
        dprintf(CRITICAL, "[CPU_SYS_PI] MCUPM_SES_LOG_SIZE size=%d\n", size);
        write32(MCUPM_SW_RSTN, read32(MCUPM_SW_RSTN) | 0x00000008);
        goto out;
    }
    memcpy(addr_va + memory_dump_size, (void *)addr, size);
    memory_dump_size = memory_dump_size + size;
    write32(MCUPM_SW_RSTN, read32(MCUPM_SW_RSTN) | 0x00000008);
#endif

#ifdef MCUPM_DVCS_LOG_SIZE
    addr = MCUPM_DVCS_LOG_ADDR;
    size = MCUPM_DVCS_LOG_SIZE;

    write32(MCUPM_SW_RSTN, read32(MCUPM_SW_RSTN) & 0xFFFFFFF7);
    if (memory_dump_size + size > SYS_PI_DB_SIZE) {
        dprintf(CRITICAL, "[CPU_SYS_PI] MCUPM_DVCS_LOG_SIZE size=%d\n", size);
        write32(MCUPM_SW_RSTN, read32(MCUPM_SW_RSTN) | 0x00000008);
        goto out;
    }
    memcpy(addr_va + memory_dump_size, (void *)addr, size);
    memory_dump_size = memory_dump_size + size;
    write32(MCUPM_SW_RSTN, read32(MCUPM_SW_RSTN) | 0x00000008);
#endif

#ifdef MCUPM_KRAKEN_LOG_SIZE
    addr = MCUPM_KRAKEN_LOG_ADDR;
    size = MCUPM_KRAKEN_LOG_SIZE;

    write32(MCUPM_SW_RSTN, read32(MCUPM_SW_RSTN) & 0xFFFFFFF7);
    if (memory_dump_size + size > SYS_PI_DB_SIZE) {
        dprintf(CRITICAL, "[CPU_SYS_PI] MCUPM_KRAKEN_LOG_SIZE size=%d\n", size);
        write32(MCUPM_SW_RSTN, read32(MCUPM_SW_RSTN) | 0x00000008);
        goto out;
    }
    memcpy(addr_va + memory_dump_size, (void *)addr, size);
    memory_dump_size = memory_dump_size + size;
    write32(MCUPM_SW_RSTN, read32(MCUPM_SW_RSTN) | 0x00000008);
#endif

out:
    dev_write(vaddr_to_paddr(addr_va), memory_dump_size);

    free(addr_va);

    return;
}

AEE_EXPDB_INIT_HOOK(SYS_CPU_SYS_PI_LOG, SYS_PI_DB_SIZE, save_cpu_sys_pi_db);
