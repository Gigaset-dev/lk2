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

/* APB Module ssusb_top_sif - USB3_SIF2_BASE */
#define SSUSB_SIFSLV_U2PHY_COM_BASE     (USB3_SIF2_BASE + 0x300)
#define SSUSB_SIFSLV_SPLLC_BASE         (USB3_SIF2_BASE + 0x700)
#define SSUSB_SIFSLV_U3PHYD_BASE        (USB3_SIF2_BASE + 0x900)
#define SSUSB_SIFSLV_U3PHYA_BASE        (USB3_SIF2_BASE + 0xB00)
#define SSUSB_SIFSLV_U3PHYA_DA_BASE     (USB3_SIF2_BASE + 0xC00)

/* referenecd from ssusb_USB20_PHY_regmap_com_T28.xls */
#define U3D_USBPHYACR0              (SSUSB_SIFSLV_U2PHY_COM_BASE+0x0000) /* bit 2~bit 30 */
#define U3D_USBPHYACR1              (SSUSB_SIFSLV_U2PHY_COM_BASE+0x0004)
#define U3D_USBPHYACR2              (SSUSB_SIFSLV_U2PHY_COM_BASE+0x0008) /* bit 0~ bit15 */
#define U3D_USBPHYACR4              (SSUSB_SIFSLV_U2PHY_COM_BASE+0x0010)
#define U3D_USBPHYACR5              (SSUSB_SIFSLV_U2PHY_COM_BASE+0x0014)
#define U3D_USBPHYACR6              (SSUSB_SIFSLV_U2PHY_COM_BASE+0x0018)
#define U3D_U2PHYACR3               (SSUSB_SIFSLV_U2PHY_COM_BASE+0x001c)
#define U3D_U2PHYACR4               (SSUSB_SIFSLV_U2PHY_COM_BASE+0x0020) /* bit8~ bit18 */
#define U3D_U2PHYAMON0              (SSUSB_SIFSLV_U2PHY_COM_BASE+0x0024)
#define U3D_U2PHYDCR0               (SSUSB_SIFSLV_U2PHY_COM_BASE+0x0060)
#define U3D_U2PHYDCR1               (SSUSB_SIFSLV_U2PHY_COM_BASE+0x0064)
#define U3D_U2PHYDTM0               (SSUSB_SIFSLV_U2PHY_COM_BASE+0x0068)
#define U3D_U2PHYDTM1               (SSUSB_SIFSLV_U2PHY_COM_BASE+0x006C)
#define U3D_U2PHYDMON0              (SSUSB_SIFSLV_U2PHY_COM_BASE+0x0070)
#define U3D_U2PHYDMON1              (SSUSB_SIFSLV_U2PHY_COM_BASE+0x0074)
#define U3D_U2PHYDMON2              (SSUSB_SIFSLV_U2PHY_COM_BASE+0x0078)
#define U3D_U2PHYDMON3              (SSUSB_SIFSLV_U2PHY_COM_BASE+0x007C)
#define U3D_U2PHYBC12C              (SSUSB_SIFSLV_U2PHY_COM_BASE+0x0080)
#define U3D_U2PHYBC12C1             (SSUSB_SIFSLV_U2PHY_COM_BASE+0x0084)
#define U3D_U2PHYREGFPPC            (SSUSB_SIFSLV_U2PHY_COM_BASE+0x00e0)
#define U3D_U2PHYVERSIONC           (SSUSB_SIFSLV_U2PHY_COM_BASE+0x00f0)
#define U3D_U2PHYREGFCOM            (SSUSB_SIFSLV_U2PHY_COM_BASE+0x00fc)

