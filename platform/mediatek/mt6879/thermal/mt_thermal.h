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


#define THERMAL_HW_PROTECT (0)  //xxx, org 1
#define THERMAL_HARDWARE_RESET_POINT (117000)
#define LVTS_DEVICE_AUTO_RCK (0)
#define DUMP_LVTS_REGISTER (0)
#define LK_LVTS_MAGIC (0x0000555)

#define PRINT_LVTS_DEVICE_WRITE_LOG (0)
#define PRINT_LVTS_CONTROLLER_WRITE_LOG (0)
#define LVTS_REFINE_MANUAL_RCK_WITH_EFUSE  (1)
#define LVTS_USE_DOMINATOR_SENSING_POINT (1)


#define _BITMASK_(_bits_) (((unsigned int) -1 >> (31 - ((1) ? _bits_))) \
        & ~((1U << ((0) ? _bits_)) - 1))

/*
 * ptp_therm_ctrl_AP  Base address: (+0x1031_5000), 0x1031_5100,
 *                                 0x1031_5200, 0x1031_5300, 0x1031_5400
 * ptp_therm_ctrl_MCU Base address: (+0x1031_6000), 0x1031_6100,
 *                                 0x1031_6200, 0x1031_6300
 */
/*
 * module                               LVTS Plan
 *=====================================================
 * MCU_BIG(TS1,TS2)                     LVTS1-0, LVTS1-1
 * MCU_LITTLE(TS3,TS4,TS5,TS6)          LVTS2-0, LVTS2-1, LVTS2-2, LVTS2-3
 * GPU(TS7,TS8)                         LVTS3-0, LVTS3-1
 * APU(TS9,TS10)                        LVTS4-0, LVTS4-1
 * SOC(TS11,TS12,TS13,TS14)             LVTS5-0, LVTS5-1, LVTS5-2, LVTS5-3
 * MD (TS15,TS16,TS17,TS18)             LVTS6-0, LVTS6-1, LVTS6-2, LVTS6-3
 */

/* private thermal sensor enum */
enum lvts_sensor_enum {
    L_TS_LVTS1_0 = 0, //(CPU_6_B)
    L_TS_LVTS1_1,//(CPU_7_B)
    L_TS_LVTS2_0,//(CPU_5_L)
    L_TS_LVTS2_1,//(CPU_0_L,CPU_1_L)
    L_TS_LVTS2_2,//(CPU_2_L,CPU-3_L)
    L_TS_LVTS2_3,//(CPU_4_L,CPU_7_B)
    L_TS_LVTS3_0,//(GPU)
    L_TS_LVTS3_1,//(GPU)
    L_TS_LVTS4_0,//(APU)
    L_TS_LVTS4_1,//(APU)
    L_TS_LVTS5_0,//(SOC_CAM)
    L_TS_LVTS5_1,//(SOC_DRAMC)
    L_TS_LVTS5_2,//(SOC_VLP)
    L_TS_LVTS5_3,//(SOC_CONNSYS)
    L_TS_LVTS6_0,//(MD)
    L_TS_LVTS6_1,//(MD)
    L_TS_LVTS6_2,//(MD)
    L_TS_LVTS6_3,//(MD)
    L_TS_LVTS_NUM
};


enum lvts_tc_enum {
    LVTS_MCU_CONTROLLER0 = 0,/* LVTSMONCTL0 */
    LVTS_MCU_CONTROLLER1,/* LVTSMONCTL0_1 */
    LVTS_AP_CONTROLLER0,/* LVTSMONCTL0 */
    LVTS_AP_CONTROLLER1,/* LVTSMONCTL0_1 */
    LVTS_AP_CONTROLLER2,/* LVTSMONCTL0_2 */
    LVTS_AP_CONTROLLER3,/* LVTSMONCTL0_3 */
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

#define LVTS_COEFF_A_X_1000         (-250460)
#define LVTS_COEFF_B_X_1000          (250460)
#define DEFAULT_EFUSE_GOLDEN_TEMP       (50)
#define DEFAULT_EFUSE_COUNT         (35000)
#define DEFAULT_EFUSE_COUNT_RC      (2750)




/* chip dependent */
/* TODO: change to new reg addr. */
#define LVTS_ADDRESS_INDEX_1    116 /* 0x11F101C0 */
#define LVTS_ADDRESS_INDEX_2    117 /* 0x11F101C4 */
#define LVTS_ADDRESS_INDEX_3    118 /* 0x11F101C8 */
#define LVTS_ADDRESS_INDEX_4    119 /* 0x11F101CC */
#define LVTS_ADDRESS_INDEX_5    190 /* 0x11F101D0 */
#define LVTS_ADDRESS_INDEX_6    191 /* 0x11F101D4 */
#define LVTS_ADDRESS_INDEX_7    192 /* 0x11F101D8 */
#define LVTS_ADDRESS_INDEX_8    193 /* 0x11F101DC */
#define LVTS_ADDRESS_INDEX_9    194 /* 0x11F101E0 */
#define LVTS_ADDRESS_INDEX_10   195 /* 0x11F101E4 */
#define LVTS_ADDRESS_INDEX_11   196 /* 0x11F101E8 */
#define LVTS_ADDRESS_INDEX_12   197 /* 0x11F101EC */
#define LVTS_ADDRESS_INDEX_13   198 /* 0x11F101F0 */
#define LVTS_ADDRESS_INDEX_14   199 /* 0x11F101F4 */
#define LVTS_ADDRESS_INDEX_15   200 /* 0x11F101F8 */
#define LVTS_ADDRESS_INDEX_16   201 /* 0x11F101FC */
#define LVTS_ADDRESS_INDEX_17   202 /* 0x11F10200 */
#define LVTS_ADDRESS_INDEX_18   203 /* 0x11F10204 */
#define LVTS_ADDRESS_INDEX_19   204 /* 0x11F10208 */
#define LVTS_ADDRESS_INDEX_20   205 /* 0x11F1020C */
#define LVTS_ADDRESS_INDEX_21   206 /* 0x11F10210 */
#define LVTS_ADDRESS_INDEX_22   207 /* 0x11F10214 */
#define LVTS_ADDRESS_INDEX_23   208 /* 0x11F10218 */
#define LVTS_ADDRESS_INDEX_24   209 /* 0x11F1021C */

