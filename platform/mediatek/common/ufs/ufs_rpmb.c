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
#include "ufs_error.h"
#include "ufs_hcd.h"
#include "ufs_interface.h"
#include "ufs_rpmb.h"
#include "ufs_types.h"
#include "ufs_utils.h"

#include <arch/mmu.h>
#include <arch/ops.h>   /* for cache maintanance APIs */
#include <endian.h>
#include <err.h>
#include <errno.h>
#include <kernel/vm.h>
#include <platform_mtk.h>
#include <platform/memory_layout.h>
#include <rng.h>
#include <smc.h>
#include <smc_id_table.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

static u8 rpmb_nonce_buf[RPMB_FRM_NONCE_LEN] __ALIGNED(CACHE_LINE);
static u8 rpmb_frame_buf[RPMB_FRM_DATA_LEN] __ALIGNED(CACHE_LINE);

/* RPMB frame buffer */
static u8 ufs_rpmb_buf[sizeof(struct rpmb_data_frame)];

/* fixed and pre-allocated in lk */
static u8 *shared_msg_addr1;
static u8 *shared_hmac_addr1;


int init_ufs_rpmb_sharemem(void)
{
    void *hmac_va = (void *)LK_HMAC_BASE;
    uint32_t hmac_size;
    paddr_t hmac_pa;
    struct __smccc_res smccc_res;
    int ret = 0;

    /* map shared memory for rpmb */
    hmac_pa = ROUNDDOWN((paddr_t)LK_HMAC_BASE_PHY, PAGE_SIZE);
    hmac_size = ROUNDUP(LK_HMAC_SIZE, PAGE_SIZE);

    /*
     * allocate desired virtual space, and MMU mapping with it.
     * rpmb share memory is used to communicate with seclib, cannot free.
     */
    ret = vmm_alloc_physical(vmm_get_kernel_aspace(),
            "vm_hmac", hmac_size,
            &hmac_va, PAGE_SIZE_SHIFT,
            (paddr_t)LK_HMAC_BASE_PHY,
            VMM_FLAG_VALLOC_SPECIFIC,
            ARCH_MMU_FLAG_CACHED);

    if (ret != NO_ERROR || hmac_va != (void *)LK_HMAC_BASE) {
        panic("err=%d, vm_hmac (0x%p) expected (0x%lx)\n",
            ret, hmac_va, LK_HMAC_BASE_PHY);
    }

    shared_msg_addr1 = (u8 *)(hmac_va);
    shared_hmac_addr1 = (u8 *)(hmac_va + 0xF00);

    if (ufs_is_in_secure_world())
        goto out;

    /* notify tfa to init HMAC  */
    memset(&smccc_res, 0, sizeof(smccc_res));

    __smc_conduit(MTK_SIP_BL_UFS_CONTROL, UFS_MTK_SIP_HMAC,
        RPMB_HMAC_INIT, 0, 0, 0, 0, 0, &smccc_res);

    return smccc_res.a0;

out:
    return ret;
}

int deinit_ufs_rpmb_sharemem(void)
{
    struct __smccc_res smccc_res;

    if (ufs_is_in_secure_world())
        goto out;

    /* notify tfa to init HMAC  */
    memset(&smccc_res, 0, sizeof(smccc_res));

    __smc_conduit(MTK_SIP_BL_UFS_CONTROL, UFS_MTK_SIP_HMAC,
        RPMB_HMAC_DEINIT, 0, 0, 0, 0, 0, &smccc_res);

    return smccc_res.a0;

out:
    return 0;
}

void rpmb_req_copy_data_for_hmac(u8 *buf, struct rpmb_data_frame *f)
{
    u32 size;

    /*
     * Copy below members for HMAC calculation
     * one by one with specifically assigning
     * buf to each member to pass buffer-overrun checker.
     *
     * u8 data[RPMB_FRM_DATA_LEN];
     * u8 nonce[RPMB_FRM_NONCE_LEN];
     * u32 wr_cnt;
     * u16 addr;
     * u16 blk_cnt;
     * u16 result;
     * u16 req_resp;
     */

    memcpy(buf, f->data, RPMB_FRM_DATA_LEN);
    buf += RPMB_FRM_DATA_LEN;

    size = sizeof(f->nonce);
    memcpy(buf, f->nonce, size);
    buf += size;

    size = sizeof(f->wr_cnt);
    memcpy(buf, &f->wr_cnt, size);
    buf += size;

    size = sizeof(f->addr);
    memcpy(buf, &f->addr, size);
    buf += size;

    size = sizeof(f->blk_cnt);
    memcpy(buf, &f->blk_cnt, size);
    buf += size;

    size = sizeof(f->result);
    memcpy(buf, &f->result, size);
    buf += size;

    size = sizeof(f->req_resp);
    memcpy(buf, &f->req_resp, size);
    buf += size;
}

static int ufs_rpmb_get_rnd(u32 *rnd)
{
    int ret = 0;

    ret = get_rnd(rnd);

    return ret;
}

static int ufs_rpmb_pre_nonce(u8 *buf, u32 size)
{
    int ret;
    u32 i, rnd_val = 0;
    u8 *ptr = buf;

    if (size == 0) {
        dprintf(CRITICAL, "[WARNING]%s: nonce size is not match %d(%d)\n",
            __func__, size, RPMB_FRM_NONCE_LEN);
        return -1;
    }

    for (i = 0; i < size / 4; i++) {
        ret = ufs_rpmb_get_rnd(&rnd_val);
        if (ret)
            goto out;

        memcpy(ptr, &rnd_val, 4);
        ptr += 4;
    }

    if (size & 0x3) {
        ret = ufs_rpmb_get_rnd(&rnd_val);
        if (ret)
            goto out;

        memcpy(ptr, &rnd_val, size & 0x3);
    }

out:
    if (ret) {
        dprintf(CRITICAL, "%s:get_rnd is not ready, ret:%d\n",
            __func__, ret);
    }

    return ret;
}

static int rpmb_hmac(uint32_t pattern, uint32_t size)
{
    struct __smccc_res smccc_res;

    if (ufs_is_in_secure_world())
        goto out;

    /* notify tfa to calculate HMAC  */
    memset(&smccc_res, 0, sizeof(smccc_res));

    __smc_conduit(MTK_SIP_BL_UFS_CONTROL, UFS_MTK_SIP_HMAC,
        RPMB_HMAC_CALC, 0, 0, 0, 0, 0, &smccc_res);

    return smccc_res.a0;

out:
    return 0;
}

static int rpmb_get_mac(struct rpmb_data_frame *pfrm)
{
    int ret = 0;

    if (pfrm == NULL) {
        dprintf(CRITICAL, "[%s]: RPMB pfrm pointer NULL\n", __func__);
        return -EIO;
    }

    rpmb_req_copy_data_for_hmac(shared_msg_addr1, pfrm);

    arch_clean_invalidate_cache_range((addr_t) shared_msg_addr1, 4096);

    ret = rpmb_hmac(0, 284);
    if (ret) {
        dprintf(CRITICAL, "[%s]: RPMB rpmb_hmac error, ret = %d\n",
            __func__, ret);
    }

    return ret;
}

