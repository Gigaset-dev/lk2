/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

/* TEEI Magic For Interface */
#define TEEI_BOOTCFG_MAGIC (0x434d4254)

/* TEEI argument version */
#define TEE_ARGUMENT_VERSION (0x00010000U)

#define KEY_LEN 32

#define SEC_APXGPT1_CON       0x220
#define SEC_APXGPT1_PRESCALE  0x224
#define SEC_APXGPT1_COUNTER_L 0x228
#define SEC_APXGPT1_COUNTER_H 0x22C

//prize add by lipengpeng 20220704 start 

#define CFG_MICROTRUST_FP_SUPPORT

//prize add by lipengpeng 20220704 end 
enum device_type {
    MT_UNUSED = 0,
    MT_UART16550 = 1,    /* secure uart */
    MT_SEC_GPT,         /* secure gp timer */
    MT_SEC_WDT,         /* secure watch dog */
    MT_SEC_CRYPTO,
    MT_SEC_SPI,       /* secure spi */
};

enum isee_flags {
    /* default value */
    ISEE_FLAGS_DEFAULT_VALUE            = 0x0000000000000000,

    /* bit 0~1 */
    ISEE_FLAGS_LOG_PATH_OFFSET          = 0,
    ISEE_FLAGS_LOG_PATH_BUFFER          = 0x0,
    ISEE_FLAGS_LOG_PATH_UART            = 0x1,
    ISEE_FLAGS_LOG_PATH_LOG_SERVER      = 0x2,
    ISEE_FLAGS_LOG_PATH_RESERVED        = 0x3,
    ISEE_FLAGS_LOG_PATH_MASK            = 0x0000000000000003,

    /* bit 2~4 */
    ISEE_FLAGS_LOG_LEVEL_OFFSET         = 2,
    ISEE_FLAGS_LOG_LEVEL_ALL            = 0x0,
    ISEE_FLAGS_LOG_LEVEL_TRACE          = 0x1,
    ISEE_FLAGS_LOG_LEVEL_DEBUG          = 0x2,
    ISEE_FLAGS_LOG_LEVEL_INFO           = 0x3,
    ISEE_FLAGS_LOG_LEVEL_WARNING        = 0x4,
    ISEE_FLAGS_LOG_LEVEL_ERROR          = 0x5,
    ISEE_FLAGS_LOG_LEVEL_NO_LOG         = 0x6,
    ISEE_FLAGS_LOG_LEVEL_RESERVED2      = 0x7,
    ISEE_FLAGS_LOG_LEVEL_MASK           = 0x000000000000001c,

    /* bit 5 */
    ISEE_FLAGS_SOTER_TYPE_OFFSET        = 5,
    ISEE_FLAGS_SOTER_TYPE_MINIMAL       = 0x0,
    ISEE_FLAGS_SOTER_TYPE_NORMAL        = 0x1,
    ISEE_FLAGS_SOTER_TYPE_MASK          = 0x0000000000000020,

    /* bit 6~7 */
    ISEE_FLAGS_VERIFY_MODE_OFFSET       = 6,
    ISEE_FLAGS_VERIFY_MODE_DISABLE_COUNTER = 0x0,
    ISEE_FLAGS_VERIFY_MODE_ENABLE_COUNTER = 0x1,
    ISEE_FLAGS_VERIFY_MODE_RESERVED0   = 0x2,
    ISEE_FLAGS_VERIFY_MODE_RESERVED1   = 0x3,
    ISEE_FLAGS_VERIFY_MODE_MASK        = 0x00000000000000C0,

    /* bit 8 */
    ISEE_FLAGS_SECURE_STORAGE_OFFSET    = 8,
    ISEE_FLAGS_SECURE_STORAGE_RPMB      = 0x0,
    ISEE_FLAGS_SECURE_STORAGE_FS        = 0x1,
    ISEE_FLAGS_SECURE_STORAGE_MASK      = 0x0000000000000100,

    /* bit 9~24 */
    ISEE_FLAGS_SOTER_FEATURE_OFFSET     = 9,
    ISEE_FLAGS_SOTER_FEATURE_RESET      = 0x0,
    ISEE_FLAGS_SOTER_FEATURE_TUI        = 0x1,
    ISEE_FLAGS_SOTER_FEATURE_GP         = 0x2,
    ISEE_FLAGS_SOTER_FEATURE_DRM        = 0x4,
    ISEE_FLAGS_SOTER_FEATURE_AARCH64    = 0x8,
    ISEE_FLAGS_SOTER_FEATURE_MULTI_TA   = 0x10,
    ISEE_FLAGS_SOTER_FEATURE_EFUSEDISABLE = 0x20,
    ISEE_FLAGS_SOTER_FEATURE_RESERVED4  = 0x40,
    ISEE_FLAGS_SOTER_FEATURE_RESERVED5  = 0x80,
    ISEE_FLAGS_SOTER_FEATURE_RESERVED6  = 0x100,
    ISEE_FLAGS_SOTER_FEATURE_RESERVED7  = 0x200,
    ISEE_FLAGS_SOTER_FEATURE_RESERVED8  = 0x400,
    ISEE_FLAGS_SOTER_FEATURE_RESERVED9  = 0x800,
    ISEE_FLAGS_SOTER_FEATURE_RESERVED10 = 0x1000,
    ISEE_FLAGS_SOTER_FEATURE_RESERVED11 = 0x2000,
    ISEE_FLAGS_SOTER_FEATURE_RESERVED12 = 0x4000,
    ISEE_FLAGS_SOTER_FEATURE_RESERVED13 = 0x8000,
    ISEE_FLAGS_SOTER_FEATURE_MASK       = 0x0000000000FFFE00,

