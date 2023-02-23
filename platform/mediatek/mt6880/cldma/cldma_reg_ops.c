/*
 * Copyright (c) 2021 MediaTek Inc. All Rights Reserved.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <arch/arch_ops.h>
#include <arch/ops.h>
#include <kernel/vm.h>
#include <platform/addressmap.h>
#include <platform/debug.h>
#include <platform/reg.h>
#include <stdio.h>
#include <stdlib.h>
#include <trace.h>

#include "cldma_reg_ops.h"

#define LOCAL_TRACE 0

void cldma_reg_set_domain_id(void)
{
    unsigned int val = 0;

    val = (DOMAIN_ID_PCIE << DOMAIN_ID_LEFT_HAND_SHIFT) |
        (DOMAIN_ID_SAP << DOMAIN_ID_RIGHT_HAND_SHIFT) |
        (DOMAIN_ID_PCIE << DOMAIN_ID_PCIE_SHIFT) |
        (DOMAIN_ID_MD << DOMAIN_ID_MD_SHIFT);
    writel(val, REG_CLDMA_DOMAIN_ID);
}

void cldma_reg_clr_tx_ldmu_err(void)
{
    unsigned int val;

    val = read32(REG_CLDMA_UL_ERR);
    if (val & CLDMA_TX_LDMU_ERR_STATUS)
        writel(CLDMA_TX_LDMU_ERR_EXIT, REG_CLDMA_UL_ERR);
}

unsigned int cldma_reg_get_tx_ldmu_err(void)
{
    return read32(REG_CLDMA_UL_ERR) & CLDMA_TX_LDMU_ERR_STATUS;
}

void cldma_reg_clr_rx_ldmu_err(void)
{
    unsigned int val;

    val = read32(REG_CLDMA_DL_ERR);
    if (val & CLDMA_RX_LDMU_ERR_STATUS)
        writel(CLDMA_RX_LDMU_ERR_EXIT, REG_CLDMA_DL_ERR);
}

unsigned int cldma_reg_get_rx_ldmu_err(void)
{
    return read32(REG_CLDMA_DL_ERR) & CLDMA_RX_LDMU_ERR_STATUS;
}

void cldma_reg_clr_tx_int(void)
{
    writel(read32(REG_CLDMA_L2TISAR0), REG_CLDMA_L2TISAR0);
    writel(read32(REG_CLDMA_L2TISAR1), REG_CLDMA_L2TISAR1);
    writel(read32(REG_CLDMA_L3TISAR0), REG_CLDMA_L3TISAR0);
    writel(read32(REG_CLDMA_L3TISAR0), REG_CLDMA_L3TISAR0);
    writel(read32(REG_CLDMA_L3TISAR1), REG_CLDMA_L3TISAR1);
    writel(read32(REG_CLDMA_L3TISAR2), REG_CLDMA_L3TISAR2);
}

void cldma_reg_clr_rx_int(void)
{
    writel(read32(REG_CLDMA_L2RISAR0), REG_CLDMA_L2RISAR0);
    writel(read32(REG_CLDMA_L2RISAR1), REG_CLDMA_L2RISAR1);
    writel(read32(REG_CLDMA_L3RISAR0), REG_CLDMA_L3RISAR0);
    writel(read32(REG_CLDMA_L3RISAR1), REG_CLDMA_L3RISAR1);
}

void cldma_reg_start_tx_que(unsigned int qno)
{
    writel(1 << qno, REG_CLDMA_UL_START_CMD_WP);
}

void cldma_reg_start_rx_que(unsigned int qno)
{
    writel(1 << qno, REG_CLDMA_DL_START_CMD_WP);
}

void cldma_reg_set_tx_start_addr(unsigned int qno, unsigned char *addr)
{
    writel(((unsigned long)addr) & 0xFFFFFFFF, REG_CLDMA_UL_START_ADDR_L(qno));
}

void cldma_reg_set_rx_start_addr(unsigned int qno, unsigned char *addr)
{
    writel(((unsigned long)addr) & 0xFFFFFFFF, REG_CLDMA_DL_START_ADDR_L(qno));
}

void cldma_reg_stop_tx_que(unsigned int qno)
{
    writel(1 << qno, REG_CLDMA_UL_STOP_CMD_WP);
}

void cldma_reg_stop_rx_que(unsigned int qno)
{
    writel(1 << qno, REG_CLDMA_DL_STOP_CMD_WP);
}

unsigned int cldma_reg_tx_error(unsigned int qno)
{
    unsigned int flags;

    flags = CLDMA_TX_ERR_INT(qno) | CLDMA_TX_ACTIVE_START_ERR_INT(qno);
    if (read32(REG_CLDMA_L2TISAR0) & flags)
        return 1;

    flags = CLDMA_TX_GPD_LEN_ERR_INT(qno);
    if (read32(REG_CLDMA_L3TISAR0) & flags)
        return 1;

    flags = CLDMA_TX_GPD_64K_ERR_INT(qno) | CLDMA_TX_GPD_DATA_LEN_MIS_ERR_INT(qno);
    if (read32(REG_CLDMA_L3TISAR1) & flags)
        return 1;

    flags = CLDMA_TX_ILLEGALL_MEM_ACCESS_INT(qno);
    if (read32(REG_CLDMA_L3TISAR2) & flags)
        return 1;

    return 0;
}

unsigned int cldma_reg_rx_error(unsigned int qno)
{
    unsigned int flags;

    flags = CLDMA_RX_ERR_INT(qno) | CLDMA_RX_ACTIVE_START_ERR_INT(qno);
    if (read32(REG_CLDMA_L2RISAR0) & flags)
        return 1;

    flags = CLDMA_RX_INACTIVE_ERR_INT(qno);
    if (read32(REG_CLDMA_L2RISAR1) & flags)
        return 1;

    flags = CLDMA_RX_GPD_LEN_ERR_INT(qno);
    if (read32(REG_CLDMA_L3RISAR0) & flags)
        return 1;

    flags = CLDMA_RX_ALLEN_ERR_INT(qno) | CLDMA_RX_ILLEGALL_MEM_ACCESS_INT(qno);
    if (read32(REG_CLDMA_L3RISAR1) & flags)
        return 1;

    return 0;
}

unsigned int cldma_reg_tx_active(unsigned int qno)
{
    return read32(REG_CLDMA_UL_STATUS) & (1 << qno);
}

unsigned int cldma_reg_rx_active(unsigned int qno)
{
    return read32(REG_CLDMA_DL_STATUS) & (1 << qno);
}


static void cldma_reg_range_dump(unsigned int start_addr, unsigned int size)
{
    unsigned int i;

    for (i = 0; i < size; i += 16) {
        LTRACEF("[0x%x] %x %x %x %x\n", start_addr, read32(start_addr),
            read32((start_addr + 4)), read32((start_addr + 8)), read32((start_addr + 12)));
        start_addr = start_addr + 16;
    }
}

void cldma_reg_dump(void)
{
    LTRACEF("===== Dump some CLDMA register =====\n");
    LTRACEF("\nREG_CLDMA_DOMAIN_ID[%x]:0x%x\n", 0x1001C000 + 0x1A08, read32(0x1001C000 + 0x1A08));
    LTRACEF("\nREG_CLDMA_UL_ERR[%x]:0x%x\n", REG_CLDMA_UL_ERR, read32(REG_CLDMA_UL_ERR));
    LTRACEF("REG_CLDMA_L2TISAR0[%x]:0x%x\n", REG_CLDMA_L2TISAR0, read32(REG_CLDMA_L2TISAR0));
    LTRACEF("REG_CLDMA_L2TISAR1[%x]:0x%x\n", REG_CLDMA_L2TISAR1, read32(REG_CLDMA_L2TISAR1));
    LTRACEF("REG_CLDMA_L3TISAR0[%x]:0x%x\n", REG_CLDMA_L3TISAR0, read32(REG_CLDMA_L3TISAR0));
    LTRACEF("REG_CLDMA_L3TISAR1[%x]:0x%x\n", REG_CLDMA_L3TISAR1, read32(REG_CLDMA_L3TISAR1));
    LTRACEF("REG_CLDMA_L3TISAR2[%x]:0x%x\n", REG_CLDMA_L3TISAR2, read32(REG_CLDMA_L3TISAR2));
    LTRACEF("REG_CLDMA_UL_STATUS[%x]:0x%x\n", REG_CLDMA_UL_STATUS, read32(REG_CLDMA_UL_STATUS));
    LTRACEF("REG_CLDMA_UL_START_ADDR_L1[%x]:0x%x\n",
        REG_CLDMA_UL_START_ADDR_L(1), read32(REG_CLDMA_UL_START_ADDR_L(1)));
    LTRACEF("REG_CLDMA_UL_START_ADDR_H1[%x]:0x%x\n",
        REG_CLDMA_UL_START_ADDR_H(1), read32(REG_CLDMA_UL_START_ADDR_H(1)));
    LTRACEF("REG_CLDMA_UL_CURR_ADDR_L1[%x]:0x%x\n",
        REG_CLDMA_UL_CURR_ADDR_L(1), read32(REG_CLDMA_UL_CURR_ADDR_L(1)));
    LTRACEF("REG_CLDMA_UL_CURR_ADDR_H1[%x]:0x%x\n",
        REG_CLDMA_UL_CURR_ADDR_H(1), read32(REG_CLDMA_UL_CURR_ADDR_H(1)));
    LTRACEF("\nREG_CLDMA_DL_ERR[%x]:0x%x\n", REG_CLDMA_DL_ERR, read32(REG_CLDMA_DL_ERR));
    LTRACEF("REG_CLDMA_L2RISAR0[%x]:0x%x\n", REG_CLDMA_L2RISAR0, read32(REG_CLDMA_L2RISAR0));
    LTRACEF("REG_CLDMA_L2RISAR1[%x]:0x%x\n", REG_CLDMA_L2RISAR1, read32(REG_CLDMA_L2RISAR1));
    LTRACEF("REG_CLDMA_L3RISAR0[%x]:0x%x\n", REG_CLDMA_L3RISAR0, read32(REG_CLDMA_L3RISAR0));
    LTRACEF("REG_CLDMA_L3RISAR1[%x]:0x%x\n", REG_CLDMA_L3RISAR1, read32(REG_CLDMA_L3RISAR1));
    LTRACEF("REG_CLDMA_DL_STATUS[%x]:0x%x\n", REG_CLDMA_DL_STATUS, read32(REG_CLDMA_DL_STATUS));
    LTRACEF("REG_CLDMA_DL_START_ADDR_L1[%x]:0x%x\n",
        REG_CLDMA_DL_START_ADDR_L(1), read32(REG_CLDMA_DL_START_ADDR_L(1)));
    LTRACEF("REG_CLDMA_DL_START_ADDR_H1[%x]:0x%x\n",
        REG_CLDMA_DL_START_ADDR_H(1), read32(REG_CLDMA_DL_START_ADDR_H(1)));
    LTRACEF("REG_CLDMA_DL_CURR_ADDR_L1[%x]:0x%x\n",
        REG_CLDMA_DL_CURR_ADDR_L(1), read32(REG_CLDMA_DL_CURR_ADDR_L(1)));
    LTRACEF("REG_CLDMA_DL_CURR_ADDR_H1[%x]:0x%x\n",
        REG_CLDMA_DL_CURR_ADDR_H(1), read32(REG_CLDMA_DL_CURR_ADDR_H(1)));

    LTRACEF("\nREG_CLDMA Range dump[0x%x ~ 0x%x]\n",
        CLDMA_PD_DL_BASE + 0x118, CLDMA_PD_DL_BASE + 0x148);
    cldma_reg_range_dump(CLDMA_PD_DL_BASE + 0x118, 0x148 - 0x118 + 4);
    LTRACEF("\nREG_CLDMA Range dump[0x%x ~ 0x%x]\n",
        CLDMA_PD_UL_BASE + 0xD0, CLDMA_PD_UL_BASE + 0x10C);
    cldma_reg_range_dump(CLDMA_PD_UL_BASE + 0xD0, 0x10C - 0xD0 + 4);
    LTRACEF("\nREG_CLDMA Range dump[0x%x ~ 0x%x]\n",
        CLDMA_AO_MISC_BASE + 0x144, CLDMA_AO_MISC_BASE + 0x158);
    cldma_reg_range_dump(CLDMA_AO_MISC_BASE + 0x144, 0x158 - 0x144 + 4);
}
