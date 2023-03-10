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

#define BIT(nr) (1 << (nr))

enum mt6360_adc_channel {
    MT6360_ADC_USBID,
    MT6360_ADC_VBUSDIV5,
    MT6360_ADC_VBUSDIV2,
    MT6360_ADC_VSYS,
    MT6360_ADC_VBAT,
    MT6360_ADC_IBUS,
    MT6360_ADC_IBAT,
    MT6360_ADC_CHG_VDDP,
    MT6360_ADC_TEMP_JC,
    MT6360_ADC_VREF_TS,
    MT6360_ADC_TS,
    MT6360_ADC_MAX,
};

#define MT6360_I2C_ID                       (0x05)
#define MT6360_SLAVE_ADDR                   (0x34)
#define MT6360_VENDOR_ID                    (0x50)

/* core control */
#define MT6360_PMU_DEV_INFO                 (0x00)
#define MT6360_PMU_CORE_CTRL1               (0x01)
#define MT6360_PMU_RST1                     (0x02)
#define MT6360_PMU_CRCEN                    (0x03)
#define MT6360_PMU_RST_PAS_CODE1            (0x04)
#define MT6360_PMU_RST_PAS_CODE2            (0x05)
#define MT6360_PMU_CORE_CTRL2               (0x06)
#define MT6360_PMU_TM_PAS_CODE1             (0x07)
#define MT6360_PMU_TM_PAS_CODE2             (0x08)
#define MT6360_PMU_TM_PAS_CODE3             (0x09)
#define MT6360_PMU_TM_PAS_CODE4             (0x0A)

