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

#include <ufs_cfg.h>
#include "ufs.h"
#include "ufs_types.h"
#include "ufs_utils.h"

#include <debug.h>
#include <endian.h>
#include <malloc.h>
#include <string.h> /* for memcpy, memset */

struct ufs_ocs_error g_ufs_ocs_error_str[] = {
    { "ABORT", 0 },
    { "ABORT_TM", 1 },
    { "FATAL_ERROR_TRANSACTION_TYPE_TM", 2 },
    { "FATAL_ERROR_COMMAND_TYPE_NOT_ONE", 3 },
    { "FATAL_ERROR_RUO_LESS_THAN_EIGHT", 4 },
    { "FATAL_ERROR_RUL_LESS_THAN_EIGHT", 5 },
    { "FATAL_ERROR_RTT_UPIU_MISMATCH_SIZE", 6 },
    { "FATAL_ERROR_DATAIN_UPIU_MISMATCH_SIZE", 7 },
    { "FATAL_ERROR_NOPI_UPIU_MISMATCH_SIZE", 8 },
    { "FATAL_ERROR_REJECT_UPIU_MISMATCH_SIZE", 9 },
    { "FATAL_ERROR_WRONG_RECEIVED_UPIU", 10 },
    { "FATAL_ERROR_LOSE_TRANSFER_REQUEST", 11 },
    { "FATAL_ERROR_LOSE_TM_REQUEST", 12 },
    { "INVALID_PRDT_ATTRIBUTES", 13 },
    { "INVALID_TASK_MANAGEMENT_FUNCTION_ATTRIBUTES_DSL_NOT_ZERO", 14 },
    { "INVALID_TASK_MANAGEMENT_FUNCTION_ATTRIBUTES_FUNC_ERROR", 15 },
    { "INVALID_CMD_TABLE_ATTRIBUTES_TRANSACTION_TYPE_NOT_ONE", 16 },
    { "INVALID_CMD_TABLE_ATTRIBUTES_COMMAND_SET_TYPE_NOT_ZERO", 17 },
    { "INVALID_CMD_TABLE_ATTRIBUTES_DSL_NOT_ZERO", 18 },
    { "INVALID_CMD_TABLE_ATTRIBUTES_PRDT_OFFSET_LESS_THAN_8", 19 },
    { "INVALID_CMD_TABLE_ATTRIBUTES_TRANSACTION_TYPE", 20 },
    { "PEER_COMMUNICATION_FAILURE_TM", 21 },
    { "PEER_COMMUNICATION_FAILURE", 22 },
    { "MISMATCH_TASK_MANAGEMENT_REQUEST_SIZE", 23 },
    { "MISMATCH_DATA_BUFFER_SIZE", 24 },
    { "MISMATCH_RESPONSE_UPIU_SIZE", 25 },
    { "MISMATCH_TASK_MANAGEMENT_RESPONSE_SIZE_UPIU", 26 },
    { "MISMATCH_TASK_MANAGEMENT_RESPONSE_SIZE_DSL_NOT_ZERO", 27 },
    { NULL, 0xFF },
};

void ufs_mtk_dump_asc_ascq(struct ufs_hba *hba, u8 asc, u8 ascq)
{
    if (asc == 0x21) {
        if (ascq == 0x00)
            dprintf(CRITICAL, "[UFS] err: LBA out of range!\n");
    } else if (asc == 0x25) {
        if (ascq == 0x00)
            dprintf(CRITICAL, "[UFS] err: Logical unit not supported!\n");
    } else if (asc == 0x29) {
        if (ascq == 0x00)
            dprintf(CRITICAL, "[UFS] warn: Power on, reset, or bus device reset occupied\n");
    }

    dprintf(CRITICAL, "[UFS] Sense Data: ASC=%x, ASCQ=%x\n", asc, ascq);
}

static inline unsigned long get_utf16(unsigned int c, enum utf16_endian endian)
{
    switch (endian) {
    default:
            return c;
    case UTF16_LITTLE_ENDIAN:
        return (u16)(c);
    case UTF16_BIG_ENDIAN:
        return SWAP_16(c);
    }
}

int utf16s_to_utf8s(const wchar_t *pwcs, int inlen, enum utf16_endian endian, u8 *s, int maxout)
{
    u8 *op;
    unsigned long u;

    op = s;

    while (inlen > 0 && maxout > 0) {

        u = get_utf16(*pwcs, endian);

        if (!u)
            break;

        pwcs++;
        inlen--;

        if (u > 0x7f)
            return 0;   // not support non-ascii code in UFS AIO driver.
        else {
            *op++ = (u8) u;
            maxout--;
        }
    }
    return op - s;
}

int ufs_util_sanitize_inquiry_string(unsigned char *s, int len)
{
    int processed_char = 0;

    for (; len > 0; (--len, ++s, ++processed_char)) {
        if (*s == 0)
            return processed_char; // return string length

        if (*s < 0x20 || *s > 0x7e)
            *s = ' ';
    }

    return processed_char;
}

