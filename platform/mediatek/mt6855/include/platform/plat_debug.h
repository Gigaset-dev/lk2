/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once
#ifdef PLAT_DFD_MCU_READY
#include <platform/plat_dfd_mcu.h>
#endif

#define TRACER_ETB_BUF_LENGTH     (0x800)

#ifndef TRACER_ETB_BUF_LENGTH
#define ETB_BUF_LENGTH              (0x0)
#else
#define ETB_BUF_LENGTH              TRACER_ETB_BUF_LENGTH
#endif

/* Setting SYS_LAST_CPU_BUS dump size */
#ifdef PLAT_DFD_MCU_BUF_LENGTH
#define LAST_DUMP_SIZE              (LASTPC_BUF_LENGTH + \
                                     RETURN_STACK_BUF_LENGTH + \
                                     LASTBUS_BUF_LENGTH + \
                                     TRACKER_BUF_LENGTH + \
                                    (ETB_BUF_LENGTH * 3) + \
                                     PLAT_DFD_MCU_BUF_LENGTH)
#else
#define LAST_DUMP_SIZE              (LASTPC_BUF_LENGTH + \
                                     RETURN_STACK_BUF_LENGTH + \
                                     LASTBUS_BUF_LENGTH + \
                                     TRACKER_BUF_LENGTH + \
                                    (ETB_BUF_LENGTH * 3))
#endif

/* Tracker INFO */
#define HAS_INFA_TRACKER
#define HAS_SLV
#define SYS_TRACK_ENTRY             64

#define AR_TRACK_LOG_OFFSET         0x0200
#define AR_TRACK_L_OFFSET           0x0400
#define AR_TRACK_H_OFFSET           0x0600
#define AW_TRACK_LOG_OFFSET         0x0800
#define AW_TRACK_L_OFFSET           0x0A00
#define AW_TRACK_H_OFFSET           0x0C00
#define W_TRACK_DATA_VALID_OFFSET   0x0020

/* AP tracker FNEW OFFSET */

#define TRACKER_VALID_S             23
#define TRACKER_VALID_E             23
#define TRACKER_SECURE_S            21
#define TRACKER_SECURE_E            21
#define TRACKER_ID_S                 8
#define TRACKER_ID_E                20
#define TRACKER_DATA_SIZE_S          4
#define TRACKER_DATA_SIZE_E          6
#define TRACKER_BURST_LEN_S          0
#define TRACKER_BURST_LEN_E          3

/* Infra tracker FAXI OFFSET */

#define INFRA_VALID_S               26
#define INFRA_VALID_E               26
#define INFRA_SECURE_S              24
#define INFRA_SECURE_E              24
#define INFRA_ID_S                   8
#define INFRA_ID_E                  23
