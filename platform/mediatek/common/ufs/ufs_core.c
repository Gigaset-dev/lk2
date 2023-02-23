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
#include "ufs_core.h"
#include "ufs_error.h"
#include "ufs_hcd.h"
#include "ufs_types.h"
#include "ufs_utils.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h> //For memcpy, memset
#include <trace.h>

#define LOCAL_TRACE 0

int ufs_scsi_unmap(struct ufs_scsi_cmd *cmd, u32 tag, u32 lun, u32 lba, u32 blk_cnt,
    unsigned long *buf, u32 buf_size)
{
    u32 exp_len;
    struct ufs_unmap_param_list *p;

    memset(cmd->cmd_data, 0, MAX_CDB_SIZE);

    exp_len = sizeof(struct ufs_unmap_param_list);

    p = (struct ufs_unmap_param_list *)buf;

    if (buf_size < exp_len)
        return -1;

    p->unmap_data_length[0] = (exp_len - 2) >> 8;
    p->unmap_data_length[1] = (exp_len - 2) & 0xFF;
    p->unmap_block_descriptor_data_length[0] = (exp_len - 8) >> 8;
    p->unmap_block_descriptor_data_length[1] = (exp_len - 8) & 0xFF;
    p->unmap_block_descriptor.unmap_logical_block_address[0] = 0;   // lba is u32 only currently
    p->unmap_block_descriptor.unmap_logical_block_address[1] = 0;   // lba is u32 only currently
    p->unmap_block_descriptor.unmap_logical_block_address[2] = 0;   // lba is u32 only currently
    p->unmap_block_descriptor.unmap_logical_block_address[3] = 0;   // lba is u32 only currently
    p->unmap_block_descriptor.unmap_logical_block_address[4] = (lba >> 24) & 0xFF;
    p->unmap_block_descriptor.unmap_logical_block_address[5] = (lba >> 16) & 0xFF;
    p->unmap_block_descriptor.unmap_logical_block_address[6] = (lba >> 8) & 0xFF;
    p->unmap_block_descriptor.unmap_logical_block_address[7] = lba & 0xFF;
    p->unmap_block_descriptor.num_logical_blocks[0] = (blk_cnt >> 24) & 0xFF;
    p->unmap_block_descriptor.num_logical_blocks[1] = (blk_cnt >> 16) & 0xFF;
    p->unmap_block_descriptor.num_logical_blocks[2] = (blk_cnt >> 8) & 0xFF;
    p->unmap_block_descriptor.num_logical_blocks[3] = blk_cnt & 0xFF;

    cmd->lun = lun;
    cmd->tag = tag;
    cmd->dir = DMA_TO_DEVICE;
    // UNMAP parameter list (header): 8 bytes + 1 UNMAP block descriptor: 16 bytes
    cmd->exp_len = exp_len;
    cmd->attr = ATTR_SIMPLE;
    cmd->cmd_len = 10;

    cmd->cmd_data[0] = UNMAP;               // opcode
    cmd->cmd_data[7] = 0;                   // parameter list length (MSB)
    cmd->cmd_data[8] = exp_len;             // parameter list length (LSB)
    cmd->data_buf = buf;

    return 0;
}

void ufs_scsi_cmd_read_capacity(struct ufs_scsi_cmd *cmd, u32 tag, u32 lun,
    unsigned long *buf)
{
    memset(cmd->cmd_data, 0, MAX_CDB_SIZE);

    cmd->lun = lun;
    cmd->tag = tag;
    cmd->dir = DMA_FROM_DEVICE;
    cmd->exp_len = 8;   // size of Read Capacity Parameter Data
    cmd->attr = ATTR_SIMPLE;
    cmd->cmd_len = 10;

    cmd->cmd_data[0] = READ_CAPACITY;       // opcode
    cmd->data_buf = buf;
}

void ufs_scsi_cmd_inquiry(struct ufs_scsi_cmd *cmd, u32 tag, u32 lun,
    unsigned long *buf, u32 evpd, u32 page)
{
    memset(cmd->cmd_data, 0, MAX_CDB_SIZE);

    cmd->lun = lun;
    cmd->tag = tag;
    cmd->dir = DMA_FROM_DEVICE;
    cmd->exp_len = 36;
    cmd->attr = ATTR_SIMPLE;
    cmd->cmd_len = 6;

    cmd->cmd_data[0] = INQUIRY;     // opcode
    cmd->cmd_data[1] = evpd;         // evpd (0: standared INQUIRY DATA)
    cmd->cmd_data[2] = page;         // page code
    cmd->cmd_data[3] = 0x0;         // allocation length (MSB)
    cmd->cmd_data[4] = 36;          // allocation length (LSB)
    cmd->cmd_data[5] = 0x0;         // control
    cmd->data_buf = buf;
}

