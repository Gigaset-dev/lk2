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

/* come from CODA mipi_tx_config H file */

#define MIPITX_LANE_CON                 (0x000CUL)
#define MIPITX_VOLTAGE_SEL              (0x0010UL)
#define MIPITX_PRESERVED                (0x0014UL)
#define MIPITX_TEST_CON                 (0x0018UL)
#define MIPITX_PLL_PWR                  (0x0028UL)
#define MIPITX_PLL_CON0                 (0x002CUL)
#define MIPITX_PLL_CON1                 (0x0030UL)
#define MIPITX_PLL_CON2                 (0x0034UL)
#define MIPITX_PLL_CON3                 (0x0038UL)
#define MIPITX_PLL_CON4                 (0x003CUL)
#define MIPITX_PHY_SEL0                 (0x0040UL)
#define MIPITX_PHY_SEL1                 (0x0044UL)
#define MIPITX_PHY_SEL2                 (0x0048UL)
#define MIPITX_PHY_SEL3                 (0x004CUL)
#define MIPITX_SW_CTRL_CON4             (0x0060UL)
#define MIPITX_CD_CON                   (0x0068UL)
#define MIPITX_DBG_CON                  (0x0070UL)
#define MIPITX_APB_ASYNC_STA            (0x0078UL)
#define MIPITX_D2P_RTCODE0              (0x0100UL)
#define MIPITX_D2P_RTCODE1              (0x0104UL)
#define MIPITX_D2P_RTCODE2              (0x0108UL)
#define MIPITX_D2P_RTCODE3              (0x010CUL)
#define MIPITX_D2P_RTCODE4              (0x0110UL)
#define MIPITX_D2N_RTCODE0              (0x0114UL)
#define MIPITX_D2N_RTCODE1              (0x0118UL)
#define MIPITX_D2N_RTCODE2              (0x011CUL)
#define MIPITX_D2N_RTCODE3              (0x0120UL)
#define MIPITX_D2N_RTCODE4              (0x0124UL)
#define MIPITX_D2_CKMODE_EN             (0x0128UL)
#define MIPITX_D2_ANA_PN_SWAP_EN        (0x012CUL)
#define MIPITX_D2P_T0A_TIEL_EN          (0x0130UL)
#define MIPITX_D2N_T0B_TIEL_EN          (0x0134UL)
#define MIPITX_D2_SER_BISTTOG           (0x0138UL)
#define MIPITX_D2_SER_DIN_SEL           (0x013CUL)
#define MIPITX_D2_DIG_PN_SWAP_EN        (0x0140UL)
#define MIPITX_D2_SW_CTL_EN             (0x0144UL)
#define MIPITX_D2_SW_LPTX_PRE_OE        (0x0148UL)
#define MIPITX_D2_SW_LPTX_OE            (0x014CUL)
#define MIPITX_D2_SW_LPTX_DP            (0x0150UL)
#define MIPITX_D2_SW_LPTX_DN            (0x0154UL)
#define MIPITX_D2_SW_LPRX_EN            (0x0158UL)
#define MIPITX_D2_SW_HSTX_PRE_OE        (0x015CUL)
#define MIPITX_D2_SW_HSTX_OE            (0x0160UL)
#define MIPITX_D2_SW_HSTX_RDY           (0x0164UL)
#define MIPITX_D2C_SW_LPTX_PRE_OE       (0x0168UL)
#define MIPITX_D2C_SW_LPTX_OE           (0x016CUL)
#define MIPITX_D2C_SW_LPRX_EN           (0x0170UL)
#define MIPITX_D2C_SW_HSTX_PRE_OE       (0x0174UL)
#define MIPITX_D2C_SW_HSTX_OE           (0x0178UL)
#define MIPITX_D2C_SW_HSTX_RDY          (0x017CUL)
#define MIPITX_D2_SW_HSTX_DATA0         (0x0180UL)
#define MIPITX_D2_SW_HSTX_DATA1         (0x0184UL)
#define MIPITX_D2_SW_HSTX_DATA2         (0x0188UL)
#define MIPITX_D2_SW_HSTX_DATA3         (0x018CUL)
#define MIPITX_D2_SW_HSTX_DATA4         (0x0190UL)
#define MIPITX_D2_SW_HSTX_DATA5         (0x0194UL)
#define MIPITX_D2_SW_HSTX_DATA6         (0x0198UL)
#define MIPITX_D2_SW_HSTX_DATA7         (0x019CUL)
#define MIPITX_D2C_SW_HSTX_DATA0        (0x01A0UL)
#define MIPITX_D2C_SW_HSTX_DATA1        (0x01A4UL)
#define MIPITX_D2C_SW_HSTX_DATA2        (0x01A8UL)
#define MIPITX_D2C_SW_HSTX_DATA3        (0x01ACUL)
#define MIPITX_D2C_SW_HSTX_DATA4        (0x01B0UL)
#define MIPITX_D2C_SW_HSTX_DATA5        (0x01B4UL)
#define MIPITX_D2C_SW_HSTX_DATA6        (0x01B8UL)
#define MIPITX_D2_SW_CD_CON             (0x01BCUL)
#define MIPITX_D2_AD_RX                 (0x01C0UL)
#define MIPITX_D2_AD_CD                 (0x01C4UL)
#define MIPITX_D0P_RTCODE0              (0x0200UL)
#define MIPITX_D0P_RTCODE1              (0x0204UL)
#define MIPITX_D0P_RTCODE2              (0x0208UL)
#define MIPITX_D0P_RTCODE3              (0x020CUL)
#define MIPITX_D0P_RTCODE4              (0x0210UL)
#define MIPITX_D0N_RTCODE0              (0x0214UL)
#define MIPITX_D0N_RTCODE1              (0x0218UL)
#define MIPITX_D0N_RTCODE2              (0x021CUL)
#define MIPITX_D0N_RTCODE3              (0x0220UL)
#define MIPITX_D0N_RTCODE4              (0x0224UL)
#define MIPITX_D0_CKMODE_EN             (0x0228UL)
#define MIPITX_D0_ANA_PN_SWAP_EN        (0x022CUL)
#define MIPITX_D0P_T0A_TIEL_EN          (0x0230UL)
#define MIPITX_D0N_T0B_TIEL_EN          (0x0234UL)
#define MIPITX_D0_SER_BISTTOG           (0x0238UL)
#define MIPITX_D0_SER_DIN_SEL           (0x023CUL)
#define MIPITX_D0_DIG_PN_SWAP_EN        (0x0240UL)
#define MIPITX_D0_SW_CTL_EN             (0x0244UL)
#define MIPITX_D0_SW_LPTX_PRE_OE        (0x0248UL)
#define MIPITX_D0_SW_LPTX_OE            (0x024CUL)
#define MIPITX_D0_SW_LPTX_DP            (0x0250UL)
#define MIPITX_D0_SW_LPTX_DN            (0x0254UL)
#define MIPITX_D0_SW_LPRX_EN            (0x0258UL)
#define MIPITX_D0_SW_HSTX_PRE_OE        (0x025CUL)
#define MIPITX_D0_SW_HSTX_OE            (0x0260UL)
#define MIPITX_D0_SW_HSTX_RDY           (0x0264UL)
#define MIPITX_D0C_SW_LPTX_PRE_OE       (0x0268UL)
#define MIPITX_D0C_SW_LPTX_OE           (0x026CUL)
#define MIPITX_D0C_SW_LPRX_EN           (0x0270UL)
#define MIPITX_D0C_SW_HSTX_PRE_OE       (0x0274UL)
#define MIPITX_D0C_SW_HSTX_OE           (0x0278UL)
#define MIPITX_D0C_SW_HSTX_RDY          (0x027CUL)
#define MIPITX_D0_SW_HSTX_DATA0         (0x0280UL)
#define MIPITX_D0_SW_HSTX_DATA1         (0x0284UL)
#define MIPITX_D0_SW_HSTX_DATA2         (0x0288UL)
#define MIPITX_D0_SW_HSTX_DATA3         (0x028CUL)
#define MIPITX_D0_SW_HSTX_DATA4         (0x0290UL)
#define MIPITX_D0_SW_HSTX_DATA5         (0x0294UL)
#define MIPITX_D0_SW_HSTX_DATA6         (0x0298UL)
#define MIPITX_D0_SW_HSTX_DATA7         (0x029CUL)
#define MIPITX_D0C_SW_HSTX_DATA0        (0x02A0UL)
#define MIPITX_D0C_SW_HSTX_DATA1        (0x02A4UL)
#define MIPITX_D0C_SW_HSTX_DATA2        (0x02A8UL)
#define MIPITX_D0C_SW_HSTX_DATA3        (0x02ACUL)
#define MIPITX_D0C_SW_HSTX_DATA4        (0x02B0UL)
#define MIPITX_D0C_SW_HSTX_DATA5        (0x02B4UL)
#define MIPITX_D0C_SW_HSTX_DATA6        (0x02B8UL)
#define MIPITX_D0_SW_CD_CON             (0x02BCUL)
#define MIPITX_D0_AD_RX                 (0x02C0UL)
#define MIPITX_D0_AD_CD                 (0x02C4UL)
#define MIPITX_CKP_RTCODE0              (0x0300UL)
#define MIPITX_CKP_RTCODE1              (0x0304UL)
#define MIPITX_CKP_RTCODE2              (0x0308UL)
#define MIPITX_CKP_RTCODE3              (0x030CUL)
#define MIPITX_CKP_RTCODE4              (0x0310UL)
#define MIPITX_CKN_RTCODE0              (0x0314UL)
#define MIPITX_CKN_RTCODE1              (0x0318UL)
#define MIPITX_CKN_RTCODE2              (0x031CUL)
#define MIPITX_CKN_RTCODE3              (0x0320UL)
#define MIPITX_CKN_RTCODE4              (0x0324UL)
#define MIPITX_CK_CKMODE_EN             (0x0328UL)
#define MIPITX_CK_ANA_PN_SWAP_EN        (0x032CUL)
#define MIPITX_CKP_T0A_TIEL_EN          (0x0330UL)
#define MIPITX_CKN_T0B_TIEL_EN          (0x0334UL)
#define MIPITX_CK_SER_BISTTOG               (0x0338UL)
#define MIPITX_CK_SER_DIN_SEL               (0x033CUL)
#define MIPITX_CK_DIG_PN_SWAP_EN            (0x0340UL)
#define MIPITX_CK_SW_CTL_EN                 (0x0344UL)
#define MIPITX_CK_SW_LPTX_PRE_OE            (0x0348UL)
#define MIPITX_CK_SW_LPTX_OE                (0x034CUL)
#define MIPITX_CK_SW_LPTX_DP                (0x0350UL)
#define MIPITX_CK_SW_LPTX_DN                (0x0354UL)
#define MIPITX_CK_SW_LPRX_EN                (0x0358UL)
#define MIPITX_CK_SW_HSTX_PRE_OE            (0x035CUL)
#define MIPITX_CK_SW_HSTX_OE                (0x0360UL)
#define MIPITX_CK_SW_HSTX_RDY               (0x0364UL)
#define MIPITX_CKC_SW_LPTX_PRE_OE           (0x0368UL)
#define MIPITX_CKC_SW_LPTX_OE               (0x036CUL)
#define MIPITX_CKC_SW_LPRX_EN               (0x0370UL)
#define MIPITX_CKC_SW_HSTX_PRE_OE           (0x0374UL)
#define MIPITX_CKC_SW_HSTX_OE               (0x0378UL)
#define MIPITX_CKC_SW_HSTX_RDY              (0x037CUL)
#define MIPITX_CK_SW_HSTX_DATA0             (0x0380UL)
#define MIPITX_CK_SW_HSTX_DATA1             (0x0384UL)
#define MIPITX_CK_SW_HSTX_DATA2             (0x0388UL)
#define MIPITX_CK_SW_HSTX_DATA3             (0x038CUL)
#define MIPITX_CK_SW_HSTX_DATA4             (0x0390UL)
#define MIPITX_CK_SW_HSTX_DATA5             (0x0394UL)
#define MIPITX_CK_SW_HSTX_DATA6             (0x0398UL)
#define MIPITX_CK_SW_HSTX_DATA7             (0x039CUL)
#define MIPITX_CKC_SW_HSTX_DATA0            (0x03A0UL)
#define MIPITX_CKC_SW_HSTX_DATA1            (0x03A4UL)
#define MIPITX_CKC_SW_HSTX_DATA2            (0x03A8UL)
#define MIPITX_CKC_SW_HSTX_DATA3            (0x03ACUL)
#define MIPITX_CKC_SW_HSTX_DATA4            (0x03B0UL)
#define MIPITX_CKC_SW_HSTX_DATA5            (0x03B4UL)
#define MIPITX_CKC_SW_HSTX_DATA6            (0x03B8UL)
#define MIPITX_CK_SW_CD_CON                 (0x03BCUL)
#define MIPITX_CK_AD_RX                     (0x03C0UL)
#define MIPITX_CK_AD_CD                     (0x03C4UL)
#define MIPITX_D1P_RTCODE0              (0x0400UL)
#define MIPITX_D1P_RTCODE1              (0x0404UL)
#define MIPITX_D1P_RTCODE2              (0x0408UL)
#define MIPITX_D1P_RTCODE3              (0x040CUL)
#define MIPITX_D1P_RTCODE4              (0x0410UL)
#define MIPITX_D1N_RTCODE0              (0x0414UL)
#define MIPITX_D1N_RTCODE1              (0x0418UL)
#define MIPITX_D1N_RTCODE2              (0x041CUL)
#define MIPITX_D1N_RTCODE3              (0x0420UL)
#define MIPITX_D1N_RTCODE4              (0x0424UL)
#define MIPITX_D1_CKMODE_EN             (0x0428UL)
#define MIPITX_D1_ANA_PN_SWAP_EN        (0x042CUL)
#define MIPITX_D1P_T0A_TIEL_EN          (0x0430UL)
#define MIPITX_D1N_T0B_TIEL_EN          (0x0434UL)
#define MIPITX_D1_SER_BISTTOG               (0x0438UL)
#define MIPITX_D1_SER_DIN_SEL               (0x043CUL)
#define MIPITX_D1_DIG_PN_SWAP_EN            (0x0440UL)
#define MIPITX_D1_SW_CTL_EN                 (0x0444UL)
#define MIPITX_D1_SW_LPTX_PRE_OE            (0x0448UL)
#define MIPITX_D1_SW_LPTX_OE                (0x044CUL)
#define MIPITX_D1_SW_LPTX_DP                (0x0450UL)
#define MIPITX_D1_SW_LPTX_DN                (0x0454UL)
#define MIPITX_D1_SW_LPRX_EN                (0x0458UL)
#define MIPITX_D1_SW_HSTX_PRE_OE            (0x045CUL)
#define MIPITX_D1_SW_HSTX_OE                (0x0460UL)
#define MIPITX_D1_SW_HSTX_RDY               (0x0464UL)
#define MIPITX_D1C_SW_LPTX_PRE_OE           (0x0468UL)
#define MIPITX_D1C_SW_LPTX_OE               (0x046CUL)
#define MIPITX_D1C_SW_LPRX_EN               (0x0470UL)
#define MIPITX_D1C_SW_HSTX_PRE_OE           (0x0474UL)
#define MIPITX_D1C_SW_HSTX_OE               (0x0478UL)
#define MIPITX_D1C_SW_HSTX_RDY              (0x047CUL)
#define MIPITX_D1_SW_HSTX_DATA0             (0x0480UL)
#define MIPITX_D1_SW_HSTX_DATA1             (0x0484UL)
#define MIPITX_D1_SW_HSTX_DATA2             (0x0488UL)
#define MIPITX_D1_SW_HSTX_DATA3             (0x048CUL)
#define MIPITX_D1_SW_HSTX_DATA4             (0x0490UL)
#define MIPITX_D1_SW_HSTX_DATA5             (0x0494UL)
#define MIPITX_D1_SW_HSTX_DATA6             (0x0498UL)
#define MIPITX_D1_SW_HSTX_DATA7             (0x049CUL)
#define MIPITX_D1C_SW_HSTX_DATA0            (0x04A0UL)
#define MIPITX_D1C_SW_HSTX_DATA1            (0x04A4UL)
#define MIPITX_D1C_SW_HSTX_DATA2            (0x04A8UL)
#define MIPITX_D1C_SW_HSTX_DATA3            (0x04ACUL)
#define MIPITX_D1C_SW_HSTX_DATA4            (0x04B0UL)
#define MIPITX_D1C_SW_HSTX_DATA5            (0x04B4UL)
#define MIPITX_D1C_SW_HSTX_DATA6            (0x04B8UL)
#define MIPITX_D1_SW_CD_CON             (0x04BCUL)
#define MIPITX_D1_AD_RX                 (0x04C0UL)
#define MIPITX_D1_AD_CD                 (0x04C4UL)
#define MIPITX_D3P_RTCODE0              (0x0500UL)
#define MIPITX_D3P_RTCODE1              (0x0504UL)
#define MIPITX_D3P_RTCODE2              (0x0508UL)
#define MIPITX_D3P_RTCODE3              (0x050CUL)
#define MIPITX_D3P_RTCODE4              (0x0510UL)
#define MIPITX_D3N_RTCODE0              (0x0514UL)
#define MIPITX_D3N_RTCODE1              (0x0518UL)
#define MIPITX_D3N_RTCODE2              (0x051CUL)
#define MIPITX_D3N_RTCODE3              (0x0520UL)
#define MIPITX_D3N_RTCODE4              (0x0524UL)
#define MIPITX_D3_CKMODE_EN             (0x0528UL)
#define MIPITX_D3_ANA_PN_SWAP_EN        (0x052CUL)
#define MIPITX_D3P_T0A_TIEL_EN          (0x0530UL)
#define MIPITX_D3N_T0B_TIEL_EN          (0x0534UL)
#define MIPITX_D3_SER_BISTTOG               (0x0538UL)
#define MIPITX_D3_SER_DIN_SEL               (0x053CUL)
#define MIPITX_D3_DIG_PN_SWAP_EN            (0x0540UL)
#define MIPITX_D3_SW_CTL_EN                 (0x0544UL)
#define MIPITX_D3_SW_LPTX_PRE_OE            (0x0548UL)
#define MIPITX_D3_SW_LPTX_OE                (0x054CUL)
#define MIPITX_D3_SW_LPTX_DP                (0x0550UL)
#define MIPITX_D3_SW_LPTX_DN                (0x0554UL)
#define MIPITX_D3_SW_LPRX_EN                (0x0558UL)
#define MIPITX_D3_SW_HSTX_PRE_OE            (0x055CUL)
#define MIPITX_D3_SW_HSTX_OE                (0x0560UL)
#define MIPITX_D3_SW_HSTX_RDY               (0x0564UL)
#define MIPITX_D3C_SW_LPTX_PRE_OE           (0x0568UL)
#define MIPITX_D3C_SW_LPTX_OE               (0x056CUL)
#define MIPITX_D3C_SW_LPRX_EN               (0x0570UL)
#define MIPITX_D3C_SW_HSTX_PRE_OE           (0x0574UL)
#define MIPITX_D3C_SW_HSTX_OE               (0x0578UL)
#define MIPITX_D3C_SW_HSTX_RDY              (0x057CUL)
#define MIPITX_D3_SW_HSTX_DATA0             (0x0580UL)
#define MIPITX_D3_SW_HSTX_DATA1             (0x0584UL)
#define MIPITX_D3_SW_HSTX_DATA2             (0x0588UL)
#define MIPITX_D3_SW_HSTX_DATA3             (0x058CUL)
#define MIPITX_D3_SW_HSTX_DATA4             (0x0590UL)
#define MIPITX_D3_SW_HSTX_DATA5             (0x0594UL)
#define MIPITX_D3_SW_HSTX_DATA6             (0x0598UL)
#define MIPITX_D3_SW_HSTX_DATA7             (0x059CUL)
#define MIPITX_D3_SW_CD_CON                     (0x05BCUL)
#define MIPITX_D3_AD_RX                         (0x05C0UL)
#define MIPITX_D3_AD_CD                         (0x05C4UL)
#define MIPITX_DPHY_BIST_CON0                   (0x0600UL)
#define MIPITX_DPHY_BIST_CON1                   (0x0604UL)
#define MIPITX_DPHY_BIST_LANE_NUM               (0x0608UL)
#define MIPITX_DPHY_BIST_TIMING_0               (0x060CUL)
#define MIPITX_DPHY_BIST_TIMING_1               (0x0610UL)
#define MIPITX_DPHY_BIST_PATTERN_0              (0x0614UL)
#define MIPITX_DPHY_BIST_PATTERN_1              (0x0618UL)
#define MIPITX_DPHY_BIST_PATTERN_2              (0x061CUL)
#define MIPITX_DPHY_BIST_PATTERN_3              (0x0620UL)
#define MIPITX_DPHY_BIST_PATTERN_4              (0x0624UL)
#define MIPITX_DPHY_BIST_PATTERN_5              (0x0628UL)
#define MIPITX_DPHY_BIST_PATTERN_6              (0x062CUL)
#define MIPITX_DPHY_BIST_PATTERN_7              (0x0630UL)
#define MIPITX_CPHY_BIST_CON0                   (0x0634UL)
#define MIPITX_CPHY_BIST_CON1                   (0x0638UL)
#define MIPITX_CPHY_BIST_WIRE_STATE             (0x063CUL)
#define MIPITX_CPHY_BIST_PRB_SEL                (0x0640UL)
#define MIPITX_CPHY_BIST_POST_CNT_0             (0x0644UL)
#define MIPITX_CPHY_BIST_POST_CNT_1             (0x0648UL)
#define MIPITX_CPHY_BIST_PREBEGIN_CNT_0         (0x064CUL)
#define MIPITX_CPHY_BIST_PREBEGIN_CNT_1         (0x0650UL)
#define MIPITX_CPHY_BIST_PROGSEQ_MSB_0          (0x0654UL)
#define MIPITX_CPHY_BIST_PROGSEQ_MSB_1          (0x0658UL)
#define MIPITX_CPHY_BIST_PROGSEQ_MSB_2          (0x065CUL)
#define MIPITX_CPHY_BIST_PROGSEQ_MSB_3          (0x0660UL)
#define MIPITX_CPHY_BIST_PROGSEQ_MSB_4          (0x0664UL)
#define MIPITX_CPHY_BIST_PROGSEQ_MSB_5          (0x0668UL)
#define MIPITX_CPHY_BIST_PROGSEQ_MSB_6          (0x066CUL)
#define MIPITX_CPHY_BIST_PROGSEQ_MSB_7          (0x0670UL)
#define MIPITX_CPHY_BIST_PROGSEQ_LSB_0          (0x0674UL)
#define MIPITX_CPHY_BIST_PROGSEQ_LSB_1          (0x0678UL)
#define MIPITX_CPHY_BIST_PROGSEQ_LSB_2          (0x067CUL)
#define MIPITX_CPHY_BIST_PRB_SEED_0             (0x0680UL)
#define MIPITX_CPHY_BIST_PRB_SEED_1             (0x0684UL)
#define MIPITX_CPHY_BIST_PRB_SEED_2             (0x0688UL)
#define MIPITX_CPHY_BIST_PRB_SEED_3             (0x068CUL)
#define MIPITX_CPHY_BIST_PRB_SEED_4             (0x0690UL)


