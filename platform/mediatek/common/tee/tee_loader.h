/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <platform/memory_layout.h>

/* ATF arguments */
#define ATF_BOOTCFG_MAGIC (0x4D415446) // String MATF in little-endian
#define DEVINFO_SIZE 4

#define BOOT_TAG_MEID_INFO (0x8861002B)
#define ME_IDENTITY_LEN 16
#define HRID_INDEX 12

struct atf_arg {
    u32 atf_magic;
    u32 tee_support;
    u64 tee_entry;
    u64 tee_boot_arg_addr;
    u32 hwuid[4];      /* HW Unique id for t-base used */
    u32 HRID[8];       /* HW random id for t-base used */
    u32 devinfo[DEVINFO_SIZE];
};

struct mtk_bl_param_t {
    u64 bootarg_loc;
    u64 bootarg_size;
    u64 bl33_start_addr;
    u64 atf_arg_addr;
};

/**************************************************************************
 * TEE FUNCTIONS
 **************************************************************************/
#define TEE_RESERVED_NAME    "tee-reserved"
#define SECMEM_RESERVED_NAME "tee-secmem"
#define DEVICE_NAME_MAX_LEN 12

#define TEE_MEM_DEF_SIZE      (BL32_SIZE)    // Default Size : 50MB
#define TEE_MEM_MAX_SIZE      (0x80000000)   // Max Size : 2GB
#define TEE_MEM_ALIGN         (0x00200000)   // 2MB
#define TEE_MEM_LOWER         (0x240000000)  // 9GB
#define TEE_MEM_LIMIT         (0x340000000)  // 13GB
#define TEE_MEM_MIRROR_OFFSET (0x400000000)  // 16GB

#define VM_TEE_NAME "vm-tee-reserved"
#define VM_BL_NAME  "vm-bl-reserved"

/* lk_boot_args[0] : ATF parameter address */
extern ulong lk_boot_args[4];

struct tee_vendor_device {
    u8 name[DEVICE_NAME_MAX_LEN];
    u32 (*get_reserved_tee_address)(u32 tee_size);
    void (*set_boot_param)(u64 param_addr, u64 tee_entry,
            u64 tee_size, u64 dram_base, u64 dram_size);
};

extern struct tee_vendor_device tee_vendor_module;
#define DECLARE_TEE_MODULE(drv_name, tee_addr, boot_param) \
    struct tee_vendor_device tee_vendor_module = { \
        .name = drv_name, \
        .get_reserved_tee_address = tee_addr, \
        .set_boot_param = boot_param, \
    }

