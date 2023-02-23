/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <trace.h>

#include "devapc.h"

#define LOCAL_TRACE 0

static uint32_t set_devapc_master_trans(enum DEVAPC_MASTER_TYPE master_type,
    uint32_t master_index, enum TRANS_TYPE trans_type)
{
    uint32_t master_register_index;
    uint32_t master_set_index;
    addr_t base;

    master_register_index = master_index / (MOD_NO_IN_1_DEVAPC * 2);
    master_set_index = master_index % (MOD_NO_IN_1_DEVAPC * 2);

    if (master_type == MASTER_TYPE_INFRA_AO &&
        master_index < MASTER_INFRA_AO_NUM) {
        base = DEVAPC_INFRA_AO_MAS_SEC_0 + master_register_index * 4;

    } else if (master_type == MASTER_TYPE_PERI_AO &&
        master_index < MASTER_PERI_AO_NUM) {
        base = DEVAPC_PERI_AO_MAS_SEC_0 + master_register_index * 4;

    } else if (master_type == MASTER_TYPE_PERI_PAR_AO &&
        master_index < MASTER_PERI_PAR_AO_NUM) {
        base = DEVAPC_PERI_PAR_AO_MAS_SEC_0 + master_register_index * 4;

    } else if (master_type == MASTER_TYPE_FMEM_AO &&
        master_index < MASTER_FMEM_AO_NUM) {
        base = DEVAPC_FMEM_AO_MAS_SEC_0 + master_register_index * 4;

    } else {
        TRACEF("[DEVAPC] Out of boundary, %s=0x%x, %s=0x%x\n",
            "master_type", master_type,
            "master_index", master_index);

        return DEVAPC_ERR_OUT_OF_BOUNDARY;
    }

    if (base) {
        reg_set_field((uint32_t *)(uintptr_t)base, 0x1 << master_set_index, trans_type);
        return DEVAPC_OK;
    }

    return DEVAPC_ERR_GENERIC;
}

