/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <debug.h>
#include <err.h>
#include <kernel/thread.h>
#include <kernel/vm.h>
#include <list.h>
#include <lib/bio.h>
#include <lib/log_store.h>
#include <lib/pl_boottags.h>
#include <lk/init.h>
#include <mblock.h>
#include <platform.h>
#include <string.h>

#define MOD "LOG_STORE_LK"
#define BOOT_TAG_MBOOT_PARAMS_INFO   0x8861001C
#define BUFF_SIZE 0x5000 /* 20k buff for early log store*/

static struct sram_log_header *log_header;
static char *pbuff;
static bool log_store_enable = true;
static char buff_early[BUFF_SIZE];

#ifdef DEBUG_LOG
#define LOG_DEBUG(fmt, ...) \
do {                        \
    log_store_enable = false; \
    dprintf(INFO, fmt, ##__VA_ARGS__); \
    log_store_enable = true;    \
} while (0)
#else
#define LOG_DEBUG(fmt, ...)
#endif

#define LOG_ERROR(fmt, ...) \
do {                        \
    log_store_enable = false; \
    dprintf(ALWAYS, fmt, ##__VA_ARGS__); \
    log_store_enable = true;    \
} while (0)

enum {
    LOG_WRITE = 0x1,        /* Log is write to buff */
    LOG_READ_KERNEL = 0x2,  /* Log have readed by kernel */
    LOG_WRITE_EMMC = 0x4,   /* log need save to emmc */
    LOG_EMPTY = 0x8,        /* log is empty */
    LOG_FULL = 0x10,        /* log is full */
    LOG_PL_FINISH = 0X20,   /* pl boot up finish */
    LOG_LK_FINISH = 0X40,   /* lk boot up finish */
    LOG_DEFAULT = LOG_WRITE_EMMC|LOG_EMPTY,
} BLOG_FLAG;

enum MBOOT_PARAMS_DEF_TYPE {
    MBOOT_PARAMS_DEF_UNKNOWN = 0,
    MBOOT_PARAMS_DEF_SRAM,
    MBOOT_PARAMS_DEF_DRAM,
};

struct boot_tag_mboot_params_info {
    u32 sram_addr;
    u32 sram_size;
    u32 def_type;
    u32 memory_info_offset;
};

struct mboot_params_memory_info {
    u32 magic1;
    u32 sram_plat_dbg_info_addr;
    u32 sram_plat_dbg_info_size;
    u32 sram_log_store_addr;
    u32 sram_log_store_size;
    u32 mrdump_addr;
    u32 mrdump_size;
    u32 dram_addr;
    u32 dram_size;
    u32 pstore_addr;
    u32 pstore_size;
    u32 pstore_console_size;
    u32 pstore_pmsg_size;
    u32 mrdump_mini_header_addr;
    u32 mrdump_mini_header_size;
    u32 magic2;
};

static void log_store_cb(print_callback_t *cb, const char *str, size_t len)
{
    static int early_len;

    /* log disable or len is 0*/
    if (log_store_enable == false || len == 0)
        return;

    /* dram buff not enable, store to static memory */
    if (pbuff == NULL && early_len != -1) {
        if (early_len == BUFF_SIZE)
            return;

        if (early_len + len > BUFF_SIZE)
            len = BUFF_SIZE - early_len;

        memcpy((char *)(buff_early + early_len), str, len);
        early_len += len;
        return;
    }

    if (((log_header->dram_buf.flag & BUFF_READY) != BUFF_READY)
        || ((log_header->dram_buf.flag & BUFF_FULL) == BUFF_FULL)
        || (pbuff == NULL))
        return;

    /* Dram buff enable, copy early log to dram */
    if (early_len > 0) {
        if ((log_header->pl_lk_log.off_lk + log_header->pl_lk_log.sz_lk + early_len)
            > log_header->dram_buf.buf_size)
            early_len = log_header->dram_buf.buf_size - log_header->pl_lk_log.off_lk -
                log_header->pl_lk_log.sz_lk;

        memcpy((char *)(pbuff + log_header->pl_lk_log.off_lk + log_header->pl_lk_log.sz_lk),
            buff_early, early_len);
        log_header->pl_lk_log.sz_lk += early_len;
        LOG_DEBUG("%s:early buff %d, copy to DRAM.\n", MOD, early_len);
        early_len = -1;
    }

    if (log_header->pl_lk_log.off_lk + log_header->pl_lk_log.sz_lk + len >
        log_header->dram_buf.buf_size) {
        len = log_header->dram_buf.buf_size - log_header->pl_lk_log.off_lk
            - log_header->pl_lk_log.sz_lk;
        log_header->dram_buf.flag |= BUFF_FULL;
    }

    memcpy((char *)(pbuff + log_header->pl_lk_log.off_lk + log_header->pl_lk_log.sz_lk), str, len);
    log_header->pl_lk_log.sz_lk += len;
}

static print_callback_t cb = {
    LIST_INITIAL_VALUE(cb.entry),
    log_store_cb,
    NULL
};

static void log_header_init(void)
{
    LOG_DEBUG("%s:lk init start.\n", MOD);

    /* SRAM buff header init */
    if (log_header->sig != SRAM_HEADER_SIG) {
        LOG_ERROR("%s:sram header 0x%x is not match: %d!\n", MOD,
            (unsigned int)log_header, log_header->sig);
        memset(log_header, 0, sizeof(struct sram_log_header));
        log_header->sig = SRAM_HEADER_SIG;
    }

    if (log_header->dram_buf.sig != DRAM_HEADER_SIG) {
        LOG_ERROR("%s:sram_dram_buff 0x%p, sig 0x%x, flag 0x%x, reset.\n", MOD,
            &(log_header->dram_buf), (unsigned int)log_header->dram_buf.sig,
            (unsigned int)log_header->dram_buf.flag);
        memset(&(log_header->dram_buf), 0, sizeof(struct dram_buf_header));
        log_header->dram_buf.sig = DRAM_HEADER_SIG;
        log_header->dram_buf.flag = BUFF_ERROR;
        memset(&(log_header->pl_lk_log), 0, sizeof(struct pl_lk_log));
        log_header->pl_lk_log.sig = LOG_STORE_SIG;
    }

    if (log_header->pl_lk_log.sig != LOG_STORE_SIG) {
        LOG_ERROR("%s: BUFF_ERROR, sig 0x%x, buff_size 0x%x, off_pl 0x%x.\n", MOD,
            log_header->pl_lk_log.sig, log_header->dram_buf.buf_size, log_header->pl_lk_log.off_pl);
        memset(&(log_header->pl_lk_log), 0, sizeof(struct pl_lk_log));
        log_header->pl_lk_log.sig = LOG_STORE_SIG;
    }

    //pbuff = (char *)paddr_to_kvaddr((paddr_t)log_header->dram_buf.buf_addr);
    LOG_DEBUG("%s:sram buff header 0x%x, sig 0x%x, buff_size 0x%x\n", MOD,
        (unsigned int)log_header, (unsigned int)log_header->dram_buf.sig,
        (unsigned int)log_header->dram_buf.buf_size);
    LOG_DEBUG("%s:pl log size 0x%x@0x%x, lk log size 0x%x@0x%x!\n", MOD,
        (unsigned int)log_header->pl_lk_log.sz_pl, (unsigned int)log_header->pl_lk_log.off_pl,
        (unsigned int)log_header->pl_lk_log.sz_lk, (unsigned int)log_header->pl_lk_log.off_lk);

    if (log_header->pl_lk_log.sz_pl + log_header->pl_lk_log.off_pl
        >= log_header->dram_buf.buf_size) {
        LOG_ERROR("%s: buff full pl size 0x%x.\n", MOD, log_header->pl_lk_log.sz_pl);
        log_header->dram_buf.flag |= BUFF_FULL;
    }

    log_header->pl_lk_log.off_lk = log_header->pl_lk_log.off_pl + log_header->pl_lk_log.sz_pl;
    log_header->pl_lk_log.lk_flag = LOG_DEFAULT;

    LOG_DEBUG("%s: header ready.\n", MOD);
}


/* init function do two things
 * 1. register print callback
 * 2. init log store header
 */

void log_store_init(paddr_t paddr, u32 len)
{
    log_header = (struct sram_log_header *)paddr_to_kvaddr(paddr);
    if (log_header == NULL || len < sizeof(struct sram_log_header)) {
        LOG_ERROR("%s: log store init failed.\n", MOD);
        return;
    }

    log_header_init();

    register_print_callback(&cb);
}

static void log_store_buf_map(unsigned int level)
{
    unsigned int start;
    unsigned int logsize;
    status_t err;
    void *vaddr = NULL;

    if (log_header == NULL)
        return;

    if (log_header->dram_buf.buf_addr != 0 && log_header->dram_buf.buf_size != 0) {
        log_store_enable = false;
        start = ROUNDDOWN((uint32_t)log_header->dram_buf.buf_addr, PAGE_SIZE);
        logsize = ROUNDUP(((uint32_t)log_header->dram_buf.buf_addr - start
            + log_header->dram_buf.buf_size), PAGE_SIZE);
        log_store_enable = true;
        LOG_DEBUG("%s:dram pl/lk log buff mapping start addr = 0x%x, size = 0x%x\n",
            MOD, start, logsize);
    } else {
        log_store_enable = false;
        logsize = ROUNDUP(LOG_STORE_SIZE, PAGE_SIZE);
        start = mblock_alloc(logsize, PAGE_SIZE, MBLOCK_NO_LIMIT, 0x80000000, 1, "log_store_new");
        log_store_enable = true;
        if (!start) {
            LOG_ERROR("%s: mblock alloc error for log_store\n", MOD);
            return;
        }
        log_header->dram_buf.buf_addr = start;
        log_header->dram_buf.buf_size = logsize;
        log_header->dram_buf.flag = 0;
        LOG_DEBUG("%s:dram pl/lk log buff new mapping start addr = 0x%x, size = 0x%x\n",
            MOD, start, logsize);
    }

    log_store_enable = false;
    err = vmm_alloc_physical(vmm_get_kernel_aspace(), "log_store", logsize,
        &vaddr, PAGE_SIZE_SHIFT, (paddr_t)start, 0, ARCH_MMU_FLAG_CACHED);
    log_store_enable = true;
    if (err != NO_ERROR) {
        LOG_ERROR("%s: map error for log_store\n", MOD);
        return;
    }
    pbuff = (char *)vaddr;
    LOG_DEBUG("%s: dram map address %p.\n", MOD, pbuff);
    log_header->dram_buf.flag |= BUFF_READY;
}

static void mboot_params_pl_boottag_hook(struct boot_tag *tag)
{
    struct boot_tag_mboot_params_info mboot_params_info;
    struct mboot_params_memory_info *memory_info;

    memcpy(&mboot_params_info, &tag->data, sizeof(mboot_params_info));

    if (mboot_params_info.def_type != MBOOT_PARAMS_DEF_SRAM &&
            mboot_params_info.def_type != MBOOT_PARAMS_DEF_DRAM) {
        LOG_ERROR("unknown def type");
        return;
    }

    if (mboot_params_info.sram_addr == 0 || mboot_params_info.sram_size == 0) {
        LOG_ERROR("sram addr size zero");
        return;
    }

    if (mboot_params_info.memory_info_offset <= mboot_params_info.sram_size) {
        LOG_ERROR("memory offset invalid value");
        return;
    }

    memory_info = (struct mboot_params_memory_info *)(
            paddr_to_kvaddr(mboot_params_info.sram_addr) +
            mboot_params_info.memory_info_offset);
    LOG_DEBUG("mboot_params_info log_store hook: 0x%x, 0x%x, 0x%x, 0x%x\n",
            mboot_params_info.sram_addr, mboot_params_info.sram_size,
            mboot_params_info.def_type, mboot_params_info.memory_info_offset);

    if (memory_info->sram_log_store_addr != 0 && memory_info->sram_log_store_size != 0) {
        log_store_init((paddr_t)memory_info->sram_log_store_addr, memory_info->sram_log_store_size);
        LOG_ERROR("log_store: init log store addr:0x%x, size: 0x%x!\n",
            memory_info->sram_log_store_addr, memory_info->sram_log_store_size);
    }
}

struct sram_log_header *get_logstore_header()
{
    return log_header;
}

char *get_logstore_buffer()
{
    return pbuff;
}


PL_BOOTTAGS_INIT_HOOK(mboot_logstore_params_tag, BOOT_TAG_MBOOT_PARAMS_INFO,
                                    mboot_params_pl_boottag_hook);
LK_INIT_HOOK(log_store_buf_map, &log_store_buf_map, LK_INIT_LEVEL_VM - 1);