/* charger control */
#define MT6360_PMU_I2C_CTRL                 (0x10)
#define MT6360_PMU_CHG_CTRL1                (0x11)
#define MT6360_PMU_CHG_CTRL2                (0x12)
#define MT6360_PMU_CHG_CTRL3                (0x13)
#define MT6360_PMU_CHG_CTRL4                (0x14)
#define MT6360_PMU_CHG_CTRL5                (0x15)
#define MT6360_PMU_CHG_CTRL6                (0x16)
#define MT6360_PMU_CHG_CTRL7                (0x17)
#define MT6360_PMU_CHG_CTRL8                (0x18)
#define MT6360_PMU_CHG_CTRL9                (0x19)
#define MT6360_PMU_CHG_CTRL10               (0x1A)
#define MT6360_PMU_CHG_CTRL11               (0x1B)
#define MT6360_PMU_CHG_CTRL12               (0x1C)
#define MT6360_PMU_CHG_CTRL13               (0x1D)
#define MT6360_PMU_CHG_CTRL14               (0x1E)
#define MT6360_PMU_CHG_CTRL15               (0x1F)
#define MT6360_PMU_CHG_CTRL16               (0x20)
#define MT6360_PMU_CHG_AICC_RESULT          (0x21)
#define MT6360_PMU_DEVICE_TYPE              (0x22)
#define MT6360_PMU_QC_CONTROL1              (0x23)
#define MT6360_PMU_QC_CONTROL2              (0x24)
#define MT6360_PMU_QC30_CONTROL1            (0x25)
#define MT6360_PMU_QC30_CONTROL2            (0x26)
#define MT6360_PMU_USB_STATUS1              (0x27)
#define MT6360_PMU_QC_STATUS1               (0x28)
#define MT6360_PMU_QC_STATUS2               (0x29)
#define MT6360_PMU_CHG_PUMP                 (0x2A)
#define MT6360_PMU_CHG_CTRL17               (0x2B)
#define MT6360_PMU_CHG_CTRL18               (0x2C)
#define MT6360_PMU_CHRDET_CTRL1             (0x2D)
#define MT6360_PMU_CHRDET_CTRL2             (0x2E)
#define MT6360_PMU_DPDN_CTRL                (0x2F)
#define MT6360_PMU_CHG_HIDDEN_CTRL1         (0x30)
#define MT6360_PMU_CHG_HIDDEN_CTRL2         (0x31)
#define MT6360_PMU_CHG_HIDDEN_CTRL3         (0x32)
#define MT6360_PMU_CHG_HIDDEN_CTRL4         (0x33)
#define MT6360_PMU_CHG_HIDDEN_CTRL5         (0x34)
#define MT6360_PMU_CHG_HIDDEN_CTRL6         (0x35)
#define MT6360_PMU_CHG_HIDDEN_CTRL7         (0x36)
#define MT6360_PMU_CHG_HIDDEN_CTRL8         (0x37)
#define MT6360_PMU_CHG_HIDDEN_CTRL9         (0x38)
#define MT6360_PMU_CHG_HIDDEN_CTRL10        (0x39)
#define MT6360_PMU_CHG_HIDDEN_CTRL11        (0x3A)
#define MT6360_PMU_CHG_HIDDEN_CTRL12        (0x3B)
#define MT6360_PMU_CHG_HIDDEN_CTRL13        (0x3C)
#define MT6360_PMU_CHG_HIDDEN_CTRL14        (0x3D)
#define MT6360_PMU_CHG_HIDDEN_CTRL15        (0x3E)
#define MT6360_PMU_CHG_HIDDEN_CTRL16        (0x3F)
#define MT6360_PMU_CHG_HIDDEN_CTRL17        (0x40)
#define MT6360_PMU_CHG_HIDDEN_CTRL18        (0x41)
#define MT6360_PMU_CHG_HIDDEN_CTRL19        (0x42)
#define MT6360_PMU_CHG_HIDDEN_CTRL20        (0x43)
#define MT6360_PMU_CHG_HIDDEN_CTRL21        (0x44)
#define MT6360_PMU_CHG_HIDDEN_CTRL22        (0x45)
#define MT6360_PMU_CHG_HIDDEN_CTRL23        (0x46)
#define MT6360_PMU_CHG_HIDDEN_CTRL24        (0x47)
#define MT6360_PMU_CHG_HIDDEN_CTRL25        (0x48)
#define MT6360_PMU_BC12_CTRL                (0x49)
#define MT6360_PMU_CHG_STAT                 (0x4A)
#define MT6360_PMU_RESV1                    (0x4B)
#define MT6360_PMU_TYPEC_OTP_TH_SEL_CODEH   (0x4E)
#define MT6360_PMU_TYPEC_OTP_TH_SEL_CODEL   (0x4F)
#define MT6360_PMU_TYPEC_OTP_HYST_TH        (0x50)
#define MT6360_PMU_TYPEC_OTP_CTRL           (0x51)
#define MT6360_PMU_ADC_BAT_DATA_H           (0x52)
#define MT6360_PMU_ADC_BAT_DATA_L           (0x53)
#define MT6360_PMU_IMID_BACKBST_ON          (0x54)
#define MT6360_PMU_IMID_BACKBST_OFF         (0x55)
#define MT6360_PMU_ADC_CONFIG               (0x56)
#define MT6360_PMU_ADC_EN2                  (0x57)
#define MT6360_PMU_ADC_IDLE_T               (0x58)
#define MT6360_PMU_ADC_RPT_1                (0x5A)
#define MT6360_PMU_ADC_RPT_2                (0x5B)
#define MT6360_PMU_ADC_RPT_3                (0x5C)
#define MT6360_PMU_ADC_RPT_ORG1             (0x5D)
#define MT6360_PMU_ADC_RPT_ORG2             (0x5E)
#define MT6360_PMU_BAT_OVP_TH_SEL_CODEH     (0x5F)
#define MT6360_PMU_BAT_OVP_TH_SEL_CODEL     (0x60)
#define MT6360_PMU_CHG_CTRL19               (0x61)
#define MT6360_PMU_VDDASUPPLY               (0x62)
#define MT6360_PMU_BC12_MANUAL              (0x63)
#define MT6360_PMU_CHGDET_FUNC              (0x64)
#define MT6360_PMU_FOD_CTRL                 (0x65)
#define MT6360_PMU_CHG_CTRL20               (0x66)
#define MT6360_PMU_CHG_HIDDEN_CTRL26        (0x67)
#define MT6360_PMU_CHG_HIDDEN_CTRL27        (0x68)
#define MT6360_PMU_RESV2                    (0x69)
#define MT6360_PMU_USBID_CTRL1              (0x6D)
#define MT6360_PMU_USBID_CTRL2              (0x6E)
#define MT6360_PMU_USBID_CTRL3              (0x6F)
/* status event */
#define MT6360_PMU_CHG_IRQ6                 (0xD5)
#define MT6360_PMU_CHG_STAT1                (0xE0)
#define MT6360_PMU_CHG_STAT2                (0xE1)
#define MT6360_PMU_CHG_STAT3                (0xE2)
#define MT6360_PMU_CHG_STAT4                (0xE3)
#define MT6360_PMU_CHG_STAT5                (0xE4)
#define MT6360_PMU_CHG_STAT6                (0xE5)
#define MT6360_PMU_DPDM_STAT                (0xE6)
#define MT6360_PMU_CHRDET_STAT              (0xE7)
#define MT6360_PMU_CHG_MASK6                (0xF5)