static void cmd_scsi_security_protocol_out(struct ufs_scsi_cmd *cmd, u32 tag,
    u32 blk_cnt, u8 protocol, u32 protocol_specific)
{
    if (cmd == NULL) {
        dprintf(CRITICAL, "[%s]: RPMB cmd pointer NULL\n", __func__);
        return;
    }

    memset(cmd->cmd_data, 0, MAX_CDB_SIZE);

    cmd->lun = WLUN_RPMB;
    cmd->tag = tag;
    cmd->dir = DMA_TO_DEVICE;
    cmd->exp_len = blk_cnt  << 9;

    cmd->attr = ATTR_SIMPLE;

    cmd->cmd_len = 12;
    cmd->cmd_data[0] = SECURITY_PROTOCOL_OUT; /* opcode */
    cmd->cmd_data[1] = protocol;
    cmd->cmd_data[2] = (protocol_specific >> 8) & 0xFF;
    cmd->cmd_data[3] = protocol_specific & 0xFF;
    cmd->cmd_data[4] = 0x0;
    cmd->cmd_data[5] = 0x0;
    cmd->cmd_data[6] = ((blk_cnt  << 9) >> 24) & 0xFF;
    cmd->cmd_data[7] = ((blk_cnt  << 9) >> 16) & 0xFF;
    cmd->cmd_data[8] = ((blk_cnt  << 9) >> 8) & 0xFF;
    cmd->cmd_data[9] = 0x0;
    cmd->cmd_data[10] = 0x0;
    cmd->cmd_data[11] = 0x0;
}

static void ufshcd_authen_result_read_req_prepare(struct ufs_scsi_cmd *cmd)
{
    struct rpmb_data_frame *pfrm = NULL;

    if (cmd == NULL) {
        dprintf(CRITICAL, "[%s]: RPMB cmd pointer NULL\n", __func__);
        return;
    }

    pfrm = cmd->data_buf;

    if (pfrm == NULL) {
        dprintf(CRITICAL, "[%s]: RPMB pfrm pointer NULL\n", __func__);
        return;
    }

    memset(pfrm, 0, sizeof(struct rpmb_data_frame));

    pfrm->req_resp = BE16(RPMB_REQ_RESULT_RD);
}

static void cmd_scsi_security_protocol_in(struct ufs_scsi_cmd *cmd, u32 tag,
    u32 blk_cnt, u8 protocol, u32 protocol_specific)
{
    if (cmd == NULL) {
        dprintf(CRITICAL, "[%s]: RPMB cmd pointer NULL\n", __func__);
        return;
    }

    memset(cmd->cmd_data, 0, MAX_CDB_SIZE);

    cmd->lun = WLUN_RPMB;
    cmd->tag = tag;
    cmd->dir = DMA_FROM_DEVICE;
    cmd->exp_len = blk_cnt * 512;

    cmd->attr = ATTR_SIMPLE;

    cmd->cmd_len = 12;
    cmd->cmd_data[0] = SECURITY_PROTOCOL_IN;
    cmd->cmd_data[1] = protocol;
    cmd->cmd_data[2] = (protocol_specific >> 8) & 0xFF;
    cmd->cmd_data[3] = protocol_specific & 0xFF;
    cmd->cmd_data[4] = 0x0;
    cmd->cmd_data[5] = 0x0;
    cmd->cmd_data[6] = ((blk_cnt  << 9) >> 24) & 0xFF;
    cmd->cmd_data[7] = ((blk_cnt  << 9) >> 16) & 0xFF;
    cmd->cmd_data[8] = ((blk_cnt  << 9) >> 8) & 0xFF;
    cmd->cmd_data[9] = 0x0;
    cmd->cmd_data[10] = 0x0;
    cmd->cmd_data[11] = 0x0;
}

#define RPMB_MAC_DUMP_DEBUG 0

#if RPMB_MAC_DUMP_DEBUG
static inline void ufshcd_rpmb_mac_dump(const u8 *mac, u32 mac_len)
{
    u32 i = 0;

    dprintf(CRITICAL, "mac dump:\n");
    if (mac == NULL) {
        dprintf(CRITICAL, "[%s]: RPMB mac pointer NULL\n", __func__);
        return;
    }

    for (i = 0; i < mac_len; i++)
        dprintf(CRITICAL, "[%d]:0x%x\n", i, *(mac+i));

    dprintf(CRITICAL, "\n");
}
#else
static inline void ufshcd_rpmb_mac_dump(const u8 *mac, u32 mac_len)
{
}
#endif

static int ufshcd_authen_wcnt_read_req_prepare(struct ufs_scsi_cmd *cmd,
    u8 *nonce, unsigned int nonce_len)
{
    struct rpmb_data_frame *pfrm = NULL;
    int ret = 0;

    if (cmd == NULL) {
        dprintf(CRITICAL, "[%s]: RPMB cmd pointer NULL\n", __func__);
        return -UFS_RPMB_DRIVER_ERROR;
    }
    if (nonce == NULL) {
        dprintf(CRITICAL, "[%s]: RPMB nonce pointer NULL\n", __func__);
        return -UFS_RPMB_DRIVER_ERROR;
    }

    pfrm = cmd->data_buf;

    if (pfrm == NULL) {
        dprintf(CRITICAL, "[%s]: RPMB pfrm pointer NULL\n", __func__);
        return -UFS_RPMB_DRIVER_ERROR;
    }

    if (nonce_len > RPMB_FRM_NONCE_LEN) {
        dprintf(CRITICAL,
            "[%s]: RPMB Fail Nonce data length larger than max allow len(%d)",
            __func__, RPMB_FRM_NONCE_LEN);
        return -UFS_RPMB_DRIVER_ERROR;
    }
    memset(pfrm, 0, sizeof(struct rpmb_data_frame));

    memcpy(pfrm->nonce, nonce, nonce_len);

    pfrm->req_resp = BE16(RPMB_REQ_READ_WR_CNT);

    return ret;
}

static int ufshcd_authensss_wcnt_read_rsp_check(u32 host_id,
    struct ufs_scsi_cmd *cmd, u32 *wcnt, u8 *nonce, unsigned int nonce_len,
    enum enum_rpmb_msg type)
{
    struct rpmb_data_frame *rpmb_frame = NULL;
    unsigned int idx = 0;
    int ret = 0;

    if (cmd == NULL) {
        dprintf(CRITICAL, "[%s]: RPMB cmd pointer NULL\n", __func__);
        return -UFS_RPMB_DRIVER_ERROR;
    }
    if (wcnt == NULL) {
        dprintf(CRITICAL, "[%s]: RPMB wcnt pointer NULL\n", __func__);
        return -UFS_RPMB_DRIVER_ERROR;
    }
    if (nonce == NULL) {
        dprintf(CRITICAL, "[%s]: RPMB nonce pointer NULL\n", __func__);
        return -UFS_RPMB_DRIVER_ERROR;
    }

    rpmb_frame = cmd->data_buf;

    if (rpmb_frame == NULL) {
        dprintf(CRITICAL, "[%s]: RPMB rpmb_frame pointer NULL\n",
            __func__);
        return -UFS_RPMB_DRIVER_ERROR;
    }

