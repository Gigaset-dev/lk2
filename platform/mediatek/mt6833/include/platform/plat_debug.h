/*
 * Copyright (c) 2022 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once
#ifdef PLAT_DFD_MCU_READY
#include <platform/plat_dfd_mcu.h>
#endif

/* Setting SYS_LAST_CPU_BUS dump size */
#ifdef PLAT_DFD_MCU_BUF_LENGTH
#define LAST_DUMP_SIZE              (LASTPC_BUF_LENGTH + \
                                     LASTBUS_BUF_LENGTH + \
                                     TRACKER_BUF_LENGTH)
#else
#define LAST_DUMP_SIZE              (LASTPC_BUF_LENGTH + \
                                     LASTBUS_BUF_LENGTH + \
                                     TRACKER_BUF_LENGTH)
#endif

/* Tracker INFO */

#define LAGACY_TRACKER_HWIP
#define HAS_INFA_TRACKER
#define HAS_PERI_TRACKER
#define HAS_SLV
#define SYS_TRACK_ENTRY             64
#define INFRA_ENTRY_NUM             32
#define PERI_ENTRY_NUM              16
#define BYTE_OFF                     8

#define AR_TRACK_LOG_OFFSET         0x0200
#define AR_TRACK_L_OFFSET           0x0100
#define AR_TRACK_H_OFFSET           0x0104
#define AW_TRACK_LOG_OFFSET         0x0800
#define AW_TRACK_L_OFFSET           0x0300
#define AW_TRACK_H_OFFSET           0x0304
#define W_TRACK_DATA_VALID_OFFSET   0x0020

/* AP tracker FNEW OFFSET */

#define TRACKER_VALID_S             24
#define TRACKER_VALID_E             24
#define TRACKER_SECURE_S            23
#define TRACKER_SECURE_E            23
#define TRACKER_ID_S                10
#define TRACKER_ID_E                22
#define TRACKER_DATA_SIZE_S          6
#define TRACKER_DATA_SIZE_E          8
#define TRACKER_BURST_LEN_S          2
#define TRACKER_BURST_LEN_E          5

/* Infra tracker FAXI OFFSET */

#define INFRA_VALID_S               26
#define INFRA_VALID_E               26
#define INFRA_SECURE_S              25
#define INFRA_SECURE_E              25
#define INFRA_ID_S                  11
#define INFRA_ID_E                  24

/* Peri tracker QAXI OFFSET */

#define PERI_VALID_S               25
#define PERI_VALID_E               25
#define PERI_SECURE_S              24
#define PERI_SECURE_E              24
#define PERI_ID_S                   8
#define PERI_ID_E                  23
