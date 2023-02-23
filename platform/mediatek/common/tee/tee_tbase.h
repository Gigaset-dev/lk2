/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */


#pragma once

/* Tbase Magic For Interface */
#define TBASE_BOOTCFG_MAGIC (0x434d4254) // String TBMC in little-endian

/* TEE version */
#define TEE_ARGUMENT_VERSION            (0x00010000U)

struct tee_arg {
    u32 magic;        // magic value from information
    u32 length;       // size of struct in bytes.
    u64 version;      // Version of structure
    u64 dRamBase;     // NonSecure DRAM start address
    u64 dRamSize;     // NonSecure DRAM size
    u64 secDRamBase;  // Secure DRAM start address
    u64 secDRamSize;  // Secure DRAM size
    u64 secIRamBase;  // Secure IRAM base
    u64 secIRamSize;  // Secure IRam size
    u64 conf_mair_el3;// MAIR_EL3 for memory attributes sharing
    u32 RFU1;
    u32 MSMPteCount;  // Number of MMU entries for MSM
    u64 MSMBase;      // MMU entries for MSM
    u64 gic_distributor_base;
    u64 gic_cpuinterface_base;
    u32 gic_version;
    u32 total_number_spi;
    u32 ssiq_number;
    u32 RFU2;
    u64 flags;
};

/**************************************************************************
 * EXPORTED FUNCTIONS
 **************************************************************************/
void tbase_secmem_param_prepare(u64 param_addr, u32 tee_entry, u32 tbase_sec_dram_size,
    u32 tee_smem_size);
void tbase_boot_param_prepare(u64 param_addr, u64 tee_entry, u64 tbase_sec_dram_size,
    u64 dram_base, u64 dram_size);

