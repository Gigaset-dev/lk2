/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <arch/mmu.h>
#include <assert.h>
#include <chip_id.h>
#include <err.h>
#include <kernel/vm.h>
#include <platform.h>
#include <platform/tee_platform_tbase.h>
#include <platform_mtk.h>
#include <string.h>
#include "tee_loader.h"
#include "tee_tbase.h"

#define MOD "[TEE_TBASE]"


#define RPMB_KEY_SIZE     (32)

#define TEE_SECURE_ISRAM_ADDR           (0x0)
#define TEE_SECURE_ISRAM_SIZE           (0x0)

/**************************************************************************
 *  EXTERNAL FUNCTIONS
 **************************************************************************/
void tbase_boot_param_prepare(u64 param_addr, u64 tee_entry,
    u64 tbase_sec_dram_size, u64 dram_base, u64 dram_size)
{
    struct tee_arg *teearg = (struct tee_arg *)param_addr;

    /* Prepare TEE boot parameters */
    teearg->magic                 = TBASE_BOOTCFG_MAGIC;   /* Trustonic's TEE magic number */
    teearg->length                = sizeof(struct tee_arg);/* Trustonic's TEE argument block size */
    //teearg->version               = TBASE_MONITOR_INTERFACE_VERSION; /* TEE version */
    teearg->dRamBase              = dram_base;             /* DRAM base address */
    teearg->dRamSize              = dram_size;             /* Full DRAM size */
    teearg->secDRamBase           = tee_entry;             /* Secure DRAM base address */
    teearg->secDRamSize           = tbase_sec_dram_size;   /* Secure DRAM size */
    teearg->secIRamBase           = TEE_SECURE_ISRAM_ADDR; /* Secure SRAM base address */
    teearg->secIRamSize           = TEE_SECURE_ISRAM_SIZE; /* Secure SRAM size */
    //teearg->conf_mair_el3         = read_mair_el3();
    //teearg->MSMPteCount           = totalPages;
    //teearg->MSMBase               = (u64)registerFileL2;
    //teearg->gic_distributor_base  = TBASE_GIC_DIST_BASE;
    //teearg->gic_cpuinterface_base = TBASE_GIC_CPU_BASE;
    //teearg->gic_version           = TBASE_GIC_VERSION;
    teearg->total_number_spi      = TBASE_SPI_COUNT;  /* Support total 256 SPIs and 32 PPIs */
    teearg->ssiq_number           = TBASE_SSIQ_NRO;   /* reserve SPI ID 292, which is ID 324 */
    //teearg->flags                 = TBASE_MONITOR_FLAGS;

#ifdef TBASE_DEBUG
    dprintf(INFO, "%s teearg.magic: 0x%x\n", MOD, teearg->magic);
    dprintf(INFO, "%s teearg.length: 0x%x\n", MOD, teearg->version);
    dprintf(INFO, "%s teearg.dRamBase: 0x%x\n", MOD, teearg->dRamBase);
    dprintf(INFO, "%s teearg.dRamSize: 0x%x\n", MOD, teearg->dRamSize);
    dprintf(INFO, "%s teearg.secDRamBase: 0x%x\n", MOD, teearg->secDRamBase);
    dprintf(INFO, "%s teearg.secDRamSize: 0x%x\n", MOD, teearg->secDRamSize);
    dprintf(INFO, "%s teearg.secIRamBase: 0x%x\n", MOD, teearg->secIRamBase);
    dprintf(INFO, "%s teearg.secIRamSize: 0x%x\n", MOD, teearg->secIRamSize);
    dprintf(INFO, "%s teearg.total_number_spi: %d\n", MOD, teearg->total_number_spi);
    dprintf(INFO, "%s teearg.ssiq_number: %d\n", MOD, teearg->ssiq_number);
#endif
}

DECLARE_TEE_MODULE("Trustonic", NULL, tbase_boot_param_prepare);

