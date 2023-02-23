/*
 *  Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <debug.h>
#include "init_mtk.h"
#include "lk/init.h"
#include "mt_thermal.h"
#include <platform/sec_devinfo.h>
#include <profiling.h>
#include <reg.h>
#include <string.h>
#include <stdlib.h>
#include <trace.h>
/*=============================================================
 * Global variable definition
 *=============================================================
 */
int tscpu_ts_lvts_temp[L_TS_LVTS_NUM];
int tscpu_ts_lvts_temp_r[L_TS_LVTS_NUM];

#define LOCAL_TRACE 0

#define THERM_LOG(fmt, args...)  dprintf(CRITICAL, "[Thermal]" fmt, ##args)
#define THERM_DLOG(fmt, args...)  LTRACEF("[Thermal]" fmt, ##args)

/*
 * module                               LVTS Plan
 *=====================================================
 * MCU_BIG(TS1,TS2,TS3,TS4)             LVTS1-0, LVTS1-1, LVTS1-2, LVTS1-3
 * MCU_LITTLE(TS5,TS6,TS7,TS8)          LVTS2-0, LVTS2-1, LVTS2-2, LVTS2-3
 * MCU_LITTLE(TS9,TS10,TS11,TS12)       LVTS3-0, LVTS3-1, LVTS3-2, LVTS3-3
 * GPU(TS13,TS14)                       LVTS4-0, LVTS4-1
 * SOC(TS15,TS16,TS17,TS18)             LVTS5-0, LVTS5-1, LVTS5-2, LVTS5-3
 * MD (TS19,TS20,TS21)                  LVTS6-0, LVTS6-1, LVTS6-2
 */


struct lvts_thermal_controller lvts_tscpu_g_tc[LVTS_CONTROLLER_NUM] = {
    [LVTS_MCU_CONTROLLER0] = {/*(MCU CPU B)*/
            .ts = {L_TS_LVTS1_0, L_TS_LVTS1_1, L_TS_LVTS1_2, L_TS_LVTS1_3},
            .ts_number = 4,
            .dominator_ts_idx = 3, //TODO: need confirm dominator sensor
            .tc_offset = 0x1000, //0x10316000 = 0x10315000 + 1000
            .tc_speed = {
                .group_interval_delay = 0x001,
                .period_unit = 0x001,
                .filter_interval_delay = 0x001,
                .sensor_interval_delay = 0x001,
            }
    },
    [LVTS_MCU_CONTROLLER1] = {/*(MCU CPU L)*/
            .ts = {L_TS_LVTS2_0, L_TS_LVTS2_1, L_TS_LVTS2_2, L_TS_LVTS2_3},
            .ts_number = 4,
            .dominator_ts_idx = 2, //TODO: need confirm dominator sensor
            .tc_offset = 0x1100,//0x10316100 = 0x10315000 + 1100
            .tc_speed = {
                .group_interval_delay = 0x001,
                .period_unit = 0x00c,
                .filter_interval_delay = 0x001,
                .sensor_interval_delay = 0x001,
            }
    },
    [LVTS_MCU_CONTROLLER2] = {/*(MCU CPU L)*/
            .ts = {L_TS_LVTS3_0, L_TS_LVTS3_1, L_TS_LVTS3_2, L_TS_LVTS3_3},
            .ts_number = 4,
            .dominator_ts_idx = 3, //TODO: need confirm dominator sensor
            .tc_offset = 0x1200, //0x10316200
            .tc_speed = {
                .group_interval_delay = 0x001,
                .period_unit = 0x00c,
                .filter_interval_delay = 0x001,
                .sensor_interval_delay = 0x001,
            }
    },
    [LVTS_AP_CONTROLLER0] = {/*(AP GPU)*/
            .ts = {L_TS_LVTS4_0, L_TS_LVTS4_1},
            .ts_number = 2,
            .dominator_ts_idx = 1, //TODO: need confirm dominator sensor
            .tc_offset = 0x0, //0x10315000
            .tc_speed = {
                .group_interval_delay = 0x001,
                .period_unit = 0x00c,
                .filter_interval_delay = 0x001,
                .sensor_interval_delay = 0x001,
            }
    },
    [LVTS_AP_CONTROLLER1] = {/*(AP SOC)*/
            .ts = {L_TS_LVTS5_0, L_TS_LVTS5_1, L_TS_LVTS5_2, L_TS_LVTS5_3},
            .ts_number = 4,
            .dominator_ts_idx = 3, //TODO: need confirm dominator sensor
            .tc_offset = 0x100, //0x10315100
            .tc_speed = {
                .group_interval_delay = 0x001,
                .period_unit = 0x00c,
                .filter_interval_delay = 0x001,
                .sensor_interval_delay = 0x001,
            }
    },
    [LVTS_AP_CONTROLLER2] = {/*(AP MD)*/
            .ts = {L_TS_LVTS6_0, L_TS_LVTS6_1, L_TS_LVTS6_2},
            .ts_number = 3,
            .dominator_ts_idx = 2, //TODO: need confirm dominator sensor
            .tc_offset = 0x200, //0x10315200
            .tc_speed = {
                .group_interval_delay = 0x001,
                .period_unit = 0x00c,
                .filter_interval_delay = 0x001,
                .sensor_interval_delay = 0x001,
            }
    }
};

/*=============================================================
 * Local variable definition
 *=============================================================
 */
static unsigned int g_golden_temp;
static unsigned int g_count_r[L_TS_LVTS_NUM];
static unsigned int g_count_rc[L_TS_LVTS_NUM];
static unsigned int g_count_rc_now[L_TS_LVTS_NUM];
static int g_use_fake_efuse;


void mt_reg_sync_writel_print(unsigned int val, void *addr)
{
    THERM_DLOG("### LVTS_REG: addr 0x%p, val 0x%x\n", addr, val);
    writel(val, addr);
}

