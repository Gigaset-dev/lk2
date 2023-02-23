/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <mtk_dcm.h>
#include <platform/addressmap.h>

/* Base Register Definition */
#define MCUSYS_TOP_BASE    (MCUCFG_BASE + 0x00000000)

/* Register Definition */
#define MCUSYS_TOP_ADB_FIFO_DCM_EN              (MCUSYS_TOP_BASE + 0x278)
#define MCUSYS_TOP_CBIP_CABGEN_1TO2_CONFIG      (MCUSYS_TOP_BASE + 0x2ac)
#define MCUSYS_TOP_CBIP_CABGEN_2TO1_CONFIG      (MCUSYS_TOP_BASE + 0x2a4)
#define MCUSYS_TOP_CBIP_CABGEN_2TO5_CONFIG      (MCUSYS_TOP_BASE + 0x2b0)
#define MCUSYS_TOP_CBIP_CABGEN_3TO1_CONFIG      (MCUSYS_TOP_BASE + 0x2a0)
#define MCUSYS_TOP_CBIP_CABGEN_4TO2_CONFIG      (MCUSYS_TOP_BASE + 0x2a8)
#define MCUSYS_TOP_CBIP_P2P_CONFIG0             (MCUSYS_TOP_BASE + 0x2b4)
#define MCUSYS_TOP_L3GIC_ARCH_CG_CONFIG         (MCUSYS_TOP_BASE + 0x294)
#define MCUSYS_TOP_MP_ADB_DCM_CFG0              (MCUSYS_TOP_BASE + 0x270)
#define MCUSYS_TOP_QDCM_CONFIG0                 (MCUSYS_TOP_BASE + 0x280)
#define MCUSYS_TOP_QDCM_CONFIG1                 (MCUSYS_TOP_BASE + 0x284)
#define MCUSYS_TOP_QDCM_CONFIG2                 (MCUSYS_TOP_BASE + 0x288)
#define MCUSYS_TOP_QDCM_CONFIG3                 (MCUSYS_TOP_BASE + 0x28c)
#define VDNR_DCM_TOP_VLP_PAR_BUS_u_VLP_PAR_BUS_CTRL_0 \
        (VLP_AO_BCRM_BASE + 0x98)
void dcm_vlp_ao_bcrm_vlp_bus_dcm(int on);
void dcm_mcusys_top_mcu_acp_dcm(int on);
void dcm_mcusys_top_mcu_adb_dcm(int on);
void dcm_mcusys_top_mcu_bus_qdcm(int on);
void dcm_mcusys_top_mcu_cbip_dcm(int on);
void dcm_mcusys_top_mcu_core_qdcm(int on);
void dcm_mcusys_top_mcu_io_dcm(int on);
