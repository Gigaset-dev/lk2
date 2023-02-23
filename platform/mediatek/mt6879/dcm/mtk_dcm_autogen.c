/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */


#include <arch/ops.h>
#include <debug.h>
#include <mtk_dcm.h>
#include <mtk_dcm_autogen.h>
#include <platform/addressmap.h>
#include <platform/reg.h>
#include <reg.h>
#include <smc.h>
#include <smc_id_table.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#define IOMEM(a) (a)
#define __raw_readl(addr)      read32(addr)
#define reg_read(addr)         __raw_readl(addr)
#define reg_write(addr, val)   writel((val), ((void *)addr))

#if 0
#define MTK_SIP_KERNEL_MCSI_NS_ACCESS    0x82000217
#define mcsi_reg_read(offset, res) \
    __smc_conduit(MTK_SIP_KERNEL_MCSI_NS_ACCESS, 0, offset, \
                        0, 0, 0, 0, &res)
#define mcsi_reg_write(offset, res) \
    __smc_conduit(MTK_SIP_KERNEL_MCSI_NS_ACCESS, 1, offset, \
                        0, 0, 0, 0, &res)
#define MCUSYS_SMC_WRITE(addr, val)  reg_write(addr, val)
#define MCSI_SMC_WRITE(addr, val)  mcsi_reg_write(val, (addr##_PHYS & 0xFFFF))
#define MCSI_SMC_READ(addr)  mcsi_reg_read(addr##_PHYS & 0xFFFF)
#endif

/*====================auto gen code 20210625_122760=====================*/
#define INFRACFG_AO_AXIMEM_BUS_DCM_REG0_MASK ((0x1f << 12) | \
            (0x1 << 17) | \
            (0x1 << 18))
#define INFRACFG_AO_AXIMEM_BUS_DCM_REG0_ON ((0x10 << 12) | \
            (0x1 << 17) | \
            (0x0 << 18))
#define INFRACFG_AO_AXIMEM_BUS_DCM_REG0_OFF ((0x10 << 12) | \
            (0x0 << 17) | \
            (0x1 << 18))

bool dcm_infracfg_ao_aximem_bus_dcm_is_on(void)
{
    bool ret = true;

    ret &= ((reg_read(INFRA_AXIMEM_IDLE_BIT_EN_0) &
        INFRACFG_AO_AXIMEM_BUS_DCM_REG0_MASK) ==
        (unsigned int) INFRACFG_AO_AXIMEM_BUS_DCM_REG0_ON);

    return ret;
}

void dcm_infracfg_ao_aximem_bus_dcm(int on)
{
    if (on) {
        /* TINFO = "Turn ON DCM 'infracfg_ao_aximem_bus_dcm'" */
        reg_write(INFRA_AXIMEM_IDLE_BIT_EN_0,
            (reg_read(INFRA_AXIMEM_IDLE_BIT_EN_0) &
            ~INFRACFG_AO_AXIMEM_BUS_DCM_REG0_MASK) |
            INFRACFG_AO_AXIMEM_BUS_DCM_REG0_ON);
    } else {
        /* TINFO = "Turn OFF DCM 'infracfg_ao_aximem_bus_dcm'" */
        reg_write(INFRA_AXIMEM_IDLE_BIT_EN_0,
            (reg_read(INFRA_AXIMEM_IDLE_BIT_EN_0) &
            ~INFRACFG_AO_AXIMEM_BUS_DCM_REG0_MASK) |
            INFRACFG_AO_AXIMEM_BUS_DCM_REG0_OFF);
    }
}

#define INFRACFG_AO_INFRA_BUS_DCM_REG0_MASK ((0x1 << 0) | \
            (0x1 << 1) | \
            (0x1 << 3) | \
            (0x1 << 4) | \
            (0x1f << 5) | \
            (0x1 << 20) | \
            (0x1 << 22) | \
            (0x1 << 23) | \
            (0x1 << 30))
#define INFRACFG_AO_INFRA_BUS_DCM_REG0_ON ((0x1 << 0) | \
            (0x1 << 1) | \
            (0x0 << 3) | \
            (0x0 << 4) | \
            (0x10 << 5) | \
            (0x1 << 20) | \
            (0x1 << 22) | \
            (0x1 << 23) | \
            (0x1 << 30))
#define INFRACFG_AO_INFRA_BUS_DCM_REG0_OFF ((0x0 << 0) | \
            (0x0 << 1) | \
            (0x0 << 3) | \
            (0x1 << 4) | \
            (0x10 << 5) | \
            (0x0 << 20) | \
            (0x0 << 22) | \
            (0x0 << 23) | \
            (0x0 << 30))
#if 0
static unsigned int infracfg_ao_infra_dcm_rg_sfsel_get(void)
{
    return (reg_read(INFRA_BUS_DCM_CTRL) >> 10) & 0x1f;
}
#endif
static void infracfg_ao_infra_dcm_rg_sfsel_set(unsigned int val)
{
    reg_write(INFRA_BUS_DCM_CTRL,
        (reg_read(INFRA_BUS_DCM_CTRL) &
        ~(0x1f << 10)) |
        (val & 0x1f) << 10);
}

bool dcm_infracfg_ao_infra_bus_dcm_is_on(void)
{
    bool ret = true;

    ret &= ((reg_read(INFRA_BUS_DCM_CTRL) &
        INFRACFG_AO_INFRA_BUS_DCM_REG0_MASK) ==
        (unsigned int) INFRACFG_AO_INFRA_BUS_DCM_REG0_ON);

    return ret;
}

void dcm_infracfg_ao_infra_bus_dcm(int on)
{
    if (on) {
        /* TINFO = "Turn ON DCM 'infracfg_ao_infra_bus_dcm'" */
        infracfg_ao_infra_dcm_rg_sfsel_set(0x1);
        reg_write(INFRA_BUS_DCM_CTRL,
            (reg_read(INFRA_BUS_DCM_CTRL) &
            ~INFRACFG_AO_INFRA_BUS_DCM_REG0_MASK) |
            INFRACFG_AO_INFRA_BUS_DCM_REG0_ON);
    } else {
        /* TINFO = "Turn OFF DCM 'infracfg_ao_infra_bus_dcm'" */
        infracfg_ao_infra_dcm_rg_sfsel_set(0x1);
        reg_write(INFRA_BUS_DCM_CTRL,
            (reg_read(INFRA_BUS_DCM_CTRL) &
            ~INFRACFG_AO_INFRA_BUS_DCM_REG0_MASK) |
            INFRACFG_AO_INFRA_BUS_DCM_REG0_OFF);
    }
}

#define INFRACFG_AO_INFRA_RX_P2P_DCM_REG0_MASK ((0xf << 0))
#define INFRACFG_AO_INFRA_RX_P2P_DCM_REG0_ON ((0x0 << 0))
#define INFRACFG_AO_INFRA_RX_P2P_DCM_REG0_OFF ((0xf << 0))

bool dcm_infracfg_ao_infra_rx_p2p_dcm_is_on(void)
{
    bool ret = true;

    ret &= ((reg_read(P2P_RX_CLK_ON) &
        INFRACFG_AO_INFRA_RX_P2P_DCM_REG0_MASK) ==
        (unsigned int) INFRACFG_AO_INFRA_RX_P2P_DCM_REG0_ON);

    return ret;
}

void dcm_infracfg_ao_infra_rx_p2p_dcm(int on)
{
    if (on) {
        /* TINFO = "Turn ON DCM 'infracfg_ao_infra_rx_p2p_dcm'" */
        reg_write(P2P_RX_CLK_ON,
            (reg_read(P2P_RX_CLK_ON) &
            ~INFRACFG_AO_INFRA_RX_P2P_DCM_REG0_MASK) |
            INFRACFG_AO_INFRA_RX_P2P_DCM_REG0_ON);
    } else {
        /* TINFO = "Turn OFF DCM 'infracfg_ao_infra_rx_p2p_dcm'" */
        reg_write(P2P_RX_CLK_ON,
            (reg_read(P2P_RX_CLK_ON) &
            ~INFRACFG_AO_INFRA_RX_P2P_DCM_REG0_MASK) |
            INFRACFG_AO_INFRA_RX_P2P_DCM_REG0_OFF);
    }
}

