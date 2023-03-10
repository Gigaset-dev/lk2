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

/*====================auto gen code 20201013_225344=====================*/
#define INFRACFG_AO_AXIMEM_BUS_DCM_REG0_MASK ((0x1f << 12) | \
            (0x1 << 17) | \
            (0x1 << 18))
#define INFRACFG_AO_AXIMEM_BUS_DCM_REG0_ON ((0x10 << 12) | \
            (0x1 << 17) | \
            (0x0 << 18))
#define INFRACFG_AO_AXIMEM_BUS_DCM_REG0_OFF ((0x10 << 12) | \
            (0x0 << 17) | \
            (0x1 << 18))

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
            (0x1 << 23) | \
            (0x1 << 30))
#define INFRACFG_AO_INFRA_BUS_DCM_REG0_ON ((0x1 << 0) | \
            (0x1 << 1) | \
            (0x0 << 3) | \
            (0x0 << 4) | \
            (0x10 << 5) | \
            (0x1 << 20) | \
            (0x1 << 23) | \
            (0x1 << 30))
#define INFRACFG_AO_INFRA_BUS_DCM_REG0_OFF ((0x0 << 0) | \
            (0x0 << 1) | \
            (0x0 << 3) | \
            (0x1 << 4) | \
            (0x10 << 5) | \
            (0x0 << 20) | \
            (0x0 << 23) | \
            (0x0 << 30))

static void infracfg_ao_infra_dcm_rg_sfsel_set(unsigned int val)
{
    reg_write(INFRA_BUS_DCM_CTRL,
        (reg_read(INFRA_BUS_DCM_CTRL) &
        ~(0x1f << 10)) |
        (val & 0x1f) << 10);
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

#define INFRACFG_AO_INFRA_CONN_BUS_DCM_REG0_MASK ((0x1 << 8))
#define INFRACFG_AO_INFRA_CONN_BUS_DCM_REG1_MASK ((0x1 << 8))
#define INFRACFG_AO_INFRA_CONN_BUS_DCM_REG2_MASK ((0x1 << 8))
#define INFRACFG_AO_INFRA_CONN_BUS_DCM_REG0_ON ((0x1 << 8))
#define INFRACFG_AO_INFRA_CONN_BUS_DCM_REG1_ON ((0x0 << 8))
#define INFRACFG_AO_INFRA_CONN_BUS_DCM_REG2_ON ((0x1 << 8))
#define INFRACFG_AO_INFRA_CONN_BUS_DCM_REG0_OFF ((0x0 << 8))
#define INFRACFG_AO_INFRA_CONN_BUS_DCM_REG1_OFF ((0x1 << 8))
#define INFRACFG_AO_INFRA_CONN_BUS_DCM_REG2_OFF ((0x0 << 8))

void dcm_infracfg_ao_infra_conn_bus_dcm(int on)
{
    if (on) {
        /* TINFO = "Turn ON DCM 'infracfg_ao_infra_conn_bus_dcm'" */
        reg_write(MODULE_SW_CG_2_SET,
            (reg_read(MODULE_SW_CG_2_SET) &
            ~INFRACFG_AO_INFRA_CONN_BUS_DCM_REG0_MASK) |
            INFRACFG_AO_INFRA_CONN_BUS_DCM_REG0_ON);
    } else {
        /* TINFO = "Turn OFF DCM 'infracfg_ao_infra_conn_bus_dcm'" */
        reg_write(MODULE_SW_CG_2_CLR,
            (reg_read(MODULE_SW_CG_2_CLR) &
            ~INFRACFG_AO_INFRA_CONN_BUS_DCM_REG1_MASK) |
            INFRACFG_AO_INFRA_CONN_BUS_DCM_REG1_OFF);
    }
}

#define INFRACFG_AO_INFRA_RX_P2P_DCM_REG0_MASK ((0xf << 0))
#define INFRACFG_AO_INFRA_RX_P2P_DCM_REG0_ON ((0x0 << 0))
#define INFRACFG_AO_INFRA_RX_P2P_DCM_REG0_OFF ((0xf << 0))

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

#define INFRACFG_AO_PERI_BUS_DCM_REG0_MASK ((0x1 << 0) | \
            (0x1 << 1) | \
            (0x1 << 3) | \
            (0x1 << 4) | \
            (0x1f << 5) | \
            (0x1f << 15) | \
            (0x1 << 20) | \
            (0x1 << 21))
