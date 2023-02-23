/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <arch/ops.h>
#include <errno.h>
#include <lib/bio.h>
#include <lib/kcmdline.h>
#include <lib/log_store.h>
#include <lk/init.h>
#include <platform.h>
#include <platform_halt.h>
#include <platform_mtk.h>
#include <platform/log_store_lk.h>
#include <platform/mboot_expdb.h>
#include <pmic_psc.h>
#include <stdlib.h>
#include <string.h>
#include <trace.h>

#define LOCAL_TRACE 0
#define EXPDB_PARTITION "expdb"
#define MOD "LOG_STORE_LK"
static u32 last_boot_phase = FLAG_INVALID;


static void set_pmic_boot_phase(u32 boot_phase)
{
    struct sram_log_header *point = NULL;
    u8 last_value;

    point = get_logstore_header();
    if (point == NULL)
        return;

    boot_phase = boot_phase & BOOT_PHASE_MASK;
    point->reserve[SRAM_HISTORY_BOOT_PHASE] &= ~BOOT_PHASE_MASK;
    point->reserve[SRAM_HISTORY_BOOT_PHASE] |= boot_phase;

    last_value = pmic_get_swreg();
    last_value &= ~BOOT_PHASE_MASK;
    boot_phase |= last_value;
    pmic_set_swreg(boot_phase);
}

static u32 get_pmic_boot_phase(void)
{
    u8 last_value = 0;

    last_value = pmic_get_swreg();
    last_value = last_value >> LAST_BOOT_PHASE_SHIFT;
    if (last_value != 0)
        last_boot_phase = last_value;

    return last_value;
}

/* check emmc log_store config valid, or re-write it */
static bool emmc_config_valid(struct log_emmc_header *log_header, u64 block_size)
{
    bool ret = true;

    if (log_header->sig != LOG_EMMC_SIG) {
        memset((void *)log_header, 0, sizeof(struct log_emmc_header));
        log_header->sig = LOG_EMMC_SIG;
        return false;
    }

    if (log_header->offset >= EMMC_LOG_BUF_SIZE - block_size) {
        log_header->offset = 0;
        ret = false;
    }

    return ret;
}

/* read expdb partition and log config header info*/
static int read_emmc_config(struct log_emmc_header *log_header)
{
    bdev_t *dev;

    dev = bio_open_by_label(EXPDB_PARTITION);
    if (!dev) {
        dprintf(CRITICAL, "%s: Partition [%s] is not exist.\n", MOD, EXPDB_PARTITION);
        return -ENODEV;
    }


    bio_read(dev, (uchar *)log_header, dev->total_size - (off_t)dev->block_size,
         sizeof(struct log_emmc_header));

    if (emmc_config_valid(log_header, dev->block_size) == false) {
        LTRACEF("%s: %s write emmc\n", MOD, __func__);
        bio_write(dev, (u8 *)log_header, dev->total_size - dev->block_size,
             sizeof(struct log_emmc_header));
    }

    bio_close(dev);

    return 0;
}

void get_pllk_config(void)
{
    struct log_emmc_header log_header;

    memset(&log_header, 0, sizeof(log_header));
    if (read_emmc_config(&log_header) < 0) {
        LTRACEF("%s: config is invalid.\n", MOD);
        return;
    }

    if (log_header.reserve_flag[PRINTK_RATELIMIT] == FLAG_ENABLE) {
        kcmdline_append("printk.devkmsg=on");
    }
}

void set_pllk_config(int type, int value)
{
    bdev_t *dev = NULL;
    struct log_emmc_header log_header;

    if (type > EMMC_STORE_FLAG_TYPE_NR) {
        LTRACEF("%s: config type %d is invalid.\n", MOD, type);
        return;
    }

    memset(&log_header, 0, sizeof(log_header));
    if (read_emmc_config(&log_header) < 0) {
        LTRACEF("%s: config type %d is invalid.\n", MOD, type);
        return;
    }

    dev = bio_open_by_label(EXPDB_PARTITION);
    if (!dev) {
        dprintf(CRITICAL, "%s: Partition [%s] is not exist.\n", MOD, EXPDB_PARTITION);
        return;
    }

    if (type == UART_LOG || type == PRINTK_RATELIMIT || type == KEDUMP_CTL) {
        if (value)
            log_header.reserve_flag[type] = FLAG_ENABLE;
        else
            log_header.reserve_flag[type] = FLAG_DISABLE;
    } else {
        log_header.reserve_flag[type] = value;
    }
    LTRACEF("%s:%s config type %d value %d.\n", MOD, __func__, type, value);
    bio_write(dev, (u8 *)&log_header, dev->total_size - (off_t)dev->block_size,
        sizeof(struct log_emmc_header));
    bio_close(dev);
}

