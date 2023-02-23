/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <arch/mmu.h>
#include <arch/ops.h>
#include <err.h>
#include <kernel/vm.h>
#include <platform/memory_layout.h>
#include <platform_mtk.h>
#include <rng.h>
#include <smc.h>
#include <smc_id_table.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <mblock.h>
#include <mmc_rpmb.h>
#include <msdc.h>

#include "mmc_cmd.h"
#include "msdc_reg.h"

#define RPMB_KEY_DUMP 0

static u8 *shared_msg_addr;
static u8 *shared_hmac_addr;

#ifndef swab16
#define swab16(x) \
    ((u16)( \
        (((u16)(x) & (u16)0x00ffU) << 8) | \
        (((u16)(x) & (u16)0xff00U) >> 8)))
#endif

#ifndef swab32
#define swab32(x) \
    ((u32)( \
        (((u32)(x) & (u32)0x000000ffUL) << 24) | \
        (((u32)(x) & (u32)0x0000ff00UL) <<  8) | \
        (((u32)(x) & (u32)0x00ff0000UL) >>  8) | \
        (((u32)(x) & (u32)0xff000000UL) >> 24)))
#endif

#ifndef cpu_to_be16
#define cpu_to_be16(x)  ({ u16 _x = x; swab16(_x); })
#endif

#ifndef cpu_to_be32
#define cpu_to_be32(x)  ({ u32 _x = x; swab32(_x); })
#endif

#ifndef be16_to_cpu
#define be16_to_cpu(x)  cpu_to_be16(x)
#endif

#ifndef be32_to_cpu
#define be32_to_cpu(x)  cpu_to_be32(x)
#endif

static const char * const rpmb_err_msg[] = {
    "",
    "General failure",
    "Authentication failure",
    "Counter failure",
    "Address failure",
    "Write failure",
    "Read failure",
    "Authentication key not yet programmed",
};

/*
 * CVE-2020-0436
 * Increase the write count to prevent DOS attack
 */
#define MMC_DUMMY_WRITE_RPMB
/*
 * MMC smc related
 */
/* mmc generic control flags */
#define MMC_MTK_SIP_HMAC    (1 << 0)

/* MMC RPMB HMAC operation */
enum {
    RPMB_HMAC_INIT = 0,
    RPMB_HMAC_DEINIT,
    RPMB_HMAC_CALC
};

/* Return if current boot stage is in secure world or not */
bool mmc_is_in_secure_world(void)
{
    u32 bstage;

    bstage = get_boot_stage();

    /* secure EL1, TFA smc cannot use */
    if ((bstage == BOOT_STAGE_BL2) || (bstage == BOOT_STAGE_BL2_EXT) ||
        (bstage == BOOT_STAGE_DA) || (bstage == BOOT_STAGE_AEE)) {
        return true;
    } else {
        return false;
    }
}

int init_rpmb_sharemem(void)
{
    void *hmac_va = (void *)LK_HMAC_BASE;
    uint32_t hmac_size;
    paddr_t hmac_pa;
    struct __smccc_res smccc_res;
    int ret = 0;
    u64 rpmb_hmac_addr_mb = 0;

    /* map shared memory for rpmb */
    hmac_pa = ROUNDDOWN((paddr_t)LK_HMAC_BASE_PHY, PAGE_SIZE);
    hmac_size = ROUNDUP(LK_HMAC_SIZE, PAGE_SIZE);

    ret = mblock_query_reserved_by_name("system_rpmb_hmac-reserved", 0);
    if (!ret) {
        /* reserve rpmb hmac buffer with mblock */
        rpmb_hmac_addr_mb = mblock_alloc(hmac_size, PAGE_SIZE, MBLOCK_NO_LIMIT,
                                hmac_pa, 0, "system_rpmb_hmac-reserved");
        if (rpmb_hmac_addr_mb != (u64)hmac_pa)
            panic("rpmb_hmac_addr (0x%x) is not taken from mb (0x%x)\n",
                hmac_pa, rpmb_hmac_addr_mb);
    }
    /*
     * allocate desired virtual space, and MMU mapping with it.
     * rpmb share memory is used to communicate with seclib, cannot free.
     */
    ret = vmm_alloc_physical(vmm_get_kernel_aspace(),
            "vm_hmac", hmac_size,
            &hmac_va, PAGE_SIZE_SHIFT,
            hmac_pa,
            VMM_FLAG_VALLOC_SPECIFIC,
            ARCH_MMU_FLAG_CACHED);

    if (ret != NO_ERROR || hmac_va != (void *)LK_HMAC_BASE)
        panic("err=%d, vm_hmac (0x%p) expected (0x%lx)\n",
            ret, hmac_va, LK_HMAC_BASE);

    shared_msg_addr = (u8 *)(hmac_va);
    shared_hmac_addr = (u8 *)(hmac_va + 0xF00);

    if (mmc_is_in_secure_world())
        goto out;

    /* notify tfa to init HMAC  */
    memset(&smccc_res, 0, sizeof(smccc_res));

    __smc_conduit(MTK_SIP_BL_MMC_CONTROL, MMC_MTK_SIP_HMAC,
        RPMB_HMAC_INIT, 0, 0, 0, 0, 0, &smccc_res);

    return smccc_res.a0;

out:
    return ret;
}