#define INFRACFG_AO_MTS_BUS_DCM_REG0_MASK ((0x1 << 31))
#define INFRACFG_AO_MTS_BUS_DCM_REG0_ON ((0x1 << 31))
#define INFRACFG_AO_MTS_BUS_DCM_REG0_OFF ((0x0 << 31))

bool dcm_infracfg_ao_mts_bus_dcm_is_on(void)
{
    bool ret = true;

    ret &= ((reg_read(INFRA_BUS_DCM_CTRL) &
        INFRACFG_AO_MTS_BUS_DCM_REG0_MASK) ==
        (unsigned int) INFRACFG_AO_MTS_BUS_DCM_REG0_ON);

    return ret;
}

void dcm_infracfg_ao_mts_bus_dcm(int on)
{
    if (on) {
        /* TINFO = "Turn ON DCM 'infracfg_ao_mts_bus_dcm'" */
        reg_write(INFRA_BUS_DCM_CTRL,
            (reg_read(INFRA_BUS_DCM_CTRL) &
            ~INFRACFG_AO_MTS_BUS_DCM_REG0_MASK) |
            INFRACFG_AO_MTS_BUS_DCM_REG0_ON);
    } else {
        /* TINFO = "Turn OFF DCM 'infracfg_ao_mts_bus_dcm'" */
        reg_write(INFRA_BUS_DCM_CTRL,
            (reg_read(INFRA_BUS_DCM_CTRL) &
            ~INFRACFG_AO_MTS_BUS_DCM_REG0_MASK) |
            INFRACFG_AO_MTS_BUS_DCM_REG0_OFF);
    }
}

#define INFRA_AO_BCRM_INFRA_BUS_DCM_REG0_MASK ((0x1 << 0) | \
            (0x1 << 1) | \
            (0x1 << 7) | \
            (0x1f << 16))
#define INFRA_AO_BCRM_INFRA_BUS_DCM_REG1_MASK ((0xfff << 20))
#define INFRA_AO_BCRM_INFRA_BUS_DCM_REG2_MASK ((0x1 << 0))
#define INFRA_AO_BCRM_INFRA_BUS_DCM_REG0_ON ((0x1 << 0) | \
            (0x1 << 1) | \
            (0x1 << 7) | \
            (0x0 << 16))
#define INFRA_AO_BCRM_INFRA_BUS_DCM_REG1_ON ((0x10 << 20))
#define INFRA_AO_BCRM_INFRA_BUS_DCM_REG2_ON ((0x1 << 0))
#define INFRA_AO_BCRM_INFRA_BUS_DCM_REG0_OFF ((0x1 << 0) | \
            (0x1 << 1) | \
            (0x0 << 7) | \
            (0x0 << 16))
#define INFRA_AO_BCRM_INFRA_BUS_DCM_REG1_OFF ((0x10 << 20))
#define INFRA_AO_BCRM_INFRA_BUS_DCM_REG2_OFF ((0x0 << 0))

bool dcm_infra_ao_bcrm_infra_bus_dcm_is_on(void)
{
    bool ret = true;

    ret &= ((reg_read(VDNR_DCM_TOP_INFRA_PAR_BUS_u_INFRA_PAR_BUS_CTRL_0) &
        INFRA_AO_BCRM_INFRA_BUS_DCM_REG0_MASK) ==
        (unsigned int) INFRA_AO_BCRM_INFRA_BUS_DCM_REG0_ON);
    ret &= ((reg_read(VDNR_DCM_TOP_INFRA_PAR_BUS_u_INFRA_PAR_BUS_CTRL_1) &
        INFRA_AO_BCRM_INFRA_BUS_DCM_REG1_MASK) ==
        (unsigned int) INFRA_AO_BCRM_INFRA_BUS_DCM_REG1_ON);
    ret &= ((reg_read(VDNR_DCM_TOP_INFRA_PAR_BUS_u_INFRA_PAR_BUS_CTRL_2) &
        INFRA_AO_BCRM_INFRA_BUS_DCM_REG2_MASK) ==
        (unsigned int) INFRA_AO_BCRM_INFRA_BUS_DCM_REG2_ON);

    return ret;
}

void dcm_infra_ao_bcrm_infra_bus_dcm(int on)
{
    if (on) {
        /* TINFO = "Turn ON DCM 'infra_ao_bcrm_infra_bus_dcm'" */
        reg_write(VDNR_DCM_TOP_INFRA_PAR_BUS_u_INFRA_PAR_BUS_CTRL_0,
            (reg_read(
            VDNR_DCM_TOP_INFRA_PAR_BUS_u_INFRA_PAR_BUS_CTRL_0) &
            ~INFRA_AO_BCRM_INFRA_BUS_DCM_REG0_MASK) |
            INFRA_AO_BCRM_INFRA_BUS_DCM_REG0_ON);
        reg_write(VDNR_DCM_TOP_INFRA_PAR_BUS_u_INFRA_PAR_BUS_CTRL_1,
            (reg_read(
            VDNR_DCM_TOP_INFRA_PAR_BUS_u_INFRA_PAR_BUS_CTRL_1) &
            ~INFRA_AO_BCRM_INFRA_BUS_DCM_REG1_MASK) |
            INFRA_AO_BCRM_INFRA_BUS_DCM_REG1_ON);
        reg_write(VDNR_DCM_TOP_INFRA_PAR_BUS_u_INFRA_PAR_BUS_CTRL_2,
            (reg_read(
            VDNR_DCM_TOP_INFRA_PAR_BUS_u_INFRA_PAR_BUS_CTRL_2) &
            ~INFRA_AO_BCRM_INFRA_BUS_DCM_REG2_MASK) |
            INFRA_AO_BCRM_INFRA_BUS_DCM_REG2_ON);
    } else {
        /* TINFO = "Turn OFF DCM 'infra_ao_bcrm_infra_bus_dcm'" */
        reg_write(VDNR_DCM_TOP_INFRA_PAR_BUS_u_INFRA_PAR_BUS_CTRL_0,
            (reg_read(
            VDNR_DCM_TOP_INFRA_PAR_BUS_u_INFRA_PAR_BUS_CTRL_0) &
            ~INFRA_AO_BCRM_INFRA_BUS_DCM_REG0_MASK) |
            INFRA_AO_BCRM_INFRA_BUS_DCM_REG0_OFF);
        reg_write(VDNR_DCM_TOP_INFRA_PAR_BUS_u_INFRA_PAR_BUS_CTRL_1,
            (reg_read(
            VDNR_DCM_TOP_INFRA_PAR_BUS_u_INFRA_PAR_BUS_CTRL_1) &
            ~INFRA_AO_BCRM_INFRA_BUS_DCM_REG1_MASK) |
            INFRA_AO_BCRM_INFRA_BUS_DCM_REG1_OFF);
        reg_write(VDNR_DCM_TOP_INFRA_PAR_BUS_u_INFRA_PAR_BUS_CTRL_2,
            (reg_read(
            VDNR_DCM_TOP_INFRA_PAR_BUS_u_INFRA_PAR_BUS_CTRL_2) &
            ~INFRA_AO_BCRM_INFRA_BUS_DCM_REG2_MASK) |
            INFRA_AO_BCRM_INFRA_BUS_DCM_REG2_OFF);
    }
}

#define INFRA_AO_BCRM_INFRA_BUS_FMEM_SUB_DCM_REG0_MASK ((0x1 << 3) | \
            (0x1 << 4))