    if (rpmb_frame->req_resp != BE16(type)) {
        /* Check response type */
        dprintf(CRITICAL, "[%s]: RPMB Fail in checking response type: 0x%x\r\n",
            __func__, BE16(rpmb_frame->req_resp));
        return -UFS_RPMB_RESPONSE_ERROR;
    } else if (BE16(rpmb_frame->result) & 0x7F) {
        idx = BE16(rpmb_frame->result) & 0x7F;
        /* Check response type */
        dprintf(CRITICAL, "[%s]: RPMB Fail in checking result: %s \r\n",
            __func__, rpmb_oper_result[idx]);
        return -UFS_RPMB_RESULT_ERROR;
    } else if (BE16(rpmb_frame->result) & 0x80) {
        /* Check Write Counter */
        dprintf(INFO, "[%s]: RPMB write counter is expired\r\n",
            __func__);
    }

    if (memcmp(rpmb_frame->nonce, nonce, nonce_len)) {
        /* For other response type noce will be "0" */
        dprintf(CRITICAL, "[%s]: RPMB Fail in checking nonce\r\n",
            __func__);
        return -UFS_RPMB_NONCE_ERROR;
    }

    /* Then check MAC */
    ret = rpmb_get_mac(rpmb_frame);
    if (ret) {
        dprintf(CRITICAL, "[%s]: rpmb_get_mac fail, ret=%d\r\n", __func__, ret);
        return -UFS_RPMB_DRIVER_ERROR;
    }

    if (memcmp(rpmb_frame->key_MAC, shared_hmac_addr1, RPMB_FRM_KEY_MAC_LEN)) {
        dprintf(CRITICAL, "[%s]: RPMB Fail in Mac Check!\n", __func__);
        dprintf(CRITICAL, "Device Return ");
        ufshcd_rpmb_mac_dump((const u8 *)rpmb_frame->key_MAC, RPMB_FRM_KEY_MAC_LEN);
        dprintf(CRITICAL, "Host Recalc");
        ufshcd_rpmb_mac_dump((const u8 *)shared_hmac_addr1, RPMB_FRM_KEY_MAC_LEN);
        return -UFS_RPMB_HMAC_ERROR;
    }

    /* Then Return WCNT */
    *wcnt = BE32(rpmb_frame->wr_cnt);

    return ret;
}

int rpmb_authen_read_counter(u32 host_id, u8 *nonce_buf, u32 nonce_len,
    u32 *wr_cnt)
{
    struct ufs_scsi_cmd cmd;
    struct ufs_hba *hba;
    int ret = 0;
    int tag;

    if (nonce_buf == NULL) {
        dprintf(CRITICAL, "[%s]: RPMB nonce_buf pointer NULL\n",
            __func__);
        return -EIO;
    }
    if (wr_cnt == NULL) {
        dprintf(CRITICAL, "[%s]: RPMB wr_cnt pointer NULL\n", __func__);
        return -EIO;
    }

    cmd.data_buf = ufs_rpmb_buf;

    hba = ufs_get_host(host_id);

    if (!ufshcd_get_free_tag(hba, &tag))
        return -EIO;

    /* Step1 write counter read request */
    cmd_scsi_security_protocol_out(&cmd, tag, 1,
                                   UFS_SECURITY_PROTOCOL,
                                   RPMB_PROTOCOL_ID);
    ufshcd_authen_wcnt_read_req_prepare(&cmd,
                                        nonce_buf, nonce_len);

    ret = ufshcd_queuecommand(hba, &cmd);
    if (ret) {
        dprintf(CRITICAL, "[%s]: Sending Write counter read request fail, ret=%d\n",
            __func__, ret);
        goto exit;
    }

    /* Step2. Resp for write counter read Request */
    cmd_scsi_security_protocol_in(&cmd, tag, 1, UFS_SECURITY_PROTOCOL,
        RPMB_PROTOCOL_ID);
    ret = ufshcd_queuecommand(hba, &cmd);
    if (ret) {
        dprintf(CRITICAL, "[%s]: Get result response fail, ret=%d\n",
            __func__, ret);
        goto exit;
    }

    /* Step3. Response frame check */
    ret = ufshcd_authensss_wcnt_read_rsp_check(host_id, &cmd, wr_cnt, nonce_buf,
        nonce_len, RPMB_RSP_READ_WR_CNT);
    if (ret) {
        dprintf(CRITICAL, "[%s]: RPMB WriteCounter Read Resp Check Fail, ret%d\n",
            __func__, ret);
    }

exit:
    ufshcd_put_tag(hba, tag);
    return ret;
}

static void dump_swpcb(struct secure_wp_config_block *swpcb)
{
    int i;

    dprintf(CRITICAL, "swpcb lun %d\n", swpcb->lun);
    dprintf(CRITICAL, "swpcb data_length %d\n", swpcb->data_length);

    for (i = 0; i < (swpcb->data_length/WP_ENTRY_SIZE); i++) {
        dprintf(CRITICAL, "swpcb entry[%d] wpt_wpf 0x%x\n", i, swpcb->wp_entry[i].wpt_wpf);
        dprintf(CRITICAL, "swpcb entry[%d] upper_addr 0x%x\n", i, swpcb->wp_entry[i].upper_addr);
        dprintf(CRITICAL, "swpcb entry[%d] lower_addr 0x%x\n", i, swpcb->wp_entry[i].lower_addr);
        dprintf(CRITICAL, "swpcb entry[%d] block_num 0x%x\n", i, swpcb->wp_entry[i].block_num);
    }
}

