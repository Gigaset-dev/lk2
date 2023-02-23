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

#include <debug.h>
#include <sys/types.h>

enum MBOOT_PARAMS_DEF_TYPE {
    MBOOT_PARAMS_DEF_UNKNOWN = 0,
    MBOOT_PARAMS_DEF_SRAM,
    MBOOT_PARAMS_DEF_DRAM,
};

enum AEE_EXP_TYPE_NUM {
    AEE_EXP_TYPE_HWT = 1,
    AEE_EXP_TYPE_KE = 2,
    AEE_EXP_TYPE_NESTED_PANIC = 3,
    AEE_EXP_TYPE_SMART_RESET = 4,
    AEE_EXP_TYPE_HANG_DETECT = 5,
    AEE_EXP_TYPE_BL33_CRASH = 6,
    AEE_EXP_TYPE_BL2_EXT_CRASH = 7,
    AEE_EXP_TYPE_AEE_LK_CRASH = 8,
    AEE_EXP_TYPE_TFA_CRASH = 9,
};

struct mboot_params_buffer {
    u32 sig;
    /* for size comptible */
    u32 off_pl;
    u32 off_lpl; /* last preloader */
    u32 sz_pl;
    u32 off_lk;
    u32 off_llk; /* last lk */
    u32 sz_lk;
    u32 padding[2]; /* size = 2 * 16 = 32 byte */
    u32 dump_step;
    u32 sz_buffer;
    u32 off_linux;
    u32 off_console;
    u32 padding2[3];
};

struct kedump_crc {
    unsigned int mboot_params_crc;
    unsigned int pstore_crc;
};

enum {
    AEE_LKDUMP_CLEAR = 0,
    AEE_LKDUMP_RAMCONSOLE_RAW,
    AEE_LKDUMP_PSTORE_RAW,
    AEE_LKDUMP_KEDUMP_CRC,
    AEE_LKDUMP_MINI_RDUMP,
    //new added before this line please
    AEE_LKDUMP_UNKNOWN
};

#define AEE_IPANIC_MAGIC            0xaee0dead
#define AEE_IPANIC_PHDR_VERSION     0x12

#define IPANIC_NR_SECTIONS          64

// ipanic partation
struct ipanic_data_header {
    u32 type;       /* data type(0-31) */
    u32 valid;      /* set to 1 when dump succeded */
    u32 offset;     /* offset in EXPDB partition */
    u32 used;       /* valid data size */
    u32 total;      /* allocated partition size */
    u32 encrypt;    /* data encrypted */
    u32 raw;        /* raw data or plain text */
    u32 compact;    /* data and header in same block, to save space */
    u8 name[32];
};

struct ipanic_header {
    u32 magic;
    u32 version;    /* ipanic version */
    u32 size;       /* ipanic_header size */
    u32 datas;      /* bitmap of data sections dumped */
    u32 dhblk;      /* data header blk size, 0 if no dup data headers */
    u32 blksize;
    u32 partsize;   /* expdb partition totoal size */
    u32 bufsize;
    u64 buf;
    u8 exp_type;
    u32 wdt_status;
    struct ipanic_data_header data_hdr[IPANIC_NR_SECTIONS];
};

#define MBOOT_PARAMS_EXP_TYPE_MAGIC 0xaeedead0
#define MBOOT_PARAMS_EXP_TYPE_DEC(exp_type) \
    ((exp_type ^ MBOOT_PARAMS_EXP_TYPE_MAGIC) < 16 ? \
            exp_type ^ MBOOT_PARAMS_EXP_TYPE_MAGIC : exp_type)

#define MEM_MAGIC1 0x61646472 // "addr"
#define MEM_MAGIC2 0x73697a65 // "size"

/* use LOG to recored important log into DB file and print it too */
#define LOG(fmt, ...)           \
    do { \
        zaee_log(fmt, ##__VA_ARGS__);       \
        dprintf(ALWAYS, fmt, ##__VA_ARGS__); \
    } while (0)

u32 aee_ddr_reserve_ready(void);
u32 aee_ddr_reserve_enable(void);
u32 aee_ddr_reserve_success(void);

bool mboot_params_get_wdt_status(unsigned int *wdt_status);
bool mboot_params_get_fiq_step(unsigned int *fiq_step);
bool mboot_params_get_exp_type(unsigned int *exp_type);
bool mboot_params_set_exp_type(unsigned int exp_type);
bool mboot_params_current_abnormal_boot(void);
void mboot_params_lk_save(unsigned int val, int index);
void mboot_params_addr_size(addr_t *addr, unsigned int *size);
void sram_plat_dbg_info_addr_size(addr_t *addr, unsigned int *size);
void pstore_addr_size(addr_t *addr, unsigned int *size);
void zaee_logbuf_addr_size(addr_t *addr, unsigned int *size);
int zaee_log(const char *fmt, ...);
void mrdump_mini_header_addr_size(addr_t *addr, unsigned int *size);
void mboot_params_set_dump_step(unsigned int step);
int mboot_params_get_dump_step(void);
bool mboot_params_reboot_by_mrdump_key(void);
unsigned int calculate_crc32(void *data, unsigned int len);