/* field definition */
#define FLD_RG_DSI_PAD_TIEL_SEL                REG_FLD(1, 8)
#define FLD_RG_DSI_BG_CORE_EN                  REG_FLD(1, 7)
#define FLD_RG_DSI_BG_LPF_EN                   REG_FLD(1, 6)
#define FLD_RG_DSI_PWR04_EN                    REG_FLD(1, 5)
#define FLD_RG_DSI_PHYCK_INV_EN                REG_FLD(1, 4)
#define FLD_RG_DSI_CPHY_EN                     REG_FLD(1, 3)
#define FLD_RG_DSI_PHY_CK_SEL                  REG_FLD(1, 2)
#define FLD_RG_DSI_ANA_CK_SEL                  REG_FLD(1, 1)
#define FLD_RG_DSI_CPHY_T1DRV_EN               REG_FLD(1, 0)

#define FLD_RG_DSI_LPCD_VTL_SEL             REG_FLD(3, 28)
#define FLD_RG_DSI_LPCD_VTH_SEL             REG_FLD(3, 24)
#define FLD_RG_DSI_LPRX_VTL_SEL             REG_FLD(3, 20)
#define FLD_RG_DSI_LPRX_VTH_SEL             REG_FLD(3, 16)
#define FLD_RG_DSI_V2I_REF_SEL              REG_FLD(4, 10)
#define FLD_RG_DSI_HSTX_LDO_REF_SEL         REG_FLD(4, 6)
#define FLD_RG_DSI_PRD_REF_SEL              REG_FLD(6, 0)

