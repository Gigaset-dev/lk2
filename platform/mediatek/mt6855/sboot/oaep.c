/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <assert.h>
#include <debug.h>
#include <malloc.h>
#include <oaep.h>
#include <rng.h>
#include <sboot.h>
#include <string.h>
#include <verified_boot.h>
#include <verified_boot_error.h>

static void dump_buf(const uint8_t *buf, uint32_t buf_sz)
{
    uint32_t idx = 0;

    if (buf_sz < 4)
        return;

    for (idx = 0; idx <= buf_sz - 4; idx += 4) {
        dprintf(INFO, "[0x%x] 0x%x 0x%x 0x%x 0x%x\n", idx,
                *(buf + idx),
                *(buf + idx + 1),
                *(buf + idx + 2),
                *(buf + idx + 3));
    }

    return;
}

static int i2osp(uint32_t integer,
                 uint8_t *osp,
                 uint32_t osp_sz)
{
    int ret = OAEP_OK;
    int32_t idx = 0;

    memset(osp, 0x0, osp_sz);
    if (osp_sz < 1)
        return ERR_OAEP_I2OSP_FAIL;

    idx = osp_sz - 1;

    while (integer != 0) {
        osp[idx] = integer % 0x100;
        integer /= 0x100;
        idx--;
        if (idx < 0) {
            ret = ERR_OAEP_I2OSP_FAIL;
            break;
        }
    }

    return ret;
}

static int mgf1_process(uint8_t *seed,
                     uint32_t seed_sz,
                     uint8_t *mask,
                     uint32_t mask_sz)
{
    uint32_t counter = 0;
    uint32_t hash_ret = 0;
    uint8_t hash[SHA256_HASH_SZ] = {0};
    uint8_t *input = NULL;
    uint32_t input_sz = 0;
    uint32_t num_round = 0;
    int ret = OAEP_OK;

    if (seed == NULL)
        return ERR_OAEP_MGF1_INVALID_SEED_BUF;

    if (seed_sz == 0)
        return ERR_OAEP_MGF1_INVALID_SEED_BUF_SZ;

    if (mask == NULL)
        return ERR_OAEP_MGF1_INVALID_MASK_BUF;

    if (mask_sz == 0)
        return ERR_OAEP_MGF1_INVALID_MASK_BUF_SZ;

    input_sz = seed_sz + MGF1_COUNTER_SZ;
    input = (uint8_t *)malloc(input_sz);
    if (input == NULL) {
        ret = ERR_OAEP_MGF1_MALLOC_FAIL;
        return ret;
    }

    memcpy(input, seed, seed_sz * sizeof(char));

    num_round = (mask_sz + SHA256_HASH_SZ - 1) / SHA256_HASH_SZ;
    for (counter = 0; counter < num_round; counter++) {
        hash_ret = 0;
        ret = i2osp(counter, input + seed_sz, MGF1_COUNTER_SZ);
        if (ret)
            goto error;
        hash_ret = sec_sha256_adapter(input, input_sz, hash);
        if (hash_ret) {
            ret = ERR_OAEP_MGF1_HASH_FAIL;
            goto error;
        }
        memcpy(mask + counter * SHA256_HASH_SZ, hash,
               OAEP_MIN(mask_sz - counter * SHA256_HASH_SZ, SHA256_HASH_SZ));
    }

error:
    if (input != NULL)
        free(input);

    return ret;
}

int oaep_encode(uint8_t *msg,
                uint32_t msg_sz,
                uint8_t *encoded_msg,
                uint32_t encoded_msg_sz)
{
    uint32_t idx = 0;
    uint8_t db_mask[DB_SZ] = {0};
    int ret = OAEP_OK;
    uint32_t rnd_val = 0;
    uint8_t *const masked_seed = encoded_msg + 1;
    uint8_t *const masked_db   = encoded_msg + 1 + SEED_SZ;
    uint8_t seed_mask[SEED_SZ] = {0};
    const uint8_t zero_hash[SEED_SZ] = {ZERO_HASH};

    if (msg == NULL) {
        ret = ERR_OAEP_INVALID_MSG_BUF;
        goto error;
    }

    if (msg_sz == 0) {
        ret = ERR_OAEP_INVALID_MSG_BUF_SZ;
        goto error;
    }

    if (encoded_msg == NULL) {
        ret = ERR_OAEP_INVALID_EMSG_BUF;
        goto error;
    }

    if (encoded_msg_sz == 0) {
        ret = ERR_OAEP_INVALID_EMSG_BUF_SZ;
        goto error;
    }

    if (encoded_msg_sz != RSA_MODULUS_SZ) {
        ret = ERR_OAEP_INVALID_EMSG_BUF_SZ;
        goto error;
    }

    if (msg_sz > DB_SZ - SEED_SZ - 1) {
        ret = ERR_OAEP_MSG_TOO_LONG;
        goto error;
    }

    memset(encoded_msg, 0x0, encoded_msg_sz);

    /* get seed from random number generator */
    if ((SEED_SZ % sizeof(uint32_t)) != 0) {
        ret = ERR_OAEP_INVALID_SEED_SZ;
        goto error;
    }

    for (idx = 0; idx < SEED_SZ; idx += 4) {
        rnd_val = 0;
        ret = get_rnd(&rnd_val);
        if (ret)
            goto error;
        memcpy(masked_seed + idx, &rnd_val, sizeof(uint32_t));
    }

    /* store db in masked_db, mask not applied yet */
    memcpy(masked_db, zero_hash, SHA256_HASH_SZ);
    assert(DB_SZ >= msg_sz+1);
    masked_db[DB_SZ - msg_sz - 1] = 0x1;
    memcpy(masked_db + DB_SZ - msg_sz, msg, msg_sz);

    /* create db_mask */
    ret = mgf1_process(masked_seed, SEED_SZ, db_mask, DB_SZ);
    if (ret)
        goto error;

    /* apply db_mask to create masked_db */
    for (idx = 0; idx < DB_SZ; idx++)
        masked_db[idx] ^= db_mask[idx];

    /* create seed_mask */
    ret = mgf1_process(masked_db, DB_SZ, seed_mask, SEED_SZ);
    if (ret)
        goto error;

    /* apply seed_mask to create masked_seed */
    for (idx = 0; idx < SEED_SZ; idx++)
        masked_seed[idx] ^= seed_mask[idx];

error:
    if (ret)
        dprintf(CRITICAL, "[oaep] encode fail (0x%x)\n", ret);

    return ret;
}

