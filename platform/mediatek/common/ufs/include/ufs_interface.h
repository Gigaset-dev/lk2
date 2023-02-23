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

#include <errno.h>
#include <sys/types.h>

enum {
    UFS_RPMB_HMAC_ERROR = 1,
    UFS_RPMB_RESULT_ERROR,
    UFS_RPMB_NONCE_ERROR,
    UFS_RPMB_RESPONSE_ERROR,
    UFS_RPMB_WRCNT_ERROR,
    UFS_RPMB_DRIVER_ERROR
};

/* Align with partition_utils.h */
#define UFS_BOOT             (2)

#define MAX_PRODUCT_ID_LEN              (16)
#define MAX_SERAL_NUMBER_LEN            (64) /* spec (126*2), 64 because code size */
struct ufs_unique_id {
    u16 vid;
    char pid[MAX_PRODUCT_ID_LEN + 1];
    char sn[MAX_SERAL_NUMBER_LEN * 2 + 1];
};

int                 ufs_init(void);
int                 ufs_deinit(void);

/* For generally ufs information query */
int                 ufs_get_active_boot_part(u32 *active_boot_part);
unsigned long long  ufs_get_device_size(void);
int                 ufs_get_unique_id(struct ufs_unique_id *id);

/* For write protect, OTP */
int ufs_set_write_protect(int dev_num, u8 partition, unsigned long blknr,
    u32 blkcnt, u8 type);
int ufs_clr_write_protect(void);

/* For generally rpmb read/write */
int ufs_otp_lock_req(char *otp_part_name);
int ufs_rpmb_read_data(u32 addr, u32 *buf, u32 buf_len, int *result);
int ufs_rpmb_write_data(u32 addr, u32 *buf, u32 buf_len, int *result);
u64 ufs_rpmb_get_lu_size(void);
u32 ufs_rpmb_get_rw_size(void);
u32 ufs_rpmb_get_setkey_flag(void);

