/*
 *  Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once


#include <bits.h>
#include <platform/addressmap.h>


#define THERMAL_HW_PROTECT                 (0)
#define THERMAL_HARDWARE_RESET_POINT       (116500)
#define LVTS_DEVICE_AUTO_RCK               (0)
#define DUMP_LVTS_REGISTER                 (0)
#define LK_LVTS_MAGIC                      (0x0000555)

#define PRINT_LVTS_DEVICE_WRITE_LOG        (0)
#define PRINT_LVTS_CONTROLLER_WRITE_LOG    (0)
#define LVTS_REFINE_MANUAL_RCK_WITH_EFUSE  (1)
#define LVTS_USE_DOMINATOR_SENSING_POINT   (1)


#define _BITMASK_(_bits_) (((unsigned int) -1 >> (31 - ((1) ? _bits_))) \
        & ~((1U << ((0) ? _bits_)) - 1))

/*
 * module            LVTS Plan
 *=====================================================
 * MCU_LITTLE LVTS1-0, LVTS1-1, LVTS1-2, LVTS1-3
 * MCU_BIG    LVTS2-0, LVTS2-1, LVTS2-2, LVTS2-3
 * MCU_BIG    LVTS3-0, LVTS3-1, LVTS3-2, LVTS3-3
 * ADCT         LVTS4-0, LVTS4-1, LVTS4-2, LVTS4-3
 * GPU            LVTS5-0, LVTS5-1
 * VPU            LVTS6-0, LVTS6-2, LVTS6-1 6-3 is null
 * DRAM CH0     LVTS7-0,
 * DRAM CH1     LVTS7-1,
 * SOC TOP      LVTS7-2,
 * DRAM CH3     LVTS8-0,
 * DRAM CH4     LVTS8-1,
 * SOC MM       LVTS8-2,
 * MD-4G        LVTS9-0
 * MD-5G        LVTS9-1
 * MD-3G        LVTS9-2
 * ptp_therm_ctrl_AP  Base address: (+0x1031_5000), 0x1031_5100,
 *                                 0x1031_5200, 0x1031_5300, 0x1031_5400
 * ptp_therm_ctrl_MCU Base address: (+0x1031_6000), 0x1031_6100,
 *                                 0x1031_6200, 0x1031_6300
 */

/* private thermal sensor enum */
enum lvts_sensor_enum {
    L_TS_LVTS1_0 = 0,    /* LVTS1-0 Little */
    L_TS_LVTS1_1,        /* LVTS1-1 Little */
    L_TS_LVTS1_2,        /* LVTS1-2 Little */
    L_TS_LVTS1_3,        /* LVTS1-3 Little */
    L_TS_LVTS2_0,        /* LVTS2-0 Big */
    L_TS_LVTS2_1,        /* LVTS2-1 Big */
    L_TS_LVTS2_2,        /* LVTS2-2 Big */
    L_TS_LVTS2_3,        /* LVTS2-3 Big */
    L_TS_LVTS3_0,        /* LVTS3-0 Big */
    L_TS_LVTS3_1,        /* LVTS3-1 Big */
    L_TS_LVTS3_2,        /* LVTS3-2 Big */
    L_TS_LVTS3_3,        /* LVTS3-3 Big */
    L_TS_LVTS5_0,        /* LVTS5-0 GPU */
    L_TS_LVTS5_1,        /* LVTS5-1 GPU */
    L_TS_LVTS6_0,        /* LVTS6-0 APU */
    L_TS_LVTS6_2,        /* LVTS6-2 APU */
    L_TS_LVTS7_0,        /* LVTS7-0 DRAM CH2 */
    L_TS_LVTS7_1,        /* LVTS7-1 DRAM CH0 */
    L_TS_LVTS7_2,        /* LVTS7-2 SOC TOP */
    L_TS_LVTS8_0,        /* LVTS8-0 DRAM CH3 */
    L_TS_LVTS8_1,        /* LVTS8-1 DRAM CH1 */
    L_TS_LVTS8_2,        /* LVTS8-2 SOC MM */
    L_TS_LVTS9_0,        /* LVTS9-0 MD-4G */
    L_TS_LVTS9_1,        /* LVTS9-1 MD-5G */
    L_TS_LVTS9_2,        /* LVTS9-2 MD-3G */
    L_TS_LVTS_NUM
};


enum lvts_tc_enum {
    LVTS_MCU_CONTROLLER0 = 0,/* LVTSMONCTL0 */
    LVTS_MCU_CONTROLLER1,    /* LVTSMONCTL0_1 */
    LVTS_MCU_CONTROLLER2,    /* LVTSMONCTL0_2 */
    LVTS_AP_CONTROLLER0,    /* LVTSMONCTL0 */
    LVTS_AP_CONTROLLER1,    /* LVTSMONCTL0_1 */
    LVTS_AP_CONTROLLER2,    /* LVTSMONCTL0_2 */
    LVTS_AP_CONTROLLER3,    /* LVTSMONCTL0_3 */
    LVTS_AP_CONTROLLER4,    /* LVTSMONCTL0_4 */
    LVTS_CONTROLLER_NUM
};