#define INFRACFG_AO_PERI_BUS_DCM_REG0_ON ((0x1 << 0) | \
            (0x1 << 1) | \
            (0x0 << 3) | \
            (0x0 << 4) | \
            (0x1f << 5) | \
            (0x1f << 15) | \
            (0x1 << 20) | \
            (0x1 << 21))
#define INFRACFG_AO_PERI_BUS_DCM_REG0_OFF ((0x0 << 0) | \
            (0x0 << 1) | \
            (0x0 << 3) | \
            (0x1 << 4) | \
            (0x1f << 5) | \
            (0x0 << 15) | \
            (0x0 << 20) | \
            (0x0 << 21))

static void infracfg_ao_peri_dcm_rg_sfsel_set(unsigned int val)
{
    reg_write(PERI_BUS_DCM_CTRL,
        (reg_read(PERI_BUS_DCM_CTRL) &
        ~(0x1f << 10)) |
        (val & 0x1f) << 10);
}

void dcm_infracfg_ao_peri_bus_dcm(int on)
{
    if (on) {
        /* TINFO = "Turn ON DCM 'infracfg_ao_peri_bus_dcm'" */
        infracfg_ao_peri_dcm_rg_sfsel_set(0x0);
        reg_write(PERI_BUS_DCM_CTRL,
            (reg_read(PERI_BUS_DCM_CTRL) &
            ~INFRACFG_AO_PERI_BUS_DCM_REG0_MASK) |
            INFRACFG_AO_PERI_BUS_DCM_REG0_ON);
    } else {
        /* TINFO = "Turn OFF DCM 'infracfg_ao_peri_bus_dcm'" */
        infracfg_ao_peri_dcm_rg_sfsel_set(0x1f);
        reg_write(PERI_BUS_DCM_CTRL,
            (reg_read(PERI_BUS_DCM_CTRL) &
            ~INFRACFG_AO_PERI_BUS_DCM_REG0_MASK) |
            INFRACFG_AO_PERI_BUS_DCM_REG0_OFF);
    }
}

#define INFRACFG_AO_PERI_MODULE_DCM_REG0_MASK ((0x1 << 29) | \
            (0x1 << 31))
#define INFRACFG_AO_PERI_MODULE_DCM_REG0_ON ((0x1 << 29) | \
            (0x1 << 31))
#define INFRACFG_AO_PERI_MODULE_DCM_REG0_OFF ((0x0 << 29) | \
            (0x0 << 31))

void dcm_infracfg_ao_peri_module_dcm(int on)
{
    if (on) {
        /* TINFO = "Turn ON DCM 'infracfg_ao_peri_module_dcm'" */
        reg_write(PERI_BUS_DCM_CTRL,
            (reg_read(PERI_BUS_DCM_CTRL) &
            ~INFRACFG_AO_PERI_MODULE_DCM_REG0_MASK) |
            INFRACFG_AO_PERI_MODULE_DCM_REG0_ON);
    } else {
        /* TINFO = "Turn OFF DCM 'infracfg_ao_peri_module_dcm'" */
        reg_write(PERI_BUS_DCM_CTRL,
            (reg_read(PERI_BUS_DCM_CTRL) &
            ~INFRACFG_AO_PERI_MODULE_DCM_REG0_MASK) |
            INFRACFG_AO_PERI_MODULE_DCM_REG0_OFF);
    }
}

#define INFRA_AO_BCRM_INFRA_BUS_DCM_REG0_MASK ((0x1 << 0) | \
            (0x1 << 1) | \
            (0x1 << 28))
#define INFRA_AO_BCRM_INFRA_BUS_DCM_REG1_MASK ((0x1f << 5))
#define INFRA_AO_BCRM_INFRA_BUS_DCM_REG2_MASK ((0x1 << 22))
#define INFRA_AO_BCRM_INFRA_BUS_DCM_REG0_ON ((0x1 << 0) | \
            (0x1 << 1) | \
            (0x1 << 28))
#define INFRA_AO_BCRM_INFRA_BUS_DCM_REG1_ON ((0x0 << 5))
#define INFRA_AO_BCRM_INFRA_BUS_DCM_REG2_ON ((0x1 << 22))
#define INFRA_AO_BCRM_INFRA_BUS_DCM_REG0_OFF ((0x1 << 0) | \
            (0x1 << 1) | \
            (0x0 << 28))