static void set_boot_phase(u32 boot_step)
{
    struct log_emmc_header log_header;
    bdev_t *dev = NULL;
    struct sram_log_header *point = NULL;

    point = get_logstore_header();
    if (point == NULL)
        return;

    boot_step = boot_step & BOOT_PHASE_MASK;

    if (point->reserve[SRAM_PMIC_BOOT_PHASE] == FLAG_ENABLE) {
        set_pmic_boot_phase(boot_step);
        if (last_boot_phase == FLAG_INVALID)
            get_pmic_boot_phase();
    } else {
        point->reserve[SRAM_HISTORY_BOOT_PHASE] &= ~BOOT_PHASE_MASK;
        point->reserve[SRAM_HISTORY_BOOT_PHASE] |= boot_step;
    }

    memset(&log_header, 0, sizeof(struct log_emmc_header));
    if (read_emmc_config(&log_header) < 0)
        return;

    // get last boot phase
    if (last_boot_phase == FLAG_INVALID)
        last_boot_phase = (log_header.reserve_flag[BOOT_STEP] >> LAST_BOOT_PHASE_SHIFT)
            & BOOT_PHASE_MASK;
    // clear now boot phase
    log_header.reserve_flag[BOOT_STEP] &= ~BOOT_PHASE_MASK;
    // set now boot phase
    log_header.reserve_flag[BOOT_STEP] |= boot_step;

    dev = bio_open_by_label(EXPDB_PARTITION);
    if (!dev) {
        dprintf(CRITICAL, "%s: Partition [%s] is not exist.\n", MOD, EXPDB_PARTITION);
        return;
    }

    bio_write(dev, (uchar *)&log_header, dev->total_size - (off_t)dev->block_size,
        sizeof(struct log_emmc_header));

    LTRACEF("%s:get last boot flag= 0x%x\n", MOD, last_boot_phase);
    bio_close(dev);
}

static u32 get_last_boot_phase(void)
{
    return last_boot_phase;
}

