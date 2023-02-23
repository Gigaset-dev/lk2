/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <assert.h>
#include <clkbuf_common.h>
#include <debug.h>
#include <libfdt.h>
#include <spmi_common.h>
#include <string.h>
#include <sys/types.h>

#include "clkbuf.h"

#if defined(CLKBUF_SPMI_BRINGUP)
#define CLKBUF_SPMI_MASTER              0
#define CLKBUF_SPMI_SLAVE               0
#else /* !defined(CLKBUF_SPMI_BRINGUP) */
#define CLKBUF_SPMI_MASTER              SPMI_MASTER_1
#define CLKBUF_SPMI_SLAVE               SPMI_SLAVE_9
#endif /* defined(CLKBUF_SPMI_BRINGUP) */
#define CLKBUF_NODE                     "mediatek,clock_buffer_ctrl"
#define CONN_PROP                       "consys"

#define VOTE_MASK                       0x7ff
#define VOTE_SHIFT                      0
#define CONN_VOTE_L_MASK                0xC0
#define CONN_VOTE_H_MASK                0x03
#define CONN_VOTE_L_SHIFT               0
#define CONN_VOTE_H_SHIFT               0

/* #define CLKBUF_BRINGUP */

static void clk_buf_tcxo_config(void)
{
#if !defined(CLKBUF_BRINGUP)

    clk_buf_disable_xo(CLKBUF_SPMI_MASTER, CLKBUF_SPMI_SLAVE,
        MT6685_DCXO_EXTBUF2_CW0, MT6685_DCXO_EXTBUF2_CW0,
        XO_BBCK2_MODE_MASK, XO_BBCK2_MODE_SHIFT,
        XO_BBCK2_EN_M_MASK, XO_BBCK2_EN_M_SHIFT);
    clk_buf_disable_xo(CLKBUF_SPMI_MASTER, CLKBUF_SPMI_SLAVE,
        MT6685_DCXO_EXTBUF7_CW0, MT6685_DCXO_EXTBUF7_CW0,
        XO_RFCK1B_MODE_MASK, XO_RFCK1B_MODE_SHIFT,
        XO_RFCK1B_EN_M_MASK, XO_RFCK1B_EN_M_SHIFT);

    clk_buf_mask_voter(CLKBUF_SPMI_MASTER, CLKBUF_SPMI_SLAVE,
        XO_BBCK2_VOTE_L_ADDR, XO_BBCK2_VOTE_H_ADDR,
        CONN_VOTE_L_MASK, CONN_VOTE_H_MASK,
        CONN_VOTE_L_SHIFT, CONN_VOTE_H_SHIFT);
    clk_buf_mask_voter(CLKBUF_SPMI_MASTER, CLKBUF_SPMI_SLAVE,
        XO_RFCK1B_VOTE_L_ADDR, XO_RFCK1B_VOTE_H_ADDR,
        CONN_VOTE_L_MASK, CONN_VOTE_H_MASK,
        CONN_VOTE_L_SHIFT, CONN_VOTE_H_SHIFT);
    clk_buf_mask_voter(CLKBUF_SPMI_MASTER, CLKBUF_SPMI_SLAVE,
        XO_RFCK2A_VOTE_L_ADDR, XO_RFCK2A_VOTE_H_ADDR,
        CONN_VOTE_L_MASK, CONN_VOTE_H_MASK,
        CONN_VOTE_L_SHIFT, CONN_VOTE_H_SHIFT);
#endif /* defined(CLKBUF_BRINGUP) */
}

int clk_buf_init(const void *fdt)
{
    char *tcxo_support = NULL;
    unsigned int *data = NULL;
    int nodeoffset;
    int len;
    int phandle;

    ASSERT(fdt != NULL);

    nodeoffset = fdt_node_offset_by_compatible(fdt, -1, CLKBUF_NODE);
    if (nodeoffset < 0) {
        dprintf(CRITICAL, "[CLKBUF] dts node not found\n");
        goto TCXO_NOT_SUPPORT;
    }

    data = (unsigned int *)fdt_getprop(fdt, nodeoffset, CONN_PROP, &len);
    if (!data || !len) {
        dprintf(CRITICAL, "[CLKBUF] get conn prop failed\n");
        goto TCXO_NOT_SUPPORT;
    }

    phandle = fdt32_to_cpu(*(unsigned int *)data);
    nodeoffset = fdt_node_offset_by_phandle(fdt, phandle);
    if (nodeoffset < 0) {
        dprintf(CRITICAL, "[CLKBUF] get conn node by phandle failed\n");
        goto TCXO_NOT_SUPPORT;
    }

    tcxo_support = (char *)fdt_getprop(fdt,
                    nodeoffset,
                    "tcxo_support",
                    &len);
    if (!tcxo_support) {
        dprintf(CRITICAL, "[CLKBUF] no tcxo support\n");
        goto TCXO_NOT_SUPPORT;
    }

    if (tcxo_support && !strcmp(tcxo_support, "true")) {
        clk_buf_tcxo_config();
        dprintf(CRITICAL, "[CLKBUF] TCXO Supported\n");
        goto TCXO_INIT_DONE;
    }
TCXO_NOT_SUPPORT:
    dprintf(CRITICAL, "[CLKBUF] TCXO not support\n");

TCXO_INIT_DONE:
    dprintf(CRITICAL, "[CLKBUF] Init Done\n");

    return 0;
}
