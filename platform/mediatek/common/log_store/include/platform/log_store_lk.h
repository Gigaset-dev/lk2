/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */


#pragma once

#include <stdint.h>
#include <sys/types.h>


#define EMMC_LOG_BUF_SIZE (0x200000)

/* emmc last block struct */
struct log_emmc_header {
    u32 sig;
    u32 offset;
    //u32 uart_flag;
    u32 reserve_flag[11];
    /* [0] used to save uart flag */
    /* [1] used to save emmc_log index */
    /* [2] used to save printk ratalimit  flag */
    /* [3] used to save kedump contrl flag */
    /* [4] used to save boot step */
};

enum EMMC_STORE_FLAG_TYPE {
    UART_LOG                = 0x00,
    LOG_INDEX               = 0X01,
    PRINTK_RATELIMIT        = 0X02,
    KEDUMP_CTL              = 0x03,
    BOOT_STEP               = 0x04,
    EMMC_STORE_FLAG_TYPE_NR,
};

#define BOOT_PHASE_MASK            0xf  // b1111
#define NOW_BOOT_PHASE_SHIFT       0x0
#define LAST_BOOT_PHASE_SHIFT      0x4
#define PMIC_BOOT_PHASE_SHIFT      0x8
#define PMIC_LAST_BOOT_PHASE_SHIFT 0xc

#define HEADER_INDEX_MAX 0x10

/* emmc store log */
struct emmc_log {
    u32 type;
    u32 start;
    u32 end;
};

#define LOG_PLLK        0x01
#define LOG_PL          0x02
#define LOG_KERNEL      0x03
#define LOG_ATF         0x04
#define LOG_GZ          0x05
#define LOG_LAST_KERNEL 0x06

#define BOOT_PHASE_PL             0x01
#define BOOT_PHASE_LK             0x02
#define BOOT_PHASE_KERNEL         0x03
#define BOOT_PHASE_ANDROID        0x04
#define BOOT_PHASE_PL_COLD_REBOOT 0X05
#define BOOT_PHASE_SUSPEND        0x06
#define BOOT_PHASE_RESUME         0X07
#define BOOT_PHASE_PRE_SUSPEND    0x08
#define BOOT_PHASE_EXIT_RESUME    0X09

void set_pllk_config(int type, int value);
void logstore_flush_header_cache(void);
