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

#include "ufs_hcd.h"

enum ufs_logical_unit {
    UFS_LU_UNKNOWN = 0,
    UFS_LU_BOOT1,
    UFS_LU_BOOT2,
    UFS_LU_USER,
    UFS_LU_RPMB,
    UFS_LU_END
};

enum {
    UFS_MODE_DEFAULT    = 0,
    UFS_MODE_DMA        = 1,
    UFS_MODE_PIO        = 2,
    UFS_MODE_MAX        = 3
};

/* Well-known LUN */

#define WLUN_ID (1<<7)
#define WLUN_REPORT_LUNS (WLUN_ID | 0x1)
#define WLUN_UFS_DEVICE (WLUN_ID | 0x50)
#define WLUN_BOOT (WLUN_ID | 0x30)
#define WLUN_RPMB (WLUN_ID | 0x44)

int     ufs_block_read(u32 host_id, u32 lun, u32 blk_start, u32 blk_cnt, unsigned long *buf);
int     ufs_block_write(u32 host_id, u32 lun, u32 blk_start, u32 blk_cnt, unsigned long *buf);
int     ufs_cfg_mode(u32 host_id, u8 mode);

void    ufs_scsi_cmd_inquiry(struct ufs_scsi_cmd *cmd, u32 tag, u32 lun,
    unsigned long *buf, u32 evpd, u32 page);
void    ufs_scsi_cmd_read_capacity(struct ufs_scsi_cmd *cmd, u32 tag, u32 lun,
    unsigned long *buf);
void    ufs_scsi_cmd_read10(struct ufs_scsi_cmd *cmd, u32 tag, u32 lun, u32 lba,
    u32 blk_cnt, unsigned long *buf, u32 attr);
void    ufs_scsi_cmd_write10(struct ufs_scsi_cmd *cmd, u32 tag, u32 lun, u32 lba,
    u32 blk_cnt, unsigned long *buf, u32 attr);
void    ufs_scsi_cmd_test_unit_ready(struct ufs_scsi_cmd *cmd, u32 tag, u32 lun);
void    ufs_scsi_cmd_start_stop_unit_pd(struct ufs_scsi_cmd *cmd, u32 tag);
int     ufs_switch_part(u32 part_id);