void ufs_scsi_cmd_read10(struct ufs_scsi_cmd *cmd, u32 tag, u32 lun, u32 lba, u32 blk_cnt,
    unsigned long *buf, u32 attr)
{
    memset(cmd->cmd_data, 0, MAX_CDB_SIZE);

    cmd->lun = lun;
    cmd->tag = tag;
    cmd->dir = DMA_FROM_DEVICE;
    cmd->exp_len = blk_cnt * UFS_BLOCK_SIZE;
    cmd->attr = attr;
    cmd->cmd_len = 10;
    cmd->cmd_data[0] = READ_10;             // opcode
    cmd->cmd_data[1] = (1 << 3);            // flags, FUA = 1
    cmd->cmd_data[2] = (lba >> 24) & 0xff;  // LBA (MSB)
    cmd->cmd_data[3] = (lba >> 16) & 0xff;  // LBA
    cmd->cmd_data[4] = (lba >> 8) & 0xff;   // LBA
    cmd->cmd_data[5] = (lba) & 0xff;        // LBA (LSB)
    cmd->cmd_data[6] = 0x0;                 // reserved and group number
    cmd->cmd_data[7] = (blk_cnt >> 8);      // transfer length (MSB)
    cmd->cmd_data[8] = blk_cnt;             // transfer length (LSB)
    cmd->cmd_data[9] = 0x0;                 // control
    cmd->data_buf = buf;
}

void ufs_scsi_cmd_write10(struct ufs_scsi_cmd *cmd, u32 tag, u32 lun, u32 lba, u32 blk_cnt,
    unsigned long *buf, u32 attr)
{
    memset(cmd->cmd_data, 0, MAX_CDB_SIZE);

    cmd->lun = lun;
    cmd->tag = tag;
    cmd->dir = DMA_TO_DEVICE;
    cmd->exp_len = blk_cnt * UFS_BLOCK_SIZE;
    cmd->attr = attr;
    cmd->cmd_len = 10;
    cmd->cmd_data[0] = WRITE_10;            // opcode
    cmd->cmd_data[1] = (1<<3);              // flags, FUA = 1
    cmd->cmd_data[2] = (lba >> 24) & 0xff;  // LBA (MSB)
    cmd->cmd_data[3] = (lba >> 16) & 0xff;  // LBA
    cmd->cmd_data[4] = (lba >> 8) & 0xff;   // LBA
    cmd->cmd_data[5] = (lba) & 0xff;        // LBA (LSB)
    cmd->cmd_data[6] = 0x0;                 // reserved and group number
    cmd->cmd_data[7] = (blk_cnt>>8);        // transfer length (MSB)
    cmd->cmd_data[8] = blk_cnt;             // transfer length (LSB)
    cmd->cmd_data[9] = 0x0;                 // control
    cmd->data_buf = buf;
}

void ufs_scsi_cmd_write_buffer(struct ufs_scsi_cmd *cmd, u32 tag, unsigned long *buf, u32 size)
{
    memset(cmd->cmd_data, 0, MAX_CDB_SIZE);

    cmd->lun = 0;
    cmd->tag = tag;
    cmd->dir = DMA_TO_DEVICE;
    cmd->exp_len = size;
    cmd->attr = ATTR_SIMPLE;
    cmd->cmd_len = 10;
    cmd->cmd_data[0] = WRITE_BUFFER;        /* Opcode */
    cmd->cmd_data[1] = 0xE;                 /* 0xE: Download firmware */
    cmd->cmd_data[2] = 0;                   /* Buffer ID = 0 */
    cmd->cmd_data[3] = 0;                   /* Buffer Offset[23:16] = 0 */
    cmd->cmd_data[4] = 0;                   /* Buffer Offset[15:08] = 0 */
    cmd->cmd_data[5] = 0;                   /* Buffer Offset[07:00] = 0 */
    cmd->cmd_data[6] = (size >> 16) & 0xff; /* Length[23:16] */
    cmd->cmd_data[7] = (size >> 8) & 0xff;  /* Length[15:08] */
    cmd->cmd_data[8] = (size) & 0xff;       /* Length[07:00] */
    cmd->cmd_data[9] = 0x0;                 /* Control = 0 */
    cmd->data_buf = buf;
}

