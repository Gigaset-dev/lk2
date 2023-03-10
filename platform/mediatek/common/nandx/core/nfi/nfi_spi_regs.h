/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#define SNF_MAC_CTL             0x500
#define         WIP                     nandx_bit(0)
#define         WIP_READY               nandx_bit(1)
#define         SF_TRIG                 nandx_bit(2)
#define         SF_MAC_EN               nandx_bit(3)
#define         MAC_XIO_SEL             nandx_bit(4)
#define SNF_MAC_OUTL            0x504
#define SNF_MAC_INL             0x508
#define SNF_RD_CTL1             0x50c
#define         PAGE_READ_CMD_SHIFT     24
#define SNF_RD_CTL2             0x510
#define SNF_RD_CTL3             0x514
#define SNF_GF_CTL1             0x518
#define         GF_ADDR_SHIFT           16
#define         GF_CMD_SHIFT            24
#define SNF_GF_CTL3             0x520
#define SNF_PG_CTL1             0x524
#define         PG_EXE_CMD_SHIFT        16
#define         PG_LOAD_CMD_SHIFT       8
#define SNF_PG_CTL2             0x528
#define SNF_PG_CTL3             0x52c
#define SNF_ER_CTL              0x530
#define SNF_ER_CTL2             0x534
#define SNF_MISC_CTL            0x538
#define         SW_RST                  nandx_bit(28)
#define         PG_LOAD_X4_EN           nandx_bit(20)
#define         X2_DATA_MODE            1
#define         X4_DATA_MODE            2
#define         DUAL_IO_MODE            5
#define         QUAD_IO_MODE            6
#define         READ_MODE_SHIFT         16
#define         LATCH_LAT_SHIFT         8
#define         LATCH_LAT_MASK          GENMASK(9, 8)
#define         DARA_READ_MODE_MASK     GENMASK(18, 16)
#define         SF2CS_SEL               nandx_bit(13)
#define         SF2CS_EN                nandx_bit(12)
#define         PG_LOAD_CUSTOM_EN       nandx_bit(7)
#define         DATARD_CUSTOM_EN        nandx_bit(6)
#define SNF_MISC_CTL2           0x53c
#define         PG_LOAD_SHIFT           16
#define SNF_DLY_CTL1            0x540
#define SNF_DLY_CTL2            0x544
#define SNF_DLY_CTL3            0x548
#define SNF_DLY_CTL4            0x54c
#define SNF_STA_CTL1            0x550
#define         SPI_STATE               GENMASK(3, 0)
#define SNF_STA_CTL2            0x554
#define SNF_STA_CTL3            0x558
#define SNF_SNF_CNFG            0x55c
#define         SPI_MODE                nandx_bit(0)
#define SNF_DEBUG_SEL           0x560
#define SPI_GPRAM_ADDR          0x800