static int ufshcd_swp_conf_block_write_prepare(u32 host_id,
    struct ufs_scsi_cmd *cmd, u16 blk_num, u32 wr_cnt, u8 type,
    unsigned long blknr, u32 blkcnt, u8 partition, u8 wp_entry,
    const u8 *data_buf)
{
    struct rpmb_data_frame *pfrm = NULL;
    int err = 0;
    struct secure_wp_config_block *swpcb = (void *)data_buf;

    if (cmd == NULL) {
        dprintf(CRITICAL, "[%s]: RPMB cmd pointer NULL\n", __func__);
        return -EIO;
    }
    if (data_buf == NULL) {
        dprintf(CRITICAL, "[%s]: RPMB data_buf pointer NULL\n", __func__);
        return -EIO;
    }

    pfrm = cmd->data_buf;

    if (pfrm == NULL) {
        dprintf(CRITICAL, "[%s]: RPMB pfrm pointer NULL\n", __func__);
        return -EIO;
    }

    memset(pfrm, 0, sizeof(struct rpmb_data_frame) * blk_num);

    pfrm->req_resp = BE16(RPMB_REQ_SEC_WPCB_WRITE);
    pfrm->blk_cnt = BE16(blk_num);
    pfrm->addr = 0;
    pfrm->wr_cnt = BE32(wr_cnt);
    /*fix nonce data to 0*/

    /* LUN */
    swpcb->lun = partition - 1;

    /* Data length */
    if (partition ==  UFS_LU_BOOT1 || partition ==  UFS_LU_BOOT2) {
         /* Boot partition only needs one secure write protect block entry */
        swpcb->data_length = WP_ENTRY_SIZE;
    } else {
         /* User partition always add new wp_entry */
        swpcb->data_length = WP_ENTRY_SIZE * (wp_entry + 1);
    }

    /* WP entries configuration */
    /* Protect entire BOOT1 & BOOT2 area */
    if (partition ==  UFS_LU_BOOT1 || partition ==  UFS_LU_BOOT2) {
        if (type == WP_DISABLE)
            swpcb->wp_entry[0].wpt_wpf = WP_WPF_DISABLE;
        else
            swpcb->wp_entry[0].wpt_wpf = WP_WPF_EN | (type << WPT_BIT);
        /* Number of Logical Blocks      : 0x0 protect all blocks */
        swpcb->wp_entry[0].block_num = 0;
        swpcb->wp_entry[0].upper_addr = 0;
        swpcb->wp_entry[0].lower_addr = 0;
    } else {
        /* only update latest wp_entry */
        if (type == WP_DISABLE)
            swpcb->wp_entry[wp_entry].wpt_wpf = WP_WPF_DISABLE;
        else
            swpcb->wp_entry[wp_entry].wpt_wpf = WP_WPF_EN | (type << WPT_BIT);

        /* Logical Block Address */
        swpcb->wp_entry[wp_entry].lower_addr = ((blknr >> 24) & 0xFF) |
                                                (((blknr >> 16) & 0xFF) << 8) |
                                                (((blknr >> 8) & 0xFF) << 16) |
                                                ((blknr & 0xFF) << 24);

        /* Number of Logical Blocks */
        swpcb->wp_entry[wp_entry].block_num = ((blkcnt >> 24) & 0xFF) |
                                               (((blkcnt >> 16) & 0xFF) << 8) |
                                               (((blkcnt >> 8) & 0xFF) << 16) |
                                               ((blkcnt & 0xFF) << 24);
    }

    dprintf(CRITICAL, "[%s]: Write swpcb\n", __func__);
    dump_swpcb(swpcb);

    /* Copy entire "data_buf" to "pfrm->data" before calculate HMAC */
    memcpy(pfrm->data, data_buf, WP_CONF_BLOCK_SIZE);

    /* Calculate MAC */
    err = rpmb_get_mac(pfrm);
    if (err) {
        dprintf(CRITICAL, "[%s]: rpmb_get_mac fail, ret=%d\r\n", __func__, err);
        goto exit;
    }
    memcpy(pfrm->key_MAC, shared_hmac_addr1, RPMB_FRM_KEY_MAC_LEN);

exit:
    return err;
}

static int ufshcd_swp_conf_block_read_prepare(u32 host_id,
    struct ufs_scsi_cmd *cmd, u16 blk_num, u8 *nonce,
    unsigned int nonce_len, u8 partition)
{
    struct rpmb_data_frame *pfrm = NULL;

    if (cmd == NULL) {
        dprintf(CRITICAL, "[%s]: RPMB cmd pointer NULL\n", __func__);
        return -EIO;
    }
    if (nonce == NULL) {
        dprintf(CRITICAL, "[%s]: RPMB nonce pointer NULL\n", __func__);
        return -EIO;
    }

    pfrm = cmd->data_buf;

    if (pfrm == NULL) {
        dprintf(CRITICAL, "[%s]: RPMB pfrm pointer NULL\n", __func__);
        return -EIO;
    }

    memset(pfrm, 0, sizeof(struct rpmb_data_frame));


    pfrm->req_resp = BE16(RPMB_REQ_SEC_WPCB_READ);
    pfrm->blk_cnt = BE16(blk_num);
    pfrm->addr = 0;

    memcpy(pfrm->nonce, nonce, nonce_len);

    pfrm->data[0] = partition - 1;

    return 0;
}

static int ufshcd_authen_data_write_rsp_check(u32 host_id,
    struct ufs_scsi_cmd *cmd, u32 wcnt, enum enum_rpmb_msg type)
{
    struct rpmb_data_frame *rpmb_frame = NULL;
    unsigned int idx = 0;
    int ret = 0;

    if (cmd == NULL) {
        dprintf(CRITICAL, "[%s]: RPMB cmd pointer NULL\n", __func__);
        return -UFS_RPMB_DRIVER_ERROR;
    }

    rpmb_frame = cmd->data_buf;

    if (rpmb_frame == NULL) {
        dprintf(CRITICAL, "[%s]: RPMB rpmb_frame pointer NULL\n", __func__);
        return -UFS_RPMB_DRIVER_ERROR;
    }

    if (rpmb_frame->req_resp != BE16(type)) {
        /* Check response type */
        dprintf(CRITICAL, "[%s]: RPMB Fail in checking response type : 0x%x\r\n",
            __func__, BE16(rpmb_frame->req_resp));
        return -UFS_RPMB_DRIVER_ERROR;
    } else if (BE16(rpmb_frame->result) & 0x7F) {
        idx = BE16(rpmb_frame->result) & 0x7F;
        /* Check response type */
        dprintf(CRITICAL, "[%s]: RPMB Fail in checking result : %s \r\n",
            __func__, rpmb_oper_result[idx]);
        return -UFS_RPMB_RESULT_ERROR;
    } else if (BE16(rpmb_frame->result) & 0x80) {
        /*Check Write Counter*/
        dprintf(CRITICAL, "[%s]: RPMB write counter is expired\r\n", __func__);
    }

    /* Then check MAC */
    ret = rpmb_get_mac(rpmb_frame);
    if (ret) {
        dprintf(CRITICAL, "[%s]: rpmb_get_mac fail, ret=%d\r\n", __func__, ret);
        return -UFS_RPMB_DRIVER_ERROR;
    }

    if (memcmp(rpmb_frame->key_MAC, shared_hmac_addr1, RPMB_FRM_KEY_MAC_LEN)) {
        dprintf(CRITICAL, "[%s]: RPMB Fail in Mac Check!\n", __func__);
        dprintf(CRITICAL, "Device Return ");
        ufshcd_rpmb_mac_dump((const u8 *)rpmb_frame->key_MAC, RPMB_FRM_KEY_MAC_LEN);
        dprintf(CRITICAL, "Host Recalc");
        ufshcd_rpmb_mac_dump((const u8 *)shared_hmac_addr1, RPMB_FRM_KEY_MAC_LEN);
        return -UFS_RPMB_HMAC_ERROR;
    }

    /* Check Wr_cnt increasement */
    if (rpmb_frame->wr_cnt != BE32(wcnt + 1)) {
        dprintf(CRITICAL, "[%s]: RPMB Fail in Write Counter Increasement!\n",
            __func__);
        return -UFS_RPMB_WRCNT_ERROR;
    }

    return ret;
}

