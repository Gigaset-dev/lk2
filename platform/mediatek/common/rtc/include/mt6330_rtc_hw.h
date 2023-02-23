/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */


#pragma once

#define MT6330_SCK_TOP_CON0_H                   (0x50d)
#define MT6330_RTC_ANA_ID                       (0x580)
#define MT6330_RTC_BBPU_L                       (0x588)
#define MT6330_RTC_BBPU_H                       (0x589)
#define MT6330_RTC_IRQ_STA                      (0x58a)
#define MT6330_RTC_IRQ_EN                       (0x58c)
#define MT6330_RTC_CII_EN_L                     (0x58e)
#define MT6330_RTC_CII_EN_H                     (0x58f)
#define MT6330_RTC_AL_MASK                      (0x590)
#define MT6330_RTC_TC_SEC                       (0x592)
#define MT6330_RTC_TC_MIN                       (0x594)
#define MT6330_RTC_TC_HOU                       (0x596)
#define MT6330_RTC_TC_DOM                       (0x598)
#define MT6330_RTC_TC_DOW                       (0x59a)
#define MT6330_RTC_TC_MTH_L                     (0x59c)
#define MT6330_RTC_TC_MTH_H                     (0x59d)
#define MT6330_RTC_TC_YEA                       (0x59e)
#define MT6330_RTC_AL_SEC_L                     (0x5a0)
#define MT6330_RTC_AL_SEC_H                     (0x5a1)
#define MT6330_RTC_AL_MIN                       (0x5a2)
#define MT6330_RTC_AL_HOU_L                     (0x5a4)
#define MT6330_RTC_AL_HOU_H                     (0x5a5)
#define MT6330_RTC_AL_DOM_L                     (0x5a6)
#define MT6330_RTC_AL_DOM_H                     (0x5a7)
#define MT6330_RTC_AL_DOW_L                     (0x5a8)
#define MT6330_RTC_AL_DOW_H                     (0x5a9)
#define MT6330_RTC_AL_MTH_L                     (0x5aa)
#define MT6330_RTC_AL_MTH_H                     (0x5ab)
#define MT6330_RTC_AL_YEA_L                     (0x5ac)
#define MT6330_RTC_AL_YEA_H                     (0x5ad)
#define MT6330_RTC_OSC32CON_L                   (0x5ae)
#define MT6330_RTC_OSC32CON_H                   (0x5af)
#define MT6330_RTC_POWERKEY1_L                  (0x5b0)
#define MT6330_RTC_POWERKEY1_H                  (0x5b1)
#define MT6330_RTC_POWERKEY2_L                  (0x5b2)
#define MT6330_RTC_POWERKEY2_H                  (0x5b3)
#define MT6330_RTC_PDN1_L                       (0x5b4)
#define MT6330_RTC_PDN1_H                       (0x5b5)
#define MT6330_RTC_PDN2_L                       (0x5b6)
#define MT6330_RTC_PDN2_H                       (0x5b7)
#define MT6330_RTC_SPAR0_L                      (0x5b8)
#define MT6330_RTC_SPAR0_H                      (0x5b9)
#define MT6330_RTC_SPAR1_L                      (0x5ba)
#define MT6330_RTC_SPAR1_H                      (0x5bb)
#define MT6330_RTC_PROT_L                       (0x5bc)
#define MT6330_RTC_PROT_H                       (0x5bd)
#define MT6330_RTC_DIFF_L                       (0x5be)
#define MT6330_RTC_DIFF_H                       (0x5bf)
#define MT6330_RTC_CALI_L                       (0x5c0)
#define MT6330_RTC_CALI_H                       (0x5c1)
#define MT6330_RTC_WRTGR                        (0x5c2)
#define MT6330_RTC_CON_L                        (0x5c4)
#define MT6330_RTC_CON_H                        (0x5c5)
#define MT6330_RTC_SEC_CTRL                     (0x5c6)
#define MT6330_RTC_INT_CNT_L                    (0x5c8)
#define MT6330_RTC_INT_CNT_H                    (0x5c9)
#define MT6330_RTC_SEC_ANA_ID                   (0x600)
#define MT6330_RTC_SEC_WRTGR                    (0x618)

