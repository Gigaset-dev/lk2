/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once

#define MT6685_XO_BUF_CTL1_L            (0x54e)
#define MT6685_XO_BUF_CTL1_H            (0x54f)
#define MT6685_XO_BUF_CTL5_L            (0x556)
#define MT6685_XO_BUF_CTL5_H            (0x557)
#define MT6685_XO_BUF_CTL6_L            (0x558)
#define MT6685_XO_BUF_CTL6_H            (0x559)
#define MT6685_XO_BUF_CTL8_L            (0x55c)
#define MT6685_XO_BUF_CTL8_H            (0x55d)
#define MT6685_XO_BUF_CTL9_L            (0x55e)
#define MT6685_XO_BUF_CTL9_H            (0x55f)
#define MT6685_DCXO_EXTBUF2_CW0         (0x79b)
#define MT6685_DCXO_EXTBUF6_CW0         (0x79f)
#define MT6685_DCXO_EXTBUF7_CW0         (0x7a0)
#define MT6685_DCXO_EXTBUF9_CW0         (0x7a2)
#define MT6685_DCXO_EXTBUF10_CW0        (0x7a3)

/* Register_SCK_REG*/
#define XO_BBCK2_VOTE_L_ADDR            (MT6685_XO_BUF_CTL1_L)
#define XO_BBCK2_VOTE_L_MASK            (0xff)
#define XO_BBCK2_VOTE_L_SHIFT           (0)
#define XO_BBCK2_VOTE_H_ADDR            (MT6685_XO_BUF_CTL1_H)
#define XO_BBCK2_VOTE_H_MASK            (0x3f)
#define XO_BBCK2_VOTE_H_SHIFT           (0)
#define XO_RFCK1A_VOTE_L_ADDR           (MT6685_XO_BUF_CTL5_L)
#define XO_RFCK1A_VOTE_L_MASK           (0xff)
#define XO_RFCK1A_VOTE_L_SHIFT          (0)
#define XO_RFCK1A_VOTE_H_ADDR           (MT6685_XO_BUF_CTL5_H)
#define XO_RFCK1A_VOTE_H_MASK           (0x3f)
#define XO_RFCK1A_VOTE_H_SHIFT          (0)
#define XO_RFCK1B_VOTE_L_ADDR           (MT6685_XO_BUF_CTL6_L)
#define XO_RFCK1B_VOTE_L_MASK           (0xff)
#define XO_RFCK1B_VOTE_L_SHIFT          (0)
#define XO_RFCK1B_VOTE_H_ADDR           (MT6685_XO_BUF_CTL6_H)
#define XO_RFCK1B_VOTE_H_MASK           (0x3f)
#define XO_RFCK1B_VOTE_H_SHIFT          (0)
#define XO_RFCK2A_VOTE_L_ADDR           (MT6685_XO_BUF_CTL8_L)
#define XO_RFCK2A_VOTE_L_MASK           (0xff)
#define XO_RFCK2A_VOTE_L_SHIFT          (0)
#define XO_RFCK2A_VOTE_H_ADDR           (MT6685_XO_BUF_CTL8_H)
#define XO_RFCK2A_VOTE_H_MASK           (0x3f)
#define XO_RFCK2A_VOTE_H_SHIFT          (0)
#define XO_RFCK2B_VOTE_L_ADDR           (MT6685_XO_BUF_CTL9_L)
#define XO_RFCK2B_VOTE_L_MASK           (0xff)
#define XO_RFCK2B_VOTE_L_SHIFT          (0)
#define XO_RFCK2B_VOTE_H_ADDR           (MT6685_XO_BUF_CTL9_H)
#define XO_RFCK2B_VOTE_H_MASK           (0x3f)
#define XO_RFCK2B_VOTE_H_SHIFT          (0)

#define XO_BBCK2_MODE_ADDR              (MT6685_DCXO_EXTBUF2_CW0)
#define XO_BBCK2_MODE_MASK              (0x3)
#define XO_BBCK2_MODE_SHIFT             (0)
#define XO_BBCK2_EN_M_ADDR              (MT6685_DCXO_EXTBUF2_CW0)
#define XO_BBCK2_EN_M_MASK              (0x1)
#define XO_BBCK2_EN_M_SHIFT             (2)
#define XO_RFCK1A_MODE_ADDR             (MT6685_DCXO_EXTBUF6_CW0)
#define XO_RFCK1A_MODE_MASK             (0x3)
#define XO_RFCK1A_MODE_SHIFT            (0)
#define XO_RFCK1A_EN_M_ADDR             (MT6685_DCXO_EXTBUF6_CW0)
#define XO_RFCK1A_EN_M_MASK             (0x1)
#define XO_RFCK1A_EN_M_SHIFT            (2)
#define XO_RFCK1B_MODE_ADDR             (MT6685_DCXO_EXTBUF7_CW0)
#define XO_RFCK1B_MODE_MASK             (0x3)
#define XO_RFCK1B_MODE_SHIFT            (0)
#define XO_RFCK1B_EN_M_ADDR             (MT6685_DCXO_EXTBUF7_CW0)
#define XO_RFCK1B_EN_M_MASK             (0x1)
#define XO_RFCK1B_EN_M_SHIFT            (2)
#define XO_RFCK2A_MODE_ADDR             (MT6685_DCXO_EXTBUF9_CW0)
#define XO_RFCK2A_MODE_MASK             (0x3)
#define XO_RFCK2A_MODE_SHIFT            (0)
#define XO_RFCK2A_EN_M_ADDR             (MT6685_DCXO_EXTBUF9_CW0)
#define XO_RFCK2A_EN_M_MASK             (0x1)
#define XO_RFCK2A_EN_M_SHIFT            (2)
#define XO_RFCK2B_MODE_ADDR             (MT6685_DCXO_EXTBUF10_CW0)
#define XO_RFCK2B_MODE_MASK             (0x3)
#define XO_RFCK2B_MODE_SHIFT            (0)
#define XO_RFCK2B_EN_M_ADDR             (MT6685_DCXO_EXTBUF10_CW0)
#define XO_RFCK2B_EN_M_MASK             (0x1)
#define XO_RFCK2B_EN_M_SHIFT            (2)