int deinit_rpmb_sharemem(void)
{
    struct __smccc_res smccc_res;
    paddr_t hmac_pa;

    hmac_pa = ROUNDDOWN((paddr_t)LK_HMAC_BASE_PHY, PAGE_SIZE);
    mblock_free(hmac_pa);

    if (mmc_is_in_secure_world())
        goto out;

    /* notify tfa to init HMAC  */
    memset(&smccc_res, 0, sizeof(smccc_res));

    __smc_conduit(MTK_SIP_BL_MMC_CONTROL, MMC_MTK_SIP_HMAC,
        RPMB_HMAC_DEINIT, 0, 0, 0, 0, 0, &smccc_res);

    return smccc_res.a0;

out:
    return 0;
}

static int rpmb_hmac(uint32_t pattern, uint32_t size)
{
    struct __smccc_res smccc_res;

    if (mmc_is_in_secure_world())
        goto out;

    /* notify tfa to calculate HMAC  */
    memset(&smccc_res, 0, sizeof(smccc_res));

    __smc_conduit(MTK_SIP_BL_MMC_CONTROL, MMC_MTK_SIP_HMAC,
        RPMB_HMAC_CALC, 0, 0, 0, 0, 0, &smccc_res);

    return smccc_res.a0;

out:
    return 0;
}

static int mmc_rpmb_get_rnd(u32 *rnd)
{
    int ret = 0;

    ret = get_rnd(rnd);

    return ret;
}

static int mmc_rpmb_pre_nonce(u8 *buf, u32 size)
{
    int ret = 0;
    unsigned int rnd_val = 0;
    u8 *ptr = buf;
    u32 i = 0;

    if (size == 0) {
        dprintf(CRITICAL, "[WARNING]%s:nonce size is not match %d(%d)\n",
            __func__, size, RPMB_SZ_NONCE);
        return -1;
    }

    for (i = 0; i < size / 4; i++) {
        ret = mmc_rpmb_get_rnd(&rnd_val);
        if (ret)
            goto out;

        memcpy(ptr, &rnd_val, 4);
        ptr += 4;
    }

    if (size & 0x3) {
        ret = mmc_rpmb_get_rnd(&rnd_val);
        if (ret)
            goto out;
        memcpy(ptr, &rnd_val, size & 0x3);
    }

out:
    if (ret) {
        dprintf(CRITICAL, "%s:get_rnd is not ready,ret:%d\n",
            __func__, ret);
    }

    return ret;
}

