/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <platform/addressmap.h>
#include <platform/reg.h>

/*******************************************************************************
 * REGISTER ADDRESS DEFINATION
 ******************************************************************************/
#define DEVAPC_INFRA_AO_BASE    (IO_BASE + 0x00030000)
#define DEVAPC_PERI_AO_BASE     (IO_BASE + 0x00034000)
#define DEVAPC_PERI_AO2_BASE    (IO_BASE + 0x00038000)
#define DEVAPC_PERI_PAR_AO_BASE (IO_BASE + 0x0003C000)
#define DEVAPC_FMEM_AO_BASE     (IO_BASE + 0x00044000)

/* Device APC for INFRA AO */
#define DEVAPC_INFRA_AO_MAS_DOM_0    (DEVAPC_INFRA_AO_BASE + 0x0900)
#define DEVAPC_INFRA_AO_MAS_DOM_1    (DEVAPC_INFRA_AO_BASE + 0x0904)

#define DEVAPC_INFRA_AO_MAS_SEC_0    (DEVAPC_INFRA_AO_BASE + 0x0A00)

#define DEVAPC_INFRA_AO_DOM_RMP_0_0  (DEVAPC_INFRA_AO_BASE + 0x0800)
#define DEVAPC_INFRA_AO_DOM_RMP_1_0  (DEVAPC_INFRA_AO_BASE + 0x0810)
#define DEVAPC_INFRA_AO_DOM_RMP_1_1  (DEVAPC_INFRA_AO_BASE + 0x0814)
#define DEVAPC_INFRA_AO_DOM_RMP_2_0  (DEVAPC_INFRA_AO_BASE + 0x0820)

#define DEVAPC_INFRA_AO_APC_CON      (DEVAPC_INFRA_AO_BASE + 0x0F00)

/* Device APC for PERI AO */
#define DEVAPC_PERI_AO_MAS_DOM_0     (DEVAPC_PERI_AO_BASE + 0x0900)

#define DEVAPC_PERI_AO_MAS_SEC_0     (DEVAPC_PERI_AO_BASE + 0x0A00)

#define DEVAPC_PERI_AO_DOM_RMP_0_0   (DEVAPC_PERI_AO_BASE + 0x0800)
#define DEVAPC_PERI_AO_DOM_RMP_0_1   (DEVAPC_PERI_AO_BASE + 0x0804)
#define DEVAPC_PERI_AO_DOM_RMP_1_0   (DEVAPC_PERI_AO_BASE + 0x0810)

#define DEVAPC_PERI_AO_APC_CON       (DEVAPC_PERI_AO_BASE + 0x0F00)

/* Device APC for PERI AO2 */
#define DEVAPC_PERI_AO2_APC_CON      (DEVAPC_PERI_AO2_BASE + 0x0F00)

/* Device APC for PERI PAR AO */
#define DEVAPC_PERI_PAR_AO_MAS_DOM_0 (DEVAPC_PERI_PAR_AO_BASE + 0x0900)
#define DEVAPC_PERI_PAR_AO_MAS_DOM_1 (DEVAPC_PERI_PAR_AO_BASE + 0x0904)
#define DEVAPC_PERI_PAR_AO_MAS_DOM_2 (DEVAPC_PERI_PAR_AO_BASE + 0x0908)

#define DEVAPC_PERI_PAR_AO_MAS_SEC_0 (DEVAPC_PERI_PAR_AO_BASE + 0x0A00)

#define DEVAPC_PERI_PAR_AO_APC_CON   (DEVAPC_PERI_PAR_AO_BASE + 0x0F00)

/* Device APC for FMEM AO */
#define DEVAPC_FMEM_AO_MAS_DOM_0     (DEVAPC_FMEM_AO_BASE + 0x0900)
#define DEVAPC_FMEM_AO_MAS_DOM_1     (DEVAPC_FMEM_AO_BASE + 0x0904)
#define DEVAPC_FMEM_AO_MAS_DOM_2     (DEVAPC_FMEM_AO_BASE + 0x0908)

#define DEVAPC_FMEM_AO_MAS_SEC_0     (DEVAPC_FMEM_AO_BASE + 0x0A00)

#define DEVAPC_FMEM_AO_DOM_RMP_0_0   (DEVAPC_FMEM_AO_BASE + 0x0800)

#define DEVAPC_FMEM_AO_APC_CON       (DEVAPC_FMEM_AO_BASE + 0x0F00)

/* -------------------------------------------------------------------------- */
/* Fields of domain index */
/* INFRA AO */
#define CONN  (0xF << 24) // DOM_0
#define SSPM  (0xF << 0) // DOM_1
#define CPUEB (0xF << 8) // DOM_1

/* PERI AO */
#define SPM   (0xF << 0) // DOM_0

/* FMEM AO */
#define MD1   (0xF << 16) // DOM_1
#define MFG   (0xF << 24) // DOM_1

/* -------------------------------------------------------------------------- */
/* SRAM DOMAIN REMAP */
#define SRAM_DOM_RMP_INIT      (0xFFFFFFFF)

#define SRAM_INFRA_AP_DOM      (0x7 << 0) /* Infra domain 0 */
#define SRAM_INFRA_SSPM_DOM    (0x7 << 24) /* Infra domain 8 */
#define SRAM_INFRA_CPUEB_DOM   (0x7 << 10) /* Infra domain 14 */

