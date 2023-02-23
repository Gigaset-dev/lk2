/*
 * Copyright (c) 2019 MediaTek Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


#pragma once

#define MT6359_RTC_BBPU                         0x588
#define MT6359_RTC_IRQ_STA                      0x58a
#define MT6359_RTC_IRQ_EN                       0x58c
#define MT6359_RTC_CII_EN                       0x58e
#define MT6359_RTC_AL_MASK                      0x590
#define MT6359_RTC_TC_SEC                       0x592
#define MT6359_RTC_TC_MIN                       0x594
#define MT6359_RTC_TC_HOU                       0x596
#define MT6359_RTC_TC_DOM                       0x598
#define MT6359_RTC_TC_DOW                       0x59a
#define MT6359_RTC_TC_MTH                       0x59c
#define MT6359_RTC_TC_YEA                       0x59e
#define MT6359_RTC_AL_SEC                       0x5a0
#define MT6359_RTC_AL_MIN                       0x5a2
#define MT6359_RTC_AL_HOU                       0x5a4
#define MT6359_RTC_AL_DOM                       0x5a6
#define MT6359_RTC_AL_DOW                       0x5a8
#define MT6359_RTC_AL_MTH                       0x5aa
#define MT6359_RTC_AL_YEA                       0x5ac
#define MT6359_RTC_OSC32CON                     0x5ae
#define MT6359_RTC_POWERKEY1                    0x5b0
#define MT6359_RTC_POWERKEY2                    0x5b2
#define MT6359_RTC_PDN1                         0x5b4
#define MT6359_RTC_PDN2                         0x5b6
#define MT6359_RTC_SPAR0                        0x5b8
#define MT6359_RTC_SPAR1                        0x5ba
#define MT6359_RTC_PROT                         0x5bc
#define MT6359_RTC_DIFF                         0x5be
#define MT6359_RTC_CALI                         0x5c0
#define MT6359_RTC_WRTGR                        0x5c2
#define MT6359_RTC_CON                          0x5c4
#define MT6359_RTC_SEC_CTRL                     0x5c6
#define MT6359_RTC_INT_CNT                      0x5c8


#define PMIC_BBPU_ADDR                          MT6359_RTC_BBPU
#define PMIC_BBPU_MASK                          0xF
#define PMIC_BBPU_SHIFT                         0
#define PMIC_CLRPKY_ADDR                        MT6359_RTC_BBPU
#define PMIC_CLRPKY_MASK                        0x1
#define PMIC_CLRPKY_SHIFT                       4
#define PMIC_RELOAD_ADDR                        MT6359_RTC_BBPU
#define PMIC_RELOAD_MASK                        0x1
#define PMIC_RELOAD_SHIFT                       5
#define PMIC_CBUSY_ADDR                         MT6359_RTC_BBPU
#define PMIC_CBUSY_MASK                         0x1
#define PMIC_CBUSY_SHIFT                        6
#define PMIC_ALARM_STATUS_ADDR                  MT6359_RTC_BBPU
#define PMIC_ALARM_STATUS_MASK                  0x1
#define PMIC_ALARM_STATUS_SHIFT                 7
#define PMIC_KEY_BBPU_ADDR                      MT6359_RTC_BBPU
#define PMIC_KEY_BBPU_MASK                      0xFF
#define PMIC_KEY_BBPU_SHIFT                     8
#define PMIC_ALSTA_ADDR                         MT6359_RTC_IRQ_STA
#define PMIC_ALSTA_MASK                         0x1
#define PMIC_ALSTA_SHIFT                        0
#define PMIC_TCSTA_ADDR                         MT6359_RTC_IRQ_STA
#define PMIC_TCSTA_MASK                         0x1
#define PMIC_TCSTA_SHIFT                        1
#define PMIC_LPSTA_ADDR                         MT6359_RTC_IRQ_STA
#define PMIC_LPSTA_MASK                         0x1
#define PMIC_LPSTA_SHIFT                        3
#define PMIC_AL_EN_ADDR                         MT6359_RTC_IRQ_EN
#define PMIC_AL_EN_MASK                         0x1
#define PMIC_AL_EN_SHIFT                        0
#define PMIC_TC_EN_ADDR                         MT6359_RTC_IRQ_EN
#define PMIC_TC_EN_MASK                         0x1
#define PMIC_TC_EN_SHIFT                        1
#define PMIC_ONESHOT_ADDR                       MT6359_RTC_IRQ_EN
#define PMIC_ONESHOT_MASK                       0x1
#define PMIC_ONESHOT_SHIFT                      2
#define PMIC_LP_EN_ADDR                         MT6359_RTC_IRQ_EN
#define PMIC_LP_EN_MASK                         0x1
#define PMIC_LP_EN_SHIFT                        3
#define PMIC_SECCII_ADDR                        MT6359_RTC_CII_EN
#define PMIC_SECCII_MASK                        0x1
#define PMIC_SECCII_SHIFT                       0
#define PMIC_MINCII_ADDR                        MT6359_RTC_CII_EN
#define PMIC_MINCII_MASK                        0x1
#define PMIC_MINCII_SHIFT                       1
#define PMIC_HOUCII_ADDR                        MT6359_RTC_CII_EN
#define PMIC_HOUCII_MASK                        0x1
#define PMIC_HOUCII_SHIFT                       2
#define PMIC_DOMCII_ADDR                        MT6359_RTC_CII_EN
#define PMIC_DOMCII_MASK                        0x1
#define PMIC_DOMCII_SHIFT                       3
#define PMIC_DOWCII_ADDR                        MT6359_RTC_CII_EN
#define PMIC_DOWCII_MASK                        0x1
#define PMIC_DOWCII_SHIFT                       4
#define PMIC_MTHCII_ADDR                        MT6359_RTC_CII_EN
#define PMIC_MTHCII_MASK                        0x1
#define PMIC_MTHCII_SHIFT                       5
#define PMIC_YEACII_ADDR                        MT6359_RTC_CII_EN
#define PMIC_YEACII_MASK                        0x1
#define PMIC_YEACII_SHIFT                       6
#define PMIC_SECCII_1_2_ADDR                    MT6359_RTC_CII_EN
#define PMIC_SECCII_1_2_MASK                    0x1
#define PMIC_SECCII_1_2_SHIFT                   7
#define PMIC_SECCII_1_4_ADDR                    MT6359_RTC_CII_EN
#define PMIC_SECCII_1_4_MASK                    0x1
#define PMIC_SECCII_1_4_SHIFT                   8
#define PMIC_SECCII_1_8_ADDR                    MT6359_RTC_CII_EN
#define PMIC_SECCII_1_8_MASK                    0x1
#define PMIC_SECCII_1_8_SHIFT                   9
#define PMIC_SEC_MSK_ADDR                       MT6359_RTC_AL_MASK
#define PMIC_SEC_MSK_MASK                       0x1
#define PMIC_SEC_MSK_SHIFT                      0
#define PMIC_MIN_MSK_ADDR                       MT6359_RTC_AL_MASK
#define PMIC_MIN_MSK_MASK                       0x1
#define PMIC_MIN_MSK_SHIFT                      1
#define PMIC_HOU_MSK_ADDR                       MT6359_RTC_AL_MASK
#define PMIC_HOU_MSK_MASK                       0x1
#define PMIC_HOU_MSK_SHIFT                      2
#define PMIC_DOM_MSK_ADDR                       MT6359_RTC_AL_MASK
#define PMIC_DOM_MSK_MASK                       0x1
#define PMIC_DOM_MSK_SHIFT                      3
#define PMIC_DOW_MSK_ADDR                       MT6359_RTC_AL_MASK
#define PMIC_DOW_MSK_MASK                       0x1
#define PMIC_DOW_MSK_SHIFT                      4
#define PMIC_MTH_MSK_ADDR                       MT6359_RTC_AL_MASK
#define PMIC_MTH_MSK_MASK                       0x1
#define PMIC_MTH_MSK_SHIFT                      5
#define PMIC_YEA_MSK_ADDR                       MT6359_RTC_AL_MASK
#define PMIC_YEA_MSK_MASK                       0x1
#define PMIC_YEA_MSK_SHIFT                      6
#define PMIC_TC_SECOND_ADDR                     MT6359_RTC_TC_SEC
#define PMIC_TC_SECOND_MASK                     0x3F
#define PMIC_TC_SECOND_SHIFT                    0
#define PMIC_TC_MINUTE_ADDR                     MT6359_RTC_TC_MIN
#define PMIC_TC_MINUTE_MASK                     0x3F
#define PMIC_TC_MINUTE_SHIFT                    0
#define PMIC_TC_HOUR_ADDR                       MT6359_RTC_TC_HOU
#define PMIC_TC_HOUR_MASK                       0x1F
#define PMIC_TC_HOUR_SHIFT                      0
#define PMIC_TC_DOM_ADDR                        MT6359_RTC_TC_DOM
#define PMIC_TC_DOM_MASK                        0x1F
#define PMIC_TC_DOM_SHIFT                       0
#define PMIC_TC_DOW_ADDR                        MT6359_RTC_TC_DOW
#define PMIC_TC_DOW_MASK                        0x7
#define PMIC_TC_DOW_SHIFT                       0
#define PMIC_TC_MONTH_ADDR                      MT6359_RTC_TC_MTH
#define PMIC_TC_MONTH_MASK                      0xF
#define PMIC_TC_MONTH_SHIFT                     0
#define PMIC_RTC_MACRO_ID_ADDR                  MT6359_RTC_TC_MTH
#define PMIC_RTC_MACRO_ID_MASK                  0xFFF
#define PMIC_RTC_MACRO_ID_SHIFT                 4
#define PMIC_TC_YEAR_ADDR                       MT6359_RTC_TC_YEA
#define PMIC_TC_YEAR_MASK                       0x7F
#define PMIC_TC_YEAR_SHIFT                      0
#define PMIC_AL_SECOND_ADDR                     MT6359_RTC_AL_SEC
#define PMIC_AL_SECOND_MASK                     0x3F
#define PMIC_AL_SECOND_SHIFT                    0
#define PMIC_BBPU_AUTO_PDN_SEL_ADDR             MT6359_RTC_AL_SEC
#define PMIC_BBPU_AUTO_PDN_SEL_MASK             0x1
#define PMIC_BBPU_AUTO_PDN_SEL_SHIFT            6
#define PMIC_BBPU_2SEC_CK_SEL_ADDR              MT6359_RTC_AL_SEC
#define PMIC_BBPU_2SEC_CK_SEL_MASK              0x1
#define PMIC_BBPU_2SEC_CK_SEL_SHIFT             7
#define PMIC_BBPU_2SEC_EN_ADDR                  MT6359_RTC_AL_SEC
#define PMIC_BBPU_2SEC_EN_MASK                  0x1
#define PMIC_BBPU_2SEC_EN_SHIFT                 8
#define PMIC_BBPU_2SEC_MODE_ADDR                MT6359_RTC_AL_SEC
#define PMIC_BBPU_2SEC_MODE_MASK                0x3
#define PMIC_BBPU_2SEC_MODE_SHIFT               9
#define PMIC_BBPU_2SEC_STAT_CLEAR_ADDR          MT6359_RTC_AL_SEC
#define PMIC_BBPU_2SEC_STAT_CLEAR_MASK          0x1
#define PMIC_BBPU_2SEC_STAT_CLEAR_SHIFT         11
#define PMIC_BBPU_2SEC_STAT_STA_ADDR            MT6359_RTC_AL_SEC
#define PMIC_BBPU_2SEC_STAT_STA_MASK            0x1
#define PMIC_BBPU_2SEC_STAT_STA_SHIFT           12
#define PMIC_RTC_LPD_OPT_ADDR                   MT6359_RTC_AL_SEC
#define PMIC_RTC_LPD_OPT_MASK                   0x3
#define PMIC_RTC_LPD_OPT_SHIFT                  13
#define PMIC_K_EOSC32_VTCXO_ON_SEL_ADDR         MT6359_RTC_AL_SEC
#define PMIC_K_EOSC32_VTCXO_ON_SEL_MASK         0x1
#define PMIC_K_EOSC32_VTCXO_ON_SEL_SHIFT        15
#define PMIC_AL_MINUTE_ADDR                     MT6359_RTC_AL_MIN
#define PMIC_AL_MINUTE_MASK                     0x3F
#define PMIC_AL_MINUTE_SHIFT                    0
#define PMIC_AL_HOUR_ADDR                       MT6359_RTC_AL_HOU
#define PMIC_AL_HOUR_MASK                       0x1F
#define PMIC_AL_HOUR_SHIFT                      0
#define PMIC_NEW_SPARE0_ADDR                    MT6359_RTC_AL_HOU
#define PMIC_NEW_SPARE0_MASK                    0xFF
#define PMIC_NEW_SPARE0_SHIFT                   8
#define PMIC_AL_DOM_ADDR                        MT6359_RTC_AL_DOM
#define PMIC_AL_DOM_MASK                        0x1F
#define PMIC_AL_DOM_SHIFT                       0
#define PMIC_NEW_SPARE1_ADDR                    MT6359_RTC_AL_DOM
#define PMIC_NEW_SPARE1_MASK                    0xFF
#define PMIC_NEW_SPARE1_SHIFT                   8
#define PMIC_AL_DOW_ADDR                        MT6359_RTC_AL_DOW
#define PMIC_AL_DOW_MASK                        0x7
#define PMIC_AL_DOW_SHIFT                       0
#define PMIC_RG_EOSC_CALI_TD_ADDR               MT6359_RTC_AL_DOW
#define PMIC_RG_EOSC_CALI_TD_MASK               0x7
#define PMIC_RG_EOSC_CALI_TD_SHIFT              5
#define PMIC_NEW_SPARE2_ADDR                    MT6359_RTC_AL_DOW
#define PMIC_NEW_SPARE2_MASK                    0xFF
#define PMIC_NEW_SPARE2_SHIFT                   8
#define PMIC_AL_MONTH_ADDR                      MT6359_RTC_AL_MTH
#define PMIC_AL_MONTH_MASK                      0xF
#define PMIC_AL_MONTH_SHIFT                     0
#define PMIC_NEW_SPARE3_ADDR                    MT6359_RTC_AL_MTH
#define PMIC_NEW_SPARE3_MASK                    0xFF
#define PMIC_NEW_SPARE3_SHIFT                   8
#define PMIC_AL_YEAR_ADDR                       MT6359_RTC_AL_YEA
#define PMIC_AL_YEAR_MASK                       0x7F
#define PMIC_AL_YEAR_SHIFT                      0
#define PMIC_RTC_K_EOSC_RSV_ADDR                MT6359_RTC_AL_YEA
#define PMIC_RTC_K_EOSC_RSV_MASK                0xFF
#define PMIC_RTC_K_EOSC_RSV_SHIFT               8
#define PMIC_XOSCCALI_ADDR                      MT6359_RTC_OSC32CON
#define PMIC_XOSCCALI_MASK                      0x1F
#define PMIC_XOSCCALI_SHIFT                     0
#define PMIC_RTC_XOSC32_ENB_ADDR                MT6359_RTC_OSC32CON
#define PMIC_RTC_XOSC32_ENB_MASK                0x1
#define PMIC_RTC_XOSC32_ENB_SHIFT               5
#define PMIC_RTC_EMBCK_SEL_MODE_ADDR            MT6359_RTC_OSC32CON
#define PMIC_RTC_EMBCK_SEL_MODE_MASK            0x3
#define PMIC_RTC_EMBCK_SEL_MODE_SHIFT           6
#define PMIC_RTC_EMBCK_SRC_SEL_ADDR             MT6359_RTC_OSC32CON
#define PMIC_RTC_EMBCK_SRC_SEL_MASK             0x1
#define PMIC_RTC_EMBCK_SRC_SEL_SHIFT            8
#define PMIC_RTC_EMBCK_SEL_OPTION_ADDR          MT6359_RTC_OSC32CON
#define PMIC_RTC_EMBCK_SEL_OPTION_MASK          0x1
#define PMIC_RTC_EMBCK_SEL_OPTION_SHIFT         9
#define PMIC_RTC_GPS_CKOUT_EN_ADDR              MT6359_RTC_OSC32CON
#define PMIC_RTC_GPS_CKOUT_EN_MASK              0x1
#define PMIC_RTC_GPS_CKOUT_EN_SHIFT             10
#define PMIC_RTC_EOSC32_VCT_EN_ADDR             MT6359_RTC_OSC32CON
#define PMIC_RTC_EOSC32_VCT_EN_MASK             0x1
#define PMIC_RTC_EOSC32_VCT_EN_SHIFT            11
#define PMIC_RTC_EOSC32_CHOP_EN_ADDR            MT6359_RTC_OSC32CON
#define PMIC_RTC_EOSC32_CHOP_EN_MASK            0x1
#define PMIC_RTC_EOSC32_CHOP_EN_SHIFT           12
#define PMIC_RTC_GP_OSC32_CON_ADDR              MT6359_RTC_OSC32CON
#define PMIC_RTC_GP_OSC32_CON_MASK              0x3
#define PMIC_RTC_GP_OSC32_CON_SHIFT             13
#define PMIC_RTC_REG_XOSC32_ENB_ADDR            MT6359_RTC_OSC32CON
#define PMIC_RTC_REG_XOSC32_ENB_MASK            0x1
#define PMIC_RTC_REG_XOSC32_ENB_SHIFT           15
#define PMIC_RTC_POWERKEY1_ADDR                 MT6359_RTC_POWERKEY1
#define PMIC_RTC_POWERKEY1_MASK                 0xFFFF
#define PMIC_RTC_POWERKEY1_SHIFT                0
#define PMIC_RTC_POWERKEY2_ADDR                 MT6359_RTC_POWERKEY2
#define PMIC_RTC_POWERKEY2_MASK                 0xFFFF
#define PMIC_RTC_POWERKEY2_SHIFT                0
#define PMIC_RTC_PDN1_ADDR                      MT6359_RTC_PDN1
#define PMIC_RTC_PDN1_MASK                      0xFFFF
#define PMIC_RTC_PDN1_SHIFT                     0
#define PMIC_RTC_PDN2_ADDR                      MT6359_RTC_PDN2
#define PMIC_RTC_PDN2_MASK                      0xFFFF
#define PMIC_RTC_PDN2_SHIFT                     0
#define PMIC_RTC_SPAR0_ADDR                     MT6359_RTC_SPAR0
#define PMIC_RTC_SPAR0_MASK                     0xFFFF
#define PMIC_RTC_SPAR0_SHIFT                    0
#define PMIC_RTC_SPAR1_ADDR                     MT6359_RTC_SPAR1
#define PMIC_RTC_SPAR1_MASK                     0xFFFF
#define PMIC_RTC_SPAR1_SHIFT                    0
#define PMIC_RTC_PROT_ADDR                      MT6359_RTC_PROT
#define PMIC_RTC_PROT_MASK                      0xFFFF
#define PMIC_RTC_PROT_SHIFT                     0
#define PMIC_RTC_DIFF_ADDR                      MT6359_RTC_DIFF
#define PMIC_RTC_DIFF_MASK                      0xFFF
#define PMIC_RTC_DIFF_SHIFT                     0
#define PMIC_POWER_DETECTED_ADDR                MT6359_RTC_DIFF
#define PMIC_POWER_DETECTED_MASK                0x1
#define PMIC_POWER_DETECTED_SHIFT               12
#define PMIC_K_EOSC32_RSV_ADDR                  MT6359_RTC_DIFF
#define PMIC_K_EOSC32_RSV_MASK                  0x1
#define PMIC_K_EOSC32_RSV_SHIFT                 14
#define PMIC_CALI_RD_SEL_ADDR                   MT6359_RTC_DIFF
#define PMIC_CALI_RD_SEL_MASK                   0x1
#define PMIC_CALI_RD_SEL_SHIFT                  15
#define PMIC_RTC_CALI_ADDR                      MT6359_RTC_CALI
#define PMIC_RTC_CALI_MASK                      0x3FFF
#define PMIC_RTC_CALI_SHIFT                     0
#define PMIC_CALI_WR_SEL_ADDR                   MT6359_RTC_CALI
#define PMIC_CALI_WR_SEL_MASK                   0x1
#define PMIC_CALI_WR_SEL_SHIFT                  14
#define PMIC_K_EOSC32_OVERFLOW_ADDR             MT6359_RTC_CALI
#define PMIC_K_EOSC32_OVERFLOW_MASK             0x1
#define PMIC_K_EOSC32_OVERFLOW_SHIFT            15
#define PMIC_WRTGR_ADDR                         MT6359_RTC_WRTGR
#define PMIC_WRTGR_MASK                         0x1
#define PMIC_WRTGR_SHIFT                        0
#define PMIC_VBAT_LPSTA_RAW_ADDR                MT6359_RTC_CON
#define PMIC_VBAT_LPSTA_RAW_MASK                0x1
#define PMIC_VBAT_LPSTA_RAW_SHIFT               0
#define PMIC_EOSC32_LPEN_ADDR                   MT6359_RTC_CON
#define PMIC_EOSC32_LPEN_MASK                   0x1
#define PMIC_EOSC32_LPEN_SHIFT                  1
#define PMIC_XOSC32_LPEN_ADDR                   MT6359_RTC_CON
#define PMIC_XOSC32_LPEN_MASK                   0x1
#define PMIC_XOSC32_LPEN_SHIFT                  2
#define PMIC_LPRST_ADDR                         MT6359_RTC_CON
#define PMIC_LPRST_MASK                         0x1
#define PMIC_LPRST_SHIFT                        3
#define PMIC_CDBO_ADDR                          MT6359_RTC_CON
#define PMIC_CDBO_MASK                          0x1
#define PMIC_CDBO_SHIFT                         4
#define PMIC_F32KOB_ADDR                        MT6359_RTC_CON
#define PMIC_F32KOB_MASK                        0x1
#define PMIC_F32KOB_SHIFT                       5
#define PMIC_GPO_ADDR                           MT6359_RTC_CON
#define PMIC_GPO_MASK                           0x1
#define PMIC_GPO_SHIFT                          6
#define PMIC_GOE_ADDR                           MT6359_RTC_CON
#define PMIC_GOE_MASK                           0x1
#define PMIC_GOE_SHIFT                          7
#define PMIC_GSR_ADDR                           MT6359_RTC_CON
#define PMIC_GSR_MASK                           0x1
#define PMIC_GSR_SHIFT                          8
#define PMIC_GSMT_ADDR                          MT6359_RTC_CON
#define PMIC_GSMT_MASK                          0x1
#define PMIC_GSMT_SHIFT                         9
#define PMIC_GPEN_ADDR                          MT6359_RTC_CON
#define PMIC_GPEN_MASK                          0x1
#define PMIC_GPEN_SHIFT                         10
#define PMIC_GPU_ADDR                           MT6359_RTC_CON
#define PMIC_GPU_MASK                           0x1
#define PMIC_GPU_SHIFT                          11
#define PMIC_GE4_ADDR                           MT6359_RTC_CON
#define PMIC_GE4_MASK                           0x1
#define PMIC_GE4_SHIFT                          12
#define PMIC_GE8_ADDR                           MT6359_RTC_CON
#define PMIC_GE8_MASK                           0x1
#define PMIC_GE8_SHIFT                          13
#define PMIC_GPI_ADDR                           MT6359_RTC_CON
#define PMIC_GPI_MASK                           0x1
#define PMIC_GPI_SHIFT                          14
#define PMIC_LPSTA_RAW_ADDR                     MT6359_RTC_CON
#define PMIC_LPSTA_RAW_MASK                     0x1
#define PMIC_LPSTA_RAW_SHIFT                    15
#define PMIC_DAT0_LOCK_ADDR                     MT6359_RTC_SEC_CTRL
#define PMIC_DAT0_LOCK_MASK                     0x1
#define PMIC_DAT0_LOCK_SHIFT                    0
#define PMIC_DAT1_LOCK_ADDR                     MT6359_RTC_SEC_CTRL
#define PMIC_DAT1_LOCK_MASK                     0x1
#define PMIC_DAT1_LOCK_SHIFT                    1
#define PMIC_DAT2_LOCK_ADDR                     MT6359_RTC_SEC_CTRL
#define PMIC_DAT2_LOCK_MASK                     0x1
#define PMIC_DAT2_LOCK_SHIFT                    2
#define PMIC_RTC_INT_CNT_ADDR                   MT6359_RTC_INT_CNT
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