#define FLD_RG_DSI_PRESERVE1                  REG_FLD(16, 16)
#define FLD_RG_DSI_PRESERVE0                  REG_FLD(16, 0)

#define FLD_RG_DSI_TEST_SEL                    REG_FLD(4, 28)
#define FLD_RG_DSI_TEST_DIV                    REG_FLD(2, 26)
#define FLD_RG_DSI_MON_DIGCK_EN                REG_FLD(1, 25)
#define FLD_RG_DSI_MON_BYTECK_EN               REG_FLD(1, 24)
#define FLD_RG_DSI_SER_DATA                    REG_FLD(8, 16)

#define FLD_DA_DSI_PLL_SDM_PWR_ACK              REG_FLD(1, 8)
#define FLD_AD_DSI_PLL_SDM_ISO_EN               REG_FLD(1, 1)
#define FLD_AD_DSI_PLL_SDM_PWR_ON               REG_FLD(1, 0)

#define FLD_RG_DSI_PLL_SDM_PCW                 REG_FLD(32, 0)

#define FLD_RG_DSI_PLL_RST_DLY                 REG_FLD(2, 20)
#define FLD_RG_DSI_PLL_BLP                     REG_FLD(1, 18)
#define FLD_RG_DSI_PLL_BR                      REG_FLD(1, 17)
#define FLD_RG_DSI_PLL_BP                      REG_FLD(1, 16)
#define FLD_RG_DSI_PLL_LVROD_EN                REG_FLD(1, 15)
#define FLD_RG_DSI_PLL_SDM_HREN                REG_FLD(1, 14)
#define FLD_RG_DSI_PLL_SDM_FRA_EN              REG_FLD(1, 13)
#define FLD_RG_DSI_PLL_PREDIV                  REG_FLD(2, 11)
#define FLD_RG_DSI_PLL_POSDIV                  REG_FLD(3, 8)
#define FLD_RG_DSI_PLL_EN_V18                  REG_FLD(1, 5)
#define FLD_RG_DSI_PLL_EN                      REG_FLD(1, 4)
#define FLD_RG_DSI_PLL_SDM_PCW_CHG             REG_FLD(1, 0)

#define FLD_RG_DSI_PLL_SDM_SSC_PRD             REG_FLD(16, 16)
#define FLD_RG_DSI_PLL_SDM_SSC_EN              REG_FLD(1, 1)
#define FLD_RG_DSI_PLL_SDM_SSC_PH_INIT         REG_FLD(1, 0)

#define FLD_RG_DSI_PLL_SDM_SSC_DELTA           REG_FLD(16, 16)
#define FLD_RG_DSI_PLL_SDM_SSC_DELTA1          REG_FLD(16, 0)

#define FLD_RG_DSI_PLL_RESERVED                REG_FLD(16, 16)
#define FLD_RG_DSI_PLL_ICHP                    REG_FLD(2, 12)
#define FLD_RG_DSI_PLL_IBIAS                   REG_FLD(2, 10)
#define FLD_RG_DSI_PLL_FS                      REG_FLD(2, 8)
#define FLD_RG_DSI_PLL_BW                      REG_FLD(3, 5)
#define FLD_RG_DSI_PLL_MONREF_EN               REG_FLD(1, 4)
#define FLD_RG_DSI_PLL_MONVC_EN                REG_FLD(2, 2)
#define FLD_RG_DSI_PLL_MONCK_EN                REG_FLD(1, 0)

