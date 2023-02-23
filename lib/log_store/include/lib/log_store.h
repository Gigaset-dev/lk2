/*
 * Copyright (c) 2020 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <sys/types.h>

#define SRAM_HEADER_SIG 0xabcd1234
#define DRAM_HEADER_SIG 0x5678ef90
#define LOG_STORE_SIG   0xcdab3412
#define LOG_EMMC_SIG    0x785690ef
#define FLAG_DISABLE    0X44495341 // acsii-DISA
#define FLAG_ENABLE     0X454E454E // acsii-ENEN
#define FLAG_INVALID    0xdeaddead


//  log flag
#define BUFF_VALID        0x01
#define CAN_FREE          0x02
#define NEED_SAVE_TO_EMMC 0x04
#define RING_BUFF         0x08
#define BUFF_FULL         0x10    // buf is full
#define ARRAY_BUFF        0X20    // array buf type, buf_point is the used buf end
#define BUFF_ALLOC_ERROR  0X40
#define BUFF_ERROR        0x80
#define BUFF_NOT_READY    0x100
#define BUFF_READY        0x200
#define BUFF_EARLY_PRINTK 0x400   // pl or lk can printk the early printk
#define BUFF_SRAM_START   0x800   /* pl fist phase reboot, need save SRAM buffer log */
#define BUFF_SRAM_SAVE    0x1000

#define LOG_STORE_SIZE 0X40000 /* Store log in DRAM buff 256KB */

/* total 32 bytes <= u32(4 bytes) * 8 = 32 bytes */
struct pl_lk_log {
    u32 sig;            // default 0xabcd1234
    u32 buff_size;      // total buf size
    u32 off_pl;         // pl offset, PLLK_LOG_SIZE
    u32 sz_pl;          // preloader size
    u32 pl_flag;        // pl log flag
    u32 off_lk;         // lk offset, sizeof((struct pl_lk_log) + sz_pl
    u32 sz_lk;          // lk log size
    u32 lk_flag;        // lk log flag
};

/* total 40 bytes <= u32(4 bytes) * 10 = 40 bytes */
struct dram_buf_header {
    u32 sig;
    u32 flag;
    u32 buf_addr;
    u32 buf_size;
    u32 buf_offsize;
    u32 buf_point;
    u32 klog_addr;
    u32 klog_size;
    u32 atf_log_addr;
    u32 atf_log_len;
};

/* total 256 bytes */
struct sram_log_header {
    u32 sig;
    u32 reboot_count;
    u32 save_to_emmc;
    struct dram_buf_header dram_buf;        // 40 bytes
    struct pl_lk_log pl_lk_log;    // 32 bytes
    u32 gz_log_addr;
    u32 gz_log_len;
    u32 reserve[41];   // reserve 41 * 4 char size(172 bytes)
    /* reserve[0] sram_log record log size */
    /* reserve[1] save block size for kernel use*/
    /* reserve[2] pmic save boot phase enable/disable */
    /* reserve[3] save history boot phase */
    /* reserve[4] save pllk itself log size*/
};
#define SRAM_RECORD_LOG_SIZE 0X00
#define SRAM_BLOCK_SIZE 0x01
#define SRAM_PMIC_BOOT_PHASE 0x02
#define SRAM_HISTORY_BOOT_PHASE 0x03
#define SRAM_PLLK_SIZE 0x04


void log_store_init(paddr_t paddr, u32 len);
struct sram_log_header *get_logstore_header(void);
char *get_logstore_buffer(void);
