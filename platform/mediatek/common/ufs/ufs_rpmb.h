/*
 * Copyright (c) 2020 MediaTek Inc.
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


#include "ufs.h"
#include "ufs_cfg.h"
#include "ufs_core.h"
#include "ufs_hcd.h"
#include "ufs_types.h"


#define UFS_SECURITY_PROTOCOL   (0xEC)
#define RPMB_PROTOCOL_ID        (0x1)
#define RPMB_FRM_STUFF_LEN      (196)
#define RPMB_FRM_KEY_MAC_LEN    (32)
#define RPMB_FRM_DATA_LEN       (256)
#define RPMB_FRM_NONCE_LEN      (16)
#define RPMB_FRM_WR_CNT_LEN     (4)
#define RPMB_FRM_ADDR_LEN       (2)
#define RPMB_FRM_BLK_CNT_LEN    (2)
#define RPMB_FRM_RESULT_LEN     (2)
#define RPMB_FRM_REQ_RESP_LEN   (2)
#define RPMB_SWPCB_VALID_DATA_SIZE (80)  /* Maximum valid length of SWPCB */
#define HMAC_TEXT_LEN           (284)


static const char rpmb_oper_result[][64] = {
    "Operation OK",
    "General failure",
    "Authentication failure(MAC not match)",
    "Counter failure(Counter not match or Inc fail) ",
    "Address failure (out of range or wrong alignment)",
    "write failure (data/counter/result wite failure)",
    "Read failure (data/counter.result read failure)",
    "Authentication Key not yet programmed",
    "Secure Write Protect Configration Block access failure",
    "Invalid Secure Write Protect Block Configration parameter",
    "Secure Write Protection not applicable",
};


struct rpmb_data_frame {
    u8 stuff[RPMB_FRM_STUFF_LEN];
    u8 key_MAC[RPMB_FRM_KEY_MAC_LEN];
    u8 data[RPMB_FRM_DATA_LEN];
    u8 nonce[RPMB_FRM_NONCE_LEN];

    u32 wr_cnt;
    u16 addr;
    u16 blk_cnt;
    u16 result;
    u16 req_resp;
};

enum enum_rpmb_msg {
    RPMB_REQ_AUTHEN_KEY_PRG     = 0x0001,
    RPMB_REQ_READ_WR_CNT        = 0x0002,
    RPMB_REQ_AUTHEN_WRITE       = 0x0003,
    RPMB_REQ_AUTHEN_READ        = 0x0004,
    RPMB_REQ_RESULT_RD          = 0x0005,
    RPMB_REQ_SEC_WPCB_WRITE     = 0x0006,
    RPMB_REQ_SEC_WPCB_READ      = 0x0007,

    RPMB_RSP_AUTHEN_KEY_PRG     = 0x0100,
    RPMB_RSP_READ_WR_CNT        = 0x0200,
    RPMB_RSP_AUTHEN_WRITE       = 0x0300,
    RPMB_RSP_AUTHEN_READ        = 0x0400,
    RPMB_RSP_SEC_WPCB_WRITE     = 0x0600,
    RPMB_RSP_SEC_WPCB_READ      = 0x0700,
};

enum {
    WP_PERMANENT = 0,
    WP_POWER_ON,
    WP_PERMANENT_AWP,
    /*
     * Important: do not change the enum above.
     * They are used as assigned value.
     */
    WP_TEMPORARY,
    WP_DISABLE,
};

#define SWP_ENTRY_RESERVED       (3)
#define WPF_BIT                  (0)
#define WPF_SIZE                 (1)
#define WPF_MASK                 (0xFE)
#define WPT_BIT                  (1)
#define WPT_SIZE                 (2)
#define WPT_MASK                 (0xF9)
struct secure_wp_entry {
    u8 wpt_wpf;
    u8 reserved1[SWP_ENTRY_RESERVED];
    u32 upper_addr;
    u32 lower_addr;
    u32 block_num;
} __PACKED;

#define SWPCB_RESERVED1            (14)
#define SWPWP_ENTRY_NUM            (4)
#define SWPCB_RESERVED2            (176)
struct secure_wp_config_block {
    u8 lun;
    u8 data_length;
    u8 reserved1[SWPCB_RESERVED1];
    struct secure_wp_entry wp_entry[SWPWP_ENTRY_NUM];
    u8 reserved2[SWPCB_RESERVED2];
} __PACKED;

#define WP_WPF_EN                  (1)
#define WP_WPF_DISABLE             (0)
#define WP_ENTRY_SIZE             (16)
#define WP_MAX_ENTRY_SIZE         (WP_ENTRY_SIZE * SWPWP_ENTRY_NUM)
#define WP_CONF_BLOCK_SIZE        (256)


/* UFS rpmb relate operation */
int init_ufs_rpmb_sharemem(void);
int deinit_ufs_rpmb_sharemem(void);