#define FLD_MIPI_TX_PHY1_SEL                   REG_FLD(4, 28)
#define FLD_MIPI_TX_CPHY1CA_SEL                REG_FLD(4, 24)
#define FLD_MIPI_TX_PHYC_SEL                   REG_FLD(4, 20)
#define FLD_MIPI_TX_PHY1AB_SEL                 REG_FLD(4, 16)
#define FLD_MIPI_TX_PHY0_SEL                   REG_FLD(4, 12)
#define FLD_MIPI_TX_CPHY0BC_SEL                REG_FLD(4, 8)
#define FLD_MIPI_TX_PHY2_SEL                   REG_FLD(4, 4)
#define FLD_MIPI_TX_CPHY_EN                    REG_FLD(1, 0)

#define FLD_MIPI_TX_CPHY2_HS_SEL               REG_FLD(2, 28)
#define FLD_MIPI_TX_CPHY1_HS_SEL               REG_FLD(2, 26)
#define FLD_MIPI_TX_CPHY0_HS_SEL               REG_FLD(2, 24)
#define FLD_MIPI_TX_LPRX0CA_SEL                REG_FLD(4, 20)
#define FLD_MIPI_TX_LPRX0BC_SEL                REG_FLD(4, 16)
#define FLD_MIPI_TX_LPRX0AB_SEL                REG_FLD(4, 12)
#define FLD_MIPI_TX_CPHYXXX_SEL                REG_FLD(4, 8)
#define FLD_MIPI_TX_PHY3_SEL                   REG_FLD(4, 4)
#define FLD_MIPI_TX_PHY2BC_SEL                 REG_FLD(4, 0)

#define FLD_MIPI_TX_PHY2BC_HSDATA_SEL          REG_FLD(4, 28)
#define FLD_MIPI_TX_PHY1_HSDATA_SEL            REG_FLD(4, 24)
#define FLD_MIPI_TX_CPHY1CA_HSDATA_SEL         REG_FLD(4, 20)
#define FLD_MIPI_TX_PHYC_HSDATA_SEL            REG_FLD(4, 16)
#define FLD_MIPI_TX_PHY1AB_HSDATA_SEL          REG_FLD(4, 12)
#define FLD_MIPI_TX_PHY0_HSDATA_SEL            REG_FLD(4, 8)
#define FLD_MIPI_TX_CPHY0BC_HSDATA_SEL         REG_FLD(4, 4)
#define FLD_MIPI_TX_PHY2_HSDATA_SEL            REG_FLD(4, 0)

#define FLD_MIPI_TX_PHY3_HSDATA_SEL            REG_FLD(4, 0)

#define FLD_MIPI_TX_SW_ANA_CK_EN           REG_FLD(1, 8)

#define FLD_MIPI_TX_CD_CON                       REG_FLD(10, 0)

#define FLD_MIPI_TX_APB_ASYNC_CNT_EN            REG_FLD(1, 6)
#define FLD_MIPI_TX_GPIO_MODE_EN                REG_FLD(1, 5)
#define FLD_MIPI_TX_DBG_OUT_EN                  REG_FLD(1, 4)

#define FLD_MIPI_TX_APB_ASYNC_ERR_ADDR    REG_FLD(10, 1)
#define FLD_MIPI_TX_APB_ASYNC_ERR         REG_FLD(1, 0)

#define FLD_DSI_D2P_RTCODE0                 REG_FLD(1, 0)

#define FLD_DSI_D2P_RTCODE1                 REG_FLD(1, 0)

#define FLD_DSI_D2P_RTCODE2                 REG_FLD(1, 0)

#define FLD_DSI_D2P_RTCODE3                 REG_FLD(1, 0)

#define FLD_DSI_D2P_RTCODE4                 REG_FLD(1, 0)

#define FLD_DSI_D2N_RTCODE0                 REG_FLD(1, 0)

#define FLD_DSI_D2N_RTCODE1                 REG_FLD(1, 0)

#define FLD_DSI_D2N_RTCODE2                 REG_FLD(1, 0)

#define FLD_DSI_D2N_RTCODE3                 REG_FLD(1, 0)

#define FLD_DSI_D2N_RTCODE4                 REG_FLD(1, 0)

#define FLD_DSI_D2_CKMODE_EN               REG_FLD(1, 0)

#define FLD_DSI_D2_ANA_PN_SWAP_EN     REG_FLD(1, 0)

#define FLD_DSI_D2P_T0A_TIEL_EN         REG_FLD(1, 0)

#define FLD_DSI_D2N_T0B_TIEL_EN         REG_FLD(1, 0)

#define FLD_DSI_D2_SER_BISTTOG           REG_FLD(1, 0)

#define FLD_DSI_D2_SER_DIN_SEL           REG_FLD(1, 0)

#define FLD_DSI_D2_DIG_PN_SWAP_EN     REG_FLD(1, 0)

#define FLD_DSI_D2_SW_CTL_EN               REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D2_SW_LPTX_PRE_OE REG_FLD(1, 8)
#define FLD_DSI_D2_SW_LPTX_PRE_OE     REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D2_SW_LPTX_OE     REG_FLD(1, 8)
#define FLD_DSI_D2_SW_LPTX_OE             REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D2_SW_LPTX_DP     REG_FLD(1, 8)
#define FLD_DSI_D2_SW_LPTX_DP             REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D2_SW_LPTX_DN     REG_FLD(1, 8)
#define FLD_DSI_D2_SW_LPTX_DN             REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D2_SW_LPRX_EN     REG_FLD(1, 8)
#define FLD_DSI_D2_SW_LPRX_EN             REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D2_SW_HSTX_PRE_OE REG_FLD(1, 8)
#define FLD_DSI_D2_SW_HSTX_PRE_OE     REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D2_SW_HSTX_OE     REG_FLD(1, 8)
#define FLD_DSI_D2_SW_HSTX_OE             REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D2_SW_HSTX_RDY   REG_FLD(1, 8)
#define FLD_DSI_D2_SW_HSTX_RDY           REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D2C_SW_LPTX_PRE_OE REG_FLD(1, 8)
#define FLD_DSI_D2C_SW_LPTX_PRE_OE   REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D2C_SW_LPTX_OE   REG_FLD(1, 8)
#define FLD_DSI_D2C_SW_LPTX_OE           REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D2C_SW_LPRX_EN   REG_FLD(1, 8)
#define FLD_DSI_D2C_SW_LPRX_EN           REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D2C_SW_HSTX_PRE_OE REG_FLD(1, 8)
#define FLD_DSI_D2C_SW_HSTX_PRE_OE   REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D2C_SW_HSTX_OE   REG_FLD(1, 8)
#define FLD_DSI_D2C_SW_HSTX_OE           REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D2C_SW_HSTX_RDY REG_FLD(1, 8)
#define FLD_DSI_D2C_SW_HSTX_RDY         REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D2_SW_HSTX_DATA0 REG_FLD(1, 8)
#define FLD_DSI_D2_SW_HSTX_DATA0       REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D2_SW_HSTX_DATA1 REG_FLD(1, 8)
#define FLD_DSI_D2_SW_HSTX_DATA1       REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D2_SW_HSTX_DATA2 REG_FLD(1, 8)
#define FLD_DSI_D2_SW_HSTX_DATA2       REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D2_SW_HSTX_DATA3 REG_FLD(1, 8)
#define FLD_DSI_D2_SW_HSTX_DATA3       REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D2_SW_HSTX_DATA4 REG_FLD(1, 8)
#define FLD_DSI_D2_SW_HSTX_DATA4       REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D2_SW_HSTX_DATA5 REG_FLD(1, 8)
#define FLD_DSI_D2_SW_HSTX_DATA5       REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D2_SW_HSTX_DATA6 REG_FLD(1, 8)
#define FLD_DSI_D2_SW_HSTX_DATA6       REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D2_SW_HSTX_DATA7 REG_FLD(1, 8)
#define FLD_DSI_D2_SW_HSTX_DATA7       REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D2C_SW_HSTX_DATA0 REG_FLD(1, 8)
#define FLD_DSI_D2C_SW_HSTX_DATA0     REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D2C_SW_HSTX_DATA1 REG_FLD(1, 8)
#define FLD_DSI_D2C_SW_HSTX_DATA1     REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D2C_SW_HSTX_DATA2 REG_FLD(1, 8)
#define FLD_DSI_D2C_SW_HSTX_DATA2     REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D2C_SW_HSTX_DATA3 REG_FLD(1, 8)
#define FLD_DSI_D2C_SW_HSTX_DATA3     REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D2C_SW_HSTX_DATA4 REG_FLD(1, 8)
#define FLD_DSI_D2C_SW_HSTX_DATA4     REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D2C_SW_HSTX_DATA5 REG_FLD(1, 8)
#define FLD_DSI_D2C_SW_HSTX_DATA5     REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D2C_SW_HSTX_DATA6 REG_FLD(1, 8)
#define FLD_DSI_D2C_SW_HSTX_DATA6     REG_FLD(1, 0)

#define FLD_DSI_D2_SW_CD_CON               REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D2_AD_LPRX_N           REG_FLD(1, 8)
#define FLD_DBG_OUT_DSI_D2_AD_LPRX_P           REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D2_AD_LPCD_N           REG_FLD(1, 8)
#define FLD_DBG_OUT_DSI_D2_AD_LPCD_P           REG_FLD(1, 0)

#define FLD_DSI_D0P_RTCODE0                 REG_FLD(1, 0)

