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
#define __raw_readl(addr)   read32(addr)
#define reg_read(addr)      __raw_readl(addr)
#define reg_write(addr, val)   writel((val), ((void *)addr))

/*====================auto gen code 20211025_122760=====================*/
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
