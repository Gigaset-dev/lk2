/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once

/* #define CLKBUF_DEBUG */

enum CLKBUF_DTS_STA {
    TCXO_CLKBUF_SUPPORT = 0,
    PCIE_CLKBUF_SUPPORT = TCXO_CLKBUF_SUPPORT,
    CLKBUF_PROP_NOT_FOUND,
    TCXO_CONN_PROP_NOT_FOUND,
    TCXO_CONN_PHANDLE_FAILED,
    TCXO_CLKBUF_NOT_SUPPORT,
    PCIE_PROP_NOT_FOUND,
    PCIE_PHANDLE_FAILED,
    PCIE_CLKBUF_NOT_SUPPORT,
};

void clk_buf_set_voter(unsigned int spmi_master, unsigned int spmi_slave,
        unsigned int addr1, unsigned int addr2,
        unsigned int vote1_mask, unsigned int vote1_shift,
        unsigned int vote2_mask, unsigned int vote2_shift);
void clk_buf_mask_voter(unsigned int spmi_master, unsigned int spmi_slave,
        unsigned int addr1, unsigned int addr2,
        unsigned int vote1_mask, unsigned int vote1_shift,
        unsigned int vote2_mask, unsigned int vote2_shift);
void clk_buf_disable_xo(unsigned int spmi_master, unsigned int spmi_slave,
        unsigned int mode_addr, unsigned int en_addr,
        unsigned int mode_mask, unsigned int mode_shift,
        unsigned int en_mask, unsigned int en_shift);

int clk_buf_init(const void *fdt);