/* U3D_USBPHYACR0 */
#define E60802_RG_USB20_MPX_OUT_SEL               (0x7<<28) /* 30:28 */
#define E60802_RG_USB20_TX_PH_ROT_SEL             (0x7<<24) /* 26:24 */
#define E60802_RG_USB20_PLL_DIVEN                 (0x7<<20) /* 22:20 */
#define E60802_RG_USB20_PLL_BR                    (0x1<<18) /* 18:18 */
#define E60802_RG_USB20_PLL_BP                    (0x1<<17) /* 17:17 */
#define E60802_RG_USB20_PLL_BLP                   (0x1<<16) /* 16:16 */
#define E60802_RG_USB20_USBPLL_FORCE_ON           (0x1<<15) /* 15:15 */
#define E60802_RG_USB20_PLL_FBDIV                 (0x7f<<8) /* 14:8 */
#define E60802_RG_USB20_PLL_PREDIV                (0x3<<6) /* 7:6 */
#define E60802_RG_USB20_INTR_EN                   (0x1<<5) /* 5:5 */
#define E60802_RG_USB20_REF_EN                    (0x1<<4) /* 4:4 */
#define E60802_RG_USB20_BGR_DIV                   (0x3<<2) /* 3:2 */
#define E60802_RG_SIFSLV_CHP_EN                   (0x1<<1) /* 1:1 */
#define E60802_RG_SIFSLV_BGR_EN                   (0x1<<0) /* 0:0 */

/* U3D_USBPHYACR1 */
#define E60802_RG_USB20_INTR_CAL                  (0x1f<<19) /* 23:19 */
#define E60802_RG_USB20_OTG_VBUSTH                (0x7<<16) /* 18:16 */
#define E60802_RG_USB20_VRT_VREF_SEL              (0x7<<12) /* 14:12 */
#define E60802_RG_USB20_TERM_VREF_SEL             (0x7<<8) /* 10:8 */
#define E60802_RG_USB20_MPX_SEL                   (0xff<<0) /* 7:0 */

/* U3D_USBPHYACR2 */
#define E60802_RG_SIFSLV_MAC_BANDGAP_EN           (0x1<<17) /* 17:17 */
#define E60802_RG_SIFSLV_MAC_CHOPPER_EN           (0x1<<16) /* 16:16 */
#define E60802_RG_USB20_CLKREF_REV                (0xffff<<0) /* 15:0 */

/* U3D_USBPHYACR4 */
#define E60802_RG_USB20_DP_ABIST_SOURCE_EN        (0x1<<31) /* 31:31 */
#define E60802_RG_USB20_DP_ABIST_SELE             (0xf<<24) /* 27:24 */
#define E60802_RG_USB20_ICUSB_EN                  (0x1<<16) /* 16:16 */
#define E60802_RG_USB20_LS_CR                     (0x7<<12) /* 14:12 */
#define E60802_RG_USB20_FS_CR                     (0x7<<8) /* 10:8 */
#define E60802_RG_USB20_LS_SR                     (0x7<<4) /* 6:4 */
#define E60802_RG_USB20_FS_SR                     (0x7<<0) /* 2:0 */

/* U3D_USBPHYACR5 */
#define E60802_RG_USB20_DISC_FIT_EN               (0x1<<28) /* 28:28 */
#define E60802_RG_USB20_INIT_SQ_EN_DG             (0x3<<26) /* 27:26 */
#define E60802_RG_USB20_HSTX_TMODE_SEL            (0x3<<24) /* 25:24 */
#define E60802_RG_USB20_SQD                       (0x3<<22) /* 23:22 */
#define E60802_RG_USB20_DISCD                     (0x3<<20) /* 21:20 */
#define E60802_RG_USB20_HSTX_TMODE_EN             (0x1<<19) /* 19:19 */
#define E60802_RG_USB20_PHYD_MONEN                (0x1<<18) /* 18:18 */
#define E60802_RG_USB20_INLPBK_EN                 (0x1<<17) /* 17:17 */
#define E60802_RG_USB20_CHIRP_EN                  (0x1<<16) /* 16:16 */
#define E60802_RG_USB20_HSTX_SRCAL_EN             (0x1<<15) /* 15:15 */
#define E60802_RG_USB20_HSTX_SRCTRL               (0x7<<12) /* 14:12 */
#define E60802_RG_USB20_HS_100U_U3_EN             (0x1<<11) /* 11:11 */
#define E60802_RG_USB20_GBIAS_ENB                 (0x1<<10) /* 10:10 */
#define E60802_RG_USB20_DM_ABIST_SOURCE_EN        (0x1<<7) /* 7:7 */
#define E60802_RG_USB20_DM_ABIST_SELE             (0xf<<0) /* 3:0 */