/* MT6360_PMU_CHG_CTRL1 : 0x11 */
#define MT6360_MASK_FORCE_SLEEP BIT(3)
#define MT6360_SHFT_FORCE_SLEEP (3)
#define MT6360_MASK_OPA_MODE    BIT(0)
#define MT6360_SHFT_OPA_MODE    (0)

/* MT6360_PMU_CHG_CTRL2 : 0x12 */
#define MT6360_MASK_IINLMTSEL   (0x0C)
#define MT6360_SHFT_IINLMTSEL   (2)
#define MT6360_MASK_TE_EN       BIT(4)
#define MT6360_SHFT_TE_EN       (4)
#define MT6360_MASK_CFO_EN      BIT(1)
#define MT6360_SHFT_CFO_EN      (1)
#define MT6360_MASK_CHG_EN      BIT(0)
#define MT6360_SHFT_CHG_EN      (0)

/* MT6360_PMU_CHG_CTRL3 : 0x13 */
#define MT6360_MASK_AICR    (0xFC)
#define MT6360_SHFT_AICR    (2)
#define MT6360_AICR_MAXVAL  (0x3F)
#define MT6360_MASK_AICR_EN BIT(1)
#define MT6360_MASK_ILIM_EN BIT(0)

/* MT6360_PMU_CHG_CTRL4 : 0x14 */
#define MT6360_MASK_VOREG   (0xFE)
#define MT6360_SHFT_VOREG   (1)
#define MT6360_VOREG_MAXVAL (0x51)

/* MT6360_PMU_CHG_CTRL6 : 0x16 */
#define MT6360_MASK_MIVR    (0xFE)
#define MT6360_SHFT_MIVR    (1)
#define MT6360_MIVR_MAXVAL  (0x5F)

/* MT6360_PMU_CHG_CTRL7 : 0x17 */
#define MT6360_MASK_ICHG    (0xFC)
#define MT6360_SHFT_ICHG    (2)
#define MT6360_ICHG_MAXVAL  (0x31)

/* MT6360_PMU_CHG_CTRL8 : 0x18 */
#define MT6360_MASK_IPREC   (0x0F)
#define MT6360_SHFT_IPREC   (0)
#define MT6360_IPREC_MAXVAL (0x0F)

/* MT6360_PMU_CHG_CTRL9 : 0x19 */
#define MT6360_MASK_IEOC    (0xF0)
#define MT6360_SHFT_IEOC    (4)
#define MT6360_IEOC_MAXVAL  (0x0F)

/* MT6360_PMU_CHG_AICC_RESULT : 0x21 */
#define MT6360_MASK_RG_AICC_RESULT  (0xFC)
#define MT6360_SHFT_RG_AICC_RESULT  (2)

/* MT6360_PMU_CHG_CTRL10 : 0x1A */
#define MT6360_MASK_OTG_OC          (0x07)
#define MT6360_SHFT_OTG_OC          (0)
#define MT6360_OTG_OC_MAXVAL        (0x07)

/* MT6360_PMU_CHG_CTRL12 : 0x1C */
#define MT6360_MASK_TMR_EN          BIT(1)
#define MT6360_SHFT_TMR_EN          (1)

/* MT6360_PMU_CHG_CTRL13 : 0x1D */
#define MT6360_MASK_CHG_WDT_EN      BIT(7)

/* MT6360_PMU_CHG_CTRL14 : 0x1E */
#define MT6360_MASK_RG_EN_AICC      BIT(7)

/* MT6360_PMU_DEVICE_TYPE : 0x22 */
#define MT6360_MASK_USBCHGEN        BIT(7)
#define MT6360_SHFT_USBCHGEN        (7)

/* MT6360_PMU_USB_STATUS1 : 0x27 */
#define MT6360_MASK_USB_STATUS      (0x70)
#define MT6360_SHFT_USB_STATUS      (4)