#define INFRA_AO_BCRM_INFRA_BUS_DCM_REG1_OFF ((0x0 << 5))
#define INFRA_AO_BCRM_INFRA_BUS_DCM_REG2_OFF ((0x0 << 22))

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
        reg_write(VDNR_DCM_TOP_INFRA_PAR_BUS_u_INFRA_PAR_BUS_CTRL_9,
            (reg_read(
            VDNR_DCM_TOP_INFRA_PAR_BUS_u_INFRA_PAR_BUS_CTRL_9) &
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
        reg_write(VDNR_DCM_TOP_INFRA_PAR_BUS_u_INFRA_PAR_BUS_CTRL_9,
            (reg_read(
            VDNR_DCM_TOP_INFRA_PAR_BUS_u_INFRA_PAR_BUS_CTRL_9) &
            ~INFRA_AO_BCRM_INFRA_BUS_DCM_REG2_MASK) |
            INFRA_AO_BCRM_INFRA_BUS_DCM_REG2_OFF);
    }
}

#define INFRA_AO_BCRM_PERI_BUS_DCM_REG0_MASK ((0x1 << 26))
#define INFRA_AO_BCRM_PERI_BUS_DCM_REG1_MASK ((0x1f << 5))
#define INFRA_AO_BCRM_PERI_BUS_DCM_REG2_MASK ((0x1 << 12))
#define INFRA_AO_BCRM_PERI_BUS_DCM_REG0_ON ((0x1 << 26))
#define INFRA_AO_BCRM_PERI_BUS_DCM_REG1_ON ((0x0 << 5))
#define INFRA_AO_BCRM_PERI_BUS_DCM_REG2_ON ((0x1 << 12))
#define INFRA_AO_BCRM_PERI_BUS_DCM_REG0_OFF ((0x0 << 26))
#define INFRA_AO_BCRM_PERI_BUS_DCM_REG1_OFF ((0x0 << 5))
#define INFRA_AO_BCRM_PERI_BUS_DCM_REG2_OFF ((0x0 << 12))

void dcm_infra_ao_bcrm_peri_bus_dcm(int on)
{
    if (on) {
        /* TINFO = "Turn ON DCM 'infra_ao_bcrm_peri_bus_dcm'" */
        reg_write(VDNR_DCM_TOP_INFRA_PAR_BUS_u_INFRA_PAR_BUS_CTRL_1,
            (reg_read(
            VDNR_DCM_TOP_INFRA_PAR_BUS_u_INFRA_PAR_BUS_CTRL_1) &
            ~INFRA_AO_BCRM_PERI_BUS_DCM_REG0_MASK) |
            INFRA_AO_BCRM_PERI_BUS_DCM_REG0_ON);
        reg_write(VDNR_DCM_TOP_INFRA_PAR_BUS_u_INFRA_PAR_BUS_CTRL_2,
            (reg_read(
            VDNR_DCM_TOP_INFRA_PAR_BUS_u_INFRA_PAR_BUS_CTRL_2) &
            ~INFRA_AO_BCRM_PERI_BUS_DCM_REG1_MASK) |
            INFRA_AO_BCRM_PERI_BUS_DCM_REG1_ON);
        reg_write(VDNR_DCM_TOP_INFRA_PAR_BUS_u_INFRA_PAR_BUS_CTRL_10,
            (reg_read(
            VDNR_DCM_TOP_INFRA_PAR_BUS_u_INFRA_PAR_BUS_CTRL_10) &
            ~INFRA_AO_BCRM_PERI_BUS_DCM_REG2_MASK) |
            INFRA_AO_BCRM_PERI_BUS_DCM_REG2_ON);
    } else {
        /* TINFO = "Turn OFF DCM 'infra_ao_bcrm_peri_bus_dcm'" */
        reg_write(VDNR_DCM_TOP_INFRA_PAR_BUS_u_INFRA_PAR_BUS_CTRL_1,
            (reg_read(
            VDNR_DCM_TOP_INFRA_PAR_BUS_u_INFRA_PAR_BUS_CTRL_1) &
            ~INFRA_AO_BCRM_PERI_BUS_DCM_REG0_MASK) |
            INFRA_AO_BCRM_PERI_BUS_DCM_REG0_OFF);
        reg_write(VDNR_DCM_TOP_INFRA_PAR_BUS_u_INFRA_PAR_BUS_CTRL_2,
            (reg_read(
            VDNR_DCM_TOP_INFRA_PAR_BUS_u_INFRA_PAR_BUS_CTRL_2) &
            ~INFRA_AO_BCRM_PERI_BUS_DCM_REG1_MASK) |
            INFRA_AO_BCRM_PERI_BUS_DCM_REG1_OFF);
        reg_write(VDNR_DCM_TOP_INFRA_PAR_BUS_u_INFRA_PAR_BUS_CTRL_10,
            (reg_read(
            VDNR_DCM_TOP_INFRA_PAR_BUS_u_INFRA_PAR_BUS_CTRL_10) &
            ~INFRA_AO_BCRM_PERI_BUS_DCM_REG2_MASK) |
            INFRA_AO_BCRM_PERI_BUS_DCM_REG2_OFF);
    }
}