#define FLD_DSI_D0P_RTCODE1                 REG_FLD(1, 0)

#define FLD_DSI_D0P_RTCODE2                 REG_FLD(1, 0)

#define FLD_DSI_D0P_RTCODE3                 REG_FLD(1, 0)

#define FLD_DSI_D0P_RTCODE4                 REG_FLD(1, 0)

#define FLD_DSI_D0N_RTCODE0                 REG_FLD(1, 0)

#define FLD_DSI_D0N_RTCODE1                 REG_FLD(1, 0)

#define FLD_DSI_D0N_RTCODE2                 REG_FLD(1, 0)

#define FLD_DSI_D0N_RTCODE3                 REG_FLD(1, 0)

#define FLD_DSI_D0N_RTCODE4                 REG_FLD(1, 0)

#define FLD_DSI_D0_CKMODE_EN               REG_FLD(1, 0)

#define FLD_DSI_D0_ANA_PN_SWAP_EN     REG_FLD(1, 0)

#define FLD_DSI_D0P_T0A_TIEL_EN         REG_FLD(1, 0)

#define FLD_DSI_D0N_T0B_TIEL_EN         REG_FLD(1, 0)

#define FLD_DSI_D0_SER_BISTTOG           REG_FLD(1, 0)

#define FLD_DSI_D0_SER_DIN_SEL           REG_FLD(1, 0)

#define FLD_DSI_D0_DIG_PN_SWAP_EN     REG_FLD(1, 0)

#define FLD_DSI_D0_SW_CTL_EN               REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D0_SW_LPTX_PRE_OE REG_FLD(1, 8)
#define FLD_DSI_D0_SW_LPTX_PRE_OE     REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D0_SW_LPTX_OE     REG_FLD(1, 8)
#define FLD_DSI_D0_SW_LPTX_OE             REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D0_SW_LPTX_DP     REG_FLD(1, 8)
#define FLD_DSI_D0_SW_LPTX_DP             REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D0_SW_LPTX_DN     REG_FLD(1, 8)
#define FLD_DSI_D0_SW_LPTX_DN             REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D0_SW_LPRX_EN     REG_FLD(1, 8)
#define FLD_DSI_D0_SW_LPRX_EN             REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D0_SW_HSTX_PRE_OE REG_FLD(1, 8)
#define FLD_DSI_D0_SW_HSTX_PRE_OE     REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D0_SW_HSTX_OE     REG_FLD(1, 8)
#define FLD_DSI_D0_SW_HSTX_OE             REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D0_SW_HSTX_RDY   REG_FLD(1, 8)
#define FLD_DSI_D0_SW_HSTX_RDY           REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D0C_SW_LPTX_PRE_OE REG_FLD(1, 8)
#define FLD_DSI_D0C_SW_LPTX_PRE_OE   REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D0C_SW_LPTX_OE   REG_FLD(1, 8)
#define FLD_DSI_D0C_SW_LPTX_OE           REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D0C_SW_LPRX_EN   REG_FLD(1, 8)
#define FLD_DSI_D0C_SW_LPRX_EN           REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D0C_SW_HSTX_PRE_OE REG_FLD(1, 8)
#define FLD_DSI_D0C_SW_HSTX_PRE_OE   REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D0C_SW_HSTX_OE   REG_FLD(1, 8)
#define FLD_DSI_D0C_SW_HSTX_OE           REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D0C_SW_HSTX_RDY REG_FLD(1, 8)
#define FLD_DSI_D0C_SW_HSTX_RDY         REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D0_SW_HSTX_DATA0 REG_FLD(1, 8)
#define FLD_DSI_D0_SW_HSTX_DATA0       REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D0_SW_HSTX_DATA1 REG_FLD(1, 8)
#define FLD_DSI_D0_SW_HSTX_DATA1       REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D0_SW_HSTX_DATA2 REG_FLD(1, 8)
#define FLD_DSI_D0_SW_HSTX_DATA2       REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D0_SW_HSTX_DATA3 REG_FLD(1, 8)
#define FLD_DSI_D0_SW_HSTX_DATA3       REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D0_SW_HSTX_DATA4 REG_FLD(1, 8)
#define FLD_DSI_D0_SW_HSTX_DATA4       REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D0_SW_HSTX_DATA5 REG_FLD(1, 8)
#define FLD_DSI_D0_SW_HSTX_DATA5       REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D0_SW_HSTX_DATA6 REG_FLD(1, 8)
#define FLD_DSI_D0_SW_HSTX_DATA6       REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D0_SW_HSTX_DATA7 REG_FLD(1, 8)
#define FLD_DSI_D0_SW_HSTX_DATA7       REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D0C_SW_HSTX_DATA0 REG_FLD(1, 8)
#define FLD_DSI_D0C_SW_HSTX_DATA0     REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D0C_SW_HSTX_DATA1 REG_FLD(1, 8)
#define FLD_DSI_D0C_SW_HSTX_DATA1     REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D0C_SW_HSTX_DATA2 REG_FLD(1, 8)
#define FLD_DSI_D0C_SW_HSTX_DATA2     REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D0C_SW_HSTX_DATA3 REG_FLD(1, 8)
#define FLD_DSI_D0C_SW_HSTX_DATA3     REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D0C_SW_HSTX_DATA4 REG_FLD(1, 8)
#define FLD_DSI_D0C_SW_HSTX_DATA4     REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D0C_SW_HSTX_DATA5 REG_FLD(1, 8)
#define FLD_DSI_D0C_SW_HSTX_DATA5     REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D0C_SW_HSTX_DATA6 REG_FLD(1, 8)
#define FLD_DSI_D0C_SW_HSTX_DATA6     REG_FLD(1, 0)

#define FLD_DSI_D0_SW_CD_CON               REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D0_AD_LPRX_N           REG_FLD(1, 8)
#define FLD_DBG_OUT_DSI_D0_AD_LPRX_P           REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D0_AD_LPCD_N           REG_FLD(1, 8)
#define FLD_DBG_OUT_DSI_D0_AD_LPCD_P           REG_FLD(1, 0)

#define FLD_DSI_CKP_RTCODE0                 REG_FLD(1, 0)

#define FLD_DSI_CKP_RTCODE1                 REG_FLD(1, 0)

#define FLD_DSI_CKP_RTCODE2                 REG_FLD(1, 0)

#define FLD_DSI_CKP_RTCODE3                 REG_FLD(1, 0)

#define FLD_DSI_CKP_RTCODE4                 REG_FLD(1, 0)

#define FLD_DSI_CKN_RTCODE0                 REG_FLD(1, 0)

#define FLD_DSI_CKN_RTCODE1                 REG_FLD(1, 0)

#define FLD_DSI_CKN_RTCODE2                 REG_FLD(1, 0)

#define FLD_DSI_CKN_RTCODE3                 REG_FLD(1, 0)

#define FLD_DSI_CKN_RTCODE4                 REG_FLD(1, 0)

#define FLD_DSI_CK_CKMODE_EN               REG_FLD(1, 0)

#define FLD_DSI_CK_ANA_PN_SWAP_EN     REG_FLD(1, 0)

#define FLD_DSI_CKP_T0A_TIEL_EN         REG_FLD(1, 0)

#define FLD_DSI_CKN_T0B_TIEL_EN         REG_FLD(1, 0)

#define FLD_DSI_CK_SER_BISTTOG           REG_FLD(1, 0)

#define FLD_DSI_CK_SER_DIN_SEL           REG_FLD(1, 0)

#define FLD_DSI_CK_DIG_PN_SWAP_EN     REG_FLD(1, 0)