struct lvts_thermal_controller_speed {
    unsigned int group_interval_delay;
    unsigned int period_unit;
    unsigned int filter_interval_delay;
    unsigned int sensor_interval_delay;
};

struct lvts_thermal_controller {
    enum lvts_sensor_enum ts[4]; /* sensor point 0 ~ 3 */
    int ts_number;
    int dominator_ts_idx; /* hw protection ref TS (index of the ts array) */
    int tc_offset;
    struct lvts_thermal_controller_speed tc_speed;
};


#if DUMP_LVTS_REGISTER
#define NUM_LVTS_DEVICE_REG (5)
static const unsigned int g_lvts_device_addrs[NUM_LVTS_DEVICE_REG] = {
        0x00,
        0x04,
        0x08,
        0x0C,
        0xF0};

static unsigned int g_lvts_device_value_b[LVTS_CONTROLLER_NUM]
    [NUM_LVTS_DEVICE_REG];
static unsigned int g_lvts_device_value_e[LVTS_CONTROLLER_NUM]
    [NUM_LVTS_DEVICE_REG];
#endif


/*=============================================================
 * LVTS Thermal Controller Register Definition
 *=============================================================
 */

#define LVTSMONCTL0_0   (THERM_CTRL_BASE + 0x000)
#define LVTSMONCTL1_0   (THERM_CTRL_BASE + 0x004)
#define LVTSMONCTL2_0   (THERM_CTRL_BASE + 0x008)
#define LVTSMONINT_0    (THERM_CTRL_BASE + 0x00C)
#define LVTSMONINTSTS_0 (THERM_CTRL_BASE + 0x010)
#define LVTSMONIDET0_0  (THERM_CTRL_BASE + 0x014)
#define LVTSMONIDET1_0  (THERM_CTRL_BASE + 0x018)
#define LVTSMONIDET2_0  (THERM_CTRL_BASE + 0x01C)
#define LVTSMONIDET3_0  (THERM_CTRL_BASE + 0x020)
#define LVTSH2NTHRE_0   (THERM_CTRL_BASE + 0x024)
#define LVTSHTHRE_0     (THERM_CTRL_BASE + 0x028)
#define LVTSCTHRE_0     (THERM_CTRL_BASE + 0x02C)
#define LVTSOFFSETH_0   (THERM_CTRL_BASE + 0x030)
#define LVTSOFFSETL_0   (THERM_CTRL_BASE + 0x034)
#define LVTSMSRCTL0_0   (THERM_CTRL_BASE + 0x038)
#define LVTSMSRCTL1_0   (THERM_CTRL_BASE + 0x03C)
#define LVTSTSSEL_0     (THERM_CTRL_BASE + 0x040)
#define LVTSDEVICETO_0  (THERM_CTRL_BASE + 0x044)
#define LVTSCALSCALE_0  (THERM_CTRL_BASE + 0x048)
#define LVTS_ID_0       (THERM_CTRL_BASE + 0x04C)
#define LVTS_CONFIG_0   (THERM_CTRL_BASE + 0x050)
#define LVTSEDATA00_0   (THERM_CTRL_BASE + 0x054)
#define LVTSEDATA01_0   (THERM_CTRL_BASE + 0x058)
#define LVTSEDATA02_0   (THERM_CTRL_BASE + 0x05C)
#define LVTSEDATA03_0   (THERM_CTRL_BASE + 0x060)
#define LVTSMSR0_0      (THERM_CTRL_BASE + 0x090)
#define LVTSMSR1_0      (THERM_CTRL_BASE + 0x094)
#define LVTSMSR2_0      (THERM_CTRL_BASE + 0x098)
#define LVTSMSR3_0      (THERM_CTRL_BASE + 0x09C)
#define LVTSIMMD0_0     (THERM_CTRL_BASE + 0x0A0)
#define LVTSIMMD1_0     (THERM_CTRL_BASE + 0x0A4)
#define LVTSIMMD2_0     (THERM_CTRL_BASE + 0x0A8)
#define LVTSIMMD3_0     (THERM_CTRL_BASE + 0x0AC)
#define LVTSRDATA0_0    (THERM_CTRL_BASE + 0x0B0)
#define LVTSRDATA1_0    (THERM_CTRL_BASE + 0x0B4)
#define LVTSRDATA2_0    (THERM_CTRL_BASE + 0x0B8)
#define LVTSRDATA3_0    (THERM_CTRL_BASE + 0x0BC)
#define LVTSPROTCTL_0   (THERM_CTRL_BASE + 0x0C0)
#define LVTSPROTTA_0    (THERM_CTRL_BASE + 0x0C4)
#define LVTSPROTTB_0    (THERM_CTRL_BASE + 0x0C8)
#define LVTSPROTTC_0    (THERM_CTRL_BASE + 0x0CC)
#define LVTSCLKEN_0     (THERM_CTRL_BASE + 0x0E4)
#define LVTSDBGSEL_0    (THERM_CTRL_BASE + 0x0E8)
#define LVTSDBGSIG_0    (THERM_CTRL_BASE + 0x0EC)
#define LVTSSPARE0_0    (THERM_CTRL_BASE + 0x0F0)
#define LVTSSPARE1_0    (THERM_CTRL_BASE + 0x0F4)
#define LVTSSPARE2_0    (THERM_CTRL_BASE + 0x0F8)
#define LVTSSPARE3_0    (THERM_CTRL_BASE + 0x0FC)
#define THERMINTST      (THERM_CTRL_BASE + 0xF04)