#define PMIC_BBPU_ADDR                          MT6330_RTC_BBPU_L
#define PMIC_BBPU_MASK                          0xF
#define PMIC_BBPU_SHIFT                         0
#define PMIC_CLRPKY_ADDR                        MT6330_RTC_BBPU_L
#define PMIC_CLRPKY_MASK                        0x1
#define PMIC_CLRPKY_SHIFT                       4
#define PMIC_RELOAD_ADDR                        MT6330_RTC_BBPU_L
#define PMIC_RELOAD_MASK                        0x1
#define PMIC_RELOAD_SHIFT                       5
#define PMIC_CBUSY_ADDR                         MT6330_RTC_BBPU_L
#define PMIC_CBUSY_MASK                         0x1
#define PMIC_CBUSY_SHIFT                        6
#define PMIC_ALARM_STATUS_ADDR                  MT6330_RTC_BBPU_L
#define PMIC_ALARM_STATUS_MASK                  0x1
#define PMIC_ALARM_STATUS_SHIFT                 7
#define PMIC_KEY_BBPU_ADDR                      MT6330_RTC_BBPU_L
#define PMIC_KEY_BBPU_MASK                      0xFF
#define PMIC_KEY_BBPU_SHIFT                     8
#define PMIC_ALSTA_ADDR                         MT6330_RTC_IRQ_STA
#define PMIC_ALSTA_MASK                         0x1
#define PMIC_ALSTA_SHIFT                        0
#define PMIC_TCSTA_ADDR                         MT6330_RTC_IRQ_STA
#define PMIC_TCSTA_MASK                         0x1
#define PMIC_TCSTA_SHIFT                        1
#define PMIC_LPSTA_ADDR                         MT6330_RTC_IRQ_STA
#define PMIC_LPSTA_MASK                         0x1
#define PMIC_LPSTA_SHIFT                        3
#define PMIC_AL_EN_ADDR                         MT6330_RTC_IRQ_EN
#define PMIC_AL_EN_MASK                         0x1
#define PMIC_AL_EN_SHIFT                        0
#define PMIC_TC_EN_ADDR                         MT6330_RTC_IRQ_EN
#define PMIC_TC_EN_MASK                         0x1
#define PMIC_TC_EN_SHIFT                        1
#define PMIC_ONESHOT_ADDR                       MT6330_RTC_IRQ_EN
#define PMIC_ONESHOT_MASK                       0x1
#define PMIC_ONESHOT_SHIFT                      2
#define PMIC_LP_EN_ADDR                         MT6330_RTC_IRQ_EN
#define PMIC_LP_EN_MASK                         0x1
#define PMIC_LP_EN_SHIFT                        3
#define PMIC_SECCII_ADDR                        MT6330_RTC_CII_EN_L
#define PMIC_SECCII_MASK                        0x1
#define PMIC_SECCII_SHIFT                       0
#define PMIC_MINCII_ADDR                        MT6330_RTC_CII_EN_L
#define PMIC_MINCII_MASK                        0x1
#define PMIC_MINCII_SHIFT                       1
#define PMIC_HOUCII_ADDR                        MT6330_RTC_CII_EN_L
#define PMIC_HOUCII_MASK                        0x1
#define PMIC_HOUCII_SHIFT                       2
#define PMIC_DOMCII_ADDR                        MT6330_RTC_CII_EN_L
#define PMIC_DOMCII_MASK                        0x1
#define PMIC_DOMCII_SHIFT                       3
#define PMIC_DOWCII_ADDR                        MT6330_RTC_CII_EN_L
#define PMIC_DOWCII_MASK                        0x1
#define PMIC_DOWCII_SHIFT                       4
#define PMIC_MTHCII_ADDR                        MT6330_RTC_CII_EN_L
#define PMIC_MTHCII_MASK                        0x1
#define PMIC_MTHCII_SHIFT                       5
#define PMIC_YEACII_ADDR                        MT6330_RTC_CII_EN_L
#define PMIC_YEACII_MASK                        0x1
#define PMIC_YEACII_SHIFT                       6
#define PMIC_SECCII_1_2_ADDR                    MT6330_RTC_CII_EN_L
#define PMIC_SECCII_1_2_MASK                    0x1
#define PMIC_SECCII_1_2_SHIFT                   7
#define PMIC_SECCII_1_4_ADDR                    MT6330_RTC_CII_EN_L
#define PMIC_SECCII_1_4_MASK                    0x1
#define PMIC_SECCII_1_4_SHIFT                   8
#define PMIC_SECCII_1_8_ADDR                    MT6330_RTC_CII_EN_L
#define PMIC_SECCII_1_8_MASK                    0x1
#define PMIC_SECCII_1_8_SHIFT                   9
#define PMIC_SEC_MSK_ADDR                       MT6330_RTC_AL_MASK
#define PMIC_SEC_MSK_MASK                       0x1
#define PMIC_SEC_MSK_SHIFT                      0
#define PMIC_MIN_MSK_ADDR                       MT6330_RTC_AL_MASK
#define PMIC_MIN_MSK_MASK                       0x1
#define PMIC_MIN_MSK_SHIFT                      1
#define PMIC_HOU_MSK_ADDR                       MT6330_RTC_AL_MASK
#define PMIC_HOU_MSK_MASK                       0x1
#define PMIC_HOU_MSK_SHIFT                      2
#define PMIC_DOM_MSK_ADDR                       MT6330_RTC_AL_MASK
#define PMIC_DOM_MSK_MASK                       0x1
#define PMIC_DOM_MSK_SHIFT                      3
#define PMIC_DOW_MSK_ADDR                       MT6330_RTC_AL_MASK
#define PMIC_DOW_MSK_MASK                       0x1
#define PMIC_DOW_MSK_SHIFT                      4
#define PMIC_MTH_MSK_ADDR                       MT6330_RTC_AL_MASK
#define PMIC_MTH_MSK_MASK                       0x1
#define PMIC_MTH_MSK_SHIFT                      5
#define PMIC_YEA_MSK_ADDR                       MT6330_RTC_AL_MASK
#define PMIC_YEA_MSK_MASK                       0x1
#define PMIC_YEA_MSK_SHIFT                      6
#define PMIC_TC_SECOND_ADDR                     MT6330_RTC_TC_SEC
#define PMIC_TC_SECOND_MASK                     0x3F
#define PMIC_TC_SECOND_SHIFT                    0
#define PMIC_TC_MINUTE_ADDR                     MT6330_RTC_TC_MIN
#define PMIC_TC_MINUTE_MASK                     0x3F
#define PMIC_TC_MINUTE_SHIFT                    0
#define PMIC_TC_HOUR_ADDR                       MT6330_RTC_TC_HOU
#define PMIC_TC_HOUR_MASK                       0x1F
#define PMIC_TC_HOUR_SHIFT                      0
#define PMIC_TC_DOM_ADDR                        MT6330_RTC_TC_DOM
#define PMIC_TC_DOM_MASK                        0x1F
#define PMIC_TC_DOM_SHIFT                       0
#define PMIC_TC_DOW_ADDR                        MT6330_RTC_TC_DOW
#define PMIC_TC_DOW_MASK                        0x7
#define PMIC_TC_DOW_SHIFT                       0
#define PMIC_TC_MONTH_ADDR                      MT6330_RTC_TC_MTH_L
#define PMIC_TC_MONTH_MASK                      0xF
#define PMIC_TC_MONTH_SHIFT                     0
#define PMIC_RTC_MACRO_ID_ADDR                  MT6330_RTC_TC_MTH_L
#define PMIC_RTC_MACRO_ID_MASK                  0xFFF
#define PMIC_RTC_MACRO_ID_SHIFT                 4
#define PMIC_TC_YEAR_ADDR                       MT6330_RTC_TC_YEA
#define PMIC_TC_YEAR_MASK                       0x7F
#define PMIC_TC_YEAR_SHIFT                      0
#define PMIC_AL_SECOND_ADDR                     MT6330_RTC_AL_SEC_L
#define PMIC_AL_SECOND_MASK                     0x3F
#define PMIC_AL_SECOND_SHIFT                    0
#define PMIC_BBPU_AUTO_PDN_SEL_ADDR             MT6330_RTC_AL_SEC_L
#define PMIC_BBPU_AUTO_PDN_SEL_MASK             0x1
#define PMIC_BBPU_AUTO_PDN_SEL_SHIFT            6
#define PMIC_BBPU_2SEC_CK_SEL_ADDR              MT6330_RTC_AL_SEC_L
#define PMIC_BBPU_2SEC_CK_SEL_MASK              0x1
#define PMIC_BBPU_2SEC_CK_SEL_SHIFT             7
#define PMIC_BBPU_2SEC_EN_ADDR                  MT6330_RTC_AL_SEC_L
#define PMIC_BBPU_2SEC_EN_MASK                  0x1
#define PMIC_BBPU_2SEC_EN_SHIFT                 8
#define PMIC_BBPU_2SEC_MODE_ADDR                MT6330_RTC_AL_SEC_L
#define PMIC_BBPU_2SEC_MODE_MASK                0x3
#define PMIC_BBPU_2SEC_MODE_SHIFT               9
#define PMIC_BBPU_2SEC_STAT_CLEAR_ADDR          MT6330_RTC_AL_SEC_L
#define PMIC_BBPU_2SEC_STAT_CLEAR_MASK          0x1
#define PMIC_BBPU_2SEC_STAT_CLEAR_SHIFT         11
#define PMIC_BBPU_2SEC_STAT_STA_ADDR            MT6330_RTC_AL_SEC_L
#define PMIC_BBPU_2SEC_STAT_STA_MASK            0x1
#define PMIC_BBPU_2SEC_STAT_STA_SHIFT           12
#define PMIC_RTC_LPD_OPT_ADDR                   MT6330_RTC_AL_SEC_L
#define PMIC_RTC_LPD_OPT_MASK                   0x3
#define PMIC_RTC_LPD_OPT_SHIFT                  13
#define PMIC_K_EOSC32_VTCXO_ON_SEL_ADDR         MT6330_RTC_AL_SEC_L
#define PMIC_K_EOSC32_VTCXO_ON_SEL_MASK         0x1
#define PMIC_K_EOSC32_VTCXO_ON_SEL_SHIFT        15
#define PMIC_AL_MINUTE_ADDR                     MT6330_RTC_AL_MIN
#define PMIC_AL_MINUTE_MASK                     0x3F
#define PMIC_AL_MINUTE_SHIFT                    0
#define PMIC_AL_HOUR_ADDR                       MT6330_RTC_AL_HOU_L
#define PMIC_AL_HOUR_MASK                       0x1F
#define PMIC_AL_HOUR_SHIFT                      0
#define PMIC_NEW_SPARE0_ADDR                    MT6330_RTC_AL_HOU_L
#define PMIC_NEW_SPARE0_MASK                    0xFF
#define PMIC_NEW_SPARE0_SHIFT                   8
#define PMIC_AL_DOM_ADDR                        MT6330_RTC_AL_DOM_L
#define PMIC_AL_DOM_MASK                        0x1F
#define PMIC_AL_DOM_SHIFT                       0
#define PMIC_NEW_SPARE1_ADDR                    MT6330_RTC_AL_DOM_L
#define PMIC_NEW_SPARE1_MASK                    0xFF
#define PMIC_NEW_SPARE1_SHIFT                   8
#define PMIC_AL_DOW_ADDR                        MT6330_RTC_AL_DOW_L
#define PMIC_AL_DOW_MASK                        0x7
#define PMIC_AL_DOW_SHIFT                       0
#define PMIC_RG_EOSC_CALI_TD_ADDR               MT6330_RTC_AL_DOW_L
#define PMIC_RG_EOSC_CALI_TD_MASK               0x7
#define PMIC_RG_EOSC_CALI_TD_SHIFT              8
#define PMIC_NEW_SPARE2_ADDR                    MT6330_RTC_AL_DOW_L
#define PMIC_NEW_SPARE2_MASK                    0x1F
#define PMIC_NEW_SPARE2_SHIFT                   11
#define PMIC_AL_MONTH_ADDR                      MT6330_RTC_AL_MTH_L
#define PMIC_AL_MONTH_MASK                      0xF
#define PMIC_AL_MONTH_SHIFT                     0
#define PMIC_NEW_SPARE3_ADDR                    MT6330_RTC_AL_MTH_L
#define PMIC_NEW_SPARE3_MASK                    0xFF
#define PMIC_NEW_SPARE3_SHIFT                   8
#define PMIC_AL_YEAR_ADDR                       MT6330_RTC_AL_YEA_L
#define PMIC_AL_YEAR_MASK                       0x7F
#define PMIC_AL_YEAR_SHIFT                      0
#define PMIC_RTC_K_EOSC_RSV_ADDR                MT6330_RTC_AL_YEA_L
#define PMIC_RTC_K_EOSC_RSV_MASK                0xFF
#define PMIC_RTC_K_EOSC_RSV_SHIFT               8
#define PMIC_XOSCCALI_ADDR                      MT6330_RTC_OSC32CON_L
#define PMIC_XOSCCALI_MASK                      0x1F
#define PMIC_XOSCCALI_SHIFT                     0
#define PMIC_RTC_XOSC32_ENB_ADDR                MT6330_RTC_OSC32CON_L
#define PMIC_RTC_XOSC32_ENB_MASK                0x1
#define PMIC_RTC_XOSC32_ENB_SHIFT               5
#define PMIC_RTC_EMBCK_SEL_MODE_ADDR            MT6330_RTC_OSC32CON_L
#define PMIC_RTC_EMBCK_SEL_MODE_MASK            0x3
#define PMIC_RTC_EMBCK_SEL_MODE_SHIFT           6
#define PMIC_RTC_EMBCK_SRC_SEL_ADDR             MT6330_RTC_OSC32CON_L
#define PMIC_RTC_EMBCK_SRC_SEL_MASK             0x1
#define PMIC_RTC_EMBCK_SRC_SEL_SHIFT            8
#define PMIC_RTC_EMBCK_SEL_OPTION_ADDR          MT6330_RTC_OSC32CON_L
#define PMIC_RTC_EMBCK_SEL_OPTION_MASK          0x1
#define PMIC_RTC_EMBCK_SEL_OPTION_SHIFT         9
#define PMIC_RTC_GPS_CKOUT_EN_ADDR              MT6330_RTC_OSC32CON_L
#define PMIC_RTC_GPS_CKOUT_EN_MASK              0x1
#define PMIC_RTC_GPS_CKOUT_EN_SHIFT             10
#define PMIC_RTC_EOSC32_VCT_EN_ADDR             MT6330_RTC_OSC32CON_L
#define PMIC_RTC_EOSC32_VCT_EN_MASK             0x1
#define PMIC_RTC_EOSC32_VCT_EN_SHIFT            11
#define PMIC_RTC_EOSC32_CHOP_EN_ADDR            MT6330_RTC_OSC32CON_L
#define PMIC_RTC_EOSC32_CHOP_EN_MASK            0x1
#define PMIC_RTC_EOSC32_CHOP_EN_SHIFT           12
#define PMIC_RTC_GP_OSC32_CON_ADDR              MT6330_RTC_OSC32CON_L
#define PMIC_RTC_GP_OSC32_CON_MASK              0x3
#define PMIC_RTC_GP_OSC32_CON_SHIFT             13
#define PMIC_RTC_REG_XOSC32_ENB_ADDR            MT6330_RTC_OSC32CON_L
#define PMIC_RTC_REG_XOSC32_ENB_MASK            0x1
#define PMIC_RTC_REG_XOSC32_ENB_SHIFT           15
#define PMIC_RTC_POWERKEY1_ADDR                 MT6330_RTC_POWERKEY1_L
#define PMIC_RTC_POWERKEY1_MASK                 0xFFFF
#define PMIC_RTC_POWERKEY1_SHIFT                0
#define PMIC_RTC_POWERKEY2_ADDR                 MT6330_RTC_POWERKEY2_L
#define PMIC_RTC_POWERKEY2_MASK                 0xFFFF
#define PMIC_RTC_POWERKEY2_SHIFT                0
#define PMIC_RTC_PDN1_ADDR                      MT6330_RTC_PDN1_L
#define PMIC_RTC_PDN1_MASK                      0xFFFF
#define PMIC_RTC_PDN1_SHIFT                     0
#define PMIC_RTC_PDN2_ADDR                      MT6330_RTC_PDN2_L
#define PMIC_RTC_PDN2_MASK                      0xFFFF
#define PMIC_RTC_PDN2_SHIFT                     0
#define PMIC_RTC_SPAR0_ADDR                     MT6330_RTC_SPAR0_L
#define PMIC_RTC_SPAR0_MASK                     0xFFFF
#define PMIC_RTC_SPAR0_SHIFT                    0
#define PMIC_RTC_SPAR1_ADDR                     MT6330_RTC_SPAR1_L
#define PMIC_RTC_SPAR1_MASK                     0xFFFF
#define PMIC_RTC_SPAR1_SHIFT                    0
#define PMIC_RTC_PROT_ADDR                      MT6330_RTC_PROT_L
#define PMIC_RTC_PROT_MASK                      0xFFFF
#define PMIC_RTC_PROT_SHIFT                     0
#define PMIC_RTC_DIFF_ADDR                      MT6330_RTC_DIFF_L
#define PMIC_RTC_DIFF_MASK                      0xFFF
#define PMIC_RTC_DIFF_SHIFT                     0
#define PMIC_POWER_DETECTED_ADDR                MT6330_RTC_DIFF_L
#define PMIC_POWER_DETECTED_MASK                0x1
#define PMIC_POWER_DETECTED_SHIFT               12
#define PMIC_K_EOSC32_RSV_ADDR                  MT6330_RTC_DIFF_L
#define PMIC_K_EOSC32_RSV_MASK                  0x1
#define PMIC_K_EOSC32_RSV_SHIFT                 14
#define PMIC_CALI_RD_SEL_ADDR                   MT6330_RTC_DIFF_L
#define PMIC_CALI_RD_SEL_MASK                   0x1
#define PMIC_CALI_RD_SEL_SHIFT                  15
#define PMIC_RTC_CALI_ADDR                      MT6330_RTC_CALI_L
#define PMIC_RTC_CALI_MASK                      0x3FFF
#define PMIC_RTC_CALI_SHIFT                     0
#define PMIC_CALI_WR_SEL_ADDR                   MT6330_RTC_CALI_L
#define PMIC_CALI_WR_SEL_MASK                   0x1
#define PMIC_CALI_WR_SEL_SHIFT                  14
#define PMIC_K_EOSC32_OVERFLOW_ADDR             MT6330_RTC_CALI_L
#define PMIC_K_EOSC32_OVERFLOW_MASK             0x1
#define PMIC_K_EOSC32_OVERFLOW_SHIFT            15
#define PMIC_WRTGR_ADDR                         MT6330_RTC_WRTGR
#define PMIC_WRTGR_MASK                         0x1
#define PMIC_WRTGR_SHIFT                        0
#define PMIC_VBAT_LPSTA_RAW_ADDR                MT6330_RTC_CON_L
#define PMIC_VBAT_LPSTA_RAW_MASK                0x1
#define PMIC_VBAT_LPSTA_RAW_SHIFT               0
#define PMIC_EOSC32_LPEN_ADDR                   MT6330_RTC_CON_L
#define PMIC_EOSC32_LPEN_MASK                   0x1
#define PMIC_EOSC32_LPEN_SHIFT                  1
#define PMIC_XOSC32_LPEN_ADDR                   MT6330_RTC_CON_L
#define PMIC_XOSC32_LPEN_MASK                   0x1
#define PMIC_XOSC32_LPEN_SHIFT                  2
#define PMIC_LPRST_ADDR                         MT6330_RTC_CON_L
#define PMIC_LPRST_MASK                         0x1
#define PMIC_LPRST_SHIFT                        3
#define PMIC_CDBO_ADDR                          MT6330_RTC_CON_L
#define PMIC_CDBO_MASK                          0x1
#define PMIC_CDBO_SHIFT                         4
#define PMIC_F32KOB_ADDR                        MT6330_RTC_CON_L
#define PMIC_F32KOB_MASK                        0x1
#define PMIC_F32KOB_SHIFT                       5
#define PMIC_GPO_ADDR                           MT6330_RTC_CON_L
#define PMIC_GPO_MASK                           0x1
#define PMIC_GPO_SHIFT                          6
#define PMIC_GOE_ADDR                           MT6330_RTC_CON_L
#define PMIC_GOE_MASK                           0x1
#define PMIC_GOE_SHIFT                          7
#define PMIC_GSR_ADDR                           MT6330_RTC_CON_L
#define PMIC_GSR_MASK                           0x1
#define PMIC_GSR_SHIFT                          8
#define PMIC_GSMT_ADDR                          MT6330_RTC_CON_L
#define PMIC_GSMT_MASK                          0x1
#define PMIC_GSMT_SHIFT                         9
#define PMIC_GPEN_ADDR                          MT6330_RTC_CON_L
#define PMIC_GPEN_MASK                          0x1
#define PMIC_GPEN_SHIFT                         10
#define PMIC_GPU_ADDR                           MT6330_RTC_CON_L
#define PMIC_GPU_MASK                           0x1
#define PMIC_GPU_SHIFT                          11
#define PMIC_GE4_ADDR                           MT6330_RTC_CON_L
#define PMIC_GE4_MASK                           0x1
#define PMIC_GE4_SHIFT                          12
#define PMIC_GE8_ADDR                           MT6330_RTC_CON_L
#define PMIC_GE8_MASK                           0x1
#define PMIC_GE8_SHIFT                          13
#define PMIC_GPI_ADDR                           MT6330_RTC_CON_L
#define PMIC_GPI_MASK                           0x1
#define PMIC_GPI_SHIFT                          14
#define PMIC_LPSTA_RAW_ADDR                     MT6330_RTC_CON_L
#define PMIC_LPSTA_RAW_MASK                     0x1
#define PMIC_LPSTA_RAW_SHIFT                    15
#define PMIC_DAT0_LOCK_ADDR                     MT6330_RTC_SEC_CTRL
#define PMIC_DAT0_LOCK_MASK                     0x1
#define PMIC_DAT0_LOCK_SHIFT                    0
#define PMIC_DAT1_LOCK_ADDR                     MT6330_RTC_SEC_CTRL
#define PMIC_DAT1_LOCK_MASK                     0x1
#define PMIC_DAT1_LOCK_SHIFT                    1
#define PMIC_DAT2_LOCK_ADDR                     MT6330_RTC_SEC_CTRL
#define PMIC_DAT2_LOCK_MASK                     0x1
#define PMIC_DAT2_LOCK_SHIFT                    2
#define PMIC_RTC_INT_CNT_ADDR                   MT6330_RTC_INT_CNT_L
#define PMIC_RTC_INT_CNT_MASK                   0x7FFF
#define PMIC_RTC_INT_CNT_SHIFT                  0


