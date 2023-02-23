/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <debug.h>
#include <oaep.h>
#include <rsa_oaep_custom.h>
#include <string.h>

int rsa_oaep_custom(void)
{
    uint32_t idx = 0;
    int ret = 0;

    uint8_t key_n[] = {RSA_TST_KEY_N};
    uint8_t key_e[] = {RSA_TST_KEY_E};
    uint8_t key_d[] = {RSA_TST_KEY_D};

    uint32_t key_n_sz = sizeof(key_n);
    uint32_t key_e_sz = sizeof(key_e);
    uint32_t key_d_sz = sizeof(key_d);

    unsigned char emsg[RSA_MODULUS_SZ] = {0};
    unsigned char msg[TST_MSG_SZ] = {0};

    uint32_t emsg_sz = RSA_MODULUS_SZ;
    uint32_t msg_sz = TST_MSG_SZ;

    dprintf(INFO, "msg = ");
    for (idx = 0; idx < TST_MSG_SZ; idx++) {
        msg[idx] = idx % 16;
        dprintf(INFO, "0x%x ", msg[idx]);
    }
    dprintf(INFO, "\n");

    ret = rsa_oaep_encrypt(msg, msg_sz, key_e, key_e_sz, key_n, key_n_sz, emsg, &emsg_sz);
    if (ret) {
        dprintf(CRITICAL, "rsa_oaep_encrypt test fail(0x%x)\n", ret);
        return ret;
    }

    dprintf(INFO, "emsg = ");
    for (idx = 0; idx < TST_MSG_SZ; idx++)
       dprintf(INFO, "0x%x ", emsg[idx]);
    dprintf(INFO, "\n");

    memset(msg, 0x0, TST_MSG_SZ);
    ret = rsa_oaep_decrypt(emsg, emsg_sz, key_d, key_d_sz, key_n, key_n_sz, msg, &msg_sz);
    if (ret) {
        dprintf(CRITICAL, "rsa_oaep_decrypt test fail(0x%x)\n", ret);
        return ret;
    }

    dprintf(INFO, "dmsg = ");
    for (idx = 0; idx < TST_MSG_SZ; idx++)
        dprintf(INFO, "0x%x ", msg[idx]);
    dprintf(INFO, "\n");

    for (idx = 0; idx < TST_MSG_SZ; idx++) {
        if (msg[idx] != (idx % 16)) {
            dprintf(CRITICAL, "rsa_oaep test failed\n");
            return ret;
        }
    }

    dprintf(INFO, "rsa_oaep test pass\n");

    return ret;
}