#define INFRA_AO_BCRM_INFRA_BUS_FMEM_SUB_DCM_REG1_MASK ((0x1 << 5) | \
            (0x1f << 15))
#define INFRA_AO_BCRM_INFRA_BUS_FMEM_SUB_DCM_REG2_MASK ((0xfff << 1) | \
            (0x1 << 13))
#define INFRA_AO_BCRM_INFRA_BUS_FMEM_SUB_DCM_REG0_ON ((0x1 << 3) | \
            (0x1 << 4))
#define INFRA_AO_BCRM_INFRA_BUS_FMEM_SUB_DCM_REG1_ON ((0x1 << 5) | \
            (0x0 << 15))
#define INFRA_AO_BCRM_INFRA_BUS_FMEM_SUB_DCM_REG2_ON ((0x10 << 1) | \
            (0x1 << 13))
#define INFRA_AO_BCRM_INFRA_BUS_FMEM_SUB_DCM_REG0_OFF ((0x1 << 3) | \
            (0x1 << 4))
#define INFRA_AO_BCRM_INFRA_BUS_FMEM_SUB_DCM_REG1_OFF ((0x0 << 5) | \
            (0x0 << 15))
#define INFRA_AO_BCRM_INFRA_BUS_FMEM_SUB_DCM_REG2_OFF ((0x10 << 1) | \
            (0x0 << 13))

bool dcm_infra_ao_bcrm_infra_bus_fmem_sub_dcm_is_on(void)
{
    bool ret = true;

    ret &= ((reg_read(VDNR_DCM_TOP_INFRA_PAR_BUS_u_INFRA_PAR_BUS_CTRL_0) &
        INFRA_AO_BCRM_INFRA_BUS_FMEM_SUB_DCM_REG0_MASK) ==
        (unsigned int) INFRA_AO_BCRM_INFRA_BUS_FMEM_SUB_DCM_REG0_ON);
    ret &= ((reg_read(VDNR_DCM_TOP_INFRA_PAR_BUS_u_INFRA_PAR_BUS_CTRL_1) &
        INFRA_AO_BCRM_INFRA_BUS_FMEM_SUB_DCM_REG1_MASK) ==
        (unsigned int) INFRA_AO_BCRM_INFRA_BUS_FMEM_SUB_DCM_REG1_ON);
    ret &= ((reg_read(VDNR_DCM_TOP_INFRA_PAR_BUS_u_INFRA_PAR_BUS_CTRL_2) &
        INFRA_AO_BCRM_INFRA_BUS_FMEM_SUB_DCM_REG2_MASK) ==
        (unsigned int) INFRA_AO_BCRM_INFRA_BUS_FMEM_SUB_DCM_REG2_ON);

    return ret;
}

void dcm_infra_ao_bcrm_infra_bus_fmem_sub_dcm(int on)
{
    if (on) {
            /* TINFO = "Turn ON DCM 'infra_ao_bcrm_infra_bus_fmem_sub_dcm'" */
        reg_write(VDNR_DCM_TOP_INFRA_PAR_BUS_u_INFRA_PAR_BUS_CTRL_0,
            (reg_read(
            VDNR_DCM_TOP_INFRA_PAR_BUS_u_INFRA_PAR_BUS_CTRL_0) &
            ~INFRA_AO_BCRM_INFRA_BUS_FMEM_SUB_DCM_REG0_MASK) |
            INFRA_AO_BCRM_INFRA_BUS_FMEM_SUB_DCM_REG0_ON);
        reg_write(VDNR_DCM_TOP_INFRA_PAR_BUS_u_INFRA_PAR_BUS_CTRL_1,
            (reg_read(
            VDNR_DCM_TOP_INFRA_PAR_BUS_u_INFRA_PAR_BUS_CTRL_1) &
            ~INFRA_AO_BCRM_INFRA_BUS_FMEM_SUB_DCM_REG1_MASK) |
            INFRA_AO_BCRM_INFRA_BUS_FMEM_SUB_DCM_REG1_ON);
        reg_write(VDNR_DCM_TOP_INFRA_PAR_BUS_u_INFRA_PAR_BUS_CTRL_2,
            (reg_read(
            VDNR_DCM_TOP_INFRA_PAR_BUS_u_INFRA_PAR_BUS_CTRL_2) &
            ~INFRA_AO_BCRM_INFRA_BUS_FMEM_SUB_DCM_REG2_MASK) |
            INFRA_AO_BCRM_INFRA_BUS_FMEM_SUB_DCM_REG2_ON);
    } else {
            /* TINFO = "Turn OFF DCM 'infra_ao_bcrm_infra_bus_fmem_sub_dcm'" */
        reg_write(VDNR_DCM_TOP_INFRA_PAR_BUS_u_INFRA_PAR_BUS_CTRL_0,
            (reg_read(
            VDNR_DCM_TOP_INFRA_PAR_BUS_u_INFRA_PAR_BUS_CTRL_0) &
            ~INFRA_AO_BCRM_INFRA_BUS_FMEM_SUB_DCM_REG0_MASK) |
            INFRA_AO_BCRM_INFRA_BUS_FMEM_SUB_DCM_REG0_OFF);
        reg_write(VDNR_DCM_TOP_INFRA_PAR_BUS_u_INFRA_PAR_BUS_CTRL_1,
            (reg_read(
            VDNR_DCM_TOP_INFRA_PAR_BUS_u_INFRA_PAR_BUS_CTRL_1) &
            ~INFRA_AO_BCRM_INFRA_BUS_FMEM_SUB_DCM_REG1_MASK) |
            INFRA_AO_BCRM_INFRA_BUS_FMEM_SUB_DCM_REG1_OFF);
        reg_write(VDNR_DCM_TOP_INFRA_PAR_BUS_u_INFRA_PAR_BUS_CTRL_2,
            (reg_read(
            VDNR_DCM_TOP_INFRA_PAR_BUS_u_INFRA_PAR_BUS_CTRL_2) &
            ~INFRA_AO_BCRM_INFRA_BUS_FMEM_SUB_DCM_REG2_MASK) |
            INFRA_AO_BCRM_INFRA_BUS_FMEM_SUB_DCM_REG2_OFF);
    }
}

#define INFRACFG_AO_MEM_DCM_EMI_GROUP_REG0_MASK ((0x1 << 20) | \
            (0x1 << 21) | \
            (0x1 << 22) | \
            (0x1 << 23) | \
            (0xf << 24) | \
            (0xf << 28))
#define INFRACFG_AO_MEM_DCM_EMI_GROUP_REG1_MASK ((0x1 << 20) | \
            (0x1 << 21) | \
            (0x1 << 22) | \
            (0x1 << 23) | \
            (0xf << 24) | \
            (0xf << 28))
#define INFRACFG_AO_MEM_DCM_EMI_GROUP_REG0_ON ((0x1 << 20) | \
            (0x0 << 21) | \
            (0x1 << 22) | \
            (0x1 << 23) | \
            (0xf << 24) | \
            (0x4 << 28))
#define INFRACFG_AO_MEM_DCM_EMI_GROUP_REG1_ON ((0x0 << 20) | \
            (0x0 << 21) | \
            (0x0 << 22) | \
            (0x0 << 23) | \
            (0x0 << 24) | \
            (0x0 << 28))
#define INFRACFG_AO_MEM_DCM_EMI_GROUP_REG0_OFF ((0x0 << 20) | \
            (0x0 << 21) | \
            (0x0 << 22) | \
            (0x0 << 23) | \
            (0x0 << 24) | \
            (0x0 << 28))
#define INFRACFG_AO_MEM_DCM_EMI_GROUP_REG1_OFF ((0x0 << 20) | \
            (0x0 << 21) | \
            (0x0 << 22) | \
            (0x0 << 23) | \
            (0x0 << 24) | \
            (0x0 << 28))