static int mmc_rpmb_pre_frame(struct mmc_rpmb_req *rpmb_req)
{
    struct mmc_rpmb_cfg *rpmb_cfg = rpmb_req->rpmb_cfg;
    u8 *data_frame = rpmb_req->data_frame;
    u32 wc;
    u16 blks = rpmb_cfg->blk_cnt;
    u16 addr;
    u16 type;
    u8 *nonce = rpmb_cfg->nonce;

    memset(data_frame, 0, 512 * blks);

    type = cpu_to_be16(rpmb_cfg->type);
    memcpy(data_frame + RPMB_TYPE_BEG, &type, 2);

    if (rpmb_cfg->type == RPMB_PROGRAM_KEY) {
        memcpy(data_frame + RPMB_MAC_BEG, rpmb_cfg->mac, RPMB_SZ_MAC);
    } else if (rpmb_cfg->type == RPMB_GET_WRITE_COUNTER ||
           rpmb_cfg->type == RPMB_READ_DATA) {
        /*
         * One package prepared
         * This request needs Nonce and type
         * If is data read, then also need addr
         */
        if (rpmb_cfg->type == RPMB_READ_DATA) {
            addr = cpu_to_be16(rpmb_cfg->addr);
            memcpy(data_frame + RPMB_ADDR_BEG, &addr, 2);
        }

        /* convert Nonce code */
        memcpy(data_frame + RPMB_NONCE_BEG, nonce, RPMB_SZ_NONCE);
    } else if (rpmb_cfg->type == RPMB_WRITE_DATA) {
        addr = cpu_to_be16(rpmb_cfg->addr);
        memcpy(data_frame + RPMB_ADDR_BEG, &addr, 2);

        wc = cpu_to_be32(rpmb_cfg->wc);

        memcpy(data_frame + RPMB_WCOUNTER_BEG, &wc, 4);

        blks = cpu_to_be16(rpmb_cfg->blk_cnt);
        memcpy(data_frame + RPMB_BLKS_BEG, &blks, 2);
    }

    return 0;
}

static int mmc_rpmb_post_frame(struct mmc_rpmb_req *rpmb_req)
{
    struct mmc_rpmb_cfg *rpmb_cfg = rpmb_req->rpmb_cfg;
    u8 *data_frame = rpmb_req->data_frame;
    u16 result;

    memcpy(&result, data_frame + RPMB_RES_BEG, 2);
    rpmb_cfg->result = cpu_to_be16(result);

    if (rpmb_cfg->type == RPMB_GET_WRITE_COUNTER ||
        rpmb_cfg->type == RPMB_WRITE_DATA) {
        rpmb_cfg->wc = cpu_to_be32(*(u32 *)&data_frame[RPMB_WCOUNTER_BEG]);
        dprintf(INFO, "%s, rpmb_cfg->wc = %x\n", __func__, rpmb_cfg->wc);
    }

    if (rpmb_cfg->type == RPMB_GET_WRITE_COUNTER ||
        rpmb_cfg->type == RPMB_READ_DATA) {
        /* nonce copy */
        memcpy(rpmb_cfg->nonce, data_frame + RPMB_NONCE_BEG, RPMB_SZ_NONCE);
    }

    if (rpmb_cfg->mac) {
        /*
         * To do. compute if mac is legal or not.
         * Current we don't do this since we just perform get wc to check if we need set key.
         */
        memcpy(rpmb_cfg->mac, data_frame + RPMB_MAC_BEG, RPMB_SZ_MAC);
    }

    return 0;
}


static int mmc_rpmb_send_command(struct mmc_card *card, u8 *data_frame,
    u16 blks, u16 type, u8 req_type)
{
    struct mmc_host *host = card->host;
    u32 maxblks = 0;
    u32 blksz = host->blklen;
    u32 leftblks;
    unsigned long blknr = 0;
    int err = MMC_ERR_NONE;
    int autocmd = 0;

    /*
     * Auto CMD23 and CMD25 or CMD18
     */
    if ((req_type == RPMB_REQ && type == RPMB_WRITE_DATA) ||
        (req_type == RPMB_REQ && type == RPMB_PROGRAM_KEY))
        msdc_set_reliable_write(host, 1);
    else
        msdc_set_reliable_write(host, 0);

    /* backup autocmd */
    autocmd = msdc_get_autocmd(host);
    msdc_set_autocmd(host, MSDC_AUTOCMD23);
    msdc_set_max_phys_segs(host);
    maxblks = host->max_phys_segs;

    if (req_type == RPMB_REQ) {
        do {
            leftblks = ((blks > maxblks) ? maxblks : blks);
            if (mmc_dev_bwrite(card, blknr, leftblks, data_frame))
                err = ERR_IO;
            blknr += leftblks;
            data_frame += maxblks * blksz;
            blks -= leftblks;
        } while (blks);
    } else {
        do {
            leftblks = ((blks > maxblks) ? maxblks : blks);
            if (mmc_dev_bread(card, blknr, leftblks, data_frame))
                err = ERR_IO;
            blknr += leftblks;
            data_frame += maxblks * blksz;
            blks -= leftblks;
        } while (blks);
    }

    /* restore autocmd */
    msdc_set_autocmd(host, autocmd);
    msdc_set_max_phys_segs(host);

    if (err)
        dprintf(CRITICAL, "%s: CMD%s failed. (%d)\n",
            __func__, ((req_type == RPMB_REQ) ? "25":"18"), err);

    return err;
}

