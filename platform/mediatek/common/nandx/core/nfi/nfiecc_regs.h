/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#define NFIECC_ENCCON           0x000
/* NFIECC_DECCON has same bit define */
#define         ECC_OP_EN               nandx_bit(0)
#define NFIECC_ENCCNFG          0x004
#define         ENCCNFG_MS_SHIFT        16
#define         ENC_BURST_EN            nandx_bit(8)
#define NFIECC_ENCDIADDR        0x008
#define NFIECC_ENCIDLE          0x00c
#define NFIECC_ENCSTA           0x07c
#define         ENC_FSM_IDLE            1
#define NFIECC_ENCIRQEN         0x080
/* NFIECC_DECIRQEN has same bit define */
#define         ECC_IRQEN               nandx_bit(0)
#define         ECC_PG_IRQ_SEL          nandx_bit(1)
#define NFIECC_ENCIRQSTA        0x084
#define         ENC_IRQSTA_GEN          nandx_bit(0)
#define NFIECC_PIO_DIRDY        0x090
#define         PIO_DI_RDY              nandx_bit(0)
#define NFIECC_PIO_DI           0x094
#define NFIECC_DECCON           0x100
#define NFIECC_DECCNFG          0x104
#define         DEC_BURST_EN            nandx_bit(8)
#define         DEC_EMPTY_EN            nandx_bit(31)
#define         DEC_CON_SHIFT           12
#define         DECCNFG_MS_SHIFT        16
#define NFIECC_DECDIADDR        0x108
#define NFIECC_DECIDLE          0x10c
#define NFIECC_DECENUM(x)       (0x114 + (x) * 4)
#define NFIECC_DECDONE          0x124
#define NFIECC_DECIRQEN         0x200
#define NFIECC_DECIRQSTA        0x204
#define         DEC_IRQSTA_GEN          nandx_bit(0)
#define NFIECC_DECFSM           0x208
#define         FSM_MASK                0x3f3fff0f
#define         FSM_IDLE                0x01011101
#define NFIECC_BYPASS           0x20c
#define         NFIECC_BYPASS_EN        nandx_bit(0)
#define NFIECC_ENCPAR(x)        (0x300 + (x) * 4)
#define NFIECC_DECEL(x)         (0x500 + (x) * 4)