bool dcm_infracfg_ao_mem_dcm_emi_group_is_on(void)
{
    bool ret = true;

    ret &= ((reg_read(INFRA_EMI_IDLE_BIT_EN_0) &
        INFRACFG_AO_MEM_DCM_EMI_GROUP_REG0_MASK) ==
        (unsigned int) INFRACFG_AO_MEM_DCM_EMI_GROUP_REG0_ON);
    ret &= ((reg_read(INFRA_EMI_IDLE_BIT_EN_1) &
        INFRACFG_AO_MEM_DCM_EMI_GROUP_REG1_MASK) ==
        (unsigned int) INFRACFG_AO_MEM_DCM_EMI_GROUP_REG1_ON);

    return ret;
}

void dcm_infracfg_ao_mem_dcm_emi_group(int on)
{
    if (on) {
        /* TINFO = "Turn ON DCM 'infracfg_ao_mem_dcm_emi_group'" */
        reg_write(INFRA_EMI_IDLE_BIT_EN_0,
            (reg_read(INFRA_EMI_IDLE_BIT_EN_0) &
            ~INFRACFG_AO_MEM_DCM_EMI_GROUP_REG0_MASK) |
            INFRACFG_AO_MEM_DCM_EMI_GROUP_REG0_ON);
        reg_write(INFRA_EMI_IDLE_BIT_EN_1,
            (reg_read(INFRA_EMI_IDLE_BIT_EN_1) &
            ~INFRACFG_AO_MEM_DCM_EMI_GROUP_REG1_MASK) |
            INFRACFG_AO_MEM_DCM_EMI_GROUP_REG1_ON);
    } else {
        /* TINFO = "Turn OFF DCM 'infracfg_ao_mem_dcm_emi_group'" */
        reg_write(INFRA_EMI_IDLE_BIT_EN_0,
            (reg_read(INFRA_EMI_IDLE_BIT_EN_0) &
            ~INFRACFG_AO_MEM_DCM_EMI_GROUP_REG0_MASK) |
            INFRACFG_AO_MEM_DCM_EMI_GROUP_REG0_OFF);
        reg_write(INFRA_EMI_IDLE_BIT_EN_1,
            (reg_read(INFRA_EMI_IDLE_BIT_EN_1) &
            ~INFRACFG_AO_MEM_DCM_EMI_GROUP_REG1_MASK) |
            INFRACFG_AO_MEM_DCM_EMI_GROUP_REG1_OFF);
    }
}

#define PERI_AO_BCRM_PERI_BUS_DCM_REG0_MASK ((0x1 << 1) | \
            (0x1 << 4) | \
            (0x1 << 7) | \
            (0x1 << 9) | \
            (0x1 << 24))
#define PERI_AO_BCRM_PERI_BUS_DCM_REG1_MASK ((0x1 << 10))
#define PERI_AO_BCRM_PERI_BUS_DCM_REG0_ON ((0x1 << 1) | \
            (0x1 << 4) | \
            (0x1 << 7) | \
            (0x1 << 9) | \
            (0x1 << 24))
#define PERI_AO_BCRM_PERI_BUS_DCM_REG1_ON ((0x1 << 10))
#define PERI_AO_BCRM_PERI_BUS_DCM_REG0_OFF ((0x0 << 1) | \
            (0x0 << 4) | \
            (0x0 << 7) | \
            (0x0 << 9) | \
            (0x0 << 24))
#define PERI_AO_BCRM_PERI_BUS_DCM_REG1_OFF ((0x0 << 10))

bool dcm_peri_ao_bcrm_peri_bus_dcm_is_on(void)
{
    bool ret = true;

    ret &= ((reg_read(VDNR_DCM_TOP_PERI_PAR_BUS_u_PERI_PAR_BUS_CTRL_0) &
        PERI_AO_BCRM_PERI_BUS_DCM_REG0_MASK) ==
        (unsigned int) PERI_AO_BCRM_PERI_BUS_DCM_REG0_ON);
    ret &= ((reg_read(VDNR_DCM_TOP_PERI_PAR_BUS_u_PERI_PAR_BUS_CTRL_1) &
        PERI_AO_BCRM_PERI_BUS_DCM_REG1_MASK) ==
        (unsigned int) PERI_AO_BCRM_PERI_BUS_DCM_REG1_ON);

    return ret;
}

void dcm_peri_ao_bcrm_peri_bus_dcm(int on)
{
    if (on) {
        /* TINFO = "Turn ON DCM 'peri_ao_bcrm_peri_bus_dcm'" */
        reg_write(VDNR_DCM_TOP_PERI_PAR_BUS_u_PERI_PAR_BUS_CTRL_0,
            (reg_read(
            VDNR_DCM_TOP_PERI_PAR_BUS_u_PERI_PAR_BUS_CTRL_0) &
            ~PERI_AO_BCRM_PERI_BUS_DCM_REG0_MASK) |
            PERI_AO_BCRM_PERI_BUS_DCM_REG0_ON);
        reg_write(VDNR_DCM_TOP_PERI_PAR_BUS_u_PERI_PAR_BUS_CTRL_1,
            (reg_read(
            VDNR_DCM_TOP_PERI_PAR_BUS_u_PERI_PAR_BUS_CTRL_1) &
            ~PERI_AO_BCRM_PERI_BUS_DCM_REG1_MASK) |
            PERI_AO_BCRM_PERI_BUS_DCM_REG1_ON);
    } else {
        /* TINFO = "Turn OFF DCM 'peri_ao_bcrm_peri_bus_dcm'" */
        reg_write(VDNR_DCM_TOP_PERI_PAR_BUS_u_PERI_PAR_BUS_CTRL_0,
            (reg_read(
            VDNR_DCM_TOP_PERI_PAR_BUS_u_PERI_PAR_BUS_CTRL_0) &
            ~PERI_AO_BCRM_PERI_BUS_DCM_REG0_MASK) |
            PERI_AO_BCRM_PERI_BUS_DCM_REG0_OFF);
        reg_write(VDNR_DCM_TOP_PERI_PAR_BUS_u_PERI_PAR_BUS_CTRL_1,
            (reg_read(
            VDNR_DCM_TOP_PERI_PAR_BUS_u_PERI_PAR_BUS_CTRL_1) &
            ~PERI_AO_BCRM_PERI_BUS_DCM_REG1_MASK) |
            PERI_AO_BCRM_PERI_BUS_DCM_REG1_OFF);
    }
}

#define VLP_AO_BCRM_VLP_BUS_DCM_REG0_MASK ((0x1 << 0) | \
            (0x1 << 1) | \
            (0x1 << 4) | \
            (0x1f << 13))
#define VLP_AO_BCRM_VLP_BUS_DCM_REG0_ON ((0x1 << 0) | \
            (0x1 << 1) | \
            (0x1 << 4) | \
            (0x0 << 13))
#define VLP_AO_BCRM_VLP_BUS_DCM_REG0_OFF ((0x1 << 0) | \
            (0x1 << 1) | \
            (0x0 << 4) | \
            (0x0 << 13))

bool dcm_vlp_ao_bcrm_vlp_bus_dcm_is_on(void)
{
    bool ret = true;

    ret &= ((reg_read(VDNR_DCM_TOP_VLP_PAR_BUS_u_VLP_PAR_BUS_CTRL_0) &
        VLP_AO_BCRM_VLP_BUS_DCM_REG0_MASK) ==
        (unsigned int) VLP_AO_BCRM_VLP_BUS_DCM_REG0_ON);

    return ret;
}