static int mmc_rpmb_start_req(struct mmc_card *card, struct mmc_rpmb_req *rpmb_req)
{
    int err = 0;
    u16 blks = rpmb_req->rpmb_cfg->blk_cnt;
    u16 type = rpmb_req->rpmb_cfg->type;
    u8 *data_frame = rpmb_req->data_frame;

    /*
     * STEP 1: send request to RPMB partition
     */
    if (type == RPMB_WRITE_DATA)
        err = mmc_rpmb_send_command(card, data_frame, blks, type, RPMB_REQ);
    else
        err = mmc_rpmb_send_command(card, data_frame, 1, type, RPMB_REQ);

    if (err) {
        dprintf(CRITICAL,
            "%s step 1, request failed (%d)\n", __func__, err);
        goto out;
    }

    /*
     * STEP 2: check write result
     * Only for WRITE_DATA or Program key
     */
    memset(data_frame, 0, 512 * blks);

    if (type == RPMB_WRITE_DATA || type == RPMB_PROGRAM_KEY) {
        data_frame[RPMB_TYPE_BEG + 1] = RPMB_RESULT_READ;
        err = mmc_rpmb_send_command(card, data_frame, 1, RPMB_RESULT_READ, RPMB_REQ);
        if (err) {
            dprintf(CRITICAL,
                "%s step 2, request result failed (%d)\n", __func__, err);
            goto out;
        }
    }

    /*
     * STEP 3: get response from RPMB partition
     */
    data_frame[RPMB_TYPE_BEG] = 0;
    data_frame[RPMB_TYPE_BEG + 1] = type;

    if (type == RPMB_READ_DATA)
        err = mmc_rpmb_send_command(card, data_frame, blks, type, RPMB_RESP);
    else
        err = mmc_rpmb_send_command(card, data_frame, 1, type, RPMB_RESP);

    if (err) {
        dprintf(CRITICAL,
            "%s step 3, response failed (%d)\n", __func__, err);
    }

out:
    return err;
}

int mmc_rpmb_check_result(u16 result)
{
    if (result) {
        dprintf(CRITICAL,
            "%s %s %s\n", __func__, rpmb_err_msg[result & 0x7],
            (result & 0x80) ? "Write counter has expired" : "");
    }

    return result;
}

