/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include "lcm_rc.h"
#ifdef NT36672C_FHDP_SHENCHAO_RC
#include "nt36672c_fhdp_shenchao_rc.h"
#endif

static const struct mtk_lcm_rc_pattern mtk_lcm_rc_pattern_list[] = {
#ifdef NT36672C_FHDP_SHENCHAO_RC
    /* nt36672c_fhdp_sc*/
    {
        .name            = "NT36672C_FHDP_SHENCHAO_RC",
        .left_top        = {864, nt36672c_fhdp_sc_left_top_pattern},
        .left_top_left   = {0, NULL},
        .left_top_right  = {0, NULL},
        .right_top       = {0, NULL},
        .left_bottom     = {0, NULL},
        .right_bottom    = {0, NULL},
    },
#endif
};

static void lcm_rc_check_size(unsigned int *src, unsigned int dst)
{
    if (src == NULL || *src == dst)
        return;

    LCM_MSG("%s, corner size not matched, %u, %u\n",
        __func__, *src, dst);
    if (*src > dst)
        *src = dst;
}

const void *mtk_lcm_get_rc_addr(char *rc, enum mtk_lcm_rc_locate locate,
        unsigned int *size)
{
    unsigned int i = 0, dst_size = 0;
    const void *addr;
    unsigned int count = sizeof(mtk_lcm_rc_pattern_list) /
            sizeof(struct mtk_lcm_rc_pattern);

    if (size == NULL || *size == 0 || rc == NULL)
        return NULL;

    for (i = 0; i < count; i++) {
        if (strcmp(mtk_lcm_rc_pattern_list[i].name, rc) == 0)
            break;
    }
    if (i == count) {
        LCM_MSG("%s, %d, failed to find pattern\n", __func__, __LINE__);
        return NULL;
    }

    switch (locate) {
    case RC_LEFT_TOP:
        dst_size = mtk_lcm_rc_pattern_list[i].left_top.size;
        addr = mtk_lcm_rc_pattern_list[i].left_top.addr;
        break;
    case RC_LEFT_TOP_LEFT:
        dst_size = mtk_lcm_rc_pattern_list[i].left_top_left.size;
        addr = mtk_lcm_rc_pattern_list[i].left_top_left.addr;
        break;
    case RC_LEFT_TOP_RIGHT:
        dst_size = mtk_lcm_rc_pattern_list[i].left_top_right.size;
        addr = mtk_lcm_rc_pattern_list[i].left_top_right.addr;
        break;
    case RC_RIGHT_TOP:
        dst_size = mtk_lcm_rc_pattern_list[i].right_top.size;
        addr = mtk_lcm_rc_pattern_list[i].right_top.addr;
        break;
    case RC_LEFT_BOTTOM:
        dst_size = mtk_lcm_rc_pattern_list[i].left_bottom.size;
        addr = mtk_lcm_rc_pattern_list[i].left_bottom.addr;
        break;
    case RC_RIGHT_BOTTOM:
        dst_size = mtk_lcm_rc_pattern_list[i].right_bottom.size;
        addr = mtk_lcm_rc_pattern_list[i].right_bottom.addr;
        break;
    default:
        addr = NULL;
        LCM_MSG("%s, %d, invalid locate:%u\n",
            __func__, __LINE__, locate);
        break;
    }

    if (dst_size > 0 && addr != NULL) {
        lcm_rc_check_size(size, dst_size);
        LCM_MSG("%s, %d, pattern:%s, locate:%d, addr=0x%lx,size=%u\n",
            __func__, __LINE__, rc, locate, (unsigned long)addr, *size);
    } else {
        LCM_MSG("%s, %d, pattern:%s, locate:%d, empty pattern\n",
            __func__, __LINE__, rc, locate);
    }

    return addr;
}