void dcm_vlp_ao_bcrm_vlp_bus_dcm(int on)
{
    if (on) {
        /* TINFO = "Turn ON DCM 'vlp_ao_bcrm_vlp_bus_dcm'" */
        reg_write(VDNR_DCM_TOP_VLP_PAR_BUS_u_VLP_PAR_BUS_CTRL_0,
            (reg_read(
            VDNR_DCM_TOP_VLP_PAR_BUS_u_VLP_PAR_BUS_CTRL_0) &
            ~VLP_AO_BCRM_VLP_BUS_DCM_REG0_MASK) |
            VLP_AO_BCRM_VLP_BUS_DCM_REG0_ON);
    } else {
        /* TINFO = "Turn OFF DCM 'vlp_ao_bcrm_vlp_bus_dcm'" */
        reg_write(VDNR_DCM_TOP_VLP_PAR_BUS_u_VLP_PAR_BUS_CTRL_0,
            (reg_read(
            VDNR_DCM_TOP_VLP_PAR_BUS_u_VLP_PAR_BUS_CTRL_0) &
            ~VLP_AO_BCRM_VLP_BUS_DCM_REG0_MASK) |
            VLP_AO_BCRM_VLP_BUS_DCM_REG0_OFF);
    }
}

#define MP_CPUSYS_TOP_ADB_DCM_REG0_MASK ((0x1 << 17))
#define MP_CPUSYS_TOP_ADB_DCM_REG1_MASK ((0x1 << 15) | \
            (0x1 << 16) | \
            (0x1 << 17) | \
            (0x1 << 18) | \
            (0x1 << 21))
#define MP_CPUSYS_TOP_ADB_DCM_REG2_MASK ((0x1 << 15) | \
            (0x1 << 16) | \
            (0x1 << 17) | \
            (0x1 << 18))
#define MP_CPUSYS_TOP_ADB_DCM_REG0_ON ((0x1 << 17))
#define MP_CPUSYS_TOP_ADB_DCM_REG1_ON ((0x1 << 15) | \
            (0x1 << 16) | \
            (0x1 << 17) | \
            (0x1 << 18) | \
            (0x1 << 21))
#define MP_CPUSYS_TOP_ADB_DCM_REG2_ON ((0x1 << 15) | \
            (0x1 << 16) | \
            (0x1 << 17) | \
            (0x1 << 18))
#define MP_CPUSYS_TOP_ADB_DCM_REG0_OFF ((0x0 << 17))
#define MP_CPUSYS_TOP_ADB_DCM_REG1_OFF ((0x0 << 15) | \
            (0x0 << 16) | \
            (0x0 << 17) | \
            (0x0 << 18) | \
            (0x0 << 21))
#define MP_CPUSYS_TOP_ADB_DCM_REG2_OFF ((0x0 << 15) | \
            (0x0 << 16) | \
            (0x0 << 17) | \
            (0x0 << 18))

bool dcm_mp_cpusys_top_adb_dcm_is_on(void)
{
    bool ret = true;

    ret &= ((reg_read(MP_CPUSYS_TOP_MP_ADB_DCM_CFG0) &
        MP_CPUSYS_TOP_ADB_DCM_REG0_MASK) ==
        (unsigned int) MP_CPUSYS_TOP_ADB_DCM_REG0_ON);
    ret &= ((reg_read(MP_CPUSYS_TOP_MP_ADB_DCM_CFG4) &
        MP_CPUSYS_TOP_ADB_DCM_REG1_MASK) ==
        (unsigned int) MP_CPUSYS_TOP_ADB_DCM_REG1_ON);
    ret &= ((reg_read(MP_CPUSYS_TOP_MCUSYS_DCM_CFG0) &
        MP_CPUSYS_TOP_ADB_DCM_REG2_MASK) ==
        (unsigned int) MP_CPUSYS_TOP_ADB_DCM_REG2_ON);

    return ret;
}

void dcm_mp_cpusys_top_adb_dcm(int on)
{
    if (on) {
        /* TINFO = "Turn ON DCM 'mp_cpusys_top_adb_dcm'" */
        reg_write(MP_CPUSYS_TOP_MP_ADB_DCM_CFG0,
            (reg_read(MP_CPUSYS_TOP_MP_ADB_DCM_CFG0) &
            ~MP_CPUSYS_TOP_ADB_DCM_REG0_MASK) |
            MP_CPUSYS_TOP_ADB_DCM_REG0_ON);
        reg_write(MP_CPUSYS_TOP_MP_ADB_DCM_CFG4,
            (reg_read(MP_CPUSYS_TOP_MP_ADB_DCM_CFG4) &
            ~MP_CPUSYS_TOP_ADB_DCM_REG1_MASK) |
            MP_CPUSYS_TOP_ADB_DCM_REG1_ON);
        reg_write(MP_CPUSYS_TOP_MCUSYS_DCM_CFG0,
            (reg_read(MP_CPUSYS_TOP_MCUSYS_DCM_CFG0) &
            ~MP_CPUSYS_TOP_ADB_DCM_REG2_MASK) |
            MP_CPUSYS_TOP_ADB_DCM_REG2_ON);
    } else {
        /* TINFO = "Turn OFF DCM 'mp_cpusys_top_adb_dcm'" */
        reg_write(MP_CPUSYS_TOP_MP_ADB_DCM_CFG0,
            (reg_read(MP_CPUSYS_TOP_MP_ADB_DCM_CFG0) &
            ~MP_CPUSYS_TOP_ADB_DCM_REG0_MASK) |
            MP_CPUSYS_TOP_ADB_DCM_REG0_OFF);
        reg_write(MP_CPUSYS_TOP_MP_ADB_DCM_CFG4,
            (reg_read(MP_CPUSYS_TOP_MP_ADB_DCM_CFG4) &
            ~MP_CPUSYS_TOP_ADB_DCM_REG1_MASK) |
            MP_CPUSYS_TOP_ADB_DCM_REG1_OFF);
        reg_write(MP_CPUSYS_TOP_MCUSYS_DCM_CFG0,
            (reg_read(MP_CPUSYS_TOP_MCUSYS_DCM_CFG0) &
            ~MP_CPUSYS_TOP_ADB_DCM_REG2_MASK) |
            MP_CPUSYS_TOP_ADB_DCM_REG2_OFF);
    }
}

#define MP_CPUSYS_TOP_APB_DCM_REG0_MASK ((0x1 << 5))
#define MP_CPUSYS_TOP_APB_DCM_REG1_MASK ((0x1 << 8))
#define MP_CPUSYS_TOP_APB_DCM_REG2_MASK ((0x1 << 16))
#define MP_CPUSYS_TOP_APB_DCM_REG0_ON ((0x1 << 5))
#define MP_CPUSYS_TOP_APB_DCM_REG1_ON ((0x1 << 8))
#define MP_CPUSYS_TOP_APB_DCM_REG2_ON ((0x1 << 16))
#define MP_CPUSYS_TOP_APB_DCM_REG0_OFF ((0x0 << 5))
#define MP_CPUSYS_TOP_APB_DCM_REG1_OFF ((0x0 << 8))
#define MP_CPUSYS_TOP_APB_DCM_REG2_OFF ((0x0 << 16))

bool dcm_mp_cpusys_top_apb_dcm_is_on(void)
{
    bool ret = true;

    ret &= ((reg_read(MP_CPUSYS_TOP_MP_MISC_DCM_CFG0) &
        MP_CPUSYS_TOP_APB_DCM_REG0_MASK) ==
        (unsigned int) MP_CPUSYS_TOP_APB_DCM_REG0_ON);
    ret &= ((reg_read(MP_CPUSYS_TOP_MCUSYS_DCM_CFG0) &
        MP_CPUSYS_TOP_APB_DCM_REG1_MASK) ==
        (unsigned int) MP_CPUSYS_TOP_APB_DCM_REG1_ON);
    ret &= ((reg_read(MP_CPUSYS_TOP_MP0_DCM_CFG0) &
        MP_CPUSYS_TOP_APB_DCM_REG2_MASK) ==
        (unsigned int) MP_CPUSYS_TOP_APB_DCM_REG2_ON);

    return ret;
}

