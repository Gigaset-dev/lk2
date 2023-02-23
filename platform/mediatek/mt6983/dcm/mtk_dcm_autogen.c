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

/*====================auto gen code 20210629_093447=====================*/
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

#define MCUSYS_TOP_MCU_ACP_DCM_REG0_MASK ((0x1 << 0) | \
            (0x1 << 16))
#define MCUSYS_TOP_MCU_ACP_DCM_REG0_ON ((0x1 << 0) | \
            (0x1 << 16))
#define MCUSYS_TOP_MCU_ACP_DCM_REG0_OFF ((0x0 << 0) | \
            (0x0 << 16))

void dcm_mcusys_top_mcu_acp_dcm(int on)
{
    if (on) {
        /* TINFO = "Turn ON DCM 'mcusys_top_mcu_acp_dcm'" */
        reg_write(MCUSYS_TOP_MP_ADB_DCM_CFG0,
            (reg_read(MCUSYS_TOP_MP_ADB_DCM_CFG0) &
            ~MCUSYS_TOP_MCU_ACP_DCM_REG0_MASK) |
            MCUSYS_TOP_MCU_ACP_DCM_REG0_ON);
    } else {
        /* TINFO = "Turn OFF DCM 'mcusys_top_mcu_acp_dcm'" */
        reg_write(MCUSYS_TOP_MP_ADB_DCM_CFG0,
            (reg_read(MCUSYS_TOP_MP_ADB_DCM_CFG0) &
            ~MCUSYS_TOP_MCU_ACP_DCM_REG0_MASK) |
            MCUSYS_TOP_MCU_ACP_DCM_REG0_OFF);
    }
}

#define MCUSYS_TOP_MCU_ADB_DCM_REG0_MASK ((0x1 << 0) | \
            (0x1 << 1) | \
            (0x1 << 2) | \
            (0x1 << 3) | \
            (0x1 << 4) | \
            (0x1 << 5) | \
            (0x1 << 6) | \
            (0x1 << 7) | \
            (0x1 << 8) | \
            (0x1 << 9) | \
            (0x1 << 10))
#define MCUSYS_TOP_MCU_ADB_DCM_REG0_ON ((0x1 << 0) | \
            (0x1 << 1) | \
            (0x1 << 2) | \
            (0x1 << 3) | \
            (0x1 << 4) | \
            (0x1 << 5) | \
            (0x1 << 6) | \
            (0x1 << 7) | \
            (0x1 << 8) | \
            (0x1 << 9) | \
            (0x1 << 10))
#define MCUSYS_TOP_MCU_ADB_DCM_REG0_OFF ((0x0 << 0) | \
            (0x0 << 1) | \
            (0x0 << 2) | \
            (0x0 << 3) | \
            (0x0 << 4) | \
            (0x0 << 5) | \
            (0x0 << 6) | \
            (0x0 << 7) | \
            (0x0 << 8) | \
            (0x0 << 9) | \
            (0x0 << 10))

void dcm_mcusys_top_mcu_adb_dcm(int on)
{
    if (on) {
        /* TINFO = "Turn ON DCM 'mcusys_top_mcu_adb_dcm'" */
        reg_write(MCUSYS_TOP_ADB_FIFO_DCM_EN,
            (reg_read(MCUSYS_TOP_ADB_FIFO_DCM_EN) &
            ~MCUSYS_TOP_MCU_ADB_DCM_REG0_MASK) |
            MCUSYS_TOP_MCU_ADB_DCM_REG0_ON);
    } else {
        /* TINFO = "Turn OFF DCM 'mcusys_top_mcu_adb_dcm'" */
        reg_write(MCUSYS_TOP_ADB_FIFO_DCM_EN,
            (reg_read(MCUSYS_TOP_ADB_FIFO_DCM_EN) &
            ~MCUSYS_TOP_MCU_ADB_DCM_REG0_MASK) |
            MCUSYS_TOP_MCU_ADB_DCM_REG0_OFF);
    }
}

#define MCUSYS_TOP_MCU_BUS_QDCM_REG0_MASK ((0x1 << 16) | \
            (0x1 << 20) | \
            (0x1 << 24))
#define MCUSYS_TOP_MCU_BUS_QDCM_REG1_MASK ((0x1 << 0) | \
            (0x1 << 4) | \
            (0x1 << 8) | \
            (0x1 << 12))
#define MCUSYS_TOP_MCU_BUS_QDCM_REG0_ON ((0x1 << 16) | \
            (0x1 << 20) | \
            (0x1 << 24))
#define MCUSYS_TOP_MCU_BUS_QDCM_REG1_ON ((0x1 << 0) | \
            (0x1 << 4) | \
            (0x1 << 8) | \
            (0x1 << 12))
