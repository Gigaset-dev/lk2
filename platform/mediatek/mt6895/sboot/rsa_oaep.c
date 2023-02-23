/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <debug.h>
#include <oaep.h>
#include <verified_boot.h>
#include <verified_boot_error.h>

int rsa_oaep_encrypt(uint8_t *msg,
                     uint32_t msg_sz,
                     uint8_t *key_e,
                     uint32_t key_e_sz,
                     uint8_t *key_n,
                     uint32_t key_n_sz,
                     uint8_t *emsg,
                     uint32_t *emsg_sz)
{
    uint32_t ret = RSA_OAEP_OK;
    int rsa_ret = 0;
    uint8_t tmp_sig[RSA_MODULUS_SZ];
    uint32_t sig_sz = RSA_MODULUS_SZ;

    if (msg == NULL) {
        ret = ERR_RSA_OAEP_INVALID_MSG_BUF;
        goto error;
    }

    if (msg_sz == 0) {
        ret = ERR_RSA_OAEP_INVALID_MSG_BUF_SZ;
        goto error;
    }

    if (key_e == NULL) {
        ret = ERR_RSA_OAEP_INVALID_KEY_E_BUF;
        goto error;
    }

    if (key_e_sz == 0) {
        ret = ERR_RSA_OAEP_INVALID_KEY_E_BUF_SZ;
        goto error;
    }

    if (key_n == NULL) {
        ret = ERR_RSA_OAEP_INVALID_KEY_N_BUF;
        goto error;
    }

    if (key_n_sz == 0) {
        ret = ERR_RSA_OAEP_INVALID_KEY_N_BUF_SZ;
        goto error;
    }

    if (emsg == NULL) {
        ret = ERR_RSA_OAEP_INVALID_EMSG_BUF;
        goto error;
    }

    if (emsg_sz == NULL) {
        ret = ERR_RSA_OAEP_INVALID_EMSG_BUF_SZ;
        goto error;
    }

    if (*emsg_sz == 0) {
        ret = ERR_RSA_OAEP_INVALID_EMSG_BUF_SZ;
        goto error;
    }

    ret = oaep_encode(msg, msg_sz, tmp_sig, RSA_MODULUS_SZ);
    if (ret)
        goto error;

    rsa_ret = sec_rsa_exptmod(tmp_sig, sig_sz,
                              emsg, emsg_sz,
                              key_e, key_e_sz,
                              key_n, key_n_sz);
    if (rsa_ret) {
        dprintf(CRITICAL, "[rsa_oeap] rsa_exptmod fail (0x%x)\n", rsa_ret);
        ret = ERR_RSA_OAEP_EXPTMOD_FAIL;
        goto error;
    }

error:
    if (ret)
        dprintf(CRITICAL, "[rsa_oeap] encrypt fail (0x%x)\n", ret);

    return ret;
}

int rsa_oaep_decrypt(uint8_t *emsg,
                     uint32_t emsg_sz,
                     uint8_t *key_d,
                     uint32_t key_d_sz,
                     uint8_t *key_n,
                     uint32_t key_n_sz,
                     uint8_t *msg,
                     uint32_t *msg_sz)
{
    int ret = RSA_OAEP_OK;
    int32_t rsa_ret = 0;
    uint8_t tmp_sig[RSA_MODULUS_SZ];
    uint32_t sig_sz = RSA_MODULUS_SZ;
    uint32_t final_msg_sz = 0;

    if (emsg == NULL) {
        ret = ERR_RSA_OAEP_INVALID_EMSG_BUF;
        goto error;
    }

    if (emsg_sz == 0) {
        ret = ERR_RSA_OAEP_INVALID_EMSG_BUF_SZ;
        goto error;
    }

    if (key_d == NULL) {
        ret = ERR_RSA_OAEP_INVALID_KEY_D_BUF;
        goto error;
    }

    if (key_d_sz == 0) {
        ret = ERR_RSA_OAEP_INVALID_KEY_D_BUF_SZ;
        goto error;
    }

    if (key_n == NULL) {
        ret = ERR_RSA_OAEP_INVALID_KEY_N_BUF;
        goto error;
    }

    if (key_n_sz == 0) {
        ret = ERR_RSA_OAEP_INVALID_KEY_N_BUF_SZ;
        goto error;
    }

    if (msg == NULL) {
        ret = ERR_RSA_OAEP_INVALID_MSG_BUF;
        goto error;
    }

    if (msg_sz == NULL) {
        ret = ERR_RSA_OAEP_INVALID_MSG_SZ_BUF;
        goto error;
    }

    final_msg_sz = *msg_sz;

    rsa_ret = sec_rsa_exptmod(emsg, emsg_sz,
                              tmp_sig, &sig_sz,
                              key_d, key_d_sz,
                              key_n, key_n_sz);
    if (rsa_ret) {
        dprintf(CRITICAL, "[rsa_oeap] rsa_exptmod fail (0x%x)\n", rsa_ret);
        ret = ERR_RSA_OAEP_EXPTMOD_FAIL;
        goto error;
    }

    ret = oaep_decode(tmp_sig, sig_sz, msg, &final_msg_sz);
    if (ret)
        goto error;

    *msg_sz = final_msg_sz;
error:
    if (ret)
        dprintf(CRITICAL, "[rsa_oeap] decrypt fail (0x%x)\n", ret);

    return ret;
}

