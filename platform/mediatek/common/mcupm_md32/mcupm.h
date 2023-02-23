/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once

#include <platform/addressmap.h>

#define MCUPM_DBG_MODE		0

#if MCUPM_DBG_MODE
#define LOCAL_TRACE		1
#define mcupm_dbg(f, ...)	dprintf("[MCUPM] " f, ##__VA_ARGS__)
#else
#define LOCAL_TRACE		0
#define mcupm_dbg(f, ...)	do {} while (0)
#endif

#define MCUPM_SW_RSTN		(MCUPM_BASE + 0x00000)
#define CFG_MCUPM_MEMADDR	(MCUPM_BASE + 0x01000)
#define SRAM_32KB_AP_BASE	(MCUPM_BASE + 0x1C000)
#define SRAM_REINIT_BASE	(MCUPM_BASE + 0x1FB40)

#define MCUPM_SRAM_SIZE		0xC000
#define SRAM_GPR_SIZE_4B	0x4
#define SRAM_32KB_SIZE		0x8000
#define SRAM_RESERVED_20B	0x00000014
#define SRAM_GPR_TOTAL_SIZE	100
#define MBOX_TABLE_TOTAL_SIZE	800
#define GPR_BASE_ADDR(x)	(MCUPM_SRAM_SIZE - \
				MBOX_TABLE_TOTAL_SIZE - \
				SRAM_GPR_TOTAL_SIZE + \
				(x * SRAM_GPR_SIZE_4B)) - \
				SRAM_32KB_SIZE + \
				SRAM_32KB_AP_BASE

#define MCUPM_INFO		GPR_BASE_ADDR(0)
#define MCUPM_ABNORMAL_BOOT	GPR_BASE_ADDR(1)
#define PLT_RD_MAGIC		GPR_BASE_ADDR(3)
#define MCUPM_LASTK_SZ		GPR_BASE_ADDR(7)
#define MCUPM_LASTK_ADDR	GPR_BASE_ADDR(8)
#define MCUPM_DATA_SZ		GPR_BASE_ADDR(9)
#define MCUPM_DATA_ADDR		GPR_BASE_ADDR(10)
#define MCUPM_RM_ADDR		GPR_BASE_ADDR(12)
#define MCUPM_RM_SZ		GPR_BASE_ADDR(13)
#define MCUPM_DM_ADDR		GPR_BASE_ADDR(14)
#define MCUPM_DM_SZ		GPR_BASE_ADDR(15)
#define MCUPM_LASTK_W_POS	GPR_BASE_ADDR(19)
#define MCUPM_LASTK_R_POS	GPR_BASE_ADDR(20)
#define MCUPM_LAST_PCLRSP	GPR_BASE_ADDR(21)
#define MCUPM_LAST_TBUF		GPR_BASE_ADDR(22)
#define MCUPM_WARM_BOOT		GPR_BASE_ADDR(23)

#define SRAM_CPUDVFS_INIT_PTBL	SRAM_REINIT_BASE
#define SRAM_CPUDVFS_INIT_VOLT_E	(SRAM_REINIT_BASE + 48)

#define MCUPM_PC		(MCUPM_SW_RSTN + 0x40)
#define MCUPM_AHB_STATUS	(MCUPM_SW_RSTN + 0x44)
#define MCUPM_AHB_M0_ADDR	(MCUPM_SW_RSTN + 0x48)
#define MCUPM_AHB_M1_ADDR	(MCUPM_SW_RSTN + 0x4C)
#define MCUPM_TBUF_WPTR		(MCUPM_SW_RSTN + 0x28)
#define MCUPM_TBUF_ADDR		(MCUPM_SW_RSTN + 0xD4)
#define MCUPM_TBUF_DATA31_0	(MCUPM_SW_RSTN + 0xE0)
#define MCUPM_TBUF_DATA63_32	(MCUPM_SW_RSTN + 0xE4)
#define MCUPM_TBUF_DATA95_64	(MCUPM_SW_RSTN + 0xE8)
#define MCUPM_TBUF_DATA127_96	(MCUPM_SW_RSTN + 0xEC)

