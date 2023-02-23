/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#define DFD_MCU_SMC_MAGIC_SETUP             0x99716150
#define DFD_MCU_SMC_MAGIC_GET_STATUS        0x99716151
#define DFD_MCU_DUMP_SIZE                   0x02000000

#define OFFSET_DFD_VALID    0
#define DFD_VALID           (1 << OFFSET_DFD_VALID)
#define DFD_START           (1 << 0)
#define DFD_FINISH          (1 << 1)

#define DFD_ADDR_MAX        0xFFFFFFFF
#define DFD_ALIGN           0x00400000

enum {
    DFD_V1_0 = 10, /* no DFD internal dump support */
    DFD_V2_0 = 20,
    DFD_V2_5 = 25,
    DFD_V2_6 = 26,
    DFD_V3_0 = 30,
    DFD_V3_5 = 35,
};

enum {
    DFD_SW_V1 = 1,
    DFD_SW_V2 = 2,
    DFD_SW_V3 = 3,
};

enum {
    DFD_CORE_PWR_OFF = 0,
    DFD_CORE_PWR_ON,
    DFD_CORE_PWR_RETENTION,
};

struct platform_dfd_op {
    void (*plt_dfd_misc_decode)(const u64 *dfd_raw_data);
    unsigned int (*plt_dfd_misc_dump)(char *buf, unsigned int max_buf_size);
    void (*plt_dfd_lastpc_decode)(const u64 *dfd_raw_data);
    void (*plt_dfd_return_stack_decode)(const u64 *dfd_raw_data);
};

struct dfd_drv {
    unsigned int hw_version;
    unsigned int sw_version;
    unsigned long dfd_timeout;
    unsigned long long buf_addr;
    unsigned long buf_length;
    unsigned long buf_addr_align;
    unsigned long buf_addr_max;
    unsigned int nr_max_core;
    unsigned int nr_big_core;
    unsigned int nr_rs_entry_little;
    unsigned int nr_rs_entry_big;
    unsigned int nr_header_row;
    unsigned int chip_id_offset;
    unsigned int check_pattern_offset;
    char chip_id[8];
    int dfd_disable_devinfo_index;
    int dfd_disable_bit;
    unsigned int cachedump_en;
    unsigned long tap_en;
    struct platform_dfd_op plt_dfd_op;
    unsigned int initialized;
};

struct decoded_lastpc {
    unsigned long long power_state;
    unsigned long long pc;
    unsigned long long mode;
    unsigned long long fp_64;
    unsigned long long sp_64;
    unsigned long fp_32;
    unsigned long sp_32;
};

struct decoded_return_stack {
    unsigned long long ptr;
    unsigned long long *entry;
};

extern uint32_t dfd_status;
extern struct dfd_drv drv;
extern struct decoded_lastpc *lastpc;
extern struct decoded_return_stack *return_stack_little, *return_stack_big;

unsigned int dfd_internal_dump_before_reboot(void);
unsigned int dfd_internal_dump_get_decoded_lastpc(char *buf, unsigned int max_buf_size);
unsigned int dfd_get_decoded_return_stack(char *buf, unsigned int max_buf_size);

void dfd_mcu_init(void *fdt);
void dfd_mcu_init_aee(void *fdt);

void save_dfd_status(void);