/*
 * RTC_PDN1:
 *     bit 0 - 5  : reserved bits
 *     bit 6      : Bypass PWRKEY bit
 *     bit 7      : Power-On Time bit
 *     bit 8 - 13 : reserved bits
 *     bit 14     : Kernel Power Off Charging
 *     bit 15     : reserved bits
 */
#define RTC_BYPASS_PWR_SHIFT                    6
#define RTC_PWRON_TIME_SHIFT                    7
#define RTC_KPOC_SHIFT                          14

/*
 * RTC_PDN2:
 *     bit 0 - 3 : MTH in power-on time
 *     bit 4     : Power-On Alarm bit
 *     bit 5 - 7 : reserved bits
 *     bit 8 - 14: YEA in power-on time
 *     bit 15    : Power-On Logo bit
 */

#define RTC_PWRON_MTH_MASK                      0xf
#define RTC_PWRON_MTH_SHIFT                     0
#define RTC_PWRON_ALARM_SHIFT                   4
#define RTC_PWRON_YEA_MASK                      0x7f
#define RTC_PWRON_YEA_SHIFT                     8
#define RTC_PWRON_LOGO_SHIFT                    15

/*
 * RTC_SPAR0:
 *     bit 0 - 5 : SEC in power-on time
 *     bit 6     : 32K less bit. True:with 32K, False:Without 32K
 *     bit 7     : enable lp detect
 *     bit 8 - 15: reserved bits
 */

#define RTC_PWRON_SEC_MASK                      0x3f
#define RTC_PWRON_SEC_SHIFT                     0
#define RTC_32K_LESS_SHIFT                      6
#define RTC_LP_DET_SHIFT                        7
/*
 * RTC_SPAR1:
 *     bit 0 - 5  : MIN in power-on time
 *     bit 6 - 10 : HOU in power-on time
 *     bit 11 - 15: DOM in power-on time
 */

#define RTC_PWRON_MIN_MASK                      0x3f
#define RTC_PWRON_MIN_SHIFT                     0
#define RTC_PWRON_HOU_MASK                      0x1f
#define RTC_PWRON_HOU_SHIFT                     6
#define RTC_PWRON_DOM_MASK                      0x1f
#define RTC_PWRON_DOM_SHIFT                     11


