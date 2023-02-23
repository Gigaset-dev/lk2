/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once

#include <platform/gpueb_plat.h>
#include <sys/types.h>

struct gpueb_region_info_st {
    uint32_t ap_loader_start;
    uint32_t ap_loader_size;
    uint32_t ap_firmware_start;
    uint32_t ap_firmware_size;
    uint32_t regdump_start;
    uint32_t regdump_size;
    uint32_t task_content_pt;
};

struct MemoryDumpGPUEB {
    /*gpueb sram*/
    char l2tcm[MDUMP_L2TCM_SIZE];
    //char l1c[MDUMP_L1C_SIZE];
    /*gpueb reg*/
    char regdump[MDUMP_REGDUMP_SIZE];
    char tbuf[MDUMP_TBUF_SIZE];
    //char dram[MDUMP_DRAM_SIZE];
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
    unsigned int loader_start;    /* gpueb loader start address*/
    unsigned int loader_size;     /* gpueb loader size */
    unsigned int fw_start;        /* gpueb fw start address */
    unsigned int fw_size;         /* gpueb fw size */
    unsigned int rdmp_start;      /* gpueb ramdump start address */
    unsigned int rdmp_size;       /* gpueb ramdump size */
    unsigned int task_content_pt; /* task content pt */
};

struct gpueb_info_t {
    unsigned int gpueb_dm_ofs; /* for gpueb data memory */
    unsigned int gpueb_dm_sz;
    unsigned int rd_ofs;      /* for gpueb ramdump */
    unsigned int rd_sz;
    vaddr_t xfile_addr;  /* for LKdump */
    unsigned int xfile_sz;
};

enum gpueb_smc_bl_op {
    GPUEB_SMC_BL_OP_SHAREDRAM = 0,
    GPUEB_SMC_BL_OP_NUMBER,
};