void ufs_scsi_cmd_test_unit_ready(struct ufs_scsi_cmd *cmd, u32 tag, u32 lun)
{
    memset(cmd->cmd_data, 0, MAX_CDB_SIZE);

    cmd->lun = lun;
    cmd->tag = tag;
    cmd->dir = DMA_NONE;
    cmd->exp_len = 0;
    cmd->attr = ATTR_SIMPLE;
    cmd->cmd_len = 6;
    cmd->cmd_data[0] = TEST_UNIT_READY; //opcode
    cmd->data_buf = NULL;
}

void ufs_scsi_cmd_start_stop_unit_pd(struct ufs_scsi_cmd *cmd, u32 tag)
{
    memset(cmd->cmd_data, 0, MAX_CDB_SIZE);

    cmd->lun = 0xD0;
    cmd->tag = tag;
    cmd->dir = DMA_NONE;
    cmd->exp_len = 0;
    cmd->attr = ATTR_SIMPLE;
    cmd->cmd_len = 6;
    cmd->cmd_data[0] = SCSI_START_STOP_UNIT; //opcode
    cmd->cmd_data[1] = 0x0; //rsvd, BIT 0: 0
    cmd->cmd_data[2] = 0x0; //rsvd
    cmd->cmd_data[3] = 0x0; //rsvd
    cmd->cmd_data[4] = 0x30; //power condition: power down
    cmd->cmd_data[5] = 0x0; //control
    cmd->data_buf = NULL;
}

int ufs_block_read(u32 host_id, u32 lun, u32 blk_start, u32 blk_cnt, unsigned long *buf)
{
    int ret;
    u32 limit_blk_cnt = UFS_MAX_DATA_SIZE_PER_CMD_MB * 1024 * 1024 / UFS_BLOCK_SIZE;
    u32 read_blk_cnt;

    struct ufs_hba *hba = ufs_get_host(host_id);

    if (hba->blk_read) {

        while (blk_cnt) {
            read_blk_cnt = (blk_cnt > limit_blk_cnt) ? limit_blk_cnt : blk_cnt;

            ret = hba->blk_read(hba, lun, blk_start, read_blk_cnt, buf);

            if (ret)
                return ret;

            blk_start += read_blk_cnt;
            blk_cnt -= read_blk_cnt;
            buf += ((read_blk_cnt * UFS_BLOCK_SIZE) / sizeof(unsigned long));
        }
    } else {
        LTRACEF("[UFS] err: null blk_read ptr\n");
        return -1;
    }

    return 0;
}

int ufs_block_write(u32 host_id, u32 lun, u32 blk_start, u32 blk_cnt, unsigned long *buf)
{
    int ret;
    u32 limit_blk_cnt = UFS_MAX_DATA_SIZE_PER_CMD_MB * 1024 * 1024 / UFS_BLOCK_SIZE;
    u32 write_blk_cnt;

    struct ufs_hba *hba = ufs_get_host(host_id);

    if (hba->blk_write) {

        while (blk_cnt) {
            write_blk_cnt = (blk_cnt > limit_blk_cnt) ? limit_blk_cnt : blk_cnt;

            ret = hba->blk_write(hba, lun, blk_start, write_blk_cnt, buf);

            if (ret)
                return ret;

            blk_start += write_blk_cnt;
            blk_cnt -= write_blk_cnt;
            buf += ((write_blk_cnt * UFS_BLOCK_SIZE) / sizeof(unsigned long));
        }
    } else {
        LTRACEF("[UFS] err: null blk_write ptr\n");
        return -1;
    }

    return 0;
}

int ufs_dma_erase(struct ufs_hba *hba, u32 lun, u32 blk_start, u32 blk_cnt)
{
    struct ufs_scsi_cmd cmd;
    int tag;
    int ret;
    u8 *ufs_temp_buf = memalign(CACHE_LINE, UFS_TEMP_BUF_SIZE);
    /* malloc fail */
    ASSERT(ufs_temp_buf);

    if (!ufshcd_get_free_tag(hba, &tag)) {
        free(ufs_temp_buf);
        return -1;
    }

    ret = ufs_scsi_unmap(&cmd, tag, lun, blk_start, blk_cnt,
        (unsigned long *)ufs_temp_buf, UFS_TEMP_BUF_SIZE);

    if (ret != 0) {
        free(ufs_temp_buf);
        return ret;
    }

    ret = ufshcd_queuecommand(hba, &cmd);

    ufshcd_put_tag(hba, tag);

    free(ufs_temp_buf);
    return ret;
}