static int ufshcd_authen_data_read_rsp_check(u32 host_id,
    struct ufs_scsi_cmd *cmd, const u8 *nonce_buf, unsigned int  nonce_len,
    u8 *data_buf, enum enum_rpmb_msg type)
{
    struct rpmb_data_frame *pfrm = NULL;
    unsigned int idx = 0;
    int ret = 0;

    if (cmd == NULL) {
        dprintf(CRITICAL, "[%s]: RPMB cmd pointer NULL\n", __func__);
        return -UFS_RPMB_DRIVER_ERROR;
    }
    if (nonce_buf == NULL) {
        dprintf(CRITICAL, "[%s]: RPMB nonce_buf pointer NULL\n", __func__);
        return -UFS_RPMB_DRIVER_ERROR;
    }
    if (data_buf == NULL) {
        dprintf(CRITICAL, "[%s]: RPMB ata_buf pointer NULL\n", __func__);
        return -UFS_RPMB_DRIVER_ERROR;
    }

    pfrm = cmd->data_buf;

    if (pfrm == NULL) {
        dprintf(CRITICAL, "[%s]: RPMB pfrm pointer NULL\n", __func__);
        return -UFS_RPMB_DRIVER_ERROR;
    }

    if (pfrm->req_resp != BE16(type)) {
        /* Check response type */
        dprintf(CRITICAL, "[%s]: RPMB Fail in checking response type : 0x%x\r\n",
            __func__, BE16(pfrm->req_resp));
        return -UFS_RPMB_RESPONSE_ERROR;
    } else if (BE16(pfrm->result) & 0x7F) {
        idx = BE16(pfrm->result) & 0x7F;
        /* Check response type */
        dprintf(CRITICAL, "[%s]: RPMB Fail in checking result : %s \r\n",
            __func__, rpmb_oper_result[idx]);
        return -UFS_RPMB_RESULT_ERROR;
    } else if (BE16(pfrm->result) & 0x80) {
        /* Check Write Counter */
        dprintf(CRITICAL, "[%s]: RPMB write counter is expired\r\n", __func__);
    }

    if (memcmp(pfrm->nonce, nonce_buf, nonce_len)) {
        /* For other response type noce will be "0" */
        dprintf(CRITICAL, "[%s]: RPMB Fail in checking nonce\r\n", __func__);
        return -UFS_RPMB_NONCE_ERROR;
    }

    ret = rpmb_get_mac(pfrm);
    if (ret) {
        dprintf(CRITICAL, "[%s]: rpmb_get_mac fail, ret=%d\r\n", __func__, ret);
        return -UFS_RPMB_DRIVER_ERROR;
    }

    if (memcmp(pfrm->key_MAC, shared_hmac_addr1, RPMB_FRM_KEY_MAC_LEN)) {
        /* For other response type noce will be "0" */
        dprintf(CRITICAL, "[%s]: RPMB Fail in checking MAC\r\n", __func__);
        dprintf(CRITICAL, "Device Return");
        ufshcd_rpmb_mac_dump((const u8 *)pfrm->key_MAC, RPMB_FRM_KEY_MAC_LEN);
        dprintf(CRITICAL, "Host Recalc");
        ufshcd_rpmb_mac_dump((const u8 *)shared_hmac_addr1, RPMB_FRM_KEY_MAC_LEN);
        return -UFS_RPMB_HMAC_ERROR;
    }

    /* Copy Secure Write Protect Block to data_buf */
    memcpy(data_buf, pfrm->data, WP_CONF_BLOCK_SIZE);

    return 0;
}

static int rpmb_authen_secure_write_protect_conf_block_write(u32 host_id,
    u32 wr_cnt, const u8 *dat_buf, u32 blk_num, u8 type, unsigned long blknr,
    u32 blkcnt, u8 partition, u8 wp_entry)
{
    struct ufs_scsi_cmd cmd;
    struct ufs_hba *hba;
    int ret = 0;
    int tag;

    if (dat_buf == NULL) {
        dprintf(CRITICAL, "[%s]: RPMB data_buf pointer NULL\n",
            __func__);
        return -EIO;
    }

    cmd.data_buf = ufs_rpmb_buf;

    hba = ufs_get_host(host_id);

    if (!ufshcd_get_free_tag(hba, &tag))
        return -EIO;

    /* Step1 write counter read request */
    cmd_scsi_security_protocol_out(&cmd, tag, blk_num,
                                   UFS_SECURITY_PROTOCOL,
                                   RPMB_PROTOCOL_ID);

    ufshcd_swp_conf_block_write_prepare(host_id, &cmd, blk_num, wr_cnt, type,
        blknr, blkcnt, partition, wp_entry, dat_buf);

    ufshcd_rpmb_mac_dump(dat_buf, RPMB_SWPCB_VALID_DATA_SIZE);

    ret = ufshcd_queuecommand(hba, &cmd);
    if (ret) {
        dprintf(CRITICAL, "[%s]: Sending Secure Write Protect Conf. fail ret=%d\n",
            __func__, ret);
        goto exit;
    }

    /* Step2 Result Register Read Request */
    cmd_scsi_security_protocol_out(&cmd, tag, 1,
                                   UFS_SECURITY_PROTOCOL,
                                   RPMB_PROTOCOL_ID);
    ufshcd_authen_result_read_req_prepare(&cmd);

    ret = ufshcd_queuecommand(hba, &cmd);
    if (ret) {
        dprintf(CRITICAL, "[%s]: Sending result register read request fail, ret=%d\n",
            __func__, ret);
        goto exit;
    }

    /* Step3 Check Response for Result Read Request */
    cmd_scsi_security_protocol_in(&cmd, tag, 1, UFS_SECURITY_PROTOCOL,
        RPMB_PROTOCOL_ID);
    ret = ufshcd_queuecommand(hba, &cmd);
    if (ret) {
        dprintf(CRITICAL, "[%s]: Sending request get result register response fail, ret=%d\n",
            __func__, ret);
        goto exit;
    }

    /* Step4 Response frame check */
    ret = ufshcd_authen_data_write_rsp_check(host_id, &cmd, wr_cnt,
               RPMB_RSP_SEC_WPCB_WRITE);
    if (ret) {
        dprintf(CRITICAL, "[%s]: RPMB WriteCounter Read Resp Check Fail, ret=%d\n",
            __func__, ret);
        goto exit;
    }

exit:
    ufshcd_put_tag(hba, tag);
    return ret;
}

static int rpmb_authen_secure_write_protect_conf_block_read(u32 host_id,
    u8 *data_buf, u8 *nonce_buf, u32 nonce_len, u32 blk_num, u8 partition)
{
    struct ufs_scsi_cmd cmd;
    struct ufs_hba *hba;
    int ret = 0;
    int tag;

    if (data_buf == NULL) {
        dprintf(CRITICAL, "[%s]: RPMB data_buf pointer NULL\n", __func__);
        return -EIO;
    }
    if (nonce_buf == NULL) {
        dprintf(CRITICAL, "[%s]: RPMB nonce_buf pointer NULL\n", __func__);
        return -EIO;
    }

    cmd.data_buf = ufs_rpmb_buf;

    hba = ufs_get_host(host_id);

    if (!ufshcd_get_free_tag(hba, &tag))
        return -EIO;

    /* Step1 authen data read request */
    cmd_scsi_security_protocol_out(&cmd, tag, 1, UFS_SECURITY_PROTOCOL,
        RPMB_PROTOCOL_ID);
    ufshcd_swp_conf_block_read_prepare(host_id, &cmd, blk_num, nonce_buf,
        nonce_len, partition);
    ret = ufshcd_queuecommand(hba, &cmd);
    if (ret) {
        dprintf(CRITICAL, "[%s]: Sending Secure Write Protect Conf. fail, ret=%d\n",
            __func__, ret);
        goto exit;
    }

    /* Step2 Send protocol in cmd for data read*/
    cmd_scsi_security_protocol_in(&cmd, tag, blk_num, UFS_SECURITY_PROTOCOL,
        RPMB_PROTOCOL_ID);
    ret = ufshcd_queuecommand(hba, &cmd);
    if (ret) {
        dprintf(CRITICAL, "[%s]: Sending request get result register response fail, ret=%d\n",
            __func__, ret);
        goto exit;
    }

    /* Step3 Check Response and parser data*/
    ret = ufshcd_authen_data_read_rsp_check(host_id, &cmd, nonce_buf, nonce_len,
        data_buf, RPMB_RSP_SEC_WPCB_READ);
    if (ret) {
        dprintf(CRITICAL, "[%s]: RPMB Data Read Resp Check Fail, ret=%d\n",
            __func__, ret);
        goto exit;
    }

exit:
    ufshcd_put_tag(hba, tag);
    return ret;
}