static int lvts_write_device(unsigned int config, unsigned int dev_reg_idx,
unsigned int data, int tc_num)
{
    unsigned int offset, cnt;

    dev_reg_idx &= 0xFF;
    data &= 0xFF;

    config = config | (dev_reg_idx << 8) | data;

    offset = lvts_tscpu_g_tc[tc_num].tc_offset;

    mt_reg_sync_writel_print(config, (void *)(LVTS_CONFIG_0 + offset));

    /*
     * LVTS Device Register Setting take 1us(by 26MHz clock source)
     * interface latency to access.
     * So we set 2~3us delay could guarantee access complete.
     */
    spin(3);

    /* Check ASIF bus status for transaction finished
     * Wait until DEVICE_ACCESS_START = 0
     */
    cnt = 0;
    while (BIT(readl(LVTS_CONFIG_0 + offset), 24)) {
        cnt++;

        if (cnt == 100) {
            THERM_LOG("write err: status not ready, reg0x%x\n",
                dev_reg_idx);
            break;
        }
        spin(2);
    }

#if PRINT_LVTS_DEVICE_WRITE_LOG
    THERM_LOG("%s LVTS_CONFIG_%d (0x%x) = 0x%x\n", __func__,
        tc_num, LVTS_CONFIG_0 + offset, readl(LVTS_CONFIG_0 + offset));
#endif
    return 1;
}

static unsigned int lvts_read_device(unsigned int config,
unsigned int dev_reg_idx, int tc_num)
{
    int offset, cnt;
    unsigned int data;

    dev_reg_idx &= 0xFF;

    config = config | (dev_reg_idx << 8) | 0x00;

    offset = lvts_tscpu_g_tc[tc_num].tc_offset;

    mt_reg_sync_writel_print(config, (void *)(LVTS_CONFIG_0 + offset));

    /* wait 2us + 3us buffer*/
    spin(5);
    /* Check ASIF bus status for transaction finished
     * Wait until DEVICE_ACCESS_START = 0
     */
    cnt = 0;
    while (BIT(readl(LVTS_CONFIG_0 + offset), 24)) {
        cnt++;
        if (cnt == 100) {
            THERM_LOG("Error: DEVICE_ACCESS_START didn't ready\n");
            break;
        }
        spin(2);
    }

    data = (readl(LVTSRDATA0_0 + offset));

    return data;
}



static int lvts_raw_to_temp(unsigned int msr_raw, enum lvts_sensor_enum ts_name)
{
    /* This function returns degree mC
     * temp[i] = a * MSR_RAW/16384 + GOLDEN_TEMP/2 + b
     * a = -250.46
     * b =  250.46
     */
    int temp_mC = 0;
    int temp1 = 0;

    temp1 = (LVTS_COEFF_A_X_1000 * ((unsigned long long int)msr_raw)) >> 14;

    temp_mC = temp1 + g_golden_temp * 500 + LVTS_COEFF_B_X_1000;

    return temp_mC;
}

static unsigned int lvts_temp_to_raw(int temp, enum lvts_sensor_enum ts_name)
{
    /* MSR_RAW = ((temp[i] - GOLDEN_TEMP/2 - b) * 16384) / a
     * a = -250.46
     * b =  250.46
     */
    unsigned int msr_raw = 0;

    msr_raw = ((long long int)((g_golden_temp * 500 + LVTS_COEFF_B_X_1000
        - temp)) << 14)/(-1 * LVTS_COEFF_A_X_1000);


    THERM_LOG("%s msr_raw = %u,temp=%d\n", __func__, msr_raw, temp);
    return msr_raw;
}