int ufs_dma_read(struct ufs_hba *hba, u32 lun, u32 blk_start, u32 blk_cnt, unsigned long *buf)
{
    struct ufs_scsi_cmd cmd;
    int tag;
    int ret;

    LTRACEF("[UFS] info: %s,L:%d,S:%d,C:%d,buf:%p\n", __func__, lun, blk_start,
        blk_cnt, buf);

    if (!ufshcd_get_free_tag(hba, &tag))
        return -1;

    ufs_scsi_cmd_read10(&cmd, tag, lun, blk_start, blk_cnt, buf, ATTR_SIMPLE);
    ret = ufshcd_queuecommand(hba, &cmd);

    ufshcd_put_tag(hba, tag);

    return ret;
}

int ufs_dma_write(struct ufs_hba *hba, u32 lun, u32 blk_start, u32 blk_cnt, unsigned long *buf)
{
    struct ufs_scsi_cmd cmd;
    int tag;
    int ret;

    LTRACEF("[UFS] info: %s,L:%d,S:%d,C:%d,buf:%p\n", __func__, lun, blk_start,
        blk_cnt, buf);

    if (!ufshcd_get_free_tag(hba, &tag))
        return -1;

    ufs_scsi_cmd_write10(&cmd, tag, lun, blk_start, blk_cnt, buf, ATTR_SIMPLE);
    ret = ufshcd_queuecommand(hba, &cmd);

    ufshcd_put_tag(hba, tag);

    return ret;
}

int ufs_dma_ffu(struct ufs_hba *hba, unsigned long *buf, u32 size)
{
    struct ufs_scsi_cmd cmd;
    int tag;
    int ret;

    LTRACEF("[UFS] info: %s,buf:%p,size:%d\n", __func__, buf, size);

    if (!ufshcd_get_free_tag(hba, &tag))
        return -1;

    ufs_scsi_cmd_write_buffer(&cmd, tag, buf, size);
    ret = ufshcd_queuecommand(hba, &cmd);

    ufshcd_put_tag(hba, tag);

    return ret;
}

static int ufs_cfg_callback(struct ufs_hba *hba, u8 mode)
{
    int ret = 0;

    hba->mode = mode;

    if (mode == UFS_MODE_DMA) {
        hba->blk_read = ufs_dma_read;
        hba->blk_write = ufs_dma_write;
        hba->nopin_nopout = ufs_dma_nopin_nopout;
        hba->query_flag = ufs_dma_query_flag;
        hba->read_descriptor = ufs_dma_read_desc;
        hba->dme_get = ufs_dma_dme_get;
        hba->dme_peer_get = ufs_dma_dme_peer_get;
        hba->dme_set = ufs_dma_dme_set;
        hba->dme_peer_set = ufs_dma_dme_peer_set;
        hba->blk_erase = ufs_dma_erase;
        hba->ffu_write = ufs_dma_ffu;
        hba->query_attr = ufs_dma_query_attr;
    } else {
        LTRACEF("[UFS] err: %s - invalid mode\n", __func__);
        ret = -1;
    }

    return ret;
}

int ufs_cfg_mode(u32 host_id, u8 mode)
{
    struct ufs_hba *hba = ufs_get_host(host_id);

    if (mode >= UFS_MODE_MAX) {
        LTRACEF("[UFS] err: unknown mode\n");
        return -1;
    }

    return ufs_cfg_callback(hba, mode);
}

int ufs_switch_part(u32 part_id)
{
    int lun;

    switch (part_id) {
    case UFS_LU_BOOT1:
        lun = UFS_LU_0;
        break;
    case UFS_LU_BOOT2:
        lun = UFS_LU_1;
        break;
    case UFS_LU_USER:
        lun = UFS_LU_2;
        break;
    default:
        dprintf(CRITICAL, "[UFS] err: %s, invalid UFS LU %d\n", __func__, part_id);
        return UFS_ERR_INVALID_LU;
    }

    return lun;
}