/* MT6360_PMU_CHG_CTRL16 : 0x2A */
#define MT6360_MASK_AICC_VTH        (0xFE)
#define MT6360_SHFT_AICC_VTH        (1)
#define MT6360_AICC_VTH_MAXVAL      (0x5F)

/* MT6360_PMU_CHG_CTRL17 : 0x2B */
#define MT6360_MASK_EN_PUMPX        BIT(7)
#define MT6360_SHFT_EN_PUMPX        (7)
#define MT6360_MASK_PUMPX_20_10     BIT(6)
#define MT6360_SHFT_PUMPX_20_10     (6)
#define MT6360_MASK_PUMPX_UP_DN     BIT(5)
#define MT6360_SHFT_PUMPX_UP_DN     (5)
#define MT6360_MASK_PUMPX_DEC       (0x1F)
#define MT6360_SHFT_PUMPX_DEC       (0)
#define MT6360_PUMPX_20_MAXVAL      (0x1D)

/* MT6360_PMU_CHG_CTRL18 : 0x2C */
#define MT6360_MASK_BAT_COMP        (0x38)
#define MT6360_SHFT_BAT_COMP        (3)
#define MT6360_BAT_COMP_MAXVAL      (0x07)
#define MT6360_MASK_VCLAMP          (0x7)
#define MT6360_SHFT_VCLAMP          (0)
#define MT6360_VCLAMP_MAXVAL        (0x07)

/* MT6360_PMU_CHG_HIDDEN_CTRL2 : 0x31 */
#define MT6360_MASK_EOC_RST         BIT(7)
#define MT6360_MASK_DISCHG          BIT(2)
#define MT6360_SHFT_DISCHG          (2)

/* MT6360_PMU_CHG_STAT : 0x4A */
#define MT6360_MASK_CHG_STAT        (0xC0)
#define MT6360_SHFT_CHG_STAT        (6)
#define MT6360_MASK_VBAT_LVL        BIT(5)
#define MT6360_SHFT_VBAT_LVL        (5)
#define MT6360_MASK_VBAT_TRI        BIT(4)
#define MT6360_SHFT_VBAT_TRI        (4)

/* MT6360_PMU_ADC_CONFIG : 0x56 */
#define MT6360_MASK_ZCV_EN          BIT(6)
#define MT6360_SHFT_ZCV_EN          (6)

/* MT6360_PMU_CHG_CTRL19 : 0x61 */
#define MT6360_MASK_CHG_VIN_OVP_VTHSEL  (0x60)
#define MT6360_SHFT_CHG_VIN_OVP_VTHSEL  (5)

/* MT6360_PMU_FOD_CTRL : 0x65 */
#define MT6360_MASK_FOD_SWEN        BIT(7)

/* MT6360_PMU_CHG_CTRL20 : 0x66 */
#define MT6360_MASK_EN_SDI          BIT(1)

/* MT6360_PMU_CHG_STAT5 : 0xE5 */
#define MT6360_MASK_TYPEC_OTP       BIT(2)

/* MT6360_PMU_FOD_STAT : 0xE7 */
#define MT6360_MASK_FOD_ALL_STAT    (0xE3)
#define MT6360_MASK_FOD_DISCHG_FAIL BIT(7)
#define MT6360_MASK_FOD_HR          BIT(6)
#define MT6360_MASK_FOD_LR          BIT(5)
#define MT6360_MASK_FOD_OV          BIT(1)
#define MT6360_MASK_FOD_DONE        BIT(0)

/* MT6360_PMU_FLED_EN : 0x7E */
#define MT6360_MASK_STROBE_EN       BIT(2)

/* MT6360_PMU_CHG_STAT1 : 0xE0 */
#define MT6360_MASK_PWR_RDY_EVT     BIT(7)
#define MT6360_SHFT_PWR_RDY_EVT     (7)
#define MT6360_MASK_MIVR_EVT        BIT(6)
#define MT6360_SHFT_MIVR_EVT        (6)
#define MT6360_MASK_CHG_TREG        BIT(4)
#define MT6360_SHFT_CHG_TREG        (4)

/* MT6360_PMU_CHRDET_STAT : 0xE7 */
#define MT6360_MASK_CHRDET_EXT_EVT  BIT(4)

/* MT6360_PMU_CHG_MASK6 : 0xF5 */
#define MT6360_MASK_ADC_DONE        BIT(4)