/* U3D_USBPHYACR6 */
#define E60802_RG_USB20_ISO_EN                    (0x1<<31) /* 31:31 */
#define E60802_RG_USB20_PHY_REV                   (0xef<<24) /* 31:24 */
#define E60802_RG_USB20_BC11_SW_EN                (0x1<<23) /* 23:23 */
#define E60802_RG_USB20_SR_CLK_SEL                (0x1<<22) /* 22:22 */
#define E60802_RG_USB20_OTG_VBUSCMP_EN            (0x1<<20) /* 20:20 */
#define E60802_RG_USB20_OTG_ABIST_EN              (0x1<<19) /* 19:19 */
#define E60802_RG_USB20_OTG_ABIST_SELE            (0x7<<16) /* 18:16 */
#define E60802_RG_USB20_HSRX_MMODE_SELE           (0x3<<12) /* 13:12 */
#define E60802_RG_USB20_HSRX_BIAS_EN_SEL          (0x3<<9) /* 10:9 */
#define E60802_RG_USB20_HSRX_TMODE_EN             (0x1<<8) /* 8:8 */
#define E60802_RG_USB20_DISCTH                    (0xf<<4) /* 7:4 */
#define E60802_RG_USB20_SQTH                      (0xf<<0) /* 3:0 */

/* U3D_U2PHYACR3 */
#define E60802_RG_USB20_HSTX_DBIST                (0xf<<28) /* 31:28 */
#define E60802_RG_USB20_HSTX_BIST_EN              (0x1<<26) /* 26:26 */
#define E60802_RG_USB20_HSTX_I_EN_MODE            (0x3<<24) /* 25:24 */
#define E60802_RG_USB20_USB11_TMODE_EN            (0x1<<19) /* 19:19 */
#define E60802_RG_USB20_TMODE_FS_LS_TX_EN         (0x1<<18) /* 18:18 */
#define E60802_RG_USB20_TMODE_FS_LS_RCV_EN        (0x1<<17) /* 17:17 */
#define E60802_RG_USB20_TMODE_FS_LS_MODE          (0x1<<16) /* 16:16 */
#define E60802_RG_USB20_HS_TERM_EN_MODE           (0x3<<13) /* 14:13 */
#define E60802_RG_USB20_PUPD_BIST_EN              (0x1<<12) /* 12:12 */
#define E60802_RG_USB20_EN_PU_DM                  (0x1<<11) /* 11:11 */
#define E60802_RG_USB20_EN_PD_DM                  (0x1<<10) /* 10:10 */
#define E60802_RG_USB20_EN_PU_DP                  (0x1<<9) /* 9:9 */
#define E60802_RG_USB20_EN_PD_DP                  (0x1<<8) /* 8:8 */

/* U3D_U2PHYACR4 */
#define E60802_RG_USB20_DP_100K_MODE              (0x1<<18) /* 18:18 */
#define E60802_RG_USB20_DM_100K_EN                (0x1<<17) /* 17:17 */
#define E60802_USB20_DP_100K_EN                   (0x1<<16) /* 16:16 */
#define E60802_USB20_GPIO_DM_I                    (0x1<<15) /* 15:15 */
#define E60802_USB20_GPIO_DP_I                    (0x1<<14) /* 14:14 */
#define E60802_USB20_GPIO_DM_OE                   (0x1<<13) /* 13:13 */
#define E60802_USB20_GPIO_DP_OE                   (0x1<<12) /* 12:12 */
#define E60802_RG_USB20_GPIO_CTL                  (0x1<<9) /* 9:9 */
#define E60802_USB20_GPIO_MODE                    (0x1<<8) /* 8:8 */
#define E60802_RG_USB20_TX_BIAS_EN                (0x1<<5) /* 5:5 */
#define E60802_RG_USB20_TX_VCMPDN_EN              (0x1<<4) /* 4:4 */
#define E60802_RG_USB20_HS_SQ_EN_MODE             (0x3<<2) /* 3:2 */
#define E60802_RG_USB20_HS_RCV_EN_MODE            (0x3<<0) /* 1:0 */