void dcm_mp_cpusys_top_apb_dcm(int on)
{
    if (on) {
        /* TINFO = "Turn ON DCM 'mp_cpusys_top_apb_dcm'" */
        reg_write(MP_CPUSYS_TOP_MP_MISC_DCM_CFG0,
            (reg_read(MP_CPUSYS_TOP_MP_MISC_DCM_CFG0) &
            ~MP_CPUSYS_TOP_APB_DCM_REG0_MASK) |
            MP_CPUSYS_TOP_APB_DCM_REG0_ON);
        reg_write(MP_CPUSYS_TOP_MCUSYS_DCM_CFG0,
            (reg_read(MP_CPUSYS_TOP_MCUSYS_DCM_CFG0) &
            ~MP_CPUSYS_TOP_APB_DCM_REG1_MASK) |
            MP_CPUSYS_TOP_APB_DCM_REG1_ON);
        reg_write(MP_CPUSYS_TOP_MP0_DCM_CFG0,
            (reg_read(MP_CPUSYS_TOP_MP0_DCM_CFG0) &
            ~MP_CPUSYS_TOP_APB_DCM_REG2_MASK) |
            MP_CPUSYS_TOP_APB_DCM_REG2_ON);
    } else {
        /* TINFO = "Turn OFF DCM 'mp_cpusys_top_apb_dcm'" */
        reg_write(MP_CPUSYS_TOP_MP_MISC_DCM_CFG0,
            (reg_read(MP_CPUSYS_TOP_MP_MISC_DCM_CFG0) &
            ~MP_CPUSYS_TOP_APB_DCM_REG0_MASK) |
            MP_CPUSYS_TOP_APB_DCM_REG0_OFF);
        reg_write(MP_CPUSYS_TOP_MCUSYS_DCM_CFG0,
            (reg_read(MP_CPUSYS_TOP_MCUSYS_DCM_CFG0) &
            ~MP_CPUSYS_TOP_APB_DCM_REG1_MASK) |
            MP_CPUSYS_TOP_APB_DCM_REG1_OFF);
        reg_write(MP_CPUSYS_TOP_MP0_DCM_CFG0,
            (reg_read(MP_CPUSYS_TOP_MP0_DCM_CFG0) &
            ~MP_CPUSYS_TOP_APB_DCM_REG2_MASK) |
            MP_CPUSYS_TOP_APB_DCM_REG2_OFF);
    }
}

#define MP_CPUSYS_TOP_BUS_PLL_DIV_DCM_REG0_MASK ((0x1 << 11) | \
            (0x1 << 24) | \
            (0x1 << 25))
#define MP_CPUSYS_TOP_BUS_PLL_DIV_DCM_REG0_ON ((0x1 << 11) | \
            (0x1 << 24) | \
            (0x1 << 25))
#define MP_CPUSYS_TOP_BUS_PLL_DIV_DCM_REG0_OFF ((0x0 << 11) | \
            (0x0 << 24) | \
            (0x0 << 25))

bool dcm_mp_cpusys_top_bus_pll_div_dcm_is_on(void)
{
    bool ret = true;

    ret &= ((reg_read(MP_CPUSYS_TOP_BUS_PLLDIV_CFG) &
        MP_CPUSYS_TOP_BUS_PLL_DIV_DCM_REG0_MASK) ==
        (unsigned int) MP_CPUSYS_TOP_BUS_PLL_DIV_DCM_REG0_ON);

    return ret;
}

void dcm_mp_cpusys_top_bus_pll_div_dcm(int on)
{
    if (on) {
        /* TINFO = "Turn ON DCM 'mp_cpusys_top_bus_pll_div_dcm'" */
        reg_write(MP_CPUSYS_TOP_BUS_PLLDIV_CFG,
            (reg_read(MP_CPUSYS_TOP_BUS_PLLDIV_CFG) &
            ~MP_CPUSYS_TOP_BUS_PLL_DIV_DCM_REG0_MASK) |
            MP_CPUSYS_TOP_BUS_PLL_DIV_DCM_REG0_ON);
    } else {
        /* TINFO = "Turn OFF DCM 'mp_cpusys_top_bus_pll_div_dcm'" */
        reg_write(MP_CPUSYS_TOP_BUS_PLLDIV_CFG,
            (reg_read(MP_CPUSYS_TOP_BUS_PLLDIV_CFG) &
            ~MP_CPUSYS_TOP_BUS_PLL_DIV_DCM_REG0_MASK) |
            MP_CPUSYS_TOP_BUS_PLL_DIV_DCM_REG0_OFF);
    }
}

#define MP_CPUSYS_TOP_CORE_STALL_DCM_REG0_MASK ((0x1 << 0))
#define MP_CPUSYS_TOP_CORE_STALL_DCM_REG0_ON ((0x1 << 0))
#define MP_CPUSYS_TOP_CORE_STALL_DCM_REG0_OFF ((0x0 << 0))

bool dcm_mp_cpusys_top_core_stall_dcm_is_on(void)
{
    bool ret = true;

    ret &= ((reg_read(MP_CPUSYS_TOP_MP0_DCM_CFG7) &
        MP_CPUSYS_TOP_CORE_STALL_DCM_REG0_MASK) ==
        (unsigned int) MP_CPUSYS_TOP_CORE_STALL_DCM_REG0_ON);

    return ret;
}

void dcm_mp_cpusys_top_core_stall_dcm(int on)
{
    if (on) {
        /* TINFO = "Turn ON DCM 'mp_cpusys_top_core_stall_dcm'" */
        reg_write(MP_CPUSYS_TOP_MP0_DCM_CFG7,
            (reg_read(MP_CPUSYS_TOP_MP0_DCM_CFG7) &
            ~MP_CPUSYS_TOP_CORE_STALL_DCM_REG0_MASK) |
            MP_CPUSYS_TOP_CORE_STALL_DCM_REG0_ON);
    } else {
        /* TINFO = "Turn OFF DCM 'mp_cpusys_top_core_stall_dcm'" */
        reg_write(MP_CPUSYS_TOP_MP0_DCM_CFG7,
            (reg_read(MP_CPUSYS_TOP_MP0_DCM_CFG7) &
            ~MP_CPUSYS_TOP_CORE_STALL_DCM_REG0_MASK) |
            MP_CPUSYS_TOP_CORE_STALL_DCM_REG0_OFF);
    }
}

#define MP_CPUSYS_TOP_CPUBIU_DCM_REG0_MASK ((0xffff << 0))
#define MP_CPUSYS_TOP_CPUBIU_DCM_REG0_ON ((0xffff << 0))
#define MP_CPUSYS_TOP_CPUBIU_DCM_REG0_OFF ((0x0 << 0))

bool dcm_mp_cpusys_top_cpubiu_dcm_is_on(void)
{
    bool ret = true;

    ret &= ((reg_read(MP_CPUSYS_TOP_MCSIC_DCM0) &
        MP_CPUSYS_TOP_CPUBIU_DCM_REG0_MASK) ==
        (unsigned int) MP_CPUSYS_TOP_CPUBIU_DCM_REG0_ON);

    return ret;
}

void dcm_mp_cpusys_top_cpubiu_dcm(int on)
{
    if (on) {
        /* TINFO = "Turn ON DCM 'mp_cpusys_top_cpubiu_dcm'" */
        reg_write(MP_CPUSYS_TOP_MCSIC_DCM0,
            (reg_read(MP_CPUSYS_TOP_MCSIC_DCM0) &
            ~MP_CPUSYS_TOP_CPUBIU_DCM_REG0_MASK) |
            MP_CPUSYS_TOP_CPUBIU_DCM_REG0_ON);
    } else {
        /* TINFO = "Turn OFF DCM 'mp_cpusys_top_cpubiu_dcm'" */
        reg_write(MP_CPUSYS_TOP_MCSIC_DCM0,
            (reg_read(MP_CPUSYS_TOP_MCSIC_DCM0) &
            ~MP_CPUSYS_TOP_CPUBIU_DCM_REG0_MASK) |
            MP_CPUSYS_TOP_CPUBIU_DCM_REG0_OFF);
    }
}