static void save_pllk_log(void)
{
    void *addr = NULL;
    size_t size = 0, emmc_remain_buf_size = 0;
    off_t offset = 0;
    bdev_t *dev = NULL;
    struct log_emmc_header log_header;
    struct emmc_log emmc_log;
    struct sram_log_header *point = NULL;

    point = get_logstore_header();
    addr = (void *)get_logstore_buffer();

    if (point == NULL || addr == NULL) {
        dprintf(CRITICAL, "%s: sram log header is null, 0x%p:0x%p.\n", MOD, point, addr);
        return;
    }

    LTRACEF("%s: start save pllk log.\n", MOD);

    memset(&log_header, 0, sizeof(log_header));
    if (read_emmc_config(&log_header) < 0) {
        dprintf(CRITICAL, "%s: read_emmc_config not correct.\n", MOD);
        return;
    }

    size = point->pl_lk_log.off_pl + point->pl_lk_log.sz_pl + point->pl_lk_log.sz_lk;

    if (size > EMMC_LOG_BUF_SIZE/4) {
        /* store size max 0.25 emm log, now is 512K */
        addr = addr + (size - EMMC_LOG_BUF_SIZE/4);
        size = EMMC_LOG_BUF_SIZE/4;
    }

    if (size % 4 != 0)
        size = size + 4 - size % 4;

    dev = bio_open_by_label(EXPDB_PARTITION);
    if (!dev) {
        dprintf(CRITICAL, "%s: Partition [%s] is not exist.\n", MOD, EXPDB_PARTITION);
        return;
    }

    emmc_remain_buf_size = EMMC_LOG_BUF_SIZE - dev->block_size - log_header.offset;
    emmc_log.start = log_header.offset;

    LTRACEF("%s: remain buf %zu, size %zu, header offset %u.\n",
        MOD, emmc_remain_buf_size, size, log_header.offset);
    LTRACEF("%s: part_size %lld, block size %zu.\n", MOD, dev->total_size, dev->block_size);
    if (size > emmc_remain_buf_size) {
        offset = dev->total_size - (off_t)EMMC_LOG_BUF_SIZE + (off_t)log_header.offset;
        bio_write(dev, addr, offset, emmc_remain_buf_size);
        offset = dev->total_size - (off_t)EMMC_LOG_BUF_SIZE;
        bio_write(dev, addr + emmc_remain_buf_size, offset, size - emmc_remain_buf_size);
        log_header.offset = size - emmc_remain_buf_size;
    } else {
        offset = dev->total_size - (off_t)EMMC_LOG_BUF_SIZE + (off_t)log_header.offset;
        bio_write(dev, addr, offset, size);
        log_header.offset = log_header.offset + size;
    }

    emmc_log.type = LOG_PLLK;
    emmc_log.end = log_header.offset;
    offset = dev->total_size - (off_t)dev->block_size + (off_t)sizeof(log_header)
        + (off_t)log_header.reserve_flag[LOG_INDEX] * (off_t)sizeof(struct emmc_log);
    LTRACEF("%s: config write emmc.\n", MOD);
    bio_write(dev, &emmc_log, offset, sizeof(struct emmc_log));
    log_header.reserve_flag[LOG_INDEX] += 1;
    log_header.reserve_flag[LOG_INDEX] = log_header.reserve_flag[LOG_INDEX] % HEADER_INDEX_MAX;

    /* re-write offset to config*/
    LTRACEF("%s: config re-write emmc.\n", MOD);
    offset = dev->total_size - (off_t)dev->block_size;
    bio_write(dev, &log_header, offset, sizeof(struct log_emmc_header));
    LTRACEF("%s: save pllk log size 0x%zx, offset 0x%x.\n", MOD, size, emmc_log.start);
    bio_close(dev);
}

static status_t logstore_sync_callback(void *data,
    platform_halt_action suggested_action, platform_halt_reason reason)
{
    struct sram_log_header *log_header = NULL;
    void *addr = NULL;

    log_header = get_logstore_header();
    if (log_header == NULL)
        return -1;
    arch_clean_invalidate_cache_range((addr_t)(log_header), sizeof(struct sram_log_header));

    addr = (void *)get_logstore_buffer();
    if (addr == NULL)
        return -1;
    arch_clean_invalidate_cache_range((addr_t)(addr), log_header->dram_buf.buf_size);

    return 0;
}

static status_t logstore_shutdown_callback(void *data,
    platform_halt_action suggested_action, platform_halt_reason reason)
{
    /* flush log_store header/buffer */
    logstore_sync_callback(NULL, 0, 0);
    save_pllk_log();
    return 0;
}

static void log_store_lk_init(unsigned int level)
{
    set_boot_phase(BOOT_PHASE_LK);
    register_platform_halt_callback("logstore_shutdown_callback",
                                    &logstore_shutdown_callback, NULL);
    get_pllk_config();
}

void logstore_flush_header_cache(void)
{
    logstore_sync_callback(NULL, 0, 0);
}

static void log_store_sync_sram(unsigned int level)
{
    register_platform_halt_callback("logstore_sync_callback",
                                    &logstore_sync_callback, NULL);
}


static void aee_save_pllk_log(CALLBACK dev_write)
{
    paddr_t buf;
    unsigned long len;
    struct sram_log_header *point = NULL;

    point = get_logstore_header();
    if (point == NULL)
        return;

    if (point->dram_buf.buf_addr == 0)
        return;

    buf = (paddr_t)(point->dram_buf.buf_addr + point->pl_lk_log.off_pl);
    len = point->pl_lk_log.sz_pl + point->pl_lk_log.sz_lk;

    if (!dev_write((uint64_t)buf, len))
        dprintf(CRITICAL, "pllk log dump fail\n");
}

AEE_EXPDB_INIT_HOOK(SYS_CUR_PLLK, 0x40000, aee_save_pllk_log);
LK_INIT_HOOK(log_store_lk_init, &log_store_lk_init, LK_INIT_LEVEL_TARGET - 1);
LK_INIT_HOOK(log_store_sync_sram, &log_store_sync_sram, LK_INIT_LEVEL_ARCH - 1);