#define FLD_DSI_CK_SW_CTL_EN               REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_CK_SW_LPTX_PRE_OE REG_FLD(1, 8)
#define FLD_DSI_CK_SW_LPTX_PRE_OE     REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_CK_SW_LPTX_OE     REG_FLD(1, 8)
#define FLD_DSI_CK_SW_LPTX_OE             REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_CK_SW_LPTX_DP     REG_FLD(1, 8)
#define FLD_DSI_CK_SW_LPTX_DP             REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_CK_SW_LPTX_DN     REG_FLD(1, 8)
#define FLD_DSI_CK_SW_LPTX_DN             REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_CK_SW_LPRX_EN     REG_FLD(1, 8)
#define FLD_DSI_CK_SW_LPRX_EN             REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_CK_SW_HSTX_PRE_OE REG_FLD(1, 8)
#define FLD_DSI_CK_SW_HSTX_PRE_OE     REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_CK_SW_HSTX_OE     REG_FLD(1, 8)
#define FLD_DSI_CK_SW_HSTX_OE             REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_CK_SW_HSTX_RDY   REG_FLD(1, 8)
#define FLD_DSI_CK_SW_HSTX_RDY           REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_CKC_SW_LPTX_PRE_OE REG_FLD(1, 8)
#define FLD_DSI_CKC_SW_LPTX_PRE_OE   REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_CKC_SW_LPTX_OE   REG_FLD(1, 8)
#define FLD_DSI_CKC_SW_LPTX_OE           REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_CKC_SW_LPRX_EN   REG_FLD(1, 8)
#define FLD_DSI_CKC_SW_LPRX_EN           REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_CKC_SW_HSTX_PRE_OE REG_FLD(1, 8)
#define FLD_DSI_CKC_SW_HSTX_PRE_OE   REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_CKC_SW_HSTX_OE   REG_FLD(1, 8)
#define FLD_DSI_CKC_SW_HSTX_OE           REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_CKC_SW_HSTX_RDY REG_FLD(1, 8)
#define FLD_DSI_CKC_SW_HSTX_RDY         REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_CK_SW_HSTX_DATA0 REG_FLD(1, 8)
#define FLD_DSI_CK_SW_HSTX_DATA0       REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_CK_SW_HSTX_DATA1 REG_FLD(1, 8)
#define FLD_DSI_CK_SW_HSTX_DATA1       REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_CK_SW_HSTX_DATA2 REG_FLD(1, 8)
#define FLD_DSI_CK_SW_HSTX_DATA2       REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_CK_SW_HSTX_DATA3 REG_FLD(1, 8)
#define FLD_DSI_CK_SW_HSTX_DATA3       REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_CK_SW_HSTX_DATA4 REG_FLD(1, 8)
#define FLD_DSI_CK_SW_HSTX_DATA4       REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_CK_SW_HSTX_DATA5 REG_FLD(1, 8)
#define FLD_DSI_CK_SW_HSTX_DATA5       REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_CK_SW_HSTX_DATA6 REG_FLD(1, 8)
#define FLD_DSI_CK_SW_HSTX_DATA6       REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_CK_SW_HSTX_DATA7 REG_FLD(1, 8)
#define FLD_DSI_CK_SW_HSTX_DATA7       REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_CKC_SW_HSTX_DATA0 REG_FLD(1, 8)
#define FLD_DSI_CKC_SW_HSTX_DATA0     REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_CKC_SW_HSTX_DATA1 REG_FLD(1, 8)
#define FLD_DSI_CKC_SW_HSTX_DATA1     REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_CKC_SW_HSTX_DATA2 REG_FLD(1, 8)
#define FLD_DSI_CKC_SW_HSTX_DATA2     REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_CKC_SW_HSTX_DATA3 REG_FLD(1, 8)
#define FLD_DSI_CKC_SW_HSTX_DATA3     REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_CKC_SW_HSTX_DATA4 REG_FLD(1, 8)
#define FLD_DSI_CKC_SW_HSTX_DATA4     REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_CKC_SW_HSTX_DATA5 REG_FLD(1, 8)
#define FLD_DSI_CKC_SW_HSTX_DATA5     REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_CKC_SW_HSTX_DATA6 REG_FLD(1, 8)
#define FLD_DSI_CKC_SW_HSTX_DATA6     REG_FLD(1, 0)

#define FLD_DSI_CK_SW_CD_CON               REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_CK_AD_LPRX_N           REG_FLD(1, 8)
#define FLD_DBG_OUT_DSI_CK_AD_LPRX_P           REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_CK_AD_LPCD_N           REG_FLD(1, 8)
#define FLD_DBG_OUT_DSI_CK_AD_LPCD_P           REG_FLD(1, 0)

#define FLD_DSI_D1P_RTCODE0                 REG_FLD(1, 0)

#define FLD_DSI_D1P_RTCODE1                 REG_FLD(1, 0)

#define FLD_DSI_D1P_RTCODE2                 REG_FLD(1, 0)

#define FLD_DSI_D1P_RTCODE3                 REG_FLD(1, 0)

#define FLD_DSI_D1P_RTCODE4                 REG_FLD(1, 0)

#define FLD_DSI_D1N_RTCODE0                 REG_FLD(1, 0)

#define FLD_DSI_D1N_RTCODE1                 REG_FLD(1, 0)

#define FLD_DSI_D1N_RTCODE2                 REG_FLD(1, 0)

#define FLD_DSI_D1N_RTCODE3                 REG_FLD(1, 0)

#define FLD_DSI_D1N_RTCODE4                 REG_FLD(1, 0)

#define FLD_DSI_D1_CKMODE_EN               REG_FLD(1, 0)

#define FLD_DSI_D1_ANA_PN_SWAP_EN     REG_FLD(1, 0)

#define FLD_DSI_D1P_T0A_TIEL_EN         REG_FLD(1, 0)

#define FLD_DSI_D1N_T0B_TIEL_EN         REG_FLD(1, 0)

#define FLD_DSI_D1_SER_BISTTOG           REG_FLD(1, 0)

#define FLD_DSI_D1_SER_DIN_SEL           REG_FLD(1, 0)

#define FLD_DSI_D1_DIG_PN_SWAP_EN     REG_FLD(1, 0)

#define FLD_DSI_D1_SW_CTL_EN               REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D1_SW_LPTX_PRE_OE REG_FLD(1, 8)
#define FLD_DSI_D1_SW_LPTX_PRE_OE     REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D1_SW_LPTX_OE     REG_FLD(1, 8)
#define FLD_DSI_D1_SW_LPTX_OE             REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D1_SW_LPTX_DP     REG_FLD(1, 8)
#define FLD_DSI_D1_SW_LPTX_DP             REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D1_SW_LPTX_DN     REG_FLD(1, 8)
#define FLD_DSI_D1_SW_LPTX_DN             REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D1_SW_LPRX_EN     REG_FLD(1, 8)
#define FLD_DSI_D1_SW_LPRX_EN             REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D1_SW_HSTX_PRE_OE REG_FLD(1, 8)
#define FLD_DSI_D1_SW_HSTX_PRE_OE     REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D1_SW_HSTX_OE     REG_FLD(1, 8)
#define FLD_DSI_D1_SW_HSTX_OE             REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D1_SW_HSTX_RDY   REG_FLD(1, 8)
#define FLD_DSI_D1_SW_HSTX_RDY           REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D1C_SW_LPTX_PRE_OE REG_FLD(1, 8)
#define FLD_DSI_D1C_SW_LPTX_PRE_OE   REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D1C_SW_LPTX_OE   REG_FLD(1, 8)
#define FLD_DSI_D1C_SW_LPTX_OE           REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D1C_SW_LPRX_EN   REG_FLD(1, 8)
#define FLD_DSI_D1C_SW_LPRX_EN           REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D1C_SW_HSTX_PRE_OE REG_FLD(1, 8)
#define FLD_DSI_D1C_SW_HSTX_PRE_OE   REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D1C_SW_HSTX_OE   REG_FLD(1, 8)
#define FLD_DSI_D1C_SW_HSTX_OE           REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D1C_SW_HSTX_RDY REG_FLD(1, 8)
#define FLD_DSI_D1C_SW_HSTX_RDY         REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D1_SW_HSTX_DATA0 REG_FLD(1, 8)
#define FLD_DSI_D1_SW_HSTX_DATA0       REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D1_SW_HSTX_DATA1 REG_FLD(1, 8)
#define FLD_DSI_D1_SW_HSTX_DATA1       REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D1_SW_HSTX_DATA2 REG_FLD(1, 8)
#define FLD_DSI_D1_SW_HSTX_DATA2       REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D1_SW_HSTX_DATA3 REG_FLD(1, 8)
#define FLD_DSI_D1_SW_HSTX_DATA3       REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D1_SW_HSTX_DATA4 REG_FLD(1, 8)
#define FLD_DSI_D1_SW_HSTX_DATA4       REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D1_SW_HSTX_DATA5 REG_FLD(1, 8)
#define FLD_DSI_D1_SW_HSTX_DATA5       REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D1_SW_HSTX_DATA6 REG_FLD(1, 8)
#define FLD_DSI_D1_SW_HSTX_DATA6       REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D1_SW_HSTX_DATA7 REG_FLD(1, 8)
#define FLD_DSI_D1_SW_HSTX_DATA7       REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D1C_SW_HSTX_DATA0 REG_FLD(1, 8)
#define FLD_DSI_D1C_SW_HSTX_DATA0     REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D1C_SW_HSTX_DATA1 REG_FLD(1, 8)
#define FLD_DSI_D1C_SW_HSTX_DATA1     REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D1C_SW_HSTX_DATA2 REG_FLD(1, 8)
#define FLD_DSI_D1C_SW_HSTX_DATA2     REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D1C_SW_HSTX_DATA3 REG_FLD(1, 8)
#define FLD_DSI_D1C_SW_HSTX_DATA3     REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D1C_SW_HSTX_DATA4 REG_FLD(1, 8)
#define FLD_DSI_D1C_SW_HSTX_DATA4     REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D1C_SW_HSTX_DATA5 REG_FLD(1, 8)
#define FLD_DSI_D1C_SW_HSTX_DATA5     REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D1C_SW_HSTX_DATA6 REG_FLD(1, 8)
#define FLD_DSI_D1C_SW_HSTX_DATA6     REG_FLD(1, 0)

#define FLD_DSI_D1_SW_CD_CON               REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D1_AD_LPRX_N           REG_FLD(1, 8)
#define FLD_DBG_OUT_DSI_D1_AD_LPRX_P           REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D1_AD_LPCD_N           REG_FLD(1, 8)
#define FLD_DBG_OUT_DSI_D1_AD_LPCD_P           REG_FLD(1, 0)

#define FLD_DSI_D3P_RTCODE0                 REG_FLD(1, 0)

#define FLD_DSI_D3P_RTCODE1                 REG_FLD(1, 0)

#define FLD_DSI_D3P_RTCODE2                 REG_FLD(1, 0)

#define FLD_DSI_D3P_RTCODE3                 REG_FLD(1, 0)

#define FLD_DSI_D3P_RTCODE4                 REG_FLD(1, 0)

#define FLD_DSI_D3N_RTCODE0                 REG_FLD(1, 0)

