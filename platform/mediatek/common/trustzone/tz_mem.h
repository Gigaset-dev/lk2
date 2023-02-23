/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once

#include <platform/memory_layout.h>
#include <stddef.h>

/*************************************/
/*     tee-secmem memory layout      */
/*************************************/
/*                                   */
/*           tee-secmem reserved mem */
/*           base address            */
/*      0x0 ------------------------ */
/*          |  TF-A args (256B)    | */
/*    0x100 ------------------------ */
/*          |  TEE/TEEI args (256B)| */
/*    0x200 ------------------------ */
/*          |  SECMEM args (256B)  | */
/*    0x300 ------------------------ */
/*          |  TEEI args (256B)    | */
/*          ------------------------ */
/*            ...                    */
/*   0x1000 ------------------------ */
/*          |  TUI, I2C ibuf (1KB) | */
/*   0x2000 ------------------------ */
/*          |  TUI, I2C obuf (1KB) | */
/* 0x300000 ------------------------ */
/*          |  SPI (2MB)           | */
/* 0x500000 ------------------------ */
/*          |  M4U sec pgtbl (1MB) | */
/* 0x600000 ------------------------ */
/*          |  CMDQ (1MB)          | */
/* 0x700000 ------------------------ */
/*          |  Reserved (1MB)      | */
/*          ------------------------ */
/*************************************/

/* SEC MEM magic */
#define SEC_MEM_MAGIC                   (0x3C562817U)
/* SEC MEM version */
#define SEC_MEM_VERSION                 (0x00010000U)
/* Tplay Table Size */
#define SEC_MEM_TPLAY_TABLE_SIZE        (0x1000)//4KB by default
#define SEC_MEM_TPLAY_MEMORY_SIZE       (0x200000)//2MB by default
/* Reserved for other <t-driver */
#define SEC_MEM_RESERVED_M4U_SIZE       (0x100000)//1MB by default
#define SEC_MEM_RESERVED_CMDQ_SIZE      (0x100000)//1MB by default
#define SEC_MEM_RESERVED_SPI_SIZE       (0x200000)//2MB by default
#define SEC_MEM_RESERVED_NONE_SIZE      (0x100000)//1MB by default

/* Reserved for I2C <t-driver */
#define CFG_MEM_RESERVED_I2C_ENABLE     (1)
#define SEC_MEM_RESERVED_I2C_SIZE       (0x1000)//4KB by default

/* Maximum size of static reserved secure memory for t-driver */
#define SEC_MEM_RESERVED_MAX_SIZE       (0x700000)

struct {
    unsigned int magic;           // Magic number
    unsigned int version;         // version
    unsigned long long svp_mem_start;   // MM sec mem pool start addr.
    unsigned long long svp_mem_end;     // MM sec mem pool end addr.
    unsigned int tplay_table_start; //tplay handle-to-physical table start
    unsigned int tplay_table_size;  //tplay handle-to-physical table size
    unsigned int tplay_mem_start;   //tplay physcial memory start address for crypto operation
    unsigned int tplay_mem_size;    //tplay phsycial memory size for crypto operation
    unsigned int secmem_obfuscation;//MM sec mem obfuscation or not
    unsigned int msg_auth_key[8]; /* size of message auth key is 32bytes(256 bits) */
    unsigned int rpmb_size;       /* size of rpmb partition */
    unsigned int shared_secmem;     //indicate if the memory is shared between REE and TEE
    unsigned int m4u_mem_start;     //reserved start address of secure memory for m4u
    unsigned int m4u_mem_size;      //reserved size of secure memory for m4u
    unsigned int cmdq_mem_start;    //reserved start address of secure memory for cmdq
    unsigned int cmdq_mem_size;     //reserved size of secure memory for cmdq
    unsigned int spi_mem_start;    //reserved start address of secure memory for spi
    unsigned int spi_mem_size;     //reserved size of secure memory for spi
#if CFG_MEM_RESERVED_I2C_ENABLE
    unsigned int i2c_mem_start;    //reserved start address of secure memory for i2c
    unsigned int i2c_mem_size;     //reserved size of secure memory for i2c
#endif
    unsigned int emmc_rel_wr_sec_c;  //emmc ext_csd[222]
#if CFG_RPMB_KEY_PROGRAMED_IN_KERNEL
    unsigned int rpmb_setkey_flag;  //indicate if the auth key had been programmed
#endif
#if CFG_GZ_REMAP_OFFSET_ENABLE
    unsigned long long gz_remap_offset;           //ddr remap offset when gz is enabled
#endif
} sec_mem_arg_t;