/* -------------------------------------------------------------------------- */
/* MM DOMAIN REMAP */
#define MM_DOM_RMP_INIT        (0xFFFFFFFF)

#define MM_INFRA_AP_DOM        (0x3 << 0) /* Infra domain 0 */
#define MM_INFRA_SSPM_DOM      (0x3 << 16) /* Infra domain 8 */

/* -------------------------------------------------------------------------- */
/* CONN DOMAIN REMAP */
#define CONN_DOM_RMP_INIT      (0xFFFFFFFF)

#define CONN_INFRA_AP_DOM      (0x3 << 0) /* Infra domain 0 */
#define CONN_INFRA_MD_DOM      (0x3 << 2) /* Infra domain 1 */
#define CONN_INFRA_ADSP_DOM    (0x3 << 20) /* Infra domain 10 */

/* -------------------------------------------------------------------------- */
/* TINYSYS/MD DOMAIN REMAP */
#define TINYSYS_DOM_RMP_INIT   (0xFFFFFFFF)
#define TINY_MD_DOM_RMP_INIT   TINYSYS_DOM_RMP_INIT

/* TINYSYS master DOMAIN REMAP */
#define INFRA_TINY_ADSP_DOM    (0xF << 0) /* ADSP domain 0 */
#define INFRA_TINY_SCP_DOM     (0xF << 4) /* SCP domain 1 */

#define TINY_MD_INFRA_AP_DOM   (0x7 << 0) /* Infra domain 0 */
#define TINY_MD_INFRA_SSPM_DOM (0x7 << 24) /* Infra domain 8 */
#define TINY_MD_INFRA_CONN_DOM (0x7 << 6) /* Infra domain 2 */

/*******************************************************************************
 * REGISTER BIT/FIELD DEFINATION
 ******************************************************************************/
#define APC_CON_SEN_BIT     0
#define APC_CON_APC_VIO_BIT 31

#define MOD_NO_IN_1_DEVAPC  16

/* -------------------------------------------------------------------------- */

/* Transaction_type */
enum TRANS_TYPE {
    NON_SECURE_TRANS = 0,
    SECURE_TRANS,
};

/* Master Type */
enum DEVAPC_MASTER_TYPE {
    MASTER_TYPE_INFRA_AO = 0,
    MASTER_TYPE_PERI_AO,
    MASTER_TYPE_PERI_AO2,
    MASTER_TYPE_PERI_PAR_AO,
    MASTER_TYPE_FMEM_AO,
};

/* Master Num */
enum DEVAPC_MASTER_NUM {
    MASTER_INFRA_AO_NUM = 6,
    MASTER_PERI_AO_NUM = 4,
    MASTER_PERI_PAR_AO_NUM = 12,
    MASTER_FMEM_AO_NUM = 11,
};

/* Master Secure index */
enum DEVAPC_INFRA_AO_MASTER_SEC_IDX {
    MASTER_SSPM_SEC = 3,
    MASTER_CPUEB_SEC = 4,
};

/* Error code */
enum DEVAPC_ERR_STATUS {
    DEVAPC_OK = 0x0,

    DEVAPC_ERR_GENERIC = 0x1000,
    DEVAPC_ERR_INVALID_CMD = 0x1001,
    DEVAPC_ERR_SLAVE_TYPE_NOT_SUPPORTED = 0x1002,
    DEVAPC_ERR_SLAVE_IDX_NOT_SUPPORTED = 0x1003,
    DEVAPC_ERR_DOMAIN_NOT_SUPPORTED = 0x1004,
    DEVAPC_ERR_PERMISSION_NOT_SUPPORTED = 0x1005,
    DEVAPC_ERR_OUT_OF_BOUNDARY = 0x1006,
};

/* DOMAIN_ID */
enum DOMAIN_ID {
    DOMAIN_0 = 0,
    DOMAIN_1,
    DOMAIN_2,
    DOMAIN_3,
    DOMAIN_4,
    DOMAIN_5,
    DOMAIN_6,
    DOMAIN_7,
    DOMAIN_8,
    DOMAIN_9,
    DOMAIN_10,
    DOMAIN_11,
    DOMAIN_12,
    DOMAIN_13,
    DOMAIN_14,
    DOMAIN_15,
};

/* -------------------------------------------------------------------------- */

static inline uint32_t uffs(uint32_t x)
{
    uint32_t r = 1;

    if (!x)
        return 0;
    if (!(x & 0xffff)) {
        x >>= 16;
        r += 16;
    }
    if (!(x & 0xff)) {
        x >>= 8;
        r += 8;
    }
    if (!(x & 0xf)) {
        x >>= 4;
        r += 4;
    }
    if (!(x & 3)) {
        x >>= 2;
        r += 2;
    }
    if (!(x & 1)) {
        x >>= 1;
        r += 1;
    }
    return r;
}

#define reg_set_field(reg, field, val) \
    do { \
        uint32_t tv = read32(reg); \
        tv &= ~(field); \
        tv |= ((val) << (uffs((uint32_t)field) - 1)); \
        write32(reg, tv); \
    } while (0)

#define reg_get_field(reg, field, val) \
    do { \
        uint32_t tv = read32(reg); \
        val = ((tv & (field)) >> (uffs((uint32_t)field) - 1)); \
    } while (0)