#define FLD_DSI_D3N_RTCODE1                 REG_FLD(1, 0)

#define FLD_DSI_D3N_RTCODE2                 REG_FLD(1, 0)

#define FLD_DSI_D3N_RTCODE3                 REG_FLD(1, 0)

#define FLD_DSI_D3N_RTCODE4                 REG_FLD(1, 0)

#define FLD_DSI_D3_CKMODE_EN               REG_FLD(1, 0)

#define FLD_DSI_D3_ANA_PN_SWAP_EN     REG_FLD(1, 0)

#define FLD_DSI_D3P_T0A_TIEL_EN         REG_FLD(1, 0)

#define FLD_DSI_D3N_T0B_TIEL_EN         REG_FLD(1, 0)

#define FLD_DSI_D3_SER_BISTTOG           REG_FLD(1, 0)

#define FLD_DSI_D3_SER_DIN_SEL           REG_FLD(1, 0)

#define FLD_DSI_D3_DIG_PN_SWAP_EN     REG_FLD(1, 0)

#define FLD_DSI_D3_SW_CTL_EN               REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D3_SW_LPTX_PRE_OE REG_FLD(1, 8)
#define FLD_DSI_D3_SW_LPTX_PRE_OE     REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D3_SW_LPTX_OE     REG_FLD(1, 8)
#define FLD_DSI_D3_SW_LPTX_OE             REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D3_SW_LPTX_DP     REG_FLD(1, 8)
#define FLD_DSI_D3_SW_LPTX_DP             REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D3_SW_LPTX_DN     REG_FLD(1, 8)
#define FLD_DSI_D3_SW_LPTX_DN             REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D3_SW_LPRX_EN     REG_FLD(1, 8)
#define FLD_DSI_D3_SW_LPRX_EN             REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D3_SW_HSTX_PRE_OE REG_FLD(1, 8)
#define FLD_DSI_D3_SW_HSTX_PRE_OE     REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D3_SW_HSTX_OE     REG_FLD(1, 8)
#define FLD_DSI_D3_SW_HSTX_OE             REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D3_SW_HSTX_RDY   REG_FLD(1, 8)
#define FLD_DSI_D3_SW_HSTX_RDY           REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D3C_SW_LPTX_PRE_OE REG_FLD(1, 8)
#define FLD_DSI_D3C_SW_LPTX_PRE_OE   REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D3C_SW_LPTX_OE   REG_FLD(1, 8)
#define FLD_DSI_D3C_SW_LPTX_OE           REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D3C_SW_LPRX_EN   REG_FLD(1, 8)
#define FLD_DSI_D3C_SW_LPRX_EN           REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D3C_SW_HSTX_PRE_OE REG_FLD(1, 8)
#define FLD_DSI_D3C_SW_HSTX_PRE_OE   REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D3C_SW_HSTX_OE   REG_FLD(1, 8)
#define FLD_DSI_D3C_SW_HSTX_OE           REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D3C_SW_HSTX_RDY REG_FLD(1, 8)
#define FLD_DSI_D3C_SW_HSTX_RDY         REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D3_SW_HSTX_DATA0 REG_FLD(1, 8)
#define FLD_DSI_D3_SW_HSTX_DATA0       REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D3_SW_HSTX_DATA1 REG_FLD(1, 8)
#define FLD_DSI_D3_SW_HSTX_DATA1       REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D3_SW_HSTX_DATA2 REG_FLD(1, 8)
#define FLD_DSI_D3_SW_HSTX_DATA2       REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D3_SW_HSTX_DATA3 REG_FLD(1, 8)
#define FLD_DSI_D3_SW_HSTX_DATA3       REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D3_SW_HSTX_DATA4 REG_FLD(1, 8)
#define FLD_DSI_D3_SW_HSTX_DATA4       REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D3_SW_HSTX_DATA5 REG_FLD(1, 8)
#define FLD_DSI_D3_SW_HSTX_DATA5       REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D3_SW_HSTX_DATA6 REG_FLD(1, 8)
#define FLD_DSI_D3_SW_HSTX_DATA6       REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D3_SW_HSTX_DATA7 REG_FLD(1, 8)
#define FLD_DSI_D3_SW_HSTX_DATA7       REG_FLD(1, 0)

#define FLD_DSI_D3_SW_CD_CON               REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D3_AD_LPRX_N           REG_FLD(1, 8)
#define FLD_DBG_OUT_DSI_D3_AD_LPRX_P           REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D3_AD_LPCD_N           REG_FLD(1, 8)
#define FLD_DBG_OUT_DSI_D3_AD_LPCD_P           REG_FLD(1, 0)

#define FLD_DPHY_BIST_SPECIFIED_PATTERN  REG_FLD(1, 24)
#define FLD_DPHY_BIST_FIX_PATTERN        REG_FLD(1, 16)
#define FLD_DPHY_BIST_MODE               REG_FLD(1, 8)
#define FLD_DPHY_BIST_ENABLE             REG_FLD(1, 0)

#define FLD_DPHY_BIST_CK_HS_PATTERN_SEL  REG_FLD(1, 16)
#define FLD_DPHY_BIST_DESKEW_MODE        REG_FLD(1, 8)
#define FLD_DPHY_BIST_HS_FREE            REG_FLD(1, 0)

#define FLD_DPHY_BIST_LANE_NUM_3     REG_FLD(1, 24)
#define FLD_DPHY_BIST_LANE_NUM_2     REG_FLD(1, 16)
#define FLD_DPHY_BIST_LANE_NUM_1     REG_FLD(1, 8)
#define FLD_DPHY_BIST_LANE_NUM_0     REG_FLD(1, 0)

#define FLD_DPHY_BIST_TIMIING_3      REG_FLD(1, 24)
#define FLD_DPHY_BIST_TIMIING_2      REG_FLD(1, 16)
#define FLD_DPHY_BIST_TIMIING_1      REG_FLD(1, 8)
#define FLD_DPHY_BIST_TIMIING_0      REG_FLD(1, 0)

#define FLD_DPHY_BIST_TIMIING_7      REG_FLD(1, 24)
#define FLD_DPHY_BIST_TIMIING_6      REG_FLD(1, 16)
#define FLD_DPHY_BIST_TIMIING_5      REG_FLD(1, 8)
#define FLD_DPHY_BIST_TIMIING_4      REG_FLD(1, 0)

#define FLD_DPHY_BIST_PATTERN_3     REG_FLD(1, 24)
#define FLD_DPHY_BIST_PATTERN_2     REG_FLD(1, 16)
#define FLD_DPHY_BIST_PATTERN_1     REG_FLD(1, 8)
#define FLD_DPHY_BIST_PATTERN_0     REG_FLD(1, 0)

#define FLD_DPHY_BIST_PATTERN_7     REG_FLD(1, 24)
#define FLD_DPHY_BIST_PATTERN_6     REG_FLD(1, 16)
#define FLD_DPHY_BIST_PATTERN_5     REG_FLD(1, 8)
#define FLD_DPHY_BIST_PATTERN_4     REG_FLD(1, 0)

#define FLD_DPHY_BIST_PATTERN_11    REG_FLD(1, 24)
#define FLD_DPHY_BIST_PATTERN_10    REG_FLD(1, 16)
#define FLD_DPHY_BIST_PATTERN_9     REG_FLD(1, 8)
#define FLD_DPHY_BIST_PATTERN_8     REG_FLD(1, 0)

#define FLD_DPHY_BIST_PATTERN_15    REG_FLD(1, 24)
#define FLD_DPHY_BIST_PATTERN_14    REG_FLD(1, 16)
#define FLD_DPHY_BIST_PATTERN_13    REG_FLD(1, 8)
#define FLD_DPHY_BIST_PATTERN_12    REG_FLD(1, 0)

#define FLD_DPHY_BIST_PATTERN_19    REG_FLD(1, 24)
#define FLD_DPHY_BIST_PATTERN_18    REG_FLD(1, 16)
#define FLD_DPHY_BIST_PATTERN_17    REG_FLD(1, 8)
#define FLD_DPHY_BIST_PATTERN_16    REG_FLD(1, 0)

#define FLD_DPHY_BIST_PATTERN_23    REG_FLD(1, 24)
#define FLD_DPHY_BIST_PATTERN_22    REG_FLD(1, 16)
#define FLD_DPHY_BIST_PATTERN_21    REG_FLD(1, 8)
#define FLD_DPHY_BIST_PATTERN_20    REG_FLD(1, 0)

#define FLD_DPHY_BIST_PATTERN_27    REG_FLD(1, 24)
#define FLD_DPHY_BIST_PATTERN_26    REG_FLD(1, 16)
#define FLD_DPHY_BIST_PATTERN_25    REG_FLD(1, 8)
#define FLD_DPHY_BIST_PATTERN_24    REG_FLD(1, 0)

#define FLD_DPHY_BIST_PATTERN_31    REG_FLD(1, 24)
#define FLD_DPHY_BIST_PATTERN_30    REG_FLD(1, 16)
#define FLD_DPHY_BIST_PATTERN_29    REG_FLD(1, 8)
#define FLD_DPHY_BIST_PATTERN_28    REG_FLD(1, 0)

#define FLD_CPHY_BIST_ESCAPE_EN          REG_FLD(1, 24)
#define FLD_CPHY_BIST_PROGSEQ_EN         REG_FLD(1, 16)
#define FLD_CPHY_BIST_MODE               REG_FLD(1, 8)
#define FLD_CPHY_BIST_START              REG_FLD(1, 0)