static void lvts_efuse_setting(void)
{
    unsigned int offset;
    unsigned int i;
    int j, s_index;
    unsigned int efuse_data;

    THERM_DLOG("%s\n", __func__);

    for (i = 0; i < countof(lvts_tscpu_g_tc); i++) {
        for (j = 0; j < lvts_tscpu_g_tc[i].ts_number; j++) {

            offset = lvts_tscpu_g_tc[i].tc_offset;
            s_index = lvts_tscpu_g_tc[i].ts[j];

            #if LVTS_DEVICE_AUTO_RCK == 0
            efuse_data =
                (((unsigned long long int)g_count_rc_now[s_index]) *
                    g_count_r[s_index]) >> 14;
            #else
                efuse_data = g_count_r[s_index];
            #endif

            switch (j) {
            case 0:
                    mt_reg_sync_writel_print(efuse_data,
                        (void *)(LVTSEDATA00_0 + offset));
                    THERM_DLOG("efuse LVTSEDATA00_%d 0x%x\n",
                        i, readl(LVTSEDATA00_0 + offset));
                break;

            case 1:
                    mt_reg_sync_writel_print(efuse_data,
                        (void *)(LVTSEDATA01_0 + offset));
                    THERM_DLOG("efuse LVTSEDATA01_%d 0x%x\n",
                        i, readl(LVTSEDATA01_0 + offset));
                break;
            case 2:
                    mt_reg_sync_writel_print(efuse_data,
                        (void *)(LVTSEDATA02_0 + offset));
                    THERM_DLOG("efuse LVTSEDATA02_%d 0x%x\n",
                        i, readl(LVTSEDATA02_0 + offset));
                break;
            case 3:
                    mt_reg_sync_writel_print(efuse_data,
                        (void *)(LVTSEDATA03_0 + offset));
                    THERM_DLOG("efuse LVTSEDATA03_%d 0x%x\n",
                        i, readl(LVTSEDATA03_0 + offset));
                break;
            default:
                    THERM_DLOG("%s, illegal ts order : %d!!\n",
                        __func__, j);
                break;
            }
        }
    }
}
static void lvts_device_identification(void)
{
    unsigned int tc_num, offset, data, cnt;

    THERM_DLOG("===== %s begin ======\n", __func__);
    for (tc_num = 0; tc_num < countof(lvts_tscpu_g_tc); tc_num++) {

        offset = lvts_tscpu_g_tc[tc_num].tc_offset; //tc offset

        // Enable LVTS_CTRL Clock
        mt_reg_sync_writel_print(0x00000001, (void *)(LVTSCLKEN_0 + offset));

        // Reset All Devices
        lvts_write_device(0xC1030000, 0xFF, 0xFF, tc_num);

        //  Read back Dev_ID with Update
        lvts_write_device(0xC5020000, 0xFC, 0x55, tc_num);

        /*  Check LVTS device ID */
        cnt = 0;
        data = (readl(LVTS_ID_0 + offset) & _BITMASK_(7:0));
        if (data != (0x81 + tc_num))
            THERM_LOG("LVTS_TC_%d, addr:0x%x, Device ID should be 0x%x, but 0x%x\n",
                tc_num, (unsigned int)(LVTS_ID_0 + offset), (0x81 + tc_num), data);

    }
}
static void lvts_Device_Enable_Init_all_Devices(void)
{
    unsigned int i;

    THERM_DLOG("%s\n", __func__);

    for (i = 0; i < countof(lvts_tscpu_g_tc); i++) {
        /* Stop Counting (RG_TSFM_ST=0) */
        lvts_write_device(0xC1030000, 0x03, 0x00, i);
        /* RG_TSFM_LPDLY[1:0]=2' 10 */
        lvts_write_device(0xC1030000, 0x07, 0xA6, i);
        /* Set LVTS device counting window 20us */
        lvts_write_device(0xC1030000, 0x05, 0x00, i);
        lvts_write_device(0xC1030000, 0x04, 0x20, i);
        /* TSV2F_CHOP_CKSEL & TSV2F_EN */
        lvts_write_device(0xC1030000, 0x0A, 0x8C, i);
        /* TSBG_DEM_CKSEL * TSBG_CHOP_EN */
        lvts_write_device(0xC1030000, 0x0C, 0xFC, i);
        /* Set TS_RSV */
        lvts_write_device(0xC1030000, 0x09, 0x8D, i);
        /* Set TS_CHOP control */
        lvts_write_device(0xC1030000, 0x08, 0xF5, i);
    }
}