#define MCUSYS_PAR_WRAP_BIG_DCM_REG0_MASK ((0x1 << 0) | \
            (0x1 << 0))
#define MCUSYS_PAR_WRAP_BIG_DCM_REG0_ON ((0x1 << 0) | \
            (0x1 << 0))
#define MCUSYS_PAR_WRAP_BIG_DCM_REG0_OFF ((0x0 << 0) | \
            (0x0 << 0))

void dcm_mcusys_par_wrap_big_dcm(int on)
{
    if (on) {
        /* TINFO = "Turn ON DCM 'mcusys_par_wrap_big_dcm'" */
        reg_write(MCUSYS_PAR_WRAP_STALL_DCM_CONF,
            (reg_read(MCUSYS_PAR_WRAP_STALL_DCM_CONF) &
            ~MCUSYS_PAR_WRAP_BIG_DCM_REG0_MASK) |
            MCUSYS_PAR_WRAP_BIG_DCM_REG0_ON);
    } else {
        /* TINFO = "Turn OFF DCM 'mcusys_par_wrap_big_dcm'" */
        reg_write(MCUSYS_PAR_WRAP_STALL_DCM_CONF,
            (reg_read(MCUSYS_PAR_WRAP_STALL_DCM_CONF) &
            ~MCUSYS_PAR_WRAP_BIG_DCM_REG0_MASK) |
            MCUSYS_PAR_WRAP_BIG_DCM_REG0_OFF);
    }
}

#define MCUSYS_PAR_WRAP_LITTLE_DCM_REG0_MASK ((0x1 << 0) | \
            (0x1 << 0) | \
            (0x1 << 0) | \
            (0x1 << 0) | \
            (0x1 << 0) | \
            (0x1 << 0))
#define MCUSYS_PAR_WRAP_LITTLE_DCM_REG0_ON ((0x1 << 0) | \
            (0x1 << 0) | \
            (0x1 << 0) | \
            (0x1 << 0) | \
            (0x1 << 0) | \
            (0x1 << 0))
#define MCUSYS_PAR_WRAP_LITTLE_DCM_REG0_OFF ((0x0 << 0) | \
            (0x0 << 0) | \
            (0x0 << 0) | \
            (0x0 << 0) | \
            (0x0 << 0) | \
            (0x0 << 0))

void dcm_mcusys_par_wrap_little_dcm(int on)
{
    if (on) {
        /* TINFO = "Turn ON DCM 'mcusys_par_wrap_little_dcm'" */
        reg_write(MCUSYS_PAR_WRAP_CPU_STALL_DCM_CTRL,
            (reg_read(MCUSYS_PAR_WRAP_CPU_STALL_DCM_CTRL) &
            ~MCUSYS_PAR_WRAP_LITTLE_DCM_REG0_MASK) |
            MCUSYS_PAR_WRAP_LITTLE_DCM_REG0_ON);
    } else {
        /* TINFO = "Turn OFF DCM 'mcusys_par_wrap_little_dcm'" */
        reg_write(MCUSYS_PAR_WRAP_CPU_STALL_DCM_CTRL,
            (reg_read(MCUSYS_PAR_WRAP_CPU_STALL_DCM_CTRL) &
            ~MCUSYS_PAR_WRAP_LITTLE_DCM_REG0_MASK) |
            MCUSYS_PAR_WRAP_LITTLE_DCM_REG0_OFF);
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
#define MP_CPUSYS_TOP_APB_DCM_REG0_ON ((0x1 << 5))
#define MP_CPUSYS_TOP_APB_DCM_REG1_ON ((0x1 << 8))
#define MP_CPUSYS_TOP_APB_DCM_REG0_OFF ((0x0 << 5))
#define MP_CPUSYS_TOP_APB_DCM_REG1_OFF ((0x0 << 8))

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
    }
}