int oaep_decode(uint8_t *encoded_msg,
                uint32_t encoded_msg_sz,
                uint8_t *msg,
                uint32_t *msg_sz)
{
    uint8_t db_mask[DB_SZ] = {0};
    uint32_t idx = 0;
    uint8_t *const masked_seed = encoded_msg + 1;
    uint8_t *const masked_db   = encoded_msg + 1 + SEED_SZ;
    int ret = OAEP_OK;
    uint8_t seed_mask[SEED_SZ] = {0};
    const uint8_t zero_hash[SEED_SZ] = {ZERO_HASH};

    if (msg == NULL) {
        ret = ERR_OAEP_INVALID_MSG_BUF;
        goto error;
    }

    if (msg_sz == NULL) {
        ret = ERR_OAEP_INVALID_MSG_BUF_SZ;
        goto error;
    }

    if (*msg_sz == 0) {
        ret = ERR_OAEP_INVALID_MSG_BUF_SZ;
        goto error;
    }

    if (encoded_msg == NULL) {
        ret = ERR_OAEP_INVALID_EMSG_BUF;
        goto error;
    }

    if (encoded_msg_sz == 0) {
        ret = ERR_OAEP_INVALID_EMSG_BUF_SZ;
        goto error;
    }

    if (encoded_msg_sz != RSA_MODULUS_SZ) {
        ret = ERR_OAEP_INVALID_EMSG_BUF_SZ;
        goto error;
    }

    memset(msg, 0x0, *msg_sz);

    /* recover seed_mask */
    ret = mgf1_process(masked_db, DB_SZ, seed_mask, SEED_SZ);
    if (ret)
        goto error;

    /* recover seed */
    /* after this step, data in masked_seed is actually seed */
    for (idx = 0; idx < SEED_SZ; idx++)
        masked_seed[idx] ^= seed_mask[idx];

    /* recover db_mask */
    ret = mgf1_process(masked_seed, SEED_SZ, db_mask, DB_SZ);
    if (ret)
        goto error;

    /* recover db */
    /* after this step, data in masked_db is actually db */
    for (idx = 0; idx < DB_SZ; idx++)
        masked_db[idx] ^= db_mask[idx];

    /* check against zero hash */
    if (memcmp(masked_db, zero_hash, SHA256_HASH_SZ)) {
        ret = ERR_OAEP_ZERO_HASH_CHK_FAIL;
        dprintf(CRITICAL, "[oaep] zero hash compare fail\n");
        dprintf(CRITICAL, "[oaep] zero hash:\n");
        dump_buf(zero_hash, SHA256_HASH_SZ);

        dprintf(CRITICAL, "[oaep] db:\n");
        dump_buf(masked_db, DB_SZ);

        goto error;
    }

    for (idx = SHA256_HASH_SZ; idx < DB_SZ; idx++) {
        if (masked_db[idx] == 0)
            continue;
        else if (masked_db[idx] == 1)
            break;
        else {
            ret = ERR_OAEP_INVALID_FORMAT;
            goto error;
        }
    }

    /* msg starts at idx + 1 */
    if (DB_SZ - idx - 1 > *msg_sz) {
        ret = ERR_OAEP_INVALID_MSG_BUF_SZ;
        goto error;
    }

    *msg_sz = DB_SZ - idx - 1;
    memcpy(msg, masked_db + idx + 1, *msg_sz);

error:
    if (ret)
        dprintf(CRITICAL, "[oaep] decode fail (0x%x)\n", ret);

    return ret;
}

