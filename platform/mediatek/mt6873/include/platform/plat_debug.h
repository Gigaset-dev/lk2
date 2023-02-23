/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#define HAS_INFA_TRACKER
#define HAS_PERI_TRACKER
#define HAS_SLV

#define SYS_TRACK_ENTRY             64
#define LAST_DUMP_SIZE              0x1000

#define INFRA_ENTRY_NUM             32
#define PERI_ENTRY_NUM              16

#define BUSTRACKER_TIMEOUT          0x300

#define AR_TRACK_L_OFFSET           0x0100
#define AR_TRACK_H_OFFSET           0x0104
#define AW_TRACK_L_OFFSET           0x0300
#define AW_TRACK_H_OFFSET           0x0304
#define W_TRACK_DATA_VALID_OFFSET   0x0020

/*FNEW OFFSET*/
#define TRACKER_VALID_S             21
#define TRACKER_VALID_E             21
#define TRACKER_SECURE_S            20
#define TRACKER_SECURE_E            20
#define TRACKER_ID_S                 8
#define TRACKER_ID_E                19
#define TRACKER_DATA_SIZE_S          4
#define TRACKER_DATA_SIZE_E          6
#define TRACKER_BURST_LEN_S          0
#define TRACKER_BURST_LEN_E          3