#define MP_CPUSYS_TOP_BUS_PLL_DIV_DCM_REG0_MASK ((0x1 << 11))
#define MP_CPUSYS_TOP_BUS_PLL_DIV_DCM_REG0_ON ((0x1 << 11))
#define MP_CPUSYS_TOP_BUS_PLL_DIV_DCM_REG0_OFF ((0x0 << 11))

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

#define MP_CPUSYS_TOP_CPUBIU_DCM_REG0_MASK ((0xffff << 0))
#define MP_CPUSYS_TOP_CPUBIU_DCM_REG0_ON ((0xffff << 0))
#define MP_CPUSYS_TOP_CPUBIU_DCM_REG0_OFF ((0x0 << 0))

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

#define MP_CPUSYS_TOP_LAST_COR_IDLE_DCM_REG0_MASK ((0x1 << 31))
#define MP_CPUSYS_TOP_LAST_COR_IDLE_DCM_REG0_ON ((0x1 << 31))
#define MP_CPUSYS_TOP_LAST_COR_IDLE_DCM_REG0_OFF ((0x0 << 31))

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
#define MP_CPUSYS_TOP_MP0_QDCM_REG0_ON ((0x1 << 3))
#define MP_CPUSYS_TOP_MP0_QDCM_REG0_OFF ((0x0 << 3))

