/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#pragma once

#define OAEP_OK                       (0x0000)
#define RSA_OAEP_OK                   (0x0000)
#define RSA_MODULUS_SZ                (256)

/* assume hash: sha256, rsa: rsa2048 */
#define MGF1_COUNTER_SZ               (4)

#define SEED_SZ                       (SHA256_HASH_SZ)
#define DB_SZ                         (RSA_MODULUS_SZ - 1 - SHA256_HASH_SZ)

/* used OAEP prefix to avoid name collision */
#define OAEP_MIN(a, b)                ((a) < (b) ? (a) : (b))

#define ZERO_HASH 0xe3, 0xb0, 0xc4, 0x42, 0x98, 0xfc, 0x1c, 0x14, \
                  0x9a, 0xfb, 0xf4, 0xc8, 0x99, 0x6f, 0xb9, 0x24, \
                  0x27, 0xae, 0x41, 0xe4, 0x64, 0x9b, 0x93, 0x4c, \
                  0xa4, 0x95, 0x99, 0x1b, 0x78, 0x52, 0xb8, 0x55

int oaep_encode(unsigned char *msg,
                unsigned int msg_sz,
                unsigned char *encoded_msg,
                unsigned int encoded_msg_sz);

int oaep_decode(unsigned char *encoded_msg,
                unsigned int encoded_msg_sz,
                unsigned char *msg,
                unsigned int *msg_sz);

int rsa_oaep_encrypt(uint8_t *msg,
                     uint32_t msg_sz,
                     uint8_t *key_e,
                     uint32_t key_e_sz,
                     uint8_t *key_n,
                     uint32_t key_n_sz,
                     uint8_t *emsg,
                     uint32_t *emsg_sz);

int rsa_oaep_decrypt(uint8_t *emsg,
                     uint32_t emsg_sz,
                     uint8_t *key_d,
                     uint32_t key_d_sz,
                     uint8_t *key_n,
                     uint32_t key_n_sz,
                     uint8_t *msg,
                     uint32_t *msg_sz);

