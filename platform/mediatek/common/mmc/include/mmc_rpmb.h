/*
 *
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <mmc_core.h>

/*=======================================================================*/
/* RPMB definition                                                       */
/*=======================================================================*/
#define RPMB_SZ_STUFF 196
#define RPMB_SZ_MAC   32
#define RPMB_SZ_DATA  256
#define RPMB_SZ_NONCE 16

#define RPMB_PROGRAM_KEY       1       /* Program RPMB Authentication Key */
#define RPMB_GET_WRITE_COUNTER 2       /* Read RPMB write counter */
#define RPMB_WRITE_DATA        3       /* Write data to RPMB partition */
#define RPMB_READ_DATA         4       /* Read data from RPMB partition */
#define RPMB_RESULT_READ       5       /* Read result request */
#define RPMB_REQ               1       /* RPMB request mark */
#define RPMB_RESP              (1 << 1)/* RPMB response mark */
#define RPMB_AVAILABLE_SECTORS 8       /* 4K page size */

#define RPMB_TYPE_BEG          510
#define RPMB_RES_BEG           508
#define RPMB_BLKS_BEG          506
#define RPMB_ADDR_BEG          504
#define RPMB_WCOUNTER_BEG      500

#define RPMB_NONCE_BEG         484
#define RPMB_DATA_BEG          228
#define RPMB_MAC_BEG           196

struct mmc_rpmb_cfg {
    u16 type;                     /* RPMB request type */
    u16 result;                   /* response or request result */
    u16 blk_cnt;                  /* Number of blocks(half sector 256B) */
    u16 addr;                     /* data address */
    u32 wc;                       /* write counter */
    u8 *nonce;                    /* Ramdom number */
    u8 *data;                     /* Buffer of the user data */
    u8 *mac;                      /* Message Authentication Code */
};

struct mmc_rpmb_req {
    struct mmc_rpmb_cfg *rpmb_cfg;
    u8 *data_frame;
};

enum {
    RPMB_HMAC_ERROR = 1,
    RPMB_RESULT_ERROR,
    RPMB_NONCE_ERROR,
};

#if CFG_RPMB_KEY_PROGRAMED_IN_KERNEL
int mmc_rpmb_set_key(u8 *key, int *rpmb_result);
#else
int mmc_rpmb_set_key(u8 *key);
#endif
int init_rpmb_sharemem(void);
int mmc_rpmb_read_data(u32 blkAddr, u8 *buf, u32 bufLen, int *result);
int mmc_rpmb_write_data(u32 blkAddr, const u8 *buf, u32 bufLen, int *result);
u32 mmc_rpmb_get_size(void);
u32 mmc_rpmb_get_rel_wr_sec_c(void);
int deinit_rpmb_sharemem(void);
void mmc_dummy_rpmb_write(void);
