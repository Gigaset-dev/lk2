/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <debug.h>
#include <errno.h>
#include <libfdt.h>
#include <malloc.h>
#include <platform/tracker.h>
#include <reg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <trace.h>

#define LOCAL_TRACE 0

static int systracker_dump(char *buf, int *wp, unsigned int entry_num)
{
    unsigned int i;
    unsigned int reg_value;
    unsigned int entry_valid;
    unsigned int entry_secure;
    unsigned int entry_id;
    unsigned int entry_address;
    unsigned int entry_data_size;
    unsigned int entry_burst_length;

    if (buf == NULL || wp == NULL)
        return -1;

    /* Get tracker info and save to buf */
    /* check if we got AP tracker timeout */
#ifdef LAGACY_TRACKER_HWIP
    if (readl(BUS_DBG_CON) & BUS_DBG_CON_TIMEOUT) {
#else
    if (readl(BUS_DBG_CON) & (BUS_DBG_CON_TIMEOUT | DBG_SLV_CHECK)) {
#endif
        *wp += snprintf(buf + *wp, TRACKER_BUF_LENGTH - *wp,
               "\n*************************** systracker ***************************\n");
        *wp += snprintf(buf + *wp, TRACKER_BUF_LENGTH - *wp,
                                   "ap tracker timeout (0x%08x)\n", readl(BUS_DBG_CON));

        for (i = 0; i < entry_num; i++) {
            entry_address      = readl(AR_TRACK_L(BUS_DBG_BASE, i));
#ifdef LAGACY_TRACKER_HWIP
            reg_value          = readl(AR_TRACK_H(BUS_DBG_BASE, i));
#else
            reg_value          = readl(BUS_DBG_AR_TRACK_LOG(BUS_DBG_BASE, i));
#endif
            entry_valid        = extract_n2mbits(reg_value, TRACKER_VALID_S, TRACKER_VALID_E);
            entry_secure       = extract_n2mbits(reg_value, TRACKER_SECURE_S, TRACKER_SECURE_E);
            entry_id           = extract_n2mbits(reg_value, TRACKER_ID_S, TRACKER_ID_E);
            entry_data_size    = extract_n2mbits(reg_value,
                                                 TRACKER_DATA_SIZE_S, TRACKER_DATA_SIZE_E);
            entry_burst_length = extract_n2mbits(reg_value,
                                                 TRACKER_BURST_LEN_S, TRACKER_BURST_LEN_E);

            *wp += snprintf(buf + *wp, TRACKER_BUF_LENGTH - *wp,
                                       "read entry = %d, valid = 0x%x, non-secure = 0x%x, read id = 0x%x, address = 0x%x, data_size = 0x%x, burst_length = 0x%x\n",
                                       i, entry_valid, entry_secure, entry_id,
                                       entry_address, entry_data_size, entry_burst_length);
            }

        for (i = 0; i < entry_num; i++) {
            entry_address      = readl(AW_TRACK_L(BUS_DBG_BASE, i));
#ifdef LAGACY_TRACKER_HWIP
            reg_value          = readl(AW_TRACK_H(BUS_DBG_BASE, i));
#else
            reg_value          = readl(BUS_DBG_AW_TRACK_LOG(BUS_DBG_BASE, i));
#endif
            entry_valid        = extract_n2mbits(reg_value, TRACKER_VALID_S, TRACKER_VALID_E);
            entry_secure       = extract_n2mbits(reg_value, TRACKER_SECURE_S, TRACKER_SECURE_E);
            entry_id           = extract_n2mbits(reg_value, TRACKER_ID_S, TRACKER_ID_E);
            entry_data_size    = extract_n2mbits(reg_value,
                                                 TRACKER_DATA_SIZE_S, TRACKER_DATA_SIZE_E);
            entry_burst_length = extract_n2mbits(reg_value,
                                                 TRACKER_BURST_LEN_S, TRACKER_BURST_LEN_E);

            *wp += snprintf(buf + *wp, TRACKER_BUF_LENGTH - *wp,
                                       "write entry = %d, valid = 0x%x, non-secure = 0x%x, write id = 0x%x, address = 0x%x, data_size = 0x%x, burst_length = 0x%x\n",
                                       i, entry_valid, entry_secure, entry_id, entry_address,
                                       entry_data_size, entry_burst_length);
        }

        *wp += snprintf(buf + *wp, TRACKER_BUF_LENGTH - *wp,
                                   "write entry ~ %d, valid = 0x%x, data = 0x%x\n", entry_num - 2,
                                   ((readl(BUS_DBG_W_TRACK_DATA_VALID)&(0x1<<6))>>6),
                                     readl(BUS_DBG_W_TRACK_DATA62));

        *wp += snprintf(buf + *wp, TRACKER_BUF_LENGTH - *wp,
                                   "write entry ~ %d, valid = 0x%x, data = 0x%x\n\n", entry_num - 1,
                                   ((readl(BUS_DBG_W_TRACK_DATA_VALID)&(0x1<<7))>>7),
                                     readl(BUS_DBG_W_TRACK_DATA63));
    }

#ifdef HAS_INFA_TRACKER
    /* check if we got infra tracker timeout */
#ifdef LAGACY_TRACKER_HWIP
    if (readl(INFRA_TRACKER_CON) & BUSTRACKER_TIMEOUT) {
#else
    if (readl(INFRA_TRACKER_CON) & (BUS_DBG_CON_TIMEOUT | DBG_SLV_CHECK)) {
#endif
        *wp += snprintf(buf + *wp, TRACKER_BUF_LENGTH - *wp,
                        "\n*************************** INFRA ***************************\n");
        *wp += snprintf(buf + *wp, TRACKER_BUF_LENGTH - *wp,
                                   "infra tracker timeout (0x%08x)\n", readl(INFRA_TRACKER_CON));
        for (i = 0; i < INFRA_ENTRY_NUM; i++) {
            entry_address = readl(AR_TRACK_L(INFRA_TRACKER_CON, i));
#ifdef LAGACY_TRACKER_HWIP
            reg_value = readl(AR_TRACK_H(INFRA_TRACKER_CON, i));
#else
            reg_value = readl(BUS_DBG_AR_TRACK_LOG(INFRA_TRACKER_CON, i));
#endif
            entry_valid        = extract_n2mbits(reg_value, INFRA_VALID_S, INFRA_VALID_E);
            entry_secure       = extract_n2mbits(reg_value, INFRA_SECURE_S, INFRA_SECURE_E);
            entry_id           = extract_n2mbits(reg_value, INFRA_ID_S, INFRA_ID_E);
            *wp += snprintf(buf + *wp, TRACKER_BUF_LENGTH - *wp,
                                       "read entry = %d, valid = 0x%x, non-secure = 0x%x, id = 0x%x, address = 0x%x\n",
                                       i, entry_valid, entry_secure, entry_id, entry_address);
        }
        for (i = 0; i < INFRA_ENTRY_NUM; i++) {
            entry_address = readl(AW_TRACK_L(INFRA_TRACKER_CON, i));
#ifdef LAGACY_TRACKER_HWIP
            reg_value = readl(AW_TRACK_H(INFRA_TRACKER_CON, i));
#else
            reg_value = readl(BUS_DBG_AW_TRACK_LOG(INFRA_TRACKER_CON, i));
#endif
            entry_valid        = extract_n2mbits(reg_value, INFRA_VALID_S, INFRA_VALID_E);
            entry_secure       = extract_n2mbits(reg_value, INFRA_SECURE_S, INFRA_SECURE_E);
            entry_id           = extract_n2mbits(reg_value, INFRA_ID_S, INFRA_ID_E);
            *wp += snprintf(buf + *wp, TRACKER_BUF_LENGTH - *wp,
                                       "write entry = %d, valid = 0x%x, non-secure = 0x%x, id = 0x%x, address = 0x%x\n",
                                       i, entry_valid, entry_secure, entry_id, entry_address);
        }
    }
#endif

#ifdef HAS_PERI_TRACKER
    /* check if we got peri tracker timeout */
#ifdef LAGACY_TRACKER_HWIP
    if (readl(PERI_TRACKER_BASE) & BUSTRACKER_TIMEOUT) {
#else
    if (readl(PERI_TRACKER_BASE) & (BUSTRACKER_TIMEOUT | DBG_SLV_CHECK)) {
#endif
        *wp += snprintf(buf + *wp, TRACKER_BUF_LENGTH - *wp,
                        "\n*************************** PERI ***************************\n");
        *wp += snprintf(buf + *wp, TRACKER_BUF_LENGTH - *wp,
                "peri tracker timeout (0x%08x)\n", readl(PERI_TRACKER_BASE));
        for (i = 0; i < PERI_ENTRY_NUM; i++) {
            entry_address = readl(AR_TRACK_L(PERI_TRACKER_BASE, i));
#ifdef LAGACY_TRACKER_HWIP
            reg_value = readl(AR_TRACK_H(PERI_TRACKER_BASE, i));
#else
            reg_value = readl(BUS_DBG_AR_TRACK_LOG(PERI_TRACKER_BASE, i));
#endif
            entry_valid        = extract_n2mbits(reg_value, PERI_VALID_S, PERI_VALID_E);
            entry_secure       = extract_n2mbits(reg_value, PERI_SECURE_S, PERI_SECURE_E);
            entry_id           = extract_n2mbits(reg_value, PERI_ID_S, PERI_ID_E);
            *wp += snprintf(buf + *wp, TRACKER_BUF_LENGTH - *wp,
                   "read entry = %d, valid = 0x%x, non-secure = 0x%x, id = 0x%x, address = 0x%x\n",
                    i, entry_valid, entry_secure, entry_id, entry_address);
        }
        for (i = 0; i < PERI_ENTRY_NUM; i++) {
            entry_address = readl(AW_TRACK_L(PERI_TRACKER_BASE, i));
#ifdef LAGACY_TRACKER_HWIP
            reg_value = readl(AW_TRACK_H(PERI_TRACKER_BASE, i));
#else
            reg_value = readl(BUS_DBG_AW_TRACK_LOG(PERI_TRACKER_BASE, i));
#endif
            entry_valid        = extract_n2mbits(reg_value, PERI_VALID_S, PERI_VALID_E);
            entry_secure       = extract_n2mbits(reg_value, PERI_SECURE_S, PERI_SECURE_E);
            entry_id           = extract_n2mbits(reg_value, PERI_ID_S, PERI_ID_E);
            *wp += snprintf(buf + *wp, TRACKER_BUF_LENGTH - *wp,
                   "write entry = %d, valid = 0x%x, non-secure = 0x%x, id = 0x%x, address = 0x%x\n",
                    i, entry_valid, entry_secure, entry_id, entry_address);
        }
    }
#endif

#ifdef HAS_SLV
    reg_value = readl(BUS_DBG_CON);
#ifndef LAGACY_TRACKER_HWIP
    if (SLV_CHECK == (reg_value & AR_RESP_ERR_CHECK_MASK) >> AR_RESP_ERR_CHECK_OFFSET)
        *wp += snprintf(buf + *wp, TRACKER_BUF_LENGTH - *wp,
                        "\n Please check error type and timeout info above.");
#endif
    if (SLV_ERR == (reg_value & AR_RESP_ERR_TYPE_MASK) >> AR_RESP_ERR_TYPE_OFFSET)
        *wp += snprintf(buf + *wp, TRACKER_BUF_LENGTH - *wp,
                                   "\n AP tracker detects AR slave error.");
    if (DEC_ERR == (reg_value & AR_RESP_ERR_TYPE_MASK) >> AR_RESP_ERR_TYPE_OFFSET)
        *wp += snprintf(buf + *wp, TRACKER_BUF_LENGTH - *wp,
                                   "\n AP tracker detects AR decode error.");
#ifndef LAGACY_TRACKER_HWIP
    if (SLV_CHECK == (reg_value & AW_RESP_ERR_CHECK_MASK) >> AW_RESP_ERR_CHECK_OFFSET)
        *wp += snprintf(buf + *wp, TRACKER_BUF_LENGTH - *wp,
                        "\n Please check error type and timeout info above.");
#endif
    if (SLV_ERR == (reg_value & AW_RESP_ERR_TYPE_MASK) >> AW_RESP_ERR_TYPE_OFFSET)
        *wp += snprintf(buf + *wp, TRACKER_BUF_LENGTH - *wp,
                                   "\n AP tracker detects AW slave error.");
    if (DEC_ERR == (reg_value & AW_RESP_ERR_TYPE_MASK) >> AW_RESP_ERR_TYPE_OFFSET)
        *wp += snprintf(buf + *wp, TRACKER_BUF_LENGTH - *wp,
                                   "\n AP tracker detects AW decode error.");

    reg_value = readl(BUS_DBG_TIMEOUT_INFO);
    if (reg_value & AR_STALL_TIMEOUT)
        *wp += snprintf(buf + *wp, TRACKER_BUF_LENGTH - *wp,
                                   "\n AP tracker detects AR stall timeout.");
    if (reg_value & AW_STALL_TIMEOUT)
        *wp += snprintf(buf + *wp, TRACKER_BUF_LENGTH - *wp,
                                   "\n AP tracker detects AW stall timeout.");

    reg_value = readl(INFRA_TRACKER_BASE);
#ifndef LAGACY_TRACKER_HWIP
    if (SLV_CHECK == (reg_value & AR_RESP_ERR_CHECK_MASK) >> AR_RESP_ERR_CHECK_OFFSET)
        *wp += snprintf(buf + *wp, TRACKER_BUF_LENGTH - *wp,
                        "\n Please check error type and timeout info above.");
#endif
    if (SLV_ERR == (reg_value & AR_RESP_ERR_TYPE_MASK) >> AR_RESP_ERR_TYPE_OFFSET)
        *wp += snprintf(buf + *wp, TRACKER_BUF_LENGTH - *wp,
                                   "\n Infra tracker detects AR slave error.");
    if (DEC_ERR == (reg_value & AR_RESP_ERR_TYPE_MASK) >> AR_RESP_ERR_TYPE_OFFSET)
        *wp += snprintf(buf + *wp, TRACKER_BUF_LENGTH - *wp,
                                   "\n Infra tracker detects AR decode error.");
#ifndef LAGACY_TRACKER_HWIP
    if (SLV_CHECK == (reg_value & AW_RESP_ERR_CHECK_MASK) >> AW_RESP_ERR_CHECK_OFFSET)
        *wp += snprintf(buf + *wp, TRACKER_BUF_LENGTH - *wp,
                        "\n Please check error type and timeout info above.");
#endif
    if (SLV_ERR == (reg_value & AW_RESP_ERR_TYPE_MASK) >> AW_RESP_ERR_TYPE_OFFSET)
        *wp += snprintf(buf + *wp, TRACKER_BUF_LENGTH - *wp,
                                   "\n Infra tracker detects AW slave error.");
    if (DEC_ERR == (reg_value & AW_RESP_ERR_TYPE_MASK) >> AW_RESP_ERR_TYPE_OFFSET)
        *wp += snprintf(buf + *wp, TRACKER_BUF_LENGTH - *wp,
                                   "\n Infra tracker detects AW decode error.");

    reg_value = readl(INFRA_TRACKER_TIMEOUT_INFO);
    if (reg_value & AR_STALL_TIMEOUT)
        *wp += snprintf(buf + *wp, TRACKER_BUF_LENGTH - *wp,
                                   "\n Infra tracker detects AR stall timeout.");
    if (reg_value & AW_STALL_TIMEOUT)
        *wp += snprintf(buf + *wp, TRACKER_BUF_LENGTH - *wp,
                                   "\n Infra tracker detects AW stall timeout.\n");
#endif

    LTRACEF("wp: %d\n", *wp);

    return strlen(buf);
}

int tracker_get(void **data, int *len, unsigned int entry_num)
{
    int ret;

    *len = 0;
    *data = malloc(TRACKER_BUF_LENGTH);
    if (*data == NULL)
        return 0;

    ret = systracker_dump(*data, len, entry_num);
    if (ret < 0 || *len > TRACKER_BUF_LENGTH) {
        *len = (*len > TRACKER_BUF_LENGTH) ? TRACKER_BUF_LENGTH : *len;
        return ret;
    }

    return 1;
}

void tracker_put(void **data)
{
    free(*data);
}