#define MCUSYS_TOP_MCU_BUS_QDCM_REG0_OFF ((0x0 << 16) | \
            (0x0 << 20) | \
            (0x0 << 24))
#define MCUSYS_TOP_MCU_BUS_QDCM_REG1_OFF ((0x0 << 0) | \
            (0x0 << 4) | \
            (0x0 << 8) | \
            (0x0 << 12))

void dcm_mcusys_top_mcu_bus_qdcm(int on)
{
    if (on) {
        /* TINFO = "Turn ON DCM 'mcusys_top_mcu_bus_qdcm'" */
        reg_write(MCUSYS_TOP_QDCM_CONFIG0,
            (reg_read(MCUSYS_TOP_QDCM_CONFIG0) &
            ~MCUSYS_TOP_MCU_BUS_QDCM_REG0_MASK) |
            MCUSYS_TOP_MCU_BUS_QDCM_REG0_ON);
        reg_write(MCUSYS_TOP_QDCM_CONFIG1,
            (reg_read(MCUSYS_TOP_QDCM_CONFIG1) &
            ~MCUSYS_TOP_MCU_BUS_QDCM_REG1_MASK) |
            MCUSYS_TOP_MCU_BUS_QDCM_REG1_ON);
    } else {
        /* TINFO = "Turn OFF DCM 'mcusys_top_mcu_bus_qdcm'" */
        reg_write(MCUSYS_TOP_QDCM_CONFIG0,
            (reg_read(MCUSYS_TOP_QDCM_CONFIG0) &
            ~MCUSYS_TOP_MCU_BUS_QDCM_REG0_MASK) |
            MCUSYS_TOP_MCU_BUS_QDCM_REG0_OFF);
        reg_write(MCUSYS_TOP_QDCM_CONFIG1,
            (reg_read(MCUSYS_TOP_QDCM_CONFIG1) &
            ~MCUSYS_TOP_MCU_BUS_QDCM_REG1_MASK) |
            MCUSYS_TOP_MCU_BUS_QDCM_REG1_OFF);
    }
}

#define MCUSYS_TOP_MCU_CBIP_DCM_REG0_MASK ((0x1 << 0))
#define MCUSYS_TOP_MCU_CBIP_DCM_REG1_MASK ((0x1 << 0))
#define MCUSYS_TOP_MCU_CBIP_DCM_REG2_MASK ((0x1 << 0))
#define MCUSYS_TOP_MCU_CBIP_DCM_REG3_MASK ((0x1 << 0))
#define MCUSYS_TOP_MCU_CBIP_DCM_REG4_MASK ((0x1 << 0))
#define MCUSYS_TOP_MCU_CBIP_DCM_REG5_MASK ((0x1 << 0) | \
            (0x1 << 1) | \
            (0x1 << 2))
#define MCUSYS_TOP_MCU_CBIP_DCM_REG0_ON ((0x0 << 0))
#define MCUSYS_TOP_MCU_CBIP_DCM_REG1_ON ((0x0 << 0))
#define MCUSYS_TOP_MCU_CBIP_DCM_REG2_ON ((0x0 << 0))
#define MCUSYS_TOP_MCU_CBIP_DCM_REG3_ON ((0x0 << 0))
#define MCUSYS_TOP_MCU_CBIP_DCM_REG4_ON ((0x0 << 0))
#define MCUSYS_TOP_MCU_CBIP_DCM_REG5_ON ((0x1 << 0) | \
            (0x1 << 1) | \
            (0x1 << 2))
#define MCUSYS_TOP_MCU_CBIP_DCM_REG0_OFF ((0x1 << 0))
#define MCUSYS_TOP_MCU_CBIP_DCM_REG1_OFF ((0x1 << 0))
#define MCUSYS_TOP_MCU_CBIP_DCM_REG2_OFF ((0x1 << 0))
#define MCUSYS_TOP_MCU_CBIP_DCM_REG3_OFF ((0x1 << 0))
#define MCUSYS_TOP_MCU_CBIP_DCM_REG4_OFF ((0x1 << 0))
#define MCUSYS_TOP_MCU_CBIP_DCM_REG5_OFF ((0x0 << 0) | \
            (0x0 << 1) | \
            (0x0 << 2))

