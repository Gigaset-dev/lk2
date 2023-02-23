/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <trace.h>

#include "sec_cfg.h"

#define LOCAL_TRACE 0

static void set_field(vaddr_t reg, u32 field, u32 val)
{
    u32 tv = read32(reg);

    tv &= ~(field);
    tv |= val;
    write32(reg, tv);
}

static void sram_enable_region_protection(unsigned int region, bool enable)
{
    /*
     * 0: disable Region n protection
     * 1: Enable Region n protection and based on Region 1's security setting
     */
    if (region == 0) {
        if (!enable) /*Disable*/
            clrbits32(SRAMROM_SEC_ADDR, (0x1<<SRAMROM_SEC_ADDR_SEC0_SEC_EN));
        else if (enable) /*Enable*/
            setbits32(SRAMROM_SEC_ADDR, (0x1<<SRAMROM_SEC_ADDR_SEC0_SEC_EN));
    } else if (region == 1) {
        if (!enable) /*Disable*/
            clrbits32(SRAMROM_SEC_ADDR, (0x1<<SRAMROM_SEC_ADDR_SEC1_SEC_EN));
        else if (enable) /*Enable*/
            setbits32(SRAMROM_SEC_ADDR, (0x1<<SRAMROM_SEC_ADDR_SEC1_SEC_EN));
    } else if (region == 2) {
        if (!enable) /*Disable*/
            clrbits32(SRAMROM_SEC_ADDR, (0x1<<SRAMROM_SEC_ADDR_SEC2_SEC_EN));
        else if (enable) /*Enable*/
            setbits32(SRAMROM_SEC_ADDR, (0x1<<SRAMROM_SEC_ADDR_SEC2_SEC_EN));
    } else if (region == 3) {
        if (!enable) /*Disable*/
            clrbits32(SRAMROM_SEC_ADDR, (0x1<<SRAMROM_SEC_ADDR_SEC3_SEC_EN));
        else if (enable) /*Enable*/
            setbits32(SRAMROM_SEC_ADDR, (0x1<<SRAMROM_SEC_ADDR_SEC3_SEC_EN));
    }
}

static void sram_set_region_addr(unsigned int sram_split_register, vaddr_t addr)
{
    if (sram_split_register == 0)
        write32(SRAMROM_SEC_ADDR, (addr & SRAMROM_SEC_ADDR_MASK));
    else if (sram_split_register == 1)
        write32(SRAMROM_SEC_ADDR1, (addr & SRAMROM_SEC_ADDR_MASK));
    else if (sram_split_register == 2)
        write32(SRAMROM_SEC_ADDR2, (addr & SRAMROM_SEC_ADDR_MASK));
}