#define INFRA_GLOBALCON_RST_0_SET (INFRACFG_AO_BASE + 0x120) //0x10001000
#define INFRA_GLOBALCON_RST_0_CLR (INFRACFG_AO_BASE + 0x124) //0x10001000
#define INFRA_GLOBALCON_RST_0_STA (INFRACFG_AO_BASE + 0x128) //0x10001000

#define INFRA_GLOBALCON_RST_4_SET (INFRACFG_AO_BASE + 0x730)
#define INFRA_GLOBALCON_RST_4_CLR (INFRACFG_AO_BASE + 0x734)
#define INFRA_GLOBALCON_RST_4_STA (INFRACFG_AO_BASE + 0x738)

#define LVTS_COF_T_SLP_GLD            (199410)
#define LVTS_COF_COUNT_R_GLD          (12052)
#define LVTS_COF_T_CONST_OFS          (280000)

#define LVTS_ADCT_COF_T_SLP_GLD       (228040)
#define LVTS_ADCT_COF_COUNT_R_GLD     (13353)
#define LVTS_ADCT_COF_T_CONST_OFS     (280000)

#define LVTS_COF_T_SLP_GLD_HT         (254410)
#define LVTS_COF_COUNT_R_GLD_HT       (15380)
#define LVTS_COF_T_CONST_OFS_HT       (170000)

#define DEFAULT_EFUSE_GOLDEN_TEMP     (60)
#define DEFAULT_EFUSE_GOLDEN_TEMP_HT  (170)
#define DEFAULT_EFUSE_COUNT           (12052)
#define DEFAULT_EFUSE_COUNT_RC        (13000)

/* chip dependent */
/* TODO: change to new reg addr. */
#define LVTS_ADDRESS_INDEX_0    117 /* 0x11F101C4 */
#define LVTS_ADDRESS_INDEX_1    118 /* 0x11F101C8 */
#define LVTS_ADDRESS_INDEX_2    119 /* 0x11F101CC */
#define LVTS_ADDRESS_INDEX_3    190 /* 0x11F101D0 */
#define LVTS_ADDRESS_INDEX_4    191 /* 0x11F101D4 */
#define LVTS_ADDRESS_INDEX_5    192 /* 0x11F101D8 */
#define LVTS_ADDRESS_INDEX_6    193 /* 0x11F101DC */
#define LVTS_ADDRESS_INDEX_7    194 /* 0x11F101E0 */
#define LVTS_ADDRESS_INDEX_8    195 /* 0x11F101E4 */
#define LVTS_ADDRESS_INDEX_9    196 /* 0x11F101E8 */
#define LVTS_ADDRESS_INDEX_10   197 /* 0x11F101EC */
#define LVTS_ADDRESS_INDEX_11   198 /* 0x11F101F0 */
#define LVTS_ADDRESS_INDEX_12   199 /* 0x11F101F4 */
#define LVTS_ADDRESS_INDEX_13   200 /* 0x11F101F8 */
#define LVTS_ADDRESS_INDEX_14   201 /* 0x11F101FC */
#define LVTS_ADDRESS_INDEX_15   202 /* 0x11F10200 */
#define LVTS_ADDRESS_INDEX_16   203 /* 0x11F10204 */
#define LVTS_ADDRESS_INDEX_17   204 /* 0x11F10208 */
#define LVTS_ADDRESS_INDEX_18   205 /* 0x11F1020C */
#define LVTS_ADDRESS_INDEX_19   206 /* 0x11F10210 */
#define LVTS_ADDRESS_INDEX_20   207 /* 0x11F10214 */
#define LVTS_ADDRESS_INDEX_21   208 /* 0x11F10218 */
#define LVTS_ADDRESS_INDEX_22   209 /* 0x11F1021C */
#define LVTS_ADDRESS_INDEX_23   181 /* 0x11F10970 */
#define LVTS_ADDRESS_INDEX_24   182 /* 0x11F10974 */
#define LVTS_ADDRESS_INDEX_25   183 /* 0x11F10978 */
#define LVTS_ADDRESS_INDEX_26   184 /* 0x11F1097C */
#define LVTS_ADDRESS_INDEX_27   185 /* 0x11F10980 */