#define FLD_CPHY_BIST_SETTLE_SKIP_EN     REG_FLD(1, 16)
#define FLD_CPHY_BIST_PACKET_TEST_MODE_OUT_EN REG_FLD(1, 8)
#define FLD_CPHY_BIST_PACKET_TEST_MODE   REG_FLD(1, 0)

#define FLD_CPHY_BIST_INIT_WIRE_STATE_2 REG_FLD(1, 24)
#define FLD_CPHY_BIST_INIT_WIRE_STATE_1 REG_FLD(1, 16)
#define FLD_CPHY_BIST_INIT_WIRE_STATE_0 REG_FLD(1, 8)
#define FLD_CPHY_BIST_WIRE_STATE_FIX REG_FLD(1, 0)

#define FLD_CPHY_BIST_PRB_SEL_2       REG_FLD(1, 24)
#define FLD_CPHY_BIST_PRB_SEL_1       REG_FLD(1, 16)
#define FLD_CPHY_BIST_PRB_SEL_0       REG_FLD(1, 8)
#define FLD_CPHY_BIST_PRB_FIX         REG_FLD(1, 0)

#define FLD_CPHY_BIST_PREBEGIN_CNT_3 REG_FLD(1, 24)
#define FLD_CPHY_BIST_PREBEGIN_CNT_2 REG_FLD(1, 16)
#define FLD_CPHY_BIST_PREBEGIN_CNT_1 REG_FLD(1, 8)
#define FLD_CPHY_BIST_PREBEGIN_CNT_0 REG_FLD(1, 0)

#define FLD_CPHY_BIST_PREBEGIN_CNT_7 REG_FLD(1, 24)
#define FLD_CPHY_BIST_PREBEGIN_CNT_6 REG_FLD(1, 16)
#define FLD_CPHY_BIST_PREBEGIN_CNT_5 REG_FLD(1, 8)
#define FLD_CPHY_BIST_PREBEGIN_CNT_4 REG_FLD(1, 0)

#define FLD_CPHY_BIST_POST_CNT_3 REG_FLD(1, 24)
#define FLD_CPHY_BIST_POST_CNT_2 REG_FLD(1, 16)
#define FLD_CPHY_BIST_POST_CNT_1 REG_FLD(1, 8)
#define FLD_CPHY_BIST_POST_CNT_0 REG_FLD(1, 0)

#define FLD_CPHY_BIST_POST_CNT_7 REG_FLD(1, 24)
#define FLD_CPHY_BIST_POST_CNT_6 REG_FLD(1, 16)
#define FLD_CPHY_BIST_POST_CNT_5 REG_FLD(1, 8)
#define FLD_CPHY_BIST_POST_CNT_4 REG_FLD(1, 0)

#define FLD_CPHY_BIST_PROGSEQ_SYMBOL_LSB_3 REG_FLD(1, 24)
#define FLD_CPHY_BIST_PROGSEQ_SYMBOL_LSB_2 REG_FLD(1, 16)
#define FLD_CPHY_BIST_PROGSEQ_SYMBOL_LSB_1 REG_FLD(1, 8)
#define FLD_CPHY_BIST_PROGSEQ_SYMBOL_LSB_0 REG_FLD(1, 0)

#define FLD_CPHY_BIST_PROGSEQ_SYMBOL_LSB_7 REG_FLD(1, 24)
#define FLD_CPHY_BIST_PROGSEQ_SYMBOL_LSB_6 REG_FLD(1, 16)
#define FLD_CPHY_BIST_PROGSEQ_SYMBOL_LSB_5 REG_FLD(1, 8)
#define FLD_CPHY_BIST_PROGSEQ_SYMBOL_LSB_4 REG_FLD(1, 0)

#define FLD_CPHY_BIST_PROGSEQ_SYMBOL_LSB_11 REG_FLD(1, 24)
#define FLD_CPHY_BIST_PROGSEQ_SYMBOL_LSB_10 REG_FLD(1, 16)
#define FLD_CPHY_BIST_PROGSEQ_SYMBOL_LSB_9 REG_FLD(1, 8)
#define FLD_CPHY_BIST_PROGSEQ_SYMBOL_LSB_8 REG_FLD(1, 0)

#define FLD_CPHY_BIST_PROGSEQ_SYMBOL_LSB_15 REG_FLD(1, 24)
#define FLD_CPHY_BIST_PROGSEQ_SYMBOL_LSB_14 REG_FLD(1, 16)
#define FLD_CPHY_BIST_PROGSEQ_SYMBOL_LSB_13 REG_FLD(1, 8)
#define FLD_CPHY_BIST_PROGSEQ_SYMBOL_LSB_12 REG_FLD(1, 0)

#define FLD_CPHY_BIST_PROGSEQ_SYMBOL_LSB_19 REG_FLD(1, 24)
#define FLD_CPHY_BIST_PROGSEQ_SYMBOL_LSB_18 REG_FLD(1, 16)
#define FLD_CPHY_BIST_PROGSEQ_SYMBOL_LSB_17 REG_FLD(1, 8)
#define FLD_CPHY_BIST_PROGSEQ_SYMBOL_LSB_16 REG_FLD(1, 0)

#define FLD_CPHY_BIST_PROGSEQ_SYMBOL_LSB_23 REG_FLD(1, 24)
#define FLD_CPHY_BIST_PROGSEQ_SYMBOL_LSB_22 REG_FLD(1, 16)
#define FLD_CPHY_BIST_PROGSEQ_SYMBOL_LSB_21 REG_FLD(1, 8)
#define FLD_CPHY_BIST_PROGSEQ_SYMBOL_LSB_20 REG_FLD(1, 0)

#define FLD_CPHY_BIST_PROGSEQ_SYMBOL_LSB_27 REG_FLD(1, 24)
#define FLD_CPHY_BIST_PROGSEQ_SYMBOL_LSB_26 REG_FLD(1, 16)
#define FLD_CPHY_BIST_PROGSEQ_SYMBOL_LSB_25 REG_FLD(1, 8)
#define FLD_CPHY_BIST_PROGSEQ_SYMBOL_LSB_24 REG_FLD(1, 0)

#define FLD_CPHY_BIST_PROGSEQ_SYMBOL_LSB_31 REG_FLD(1, 24)
#define FLD_CPHY_BIST_PROGSEQ_SYMBOL_LSB_30 REG_FLD(1, 16)
#define FLD_CPHY_BIST_PROGSEQ_SYMBOL_LSB_29 REG_FLD(1, 8)
#define FLD_CPHY_BIST_PROGSEQ_SYMBOL_LSB_28 REG_FLD(1, 0)

#define FLD_CPHY_BIST_PROGSEQ_SYMBOL_MSB_3 REG_FLD(1, 24)
#define FLD_CPHY_BIST_PROGSEQ_SYMBOL_MSB_2 REG_FLD(1, 16)
#define FLD_CPHY_BIST_PROGSEQ_SYMBOL_MSB_1 REG_FLD(1, 8)
#define FLD_CPHY_BIST_PROGSEQ_SYMBOL_MSB_0 REG_FLD(1, 0)

#define FLD_CPHY_BIST_PROGSEQ_SYMBOL_MSB_7 REG_FLD(1, 24)
#define FLD_CPHY_BIST_PROGSEQ_SYMBOL_MSB_6 REG_FLD(1, 16)
#define FLD_CPHY_BIST_PROGSEQ_SYMBOL_MSB_5 REG_FLD(1, 8)
#define FLD_CPHY_BIST_PROGSEQ_SYMBOL_MSB_4 REG_FLD(1, 0)

#define FLD_CPHY_BIST_PROGSEQ_SYMBOL_MSB_9 REG_FLD(1, 8)
#define FLD_CPHY_BIST_PROGSEQ_SYMBOL_MSB_8 REG_FLD(1, 0)

#define FLD_CPHY_BIST_PRB_SEED_3   REG_FLD(1, 24)
#define FLD_CPHY_BIST_PRB_SEED_2   REG_FLD(1, 16)
#define FLD_CPHY_BIST_PRB_SEED_1   REG_FLD(1, 8)
#define FLD_CPHY_BIST_PRB_SEED_0   REG_FLD(1, 0)

#define FLD_CPHY_BIST_PRB_SEED_7   REG_FLD(1, 24)
#define FLD_CPHY_BIST_PRB_SEED_6   REG_FLD(1, 16)
#define FLD_CPHY_BIST_PRB_SEED_5   REG_FLD(1, 8)
#define FLD_CPHY_BIST_PRB_SEED_4   REG_FLD(1, 0)

#define FLD_CPHY_BIST_PRB_SEED_11  REG_FLD(1, 24)
#define FLD_CPHY_BIST_PRB_SEED_10  REG_FLD(1, 16)
#define FLD_CPHY_BIST_PRB_SEED_9   REG_FLD(1, 8)
#define FLD_CPHY_BIST_PRB_SEED_8   REG_FLD(1, 0)

#define FLD_CPHY_BIST_PRB_SEED_15  REG_FLD(1, 24)
#define FLD_CPHY_BIST_PRB_SEED_14  REG_FLD(1, 16)
#define FLD_CPHY_BIST_PRB_SEED_13  REG_FLD(1, 8)
#define FLD_CPHY_BIST_PRB_SEED_12  REG_FLD(1, 0)

#define FLD_CPHY_BIST_PRB_SEED_17  REG_FLD(1, 8)
#define FLD_CPHY_BIST_PRB_SEED_16  REG_FLD(1, 0)