#define MP_CPUSYS_TOP_CPU_PLL_DIV_0_DCM_REG0_MASK ((0x1 << 24) | \
            (0x1 << 25))
#define MP_CPUSYS_TOP_CPU_PLL_DIV_0_DCM_REG0_ON ((0x1 << 24) | \
            (0x1 << 25))
#define MP_CPUSYS_TOP_CPU_PLL_DIV_0_DCM_REG0_OFF ((0x0 << 24) | \
            (0x0 << 25))

bool dcm_mp_cpusys_top_cpu_pll_div_0_dcm_is_on(void)
{
    bool ret = true;

    ret &= ((reg_read(MP_CPUSYS_TOP_CPU_PLLDIV_CFG0) &
        MP_CPUSYS_TOP_CPU_PLL_DIV_0_DCM_REG0_MASK) ==
        (unsigned int) MP_CPUSYS_TOP_CPU_PLL_DIV_0_DCM_REG0_ON);

    return ret;
}

void dcm_mp_cpusys_top_cpu_pll_div_0_dcm(int on)
{
    if (on) {
        /* TINFO = "Turn ON DCM 'mp_cpusys_top_cpu_pll_div_0_dcm'" */
        reg_write(MP_CPUSYS_TOP_CPU_PLLDIV_CFG0,
            (reg_read(MP_CPUSYS_TOP_CPU_PLLDIV_CFG0) &
            ~MP_CPUSYS_TOP_CPU_PLL_DIV_0_DCM_REG0_MASK) |
            MP_CPUSYS_TOP_CPU_PLL_DIV_0_DCM_REG0_ON);
    } else {
        /* TINFO = "Turn OFF DCM 'mp_cpusys_top_cpu_pll_div_0_dcm'" */
        reg_write(MP_CPUSYS_TOP_CPU_PLLDIV_CFG0,
            (reg_read(MP_CPUSYS_TOP_CPU_PLLDIV_CFG0) &
            ~MP_CPUSYS_TOP_CPU_PLL_DIV_0_DCM_REG0_MASK) |
            MP_CPUSYS_TOP_CPU_PLL_DIV_0_DCM_REG0_OFF);
    }
}

#define MP_CPUSYS_TOP_CPU_PLL_DIV_1_DCM_REG0_MASK ((0x1 << 24) | \
            (0x1 << 25))
#define MP_CPUSYS_TOP_CPU_PLL_DIV_1_DCM_REG0_ON ((0x1 << 24) | \
            (0x1 << 25))
#define MP_CPUSYS_TOP_CPU_PLL_DIV_1_DCM_REG0_OFF ((0x0 << 24) | \
            (0x0 << 25))

bool dcm_mp_cpusys_top_cpu_pll_div_1_dcm_is_on(void)
{
    bool ret = true;

    ret &= ((reg_read(MP_CPUSYS_TOP_CPU_PLLDIV_CFG1) &
        MP_CPUSYS_TOP_CPU_PLL_DIV_1_DCM_REG0_MASK) ==
        (unsigned int) MP_CPUSYS_TOP_CPU_PLL_DIV_1_DCM_REG0_ON);

    return ret;
}

void dcm_mp_cpusys_top_cpu_pll_div_1_dcm(int on)
{
    if (on) {
        /* TINFO = "Turn ON DCM 'mp_cpusys_top_cpu_pll_div_1_dcm'" */
        reg_write(MP_CPUSYS_TOP_CPU_PLLDIV_CFG1,
            (reg_read(MP_CPUSYS_TOP_CPU_PLLDIV_CFG1) &
            ~MP_CPUSYS_TOP_CPU_PLL_DIV_1_DCM_REG0_MASK) |
            MP_CPUSYS_TOP_CPU_PLL_DIV_1_DCM_REG0_ON);
    } else {
        /* TINFO = "Turn OFF DCM 'mp_cpusys_top_cpu_pll_div_1_dcm'" */
        reg_write(MP_CPUSYS_TOP_CPU_PLLDIV_CFG1,
            (reg_read(MP_CPUSYS_TOP_CPU_PLLDIV_CFG1) &
            ~MP_CPUSYS_TOP_CPU_PLL_DIV_1_DCM_REG0_MASK) |
            MP_CPUSYS_TOP_CPU_PLL_DIV_1_DCM_REG0_OFF);
    }
}

#define MP_CPUSYS_TOP_FCM_STALL_DCM_REG0_MASK ((0x1 << 4))
#define MP_CPUSYS_TOP_FCM_STALL_DCM_REG0_ON ((0x1 << 4))
#define MP_CPUSYS_TOP_FCM_STALL_DCM_REG0_OFF ((0x0 << 4))

bool dcm_mp_cpusys_top_fcm_stall_dcm_is_on(void)
{
    bool ret = true;

    ret &= ((reg_read(MP_CPUSYS_TOP_MP0_DCM_CFG7) &
        MP_CPUSYS_TOP_FCM_STALL_DCM_REG0_MASK) ==
        (unsigned int) MP_CPUSYS_TOP_FCM_STALL_DCM_REG0_ON);

    return ret;
}

void dcm_mp_cpusys_top_fcm_stall_dcm(int on)
{
    if (on) {
        /* TINFO = "Turn ON DCM 'mp_cpusys_top_fcm_stall_dcm'" */
        reg_write(MP_CPUSYS_TOP_MP0_DCM_CFG7,
            (reg_read(MP_CPUSYS_TOP_MP0_DCM_CFG7) &
            ~MP_CPUSYS_TOP_FCM_STALL_DCM_REG0_MASK) |
            MP_CPUSYS_TOP_FCM_STALL_DCM_REG0_ON);
    } else {
        /* TINFO = "Turn OFF DCM 'mp_cpusys_top_fcm_stall_dcm'" */
        reg_write(MP_CPUSYS_TOP_MP0_DCM_CFG7,
            (reg_read(MP_CPUSYS_TOP_MP0_DCM_CFG7) &
            ~MP_CPUSYS_TOP_FCM_STALL_DCM_REG0_MASK) |
            MP_CPUSYS_TOP_FCM_STALL_DCM_REG0_OFF);
    }
}

#define MP_CPUSYS_TOP_LAST_COR_IDLE_DCM_REG0_MASK ((0x1 << 31))
#define MP_CPUSYS_TOP_LAST_COR_IDLE_DCM_REG0_ON ((0x1 << 31))
#define MP_CPUSYS_TOP_LAST_COR_IDLE_DCM_REG0_OFF ((0x0 << 31))

bool dcm_mp_cpusys_top_last_cor_idle_dcm_is_on(void)
{
    bool ret = true;

    ret &= ((reg_read(MP_CPUSYS_TOP_BUS_PLLDIV_CFG) &
        MP_CPUSYS_TOP_LAST_COR_IDLE_DCM_REG0_MASK) ==
        (unsigned int) MP_CPUSYS_TOP_LAST_COR_IDLE_DCM_REG0_ON);

    return ret;
}

