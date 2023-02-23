/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <platform/addressmap.h>
#include <platform/plat_debug.h>

#define BUS_DBG_CON                 (BUS_DBG_BASE + 0x0000)
#define INFRA_TRACKER_CON           (INFRA_TRACKER_BASE + 0x0000)
#define BUS_DBG_TIMEOUT_INFO        (BUS_DBG_BASE + 0x0028)
#define INFRA_TRACKER_TIMEOUT_INFO  (INFRA_TRACKER_BASE + 0x0028)

/* After SPOID0567 the BUS_DBG_AR_TRACK_LOG offset has changed */
#ifdef LAGACY_TRACKER_HWIP
#define BYTE_OFF                    (8)
#else
#define BYTE_OFF                    (4)
#endif

#define AR_TRACK_L(__base, __n)     (__base + AR_TRACK_L_OFFSET + BYTE_OFF * (__n))
#define AW_TRACK_L(__base, __n)     (__base + AW_TRACK_L_OFFSET + BYTE_OFF * (__n))
#define AR_TRACK_H(__base, __n)     (__base + AR_TRACK_H_OFFSET + BYTE_OFF * (__n))
#define AW_TRACK_H(__base, __n)     (__base + AW_TRACK_H_OFFSET + BYTE_OFF * (__n))

#ifndef LAGACY_TRACKER_HWIP
#define BUS_DBG_AR_TRACK_LOG(__base, __n)     (__base + AR_TRACK_LOG_OFFSET + BYTE_OFF * (__n))
#define BUS_DBG_AW_TRACK_LOG(__base, __n)     (__base + AW_TRACK_LOG_OFFSET + BYTE_OFF * (__n))
#endif

#define BUS_DBG_W_TRACK_DATA_VALID  (BUS_DBG_BASE + W_TRACK_DATA_VALID_OFFSET)
#ifdef LAGACY_TRACKER_HWIP
#define BUS_DBG_W_TRACK_DATA62      (BUS_DBG_BASE + 0x05D8)
#define BUS_DBG_W_TRACK_DATA63      (BUS_DBG_BASE + 0x05DC)
#else
#define BUS_DBG_W_TRACK_DATA62      (BUS_DBG_BASE + 0x0EF8)
#define BUS_DBG_W_TRACK_DATA63      (BUS_DBG_BASE + 0x0EFC)
#endif

#define BUS_DBG_CON_IRQ_AR_STA0     (0x00000100)
#define BUS_DBG_CON_IRQ_AW_STA0     (0x00000200)
#define BUS_DBG_CON_IRQ_AR_STA1     (0x00100000)
#define BUS_DBG_CON_IRQ_AW_STA1     (0x00200000)
#define BUS_DBG_CON_TIMEOUT         (BUS_DBG_CON_IRQ_AR_STA0|BUS_DBG_CON_IRQ_AW_STA0| \
                                     BUS_DBG_CON_IRQ_AR_STA1|BUS_DBG_CON_IRQ_AW_STA1)

/* 8KB buffer is sufficient for tracker */
#define TRACKER_BUF_LENGTH          (32000)

/* INFRA BUS TRACKER */
#define INFRA_ENTRY_NUM             (32)
#define BUSTRACKER_TIMEOUT          (0x300)

#define AR_STALL_TIMEOUT            (0x00000080)
#define AW_STALL_TIMEOUT            (0x00008000)
#define AR_RESP_ERR_TYPE_OFFSET     (24)
#define AR_RESP_ERR_CHECK_OFFSET    (25)
#define AW_RESP_ERR_TYPE_OFFSET     (26)
#define AW_RESP_ERR_CHECK_OFFSET    (27)
#define AR_RESP_ERR_CHECK_MASK      (0x02000000)
#define AR_RESP_ERR_TYPE_MASK       (0x03000000)
#define AW_RESP_ERR_CHECK_MASK      (0x08000000)
#define AW_RESP_ERR_TYPE_MASK       (0x0C000000)
#define DBG_SLV_CHECK               (AR_RESP_ERR_CHECK_MASK|AW_RESP_ERR_CHECK_MASK)
#define SLV_CHECK                   (1)
#define SLV_ERR                     (2)
#define DEC_ERR                     (3)

int tracker_get(void **data, int *len, unsigned int entry_num);
void tracker_put(void **data);

static inline unsigned int extract_n2mbits(unsigned int input, unsigned int n, unsigned int m)
{
    /*
     * 1. ~0 = 1111 1111 1111 1111 1111 1111 1111 1111
     * 2. ~0 << (m - n + 1) = 1111 1111 1111 1111 1100 0000 0000 0000
     * // assuming we are extracting 14 bits, the +1 is added for inclusive selection
     * 3. ~(~0 << (m - n + 1)) = 0000 0000 0000 0000 0011 1111 1111 1111
     */
    int mask;

    if (n > m) {
        n = n + m;
        m = n - m;
        n = n - m;
    }
    mask = ~(~0 << (m - n + 1));
    return (input >> n) & mask;
}