void dcm_mcusys_top_mcu_cbip_dcm(int on)
{
    if (on) {
        /* TINFO = "Turn ON DCM 'mcusys_top_mcu_cbip_dcm'" */
        reg_write(MCUSYS_TOP_CBIP_CABGEN_3TO1_CONFIG,
            (reg_read(MCUSYS_TOP_CBIP_CABGEN_3TO1_CONFIG) &
            ~MCUSYS_TOP_MCU_CBIP_DCM_REG0_MASK) |
            MCUSYS_TOP_MCU_CBIP_DCM_REG0_ON);
        reg_write(MCUSYS_TOP_CBIP_CABGEN_2TO1_CONFIG,
            (reg_read(MCUSYS_TOP_CBIP_CABGEN_2TO1_CONFIG) &
            ~MCUSYS_TOP_MCU_CBIP_DCM_REG1_MASK) |
            MCUSYS_TOP_MCU_CBIP_DCM_REG1_ON);
        reg_write(MCUSYS_TOP_CBIP_CABGEN_4TO2_CONFIG,
            (reg_read(MCUSYS_TOP_CBIP_CABGEN_4TO2_CONFIG) &
            ~MCUSYS_TOP_MCU_CBIP_DCM_REG2_MASK) |
            MCUSYS_TOP_MCU_CBIP_DCM_REG2_ON);
        reg_write(MCUSYS_TOP_CBIP_CABGEN_1TO2_CONFIG,
            (reg_read(MCUSYS_TOP_CBIP_CABGEN_1TO2_CONFIG) &
            ~MCUSYS_TOP_MCU_CBIP_DCM_REG3_MASK) |
            MCUSYS_TOP_MCU_CBIP_DCM_REG3_ON);
        reg_write(MCUSYS_TOP_CBIP_CABGEN_2TO5_CONFIG,
            (reg_read(MCUSYS_TOP_CBIP_CABGEN_2TO5_CONFIG) &
            ~MCUSYS_TOP_MCU_CBIP_DCM_REG4_MASK) |
            MCUSYS_TOP_MCU_CBIP_DCM_REG4_ON);
        reg_write(MCUSYS_TOP_CBIP_P2P_CONFIG0,
            (reg_read(MCUSYS_TOP_CBIP_P2P_CONFIG0) &
            ~MCUSYS_TOP_MCU_CBIP_DCM_REG5_MASK) |
            MCUSYS_TOP_MCU_CBIP_DCM_REG5_ON);
    } else {
        /* TINFO = "Turn OFF DCM 'mcusys_top_mcu_cbip_dcm'" */
        reg_write(MCUSYS_TOP_CBIP_CABGEN_3TO1_CONFIG,
            (reg_read(MCUSYS_TOP_CBIP_CABGEN_3TO1_CONFIG) &
            ~MCUSYS_TOP_MCU_CBIP_DCM_REG0_MASK) |
            MCUSYS_TOP_MCU_CBIP_DCM_REG0_OFF);
        reg_write(MCUSYS_TOP_CBIP_CABGEN_2TO1_CONFIG,
            (reg_read(MCUSYS_TOP_CBIP_CABGEN_2TO1_CONFIG) &
            ~MCUSYS_TOP_MCU_CBIP_DCM_REG1_MASK) |
            MCUSYS_TOP_MCU_CBIP_DCM_REG1_OFF);
        reg_write(MCUSYS_TOP_CBIP_CABGEN_4TO2_CONFIG,
            (reg_read(MCUSYS_TOP_CBIP_CABGEN_4TO2_CONFIG) &
            ~MCUSYS_TOP_MCU_CBIP_DCM_REG2_MASK) |
            MCUSYS_TOP_MCU_CBIP_DCM_REG2_OFF);
        reg_write(MCUSYS_TOP_CBIP_CABGEN_1TO2_CONFIG,
            (reg_read(MCUSYS_TOP_CBIP_CABGEN_1TO2_CONFIG) &
            ~MCUSYS_TOP_MCU_CBIP_DCM_REG3_MASK) |
            MCUSYS_TOP_MCU_CBIP_DCM_REG3_OFF);
        reg_write(MCUSYS_TOP_CBIP_CABGEN_2TO5_CONFIG,
            (reg_read(MCUSYS_TOP_CBIP_CABGEN_2TO5_CONFIG) &
            ~MCUSYS_TOP_MCU_CBIP_DCM_REG4_MASK) |
            MCUSYS_TOP_MCU_CBIP_DCM_REG4_OFF);
        reg_write(MCUSYS_TOP_CBIP_P2P_CONFIG0,
            (reg_read(MCUSYS_TOP_CBIP_P2P_CONFIG0) &
            ~MCUSYS_TOP_MCU_CBIP_DCM_REG5_MASK) |
            MCUSYS_TOP_MCU_CBIP_DCM_REG5_OFF);
    }
}

#define MCUSYS_TOP_MCU_CORE_QDCM_REG0_MASK ((0x1 << 0) | \
            (0x1 << 4) | \
            (0x1 << 8) | \
            (0x1 << 12))
#define MCUSYS_TOP_MCU_CORE_QDCM_REG1_MASK ((0x1 << 0) | \
            (0x1 << 4))