/* U3D_U2PHYAMON0 */
#define E60802_RGO_USB20_GPIO_DM_O                (0x1<<1) /* 1:1 */
#define E60802_RGO_USB20_GPIO_DP_O                (0x1<<0) /* 0:0 */

/* U3D_U2PHYDCR0 */
#define E60802_RG_USB20_CDR_TST                   (0x3<<30) /* 31:30 */
#define E60802_RG_USB20_GATED_ENB                 (0x1<<29) /* 29:29 */
#define E60802_RG_USB20_TESTMODE                  (0x3<<26) /* 27:26 */
#define E60802_RG_SIFSLV_USB20_PLL_STABLE         (0x1<<25) /* 25:25 */
#define E60802_RG_SIFSLV_USB20_PLL_FORCE_ON       (0x1<<24) /* 24:24 */
#define E60802_RG_USB20_PHYD_RESERVE              (0xffff<<8) /* 23:8 */
#define E60802_RG_USB20_EBTHRLD                   (0x1<<7) /* 7:7 */
#define E60802_RG_USB20_EARLY_HSTX_I              (0x1<<6) /* 6:6 */
#define E60802_RG_USB20_TX_TST                    (0x1<<5) /* 5:5 */
#define E60802_RG_USB20_NEGEDGE_ENB               (0x1<<4) /* 4:4 */
#define E60802_RG_USB20_CDR_FILT                  (0xf<<0) /* 3:0 */

/* U3D_U2PHYDCR1 */
#define E60802_RG_USB20_PROBE_SEL                 (0xff<<24) /* 31:24 */
#define E60802_RG_USB20_DRVVBUS                   (0x1<<23) /* 23:23 */
#define E60802_RG_DEBUG_EN                        (0x1<<22) /* 22:22 */
#define E60802_RG_USB20_OTG_PROBE                 (0x3<<20) /* 21:20 */
#define E60802_RG_USB20_SW_PLLMODE                (0x3<<18) /* 19:18 */
#define E60802_RG_USB20_BERTH                     (0x3<<16) /* 17:16 */
#define E60802_RG_USB20_LBMODE                    (0x3<<13) /* 14:13 */
#define E60802_RG_USB20_FORCE_TAP                 (0x1<<12) /* 12:12 */
#define E60802_RG_USB20_TAPSEL                    (0xfff<<0) /* 11:0 */

/* U3D_U2PHYDTM0 */
#define E60802_RG_UART_MODE                       (0x3<<30) /* 31:30 */
#define E60802_FORCE_UART_I                       (0x1<<29) /* 29:29 */
#define E60802_FORCE_UART_BIAS_EN                 (0x1<<28) /* 28:28 */
#define E60802_FORCE_UART_TX_OE                   (0x1<<27) /* 27:27 */
#define E60802_FORCE_UART_EN                      (0x1<<26) /* 26:26 */
#define E60802_FORCE_USB_CLKEN                    (0x1<<25) /* 25:25 */
#define E60802_FORCE_DRVVBUS                      (0x1<<24) /* 24:24 */
#define E60802_FORCE_DATAIN                       (0x1<<23) /* 23:23 */
#define E60802_FORCE_TXVALID                      (0x1<<22) /* 22:22 */
#define E60802_FORCE_DM_PULLDOWN                  (0x1<<21) /* 21:21 */
#define E60802_FORCE_DP_PULLDOWN                  (0x1<<20) /* 20:20 */
#define E60802_FORCE_XCVRSEL                      (0x1<<19) /* 19:19 */
#define E60802_FORCE_SUSPENDM                     (0x1<<18) /* 18:18 */
#define E60802_FORCE_TERMSEL                      (0x1<<17) /* 17:17 */
#define E60802_FORCE_OPMODE                       (0x1<<16) /* 16:16 */
#define E60802_UTMI_MUXSEL                        (0x1<<15) /* 15:15 */
#define E60802_RG_RESET                           (0x1<<14) /* 14:14 */
#define E60802_RG_DATAIN                          (0xf<<10) /* 13:10 */
#define E60802_RG_TXVALIDH                        (0x1<<9) /* 9:9 */
#define E60802_RG_TXVALID                         (0x1<<8) /* 8:8 */
#define E60802_RG_DMPULLDOWN                      (0x1<<7) /* 7:7 */
#define E60802_RG_DPPULLDOWN                      (0x1<<6) /* 6:6 */
#define E60802_RG_XCVRSEL                         (0x3<<4) /* 5:4 */
#define E60802_RG_SUSPENDM                        (0x1<<3) /* 3:3 */
#define E60802_RG_TERMSEL                         (0x1<<2) /* 2:2 */
#define E60802_RG_OPMODE                          (0x3<<0) /* 1:0 */