int mmc_rpmb_get_wc(u32 *wc, int *rpmb_result)
{
    struct mmc_card *card = mmc_get_card(0);
    struct mmc_host *host = mmc_get_host(0);
    struct mmc_rpmb_cfg rpmb_cfg;
    struct mmc_rpmb_req rpmb_req;
    u8 rpmb_frame[512];
    u8 nonce[16];
    u8 val;
    int ret = 0;

    memset(&rpmb_cfg, 0, sizeof(struct mmc_rpmb_cfg));
    ret = mmc_rpmb_pre_nonce(nonce, RPMB_SZ_NONCE);
    if (ret) {
        dprintf(CRITICAL, "%s, get nonce fail!\n", __func__);
        return ret;
    }

    rpmb_cfg.type = RPMB_GET_WRITE_COUNTER;
    rpmb_cfg.result = 0;
    rpmb_cfg.blk_cnt = 1;
    rpmb_cfg.addr = 0;
    rpmb_cfg.wc = 0;
    rpmb_cfg.nonce = nonce;
    rpmb_cfg.data = NULL;
    rpmb_cfg.mac = NULL;

    /*
     * 1. Switch to RPMB partition.
     */
    if (host->curr_part != EXT_CSD_PART_CFG_RPMB_PART) {
        val = (card->ext_csd.part_cfg & ~0x7) | (EXT_CSD_PART_CFG_RPMB_PART & 0x7);
        ret = mmc_set_part_config(card, val);
        if (ret) {
            dprintf(CRITICAL,
                "%s, mmc_set_part_config failed!! (%x)\n", __func__, ret);
            return ret;
        }

        dprintf(INFO, "%s, mmc_set_part_config done!!\n", __func__);
    }
    rpmb_req.rpmb_cfg = &rpmb_cfg;
    rpmb_req.data_frame = rpmb_frame;


    /*
     * 2. Prepare get wc data frame.
     */
    ret = mmc_rpmb_pre_frame(&rpmb_req);

    /*
     * 3. CMD 23 and followed CMD25/18 procedure.
     */
    ret = mmc_rpmb_start_req(card, &rpmb_req);
    if (ret) {
        dprintf(CRITICAL,
            "%s, mmc_rpmb_part_ops failed!! (%x)\n", __func__, ret);
        return ret;
    }

    ret = mmc_rpmb_post_frame(&rpmb_req);

    dprintf(INFO, "%s, rpmb_req.result=%x\n", __func__, rpmb_cfg.result);

    memcpy(shared_msg_addr, rpmb_frame + RPMB_DATA_BEG, 284);
    arch_clean_invalidate_cache_range((addr_t) shared_msg_addr, 4096);
    ret = rpmb_hmac(0, 284);
    if (ret)
        dprintf(CRITICAL, "%s, rpmb_hmac error. (0x%x)\n", __func__, ret);

    if (memcmp(rpmb_frame + RPMB_MAC_BEG, shared_hmac_addr, 32) != 0) {
        dprintf(CRITICAL, "%s, compare hmac error!!\n", __func__);
        return -RPMB_HMAC_ERROR;
    }
    if (memcmp(nonce, rpmb_cfg.nonce, RPMB_SZ_NONCE) != 0) {
        dprintf(CRITICAL, "%s, compare nonce error!!\n", __func__);
        return -RPMB_NONCE_ERROR;
    }

    /*
     * 4. Check result.
     */
    *rpmb_result = mmc_rpmb_check_result(rpmb_cfg.result);
    *wc = rpmb_cfg.wc;

    return ret;
}

#if CFG_RPMB_KEY_PROGRAMED_IN_KERNEL
int mmc_rpmb_set_key(u8 *key, int *rpmb_result)
{
    struct mmc_host *host = mmc_get_host(0);
    struct mmc_card *card = mmc_get_card(0);
    struct mmc_rpmb_cfg rpmb_cfg;
    struct mmc_rpmb_req rpmb_req;
    u16 result = 0;
    u8 rpmb_frame[512];
    u8 val;
    int ret;
    u32 wc;
    int i;

    *rpmb_result = 0;
    ret = mmc_rpmb_get_wc(&wc, rpmb_result);

#if RPMB_KEY_DUMP
    dprintf(INFO, "rpmb key=\n");
    for (i = 0; i < 32; i++)
        dprintf(INFO, "0x%x, ", key[i]);
        dprintf(INFO, "\n");
#endif

    /* if any errors, return it */
    if (ret) {
        printf("%s, get wc failed!! (%d)\n", __func__, ret);
        return ret;
    }

    if (*rpmb_result != 7) {
        print("mmc rpmb key already programmed!!\n");
        return ret;
    }

    print("mmc rpmb key will be programmed in kernel, skip now!\n");

    return ret;

}

