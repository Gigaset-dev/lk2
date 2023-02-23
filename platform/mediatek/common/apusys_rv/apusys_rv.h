/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once
#include <err.h>

#define APUSYS_SEC_MEM_ALIGN 0x100000  /* 1M (for secure IOMMU IOVA mapping alignment) */
#define APUSYS_AEE_COREDUMP_MEM_ALIGN 0x10000  /* 64K (minimal size for EMI MPU) */
#define APUSYS_MEM_LIMIT 0x90000000  /* max address can APUSYS remap */
#define APUSYS_MEM_LOWER_BOUND 0x240000000
#define APUSYS_MEM_UPPER_BOUND 0x340000000
#define APUSYS_FW_ALIGN 16 /* for mdla dma alignment limitation */

/* image name definition */
#define IMG_NAME_APUSYS_A       "tinysys-apusys-RV33_A"

/******************************************************************************
 * 1. New entries must be appended to the end of the structure.
 * 2. Do NOT use conditional option such as #ifdef inside the structure.
 */
enum MTK_APUSYS_SMC_CALL_OP {
    MTK_APUSYS_SMC_OP_DEVAPC_INIT = 0,
    MTK_APUSYS_SMC_OP_APUSYS_RV_INIT,
    MTK_APUSYS_SMC_OP_APUSYS_RV_SETUP_SECURE_MEM,
    MTK_APUSYS_SMC_OP_APUSYS_RV_SETUP_AEE_COREDUMP_MEM,
    MTK_APUSYS_SMC_OP_APUSYS_PWR_INIT,
    MTK_APUSYS_SMC_OP_APUSYS_RV_SETUP_MBOX_MPU,
    MTK_APUSYS_SMC_OP_SECURITY_CTRL_INIT,
    MTK_APUSYS_SMC_OP_NUM
};

enum PT_ID_APUSYS {
    PT_ID_APUSYS_FW,
    PT_ID_APUSYS_XFILE,
    PT_ID_MDLA_FW_BOOT,
    PT_ID_MDLA_FW_MAIN,
    PT_ID_MDLA_XFILE,
    PT_ID_MVPU_FW,
    PT_ID_MVPU_XFILE,
    PT_ID_MVPU_SEC_FW,
    PT_ID_MVPU_SEC_XFILE
};

struct apusys_secure_info_t {
    unsigned int total_sz;
    unsigned int up_code_buf_ofs;
    unsigned int up_code_buf_sz;

    unsigned int up_fw_ofs;
    unsigned int up_fw_sz;
    unsigned int up_xfile_ofs;
    unsigned int up_xfile_sz;
    unsigned int mdla_fw_boot_ofs;
    unsigned int mdla_fw_boot_sz;
    unsigned int mdla_fw_main_ofs;
    unsigned int mdla_fw_main_sz;
    unsigned int mdla_xfile_ofs;
    unsigned int mdla_xfile_sz;
    unsigned int mvpu_fw_ofs;
    unsigned int mvpu_fw_sz;
    unsigned int mvpu_xfile_ofs;
    unsigned int mvpu_xfile_sz;
    unsigned int mvpu_sec_fw_ofs;
    unsigned int mvpu_sec_fw_sz;
    unsigned int mvpu_sec_xfile_ofs;
    unsigned int mvpu_sec_xfile_sz;

    unsigned int up_coredump_ofs;
    unsigned int up_coredump_sz;
    unsigned int mdla_coredump_ofs;
    unsigned int mdla_coredump_sz;
    unsigned int mvpu_coredump_ofs;
    unsigned int mvpu_coredump_sz;
    unsigned int mvpu_sec_coredump_ofs;
    unsigned int mvpu_sec_coredump_sz;
};

struct apusys_aee_coredump_info_t {
    unsigned int up_coredump_ofs;
    unsigned int up_coredump_sz;
    unsigned int regdump_ofs;
    unsigned int regdump_sz;
    unsigned int mdla_coredump_ofs;
    unsigned int mdla_coredump_sz;
    unsigned int mvpu_coredump_ofs;
    unsigned int mvpu_coredump_sz;
    unsigned int mvpu_sec_coredump_ofs;
    unsigned int mvpu_sec_coredump_sz;

    unsigned int up_xfile_ofs;
    unsigned int up_xfile_sz;
    unsigned int mdla_xfile_ofs;
    unsigned int mdla_xfile_sz;
    unsigned int mvpu_xfile_ofs;
    unsigned int mvpu_xfile_sz;
    unsigned int mvpu_sec_xfile_ofs;
    unsigned int mvpu_sec_xfile_sz;
};

#define PART_HEADER_DEFAULT_ADDR    (0xFFFFFFFF)
#define LOAD_ADDR_MODE_BACKWARD     (0x00000000)
#define PART_MAGIC          0x58881688
#define EXT_MAGIC           0x58891689
#define PT_MAGIC            0x58901690

struct ptimg_hdr_t {
    unsigned int magic;     /* magic number*/
    unsigned int hdr_size;  /* header size */
    unsigned int img_size;  /* img size */
    unsigned int align;     /* alignment */
    unsigned int id;        /* image id */
    unsigned int addr;      /* memory addr */
};

status_t apu_load_apusys_rv(void *fdt);