static int rpmb_authen_secure_write_protect_conf_block_check(u8 *data_buf,
    u8 *wp_entry, unsigned long blknr, u32 blkcnt, u8 partition, u8 type)
{
    static int round;

    struct secure_wp_config_block *swpcb = (void *)data_buf;

    if (data_buf == NULL) {
        dprintf(CRITICAL, "[%s]: RPMB data_buf pointer NULL\n", __func__);
        return -EIO;
    }
    if (wp_entry == NULL) {
        dprintf(CRITICAL, "[%s]: RPMB wp_entry pointer NULL\n", __func__);
        return -EIO;
    }

    dprintf(CRITICAL, "[%s]: Read swpcb\n", __func__);
    dump_swpcb(swpcb);

    /* For Boot1/Boot2, only lock all region. And only have one wp_entry */
    if (partition == UFS_LU_BOOT1 || partition == UFS_LU_BOOT2 ||
            type == WP_DISABLE) {
        memset(&(swpcb->wp_entry[0]), 0x00, WP_ENTRY_SIZE * SWPWP_ENTRY_NUM);
        *wp_entry = 0;
        return 0;
    }

    /*
     * For User partition, clear wp_entry in first round, and wp_entry set 0
     * In second run later, set to last read entry end.
     */
    if (round == 0) {
        memset(&(swpcb->wp_entry[0]), 0, WP_ENTRY_SIZE * SWPWP_ENTRY_NUM);
        *wp_entry = 0;
    } else {
        if (swpcb->data_length == WP_MAX_ENTRY_SIZE) {
            dprintf(CRITICAL, "[%s]: RPMB exceed WP entries size!!!\n",
                __func__);
            return -EIO;
        }
        *wp_entry = swpcb->data_length / WP_ENTRY_SIZE;
    }

    round++;

    return 0;
}

static int rpmb_ufs_set_wp(u32 host_id, u8 type, unsigned long blknr, u32 blkcnt,
    u8 partition)
{
    u8 *nonce_buf = NULL;
    u8 *data_buf = NULL;
    u32 wr_cnt = 0;
    int ret = 0;
    u8 wp_entry = 0;

    nonce_buf = rpmb_nonce_buf;
    ufs_rpmb_pre_nonce(nonce_buf, RPMB_FRM_NONCE_LEN);

    data_buf = rpmb_frame_buf;
    memset(data_buf, 0, RPMB_FRM_DATA_LEN);

    /* Read Old Secure Write Protect Conf Block */
    ret = rpmb_authen_read_counter(host_id, nonce_buf, RPMB_FRM_NONCE_LEN,
        &wr_cnt);
    if (ret) {
        dprintf(CRITICAL, "[%s]: RPMB rpmb_authen_read_counter fail! ret=%d\n",
                __func__, ret);
        goto exit;
    }

    ret = rpmb_authen_secure_write_protect_conf_block_read(host_id, data_buf,
        nonce_buf, RPMB_FRM_NONCE_LEN, 1, partition);
    if (ret) {
        dprintf(CRITICAL, "[%s]: RPMB Read Old SWPCB fail! ret=%d\n",
            __func__, ret);
        goto exit;
    } else {
        dprintf(INFO, "Authenticated Data Read Success!\n");
    }

    ret = rpmb_authen_secure_write_protect_conf_block_check(data_buf, &wp_entry,
        blknr, blkcnt, partition, type);
    if (ret) {
        dprintf(CRITICAL, "[%s]: RPMB Old SPWCB check fail! ret=%d\n",
            __func__, ret);
        goto exit;
    }

    /* Get Current Write Counter */
    ret = rpmb_authen_read_counter(host_id, nonce_buf, RPMB_FRM_NONCE_LEN,
                                   &wr_cnt);
    if (ret) {
        dprintf(CRITICAL, "[%s]: RPMB Get Write Counter fail! ret=%d\n",
            __func__, ret);
        goto exit;
    }

    /* Authenticated Data Write */
    ret = rpmb_authen_secure_write_protect_conf_block_write(host_id, wr_cnt,
            data_buf, 1, type, blknr, blkcnt, partition, wp_entry);

    if (ret) {
        dprintf(CRITICAL, "[%s]: RPMB SWPCB Write Fail! ret=%d\n",
            __func__, ret);
        goto exit;
    } else
        dprintf(INFO, "Authenticated Data Write Success 1 blks\n");

exit:

    return ret;
}

int rpmb_ufs_clr_wp(u32 host_id, u8 partition)
{
    u8 *nonce_buf = NULL;
    u8 *data_buf = NULL;
    u32 wr_cnt = 0;
    int ret = 0;
    u8 wp_entry = 0;
    struct secure_wp_config_block *swpcb;

    nonce_buf = rpmb_nonce_buf;
    ufs_rpmb_pre_nonce(nonce_buf, RPMB_FRM_NONCE_LEN);

    data_buf = rpmb_frame_buf;
    memset(data_buf, 0, RPMB_FRM_DATA_LEN);

    swpcb = (void *)data_buf;
    swpcb->data_length = 16;
    wp_entry = 0;

    /* Get Current Write Counter */
    ret = rpmb_authen_read_counter(host_id, nonce_buf, RPMB_FRM_NONCE_LEN,
                                   &wr_cnt);
    if (ret) {
        dprintf(CRITICAL, "[%s]: RPMB Get Write Counter fail! ret=%d\n",
            __func__, ret);
        goto exit;
    }

    /* Authenticated Data Write */
    ret = rpmb_authen_secure_write_protect_conf_block_write(host_id, wr_cnt,
            data_buf, 1, WP_DISABLE, 0, 0, partition, wp_entry);

    if (ret) {
        dprintf(CRITICAL, "[%s]: RPMB SWPCB Write Fail! ret=%d\n",
            __func__, ret);
        goto exit;
    } else {
        dprintf(INFO, "Authenticated Data Write Success 1 blks\n");
    }

exit:
    return ret;
}