#define MCUSYS_TOP_MCU_CORE_QDCM_REG0_ON ((0x1 << 0) | \
            (0x1 << 4) | \
            (0x1 << 8) | \
            (0x1 << 12))
#define MCUSYS_TOP_MCU_CORE_QDCM_REG1_ON ((0x1 << 0) | \
            (0x1 << 4))
#define MCUSYS_TOP_MCU_CORE_QDCM_REG0_OFF ((0x0 << 0) | \
            (0x0 << 4) | \
            (0x0 << 8) | \
            (0x0 << 12))
#define MCUSYS_TOP_MCU_CORE_QDCM_REG1_OFF ((0x0 << 0) | \
            (0x0 << 4))

void dcm_mcusys_top_mcu_core_qdcm(int on)
{
    if (on) {
        /* TINFO = "Turn ON DCM 'mcusys_top_mcu_core_qdcm'" */
        reg_write(MCUSYS_TOP_QDCM_CONFIG2,
            (reg_read(MCUSYS_TOP_QDCM_CONFIG2) &
            ~MCUSYS_TOP_MCU_CORE_QDCM_REG0_MASK) |
            MCUSYS_TOP_MCU_CORE_QDCM_REG0_ON);
        reg_write(MCUSYS_TOP_QDCM_CONFIG3,
            (reg_read(MCUSYS_TOP_QDCM_CONFIG3) &
            ~MCUSYS_TOP_MCU_CORE_QDCM_REG1_MASK) |
            MCUSYS_TOP_MCU_CORE_QDCM_REG1_ON);
    } else {
        /* TINFO = "Turn OFF DCM 'mcusys_top_mcu_core_qdcm'" */
        reg_write(MCUSYS_TOP_QDCM_CONFIG2,
            (reg_read(MCUSYS_TOP_QDCM_CONFIG2) &
            ~MCUSYS_TOP_MCU_CORE_QDCM_REG0_MASK) |
            MCUSYS_TOP_MCU_CORE_QDCM_REG0_OFF);
        reg_write(MCUSYS_TOP_QDCM_CONFIG3,
            (reg_read(MCUSYS_TOP_QDCM_CONFIG3) &
            ~MCUSYS_TOP_MCU_CORE_QDCM_REG1_MASK) |
            MCUSYS_TOP_MCU_CORE_QDCM_REG1_OFF);
    }
}

#define MCUSYS_TOP_MCU_IO_DCM_REG0_MASK ((0x1 << 0) | \
            (0x1 << 12))
#define MCUSYS_TOP_MCU_IO_DCM_REG1_MASK ((0x1 << 0))
#define MCUSYS_TOP_MCU_IO_DCM_REG0_ON ((0x1 << 0) | \
            (0x1 << 12))
#define MCUSYS_TOP_MCU_IO_DCM_REG1_ON ((0x1 << 0))
#define MCUSYS_TOP_MCU_IO_DCM_REG0_OFF ((0x0 << 0) | \
            (0x0 << 12))
#define MCUSYS_TOP_MCU_IO_DCM_REG1_OFF ((0x0 << 0))

void dcm_mcusys_top_mcu_io_dcm(int on)
{
    if (on) {
        /* TINFO = "Turn ON DCM 'mcusys_top_mcu_io_dcm'" */
        reg_write(MCUSYS_TOP_QDCM_CONFIG0,
            (reg_read(MCUSYS_TOP_QDCM_CONFIG0) &
            ~MCUSYS_TOP_MCU_IO_DCM_REG0_MASK) |
            MCUSYS_TOP_MCU_IO_DCM_REG0_ON);
        reg_write(MCUSYS_TOP_L3GIC_ARCH_CG_CONFIG,
            (reg_read(MCUSYS_TOP_L3GIC_ARCH_CG_CONFIG) &
            ~MCUSYS_TOP_MCU_IO_DCM_REG1_MASK) |
            MCUSYS_TOP_MCU_IO_DCM_REG1_ON);
    } else {
        /* TINFO = "Turn OFF DCM 'mcusys_top_mcu_io_dcm'" */
        reg_write(MCUSYS_TOP_QDCM_CONFIG0,
            (reg_read(MCUSYS_TOP_QDCM_CONFIG0) &
            ~MCUSYS_TOP_MCU_IO_DCM_REG0_MASK) |
            MCUSYS_TOP_MCU_IO_DCM_REG0_OFF);
        reg_write(MCUSYS_TOP_L3GIC_ARCH_CG_CONFIG,
            (reg_read(MCUSYS_TOP_L3GIC_ARCH_CG_CONFIG) &
            ~MCUSYS_TOP_MCU_IO_DCM_REG1_MASK) |
            MCUSYS_TOP_MCU_IO_DCM_REG1_OFF);
    }
}