#define CPUEB_CFGREG_DBG_MON_PC	0x184

#define AEE_LKDUMP_MCUPM_COREDUMP_SZ	0x40080
#define AEE_LKDUMP_MCUPM_DATA_SZ	0x400
#define AEE_LKDUMP_MCUPM_LAST_LOG_SZ	0x400

#define MDUMP_L2TCM_SIZE	MCUPM_SRAM_SIZE
#define MDUMP_REGDUMP_SIZE	0x003f00
#define MDUMP_TBUF_SIZE		0x000100
#define MDUMP_DRAM_SIZE		0x100000
#define MCUPM_TBUF_SIZE		0x80

#define INITIAL_MAGIC		0xdeadbeef

#define MCUPM_MEM_SIZE		0x00100000
#define MCUPM_MEM_ALIGN		0x00010000
#define MCUPM_MEM_LIMIT		0x80000000

#define PT_MAGIC		0x58901690
#define PT_ID_MCUPM_DM		0
#define PT_ID_MCUPM_PM		1
#define PT_ID_MCUPM_XFILE	99
#define MOD			"[MCUPMLDR]"

#define MCUPM_LOADER_SIZE	0x2000
#define MCUPM_TCM_SZ		0x000E0000
#define MCUPM_IMG_SZ		0x000C0000
#define MCUPM_IMG_OFS		0x2000
#define MCUPM_RDMP_OFS		0x20000

struct mcupm_region_info_st {
    uint32_t ap_loader_start;
    uint32_t ap_loader_size;
    uint32_t ap_firmware_start;
    uint32_t ap_firmware_size;
    uint32_t regdump_start;
    uint32_t regdump_size;
    uint32_t task_content_pt;
};

struct MemoryDumpMCUPM {
    /*mcupm sram*/
    char l2tcm[MDUMP_L2TCM_SIZE];
    //char l1c[MDUMP_L1C_SIZE];
    /*mcupm reg*/
    char regdump[MDUMP_REGDUMP_SIZE];
    char tbuf[MDUMP_TBUF_SIZE];
    char dram[MDUMP_DRAM_SIZE];
};

struct ptimg_hdr_t {
    unsigned int magic;     /* magic number*/
    unsigned int hdr_size;  /* data size */
    unsigned int img_size;  /* img size */
    unsigned int align;     /* align */
    unsigned int id;        /* image id */
    unsigned int addr;      /* memory addr */
};

struct region_info_t {
    unsigned int loader_start;    /* mcupm loader start address*/
    unsigned int loader_size;     /* mcupm loader size */
    unsigned int fw_start;        /* mcupm fw start address */
    unsigned int fw_size;         /* mcupm fw size */
    unsigned int rdmp_start;      /* mcupm ramdump start address */
    unsigned int rdmp_size;       /* mcupm ramdump size */
    unsigned int task_content_pt; /* task content pt */
};

struct mcupm_info_t {
    unsigned int mcupm_dm_ofs; /* for mcupm data memory */
    unsigned int mcupm_dm_sz;
    unsigned int rd_ofs;      /* for mcupm ramdump */
    unsigned int rd_sz;
    unsigned int xfile_addr;  /* for LKdump */
    unsigned int xfile_sz;
};

#define MCUPM_TBUF0_L      (MCUPM_SW_RSTN + 0x0E0)
#define MCUPM_TBUF0_H        (MCUPM_SW_RSTN + 0x0F0)
#define MMU_MEMORY_TYPE_NORMAL_WRITE_BACK               (0x7 << 2)
#define MMU_MEMORY_AP_P_RW_U_NA     (0x0 << 6)

#define SZ_2M  0x200000
#define SZ_4G  0x100000000

#define SHIFT_2M  (21)
#define SHIFT_16M (24)

#define SECTION_SIZE SZ_2M
#define AEE_MAP_SIZE SZ_2M
#define AEE_MAP_ALIGN SHIFT_2M