static void lvts_thermal_cal_prepare(void)
{
    unsigned int temp[20];
    int i, offset;
    char buffer[512];


    THERM_LOG("===== %s ======\n", __func__);


    temp[0] = get_devinfo_with_index(LVTS_ADDRESS_INDEX_1);
    temp[1] = get_devinfo_with_index(LVTS_ADDRESS_INDEX_2);
    temp[2] = get_devinfo_with_index(LVTS_ADDRESS_INDEX_3);
    temp[3] = get_devinfo_with_index(LVTS_ADDRESS_INDEX_4);
    temp[4] = get_devinfo_with_index(LVTS_ADDRESS_INDEX_5);
    temp[5] = get_devinfo_with_index(LVTS_ADDRESS_INDEX_6);
    temp[6] = get_devinfo_with_index(LVTS_ADDRESS_INDEX_7);
    temp[7] = get_devinfo_with_index(LVTS_ADDRESS_INDEX_8);
    temp[8] = get_devinfo_with_index(LVTS_ADDRESS_INDEX_9);
    temp[9] = get_devinfo_with_index(LVTS_ADDRESS_INDEX_10);
    temp[10] = get_devinfo_with_index(LVTS_ADDRESS_INDEX_11);
    temp[11] = get_devinfo_with_index(LVTS_ADDRESS_INDEX_12);
    temp[12] = get_devinfo_with_index(LVTS_ADDRESS_INDEX_13);
    temp[13] = get_devinfo_with_index(LVTS_ADDRESS_INDEX_14);
    temp[14] = get_devinfo_with_index(LVTS_ADDRESS_INDEX_15);
    temp[15] = get_devinfo_with_index(LVTS_ADDRESS_INDEX_16);

    temp[16] = get_devinfo_with_index(LVTS_ADDRESS_INDEX_17);
    temp[17] = get_devinfo_with_index(LVTS_ADDRESS_INDEX_18);
    temp[18] = get_devinfo_with_index(LVTS_ADDRESS_INDEX_19);

    temp[19] = get_devinfo_with_index(LVTS_ADDRESS_INDEX_20);


    for (i = 0; i < countof(temp); i++) {
        if (i%5 == 0)
            THERM_LOG("[lvts_call] ");

        THERM_LOG("(%d: 0x%x)", i, temp[i]);
        if (i%5 == 4)
            THERM_LOG("\n");

    }

    g_golden_temp = ((temp[0] & _BITMASK_(31:24)) >> 24);

    g_count_r[L_TS_LVTS1_0] =  ((temp[1] & _BITMASK_(31:16))>>16);
    g_count_r[L_TS_LVTS1_1] =  (temp[1] & _BITMASK_(15:0));
    g_count_r[L_TS_LVTS1_2] =  ((temp[2] & _BITMASK_(31:16))>>16);
    g_count_r[L_TS_LVTS1_3] =  (temp[2] & _BITMASK_(15:0));
    g_count_r[L_TS_LVTS2_0] =  ((temp[3] & _BITMASK_(31:16))>>16);
    g_count_r[L_TS_LVTS2_1] =  (temp[3] & _BITMASK_(15:0));
    g_count_r[L_TS_LVTS2_2] =  ((temp[4] & _BITMASK_(31:16))>>16);
    g_count_r[L_TS_LVTS2_3] =  (temp[4] & _BITMASK_(15:0));
    g_count_r[L_TS_LVTS3_0] =  ((temp[5] & _BITMASK_(31:16))>>16);
    g_count_r[L_TS_LVTS3_1] =  (temp[5] & _BITMASK_(15:0));
    g_count_r[L_TS_LVTS3_2] =  ((temp[6] & _BITMASK_(31:16))>>16);
    g_count_r[L_TS_LVTS3_3] =  (temp[6] & _BITMASK_(15:0));
    g_count_r[L_TS_LVTS4_0] =  ((temp[7] & _BITMASK_(31:16))>>16);
    g_count_r[L_TS_LVTS4_1] =  (temp[7] & _BITMASK_(15:0));
    g_count_r[L_TS_LVTS5_0] =  ((temp[8] & _BITMASK_(31:16))>>16);
    g_count_r[L_TS_LVTS5_1] =  (temp[8] & _BITMASK_(15:0));
    g_count_r[L_TS_LVTS5_2] =  ((temp[9] & _BITMASK_(31:16))>>16);
    g_count_r[L_TS_LVTS5_3] =  (temp[9] & _BITMASK_(15:0));
    g_count_r[L_TS_LVTS6_0] =  ((temp[10] & _BITMASK_(31:16))>>16);
    g_count_r[L_TS_LVTS6_1] =  (temp[10] & _BITMASK_(15:0));
    g_count_r[L_TS_LVTS6_2] =  ((temp[11] & _BITMASK_(31:16))>>16);


    g_count_rc[LVTS_MCU_CONTROLLER0] = (temp[13] & _BITMASK_(23:0));

    g_count_rc[LVTS_MCU_CONTROLLER1] = (temp[14] & _BITMASK_(23:0));

    g_count_rc[LVTS_MCU_CONTROLLER2] = (temp[15] & _BITMASK_(23:0));

    g_count_rc[LVTS_AP_CONTROLLER0] = (temp[16] & _BITMASK_(23:0));

    g_count_rc[LVTS_AP_CONTROLLER1] = (temp[17] & _BITMASK_(23:0));

    g_count_rc[LVTS_AP_CONTROLLER2] = (temp[18] & _BITMASK_(23:0));



    for (i = 0; i < countof(temp); i++) {
        if (i == 0) {
            if (((temp[0] & _BITMASK_(31:24))>>24) != 0)
                break;
        } else {
            if (temp[i] != 0)
                break;
        }
    }

    if (i == countof(temp)) {
        /* It means all efuse data are equal to 0 */
        THERM_LOG(
        "[lvts_cal] This sample is not calibrated, fake !!\n");
        g_golden_temp = DEFAULT_EFUSE_GOLDEN_TEMP;
        for (i = 0; i < L_TS_LVTS_NUM; i++) {
            g_count_r[i] = DEFAULT_EFUSE_COUNT;
            g_count_rc[i] = DEFAULT_EFUSE_COUNT_RC;
        }

        g_use_fake_efuse = 1;
    }

    THERM_LOG("[lvts_cal] g_golden_temp = %d\n", g_golden_temp);

    offset = snprintf(buffer, sizeof(buffer),
        "[lvts_cal] num:g_count_r:g_count_rc ");

    for (i = 0; i < L_TS_LVTS_NUM; i++) {

        if (((sizeof(buffer) - offset) <= 0) || (offset < 0)) {
            THERM_LOG("%s %d error\n", __func__, __LINE__);
            break;
        }

        offset += snprintf(buffer + offset,
            (sizeof(buffer) - offset), "%d:%d:%d ",
            i, g_count_r[i], g_count_rc[i]);
    }

    /* write gold temp to spare reg LVTSSPARE1_0 */
    offset = lvts_tscpu_g_tc[0].tc_offset;
    mt_reg_sync_writel_print(g_golden_temp,
        (void *)(offset + LVTSSPARE1_0));

    THERM_LOG("%s\n", buffer);
}

/*
 * temperature2 to set the middle threshold for interrupting CPU.
 * -275000 to disable it.
 */
static void lvts_set_tc_trigger_hw_protect(
int temperature, int temperature2, int tc_num)
{
    int temp = 0, raw_high, config, offset;
    #if LVTS_USE_DOMINATOR_SENSING_POINT
    unsigned int d_index;
    enum lvts_sensor_enum ts_name;
    #endif
    offset = lvts_tscpu_g_tc[tc_num].tc_offset;

    THERM_DLOG("%s t1=%d t2=%d\n",
        __func__, temperature, temperature2);

    #if LVTS_USE_DOMINATOR_SENSING_POINT
    if (lvts_tscpu_g_tc[tc_num].dominator_ts_idx <
                lvts_tscpu_g_tc[tc_num].ts_number) {
        d_index = lvts_tscpu_g_tc[tc_num].dominator_ts_idx;
    } else {
        THERM_LOG(
            "Error: LVTS TC%d, dominator_ts_idx = %d should smaller than ts_number = %d\n",
            tc_num,
            lvts_tscpu_g_tc[tc_num].dominator_ts_idx,
            lvts_tscpu_g_tc[tc_num].ts_number);

        THERM_LOG(
            "Use the sensor point 0 as the dominated sensor\n");
            d_index = 0;
    }

    ts_name = lvts_tscpu_g_tc[tc_num].ts[d_index];

    THERM_LOG("%s # in tc%d , the dominator ts_name is %d\n",
        __func__, tc_num, ts_name);

    /* temperature to trigger SPM state2 */
    raw_high = lvts_temp_to_raw(temperature, ts_name);
    #else
    raw_high = lvts_temp_to_raw(temperature, 0);
    #endif

    #ifndef CONFIG_LVTS_DYNAMIC_ENABLE_REBOOT
    temp = readl(offset + LVTSMONINT_0);
    /* disable trigger SPM interrupt */
    mt_reg_sync_writel_print(temp & 0x00000000,
        (void *)(offset + LVTSMONINT_0));
    #endif

    temp = readl(offset + LVTSPROTCTL_0) & ~(0xF << 16);
    #if LVTS_USE_DOMINATOR_SENSING_POINT
    /* Select protection sensor */
    config = ((d_index << 2) + 0x2) << 16;
    mt_reg_sync_writel_print(temp | config,
        (void *)(offset + LVTSPROTCTL_0));
    #else
    /* Maximum of 4 sensing points */
    config = (0x1 << 16);
    mt_reg_sync_writel_print(temp | config,
        (void *)(offset + LVTSPROTCTL_0));
    #endif

    /* set hot to HOT wakeup event */
    mt_reg_sync_writel_print(raw_high,
        (void *)(offset + LVTSPROTTC_0));

    #ifndef CONFIG_LVTS_DYNAMIC_ENABLE_REBOOT
    /* enable trigger Hot SPM interrupt */
    mt_reg_sync_writel_print(temp | 0x80000000,
        (void *)(offset + LVTSMONINT_0));
    #endif

}