void dcm_mp_cpusys_top_mp0_qdcm(int on)
{
    if (on) {
        /* TINFO = "Turn ON DCM 'mp_cpusys_top_mp0_qdcm'" */
        reg_write(MP_CPUSYS_TOP_MP_MISC_DCM_CFG0,
            (reg_read(MP_CPUSYS_TOP_MP_MISC_DCM_CFG0) &
            ~MP_CPUSYS_TOP_MP0_QDCM_REG0_MASK) |
            MP_CPUSYS_TOP_MP0_QDCM_REG0_ON);
    } else {
        /* TINFO = "Turn OFF DCM 'mp_cpusys_top_mp0_qdcm'" */
        reg_write(MP_CPUSYS_TOP_MP_MISC_DCM_CFG0,
            (reg_read(MP_CPUSYS_TOP_MP_MISC_DCM_CFG0) &
            ~MP_CPUSYS_TOP_MP0_QDCM_REG0_MASK) |
            MP_CPUSYS_TOP_MP0_QDCM_REG0_OFF);
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

#define MCUSYS_CFG_REG_APB_DCM_REG0_MASK ((0x1 << 16))
#define MCUSYS_CFG_REG_APB_DCM_REG0_ON ((0x1 << 16))
#define MCUSYS_CFG_REG_APB_DCM_REG0_OFF ((0x0 << 16))

void dcm_mcusys_cfg_reg_apb_dcm(int on)
{
    if (on) {
        /* TINFO = "Turn ON DCM 'mcusys_cfg_reg_apb_dcm'" */
        reg_write(MCUSYS_CFG_REG_MP0_DCM_CFG0,
            (reg_read(MCUSYS_CFG_REG_MP0_DCM_CFG0) &
            ~MCUSYS_CFG_REG_APB_DCM_REG0_MASK) |
            MCUSYS_CFG_REG_APB_DCM_REG0_ON);
    } else {
        /* TINFO = "Turn OFF DCM 'mcusys_cfg_reg_apb_dcm'" */
        reg_write(MCUSYS_CFG_REG_MP0_DCM_CFG0,
            (reg_read(MCUSYS_CFG_REG_MP0_DCM_CFG0) &
            ~MCUSYS_CFG_REG_APB_DCM_REG0_MASK) |
            MCUSYS_CFG_REG_APB_DCM_REG0_OFF);
    }
}

#define MCUSYS_CFG_REG_CORE_STALL_DCM_REG0_MASK ((0x1 << 0))
#define MCUSYS_CFG_REG_CORE_STALL_DCM_REG0_ON ((0x1 << 0))
#define MCUSYS_CFG_REG_CORE_STALL_DCM_REG0_OFF ((0x0 << 0))

void dcm_mcusys_cfg_reg_core_stall_dcm(int on)
{
    if (on) {
        /* TINFO = "Turn ON DCM 'mcusys_cfg_reg_core_stall_dcm'" */
        reg_write(MCUSYS_CFG_REG_MP0_DCM_CFG7,
            (reg_read(MCUSYS_CFG_REG_MP0_DCM_CFG7) &
            ~MCUSYS_CFG_REG_CORE_STALL_DCM_REG0_MASK) |
            MCUSYS_CFG_REG_CORE_STALL_DCM_REG0_ON);
    } else {
        /* TINFO = "Turn OFF DCM 'mcusys_cfg_reg_core_stall_dcm'" */
        reg_write(MCUSYS_CFG_REG_MP0_DCM_CFG7,
            (reg_read(MCUSYS_CFG_REG_MP0_DCM_CFG7) &
            ~MCUSYS_CFG_REG_CORE_STALL_DCM_REG0_MASK) |
            MCUSYS_CFG_REG_CORE_STALL_DCM_REG0_OFF);
    }
}

#define MCUSYS_CFG_REG_FCM_STALL_DCM_REG0_MASK ((0x1 << 4))
#define MCUSYS_CFG_REG_FCM_STALL_DCM_REG0_ON ((0x1 << 4))
#define MCUSYS_CFG_REG_FCM_STALL_DCM_REG0_OFF ((0x0 << 4))

void dcm_mcusys_cfg_reg_fcm_stall_dcm(int on)
{
    if (on) {
        /* TINFO = "Turn ON DCM 'mcusys_cfg_reg_fcm_stall_dcm'" */
        reg_write(MCUSYS_CFG_REG_MP0_DCM_CFG7,
            (reg_read(MCUSYS_CFG_REG_MP0_DCM_CFG7) &
            ~MCUSYS_CFG_REG_FCM_STALL_DCM_REG0_MASK) |
            MCUSYS_CFG_REG_FCM_STALL_DCM_REG0_ON);
    } else {
        /* TINFO = "Turn OFF DCM 'mcusys_cfg_reg_fcm_stall_dcm'" */
        reg_write(MCUSYS_CFG_REG_MP0_DCM_CFG7,
            (reg_read(MCUSYS_CFG_REG_MP0_DCM_CFG7) &
            ~MCUSYS_CFG_REG_FCM_STALL_DCM_REG0_MASK) |
            MCUSYS_CFG_REG_FCM_STALL_DCM_REG0_OFF);
    }
}

#define MCUSYS_CFG_REG_MP0_QDCM_REG0_MASK ((0x1 << 0) | \
            (0x1 << 1) | \
            (0x1 << 2) | \
            (0x1 << 3))
#define MCUSYS_CFG_REG_MP0_QDCM_REG0_ON ((0x1 << 0) | \
            (0x1 << 1) | \
            (0x1 << 2) | \
            (0x1 << 3))
#define MCUSYS_CFG_REG_MP0_QDCM_REG0_OFF ((0x0 << 0) | \
            (0x0 << 1) | \
            (0x0 << 2) | \
            (0x0 << 3))

void dcm_mcusys_cfg_reg_mp0_qdcm(int on)
{
    if (on) {
        /* TINFO = "Turn ON DCM 'mcusys_cfg_reg_mp0_qdcm'" */
        reg_write(MCUSYS_CFG_REG_MP0_DCM_CFG0,
            (reg_read(MCUSYS_CFG_REG_MP0_DCM_CFG0) &
            ~MCUSYS_CFG_REG_MP0_QDCM_REG0_MASK) |
            MCUSYS_CFG_REG_MP0_QDCM_REG0_ON);
    } else {
        /* TINFO = "Turn OFF DCM 'mcusys_cfg_reg_mp0_qdcm'" */
        reg_write(MCUSYS_CFG_REG_MP0_DCM_CFG0,
            (reg_read(MCUSYS_CFG_REG_MP0_DCM_CFG0) &
            ~MCUSYS_CFG_REG_MP0_QDCM_REG0_MASK) |
            MCUSYS_CFG_REG_MP0_QDCM_REG0_OFF);
    }
}


