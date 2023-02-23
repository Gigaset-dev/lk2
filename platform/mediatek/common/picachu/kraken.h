/*
 * Copyright (c) 2022 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once

#include <stdint.h>

#define MAX_SHADOW_ENTRY 64
#define symb_PTPOD 0

struct kraken_handle_t {
    uint32_t addr;
};

struct kraken_shadow_rule_t {
    uint32_t expr_pos;
    uint32_t expr_len;
    uint32_t shadowed_node_pos;
    uint32_t val_pos;
    uint32_t val_len;
};

struct kraken_cache_ent_t {
    uint16_t addr;
    int32_t val;
};

struct kraken_session_t {
    uint8_t *image;
    uint32_t img_len;

    uint8_t *img_shadow_table_block;
    uint8_t *img_symbtree_block;

    struct kraken_shadow_rule_t shadow_table[MAX_SHADOW_ENTRY];

    uint8_t result_commit_enabled;
};


#define for_each_item(kd, handle, iter_kh, idx)                        \
    for (idx = 0, iter_kh = kraken_get_list_memb(idx, kd, handle); \
         idx < kraken_get_list_len(kd, handle);                    \
         idx ++, iter_kh = kraken_get_list_memb(idx, kd, handle))
#define is_result_commit_enabled(kd)                                   \
    ((kd)->result_commit_enabled)
#define set_result_commit_enabled(kd, enable)                          \
    ((kd)->result_commit_enabled = (enable))

int init_kraken(struct kraken_session_t *kd,
        uint8_t *image_buf, int buf_size);
int kraken_get_root_symbol(int symb_id, struct kraken_handle_t *kh,
               uint16_t pos);
struct kraken_handle_t kraken_get_dict_memb(int memb_offset,
                        struct kraken_session_t *kd,
                        struct kraken_handle_t kh);
struct kraken_handle_t kraken_get_list_memb(int memb_offset,
                        struct kraken_session_t *kd,
                        struct kraken_handle_t kh);
uint8_t kraken_get_list_len(struct kraken_session_t *kd,
                struct kraken_handle_t kh);



/* interpreter part */

#define DEBUG 0

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

enum OPCODE {
    K_INT      = 0x00,
    K_UINT     = 0x78,
    K_ADD      = 0x80,
    K_SUB      = 0x40,
    K_MUL      = 0xc0,
    K_DEV      = 0x20,
    K_AND      = 0xa0,
    K_OR       = 0x60,
    K_L_SHIFT  = 0xe0,
    K_R_SHIFT  = 0x10,
    K_BOOL_AND = 0x90,
    K_BOOL_OR  = 0x50,
    K_BOOL_EQ  = 0x48,
    K_BOOL_NE  = 0x18,

    K_VAR      = 0xd0,

    K_FUNC_MAX = 0x30,
    K_FUNC_MIN = 0xb0,
    K_FUNC_GET = 0x70,
    K_FUNC_EFUSE = 0x08,
    K_FUNC_ISAGING = 0xc8,
    K_FUNC_ISSLT = 0x28,
    K_FUNC_ISMC50 = 0x38,
    K_FUNC_KRAKEN_OP_A = 0xa8,
    K_FUNC_KRAKEN_OP_B = 0x68,
    K_FUNC_KRAKEN_OP_C  = 0xe8,
    K_FUNC_KRAKEN_OP_D = 0x98,
    K_FUNC_KRAKEN_OP_E = 0x58,
    K_FUNC_KRAKEN_OP_F = 0xd8,
    K_HEXCHUNK = 0xb8,

    /* end-of-expression */
    K_EOE      = 0xf0,
};

enum INDEX_NODE_TYPE {
    EXPR  = 0x00,
    PTR   = 0x01,
    ARRAY = 0x02,
    DICT  = 0x03,
};