static int lvts_read_tc_raw_and_temp(
        unsigned int *tempmsr_name, enum lvts_sensor_enum ts_name)
{
    int temp = 0, raw = 0, raw1 = 0, raw2 = 0;


    if (tempmsr_name == 0)
        return 0;

    raw = readl((tempmsr_name));
    raw1 = (raw & 0x10000) >> 16; //bit 16 : valid bit
    raw2 = raw & 0xFFFF;
    temp = lvts_raw_to_temp(raw2, ts_name);

    if (raw2 == 0) {
        /* 26111 is magic num
         * this is to keep system alive for a while
         * to wait HW init done,
         * because 0 msr raw will translates to 28x'C
         * and then 28x'C will trigger a SW reset.
         *
         * if HW init finish, this msr raw will not be 0,
         * system can report normal temperature.
         * if wait over 60 times zero, this means something
         * wrong with HW, must trigger BUG on and dump useful
         * register for debug.
         */

        temp = 26111;
    }

    THERM_DLOG(
        "[LVTS_MSR] ts%d msr_all=%x, valid=%d, msr_temp=%d, temp=%d\n",
        ts_name, raw, raw1, raw2, temp);

    tscpu_ts_lvts_temp_r[ts_name] = raw2;

    return temp;
}

static void lvts_tscpu_thermal_read_tc_temp(
        int tc_num, enum lvts_sensor_enum type, int order)
{
    unsigned int offset;

    offset = lvts_tscpu_g_tc[tc_num].tc_offset;

    switch (order) {
    case 0:
        tscpu_ts_lvts_temp[type] =
            lvts_read_tc_raw_and_temp((unsigned int *)(offset + LVTSMSR0_0), type);
        break;
    case 1:
        tscpu_ts_lvts_temp[type] =
            lvts_read_tc_raw_and_temp((unsigned int *)(offset + LVTSMSR1_0), type);
        break;
    case 2:
        tscpu_ts_lvts_temp[type] =
            lvts_read_tc_raw_and_temp((unsigned int *)(offset + LVTSMSR2_0), type);
        break;
    case 3:
        tscpu_ts_lvts_temp[type] =
            lvts_read_tc_raw_and_temp((unsigned int *)(offset + LVTSMSR3_0), type);
        break;
    default:
        tscpu_ts_lvts_temp[type] =
            lvts_read_tc_raw_and_temp((unsigned int *)(offset + LVTSMSR0_0), type);
            THERM_LOG(
            "%s order %d tc_num %d type %d temp %d\n",
            __func__, order, tc_num, type,
            tscpu_ts_lvts_temp[type]);
        break;
    }

    /*
     * THERM_LOG("%s order %d tc_num %d type %d temp %d\n",
     * __func__, order, tc_num, type,
     * tscpu_ts_lvts_temp[type]);
     */
}

static void read_all_tc_lvts_temperature(void)
{
    unsigned int i;
    int j;

    for (i = 0; i < countof(lvts_tscpu_g_tc); i++)
        for (j = 0; j < lvts_tscpu_g_tc[i].ts_number; j++)
            lvts_tscpu_thermal_read_tc_temp(i,
            lvts_tscpu_g_tc[i].ts[j], j);
}

/* pause ALL periodoc temperature sensing point */
static void lvts_pause_all_sensing_points(void)
{
    unsigned int i;
    int temp, offset;

    THERM_DLOG("%s\n", __func__);

    for (i = 0; i < countof(lvts_tscpu_g_tc); i++) {
        offset = lvts_tscpu_g_tc[i].tc_offset;
        temp = readl(offset + LVTSMSRCTL1_0);
        /* set bit8=bit1=bit2=bit3=1 to pause sensing point 0,1,2,3 */
        mt_reg_sync_writel_print((temp | 0x10E),
            (void *)(offset + LVTSMSRCTL1_0));
    }
}


/*
 * disable ALL periodoc temperature sensing point
 */
static void lvts_disable_all_sensing_points(void)
{
    int offset;
    unsigned int i = 0;

    THERM_DLOG("%s\n", __func__);

    for (i = 0; i < countof(lvts_tscpu_g_tc); i++) {
        offset = lvts_tscpu_g_tc[i].tc_offset;
        mt_reg_sync_writel_print(0x00000000, (void *)(offset + LVTSMONCTL0_0));
    }
}