#else
int mmc_rpmb_set_key(u8 *key)
{
    struct mmc_card *card = mmc_get_card(0);
    struct mmc_rpmb_cfg rpmb_cfg;
    struct mmc_rpmb_req rpmb_req;
    u8 rpmb_frame[512];
    u8 val;
    int ret, rpmb_result = 0;
    u32 wc;
#if RPMB_KEY_DUMP
    int i;
#endif

    ret = mmc_rpmb_get_wc(&wc, &rpmb_result);

#if RPMB_KEY_DUMP
    dprintf(INFO, "rpmb key=\n");
    for (i = 0; i < 32; i++)
        dprintf(INFO, "0x%x, ", key[i]);
#endif

    /* if any errors, return it */
    if (ret) {
        dprintf(CRITICAL, "%s, get wc failed!! (%d)\n", __func__, ret);
        return ret;
    }

    if (rpmb_result != 7) {
        dprintf(CRITICAL, "mmc rpmb key is already programmed!!\n");
        return ret;
    }

    memset(&rpmb_cfg, 0, sizeof(struct mmc_rpmb_cfg));

    rpmb_cfg.type = RPMB_PROGRAM_KEY;
    rpmb_cfg.result = 0;
    rpmb_cfg.blk_cnt = 1;
    rpmb_cfg.addr = 0;
    rpmb_cfg.wc = 0;
    rpmb_cfg.nonce = NULL;
    rpmb_cfg.data = NULL;
    rpmb_cfg.mac = key;

    /*
     * 1. Switch to RPMB partition.
     */
    val = (card->ext_csd.part_cfg & ~0x7) | (EXT_CSD_PART_CFG_RPMB_PART & 0x7);
    ret = mmc_set_part_config(card, val);
    if (ret) {
        dprintf(CRITICAL,
            "%s, mmc_set_part_config failed!! (%x)\n", __func__, ret);
        return ret;
    }

    rpmb_req.rpmb_cfg = &rpmb_cfg;
    rpmb_req.data_frame = rpmb_frame;

    /*
     * 2. Prepare program key data frame.
     */
    ret = mmc_rpmb_pre_frame(&rpmb_req);

    /*
     * 3. CMD 23 and followed CMD25/18 procedure.
     */
    ret = mmc_rpmb_start_req(card, &rpmb_req);
    if (ret) {
        dprintf(CRITICAL,
            "%s, mmc_rpmb_part_ops failed!! (%x)\n", __func__, ret);
        return ret;
    }

    ret = mmc_rpmb_post_frame(&rpmb_req);
    dprintf(INFO, "%s, rpmb_req.result=%x\n", __func__, rpmb_cfg.result);

    /*
     * 4. Check result.
     */
    ret = mmc_rpmb_check_result(rpmb_cfg.result);
    if (ret == 0)
        dprintf(INFO, "RPMB key is successfully programmed!!\n");

    return ret;
}
#endif

u32 mmc_rpmb_get_size(void)
{
    struct mmc_card *card = mmc_get_card(0);

    return card->ext_csd.rpmb_sz;
}

//get ext_csd[222]
u32 mmc_rpmb_get_rel_wr_sec_c(void)
{
    struct mmc_card *card = mmc_get_card(0);

    dprintf(INFO, "RPMB rel_wr_sec=%d\n", card->ext_csd.rel_wr_sec_c);

    return card->ext_csd.rel_wr_sec_c;
}

#define RPMB_DUMMY_SIZE 256
/*
 * Make sure the write count is incremented at every boot time.
 * to prevent linux kernel Replay Attacks.
 */
void mmc_dummy_rpmb_write(void)
{
    int ret = 0, result = 0;
    u8 data_buf[RPMB_DUMMY_SIZE] = {0};

    ret = mmc_rpmb_read_data(0x0, data_buf, RPMB_DUMMY_SIZE, &result);
    if (ret) {
        dprintf(CRITICAL, "[%s] read failed\n", __func__);
        return;
    }

    ret = mmc_rpmb_write_data(0x0, data_buf, RPMB_DUMMY_SIZE, &result);
    if (ret)
        dprintf(CRITICAL, "[%s] write failed\n", __func__);
}