void device_APC_dom_setup(void)
{
    uint32_t val;

    LTRACEF("[DEVAPC] Device APC domain init setup:\n");

    /* Set DEVAPC AO reg to SWd access only */
    clrbits32(DEVAPC_INFRA_AO_APC_CON, 4);
    clrbits32(DEVAPC_PERI_AO_APC_CON, 4);
    clrbits32(DEVAPC_PERI_AO2_APC_CON, 4);
    clrbits32(DEVAPC_PERI_PAR_AO_APC_CON, 4);
    clrbits32(DEVAPC_FMEM_AO_APC_CON, 4);

    val = 0x1 << APC_CON_SEN_BIT | 0x1 << APC_CON_APC_VIO_BIT;

    write32(DEVAPC_INFRA_AO_APC_CON, val);
    write32(DEVAPC_PERI_AO_APC_CON, val);
    write32(DEVAPC_PERI_AO2_APC_CON, val);
    write32(DEVAPC_PERI_PAR_AO_APC_CON, val);
    write32(DEVAPC_FMEM_AO_APC_CON, val);

    LTRACEF("[DEVAPC] APC_CON: %s(0x%x), %s(0x%x), %s(0x%x), %s(0x%x), %s(0x%x)\n",
        "INFRA_AO", read32(DEVAPC_INFRA_AO_APC_CON),
        "PERI_AO", read32(DEVAPC_PERI_AO_APC_CON),
        "PERI_AO2", read32(DEVAPC_PERI_AO2_APC_CON),
        "PERI_PAR_AO", read32(DEVAPC_PERI_PAR_AO_APC_CON),
        "FMEM_AO", read32(DEVAPC_FMEM_AO_APC_CON)
       );

    /* SSPM will access SSPM memory region, but SSPM region is protected as
     * secure access only by EMI MPU. So this setting is necessary.
     */
    set_devapc_master_trans(MASTER_TYPE_INFRA_AO,
        MASTER_SSPM_SEC, SECURE_TRANS);

    /* MCUPM/CPUEB need to access MCUSYS secure reg for low power scenario */
    set_devapc_master_trans(MASTER_TYPE_INFRA_AO,
        MASTER_CPUEB_SEC, SECURE_TRANS);

    LTRACEF("[DEVAPC] %s:0x%x, %s:0x%x, %s:0x%x, %s:0x%x\n",
        "INFRA_AO_MAS_SEC_0",
        read32(DEVAPC_INFRA_AO_MAS_SEC_0),
        "PERI_AO_MAS_SEC_0",
        read32(DEVAPC_PERI_AO_MAS_SEC_0),
        "PERI_PAR_AO_MAS_SEC_0",
        read32(DEVAPC_PERI_PAR_AO_MAS_SEC_0),
        "FMEM_AO_MAS_SEC_0",
        read32(DEVAPC_FMEM_AO_MAS_SEC_0)
       );

/* -------------------------------------------------------------------------- */
/* INFRA MASTER DOMAIN settings */

    /* Set MD1 to DOMAIN1(MD1) */
    reg_set_field(DEVAPC_FMEM_AO_MAS_DOM_1, MD1, DOMAIN_1);

    /* Set CONN to DOMAIN2(CONN) */
    reg_set_field(DEVAPC_INFRA_AO_MAS_DOM_0, CONN, DOMAIN_2);

    /* Set SSHUB(SSHUB=SCP=Sensor Hub) to DOMAIN3(SCP) */
    /* SCP domain is not set by DEVAPC */

    /* Set MFG(GPU) to DOMAIN6(MFG) */
    reg_set_field(DEVAPC_FMEM_AO_MAS_DOM_1, MFG, DOMAIN_6);

    /* Set SSPM to DOMAIN8(SSPM) */
    reg_set_field(DEVAPC_INFRA_AO_MAS_DOM_1, SSPM, DOMAIN_8);

    /* Set SPM to DOMAIN9(SPM) */
    reg_set_field(DEVAPC_PERI_AO_MAS_DOM_0, SPM, DOMAIN_9);

    /* Set ADSP(Audio DSP, HIFI3) to DOMAIN10(ADSP) */
    /* ADSP domain is not set by DEVAPC */

    /* Set MCUPM/CPUEB to DOMAIN14 */
    reg_set_field(DEVAPC_INFRA_AO_MAS_DOM_1, CPUEB, DOMAIN_14);

    /* MM domain(4) is not set by DEVAPC */
    /* MD HW domain(7) is not set by DEVAPC */
    /* GZ domain(11) is not set by DEVAPC */

    LTRACEF("[DEVAPC] Domain Setup: INFRA_AO (0x%x), (0x%x)\n",
        read32(DEVAPC_INFRA_AO_MAS_DOM_0),
        read32(DEVAPC_INFRA_AO_MAS_DOM_1)
       );
    LTRACEF("[DEVAPC] Domain Setup: PERI_AO (0x%x)\n",
        read32(DEVAPC_PERI_AO_MAS_DOM_0)
       );
    LTRACEF("[DEVAPC] Domain Setup: PERI_PAR_AO (0x%x), (0x%x), (0x%x)\n",
        read32(DEVAPC_PERI_PAR_AO_MAS_DOM_0),
        read32(DEVAPC_PERI_PAR_AO_MAS_DOM_1),
        read32(DEVAPC_PERI_PAR_AO_MAS_DOM_2)
       );
    LTRACEF("[DEVAPC] Domain Setup: FMEM_AO (0x%x), (0x%x), (0x%x)\n",
        read32(DEVAPC_FMEM_AO_MAS_DOM_0),
        read32(DEVAPC_FMEM_AO_MAS_DOM_1),
        read32(DEVAPC_FMEM_AO_MAS_DOM_2)
       );

/* -------------------------------------------------------------------------- */
/* DOMAIN REMAP settings */

    /* SRAM domain remap settings
     *
     * 4-bit domain to 3-bit domain:
     * APMCU from 0 to 0
     * SSPM from 8 to 1
     * MCUPM/CPUEB from 14 to 2
     * others from xxxx to 7
     */
    write32(DEVAPC_INFRA_AO_DOM_RMP_1_0, SRAM_DOM_RMP_INIT);
    write32(DEVAPC_INFRA_AO_DOM_RMP_1_1, SRAM_DOM_RMP_INIT);
    reg_set_field(DEVAPC_INFRA_AO_DOM_RMP_1_0, SRAM_INFRA_AP_DOM, DOMAIN_0);
    reg_set_field(DEVAPC_INFRA_AO_DOM_RMP_1_0, SRAM_INFRA_SSPM_DOM,
        DOMAIN_1);
    reg_set_field(DEVAPC_INFRA_AO_DOM_RMP_1_1, SRAM_INFRA_CPUEB_DOM,
        DOMAIN_2);

    LTRACEF("[DEVAPC] SRAMROM DOM_REMAP 0:0x%x, 1:0x%x\n",
        read32(DEVAPC_INFRA_AO_DOM_RMP_1_0),
        read32(DEVAPC_INFRA_AO_DOM_RMP_1_1));

    /* MMSYS domain remap settings
     *
     * 4-bit domain to 2-bit domain:
     * APMCU from 0 to 0
     * SSPM from 8 to 1
     * others from xxxx to 3
     */
    write32(DEVAPC_INFRA_AO_DOM_RMP_2_0, MM_DOM_RMP_INIT);
    reg_set_field(DEVAPC_INFRA_AO_DOM_RMP_2_0, MM_INFRA_AP_DOM, DOMAIN_0);
    reg_set_field(DEVAPC_INFRA_AO_DOM_RMP_2_0, MM_INFRA_SSPM_DOM, DOMAIN_1);

    LTRACEF("[DEVAPC] MMSYS DOM_REMAP 0:0x%x\n",
        read32(DEVAPC_INFRA_AO_DOM_RMP_2_0));

    /* CONNSYS domain remap settings
     *
     * 4-bit domain to 2-bit domain:
     * APMCU from 0 to 0
     * others from xxxx to 3
     */
    write32(DEVAPC_PERI_AO_DOM_RMP_1_0, CONN_DOM_RMP_INIT);
    reg_set_field(DEVAPC_PERI_AO_DOM_RMP_1_0, CONN_INFRA_AP_DOM, DOMAIN_0);
    reg_set_field(DEVAPC_PERI_AO_DOM_RMP_1_0, CONN_INFRA_MD_DOM, DOMAIN_1);
    reg_set_field(DEVAPC_PERI_AO_DOM_RMP_1_0, CONN_INFRA_ADSP_DOM, DOMAIN_2);

    LTRACEF("[DEVAPC] CONNSYS DOM_REMAP 0:0x%x\n",
        read32(DEVAPC_PERI_AO_DOM_RMP_1_0));

    /* TINYSYS master domain remap settings
     * There are 2 paths:
     * 1. TINYSYS to non-EMI (INFRA_AO)
     * 2. TINYSYS to EMI (FMEM_AO)
     *
     * 3-bit domain to 4-bit domain:
     * ADSP from 0 to 10
     * SCP from 1 to 3
     * all from xxx to 15
     */
    write32(DEVAPC_INFRA_AO_DOM_RMP_0_0, TINYSYS_DOM_RMP_INIT);
    write32(DEVAPC_FMEM_AO_DOM_RMP_0_0, TINYSYS_DOM_RMP_INIT);
    reg_set_field(DEVAPC_INFRA_AO_DOM_RMP_0_0, INFRA_TINY_ADSP_DOM,
        DOMAIN_10);
    reg_set_field(DEVAPC_INFRA_AO_DOM_RMP_0_0, INFRA_TINY_SCP_DOM,
        DOMAIN_3);
    reg_set_field(DEVAPC_FMEM_AO_DOM_RMP_0_0, INFRA_TINY_ADSP_DOM,
        DOMAIN_10);
    reg_set_field(DEVAPC_FMEM_AO_DOM_RMP_0_0, INFRA_TINY_SCP_DOM,
        DOMAIN_3);

    LTRACEF("[DEVAPC] TINYSYS master DOM_REMAP to: %s:0x%x, %s:0x%x\n",
        "non-EMI", read32(DEVAPC_INFRA_AO_DOM_RMP_0_0),
        "EMI", read32(DEVAPC_FMEM_AO_DOM_RMP_0_0)
       );

    /* TINYSYS/MD slave domain remap settings
     *
     * 4-bit domain to 3-bit domain:
     * APMCU from 0 to 0
     * SSPM from 8 to 1
     * CONN from 2 to 2
     * others from xxxx to 7
     */
    write32(DEVAPC_PERI_AO_DOM_RMP_0_0, TINY_MD_DOM_RMP_INIT);
    write32(DEVAPC_PERI_AO_DOM_RMP_0_1, TINY_MD_DOM_RMP_INIT);
    reg_set_field(DEVAPC_PERI_AO_DOM_RMP_0_0, TINY_MD_INFRA_AP_DOM,
        DOMAIN_0);
    reg_set_field(DEVAPC_PERI_AO_DOM_RMP_0_0, TINY_MD_INFRA_SSPM_DOM,
        DOMAIN_1);
    reg_set_field(DEVAPC_PERI_AO_DOM_RMP_0_0, TINY_MD_INFRA_CONN_DOM,
            DOMAIN_2);

    LTRACEF("[DEVAPC] TINYSYS/MD slave DOM_REMAP 0:0x%x, 1:0x%x\n",
        read32(DEVAPC_PERI_AO_DOM_RMP_0_0),
        read32(DEVAPC_PERI_AO_DOM_RMP_0_1));

}