static void lvts_enable_all_sensing_points(void)
{
    int offset;
    unsigned int i = 0;

    THERM_DLOG("%s\n", __func__);

    for (i = 0; i < countof(lvts_tscpu_g_tc); i++) {

        offset = lvts_tscpu_g_tc[i].tc_offset;

        THERM_LOG("%s %d:%d\n", __func__, i,
        lvts_tscpu_g_tc[i].ts_number);

        switch (lvts_tscpu_g_tc[i].ts_number) {
        case 1:
                /* enable sensing point 0 */
                mt_reg_sync_writel_print(0x00000201,
                (void *)(offset + LVTSMONCTL0_0));
            break;
        case 2:
                /* enable sensing point 0,1 */
                mt_reg_sync_writel_print(0x00000203,
                (void *)(offset + LVTSMONCTL0_0));
            break;
        case 3:
                /* enable sensing point 0,1,2 */
                mt_reg_sync_writel_print(0x00000207,
                (void *)(offset + LVTSMONCTL0_0));
            break;
        case 4:
                /* enable sensing point 0,1,2,3*/
                mt_reg_sync_writel_print(0x0000020F,
                (void *)(offset + LVTSMONCTL0_0));
            break;
        default:
                THERM_LOG("Error at %s\n", __func__);
            break;
        }
    }
}

static void lvts_set_init_flag(void)
{
    int offset;
    unsigned int i;

    THERM_DLOG("%s\n", __func__);

    /*write LK init done flag to inform kernel */

    for (i = 0; i < countof(lvts_tscpu_g_tc); i++) {

        offset = lvts_tscpu_g_tc[i].tc_offset;

        THERM_DLOG("%s %d:%d, offset:0x%x\n", __func__, i,
            lvts_tscpu_g_tc[i].ts_number, offset);

        switch (lvts_tscpu_g_tc[i].ts_number) {
        case 1:
                /* enable sensing point 0 */
                mt_reg_sync_writel_print(LK_LVTS_MAGIC,
                (void *)(offset + LVTSSPARE0_0));
            break;
        case 2:
                /* enable sensing point 0,1 */
                mt_reg_sync_writel_print(LK_LVTS_MAGIC,
                (void *)(offset + LVTSSPARE0_0));
            break;
        case 3:
                /* enable sensing point 0,1,2 */
                mt_reg_sync_writel_print(LK_LVTS_MAGIC,
                (void *)(offset + LVTSSPARE0_0));
            break;
        case 4:
                /* enable sensing point 0,1,2,3*/
                mt_reg_sync_writel_print(LK_LVTS_MAGIC,
                (void *)(offset + LVTSSPARE0_0));
            break;
        default:
                THERM_LOG("Error at %s\n", __func__);
            break;
        }
    }
}


static void lvts_configure_polling_speed_and_filter(unsigned int tc_num)
{
    unsigned int offset, lvtsMonCtl1, lvtsMonCtl2;

    THERM_DLOG("%s\n", __func__);

    offset = lvts_tscpu_g_tc[tc_num].tc_offset;

    lvtsMonCtl1 = (((lvts_tscpu_g_tc[tc_num].tc_speed.group_interval_delay
        << 20) & _BITMASK_(29:20)) |
        (lvts_tscpu_g_tc[tc_num].tc_speed.period_unit & _BITMASK_(9:0)));

    lvtsMonCtl2 = (((lvts_tscpu_g_tc[tc_num].tc_speed.filter_interval_delay
        << 16) & _BITMASK_(25:16)) |
        (lvts_tscpu_g_tc[tc_num].tc_speed.sensor_interval_delay & _BITMASK_(9:0)));

    /*
     * Calculating period unit in Module clock x 256, and the Module clock
     * will be changed to 26M when Infrasys enters Sleep mode.
     */

    /*
     * bus clock 66M counting unit is
     *           12 * 1/66M * 256 = 12 * 3.879us = 46.545 us
     */
    mt_reg_sync_writel_print(lvtsMonCtl1, (void *)(offset + LVTSMONCTL1_0));
    /*
     * filt interval is 1 * 46.545us = 46.545us,
     * sen interval is 429 * 46.545us = 19.968ms
     */
    mt_reg_sync_writel_print(lvtsMonCtl2, (void *)(offset + LVTSMONCTL2_0));

    /* temperature sampling control, one sampling */
    mt_reg_sync_writel_print(0, (void *)(offset + LVTSMSRCTL0_0));

    spin(1);
    THERM_LOG("%s %d, LVTSMONCTL1_0= 0x%x,LVTSMONCTL2_0= 0x%x,LVTSMSRCTL0_0= 0x%x\n",
        __func__, tc_num,
    readl(LVTSMONCTL1_0 + offset),
    readl(LVTSMONCTL2_0 + offset),
    readl(LVTSMSRCTL0_0 + offset));
}

static void lvts_tscpu_thermal_initial_all_tc(void)
{
    int offset;
    unsigned int i = 0;

    THERM_DLOG("%s\n", __func__);

    for (i = 0; i < countof(lvts_tscpu_g_tc); i++) {
        offset = lvts_tscpu_g_tc[i].tc_offset;

        /*  set sensor index of LVTS */
        mt_reg_sync_writel_print(0x13121110, (void *)(LVTSTSSEL_0 + offset));
        /*  set calculation scale rules */
        mt_reg_sync_writel_print(0x00000300, (void *)(LVTSCALSCALE_0 + offset));
        /* Set Device Single mode */
        lvts_write_device(0xC1030000, 0x06, 0xB8, i);

        lvts_configure_polling_speed_and_filter(i);
    }

    #if DUMP_LVTS_REGISTER_FOR_ZERO_RAW_ISSUE
    read_controller_reg_before_active();
    #endif
}