int mmc_rpmb_read_data(u32 blkAddr, u8 *buf, u32 bufLen, int *result)
{
    struct mmc_card *card = mmc_get_card(0);
    struct mmc_host *host = mmc_get_host(0);
    struct mmc_rpmb_cfg rpmb_cfg;
    struct mmc_rpmb_req rpmb_req;
    u32 blkLen;
    u32 left_size = bufLen;
    u32 tran_size;
    u8 rpmb_frame[512];
    u8 nonce[RPMB_SZ_NONCE];
    u8 val;
    u32 ret = 0, i;

    /*
     * 1. Switch to RPMB partition.
     */
    if (host->curr_part != EXT_CSD_PART_CFG_RPMB_PART) {
        val = (card->ext_csd.part_cfg & ~0x7) | (EXT_CSD_PART_CFG_RPMB_PART & 0x7);
        ret = mmc_set_part_config(card, val);
        if (ret) {
            dprintf(CRITICAL,
                "%s, mmc_set_part_config failed!! (0x%x)\n", __func__, ret);
            return ret;
        }

        dprintf(INFO, "%s, mmc_set_part_config done!!\n", __func__);
    }

    blkLen = (bufLen % RPMB_SZ_DATA) ? (bufLen / RPMB_SZ_DATA + 1) : (bufLen / RPMB_SZ_DATA);

    for (i = 0; i < blkLen; i++) {
        memset(&rpmb_cfg, 0, sizeof(struct mmc_rpmb_cfg));
        ret = mmc_rpmb_pre_nonce(nonce, RPMB_SZ_NONCE);
        if (ret) {
            dprintf(CRITICAL, "%s, get nonce fail!\n", __func__);
            return ret;
        }

        rpmb_cfg.type = RPMB_READ_DATA;
        rpmb_cfg.result = 0;
        rpmb_cfg.blk_cnt = 1;
        rpmb_cfg.addr = blkAddr + i;
        rpmb_cfg.wc = 0;
        rpmb_cfg.nonce = nonce;
        rpmb_cfg.data = NULL;
        rpmb_cfg.mac = NULL;

        rpmb_req.rpmb_cfg = &rpmb_cfg;
        rpmb_req.data_frame = rpmb_frame;

        /*
         * 2. Prepare read data frame.
         */
        ret = mmc_rpmb_pre_frame(&rpmb_req);

        /*
         * 3. CMD 23 and followed CMD25/18 procedure.
         */
        ret = mmc_rpmb_start_req(card, &rpmb_req);
        if (ret) {
            dprintf(CRITICAL,
                "%s, mmc_rpmb_start_req failed!! (0x%x)\n", __func__, ret);
            return ret;
        }

        ret = mmc_rpmb_post_frame(&rpmb_req);
        dprintf(INFO, "%s, rpmb_req.result=%x\n", __func__, rpmb_cfg.result);

        /*
         * 4. Check result.
         */
        *result = mmc_rpmb_check_result(rpmb_cfg.result);
        if (*result)
            return -RPMB_RESULT_ERROR;

        /*
         * 5. Authenticate hmac.
         */
        memcpy(shared_msg_addr, rpmb_frame + RPMB_DATA_BEG, 284);

        arch_clean_invalidate_cache_range((addr_t) shared_msg_addr, 4096);

        ret = rpmb_hmac(0, 284);
        if (ret)
            dprintf(CRITICAL, "%s, rpmb_hmac error. (0x%x)\n", __func__, ret);

        if (memcmp(rpmb_frame + RPMB_MAC_BEG, shared_hmac_addr, 32) != 0) {
            dprintf(CRITICAL, "%s, compare hmac error!!\n", __func__);
            return -RPMB_HMAC_ERROR;
        }

        /*
         * 6. Authenticate nonce.
         */
        if (memcmp(nonce, rpmb_cfg.nonce, RPMB_SZ_NONCE) != 0) {
            dprintf(CRITICAL, "%s, compare nonce error!!\n", __func__);
            return -RPMB_NONCE_ERROR;
        }

        if (left_size >= RPMB_SZ_DATA)
            tran_size = RPMB_SZ_DATA;
        else
            tran_size = left_size;

        memcpy((u8 *)buf + i * RPMB_SZ_DATA, rpmb_frame + RPMB_DATA_BEG, tran_size);

        left_size -= tran_size;
    }

    return ret;
}