int ufs_set_write_protect(int dev_num, u8 partition, unsigned long blknr,
    u32 blkcnt, u8 type)
{
    int err = 0;

    dprintf(INFO, "[%s]: partition=%d, blknr=0x%lx, blkcnt=0x%x, wp_type=%d\n",
        __func__, partition, blknr, blkcnt, type);


    if (type == WP_POWER_ON || type == WP_PERMANENT || type == WP_PERMANENT_AWP) {
        dprintf(INFO, "[%s]: blknr=0x%lx, blkcnt=0x%x\n", __func__, blknr,
            blkcnt);
        err = rpmb_ufs_set_wp(dev_num, type, blknr, blkcnt, partition);
        if (err) {
            dprintf(CRITICAL, "[%s]: RPMB Set WP fail, partition(%d) err=%d\n",
                __func__, partition, err);
        }
    } else if (type == WP_TEMPORARY) {
        dprintf(CRITICAL, "[%s]: RPMB WP Type Not Support\n", __func__);
    } else if (type == WP_DISABLE) {
        err = rpmb_ufs_clr_wp(dev_num, partition);
        if (err) {
            dprintf(CRITICAL, "[%s]: RPMB rpmb_ufs_clear_wp err%d\n",
                __func__, err);
        }
    } else {
        err = -EIO;
    }

    return err;
}

int ufs_clr_write_protect(void)
{
    int err = 0;

    err = ufs_set_write_protect(0, UFS_LU_BOOT1, 0, 0, WP_DISABLE);
    if (err) {
        dprintf(CRITICAL, "[%s]: Clear UFS_LU_BOOT1 write protect fail\n", __func__);
        goto out;
    }

    err = ufs_set_write_protect(0, UFS_LU_BOOT2, 0, 0, WP_DISABLE);
    if (err) {
        dprintf(CRITICAL, "[%s]: Clear UFS_LU_BOOT2 write protect fail\n", __func__);
        goto out;
    }

    err = ufs_set_write_protect(0, UFS_LU_USER, 0, 0, WP_DISABLE);
    if (err) {
        dprintf(CRITICAL, "[%s]: Clear UFS_LU_USER write protect fail\n", __func__);
        goto out;
    }

out:
    return err;
}

static int ufshcd_rpmb_read_data_req_prepare(struct ufs_scsi_cmd *cmd,
        u32 size, u16 addr, u8 *nonce)
{
    struct rpmb_data_frame *pfrm = NULL;

    if (cmd == NULL) {
        dprintf(CRITICAL, "[%s]: RPMB cmd pointer NULL\n", __func__);
        return -EIO;
    }

    pfrm = cmd->data_buf;

    if (pfrm == NULL) {
        dprintf(CRITICAL, "[%s]: RPMB pfrm pointer NULL\n", __func__);
        return -EIO;
    }

    memset(pfrm, 0, sizeof(struct rpmb_data_frame));

    /* Prepare write frame */
    pfrm->addr = BE16(addr);
    pfrm->blk_cnt = BE16(1);
    pfrm->req_resp = BE16(RPMB_REQ_AUTHEN_READ);
    memcpy(pfrm->nonce, nonce, RPMB_FRM_NONCE_LEN);

    return 0;
}

int ufs_rpmb_read_data(u32 addr, u32 *buf, u32 buf_len, int *result)
{
    struct ufs_scsi_cmd cmd;
    struct ufs_hba *hba;
    struct rpmb_data_frame *rpmb_frame = NULL;
    int tag;
    u8 *nonce_buf = NULL;
    u8 *data_buf;
    u32 host_id = 0;
    u32 blk_len;
    u32 left_size = buf_len;
    u32 tran_size;
    u32 i;
    int ret = 0;

    cmd.data_buf = ufs_rpmb_buf;
    hba = ufs_get_host(0);

    if (buf_len % RPMB_FRM_DATA_LEN != 0) {
        dprintf(CRITICAL, "[%s]: RPMB read write need 256 ytes align\n",
            __func__);
        return -UFS_RPMB_DRIVER_ERROR;
    }
    blk_len = buf_len / RPMB_FRM_DATA_LEN;
    data_buf = (u8 *)buf;

    if (!ufshcd_get_free_tag(hba, &tag))
        return -UFS_RPMB_DRIVER_ERROR;

    nonce_buf = rpmb_nonce_buf;
    ufs_rpmb_pre_nonce(nonce_buf, RPMB_FRM_NONCE_LEN);

    for (i = 0; i < blk_len; i++) {
        /* Step 1. Authen data read request */
        if (left_size >= RPMB_FRM_DATA_LEN)
            tran_size = RPMB_FRM_DATA_LEN;
        else
            tran_size = left_size;
        cmd_scsi_security_protocol_out(&cmd, tag, 1,
                           UFS_SECURITY_PROTOCOL,
                           RPMB_PROTOCOL_ID);
        ret = ufshcd_rpmb_read_data_req_prepare(&cmd, tran_size, addr + i, nonce_buf);
        if (ret) {
            dprintf(CRITICAL, "[%s]: RPMB Fail in prepare frame, ret=%d\n",
                __func__, ret);
            ret = -UFS_RPMB_DRIVER_ERROR;
            goto exit;
        }
        ret = ufshcd_queuecommand(hba, &cmd);
        if (ret) {
            dprintf(CRITICAL, "[%s]: RPMB Fail in send read frame, ret=%d\n",
                __func__, ret);
            ret = -UFS_RPMB_DRIVER_ERROR;
            goto exit;
        }

        /* Step 2. Authen data read response */
        cmd_scsi_security_protocol_in(&cmd, tag, 1,
                           UFS_SECURITY_PROTOCOL,
                           RPMB_PROTOCOL_ID);
        ret = ufshcd_queuecommand(hba, &cmd);
        if (ret) {
            dprintf(CRITICAL, "[%s]: RPMB Fail in get read response, ret=%d\n",
                __func__, ret);
            ret = -UFS_RPMB_DRIVER_ERROR;
            goto exit;
        }

        /* Step3. Check Response and parser data*/
        rpmb_frame = cmd.data_buf;
        if (result != NULL) {
            *result = BE16(rpmb_frame->result);
            if (*result) {
                dprintf(CRITICAL, "[%s]: result = %d\n", __func__, *result);
                return -UFS_RPMB_RESULT_ERROR;
            }
        }
        ret = ufshcd_authen_data_read_rsp_check(host_id, &cmd, nonce_buf,
            RPMB_FRM_NONCE_LEN, (data_buf + i * RPMB_FRM_DATA_LEN),
            RPMB_RSP_AUTHEN_READ);
        if (ret) {
            dprintf(CRITICAL, "[%s]: RPMB Data Read Resp Check Fail, ret=%d\n",
                __func__, ret);
            goto exit;
        }

        left_size -= tran_size;
    }

exit:
    ufshcd_put_tag(hba, tag);

    return ret;
}

static int ufshcd_rpmb_write_data_req_prepare(struct ufs_scsi_cmd *cmd,
    const u8 *data_buf, u32 size, u32 wr_cnt, u16 addr)
{
    struct rpmb_data_frame *pfrm = NULL;
    int ret = 0;

    if (cmd == NULL) {
        dprintf(CRITICAL, "[%s]: RPMB cmd pointer NULL\n", __func__);
        return -EIO;
    }
    if (data_buf == NULL) {
        dprintf(CRITICAL, "[%s]: RPMB data pointer NULL\n", __func__);
        return -EIO;
    }

    pfrm = cmd->data_buf;

    if (pfrm == NULL) {
        dprintf(CRITICAL, "[%s]: RPMB pfrm pointer NULL\n", __func__);
        return -EIO;
    }

    memset(pfrm, 0, sizeof(struct rpmb_data_frame));

    /* Prepare write frame */
    memcpy(pfrm->data, data_buf, size);
    pfrm->wr_cnt = BE32(wr_cnt);
    pfrm->addr = BE16(addr);
    pfrm->blk_cnt = BE16(1);
    pfrm->req_resp = BE16(RPMB_REQ_AUTHEN_WRITE);

    /* Compute hmac. */
    ret = rpmb_get_mac(pfrm);
    if (ret) {
        dprintf(CRITICAL, "[%s]: rpmb_get_mac fail, ret=%d\r\n", __func__, ret);
        goto exit;
    }
    memcpy(pfrm->key_MAC, shared_hmac_addr1, RPMB_FRM_KEY_MAC_LEN);

exit:
    return ret;
}