#if THERMAL_HW_PROTECT
static void lvts_config_all_tc_hw_protect(int temperature, int temperature2)
{
    unsigned int i = 0;
    int wd_api_ret;
    struct wd_api *wd_api;

    THERM_DLOG("===== %s begin ======\n", __func__);
    THERM_LOG("%s,temperature=%d,temperature2=%d,\n", __func__
        temperature, temperature2);

    /* spend 860~1463 us */
    /* Thermal need to config to direct reset mode
     * this API provide by Weiqi Fu(RGU SW owner).
     */

    /* The way of setting hardware protection is different than Android* load. */
    //xxx, mtk_wdt_request_en_set(MTK_WDT_REQ_MODE_THERMAL, WD_REQ_EN);

    /* Move thermal HW protection ahead... */
    for (i = 0; i < countof(lvts_tscpu_g_tc); i++)
        lvts_set_tc_trigger_hw_protect(temperature, temperature2, i);

    mtk_wdt_request_mode_set(MTK_WDT_REQ_MODE_THERMAL, WD_REQ_RST_MODE);
}
#endif

static void lvts_device_check_counting_status(int tc_num)
{
    /* Check this when LVTS device is counting for
     * a temperature or a RC now
     */

    int offset, cnt;

    offset = lvts_tscpu_g_tc[tc_num].tc_offset; //tc offset

    cnt = 0;
    while (BIT(readl(LVTS_CONFIG_0 + offset), 25)) {
        cnt++;

        if (cnt == 100) {
            THERM_LOG("Error: DEVICE_SENSING_STATUS didn't ready\n");
            break;
        }
        spin(2);
    }
}


#if DUMP_LVTS_REGISTER
static void read_device_reg_before_active(void)
{
    int i, j;
    unsigned int addr, data;

    for (i = 0; i < countof(lvts_tscpu_g_tc); i++) {
        for (j = 0; j < NUM_LVTS_DEVICE_REG; j++) {
            addr = g_lvts_device_addrs[j];
            data =  lvts_read_device(0x81020000, addr, i);
            g_lvts_device_value_b[i][j] = data;
        }
    }
}
#endif

static void lvts_device_read_count_RC_N(void)
{
    /* Resistor-Capacitor Calibration */
    /* count_RC_N: count RC now */
    int k,  j, offset, num_ts, s_index;
    unsigned int data, i;
    char buffer[512];

#if LVTS_REFINE_MANUAL_RCK_WITH_EFUSE
    unsigned int  rc_data;
    int refine_data_idx[4] = {0};
    /*
     * comare count_rc_now with efuse.
     * > 6%, use efuse RC instead of count_rc_now
     * < 6%, keep count_rc_now value
     */
    int count_rc_delta = 0;
#endif


    for (i = 0; i < countof(lvts_tscpu_g_tc); i++) {
        offset = lvts_tscpu_g_tc[i].tc_offset;
        num_ts = lvts_tscpu_g_tc[i].ts_number;

        /* Set LVTS Manual-RCK operation */
        lvts_write_device(0xC1030000, 0x0E, 0x00, i);

#if LVTS_REFINE_MANUAL_RCK_WITH_EFUSE
        /*
         * set 0xff to clear refine_data_idx
         * in each thermal controller,
         * max sensor num = 4
         */
        for (k = 0; k < 4; k++)
            refine_data_idx[k] = 0xff;
#endif

        for (j = 0; j < num_ts; j++) {
            s_index = lvts_tscpu_g_tc[i].ts[j];

            /* Select sensor-N with RCK */
            lvts_write_device(0xC1030000, 0x0D, j, i);
            /* Set Device Low-Power Single mode */
            lvts_write_device(0xC1030000, 0x06, 0xf8, i);
            /* Set Device Counting windows */
            lvts_write_device(0xC1030000, 0x05, 0x00, i);
            /* Set Device Counting windows */
            lvts_write_device(0xC1030000, 0x04, 0x20, i);
            /* Wait 8us for device settle + 2us buffer*/
            spin(10);
            /* Kick-off RCK counting */
            lvts_write_device(0xC1030000, 0x03, 0x02, i);

            /* wait 40us + 10us buffer */
            spin(50);

            lvts_device_check_counting_status(i);

            /* Get RCK count data (sensor-N) */
            data = lvts_read_device(0xC1020000, 0x00, i);
            /* wait 2us + 3us buffer*/
            spin(5);

#if LVTS_REFINE_MANUAL_RCK_WITH_EFUSE
            rc_data = (data & _BITMASK_(23:0));

            THERM_DLOG("rc_data: 0x%x\n", rc_data);

            /*
             * if count rc now = 0 , use efuse rck insead of
             * count_rc_now
             */
            if (rc_data == 0) {
                refine_data_idx[j] = s_index;
                THERM_LOG("rc_data %d, data_idx[%d]=%d\n",
                rc_data, j, s_index);
            } else {
                if (g_count_rc[i] > rc_data)
                    count_rc_delta =
                        (g_count_rc[i] * 1000) / rc_data;
                else
                    count_rc_delta =
                        (rc_data * 1000) / g_count_rc[i];
                    /*
                     * if delta > 6%, use efuse rck insead of
                     * count_rc_now
                     */
                    THERM_DLOG("- rc_data %d, s_index=%d delta=%d",
                        rc_data, s_index, count_rc_delta);

                if (count_rc_delta > 1061) {
                    refine_data_idx[j] = s_index;
                    THERM_LOG(
                        "delta %d, data_idx[%d]=%d\n",
                        count_rc_delta, j, s_index);
                }
            }

            //lvts_printk("i=%d, j=%d, s_index=%d, rc_data=%d\n",
            //  i, j, s_index, rc_data);
            //lvts_printk("(g_count_rc[i]*1000)=%d, rc_delta=%d\n",
            //  (g_count_rc[i]*1000), count_rc_delta);

#endif

            /* Get RCK value from LSB[23:0] */
            g_count_rc_now[s_index] = (data & _BITMASK_(23:0));
        }

#if LVTS_REFINE_MANUAL_RCK_WITH_EFUSE
        for (j = 0; j < num_ts; j++) {
            if (refine_data_idx[j] != 0xff) {
                 g_count_rc_now[refine_data_idx[j]] = g_count_rc[i];
                 THERM_LOG("refine_data_idx[%d]=%d\n",
                     j, refine_data_idx[j]);
            }
        }
#endif

        /* Recover Setting for Normal Access on
         * temperature fetch
         */
        /* Select Sensor-N without RCK */
        lvts_write_device(0x81030000, 0x0D, 0x10, i);
    }

    offset = snprintf(buffer, sizeof(buffer), "[COUNT_RC_NOW] ");
    for (i = 0; i < L_TS_LVTS_NUM; i++) {

        if (((sizeof(buffer) - offset) <= 0) || (offset < 0)) {
            THERM_LOG("%s %d error\n", __func__, __LINE__);
            break;
        }

        offset += snprintf(buffer + offset, (sizeof(buffer) - offset), "%d:%d ",
            i, g_count_rc_now[i]);
    }

    THERM_LOG("%s\n", buffer);

    #if DUMP_LVTS_REGISTER_FOR_ZERO_RAW_ISSUE
    read_device_reg_before_active();
    #endif
}