void sram_sec_init(vaddr_t start_addr)
{
    LTRACEF("[SEC_CFG] [B]SRAMROM SEC_ADDR:0x%x, SEC_ADDR1:0x%x, SEC_ADDR2:0x%x\n",
        read32(SRAMROM_SEC_ADDR), read32(SRAMROM_SEC_ADDR1), read32(SRAMROM_SEC_ADDR2));
    LTRACEF("[SEC_CFG] [B]SRAMROM SEC_CTRL:0x%x, SEC_CTRL2:0x%x, SEC_CTRL5:0x%x, SEC_CTRL6:0x%x\n",
        read32(SRAMROM_SEC_CTRL), read32(SRAMROM_SEC_CTRL2),
        read32(SRAMROM_SEC_CTRL5), read32(SRAMROM_SEC_CTRL6));
    LTRACEF("[SEC_CFG] [B]SRAMROM DM_REMAP0:0x%x,  DM_REMAP1:0x%x\n",
        read32(SRAMROM_DM_REMAP0), read32(SRAMROM_DM_REMAP1));

    /* Set Region Address Split Info */

    /* Split to 2 regions (region 0 and region 3) */
    sram_set_region_addr(0, start_addr);
    sram_set_region_addr(1, start_addr);
    sram_set_region_addr(2, start_addr);

    /* Enable region 0 and region 3 protection */
    sram_enable_region_protection(0, true);
    sram_enable_region_protection(3, true);

    /* Set permission for Region 0 */
    set_field(SRAMROM_SEC_CTRL, SRAMROM_SEC_CTRL_SEC0_DOM0_MASK,
        PERMIT_S_RW_NS_BLOCK << SRAMROM_SEC_CTRL_SEC0_DOM0_SHIFT);
    set_field(SRAMROM_SEC_CTRL, SRAMROM_SEC_CTRL_SEC0_DOM1_MASK,
        PERMIT_S_BLOCK_NS_BLOCK << SRAMROM_SEC_CTRL_SEC0_DOM1_SHIFT);
    set_field(SRAMROM_SEC_CTRL, SRAMROM_SEC_CTRL_SEC0_DOM2_MASK,
        PERMIT_S_BLOCK_NS_BLOCK << SRAMROM_SEC_CTRL_SEC0_DOM2_SHIFT);
    set_field(SRAMROM_SEC_CTRL, SRAMROM_SEC_CTRL_SEC0_DOM3_MASK,
        PERMIT_S_BLOCK_NS_BLOCK << SRAMROM_SEC_CTRL_SEC0_DOM3_SHIFT);
    set_field(SRAMROM_SEC_CTRL2, SRAMROM_SEC_CTRL2_SEC0_DOM4_MASK,
        PERMIT_S_BLOCK_NS_BLOCK << SRAMROM_SEC_CTRL2_SEC0_DOM4_SHIFT);
    set_field(SRAMROM_SEC_CTRL2, SRAMROM_SEC_CTRL2_SEC0_DOM5_MASK,
        PERMIT_S_BLOCK_NS_BLOCK << SRAMROM_SEC_CTRL2_SEC0_DOM5_SHIFT);
    set_field(SRAMROM_SEC_CTRL2, SRAMROM_SEC_CTRL2_SEC0_DOM6_MASK,
        PERMIT_S_BLOCK_NS_BLOCK << SRAMROM_SEC_CTRL2_SEC0_DOM6_SHIFT);
    set_field(SRAMROM_SEC_CTRL2, SRAMROM_SEC_CTRL2_SEC0_DOM7_MASK,
        PERMIT_S_BLOCK_NS_BLOCK << SRAMROM_SEC_CTRL2_SEC0_DOM7_SHIFT);

    /* Set permission for Region 3 */
    set_field(SRAMROM_SEC_CTRL5, SRAMROM_SEC_CTRL5_SEC3_DOM0_MASK,
        PERMIT_S_RW_NS_RW << SRAMROM_SEC_CTRL5_SEC3_DOM0_SHIFT);
    set_field(SRAMROM_SEC_CTRL5, SRAMROM_SEC_CTRL5_SEC3_DOM1_MASK,
        PERMIT_S_RW_NS_RW << SRAMROM_SEC_CTRL5_SEC3_DOM1_SHIFT);
    set_field(SRAMROM_SEC_CTRL5, SRAMROM_SEC_CTRL5_SEC3_DOM2_MASK,
        PERMIT_S_RW_NS_RW << SRAMROM_SEC_CTRL5_SEC3_DOM2_SHIFT);
    set_field(SRAMROM_SEC_CTRL5, SRAMROM_SEC_CTRL5_SEC3_DOM3_MASK,
        PERMIT_S_BLOCK_NS_BLOCK << SRAMROM_SEC_CTRL5_SEC3_DOM3_SHIFT);
    set_field(SRAMROM_SEC_CTRL6, SRAMROM_SEC_CTRL6_SEC3_DOM4_MASK,
        PERMIT_S_BLOCK_NS_BLOCK << SRAMROM_SEC_CTRL6_SEC3_DOM4_SHIFT);
    set_field(SRAMROM_SEC_CTRL6, SRAMROM_SEC_CTRL6_SEC3_DOM5_MASK,
        PERMIT_S_BLOCK_NS_BLOCK << SRAMROM_SEC_CTRL6_SEC3_DOM5_SHIFT);
    set_field(SRAMROM_SEC_CTRL6, SRAMROM_SEC_CTRL6_SEC3_DOM6_MASK,
        PERMIT_S_BLOCK_NS_BLOCK << SRAMROM_SEC_CTRL6_SEC3_DOM6_SHIFT);
    set_field(SRAMROM_SEC_CTRL6, SRAMROM_SEC_CTRL6_SEC3_DOM7_MASK,
        PERMIT_S_BLOCK_NS_BLOCK << SRAMROM_SEC_CTRL6_SEC3_DOM7_SHIFT);

    LTRACEF("[SEC_CFG] [A]SRAMROM SEC_ADDR:0x%x, SEC_ADDR1:0x%x, SEC_ADDR2:0x%x\n",
        read32(SRAMROM_SEC_ADDR), read32(SRAMROM_SEC_ADDR1), read32(SRAMROM_SEC_ADDR2));
    LTRACEF("[SEC_CFG] [A]SRAMROM SEC_CTRL:0x%x, SEC_CTRL2:0x%x, SEC_CTRL5:0x%x, SEC_CTRL6:0x%x\n",
        read32(SRAMROM_SEC_CTRL), read32(SRAMROM_SEC_CTRL2),
        read32(SRAMROM_SEC_CTRL5), read32(SRAMROM_SEC_CTRL6));
    LTRACEF("[SEC_CFG] [A]SRAMROM DM_REMAP0:0x%x,  DM_REMAP1:0x%x\n",
        read32(SRAMROM_DM_REMAP0), read32(SRAMROM_DM_REMAP1));
}