void dcm_mp_cpusys_top_last_cor_idle_dcm(int on)
{
    if (on) {
        /* TINFO = "Turn ON DCM 'mp_cpusys_top_last_cor_idle_dcm'" */
        reg_write(MP_CPUSYS_TOP_BUS_PLLDIV_CFG,
            (reg_read(MP_CPUSYS_TOP_BUS_PLLDIV_CFG) &
            ~MP_CPUSYS_TOP_LAST_COR_IDLE_DCM_REG0_MASK) |
            MP_CPUSYS_TOP_LAST_COR_IDLE_DCM_REG0_ON);
    } else {
        /* TINFO = "Turn OFF DCM 'mp_cpusys_top_last_cor_idle_dcm'" */
        reg_write(MP_CPUSYS_TOP_BUS_PLLDIV_CFG,
            (reg_read(MP_CPUSYS_TOP_BUS_PLLDIV_CFG) &
            ~MP_CPUSYS_TOP_LAST_COR_IDLE_DCM_REG0_MASK) |
            MP_CPUSYS_TOP_LAST_COR_IDLE_DCM_REG0_OFF);
    }
}

#define MP_CPUSYS_TOP_MISC_DCM_REG0_MASK ((0x1 << 1) | \
            (0x1 << 4))
#define MP_CPUSYS_TOP_MISC_DCM_REG0_ON ((0x1 << 1) | \
            (0x1 << 4))
#define MP_CPUSYS_TOP_MISC_DCM_REG0_OFF ((0x0 << 1) | \
            (0x0 << 4))

bool dcm_mp_cpusys_top_misc_dcm_is_on(void)
{
    bool ret = true;

    ret &= ((reg_read(MP_CPUSYS_TOP_MP_MISC_DCM_CFG0) &
        MP_CPUSYS_TOP_MISC_DCM_REG0_MASK) ==
        (unsigned int) MP_CPUSYS_TOP_MISC_DCM_REG0_ON);

    return ret;
}

void dcm_mp_cpusys_top_misc_dcm(int on)
{
    if (on) {
        /* TINFO = "Turn ON DCM 'mp_cpusys_top_misc_dcm'" */
        reg_write(MP_CPUSYS_TOP_MP_MISC_DCM_CFG0,
            (reg_read(MP_CPUSYS_TOP_MP_MISC_DCM_CFG0) &
            ~MP_CPUSYS_TOP_MISC_DCM_REG0_MASK) |
            MP_CPUSYS_TOP_MISC_DCM_REG0_ON);
    } else {
        /* TINFO = "Turn OFF DCM 'mp_cpusys_top_misc_dcm'" */
        reg_write(MP_CPUSYS_TOP_MP_MISC_DCM_CFG0,
            (reg_read(MP_CPUSYS_TOP_MP_MISC_DCM_CFG0) &
            ~MP_CPUSYS_TOP_MISC_DCM_REG0_MASK) |
            MP_CPUSYS_TOP_MISC_DCM_REG0_OFF);
    }
}

#define MP_CPUSYS_TOP_MP0_QDCM_REG0_MASK ((0x1 << 3))
#define MP_CPUSYS_TOP_MP0_QDCM_REG1_MASK ((0x1 << 0) | \
            (0x1 << 1) | \
            (0x1 << 2) | \
            (0x1 << 3))
#define MP_CPUSYS_TOP_MP0_QDCM_REG0_ON ((0x1 << 3))
#define MP_CPUSYS_TOP_MP0_QDCM_REG1_ON ((0x1 << 0) | \
            (0x1 << 1) | \
            (0x1 << 2) | \
            (0x1 << 3))
#define MP_CPUSYS_TOP_MP0_QDCM_REG0_OFF ((0x0 << 3))
#define MP_CPUSYS_TOP_MP0_QDCM_REG1_OFF ((0x0 << 0) | \
            (0x0 << 1) | \
            (0x0 << 2) | \
            (0x0 << 3))

bool dcm_mp_cpusys_top_mp0_qdcm_is_on(void)
{
    bool ret = true;

    ret &= ((reg_read(MP_CPUSYS_TOP_MP_MISC_DCM_CFG0) &
        MP_CPUSYS_TOP_MP0_QDCM_REG0_MASK) ==
        (unsigned int) MP_CPUSYS_TOP_MP0_QDCM_REG0_ON);
    ret &= ((reg_read(MP_CPUSYS_TOP_MP0_DCM_CFG0) &
        MP_CPUSYS_TOP_MP0_QDCM_REG1_MASK) ==
        (unsigned int) MP_CPUSYS_TOP_MP0_QDCM_REG1_ON);

    return ret;
}

void dcm_mp_cpusys_top_mp0_qdcm(int on)
{
    if (on) {
        /* TINFO = "Turn ON DCM 'mp_cpusys_top_mp0_qdcm'" */
        reg_write(MP_CPUSYS_TOP_MP_MISC_DCM_CFG0,
            (reg_read(MP_CPUSYS_TOP_MP_MISC_DCM_CFG0) &
            ~MP_CPUSYS_TOP_MP0_QDCM_REG0_MASK) |
            MP_CPUSYS_TOP_MP0_QDCM_REG0_ON);
        reg_write(MP_CPUSYS_TOP_MP0_DCM_CFG0,
            (reg_read(MP_CPUSYS_TOP_MP0_DCM_CFG0) &
            ~MP_CPUSYS_TOP_MP0_QDCM_REG1_MASK) |
            MP_CPUSYS_TOP_MP0_QDCM_REG1_ON);
    } else {
        /* TINFO = "Turn OFF DCM 'mp_cpusys_top_mp0_qdcm'" */
        reg_write(MP_CPUSYS_TOP_MP_MISC_DCM_CFG0,
            (reg_read(MP_CPUSYS_TOP_MP_MISC_DCM_CFG0) &
            ~MP_CPUSYS_TOP_MP0_QDCM_REG0_MASK) |
            MP_CPUSYS_TOP_MP0_QDCM_REG0_OFF);
        reg_write(MP_CPUSYS_TOP_MP0_DCM_CFG0,
            (reg_read(MP_CPUSYS_TOP_MP0_DCM_CFG0) &
            ~MP_CPUSYS_TOP_MP0_QDCM_REG1_MASK) |
            MP_CPUSYS_TOP_MP0_QDCM_REG1_OFF);
    }
}

#define CPCCFG_REG_EMI_WFIFO_REG0_MASK ((0x1 << 0) | \
            (0x1 << 1) | \
            (0x1 << 2) | \
            (0x1 << 3))
#define CPCCFG_REG_EMI_WFIFO_REG0_ON ((0x1 << 0) | \
            (0x1 << 1) | \
            (0x1 << 2) | \
            (0x1 << 3))
#define CPCCFG_REG_EMI_WFIFO_REG0_OFF ((0x0 << 0) | \
            (0x0 << 1) | \
            (0x0 << 2) | \
            (0x0 << 3))

bool dcm_cpccfg_reg_emi_wfifo_is_on(void)
{
    bool ret = true;

    ret &= ((reg_read(CPCCFG_REG_EMI_WFIFO) &
        CPCCFG_REG_EMI_WFIFO_REG0_MASK) ==
        (unsigned int) CPCCFG_REG_EMI_WFIFO_REG0_ON);

    return ret;
}

void dcm_cpccfg_reg_emi_wfifo(int on)
{
    if (on) {
        /* TINFO = "Turn ON DCM 'cpccfg_reg_emi_wfifo'" */
        reg_write(CPCCFG_REG_EMI_WFIFO,
            (reg_read(CPCCFG_REG_EMI_WFIFO) &
            ~CPCCFG_REG_EMI_WFIFO_REG0_MASK) |
            CPCCFG_REG_EMI_WFIFO_REG0_ON);
    } else {
        /* TINFO = "Turn OFF DCM 'cpccfg_reg_emi_wfifo'" */
        reg_write(CPCCFG_REG_EMI_WFIFO,
            (reg_read(CPCCFG_REG_EMI_WFIFO) &
            ~CPCCFG_REG_EMI_WFIFO_REG0_MASK) |
            CPCCFG_REG_EMI_WFIFO_REG0_OFF);
    }
}