static void lvts_device_enable_auto_rck(void)
{
    unsigned int i;

    THERM_DLOG("===== %s begin ======\n", __func__);

    for (i = 0; i < countof(lvts_tscpu_g_tc); i++) {
        /* Set LVTS AUTO-RCK operation */
        lvts_write_device(0xC1030000, 0x0E, 0x01, i);
    }
}

static int lvts_tscpu_max_temperature(void)
{
    unsigned int i;
    int j;
    int max = 0;

    THERM_DLOG("===== %s begin ======\n", __func__);
    for (i = 0; i < countof(lvts_tscpu_g_tc); i++) {
        for (j = 0; j < lvts_tscpu_g_tc[i].ts_number; j++) {
            if (i == 0 && j == 0) {
                max = tscpu_ts_lvts_temp[lvts_tscpu_g_tc[i].ts[j]];
            } else {
                if (max < tscpu_ts_lvts_temp[lvts_tscpu_g_tc[i].ts[j]])
                        max = tscpu_ts_lvts_temp[lvts_tscpu_g_tc[i].ts[j]];
            }
        }
    }
    return max;
}

static void lvts_print_mcu_temp(void)
{
    int i;

    //THERM_LOG("===== %s begin ======\n", __func__);
    for (i = 0; i < L_TS_LVTS_NUM; i++)
        THERM_LOG("LVTS%d=%d,", (i), tscpu_ts_lvts_temp[i]);

    THERM_DLOG("\n");
}

static void lvts_tscpu_reset_thermal(void)
{
    /* chip dependent, Have to confirm with DE */

    int temp = 0;
    int temp2 = 0;

    THERM_DLOG("%s\n", __func__);

    /* reset AP thremal ctrl */
    /* TODO: Is it necessary to read INFRA_GLOBALCON_RST_0_SET? */
    temp = readl(INFRA_GLOBALCON_RST_0_SET);

    /* 1: Enables thermal control software reset */
    temp |= 0x800000;
    mt_reg_sync_writel_print(temp, (void *)(INFRA_GLOBALCON_RST_0_SET));

    /* TODO: How long to set the reset bit? */

    /* un reset */
    /* TODO: Is it necessary to read INFRA_GLOBALCON_RST_0_CLR? */
    temp = readl(INFRA_GLOBALCON_RST_0_CLR);

    /* 1: Enable reset Disables thermal control software reset */
    temp |= 0x800000;
    mt_reg_sync_writel_print(temp, (void *)(INFRA_GLOBALCON_RST_0_CLR));

    /* reset MCU thremal ctrl */
    /* TODO: Is it necessary to read INFRA_GLOBALCON_RST_0_SET? */
    temp2 = readl(INFRA_GLOBALCON_RST_4_SET);

    /* 1: Enables thermal control software reset */
    temp2 |= 0x00001000;
    mt_reg_sync_writel_print(temp2, (void *)(INFRA_GLOBALCON_RST_4_SET));

    /* TODO: How long to set the reset bit? */

    /* un reset */
    /* TODO: Is it necessary to read INFRA_GLOBALCON_RST_0_CLR? */
    temp2 = readl(INFRA_GLOBALCON_RST_4_CLR);

    /* 1: Enable reset Disables thermal control software reset */
    temp2 |= 0x00001000;

    mt_reg_sync_writel_print(temp2, (void *)(INFRA_GLOBALCON_RST_4_CLR));
}

static void lvts_thermal_init(uint level)
{

    THERM_LOG("===== %s begin ======\n", __func__);
    PROFILING_START("lvts_thermal_init");

    lvts_tscpu_reset_thermal();

    lvts_thermal_cal_prepare();
    lvts_device_identification();
    lvts_Device_Enable_Init_all_Devices();
#if LVTS_DEVICE_AUTO_RCK == 0
    lvts_device_read_count_RC_N();
#else
    lvts_device_enable_auto_rck();
#endif
    lvts_efuse_setting();

    lvts_disable_all_sensing_points();
    lvts_tscpu_thermal_initial_all_tc();
    lvts_enable_all_sensing_points();

    lvts_set_init_flag();

#if THERMAL_HW_PROTECT
    lvts_config_all_tc_hw_protect(THERMAL_HARDWARE_RESET_POINT, 60000);
#endif
    PROFILING_END();
    THERM_LOG("thermal_init: thermal initialized\n");

}

MT_LK_INIT_HOOK_BL2_EXT(lvts_thermal, lvts_thermal_init, LK_INIT_LEVEL_PLATFORM);