    /* bit 25 */
    ISEE_FLAGS_SOTER_SYSTEM_DEBUG_OFFSET = 25,
    ISEE_FLAGS_SOTER_SYSTEM_DEBUG_CLOSE  = 0x0,
    ISEE_FLAGS_SOTER_SYSTEM_DEBUG_OPEN   = 0x1,
    ISEE_FLAGS_SOTER_SYSTEM_DEBUG_MASK   = 0x0000000002000000,

    /* bit 26 */
    ISEE_FLAGS_RPMB_KEY_PROGRAM_OFFSET   = 26,
    ISEE_FLAGS_RPMB_KEY_IS_PROGRAMMED    = 0x0,
    ISEE_FLAGS_RPMB_KEY_NOT_PROGRAMMED   = 0x1,
    ISEE_FLAGS_RPMB_KEY_PROGRAM_MASK     = 0x0000000004000000,

    /* bit 27 */
    ISEE_FLAGS_BOOT_MODE_OFFSET        = 27,
    ISEE_FLAGS_BOOT_RELEASE_MODE       = 0x0,
    ISEE_FLAGS_BOOT_DEBUG_MODE         = 0x1,
    ISEE_FLAGS_BOOT_MODE_MASK          = 0x0000000008000000,

    /* bit 28 */
    ISEE_FLAGS_SYSTRACE_MODE_OFFSET    = 28,
    ISEE_FLAGS_SYSTRACE_DISABLE_MODE   = 0x0,
    ISEE_FLAGS_SYSTRACE_ENABLE_MODE    = 0x1,
    ISEE_FLAGS_SYSTRACE_MODE_MASK      = 0x0000000010000000,
//prize add by lipengpeng 20220704 start 

    /* Add this for enc ta log start */
    /* bit 29 */
    ISEE_FLAGS_LOG_CRYPTO_OFFSET    = 29,
    ISEE_FLAGS_LOG_CRYPTO_DISABLE_MODE   = 0b0,
    ISEE_FLAGS_LOG_CRYPTO_ENABLE_MODE    = 0b1,
    ISEE_FLAGS_LOG_CRYPTO_MASK      = 0x0000000020000000,
    /* Add this for enc ta log end */
	
//prize add by lipengpeng 20220704 end 	
};

/*
 * because different 32/64 system or different align rules ,
 * we use attribute packed  ,only init once so not speed probelm,
 */
struct tee_dev {
    u32 dev_type;       /* secure device type ,enum device_type */
    u64 base_addr;      /* secure deivice base address */
    u32 intr_num;       /* irq number for device */
    u32 apc_num;        /* secure  device apc (secure attribute) */
    u32 param[3];       /* others paramenter ,baudrate,speed,etc */
} __PACKED;

struct tee_arg {
    unsigned int magic;                 /* magic value from information */
    unsigned int length;                /* size of struct in bytes. */
    unsigned long long tee_log_base;    /* TEE Log Buffer*/
    unsigned long long secDRamBase;     /* Secure DRAM start address */
    unsigned long long secDRamSize;     /* Secure DRAM size */
    unsigned long long gic_distributor_base;
    unsigned long long gic_redistributor_base;
    unsigned int gic_version;
    unsigned int total_number_spi;
    unsigned int ssiq_number[3];
    struct tee_dev tee_dev[5];
    unsigned long long flags;
    unsigned int chip_hw_code;
    unsigned long long share_mem_paddr;
    unsigned long long share_mem_size;
} __PACKED;

struct tee_keys {
    u32 magic;
    u32 version;
    u8 rpmb_key[KEY_LEN];    /* rpmb key */
    u8 hw_id[16];            /* modify hwid is 16byte */
    u8 hr_id[KEY_LEN];        /* modify hrid is 32byte */
    u32 hrid_size;            /* modify hrid is size 4byte */
    u32 hwid_size;            /* hwid size */
};

#ifdef CFG_MICROTRUST_FP_SUPPORT
/*before use spi on TEE, set right SPI_INDEX for current platform */
#define SPI_INDEX   74

//#define DEBUG_ENABLE
#ifdef DEBUG_ENABLE
    #define DEBUG (1 << 3)
#else
    #define DEBUG 0
#endif

//#define LOOPBACK_ENABLE
#ifdef LOOPBACK_ENABLE
    #define LOOPBACK (1 << 4)
#else
    #define LOOPBACK 0
#endif
#endif

#define TEE_LOG_BUF_SIZE  0x20000
#define TEEI_RESERVED_REE_SHAREMEM_SIZE (12*1024*1024) /* 12MB */

/**************************************************************************
 * EXPORTED FUNCTIONS
 **************************************************************************/
void teei_boot_param_prepare(u64 param_addr, u64 tee_entry, u64 teei_sec_dram_size,
    u64 dram_base, u64 dram_size);
/* extern unsigned long rtcgettime_teei(void); */