int ufs_rpmb_write_data(u32 addr, u32 *buf, u32 buf_len, int *result)
{
    struct ufs_scsi_cmd cmd;
    struct ufs_hba *hba;
    struct rpmb_data_frame *rpmb_frame = NULL;
    int tag;
    u8 *nonce_buf = NULL;
    u8 *data_buf;
    u32 wr_cnt = 0;
    u32 host_id = 0;
    u32 blk_len;
    u32 left_size = buf_len;
    u32 tran_size;
    u32 i;
    int ret = 0;

    cmd.data_buf = ufs_rpmb_buf;
    hba = ufs_get_host(0);
    if (buf_len % RPMB_FRM_DATA_LEN != 0) {
        dprintf(CRITICAL, "[%s]: RPMB read write need 256 ytes align\n",
            __func__);
        return -UFS_RPMB_DRIVER_ERROR;
    }
    blk_len = buf_len / RPMB_FRM_DATA_LEN;
    data_buf = (u8 *)buf;

    /* Read RPMB write counter */
    nonce_buf = rpmb_nonce_buf;
    ufs_rpmb_pre_nonce(nonce_buf, RPMB_FRM_NONCE_LEN);
    ret = rpmb_authen_read_counter(host_id, nonce_buf, RPMB_FRM_NONCE_LEN,
                       &wr_cnt);
    if (ret) {
        dprintf(CRITICAL, "[%s]: RPMB rpmb_authen_read_counter fail! ret=%d\n",
            __func__, ret);
        ret = -UFS_RPMB_DRIVER_ERROR;
        goto free_nonce_buf;
    }

    if (!ufshcd_get_free_tag(hba, &tag)) {
        ret = -UFS_RPMB_DRIVER_ERROR;
        goto free_nonce_buf;
    }

    for (i = 0; i < blk_len; i++) {
        /* Step 1. Authen data write request */
        if (left_size >= RPMB_FRM_DATA_LEN)
            tran_size = RPMB_FRM_DATA_LEN;
        else
            tran_size = left_size;
        cmd_scsi_security_protocol_out(&cmd, tag, 1,
                           UFS_SECURITY_PROTOCOL,
                           RPMB_PROTOCOL_ID);
        ret = ufshcd_rpmb_write_data_req_prepare(&cmd,
            (data_buf + i * RPMB_FRM_DATA_LEN), tran_size, wr_cnt, addr + i);
        if (ret) {
            dprintf(CRITICAL, "[%s]: Prepare frame fail, ret=%d\n",
                __func__, ret);
            ret = -UFS_RPMB_DRIVER_ERROR;
            goto exit;
        }
        ret = ufshcd_queuecommand(hba, &cmd);
        if (ret) {
            dprintf(CRITICAL, "[%s]: Send write frame fail, ret=%d\n",
                __func__, ret);
            ret = -UFS_RPMB_DRIVER_ERROR;
            goto exit;
        }

        /* Step 2. Result Register Read Request */
        cmd_scsi_security_protocol_out(&cmd, tag, 1,
                           UFS_SECURITY_PROTOCOL,
                           RPMB_PROTOCOL_ID);
        ufshcd_authen_result_read_req_prepare(&cmd);
        ret = ufshcd_queuecommand(hba, &cmd);
        if (ret) {
            dprintf(CRITICAL, "[%s]: Sending result read request fail, ret=%d\n",
                __func__, ret);
            ret = -UFS_RPMB_DRIVER_ERROR;
            goto exit;
        }

        /* Step 3. Check Response for Result Read Request */
        cmd_scsi_security_protocol_in(&cmd, tag, 1,
                        UFS_SECURITY_PROTOCOL,
                        RPMB_PROTOCOL_ID);
        ret = ufshcd_queuecommand(hba, &cmd);
        if (ret) {
            dprintf(CRITICAL, "[%s]: Get result response fail, ret=%d\n",
                __func__, ret);
            ret = -UFS_RPMB_DRIVER_ERROR;
            goto exit;
        }

        /* Step 4. Response frame check */
        rpmb_frame = cmd.data_buf;
        if (result != NULL) {
            *result = BE16(rpmb_frame->result);
            if (*result) {
                dprintf(CRITICAL, "[%s]: result = %d\n", __func__, *result);
                return -UFS_RPMB_RESULT_ERROR;
            }
        }
        ret = ufshcd_authen_data_write_rsp_check(host_id, &cmd, wr_cnt,
               RPMB_RSP_AUTHEN_WRITE);
        if (ret) {
            dprintf(CRITICAL, "[%s]: Response frame check Check Fail, ret=%d\n",
                __func__, ret);
            goto exit;
        }

        left_size -= tran_size;
        wr_cnt++;
    }

exit:
    ufshcd_put_tag(hba, tag);

free_nonce_buf:

    return ret;
}

/* Retrun maximun number of rpmb frames allowed in single upiu */
u32 ufs_rpmb_get_rw_size(void)
{
    struct ufs_hba *hba = ufs_get_host(0);

    return hba->dev_info.rpmb_rw_size;
}

/* Retrun RPMB lu size in bytes */
u64 ufs_rpmb_get_lu_size(void)
{
    struct ufs_hba *hba = ufs_get_host(0);

    return hba->dev_info.rpmb_lu_size;
}

u32 ufs_rpmb_get_setkey_flag(void)
{
    struct ufs_hba *hba = ufs_get_host(0);
    u8 *nonce_buf = NULL;
    u32 wr_cnt = 0;
    int ret = 0;
    u32 setkey_flag;

    nonce_buf = rpmb_nonce_buf;
    if (ufs_rpmb_pre_nonce(nonce_buf, RPMB_FRM_NONCE_LEN))
        memset(nonce_buf, 0, RPMB_FRM_NONCE_LEN);

    /*
     * Read write counter and check ret
     * 0 : no fail, key has programed
     * -1: UFS_RPMB_HMAC_ERROR, but result is correct, key has programed
     * -2: UFS_RPMB_RESULT_ERROR, key has not programed
     */
    ret = rpmb_authen_read_counter(0, nonce_buf, RPMB_FRM_NONCE_LEN,
        &wr_cnt);
    if (ret == 0 || ret == -UFS_RPMB_HMAC_ERROR)
        setkey_flag = 0;
    else
        setkey_flag = 1;

    dprintf(CRITICAL, "[%s]: RPMB setkey_flag=%d\n",
                __func__, setkey_flag);

    return setkey_flag;
}