/* U3D_U2PHYDTM1 */
#define E60802_RG_USB20_PRBS7_EN                  (0x1<<31) /* 31:31 */
#define E60802_RG_USB20_PRBS7_BITCNT              (0x3f<<24) /* 29:24 */
#define E60802_RG_USB20_CLK48M_EN                 (0x1<<23) /* 23:23 */
#define E60802_RG_USB20_CLK60M_EN                 (0x1<<22) /* 22:22 */
#define E60802_RG_UART_I                          (0x1<<19) /* 19:19 */
#define E60802_RG_UART_BIAS_EN                    (0x1<<18) /* 18:18 */
#define E60802_RG_UART_TX_OE                      (0x1<<17) /* 17:17 */
#define E60802_RG_UART_EN                         (0x1<<16) /* 16:16 */
#define E60802_RG_IP_U2_PORT_POWER                (0x1<<15) /* 15:15 */
#define E60802_FORCE_IP_U2_PORT_POWER             (0x1<<14) /* 14:14 */
#define E60802_FORCE_VBUSVALID                    (0x1<<13) /* 13:13 */
#define E60802_FORCE_SESSEND                      (0x1<<12) /* 12:12 */
#define E60802_FORCE_BVALID                       (0x1<<11) /* 11:11 */
#define E60802_FORCE_AVALID                       (0x1<<10) /* 10:10 */
#define E60802_FORCE_IDDIG                        (0x1<<9) /* 9:9 */
#define E60802_FORCE_IDPULLUP                     (0x1<<8) /* 8:8 */
#define E60802_RG_VBUSVALID                       (0x1<<5) /* 5:5 */
#define E60802_RG_SESSEND                         (0x1<<4) /* 4:4 */
#define E60802_RG_BVALID                          (0x1<<3) /* 3:3 */
#define E60802_RG_AVALID                          (0x1<<2) /* 2:2 */
#define E60802_RG_IDDIG                           (0x1<<1) /* 1:1 */
#define E60802_RG_IDPULLUP                        (0x1<<0) /* 0:0 */

/* U3D_U2PHYDMON0 */
#define E60802_RG_USB20_PRBS7_BERTH               (0xff<<0) /* 7:0 */

/* U3D_U2PHYDMON1 */
#define E60802_USB20_UART_O                       (0x1<<31) /* 31:31 */
#define E60802_RGO_USB20_LB_PASS                  (0x1<<30) /* 30:30 */
#define E60802_RGO_USB20_LB_DONE                  (0x1<<29) /* 29:29 */
#define E60802_AD_USB20_BVALID                    (0x1<<28) /* 28:28 */
#define E60802_USB20_IDDIG                        (0x1<<27) /* 27:27 */
#define E60802_AD_USB20_VBUSVALID                 (0x1<<26) /* 26:26 */
#define E60802_AD_USB20_SESSEND                   (0x1<<25) /* 25:25 */
#define E60802_AD_USB20_AVALID                    (0x1<<24) /* 24:24 */
#define E60802_USB20_LINE_STATE                   (0x3<<22) /* 23:22 */
#define E60802_USB20_HST_DISCON                   (0x1<<21) /* 21:21 */
#define E60802_USB20_TX_READY                     (0x1<<20) /* 20:20 */
#define E60802_USB20_RX_ERROR                     (0x1<<19) /* 19:19 */
#define E60802_USB20_RX_ACTIVE                    (0x1<<18) /* 18:18 */
#define E60802_USB20_RX_VALIDH                    (0x1<<17) /* 17:17 */
#define E60802_USB20_RX_VALID                     (0x1<<16) /* 16:16 */
#define E60802_USB20_DATA_OUT                     (0xffff<<0) /* 15:0 */