int mmc_rpmb_write_data(u32 blkAddr, const u8 *buf, u32 bufLen, int *result)
{
    struct mmc_card *card = mmc_get_card(0);
    struct mmc_rpmb_cfg rpmb_cfg;
    struct mmc_rpmb_req rpmb_req;
    u32 blkLen;
    u32 left_size = bufLen;
    u32 tran_size;
    u8 rpmb_frame[512];
    int ret = 0;
    u32 wc = 0;
    u32 i = 0;

    blkLen = (bufLen % RPMB_SZ_DATA) ? (bufLen / RPMB_SZ_DATA + 1) : (bufLen / RPMB_SZ_DATA);

    for (i = 0; i < blkLen; i++) {

        ret = mmc_rpmb_get_wc(&wc, result);

        /* if any errors, return it */
        if (ret) {
            dprintf(CRITICAL, "%s, get wc failed!! (%d)\n", __func__, ret);
            return ret;
        }

        memset(&rpmb_cfg, 0, sizeof(struct mmc_rpmb_cfg));

        rpmb_cfg.type = RPMB_WRITE_DATA;
        rpmb_cfg.result = 0;
        rpmb_cfg.blk_cnt = 1;
        rpmb_cfg.addr = blkAddr + i;
        rpmb_cfg.wc = wc;
        rpmb_cfg.nonce = NULL;
        rpmb_cfg.data = buf;
        rpmb_cfg.mac = NULL;

        rpmb_req.rpmb_cfg = &rpmb_cfg;
        rpmb_req.data_frame = rpmb_frame;

        /*
         * 1. Prepare write data frame.
         */
        ret = mmc_rpmb_pre_frame(&rpmb_req);

        if (left_size >= RPMB_SZ_DATA)
            tran_size = RPMB_SZ_DATA;
        else
            tran_size = left_size;

        memcpy(rpmb_frame + RPMB_DATA_BEG, (u8 *)buf + i * RPMB_SZ_DATA, tran_size);

        /*
         * 2. Compute hmac.
         */
        memcpy(shared_msg_addr, rpmb_frame + RPMB_DATA_BEG, 284);
        arch_clean_invalidate_cache_range((addr_t) shared_msg_addr, 4096);

        ret = rpmb_hmac(0, 284);
        if (ret)
            dprintf(CRITICAL, "%s, rpmb_hmac error!! (0x%x)\n", __func__, ret);

        memcpy(rpmb_frame + RPMB_MAC_BEG, shared_hmac_addr, 32);

        /*
         * 3. CMD 23 and followed CMD25/18 procedure.
         */
        ret = mmc_rpmb_start_req(card, &rpmb_req);
        if (ret) {
            dprintf(CRITICAL,
                "%s, mmc_rpmb_part_ops failed!! (0x%x)\n", __func__, ret);
            return ret;
        }

        ret = mmc_rpmb_post_frame(&rpmb_req);

        dprintf(INFO, "%s, rpmb_req.result=%x\n", __func__, rpmb_cfg.result);

        /*
         * 4. Check result.
         */
        *result = mmc_rpmb_check_result(rpmb_cfg.result);
        if (*result)
            return -RPMB_RESULT_ERROR;

        /*
         * 5. Authenticate hmac.
         */
        memcpy(shared_msg_addr, rpmb_frame + RPMB_DATA_BEG, 284);

        arch_clean_invalidate_cache_range((addr_t) shared_msg_addr, 4096);

        ret = rpmb_hmac(0, 284);
        if (ret)
            dprintf(CRITICAL, "%s, rpmb_hmac error. (0x%x)\n", __func__, ret);

        if (memcmp(rpmb_frame + RPMB_MAC_BEG, shared_hmac_addr, 32) != 0) {
            dprintf(CRITICAL, "%s, compare hmac error!!\n", __func__);
            return -RPMB_HMAC_ERROR;
        }

        left_size -= tran_size;
    }

    return ret;
}