/* U3D_U2PHYDMON2 */
#define E60802_RGO_TXVALID_CNT                    (0xff<<24) /* 31:24 */
#define E60802_RGO_RXACTIVE_CNT                   (0xff<<16) /* 23:16 */
#define E60802_RGO_USB20_LB_BERCNT                (0xff<<8) /* 15:8 */
#define E60802_USB20_PROBE_OUT                    (0xff<<0) /* 7:0 */

/* U3D_U2PHYDMON3 */
#define E60802_RGO_USB20_PRBS7_ERRCNT             (0xffff<<16) /* 31:16 */
#define E60802_RGO_USB20_PRBS7_DONE               (0x1<<3) /* 3:3 */
#define E60802_RGO_USB20_PRBS7_LOCK               (0x1<<2) /* 2:2 */
#define E60802_RGO_USB20_PRBS7_PASS               (0x1<<1) /* 1:1 */
#define E60802_RGO_USB20_PRBS7_PASSTH             (0x1<<0) /* 0:0 */

/* U3D_U2PHYBC12C */
#define E60802_RG_SIFSLV_CHGDT_DEGLCH_CNT         (0xf<<28) /* 31:28 */
#define E60802_RG_SIFSLV_CHGDT_CTRL_CNT           (0xf<<24) /* 27:24 */
#define E60802_RG_SIFSLV_CHGDT_FORCE_MODE         (0x1<<16) /* 16:16 */
#define E60802_RG_CHGDT_ISRC_LEV                  (0x3<<14) /* 15:14 */
#define E60802_RG_CHGDT_VDATSRC                   (0x1<<13) /* 13:13 */
#define E60802_RG_CHGDT_BGVREF_SEL                (0x7<<10) /* 12:10 */
#define E60802_RG_CHGDT_RDVREF_SEL                (0x3<<8) /* 9:8 */
#define E60802_RG_CHGDT_ISRC_DP                   (0x1<<7) /* 7:7 */
#define E60802_RG_SIFSLV_CHGDT_OPOUT_DM           (0x1<<6) /* 6:6 */
#define E60802_RG_CHGDT_VDAT_DM                   (0x1<<5) /* 5:5 */
#define E60802_RG_CHGDT_OPOUT_DP                  (0x1<<4) /* 4:4 */
#define E60802_RG_SIFSLV_CHGDT_VDAT_DP            (0x1<<3) /* 3:3 */
#define E60802_RG_SIFSLV_CHGDT_COMP_EN            (0x1<<2) /* 2:2 */
#define E60802_RG_SIFSLV_CHGDT_OPDRV_EN           (0x1<<1) /* 1:1 */
#define E60802_RG_CHGDT_EN                        (0x1<<0) /* 0:0 */

/* U3D_U2PHYBC12C1 */
#define E60802_RG_CHGDT_REV                       (0xff<<0) /* 7:0 */

/* U3D_REGFPPC */
#define E60802_USB11_OTG_REG                      (0x1<<4) /* 4:4 */
#define E60802_USB20_OTG_REG                      (0x1<<3) /* 3:3 */
#define E60802_CHGDT_REG                          (0x1<<2) /* 2:2 */
#define E60802_USB11_REG                          (0x1<<1) /* 1:1 */
#define E60802_USB20_REG                          (0x1<<0) /* 0:0 */

/* U3D_VERSIONC */
#define E60802_VERSION_CODE_REGFILE               (0xff<<24) /* 31:24 */
#define E60802_USB11_VERSION_CODE                 (0xff<<16) /* 23:16 */
#define E60802_VERSION_CODE_ANA                   (0xff<<8) /* 15:8 */
#define E60802_VERSION_CODE_DIG                   (0xff<<0) /* 7:0 */

/* U3D_REGFCOM */
#define E60802_RG_PAGE                            (0xff<<24) /* 31:24 */
#define E60802_I2C_MODE                           (0x1<<16) /* 16:16 */
